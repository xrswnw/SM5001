#include "AnyID_SM5001_Water.h"

WATER_INFO g_sWaterInfo = {0};
WATER_RX_BUF g_sWaterTempRcv = {0};
WATER_RSPFRAME g_sWaterRspFrame = {0}         ;
u8 g_nWaterGetUidBuffer[WATER_GETUID_BUFFER_LEN] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

void Water_Init()
{
    Water_InitInterface(WATER_BAUDRARE);
    Water_ConfigInt(ENABLE);
    Water_InitTxDma(g_sWaterInfo.txBuf.buffer, g_sWaterInfo.txBuf.len);
    Water_InitRxDma(g_sWaterInfo.rxBuf.buffer, g_sWaterInfo.rxBuf.len);
    g_sWaterInfo.mode = WATER_MODE_INFO;
    Water_EnableRxDma();
    
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



void Water_TransmitCmd(WATER_INFO *pWaterInfo, u32 tick)
{
    if(pWaterInfo->mode == WATER_MODE_INFO)
    {
        if(pWaterInfo->txBuf.mode == WATER_STAT_MODE_RFID)
        {
            g_sWaterInfo.txBuf.addr = WATER_ADDR_RFID;
            g_sWaterInfo.txBuf.cmd = WATER_CMD_GET_UID; 
            pWaterInfo->txBuf.tick[WATER_STAT_MODE_RFID] = tick;
            pWaterInfo->txBuf.len = Water_FormatFarme(&(pWaterInfo->txBuf), g_nWaterGetUidBuffer, WATER_GETUID_BUFFER_LEN);
            pWaterInfo->txBuf.mode = WATER_STAT_MODE_RFID;		////----------------------
        }
        else if(pWaterInfo->txBuf.mode == WATER_STAT_MODE_WATER_1)
        {
            g_sWaterInfo.txBuf.addr = WATER_ADDR_WATER_1;
            g_sWaterInfo.txBuf.cmd = WATER_CMD_GET_UID; 
            pWaterInfo->txBuf.tick[WATER_STAT_MODE_WATER_1] = tick;
            pWaterInfo->txBuf.len = Water_FormatFarme(&(pWaterInfo->txBuf), 0, 0);
            pWaterInfo->txBuf.mode = WATER_STAT_MODE_WATER_2;
        }
        else if(pWaterInfo->txBuf.mode == WATER_STAT_MODE_WATER_2)
        {
            g_sWaterInfo.txBuf.addr = WATER_ADDR_WATER_2;
            g_sWaterInfo.txBuf.cmd = WATER_CMD_GET_UID; 
            pWaterInfo->txBuf.tick[WATER_STAT_MODE_WATER_2] = tick;
            pWaterInfo->txBuf.len = Water_FormatFarme(&(pWaterInfo->txBuf), 0, 0);
            pWaterInfo->txBuf.mode = WATER_STAT_MODE_RFID;

        }
    }
    else if(pWaterInfo->mode == WATER_MODE_CMD)
    {
            pWaterInfo->mode = WATER_MODE_INFO;
    
    
    }
}


           /*

u16 Watre_ResponseFrame(u8 *pParam, u8 len, WATER_RSPFRAME *pOpResult)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG; 

    if(len > UART_FRAME_PARAM_MAX_LEN)
    {
        pOpResult->buffer[pos++] = (len >> 0) & 0xFF;
        pOpResult->buffer[pos++] = (len >> 8) & 0xFF;

        memcpy(pOpResult->buffer + pos, pParam, len);
        pos += len;
    }
    else
    {
        if(pOpResult->flag == DEVICE_RSPFRAME_FLAG_OK)
        {
            memcpy(pOpResult->buffer + pos, pParam, len);
        }
        else
        {
            memset(pOpResult->buffer + pos, 0, len);
        }
        pos += len;
        pOpResult->buffer[pos++] = pOpResult->err;
        pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
        pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
        
    }

    crc = a_GetCrc(pOpResult->buffer, pos); //从LEN开始计算crc
    pOpResult->buffer[pos++] = crc & 0xFF;
    pOpResult->buffer[pos++] = (crc >> 8) & 0xFF;

    pOpResult->len = pos;

    return pos;
}
             */
