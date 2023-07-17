#include "AnyID_SM5001_IO.h"

IO_INFO g_sIoInfo = {0};

void IO_Init()
{
    g_sIoInfo.eleMacTick = 0;
    IO_InitInterface();
}

u8 IO_Sersor_Chk(u8 tUp, u8 tDown, u8 limitTempr)
{
    u8 sersorState = 0;
    
    if(IO_Door_Chk())
    {
         sersorState |=  IO_SENSOR_STAT_DOOR_OPEN;
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

	if((tUp >= limitTempr) || (tDown >= limitTempr))
    {
		sersorState |= IO_SENSOR_STAT_TEMPR_CHANGE;
    }



    return sersorState;
}



