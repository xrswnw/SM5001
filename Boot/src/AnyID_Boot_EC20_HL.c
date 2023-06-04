#include "AnyID_Boot_EC20_HL.h"

#define EC20_INT_CHANNEL        UART5_IRQn
const PORT_INF EC20_PORT_TX   = {GPIOC, GPIO_Pin_12};
const PORT_INF EC20_PORT_RX   = {GPIOD, GPIO_Pin_2};
const PORT_INF EC20_PORT_KEY  = {GPIOB, GPIO_Pin_4};
const PORT_INF EC20_PORT_CTRL =  {GPIOB, GPIO_Pin_5};  //0-1变化退出透传模式

void EC20_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = EC20_PORT_KEY.Pin;
    GPIO_Init(EC20_PORT_KEY.Port, &GPIO_InitStructure);
    EC20_KeyLow();

    GPIO_InitStructure.GPIO_Pin = EC20_PORT_CTRL.Pin;
    GPIO_Init(EC20_PORT_CTRL.Port, &GPIO_InitStructure);
  
    EC20_CtrlHigh();

    USART_DeInit(EC20_PORT);

    GPIO_InitStructure.GPIO_Pin = EC20_PORT_TX.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(EC20_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = EC20_PORT_RX.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(EC20_PORT_RX.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(EC20_PORT, &USART_InitStructure);

    USART_Cmd(EC20_PORT, ENABLE);

}

void EC20_ConfigInt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = EC20_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void EC20_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(EC20_PORT, USART_IT_RXNE | USART_IT_TC);

    USART_ITConfig(EC20_PORT, USART_IT_RXNE, rxState);
    USART_ITConfig(EC20_PORT, USART_IT_TC, txState);
}

void EC20_WriteByte(u8 ch)
{
    while(((EC20_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
	(EC20_PORT)->DR = (ch & (u16)0x01FF);
}

void EC20_Delayms(u32 n)
{
    //n *= 0x6000;                    //3800-72M 6000-120M
    n *= 0x3800;
    n++;
    while(n--);
}

void EC20_WriteBuffer(u8 *pFrame, u16 len)
{
    u16 i = 0;
    for(i = 0; i < len; i++)
    {
        while(((EC20_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
        (EC20_PORT)->DR = (pFrame[i] & (u16)0x01FF);
    }
}



void EC20_WriteCmd(char *str)
{
    EC20_WriteBuffer((u8 *)str, strlen(str));
    EC20_WriteByte('\r');
    EC20_WriteByte('\n');
}

