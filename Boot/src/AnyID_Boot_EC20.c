#include "AnyID_Boot_EC20.h"

UART_RCVFRAME g_sEC20RcvFrame = {0};
EC20_CONNECT g_sEC20Connect = {0};
EC20_RCVBUFFER g_sEC20RcvBuffer = {0};
EC20_PARAMS g_sEC20Params = {0};

u8 bOk = 0; 

u8 g_nSoftWare[EC20_SOFTVERSON_LEN] = {0};
u8 g_nImei[EC20_IMEI_LEN];
u8 g_nImsiStr[EC20_IMSI_LEN];


void EC20_Init()
{
    EC20_InitInterface(EC20_BAUDRARE);
    EC20_ConfigInt();
    EC20_EnableInt(ENABLE, DISABLE);

}

void EC20_ConnectInit(EC20_CONNECT *pCntOp, u8 cmd, EC20_PARAMS *pParams)
{
    u8 num = 0;

    memset(pCntOp, 0, sizeof(EC20_CONNECT));
    if(cmd == EC20_CNT_CMD_PWRON)
    {
        pCntOp->to[num] = EC20_CNT_TIME_100MS* 10;       pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_PWRON;          //开启电�?0ms
        pCntOp->to[num] = EC20_CNT_TIME_100MS * 10;      pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_KEYON;          //开�?=500ms
        pCntOp->to[num] = EC20_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 25;   pCntOp->op[num++] = EC20_CNT_OP_COMM;           //开�?0s，串口有响应
        pCntOp->to[num] = EC20_CNT_TIME_500MS  * 2;      pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_ATE0;
        pCntOp->to[num] = EC20_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 10;    pCntOp->op[num++] = EC20_CNT_OP_IMEI;
        pCntOp->to[num] = EC20_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 10;    pCntOp->op[num++] = EC20_CNT_OP_IMSI;
        pCntOp->to[num] = EC20_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 10;    pCntOp->op[num++] = EC20_CNT_OP_DTR1;        
        pCntOp->to[num] = EC20_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 40;   pCntOp->op[num++] = EC20_CNT_OP_CPIN;           //查询手机�?=20s
        pCntOp->to[num] = EC20_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 180;  pCntOp->op[num++] = EC20_CNT_OP_CREG;           //查询是否注册网络>=90s
        pCntOp->to[num] = EC20_CNT_TIME_500MS * 2;       pCntOp->repeat[num] = 180;  pCntOp->op[num++] = EC20_CNT_OP_CGREG;          //查询是否注册网络>=60s
        pCntOp->to[num] = EC20_CNT_TIME_1S;              pCntOp->repeat[num] = 10;    pCntOp->op[num++] = EC20_CNT_OP_APN;            //PDP参数配置
        pCntOp->to[num] = EC20_CNT_TIME_1S * 4;          pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_DEACT;          //40s
        pCntOp->to[num] = EC20_CNT_TIME_1S * 15;         pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_ACT;            //150s 激活 PDP
        pCntOp->to[num] = EC20_CNT_TIME_1S * 15;         pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_HTTP_QGET_IP; 
        pCntOp->to[num] = EC20_CNT_TIME_1S * 15;         pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_QIDNSCFG; 
        
        
        //HTTP_CFG
        pCntOp->to[num] = EC20_CNT_TIME_1S ;         pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_HTTP_QHTTPCFG_ID;       //HTTP服务器参数
        pCntOp->to[num] = EC20_CNT_TIME_1S ;         pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_HTTP_QHTTPCFG_REQUST;
        pCntOp->to[num] = EC20_CNT_TIME_1S ;         pCntOp->repeat[num] = 2;    pCntOp->op[num++] = EC20_CNT_OP_HTTP_QHTTPCFG_RESPONSE;
    }
    else
    {
        pCntOp->to[num] = EC20_CNT_TIME_1S * 10;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = EC20_CNT_OP_CLOSE;
        pCntOp->to[num] = EC20_CNT_TIME_100MS * 9;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = EC20_CNT_OP_KEYOFF;          //关机>=650ms
        pCntOp->to[num] = EC20_CNT_TIME_1S * 2;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = EC20_CNT_OP_WAIT; 
        pCntOp->to[num] = EC20_CNT_TIME_100MS;  pCntOp->repeat[num] = 1;    pCntOp->op[num++] = EC20_CNT_OP_PWROFF;
    }
    pCntOp->state = EC20_CNT_OP_STAT_TX;
    pCntOp->cmd = cmd;
    pCntOp->num = num;

    memcpy(&pCntOp->params, pParams, sizeof(EC20_PARAMS));
}

char g_aGprsServerInfo[EC20_PAR_ADDR_URL_LEN + 32] = {0};


void EC20_ConnectTxCmd(EC20_CONNECT *pCntOp, u32 sysTick)
{
    u8 op = 0;
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case EC20_CNT_OP_WAIT:   //不需要发送，等待即可
            break;
        case EC20_CNT_OP_PWROFF:    
            EC20_CtrlLow();
            EC20_KeyLow();
            break;
        case EC20_CNT_OP_PWRON:
            EC20_CtrlLow();
            EC20_KeyLow();
            break;
        case EC20_CNT_OP_KEYON:
        case EC20_CNT_OP_KEYOFF:
            EC20_KeyHigh();
            break;
        case EC20_CNT_OP_COMM:
            EC20_WriteCmd("AT");
            break;
        case EC20_CNT_OP_ATE0:
            EC20_WriteCmd("ATE0");
            break;
        case EC20_CNT_OP_IMEI:
            EC20_WriteCmd("AT+GSN");
            break;
        case EC20_CNT_OP_IMSI:
            EC20_WriteCmd("AT+CIMI");
            break;
        case EC20_CNT_OP_CCLK:
            EC20_WriteCmd("AT+CCLK?");
            break;
        case EC20_CNT_OP_DTR1:
            EC20_WriteCmd("AT&D1");
            break;
        case EC20_CNT_OP_CPIN:
            EC20_WriteCmd("AT+CPIN?");
            break;
        case EC20_CNT_OP_CREG:
            EC20_WriteCmd("AT+CREG?");
            break;
        case EC20_CNT_OP_CGREG:
            EC20_WriteCmd("AT+CGREG?");
            break;
        case EC20_CNT_OP_APN:

            EC20_WriteCmd("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",1");
            break;
        case EC20_CNT_OP_DEACT:
          
            EC20_WriteCmd("AT+QIDEACT=1");
            break;
        case EC20_CNT_OP_ACT:
          
            EC20_WriteCmd("AT+QIACT=1");
            break;
        case EC20_CNT_OP_HTTP_QGET_IP:
          
            EC20_WriteCmd("AT+QIACT?");
            break;
        case EC20_CNT_OP_QIDNSCFG:
          
            EC20_WriteCmd("AT+QIDNSCFG");
            break;
        case EC20_CNT_OP_NTP:
          
            EC20_WriteCmd("AT+QNTP=1,\"202.112.10.36\",123");
            break;
        case EC20_CNT_OP_CLOSE:
          
            EC20_Delayms(10);
            EC20_WriteCmd("AT+QICLOSE=1");
            break;
        case EC20_CNT_OP_HTTP_QHTTPCFG_ID:

            EC20_WriteCmd("AT+QHTTPCFG=\"contextid\",1");
            break;
        case EC20_CNT_OP_HTTP_QHTTPCFG_REQUST:

            EC20_WriteCmd("AT+QHTTPCFG=\"requestheader\",0");
            break;
        case EC20_CNT_OP_HTTP_QHTTPCFG_RESPONSE:

            EC20_WriteCmd("AT+QHTTPCFG=\"responseheader\",1");
            break;
    }
    pCntOp->tick = sysTick;
}

BOOL EC20_ConnectCheckRsp(EC20_CONNECT *pCntOp, u8 *pRxBuf, u8 len)
{
    u8 op = 0;
    BOOL bOK = FALSE;
    
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case EC20_CNT_OP_WAIT:
        case EC20_CNT_OP_PWROFF:
        case EC20_CNT_OP_PWRON:
        case EC20_CNT_OP_KEYON:
            break;
        case EC20_CNT_OP_COMM:
        case EC20_CNT_OP_ATE0:
        case EC20_CNT_OP_DEACT:
        case EC20_CNT_OP_ACT:
        case EC20_CNT_OP_APN:
        case EC20_CNT_OP_DTR1:
        case EC20_CNT_OP_CLOSE:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case EC20_CNT_OP_IMEI:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                memcpy(g_nImsiStr, pRxBuf + 2, EC20_IMEI_LEN );
                a_Str2Hex((char *)g_nImsiStr, g_nImei);
                bOK = TRUE;
            }
            break;
        case EC20_CNT_OP_IMSI:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {

                memcpy(pCntOp->imsi, pRxBuf + 2, EC20_IMEI_LEN + 2);
                bOK = TRUE;
            }
            break;
        case EC20_CNT_OP_HTTP_QGET_IP:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                memcpy(&(g_sEC20RcvBuffer.ipStr), pRxBuf + 17, EC20_CNT_INDEX_LEN * 8);
                bOK = TRUE;
            }
            break;
            
        case EC20_CNT_OP_NTP:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                if(strstr((char const *)pRxBuf, "QNTP") != NULL)
                {
                    bOK = TRUE;
                }
            }
            break;
        case EC20_CNT_OP_CPIN:
            if(strstr((char const *)pRxBuf, "READY") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case EC20_CNT_OP_CREG:
            if(strstr((char const *)pRxBuf, "+CREG") != NULL)
            {
                char *p = strstr((char const *)pRxBuf, ",");
                if(p[1] == '1' || p[1] == '5')
                {
                    bOK = TRUE;
                }
            }
            break;
        case EC20_CNT_OP_CGREG:
            if(strstr((char const *)pRxBuf, "+CGREG") != NULL)
            {
                char *p = strstr((char const *)pRxBuf, ",");
                if(p[1] == '1' || p[1] == '5')
                {
                    bOK = TRUE;
                }
            }
            break;
        case EC20_CNT_OP_OPEN:
            if(strstr((char const *)pRxBuf, "CONNECT") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case EC20_CNT_OP_QIDNSCFG:
            if(strstr((char const *)pRxBuf, "CONNECT") != NULL)
            {
                bOK = TRUE;
            }
            break;
            
        case  EC20_CNT_OP_HTTP_QHTTPCFG_ID:
           if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case  EC20_CNT_OP_HTTP_QHTTPCFG_REQUST:
           if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case  EC20_CNT_OP_HTTP_QHTTPCFG_RESPONSE:
           if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
            }
            break;
        case  EC20_CNT_OP_HTTP_QSSLCFG:
           if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
            }
            break;    
    }
    return bOK;
}


void EC20_ConnectStep(EC20_CONNECT *pCntOp)
{
    u8 op = 0;

    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case EC20_CNT_OP_KEYON:
        case EC20_CNT_OP_KEYOFF:

            pCntOp->repeat[pCntOp->index] = 0;
            pCntOp->index++;
            pCntOp->result = EC20_CNT_RESULT_OK;
            break;
        case EC20_CNT_OP_WAIT:
        case EC20_CNT_OP_PWROFF:
        case EC20_CNT_OP_PWRON:
        case EC20_CNT_OP_CLOSE:
            pCntOp->repeat[pCntOp->index] = 0;
            pCntOp->index++;
            pCntOp->result = EC20_CNT_RESULT_OK;
            break;
        case EC20_CNT_OP_COMM:
        case EC20_CNT_OP_ATE0:
        case EC20_CNT_OP_DEACT:
        case EC20_CNT_OP_ACT:
        case EC20_CNT_OP_APN:
        case EC20_CNT_OP_IMEI:
        case EC20_CNT_OP_IMSI:
        case EC20_CNT_OP_CCLK:
        case EC20_CNT_OP_CPIN:
        case EC20_CNT_OP_CREG:
        case EC20_CNT_OP_CGREG:
        case EC20_CNT_OP_OPEN:
        case EC20_CNT_OP_DTR1:
        case EC20_CNT_OP_NTP:
        case EC20_CNT_OP_QIDNSCFG:
        case EC20_CNT_OP_HTTP_QGET_IP:
        case EC20_CNT_OP_HTTP_QHTTPCFG_ID:
        case EC20_CNT_OP_HTTP_QHTTPCFG_REQUST:
        case EC20_CNT_OP_HTTP_QHTTPCFG_RESPONSE:
        case EC20_CNT_OP_HTTP_QHTTPCFG_SSL:
        case EC20_CNT_OP_HTTP_QSSLCFG:
        case EC20_CNT_OP_HTTP_QHTTPURL:
            if(pCntOp->result == EC20_CNT_RESULT_OK)
            {
                pCntOp->repeat[pCntOp->index] = 0;
                pCntOp->index++;
            }
            break;
    }
}

BOOL EC20_ConnectCheckClose(u8 *pBuffer)
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


