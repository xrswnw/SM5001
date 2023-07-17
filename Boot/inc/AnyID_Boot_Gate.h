#ifndef _ANYID_BOOT_GATE_
#define _ANYID_BOOT_GATE_

#include "AnyID_Boot_Gate_HL.h"
#include "AnyID_Boot_Uart.h"
#include "AnyID_Boot_FlashM25.h"
extern UART_RCVFRAME g_sGateRcvFrame;

#define GATE_BOOT_SECTOR_ADDR           0x08004000
#define GATE_BOOT_SECTOR_SIZE           1024
#define GATE_BOOT_SRC_ADDR              0x08020000 //--------------≤‚ ‘


#define GATE_RX_STAT_IDLE           0
#define GATE_RX_STAT_OVER           1
#define GATE_RX_STAT_END            2

#define GATE_UPDATA_OP_TICK         5
typedef struct gateFrame{
    u8 buffer[UART_BUFFER_MAX_LEN];
    u16 len;
    u8 state;
}GATE_FRAME;

#define GATE_BOOT_TO_TIM            1000

#define GATE_OP_STAT_IDLE           0
#define GATE_OP_STAT_TX             1
#define GATE_OP_STAT_RX             2
#define GATE_OP_STAT_WAIT           3
#define GATE_OP_STAT_TO             4
#define GATE_OP_STAT_STEP           0x80
typedef struct gateBootInfo{
    GATE_FRAME txFrame;
    u16 sectorNum;
    u16 sectorIndex;
    u16 slvIndex;
    u8 bin[GATE_BOOT_SECTOR_SIZE];
    u32 tick;
    u8 cmd;
    u8 repat;
    u8 state;
}GATE_BOOTINFO;
extern GATE_BOOTINFO g_sGateBootInfo;

#define Gate_ClearBootInfo()        do{ \
                                        g_sGateBootInfo.txFrame.len = 0;\
                                        g_sGateBootInfo.sectorIndex = 0;\
                                        g_sGateBootInfo.tick = 0;\
                                        g_sGateBootInfo.cmd = 0;\
                                        g_sGateBootInfo.state = 0;\
                                    }while(0);

void Gate_StepBoot(GATE_BOOTINFO *pBootInfo, UART_RCVFRAME *pRcvFrame);
void Gate_ProcessBootFrame(UART_RCVFRAME *pRcvFrame);

#endif
