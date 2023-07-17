#ifndef _ANYID_SM5001_IO_HL_H
#define _ANYID_SM5001_IO_HL_H

#include "AnyID_SM5001_Config.h"



 
extern const PORT_INF IO_MOTOR_CTL;
#define IO_Door_Close()                  IO_MOTOR_CTL.Port->BRR = IO_MOTOR_CTL.Pin
#define IO_Door_Open()                   IO_MOTOR_CTL.Port->BSRR = IO_MOTOR_CTL.Pin

extern const PORT_INF IO_LED_CTL;
#define IO_Led_Close()                  IO_LED_CTL.Port->BRR = IO_LED_CTL.Pin
#define IO_Led_Open()                   IO_LED_CTL.Port->BSRR = IO_LED_CTL.Pin

extern const PORT_INF IO_FAN_CTL;
#define IO_Fan_Close()                  IO_FAN_CTL.Port->BRR = IO_FAN_CTL.Pin
#define IO_Fan_Open()                   IO_FAN_CTL.Port->BSRR = IO_FAN_CTL.Pin

extern const PORT_INF IO_REALY_CTL;
#define IO_Realy_Close()                  IO_REALY_CTL.Port->BRR = IO_REALY_CTL.Pin
#define IO_Realy_Open()                   IO_REALY_CTL.Port->BSRR = IO_REALY_CTL.Pin

extern const PORT_INF IO_SMOKE_INPUT;
#define IO_Smoke_Chk()              ((IO_SMOKE_INPUT.Port->IDR & IO_SMOKE_INPUT.Pin) == (uint32_t)Bit_RESET)

extern const PORT_INF IO_WATER_INPUT;
#define IO_Water_Chk()              ((IO_WATER_INPUT.Port->IDR & IO_WATER_INPUT.Pin) != (uint32_t)Bit_RESET)

extern const PORT_INF IO_LIMIT_INPUT;
#define IO_Rfid_Chk()              ((IO_LIMIT_INPUT.Port->IDR & IO_LIMIT_INPUT.Pin)  != (uint32_t)Bit_RESET)

extern const PORT_INF IO_LIMIT_INPUT;
#define IO_Door_Chk()              (IO_LIMIT_INPUT.Port->IDR & IO_LIMIT_INPUT.Pin)

extern const PORT_INF IO_PWR_INPUT;
#define IO_Pwr_Chk()              ((IO_PWR_INPUT.Port->IDR & IO_PWR_INPUT.Pin) != (uint32_t)Bit_RESET)

extern const PORT_INF IO_PWR_ERR_INPUT;
#define IO_Pwr_Err_Chk()              ((IO_PWR_ERR_INPUT.Port->IDR & IO_PWR_ERR_INPUT.Pin) != (uint32_t)Bit_RESET)




void IO_InitInterface();

#endif
