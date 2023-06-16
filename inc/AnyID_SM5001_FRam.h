#ifndef _ANYID_SM5001_FRAM_
#define _ANYID_SM5001_FRAM_

#include "AnyID_FRam.h"

#define FRAM_BOOT_APP_OK                    0x5555
#define FRAM_BOOT_APP_FAIL                  0xAAAA
#define FRAM_BOOT_APP_DATA_DOWD             0x1111
#define FRAM_VERSION_SIZE                   17

typedef struct framBootDevicePar{
    u8 flag;
    u16 appState;
    u16 addr;
    u8 verSion[FRAM_VERSION_SIZE];
    u16 crc;
}FRAM_BOOTPARAMS;

extern FRAM_BOOTPARAMS g_sFramBootParamenter;






#define FRAME_MEM_SIZE                      0x2000
#define FRAME_UID_LEN                           8
#define FRAME_INFO_ADDR                     0x0000
#define FRAME_INFO_BACKUP_ADDR              0x0200
#define REAM_IMEI_LEN                           15

#define FRAME_MQTT_KEY_KEY                      150

#define FRAME_BOOT_INFO_BACKUP_ADDR         (FRAME_MEM_SIZE - 512) 
#define FRAME_BOOT_INFO_ADDR                (FRAME_MEM_SIZE - sizeof(FRAM_BOOTPARAMS)) 

#define FRAME_KEY_ADDR                      0x1200          //key
#define FRAME_KEY_BACKUP_ADDR               0x1280          //key

#define FRAME_MQTT_KEY_ADDR                      0x0400          //key
#define FRAME_MQTT_KEY_BACKUP_ADDR               0x0600          //key

#define FRAM_KEY_DFT_ADDR                   0x08008000
#define FRAM_KEY1_POS_START                 12
#define FRAM_KEY2_POS_START                 32
#define FRAM_KEY3_POS_START                 52
typedef struct frameKeyParams{
    u8 keyBuffer[60];
    u32 crc;
}FRAM_KEY;



typedef struct mqttKeyParams{
    u8 len;
    u8 keyBuffer[FRAME_MQTT_KEY_KEY];
    u8 uid[FRAME_UID_LEN];
    u8 imsiStr[REAM_IMEI_LEN + 2];
    u32 crc;
}MQTT_FRAM_KEY;

extern MQTT_FRAM_KEY g_sMqttKey;
extern FRAM_KEY g_sFramKeyInfo;
extern u32 g_nFRamKey1;
extern u32 g_nFRamKey2;
extern u32 g_nFRamKey3;

#define FRam_CheckKey1()                    (*((u32 *)(g_sFramKeyInfo.keyBuffer + FRAM_KEY1_POS_START)) == g_nFRamKey1)
#define FRam_CheckKey2()                    (*((u32 *)(g_sFramKeyInfo.keyBuffer + FRAM_KEY2_POS_START)) == g_nFRamKey2)
#define FRam_CheckKey3()                    (*((u32 *)(g_sFramKeyInfo.keyBuffer + FRAM_KEY3_POS_START)) == g_nFRamKey3)
#define FRam_CheckKey4()                    (TRUE)
#define FRam_CheckKey5()                    (TRUE)
#define FRam_CheckKey()                     (FRam_CheckKey1() && FRam_CheckKey2() && FRam_CheckKey3() && FRam_CheckKey4() && FRam_CheckKey5())

void Fram_ReadBootParamenter(void);
BOOL Fram_WriteBootParamenter(void);

void FRam_ReadKey(void);
BOOL FRam_WriteKey(void);
void FRam_EncryptKey(u8 *pCpuId);
u32	FRam_KeeloqEncrypt(u32 data, u64 key);
u32	FRam_KeeloqDecrypt(u32 data, u64 key);

#endif

