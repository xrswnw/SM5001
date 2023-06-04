#ifndef _ANYID_SM5001_W232_HL_
#define _ANYID_SM5001_W232_HL_

#include "AnyID_SM5001_Config.h"

#define W232_PORT                       UART5
#define W232_BAUDRARE                   115200
#define W232_IRQHandler                 UART5_IRQHandler
#define W232_DR_ADDRESS                 (W232_PORT->DR)


extern const PORT_INF W232_PORT_PWR;
#define W232_PowerOn()          W232_PORT_PWR.Port->BSRR = W232_PORT_PWR.Pin
#define W232_PowerOff()         W232_PORT_PWR.Port->BRR = W232_PORT_PWR.Pin

extern const PORT_INF W232_PORT_KEY;
#define W232_KeyHigh()          W232_PORT_KEY.Port->BSRR = W232_PORT_KEY.Pin
#define W232_KeyLow()           W232_PORT_KEY.Port->BRR = W232_PORT_KEY.Pin


extern const PORT_INF W232_PORT_CTRL;
#define W232_CtrlHigh()         W232_PORT_CTRL.Port->BSRR = W232_PORT_CTRL.Pin
#define W232_CtrlLow()          W232_PORT_CTRL.Port->BRR = W232_PORT_CTRL.Pin

/*
extern const PORT_INF W232_PORT_STAT;
#define W232_GetState()         ((W232_PORT_STAT.Port)->IDR & (W232_PORT_STAT.Pin))
*/

#define W232_ReadByte()                 ((u16)(W232_PORT->DR & (u16)0x01FF))

void W232_InitInterface(u32 baudrate);
void W232_ConfigInt(void);
void W232_ConfigDma(u8 *pTxBuffer, u32 size);
void W232_EnableInt(FunctionalState rxState, FunctionalState txState);

void W232_WriteByte(u8 ch);
void W232_Delayms(u32 n);
void W232_WriteBuffer(u8 *pFrame, u16 len);
void W232_WriteCmd(char *str);

#endif

