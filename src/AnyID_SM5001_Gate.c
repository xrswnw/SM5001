#include "AnyID_SM5001_Gate.h"

GATE_OPINFO g_sGateOpInfo = {0};
GATE_INFO g_aGateSlvInfo[GATE_SLAVER_NUM << 1] = {0};
GATE_STAT g_aGateSlvStat[GATE_SLAVER_NUM << 1] = {0};
void Gate_Init(GATE_PARAMS *pGateParams, u32 tick)
{
    u8 i = 0;
    Gate_InitInterface(GATE_BAUDRARE);
    Gate_ConfigInt(ENABLE);
    Gate_InitTxDma(g_sGateOpInfo.txFrame.buffer, GATE_TX_FRAME_LEN);
    Gate_InitRxDma(g_sGateOpInfo.rxFrame.buffer, GATE_RX_FRAME_LEN);

    Gate_ClearOpInfo();
    g_sGateOpInfo.mode = GATE_MODE_STARTUP;
    g_sGateOpInfo.state = GATE_OP_STAT_INIT;
    g_sGateOpInfo.tick = tick;
    g_sGateOpInfo.pGateParams = pGateParams;

    for(i = 0; i < (GATE_SLAVER_NUM << 1); i++)
    {
        memset(g_aGateSlvInfo + i, 0, sizeof(GATE_INFO));
        g_aGateSlvInfo[i].state = GATE_STAT_UNKNOW;     //默认未知状态
    }
}

void Gate_Stop(void)
{
    USART_TypeDef* uart = NULL;

    (GATE_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    (GATE_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;

    Gate_ConfigInt(DISABLE);
    
    USART_ITConfig(uart, USART_IT_IDLE, DISABLE);
    USART_DMACmd(uart, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(uart, USART_DMAReq_Rx, DISABLE);
    USART_Cmd(uart, DISABLE);
    USART_DeInit(uart);
}

void Gate_GetNextOp(GATE_OPINFO *pGateOpInfo, u32 tick)
{
    if(pGateOpInfo->mode == GATE_MODE_STARTUP)              //开机延时完成，配置设备参数
    {
        Gate_ClearOpInfo();
        pGateOpInfo->rpt = 0;
        pGateOpInfo->mode = GATE_MODE_INIT;
        pGateOpInfo->state = GATE_OP_STAT_TX;
        pGateOpInfo->cmd = GATE_FRAME_CMD_SET_PARAMS;       //设置参数
        pGateOpInfo->tick = tick;
    }
    else if(pGateOpInfo->mode == GATE_MODE_INIT || pGateOpInfo->mode == GATE_MODE_INFO)
    {
        if(pGateOpInfo->slvCmd.cmd != 0)
        {
            Gate_ClearOpInfo();
            pGateOpInfo->mode = GATE_MODE_CMD;
            pGateOpInfo->state = GATE_OP_STAT_TX;
            pGateOpInfo->cmd = pGateOpInfo->slvCmd.cmd;      
            pGateOpInfo->tick = tick;
        }
        else
        {
            pGateOpInfo->slvIndex++;
            pGateOpInfo->rpt = 0;
            if(pGateOpInfo->slvIndex >= GATE_SLAVER_NUM)
            {
                Gate_ClearOpInfo();
                pGateOpInfo->mode = GATE_MODE_INFO;
                pGateOpInfo->state = GATE_OP_STAT_TX;
                pGateOpInfo->cmd = GATE_FRAME_CMD_GET_ININFO;       //获取信息
                pGateOpInfo->tick = tick;
            }
        }
    }
    else if(pGateOpInfo->mode == GATE_MODE_CMD)
    {
        Gate_ClearOpInfo();
        pGateOpInfo->mode = GATE_MODE_INFO;
        pGateOpInfo->state = GATE_OP_STAT_TX;
        pGateOpInfo->cmd = GATE_FRAME_CMD_GET_ININFO;       //获取信息
        pGateOpInfo->tick = tick;
    }
}

void Gate_FormatCmd(GATE_OPINFO *pGateOpInfo, u8 *pParams, u16 paramsLen)
{
    u16 pos = 0;
    u16 crc  = 0;
    u8 *pBuffer = NULL;
    u16 addr = 0;

    addr = pGateOpInfo->slvIndex + 1;
    pBuffer = pGateOpInfo->txFrame.buffer;
    
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = pGateOpInfo->cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    memcpy(pBuffer + pos, pParams, paramsLen);
    pos += paramsLen;
    
    pBuffer[2] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;
                                                                     
    pGateOpInfo->txFrame.len = pos;
}

void Gate_TxFrame(GATE_OPINFO *pGateOpInfo, u32 tick)
{
    Gate_Delayms(100);
    if(pGateOpInfo->mode == GATE_MODE_INIT)
    {
        Gate_FormatCmd(pGateOpInfo, (u8 *)pGateOpInfo->pGateParams, GATE_PARAMS_LEN);
        Gate_EnableTxDma(pGateOpInfo->txFrame.buffer, pGateOpInfo->txFrame.len);
        pGateOpInfo->tick = tick;
    }
    else if(pGateOpInfo->mode == GATE_MODE_INFO)
    {
        Gate_FormatCmd(pGateOpInfo, NULL, 0);
        Gate_EnableTxDma(pGateOpInfo->txFrame.buffer, pGateOpInfo->txFrame.len);
        pGateOpInfo->tick = tick;
    }
    else if(pGateOpInfo->mode == GATE_MODE_CMD)
    {
        Gate_FormatCmd(pGateOpInfo, pGateOpInfo->slvCmd.params, pGateOpInfo->slvCmd.paramsLen);
        Gate_EnableTxDma(pGateOpInfo->txFrame.buffer, pGateOpInfo->txFrame.len);
        pGateOpInfo->tick = tick;
        pGateOpInfo->tickCmd = tick;
    }

}

BOOL Gate_CheckRspFrame(GATE_RXFRAME *pRxFrame, u16 *pStartPos)
{
    BOOL b = FALSE;
    u16 pos = 0;

    if(pRxFrame->len >= UART_RFRAME_MIN_LEN)
    {
        while(pos + UART_RFRAME_MIN_LEN <= pRxFrame->len)
        {
            if(pRxFrame->buffer[pos + 0] == UART_FRAME_FLAG_HEAD1 && pRxFrame->buffer[pos + 1] == UART_FRAME_FLAG_HEAD2)
            {
                u16 frameLen = 0;
                u16 crc1 = 0, crc2 = 0;
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
    return b;
}

void Gate_CfgSlaver(GATE_OPINFO *pGateOpInfo)
{
    Gate_ClearOpInfo();
    pGateOpInfo->cmd = GATE_FRAME_CMD_SET_PARAMS;
}

void Gate_ChagSlaver(GATE_SLVCMD *pGateSlvCmd, u8 index, u8 subIndex, u8 mode)
{
    memset(pGateSlvCmd, 0, sizeof(GATE_SLVCMD));
    pGateSlvCmd->index = index;
    pGateSlvCmd->cmd = GATE_FRAME_CMD_CHARGE;
    pGateSlvCmd->params[pGateSlvCmd->paramsLen++] = subIndex;
    pGateSlvCmd->params[pGateSlvCmd->paramsLen++] = mode;
}

void Gate_BrwBat(GATE_SLVCMD *pGateSlvCmd, u8 index, u8 subIndex, u8 *pBatSn)
{
    memset(pGateSlvCmd, 0, sizeof(GATE_SLVCMD));
    pGateSlvCmd->index = index;
    pGateSlvCmd->cmd = GATE_FRAME_CMD_BRWBAT;
    pGateSlvCmd->params[pGateSlvCmd->paramsLen++] = subIndex;
    memcpy(pGateSlvCmd->params + pGateSlvCmd->paramsLen, pBatSn, BAT_SN_LEN);
    pGateSlvCmd->paramsLen += BAT_SN_LEN;
}