#include "AnyID_Boot_Device.h"

DEVICE_SENVER_TXBUFFER g_sDeviceServerTxBuf = {0};
DEVICE_UPDATA_INFO g_sDeviceUpDataInfo = {0};


char g_nHttpAtBuf[EC20_STR_BUFFER_RSP_LEN] = {0};
char g_nHttpBuf[EC20_STR_BUFFER_RSP_LEN] = {0};

u8 g_nlen = 0;
u8 g_nAtLen[3] = {0}; 
void Device_CommunTxCmd(DEVICE_SENVER_TXBUFFER *pCntOp, u32 sysTick)
{
    u8 op = 0;

    char atBuf[EC20_STR_BUFFER_RSP_LEN] = {0};
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {

        case DEVICE_HTTP_GET_REQUEST_CKECK:
          
            memset(g_nHttpAtBuf, 0, EC20_STR_BUFFER_RSP_LEN);
            memset(atBuf, 0, EC20_STR_BUFFER_RSP_LEN);
            
            strcat(atBuf,"GET http://iot-api.heclouds.com/fuse-ota/");
            strcat(atBuf,*(&EC20_PRDOCT_ID));
            strcat(atBuf,"/");
            strcat(atBuf, (const char *)(&g_nImsiStr));
            strcat(atBuf,"/");
            strcat(atBuf,"check?type=1&version=");
            strcat(atBuf, (const char *)(&g_nSoftWare));
            memcpy(g_nHttpAtBuf, atBuf, strlen(atBuf) - 2);
            strcat(g_nHttpAtBuf," HTTP/1.1\r\n");
            strcat(g_nHttpAtBuf,"Content-Type: application/json\r\n");
            strcat(g_nHttpAtBuf,"authorization:");
            strcat(g_nHttpAtBuf,*(&HTTPTOKEN));
            strcat(g_nHttpAtBuf,"\r\n");
            strcat(g_nHttpAtBuf,"host:iot-api.heclouds.com\r\n");
            Uart_WriteCmd(g_nHttpAtBuf);
            
            EC20_WriteCmd(g_nHttpAtBuf);
            break;
        case DEVICE_HTTP_GET_REQUEST_DOWNLOAD:
            memset(g_nHttpAtBuf, 0, EC20_STR_BUFFER_RSP_LEN);
            memset(atBuf, 0, EC20_STR_BUFFER_RSP_LEN);
            
            strcat(g_nHttpAtBuf,"GET http://iot-api.heclouds.com/fuse-ota/");
            strcat(g_nHttpAtBuf,*(&EC20_PRDOCT_ID));
            strcat(g_nHttpAtBuf,"/");
            strcat(g_nHttpAtBuf, (const char *)(&g_nImsiStr));
            strcat(g_nHttpAtBuf,"/");
            strcat(g_nHttpAtBuf, g_sDeviceUpDataInfo.tid);
            strcat(g_nHttpAtBuf, "/download HTTP/1.1\r\n");
            strcat(g_nHttpAtBuf,"Content-Type: application/json\r\n");
            strcat(g_nHttpAtBuf,"authorization: ");
            strcat(g_nHttpAtBuf,*(&HTTPTOKEN));
            strcat(g_nHttpAtBuf,"\r\n");
            strcat(g_nHttpAtBuf,"Range: -1000\r\n");
            strcat(g_nHttpAtBuf,"host:iot-api.heclouds.com\r\n");
            Uart_WriteCmd(g_nHttpAtBuf);
            EC20_WriteCmd(g_nHttpAtBuf);
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
        case DEVICE_HTTP_GET_REQUEST_CKECK:
            if(strstr((char const *)pRxBuf, "HTTP/1.1 200") != NULL)
            {
                if(Device_Chk_VersionFrame(pRxBuf, &g_sDeviceUpDataInfo))
                {
                    bOK = TRUE;
                }
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
        Device_At_Rsp(EC20_CNT_TIME_1S, EC20_CNT_REPAT_NULL, DEVICE_HTTP_GET_RONSPOND);
    }
    else if(strstr((char const *)pRxBuf, "+CME ERROR: 703") != NULL)
    {
        bOK = TRUE;
    }
    else if(strstr((char const *)pRxBuf, "+QHTTPGET: 702") != NULL)
    {
        bOK = TRUE;
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



BOOL Device_Chk_VersionFrame(u8 *pBuffer, DEVICE_UPDATA_INFO *pDataInfo)
{
    BOOL bOk = FALSE;
    
    memset(pDataInfo, 0, sizeof(DEVICE_UPDATA_INFO));
     if(strstr((char const *)pBuffer, "\"code\":0") != NULL)
    {
        if(strstr((char const *)pBuffer, "SM5001") != NULL)
        {
            
            
            memcpy(pDataInfo->name, pBuffer + DEVICE_UPDATA_BUFFER_NAME_LEN, DEVICE_SOFTVERSION_NAME_LEN);
            memcpy(pDataInfo->tid, pBuffer + DEVICE_UPDATA_BUFFER_TID_LEN, DEVICE_SOFTVERSION_TID_LEN);
            memcpy(pDataInfo->bufferSize, pBuffer + DEVICE_UPDATA_BUFFER_SIZE_LEN, DEVICE_SOFTVERSION_BUFFER_SIZE);
            memcpy(pDataInfo->md5, pBuffer + DEVICE_UPDATA_BUFFER_MD5_LEN, DEVICE_SOFTVERSION_MD5);
            
            pDataInfo->flag = DEVICE_UPDATA_FLAG_DOWN; 
            bOk = TRUE;
        } 
        else if(strstr((char const *)pBuffer, "SM5002") != NULL)
        {

            
            memcpy(pDataInfo->name, pBuffer + DEVICE_UPDATA_BUFFER_NAME_LEN, DEVICE_SOFTVERSION_NAME_LEN);
            memcpy(pDataInfo->tid, pBuffer + DEVICE_UPDATA_BUFFER_TID_LEN, DEVICE_SOFTVERSION_TID_LEN);
            memcpy(pDataInfo->bufferSize, pBuffer + DEVICE_UPDATA_BUFFER_SIZE_LEN, DEVICE_SOFTVERSION_BUFFER_SIZE);
            memcpy(pDataInfo->md5, pBuffer + DEVICE_UPDATA_BUFFER_MD5_LEN, DEVICE_SOFTVERSION_MD5);
            
            pDataInfo->flag = DEVICE_UPDATA_FLAG_DOWN; 
            bOk = TRUE;
        }
        
    }   
    
    return bOk;
}


BOOL Device_GetData(u8 *pBuffer)
{
    BOOL tF = FALSE;
    
    
    

    return tF;
}

BOOL Device_ChkData(u8 *pBuffer)
{
    BOOL tF = FALSE;
    
    
    

    return tF;
}