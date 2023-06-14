#ifndef _ANYID_FLASHM25_
#define _ANYID_FLASHM25_

#include "AnyID_Boot_FLASH_HL.h"

//写状态最大15ms
//页编程最大5ms
//段擦除最大3s
#define FLASH_CMD_WREN          0x06
#define FLASH_CMD_WRDI          0x04
#define FLASH_CMD_RDSR          0x05
#define FLASH_CMD_WRSR          0x01
#define FLASH_CMD_READ          0x03
#define FLASH_CMD_FSTRD         0x0B
#define FLASH_CMD_PP            0x02
#define FLASH_CMD_SE            0xD8
#define FLASH_CMD_BE            0x60
#define FLASH_CMD_DP            0xB9
#define FLASH_CMD_RES           0xAB
#define FLASH_CMD_GET_ID        0x90

#define FLASH_STAT_SRWD         0x80
#define FLASH_STAT_BP           0x1C
#define FLASH_STAT_WEL          0x02
#define FLASH_STAT_WIP          0x01


void Flash_Init(void);
BOOL Flash_WriteBuffer(u32 addr, u16 len, u8 *pBuffer);
BOOL Flash_EraseSector(u32 addr);
BOOL Flash_ReadBuffer(u32 addr, u16 len, u8 *pBuffer);
BOOL Flash_WritePage(u32 addr, u32 len, u8 *pBuffer);
BOOL Flash_EraseAllmemory(u32 addr);
void Flash_ReadId();
#define Flash_CheckBusy(s)      (s & (FLASH_STAT_WIP | FLASH_STAT_WEL))


#endif
