#include "AnyID_SM5001_IO.h"

IO_INFO g_sIoInfo = {0};

void IO_Init()
{
    g_sIoInfo.eleMacTick = 0;
    IO_InitInterface();
}
//sersorState
u8 IO_Sersor_Chk(u8 tUp, u8 tDown, u8 limitTempr)
{
    u8 State = 0;
    
    if(IO_Door_Chk())
    {
         State |=  IO_SENSOR_STAT_DOOR_OPEN;
    } 
    if(IO_Smoke_Chk())
    {
         State |= IO_SENSOR_STAT_SMOKE; 
    }

     if(IO_Water_Chk())
    {
         State |= IO_SENSOR_STAT_WATER;
    }
     if(IO_Pwr_Chk())
    {
         State |= IO_SENSOR_STAT_THUNDER;
    }

     if(IO_Pwr_Err_Chk())
    {
         State |= IO_SENSOR_STAT_THUNDER_ERR;
    }

	if(g_sIoInfo.sersorState & IO_SENSOR_STAT_TEMPR_CHANGE)
	{
		if((tUp >= limitTempr - IO_TEMPR_BORDER) || (tDown >= limitTempr - IO_TEMPR_BORDER))			//风扇关闭在界限值低两度左右
		{
			State |= IO_SENSOR_STAT_TEMPR_CHANGE;
		}	
	}
	else
	{
		if((tUp >= limitTempr) || (tDown >= limitTempr))
		{
			State |= IO_SENSOR_STAT_TEMPR_CHANGE;
		}	
	}
	/*
	if((tUp >= limitTempr) || (tDown >= limitTempr))
    {
		State |= IO_SENSOR_STAT_TEMPR_CHANGE;
    }

*/

    return State;
}



