#include "AnyID_SM5001_SysCfg.h"

u32 g_nSysState = SYS_STAT_IDLE;
u32 g_nLedDelayTime = 0;
u32 g_nTestIndex = 0;


#if SYS_ENABLE_WDT
    #define SYS_ENABLE_TEST         0
#else
    #define SYS_ENABLE_TEST         1
#endif

void Sys_Delayms(u32 n)
{
    //72MHZ
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
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_10);    //PLLÔÚ×îºóÉèÖÃ

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
                           RCC_APB2Periph_AFIO  |
                           RCC_APB2Periph_SPI1  |
                           RCC_APB2Periph_USART1|
                           RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_ADC1  , state);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, state);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 |
                           RCC_APB1Periph_USART3 |
                           RCC_APB1Periph_UART5  |
                           RCC_APB1Periph_UART4  |
                           RCC_APB1Periph_PWR    |
                           RCC_APB1Periph_BKP , state);

}

void Sys_CfgNVIC(void)
{
    //NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM
    //Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  //VECT_TAB_FLASH
    //Set the Vector Table base location at 0x08000000
#ifdef _ANYID_BOOT_GD32_
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
#else
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
#endif

    //Configure the Priority Group to 2 bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


const PORT_INF SYS_RUN_LED  = {GPIOB, GPIO_Pin_1};


void Sys_CtrlIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
     
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = SYS_RUN_LED.Pin;
    GPIO_Init(SYS_RUN_LED.Port, &GPIO_InitStructure);

}


void Sys_Init(void)
{
    Sys_CfgClock();
    Sys_CfgNVIC();
    Sys_CfgPeriphClk(ENABLE);
    Sys_CtrlIOInit();


#if SYS_ENABLE_WDT
    WDG_InitIWDG();
#endif
    
    FRam_InitInterface();
    IO_Init();
    
    W232_Init();
    IO_Led_Open();
    IO_Fan_Open();
    Gate_Init(&g_sDeviceParams.gateParams, g_nSysTick);
    Water_Init();
    //IO_Realy_Open(); a_SetStateBit(g_sIoInfo.state,IO_STAT_RELAY);                      //¼ÌµçÆ÷¿ªÆôÎÊÌâ
    Sys_Delayms(500);WDG_FeedIWDog();Sys_Delayms(500);WDG_FeedIWDog();Sys_Delayms(500);WDG_FeedIWDog();
    Sys_Delayms(500);WDG_FeedIWDog();Sys_Delayms(500);WDG_FeedIWDog();Sys_Delayms(500);WDG_FeedIWDog();  
    Sound_Init();
    Device_Init();
    IO_Fan_Close();
    IO_Led_Close();
#if SYS_ENABLE_WDT
    WDG_FeedIWDog();
#endif
    STick_InitSysTick();  
    AD_InitInterface();
    AD_Init();
    AD_DmaInit();
    a_SetState(g_nSysState, SYS_STAT_IDLE);    
    a_SetStateBit(g_nSysState, SYS_STAT_KEY_CHK); 
    Sys_LedOff();
    Sys_EnableInt();
    
}

void Sys_LedTask(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_RUNLED))
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_RUNLED);
        g_nLedDelayTime++;
        if(a_CheckStateBit(g_nSysState, SYS_STAT_MQTT_ACCESS))
        {
            if(g_nLedDelayTime % 20 == 0)
            {
                
                Sys_LedOff();
            }
            else
            {
                 Sys_LedOn();
            }
            
        }
        else
        {
            if(g_nLedDelayTime & 0x01)
            {
                
                Sys_LedOn();
            }
            else
            {
                 Sys_LedOff();
            }
        
        }
        
        if(a_CheckStateBit(g_sDeviceTestInfo.flag, DEVICE_TEST_SERSOR))
        {
            if(a_CheckStateBit(g_sDeviceTestInfo.err, DEVICE_TEST_ERR_TEMPR))
            {
                if(g_nLedDelayTime % 0x0A == 0)
                {
                    
                    IO_Led_Open();
                }
                else
                {
                     IO_Led_Close();
                }
            }
            else if(a_CheckStateBit(g_sDeviceTestInfo.err, DEVICE_TEST_ERR_ELECT))
            {
                if(g_nLedDelayTime & 0x01)
                {
                    
                    IO_Led_Open();
                }
                else
                {
                     IO_Led_Close();
                }
            }
            else
            {
				W232_KeyHigh();
				W232_CtrlHigh();
				#if SYS_ENABLE_WDT
				WDG_FeedIWDog();
				#endif
				Reader_Delayms(500);
				#if SYS_ENABLE_WDT
				WDG_FeedIWDog();
				#endif
				Reader_Delayms(500);
				W232_CtrlLow();
				W232_KeyLow();
				#if SYS_ENABLE_WDT
				WDG_FeedIWDog();
				#endif
				Reader_Delayms(500);
				Sys_SoftReset();
            }


        }
		

			
#if SYS_ENABLE_WDT
WDG_FeedIWDog();
#endif
    }
  
}


void Sys_GateTask(void)
{

    if(Gate_UartCheckErr())
    {
        Gate_Stop();
        Sys_Delayms(10);
        Gate_Init(&g_sDeviceParams.gateParams, g_nSysTick);
    }

	if(!a_CheckStateBit(g_sDeviceTestInfo.flag, GATE_FLAG_DOOR_TEST | DEVICE_TEST_SERSOR))
	{
    if(Gate_IsRcvFrame(g_sGateOpInfo.rxFrame))
    {
        if(g_sGateOpInfo.rxFrame.len >= UART_RFRAME_MIN_LEN && g_sGateOpInfo.state == GATE_OP_STAT_WAIT)
        {
            u16 startPos = 0;
            if(Gate_CheckRspFrame(&g_sGateOpInfo.rxFrame, &startPos))
            {
                if(Device_GateProceRspFrame(g_sGateOpInfo.rxFrame.buffer + startPos, &g_sGateOpInfo, g_nSysTick))
                {
                  a_SetState(g_sGateOpInfo.rpt, SYS_NULL_TICK);//Çå¿ÕRPT±êÖ¾
                    
                    if(g_sGateOpInfo.batOpState == GATE_OP_BAT_STAT_ING)
                    {
                        g_sGateOpInfo.state = GATE_OP_STAT_WAIT;
                    }
                    else if(g_sGateOpInfo.batOpState == GATE_OP_BAT_STAT_OPEN)
                    {
                        g_sGateOpInfo.state = GATE_OP_STAT_WAIT;
                    }
                    else
                    {
                       Gate_StartOpDelay(GATE_OP_DLY_TIM, g_nSysTick);         //ÑÓÊ±µÈ´ýÏÂÒ»´Î²Ù×÷
                    }
                    
                }
            }
        }
        Gate_ResetFrame(g_sGateOpInfo.rxFrame);
        if(g_sGateOpInfo.state == GATE_OP_STAT_WAIT)                      
        {
            Gate_EnableRxDma();
        }
    }

    if(g_sGateOpInfo.state == GATE_OP_STAT_INIT)     //¿ª»úÄ£Ê½
    {
        if(g_sGateOpInfo.tick + GATE_INIT_DLY_TIM < g_nSysTick)
        {
            g_sGateOpInfo.state = GATE_OP_STAT_NXT;
        }
    }
    else if(g_sGateOpInfo.state == GATE_OP_STAT_DLY)
    {
        if(g_sGateOpInfo.tick + GATE_OP_DLY_TIM < g_nSysTick)
        {
            g_sGateOpInfo.state = GATE_OP_STAT_NXT;

        }
    }
    else if(g_sGateOpInfo.state == GATE_OP_STAT_TX)
    {
       if(g_sGateOpInfo.mode == GATE_MODE_MAIN_ERR_INFO)
       {
			Device_FormatMainInfo(&g_sGateOpInfo);
       }
       Gate_TxFrame(&g_sGateOpInfo, g_nSysTick);
       g_sGateOpInfo.state = GATE_OP_STAT_WAIT;
	   
	   
       if(g_sGateOpInfo.batOpState == GATE_OP_BAT_STAT_ING)
       {
          g_sGateOpInfo.tickCmd = g_nSysTick;
       }
    }
    else if(g_sGateOpInfo.state == GATE_OP_STAT_WAIT)
    {
        if(g_sGateOpInfo.batOpState == GATE_OP_BAT_STAT_OPEN)
        {
             g_nBratIngTick = g_nSysTick  ;
            g_sGateOpInfo.batOpState = GATE_OP_BAT_STAT_ING ;
            g_sGateOpInfo.batInfoRepat = 0;
            g_sGateOpInfo.tick = g_nSysTick;
        }
        else if(g_sGateOpInfo.batOpState == GATE_OP_BAT_STAT_ING)
        {
            g_sGateOpInfo.tick = g_nSysTick;
            if(g_nBratIngTick + GATE_OP_BR_BAT_TIM < g_nSysTick)
            {
                g_nBratIngTick =  g_nSysTick;
                g_sGateOpInfo.batInfoRepat ++;
                if(g_sGateOpInfo.cmd == GATE_FRAME_CMD_RTNBAT)
                {
                    Device_GateBatTwice(GATE_FRAME_CMD_RTNBAT, g_sGateOpInfo.add) ; Gate_TxFrame(&g_sGateOpInfo, g_nSysTick);
                    if(g_sGateOpInfo.batInfoRepat > GATE_OP_BAT_INFO_REPATNUM)
                    {
                      	Gate_BatOpInfo();
                      	g_sDeviceImpRspInfo.rtuLen = Device_ResponseRtBat(g_sDeviceImpRspInfo.rtuBuffer);
                     	Device_AtRsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_REBAT);
                    }
                    
                }
                else if(g_sGateOpInfo.cmd == GATE_FRAME_CMD_BRWBAT)
                {
                    Device_GateBatTwice(GATE_FRAME_CMD_BRWBAT, g_sGateOpInfo.add) ;Gate_TxFrame(&g_sGateOpInfo, g_nSysTick);
                    if(g_sGateOpInfo.batInfoRepat > GATE_OP_BAT_INFO_REPATNUM)
                    {
                    	Gate_BatOpInfo();
                    	g_sDeviceImpRspInfo.rtuLen = Device_ResponseBrBat(g_sDeviceImpRspInfo.rtuBuffer);
                    	Device_AtRsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_BWBAT);
                    }
                    
                }
                g_nBatOpenFlag = TRUE;
            }
        }
      
        if(a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_BWBAT) || a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_REBAT)) //cmdÃüÁî³¬Ê±´¦Àí add
        {
            if(g_sGateOpInfo.tickCmd + GATE_OP_MQTT_CMD_TIM < g_nSysTick)
            {
				g_sGateOpInfo.rpt++;
                if(g_sGateOpInfo.rpt < GATE_RPT_NUM)
                {
                    g_sGateOpInfo.state = GATE_OP_STAT_TX;
                }
                else
                {	a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_BWBAT); 
                	a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_REBAT);
                    g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                    Device_AtRsp(W232_CNT_TIME_1S, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
                }
            }
        
        }
		else  if( a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE))  //cmdÃüÁî³¬Ê±´¦Àí   add
		{
			 if(g_sGateOpInfo.tickCmd + GATE_OP_MQTT_CMD_TIM < g_nSysTick)
            {   
                g_sGateOpInfo.rpt++;
                if(g_sGateOpInfo.rpt < GATE_RPT_NUM)
                {
                    g_sGateOpInfo.state = GATE_OP_STAT_TX;
                }
                else
                {
					a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_GATE);
                    g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
                    g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
                    Device_AtRsp(W232_CNT_TIME_1S, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
                }
            }
		}
		else 
		{
			if(g_sGateOpInfo.tick + GATE_OP_TO_TIM < g_nSysTick)
			{
				g_sGateOpInfo.comErr[g_sGateOpInfo.slvIndex]++;
				g_sGateOpInfo.rpt++;
				if(g_sGateOpInfo.rpt < GATE_RPT_NUM)
				{
					g_sGateOpInfo.state = GATE_OP_STAT_TX;
				}
				else
				{
					Gate_StartOpDelay(GATE_OP_DLY_TIM, g_nSysTick);
				}
				if(g_sGateOpInfo.comErr[g_sGateOpInfo.slvIndex] >= DEVICE_GATE_OP_TICK && (g_aGateSlvInfo[2 * (g_sGateOpInfo.slvIndex)].state != GATE_STAT_COMERR
																						   || g_aGateSlvInfo[2 * (g_sGateOpInfo.slvIndex)].flag != GATE_STAT_COMERR))
				{
					g_aGateSlvInfo[2 * (g_sGateOpInfo.slvIndex)].state = GATE_STAT_COMERR;
					g_aGateSlvInfo[2 * (g_sGateOpInfo.slvIndex) + 1].state = GATE_STAT_COMERR;
					g_aGateSlvInfo[2 * (g_sGateOpInfo.slvIndex)].flag = GATE_STAT_COMERR;
					g_aGateSlvInfo[2 * (g_sGateOpInfo.slvIndex) + 1].flag = GATE_STAT_COMERR;
				}
			}
		
		}
    }
    else if(g_sGateOpInfo.state == GATE_OP_STAT_NXT)
    {
        Gate_GetNextOp(&g_sGateOpInfo, g_nSysTick);
        g_sGateOpInfo.state = GATE_OP_STAT_TX;
    }
	}
}

void Sys_SoundTask()
{
     //´®¿Ú´íÎó´¦Àí:ÖØÐÂ³õÊ¼»¯
    if(USART_GetFlagStatus(SOUND_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE ))
    {
        USART_ClearFlag(SOUND_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE );
        Sound_InitInterface(SOUND_BAUDRARE);
        Sys_Delayms(10);
        Sound_ConfigInt();
        memset(&g_sSoundInfo, 0, sizeof(SOUND_INFO));
        g_sSoundInfo.state = SOUND_STAT_IDLE; 
    }
     if(Sound_IsRcvFrame(g_sSoundInfo))
    {
        if(a_CheckStateBit(g_sSoundInfo.state, SOUND_STAT_RCV))
        {
            if(Sounde_Chk(g_sSoundInfo.rxBuf.buffer))  
            {
                g_sSoundInfo.txBuf.result = SOUND_RESULT_OK;
                g_sSoundInfo.txBuf.flag = SOUNE_VOICE_IDLE;
                a_SetState(g_sSoundInfo.state, SOUND_STAT_STEP);
                Sound_ClearRxBuffer();
                Sounde_Close();
                if(g_sSoundInfo.test == SOUND_VOICE_TEST_FLAG)
                {
                    a_SetState(g_sSoundInfo.test , SYS_NULL_TICK);
                    Device_VoiceApoFrame(SOUND_CNT_TIME_1S * 2, SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_DI);
                }
                
            }
            else
            {
                g_sSoundInfo.state = SOUND_STAT_RCV;                         
                g_sSoundInfo.idleTime = 0;
            }
        }
    }
    

    
    if(a_CheckStateBit(g_sSoundInfo.state, SOUND_STAT_TX))      
    {
        a_ClearStateBit(g_sSoundInfo.state, SOUND_STAT_TX);

        if(g_sSoundInfo.txBuf.index < g_sSoundInfo.txBuf.num)
        {
            Sound_ClearRxBuffer();Sounde_Open();
            Sound_CtrTxCmd(&g_sSoundInfo.txBuf, g_nSysTick);
            g_sSoundInfo.txBuf.flag = SOUNE_VOICE_LOADING;
            a_SetState(g_sSoundInfo.state, SOUND_STAT_RCV | SOUND_STAT_WAIT);
        }
        
    }
    
    if(a_CheckStateBit(g_sSoundInfo.state, SOUND_STAT_WAIT))    
    {
        if(g_sSoundInfo.txBuf.tick + g_sSoundInfo.txBuf.to[g_sSoundInfo.txBuf.index] < g_nSysTick)
        {
            g_sSoundInfo.txBuf.repeat[g_sSoundInfo.txBuf.index]--;              
            if(g_sSoundInfo.txBuf.repeat[g_sSoundInfo.txBuf.index] == 0)
            {
                a_ClearStateBit(g_sSoundInfo.state, SOUND_STAT_WAIT);
                a_SetState(g_sSoundInfo.state, SOUND_STAT_STEP);
                g_sSoundInfo.txBuf.result = SOUND_RESULT_TO;  
                g_sSoundInfo.txBuf.flag = SOUNE_VOICE_IDLE;

            }
            else
            {
                //g_sSoundInfo.state = SOUND_STAT_TX;            
            }
        }
    }
    
    if(a_CheckStateBit(g_sSoundInfo.state, SOUND_STAT_STEP))    
    {
        a_ClearStateBit(g_sSoundInfo.state, SOUND_STAT_STEP);
        Sound_ConnectStep(&g_sSoundInfo.txBuf); 
        
            a_SetState(g_sSoundInfo.state, SOUND_STAT_TX);
            if(g_sSoundInfo.txBuf.index >= g_sSoundInfo.txBuf.num )              
            {
                memset(&g_sSoundInfo.txBuf, 0 ,sizeof(SOUND_TX_BUF));
            }
        
    }
    
    if(a_CheckStateBit(g_nSysState, SYS_STAT_VOICE_CHK))
    {
        static u8 sampleTick = 0;
        
        a_ClearStateBit(g_nSysState, SYS_STAT_VOICE_CHK);
        sampleTick++;
        if(sampleTick >= DEVICE_IO_SAMPLE_TIME * 5)
        {
            a_SetState(sampleTick, SYS_NULL_TICK);
            Device_VoiceCtr();
        }
    }
}

 


void Sys_ElectTask()
{
  
    if(USART_GetFlagStatus(ELECT_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
		Elect_Stop();
		USART_ClearFlag(ELECT_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE); 
		Sys_Delayms(5);
		memset(&(g_sElectInfo.rxBuf), 0, sizeof(ELECT_INFO));
		Elect_Init(0);
		g_sElectInfo.state = ELECT_STAT_IDLE;

    }
	
    if(Elect_IsRcvFrame(g_sElectInfo))
    {
        Elect_ResetFrame(g_sElectInfo);
        Elect_EnableRxDma();
        if(g_sElectInfo.rxBuf.len >= ELECT_BUF_MIN_LEN)
        {
            if(Elect_CheckFrame(&g_sElectInfo.rxBuf))
            { 
                Elect_GetValue(g_sElectInfo);
                g_sElectInfo.value = Float32(g_sElectInfo.electValue);
              
            }
        }

    }

    if(a_CheckStateBit(g_sElectInfo.state, ELECT_STAT_TX)) 
    {
        a_ClearStateBit(g_sElectInfo.state, ELECT_STAT_TX);
        g_sElectInfo.time ++;
        if(g_sElectInfo.time > ELECT_SAMPLE_TIME)
        {
              g_sElectInfo.tick = g_nSysTick;
              a_SetState(g_sElectInfo.time, SYS_NULL_TICK);
              Elect_TransmitCmd(&g_sElectInfo.txBuf);
              a_SetStateBit(g_sElectInfo.state, ELECT_STAT_WAIT);   
        }
        
    }
    if(a_CheckStateBit(g_sElectInfo.state, ELECT_STAT_WAIT)) 
    {  
        if(g_sElectInfo.tick + ELECT_OP_TIME < g_nSysTick) 
        {
            g_sElectInfo.repeat ++ ;
            if(g_sElectInfo.repeat > ELECT_SAMPLE_NUM)       
            { 
                g_sElectInfo.tick  = g_nSysTick;
                g_sElectInfo.electValue = ELECT_GET_VALUE_FAIL;
                a_SetState(g_sElectInfo.repeat, SYS_NULL_TICK);
                a_SetStateBit(g_sIoInfo.state,IO_STAT_ELECT_FAIL);
            }  
            a_ClearStateBit(g_sElectInfo.state, ELECT_STAT_WAIT);
            a_SetStateBit(g_sElectInfo.state, ELECT_STAT_TX);  
        }
    }
    
    if(a_CheckStateBit(g_sElectInfo.state, ELECT_STAT_RCV))
    {
        a_SetState(g_sElectInfo.repeat, SYS_NULL_TICK); 
    }
}


void Sys_W232Task(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU))
    {
        return; //Ö»ÓÐ²»ÊÇÍ¸´«Ä£Ê½£¬²ÅÐèÒª°´ÕÕATÖ¸Áî½âÎö
    }

    if(USART_GetFlagStatus(W232_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(W232_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        W232_InitInterface(W232_BAUDRARE);
        W232_ConfigInt();
        W232_EnableInt(ENABLE, DISABLE);
    }
    if(a_CheckStateBit(g_nSysState,SYS_STAT_MQTT_OFFLINE))
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_MQTT_OFFLINE);
        Sys_LedOff();
		W232_KeyHigh();
		W232_CtrlHigh();
        #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
        #endif
        Reader_Delayms(500);
        #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
        #endif
        Reader_Delayms(500);
		W232_CtrlLow();
        W232_KeyLow();
        g_sDeviceParams.offLineTime = 0;
        W232_ConnectInit(&g_sW232Connect, W232_CNT_CMD_PWRON, &g_sDeviceParams.serverParams);
        a_ClearStateBit(g_nSysState, SYS_STAT_MQTT_ACCESS);
        a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_TX);
        //a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_RX | W232_CNT_OP_STAT_WAIT);
      
    }
    if(Uart_IsRcvFrame(g_sW232RcvFrame))
    {
        if(a_CheckStateBit(g_sW232Connect.state, W232_CNT_OP_STAT_RX))
        {        
            if(W232_ConnectCheckRsp(&g_sW232Connect, g_sW232RcvFrame.buffer))   //Èç¹ûÐ£ÑéÏìÓ¦Ö¡Ê§°Ü£¬¾Í¼ÌÐø½ÓÊÕ£¬·ñÔò¸´Î»½ÓÊÕ»º³åÇø
            {
                g_sW232Connect.result = W232_CNT_RESULT_OK;
                a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_STEP);
                W232_ClearRxBuffer();
            }
            else
            {
                g_sW232RcvFrame.state = UART_FLAG_RCV;                          //¼ÌÐø½ÓÊÕ
                g_sW232RcvFrame.idleTime = 0;
            }
        }
    }
    
    if(a_CheckStateBit(g_sW232Connect.state, W232_CNT_OP_STAT_TX))      //·¢ËÍATÖ¸Áî
    {
        a_ClearStateBit(g_sW232Connect.state, W232_CNT_OP_STAT_TX);

        if(g_sW232Connect.index < g_sW232Connect.num)
        {
            W232_ClearRxBuffer();
            W232_ConnectTxCmd(&g_sW232Connect, g_nSysTick);
            a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_RX | W232_CNT_OP_STAT_WAIT);
        }
    }

    //³¬Ê±
    if(a_CheckStateBit(g_sW232Connect.state, W232_CNT_OP_STAT_WAIT))    //ÅÐ¶Ï³¬Ê±£¬Ã¿ÌõÖ¸ÁîµÄ³¬Ê±Ê±¼ä¶¼²»Ò»Ñù
    {
        if(g_sW232Connect.tick + g_sW232Connect.to[g_sW232Connect.index] < g_nSysTick)
        {
            g_sW232Connect.repeat[g_sW232Connect.index]--;              //ÓÐÐ©Ö¸ÁîÔÊÐí¶à´Î²Ù×÷
            if(g_sW232Connect.repeat[g_sW232Connect.index] == 0)
            {
                a_ClearStateBit(g_sW232Connect.state, W232_CNT_OP_STAT_WAIT);
                a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_STEP);
				g_sW232Connect.comErr ++;
                g_sW232Connect.result = W232_CNT_RESULT_TO;             //µ±Ç°²Ù×÷³¬Ê±£¬GPRSÖ´ÐÐÁ÷³Ì´íÎó
            }
            else
            {
                g_sW232Connect.state = W232_CNT_OP_STAT_TX;             //ÖØ¸´·¢ËÍ
            }
        }
    }

    if(a_CheckStateBit(g_sW232Connect.state, W232_CNT_OP_STAT_STEP))    //ÏÂÒ»²½Âß¼­´¦Àí
    {
        a_ClearStateBit(g_sW232Connect.state, W232_CNT_OP_STAT_STEP);
        W232_ConnectStep(&g_sW232Connect);                              //²Ù×÷²½Öè´¦Àí
        if(g_sW232Connect.result == W232_CNT_RESULT_OK)                 //ËùÓÐÁ÷³ÌÖ´ÐÐÍê³É£¬²¢ÇÒ³É¹¦£¬±íÊ¾½øÈëDTUÄ£Ê½
        {
            g_sW232Connect.state = W232_CNT_OP_STAT_TX;
            if(g_sW232Connect.index == g_sW232Connect.num)              //Èç¹ûÊÇ¿ª»ú²Ù×÷Ö´ÐÐÍê³É£¬±íÊ¾½øÈëDTU£¬»òÕß¹Ø±Õ
            {
                if(g_sW232Connect.cmd == W232_CNT_CMD_PWRON)            
                {
                    g_sW232Connect.state = W232_CNT_OP_STAT_DTU;
                    a_SetStateBit(g_nSysState, SYS_STAT_LTEDTU);        //Á¬½ÓMQTT·þÎñÆ÷³É¹¦
                    g_sW232RcvFrame.state = W232_CNT_OP_STAT_TEST;
                    a_SetStateBit(g_nSysState, SYS_STAT_MQTT_ACCESS);
                    g_sDeviceParams.offLineTime = 0;
                    g_sDeviceParams.heartTick = 0;
					memcpy(g_sMqttKey.imsiStr, g_nImsiStr, REAM_IMEI_LEN);
                    Device_WriteMqttKey();
                    Device_GateStateInit();
                    Device_VoiceApoFrame(SOUND_CNT_TIME_1S * 2, SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_OPEN_DEVICE);

                }
                else
                {
                	a_SetStateBit(g_nSysState,SYS_STAT_MQTT_OFFLINE);
                }
            }
        }
        else													//³õÊ¼»¯Ê§°Ü¹Ø»ú£¿£¿£¿¡¢¡¢¡¢
        {
			a_SetStateBit(g_nSysState,SYS_STAT_MQTT_OFFLINE);
			/*
            if(g_sW232Connect.cmd == W232_CNT_CMD_PWRON)               
            {
				W232_KeyHigh();
				W232_CtrlHigh();
				#if SYS_ENABLE_WDT
				WDG_FeedIWDog();
				#endif
				Reader_Delayms(500);
				#if SYS_ENABLE_WDT
				WDG_FeedIWDog();
				#endif
				Reader_Delayms(500);
				W232_CtrlLow();
				W232_KeyLow();
                W232_ConnectInit(&g_sW232Connect, W232_CNT_CMD_PWROFF, &g_sDeviceParams.serverParams);
                a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_TX);
            }
			 */                       
        }
    }
     

}



void Sys_ServerTask(void)
{
    if(!a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU))  //Ö»ÓÐÍ¸´«ÁË£¬²ÅÐèÒª½øÈë¸ÃÈÎÎñ
    {
        return;
    }
    
    //´®¿Ú´íÎó´¦Àí:ÖØÐÂ³õÊ¼»¯
    if(USART_GetFlagStatus(W232_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(W232_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        W232_InitInterface(W232_BAUDRARE);
        W232_ConfigInt();
        W232_EnableInt(ENABLE, DISABLE);
    }
    
    if(Uart_IsRcvFrame(g_sW232RcvFrame))
    {
		  if(a_CheckStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_RX_AT))
			{
				if(Device_CommunCheckRsp(&g_sDeviceServerTxBuf, g_sW232RcvFrame.buffer))   
				{
					g_sW232RcvBuffer.tick = g_nSysTick;
					g_sDeviceServerTxBuf.state = DEVICE_SERVER_TXSTAT_IDLE;                             //
					a_SetStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXST_STEP); 
					W232_ClearRxBuffer();
				}
				else
				{
					g_sW232RcvFrame.state = UART_FLAG_RCV;                          //¼ÌÐø½ÓÊÕ
					g_sW232RcvFrame.idleTime = 0;
				}
			}

            if(g_sW232RcvFrame.index > 0)
            {
                if(!a_CheckStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_RX_AT) && !a_CheckStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_WAIT))
                 {
                    if(Device_CheckRsp(&g_sW232Connect, g_sW232RcvFrame.buffer, g_sW232RcvFrame.index))
                    {
                        g_sW232RcvBuffer.tick = g_nSysTick;
                        if(g_sW232RcvBuffer.flag == W232_RESPONES_CMD_RESP)
                        {   
                            g_sW232RcvBuffer.flag = W232_RESPONES_NULL;
                            g_sDeviceServerTxBuf.result = W232_CNT_RESULT_OK; 
                        }
                        else
                        {
                            Device_ServerProcessRxInfo(&g_sW232RcvBuffer, g_nSysTick);
                        }
                    }    
                 }
                if(W232_ConnectCheckClose(g_sW232RcvFrame.buffer))
                {
                    W232_ConnectInit(&g_sW232Connect, W232_CNT_CMD_PWROFF, &g_sDeviceParams.serverParams);
                    a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_TX);
                    a_ClearStateBit(g_nSysState, SYS_STAT_LTEDTU);
                }
            }                                                     
			memset(&g_sW232RcvFrame, 0, sizeof(UART_RCVFRAME));
    }
    
    
    
    if(a_CheckStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXST_AT))      
    {
        a_ClearStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXST_AT);

        if(g_sDeviceServerTxBuf.index < g_sDeviceServerTxBuf.num)
        {
            W232_ClearRxBuffer();
            Device_CommunTxCmd(&g_sDeviceServerTxBuf, g_nSysTick);
            a_SetStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_RX_AT | DEVICE_SERVER_TXSTAT_WAIT);
        }
    }
    
    if(a_CheckStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_WAIT))    
    {
        if(g_sDeviceServerTxBuf.tick + g_sDeviceServerTxBuf.to[g_sDeviceServerTxBuf.index] < g_nSysTick)
        {
            g_sDeviceServerTxBuf.repeat[g_sDeviceServerTxBuf.index ]--;              
            if(g_sDeviceServerTxBuf.repeat[g_sDeviceServerTxBuf.index] == 0)
            {
                a_ClearStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXSTAT_WAIT);
                a_SetStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXST_STEP);
				g_sW232Connect.comErr ++;
                g_sDeviceServerTxBuf.result = W232_CNT_RESULT_TO;             
            }
        }
    }
    
    
     if(a_CheckStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXST_STEP))    //ÏÂÒ»²½Âß¼­´¦Àí
    {
        a_ClearStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXST_STEP);
        Device_CommunStep(&g_sDeviceServerTxBuf);   
        a_SetStateBit(g_sDeviceServerTxBuf.state, DEVICE_SERVER_TXST_AT);
        if(g_sDeviceServerTxBuf.index >= g_sDeviceServerTxBuf.num || g_sDeviceServerTxBuf.num > W232_CNT_OP_NUM)
        {
            memset(&g_sDeviceServerTxBuf, 0, sizeof(DEVICE_SENVER_TXBUFFER));
        }
    }
}




void Sys_WaterTask()
{
    if(USART_GetFlagStatus(WATER_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
		Elect_Stop();
		USART_ClearFlag(WATER_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE); 
		Sys_Delayms(5);
		memset(&(g_sWaterInfo.rxBuf), 0, sizeof(WATER_INFO));
		Water_Init();
		g_sWaterInfo.state = WATER_STAT_IDLE;

    }

	if(Water_IsRcvFrame(g_sWaterInfo))
    {
		memcpy(&g_sWaterTempRcv, &g_sWaterInfo.rxBuf, sizeof(WATER_RX_BUF));
		if(g_sWaterInfo.state == WATER_STAT_RCV && g_sWaterTempRcv.len > 5)
		{
			u16 startPos = 0;
			
			if(Water_CheckRspFrame(&g_sWaterTempRcv, &startPos))
			{
				Device_WaterProceRspFrame(g_sWaterTempRcv.buffer, &g_sWaterInfo,  g_sWaterTempRcv.len);
			}
		}
		Water_EnableRxDma();
		Water_StartOpDelay(g_nSysTick);
    }
	
	if(g_sWaterInfo.state == WATER_STAT_TX)
	{
		Water_TxFrame(&g_sWaterInfo, g_nSysTick);
		g_sWaterInfo.state = WATER_STAT_WAIT;
	
	}
	else if(g_sWaterInfo.state == WATER_STAT_DELAY)
	{
		if(g_sWaterInfo.tick + WATER_OP_DLY_TIM < g_nSysTick)
        {
            g_sWaterInfo.state = WATER_STAT_OP_NXT;

        }
	}
	else if(g_sWaterInfo.state == WATER_STAT_OP_NXT)
	{
		Water_GetNextOp(&g_sWaterInfo, g_nSysTick);
		g_sWaterInfo.state = WATER_STAT_TX;
	}
	else if(g_sWaterInfo.state == WATER_STAT_WAIT)
	{
			if( a_CheckStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_RELAY)) 
			{   
				if(g_sWaterInfo.tick + WATER_OP_TIM < g_nSysTick)
				{  
					g_sWaterInfo.repat++;
					if(g_sWaterInfo.repat < WATER_COM_OP_TICK)
					{
						g_sWaterInfo.state = WATER_STAT_TX;
					}
					else
					{
						a_ClearStateBit(g_sDeviceRspFrame.mark, DEVIDE_MARK_RELAY);
						g_sDeviceRspFrame.err = READER_RESPONSE_ERR_DEVICE;
						g_sDeviceRspFrame.len = Device_ResponseFrame(NULL, 0, &g_sDeviceRspFrame);
						Device_AtRsp(W232_CNT_TIME_1S, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_CMD);
					}
				}
				
			}
			else
			{
				if(g_sWaterInfo.tick + WATER_OP_TIM < g_nSysTick)
				{
					if(g_sWaterInfo.repat >= WATER_COM_OP_TICK)
					{
						g_sWaterInfo.repat = 0;
						Water_StartOpDelay(g_nSysTick);
						if(g_sWaterInfo.mode == WATER_STAT_MODE_GET_INFO_RFID)
						{
							g_sWaterInfo.comErr[WATER_DEVICE_RFID]++;		
						}
						else if(g_sWaterInfo.mode == WATER_STAT_MODE_GET_INFO_RELAY || g_sWaterInfo.mode == WATER_STAT_MODE_CTR_CMD_RELAY)
						{
							g_sWaterInfo.comErr[WATER_DEVICE_RELAY]++;	
						}
					}
					else
					{
						g_sWaterInfo.repat++;
						g_sWaterInfo.state = WATER_STAT_TX;
					}
				}
			}
	}
	
	
    //
    if(a_CheckStateBit(g_nSysState, SYS_STAT_KEY_CHK))
    {
        if(!memcmp(g_sMqttKey.uid, g_sWaterInfo.uid, FRAME_UID_LEN) && Device_Chk_Uid(g_sMqttKey.uid))
        {
            Device_VoiceApoFrame(SOUND_CNT_TIME_1S * 2, SOUND_REPAT_NULL, SOUND_VOICE_DI, SOUND_VOC_DI);
            memset(g_sWaterInfo.uid, 0 ,FRAME_UID_LEN);
            a_SetStateBit(g_sDeviceTestInfo.flag, GATE_FLAG_DOOR_TEST);
			g_sDeviceTestInfo.gateAddr = 0x01;
			Device_AnsyFrame(g_sDeviceTestInfo.gateAddr, GATE_FRAME_CMD_SET_OUTINFO, GATE_CTR_DOOR_OPEN);
            a_ClearStateBit(g_nSysState, SYS_STAT_KEY_CHK); 
        }                                    
    }

}



void Sys_IOTask()
{  
    static u8 tempState = 0, tempDeviceState = 0, tempMaskFlag = 0;
    
    if(a_CheckStateBit(g_nSysState, SYS_STAT_SENSOR_CHK))												//IO²É¼¯
    {   
        if(g_sIoInfo.sersorState != tempState || tempDeviceState != g_sIoInfo.deviceState || tempMaskFlag != g_sIoInfo.maskFlag)
        {
            g_sIoInfo.tick++;
            if(g_sIoInfo.tick > DEVICE_REPEAT_TIME)
            {
                a_SetState(g_sIoInfo.tick, SYS_NULL_TICK);
                g_sIoInfo.sersorState = tempState;
				tempDeviceState = g_sIoInfo.deviceState;
				tempMaskFlag = g_sIoInfo.maskFlag;
                Device_IoCtr(); 
            }
        }
        else
        {
            a_SetState(g_sIoInfo.tick, SYS_NULL_TICK);
            tempState = IO_Sersor_Chk(g_sDeviceParams.temprUp.t, g_sDeviceParams.temprDown.t, g_sDeviceParams.gateParams.alarmTmpr);
			
        }
        a_ClearStateBit(g_nSysState, SYS_STAT_SENSOR_CHK);
        
    }
    
	if(g_sIoInfo.state & IO_DEVICE_STAT_DOOR)															//ÃÅ¿Ø
	{
		if(/*(g_sIoInfo.sersorState & IO_SENSOR_STAT_DOOR_OPEN)|| */ g_sIoInfo.ctrlDoorTick + DEVICE_CTR_DOOR_TIME < g_nSysTick)//å¼€é”500ms
		{
			IO_Door_Close();
			g_sIoInfo.state &= ~IO_DEVICE_STAT_DOOR;
		}
	}
}

void Sys_AdTask()   
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_AD))
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_AD);
		Win_CalAvg(&g_sDeviceParams.temprUp, AD_GetTemper1Value());
		AD_GetTmpr(&g_sDeviceParams.temprUp);
		Win_CalAvg(&g_sDeviceParams.temprDown, AD_GetTemper2Value());
		AD_GetTmpr(&g_sDeviceParams.temprDown);
    }
}

void Sys_HeratTask()
{ 
    if(!a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU) || g_sGateOpInfo.batOpState == GATE_OP_BAT_STAT_ING) 
    {
        return;
    }

    if(a_CheckStateBit(g_nSysState, SYS_STAT_MQTT_HEART))  
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_MQTT_HEART);
        g_sDeviceParams.heartTick ++;
        
        if(g_sDeviceParams.heartTick > (g_sDeviceParams.gateTick * DEVICE_HEART_MIN))
        {
            g_sDeviceParams.heartTick = 0;
            g_sDeviceParams.offLineTime ++;
            Device_AtRsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_CNT_OP_GET_QSQ);       //«@È¡ÐÅÌ–Ç¿¶È
            Device_AtRsp(W232_CNT_TIME_500MS, W232_CNT_REPAT_NULL, W232_MQTT_TOPIC_HEART);
        }
        
    }
    
    if(g_sDeviceParams.offLineTime >=  W232_HEART_OFFLINE_TIME)
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_LTEDTU);                                  //ëx¾Q£¬µÈ´ýÖØßB
       // Water_WriteStr("twice link") ;                                                //ëx¾QÌŽÀí
        a_SetStateBit(g_nSysState,SYS_STAT_MQTT_OFFLINE);
    }
    
    if(a_CheckStateBit(g_nSysState, SYS_STAT_GATE_STAT_CHK))
    {
        Device_GateStateRsp();
        a_ClearStateBit(g_nSysState, SYS_STAT_GATE_STAT_CHK);
    }
    if(memcmp(g_sMqttKey.imsiStr, g_nImsiStr,W232_IMSI_LEN))
    {
        Device_ReadMqttKey();
        memcpy(g_nImsiStr, g_sMqttKey.imsiStr,W232_IMSI_LEN);
        if(memcmp(g_sMqttKey.imsiStr, g_nImsiStr,W232_IMSI_LEN) )
        {
            a_ClearStateBit(g_nSysState, SYS_STAT_LTEDTU);
            W232_ConnectInit(&g_sW232Connect, W232_CNT_CMD_PWRON, &g_sDeviceParams.serverParams);
            a_SetState(g_sW232Connect.state, W232_CNT_OP_STAT_TX);
            //Water_WriteStr("imei over") ;
        }

    }
    
    
}




void Sys_TestTask()
{
    if(!a_CheckStateBit(g_sDeviceTestInfo.flag, GATE_FLAG_DOOR_TEST | DEVICE_TEST_SERSOR)) 
    {
        return;
    }
	
	if(a_CheckStateBit(g_sDeviceTestInfo.flag, GATE_FLAG_DOOR_TEST))
	{
		if(Gate_IsRcvFrame(g_sGateOpInfo.rxFrame))
		{
			if(g_sGateOpInfo.rxFrame.len >= UART_RFRAME_MIN_LEN && g_sGateOpInfo.state == GATE_OP_STAT_WAIT)
			{
				u16 startPos = 0;
				if(Gate_CheckRspFrame(&g_sGateOpInfo.rxFrame, &startPos))
				{
					if(Device_GateProceRspFrame(g_sGateOpInfo.rxFrame.buffer + startPos, &g_sGateOpInfo, g_nSysTick))
					{
						a_SetState(g_sGateOpInfo.rpt, SYS_NULL_TICK);//Çå¿ÕRPT±êÖ¾
						Gate_StartOpDelay(GATE_OP_DLY_TIM, g_nSysTick);         //ÑÓÊ±µÈ´ýÏÂÒ»´Î²Ù×÷
					}
				}
			}
			Gate_ResetFrame(g_sGateOpInfo.rxFrame);
			if(g_sGateOpInfo.state == GATE_OP_STAT_WAIT)                      
			{
				Gate_EnableRxDma();
			}
		}
		 if(g_sGateOpInfo.state == GATE_OP_STAT_DLY)
		{
			if(g_sGateOpInfo.tick + GATE_OP_DLY_TIM < g_nSysTick)
			{
				g_sGateOpInfo.state = GATE_OP_STAT_NXT;

			}
		}
		else if(g_sGateOpInfo.state == GATE_OP_STAT_TX)
		{
		   Gate_TxFrame(&g_sGateOpInfo, g_nSysTick);
		   g_sGateOpInfo.state = GATE_OP_STAT_WAIT;
		}
		else if(g_sGateOpInfo.state == GATE_OP_STAT_WAIT)
		{
			if(g_sGateOpInfo.tick + GATE_OP_TO_TIM < g_nSysTick)
			{
				g_sGateOpInfo.comErr[g_sGateOpInfo.slvIndex]++;
				g_sGateOpInfo.rpt++;
				if(g_sGateOpInfo.rpt < GATE_RPT_NUM)
				{
					g_sGateOpInfo.state = GATE_OP_STAT_TX;
					
				}
				else
				{
					g_sDeviceTestInfo.gateAddr ++;
					Gate_StartOpDelay(GATE_OP_DLY_TIM, g_nSysTick);
				}
			}
		   
		}
		else if(g_sGateOpInfo.state == GATE_OP_STAT_NXT)
		{
			Gate_GetNextOp(&g_sGateOpInfo, g_nSysTick);
			g_sDeviceTestInfo.gateAddr ++;
			if(g_sDeviceTestInfo.gateAddr <= (g_sDeviceParams.gateNum << 1))
			{
				Device_AnsyFrame(g_sDeviceTestInfo.gateAddr, GATE_FRAME_CMD_SET_OUTINFO, GATE_CTR_DOOR_OPEN);
			}
			else
			{
				if(a_CheckStateBit(g_nSysState, SYS_STAT_LTEDTU))         //ÁªÍøÇø·Ö£¬Î´ÁªÍø¿ªÆô×Ô¼ì£¬ÁªÍøÖ»¿ª¸÷¸ö²ÖÃÅ
				{
					g_sDeviceTestInfo.flag = DEVICE_TEST_NULL;
					IO_Door_Open();
					g_sIoInfo.state |= IO_DEVICE_STAT_DOOR;
					g_sIoInfo.ctrlDoorTick = g_nSysTick;
					a_SetStateBit(g_nSysState, SYS_STAT_KEY_CHK);
				}
				else
				{
					IO_Door_Open();
					Reader_Delayms(400);
					#if SYS_ENABLE_WDT
					WDG_FeedIWDog();
					#endif
					IO_Door_Close();
					IO_Fan_Open();
					Reader_Delayms(400);
					#if SYS_ENABLE_WDT
					WDG_FeedIWDog();
					#endif
					IO_Fan_Close();
					IO_Led_Open();
					Reader_Delayms(400);
					#if SYS_ENABLE_WDT
					WDG_FeedIWDog();
					#endif
					IO_Led_Close();
					g_sDeviceTestInfo.flag = DEVICE_TEST_SERSOR;
				}
			}
			g_sGateOpInfo.state = GATE_OP_STAT_TX;
		}
    }
	if(a_CheckStateBit(g_sDeviceTestInfo.flag, DEVICE_TEST_SERSOR))
	{
		Device_ChkSersor();
	}
}


