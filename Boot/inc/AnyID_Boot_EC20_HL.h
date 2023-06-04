#ifndef _ANYID_BOOT_EC20_HL_H
#define _ANYID_BOOT_EC20_HL_H

#include "AnyID_Boot_Config.h"

#define EC20_PORT                       UART5
#define EC20_BAUDRARE                   115200
#define EC20_IRQHandler                 UART5_IRQHandler
#define EC20_DR_ADDRESS                 (EC20_PORT->DR)


extern const PORT_INF EC20_PORT_PWR;
#define EC20_PowerOn()          EC20_PORT_PWR.Port->BSRR = EC20_PORT_PWR.Pin
#define EC20_PowerOff()         EC20_PORT_PWR.Port->BRR = EC20_PORT_PWR.Pin

extern const PORT_INF EC20_PORT_KEY;
#define EC20_KeyHigh()          EC20_PORT_KEY.Port->BSRR = EC20_PORT_KEY.Pin
#define EC20_KeyLow()           EC20_PORT_KEY.Port->BRR = EC20_PORT_KEY.Pin


extern const PORT_INF EC20_PORT_CTRL;
#define EC20_CtrlHigh()         EC20_PORT_CTRL.Port->BSRR = EC20_PORT_CTRL.Pin
#define EC20_CtrlLow()          EC20_PORT_CTRL.Port->BRR = EC20_PORT_CTRL.Pin

#define EC20_ReadByte()                 ((u16)(EC20_PORT->DR & (u16)0x01FF))

void EC20_InitInterface(u32 baudrate);
void EC20_ConfigInt(void);
void EC20_ConfigDma(u8 *pTxBuffer, u32 size);
void EC20_EnableInt(FunctionalState rxState, FunctionalState txState);

void EC20_WriteByte(u8 ch);
void EC20_Delayms(u32 n);
void EC20_WriteBuffer(u8 *pFrame, u16 len);
void EC20_WriteCmd(char *str);

#endif

