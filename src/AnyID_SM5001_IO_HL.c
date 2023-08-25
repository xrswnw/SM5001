#include "AnyID_SM5001_IO_HL.h"


const PORT_INF IO_MOTOR_CTL = {GPIOB, GPIO_Pin_9};
const PORT_INF IO_LED_CTL = {GPIOB, GPIO_Pin_3};
const PORT_INF IO_SMOKE_INPUT = {GPIOA, GPIO_Pin_6};
const PORT_INF IO_WATER_INPUT = {GPIOA, GPIO_Pin_7};
const PORT_INF IO_LIMIT_INPUT = {GPIOA, GPIO_Pin_5};
const PORT_INF IO_DOOR_INPUT = {GPIOC, GPIO_Pin_7};
const PORT_INF IO_PWR_INPUT   = {GPIOA, GPIO_Pin_11};
const PORT_INF IO_PWR_ERR_INPUT = {GPIOA, GPIO_Pin_12};
const PORT_INF IO_FAN_CTL     =     {GPIOB, GPIO_Pin_2};
const PORT_INF IO_REALY_CTL   =     {GPIOC, GPIO_Pin_6};

void IO_InitInterface()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = IO_MOTOR_CTL.Pin;
    GPIO_Init(IO_MOTOR_CTL.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = IO_LED_CTL.Pin;
    GPIO_Init(IO_LED_CTL.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = IO_FAN_CTL.Pin;
    GPIO_Init(IO_FAN_CTL.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = IO_REALY_CTL.Pin;
    GPIO_Init(IO_REALY_CTL.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = IO_SMOKE_INPUT.Pin ;
    GPIO_Init(IO_SMOKE_INPUT.Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IO_DOOR_INPUT.Pin ;
    GPIO_Init(IO_DOOR_INPUT.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =  IO_WATER_INPUT.Pin;
    GPIO_Init(IO_WATER_INPUT.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =  IO_LIMIT_INPUT.Pin;
    GPIO_Init(IO_LIMIT_INPUT.Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin =  IO_PWR_INPUT.Pin;
    GPIO_Init(IO_PWR_INPUT.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =  IO_PWR_ERR_INPUT.Pin;
    GPIO_Init(IO_PWR_ERR_INPUT.Port, &GPIO_InitStructure);
    
    
}


