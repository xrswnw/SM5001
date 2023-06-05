#include "AnyID_SM5001_Device.h"

const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE]@0x08005000 = " SM5001 23060500 G230200";

#define READER_HARDWARE_VERSION    "SM500100"
#define READER_SOFTWARE_VERSION    "23060500"

READER_RSPFRAME g_sDeviceRspFrame = {0};
DEVICE_PARAMS g_sDeviceParams = {0};
DEVICE_TEST g_nDeviceTestInfo  = {0};
DEVICE_SENVER_TXBUFFER g_nDeviceServerTxBuf = {0};
DEVICE_IMPRSP_INFO g_nDeviceImpRspInfo = {0};

void Device_Init()
{
    Device_ReadDeviceParamenter();
   /*
    memcpy(g_sMqttKey.keyBuffer, TESTTOKEN, W232_TOKEN_LEN);
    g_sMqttKey.len = W232_TOKEN_LEN;
    memset(g_sMqttKey.keyBuffer + g_sMqttKey.len, 0, FRAME_MQTT_KEY_KEY - g_sMqttKey.len);
   
    g_sMqttKey.uid[0] = 0xC9;
    g_sMqttKey.uid[1] = 0xBD;
    g_sMqttKey.uid[2] = 0xCF;
    g_sMqttKey.uid[3] = 0x5D;
    g_sMqttKey.uid[4] = 0x50;
    g_sMqttKey.uid[5] = 0x01;
    g_sMqttKey.uid[6] = 0x04;
    g_sMqttKey.uid[7] = 0xE0;
    
    Device_WriteMqttKey();      
         */                
    Device_ReadMqttKey();

    
}



void Reader_Delayms(u32 n)
{
    //72MHZ
    n *= 0x6000;
    n++;
    while(n--);
}


void Device_ReadDeviceParamenter(void)                                         //OK
{
     BOOL b = FALSE, bBackup = FALSE;
    b = FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    if(b)
    {
        u32 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sDeviceParams), (sizeof(DEVICE_PARAMS)) - DEVICE_CRC32_LEN);
        crc2 = g_sDeviceParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }  
    bBackup = FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    if(bBackup)
    {
        u32 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sDeviceParams), (sizeof(DEVICE_PARAMS)) - DEVICE_CRC32_LEN);
        crc2 = g_sDeviceParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作                       .
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }
    if(b == FALSE && bBackup == FALSE)
    {
        memset(&g_sDeviceParams, 0, sizeof(g_sDeviceParams));
        
        //g_sDeviceParams.mode =  DEVICE_MODE_SFG;
        g_sDeviceParams.gateTick = GATE_OP_DLY_TIM;
        g_sDeviceParams.gateTxTick =  GATE_OP_TX_TIM;             
        g_sDeviceParams.gateNum = GATE_SLAVER_NUM;
        g_sDeviceParams.gateParams.ledLowVolLev = DEVICE_LED_LOWVOL_DFT;
        g_sDeviceParams.gateParams.alarmTmpr = DEVICE_TMPR_ALARM_DFT;
        g_sDeviceParams.gateParams.chagParams.fulVolLev = CHAG_VOL_LEV_FUL;
        g_sDeviceParams.gateParams.chagParams.lowVolLev = CHAG_VOL_LEV_LOW;
        g_sDeviceParams.gateParams.chagParams.higVolLev = CHAG_VOL_LEV_HIG;
        g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].vol = CHAG_VOL_STEP1;
        g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].cur = CHAG_CUR_STEP1;
        g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].vol = CHAG_VOL_STEP2;
        g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].cur = CHAG_CUR_STEP2;
        g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].vol = CHAG_VOL_STEP3;
        g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].cur = CHAG_CUR_STEP3;

        Device_WriteDeviceParamenter();
    }
    else if(b == TRUE && bBackup == FALSE)
    {
        FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
        FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    }
    else if(b == FALSE && bBackup == TRUE)
    {
        FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
        FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    }
    
    Fram_ReadBootParamenter();
    if((g_sFramBootParamenter.appState != FRAM_BOOT_APP_OK))
    {
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
        Fram_WriteBootParamenter();
    }  

}




void Device_ReadMqttKey()                                         //OK
{
     BOOL b = FALSE, bBackup = FALSE;
    b = FRam_ReadBuffer(FRAME_MQTT_KEY_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
    if(b)
    {
        u32 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sMqttKey), (sizeof(MQTT_FRAM_KEY)) - DEVICE_CRC32_LEN);
        crc2 = g_sMqttKey.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }  
    bBackup = FRam_ReadBuffer(FRAME_MQTT_KEY_BACKUP_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
    if(bBackup)
    {
        u32 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sMqttKey), (sizeof(MQTT_FRAM_KEY)) - DEVICE_CRC32_LEN);
        crc2 = g_sMqttKey.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }
    if(b == FALSE && bBackup == FALSE)
    {
        //memset(&g_sMqttKey, 0, sizeof(g_sMqttKey));
        memcpy(g_sMqttKey.keyBuffer, TESTTOKEN, W232_TOKEN_LEN);
        g_sMqttKey.len = W232_TOKEN_LEN;
        
        //密匙錯誤，丟失，指示
        Device_WriteMqttKey();
    }
    else if(b == TRUE && bBackup == FALSE)
    {
        FRam_ReadBuffer(FRAME_MQTT_KEY_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
        FRam_WriteBuffer(FRAME_MQTT_KEY_BACKUP_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
    }
    else if(b == FALSE && bBackup == TRUE)
    {
        FRam_ReadBuffer(FRAME_MQTT_KEY_BACKUP_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
        FRam_WriteBuffer(FRAME_MQTT_KEY_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
    }
    
    
    
}


BOOL Device_WriteDeviceParamenter(void)
{
    BOOL b = FALSE;

    g_sDeviceParams.crc = 0;
    g_sDeviceParams.crc = a_GetCrc((u8 *)(&g_sDeviceParams), (sizeof(DEVICE_PARAMS)) - DEVICE_CRC32_LEN);

    b = FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    b = FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    
    return b;
}


BOOL Device_WriteMqttKey()
{
    BOOL b = FALSE;

    g_sMqttKey.crc = 0;
    g_sMqttKey.crc = a_GetCrc((u8 *)(&g_sMqttKey), (sizeof(MQTT_FRAM_KEY)) - DEVICE_CRC32_LEN);

    b = FRam_WriteBuffer(FRAME_MQTT_KEY_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
    b = FRam_WriteBuffer(FRAME_MQTT_KEY_BACKUP_ADDR, sizeof(MQTT_FRAM_KEY), (u8 *)(&g_sMqttKey));
    
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
        if(pOpResult->flag == DEVICE_RSPFRAME_FLAG_OK)
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

u16 Device_ResponseRtBat(u8 *pBuffer)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG; 
    pBuffer[pos++] = g_sGateOpInfo.rtnBat.step;
    pBuffer[pos++] = g_sGateOpInfo.rtnBat.flag;
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pBuffer, pos); //从LEN开始计算crc
    pBuffer[pos++] = crc & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

u16 Device_ResponseBrBat(u8 *pBuffer)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG; 
    pBuffer[pos++] = g_sGateOpInfo.brwBat.step;
    pBuffer[pos++] = g_sGateOpInfo.brwBat.flag;
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pBuffer, pos); //从LEN开始计算crc
    pBuffer[pos++] = crc & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

u16 Device_ResponseInfoChg(u8 *pBuffer, u8 add, u8 gateState, u8 batState, u8 chagState, u32 mainState)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    if(add == DEVICE_SM5001_ID)
    {
        pBuffer[pos++] = (mainState >> 24 ) & 0xFF; 
        pBuffer[pos++] = (mainState >> 16 ) & 0xFF;
        pBuffer[pos++] = (mainState >> 8 ) & 0xFF;
        pBuffer[pos++] = (mainState >> 0 ) & 0xFF;
    
    }
    else
    {
        pBuffer[pos++] = UART_FRAME_PARAM_RFU; 
        pBuffer[pos++] = gateState;
        pBuffer[pos++] = batState;
        pBuffer[pos++] = chagState;
    }
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pBuffer, pos); //从LEN开始计算crc
    pBuffer[pos++] = crc & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;
    
    return pos;
}

u16 Device_ResponseBat(READER_RSPFRAME *pOpResult, u8 mode)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG; 
    if(mode == GATE_FRAME_CMD_BRWBAT)
    {
        pOpResult->buffer[pos++] = g_sGateOpInfo.brwBat.step;
        pOpResult->buffer[pos++] = g_sGateOpInfo.brwBat.flag;
    }
    else if(mode == GATE_FRAME_CMD_RTNBAT)
    {
        pOpResult->buffer[pos++] = g_sGateOpInfo.rtnBat.step;
        pOpResult->buffer[pos++] = g_sGateOpInfo.rtnBat.flag;
    }
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pOpResult->buffer, pos); //从LEN开始计算crc
    pOpResult->buffer[pos++] = crc & 0xFF;
    pOpResult->buffer[pos++] = (crc >> 8) & 0xFF;

    pOpResult->len = pos;

    return pos;
}

u16 Device_GateResponse(READER_RSPFRAME *pOpResult, u8 *pParam, u8 len)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG;   
    memcpy(pOpResult->buffer + pos, pParam, len);
    pos += len;
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pOpResult->buffer, pos); //从LEN开始计算crc
    pOpResult->buffer[pos++] = crc & 0xFF;
    pOpResult->buffer[pos++] = (crc >> 8) & 0xFF;

    pOpResult->len = pos;

    return pos;
}


u16 Device_ResponseCfg( READER_RSPFRAME *pOpResult)
{
    u16 pos = 0;
    u16 crc = 0;
    
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG; 
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG; 
    pOpResult->buffer[pos++] = g_sDeviceParams.gateTick;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateTxTick;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateNum;
    pOpResult->buffer[pos++] = g_sDeviceParams.rfu1;
    pOpResult->buffer[pos++] = g_sDeviceParams.rfu2 >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.rfu2 >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.ledLowVolLev >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.ledLowVolLev >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.alarmTmpr >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.alarmTmpr >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.fulVolLev >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.fulVolLev >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.lowVolLev >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.lowVolLev >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.higVolLev >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.higVolLev >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].vol >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].vol >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].cur  >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].cur  >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].vol >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].vol >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].cur >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].cur >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].vol >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].vol >> 0;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].cur >> 8;
    pOpResult->buffer[pos++] = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].cur >> 0;
    pOpResult->buffer[pos++] = pOpResult->err;
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pOpResult->buffer, pos); //从LEN开始计算crc
    pOpResult->buffer[pos++] = crc & 0xFF;
    pOpResult->buffer[pos++] = (crc >> 8) & 0xFF;

    pOpResult->len = pos;

    return pos;
}



u16 Device_ResponseGateFrame(u8 add, u8 mode, READER_RSPFRAME *pOpResult)
{
    u16 pos = 0;
    u16 crc = 0;
    u8 index = 0;
    u8 i = 0;
    pOpResult->buffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    if(mode == DEVICE_GET_GATEINFO_MODE_ONCE)
    {
          if(add <=(GATE_SLAVER_NUM << 1))
          {
             pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.tmpr;
             pOpResult->buffer[pos++] =  (g_aGateSlvInfo[add].sensorInfo.sensorState.fan << 3) | 
                                       (g_aGateSlvInfo[add].sensorInfo.sensorState.smoke << 2) | 
                                        (g_aGateSlvInfo[add].sensorInfo.sensorState.rfid << 1) | 
                                          (g_aGateSlvInfo[add].sensorInfo.sensorState.door << 0);
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.state >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.verHard >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.verHard >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.verSoft >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.verSoft >> 0;
            memcpy(pOpResult->buffer + pos, g_aGateSlvInfo[add].sensorInfo.batInfo.sn, BAT_SN_LEN);
            pos +=  BAT_SN_LEN ;

            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.volValue >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.volValue >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.volLev >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.cycleCount >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.cycleCount >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.chargeStatus >> 0;
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.maxChagVol >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.maxChagVol >> 0;
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.maxChagCur >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.maxChagCur >> 0;
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.remainCap >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.remainCap >> 0;
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.curValue >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.curValue >> 0;
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.tmpr >> 0;
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.status.dltVol >> 0;
            

            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.err.fullState >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.err.protectStatus1 >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.err.alarmStatus1 >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.err.alarmStatus2 >> 0;  
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.err.diagStatus >> 0;
             
            for(i = 0; i< BAT_UNIT_NUM; i++)
            {
                pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.unitVol[i] >> 8;
                pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.batInfo.unitVol[i] >> 0;

            }
            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.state >> 0;
            memcpy(pOpResult->buffer + pos, g_aGateSlvInfo[add].sensorInfo.chagInfo.vendorName,CHAG_VENDOR_NAME_LEN);
            pos += CHAG_VENDOR_NAME_LEN;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.softVer >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.softVer >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.hardVer >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.hardVer >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.type >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.type >> 0;            
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.pwr >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.pwr >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.maxVol >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.maxVol >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.maxCur >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.maxCur >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.chagVol >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.chagVol >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.chagCur >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.chagCur >> 0;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.errStatusInfo >> 8;
            pOpResult->buffer[pos++] = g_aGateSlvInfo[add].sensorInfo.chagInfo.errStatusInfo >> 0;
          }
    
    }
    else
    {
        for(index = 0 ;index < (GATE_SLAVER_NUM << 1); index ++)
        {
          pOpResult->buffer[pos++] = index + 1;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.tmpr >> 0;
          pOpResult->buffer[pos++] = (g_aGateSlvInfo[index].sensorInfo.sensorState.fan << 3) |
                                   (g_aGateSlvInfo[index].sensorInfo.sensorState.smoke << 2) | 
                                    (g_aGateSlvInfo[index].sensorInfo.sensorState.rfid << 1) | 
                                      (g_aGateSlvInfo[index].sensorInfo.sensorState.door << 0);
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.state >> 0;
          memcpy(pOpResult->buffer + pos, g_aGateSlvInfo[index].sensorInfo.batInfo.sn, BAT_SN_LEN);
          pos +=  BAT_SN_LEN ;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.volValue >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.volValue >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.volLev >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.cycleCount >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.cycleCount >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.chargeStatus >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.remainCap >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.remainCap >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.curValue >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.curValue >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.tmpr >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.dltVol >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.err.fullState >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.err.protectStatus1 >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.err.alarmStatus1 >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.err.alarmStatus2 >> 0;  
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.err.diagStatus >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.state >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.pwr >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.pwr >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.maxVol >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.maxVol >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.maxCur >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.maxCur >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.chagVol >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.chagVol >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.chagCur >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.chagCur >> 0;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.errStatusInfo >> 8;
          pOpResult->buffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.errStatusInfo >> 0;
          
        }
    }
      
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    pOpResult->buffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pOpResult->buffer, pos); //从LEN开始计算crc
    pOpResult->buffer[pos++] = crc & 0xFF;
    pOpResult->buffer[pos++] = (crc >> 8) & 0xFF;

    pOpResult->len = pos;

    return pos;
}







const PORT_INF DEV_INSEN_WAT_FB = {GPIOA, GPIO_Pin_7}; 
void Device_InitInSensor(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};


    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    
    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_WAT_FB.Pin;
    GPIO_Init(DEV_INSEN_WAT_FB.Port, &GPIO_InitStructure);
}



void Device_GateRsvSlvInfo(u8 *pParams, u16 paramsLen, GATE_OPINFO *pOpInfo)
{
    u16 pos = 0;
    u8 num = 0, i = 0;
    u8 infoIdx = 0;
    GATE_SENSORINFO *pInfo = NULL, *pPreInfo = NULL;
    
    num = pParams[pos++];
    for(i = 0; i < num; i++)
    {
        pInfo = (GATE_SENSORINFO *)(pParams + pos);
        infoIdx = (pOpInfo->slvIndex << 1) + pInfo->addr;
        pPreInfo = &g_aGateSlvInfo[infoIdx].sensorInfo;
        if(g_aGateSlvInfo[infoIdx].state != GATE_STAT_OK)
        {
            g_aGateSlvInfo[infoIdx].state = GATE_STAT_OK;
            g_aGateSlvInfo[infoIdx].bTxInfo = TRUE;
            
        }
        else 
        {
            if((memcmp(&pInfo->sensorState, &pPreInfo->sensorState, 1) != 0) ||
               (pInfo->batInfo.state != pPreInfo->batInfo.state) || 
               (pInfo->chagInfo.state != pPreInfo->chagInfo.state))     //状态发生变化
            {
                g_aGateSlvInfo[infoIdx].bTxInfo = TRUE;//
            }
            else
            {
                if(g_aGateSlvInfo[infoIdx].txTick + g_sDeviceParams.gateTxTick < g_nSysTick)
                {
                    g_aGateSlvInfo[infoIdx].bTxInfo = TRUE;
                }
            }
        }
        memcpy(pPreInfo, pInfo, sizeof(GATE_SENSORINFO));

        pos += sizeof(GATE_SENSORINFO);
    }
}



BOOL Device_GateProceRspFrame(u8 *pFrame, GATE_OPINFO *pOpInfo, u32 tick)
{
    BOOL b = FALSE;
    u8 *pParams = NULL;
    u16 paramsLen = 0;
    u16 addr = 0;

    addr = *((u16 *)(pFrame + UART_FRAME_POS_SRCADDR));
    if(addr == pOpInfo->slvIndex + 1 && pFrame[UART_RFRAME_POS_CMD] == pOpInfo->cmd)
    {
        b = TRUE;
        if(pFrame[UART_FRAME_POS_LEN] == 0)
        {
            pParams = pFrame + UART_RFRAME_POS_PAR + 2;
            paramsLen = *((u16 *)(pFrame + UART_RFRAME_POS_PAR));
        }
        else
        {
            pParams = pFrame + UART_RFRAME_POS_PAR;
            paramsLen = pFrame[UART_FRAME_POS_LEN] + 3 - UART_RFRAME_MIN_LEN;
        }
        switch(pOpInfo->cmd)
        {

            case GATE_FRAME_CMD_GET_ININFO:
                
                Device_GateRsvSlvInfo(pParams, paramsLen, pOpInfo);
            break;
            case GATE_FRAME_CMD_SET_OUTINFO:
              
              a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE);
              if(!a_CheckStateBit(g_nDeviceTestInfo.flag, DEVICE_TEST_FLAG_DOOE_MODE))
              {
                  Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
              }
            break;
            case GATE_FRAME_CMD_CHARGE:
              
                a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE);
                pOpInfo->slvCmd.cmd = 0;            //操作完成，清空操作
                Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
            break;
            case GATE_FRAME_CMD_RTNBAT:
            if(paramsLen == 2)
            {
                  if(a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_REBAT))
                  {  
                       pOpInfo->rtnBat.step  = pParams[0];     
                        pOpInfo->rtnBat.flag = pParams[1]; 
                       a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_REBAT);
                       pOpInfo->batOpState = GATE_OP_BAT_STAT_ING;
                       g_sDeviceRspFrame.len = Device_ResponseBat(&g_sDeviceRspFrame, GATE_FRAME_CMD_RTNBAT);
                       Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
                  }
                  else
                  {
                    if(pParams[0] == 0x00)
                    {
                        b = TRUE;       //这里要上报数据

                        pOpInfo->rtnBat.tick
                          = g_nSysTick;
                        pOpInfo->rtnBat.step  = pParams[0];     
                        pOpInfo->rtnBat.flag = pParams[1]; 
                        pOpInfo->batOpState = GATE_OP_BAT_STAT_OVER;
                        g_nDeviceImpRspInfo.rtuLen = Device_ResponseRtBat(g_nDeviceImpRspInfo.rtuBuffer);
                        Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_REBAT);
                        pOpInfo->slvCmd.cmd = 0;            //操作完成，清空操作

                    }
                    else
                    {
                        if((pOpInfo->rtnBat.step != pParams[0]) || (pOpInfo->rtnBat.flag != pParams[1]))
                        {

                           pOpInfo->rtnBat.tick = g_nSysTick;
                           pOpInfo->rtnBat.step  = pParams[0];     
                           pOpInfo->rtnBat.flag = pParams[1];
                           pOpInfo->batOpState = GATE_OP_BAT_STAT_ING;
                           g_nDeviceImpRspInfo.rtuLen = Device_ResponseRtBat(g_nDeviceImpRspInfo.rtuBuffer);
                           Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_REBAT);
                            
                        }
                        b = FALSE;
                        pOpInfo->tick = g_nSysTick;
                    }
                    
                  }  
                  Device_Ctr_BatVolce(g_sGateOpInfo.add, DEVICE_BAT_RTN, pOpInfo->rtnBat.step, pOpInfo->rtnBat.flag)  ;
            }
            else
            {
                b = FALSE;
                pOpInfo->tick = g_nSysTick;
            }
            break;
            case GATE_FRAME_CMD_GET_VERSON:
                
            break;
            case GATE_FRAME_CMD_BRWBAT:
            if(paramsLen == 2)
            {
                    if(a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_BWBAT))
                    {  
                        pOpInfo->brwBat.step  = pParams[0];     
                        pOpInfo->brwBat.flag = pParams[1];
                        a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_BWBAT);
                        pOpInfo->batOpState = GATE_OP_BAT_STAT_ING;
                        g_sDeviceRspFrame.len = Device_ResponseBat(&g_sDeviceRspFrame, GATE_FRAME_CMD_BRWBAT);
                        Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
                    }
                    else
                    {
                      if(pParams[0] == 0x00)
                      {
                          b = TRUE;       //这里要上报数据
                          pOpInfo->slvCmd.cmd = 0;            //操作完成，清空操作
                          pOpInfo->brwBat.step  = pParams[0];     
                          pOpInfo->brwBat.flag = pParams[1];
                          pOpInfo->brwBat.tick = g_nSysTick;
                          pOpInfo->batOpState = GATE_OP_BAT_STAT_OVER;
                          g_nDeviceImpRspInfo.brwLen = Device_ResponseBrBat(g_nDeviceImpRspInfo.brwBuffer);
                          Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_BWBAT);
                        
                      }
                      else
                      {
                          if((pOpInfo->brwBat.step != pParams[0]) || (pOpInfo->brwBat.flag != pParams[1]))
                          {
                              pOpInfo->brwBat.tick = g_nSysTick;
                              pOpInfo->brwBat.step  = pParams[0];     
                              pOpInfo->brwBat.flag = pParams[1];
                              pOpInfo->batOpState = GATE_OP_BAT_STAT_ING;
                              g_nDeviceImpRspInfo.brwLen = Device_ResponseBrBat(g_nDeviceImpRspInfo.brwBuffer);
                              Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_BWBAT);
                          }
                          b = FALSE;
                          pOpInfo->tick = g_nSysTick;

                      }
                      
                    }
                    Device_Ctr_BatVolce(g_sGateOpInfo.add, DEVICE_BAT_BRW, pOpInfo->brwBat.step, pOpInfo->brwBat.flag);
                    
            }
            else
            {
                b = FALSE;
                pOpInfo->tick = g_nSysTick;
            }

            break;
            case GATE_FRAME_CMD_SET_PARAMS:   //  10
              if(paramsLen == 4)
              {
                    //版本信息
                    memcpy(g_aGateSlvInfo[2 * (addr - 1)].softWare, pParams, GATE_VERSION_LEN);
                    memcpy(g_aGateSlvInfo[2 * (addr - 1) + 1].softWare, pParams, GATE_VERSION_LEN);
                    memcpy(g_aGateSlvInfo[2 * (addr - 1)].hardWare, pParams + GATE_VERSION_LEN, GATE_VERSION_LEN);
                    memcpy(g_aGateSlvInfo[2 * (addr - 1) + 1].hardWare, pParams + GATE_VERSION_LEN, GATE_VERSION_LEN);
                  //g_aGateSlvInfo[2 * (addr - 1)].softWare = g_aGateSlvInfo[2 * (addr - 1) + 1].softWare = *(pParams + 0) << 24 | *(pParams + 1) << 16 | *(pParams + 2) << 8 | *(pParams + 3) << 0;  
              } 
            break;
            case GATE_FRAME_CMD_GET_PARAMS:
            break;
          case GATE_FRAME_CMD_PLANE_BAT:
                
                a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE);
                g_sDeviceRspFrame.len = Device_GateResponse(&g_sDeviceRspFrame, pParams, 1);
                Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
          break;
        }
    }
    return b;
}



u16 Device_WaterProceRspFrame(u8 *pFrame, WATER_INFO *pOpInfo, u8 len)
{
    u8 cmd = 0;
    u8 paramsLen = 0;

    cmd = *(pFrame + UART_FRAME_POS_CMD + 1);
    pOpInfo->txBuf.cmd = cmd;
    pOpInfo->txBuf.flag = UART_FRAME_FLAG_OK;
    pOpInfo->txBuf.err = UART_FRAME_RSP_NOERR;
    pOpInfo->txBuf.len = 0;
  paramsLen = len - UART_FRAME_MIN_LEN;
    switch(cmd)
    {
        case WATER_CMD_GET_UID:
            if(paramsLen > 0)
            {
                if(memcmp(pOpInfo->uid, pFrame + 20, WATER_UID_LEN))
                {
                    memcpy(pOpInfo->uid, pFrame + 20, WATER_UID_LEN);
                }
                g_sWaterInfo.txBuf.repat[WATER_STAT_MODE_RFID] = 0;
                g_sWaterInfo.txBuf.result[WATER_STAT_MODE_RFID] = TRUE;
            }
            break;
        case DEVICE_CMD_MQTT_KEY_WRITE:
            if(paramsLen > 0)
            {

            }
            break;  
          case DEVICE_CMD_MQTT_GET_IMEI:
            if(paramsLen > 0)
            {
                //Water_WriteStr((char *)g_nImsiStr);
                //Reader_Delayms(5000);
            }
            break; 
            
    }
    if(pOpInfo->txBuf.len == 0)
    {
        pOpInfo->txBuf.flag = UART_FRAME_FLAG_FAIL;
        pOpInfo->txBuf.err = UART_FRAME_RSP_CRCERR;
        //pOpInfo->txBuf.len = Reader_ResponseFrame(NULL, 0, &g_sReaderRspFrame);
    }
    return pOpInfo->txBuf.len;
}





u16 Reader_ProcessUartFrame(u8 *pFrame, u8 add, u16 len, u32 tick)
{ 
    u8 cmd = 0;
    BOOL bRfOperation = FALSE;
    u16 paramsLen = 0;
    u8 data = 0;
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
            case DEVICE_CMD_RESET:
                if(paramsLen == 0)
                {
                    if(add == DEVICE_SM5001_ID)
                    {
                        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                        g_sDeviceRspFrame.flag = DEVICE_RESPONSE_FLAG_RESET;
                    }
                    else if(add == DEVICE_SM5003_ID)
                    {
                    
                    }
                    else if(add >= 1 && add <= GATE_SLAVER_NUM * 2)
                    {
                    
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    }
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
            case DEVICE_CMD_GET_VERSION:
                if(paramsLen == 20)
                {
                    g_sDeviceRspFrame.len = Device_ResponseFrame((u8 *)DEVICE_VERSION, DEVICE_VERSION_SIZE, &g_sDeviceRspFrame);
                }
                break;
            case DEVICE_CMD_GET_CPUID:
                if(paramsLen == 0)
                {
                    g_sDeviceRspFrame.len = Device_ResponseFrame((u8 *)STM32_CPUID_ADDR, STM32_CPUID_LEN, &g_sDeviceRspFrame);
                }
                break;
            case  DEVICE_CMD_GET_CFG:
                if(paramsLen == 0)
                {
                    if(add == DEVICE_SM5001_ID)
                    {
                        g_sDeviceRspFrame.len = Device_ResponseCfg(&g_sDeviceRspFrame);

                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                    }
                }
                break;
            case  DEVICE_CMD_GATE_DISENABLE:
                if(paramsLen == 0)
                {
                    if(add == DEVICE_SM5001_ID)
                    {
                        g_nDeviceTestInfo.flag = DEVICE_TEST_FLAG_DOOE_MODE ;
                        Device_Voice_Apo(SOUND_CNT_TIME_1S * 2, SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_DI);

                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                    }
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
            case DEVICE_CMD_SET_CFG:
                if(paramsLen == DEVICE_SET_CFG_FRAME_LEN + 4)
                {
                    
                    if(add == DEVICE_SM5001_ID)
                    {
                        Device_Set_Cfg(pFrame);
                        
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    }
                    
                }
                
                g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                break;

            case DEVICE_CMD_ACTCTL_CTL:
                if(paramsLen == DEVICE_ACTCTL_CTL_FRAME_LEN)
                {
                    if(add == DEVICE_SM5001_ID)
                    {
                        Modele_Ctl(data);
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    }  
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
            case GATE_FRAME_CMD_SET_OUTINFO:
                if(paramsLen == DEVICE_ACTCTL_CTL_FRAME_LEN)
                {
                    a_SetStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE);

                    if(Device_Chk_Gate(add))
                    {
                        if(*(pFrame + 0) & DEVICE_OUT_CTRL_POS_FAN || *(pFrame + 0) & DEVICE_OUT_CTRL_POS_DOOR || *(pFrame + 0) == DEVICE_OUT_NULL)
                        {
                            Device_Ansy_Frame(add, GATE_FRAME_CMD_SET_OUTINFO, *(pFrame + 0));
                            Gate_TxFrame(&g_sGateOpInfo, tick);
                           
                        }
                        else
                        {
                            Device_Ansy_Frame(add, GATE_FRAME_CMD_SET_OUTINFO, *(pFrame + 0));
                            Gate_TxFrame(&g_sGateOpInfo, tick);
                            g_sDeviceRspFrame.err = READER_RESPONSE_ERR_FREAM;
                        
                        }
                    }
                    else
                    {
                        Device_Ansy_Frame(add, GATE_FRAME_CMD_SET_OUTINFO, *(pFrame + 0));
                        Gate_TxFrame(&g_sGateOpInfo, tick);
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        
                    }  
                    
                    
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
                
            case GATE_FRAME_CMD_CHARGE:
                if(paramsLen == DEVICE_ACTCTL_CTL_FRAME_LEN)
                {
                  
                    a_SetStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE);
                    if(Device_Chk_Gate(add))
                    {
                        if(*(pFrame + 0) & DEVICE_OUT_CTRL_POS_FAN || *(pFrame + 0) & DEVICE_OUT_CTRL_POS_DOOR || *(pFrame + 0) == DEVICE_OUT_NULL)
                        {
                            Device_Ansy_Frame(add, GATE_FRAME_CMD_CHARGE, *(pFrame + 0));
                            Gate_TxFrame(&g_sGateOpInfo, tick);
                            
                        }
                        else
                        {
                            Device_Ansy_Frame(add, GATE_FRAME_CMD_CHARGE, *(pFrame + 0));
                            Gate_TxFrame(&g_sGateOpInfo, tick);
                            g_sDeviceRspFrame.err = READER_RESPONSE_ERR_FREAM;
                        
                        }
                    }
                    else
                    {
                        Device_Ansy_Frame(add, GATE_FRAME_CMD_CHARGE, *(pFrame + 0));
                        Gate_TxFrame(&g_sGateOpInfo, tick);
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        
                    }  
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
    
          case GATE_FRAME_CMD_RTNBAT:
                if(paramsLen == DEVICE_RETURN_BAT_FRAME_LEN)
                {
                    static u8 snRData[DEVICE_BAT_SN_LEN]={0}               ;
                    a_SetStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_REBAT);
                    if(Device_Chk_Gate(add))
                    {
                        if(g_sGateOpInfo.batOpState != GATE_OP_BAT_STAT_ING)
                        {
                            Device_Gate_RtBat(add);
                            memcpy(g_sGateOpInfo.slvCmd.params + 1 , pFrame, DEVICE_BAT_SN_LEN);
                            memcpy(snRData , pFrame, DEVICE_BAT_SN_LEN);
                            Gate_TxFrame(&g_sGateOpInfo, tick);
                        }
                       // else
                        {
                            //Device_Voice_Apo(SOUND_CNT_TIME_1S * 8, SOUND_REPAT_NULL, SOUND_VOICE_NO_BAT, SOUND_VOC_NO_BAT_CHANGE);
                        }
                    }
                    else
                    {
                        Gate_TxFrame(&g_sGateOpInfo, tick);
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        
                    }  
                    //Water_EnableTxDma(pFrame, paramsLen);
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
        case GATE_FRAME_CMD_BRWBAT:
                if(paramsLen == DEVICE_RETURN_BAT_FRAME_LEN)
                {
                    a_SetStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_BWBAT);
                    static u8 snBData[DEVICE_BAT_SN_LEN]={0}               ;
                    if(Device_Chk_Gate(add))
                    {
                        if(Device_ChkBat_Num())
                        {
                            Device_Gate_BrBat(add);
                            memcpy(g_sGateOpInfo.slvCmd.params + 1 , pFrame, DEVICE_BAT_SN_LEN);
                            memcpy(snBData , pFrame, DEVICE_BAT_SN_LEN);
                            Gate_TxFrame(&g_sGateOpInfo, tick);
                        }
                        else
                        {
                            Device_Voice_Apo(SOUND_CNT_TIME_1S * 8, SOUND_REPAT_NULL, SOUND_VOICE_NO_BAT, SOUND_VOC_NO_BAT_CHANGE);
                        }
                    }
                    else
                    {
                        Gate_TxFrame(&g_sGateOpInfo, tick);
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        
                    }
                     //Water_EnableTxDma(pFrame, paramsLen);
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
          case GATE_FRAME_CMD_PLANE_BAT:
                if(paramsLen == DEVICE_RETURN_BAT_FRAME_LEN + 1)
                {
                    a_SetStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE);
                    
                    if(Device_Chk_Gate(add))
                    {
                        if(g_sGateOpInfo.batOpState != GATE_OP_BAT_STAT_ING)
                        {
                          Device_Gate_PlBat(add, *(pFrame + 1));
                          memcpy(g_sGateOpInfo.slvCmd.params + 1 , pFrame, DEVICE_BAT_SN_LEN + 1);
                          Gate_TxFrame(&g_sGateOpInfo, tick);
                        }
                    }
                    else
                    {                                                                                   
                        Gate_TxFrame(&g_sGateOpInfo, tick);
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                        
                    }  
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                }
                break;
          case GATE_FRAME_CMD_GET_ININFO:
                if(paramsLen == DEVICE_ACTCTL_CTL_FRAME_LEN)
                {
                    if( *(pFrame + 0) == DEVICE_GET_GATEINFO_MODE_ONCE || *(pFrame + 0) == DEVICE_GET_GATEINFO_MODE_ALL)
                    {
                        g_sDeviceRspFrame.len = Device_ResponseGateFrame(add -1 , *(pFrame + 0) ,&g_sDeviceRspFrame);
                    }
                    else
                    {
                        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_FREAM;
                        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                    
                    }
                }
                break;
            case DEVICE_CMD_GATE_KEY_CTR:
                if(paramsLen == DEVICE_KEY_UID_LEN)
                {
                         memcpy(&g_sMqttKey.uid, pFrame , FRAME_UID_LEN);
                         g_sDeviceRspFrame.err = READER_RESPONSE_NOERR;
                         Device_WriteMqttKey();
                         g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);     
                }
                break;

        }
     if(g_sDeviceRspFrame.len == 0 && bRfOperation == FALSE)
    {
        g_sDeviceRspFrame.flag = DEVICE_RSPFRAME_FLAG_FAIL;
        g_sDeviceRspFrame.err = DEVICE_OPTAG_RESPONSE_PARERR;
        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
    }
 
    return g_sDeviceRspFrame.len ;
}


void Device_Gate_Rsp(u8 flag)
{
  


}

BOOL Device_Set_Cfg(u8 *pBuffer)
{
    BOOL bOk = TRUE;
    u8 pos = 0;
  
    g_sDeviceParams.gateTick = (*(pBuffer + pos++) & 0xFF);
    g_sDeviceParams.gateTxTick = (*(pBuffer + pos++) & 0xFF);
    g_sDeviceParams.gateNum = (*(pBuffer + pos++) & 0xFF);
    g_sDeviceParams.rfu1 = (*(pBuffer + pos++) & 0xFF);
    g_sDeviceParams.rfu2 = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.rfu2 = g_sDeviceParams.rfu2 | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.ledLowVolLev = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.ledLowVolLev = g_sDeviceParams.gateParams.ledLowVolLev | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.alarmTmpr = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.alarmTmpr = g_sDeviceParams.gateParams.alarmTmpr | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.fulVolLev = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.fulVolLev = g_sDeviceParams.gateParams.chagParams.fulVolLev | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.lowVolLev = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.lowVolLev = g_sDeviceParams.gateParams.chagParams.lowVolLev | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.higVolLev = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.higVolLev = g_sDeviceParams.gateParams.chagParams.higVolLev | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].vol = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].vol = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].vol | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].cur = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].cur = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX0].cur | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].vol = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].vol = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].vol | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].cur = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].cur = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX1].cur | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].vol = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].vol = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].vol | ((*(pBuffer + pos++) & 0xFF) << 0);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].cur = ((*(pBuffer + pos++) & 0xFF) << 8);
    g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].cur = g_sDeviceParams.gateParams.chagParams.stepParams[CHAG_STEP_IDX2].cur | ((*(pBuffer + pos++) & 0xFF) << 0);

    Device_WriteDeviceParamenter();

    return bOk;

}

void Device_ServerProcessRxInfo(W232_RCVBUFFER *pRcvBuffer, u32 tick)               //处理服务器下发数据
{

    u16 crc1 = 0, crc2 = 0;
   
    if(pRcvBuffer->len >= DEVICE_FREAM_MIN_LEN && strlen(pRcvBuffer->bufferStr) > 0)//(pRcvBuffer->cmd == DEVICE_CMD_GET_CFG || pRcvBuffer->cmd == DEVICE_CMD_RESET )//|| pRcvBuffer->cmd == GATE_FRAME_CMD_GET_ININFO) 
    {
        crc1 = Uart_GetFrameCrc(pRcvBuffer->buffer, pRcvBuffer->len);
        crc2 = a_GetCrc(pRcvBuffer->buffer, pRcvBuffer->len - 2);

    }
    else if(pRcvBuffer->len == 0)
    {
       crc1 = crc2 = 0xFF;
    }
    else
    {
        crc1 = 0xFF;
    }
    
    if(crc1 == crc2)
    {
        u16 txLen = 0;
    
        if(!a_CheckStateBit(g_nDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_RX_AT) && !a_CheckStateBit(g_nDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_WAIT))
        {
          txLen = Reader_ProcessUartFrame(pRcvBuffer->buffer, pRcvBuffer->addr, pRcvBuffer->len, tick);
        }
        if(txLen > 0)
        {
          if(!a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE) && !a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_BWBAT) && !a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_REBAT))
          {
            Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);     
            if(g_sDeviceRspFrame.cmd == DEVICE_CMD_RESET && g_sDeviceRspFrame.flag == DEVICE_RESPONSE_FLAG_RESET)
            {
                Reader_Delayms(5);
                Sys_SoftReset();
            }
          }
        }
    }
    else
    {  
        g_sDeviceRspFrame.err = READER_RESPONSE_ERR_LEN;
        g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
        Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);

    }

}

void Modele_Ctl(u8 cmd)
{
    if(cmd & DEVICE_MODELE_LED)
    {
        IO_Led_Open();
        g_sIoInfo.state |= IO_STAT_LIGHT;
        
    }
    else 
    {
        IO_Led_Close();
        g_sIoInfo.state &= ~ IO_STAT_LIGHT;
    }


    if(cmd & DEVICE_MODELE_FAN)
    {
        IO_Fan_Open();
        g_sIoInfo.state |= IO_STAT_FAN;
    }
    else 
    {
        IO_Fan_Close();
        g_sIoInfo.state &= ~ IO_STAT_FAN;
    }


    if(cmd & DEVICE_MODELE_MOTOR)
    {
        IO_Door_Open();
    }
    else 
    {
        IO_Door_Close();
        
    }


    if(cmd & DEVICE_MODELE_RELAY)
    {
        IO_Realy_Open();
        g_sIoInfo.state |= IO_STAT_RELAY;
    }
    else
    {
        IO_Realy_Close();
        g_sIoInfo.state &= ~ IO_STAT_RELAY;
    }      
}



void Device_UpHeartData(u32 id, u8 *pBuffer, char *strAtBuff, char *strRspBuff, u16 lenth)
{
    char strRspbuffer[W232_STR_BUFFER_HEART_LEN] ={0};  
    a_Hex2Str(strRspbuffer, pBuffer, lenth);

    sprintf(strRspBuff,"{\"id\":%.8d,\"dp\":{\"device\":[{\"v\":\"FF20%.4d%s\"}]}}",id, lenth, strRspbuffer);
    sprintf(strAtBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/dp/post/json\",%d",W232_PRDOCT_ID, g_nImsiStr, strlen(strRspBuff));
}




void Reader_UpReBatata(u32 id, u8 *pBuffer, u16 lenth)
{
    char strBuff1[W232_STR_BUFFER_HEART_LEN] ={0};
    char strBuff[W232_STR_BUFFER_LEN] ={0};
    char strRspbuffer[W232_STR_BUFFER_HEART_LEN] ={0};  
    
    a_Hex2Str(strRspbuffer, pBuffer, lenth);

    sprintf(strBuff1,"{\"id\":%.8d,\"dp\":{\"rebat\":[{\"v\":\"FF20%.4d%s\"}]}}",id, lenth, strRspbuffer);
    sprintf(strBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/dp/post/json\",%d",W232_PRDOCT_ID, g_sW232Connect.imei, strlen(strBuff1));
    W232_WriteCmd(strBuff);
    W232_Delayms(50);
    W232_WriteBuffer((u8 *)strBuff1, strlen(strBuff1));
    

}

u16 Device_HeartFormat(u8 *pBuffer, u32 tick)
{
    u16 pos = 0;
    u16 crc = 0;
    u8 index = 0;

    pBuffer[pos++] = 0x1F;
    pBuffer[pos++] = (tick >> 24) & 0xFF;
    pBuffer[pos++] = (tick >> 16) & 0xFF;
    pBuffer[pos++] = (tick >>  8) & 0xFF;
    pBuffer[pos++] = (tick >>  0) & 0xFF;
  //柜体信息      
    pBuffer[pos++] = 0xFF;
    pBuffer[pos++] = g_nImei[0];
    pBuffer[pos++] = g_nImei[1];
    pBuffer[pos++] = g_nImei[2];
    pBuffer[pos++] = g_nImei[3];
    pBuffer[pos++] = g_nImei[4];
    pBuffer[pos++] = g_nImei[5];
    pBuffer[pos++] = g_nImei[6];
    pBuffer[pos++] = (g_nImei[7] & 0xF0);
    
    memcpy(pBuffer + pos, READER_SOFTWARE_VERSION, DEVICE_VERSION_LEN);
    pos += DEVICE_VERSION_LEN;
    memcpy(pBuffer + pos, READER_HARDWARE_VERSION, DEVICE_VERSION_LEN);
    pos += DEVICE_VERSION_LEN;
   
    pBuffer[pos++] = (g_sElectInfo.electValue >> 24) & 0xFF;
    pBuffer[pos++] = (g_sElectInfo.electValue >> 16) & 0xFF;
    pBuffer[pos++] = (g_sElectInfo.electValue >>  8) & 0xFF;
    pBuffer[pos++] = (g_sElectInfo.electValue >>  0) & 0xFF;
    pBuffer[pos++] = (g_sDeviceParams.temprUp.t >> 0) & 0xFF;
    pBuffer[pos++] = (g_sDeviceParams.temprDown.t >> 0) & 0xFF;

    pBuffer[pos++] = g_sIoInfo.warnValue;
    //喷淋信息
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    
  //仓体信息
    for(index = 0 ;index < (GATE_SLAVER_NUM << 1); index ++)
    {
        pBuffer[pos++] = index + 1;
        if(g_aGateSlvInfo[index].bTxInfo)
        {
            //memcpy(pBuffer + pos, &g_aGateSlvInfo[index].softWare, 4);
            //pos += 4;
            /*pBuffer[pos++] = (g_aGateSlvInfo[index].softWare >> 24) & 0xFF;
            pBuffer[pos++] = (g_aGateSlvInfo[index].softWare >> 16) & 0xFF;
            pBuffer[pos++] = (g_aGateSlvInfo[index].softWare >>  8) & 0xFF;
            pBuffer[pos++] = (g_aGateSlvInfo[index].softWare >>  0) & 0xFF;
            */
            memcpy(pBuffer + pos, g_aGateSlvInfo[index].softWare, GATE_VERSION_LEN);
            pos += GATE_VERSION_LEN;
            memcpy(pBuffer + pos, g_aGateSlvInfo[index].hardWare, GATE_VERSION_LEN);
            pos += GATE_VERSION_LEN;
            pBuffer[pos++] = g_aGateSlvInfo[index].sensorInfo.tmpr;
            pBuffer[pos++] = (g_aGateSlvInfo[index].sensorInfo.sensorState.fan << 3) | 
                           (g_aGateSlvInfo[index].sensorInfo.sensorState.smoke << 2) | 
                            (g_aGateSlvInfo[index].sensorInfo.sensorState.rfid << 1) | 
                             (g_aGateSlvInfo[index].sensorInfo.sensorState.door << 0);
            pBuffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.state;
            pBuffer[pos++] = g_aGateSlvInfo[index].sensorInfo.batInfo.status.volLev;
            pBuffer[pos++] = g_aGateSlvInfo[index].sensorInfo.chagInfo.state;
        
        }
        else
        {
            memset(pBuffer + pos, 0xFF, DEVICE_GATE_LEN);
            pos += DEVICE_GATE_LEN;
      
        }
    
        }
    
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;
    crc = a_GetCrc(pBuffer, pos); //从LEN开始计算crc
    pBuffer[pos++] = crc & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;



   return pos;
}




void Device_ChkTempr()
{
    Win_CalAvg(&g_sDeviceParams.temprUp, AD_GetTemper1Value());
    AD_GetTmpr(&g_sDeviceParams.temprUp);
    Win_CalAvg(&g_sDeviceParams.temprDown, AD_GetTemper2Value());
    AD_GetTmpr(&g_sDeviceParams.temprDown);

    if((g_sDeviceParams.temprUp.t >= DEVICE_TEMPR_HIGH) || (g_sDeviceParams.temprDown.t >=  DEVICE_TEMPR_HIGH))
    {
        g_sIoInfo.flag |= IO_FLAG_TEMPR_UP;
    }
    else
    {
        g_sIoInfo.flag &= ~ IO_FLAG_TEMPR_UP;
        
    }
}




void Device_IO_Ctr(u8 flag)
{
    
     if(a_CheckStateBit(g_sIoInfo.flag, IO_FLAG_TEMPR_UP) || a_CheckStateBit(g_sIoInfo.flag, IO_FLAG_FIRE) || a_CheckStateBit(g_sIoInfo.state, IO_STAT_WATER))//////smoke 
     {
        if(!a_CheckStateBit(g_sIoInfo.state, IO_STAT_FAN) || a_CheckStateBit(g_sIoInfo.state, IO_STAT_RELAY))
       {
          IO_Fan_Open();
          IO_Realy_Close();    //测试关闭
          a_SetStateBit(g_sIoInfo.state,IO_STAT_FAN);
          a_ClearStateBit(g_sIoInfo.state,IO_STAT_RELAY);
       }
     }
    else
    {
       if(!a_CheckStateBit(g_sIoInfo.state, IO_STAT_RELAY))
       {
         IO_Realy_Open();
         a_SetStateBit(g_sIoInfo.state,IO_STAT_RELAY);
       }
       
        if(a_CheckStateBit(g_sIoInfo.state, IO_STAT_FAN))
       {
         IO_Fan_Close();
         a_ClearStateBit(g_sIoInfo.state,IO_STAT_FAN);
       }
    }


}

void Device_Ctr_BatVolce(u16 add, u8 mode, u8 step, u8 flag)
{

  if(mode == DEVICE_BAT_RTN)
  {
      if(step == DEVICE_STEP_OVER)
      {
          switch(flag)
          {
            case DEVICE_STEP_FLAG_OK:

                    Device_Voice_Apo(SOUND_CNT_TIME_1S * 8, SOUND_REPAT_NULL, SOUND_VOICE_RTU_BAT_OK, SOUND_VOC_RETURN_OK );
                  //还电池完成
              break;
            case DEVICE_STEP_FLAG_DOOR_FAIL:
              
              break;
            case DEVICE_STEP_FLAG_BAT_NO_RTN:
              
                    Device_Voice_Apo(SOUND_CNT_TIME_1S * 8, SOUND_REPAT_NULL, SOUND_VOICE_NO_BAT, SOUND_VOC_RETURN_FAIL);
              break;
            case DEVICE_STEP_FLAG_BAT_FRAME_ERR:
              
              break;
          }
        
      }
      else if(step == DEVICE_STEP_ONE)
      {
          switch(flag)
          {
            case DEVICE_STEP_FLAG_DOOR_OK:
              
              Device_Voice_Apo(SOUND_CNT_TIME_1S * 8, SOUND_REPAT_NULL, SOUND_VOICE_RTU_BAT, g_sGateOpInfo.add + 1);
              break;
            case DEVICE_STEP_FLAG_DOOR_LOADING:
              
              break;
          }
      }
      else if(step == DEVICE_STEP_TWICE)  
      {
          switch(flag)
          {
            case DEVICE_STEP_FLAG_DOOR_NOCLOSE_BAT_INSER:

              if(g_sGateOpInfo.batAddr != GATE_BAT_OUT)
              {
                g_sGateOpInfo.batAddr = GATE_BAT_OUT;
                //Device_Voice_Apo(SOUND_CNT_TIME_1S , SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_DI);
              }
              break;
            case DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_NOINSER:
              if(g_sGateOpInfo.batAddr != GATE_BAT_NULL)
              {
                   g_sGateOpInfo.batAddr = GATE_BAT_NULL;
              }
              break;
            case DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_FAIL:
             
              if(g_sGateOpInfo.batAddr != GATE_BAT_FAIL)
              {
                g_sGateOpInfo.batAddr = GATE_BAT_FAIL;
                Device_Voice_Apo(SOUND_CNT_TIME_1S * 8, SOUND_REPAT_NULL, SOUND_VOICE_BAT_SN_FAIL, SOUND_VOC_BATTER_CODE_FAIL);
              }
              break;
          case DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_LOADING:
              if(g_sGateOpInfo.batAddr != GATE_BAT_NULL)
              {
                   g_sGateOpInfo.batAddr = GATE_BAT_NULL;
              }
              break;
          }
      }
  }
  else if(mode == DEVICE_BAT_BRW)
  {
       if(step == DEVICE_STEP_OVER)
      {
          switch(flag)
          {
            case DEVICE_STEP_FLAG_OK:
                Device_Voice_Apo(SOUND_CNT_TIME_1S , SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_DI);
                  //借电池完成
              break;
            case DEVICE_STEP_FLAG_DOOR_FAIL:
              
              break;
            case DEVICE_STEP_FLAG_BAT_NO_RTN:
              //Device_Voice_Apo(SOUND_CNT_TIME_1S * 2, SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_DI);
              
              break;
            case DEVICE_STEP_FLAG_BAT_FRAME_ERR:
              
              break;
          }
        
      }
      else if(step == DEVICE_STEP_ONE)
      {
          switch(flag)
          {
            case DEVICE_STEP_FLAG_DOOR_OK:
              
              Device_Voice_Apo(SOUND_CNT_TIME_1S * 8, SOUND_REPAT_NULL, SOUND_VOICE_BRW_BAT, g_sGateOpInfo.add + 1);
              break;
            case DEVICE_STEP_FLAG_DOOR_LOADING:
              
              break;
          }
      }
      else if(step == DEVICE_STEP_TWICE)  
      {
          switch(flag)
          {
            case DEVICE_STEP_FLAG_DOOR_NOCLOSE_BAT_INSER:
              
              if(g_sGateOpInfo.batAddr != GATE_BAT_IN)
              {
                g_sGateOpInfo.batAddr = GATE_BAT_IN;
                //Device_Voice_Apo(SOUND_CNT_TIME_1S * 2, SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_DI);
              }
              break;
            case DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_NOINSER:
              if(g_sGateOpInfo.batAddr != GATE_BAT_NULL)
              {
                   g_sGateOpInfo.batAddr = GATE_BAT_NULL;
              }
             
              break;
            case DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_FAIL:
              
              break;
          case DEVICE_STEP_FLAG_DOOR_CLOSE_BAT_LOADING:
              if(g_sGateOpInfo.batAddr != GATE_BAT_NULL)
              {
                   g_sGateOpInfo.batAddr = GATE_BAT_NULL;
              }
              break;
          }
      }
  
  }

}

BOOL Device_CheckRsp(W232_CONNECT *pCntOp, u8 *pRxBuf, u8 len)
{
    BOOL bOK = FALSE;
    char strBuffRequst[W232_STR_BUFFER_RSP_LEN] = {0};
    char strBuffJsonAccept[W232_STR_BUFFER_RSP_LEN] = {0};
    char strBuffJsonReject[W232_STR_BUFFER_RSP_LEN] = {0};
    char strBuffcmdAccept[W232_STR_BUFFER_RSP_LEN] = {0};
    char strBuffcmdOta[W232_STR_BUFFER_RSP_LEN] = {0};
    
    sprintf(strBuffRequst,"$sys/%.6s/%.15s/cmd/request/",W232_PRDOCT_ID,g_nImsiStr);
    sprintf(strBuffJsonAccept,"$sys/%.6s/%.15s/dp/post/json/accepted",W232_PRDOCT_ID, g_nImsiStr);
    sprintf(strBuffJsonReject,"$sys/%.6s/%.15s/dp/post/json/rejected",W232_PRDOCT_ID, g_nImsiStr);
    sprintf(strBuffcmdAccept,"$sys/%.6s/%.15s/cmd/response/%.36s/accepted", W232_PRDOCT_ID, g_nImsiStr,pCntOp->requestId);
    sprintf(strBuffcmdOta,"$sys/%.6s/%.15s/ota/inform ", W232_PRDOCT_ID, g_nImsiStr);
    if(strstr((char const *)pRxBuf, strBuffRequst) != NULL)
    {
        memcpy(pCntOp->requestId, pRxBuf + W232_RQUEST_ID_POS, W232_RQUEST_ID_LEN);
        if((*(pCntOp->requestId +  DEVICE_MQTT_FRAME_CMDID_TAG_1) == DEVICE_MQTT_FRAME_CMDID_MASK) &&        //过滤损坏数据帧，否则被服务器踢出
           (*(pCntOp->requestId +  DEVICE_MQTT_FRAME_CMDID_TAG_2) == DEVICE_MQTT_FRAME_CMDID_MASK) && 
           (*(pCntOp->requestId +  DEVICE_MQTT_FRAME_CMDID_TAG_3) == DEVICE_MQTT_FRAME_CMDID_MASK) && 
           (*(pCntOp->requestId +  DEVICE_MQTT_FRAME_CMDID_TAG_4) == DEVICE_MQTT_FRAME_CMDID_MASK))
        {
            bOK = TRUE;
            g_sW232RcvBuffer.flag = W232_RESPONES_CMD_GET;
             memcpy(pCntOp->requestBuffer, pRxBuf + W232_RQUEST_ID_POS + W232_RQUEST_ID_LEN + 3, W232_RQUEST_BUFFER_LEN);
            W232_DataHandle(&g_sW232RcvBuffer, pRxBuf + W232_RQUEST_ID_POS + W232_RQUEST_ID_LEN + 3);
        }
        else
        {
            bOK = FALSE;
        }

    }
    else if(strstr((char const *)pRxBuf, strBuffJsonAccept) != NULL)
    {
        bOK = TRUE;
        g_sDeviceParams.offLineTime = 0;
        g_sW232RcvBuffer.flag = W232_RESPONES_CMD_RESP;

    }
    else if(strstr((char const *)pRxBuf, strBuffJsonReject) != NULL)
    {
        bOK = FALSE;
        g_sW232RcvBuffer.flag = W232_RESPONES_CMD_RESP;

    }
    else if(strstr((char const *)pRxBuf, strBuffcmdAccept) != NULL)
    {
        bOK = TRUE;
        g_sW232RcvBuffer.flag = W232_RESPONES_CMD_RESP;

    }
    else if(strstr((char const *)pRxBuf, strBuffcmdOta) != NULL)
    {
        //更新通知
        bOK = TRUE;
        g_sW232RcvBuffer.flag = W232_RESPONES_CMD_RESP;
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
        Fram_WriteBootParamenter();
        Reader_Delayms(5);
        Sys_SoftReset();

    }
   
    return bOK;
}



char g_nMqttAtBuf[W232_STR_BUFFER_HEART_LEN] = {0};
char g_nMqttBuf[W232_STR_BUFFER_HEART_LEN] = {0};

void Device_CommunTxCmd(DEVICE_SENVER_TXBUFFER *pCntOp, u32 sysTick)
{
    u8 op = 0;

    op = pCntOp->op[pCntOp->index];
    
    switch(op)
    {
        case W232_MQTT_TOPIC_HEART:

            pCntOp->len = Device_HeartFormat(pCntOp->buffer, g_nSysTick);
            Device_UpHeartData(sysTick,pCntOp->buffer, g_nMqttAtBuf, g_nMqttBuf, pCntOp->len);
            W232_WriteCmd(g_nMqttAtBuf);
            break;
        case W232_MQTT_TOPIC_REBAT:

            Device_RtuRsp(g_nDeviceImpRspInfo.rtuBuffer, g_nMqttAtBuf, g_nMqttBuf, g_sGateOpInfo.add, g_nSysTick);
            W232_WriteCmd(g_nMqttAtBuf);
            break;
        case W232_MQTT_TOPIC_BWBAT:
          
            Device_BrwRsp(g_nDeviceImpRspInfo.brwBuffer, g_nMqttAtBuf, g_nMqttBuf, g_sGateOpInfo.add, g_nSysTick);
            W232_WriteCmd(g_nMqttAtBuf);
            break;
        case W232_MQTT_TOPIC_WARN:
          
            Device_InfoChgRsp(g_nDeviceImpRspInfo.warnBuffer, g_nMqttAtBuf, g_nMqttBuf, g_nDeviceImpRspInfo.add, g_nSysTick);
            W232_WriteCmd(g_nMqttAtBuf);
            break;
        case W232_MQTT_TOPIC_CMD:
          
            Device_PostRsp(&g_sW232Connect,g_sDeviceRspFrame.buffer, g_nMqttAtBuf, g_nMqttBuf, g_sDeviceRspFrame.len);
            W232_WriteCmd(g_nMqttAtBuf);
            //memset(&g_sDeviceRspFrame, 0, sizeof(READER_RSPFRAME));
            break;
            
    }
    pCntOp->tick = sysTick;
}


void Device_PostRsp(W232_CONNECT *pCntOp0,u8 *pBuffer, char *strAtBuff, char *strRspBuff,u16 len)
{  
    char strRspbuffer[W232_STR_BUFFER_LEN] ={0};
    char strRsplen[4] = {0};
    u8 hexLen[2] = {0};
      
    hexLen[0] = (len & 0xFF >> 8);
    hexLen[1] = (len & 0xFF >> 0); 
    a_Hex2Str(strRspbuffer, pBuffer, len);
    a_Hex2Str(strRsplen, hexLen, 2);
    sprintf(strRspBuff,"{\"id\":%.8s,\"dp\":{\"device\":[{\"v\":\"%.2s%.2s%.4s%s\"}]}}", g_sW232RcvBuffer.idStr, g_sW232RcvBuffer.addStr, g_sW232RcvBuffer.cmdStr, strRsplen, strRspbuffer);
    sprintf(strAtBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/cmd/response/%.36s\",%d",W232_PRDOCT_ID,g_nImsiStr, pCntOp0->requestId, strlen(strRspBuff));
    
}


BOOL Device_CommunCheckRsp(DEVICE_SENVER_TXBUFFER *pCntOp, u8 *pRxBuf)
{
    u8 op = 0;

    BOOL bOK = FALSE;
    op = pCntOp->op[pCntOp->index];

    switch(op)
    {
        case W232_MQTT_TOPIC_HEART:
            if(strstr((char const *)pRxBuf, ">") != NULL)
            {
                bOK = TRUE;
                W232_WriteCmd(g_nMqttBuf);
            }
            break;
        case W232_MQTT_TOPIC_REBAT:
           if(strstr((char const *)pRxBuf, ">") != NULL)
            {
                bOK = TRUE;
                W232_WriteCmd(g_nMqttBuf);

            }
            break;
        case W232_MQTT_TOPIC_BWBAT:
            if(strstr((char const *)pRxBuf, ">") != NULL)
            {
                bOK = TRUE;
                W232_WriteCmd(g_nMqttBuf);
            }
            break;
        case W232_MQTT_TOPIC_WARN:
            if(strstr((char const *)pRxBuf, ">") != NULL)
            {
                bOK = TRUE;
                W232_WriteCmd(g_nMqttBuf);
            }
            break;
        case W232_MQTT_TOPIC_CMD:
            if(strstr((char const *)pRxBuf, ">") != NULL)
            {
                bOK = TRUE;
                W232_WriteCmd(g_nMqttBuf);
            }
            break;
    }
    memset(g_nMqttBuf, 0, W232_STR_BUFFER_HEART_LEN);
    
    return bOK;
}



void Device_CommunStep(DEVICE_SENVER_TXBUFFER *pCntOp)
{
    u8 op = 0;
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case W232_MQTT_TOPIC_HEART:
        case W232_MQTT_TOPIC_REBAT:
        case W232_MQTT_TOPIC_BWBAT:
        case W232_MQTT_TOPIC_WARN:
        case W232_MQTT_TOPIC_CMD:
            if(pCntOp->result == W232_CNT_RESULT_OK)
            {
                pCntOp->repeat[pCntOp->index] = 0;
                pCntOp->index++;
            }
            else
            {
                pCntOp->repeat[pCntOp->index] = 0;
                pCntOp->index++;
                
                //上传失败，离线存储处理
            }
            break;
    }
}


void Device_RtuRsp(u8 *pBuffer, char *strAtBuff, char *strRspBuff, u8 addr, u32 id)
{  
    char strRspbuffer[W232_STR_BUFFER_LEN] ={0};
    char strRspAdd[2] = {0};
    u8 hexAdd[1] = {0};
    
    hexAdd[0] = addr + 1;
    a_Hex2Str(strRspAdd, hexAdd, 1);
    a_Hex2Str(strRspbuffer, pBuffer, 7);
    sprintf(strRspBuff,"{\"id\":%.8d,\"dp\":{\"rebat\":[{\"v\":\"%.2s3A0007%.14s\"}]}}", id, strRspAdd, strRspbuffer);
    sprintf(strAtBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/dp/post/json\",%d",W232_PRDOCT_ID, g_nImsiStr, strlen(strRspBuff));

}

void Device_BrwRsp(u8 *pBuffer, char *strAtBuff, char *strRspBuff, u8 addr, u32 id)
{  
    char strRspbuffer[W232_STR_BUFFER_LEN] ={0};
    char strRspAdd[2] = {0};
    u8 hexAdd[1] = {0};
    
    hexAdd[0] = addr + 1;
    a_Hex2Str(strRspAdd, hexAdd, 1);
    a_Hex2Str(strRspbuffer, pBuffer, 7);
    sprintf(strRspBuff,"{\"id\":%.8d,\"dp\":{\"bwbat\":[{\"v\":\"%.2s3B0007%.14s\"}]}}", id, strRspAdd, strRspbuffer);
    sprintf(strAtBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/dp/post/json\",%d",W232_PRDOCT_ID, g_nImsiStr, strlen(strRspBuff));
    
}

void Device_InfoChgRsp(u8 *pBuffer, char *strAtBuff, char *strRspBuff, u8 addr, u32 id)
{  
    char strRspbuffer[W232_STR_BUFFER_LEN] ={0};
    char strRspAdd[2] = {0};
    u8 hexAdd[1] = {0};
    
    hexAdd[0] = addr + 1;
    a_Hex2Str(strRspAdd, hexAdd, 1);
    a_Hex2Str(strRspbuffer, pBuffer, 9);
    sprintf(strRspBuff,"{\"id\":%.8d,\"dp\":{\"state\":[{\"v\":\"%.2s220009%.14s\"}]}}", id, strRspAdd, strRspbuffer);
    sprintf(strAtBuff,"AT+QMTPUBEX=0,0,0,0,\"$sys/%.6s/%.15s/dp/post/json\",%d",W232_PRDOCT_ID, g_nImsiStr, strlen(strRspBuff));

}


void Device_Gate_StateChk(u8 index)
{ 

    if(index == DEVICE_SM5001_ID)
    {
        if(memcmp(&(g_sIoInfo.tempState), &(g_sIoInfo.state), sizeof(u32)))
       {
            memcpy(&(g_sIoInfo.tempState), &(g_sIoInfo.state), sizeof(u32));
            g_nDeviceImpRspInfo.add = DEVICE_SM5001_ID - 1;
            g_nDeviceImpRspInfo.warnLen = Device_ResponseInfoChg(g_nDeviceImpRspInfo.warnBuffer, DEVICE_SM5001_ID,0, 0, 0, g_sIoInfo.tempState);
            Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_WARN);
       }
      
    }
    else
    {
       if(memcmp(&(g_aGateSlvStat[index].sensorState), &(g_aGateSlvInfo[index].sensorInfo.sensorState), sizeof(GATE_STATINFO)) ||
          memcmp(&(g_aGateSlvStat[index].batState), &(g_aGateSlvInfo[index].sensorInfo.batInfo.state), sizeof(u8)) ||
          memcmp(&(g_aGateSlvStat[index].chagState), &(g_aGateSlvInfo[index].sensorInfo.chagInfo.state), sizeof(u8)))     //状态变化
       {
          memcpy(&(g_aGateSlvStat[index].sensorState), &(g_aGateSlvInfo[index].sensorInfo.sensorState), sizeof(GATE_STATINFO));
          memcpy(&(g_aGateSlvStat[index].batState), &(g_aGateSlvInfo[index].sensorInfo.batInfo.state), sizeof(u8));
          memcpy(&(g_aGateSlvStat[index ].chagState), &(g_aGateSlvInfo[index].sensorInfo.chagInfo.state), sizeof(u8));
          g_nDeviceImpRspInfo.add = index ;
          g_nDeviceImpRspInfo.warnLen = Device_ResponseInfoChg(g_nDeviceImpRspInfo.warnBuffer, DEVICE_SM5002_ID,(g_aGateSlvStat[index].sensorState.fan << 3) | 
                                                                                                              (g_aGateSlvStat[index].sensorState.smoke << 2) | 
                                                                                                               (g_aGateSlvStat[index].sensorState.rfid << 1) | 
                                                                                                               (g_aGateSlvStat[index].sensorState.door << 0), g_aGateSlvStat[index].batState, g_aGateSlvStat[index].chagState, 0);
          Device_At_Rsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_WARN);
       }
    }
   


}


void Device_VoiceCtr()
{
    if(g_sIoInfo.state & IO_STAT_FIRE)
    {
      if(!a_CheckStateBit(g_sSoundInfo.state, SOUND_STAT_WAIT))
      {
          Device_Voice_Apo(SOUND_CNT_TIME_1S * 3, SOUND_REPAT_NULL, SOUND_VOICE_FIRE_WRAN, SOUND_VOC_FIRE_WARN);
      }
    }
}





u8 Device_UrlEncode(char *sign)
{

	char sign_t[40];
	unsigned char i = 0, j = 0;
	unsigned char sign_len = strlen(sign);
	
	if(sign == (void *)0 || sign_len < 28)
		return 1;
	
	for(; i < sign_len; i++)
	{
		sign_t[i] = sign[i];
		sign[i] = 0;
	}
	sign_t[i] = 0;
	
	for(i = 0, j = 0; i < sign_len; i++)
	{
		switch(sign_t[i])
		{
			case '+':
				strcat(sign + j, "%2B");j += 3;
			break;
			
			case ' ':
				strcat(sign + j, "%20");j += 3;
			break;
			
			case '/':
				strcat(sign + j, "%2F");j += 3;
			break;
			
			case '?':
				strcat(sign + j, "%3F");j += 3;
			break;
			
			case '%':
				strcat(sign + j, "%25");j += 3;
			break;
			
			case '#':
				strcat(sign + j, "%23");j += 3;
			break;
			
			case '&':
				strcat(sign + j, "%26");j += 3;
			break;
			
			case '=':
				strcat(sign + j, "%3D");j += 3;
			break;
			
			default:
				sign[j] = sign_t[i];j++;
			break;
		}
	}
	
	sign[j] = 0;
	
	return 0;

}





#define METHOD		"sha1"
u8 Device_Ota_Token(char *ver, char *res, unsigned int et, char *access_key, char *authorization_buf, unsigned short authorization_buf_len)
{
	
	size_t olen = 0;
	
	char sign_buf[40];							//保存签名的Base64编码结果 和 URL编码结果
	char hmac_sha1_buf[40];						//保存签名
	char access_key_base64[40];					//保存access_key的Base64编码结合
	char string_for_signature[56];				//保存string_for_signature，这个是加密的key

//----------------------------------------------------参数合法性--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------将access_key进行Base64解码----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
	//UsartPrintf(USART_DEBUG, "access_key_base64: %s\r\n", access_key_base64);
	
//----------------------------------------------------计算string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	//UsartPrintf(USART_DEBUG, "string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------加密-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
	//UsartPrintf(USART_DEBUG, "hmac_sha1_buf: %s\r\n", hmac_sha1_buf);
	
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
	Device_UrlEncode(sign_buf);
	//UsartPrintf(USART_DEBUG, "sign_buf: %s\r\n", sign_buf);
	
//----------------------------------------------------计算Token--------------------------------------------------------------------
	snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	//UsartPrintf(USART_DEBUG, "Token: %s\r\n", token_buf);
	
	return 0;

}
