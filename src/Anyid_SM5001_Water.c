#include "AnyID_SM5001_Water.h"

WATER_INFO g_sWaterInfo = {0};
WATER_RX_BUF g_sWaterTempRcv = {0};
WATER_RSPFRAME g_sWaterRspFrame = {0};
WATER_DEVICE_RELAY_INFO g_sWaterDeviceRelayInfo[WATER_DEVICE_RELAY_NUM] ;
u8 g_nWaterGetUidBuffer[WATER_GETUID_BUFFER_LEN] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

void Water_Init()
{
    Water_InitInterface(WATER_BAUDRARE);
    Water_ConfigInt(ENABLE);
    Water_InitTxDma(g_sWaterInfo.txBuf.buffer, g_sWaterInfo.txBuf.len);
    Water_InitRxDma(g_sWaterInfo.rxBuf.buffer, g_sWaterInfo.rxBuf.len);
	memset(g_sWaterDeviceRelayInfo, 0, sizeof(WATER_DEVICE_RELAY_INFO) * WATER_DEVICE_RELAY_NUM);

    Water_EnableRxDma();
	Water_ClearInfo(); 
	g_sWaterInfo.mode = WATER_STAT_MODE_GET_INFO_RFID;
	g_sWaterInfo.state = WATER_STAT_TX;
	g_sWaterInfo.txBuf.addr = WATER_ADDR_RFID;
	g_sWaterInfo.txBuf.cmd = WATER_CMD_GET_UID; 
	g_sWaterInfo.txBuf.len = Water_FormatFarme(&(g_sWaterInfo.txBuf), g_nWaterGetUidBuffer, WATER_GETUID_BUFFER_LEN);
	

}

u16 Water_FormatFarme(WATER_TX_BUF *pTxBuf, u8 *pBuffer, u8 len)
{
    u16 pos = 0;
    u16 crc  = 0;
                                                                                                                                          
    pTxBuf->buffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pTxBuf->buffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pTxBuf->buffer[pos++] = 0x00;                      // length
    pTxBuf->buffer[pos++] = 0x00;
    pTxBuf->buffer[pos++] = 0x00;
    pTxBuf->buffer[pos++] = (pTxBuf->addr >> 0) & 0xFF; 
    pTxBuf->buffer[pos++] = (pTxBuf->addr >> 8) & 0xFF;
    pTxBuf->buffer[pos++] = pTxBuf->cmd;                       // cmd
    pTxBuf->buffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    memcpy(pTxBuf->buffer + pos, pBuffer, len);
    pos += len;
    pTxBuf->buffer[UART_FRAME_POS_LEN] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节
    crc = a_GetCrc(pTxBuf->buffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pTxBuf->buffer[pos++] = (crc >> 0) & 0xFF;
    pTxBuf->buffer[pos++] = (crc >> 8) & 0xFF;
    pTxBuf->len = pos;
    return pos;
}


void Water_TxFrame(WATER_INFO *pWaterInfo, u32 tick)
{
	if(pWaterInfo->mode == WATER_STAT_MODE_GET_INFO_RFID)
	{
		Water_EnableTxDma(g_sWaterInfo.txBuf.buffer, g_sWaterInfo.txBuf.len);
		pWaterInfo->tick = tick;
	}
	else if(pWaterInfo->mode == WATER_STAT_MODE_GET_INFO_RELAY)
	{
		Water_EnableTxDma(g_sWaterInfo.txBuf.buffer, g_sWaterInfo.txBuf.len);
		pWaterInfo->tick = tick;
		
	}
    else if(pWaterInfo->mode == WATER_STAT_MODE_CTR_CMD_RELAY)
    {
		pWaterInfo->tick = tick;
		pWaterInfo->txBuf.len = Water_ModbusForamtFrame(&(pWaterInfo->txBuf));
		Water_EnableTxDma(g_sWaterInfo.txBuf.buffer, g_sWaterInfo.txBuf.len);
		
    }
}
	 
				 
BOOL Water_CheckRspFrame(WATER_RX_BUF *pRxFrame, u16 *pStartPos)
{
    BOOL b = FALSE;
    u16 pos = 0;
	u16 crc1 = 0, crc2 = 0;
	
	if(g_sWaterInfo.mode == WATER_STAT_MODE_GET_INFO_RFID)
	{
		if(pRxFrame->len >= UART_RFRAME_MIN_LEN)
		{
			while(pos + UART_RFRAME_MIN_LEN <= pRxFrame->len)
			{
				if(pRxFrame->buffer[pos + 0] == UART_FRAME_FLAG_HEAD1 && pRxFrame->buffer[pos + 1] == UART_FRAME_FLAG_HEAD2)
				{
					u16 frameLen = 0;

					if(pRxFrame->buffer[pos + UART_FRAME_POS_LEN])
					{
						frameLen = pRxFrame->buffer[pos + 2] + 3;
					}
					else
					{
						frameLen = pRxFrame->buffer[pos + UART_RFRAME_POS_PAR + 1];
						frameLen <<= 8;
						frameLen |= pRxFrame->buffer[pos + UART_RFRAME_POS_PAR + 0];

						frameLen += UART_RFRAME_MIN_LEN;
					}
					if(frameLen >= UART_RFRAME_MIN_LEN && pos + frameLen <= pRxFrame->len)
					{
						crc1 = a_GetCrc(pRxFrame->buffer + pos + 2, frameLen - 4);
						crc2 = pRxFrame->buffer[pos + frameLen - 1];
						crc2 <<= 8;
						crc2 |= pRxFrame->buffer[pos + frameLen - 2];
						if(crc1 == crc2)
						{
							*pStartPos = pos;
							b = TRUE;
						}
					}
				}
				pos++;
			}
		}
	}
	else if(g_sWaterInfo.mode == WATER_STAT_MODE_CTR_CMD_RELAY || g_sWaterInfo.mode ==WATER_STAT_MODE_GET_INFO_RELAY)
	{
		if(pRxFrame->len)
		{
			u16 crc1 = 0, crc2 = 0;
			crc1 = Modbus_GetCrc16(pRxFrame->buffer, pRxFrame->len - 2);
			crc2 = pRxFrame->buffer[pRxFrame->len - 1] ;
			crc2 <<= 8;
			crc2 |= pRxFrame->buffer[pRxFrame->len - 2];		
			if(crc1 == crc2)
			{
				*pStartPos = 2;
				b = TRUE;
			}
		}
	}	
    return b;
}



u16 Water_ModbusForamtFrame(WATER_TX_BUF *pTxBuf)
{
	u16 pos = 0;
	u16 crc = 0;

	pTxBuf->buffer[pos++] = (pTxBuf->addr >> 0) & 0xFF; 
	pTxBuf->buffer[pos++] = pTxBuf->cmd;
	pTxBuf->buffer[pos++] = (pTxBuf->startAddr >> 8) & 0xFF; 
    pTxBuf->buffer[pos++] = (pTxBuf->startAddr >> 0) & 0xFF;
	pTxBuf->buffer[pos++] = (pTxBuf->frame >> 8) & 0xFF; 
    pTxBuf->buffer[pos++] = (pTxBuf->frame >> 0) & 0xFF;

	crc = Modbus_GetCrc16(pTxBuf->buffer,pos);
	pTxBuf->buffer[pos++] = (crc >> 0) & 0xFF;
    pTxBuf->buffer[pos++] = (crc >> 8) & 0xFF;
	
	return pos;
}



#define POLYNOMIAL                      0xA001   //x^16 + x^12 + x^5 + 1
#define PRESET_VALUE                    0xFFFF
u16 Modbus_GetCrc16(u8 *pBuffer, u8 len)
{
    u16 i = 0;
    u16 crc = 0;
    u8 j = 0;

    crc = PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pBuffer[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }

    return crc;
}



void Water_GetNextOp(WATER_INFO *pWaterInfo, u32 tick)
{
	if(pWaterInfo->mode == WATER_STAT_MODE_GET_INFO_RELAY)
    {
		Water_ClearInfo();
		pWaterInfo->txBuf.addr = WATER_ADDR_RFID;
		pWaterInfo->txBuf.cmd = WATER_CMD_GET_UID; 
		pWaterInfo->txBuf.len = Water_FormatFarme(&(pWaterInfo->txBuf), g_nWaterGetUidBuffer, WATER_GETUID_BUFFER_LEN);
		pWaterInfo->mode = WATER_STAT_MODE_GET_INFO_RFID;
    }
    else if(pWaterInfo->mode == WATER_STAT_MODE_GET_INFO_RFID)
    {
        Water_ClearInfo();
		/*
		pWaterInfo->txBuf.cmd = WATER_MODBUS_READ_PDU;
		pWaterInfo->txBuf.addr = WATER_DEVICE_ADD_RELAY;
		pWaterInfo->txBuf.startAddr = 0;
		pWaterInfo->txBuf.frame = 0x0040;
		pWaterInfo->txBuf.len = Water_ModbusForamtFrame(&(pWaterInfo->txBuf));
*/
		pWaterInfo->txBuf.addr = WATER_ADDR_RFID;
		pWaterInfo->txBuf.cmd = WATER_CMD_GET_UID; 
		pWaterInfo->txBuf.len = Water_FormatFarme(&(pWaterInfo->txBuf), g_nWaterGetUidBuffer, WATER_GETUID_BUFFER_LEN);
		pWaterInfo->mode = WATER_STAT_MODE_GET_INFO_RFID;

    }
	else if(pWaterInfo->mode == WATER_STAT_MODE_CTR_CMD_RELAY)
    {
        Water_ClearInfo();
		pWaterInfo->mode = WATER_STAT_MODE_GET_INFO_RELAY;
    }
}

void Water_AnsyRelayInfo(u8 *pBuffer)
{
	u8 index = 0;

	for(index = 0; index < (WATER_DEVICE_RELAY_NUM >> 1); index++)
	{
		g_sWaterDeviceRelayInfo[index * 2].doutdv = (BOOL)(*(pBuffer + index + 3) & 0x01);
		g_sWaterDeviceRelayInfo[index * 2 + 1].doutdv = (BOOL)(*(pBuffer + index + 3) >> 4 & 0x01);
	}
}
