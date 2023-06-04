#include "AnyID_SM5001_IO.h"

IO_INFO g_sIoInfo = {0};

void IO_Init()
{
    g_sIoInfo.eleMacTick = 0;
    IO_InitInterface();
}




u8 IO_Stat_Chk()
{
    if(IO_Smoke_Chk())
    {
         g_sIoInfo.state |= IO_STAT_FIRE; 
         g_sIoInfo.flag |= IO_FLAG_FIRE;
    }
    else
    {
        g_sIoInfo.state &= ~IO_STAT_FIRE;
        g_sIoInfo.flag &= ~IO_FLAG_FIRE;
    }
     if(IO_Water_Chk())
    {
         g_sIoInfo.state |= IO_STAT_WATER;
    }
    else
    {
        g_sIoInfo.state &= ~IO_STAT_WATER;
    }
     if(IO_Door_Chk())
    {
         g_sIoInfo.state |=  IO_STAT_DOOR;
    }
    else
    {
        g_sIoInfo.state &= ~IO_STAT_DOOR;
    }
     if(IO_Pwr_Chk())
    {
         g_sIoInfo.state |= IO_STAT_THUNDER_ATTCK;
    }
    else
    {
        g_sIoInfo.state &= ~IO_STAT_THUNDER_ATTCK;
    }
     if(IO_Pwr_Err_Chk())
    {
         g_sIoInfo.state |= IO_STAT_THUNDER;
    }
    else
    {
        g_sIoInfo.state &= ~IO_STAT_THUNDER;
    }
  return g_sIoInfo.state;

}



