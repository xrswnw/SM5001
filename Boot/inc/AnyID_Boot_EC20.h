#ifndef _ANYID_BOOT_EC20_H
#define _ANYID_BOOT_EC20_H

#include "AnyID_Boot_EC20_HL.h"
#include "AnyID_Uart_Receive.h"




extern UART_RCVFRAME g_sEC20RcvFrame;
#define EC20_ClearRxBuffer()                    memset(&g_sEC20RcvFrame, 0,  sizeof(UART_RCVFRAME)) //����һ��Ҫ��memset



#define TESTTOKEN       "version=2018-10-31&res=products%2F598243%2Fdevices%2F865396056278381&et=2599881240&method=md5&sign=KJKG%2Bc7bLEz25s%2Bid9OHtA%3D%3D"

#define HTTPTOKEN       "version=2022-05-01&res=userid%2F332186&et=2599881240&method=md5&sign=qTJY0zpI84yQihbBTZgiwA%3D%3D"
#define HTTPTOKEN_LEN   97 
#define EC20_PRDOCT_ID                          "598243"  


  
#define EC20_IP_LEN                             4
#define EC20_STR_AT_LEN                         64
#define EC20_STR_BUFFER_RSP_LEN                 512
#define EC20_STR_BUFFER_LEN                     1024
#define EC20_PAR_ADDR_MOD_IP                    0
#define EC20_PAR_ADDR_MOD_URL                   1
#define EC20_PAR_ADDR_URL_LEN                   64
#define EC20_RQUEST_ID_LEN                      36
#define EC20_RQUEST_ID_POS                      57
#define EC20_RQUEST_ID_JSON_POS                 69
#define EC20_RQUEST_BUFFER_LEN                  128



#define EC20_MQTT_TOPIC_HEART                   1
#define EC20_MQTT_TOPIC_REBAT                   2
#define EC20_MQTT_TOPIC_BWBAT                   3
#define EC20_MQTT_TOPIC_WARN                    4
#define EC20_MQTT_TOPIC_CMD                     5


#define EC20_SERVER_TXSTAT_IDLE       0
#define EC20_SERVER_TXSTAT_WAIT       1
#define EC20_SERVER_TXSTAT_OVER       2
#define EC20_SERVER_TXSTAT_OK         3
#define EC20_SERVER_TXSTAT_RX         4
         

#define EC20_RSP_JSON                           0x01
#define EC20_RSP_CMD                            0x02
typedef struct EC20Par{
    u8 ip[EC20_IP_LEN];
    u16 port;
    char url[EC20_PAR_ADDR_URL_LEN + 1];
    u8 mode;
}EC20_PARAMS;

#define EC20_CNT_CMD_PWROFF                     0
#define EC20_CNT_CMD_PWRON                      1

#define EC20_CNT_OP_PWROFF                      0
#define EC20_CNT_OP_PWRON                       1
#define EC20_CNT_OP_KEYON                       2
#define EC20_CNT_OP_KEYOFF                      3
#define EC20_CNT_OP_CPIN                        4
#define EC20_CNT_OP_CREG                        5
#define EC20_CNT_OP_CGREG                       6
#define EC20_CNT_OP_APN                         7
#define EC20_CNT_OP_DEACT                       8
#define EC20_CNT_OP_ACT                         9
#define EC20_CNT_OP_OPEN                        10
#define EC20_CNT_OP_COMM                        11  //�����󣬴�����Ҫ10s������Ӧ����
#define EC20_CNT_OP_ATE0                        12
#define EC20_CNT_OP_IMEI                        13
#define EC20_CNT_OP_CCLK                        14
#define EC20_CNT_OP_WAIT                        15
#define EC20_CNT_OP_CLOSE                       16
#define EC20_CNT_OP_DTUOFF                      17
#define EC20_CNT_OP_DTR1                        18
#define EC20_CNT_OP_NTP                         19   //ntpͬ��ϵͳʱ�䣬����������֮ǰ����
#define EC20_CNT_OP_CSQ                         20      
#define EC20_CNT_OP_QMTCFG                      21
#define EC20_CNT_OP_QMTOPEN                     22
#define EC20_CNT_OP_QMTCLOSE                    23
#define EC20_CNT_OP_QMTCONN                     24
#define EC20_CNT_OP_QMTSUB                      25
#define EC20_CNT_OP_QMTPUBEX                    26
#define EC20_CNT_OP_IMSI                        27
#define EC20_CNT_OP_QMTSUB_CMD                  28
#define EC20_CNT_OP_QMTSUB_JSON_ACCEPT          29
#define EC20_CNT_OP_QMTSUB_JSON_REJECT          30
#define EC20_CNT_OP_QMTCFG_VERSION              31       
#define EC20_CNT_OP_QMTCFG_TIME                 32
#define EC20_CNT_OP_QMTSUB_RESPONSE_CMD         33
#define EC20_CNT_OP_HTTP_QICSGP                 34
#define EC20_CNT_OP_HTTP_QIACT                  35
#define EC20_CNT_OP_QIDNSCFG                    36

#define EC20_CNT_OP_HTTP_QSSLCFG                37
#define EC20_CNT_OP_HTTP_QHTTPURL               38
#define EC20_CNT_OP_HTTP_QGET                   40
#define EC20_CNT_OP_HTTP_QPOST                  41
#define EC20_CNT_OP_HTTP_QIDEACT                42
#define EC20_CNT_OP_HTTP_QGET_IP                43
#define EC20_CNT_OP_HTTP_QHTTPCFG_ID            44
#define EC20_CNT_OP_HTTP_QHTTPCFG_REQUST        45
#define EC20_CNT_OP_HTTP_QHTTPCFG_RESPONSE      46
#define EC20_CNT_OP_HTTP_QHTTPCFG_SSL           47
#define EC20_CNT_OP_HTTP_QHTTPCFG_CONTENT_TYPE  48

#define EC20_CNT_OP_HTTP_LINK                   50
#define EC20_CNT_OP_CHECK_LINK                  51



#define EC20_CNT_OP_STAT_IDLE                   0x0001
#define EC20_CNT_OP_STAT_TX                     0x0002
#define EC20_CNT_OP_STAT_RX                     0x0004
#define EC20_CNT_OP_STAT_STEP                   0x0008
#define EC20_CNT_OP_STAT_WAIT                   0x0020
#define EC20_CNT_OP_STAT_DTU                    0x0040
#define EC20_CNT_OP_STAT_TEST                   0x0080
#define EC20_CNT_OP_STAT_TX_AT                  0x0100
#define EC20_CNT_OP_STAT_RX_AT                  0x0200




#define EC20_CNT_TIME_1S                        200
#define EC20_CNT_TIME_500MS                     100
#define EC20_CNT_TIME_100MS                     20

#define EC20_CNT_REPAT_NULL                     1
#define EC20_CNT_REPAT_THREE                    3
#define EC20_CNT_OP_NUM                         32
#define EC20_CNT_RESULT_OK                      0
#define EC20_CNT_RESULT_FAIL                    1
#define EC20_CNT_RESULT_TO                      2

#define EC20_RESPONES_NULL                      0x00
#define EC20_RESPONES_CMD_RESP                  0x02
#define EC20_RESPONES_CMD_GET                   0x01
#define EC20_RESPONES_CMD_RESP                  0x02
#define EC20_RESPONES_JSON_ACCEPT               0x04
#define EC20_RESPONES_JSON_REJECT               0x08


#define EC20_CNT_MODE_CONNECT                   1
#define EC20_CNT_MODE_DISCONCT                  0
#define EC20_IMEI_LEN                           15
#define EC20_IMSI_LEN                           15
#define EC20_RTC_LEN                            6

#define EC20_RTC_DATA_ID_POS                    6
#define EC20_RTC_DATA_INDEX_POS                 37
#define EC20_RTC_DATA_CMD_POS                   39
#define EC20_RTC_DATA_LEN_POS                   41
#define EC20_RTC_DATA_BUFFER_POS                45

#define EC20_CNT_INDEX_LEN                      2
#define EC20_TOKEN_LEN                          131//129//127
#define EC20_CNT_IMEI_SRT_LEN					8

#define EC20_HEART_OFFLINE_TIME                 0x08

#define EC20_SERVER_TX_TO_TIME                  100
#define EC20_SERVER_RSP_LEN                     1500

#define EC20_SOFTVERSON_LEN                   17
typedef struct EC20Connect{
    EC20_PARAMS params;
    u8 imsi[EC20_IMSI_LEN + 2];
    u8 op[EC20_CNT_OP_NUM];
    u32 to[EC20_CNT_OP_NUM];
    u8 repeat[EC20_CNT_OP_NUM];
    u8 num;
    u8 index;
    u16 state;
    u8 cmd;
    u32 tick;
    u8 result;
}EC20_CONNECT;
extern EC20_CONNECT g_sEC20Connect;

    
typedef struct EC20RcvButter{
    u8 state;
    u8 flag;
    u16 len;
    u8 buffer[EC20_SERVER_RSP_LEN];
    char ipStr[EC20_CNT_INDEX_LEN * 8];
}EC20_RCVBUFFER;
extern EC20_RCVBUFFER g_sEC20RcvBuffer;

#define EC20_CheckRtcOk(p)                      ((p)[0] >= 21)

extern u8 bOk;
extern u8 g_nSoftWare[EC20_SOFTVERSON_LEN];
extern char g_nReaderTkoen[EC20_TOKEN_LEN];
extern u8 g_nImei[EC20_IMEI_LEN ];
extern u8 g_nImsiStr[EC20_IMSI_LEN];
extern EC20_PARAMS g_sEC20Params;


BOOL EC20_ConnectCheckRsp(EC20_CONNECT *pCntOp, u8 *pRxBuf, u8 len);
BOOL EC20_ConnectCheckClose(u8 *pBuffer);
BOOL EC20CheckRsp(u8 *pRxBuf);
BOOL EC20_CheckRsp(EC20_CONNECT *pCntOp, u8 *pRxBuf);


void EC20_Init();
void EC20_ConnectInit(EC20_CONNECT *pCntOp, u8 cmd, EC20_PARAMS *pParams);
void EC20_ConnectTxCmd(EC20_CONNECT *pCntOp, u32 sysTick);
void EC20_ConnectStep(EC20_CONNECT *pCntOp);



#endif

