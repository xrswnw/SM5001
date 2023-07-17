#include "AnyID_Boot_Gate.h"

GATE_BOOTINFO g_sGateBootInfo = {0};

UART_RCVFRAME g_sGateRcvFrame = {0};
void Gate_StepBoot(GATE_BOOTINFO *pBootInfo, UART_RCVFRAME *pRcvFrame)
{
    switch(pBootInfo->cmd)
    {
        case UART_FRAME_CMD_RESET:           //复位系统成功
            pBootInfo->cmd = UART_FRAME_CMD_BOOT;
            pBootInfo->txFrame.len = Uart_ReqEnableBoot(pBootInfo->txFrame.buffer, pBootInfo->slvIndex + 1);
        break;
        case UART_FRAME_CMD_BOOT:          //使能系统进入boot成功
            pBootInfo->cmd = UART_FRAME_CMD_ERASE;
            pBootInfo->txFrame.len = Uart_ReqErase(pBootInfo->txFrame.buffer, pBootInfo->slvIndex + 1, pBootInfo->sectorIndex);
        break;
        case UART_FRAME_CMD_ERASE:          //擦除
            pBootInfo->sectorIndex++;
            if(pBootInfo->sectorIndex < pBootInfo->sectorNum)
            {
                pBootInfo->cmd = UART_FRAME_CMD_ERASE;
                pBootInfo->txFrame.len = Uart_ReqErase(pBootInfo->txFrame.buffer, pBootInfo->slvIndex + 1, pBootInfo->sectorIndex);
            }
            else
            {
                u32 offsetAddr = 0;
                offsetAddr = (pBootInfo->sectorIndex - 1) * GATE_BOOT_SECTOR_SIZE;    //从末尾下载
                pBootInfo->cmd = UART_FRAME_CMD_DL;
                pBootInfo->txFrame.len = Uart_ReqDownload(pBootInfo->txFrame.buffer, pBootInfo->slvIndex + 1, 
                                                          GATE_BOOT_SECTOR_ADDR + offsetAddr, GATE_BOOT_SECTOR_SIZE, g_sGateBootInfo.bin);
                                                          //((u8 *)GATE_BOOT_SRC_ADDR) + offsetAddr);
            }
        break;
        case UART_FRAME_CMD_DL:          //下载
            pBootInfo->sectorIndex--;
            if(pBootInfo->sectorIndex > 0)
            {
                u32 offsetAddr = 0;
                offsetAddr = (pBootInfo->sectorIndex - 1) * GATE_BOOT_SECTOR_SIZE;    //从末尾下载
                pBootInfo->cmd = UART_FRAME_CMD_DL;
                pBootInfo->txFrame.len = Uart_ReqDownload(pBootInfo->txFrame.buffer, pBootInfo->slvIndex + 1, 
                                                          GATE_BOOT_SECTOR_ADDR + offsetAddr, GATE_BOOT_SECTOR_SIZE, g_sGateBootInfo.bin);
                                                          //((u8 *)GATE_BOOT_SRC_ADDR) + offsetAddr);
            }
            else
            {
                pBootInfo->cmd = UART_FRAME_CMD_JMP;
                pBootInfo->txFrame.len = Uart_ReqJump(pBootInfo->txFrame.buffer, pBootInfo->slvIndex + 1);
            }
        break;
        case UART_FRAME_CMD_JMP:          //下载
        {
            pBootInfo->slvIndex++;
        }
        default:
        break;
    }
    Gate_Delayms(100);              //等待
}



void Gate_ProcessBootFrame(UART_RCVFRAME *pRcvFrame)
{
  
    u16 crc1 = 0, crc2 = 0;
    memcpy(&g_sUartTempRcvFrame, pRcvFrame, sizeof(UART_RCVFRAME));


    crc1 = Uart_GetFrameCrc(g_sUartTempRcvFrame.buffer, g_sUartTempRcvFrame.index);
    crc2 = a_GetCrc(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_LEN, g_sUartTempRcvFrame.index - 4);

    if(crc1 == crc2)
    {
        u8 cmd = g_sUartTempRcvFrame.buffer[UART_FRAME_POS_CMD + 1];
        u16 destAddr = 0;
        
        destAddr = *((u16 *)(pRcvFrame->buffer + UART_FRAME_POS_DESTADDR));
        if(destAddr != 0x0000)
        {
            return;
        }
        switch(cmd)
        {
            case UART_FRAME_CMD_RESET:
            case UART_FRAME_CMD_BOOT:
            case UART_FRAME_CMD_JMP:
                if(g_sGateBootInfo.cmd == cmd)
                {
                    g_sGateBootInfo.state = GATE_OP_STAT_RX | GATE_OP_STAT_STEP;
                }
                break;
            case UART_FRAME_CMD_ERASE:
                if(g_sGateBootInfo.cmd == cmd)
                {
                    if(g_sUartTempRcvFrame.buffer[UART_FRAME_POS_PAR + 1] == 1)
                    {
                        g_sGateBootInfo.state = GATE_OP_STAT_RX | GATE_OP_STAT_STEP;
                    }
                }
                break;
            case UART_FRAME_CMD_DL:
                if(g_sGateBootInfo.cmd == cmd)
                {
                    if(memcmp(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 1, g_sGateBootInfo.txFrame.buffer + UART_FRAME_POS_PAR, 9 + GATE_BOOT_SECTOR_SIZE) == 0)
                    {
                        g_sGateBootInfo.state = GATE_OP_STAT_RX | GATE_OP_STAT_STEP;
                    }
                }
                break;
            default:
                break;
        }
    }
}



