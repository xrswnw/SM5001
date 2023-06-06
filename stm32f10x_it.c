/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    g_nSysTick ++;
  //心跳时钟调整，设置5ms心跳     

    if((g_nSysTick % 21) == 0)
    {
        a_SetStateBit(g_nSysState, SYS_STAT_RUNLED | SYS_STAT_SENSOR_CHK | SYS_STAT_TEMP_CHK | SYS_STAT_MQTT_HEART | SYS_STAT_AD | SYS_STAT_VOICE_CHK |  SYS_STAT_WATER_CTR | SYS_STAT_TEST_TIM | SYS_STAT_GATE_STAT_CHK);  
        a_SetStateBit(g_sElectInfo.state, ELECT_STAT_TX);
    }
    
    
    Uart_IncIdleTime(STICK_TIME_MS, g_sUartRcvFrame);
    Uart_IncIdleTime(STICK_TIME_MS, g_sW232RcvFrame);
  
}

void SOUND_IRQHandler(void)
{
    if(SOUND_PORT->SR & USART_FLAG_RXNE)
    {
        u8 byte =0;
        
        USART_ClearFlag(SOUND_PORT,USART_FLAG_RXNE);
        byte =  Sound_ReadByte();
        Sound_ReceiveFrame(byte,&g_sSoundInfo);
    }
    else
    {
        Sound_ReadByte();
    }
    SOUND_PORT->SR &= (~0x3FF);
}


 
void RTC_IRQHandler(void)
{

    if(RTC_GetITStatus(RTC_IT_OW)  == RESET)
    {
        RTC_ClearFlag(RTC_IT_OW);     //更新RTC，不需要报警
        //Rtc_UpdateAlarmTime(RTC_ALARM_TIME);
        RTC_SetCounter(0);
        g_sRtcTime.counter ++;
        // Wait for RTC registers synchronization
        RTC_WaitForSynchro();
        // Wait until last write operation on RTC registers has finished
        RTC_WaitForLastTask();
    }
}




u16 g_nElectSr = 0;
u16 g_nElectDr = 0;
void ELECT_IRQHandler(void)
{
    if(ELECT_PORT->SR & USART_FLAG_IDLE)
    {    
        Elect_DisableRxDma();
        g_sElectInfo.rxBuf.len = Elect_GetRxLen(); 
        g_sElectInfo.state = ELECT_STAT_RCV;    //接收数据完成
    }    
    
    g_nElectSr = ELECT_PORT->SR;  //通过读取SR和DR清空中断标志
    g_nElectDr = ELECT_PORT->DR;    
}

void ELECT_RxDMAIRQHandler(void)
{
    Elect_DisableRxDma(); //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sElectInfo.rxBuf.len = Elect_GetRxLen(); 
    g_sElectInfo.state = ELECT_STAT_RCV;    //接收数据完成
}

void ELECT_TxDMAIRQHandler(void)
{
    Elect_DisableTxDma();                    //DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    g_sElectInfo.state = ELECT_STAT_TX_IDLE;
}



u16 g_nWaterSr = 0;
u16 g_nWaterDr = 0;
void Water_IRQHandler(void)
{
    if(WATER_PORT->SR & USART_FLAG_IDLE)
    {    
        Water_DisableRxDma();
        g_sWaterInfo.rxBuf.len = Water_GetRxLen(); 
        g_sWaterInfo.state = WATER_STAT_RCV;    //接收数据完成
    }    
    
    g_nWaterSr = WATER_PORT->SR;  //通过读取SR和DR清空中断标志
    g_nWaterDr = WATER_PORT->DR;    
}

void WATER_RxDMAIRQHandler(void)
{
    Water_DisableRxDma(); //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sWaterInfo.rxBuf.len = Water_GetRxLen(); 
    g_sWaterInfo.state = WATER_STAT_RCV;    //接收数据完成
}

void WATER_TxDMAIRQHandler(void)
{
    Water_DisableTxDma();//DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    Water_EnableRxDma();
    g_sWaterInfo.state = WATER_STAT_TX_IDLE;
}


 u8 g_nW232RxByte = 0;


void W232_IRQHandler(void)
{
    if(USART_GetITStatus(W232_PORT, USART_IT_RXNE) != RESET)        //reg
    {
        g_nW232RxByte = W232_ReadByte();

        g_sW232RcvFrame.buffer[g_sW232RcvFrame.index++] = g_nW232RxByte;
        if(g_sW232RcvFrame.index < UART_BUFFER_MAX_LEN)
        {
            g_sW232RcvFrame.state |= UART_FLAG_RCV;
        }
        else
        {
           g_sW232RcvFrame.state = UART_STAT_END; 
        }
        g_sW232RcvFrame.idleTime = 0;
    }
    else
    {
        W232_ReadByte();
    }
    W232_PORT->SR &= (~0x3FF);
}

/*
u16 g_nLteSr = 0;
u16 g_nLteDr = 0;
void W232_IRQHandler(void)
{
    if(W232_PORT->SR & USART_IT_RXNE)
    {    
        g_nLteDr = W232_ReadByte();
        g_sW232RcvFrame.buffer[g_sW232RcvFrame.index++] =  g_nLteDr;
        if(g_sW232RcvFrame.index  >= UART_BUFFER_MAX_LEN)
        {
           g_sW232RcvFrame.state |=  UART_FLAG_RCV;     //接收数据完成
        }
        else
        {
            g_sW232RcvFrame.state = UART_STAT_END;
        }
        g_sW232RcvFrame.idleTime = 0;
    }

    g_nLteSr = W232_PORT->SR;                                       //通过读取SR和DR清空中断标志
    g_nLteDr = W232_PORT->DR;  
}

          */



void ADC1_2_IRQHandler(void)
{

}

void Gate_RxDMAIRQHandler(void)
{
    Gate_DisableRxDma();                                    //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sGateOpInfo.rxFrame.len = Gate_GetRxLen(); 
    g_sGateOpInfo.rxFrame.state = UART_STAT_RX_OVR;         //接收数据完成
}

void Gate_TxDMAIRQHandler(void)
{
    Gate_DisableTxDma();                                    //DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    Gate_EnableRxDma();                                     //使能接收
}

u16 g_nGateSr = 0;
u16 g_nGateDr = 0;
void Gate_IRQHandler(void)
{
    if(GATE_PORT->SR & GATE_SR_IDLE)
    {    
        Gate_DisableRxDma();
        g_sGateOpInfo.rxFrame.len = Gate_GetRxLen(); 
        g_sGateOpInfo.rxFrame.state = UART_STAT_RX_END;     //接收数据完成
    }

    g_nGateSr = GATE_PORT->SR;                              //通过读取SR和DR清空中断标志
    g_nGateDr = GATE_PORT->DR;    
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
