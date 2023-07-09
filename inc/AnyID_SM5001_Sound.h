#ifndef _ANYID_SM5001_SOUND_
#define _ANYID_SM5001_SOUND_

#include "AnyID_SM5001_Config.h"
#include "AnyID_SM5001_Sound_HL.h"
#include "AnyID_SM5001_SysTick_HL.h"

#define SOUND_STAT_IDLE              0x80
#define SOUND_STAT_RCV               0x01
#define SOUND_STAT_OVR               0x02
#define SOUND_STAT_TX                0x04
#define SOUND_STAT_WAIT              0x08
#define SOUND_STAT_END               0x10
#define SOUND_STAT_STEP              0x20
#define SOUND_STAT_CHK               0x40

#define SOUND_FLAG_IDLE              0x00
#define SOUND_FLAG_HRAD              0x01
#define SOUND_FLAG_ADD               0x02
#define SOUND_FLAG_LEN               0x03
#define SOUND_FLAG_DATA              0x04
#define SOUND_FLAG_FAIL              0x04

#define SOOUND_FEEDBACK_NO              0
#define SOOUND_FEEDBACK_NEED            1
#define SOUND_FRAME_HEAD                0x7E
#define SOUND_FRAME_EOF                 0xEF
#define SOUND_FRAME_ADD                 0xFF
#define SOUND_FRAME_CMD_APPOINT_NUM     0x03
#define SOUND_FRAME_CMD_APPOINT_FOLDER  0x0F
#define SOUND_FRAME_CMD_VOL_HIGH        0x04
#define SOUND_FRAME_CMD_VOL_LOW         0x05
#define SOUND_FRAME_CMD_VOL_STR_AOP     0x06
#define SOUND_FRAME_CMD_ROUND_STAR      0x11
#define SOUND_FRAME_CMD_ROUND_STOP      0x16
#define SOUND_FRAME_CMD_GET_VOL         0x43
#define SOUND_FRAME_CMD_GET_NUM         0x49

#define SOUND_BUF_LEN		        20
#define SOUND_BUF_MIN_LEN		0x05

#define SOUND_VOICE_STOP                0


#define SOUND_FOLDER_RETUEN_BAT         0x01
#define SOUND_FOLDER_BRWORR_BAT         0x02
#define SOUND_FOLDER_PROMPT             0x03

#define SOUND_VOC_BATT_1         1
#define SOUND_VOC_BATT_2         2
#define SOUND_VOC_BATT_3         3
#define SOUND_VOC_BATT_4         4
#define SOUND_VOC_BATT_5         5
#define SOUND_VOC_BATT_6         6
#define SOUND_VOC_BATT_7         7
#define SOUND_VOC_BATT_8         8
#define SOUND_VOC_BATT_9         9
#define SOUND_VOC_BATT_10        10
#define SOUND_VOC_BATT_11        11
#define SOUND_VOC_BATT_12        12
#define SOUND_VOC_BATT_13        13
#define SOUND_VOC_BATT_14        14
#define SOUND_VOC_BATT_15        15




//
#define SOUND_VOC_NO_BAT_CHANGE         1              //无电池可换
#define SOUND_VOC_DI                    2
#define SOUND_VOC_RETURN_OK             3
#define SOUND_VOC_RETURN_FAIL           4
#define SOUND_VOC_BATTER_UNKNOW1        5              //未识别电池
#define SOUND_VOC_BATTER_UNKNOW2        6
#define SOUND_VOC_TEMPCUSTODY_OK        7              //暂存成功
#define SOUND_VOC_TEMPCUSTODY_FAIL      8
#define SOUND_VOC_BATTER_CODE_FAIL      9
#define SOUND_VOC_PACKAGE_FAIL          10              //套餐到期
#define SOUND_VOC_FIRE_WARN             11
#define SOUND_VOC_OPEN_DEVICE           12



#define SOUND_VOC_FRAME_NULL             0

#define SOUND_VOICE_RTU_BAT             1
#define SOUND_VOICE_BRW_BAT             2
#define SOUND_VOICE_NO_BAT              3
#define SOUND_VOICE_DI                  4
#define SOUND_VOICE_RTU_BAT_OK          5
#define SOUND_VOICE_BRW_BAT_OK          6
#define SOUND_VOICE_RTU_FAIL            7
#define SOUND_VOICE_BAT_IDI_FAIL        8
#define SOUND_VOICE_TEMP_MEMORY_OK      9
#define SOUND_VOICE_TEMP_MEMORY_FAIL    10
#define SOUND_VOICE_BAT_SN_FAIL         11
#define SOUND_VOICE_PAKEAGE_DIE         12
#define SOUND_VOICE_FIRE_WRAN           13
#define SOUND_VOICE_CTR_STRENGH         14
#define SOUND_VOICE_GET_STRENGH         15



#define SOUND_VOCIE_STR_UP              0x04
#define SOUND_VOCIE_STR_DOWN            0x05


#define SOUND_REPAT_THREE               3
#define SOUND_OP_NUM                    32
#define SOUND_RESULT_OK                 0
#define SOUND_RESULT_FAIL               1
#define SOUND_RESULT_TO                 2


#define SOUND_VOCIE_MAX                 0x1E
#define SOUND_VOCIE_NORMAL              0x0F
#define SOUND_OP_TIM                    800

#define SOUND_VOICE_NUM                 100
#define SOUND_REPAT_TIME                3
#define SOUND_REPAT_NULL                1

#define SOUND_CNT_TIME_1S               200
#define SOUND_CNT_TIME_500MS            100
#define SOUND_CNT_TIME_100MS            20

#define SOUNE_VOICE_IDLE                0
#define SOUNE_VOICE_LOADING             1

#define SOUND_VOICE_TEST_FLAG           1

#define Sound_IsRcvFrame(rcvFrame)               ((rcvFrame).state == SOUND_STAT_RCV || (rcvFrame).state == SOUND_STAT_OVR)
#define Sound_ResetFrame(rcvFrame)               do{(rcvFrame).state = SOUND_STAT_IDLE ; (rcvFrame).repeat =0;}while(0)

#define Sound_ClearRxBuffer()                    memset(&g_sSoundInfo.rxBuf, 0,  sizeof(SOUND_RX_BUF)) //这里一定要用memset
#define Sound_Voice_Appoint(t,fn,vn,c)                do{g_sSoundInfo.txBuf.tick = t;g_sSoundInfo.txBuf.data = 0x0000;g_sSoundInfo.txBuf.data |= (fn << 8);g_sSoundInfo.txBuf.data |= (vn << 0);g_sSoundInfo.txBuf.cmd = c;}while(0)

typedef struct soundVoice{
    u8 flag;
    u8 state;
    u32 tick;
}SOUND_VOICE;



typedef struct soundTxBuffer{
    u8 cmd;
    u16 data;
    u8 flag;
    u8 num;
    u8 index;
    u8 result;
    u8 len;
    u8 id[SOUND_OP_NUM];
    u8 op[SOUND_OP_NUM];
    u8 buffer[SOUND_BUF_LEN];
    u8 cd[SOUND_OP_NUM];
    u8 repeat[SOUND_OP_NUM];
    u32 to[SOUND_OP_NUM];
    u32 tick;
}SOUND_TX_BUF;


typedef struct soundRxBuffer{
    u8 cmd;
    u8 len;
    u8 buffer[SOUND_BUF_LEN];
    u16 data;
}SOUND_RX_BUF;


typedef struct soundInfo{
    u8 test;
    u8 state;
    u8 flag;
    u8 index;
    u32 idleTime;
    SOUND_TX_BUF txBuf;
    SOUND_RX_BUF rxBuf;
}SOUND_INFO;
extern SOUND_INFO g_sSoundInfo ;

u8 Sound_FormaFrame_ApoVoice(u8 cmd,u8 feedback, u8 folderName, u8 voiceName, u8 *pFrame);
u8 Sound_FormaFrame(u8 cmd,u8 feedback, u16 dat, u8 *pFrame);
u8 Sounde_Chk(u8 *pFrame);

void Sound_Init();
void Sound_ReceiveFrame(u8 byte, SOUND_INFO *Info);
void Sound_Sum(u8 *pFrame, u8 len);
void Sound_CtrTxCmd(SOUND_TX_BUF *pCntOp, u32 sysTick);
void Sound_ConnectStep(SOUND_TX_BUF *pCntOp);
void Sound_TransmitCmd(u8 feedback);

#endif
