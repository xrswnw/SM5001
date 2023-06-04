#ifndef _ANYID_SM5001_SYSCFG_H
#define _ANYID_SM5001_SYSCFG_H

#include "AnyID_SM5001_SysTick_HL.h"
#include "AnyID_SM5001_RTC.h"
#include "AnyID_SM5001_Device.h"
#include "AnyID_SM5001_WDG_HL.h"
extern u32 g_nSysState;
#define SYS_NULL_TICK                   0

#define SYS_STAT_IDLE                   0x00000001	        //
#define SYS_STAT_RUNLED                 0x00000002
#define SYS_STAT_UART_TX                0x00000010
#define SYS_STAT_UART_WAIT              0x00000020
#define SYS_STAT_UART_RX                0x00000040
#define SYS_STAT_LTEDTU                 0x00000080
#define SYS_STAT_ELEMAC_CTL             0x00000100
#define SYS_STAT_LED_CTL                0x00000200
#define SYS_STAT_SENSOR_CHK             0x00000400
#define SYS_STAT_TEMP_CHK               0x00000800
#define SYS_STAT_MQTT_ACCESS            0x00001000    
#define SYS_STAT_MQTT_RQUEST            0x00002000    
#define SYS_STAT_MQTT_HEART             0x00004000  
#define SYS_STAT_AD                     0x00008000
#define SYS_STAT_MQTT_OFFLINE           0x00010000
#define SYS_STAT_VOICE_CHK              0x00020000
#define SYS_STAT_GATE_STAT_CHK          0x00040000
#define SYS_STAT_WATER_CTR              0x00080000
#define SYS_STAT_KEY_CHK                0x00100000
#define SYS_STAT_TEST_MODE              0x00200000
#define SYS_STAT_TEST_TIM               0x00400000

//²âÊÔ
#define SYS_STAT_FIRE_TEST               0x80000000
#define SYS_STAT_SOUND_TEST              0x40000000
#define SYS_STAT_LTE_TEST                0x20000000
#define SYS_STAT_GATE_TEST               0x10000000

#define SYS_LED_DELAY                   20
#define SYS_CHECK_CFGMODE_TIM           200

extern const PORT_INF SYS_RUN_LED;
#define Sys_LedOff()                  SYS_RUN_LED.Port->BSRR = SYS_RUN_LED.Pin
#define Sys_LedOn()                   SYS_RUN_LED.Port->BRR = SYS_RUN_LED.Pin


void Sys_Delayms(u32 n);
void Sys_CfgClock(void);
void Sys_CfgPeriphClk(FunctionalState state);
void Sys_CfgNVIC(void);

void Sys_CtrlIOInit(void);
u8 Sys_GetDevAddr(void);

void Sys_Init(void);
void Sys_SoundTask();
void Sys_LedTask(void);
void Sys_UartTask(void);

void Sys_HeratTask();
void Sys_ServerTask(void);
void Sys_IOTask();
void Sys_ElectTask();
void Sys_W232Task(void);
void Sys_ServerTask(void);
void Sys_GateTask(void);
void Sys_AdTask();
void Sys_WaterTask();
void Sys_TestTask();

#endif
