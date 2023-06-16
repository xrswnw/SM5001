#ifndef ANYID_BOOT_DEVICE_H
#define ANYID_BOOT_DEVICE_H

#include "AnyID_Boot_EC20.h"
#include "AnyID_Boot_Uart.h"
#include "AnyID_Boot_FlashM25.h"
#include "AnyID_Boot_FRam.h"

#define DEVICE_SERVER_TXSTAT_IDLE       0
#define DEVICE_SERVER_TXSTAT_WAIT       1
#define DEVICE_SERVER_TXSTAT_OVER       2
#define DEVICE_SERVER_TXSTAT_OK         3
#define DEVICE_SERVER_TXSTAT_RX         4


#define DEVICE_FLAG_MAST                5
#define DEVICE_GATE_LEN                 9
#define DEVICE_VERSION_LEN              4
#define DEVICE_SERVER_RSP_LEN           256
#define DEVICE_SERVER_MIN_LEN           50

#define DEVICE_SERVERRSP_NUM            20


#define DEVICE_TYPE_SM5001              1
#define DEVICE_TYPE_SM5002              2
#define DEVICE_TYPE_SM5003              3



#define DEVICE_UPDATA_FLAG_RQ           1
#define DEVICE_UPDATA_FLAG_DOWN         2
#define DEVICE_UPDATA_FLAG_DOWNING      3
#define DEVICE_UPDATA_FLAG_FAIL         4
#define DEVICE_UPDATA_FLAG_OVER         5
#define DEVICE_UPDATA_START             6

#define DEVICE_DATA_MASK_R              0x0D
#define DEVICE_DATA_MASK_N              0x0A

#define DEVICE_UPDATA_ERASE_FLASH       0x00
#define DEVICE_UPDATA_WRITE_FLASH       0x01

#define DEVICE_UPDATA_CHK_TIME          30

#define DEVICE_UPDATA_SM5001            1
#define DEVICE_UPDATA_SM5002            2

#define DEVICE_UPDATA_BUFFER_NAME_LEN           286  
#define DEVICE_UPDATA_BUFFER_TID_LEN            311 
#define DEVICE_UPDATA_BUFFER_SIZE_LEN           325
#define DEVICE_UPDATA_BUFFER_MD5_LEN            338

#define DEVICE_HTTP_URL_LINK                     1
#define DEVICE_HTTP_GET_REQUEST                  2
#define DEVICE_HTTP_GET_REQUEST_CKECK            3
#define DEVICE_HTTP_GET_REQUEST_DOWNLOAD         4
#define DEVICE_HTTP_GET_RONSPOND                 5

#define Device_Erase_Flash()                    do{\
                                                    Flash_EraseSector(0);\
                                                    Device_Delayms(1000); \
                                                    Flash_EraseSector(64 * 1024);\
                                                    Device_Delayms(1000);\
                                                   }while(0)
#define Device_At_Rsp(opt,repat,cmd)      do{g_sDeviceServerTxBuf.to[g_sDeviceServerTxBuf.num] = opt;g_sDeviceServerTxBuf.repeat[g_sDeviceServerTxBuf.num] = repat;g_sDeviceServerTxBuf.op[g_sDeviceServerTxBuf.num++] = cmd;g_sDeviceServerTxBuf.state = EC20_CNT_OP_STAT_TX;}while(0)


typedef struct deviceSenverTxBuff{
    u8 num;
    u8 index;
    u8 result;
    u16 state;
    u16 len;
    u8 id[EC20_CNT_OP_NUM];
    u8 op[EC20_CNT_OP_NUM];
    u8 txBuffer[DEVICE_SERVER_RSP_LEN];
    u8 repeat[EC20_CNT_OP_NUM];
    u32 to[EC20_CNT_OP_NUM];
    u32 tick;
}DEVICE_SENVER_TXBUFFER;

#define DEVICE_SOFTVERSION_NAME_LEN             8+8+1
#define DEVICE_SOFTVERSION_TID_LEN              6
#define DEVICE_SOFTVERSION_BUFFER_SIZE          6
#define DEVICE_SOFTVERSION_MD5                  32
#define DEVICE_SOFTVERSION_DATA_LEN             2048
typedef struct deviceUpDataInfo{
    u8 type;
    u8 state;
    u8 flag;
    u8 step;
    u8 num;
    u8 data[DEVICE_SOFTVERSION_DATA_LEN+ 2];
    char name[DEVICE_SOFTVERSION_NAME_LEN + 2];
    char tid[DEVICE_SOFTVERSION_TID_LEN + 2];
    char bufferSize[DEVICE_SOFTVERSION_BUFFER_SIZE + 2];
    char md5[DEVICE_SOFTVERSION_MD5 + 2];
    u32 tick;
}DEVICE_UPDATA_INFO;

extern DEVICE_UPDATA_INFO g_sDeviceUpDataInfo;
extern DEVICE_SENVER_TXBUFFER g_sDeviceServerTxBuf;

u32 Device_Search_Data(u8 *pBuffer);

BOOL Device_WeiteData(DEVICE_UPDATA_INFO *pDataInfo);
BOOL Device_CommunCheckRsp(DEVICE_SENVER_TXBUFFER *pCntOp, u8 *pRxBuf);
BOOL Device_Chk_VersionFrame(u8 *pBuffer, DEVICE_UPDATA_INFO *pDataInfo);
BOOL Device_Erase_McuFlash(u32 addr);
BOOL Device_Chk_Version();

void Device_Delayms(u32 n);
void Device_CommunStep(DEVICE_SENVER_TXBUFFER *pCntOp);
void Device_CommunTxCmd(DEVICE_SENVER_TXBUFFER *pCntOp, u32 sysTick);
void Device_Analys_Data(DEVICE_UPDATA_INFO *pDataInfo);
void Device_Format_LinkToken();

char *Device_Analys_Data_Len(char *lenBuffer, u8 step);
#endif