#include "AnyID_SM5001_Reader.h"

const u8 READER_VERSION[READER_VERSION_SIZE]@0x08005000 = " SM5001 23050800 G230200";
#define READER_SOFTWARE_VERSION    0x23050800

READER_RSPFRAME g_sDeviceRspFrame = {0};
READER_DEVPARAMS g_sReaderDevParams = {0};
READER_SERVERTX g_sReaderServerTx = {0};
DEVICE_INFO g_sDeviceInfo = {0};

void Reader_Init()
{
    Reader_ReadDeviceParamenter();

    memcpy(g_nReaderTkoen, TESTTOKEN, W232_TOKEN_LEN);
    
}

void Reader_Delayms(u32 n)
{
    //72MHZ
    n *= 0x6000;
    n++;
    while(n--);
}


BOOL Reader_ReadDeviceParamenter(void)                                         //OK
{
    BOOL b = FALSE, bBackup = FALSE;
    BOOL bOk = FALSE;

    b = FRam_ReadBuffer(FRAM_SYS_INFO_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));
    if(b)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sReaderDevParams), (sizeof(READER_DEVPARAMS)) - 4);
        crc2 = g_sReaderDevParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }
    
    bBackup = FRam_ReadBuffer(FRAM_SYS_INFO_BACKUP_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));
    if(bBackup)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sReaderDevParams), (sizeof(READER_DEVPARAMS)) - 4);
        crc2 = g_sReaderDevParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }

    if(b == FALSE && bBackup == FALSE)
    {
        memset(&g_sReaderDevParams, 0, sizeof(g_sReaderDevParams));
        g_sReaderDevParams.heartTime = 0x0A;
        g_sReaderDevParams.boxTime = 0x05;
        g_sReaderDevParams.boxNum = READER_BOX_NUM;
        g_sReaderDevParams.rfu1 = 0x00;
        g_sReaderDevParams.rfu2 = 0x0000;
        g_sReaderDevParams.lowVoltageWran = 0x0050;
        g_sReaderDevParams.boxFanTemp = 0x001E;
        g_sReaderDevParams.fullElect = 0x005F;
        g_sReaderDevParams.lowElect = 0x000A;
        g_sReaderDevParams.highElect = 0x005A;
        g_sReaderDevParams.lowVoltageCharge = 0x0258;
        g_sReaderDevParams.lowCurrentCharge = 0x0258;
        g_sReaderDevParams.fastVoltageCharge = 0x02B2;
        g_sReaderDevParams.fastCurrentCharge = 0x03E8;
        g_sReaderDevParams.slowVoltageCharge = 0x02B2;
        g_sReaderDevParams.slowCurrentCharge = 0x02BC;

        Reader_WriteDeviceParamenter();
    }
     else if(b == TRUE && bBackup == FALSE)
    {
        FRam_ReadBuffer(FRAM_SYS_INFO_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));
        FRam_WriteBuffer(FRAM_SYS_INFO_BACKUP_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));
    }
    else if(b == FALSE && bBackup == TRUE)
    {
        FRam_ReadBuffer(FRAM_SYS_INFO_BACKUP_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));
        FRam_WriteBuffer(FRAM_SYS_INFO_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));
    }
    
    Fram_ReadBootParams();
    
    if((g_sFramBootParams.appState != FRAM_BOOT_APP_OK)|| (g_sFramBootParams.addr != g_sReaderDevParams.addr))
    {
        g_sFramBootParams.appState = FRAM_BOOT_APP_OK;
        g_sFramBootParams.addr = g_sReaderDevParams.addr;
        Fram_WriteBootParams();
        Fram_WriteBackupBootParams();
    }
    if(b == TRUE && bBackup == TRUE)
    {
        bOk =  TRUE;;
    }

    return bOk;
    
}


BOOL Reader_WriteDeviceParamenter(void)
{
    BOOL b = FALSE;

    g_sReaderDevParams.crc = 0;
    g_sReaderDevParams.crc = a_GetCrc((u8 *)(&g_sReaderDevParams), (sizeof(READER_DEVPARAMS)) - 4);

    b = FRam_WriteBuffer(FRAM_SYS_INFO_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));
    b = FRam_WriteBuffer(FRAM_SYS_INFO_BACKUP_ADDR, sizeof(READER_DEVPARAMS), (u8 *)(&g_sReaderDevParams));

    return b;
}



u16 Device_ResponseFrame(u8 *pParam, u8 len, READER_RSPFRAME *pOpResult)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG; 

    if(len > UART_FRAME_PARAM_MAX_LEN)
    {
        pOpResult->buffer[pos++] = (len >> 0) & 0xFF;
        pOpResult->buffer[pos++] = (len >> 8) & 0xFF;

        memcpy(pOpResult->buffer + pos, pParam, len);
        pos += len;
    }
    else
    {
        if(pOpResult->flag == READER_RSPFRAME_FLAG_OK)
        {
            memcpy(pOpResult->buffer + pos, pParam, len);
        }
        else
        {
            memset(pOpResult->buffer + pos, 0, len);
        }
        pos += len;
        pOpResult->buffer[pos++] = pOpResult->err;
        pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
        pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
        
    }

    crc = a_GetCrc(pOpResult->buffer, pos); //从LEN开始计算crc
    pOpResult->buffer[pos++] = crc & 0xFF;
    pOpResult->buffer[pos++] = (crc >> 8) & 0xFF;

    pOpResult->len = pos;

    return pos;
}


/*


u8 Device_ResponseFrame(u8 *pParam, u8 len, READER_RSPFRAME *pOpResult)
{
    u16 pos = 0;
    u16 crc = 0;

    pOpResult->buffer[pos++] = UART_FRAME_FLAG_HEAD1; // frame head first byte
    pOpResult->buffer[pos++] = UART_FRAME_FLAG_HEAD2; // frame haed second byte
    pOpResult->buffer[pos++] = 0x00;   // length
    pOpResult->buffer[pos++] = (g_sReaderDevParams.addr >> 0) & 0xFF;
    pOpResult->buffer[pos++] = (g_sReaderDevParams.addr >> 8) & 0xFF;
    pOpResult->buffer[pos++] = (pOpResult->destAddr >> 0) & 0xFF;
    pOpResult->buffer[pos++] = (pOpResult->destAddr >> 8) & 0xFF;
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pOpResult->buffer[pos++] = pOpResult->cmd;               // cmd

    if(len > UART_FRAME_PARAM_MAX_LEN)
    {
        pOpResult->buffer[pos++] = (len >> 0) & 0xFF;
        pOpResult->buffer[pos++] = (len >> 8) & 0xFF;

        memcpy(pOpResult->buffer + pos, pParam, len);
        pos += len;
    }
    else
    {
        if(pOpResult->flag == READER_RSPFRAME_FLAG_OK)
        {
            memcpy(pOpResult->buffer + pos, pParam, len);
        }
        else
        {
            memset(pOpResult->buffer + pos, 0, len);
        }
        pos += len;
        pOpResult->buffer[pos++] = pOpResult->flag;
        pOpResult->buffer[pos++] = pOpResult->err;
        pOpResult->buffer[UART_FRAME_POS_LEN] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节
    }

    crc = a_GetCrc(pOpResult->buffer + UART_FRAME_POS_LEN, pos - UART_FRAME_POS_LEN); //从LEN开始计算crc
    pOpResult->buffer[pos++] = crc & 0xFF;
    pOpResult->buffer[pos++] = (crc >> 8) & 0xFF;

    pOpResult->len = pos;

    return pos;
}

*/



u16 Reader_ProcessUartFrame(u8 *pFrame, u8 add, u16 len)
{ 
    u8 cmd = 0;
    BOOL bRfOperation = FALSE;
    u16 paramsLen = 0;
    u8 data = 0;
    u8 pos = 0;
    g_sDeviceRspFrame.len = 0;
    
    memset(&g_sDeviceRspFrame, 0, sizeof(READER_RSPFRAME));
    cmd = g_sW232RcvBuffer.cmd;
    if(cmd == UART_FRAME_RESPONSE_FLAG)
    {
        cmd = *(pFrame + UART_FRAME_POS_MQTT_CMD + 1);
    }

    g_sDeviceRspFrame.cmd = cmd;
    g_sDeviceRspFrame.flag = UART_FRAME_FLAG_OK;
    g_sDeviceRspFrame.err = UART_FRAME_RSP_NOERR;
   
    data = *(pFrame + 0);
    paramsLen = len - UART_FRAME_MIN_MQTT_LEN;

        switch(cmd)
        {
            case READER_CMD_RESET:
                if(paramsLen == 0)
                {
                    if(add == READER_SM5001_ID)
                    {
                        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                        g_sDeviceRspFrame.flag = READER_RESPONSE_FLAG_RESET;
                    }
                    else if(add == READER_SM5003_ID)
                    {
                    
                    }
                    else if(add >= 1 && add <= READER_BOX_NUM * 2)
                    {
                    
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    }
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
            case READER_CMD_GET_VERSION:
                if(paramsLen == 0)
                {
                    if(add == READER_SM5001_ID)
                    {
                         if(data == READER_CMD_NORMAL)
                        {
                            g_sDeviceRspFrame.len = Device_ResponseFrame((u8 *)READER_VERSION, 24, &g_sDeviceRspFrame);
                        }
                        else
                        {
                        
                        }   
                    }
                    else
                    {
                    
                    }
                }
                break;
            case READER_CMD_GET_CPUID:
                if(paramsLen == 0)
                {
                    g_sDeviceRspFrame.len = Device_ResponseFrame((u8 *)STM32_CPUID_ADDR, STM32_CPUID_LEN, &g_sDeviceRspFrame);
                }
                break;
            case  READER_CMD_GET_CFG:
                if(paramsLen == 0)
                {
                    if(add == READER_SM5001_ID)
                    {
                        u8 buffer[READER_SET_CFG_FRAME_LEN] = {0};
                        buffer[pos++] = g_sReaderDevParams.heartTime;
                        buffer[pos++] = g_sReaderDevParams.boxTime;
                        buffer[pos++] = g_sReaderDevParams.boxNum;
                        buffer[pos++] = g_sReaderDevParams.rfu1;
                        buffer[pos++] = g_sReaderDevParams.rfu2 >> 8;
                        buffer[pos++] = g_sReaderDevParams.rfu2 >> 0;
                        buffer[pos++] = g_sReaderDevParams.lowVoltageWran >> 8;
                        buffer[pos++] = g_sReaderDevParams.lowVoltageWran >> 0;
                        buffer[pos++] = g_sReaderDevParams.boxFanTemp >> 8;
                        buffer[pos++] = g_sReaderDevParams.boxFanTemp >> 0;
                        buffer[pos++] = g_sReaderDevParams.fullElect >> 8;
                        buffer[pos++] = g_sReaderDevParams.fullElect >> 0;
                        buffer[pos++] = g_sReaderDevParams.lowElect >> 8;
                        buffer[pos++] = g_sReaderDevParams.lowElect >> 0;
                        buffer[pos++] = g_sReaderDevParams.highElect >> 8;
                        buffer[pos++] = g_sReaderDevParams.highElect >> 0;
                        buffer[pos++] = g_sReaderDevParams.lowVoltageCharge >> 8;
                        buffer[pos++] = g_sReaderDevParams.lowVoltageCharge >> 0;
                        buffer[pos++] = g_sReaderDevParams.lowCurrentCharge >> 8;
                        buffer[pos++] = g_sReaderDevParams.lowCurrentCharge >> 0;
                        buffer[pos++] = g_sReaderDevParams.fastVoltageCharge >> 8;
                        buffer[pos++] = g_sReaderDevParams.fastVoltageCharge >> 0;
                        buffer[pos++] = g_sReaderDevParams.fastCurrentCharge >> 8;
                        buffer[pos++] = g_sReaderDevParams.fastCurrentCharge >> 0;
                        buffer[pos++] = g_sReaderDevParams.slowVoltageCharge >> 8;
                        buffer[pos++] = g_sReaderDevParams.slowVoltageCharge >> 0;
                        buffer[pos++] = g_sReaderDevParams.slowCurrentCharge >> 8;
                        buffer[pos++] = g_sReaderDevParams.slowCurrentCharge >> 0;
                        g_sDeviceRspFrame.len = Device_ResponseFrame(buffer, READER_SET_CFG_FRAME_LEN, &g_sDeviceRspFrame);
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                    }

                    //g_sDeviceRspFrame.len = Device_ResponseFrame((u8 *)&g_sReaderDevParams.heartTime, READER_SET_CFG_FRAME_LEN, &g_sDeviceRspFrame);
                }
                break;
            case READER_CMD_SET_CFG:
                if(paramsLen == READER_SET_CFG_FRAME_LEN + 4)
                {
                    
                    if(add == READER_SM5001_ID)
                    {
                        g_sReaderDevParams.heartTime = (*(pFrame + pos++) & 0xFF);
                        g_sReaderDevParams.boxTime = (*(pFrame + pos++) & 0xFF);
                        g_sReaderDevParams.boxNum = (*(pFrame + pos++) & 0xFF);
                        g_sReaderDevParams.rfu1 = (*(pFrame + pos++) & 0xFF);
                        g_sReaderDevParams.rfu2 = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.rfu2 = g_sReaderDevParams.rfu2 | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.lowVoltageWran = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.lowVoltageWran = g_sReaderDevParams.lowVoltageWran | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.boxFanTemp = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.boxFanTemp = g_sReaderDevParams.boxFanTemp | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.fullElect = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.fullElect = g_sReaderDevParams.fullElect | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.lowElect = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.lowElect = g_sReaderDevParams.lowElect | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.highElect = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.highElect = g_sReaderDevParams.highElect | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.lowVoltageCharge = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.lowVoltageCharge = g_sReaderDevParams.lowVoltageCharge | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.lowCurrentCharge = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.lowCurrentCharge = g_sReaderDevParams.lowCurrentCharge | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.fastVoltageCharge = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.fastVoltageCharge = g_sReaderDevParams.fastVoltageCharge | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.fastCurrentCharge = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.fastCurrentCharge = g_sReaderDevParams.fastCurrentCharge | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.slowVoltageCharge = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.slowVoltageCharge = g_sReaderDevParams.slowVoltageCharge | ((*(pFrame + pos++) & 0xFF) << 0);
                        g_sReaderDevParams.slowCurrentCharge = ((*(pFrame + pos++) & 0xFF) << 8);
                        g_sReaderDevParams.slowCurrentCharge = g_sReaderDevParams.slowCurrentCharge | ((*(pFrame + pos++) & 0xFF) << 0);

                        Reader_WriteDeviceParamenter();
                        
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    }
                    
                }
                
                g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                break;

            case READER_CMD_ACTCTL_CTL:
                if(paramsLen == READER_ACTCTL_CTL_FRAME_LEN)
                {
                    if(add == READER_SM5001_ID)
                    {
                        Modele_Ctl(data);
                        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    }  
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;

        }
    

     if(g_sDeviceRspFrame.len == 0 && bRfOperation == FALSE)
    {
        g_sDeviceRspFrame.flag = READER_RSPFRAME_FLAG_FAIL;
        g_sDeviceRspFrame.err = READER_OPTAG_RESPONSE_PARERR;
        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
    }
 
    return g_sDeviceRspFrame.len ;
}


void Reader_ServerProcessRxInfo(W232_RCVBUFFER *pRcvBuffer, READER_SERVERTX *pServerTx)               //处理服务器下发数据
{

    u16 crc1 = 0, crc2 = 0;
    
    if(pRcvBuffer->cmd == READER_CMD_GET_CFG || pRcvBuffer->cmd == READER_CMD_RESET) 
    {
        if(pRcvBuffer->len)
        {
          crc1 = 0xFF;
        }
    }
    else
    {
        if(pRcvBuffer->len)
        {
            crc1 = Uart_GetFrameCrc(pRcvBuffer->buffer, pRcvBuffer->len);
            crc2 = a_GetCrc(pRcvBuffer->buffer, pRcvBuffer->len - 2);
        }
    }

    if(crc1 == crc2)
    {
        u16 txLen = 0;
       
        txLen = Reader_ProcessUartFrame(pRcvBuffer->buffer, pRcvBuffer->addr, pRcvBuffer->len);
        if(txLen > 0)
        {
            W232_PostRsp(&g_sW232Connect, g_sDeviceRspFrame.buffer, g_sDeviceRspFrame.len);
            if(g_sDeviceRspFrame.cmd == READER_CMD_RESET && g_sDeviceRspFrame.flag == READER_RESPONSE_FLAG_RESET)
            {
                Reader_Delayms(5);
                Sys_SoftReset();
            }
        }
    }
    else
    {
        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_LEN;
        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
        W232_PostRsp(&g_sW232Connect, g_sDeviceRspFrame.buffer, g_sDeviceRspFrame.len);
    }

}

void Modele_Ctl(u8 cmd)
{


    if(cmd & READER_MODELE_LED)
    {
        IO_Led_Open();
    }
    else 
    {
        IO_Led_Close();
    }


    if(cmd & READER_MODELE_FAN)
    {
        IO_Fan_Open();
    }
    else 
    {
        IO_Fan_Close();
    }


    if(cmd & READER_MODELE_MOTOR)
    {
        IO_Door_Open();
    }
    else 
    {
        IO_Door_Close();
    }


    if(cmd & READER_MODELE_RELAY)
    {
        IO_Realy_Open();
    }
    else
    {
        IO_Realy_Close();
    }      
}



/*

void Reader_ServerProcessRxInfo(u8 *pFrame, u16 len, READER_SERVERTX *pServerTx)
{
    u16 pos = 0;
    BOOL bRxFrame = FALSE;
    while(pos < len)
    {
        bRxFrame = FALSE;
        if(pFrame[pos] == READER_SVR_RSPCMD_LOGIN)     //登录响应
        {
            if(pos + READER_SVR_LOGIN_RSP_LEN <= len)
            {
                if(pServerTx->authOk == FALSE)//登录响应
                {
                    u16 errCode = 0;
                    a_ArrayToU16(errCode, pFrame + pos + 1);
                    if(errCode == READER_SVR_RSP_OK)
                    {
                        pServerTx->authOk = TRUE;                   //登录成功，可以上传数据
                        pServerTx->state = READER_SERVER_TXSTAT_OK;
                    }
                }
                bRxFrame = TRUE;
                pos = pos + READER_SVR_LOGIN_RSP_LEN;
            }
        }
        else if(pFrame[pos] == READER_SVR_CMD_DOWN) //下行数据报文
        {
            READER_SVRMSGINFO svrMsg = {0};
            
            if(Reader_ServerGetMsgInfo(pFrame, pos, len, &svrMsg))  //从len
            {
               
            } 
        }
        else if(pFrame[pos] == (READER_SVR_CMD_UP | READER_SVR_CMD_RSP_FLAG))        //上行数据响应 平台下发到设备
        {
            u16 rspLen = 0, sn = 0, errCode = 0;

            a_ArrayToU16(rspLen, pFrame + pos + 1); 
            a_ArrayToU16(sn, pFrame + pos + 1 + 2); 
            a_ArrayToU16(errCode, pFrame + pos + 1 + 4); 
                
            if(pos + READER_SVR_MSG_RSP_LEN <= len && rspLen + READER_SVR_FRM_POS_PAR == READER_SVR_MSG_RSP_LEN)
            {
                bRxFrame = TRUE;
                if(sn == pServerTx->sn)
                {
                    if(errCode == READER_SVR_RSP_OK)
                    {
                        //成功
                        pServerTx->state = READER_SERVER_TXSTAT_OK;     //上报成功
                        Reader_ServerRmvTxRcd(&g_sReaderRcdBuf, sn);    //清空buf缓冲区对应的记录数据
                    }
                    else
                    {
                        //报文解析失败
                        pServerTx->state = READER_SERVER_TXSTAT_WAIT;
                    }
                }
                pos += READER_SVR_MSG_RSP_LEN;
            }
        }
        if(bRxFrame == FALSE)
        {
            pos++;
        }
    }
}
*/


void Reader_UpHeartData(u32 id, u8 *pBuffer, u16 lenth)
{
    char strBuff1[W232_STR_BUFFER_HEART_LEN] ={0};
    char strBuff[W232_STR_BUFFER_LEN] ={0};
    char strRspbuffer[W232_STR_BUFFER_HEART_LEN] ={0};  
    
    a_Hex2Str(strRspbuffer, pBuffer, lenth);

    sprintf(strBuff1,"{\"id\":%.8d,\"dp\":{\"index\":[{\"v\":\"FF\"}],\"cmd\":[{\"v\":\"20\"}],\"len\":[{\"v\":\"%d\"}],\"data\":[{\"v\":\"%s\"}]}}",id, lenth, strRspbuffer);
    sprintf(strBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/dp/post/json\",%d",W232_PRDOCT_ID, g_sW232Connect.imei, strlen(strBuff1));
    W232_WriteCmd(strBuff);
    W232_Delayms(100);
    W232_WriteBuffer((u8 *)strBuff1, strlen(strBuff1));

}

u16 Reader_HeartFormat(u8 *pBuffer, u8 *pWraeBuffer, u8 *pWaterBuffer)
{
    u16 pos = 0;
    u16 crc = 0;
    pBuffer[pos++] = g_sW232Connect.imeiStr[0];
    pBuffer[pos++] = g_sW232Connect.imeiStr[1];
    pBuffer[pos++] = g_sW232Connect.imeiStr[2];
    pBuffer[pos++] = g_sW232Connect.imeiStr[3];
    pBuffer[pos++] = g_sW232Connect.imeiStr[4];
    pBuffer[pos++] = g_sW232Connect.imeiStr[5];
    pBuffer[pos++] = g_sW232Connect.imeiStr[6];
    pBuffer[pos++] = (g_sW232Connect.imeiStr[7] & 0xF0);

    pBuffer[pos++] = (g_sReaderDevParams.heartTime >> 8) & 0xFF;
    pBuffer[pos++] = (g_sReaderDevParams.heartTime >> 0) & 0xFF;
//柜体信息    
    pBuffer[pos++] = (READER_SOFTWARE_VERSION >> 24) & 0xFF;
    pBuffer[pos++] = (READER_SOFTWARE_VERSION >> 16) & 0xFF;
    pBuffer[pos++] = (READER_SOFTWARE_VERSION >>  8) & 0xFF;
    pBuffer[pos++] = (READER_SOFTWARE_VERSION >>  0) & 0xFF;

    pBuffer[pos++] = (g_sElectInfo.electValue >> 24) & 0xFF;
    pBuffer[pos++] = (g_sElectInfo.electValue >> 16) & 0xFF;
    pBuffer[pos++] = (g_sElectInfo.electValue >>  8) & 0xFF;
    pBuffer[pos++] = (g_sElectInfo.electValue >>  0) & 0xFF;

    pBuffer[pos++] = (g_aAdBuffer[0] & 0xFFFF) >> 8;
    pBuffer[pos++] = (g_aAdBuffer[0] & 0xFFFF) >> 0;
    pBuffer[pos++] = (g_aAdBuffer[1] & 0xFFFF) >> 8;
    pBuffer[pos++] = (g_aAdBuffer[1] & 0xFFFF) >> 0;

    pBuffer[pos++] = g_sIoInfo.warnValue;
//  仓体信息
    
    
    
    
    
    //
    
    
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pBuffer, pos); //从LEN开始计算crc
    pBuffer[pos++] = crc & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}
