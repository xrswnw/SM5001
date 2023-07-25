#include "AnyID_Boot_Device.h"

DEVICE_SENVER_TXBUFFER g_sDeviceServerTxBuf = {0};
DEVICE_UPDATA_INFO g_sDeviceUpDataInfo = {0};

char g_nHttpAtBuf[EC20_STR_BUFFER_RSP_LEN] = {0};
char g_nHttpBuf[EC20_STR_BUFFER_RSP_LEN] = {0};

void Device_Delayms(u32 n)           
{
    n *= 0x6000;
    n++;
    while(n--);
}


void Device_CommunTxCmd(DEVICE_SENVER_TXBUFFER *pCntOp, u32 sysTick)
{
   
    u8 op = 0;
    char atBuf[128] = {0};//------？？？？
    char lenBuf[20] = {0};
    op = pCntOp->op[pCntOp->index];
	
	

    switch(op)
    {

        case DEVICE_HTTP_GET_REQUEST_CKECK:
          
            memset(g_nHttpAtBuf, 0, EC20_STR_BUFFER_RSP_LEN);

            strcat(g_nHttpAtBuf,"GET http://iot-api.heclouds.com/fuse-ota/");
            strcat(g_nHttpAtBuf,*(&EC20_PRDOCT_ID));
            strcat(g_nHttpAtBuf,"/");
            strcat(g_nHttpAtBuf, (const char *)(&g_nImsiStr));

            strcat(g_nHttpAtBuf,"/check?type=1&version=");
            strcat(g_nHttpAtBuf, (char *)&g_sFramBootParamenter.currentVerSion);

            strcat(g_nHttpAtBuf," HTTP/1.1\r\n");
            strcat(g_nHttpAtBuf,"Content-Type: application/json\r\n");
            strcat(g_nHttpAtBuf,"authorization:");
            strcat(g_nHttpAtBuf,*(&HTTPTOKEN));
            strcat(g_nHttpAtBuf,"\r\n");
            strcat(g_nHttpAtBuf,"host:iot-api.heclouds.com\r\n");
            
            EC20_WriteCmd(g_nHttpAtBuf);
            break;
        case DEVICE_HTTP_GET_REQUEST_DOWNLOAD:
          
            memset(g_nHttpAtBuf, 0, EC20_STR_BUFFER_RSP_LEN);

            if(g_sDeviceUpDataInfo.step <= g_sDeviceUpDataInfo.num)
            {
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
                strcat(g_nHttpAtBuf,Device_Analys_Data_Len(lenBuf, g_sDeviceUpDataInfo.step));
                strcat(g_nHttpAtBuf,"\r\n");
                strcat(g_nHttpAtBuf,"host:iot-api.heclouds.com\r\n");
            }
            //Gate_WriteCmd(g_nHttpAtBuf);
            EC20_WriteCmd(g_nHttpAtBuf);
            break;
    case DEVICE_HTTP_POST_INFO_VERSION:
          memset(g_nHttpAtBuf, 0, EC20_STR_BUFFER_RSP_LEN);
			
          
                strcat(g_nHttpAtBuf,"POST http://iot-api.heclouds.com/fuse-ota/");
                strcat(g_nHttpAtBuf,*(&EC20_PRDOCT_ID));
                strcat(g_nHttpAtBuf,"/");
                strcat(g_nHttpAtBuf, (const char *)(&g_nImsiStr));
                strcat(g_nHttpAtBuf,"/");
                strcat(g_nHttpAtBuf, g_sDeviceUpDataInfo.tid);
                strcat(g_nHttpAtBuf, "/status HTTP/1.1\r\n");
                strcat(g_nHttpAtBuf,"authorization: ");
                strcat(g_nHttpAtBuf,*(&HTTPTOKEN));
                strcat(g_nHttpAtBuf,"\r\n");
				strcat(g_nHttpAtBuf,"Content-Type: application/json\r\n");
				strcat(g_nHttpAtBuf,"data: {step: 99}\r\n");
                                    //升级状态上报,动态上报
                strcat(g_nHttpAtBuf,"host:iot-api.heclouds.com\r\n");
				
         		//Gate_WriteCmd(g_nHttpAtBuf);
				EC20_WriteCmd(g_nHttpAtBuf);
      
      break;
    }
    pCntOp->tick = sysTick;
}



BOOL Device_CommunCheckRsp(DEVICE_SENVER_TXBUFFER *pCntOp, u8 *pRxBuf)
{
    u8 op = 0;

    BOOL bOK = FALSE;
    u32 len = 0;
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
          
            if((strstr((char const *)pRxBuf, "HTTP/1.1 206") != NULL) || (strstr((char const *)pRxBuf, "HTTP/1.1 200") != NULL))
            {
			  	u16 crc1 = 0, crc2 = 0;
				
				
				len = Device_Search_Data(pRxBuf);
				memcpy(g_nFlashDownData, pRxBuf + len, FLASH_DOWNLOAD_LEN);
				if(g_sDeviceUpDataInfo.step == g_sDeviceUpDataInfo.num)
				{
				   crc1 = Uart_GetFrameCrc(g_nFlashDownData, DEVICE_FRAME_END_LEN);
				   crc2 = a_GetCrc(g_nFlashDownData + UART_FRAME_POS_LEN, DEVICE_FRAME_END_LEN - 4);

				}
				else
				{
					crc1 = Uart_GetFrameCrc(g_nFlashDownData, FLASH_DOWNLOAD_LEN);
					crc2 = a_GetCrc(g_nFlashDownData + UART_FRAME_POS_LEN, FLASH_DOWNLOAD_LEN - 4);
				}
				if(crc1 == crc2)
				{
					memcpy(g_nFlashUpData, g_nFlashDownData + 18, FLASH_UPDATA_LEN);
				}
				else
				{
					g_sDeviceUpDataInfo.flag = DEVICE_UPDATA_FLAG_FAIL;
				}
                bOK = TRUE;
            }

            break;
        case DEVICE_HTTP_GET_RONSPOND:
            if(strstr((char const *)pRxBuf, "CONNECT") != NULL)
            {
                
                bOK = TRUE;
            }
            break;
		case DEVICE_HTTP_POST_INFO_VERSION:
		//	Gate_WriteCmd(pRxBuf);
		if(strstr((char const *)pRxBuf, "CONNECT") != NULL)
            {
                
                bOK = TRUE;
            }
			break;
    }
	g_sDeviceUpDataInfo.linkTick = 0;
    
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


BOOL Device_Chk_VersionFrame(u8 *pBuffer, DEVICE_UPDATA_INFO *pDataInfo)
{
    BOOL bOk = FALSE;
    
    memset(pDataInfo, 0, sizeof(DEVICE_UPDATA_INFO));

    if(strstr((char const *)pBuffer, "\"code\":0") != NULL)
    {
        if(strstr((char const *)pBuffer, "SM5001_") != NULL)     //版本相同无需更改，但平台有筛选功能，暂不处理
        {
            memcpy(pDataInfo->name, pBuffer + DEVICE_UPDATA_BUFFER_NAME_LEN, DEVICE_SOFTVERSION_NAME_LEN);
            memcpy(pDataInfo->tid, pBuffer + DEVICE_UPDATA_BUFFER_TID_LEN, DEVICE_SOFTVERSION_TID_LEN);
            memcpy(pDataInfo->bufferSize, pBuffer + DEVICE_UPDATA_BUFFER_SIZE_LEN, DEVICE_SOFTVERSION_BUFFER_SIZE);
            memcpy(pDataInfo->md5, pBuffer + DEVICE_UPDATA_BUFFER_MD5_LEN, DEVICE_SOFTVERSION_MD5);
            Device_Analys_Data(pDataInfo);
            Device_Erase_Flash();  
            g_sFramBootParamenter.size = pDataInfo->num ;
            if(*(pDataInfo->name + 16) == 0x31 && *(pDataInfo->name + 17) == 0x30 && *(pDataInfo->name + 18) == 0x30 && *(pDataInfo->name + 19) == 0x30)
            {
                g_sFramBootParamenter.flag = DEVICE_TYPE_SM5001;                    //-----主控更新
                pDataInfo->type = DEVICE_TYPE_SM5001;
                pDataInfo->flag = DEVICE_UPDATA_FLAG_DOWN; 	
            }else if(*(pDataInfo->name + 16) == 0x30 && *(pDataInfo->name + 17) == 0x31 && *(pDataInfo->name + 18) == 0x30 && *(pDataInfo->name + 19) == 0x30)
            {
                g_sFramBootParamenter.flag = DEVICE_TYPE_SM5002;                  //-----仓控更新
                pDataInfo->type = DEVICE_TYPE_SM5002;
                pDataInfo->flag = DEVICE_UPDATA_FLAG_DOWN; 	
            }
            else if(*(pDataInfo->name + 16) == 0x30 && *(pDataInfo->name + 17) == 0x30 && *(pDataInfo->name + 18) == 0x31 && *(pDataInfo->name + 19) == 0x30)
            {
                g_sFramBootParamenter.flag = DEVICE_TYPE_SM5003;
                pDataInfo->type = DEVICE_TYPE_SM5003;
                pDataInfo->flag = DEVICE_UPDATA_FLAG_DOWN; 	
            }
            else
            {
                pDataInfo->flag = DEVICE_UPDATA_INFORM_OK;  //无需更新
            }

            bOk = TRUE;
        } 
    }
	else if(strstr((char const *)pBuffer, "\"code\":12012") != NULL)
	{
		g_sFramBootParamenter.appState = FRAM_BOOT_APP_NULL_REPLACE;
	}
		

    
    return bOk;
}


BOOL Device_WeiteData(DEVICE_UPDATA_INFO *pDataInfo)
{
    BOOL tF = FALSE;
    memset(g_nFlashUpTempData, 0, FLASH_UPDATA_LEN);
    if(Flash_WriteBuffer(FLASH_DATA_OPEN_ADDR + (pDataInfo->step) * FLASH_UPDATA_LEN , FLASH_UPDATA_LEN, g_nFlashUpData))
    {
        if(Flash_ReadBuffer(FLASH_DATA_OPEN_ADDR + (pDataInfo->step) * FLASH_UPDATA_LEN , FLASH_UPDATA_LEN, g_nFlashUpTempData))
        {
            if(!memcmp(g_nFlashUpData, g_nFlashUpTempData, FLASH_UPDATA_LEN))
            {
                tF = TRUE;
            }
        }
    }
    
    

    return tF;
}


void Device_Analys_Data(DEVICE_UPDATA_INFO *pDataInfo)
{
    static u32 len = 0;

    len = a_atoi((u8 *)(pDataInfo->bufferSize), DEVICE_SOFTVERSION_BUFFER_SIZE, STD_LIB_FMT_DEC);
    pDataInfo->num = (len / FLASH_DOWNLOAD_LEN) ; 
	//FLASH_DOWNLOAD_LEN pDataInfo->num = len / FLASH_UPDATA_LEN;
}

char *Device_Analys_Data_Len(char *lenBuffer, u8 step)
{
    char lenStar[6] = {0}, lenStop[6] = {0};
    u32 lenStarValue = 0, lenStopValue = 0;
    /*
    lenStarValue = step * FLASH_UPDATA_LEN;  
    lenStopValue = (step + 1) * FLASH_UPDATA_LEN - 1; 
    */
	if(g_sDeviceUpDataInfo.step == g_sDeviceUpDataInfo.num)
	{
		lenStarValue = step * FLASH_DOWNLOAD_LEN;  
		lenStopValue = step * FLASH_DOWNLOAD_LEN + DEVICE_FRAME_END_LEN -1; 
	}
	else
	{
		lenStarValue = step * FLASH_DOWNLOAD_LEN;  
		lenStopValue = (step + 1) * FLASH_DOWNLOAD_LEN - 1; 
	}
	a_itoa(lenStarValue, (u8 *)lenStar, 0, 6);
    a_itoa(lenStopValue, (u8 *)lenStop, 0, 6);

    strcat(lenBuffer, "Range: ");
    strcat(lenBuffer, lenStar);
    strcat(lenBuffer, "-");
    strcat(lenBuffer, lenStop);
    return lenBuffer;
}


u32 Device_Search_Data(u8 *pBuffer)
{
    u32  index = 0 , i = 0;
    
    for(index = 0; index < FLASH_UPDATA_LEN; index++)
    {
        if(*(pBuffer + index) == DEVICE_DATA_MASK_R)
        {
            if(*(pBuffer + index + 1) == DEVICE_DATA_MASK_N)
            {
                if(*(pBuffer + index + 1 + 1) == DEVICE_DATA_MASK_R)
                {
                    if(*(pBuffer + index + 1 + 1 + 1) == DEVICE_DATA_MASK_N)
                    {
                        i = index + 4;
                        break; 
                    }
                    else
                    {
                        i = 0;
                    }
                }
                else
                {
                    i = 0;
                }
            }     
        }
    
    }
    
    return i;
}


BOOL Device_Chk_Version()
{
    BOOL tf = FALSE;     //版本信息校验
	
	
	//if(memcmp(g_sFramBootParamenter.aimVerSion,g_sFramBootParamenter.currentVerSion, FRAM_VERSION_SIZE))    //版本信息校验，相同则不更新
	//{
		if(Flash_ReadBuffer(FLASH_DATA_OPEN_ADDR + 4 * FLASH_UPDATA_LEN, FLASH_UPDATA_LEN, g_nFlashUpData))
		{
			if(g_sFramBootParamenter.flag == DEVICE_TYPE_SM5001)
			{     
			   if(!memcmp(g_nFlashUpData, (u8 *)DEVICE_BOOT_VER_ADDR, 6))	//固件信息校驗
			   { 
					tf = TRUE;
			   }
			}
			else if(g_sFramBootParamenter.flag == DEVICE_TYPE_SM5002 || g_sFramBootParamenter.flag == DEVICE_TYPE_SM5003)
			{
				tf = TRUE;
			}
		}
	//}
    return tf;
}








BOOL Device_Erase_McuFlash(u32 addr)
{
    BOOL tf = FALSE;




    return tf;
}