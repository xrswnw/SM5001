#ifndef _ANYID_SM5001_GATE_
#define _ANYID_SM5001_GATE_

#include "AnyID_SM5001_Gate_HL.h"
#include "AnyID_Uart_Receive.h"


#define GATE_DOOR_OPEN                1
#define GATE_DOOR_CLOSE               0
#define GATE_RFID_OPEN                1
#define GATE_RFID_CLOSE               0
#define GATE_SMOKE_ALARM              1
#define GATE_SMOKE_NORM               0


#define GATE_BAT_NULL                   0
#define GATE_BAT_IN                     1
#define GATE_BAT_OUT                    2
#define GATE_BAT_FAIL                   3
#define GATE_BAT_OK                     4

typedef struct gateStateInfo{
    u8 door: 1;     // 1-开门      0-保持
    u8 rfid: 1;     // 1-开门      0-保持
    u8 smoke: 1;    // 1-报警      0-正常
    u8 fan: 1;      // 1-开启      0-关闭
    u8 rfu: 4;
}GATE_STATINFO;

typedef struct batStatusInfo{
    u16 volValue;       //电池电压，单位0.1V
    u8 volLev;          //电量，单位1%
    u16 cycleCount;     //循环次数
    u8 chargeStatus;    //充电状态 1-空闲 1-放电 2-充电
    u16 maxChagVol;     //满充电压，单位0.1V
    u16 maxChagCur;     //最大充电电流，单位0.1A
    u16 remainCap;      //剩余容量，单位0.01Ah
    u16 curValue;       //充放电电流，单位0.1A
    u8 tmpr;            //温度
    u8 dltVol;          //单体最大压差，单位1mV
}BAT_STATUSINFO;

#define BAT_ERR1_MSK                    0xFF
#define BAT_ERR2_MSK                    0xFA
#define BAT_ERR3_MSK                    0xFF
#define BAT_ERR4_MSK                    0xFF
typedef struct batErrInfo{
    u8 fullState;
    u8 protectStatus1;
    u8 alarmStatus1;
    u8 alarmStatus2;
    u8 diagStatus;
}BAT_ERRINFO;

#define BAT_SN_LEN                      6






#define BAT_STAT_EMPTY                  0x00        //空-无电池
#define BAT_STAT_EXIST                  0x01        //存在电池
#define BAT_STAT_CHARGE                 0x20        //电池充电中
#define BAT_STAT_FULL                   0x40        //电池满电
#define BAT_STAT_ERR                    0x80        //电池故障
#define BAT_UNIT_NUM                    20
extern const u8 g_nBatBoardcastSn[BAT_SN_LEN];
typedef struct batInfo{
    u8 state;
    u16 verHard;
    u16 verSoft;
    u8 sn[BAT_SN_LEN];
    BAT_STATUSINFO status;
    BAT_ERRINFO err;
    u16 unitVol[BAT_UNIT_NUM];
}BAT_INFO;


#define CHAG_STAT_EMPTY         0x00
#define CHAG_STAT_EXIST         0x01        //存在
#define CHAG_STAT_CHARGE        0x20        //电池充电中
#define CHAG_STAT_ERR           0x80        //故障


#define GATE_CTR_DOOR_OPEN	   0x02

#define CHAG_VENDOR_NAME_LEN    4
#define CHAG_ERR_FLAG_LOW_MSK   0x000F
#define CHAG_ERR_CODE_MIN       0x000A
#define CHAG_ERR_FLAG_HIG_MSK   0xCF10
typedef struct chargeDevInfo{
    u8 state;
    u8 vendorName[CHAG_VENDOR_NAME_LEN];
    u16 softVer;
    u16 hardVer;
    u16 type;
    u16 pwr;
    u16 maxVol;
    u16 maxCur;
    u16 chagVol;
    u16 chagCur;
    u16 errStatusInfo;
}CHAG_INFO;

typedef struct gateSensorInfo{
    u16 flag;
    u8 addr;
    u8 tmpr;            //温度
    GATE_STATINFO sensorState;                 
    BAT_INFO batInfo;
    CHAG_INFO chagInfo;
}GATE_SENSORINFO;

#define GATE_VERSION_LEN        8


#define GATE_STAT_UNKNOW            0xFF        //未知状态
#define GATE_STAT_OK                0x00        //正常
#define GATE_STAT_COMERR            0x01        //通信故障
typedef struct gateSlvInfo{
    BOOL bTxInfo;                               //通信故障判断
    u32 txTick;
    u8 state;  
    char softWare[GATE_VERSION_LEN];
    char hardWare[GATE_VERSION_LEN];
    GATE_SENSORINFO sensorInfo;
}GATE_INFO;


typedef struct gateSlvState{
    u8 batState;
    u8 chagState;
    GATE_STATINFO sensorState;
}GATE_STAT;

#define GATE_FRAME_CMD_GET_VERSON       0xF7
#define GATE_FRAME_CMD_PLANE_BAT        0x36
#define GATE_FRAME_CMD_GET_ININFO       0x37
#define GATE_FRAME_CMD_SET_OUTINFO      0x38
#define GATE_FRAME_CMD_CHARGE           0x39
#define GATE_FRAME_CMD_RTNBAT           0x3A
#define GATE_FRAME_CMD_BRWBAT           0x3B
#define GATE_FRAME_CMD_SET_PARAMS       0x3C
#define GATE_FRAME_CMD_GET_PARAMS       0x3D

#define GATE_FRAME_CMD_GATE_TEST        0x88

#define GATE_RRAME_OPEN_DOOR            0x02


typedef struct gateTxFrame{
    u8 buffer[GATE_TX_FRAME_LEN];
    u16 len;
}GATE_TXFRAME;

typedef struct gateRxFrame{
    u8 buffer[GATE_RX_FRAME_LEN];
    u16 len;
    u8 state;
}GATE_RXFRAME;

typedef struct gateBRBatInfo{   //电池操作持续时间 
  u8 flag;
  u32 limitTime;
}GATE_BRBATINFO;

#define GATE_OP_STAT_INIT       0
#define GATE_OP_STAT_IDLE       1
#define GATE_OP_STAT_TX         2
#define GATE_OP_STAT_WAIT       3
#define GATE_OP_STAT_RX         4
#define GATE_OP_STAT_TO         5
#define GATE_OP_STAT_NXT        6
#define GATE_OP_STAT_DLY        7
#define GATE_OP_CMD_WAIT        8

#define GATE_INIT_DLY_TIM       (200 * 3)       //开机延时，等待从机工作正常，才可以操作设别
#define GATE_OP_DLY_TIM         60
#define GATE_OP_TO_TIM          200
#define GATE_OP_MQTT_CMD_TIM    200
#define GATE_OP_TX_TIM          0x05

#define GATE_OP_BR_BAT_TIM      200
#define GATE_OP_BR_BAT_OP_TIM   25 * 200

#define GATE_OP_TIM_FRAME       0xEE

#define CHAG_STEP_NUM           3
#define CHAG_STEP_IDX0          0
#define CHAG_STEP_IDX1          1
#define CHAG_STEP_IDX2          2

#define CHAG_VOL_STEP1          600
#define CHAG_VOL_STEP2          690
#define CHAG_VOL_STEP3          690

#define CHAG_CUR_STEP1          600
#define CHAG_CUR_STEP2          1000
#define CHAG_CUR_STEP3          700

#define CHAG_LOW_VOL_MIN_TIM    2000    //最少充电10s 
#define CHAG_VOL_LEV_LOW        10      //
#define CHAG_VOL_LEV_HIG        90      //
#define CHAG_VOL_LEV_FUL        95      //
typedef struct chagParams{
    u16 fulVolLev;              //充满阈值
    u16 lowVolLev;              //低电量阈值
    u16 higVolLev;              //高电量阈值
    struct chagVolCur{
        u16 vol;
        u16 cur;
    }stepParams[CHAG_STEP_NUM];
}CHAG_PARAMS;

#define GATE_CHAG_ENABLE        1
#define GATE_CHAG_DISABLE       0

#define GATE_SUB_INDEX1         1
#define GATE_SUB_INDEX0         0

#define GATE_PARAMS_LEN         24
typedef struct gateParams{
    u16 rfu;              
    u16 ledLowVolLev;                   //低电压，这是状态指示用的
    u16 alarmTmpr;                      //报警温度阈值
    CHAG_PARAMS chagParams;             //充电机工作参数
}GATE_PARAMS;


//开机延时，再初始化仓控板参数，再获取仓控板信息
#define GATE_SLAVER_NUM         6			//现阶段最大支持六块级联
#define GATE_MODE_STARTUP       0x00    //开机延时，等待仓控板正常工作
#define GATE_MODE_INIT          0x01    //初始化仓控板配置参数
#define GATE_MODE_INFO          0x02    //获取仓控板信息
#define GATE_MODE_CMD           0x03    //命令操作
#define GATE_MODE_GETVER        0x05    //初始化仓控板配置参数
#define GATE_MODE_CTL_CMD       0x04    //命令操作
#define GATE_MODE_PARPER        0x06    //初始化仓控板配置参数
#define GATE_MODE_MAIN_ERR_INFO 0x07    //获取仓控板信息
//#define GATE_MODE_RTNBAT        0x04    //还电池
//#define GATE_MODE_BRWBAT        0x05    //借电池

#define GATE_OPCMD_PARAMS_LEN   64

#define GATE_OP_BAT_STAT_IDLE                       0x00
#define GATE_OP_BAT_STAT_OVER                       0x01
#define GATE_OP_BAT_STAT_ING                        0x02
#define GATE_OP_BAT_STAT_OPEN                       0x04
#define GATE_OP_BAT_STAT_STEP                       0x08


#define GATE_OP_BAT_INFO_REPATNUM                       5

//#define GATE_FLAG_DOOR_TEST             			1



typedef struct gateSlvCmd{
    u8 index;
    u8 cmd;
    u8 paramsLen;
    u8 params[GATE_OPCMD_PARAMS_LEN];
}GATE_SLVCMD;

typedef struct gateOpBatResult{
    u8 step;
    u8 flag;
    u32 tick;
}GATE_OPBAT;

#define GATE_RPT_NUM            4       //通信重发次数
typedef struct gateOpInfo{
    u8 mode;
    u8 batState;
    u8 state;
    u8 add;
    u8 slvIndex;
    u8 batOpState;
    u32 tick;
    u32 tickCmd;
    u32 tickBat;
    GATE_RXFRAME rxFrame;
    GATE_TXFRAME txFrame;
    u32 comErr[GATE_SLAVER_NUM];
    u8 cmd;
    u8 rpt;
    //
    u8 flag;
    u8 relust;
    u8 batInfoRepat;
    GATE_PARAMS *pGateParams;
    GATE_SLVCMD slvCmd;
    GATE_OPBAT brwBat;
    GATE_OPBAT rtnBat;
}GATE_OPINFO;
extern GATE_OPINFO g_sGateOpInfo;
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
#define Gate_StartOpDelay(d, t) do{g_sGateOpInfo.state = GATE_OP_STAT_DLY; g_sGateOpInfo.tick = (t); }while(0)

									
#define Gate_BatOpInfo()		do{\
									g_sGateOpInfo.batInfoRepat = 0;\
									g_sGateOpInfo.batOpState = GATE_OP_BAT_STAT_OVER ;\
									g_sGateOpInfo.rtnBat.step = 0;\
									g_sGateOpInfo.rtnBat.flag = GATE_OP_TIM_FRAME;\
								  }while(0)
    
typedef struct gateTestInfo{
  u8 buffer[GATE_PARAMS_LEN];
  u8 len;
}GATE_TEST_INFO;
                                  
extern GATE_STAT g_aGateSlvStat[GATE_SLAVER_NUM << 1] ;
extern GATE_INFO g_aGateSlvInfo[GATE_SLAVER_NUM << 1];      //每个从站有两个子设备（仓控）
extern GATE_TEST_INFO g_nGateTestInfo ;
extern GATE_BRBATINFO g_nGateBRBatInfo ;

u8 Gate_FormatTestFrame(u8 *pBuffer,u8 add,u8 id,u8 cmd);

BOOL Gate_CheckRspFrame(GATE_RXFRAME *pRxFrame, u16 *pStartPos);

void Gate_Init(GATE_PARAMS *pGateParams, u32 tick);
void Gate_Stop(void);
void Gate_FormatCmd(GATE_OPINFO *pGateOpInfo, u8 *pParams, u16 paramsLen);
void Gate_GetNextOp(GATE_OPINFO *pGateOpInfo, u16 gateNum, u32 tick);
void Gate_TxFrame(GATE_OPINFO *pGateOpInfo, u32 tick);
void Gate_CfgSlaver(GATE_OPINFO *pGateOpInfo);
void Gate_ChagSlaver(GATE_SLVCMD *pGateSlvCmd, u8 index, u8 subIndex, u8 mode);
void Gate_BrwBat(GATE_SLVCMD *pGateSlvCmd, u8 index, u8 subIndex, u8 *pBatSn);

#endif
