#ifndef _ANYID_BOOT_GATE_HL_
#define _ANYID_BOOT_GATE_HL_

#include "AnyID_Boot_Config.h"

#define GATE_FRAME_LEN                      (1024 + 32)

#define GATE_PORT                           UART4
#define GATE_BAUDRARE                       115200
#define Gate_IRQHandler                     UART4_IRQHandler
#define Gate_ChkTxOver()                    while(((GATE_PORT)->SR & USART_FLAG_TC) == (u16)RESET)

extern const PORT_INF GATE_PORT_CTRL;
#define Gate_EnableRx()                     (GATE_PORT_CTRL.Port->BRR  = GATE_PORT_CTRL.Pin)
#define Gate_EnableTx()                     (GATE_PORT_CTRL.Port->BSRR = GATE_PORT_CTRL.Pin)

#define Gate_ReadByte()                     ((u16)(GATE_PORT->DR & (u16)0x01FF))

                                                
#define GATE_SR_IDLE                        0x0010  

void Gate_InitInterface(u32 baudrate);
void Gate_EnableInt(FunctionalState rxState, FunctionalState txState);
void Gate_ConfigInt(FunctionalState state);
void Gate_WriteCmd(char *str);
void Gate_Delayms(u32 n);             //œµÕ≥—” ±n∫¡√Î
void Gate_WriteBuffer(u8 *pBuffer, u32 len);
void Gate_WriteByte(u8 ch);
BOOL Gate_UartCheckErr(void);
#endif



