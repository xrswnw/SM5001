#include "AnyID_SM5001_SysCfg.h"

int main(void)
{
    Sys_Init();
    while(1)
    {
        Sys_LedTask();
        Sys_IOTask();
        //Sys_UartTask();
        Sys_HeratTask();
        Sys_ElectTask();
        Sys_W232Task();
        Sys_ServerTask();
        Sys_SoundTask();
        
        Sys_AdTask();
        Sys_GateTask();
        Sys_WaterTask();
        Sys_TestTask();
    }
}






