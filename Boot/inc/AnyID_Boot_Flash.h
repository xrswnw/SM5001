#ifndef _ANYID_BOOT_FLASH_H
#define _ANYID_BOOT_FLASH_H

#include "AnyID_BOOT_Config.h"

#include "AnyID_FRam.h"


#define FRAME_SN_SIZE                       200

BOOL FRam_ReadBuffer(u32 addr, u16 len, u8 *pBuffer);
BOOL FRam_WriteBuffer(u32 addr, u16 len, u8 *pBuffer);
void FRam_Delayms(u32 n);

typedef struct framDeviceParams{
    u16 br;
    u16 rfu0;
    u16 rfu1;
    u16 crc;
}FRAM_DEVPARAMS;
extern FRAM_DEVPARAMS g_sFramDevParams;

void FRam_InitInterface(void);
void Fram_ReadBootParamenter(void);
BOOL Fram_WriteBootParamenter(void);
BOOL Fram_WriteBackupBootParamenter(void);
u32 Fram_GetUartBaudrate(void);

void Fram_ReadDevParams();
void Fram_WriteDevParams(void);
#endif

