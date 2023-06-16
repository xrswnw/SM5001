#ifndef _ANYID_SM5001_SOUND_HL_H
#define _ANYID_SM5001_SOUND_HL_H

#include "AnyID_SM5001_Config.h"

#define SOUND_PORT                      USART1
#define SOUND_IRQHandler                USART1_IRQHandler
                                             
#define SOUND_BAUDRARE                  9600

#define Sound_ChkTxOver()                 while(((SOUND_PORT)->SR & USART_FLAG_TC) == (u16)RESET)



extern const PORT_INF SOUND_PORT_CTL;
#define Sounde_Close()                  SOUND_PORT_CTL.Port->BRR = SOUND_PORT_CTL.Pin
#define Sounde_Open()                   SOUND_PORT_CTL.Port->BSRR = SOUND_PORT_CTL.Pin


#define Sound_ReadByte()                  ((u16)(SOUND_PORT->DR & (u16)0x01FF))
void Sound_InitInterface(u32 baudrate);
void Sound_ConfigInt(void);
void Sound_EnableInt(FunctionalState rxState, FunctionalState txState);

void Sound_Delayms(u32 n);
void Sound_Delay100us(u32 n);
void Sound_WriteByte(u8 ch);
void Sound_WriteBuffer(u8 *pFrame, u16 len);


#endif