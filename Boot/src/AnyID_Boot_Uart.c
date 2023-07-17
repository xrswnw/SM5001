#include "AnyID_Boot_Uart.h"

#define POLYNOMIAL                      0x8408   //x^16 + x^12 + x^5 + 1
#define PRESET_VALUE                    0xFFFF
#define ISO15693_CRC_RESULT             0x0F47
u16 Uart_CalCrc(u8 *pFrame, u16 len)
{
    u16 crc = 0;
    u16 i = 0, j = 0;
    crc = PRESET_VALUE;
    for(i = 0; i< len; i++)
    {
        crc = crc ^ pFrame[i];
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
    crc = ~crc;
    return crc;
}

u32 Uart_ReqReset(u8 *pFrame, u16 slvAddr)
{
    u32 pos = 0;
    u16 crc = 0;

    pFrame[pos++] = 0x7E; // frame head first byte
    pFrame[pos++] = 0x55; // frame haed second byte
    pFrame[pos++] = 8;   // command length
    pFrame[pos++] = 0x00;
    pFrame[pos++] = 0x00;
    pFrame[pos++] = (slvAddr >> 0) & 0xFF;
    pFrame[pos++] = (slvAddr >> 8) & 0xFF;
    pFrame[pos++] = UART_FRAME_CMD_RESET;   // computer command serial number
    pFrame[pos++] = 0x00;       // RFU

    crc = Uart_CalCrc(pFrame + 2, pFrame[2] - 1);
    pFrame[pos++] = crc & 0xFF;
    pFrame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

u32 Uart_ReqEnableBoot(u8 *pFrame, u16 slvAddr)
{
    u32 pos = 0;
    u16 crc = 0;

    pFrame[pos++] = 0x7E; // frame head first byte
    pFrame[pos++] = 0x55; // frame haed second byte
    pFrame[pos++] = 8;   // command length
    pFrame[pos++] = 0x00;
    pFrame[pos++] = 0x00;
    pFrame[pos++] = (slvAddr >> 0) & 0xFF;
    pFrame[pos++] = (slvAddr >> 8) & 0xFF;
    pFrame[pos++] = UART_FRAME_CMD_BOOT;   // computer command serial number
    pFrame[pos++] = 0x00;       // RFU

    crc = Uart_CalCrc(pFrame + 2, pFrame[2] - 1);
    pFrame[pos++] = crc & 0xFF;
    pFrame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

u32 Uart_ReqErase(u8 *pFrame, u16 slvAddr, u32 sectorIndex)
{
    u32 pos = 0;
    u16 crc = 0;

    pFrame[pos++] = 0x7E; // frame head first byte
    pFrame[pos++] = 0x55; // frame haed second byte
    pFrame[pos++] = 12;   // command length
    pFrame[pos++] = 0x00;
    pFrame[pos++] = 0x00;
    pFrame[pos++] = (slvAddr >> 0) & 0xFF;
    pFrame[pos++] = (slvAddr >> 8) & 0xFF;
    pFrame[pos++] = UART_FRAME_CMD_ERASE;   // computer command serial number
    pFrame[pos++] = 0x00;       // RFU

    pFrame[pos++] = (sectorIndex >> 0) & 0xFF;
	pFrame[pos++] = (sectorIndex >> 8) & 0xFF;
	pFrame[pos++] = (sectorIndex >> 16) & 0xFF;
	pFrame[pos++] = (sectorIndex >> 24) & 0xFF;
	
    crc = Uart_CalCrc(pFrame + 2, pFrame[2] - 1);
    pFrame[pos++] = crc & 0xFF;
    pFrame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

u32 Uart_ReqDownload(u8 *pFrame, u16 slvAddr, u32 addr, u32 size, u8 *pBin)
{
    u32 pos = 0;
    u16 crc = 0;

    pFrame[pos++] = 0x7E; // frame head first byte
    pFrame[pos++] = 0x55; // frame haed second byte
    pFrame[pos++] = 0;   // command length
    pFrame[pos++] = 0x00;
    pFrame[pos++] = 0x00;
    pFrame[pos++] = (slvAddr >> 0) & 0xFF;
    pFrame[pos++] = (slvAddr >> 8) & 0xFF;
    pFrame[pos++] = UART_FRAME_CMD_DL;   // computer command serial number
    pFrame[pos++] = 0x00;       // RFU

    pFrame[pos++] = 1;
    pFrame[pos++] = (addr >> 0) & 0xFF;
    pFrame[pos++] = (addr >> 8) & 0xFF;
    pFrame[pos++] = (addr >> 16) & 0xFF;
    pFrame[pos++] = (addr >> 24) & 0xFF;
    pFrame[pos++] = (size >> 0) & 0xFF;
    pFrame[pos++] = (size >> 8) & 0xFF;
    pFrame[pos++] = (size >> 16) & 0xFF;
    pFrame[pos++] = (size >> 24) & 0xFF;

    memcpy(pFrame + pos, pBin, size);
    pos += size;
    
    crc = Uart_CalCrc(pFrame + 2, pos - 2);
    pFrame[pos++] = crc & 0xFF;
    pFrame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

u32 Uart_ReqJump(u8 *pFrame, u16 slvAddr)
{
    u32 pos = 0;
    u16 crc = 0;

    pFrame[pos++] = 0x7E; // frame head first byte
    pFrame[pos++] = 0x55; // frame haed second byte
    pFrame[pos++] = 8;   // command length
    pFrame[pos++] = 0x00;
    pFrame[pos++] = 0x00;
    pFrame[pos++] = (slvAddr >> 0) & 0xFF;
    pFrame[pos++] = (slvAddr >> 8) & 0xFF;
    pFrame[pos++] = UART_FRAME_CMD_JMP;   // computer command serial number
    pFrame[pos++] = 0x00;       // RFU

    crc = Uart_CalCrc(pFrame + 2, pFrame[2] - 1);
    pFrame[pos++] = crc & 0xFF;
    pFrame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

BOOL Uart_EraseFlash(u32 addr)
{
    if(FLASH_ErasePage(addr) == FLASH_COMPLETE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL BL_WriteImagePage(u32 addr, u8 *pBuffer, u32 size)
{
    u32 i = 0;
    u32 *p = NULL;
    FLASH_Status flashStatus = FLASH_BUSY;
    BOOL b = TRUE;

    p = (u32 *)pBuffer;

    //如果发生错误就延时等待，并继续写数据，直到写数据成功
    for(i = 0; i < size; i += 4)
    {
        flashStatus = FLASH_ProgramWord(addr + i, p[i >> 2]);
        if(flashStatus != FLASH_COMPLETE)
        {
            b = FALSE;
            break;
        }

    }

    return b;
}