#include "AnyID_SM5001_AD.h"

#define AD_TMPR_LV_NUM          90   
#define AD_TMPR_MIN             -20  
u16 const g_aADTmprBuf[AD_TMPR_LV_NUM] = {
62, 66, 70, 74, 78, 83, 88, 93, 98, 103, 
109, 115, 121, 128, 135, 142, 150, 158, 
166, 174, 183, 193, 202, 213, 223, 234, 
246, 257, 270, 283, 296, 310, 324, 339, 
354, 370, 387, 404, 422, 440, 459, 479, 
499, 520, 541, 564, 587, 610, 634, 659, 
685, 712, 739, 767, 795, 825, 855, 885, 
917, 949, 982, 1016, 1051, 1086, 1122, 1158, 
1196, 1234, 1273, 1312, 1352, 1393, 1434, 1476, 
1519, 1562, 1606, 1650, 1695, 1740, 1786, 1833, 
1879, 1926, 1974, 2022, 2070, 2119, 2168, 2217,
};
void Win_CalAvg(WINAVG_INFO *pInfo, u32 value)
{
    //16´°¿ÚÆ½»¬ÂË²¨
    pInfo->index++;
    pInfo->sum -= pInfo->buffer[pInfo->index & 0x0F];
    if(pInfo->index == 0)
    {
        pInfo->index = WIN_SAMPLE_NUM;
    }
    pInfo->buffer[pInfo->index & 0x0F] = value;
    pInfo->sum += value;
    if(pInfo->index >= WIN_SAMPLE_NUM)
    {
        pInfo->avg = pInfo->sum >> 4;
    }
    else
    {
        pInfo->avg = pInfo->sum / pInfo->index;
    }
}

void AD_GetTmpr(WINAVG_INFO *pInfo)
{
    u8 i = 0;
    pInfo->t = AD_TMPR_MIN;
    for(i = 0; i < AD_TMPR_LV_NUM; i++)
    {
        if(pInfo->avg < g_aADTmprBuf[i])
        {
            break;
        }
    }
    pInfo->t += i;
}

BOOL AD_CheckAlarm(WINAVG_INFO *pInfo, int alarmValue)
{
    BOOL b = TRUE;
    if(pInfo->t > alarmValue)
    {
        pInfo->alarmTick++;
        if(pInfo->alarmTick < AD_CHECK_ALARM_TME)
        {
            b = FALSE;
        }
    }
    else if(pInfo->alarmTick > 0)
    {
        if(pInfo->t + AD_ALARM_THRD < alarmValue)
        {
            pInfo->alarmTick = 0;
            b = FALSE;
        }
    }
    else
    {
        b = FALSE;
    }
    return b;
}