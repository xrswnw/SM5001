#ifndef _ANYID_SM5001_AD_
#define _ANYID_SM5001_AD_

#include "AnyID_SM5001_AD_HL.h"

#define WIN_SAMPLE_NUM                  16
typedef struct temperatureInfo{
    u32 sum;
    u32 avg;
    u32 index;
    u32 buffer[WIN_SAMPLE_NUM];
    int t;
    u32 alarmTick;
}WINAVG_INFO;

#define AD_CHECK_ALARM_TME              1000    //超过10s
#define AD_ALARM_THRD                   2       //回退的时候需要一个阈值空间

void Win_CalAvg(WINAVG_INFO *pInfo, u32 value);
void AD_GetTmpr(WINAVG_INFO *pInfo);
BOOL AD_CheckAlarm(WINAVG_INFO *pInfo, int alarmValue);
#endif