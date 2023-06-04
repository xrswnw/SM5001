#ifndef _ANYID_BOOT_UART_HL_
#define _ANYID_BOOT_UART_HL_

#include "AnyID_Boot_Config.h"

#define UART_PORT                       USART3
#define Uart_IRQHandler                 USART3_IRQHandler

#define Uart_ChkTxOver()                while(((UART_PORT)->SR & USART_FLAG_TC) == (u16)RESET)
extern const PORT_INF UART_PORT_CTRL;
#define Uart_EnableRx()                     (UART_PORT_CTRL.Port->BRR  = UART_PORT_CTRL.Pin)
#define Uart_EnableTx()                     (UART_PORT_CTRL.Port->BSRR = UART_PORT_CTRL.Pin)


#define UART_BAUDRARE                   115200
#define Uart_ReadByte()                 ((u16)(UART_PORT->DR & (u16)0x01FF))

void Uart_InitInterface(u32 baudrate);
void Uart_ConfigInt(void);
void Uart_EnableInt(FunctionalState rxState, FunctionalState txState);
void Uart_WriteCmd(char *str);
void Uart_WriteByte(u8 ch);
void Uart_WriteBuffer(u8 *pFrame, u16 len);
#endif
