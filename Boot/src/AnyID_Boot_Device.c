#include "AnyID_Boot_Device.h"

DEVICE_SENVER_TXBUFFER g_sDeviceServerTxBuf = {0};


char g_nHttpAtBuf[EC20_STR_BUFFER_RSP_LEN] = {0};
char g_nHttpBuf[EC20_STR_BUFFER_RSP_LEN] = {0};
u8 g_nlen = 0;
u8 g_nAtLen[3] = {0}; 
void Device_CommunTxCmd(DEVICE_SENVER_TXBUFFER *pCntOp, u32 sysTick)
{
    u8 op = 0;

    char atLenBuf[EC20_STR_AT_LEN] = {0};
    char atBuf[EC20_STR_BUFFER_RSP_LEN] = {0};
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case DEVICE_HTTP_URL_LINK:
            
            EC20_WriteCmd("AT+QHTTPURL=28,6000");
            break;
        case DEVICE_HTTP_GET_REQUEST:

            EC20_WriteCmd("AT+QHTTPGET=25");
            break;
        case DEVICE_HTTP_GET_REQUEST_CKECK:

            //sprintf(g_nHttpAtBuf, EC20_STR_BUFFER_RSP_LEN,"GET https://iot-api.heclouds.com/fuse-ota/%.6s/%.15s/check?type=1&version=%.8s HTTP/1.1\r\nAuthorization:%.97s\r\n", EC20_PRDOCT_ID, g_nImsiStr, g_nSoftWare, HTTPTOKEN);
            memset(g_nHttpAtBuf, 0, EC20_STR_BUFFER_RSP_LEN);
            memset(atBuf, 0, EC20_STR_BUFFER_RSP_LEN);
            
            strcat(atBuf,"GET https://iot-api.heclouds.com/fuse-ota");
            strcat(atBuf,"/");
            strcat(atBuf,*(&EC20_PRDOCT_ID));
            strcat(atBuf,"/");
            strcat(atBuf, (const char *)(&g_nImsiStr));
            strcat(atBuf,"/");
            strcat(atBuf,"check?type=1&version=");
            strcat(atBuf, (const char *)(&g_nSoftWare));
            memcpy(g_nHttpAtBuf, atBuf, strlen(atBuf) - 2);
            strcat(g_nHttpAtBuf," HTTP/1.1\r\nAuthorization:");
            strcat(g_nHttpAtBuf,*(&HTTPTOKEN));
            strcat(g_nHttpAtBuf,"\r\n");
            strcat(atLenBuf,"AT+QHTTPGET=");
            
            g_nlen = strlen(g_nHttpAtBuf);
            a_itoa(strlen(g_nHttpAtBuf), g_nAtLen,0,3);
            
            strcat(atLenBuf, (char *)g_nAtLen);
  
            Uart_WriteCmd(atLenBuf);
            EC20_WriteCmd(atLenBuf);
          
            //memset(g_nHttpAtBuf, 0, EC20_STR_BUFFER_RSP_LEN);
           // memset(atBuf, 0, EC20_STR_BUFFER_RSP_LEN);

            break;
        case DEVICE_HTTP_GET_RONSPOND:

            EC20_WriteCmd("AT+QHTTPREAD=80");
            break;
            
    }
    pCntOp->tick = sysTick;
}



BOOL Device_CommunCheckRsp(DEVICE_SENVER_TXBUFFER *pCntOp, u8 *pRxBuf)
{
    u8 op = 0;

    BOOL bOK = FALSE;
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case DEVICE_HTTP_URL_LINK:
            if(strstr((char const *)pRxBuf, "CONNECT") != NULL)
            {
                bOK = TRUE;
                EC20_WriteCmd("https://iot-api.heclouds.com");
            }
            break; 
          break;
        case DEVICE_HTTP_GET_REQUEST:
            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
                 // EC20_WriteCmd("+QHTTPGET: 0,200,21472");
                //EC20_WriteCmd("GET https://www.baidu.com");
            }
            break;
        case DEVICE_HTTP_GET_REQUEST_CKECK:

            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;
                Uart_WriteCmd(g_nHttpAtBuf);
                EC20_WriteCmd(g_nHttpAtBuf);

            }
            break;
        case DEVICE_HTTP_GET_REQUEST_DOWNLOAD:

            if(strstr((char const *)pRxBuf, "OK") != NULL)
            {
                bOK = TRUE;

            }
            break;
        case DEVICE_HTTP_GET_RONSPOND:
            if(strstr((char const *)pRxBuf, "CONNECT") != NULL)
            {
                
                bOK = TRUE;
            }
            break;
    }
    
    return bOK;
}


void Device_CommunStep(DEVICE_SENVER_TXBUFFER *pCntOp)
{
    u8 op = 0;
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
            case DEVICE_HTTP_URL_LINK:
            case DEVICE_HTTP_GET_REQUEST:
            case DEVICE_HTTP_GET_REQUEST_CKECK:
            case DEVICE_HTTP_GET_REQUEST_DOWNLOAD:
            case DEVICE_HTTP_GET_RONSPOND :
            if(pCntOp->result == EC20_CNT_RESULT_OK)
            {
                pCntOp->repeat[pCntOp->index] = 0;
                pCntOp->index++;
            }
            break;
    }
}





BOOL Device_CheckRsp(EC20_RCVBUFFER *pCntOp, u8 *pRxBuf, u16 len)
{
    BOOL bOK = FALSE;

    if(strstr((char const *)pRxBuf, "+QHTTPGET: 0,200") != NULL)
    {
        //bOK = TRUE;
        Device_At_Rsp(EC20_CNT_TIME_1S, EC20_CNT_REPAT_NULL, DEVICE_HTTP_GET_RONSPOND);
    } 
    else if(strstr((char const *)pRxBuf, "+QHTTPGET") != NULL)
    {
        bOK = TRUE;
       // Device_At_Rsp(EC20_CNT_TIME_1S, EC20_CNT_REPAT_NULL, DEVICE_HTTP_GET_RONSPOND);
    } 
    else if(strstr((char const *)pRxBuf, "+QHTTPREAD: 0") != NULL)
    {
        bOK = TRUE;
        memcpy(pCntOp->buffer, pRxBuf, len);
    } 
    return bOK;
}




void Device_ServerProcessRxInfo(EC20_RCVBUFFER *pRcvBuffer, u32 tick)               //处理服务器下发数据
{

    u16 crc1 = 0, crc2 = 0;
   
    if(1)//(pRcvBuffer->cmd == DEVICE_CMD_GET_CFG || pRcvBuffer->cmd == DEVICE_CMD_RESET )//|| pRcvBuffer->cmd == GATE_FRAME_CMD_GET_ININFO) 
    {
       // crc1 = Uart_GetFrameCrc(pRcvBuffer->buffer, pRcvBuffer->len);
        //crc2 = a_GetCrc(pRcvBuffer->buffer, pRcvBuffer->len - 2);

    }

    
    if(crc1 == crc2)
    {
        u16 txLen = 0;
       
        //txLen = Reader_ProcessUartFrame(pRcvBuffer->buffer, pRcvBuffer->addr, pRcvBuffer->len, tick);
        if(txLen > 0)
        {
         
        }
    }
    else
    {  


    }

}


