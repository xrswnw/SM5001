#ifndef _ANYID_BOOT_UART_
#define _ANYID_BOOT_UART_

#include "AnyID_Boot_Config.h"
#include "AnyID_Uart_Receive.h"

#define UART_FRAME_FLAG_OK              0x00
#define UART_FRAME_FLAG_FAIL            0x01
#define UART_FRAME_RSP_NOERR            0x00
#define UART_FRAME_RSP_TAGERR           0x01
#define UART_FRAME_RSP_CRCERR           0x02
#define UART_FRAME_RSP_NORSP            0x03

#define UART_FRAME_CMD_BOOT           1
#define UART_FRAME_CMD_DL             2
#define UART_FRAME_CMD_JMP            3
#define UART_FRAME_CMD_RESET          4
#define UART_FRAME_CMD_ERASE          5
#define UART_FRAME_CMD_VER            0xF7

#define UART_FRAME_RESPONSE_FLAG      0x1F

#define UART_FRAME_OP_OK              0
#define UART_FRAME_OP_FAIL            1

u16 Uart_CalCrc(u8 *pFrame, u16 len);

u32 Uart_ReqReset(u8 *pFrame, u16 slvAddr);
u32 Uart_ReqEnableBoot(u8 *pFrame, u16 slvAddr);
u32 Uart_ReqErase(u8 *pFrame, u16 slvAddr, u32 sectorIndex);
u32 Uart_ReqDownload(u8 *pFrame, u16 slvAddr, u32 addr, u32 size, u8 *pBin);
u32 Uart_ReqJump(u8 *pFrame, u16 slvAddr);

BOOL Uart_EraseFlash(u32 addr);
BOOL BL_WriteImagePage(u32 addr, u8 *pBuffer, u32 size);

#endif
