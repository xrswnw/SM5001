#ifndef _ANYID_SM5001_READER_H
#define _ANYID_SM5001_READER_H

#include "AnyID_SM5001_Config.h"
#include "AnyID_SM5001_Uart.h"
#include "AnyID_SM5001_FRam.h"
#include "AnyID_SM5001_W232.h"
#include "AnyID_SM5001_SysTick_HL.h"
#include  "stdlib.h"
#include "AnyID_Uart_Receive.h"
#include "AnyID_FRam.h"
#include "AnyID_SM5001_IO.h"
#include "AnyID_SM5001_Elect.h"
#include "AnyID_SM5001_Sound.h"


#define READER_BOX_NUM                  0x06


#define READER_VERSION_SIZE                   50
#define READER_CMD_RESET                    0x04
#define READER_CMD_GET_CPUID                0xF8
#define READER_CMD_ACTCTL_CTL               0x21
#define READER_CMD_RF_CTRL                  0xF0
#define READER_CMD_SET_CFG                  0xF4
#define READER_CMD_GET_CFG                  0xF5
#define READER_CMD_GET_VERSION              0xF7



#define READER_SET_CFG_FRAME_LEN            0x1C    
#define READER_ACTCTL_CTL_FRAME_LEN         0x05  

#define READER_REPEATTIME                     5

#define READER_REVFRAM_LEN_POS                2
#define READER_REVFRAM_LEN_MQTT_POS           1

#define READER_FRAME_FLAG_OK             0x00
#define READER_FRAME_FLAG_FAIL           0x01

#define READER_FRAME_INFO_OK             0x00
#define READER_FRAME_INFO_FAIL           0x04



#define READER_FRAME_ERR_DATA           0x01

#define READER_RSPFRAME_DAT_LEN         (2560)
#define READER_RSPFRAME_LEN             (READER_RSPFRAME_DAT_LEN + 32)

#define READER_RSPFRAME_FLAG_OK          0x00
#define READER_RSPFRAME_FLAG_FAIL        0x01
#define READER_OPTAG_RESPONSE_NOERR      0x00
#define READER_OPTAG_RESPONSE_TAGERR     0x01
#define READER_OPTAG_RESPONSE_CRCERR     0x02
#define READER_OPTAG_RESPONSE_NORSP      0x03
#define READER_OPTAG_RESPONSE_PARERR     0x04


#define READER_RESPONSE_NOERR           0x00
#define READER_RESPONSE_ERR_LEN         0x01
#define READER_RESPONSE_ERR_FREAM       0x02
#define READER_RESPONSE_ERR_DEVICE      0x03

#define READER_RESPONSE_FLAG_RESET      0x01

#define READER_FRAME_BROADCAST_ADDR         0xFFFF

#define READER_SVR_CMD_LOGIN            0x01
#define READER_SVR_CMD_UP               0x02
#define READER_SVR_CMD_DOWN             0x03

#define READER_SERVER_TXSTAT_IDLE       0
#define READER_SERVER_TXSTAT_WAIT       1
#define READER_SERVER_TXSTAT_OVER       2
#define READER_SERVER_TXSTAT_OK         3
#define READER_SERVER_TXSTAT_RX         4

//modele

#define READER_MODELE_LED               0x01
#define READER_MODELE_FAN               0x02
#define READER_MODELE_MOTOR             0x04
#define READER_MODELE_RELAY             0x08

#define READER_CMD_NORMAL               0x00
#define READER_TIME_HEART               50                      //测试5s

#define READER_SERVER_BAT_VALUE_LEN     5
#define READER_SERVER_ID_LEN            8
#define READER_ID_LEN                   W232_IMEI_LEN
#define READER_SERVER_AUTH_CODE_LEN     43
#define READER_SERVER_TX_RPT            3

#define READER_TAG_RCD_SIZE             1500    
#define READER_RSP_ID_LEN               36


#define READER_SM5001_ID                0xFF
#define READER_SM5003_ID                0x00

typedef struct flashDevicePar{
    W232_PARAMS serverParams;
    u8 workMode;
    u8 heartTime;                       //心跳頻率
    u8 boxTime;                         //倉信息上報頻率
    u8 boxNum;                          //倉數目，一個蒼控板控制兩個倉
    u8 rfu1;                            
    u16 rfu2;
    u16 lowVoltageWran;                 //電池低電壓指示閾值
    u16 boxFanTemp;                     //倉風扇開啓溫度閾值
    u16 fullElect;                      //電池滿電量閾值
    u16 lowElect;                       //電池低電量閾值
    u16 highElect;                      //電池高電量閾值
    u16 lowVoltageCharge;               //低電壓充電電壓
    u16 lowCurrentCharge;               //低電壓充電電流
    u16 fastVoltageCharge;              //快充充電電壓
    u16 fastCurrentCharge;              //快充充電電流
    u16 slowVoltageCharge;              //慢充充電電壓
    u16 slowCurrentCharge;              //慢充充電電流
    u8 addr;                    
    u32 crc;
}READER_DEVPARAMS;
extern READER_DEVPARAMS g_sReaderDevParams;



typedef struct readerRspFrame{
    u16 len;
    u8 buffer[READER_RSPFRAME_LEN];
    u8 flag;
    u8 err;
    u8 state;
    u8 cmd;
    u8 repeat;
}READER_RSPFRAME;
extern READER_RSPFRAME g_sDeviceRspFrame;

typedef struct deviceInfo{
    WINAVG_INFO tempr;
}DEVICE_INFO;
extern DEVICE_INFO g_sDeviceInfo;





typedef struct readerServerTx{
    u8 buf[READER_TAG_RCD_SIZE];
    u16 len;
    u8 repeat;
    u8 state;
    u8 id[READER_RSP_ID_LEN];
    u32 tick;
}READER_SERVERTX;
extern READER_SERVERTX g_sReaderServerTx;

void Reader_Init();
void Reader_ServerProcessRxInfo(W232_RCVBUFFER *pRcvBuffer, READER_SERVERTX *pServerTx); 
void Modele_Ctl(u8 cmd);
void Reader_Delayms(u32 n);
void Reader_UpHeartData(u32 id, u8 *pBuffer, u16 lenth);


BOOL Reader_ReadDeviceParamenter(void);
BOOL Reader_WriteDeviceParamenter(void);


u16 Device_ResponseFrame(u8 *pParam, u8 len, READER_RSPFRAME *pOpResult);
u16 Reader_ProcessUartFrame(u8 *pFrame, u8 add, u16 len);
u16 Reader_HeartFormat(u8 *pBuffer, u8 *pWraeBuffer, u8 *pWaterBuffer);


#endif
