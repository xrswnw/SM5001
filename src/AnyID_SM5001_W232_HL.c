#include "AnyID_SM5001_W232_HL.h"

#define W232_INT_CHANNEL        UART5_IRQn
const PORT_INF W232_PORT_TX   = {GPIOC, GPIO_Pin_12};
const PORT_INF W232_PORT_RX   = {GPIOD, GPIO_Pin_2};
const PORT_INF W232_PORT_KEY  = {GPIOB, GPIO_Pin_4};
//const PORT_INF W232_PORT_PWR  = {GPIOB, GPIO_Pin_4};
//const PORT_INF W232_PORT_STAT = {GPIOB, GPIO_Pin_5};

const PORT_INF W232_PORT_CTRL =  {GPIOB, GPIO_Pin_5};  //0-1变化退出透传模式



void W232_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = W232_PORT_KEY.Pin;
    GPIO_Init(W232_PORT_KEY.Port, &GPIO_InitStructure);
    W232_KeyLow();

    GPIO_InitStructure.GPIO_Pin = W232_PORT_CTRL.Pin;
    GPIO_Init(W232_PORT_CTRL.Port, &GPIO_InitStructure);
    W232_CtrlHigh();
    USART_DeInit(W232_PORT);

    GPIO_InitStructure.GPIO_Pin = W232_PORT_TX.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(W232_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = W232_PORT_RX.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(W232_PORT_RX.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    //Configure USART
    USART_Init(W232_PORT, &USART_InitStructure);

    USART_Cmd(W232_PORT, ENABLE);
    //USART_DMACmd(W232_PORT, USART_DMAReq_Tx, ENABLE);
}

void W232_ConfigInt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = W232_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void W232_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(W232_PORT, USART_IT_RXNE | USART_IT_TC);

    USART_ITConfig(W232_PORT, USART_IT_RXNE, rxState);
    USART_ITConfig(W232_PORT, USART_IT_TC, txState);
}

void W232_WriteByte(u8 ch)
{
    while(((W232_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
	(W232_PORT)->DR = (ch & (u16)0x01FF);
}

void W232_Delayms(u32 n)
{
    //n *= 0x6000;                    //3800-72M 6000-120M
    n *= 0x3800;
    n++;
    while(n--);
}

void W232_WriteBuffer(u8 *pFrame, u16 len)
{
    u16 i = 0;
    for(i = 0; i < len; i++)
    {
        while(((W232_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
        (W232_PORT)->DR = (pFrame[i] & (u16)0x01FF);
    }
}

void W232_WriteCmd(char *str)
{
    W232_WriteBuffer((u8 *)str, strlen(str));
    W232_WriteByte('\r');
    W232_WriteByte('\n');
}

