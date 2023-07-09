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
    }
    else
    {
        g_sIoInfo.state &= ~IO_STAT_FIRE;
    }
     if(IO_Water_Chk())
    {
         g_sIoInfo.state |= IO_STAT_WATER;
    }
    else
    {
        g_sIoInfo.state &= ~IO_STAT_WATER;
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


u8 IO_Sersor_Chk()
{
    u8 sersorState = 0;
    
     if(IO_Door_Chk())
    {
         sersorState |=  IO_SENSOR_STAT_DOOR;
    } 
    if(IO_Smoke_Chk())
    {
         sersorState |= IO_SENSOR_STAT_SMOKE; 
    }

     if(IO_Water_Chk())
    {
         sersorState |= IO_SENSOR_STAT_WATER;
    }
     if(IO_Pwr_Chk())
    {
         sersorState |= IO_SENSOR_STAT_THUNDER;
    }

     if(IO_Pwr_Err_Chk())
    {
         sersorState |= IO_SENSOR_STAT_THUNDER_ERR;
    }



    return sersorState;
}



