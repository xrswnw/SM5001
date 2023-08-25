#ifndef _ANYID_SM5001_IO_H
#define _ANYID_SM5001_IO_H

#include "AnyID_SM5001_IO_HL.h"
#include "AnyID_SM5001_AD.h"


#define IO_ELEMAC_OPEN          0x01
#define IO_LED_OPEN             0x02

#define IO_ELEMAC_TIME          2000


#define IO_SMOKE_SET            0x01
#define IO_WATER_SET            0x02
#define IO_DOOR_SET             0x04
#define IO_SENSOR_SET             0xFF


#define IO_STAT_DOOR             0x0001
#define IO_STAT_FIRE             0x0002
#define IO_STAT_FAN              0x0004
#define IO_STAT_WATER            0x0008
#define IO_STAT_THUNDER          0x0010
#define IO_STAT_THUNDER_ATTCK    0x0020
#define IO_STAT_LIGHT            0x0040
#define IO_STAT_RELAY            0x0080
#define IO_STAT_SOUND_FAIL       0x0100
#define IO_STAT_ELECT_FAIL       0x0200


#define IO_FLAG_DOOR             0x0001
#define IO_FLAG_FIRE             0x0002
#define IO_FLAG_FAN              0x0004
#define IO_FLAG_WATER            0x0008
#define IO_FLAG_THUNDER          0x0010
#define IO_FLAG_THUNDER_ATTCK    0x0020
#define IO_FLAG_LIGHT            0x0040
#define IO_FLAG_RELAY            0x0080
#define IO_FLAG_SOUND_FAIL       0x0100
#define IO_FLAG_ELECT_FAIL       0x0200

#define IO_FLAG_TEMPR_UP         0x0001
#define IO_FLAG_FIRE             0x0002
#define IO_TEMPR_BORDER				2


#define IO_SENSOR_STAT_DOOR_OPEN       0x01
#define IO_SENSOR_STAT_SMOKE           0x02
#define IO_SENSOR_STAT_WATER           0x04
#define IO_SENSOR_STAT_THUNDER         0x08
#define IO_SENSOR_STAT_THUNDER_ERR     0x10
#define IO_SENSOR_STAT_TEMPR_CHANGE    0x20
#define IO_SENSOR_STAT_RFID_OPEN       0x40

#define IO_SENSOR_STAT_RFID            0x08
#define IO_SENSOR_STAT_TEMPR           0x40


#define IO_DEVICE_STAT_FAN             0x01
#define IO_DEVICE_STAT_RELAY           0x02
#define IO_DEVICE_STAT_DOOR            0x04
#define IO_DEVICE_STAT_LED             0x08

#define IO_DEVICE_MASK_FLAG_FAN         0x01
#define IO_DEVICE_MASK_FLAG_RELAY       0x02
#define IO_DEVICE_MASK_FLAG_DOOR        0x04
#define IO_DEVICE_MASK_FLAG_LED         0x08


typedef struct ioInfo{
    u8 ioState ;
    u8 eleMacTick ;  
    u8 warnValue;
    u8 deviceState;
    u8 sersorState;
    u8 tick;
    u8 maskFlag;
    u8 tempState1;
    u8 tempFlag;
    u32 state;
    u32 tempState;
    u32 flag;
    u32 ctrlDoorTick;
}IO_INFO;
extern IO_INFO g_sIoInfo;



void IO_Init();

u8 IO_Sersor_Chk(u8 tUp, u8 tDown, u8 limitTempr);

#endif
