#include "AnyID_SM5001_Elect_HL.h"


#define ELECT_INT_CHANNEL           USART2_IRQn
const PORT_INF ELECT_PORT_TX      = {GPIOA, GPIO_Pin_2};
const PORT_INF ELECT_PORT_RX      = {GPIOA, GPIO_Pin_3};
const PORT_INF ELECT_PORT_CTRL    = {GPIOA, GPIO_Pin_4};

void Elect_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = ELECT_PORT_CTRL.Pin;
    GPIO_Init(ELECT_PORT_CTRL.Port, &GPIO_InitStructure);
    Elect_EnableRx();
    
    GPIO_InitStructure.GPIO_Pin = ELECT_PORT_TX.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(ELECT_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ELECT_PORT_RX.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(ELECT_PORT_RX.Port, &GPIO_InitStructure);    
    
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    //Configure USART
    USART_Init(ELECT_PORT, &USART_InitStructure);
    // Enable the USART
    USART_Cmd(ELECT_PORT, ENABLE);
    //保证数据准确，启用DMA 接收
    USART_ITConfig(ELECT_PORT, USART_IT_IDLE, ENABLE);
    USART_DMACmd(ELECT_PORT, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(ELECT_PORT, USART_DMAReq_Rx, ENABLE);
}

void Elect_ConfigInt(FunctionalState state)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = ELECT_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;

    NVIC_Init(&NVIC_InitStructure);
}

void Elect_InitTxDma(u8 *pTxBuffer, u32 len)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    DMA_InitTypeDef  DMA_InitStructure = {0};

    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pTxBuffer;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ELECT_PORT->DR));
    DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

    DMA_Init(ELECT_TXDMA_CH, &DMA_InitStructure);
    
    DMA_ITConfig(ELECT_TXDMA_CH, DMA_IT_TC, ENABLE);  
    Elect_DisableTxDma();// 关闭DMA
	
    NVIC_InitStructure.NVIC_IRQChannel = ELECT_TXDMA_INT;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_TXDMA >> 2;    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_TXDMA & 0x03;;
    
    NVIC_Init(&NVIC_InitStructure);
}


void Elect_InitRxDma(u8 *pRxBuffer, u32 len)
{
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit(ELECT_RXDMA_CH);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ELECT_PORT->DR));
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pRxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    
    DMA_Init(ELECT_RXDMA_CH, &DMA_InitStructure);
    DMA_ITConfig(ELECT_RXDMA_CH, DMA_IT_TC, ENABLE); 
    Elect_DisableRxDma();
    
    NVIC_InitStructure.NVIC_IRQChannel = ELECT_RXDMA_INT;   

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RXDMA >> 2;    //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RXDMA & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}


void Elect_Stop()
{
    Elect_DisableRxDma();
    Elect_DisableTxDma();
    Elect_ConfigInt(DISABLE);
    USART_ITConfig(ELECT_PORT, USART_IT_IDLE, DISABLE);
    USART_DMACmd(ELECT_PORT, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(ELECT_PORT, USART_DMAReq_Rx, DISABLE);
    USART_Cmd(ELECT_PORT, DISABLE);
    USART_DeInit(ELECT_PORT);
}

void Elect_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(ELECT_PORT, USART_IT_RXNE | USART_IT_TC);

    USART_ITConfig(ELECT_PORT, USART_IT_RXNE, rxState);                    //
    USART_ITConfig(ELECT_PORT, USART_IT_TC, txState);                     //发送中断
}

void Elect_WriteByte(u8 ch)
{
    while(((ELECT_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
	(ELECT_PORT)->DR = (ch & (u16)0x01FF);
}

void Elect_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Elect_WriteBuffer(u8 *pFrame, u16 len)
{
    u32 i = 0;
    Elect_EnableTx();
    for(i = 0; i < len; i++)
    {
        Elect_WriteByte(pFrame[i]);
    }
    Elect_ChkTxOver();
    Elect_EnableRx();
}
