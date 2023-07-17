#include "AnyID_Boot_Gate_HL.h"


#define GATE_INT_CHANNEL         UART4_IRQn

const PORT_INF GATE_PORT_TX     = {GPIOC, GPIO_Pin_10};
const PORT_INF GATE_PORT_RX     = {GPIOC, GPIO_Pin_11};
const PORT_INF GATE_PORT_CTRL   = {GPIOB, GPIO_Pin_8};
void Gate_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = GATE_PORT_CTRL.Pin;
    GPIO_Init(GATE_PORT_CTRL.Port, &GPIO_InitStructure);
    Gate_EnableRx();

    USART_DeInit(GATE_PORT);

    GPIO_InitStructure.GPIO_Pin = GATE_PORT_TX.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GATE_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GATE_PORT_RX.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
    GPIO_Init(GATE_PORT_RX.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    // Configure USART
    USART_Init(GATE_PORT, &USART_InitStructure);
    // Enable the USART
    USART_Cmd(GATE_PORT, ENABLE);
    USART_ITConfig(GATE_PORT, USART_IT_IDLE, ENABLE);
}

void Gate_ConfigInt(FunctionalState state)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = GATE_INT_CHANNEL;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_R485 >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_R485 & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;

    NVIC_Init(&NVIC_InitStructure);
}

void Gate_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(GATE_PORT, USART_IT_RXNE | USART_FLAG_TXE);

    USART_ITConfig(GATE_PORT, USART_IT_RXNE, rxState);
    USART_ITConfig(GATE_PORT, USART_IT_TC, txState);
}

void Gate_Delayms(u32 n)             //œµÕ≥—” ±n∫¡√Î
{
    n *= 0x6000;
    n++;
    while(n--);
}

void Gate_WriteBuffer(u8 *pBuffer, u32 len)
{
    u32 i = 0;

    Gate_EnableTx();
    Gate_Delayms(1);
    for(i = 0; i < len; i++)
    {
        (GATE_PORT)->DR = pBuffer[i];
        while(((GATE_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
    }
    Gate_Delayms(1);
    Gate_EnableRx();
}

BOOL Gate_UartCheckErr(void)
{
    BOOL b = FALSE;

    if(USART_GetFlagStatus(GATE_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(GATE_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        b = TRUE;
    }
    
    return b;
}

