#ifndef _ANYID_SM5001_ELECT_H
#define _ANYID_SM5001_ELECT_H

#include "AnyID_SM5001_Config.h"
#include "AnyID_SM5001_Elect_HL.h"

#define ELECT_REG_ADDR_ELECTVALUE	0x4000
#define ELECT_REG_ADDR_A_V_VALUE	0x2000
#define ELECT_REG_ADDR_A_I_VALUE	0x2002




#define ELECT_GET_VALUE_FAIL            0xFFFFFFFF

#define ELECT_STAT_IDLE                 0x00
#define ELECT_STAT_RCV 		        0x01
#define ELECT_STAT_OVR 		        0x04
#define ELECT_STAT_TX 		        0x08
#define ELECT_STAT_TX_IDLE              0x10
#define ELECT_STAT_WAIT                 0x20

#define ELECT_FLAG_IDLE                 0x00
#define ELECT_FLAG_CMD 		        0x01
#define ELECT_FLAG_REG 		        0x04
#define ELECT_FLAG_OVR		        0x08



#define ELECT_SAMPLE_TIME               60
#define ELECT_MODBUS_READ_REG           0x03
#define ELECT_MODBUS_SET_REG            0x10

#define ELECT_BUF_MAX_LEN               256
#define ELECT_BUF_MIN_LEN               0x09

#define ELECT_WIGHT_VALUE_STABLE  	0x01
#define ELECT_WIGHT_VALUE_FAIL      	0x00
#define ELECTWIGHT_VALUE_NUM		3


#define ELECT_ELECT_VALUE_POS             0x03
#define ELECT_BUFFER_MAX_LEN             (256 + 32) 

#define ELECT_WOEK_NORMAL                 0x00
#define ELECT_WORK_SET_ZERO               0x01

#define ELECT_STAT_OPEN_SAMPLE            0x01
#define ELECT_STAT_STOP_SAMPLE            0x02

#define ELECT_OP_TIME                           20
#define ELECT_SAMPLE_NUM             3
#define Elect_IsRcvFrame(rcvFrame)               ((rcvFrame).state == ELECT_STAT_RCV || (rcvFrame).state == ELECT_STAT_OVR)
#define Elect_ResetFrame(rcvFrame)               do{(rcvFrame).state = ELECT_STAT_IDLE; rcvFrame.repeat=0;}while(0)

#define Elect_GetValue(v)               do{v.electValue = (v.rxBuf.buffer[ELECT_ELECT_VALUE_POS] << 24) |(v.rxBuf.buffer[ELECT_ELECT_VALUE_POS + 1] << 16) | (v.rxBuf.buffer[ELECT_ELECT_VALUE_POS + 2] <<  8) | (v.rxBuf.buffer[ELECT_ELECT_VALUE_POS + 3] << 0);}while(0)


typedef struct electTxBuf{
    u8 cmd;
    u32 len;
    u16 regAdd;
    u16 regNum;
    u8 buffer[ELECT_BUF_MAX_LEN];
    u32 crc16;
}ELECT_TX_BUF;

typedef struct electRxBuf{
    u8 cmd;
    u32 len;
    u8 buffer[ELECT_BUF_MAX_LEN];
    u32 crc16;
}ELECT_RX_BUF;

typedef struct electtInfo{
    u8 state;
    u8 flag;
    u8 repeat;
    u8 index;
    u8 num;
    u32 electValue;
    u32 value;
    u32 tick; 
    u32 time;
    ELECT_TX_BUF txBuf;
    ELECT_RX_BUF rxBuf;
}ELECT_INFO;
extern ELECT_INFO g_sElectInfo ;
extern ELECT_RX_BUF g_sElectTempRcv;

u8 Elect_FormatFrame(u8 cmd,u16 regAdd,u16 regNum,u8 *pFrame);
u16 Elect_GetCrc16(u8 *pBuffer, u8 len);
u32 Float32(u32 NumFloat32);

BOOL Elect_CheckFrame(ELECT_RX_BUF *pRcvFrame);

void Elect_Init();
void Elect_TransmitCmd(ELECT_TX_BUF *pBuffer);

#endif