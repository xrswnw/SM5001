#ifndef _ANYID_WAREHOUSE_WIGHT_H
#define _ANYID_WAREHOUSE_WIGHT_H

#include "AnyID_SM5001_Config.h"
#include "AnyID_SM5001_WAREHOUSE_HL.h"
Warehouse
WAREHOUSE
#define WAREHOUSE_REG_ADDR_WGT		0x0001
#define WAREHOUSE_REG_ADDR_ZERO		0x0002
#define WAREHOUSE_REG_ADDR_SPEED		0x0009
#define WAREHOUSE_REG_ADDR_RTO		0x000B
#define WAREHOUSE_REG_ADDR_SET_ADDR		0x000F


#define WAREHOUSE_STAT_IDLE 	                0x00
#define WAREHOUSE_STAT_RCV 		        0x01
#define WAREHOUSE_STAT_OVR 		        0x04
#define WAREHOUSE_STAT_TX 		        0x08
#define WAREHOUSE_STAT_TX_IDLE                0x10
#define WAREHOUSE_STAT_WAIT                   0x20

#define WAREHOUSE_FLAG_IDLE 	                0x00
#define WAREHOUSE_FLAG_CMD 		        0x01
#define WAREHOUSE_FLAG_REG 		        0x04
#define WAREHOUSE_FLAG_OVR		        0x08

#define WAREHOUSE_MODBUS_READ_REG             0x03
#define WAREHOUSE_MODBUS_SET_REG              0x10

#define WAREHOUSE_BUF_MAX_LEN	                0x20
#define WAREHOUSE_BUF_MIN_LEN	                0x09

#define WAREHOUSE_WIGHT_VALUE_STABLE  	0x01
#define WAREHOUSE_WIGHT_VALUE_FAIL      	0x00
#define WAREHOUSEWIGHT_VALUE_NUM		3


#define WAREHOUSE_WIGHT_VALUE_POS             0x03
#define WAREHOUSE_BUFFER_MAX_LEN             (256 + 32) 

#define WAREHOUSE_WOEK_NORMAL                 0x00
#define WAREHOUSE_WORK_SET_ZERO               0x01

#define WAREHOUSE_STAT_OPEN_SAMPLE            0x01
#define WAREHOUSE_STAT_STOP_SAMPLE            0x02


#define WAREHOUSE_STAT_SAMPLE_NUM             3
#define Warehouse_IsRcvFrame(rcvFrame)               ((rcvFrame).state == WAREHOUSE_STAT_RCV || (rcvFrame).state == WAREHOUSE_STAT_OVR)
#define Warehouse_ResetFrame(rcvFrame)               do{(rcvFrame).state = WAREHOUSE_STAT_IDLE; rcvFrame.repeat=0;}while(0)

typedef struct wihgtTxBuf{
    u8 cmd;
    u16 regAdd;
    u16 regNum;
    u16 data0;
    u16 data1;
    u32 len;
    u8 buffer[WAREHOUSE_BUF_MAX_LEN];
    u32 crc16;
}WAREHOUSE_TX_BUF;

typedef struct wihgtRxBuf{
    u8 cmd;
    u32 len;
    u8 hg;
    u8 point;
    u32 wight;
    u16 regAdd;
    u8 buffer[WAREHOUSE_BUF_MAX_LEN];
    u32 crc16;
}WAREHOUSE_RX_BUF;

typedef struct wihgtInfo{
    u8 state;
    u8 flag;
    u8 repeat;
    u8 index;
    u8 num;
    u8 mode;
    u8 witghSmple;
    u32 wightValue;
    u32 wightTemp;
    u32 tick; 
    WAREHOUSE_TX_BUF txBuf;
    WAREHOUSE_RX_BUF rxBuf;
}WAREHOUSE_INFO;
extern WAREHOUSE_INFO g_sWAREHOUSEInfo ;
extern WAREHOUSE_RX_BUF g_sWAREHOUSETempRcv;

void WAREHOUSE_Init();
void WAREHOUSE_TransmitCmd(WAREHOUSE_TX_BUF *pBuffer);
void WAREHOUSE_ChgValue(u32 value, char *buf);
void WAREHOUSE_Adjust(WAREHOUSE_TX_BUF *pBuffer);
void WAREHOUSE_RtoChg(WAREHOUSE_TX_BUF *pBuffer);
void WAREHOUSE_Filt_Chg();
BOOL WAREHOUSE_CheckFrame(WAREHOUSE_RX_BUF *pRcvFrame);

u8 WAREHOUSE_GetValue(WAREHOUSE_INFO *pBuffer);
u8 WAREHOUSE_FormatFrame(u8 cmd,u16 regAdd,u16 regNum,u8 *pFrame);
u16 WAREHOUSE_GetCrc16(u8 *pBuffer, u8 len);



#endif