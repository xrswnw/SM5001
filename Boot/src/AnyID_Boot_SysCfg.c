#include "AnyID_Boot_SysCfg.h"

u32 g_nSysState = 0;
u32 g_nDeviceNxtEraseAddr = 0;
u32 g_nDeviceNxtDownloadAddr = 0;
u8  g_nDeviceComType = 0;

#define SYS_BOOT_VER_SIZE               50
const u8 SYS_BOOT_VERSION[SYS_BOOT_VER_SIZE] = "Boot V3.1_22032600 GD32F3xx";

void Sys_Delayms(u32 n)             //系统延时n毫秒
{
    n *= 0x6000;
    n++;
    while(n--);
}

void Sys_CfgClock(void)
{
    ErrorStatus HSEStartUpStatus;

    RCC_DeInit();
    //Enable HSE
    RCC_HSEConfig(RCC_HSE_ON);

    //Wait till HSE is ready
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if(HSEStartUpStatus == SUCCESS)
    {
        //HCLK = SYSCLK = 72.0M
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        //PCLK2 = HCLK = 72.0M
        RCC_PCLK2Config(RCC_HCLK_Div1);

        //PCLK1 = HCLK/2 = 33.9M
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //ADCCLK = PCLK2/2
        RCC_ADCCLKConfig(RCC_PCLK2_Div8);

        // Select USBCLK source 72 / 1.5 = 48M
        RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

        //Flash 2 wait state
        FLASH_SetLatency(FLASH_Latency_2);

        //Enable Prefetch Buffer
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        //PLLCLK = 12.00MHz * 10 = 120 MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_10);    //PLL在最后设置

        //Enable PLL
        RCC_PLLCmd(ENABLE);

        //Wait till PLL is ready
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        //Select PLL as system clock source
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        //Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
}

void Sys_CfgPeriphClk(FunctionalState state)
{
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                          RCC_APB2Periph_GPIOB |
                          RCC_APB2Periph_GPIOC |
                          RCC_APB2Periph_GPIOD |
                          RCC_APB2Periph_AFIO, state);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | 
                           RCC_APB1Periph_USART3 |
                           RCC_APB1Periph_UART5  , state);

}

void Sys_CfgNVIC(void)
{
    //NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM
    //Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  //VECT_TAB_FLASH
    //Set the Vector Table base location at 0x08000000
#ifdef _ANYID_BOOT_STM32_
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
#else
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
#endif

    //Configure the Priority Group to 2 bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

const PORT_INF SYS_RUNLED_COM = {GPIOB, GPIO_Pin_1};

void Sys_CtrlIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = SYS_RUNLED_COM.Pin;
    GPIO_Init(SYS_RUNLED_COM.Port, &GPIO_InitStructure);

}



void Sys_Init(void)
{
#if SYS_ENABLE_WDT
    WDG_InitIWDG();
#endif
    //
    Sys_CfgClock();

    Sys_CfgNVIC();
    Sys_CfgPeriphClk(ENABLE);

    //禁止中断
    Sys_DisableInt();
    Sys_CtrlIOInit();
    Sys_RunLedOn();

    FRam_InitInterface();
    Fram_ReadBootParamenter();
    
    Flash_InitInterface();
    Flash_Init();
    
    //
    Flash_ReadId();
    Flash_Demo();

   g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;//测试
    

    Uart_InitInterface(UART_BAUDRARE);
    Uart_ConfigInt();
    Uart_EnableInt(ENABLE, DISABLE);
    
    EC20_Init();
    Sys_Delayms(5);
    /*
    memcpy(g_nSoftWare, (u8 *)(SYS_BOOT_VER_ADDR ), EC20_SOFTVERSON_LEN);
    
    memcpy(g_sFramBootParamenter.verSion, g_nSoftWare, EC20_SOFTVERSON_LEN);
    Fram_WriteBootParamenter();      */
    
    //SysTick 初始化 5ms
    STick_InitSysTick();
    Sys_RunLedOff();

    EC20_ConnectInit(&g_sEC20Connect, EC20_CNT_CMD_PWRON, &g_sEC20Params);
    a_SetState(g_sEC20Connect.state, EC20_CNT_OP_STAT_TX);
 
    //使能中断
    Sys_EnableInt();
}

void Sys_LedTask(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_RUNLED))
    {
        static u32 ledTimes = 0;

        a_ClearStateBit(g_nSysState, SYS_STAT_RUNLED);

        ledTimes++;

        if(ledTimes & 0x01)
        {
            Sys_RunLedOff();

        }
        else
        {
            Sys_RunLedOn();

        }

    #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    #endif
    }
}

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
void Sys_Jump(u32 address)
{
    u32 stackAddr = 0;
    Sys_DisableInt();
    stackAddr = *((vu32 *)address);
    //查看栈地址是否在RAM区或者CCRAM区
    if((stackAddr & 0x2FFE0000) == 0x20000000)
    {
        JumpAddress = *(vu32 *)(address + 4);
        Jump_To_Application = (pFunction) JumpAddress;

        __set_MSP(*(vu32 *)address);
        Jump_To_Application();
    }
    else
    {
        a_SetState(g_nSysState, SYS_STAT_IDLE);
        g_nSysTick = 0;
    }
    //while(1)
    {
    #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    #endif
    }
    Sys_EnableInt();
}

void Sys_BootTask(void)
{
    static u32 bootState =0;
    
    if(bootState != g_sFramBootParamenter.appState)
    {
        if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_OK  || g_sFramBootParamenter.appState == FRAM_BOOT_APP_NULL_REPLACE)
        {
            a_SetState(g_nSysState, SYS_STAT_IDLE);
        }
        else if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_DATA_DOWD || g_sFramBootParamenter.appState == FRAM_BOOT_APP_FAIL)
        {
            Device_Version_UpData();
            a_SetStateBit(g_nSysState, SYS_STAT_DOWNLOAD);
            g_sDeviceUpDataInfo.flag = DEVICE_UPDATA_FLAG_RQ;
        }
        else if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_DATA_DOWN_OVER)
        {
            a_SetStateBit(g_nSysState, SYS_STAT_CHK_VERSION);
        }
        else if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_REPLACE )
        {
            Device_Version_UpData();
            a_SetStateBit(g_nSysState, SYS_STAT_REPLACE_DATA);
            g_nDeviceNxtEraseAddr = SYS_APP_START_ADDR;
            FLASH_Unlock();
        }
        else if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_REPLACE_OVER)
        {
            //g_sDeviceUpDataInfo.tF = FALSE;
          Device_Version_UpData();
          a_SetStateBit(g_nSysState, SYS_STAT_INFORM_INFO);
        }
        
        bootState = g_sFramBootParamenter.appState;
        Fram_WriteBootParamenter();
    }
    
    if(a_CheckStateBit(g_nSysState, SYS_STAT_IDLE))
    {
        if(g_nSysTick > 40)               
        {
            a_SetStateBit(g_nSysState, SYS_STAT_JMP);
        }
    }

    if(a_CheckStateBit(g_nSysState, SYS_STAT_JMP))
    {
       Sys_RunLedOff();
       a_ClearStateBit(g_nSysState, SYS_STAT_JMP);
       if(Sys_CheckVersion())
       {
          if(a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU))
          {
              Device_At_Rsp(EC20_CNT_TIME_1S, EC20_CNT_REPAT_NULL, DEVICE_HTTP_GET_REQUEST_CKECK);
          }
          Sys_Jump(SYS_APP_START_ADDR);
       }
       else
       {
          g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
       }
    }
    
    if(a_CheckStateBit(g_nSysState, SYS_STAT_CHK_VERSION))
    {
       a_ClearStateBit(g_nSysState, SYS_STAT_CHK_VERSION);
       
       if(Device_Chk_Version())                                            //检查是否需要更新
       {
            g_sFramBootParamenter.appState = FRAM_BOOT_APP_REPLACE;
            if(g_sFramBootParamenter.flag == DEVICE_TYPE_SM5002 || g_sFramBootParamenter.flag == DEVICE_TYPE_SM5003)
            {
              g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
            }
       }
       else
       {
            g_sFramBootParamenter.appState = FRAM_BOOT_APP_NULL_REPLACE;
       }
    }
}

void Sys_UartTask(void)
{
    //串口错误处理:重新初始化
    if(USART_GetFlagStatus(UART_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(UART_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        Uart_InitInterface(UART_BAUDRARE);
        Uart_ConfigInt();
        Uart_EnableInt(ENABLE, DISABLE);
    }
   
    //串口数据帧解析
    /*
    if(Uart_IsRcvFrame(g_sUartRcvFrame))
    {
        Sys_ProcessBootFrame(&g_sUartRcvFrame, g_nDeviceComType);
        Uart_ResetFrame(g_sUartRcvFrame);
    }
*/
}

BOOL Sys_CheckVersion(void)
{
    BOOL b = FALSE;
    u8 *p = (u8 *)SYS_BOOT_VER_ADDR;
    u8 i = 0, c = 0;
        
    if(memcmp(p, SYS_VER_HEAD, SYS_VER_HEAD_SIZE) == 0) //设备型号正确
    {
        for(i = SYS_VER_HEAD_SIZE; i < SYS_VERSION_SIZE; i++)
        {
            c = *p++;
            if((c < ' ' || c > 127) && (c != 0x00))
            {
                break;
            }
        }
        if(i == SYS_VERSION_SIZE)
        {
            b = TRUE;
        }
    }
    
    
    
    if(b == FALSE)
    {
        if(memcmp((u8 *)SYS_BOOT_HARDTYPE_ADDR, SYS_VER_HARD_TYPE, sizeof(SYS_VER_HARD_TYPE) - 1) == 0) //设备硬件型号正确也可以
        {
            b = TRUE;
        }
    }
    
    return b;
}

void Sys_ProcessBootFrame(UART_RCVFRAME *pRcvFrame, u8 com)
{
  
    u16 crc1 = 0, crc2 = 0;
    memcpy(&g_sUartTempRcvFrame, pRcvFrame, sizeof(UART_RCVFRAME));


    crc1 = Uart_GetFrameCrc(g_sUartTempRcvFrame.buffer, g_sUartTempRcvFrame.index);
    crc2 = a_GetCrc(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_LEN, g_sUartTempRcvFrame.index - 4);

    if(crc1 == crc2)
    {
        u8 cmd = g_sUartTempRcvFrame.buffer[UART_FRAME_POS_CMD];
        u16 destAddr = 0;
        
        destAddr = *((u16 *)(pRcvFrame->buffer + UART_FRAME_POS_DESTADDR));
        if((destAddr != SYS_FRAME_BROADCAST_ADDR) && (destAddr != g_sFramBootParamenter.addr))
        {
            return;
        }
        switch(cmd)
        {
            case UART_FRAME_CMD_RESET:
                g_nSysTick = 0;
                g_sUartTxFrame.len = Uart_RspReset();
                break;
            case UART_FRAME_CMD_ERASE:
                if(a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
                {
                    BOOL bOk = FALSE;
                    u32 addr = 0;
                    u8 sector = 0;
                    
                    sector = g_sUartTempRcvFrame.buffer[UART_FRAME_POS_PAR];
                    addr = SYS_APP_START_ADDR + (sector << 10);         //每个扇区1K
                    
                    if(addr >= SYS_APP_START_ADDR)
                    {
                        if(g_nDeviceNxtEraseAddr == addr)               //擦除地址必须是连续的，否则会有区域未擦除
                        {
                            g_nDeviceNxtEraseAddr = addr + (1 << 10);   //每个扇区1K
                            
                            bOk = Uart_EraseFlash(addr); 
                            g_sUartTxFrame.len = Uart_RspErase(bOk);
                        }
                    }
                }
                break;
            case UART_FRAME_CMD_DL:
                if(a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
                {
                    BOOL bCheck = FALSE;
                    u32 addr = 0;
                    u32 size = 0;

                    if(g_sUartTempRcvFrame.buffer[UART_FRAME_POS_LEN] == 0x00)
                    {
                        bCheck = (BOOL)(g_sUartTempRcvFrame.buffer[UART_FRAME_POS_PAR + 0]);
                        addr = *((u32 *)(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 1));
                        size = *((u32 *)(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 5));
                        if(addr >= SYS_APP_START_ADDR)
                        {
                            //第一次不需要判定地址连续问题，因为boot程序是从后向前写数据，第一次不知道地址是什么
                            if(addr + size == g_nDeviceNxtDownloadAddr || g_nDeviceNxtDownloadAddr == 0)
                            {
                                g_nDeviceNxtDownloadAddr = addr;
                                //帧参数之前部分 + 基本参数(1 + 4 + 4) + size + crclen;
                                //frameLen = UART_FRAME_POS_PAR + 9 + size + 2;
                                Sys_RunLedOn();
                                if(BL_WriteImagePage(addr, g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 9, size))
                                {
                                    g_sUartTxFrame.len = Uart_RspDownload(bCheck, addr, size);
                                }

                                Sys_RunLedOff();

                            }
                        }
                    }
                }                       
                break;
            case UART_FRAME_CMD_BOOT:
                g_sUartTxFrame.len = Uart_RspBoot();
                if(g_sUartTxFrame.len)      //下面擦除操作消耗时间较长
                {

                        Uart_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);

                   Sys_Delayms(2);         //等待最后一个字节发送完成
                   g_sUartTxFrame.len = 0;
                }
                a_ClearStateBit(g_nSysState, SYS_STAT_IDLE);
                a_SetStateBit(g_nSysState, SYS_STAT_DOWNLOAD);
                g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
                Fram_WriteBootParamenter();
                Fram_WriteBackupBootParamenter();
                FLASH_Unlock();
                
                Uart_EraseFlash(SYS_BOOT_VER_ADDR);          //版本信息区域擦除
                g_nDeviceNxtEraseAddr = SYS_APP_START_ADDR;
                g_nDeviceNxtDownloadAddr = 0;                   //boot是由后向前写入数据
                
                break;
            case UART_FRAME_CMD_JMP:
                if(Sys_CheckVersion() == TRUE)
                {
                    g_sUartTxFrame.len = Uart_RspJmp();
                    a_SetStateBit(g_nSysState, SYS_STAT_JMP);
                    g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
                    Fram_WriteBootParamenter();
                    Fram_WriteBackupBootParamenter();
                    FLASH_Lock();
                }
                break;
            case UART_FRAME_CMD_VER:
                g_sUartTxFrame.len = Uart_RspFrame(g_sUartTxFrame.frame, cmd, (u8 *)SYS_BOOT_VERSION, SYS_BOOT_VER_SIZE, UART_FRAME_FLAG_OK, UART_FRAME_RSP_NOERR);
                break;
            default:
                break;
        }
    }
    if(g_sUartTxFrame.len)
    {
        Uart_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
        Sys_Delayms(2);         //等待最后一个字节发送完成
        g_sUartTxFrame.len = 0;
    }

}





void Sys_EC20Task(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU))
    {
        return; //只有不是透传模式，才需要按照AT指令解析
    }

    if(USART_GetFlagStatus(EC20_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(EC20_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        EC20_InitInterface(EC20_BAUDRARE);
        EC20_ConfigInt();
        EC20_EnableInt(ENABLE, DISABLE);
    }

    if(Uart_IsRcvFrame(g_sEC20RcvFrame))
    {
        Uart_WriteBuffer(g_sEC20RcvFrame.buffer, g_sEC20RcvFrame.index);
        if(a_CheckStateBit(g_sEC20Connect.state, EC20_CNT_OP_STAT_RX))
        {
            if(EC20_ConnectCheckRsp(&g_sEC20Connect, g_sEC20RcvFrame.buffer, g_sEC20RcvFrame.index))   //如果校验响应帧失败，就继续接收，否则复位接收缓冲区
            {
                g_sEC20Connect.result = EC20_CNT_RESULT_OK;
                a_SetState(g_sEC20Connect.state, EC20_CNT_OP_STAT_STEP);
                
                EC20_ClearRxBuffer();
            }
            else
            {
                g_sEC20RcvFrame.state = UART_FLAG_RCV;                          //继续接收
                g_sEC20RcvFrame.idleTime = 0;
            }
        }
    }
    
    //发送
    if(a_CheckStateBit(g_sEC20Connect.state, EC20_CNT_OP_STAT_TX))      //发送AT指令
    {
        a_ClearStateBit(g_sEC20Connect.state, EC20_CNT_OP_STAT_TX);

        if(g_sEC20Connect.index < g_sEC20Connect.num)
        {
            EC20_ClearRxBuffer();
            EC20_ConnectTxCmd(&g_sEC20Connect, g_nSysTick);
            a_SetState(g_sEC20Connect.state, EC20_CNT_OP_STAT_RX | EC20_CNT_OP_STAT_WAIT);
        }
    }

    //超时
    if(a_CheckStateBit(g_sEC20Connect.state, EC20_CNT_OP_STAT_WAIT))    //判断超时，每条指令的超时时间都不一样
    {
        if(g_sEC20Connect.tick + g_sEC20Connect.to[g_sEC20Connect.index] < g_nSysTick)
        {
            g_sEC20Connect.repeat[g_sEC20Connect.index]--;              //有些指令允许多次操作
            if(g_sEC20Connect.repeat[g_sEC20Connect.index] == 0)
            {
                a_ClearStateBit(g_sEC20Connect.state, EC20_CNT_OP_STAT_WAIT);
                a_SetState(g_sEC20Connect.state, EC20_CNT_OP_STAT_STEP);
                g_sEC20Connect.result = EC20_CNT_RESULT_TO;             //当前操作超时，GPRS执行流程错误
            }
            else
            {
                g_sEC20Connect.state = EC20_CNT_OP_STAT_TX;             //重复发送
            }
        }
    }

    if(a_CheckStateBit(g_sEC20Connect.state, EC20_CNT_OP_STAT_STEP))    //下一步逻辑处理
    {
        a_ClearStateBit(g_sEC20Connect.state, EC20_CNT_OP_STAT_STEP);

        EC20_ConnectStep(&g_sEC20Connect);                              //操作步骤处理
        if(g_sEC20Connect.result == EC20_CNT_RESULT_OK)                 //所有流程执行完成，并且成功，表示进入DTU模式
        {
            g_sEC20Connect.state = EC20_CNT_OP_STAT_TX;
            if(g_sEC20Connect.index == g_sEC20Connect.num)              //如果是开机操作执行完成，表示进入DTU，或者关闭
            {
                a_ClearStateBit(g_nSysState, SYS_STAT_HTTP_TEST);
                if(g_sEC20Connect.cmd == EC20_CNT_CMD_PWRON)            
                {
                    g_sEC20Connect.state = EC20_CNT_OP_STAT_DTU;
                    a_SetStateBit(g_nSysState, SYS_STAT_LTEDTU);        //连接HTTP服务器成功
                    g_sEC20RcvBuffer.state = EC20_CNT_OP_STAT_TEST;
                }
                else if(g_sEC20Connect.cmd == EC20_CNT_CMD_PWROFF) 
                {

                    EC20_KeyLow();//关机指令执行完成，再次确认关闭
                    g_sEC20Connect.state = EC20_CNT_OP_STAT_IDLE;
                }
            }
        }
        else
        {
            if(g_sEC20Connect.cmd == EC20_CNT_CMD_PWRON)                //开机执行失败，请求关闭，这里不需要重连了，因为下次还是会重连的
            {
                EC20_ConnectInit(&g_sEC20Connect, EC20_CNT_CMD_PWROFF, &g_sEC20Params);
                a_SetState(g_sEC20Connect.state, EC20_CNT_OP_STAT_TX);
            }
            else                                                        //如果关机失败，就直接关闭电源
            {
                g_sEC20Connect.state = EC20_CNT_OP_STAT_IDLE;
                EC20_KeyLow();
            }
        }
    }
   
}



void Sys_ServerTask(void)
{
    if(!a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU))  //只有透传了，才需要进入该任务
    {
        return;
    }
    //串口错误处理:重新初始化

    if(USART_GetFlagStatus(EC20_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(EC20_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        EC20_InitInterface(EC20_BAUDRARE);
        EC20_ConfigInt();
        EC20_EnableInt(ENABLE, DISABLE);
    }
    
     if(Uart_IsRcvFrame(g_sEC20RcvFrame))
    {
        
       if(a_CheckStateBit(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_RX))
        {
            if(Device_CommunCheckRsp(&g_sDeviceServerTxBuf, g_sEC20RcvFrame.buffer))   
            {
                g_sDeviceServerTxBuf.result = EC20_CNT_RESULT_OK;
                a_SetState(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_STEP);
            }
            else
            {
                g_sDeviceServerTxBuf.result = EC20_CNT_RESULT_FAIL;
                g_sEC20RcvFrame.state = UART_FLAG_RCV;                          //继续接收
                g_sEC20RcvFrame.idleTime = 0;
            }
            g_sDeviceServerTxBuf.index++;   
        }
        if(EC20_ConnectCheckClose(g_sEC20RcvFrame.buffer))
        {
            EC20_ConnectInit(&g_sEC20Connect, EC20_CNT_CMD_PWROFF, &g_sEC20Params);
            a_SetState(g_sEC20Connect.state, EC20_CNT_OP_STAT_TX);
            a_ClearStateBit(g_nSysState, SYS_STAT_LTEDTU);
        }
        EC20_ClearRxBuffer();
    }
     
    if(a_CheckStateBit(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_TX))      
    {
        a_ClearStateBit(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_TX);

        if(g_sDeviceServerTxBuf.index < g_sDeviceServerTxBuf.num)
        {
            EC20_ClearRxBuffer();
            Device_CommunTxCmd(&g_sDeviceServerTxBuf, g_nSysTick);
            a_SetState(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_RX | DEVICE_SERVER_TXSTAT_WAIT);
        }
    }
    
    if(a_CheckStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_WAIT))    
     {
        if(g_sDeviceServerTxBuf.tick + g_sDeviceServerTxBuf.to[g_sDeviceServerTxBuf.index] < g_nSysTick)
        {
            g_sDeviceServerTxBuf.repeat[g_sDeviceServerTxBuf.index - 1]--;              
            if(g_sDeviceServerTxBuf.repeat[g_sDeviceServerTxBuf.index - 1] == 0)
            {
                a_ClearStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_WAIT);
                a_SetState(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_STEP);
                g_sDeviceServerTxBuf.result = EC20_CNT_RESULT_TO;             
            }
            else
            {
                g_sDeviceServerTxBuf.state = EC20_CNT_OP_STAT_TX;             
            }
        }
    }
   
  
     if(a_CheckStateBit(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_STEP))    //下一步逻辑处理
    {
        a_ClearStateBit(g_sDeviceServerTxBuf.state, EC20_CNT_OP_STAT_STEP);
        Device_CommunStep(&g_sDeviceServerTxBuf); 
        if(g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_FLAG_DOWNING)
        {
            if(g_sDeviceServerTxBuf.result == EC20_CNT_RESULT_OK)
            {
                if(Device_WeiteData(&g_sDeviceUpDataInfo))
                {
                    if(g_sDeviceUpDataInfo.step == g_sDeviceUpDataInfo.num)
                    {
                        g_sDeviceUpDataInfo.flag = DEVICE_UPDATA_FLAG_OVER;
                    }
                    else
                    {
                        g_sDeviceUpDataInfo.flag = DEVICE_UPDATA_FLAG_DOWN;
                        g_sDeviceUpDataInfo.step ++;
                    }
                }
                else
                {
                    g_sDeviceUpDataInfo.flag = DEVICE_UPDATA_FLAG_FAIL;
                    
                    //下载中途失败、、
                }
                
            }
        }

        if(g_sDeviceServerTxBuf.index >= g_sDeviceServerTxBuf.num)
        {
            g_sDeviceServerTxBuf.num = 0;   
            g_sDeviceServerTxBuf.index = 0;

        }
    }


}

void Sys_DownDataTask()
{   
    static u8 upTime = 0, upTick = 0, infromTick = 0, infromTime = 0; 
    if(a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU) && a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD)) 
    {
        if(a_CheckStateBit(g_nSysState, SYS_STAT_UPDATA))
        {
            if(upTime == DEVICE_UPDATA_CHK_TIME)
            {
                upTime = 0;
                if(g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_FLAG_RQ)
                {
                    upTick ++;
                    Device_At_Rsp(EC20_CNT_TIME_1S, EC20_CNT_REPAT_NULL, DEVICE_HTTP_GET_REQUEST_CKECK);
                }
                else if(g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_FLAG_DOWN)
                {   
                    g_sDeviceUpDataInfo.flag = DEVICE_UPDATA_FLAG_DOWNING;
                    Device_At_Rsp(EC20_CNT_TIME_1S * 2, EC20_CNT_REPAT_NULL, DEVICE_HTTP_GET_REQUEST_DOWNLOAD);
                }
                else if(g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_FLAG_FAIL)
                {   
                    g_sFramBootParamenter.appState = FRAM_BOOT_APP_DATA_DOWD;
                }
                else if(g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_FLAG_OVER)
                {   
                    a_ClearStateBit(g_nSysState, SYS_STAT_DOWNLOAD)  ;
                    g_sFramBootParamenter.appState = FRAM_BOOT_APP_DATA_DOWN_OVER;
                }
                else
                {
                   g_sDeviceUpDataInfo.flag = DEVICE_UPDATA_FLAG_RQ;
                }
            
            }
            else
            {
                upTime ++;
            }
            
            if(upTick == DEVICE_UPDATA_CHK_TIME)
            {
                upTick = 0;
                g_sFramBootParamenter.appState = FRAM_BOOT_APP_NULL_REPLACE;
            }
            a_ClearStateBit(g_nSysState, SYS_STAT_UPDATA);
        }
    }
    
    
     if(a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU) && a_CheckStateBit(g_nSysState, SYS_STAT_INFORM_INFO)) 
    {
        if(a_CheckStateBit(g_nSysState, SYS_STAT_UPDATA))
        {
            if(infromTick == DEVICE_UPDATA_CHK_TIME)
            {
                infromTick = 0;
                if(g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_FLAG_OVER || g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_FLAG_NULL )
                {
                    infromTime ++;
                    memcpy(g_nSoftWare, (u8 *)(SYS_BOOT_VER_ADDR ), EC20_SOFTVERSON_LEN);
                    Device_At_Rsp(EC20_CNT_TIME_1S, EC20_CNT_REPAT_NULL, DEVICE_HTTP_GET_REQUEST_CKECK);
                }
                else if(g_sDeviceUpDataInfo.flag == DEVICE_UPDATA_INFORM_OK)
                {
                    g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
                }
            }
            else
            {
                infromTick ++;
            }
            
            if(infromTime == DEVICE_UPDATA_CHK_TIME)
            {
                infromTime = 0;
                g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
            }
            a_ClearStateBit(g_nSysState, SYS_STAT_UPDATA);
        }
    }

}

void Sys_ReplaceDeviceTask()
{
    static u8 flashTime = 0, sector = 0; 
    u32 addr = 0;
    if(a_CheckStateBit(g_nSysState, SYS_STAT_REPLACE_DATA))  //固件下载完成，进入更新流程
    {
        if(g_sFramBootParamenter.flag == DEVICE_TYPE_SM5001)
        {
            if(a_CheckStateBit(g_nSysState, SYS_STAT_WR_RE_FLASH))
            {
               a_ClearStateBit(g_nSysState, SYS_STAT_WR_RE_FLASH);
               
               if(flashTime == 2)
               {
                    flashTime = 0;
                    if(g_sDeviceUpDataInfo.state == DEVICE_UPDATA_ERASE_FLASH)
                    {
                        addr = SYS_APP_START_ADDR + (sector << 10);
                        if(addr >= SYS_APP_START_ADDR)
                        {
                            if(g_nDeviceNxtEraseAddr == addr)               //擦除地址必须是连续的，否则会有区域未擦除
                            {
                                g_nDeviceNxtEraseAddr = addr + (1 << 10);   //每个扇区1K
                                if(addr <= SYS_APP_START_ADDR + (g_sFramBootParamenter.size + 1) *  (1 << 10))
                                {
                                    if(Uart_EraseFlash(addr))
                                    {
                                        sector++;
                                    }
                                    else
                                    {
                                        addr = 0;
                                        sector = 0;
                                        g_nDeviceNxtEraseAddr = 0;
                                        Sys_RunLedOn();   //擦除失败
                                        g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
                                    }
                                }
                                else
                                {
                                    addr = 0;
                                    sector = 0;
                                    g_nDeviceNxtEraseAddr = 0;
                                    g_sDeviceUpDataInfo.state = DEVICE_UPDATA_WRITE_FLASH;
                                }
                            }
                        }
                    }
                    else if(g_sDeviceUpDataInfo.state == DEVICE_UPDATA_WRITE_FLASH)
                    {
                        addr = SYS_APP_START_ADDR + sector * FLASH_UPDATA_LEN;
                        if(addr >= SYS_APP_START_ADDR)
                        {
                            if(sector <= g_sFramBootParamenter.size + 1)
                            {
                                if(Flash_ReadBuffer(FLASH_DATA_OPEN_ADDR + sector * FLASH_UPDATA_LEN, FLASH_UPDATA_LEN, g_nFlashUpData))
                                {
                                    if(BL_WriteImagePage(addr, g_nFlashUpData, FLASH_UPDATA_LEN))
                                    {
                                      sector ++;
                                    }
                                    else
                                    {
                                        sector = 0;
                                        Sys_RunLedOn();
                                        g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
                                        //写入失败
                                    }
                                }
                                            
                                Sys_RunLedOff();
                            }
                            else
                            {
                                FLASH_Lock();
                                g_sFramBootParamenter.appState = FRAM_BOOT_APP_REPLACE_OVER;
                            }
                        }
                        
                    }
               }
               else
               {
                    flashTime++;
               }
                a_ClearStateBit(g_nSysState, SYS_STAT_WR_RE_FLASH);
            }                                  
          
        }
        else if(g_sFramBootParamenter.flag == DEVICE_TYPE_SM5002)
        {
        
        }
        else if(g_sFramBootParamenter.flag == DEVICE_TYPE_SM5003)
        {
        
        }
    }

}

