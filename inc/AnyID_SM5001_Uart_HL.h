#ifndef _ANYID_SM5001_UART_HL_H
#define _ANYID_SM5001_UART_HL_H

#include "AnyID_SM5001_Config.h"

#define UART_PORT                       USART3
//#define Uart_IRQHandler                 USART3_IRQHandler

#define UART_BAUDRARE                   115200

#define Uart_ReadByte()                 ((u16)(UART_PORT->DR & (u16)0x01FF))
void Uart_InitInterface(u32 baudrate);

void Uart_ConfigInt(void);
void Uart_EnableInt(FunctionalState rxState, FunctionalState txState);

void Uart_WriteByte(u8 ch);
void Uart_WriteBuffer(u8 *pFrame, u16 len);

#endif
