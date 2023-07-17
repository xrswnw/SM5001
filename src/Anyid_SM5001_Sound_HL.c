#include "AnyID_SM5001_Sound_HL.h"


#define SOUND_INT_CHANNEL           USART1_IRQn
const PORT_INF SOUND_PORT_TX      = {GPIOA, GPIO_Pin_9};
const PORT_INF SOUND_PORT_RX      = {GPIOA, GPIO_Pin_10};
const PORT_INF SOUND_PORT_CTL     = {GPIOC, GPIO_Pin_2};

/*
#define SOUND_INT_CHANNEL           UART4_IRQn
const PORT_INF SOUND_PORT_TX      = {GPIOC, GPIO_Pin_10};
const PORT_INF SOUND_PORT_RX      = {GPIOC, GPIO_Pin_11};*/
void  Sound_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    USART_DeInit(SOUND_PORT);

    GPIO_InitStructure.GPIO_Pin = SOUND_PORT_TX.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SOUND_PORT_TX.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = SOUND_PORT_CTL.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SOUND_PORT_CTL.Port, &GPIO_InitStructure);
    
    //Sounde_Open();
    GPIO_InitStructure.GPIO_Pin = SOUND_PORT_RX.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SOUND_PORT_RX.Port, &GPIO_InitStructure);    
    
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    //Configure USART
    USART_Init(SOUND_PORT, &USART_InitStructure);
    // Enable the USART
    USART_Cmd(SOUND_PORT, ENABLE);
}

void Sound_ConfigInt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = SOUND_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void Sound_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(SOUND_PORT, USART_IT_RXNE | USART_IT_TC);

    USART_ITConfig(SOUND_PORT, USART_IT_RXNE, rxState);
    USART_ITConfig(SOUND_PORT, USART_IT_TC, txState);
}

void Sound_WriteByte(u8 ch)
{
    while(((SOUND_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
	(SOUND_PORT)->DR = (ch & (u16)0x01FF);
}

void Sound_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Sound_WriteBuffer(u8 *pFrame, u16 len)
{
    u16 i = 0;
    for(i = 0; i < len; i++)
    {

        (SOUND_PORT)->DR = (pFrame[i] & (u16)0x01FF);
        while(((SOUND_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
    }
}


