#ifndef _ANYID_BOOT_FRAM_
#define _ANYID_BOOT_FRAM_

#include "AnyID_FRam.h"

#define FRAM_BOOT_APP_OK                    0x5555
#define FRAM_BOOT_APP_FAIL                  0xAAAA
#define FRAM_BOOT_APP_DATA_DOWD             0x1111
#define FRAM_BOOT_APP_DATA_DOWN_OVER        0x3333
#define FRAM_BOOT_APP_REPLACE               0x4444
#define FRAM_BOOT_APP_NULL_REPLACE          0x7777
#define FRAM_BOOT_APP_REPLACE_OVER          0x8888

#define FRAM_VERSION_SIZE                   22
#define FRAM_BR_9600                        2
#define FRAM_BR_38400                       1
#define FRAM_BR_115200                      0
typedef struct framBootDevicePar{
    u8 flag;
    u8 size;    //2kһƬ
    u8 gateNum;
    u8 rfu;
    u16 appState;
    u16 addr;
    u8 currentVerSion[FRAM_VERSION_SIZE];    //当前版本信息
    u8 aimVerSion[FRAM_VERSION_SIZE];        //目标版本信息
    u16 crc;
}FRAM_BOOTPARAMS;
extern FRAM_BOOTPARAMS g_sFramBootParamenter;




#define FRAME_MEM_SIZE                      0x2000    //8k
#define FRAME_BOOT_INFO_BACKUP_ADDR         (FRAME_MEM_SIZE - 512) 
#define FRAME_BOOT_INFO_ADDR                (FRAME_MEM_SIZE - sizeof(FRAM_BOOTPARAMS))      


void Fram_ReadBootParamenter(void);
BOOL Fram_WriteBootParamenter(void);
u32 Fram_GetUartBaudrate(void);
BOOL Fram_WriteBackupBootParamenter(void);
#endif

