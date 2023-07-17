#ifndef _ANYID_SM5001_DEVICE_H
#define _ANYID_SM5001_DEVICE_H

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
#include "AnyID_SM5001_Gate.h"
#include "AnyID_SM5001_Water.h"
#include "AnyID_SM5001_WDG_HL.h"

#include "hmac_sha1.h"                                                                     
#include "base64.h"
#include "md5.h"

extern const PORT_INF DEV_INSEN_LOAD;
#define Device_InSensorLoadLow()                (DEV_INSEN_LOAD.Port->BRR = DEV_INSEN_LOAD.Pin)  
#define Device_InSensorLoadHigh()               (DEV_INSEN_LOAD.Port->BSRR = DEV_INSEN_LOAD.Pin)

extern const PORT_INF DEV_INSEN_DAT;
#define Device_GetInSensor()                     (DEV_INSEN_DAT.Port->IDR & DEV_INSEN_DAT.Pin)

extern const PORT_INF DEV_INSEN_RFID0;


extern const PORT_INF DEV_INSEN_WAT_FB;
#define Device_GetWaterFB()                       (DEV_INSEN_WAT_FB.Port->IDR & DEV_INSEN_WAT_FB.Pin)


#define DEVICE_VERSION_SIZE                   50


#define DEVICE_CMD_RESET                    0x04
#define DEVICE_CMD_GET_CPUID                0xF8
#define DEVICE_CMD_ACTCTL_CTL               0x21
#define DEVICE_CMD_RF_CTRL                  0xF0
#define DEVICE_CMD_SET_CFG                  0xF4
#define DEVICE_CMD_GET_CFG                  0xF5
#define DEVICE_CMD_GET_VERSION              0xF7
#define DEVICE_CMD_GATE_DISENABLE           0x23
#define DEVICE_CMD_GATE_KEY_CTR             0x24
#define DEVICE_CMD_GATE_VOICE_CTR           0x25
#define DEVICE_CMD_CTR_DOOR                 0x21
#define DEVICE_CMD_CTR_LED                  0x26
#define DEVICE_CMD_CTR_RELAY                0x27
#define DEVICE_CMD_CTR_FAN                  0x28
#define DEVICE_CMD_VERSION_UPDATA           0x29

#define DEVICE_CMD_MQTT_KEY_WRITE           0x52
#define DEVICE_CMD_MQTT_GET_IMEI            0x53


#define DEVICE_MQTT_FRAME_CMDID_TAG_1           8
#define DEVICE_MQTT_FRAME_CMDID_TAG_2           13
#define DEVICE_MQTT_FRAME_CMDID_TAG_3           18
#define DEVICE_MQTT_FRAME_CMDID_TAG_4           23

#define DEVICE_MQTT_FRAME_CMDID_MASK            0x2D
#define DEVICE_SERVER_SATA_IDLE             0
#define DEVICE_SERVER_SATA_WATI_TX          1
#define DEVICE_SERVER_SATA_TX               2
#define DEVICE_SERVER_SATA_RX               3


#define DEVICE_CTR_DOOR_TIME                    100
#define DEVICE_TEMPR_HIGH                       32

#define DEVICE_HEART_MIN                     570

#define DEVICE_SET_CFG_FRAME_LEN            0x1C    
#define DEVICE_ACTCTL_CTL_FRAME_LEN         0x05  
#define DEVICE_RETURN_BAT_FRAME_LEN         0x0A

#define DEVICE_FREAM_MIN_LEN            5

#define DEVICE_REPEAT_TIME              5

#define DEVICE_IO_SAMPLE_TIME           2

#define DEVICE_STEP_OVER        0x00
#define DEVICE_STEP_ONE         0x01
#define DEVICE_STEP_TWICE       0x02

#define DEVICE_MASK_FLAG_NULL                   0
#define DEVICE_MASK_FLAG_MAST                   0x01
#define DEVICE_STEP_FLAG_OK                     0x00
#define DEVICE_STEP_FLAG_DOOR_FAIL              0x01
#define DEVICE_STEP_FLAG_BAT_NO_RTN             0x02
#define DEVICE_STEP_FLAG_BAT_FRAME_ERR          0x03

#define DEVICE_STEP_FLAG_DOOR_OK                0x00
#define DEVICE_STEP_FLAG_DOOR_LOADING           0x01

#define DEVICE_STEP_FLAG_DOOR_NOCLOSE_BAT_INSER  0x01
#define DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_NOINSER  0x02
#define DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_FAIL     0x04
#define DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_LOADING  0xFF

#define DEVICE_FRAME_FLAG_OK             0x00
#define DEVICE_FRAME_FLAG_FAIL           0x01
#define DEVICE_FRAME_INFO_OK             0x00
#define DEVICE_FRAME_INFO_FAIL           0x04

#define DEVICE_IO_DEVICE_CLOSE           0x00
#define DEVICE_IO_DEVICE_OPEN            0x01

#define DEVICE_FRAME_ERR_DATA            0x01

#define DEVICE_RSPFRAME_DAT_LEN         (5120)
#define DEVICE_RSPFRAME_LEN             (DEVICE_RSPFRAME_DAT_LEN + 32)

#define DEVICE_RSPFRAME_FLAG_OK          0x00
#define DEVICE_RSPFRAME_FLAG_FAIL        0x01
#define DEVICE_OPTAG_RESPONSE_NOERR      0x00
#define DEVICE_OPTAG_RESPONSE_TAGERR     0x01
#define DEVICE_OPTAG_RESPONSE_CRCERR     0x02
#define DEVICE_OPTAG_RESPONSE_NORSP      0x03
#define DEVICE_OPTAG_RESPONSE_PARERR     0x04


#define READER_RESPONSE_NOERR           0x00
#define READER_RESPONSE_ERR_LEN         0x01
#define READER_RESPONSE_ERR_FREAM       0x02
#define READER_RESPONSE_ERR_DEVICE      0x03

#define DEVICE_RESPONSE_FLAG_RESET      0x01
#define DEVICE_RESPONSE_FLAG_UPDATA     0x02

#define DEVICE_FRAME_BROADCAST_ADDR     0xFFFF


#define DEVICE_SERVER_TXSTAT_IDLE       0x00000000
#define DEVICE_SERVER_TXSTAT_WAIT       0x00000001
#define DEVICE_SERVER_TXSTAT_OVER       0x00000002
#define DEVICE_SERVER_TXSTAT_OK         0x00000004
#define DEVICE_SERVER_TXSTAT_RX         0x00000008
#define DEVICE_SERVER_TXSTAT_BUFF       0x00000010
#define DEVICE_SERVER_TXST_AT           0x00000020
#define DEVICE_SERVER_TXST_STEP         0x00000040
#define DEVICE_SERVER_TXSTAT_RX_AT      0x00000080


#define DEVICE_SERVERRSP_NUM            20
#define DEVICE_TEST_SERSOR				2
//modele

#define DEVICE_GATE_OP_TICK             20

//参数校验
#define DEVICE_NORMAL_ADDR              0x01
#define DEVICE_NORMAL_HEART_TIME        20
#define DEVICE_NORMAL_TEMPR_MAST        40
//

#define DEVICE_MODELE_LED               0x01
#define DEVICE_MODELE_FAN               0x02
#define DEVICE_MODELE_MOTOR             0x04
#define DEVICE_MODELE_RELAY             0x08

#define DEVICE_BAT_SN_LEN               6

#define DEVICE_BAT_RTN                  0x01
#define DEVICE_BAT_BRW                  0x02 

#define DEVICE_HEART_TIME				1
#define DEVICE_CMD_NORMAL               0x00
#define DEVICE_TIME_HEART               50                      //测试5s

#define DEVICE_SERVER_BAT_VALUE_LEN     5
#define DEVICE_SERVER_ID_LEN            8
#define DEVICE_ID_LEN                   W232_IMEI_LEN
#define DEVICE_SERVER_AUTH_CODE_LEN     43
#define DEVICE_SERVER_TX_RPT            3

#define DEVICE_TAG_RCD_SIZE             1500    
#define DEVICE_RSP_ID_LEN               36


#define DEVICE_SM5001_ID                0xFF
#define DEVICE_SM5002_ID                0x05
#define DEVICE_SM5003_ID                0x00

#define DEVICE_TMPR_ALARM_DFT           30
#define DEVICE_LED_LOWVOL_DFT           80

#define DEVICE_CRC32_LEN                4

#define DEVICE_STAT_CHK_TIM             5
#define DEVICE_MODE_SFG                 0x00
#define DEVICE_MODE_NORMAL              0x01

#define DEVIDE_MARK_MAIN                0x00000001
#define DEVIDE_MARK_GATE                0x00000002
#define DEVIDE_MARK_REBAT               0x00000004
#define DEVIDE_MARK_BWBAT               0x00000008
#define DEVICE_OUT_NULL                 0x00

#define DEVICE_OUT_CTRL_POS_FAN         0x01
#define DEVICE_OUT_CTRL_POS_DOOR        0x02

#define DEVICE_OUT_CTRL_POS_MOTOR       0x01

#define DEVICE_GET_GATEINFO_MODE_ONCE   0x00
#define DEVICE_GET_GATEINFO_MODE_ALL    0xFF

#define DEVICE_FLAG_MAST                5
#define DEVICE_GATE_LEN                 21
#define DEVICE_VERSION_LEN              8
#define DEVICE_SERVER_RSP_LEN           1500

#define DEVICE_KEY_UID_LEN              0x0C



#define DEVICE_TEST_FLAG_DOOE_MODE           0x00000001

#define DEVICE_TEST_DOOR_OPEN_TIM               5
#define DEVICE_GATE_OPEN_DOOR                   0x02

#define DEVICE_TEST_ERR_TEMPR		    0x01
#define DEVICE_TEST_ERR_ELECT		    0x02



#define Device_ChkGate(v)                      (v != DEVICE_SM5001_ID && v != DEVICE_SM5003_ID)
#define Device_AnsyFrame(add, c, frame)        do{g_sGateOpInfo.mode = GATE_MODE_CMD;g_sGateOpInfo.state = GATE_OP_STAT_TX;g_sGateOpInfo.cmd = c;g_sGateOpInfo.slvIndex = (add -1)>> 1;g_sGateOpInfo.slvCmd.paramsLen = 2;g_sGateOpInfo.slvCmd.params[0] = (add + 1)% 2;g_sGateOpInfo.slvCmd.params[1] = frame;}while(0)    
#define Device_AnsyTestFrame(add, c, frame)   do{g_sGateOpInfo.mode = GATE_MODE_CMD;g_sGateOpInfo.state = GATE_OP_STAT_TX;g_sGateOpInfo.cmd = c;g_sGateOpInfo.slvIndex = add ;g_sGateOpInfo.slvCmd.paramsLen = 2;g_sGateOpInfo.slvCmd.params[0] = (add + 1)% 2;g_sGateOpInfo.slvCmd.params[1] = frame;}while(0)
#define Device_GateRtBat(add)                  do{g_sGateOpInfo.add = add - 1;g_sGateOpInfo.state = GATE_OP_STAT_TX;g_sGateOpInfo.mode = GATE_MODE_CMD;g_sGateOpInfo.cmd = GATE_FRAME_CMD_RTNBAT;g_sGateOpInfo.slvIndex = (add -1)>> 1;g_sGateOpInfo.slvCmd.params[0] = (add + 1)% 2;g_sGateOpInfo.slvCmd.paramsLen = DEVICE_BAT_SN_LEN + 1;}while(0)                                 
#define Device_GateBrBat(add)                  do{g_sGateOpInfo.add = add - 1;g_sGateOpInfo.state = GATE_OP_STAT_TX;g_sGateOpInfo.mode = GATE_MODE_CMD;g_sGateOpInfo.cmd = GATE_FRAME_CMD_BRWBAT;g_sGateOpInfo.slvIndex = (add -1)>> 1;g_sGateOpInfo.slvCmd.params[0] = (add + 1)% 2;g_sGateOpInfo.slvCmd.paramsLen = DEVICE_BAT_SN_LEN + 1;}while(0) 
#define Device_GatePlBat(add,v)                do{g_sGateOpInfo.add = add - 1;g_sGateOpInfo.state = GATE_OP_STAT_TX;g_sGateOpInfo.mode = GATE_MODE_CMD;g_sGateOpInfo.cmd = GATE_FRAME_CMD_PLANE_BAT;g_sGateOpInfo.slvIndex = (add -1)>> 1;g_sGateOpInfo.slvCmd.params[0] = (add + 1)% 2;g_sGateOpInfo.slvCmd.params[1]= v;g_sGateOpInfo.slvCmd.paramsLen = DEVICE_BAT_SN_LEN + 1 + 1;}while(0)
                                          

#define Device_VoiceApoFrame(opt,repat,cmd,vn)     do{g_sSoundInfo.txBuf.index = 0;g_sSoundInfo.txBuf.num = 0;/**/g_sSoundInfo.txBuf.cd[g_sSoundInfo.txBuf.num] = SOUND_FRAME_CMD_APPOINT_FOLDER; g_sSoundInfo.txBuf.to[g_sSoundInfo.txBuf.num] = opt;g_sSoundInfo.txBuf.id[g_sSoundInfo.txBuf.num] = vn;g_sSoundInfo.txBuf.repeat[g_sSoundInfo.txBuf.num] = repat;g_sSoundInfo.txBuf.op[g_sSoundInfo.txBuf.num++] = cmd;g_sSoundInfo.state = SOUND_STAT_TX;}while(0)
#define Device_VoiceCtrFrame(opt,repat,cmd,vn)     do{g_sSoundInfo.txBuf.index = 0;g_sSoundInfo.txBuf.num = 0;g_sSoundInfo.txBuf.to[g_sSoundInfo.txBuf.num] = opt;g_sSoundInfo.txBuf.id[g_sSoundInfo.txBuf.num] = vn;g_sSoundInfo.txBuf.repeat[g_sSoundInfo.txBuf.num] = repat;g_sSoundInfo.txBuf.cd[g_sSoundInfo.txBuf.num] = SOUND_FRAME_CMD_VOL_STR_AOP;g_sSoundInfo.txBuf.op[g_sSoundInfo.txBuf.num++] = cmd;g_sSoundInfo.state = SOUND_STAT_TX;}while(0)
#define Device_AtRsp(opt,repat,cmd)                do{g_sDeviceServerTxBuf.to[g_sDeviceServerTxBuf.num] = opt;g_sDeviceServerTxBuf.repeat[g_sDeviceServerTxBuf.num] = repat;g_sDeviceServerTxBuf.op[g_sDeviceServerTxBuf.num++] = cmd;g_sDeviceServerTxBuf.state |= DEVICE_SERVER_TXST_AT;}while(0)


#define Device_ChkDeviceStat(stat)          ({\
                                                  u8 bOk = 0;\
                                                  if(g_sIoInfo.deviceState & stat)\
                                                  {\
                                                        bOk = 1;\
                                                  }\
                                                    bOk;\
                                                })



#define Device_ChkBat_Num()       ({\
                                         u8 i = 0;\
                                         u8 num = 0;\
                                         for(i = 0; i < (GATE_SLAVER_NUM << 1); i++)\
                                         {\
                                                if(g_aGateSlvInfo[i].sensorInfo.batInfo.sn[0] != 0x00 && g_aGateSlvInfo[i].sensorInfo.batInfo.sn[1] != 0x00 && g_aGateSlvInfo[i].sensorInfo.batInfo.sn[5] != 0x00)\
                                                  {\
                                                    num ++;\
                                                  }\
                                         }\
                                         num;\
                                    })\


#define Device_Chk_Uid(p)       (p[0] != 0x00 && p[1] != 0x00 && p[1] != 0x00 && p[2] != 0x00 && p[3] != 0x00 && p[4] != 0x00 && p[5] != 0x00 && p[6] != 0x00) 
#define Gate_ClearOpInfo()      do{\
                                    g_sGateOpInfo.mode = 0;\
                                    g_sGateOpInfo.state = 0;\
                                    g_sGateOpInfo.slvIndex = 0;\
                                    g_sGateOpInfo.tick = 0;\
                                    memset(&g_sGateOpInfo.rxFrame, 0, sizeof(GATE_RXFRAME));\
                                    memset(&g_sGateOpInfo.txFrame, 0, sizeof(GATE_TXFRAME));\
                                    g_sGateOpInfo.cmd = 0;\
                                    g_sGateOpInfo.rpt = 0;\
                                }while(0)

typedef struct deviceParams{
    W232_PARAMS serverParams;
    WINAVG_INFO temprUp;
    WINAVG_INFO temprDown;
    GATE_PARAMS gateParams; 
    u8 voiceSth; 
    u8 electMode;  
    u8 offLineTime;
    u8 addr; 
    u8 rfu1; 
    u16 gateTick;                       //主控心跳
    u16 gateNum;                        //仓控板数目
    u16 gateTxTick;                     //仓信息上报记录时间间隔
    u16 rfu2;
    u16 heartTick;
    GATE_PARAMS gateParams0; 
    u32 crc;
}DEVICE_PARAMS;

typedef struct deviceTest{
     u8 num;
     u8 index;  
     u32 flag;
}DEVICE_TEST ;

typedef struct readerRspFrame{
    u16 len;
    u8 buffer[DEVICE_RSPFRAME_LEN];
    u8 admFlag;
    u8 flag;
    u8 err;
    u8 state;
    u8 cmd;
    u32 mark;
    u8 repeat;
}READER_RSPFRAME;

typedef struct deviceSenverTxBuff{
    u8 num;
    u8 index;
    u8 result;
    u8 sigNum;
    u8 channelErrNum;
    u16 len;
    u8 id[W232_CNT_OP_NUM];
    u8 op[W232_CNT_OP_NUM];
    u8 buffer[DEVICE_SERVER_RSP_LEN];
    u8 repeat[W232_CNT_OP_NUM];
    u32 to[W232_CNT_OP_NUM];
    u32 tick;
    u32 state;
}DEVICE_SENVER_TXBUFFER;

typedef struct deviceImpRspInfo{
  u8 add;
  u8 rtuLen;
  u8 brwLen;
  u8 warnLen;
  u8 rtuBuffer[UART_FRAME_POS_MQTT_NORMAL_LEN];
  u8 brwBuffer[UART_FRAME_POS_MQTT_NORMAL_LEN];
  u8 warnBuffer[UART_FRAME_POS_MQTT_NORMAL_LEN];
}DEVICE_IMPRSP_INFO;

typedef struct deviceTestInfo{
	u8 flag;
	u8 gateAddr;
	u8 err;
	u32 tick;
}DEVICE_TEST_INFO;



extern BOOL g_nBatOpenFlag;
extern u32 g_nBratIngTick  ;
extern DEVICE_IMPRSP_INFO g_sDeviceImpRspInfo;
extern DEVICE_SENVER_TXBUFFER g_sDeviceServerTxBuf;
extern READER_RSPFRAME g_sDeviceRspFrame;
extern DEVICE_PARAMS g_sDeviceParams;
extern DEVICE_TEST_INFO g_sDeviceTestInfo;


void Device_Init();
void Device_ServerProcessRxInfo(W232_RCVBUFFER *pRcvBuffer, u32 tick); 
void Reader_Delayms(u32 n);
void Device_GateRsvSlvInfo(u8 *pParams, u16 paramsLen, GATE_OPINFO *pOpInfo);
void Device_ReadDeviceParamenter(void);
void Device_ReadMqttKey();
void Device_IoCtr();
void Device_CtrBatVolce(u16 add, u8 mode, u8 step, u8 flag);
void Device_VoiceCtr();
void Device_InfoChgRsp(u8 *pBuffer, char *strAtBuff, char *strRspBuff, u8 addr, u32 id);
void Device_RtuRsp(u8 *pBuffer, char *strAtBuff, char *strRspBuff, u8 addr, u32 id);
void Device_BrwRsp(u8 *pBuffer, char *strAtBuff, char *strRspBuff, u8 addr, u32 id);
void Device_UpHeartData(u32 id, u8 *pBuffer, char *strAtBuff, char *strRspBuff, u16 lenth);
void Device_PostRsp(W232_CONNECT *pCntOp0,u8 *pBuffer, char *strAtBuff, char *strRspBuff,u16 len);
void Device_CommunTxCmd(DEVICE_SENVER_TXBUFFER *pCntOp, u32 sysTick);
void Device_CommunStep(DEVICE_SENVER_TXBUFFER *pCntOp);
void Device_GateStateInit();
void Device_GateStateRsp();
void Device_GateBatTwice(u8 cmd, u8 addr);
BOOL Device_ChkSersor();

BOOL Device_CheckRsp(W232_CONNECT *pCntOp, u8 *pRxBuf, u8 len) ;
BOOL Device_WriteDeviceParamenter(void);
BOOL Device_SetCfg(u8 *pBuffer);
BOOL Device_WriteMqttKey();
BOOL Device_GateProceRspFrame(u8 *pFrame, GATE_OPINFO *pOpInfo, u32 tick);
BOOL Device_CommunCheckRsp(DEVICE_SENVER_TXBUFFER *pCntOp, u8 *pRxBuf);
BOOL Device_ChkVersion();
BOOL Device_ParmenterChkCode(DEVICE_PARAMS *pPaementInfo);

u16 Device_WaterProceRspFrame(u8 *pFrame, WATER_INFO *pOpInfo, u8 len);
u16 Device_ResponseGateFrame(u8 add, u8 mode, READER_RSPFRAME *pOpResult);
u16 Device_ResponseFrame(u8 *pParam, u8 len, READER_RSPFRAME *pOpResult);
u16 Reader_ProcessUartFrame(u8 *pFrame, u8 add, u16 len, u32 tick);
u16 Device_HeartFormat(u8 *pBuffer, u32 tick);
u16 Device_ResponseBrBat(u8 *pBuffer);
u16 Device_ResponseBat(READER_RSPFRAME *pOpResult, u8 mode);
u16 Device_ResponseRtBat(u8 *pBuffer);
u16 Device_ResponseCfg( READER_RSPFRAME *pOpResult);
u16 Device_ResponseInfoChg(u8 *pBuffer, u8 add, u8 gateState, u8 batState, u8 chagState, u32 mainState);
u16 Device_GateResponse(READER_RSPFRAME *pOpResult, u8 *pParam, u8 len);

#endif
