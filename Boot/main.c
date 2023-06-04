#include "AnyID_Boot_SysCfg.h"
#include "AnyID_Boot_Uart.h"

int main(void)
{
    Sys_Init();
    while(1)
    {
        Sys_LedTask();
        Sys_BootTask();
        Sys_UartTask();
        Sys_EC20Task();
        Sys_ServerTask();

    }
}


#ifdef  DEBUG
void assert_failed(u8* file, u32 line)
{
    while(1)
    {
    }
}
#endif
