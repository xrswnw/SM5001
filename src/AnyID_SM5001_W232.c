#include "AnyID_SM5001_W232.h"

UART_RCVFRAME g_sW232RcvFrame = {0};
W232_CONNECT g_sW232Connect = {0};
W232_RCVBUFFER g_sW232RcvBuffer = {0};

u8 bOk = 0; 

char g_nReaderTkoen[W232_TOKEN_LEN] = {0};
u8 g_nImei[W232_IMEI_LEN + 2];
u8 g_nImsiStr[W232_IMSI_LEN + 2];


void W232_Init()
{
    W232_InitInterface(W232_BAUDRARE);
    W232_ConfigInt();
    W232_EnableInt(ENABLE, DISABLE);

}

void W232_ConnectInit(W232_CONNECT *pCntOp, u8 cmd, W232_PARAMS *pParams)
{
    u8 num = 0;

    memset(pCntOp, 0, sizeof(W232_CONNECT));
    if(cmd == W232_CNT_CMD_PWRON)
    {
        pCntOp->to[num] = W232_CNT_TIME_100MS* 10;       pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_PWRON;          //开启电�?0ms
        pCntOp->to[num] = W232_CNT_TIME_100MS * 10;      pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_KEYON;          //开�?=500ms
        pCntOp->to[num] = W232_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 25;   pCntOp->op[num++] = W232_CNT_OP_COMM;           //开�?0s，串口有响应
        pCntOp->to[num] = W232_CNT_TIME_500MS  * 2;      pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_ATE0;
        //pCntOp->to[num] = W232_CNT_TIME_500MS  * 2;      pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_GET_QSQ;
        
        pCntOp->to[num] = W232_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 10;    pCntOp->op[num++] = W232_CNT_OP_IMEI;
        pCntOp->to[num] = W232_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 10;    pCntOp->op[num++] = W232_CNT_OP_IMSI;
        pCntOp->to[num] = W232_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_DTR1;        
        pCntOp->to[num] = W232_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 40;   pCntOp->op[num++] = W232_CNT_OP_CPIN;           //查询手机�?=20s
        pCntOp->to[num] = W232_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 180;  pCntOp->op[num++] = W232_CNT_OP_CREG;           //查询是否注册网络>=90s
        pCntOp->to[num] = W232_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 180;  pCntOp->op[num++] = W232_CNT_OP_CGREG;          //查询是否注册网络>=60s
        pCntOp->to[num] = W232_CNT_TIME_1S * 4;          pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_DEACT;          //40s
        pCntOp->to[num] = W232_CNT_TIME_1S * 15;         pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_ACT;            //150s
        pCntOp->to[num] = W232_CNT_TIME_1S * 4;          pCntOp->repeat[num] = 4;    pCntOp->op[num++] = W232_CNT_OP_QMTCFG;          //配置MQTT参数
/*time*/ pCntOp->to[num] = W232_CNT_TIME_1S * 4;          pCntOp->repeat[num] = 4;    pCntOp->op[num++] = W232_CNT_OP_QMTCFG_TIME;
        pCntOp->to[num] = W232_CNT_TIME_1S * 4 ;         pCntOp->repeat[num] = 50;    pCntOp->op[num++] = W232_CNT_OP_QMTCFG_VERSION;
        pCntOp->to[num] = W232_CNT_TIME_1S * 4;          pCntOp->repeat[num] = 2;    pCntOp->op[num++] = W232_CNT_OP_QMTOPEN;             //连接网络
        pCntOp->to[num] = W232_CNT_TIME_1S * 4;          pCntOp->repeat[num] = 5;    pCntOp->op[num++] = W232_CNT_OP_QMTCONN;           //连接MQTT客户�?
        pCntOp->to[num] = W232_CNT_TIME_1S * 4 ;         pCntOp->repeat[num] = 50;    pCntOp->op[num++] = W232_CNT_OP_QMTSUB_CMD;              //订阅主�
        pCntOp->to[num] = W232_CNT_TIME_1S * 4 ;         pCntOp->repeat[num] = 50;    pCntOp->op[num++] = W232_CNT_OP_QMTSUB_JSON_ACCEPT;              //订阅主题
        pCntOp->to[num] = W232_CNT_TIME_1S * 4 ;         pCntOp->repeat[num] = 50;    pCntOp->op[num++] = W232_CNT_OP_QMTSUB_OTA;  
        pCntOp->to[num] = W232_CNT_TIME_1S * 4 ;         pCntOp->repeat[num] = 50;    pCntOp->op[num++] = W232_CNT_OP_QMTSUB_RESPONSE_CMD;  
       //pCntOp->to[num] = W232_CNT_TIME_1S * 4 ;         pCntOp->repeat[num] = 50;    pCntOp->op[num++] = W232_CNT_OP_QMTSUB_OTA;  
    
    }
    else
    {
        pCntOp->to[num] = W232_CNT_TIME_1S * 10;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = W232_CNT_OP_CLOSE;
        pCntOp->to[num] = W232_CNT_TIME_100MS * 9;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = W232_CNT_OP_KEYOFF;          //关机>=650ms
        pCntOp->to[num] = W232_CNT_TIME_1S * 2;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = W232_CNT_OP_WAIT; 
        pCntOp->to[num] = W232_CNT_TIME_100MS;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = W232_CNT_OP_PWROFF;
    }
    pCntOp->state = W232_CNT_OP_STAT_TX;
    pCntOp->cmd = cmd;
    pCntOp->num = num;

    memcpy(&pCntOp->params, pParams, sizeof(W232_PARAMS));
}

char g_aGprsServerInfo[W232_PAR_ADDR_URL_LEN + 32] = {0};


void W232_ConnectTxCmd(W232_CONNECT *pCntOp, u32 sysTick)
{
    u8 op = 0;
    char strBuff[W232_STR_BUFFER_LEN] = {0};
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case W232_CNT_OP_WAIT:   //不需要发送，等待即可
            break;
        case W232_CNT_OP_PWROFF:    
            W232_CtrlLow();
            W232_KeyLow();
            break;
        case W232_CNT_OP_PWRON:
            W232_CtrlLow();
            W232_KeyLow();
            break;
        case W232_CNT_OP_KEYON:
        case W232_CNT_OP_KEYOFF:
            W232_KeyHigh();
            break;
        case W232_CNT_OP_COMM:
            W232_WriteCmd("AT");
            break;
        case W232_CNT_OP_ATE0:
            W232_WriteCmd("ATE0");
            break;
        case W232_CNT_OP_GET_QSQ:
            W232_WriteCmd("AT+CSQ");
            break;
        case W232_CNT_OP_IMEI:
            W232_WriteCmd("AT+GSN");
            break;
        case W232_CNT_OP_IMSI:
            W232_WriteCmd("AT+CIMI");
            break;
        case W232_CNT_OP_CCLK:
            W232_WriteCmd("AT+CCLK?");
            break;
        case W232_CNT_OP_DTR1:
            W232_WriteCmd("AT&D1");
            break;
        case W232_CNT_OP_CPIN:
            W232_WriteCmd("AT+CPIN?");
            break;
        case W232_CNT_OP_CREG:
            W232_WriteCmd("AT+CREG?");
            break;
        case W232_CNT_OP_CGREG:
            W232_WriteCmd("AT+CGREG?");
            break;
        case W232_CNT_OP_APN:
            W232_WriteCmd("AT+QICSGP=1,1,\"UNINET\",\"\",\"\",1");
            break;
        case W232_CNT_OP_DEACT:
            W232_WriteCmd("AT+QIDEACT=1");
            break;
        case W232_CNT_OP_ACT:
            W232_WriteCmd("AT+QIACT=1");
            break;
        case W232_CNT_OP_NTP:
            W232_WriteCmd("AT+QNTP=1,\"202.112.10.36\",123");
            break;
        case W232_CNT_OP_CLOSE:
            //W232_CtrlHigh();    //先退出透传模式
            W232_Delayms(10);
            W232_WriteCmd("AT+QICLOSE=1");
            break;
        case W232_CNT_OP_OPEN:
          
            if(pCntOp->params.mode == W232_PAR_ADDR_MOD_IP)
            {
                sprintf(g_aGprsServerInfo, "AT+QIOPEN=1,0,\"TCP\",\"%d.%d.%d.%d\",%d,0,2", pCntOp->params.ip[0], pCntOp->params.ip[1], pCntOp->params.ip[2], pCntOp->params.ip[3], pCntOp->params.port);
            }
            else
            {
                sprintf(g_aGprsServerInfo, "AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,2", pCntOp->params.url, pCntOp->params.port);
            }
            W232_WriteCmd(g_aGprsServerInfo);
            break;
        case W232_CNT_OP_QMTCFG:
          /**/
            W232_WriteCmd("AT+QMTCFG=\"qmtping\",0,60");                        //心跳
            W232_WriteCmd("AT+QMTCFG=\"recv/mode\",0,0,1");                      //接收模式，以URC模式上报，包含数据长�?
            W232_WriteCmd("AT+QMTCFG=\"send/mode\",0,1");                      //发送模式，16进制
            W232_WriteCmd("AT+QMTCFG=\"onenet\",0,\"598243\",\"zexoEnvCWH86x8eWjfpW32wUlWbELWEbUO1REV3OYyg=\""); //配置移动平台 
            break;
        case  W232_CNT_OP_QMTCFG_TIME:
              W232_WriteCmd("AT+QMTCFG=\"keepalive\",0,295"); 
          break;
        case W232_CNT_OP_QMTCFG_VERSION:
            
            W232_WriteCmd("AT+QMTCFG=\"version\",0,4");  
            break;
            
        case W232_CNT_OP_QMTOPEN:

            W232_WriteCmd("AT+QMTOPEN=0,\"mqtts.heclouds.com\",1883"); 
            break;
        case W232_CNT_OP_QMTCONN:

           sprintf(strBuff,"AT+QMTCONN=0,\"%.15s\",\"%.6s\",\"%s\"",g_nImsiStr, W232_PRDOCT_ID, TESTTOKEN);
            W232_WriteCmd(strBuff);
            break;
        case W232_CNT_OP_QMTSUB_CMD:
          
            sprintf(strBuff,"AT+QMTSUB=0,1,\"$sys/%.6s/%.15s/cmd/request/+\",1",W232_PRDOCT_ID,g_nImsiStr);
            W232_WriteCmd(strBuff);
            break;
        case W232_CNT_OP_QMTSUB_JSON_ACCEPT:
          
            sprintf(strBuff,"AT+QMTSUB=0,1,\"$sys/%.6s/%.15s/dp/post/json/accepted\",1",W232_PRDOCT_ID,g_nImsiStr);
            W232_WriteCmd(strBuff);
            break;
        case W232_CNT_OP_QMTSUB_JSON_REJECT:
          
            sprintf(strBuff,"AT+QMTSUB=0,1,\"$sys/%.6s/%.15s/dp/post/json/rejected\",1",W232_PRDOCT_ID,g_nImsiStr);
            W232_WriteCmd(strBuff);
            break;
        case W232_CNT_OP_QMTSUB_RESPONSE_CMD:
          
            sprintf(strBuff,"AT+QMTSUB=0,1,\"$sys/%.6s/%.15s/cmd/response/+/accepted\",1",W232_PRDOCT_ID,g_nImsiStr);
            W232_WriteCmd(strBuff);
            break;
        case W232_CNT_OP_QMTSUB_OTA:
            sprintf(strBuff,"AT+QMTSUB=0,1,\"$sys/%.6s/%.15s/ota/inform\",1",W232_PRDOCT_ID,g_nImsiStr);
            W232_WriteCmd(strBuff);
            break;
        case W232_CNT_OP_QMTPUBEX:
          
            sprintf(strBuff,"{\"id\":111,\"dp\":{\"index\":[{\"v\":100,\"t\":1682580203}]}}");
            W232_WriteCmd("AT+QMTPUBEX=0,0,0,0,\"$sys/598243/865396056277961/dp/post/json\",57");
            break;
    }
    pCntOp->tick = sysTick;
}

BOOL W232_ConnectCheckRsp(W232_CONNECT *pCntOp, u8 *pRxBuf)
{
    u8 op = 0;
    BOOL bOK = FALSE;
    char strBuff[W232_STR_BUFFER_LEN] = {0};
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case W232_CNT_OP_WAIT:
        case W232_CNT_OP_PWROFF:
        case W232_CNT_OP_PWRON:
        case W232_CNT_OP_KEYON:
            break;
        case W232_CNT_OP_COMM:
        case W232_CNT_OP_ATE0:
        case W232_CNT_OP_DEACT:
        case W232_CNT_OP_ACT:
        case W232_CNT_OP_APN:
        case W232_CNT_OP_DTR1:
        case W232_CNT_OP_CLOSE:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case  W232_CNT_OP_GET_QSQ:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
                memcpy(pCntOp->sigStr, pRxBuf + 8, 2);
                memcpy(pCntOp->channelErrStr, pRxBuf + 11, 2);
                pCntOp->sigNum = a_atoi((u8 *)pCntOp->sigStr, 2, STD_LIB_FMT_DEC);
                pCntOp->channelErrNum = a_atoi((u8 *)pCntOp->channelErrStr, 2,STD_LIB_FMT_DEC);
            }
            break;
        case W232_CNT_OP_IMEI:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                memcpy(pCntOp->imei, pRxBuf + 2, W232_IMEI_LEN + 2);
                memcpy(g_nImsiStr, pRxBuf+ 2, W232_IMEI_LEN + 2);
                memcpy(g_sMqttKey.imsiStr, pRxBuf + 2, W232_IMEI_LEN + 2);
                a_Str2Hex((char *)pCntOp->imei, pCntOp->imeiStr);
                a_Str2Hex((char *)g_nImsiStr, g_nImei);
                bOK = TRUE;
            }
            break;
        case W232_CNT_OP_IMSI:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                memcpy(pCntOp->imsi, pRxBuf + 2, W232_IMEI_LEN + 2);
                bOK = TRUE;
            }
            break;
        case W232_CNT_OP_NTP:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                if(strstr((char const *)pRxBuf, "QNTP") != NULL)
                {
                    bOK = TRUE;
                }
            }
            break;
        case W232_CNT_OP_CCLK:
            if(strstr((char const *)pRxBuf, "OK") != NULL)  //这里还要获取时间
            {
                if(W232_ConnectGetCClk(pRxBuf, 28, pCntOp->rtc) == FALSE)
                {
                    memset(pCntOp->rtc, 0, W232_RTC_LEN);
                }
                bOK = TRUE;
            }
            break;
        case W232_CNT_OP_CPIN:
            if(strstr((char const *)pRxBuf, "READY") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case W232_CNT_OP_CREG:
            if(strstr((char const *)pRxBuf, "+CREG") != NULL)
            {
                char *p = strstr((char const *)pRxBuf, ",");
                if(p[1] == '1' || p[1] == '5')
                {
                    bOK = TRUE;
                }
            }
            break;
        case W232_CNT_OP_CGREG:
            if(strstr((char const *)pRxBuf, "+CGREG") != NULL)
            {
                char *p = strstr((char const *)pRxBuf, ",");
                if(p[1] == '1' || p[1] == '5')
                {
                    bOK = TRUE;
                }
            }
            break;
        case W232_CNT_OP_OPEN:
        case W232_CNT_OP_QMTCFG:
        case W232_CNT_OP_QMTCFG_TIME:
        case W232_CNT_OP_QMTCFG_VERSION:
        case W232_CNT_OP_QMTOPEN:
        case W232_CNT_OP_QMTCONN:
        case W232_CNT_OP_QMTSUB:
        case W232_CNT_OP_QMTSUB_CMD:
        case W232_CNT_OP_QMTSUB_JSON_ACCEPT:
        case W232_CNT_OP_QMTSUB_JSON_REJECT:
        case W232_CNT_OP_QMTSUB_RESPONSE_CMD:
        case W232_CNT_OP_QMTSUB_OTA:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case W232_CNT_OP_QMTPUBEX:
            if(strstr((char const *)pRxBuf, ">") != NULL)
            {
              
                sprintf(strBuff,"{\"id\":111,\"dp\":{\"index\":[{\"v\":100,\"t\":1682580203}]}}");
                W232_WriteBuffer((u8 *)strBuff, 40);
                W232_WriteBuffer((u8 *)strBuff, 40);
                bOK = TRUE;
                //数据发�?
            }
            break;
    }
	g_sW232Connect.comErr  = 0;
    return bOK;
}


void W232_ConnectStep(W232_CONNECT *pCntOp)
{
    u8 op = 0;

    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case W232_CNT_OP_KEYON:
        case W232_CNT_OP_KEYOFF:
            //W232_KeyLow();
            pCntOp->repeat[pCntOp->index] = 0;
            pCntOp->index++;
            pCntOp->result = W232_CNT_RESULT_OK;
            break;
        case W232_CNT_OP_WAIT:
        case W232_CNT_OP_PWROFF:
        case W232_CNT_OP_PWRON:
        case W232_CNT_OP_CLOSE:
            pCntOp->repeat[pCntOp->index] = 0;
            pCntOp->index++;
            pCntOp->result = W232_CNT_RESULT_OK;
            break;
        case W232_CNT_OP_COMM:
        case W232_CNT_OP_ATE0:
        case W232_CNT_OP_DEACT:
        case W232_CNT_OP_ACT:
        case W232_CNT_OP_APN:
        case W232_CNT_OP_GET_QSQ:
        case W232_CNT_OP_IMEI:
        case W232_CNT_OP_IMSI:
        case W232_CNT_OP_CCLK:
        case W232_CNT_OP_CPIN:
        case W232_CNT_OP_CREG:
        case W232_CNT_OP_CGREG:
        case W232_CNT_OP_OPEN:
        case W232_CNT_OP_DTR1:
        case W232_CNT_OP_NTP:
        case W232_CNT_OP_QMTCFG:
        case W232_CNT_OP_QMTCFG_VERSION:
        case W232_CNT_OP_QMTCFG_TIME:
        case W232_CNT_OP_QMTOPEN:
        case W232_CNT_OP_QMTCONN:
        case W232_CNT_OP_QMTSUB:
        case W232_CNT_OP_QMTSUB_CMD:
        case W232_CNT_OP_QMTSUB_JSON_ACCEPT:
        case W232_CNT_OP_QMTSUB_JSON_REJECT:
        case W232_CNT_OP_QMTSUB_RESPONSE_CMD:
        case W232_CNT_OP_QMTPUBEX:
        case W232_CNT_OP_QMTSUB_OTA:
            if(pCntOp->result == W232_CNT_RESULT_OK)
            {
                pCntOp->repeat[pCntOp->index] = 0;
                pCntOp->index++;
            }
            break;
    }
}

BOOL W232_ConnectCheckClose(u8 *pBuffer)
{
    if((strstr((char const *)pBuffer, "closed\r\n") != NULL) || (strstr((char const *)pBuffer, "pdpdeact\r\n") != NULL))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



BOOL W232_ConnectGetCClk(u8 *pRxBuf, u8 len, u8 *pRtc)
{
    int pos = 0;
    u8 rtcPos = 0;
    BOOL b = TRUE;
    u8 t = 0;

    // +CCLK: "21/07/23,08:09:17+00"
    //�?
    pos = a_IndexOf(pRxBuf, pos, len, '"');
    if(b && pos >= 0)
    {
        b = FALSE;
        t = a_atoi(pRxBuf + pos + 1, 2, STD_LIB_FMT_DEC);
        pos += 3;
        if(21 <= t && t <= 99)
        {
            pRtc[rtcPos++] = t;
            b = TRUE;
        }
    }
    //�?
    pos = a_IndexOf(pRxBuf, pos, len, '/');
    if(b && pos >= 0)
    {
        b = FALSE;
        t = a_atoi(pRxBuf + pos + 1, 2, STD_LIB_FMT_DEC);
        pos += 3;
        if(1 <= t && t <= 12)
        {
            pRtc[rtcPos++] = t;
            b = TRUE;
        }
    }
    //�?
    pos = a_IndexOf(pRxBuf, pos, len, '/');
    if(b && pos >= 0)
    {
        b = FALSE;
        t = a_atoi(pRxBuf + pos + 1, 2, STD_LIB_FMT_DEC);
        pos += 3;
        if(1 <= t && t <= 31)
        {
            pRtc[rtcPos++] = t;
            b = TRUE;
        }
    }
    //�?
    pos = a_IndexOf(pRxBuf, pos, len, ',');
    if(b && pos >= 0)
    {
        b = FALSE;
        t = a_atoi(pRxBuf + pos + 1, 2, STD_LIB_FMT_DEC);
        pos += 3;
        if(t <= 23)
        {
            pRtc[rtcPos++] = t;
            b = TRUE;
        }
    }
    //�?
    pos = a_IndexOf(pRxBuf, pos, len, ':');
    if(b && pos >= 0)
    {
        b = FALSE;
        t = a_atoi(pRxBuf + pos + 1, 2, STD_LIB_FMT_DEC);
        pos += 3;
        if(t <= 59)
        {
            pRtc[rtcPos++] = t;
            b = TRUE;
        }
    }
    //�?
    pos = a_IndexOf(pRxBuf, pos, len, ':');
    if(b && pos >= 0)
    {
        b = FALSE;
        t = a_atoi(pRxBuf + pos + 1, 2, STD_LIB_FMT_DEC);
        pos += 3;
        if(t <= 59)
        {
            pRtc[rtcPos++] = t;
            b = TRUE;
        }
    }
    
    if(b)   //校验是否是合法时�?
    {
        
    }

    return b;
}


void W232_PostRsp(W232_CONNECT *pCntOp,u8 *pBuffer, u16 len)
{  
  /*
    char strBuff[W232_STR_BUFFER_LEN] = {0};
    char strBuff1[W232_STR_BUFFER_LEN] = {0};
    char strRspbuffer[W232_STR_BUFFER_LEN] ={0};
    char strRsplen[4] = {0};
    u8 hexLen[2] = {0};
      
    hexLen[0] = (len & 0xFF >> 8);
    hexLen[1] = (len & 0xFF >> 0); 
    a_Hex2Str(strRspbuffer, pBuffer, len);

    a_Hex2Str(strRsplen, hexLen, 2);
    sprintf(strBuff1,"{\"id\":%.8s,\"dp\":{\"device\":[{\"v\":\"%.2s%.2s%.4s%s\"}]}}", g_sW232RcvBuffer.idStr, g_sW232RcvBuffer.addStr, g_sW232RcvBuffer.cmdStr, strRsplen, strRspbuffer);
    sprintf(strBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/cmd/response/%.36s\",%d",W232_PRDOCT_ID,g_nImsiStr, pCntOp->requestId, strlen(strBuff1));
   
    W232_WriteCmd(strBuff);

    char strTestRspbuffer[W232_STR_BUFFER_LEN] = {0};
    sprintf(strTestRspbuffer,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/865396056278381/cmd/response/%.36s\",%d",W232_PRDOCT_ID,  pCntOp->requestId,strlen(strBuff1));  
    if(memcmp(strBuff, strTestRspbuffer, strlen(strTestRspbuffer)))
    {
        Water_WriteStr("cmd rsp") ;
        Water_WriteBuffer((u8 *)strBuff, strlen(strBuff)) ;
        Water_WriteBuffer((u8 *)strTestRspbuffer, strlen(strTestRspbuffer)) ;
    }
    W232_Delayms(50);
    W232_WriteBuffer((u8 *)strBuff1,strlen(strBuff1));
*/
}



BOOL W232_DataHandle(W232_RCVBUFFER *pData, u8 *pBuffer)
{
    BOOL bOk = FALSE;
    if(pData->flag == W232_RESPONES_CMD_GET)
    {
        u8 tempLen[2] = {0};
        //Water_WriteStr("����2\r\n");
        memset(pData, 0, sizeof(W232_RCVBUFFER));
        memcpy(pData->idStr, pBuffer + W232_RTC_DATA_ID_POS, W232_CNT_INDEX_LEN * 4);
        memcpy(pData->addStr, pBuffer + W232_RTC_DATA_INDEX_POS, W232_CNT_INDEX_LEN);
        memcpy(pData->cmdStr, pBuffer + W232_RTC_DATA_CMD_POS, W232_CNT_INDEX_LEN);
        memcpy(pData->lenStr, pBuffer + W232_RTC_DATA_LEN_POS, W232_CNT_INDEX_LEN * 2);
        a_Str2Hex(pData->idStr, g_sW232RcvBuffer.id);
        a_Str2Hex(pData->addStr, &g_sW232RcvBuffer.addr);
        a_Str2Hex(pData->cmdStr, &g_sW232RcvBuffer.cmd);
        a_Str2Hex(pData->lenStr, tempLen);
        //Water_WriteStr("����3\r\n");
        if((tempLen[0] << 8)|(tempLen[1] << 0))
        {
            g_sW232RcvBuffer.len = ((tempLen[0] & 0xFF) << 8) | ((tempLen[1] & 0xFF) << 0);
            if(g_sW232RcvBuffer.len < W232_RQUEST_BUFFER_LEN)                            //����У�飬Խ��
            {
              bOk = TRUE;
              memcpy(pData->bufferStr, pBuffer + W232_RTC_DATA_BUFFER_POS, g_sW232RcvBuffer.len * 2);
              a_Str2Hex(pData->bufferStr,g_sW232RcvBuffer.buffer);
            }
            else
            {
              bOk = TRUE;
              memset(&g_sW232RcvBuffer.buffer, 0, W232_RQUEST_BUFFER_LEN);
            }
        }
        else
        {     bOk = TRUE;
              memset(&g_sW232RcvBuffer.buffer, 0, W232_RQUEST_BUFFER_LEN);
        }
        //Water_WriteStr("����4\r\n");
    }
    else if(pData->flag == W232_RESPONES_JSON_ACCEPT)
    {
        memset(&g_sW232RcvBuffer, 0, sizeof(W232_RCVBUFFER));
        memcpy(pData->idStr, pBuffer + W232_RTC_DATA_ID_POS, W232_CNT_INDEX_LEN * 4);
        a_Str2Hex(pData->idStr, g_sW232RcvBuffer.id);
    
    }
    return bOk;
  
}