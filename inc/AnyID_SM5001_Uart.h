#ifndef _ANYID_SM5001_UART_H
#define _ANYID_SM5001_UART_H

#include "AnyID_SM5001_Uart_HL.h"
#include "AnyID_Uart_Receive.h"

extern UART_RCVFRAME g_sUartRcvTempFrame;
extern UART_RCVFRAME g_sRfidRcvTempFrame;
void Uart_Init();
#endif
