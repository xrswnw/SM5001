#ifndef _ANYID_SM5001_W232_
#define _ANYID_SM5001_W232_

#include "AnyID_SM5001_W232_HL.h"
#include "AnyID_Uart_Receive.h"
#include "AnyID_SM5001_FRam.h"
#include "AnyID_SM5001_Elect.h"


#include "AnyID_SM5001_Water.h"
extern UART_RCVFRAME g_sW232RcvFrame;
#define W232_ClearRxBuffer()                    memset(&g_sW232RcvFrame, 0,  sizeof(UART_RCVFRAME)) //这里一定要用memset



//#define TESTTOKEN       "version=2018-10-31&res=products%2F598243%2Fdevices%2F865396056278381&et=2599881240&method=md5&sign=KJKG%2Bc7bLEz25s%2Bid9OHtA%3D%3D"

#define TESTTOKEN       "version=2018-10-31&res=products%2F598243&et=2599881240&method=md5&sign=n34n1AfhT1zyxT3Fftetug%3D%3D"

#define W232_PRDOCT_ID                          "598243"    
#define W232_IP_LEN                             4
#define W232_STR_BUFFER_RSP_LEN                 512
#define W232_STR_BUFFER_LEN                     2048
#define W232_STR_BUFFER_HEART_LEN               2048
#define W232_PAR_ADDR_MOD_IP                    0
#define W232_PAR_ADDR_MOD_URL                   1
#define W232_PAR_ADDR_URL_LEN                   64
#define W232_RQUEST_ID_LEN                      36
#define W232_RQUEST_ID_POS                      57
#define W232_RQUEST_ID_JSON_POS                 69
#define W232_RQUEST_BUFFER_LEN                  128



#define W232_MQTT_TOPIC_HEART                   1
#define W232_MQTT_TOPIC_REBAT                   2
#define W232_MQTT_TOPIC_BWBAT                   3
#define W232_MQTT_TOPIC_WARN                    4
#define W232_MQTT_TOPIC_CMD                     5
         

#define W232_RSP_JSON                           0x01
#define W232_RSP_CMD                            0x02
typedef struct w232Par{
    u8 ip[W232_IP_LEN];
    u16 port;
    char url[W232_PAR_ADDR_URL_LEN + 1];
    u8 mode;
}W232_PARAMS;

#define W232_CNT_CMD_PWROFF                     0
#define W232_CNT_CMD_PWRON                      1

#define W232_CNT_OP_PWROFF                      0
#define W232_CNT_OP_PWRON                       1
#define W232_CNT_OP_KEYON                       2
#define W232_CNT_OP_KEYOFF                      3
#define W232_CNT_OP_CPIN                        4
#define W232_CNT_OP_CREG                        5
#define W232_CNT_OP_CGREG                       6
#define W232_CNT_OP_APN                         7
#define W232_CNT_OP_DEACT                       8
#define W232_CNT_OP_ACT                         9
#define W232_CNT_OP_OPEN                        10
#define W232_CNT_OP_COMM                        11  //开机后，串口需要10s才能响应命令
#define W232_CNT_OP_ATE0                        12
#define W232_CNT_OP_IMEI                        13
#define W232_CNT_OP_CCLK                        14
#define W232_CNT_OP_WAIT                        15
#define W232_CNT_OP_CLOSE                       16
#define W232_CNT_OP_DTUOFF                      17
#define W232_CNT_OP_DTR1                        18
#define W232_CNT_OP_NTP                         19   //ntp同步系统时间，必须在联网之前操作
#define W232_CNT_OP_CSQ                         20      
#define W232_CNT_OP_QMTCFG                      21
#define W232_CNT_OP_QMTOPEN                     22
#define W232_CNT_OP_QMTCLOSE                    23
#define W232_CNT_OP_QMTCONN                     24
#define W232_CNT_OP_QMTSUB                      25
#define W232_CNT_OP_QMTPUBEX                    26
#define W232_CNT_OP_IMSI                        27
#define W232_CNT_OP_QMTSUB_CMD                  28
#define W232_CNT_OP_QMTSUB_JSON_ACCEPT          29
#define W232_CNT_OP_QMTSUB_JSON_REJECT          30
#define W232_CNT_OP_QMTCFG_VERSION              31       
#define W232_CNT_OP_QMTCFG_TIME                 32
#define W232_CNT_OP_QMTSUB_RESPONSE_CMD         33
#define W232_CNT_OP_QMTSUB_OTA                  34


#define W232_CNT_OP_STAT_IDLE                   0x0001
#define W232_CNT_OP_STAT_TX                     0x0002
#define W232_CNT_OP_STAT_RX                     0x0004
#define W232_CNT_OP_STAT_STEP                   0x0008
#define W232_CNT_OP_STAT_WAIT                   0x0020
#define W232_CNT_OP_STAT_DTU                    0x0040
#define W232_CNT_OP_STAT_TEST                   0x0080
#define W232_CNT_OP_STAT_TX_AT                  0x0100
#define W232_CNT_OP_STAT_RX_AT                  0x0200

#define W232_CNT_TIME_1S                        200
#define W232_CNT_TIME_500MS                    100
#define W232_CNT_TIME_100MS                     20

#define W232_CNT_REPAT_NULL                     1
#define W232_CNT_REPAT_THREE                    3
#define W232_CNT_OP_NUM                         32
#define W232_CNT_RESULT_OK                      0
#define W232_CNT_RESULT_FAIL                    1
#define W232_CNT_RESULT_TO                      2

#define W232_RESPONES_NULL                      0x00
#define W232_RESPONES_CMD_RESP                  0x02
#define W232_RESPONES_CMD_GET                   0x01
#define W232_RESPONES_CMD_RESP                  0x02
#define W232_RESPONES_JSON_ACCEPT               0x04
#define W232_RESPONES_JSON_REJECT               0x08


#define W232_CNT_MODE_CONNECT                   1
#define W232_CNT_MODE_DISCONCT                  0
#define W232_IMEI_LEN                           15
#define W232_IMSI_LEN                           15
#define W232_RTC_LEN                            6

#define W232_RTC_DATA_ID_POS                    6
#define W232_RTC_DATA_INDEX_POS                 37
#define W232_RTC_DATA_CMD_POS                   39
#define W232_RTC_DATA_LEN_POS                   41
#define W232_RTC_DATA_BUFFER_POS                45

#define W232_CNT_INDEX_LEN                      2
#define W232_TOKEN_LEN                          99//131//129//129//127
#define W232_CNT_IMEI_SRT_LEN					8

#define W232_HEART_OFFLINE_TIME                 20

#define W232_SERVER_TX_TO_TIME                  100
typedef struct w232Connect{
    W232_PARAMS params;
    u8 imei[W232_IMEI_LEN + 2];
    u8 imeiStr[8];
    u8 imsi[W232_IMSI_LEN + 2];
    u8 rtc[W232_RTC_LEN];
    u8 op[W232_CNT_OP_NUM];
    u32 to[W232_CNT_OP_NUM];
    u8 repeat[W232_CNT_OP_NUM];
    u8 requestId[W232_RQUEST_ID_LEN];
    u8 requestBuffer[W232_RQUEST_BUFFER_LEN];
    u8 num;
    u8 index;
    u16 state;
    u8 cmd;
    BOOL postFlag;
    u32 tick;
    u8 result;
}W232_CONNECT;
extern W232_CONNECT g_sW232Connect;

    
typedef struct w232RcvButter{
    u8 heartTime;
    u8 repat;
    u8 state;
    u8 flag;
    u8 addr;
    u8 cmd;
    u16 len;
    u16 offLineTime;
    u8 id[W232_CNT_INDEX_LEN * 2];
    u8 buffer[W232_RQUEST_BUFFER_LEN];
    char idStr[W232_CNT_INDEX_LEN * 4];
    char addStr[W232_CNT_INDEX_LEN];
    char cmdStr[W232_CNT_INDEX_LEN];
    char lenStr[W232_CNT_INDEX_LEN * 2];
    char bufferStr[W232_RQUEST_BUFFER_LEN];
    u32 tick;
}W232_RCVBUFFER;
extern W232_RCVBUFFER g_sW232RcvBuffer;

#define W232_CheckRtcOk(p)                      ((p)[0] >= 21)

extern u8 bOk;
extern char g_nReaderTkoen[W232_TOKEN_LEN];
extern u8 g_nImei[W232_IMEI_LEN + 2];
extern u8 g_nImsiStr[W232_IMSI_LEN + 2];


BOOL W232_SendBuffer( u8 *pRxBuf);
BOOL W232_ConnectCheckRsp(W232_CONNECT *pCntOp, u8 *pRxBuf);
BOOL W232_ConnectCheckClose(u8 *pBuffer);
BOOL W232_ConnectGetCClk(u8 *pRxBuf, u8 len, u8 *pRtc);
BOOL W232CheckRsp(u8 *pRxBuf);
BOOL W232_CheckRsp(W232_CONNECT *pCntOp, u8 *pRxBuf);
BOOL W232_FormatRsp(W232_CONNECT *pCntOp);

void W232_Init();
void W232_ConnectInit(W232_CONNECT *pCntOp, u8 cmd, W232_PARAMS *pParams);
void W232_ConnectTxCmd(W232_CONNECT *pCntOp, u32 sysTick);
void W232_ConnectStep(W232_CONNECT *pCntOp);
void W232_PostRsp(W232_CONNECT *pCntOp,u8 *pBuffer, u16 len);
BOOL W232_DataHandle(W232_RCVBUFFER *pData, u8 *pBuffer);
void W232_PostRtBatRsp(W232_CONNECT *pCntOp,u8 *pBuffer, u8 addr, u32 id);
void W232_PostBwBatRsp(W232_CONNECT *pCntOp,u8 *pBuffer, u8 addr, u32 id);


#endif

