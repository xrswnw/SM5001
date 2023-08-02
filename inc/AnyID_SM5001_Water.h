#ifndef _ANYID_SM5001_WATER_H
#define _ANYID_SM5001_WATER_H

#include "AnyID_SM5001_Config.h"
#include "AnyID_SM5001_Water_HL.h"
#include "AnyID_Uart_Receive.h"


#define WATER_STAT_IDLE                 0x00
#define WATER_STAT_RCV                  0x01
#define WATER_STAT_OVR 		        0x04
#define WATER_STAT_TX 		        0x08
#define WATER_STAT_TX_IDLE              0x10
#define WATER_STAT_WAIT                 0x20


#define WATER_COM_OP_TICK             3
#define WATER_BUF_MAX_LEN               48
#define WATER_UID_LEN                   8
#define WATER_BUFFER_MAX_LEN            (256 + 32) 

#define WATER_MODE_INFO          0x01    //获取噴淋信息
#define WATER_MODE_CMD           0x02    //命令操作

#define WATER_STAT_MODE_RFID            0
#define WATER_STAT_MODE_WATER_1         1
#define WATER_STAT_MODE_WATER_2         2

#define WATER_ADDR_RFID                 0x0001
#define WATER_ADDR_WATER_1              0x0002
#define WATER_ADDR_WATER_2              0x0003

#define WATER_SAMPLE_NUM                5
#define WATER_SAMPLE_TIME               3

#define WATER_SAMPLE_OP_TIME            100

#define WATER_NUM                       12
#define WATER_GETUID_BUFFER_LEN         7
#define Water_IsRcvFrame(rcvFrame)               ((rcvFrame).state == WATER_STAT_RCV || (rcvFrame).state == WATER_STAT_OVR)
#define Water_ResetFrame(rcvFrame)               do{(rcvFrame).state = WATER_STAT_IDLE; rcvFrame.rxBuf.len = 0;memset(&(rcvFrame.rxBuf), 0, sizeof(WATER_RX_BUF));}while(0)
#define Water_ResetFrame0(rcvFrame)              (memset(&(rcvFrame), 0, sizeof(WATER_RX_BUF)))

#define WATER_GET_UID_BUFFER_LEN        0x14

#define WATER_CMD_GET_UID               0x30
#define WATER_CMD_GET_WATER_INFO        0x47
#define WATER_CMD_GET_WATER_OUTCTR      0x48


#define WATER_BUFFER_PAEMAS_LEN         20
#define WATER_DEVICE_NUM                20

                             
typedef struct WaterTxBuf{
    u8 len;
    u8 buffer[WATER_BUF_MAX_LEN];
    u8 flag;
    u8 err;
    u8 mode;
    u8 cmd;
    u16 addr;
    BOOL result[WATER_DEVICE_NUM];
    u8 repat[WATER_DEVICE_NUM];
    u32 tick[WATER_DEVICE_NUM];
}WATER_TX_BUF;

typedef struct WaterRxBuf{
    u8 cmd;
    u32 len;
    u8 buffer[WATER_BUF_MAX_LEN];
    u32 crc16;
}WATER_RX_BUF;


typedef struct waterOpInfo{
    u16 addr;
    u8 tempr;
    u8 state;
}WATER_OPINFO;


typedef struct WatertInfo{
    u8 state;
    u8 flag;
    u8 mode;
    u8 parmas;
	u8 comErr;
	u8 smapleTick;
	u8 smapleUid[WATER_UID_LEN];
    u8 uid[WATER_UID_LEN];
    WATER_OPINFO slaveInfo[2];
    WATER_TX_BUF txBuf;
    WATER_RX_BUF rxBuf;
}WATER_INFO;


typedef struct waterRspFrame{
    u16 len;
    u8 buffer[WATER_BUF_MAX_LEN];
    u8 flag;
    u8 err;
    u8 state;
    u8 cmd;
    u8 repeat;
}WATER_RSPFRAME;
extern WATER_RSPFRAME g_sWaterRspFrame;

extern WATER_INFO g_sWaterInfo ;
extern WATER_RX_BUF g_sWaterTempRcv;
extern u8 g_nWaterGetUidBuffer[WATER_GETUID_BUFFER_LEN];

BOOL Water_CheckFrame(WATER_RX_BUF *pRcvFrame);
BOOL Water_CheckRspFrame(WATER_RX_BUF *pRxFrame, u16 *pStartPos);

void Water_Init();
void Water_TransmitCmd(WATER_INFO *pWaterInfo, u32 tick);
#endif