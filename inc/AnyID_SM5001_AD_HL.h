#ifndef _ANYID_SM5001_AD_HL_
#define _ANYID_SM5001_AD_HL_

#include "AnyID_SM5001_Config.h"

//AIN0-ISO14443AÄ£ÄâÁ¿ÊäÈë
#define AD_CHANNEL_NUM              2
#define AD_TEMP1_CHANNEL            ADC_Channel_0
#define AD_TEMP2_CHANNEL            ADC_Channel_1
#define AD_TIME                     ADC_SampleTime_239Cycles5
#define AD_PORT                     ADC1
#define AD_VALUE_MASK               0xFFF

#define AD_DMA_CHANNEL              DMA1_Channel1

void AD_InitInterface(void);
void AD_Init(void);
void AD_DmaInit(void);
#define AD_Start()                  (AD_PORT)->CR2 |= CR2_EXTTRIG_SWSTART_Set

extern u16 g_aAdBuffer[AD_CHANNEL_NUM];
#define AD_GetTemper1Value()        (g_aAdBuffer[0] & AD_VALUE_MASK)
#define AD_GetTemper2Value()        (g_aAdBuffer[1] & AD_VALUE_MASK)
#endif

