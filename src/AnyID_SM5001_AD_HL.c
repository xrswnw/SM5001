#include "AnyID_SM5001_AD_HL.h"

u16 g_aAdBuffer[AD_CHANNEL_NUM] = {0};

const PORT_INF AD_TEMP1_COM = {GPIOA, GPIO_Pin_0};
const PORT_INF AD_TEMP2_COM = {GPIOA, GPIO_Pin_1};
void AD_InitInterface(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = AD_TEMP1_COM.Pin;
    GPIO_Init(AD_TEMP1_COM.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = AD_TEMP2_COM.Pin;
    GPIO_Init(AD_TEMP2_COM.Port, &GPIO_InitStructure);
}

void AD_Init(void)
{
    //一般5ms采集一次即可
    ADC_InitTypeDef ADC_InitStructure;

    ADC_DeInit(AD_PORT);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //独立工作模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                           //扫描方式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                     //连续转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //外部触发禁止
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = AD_CHANNEL_NUM;                    //用于转换的通道数
    ADC_Init(AD_PORT, &ADC_InitStructure);

    ADC_RegularChannelConfig(AD_PORT, AD_TEMP1_CHANNEL, 1, AD_TIME);
    ADC_RegularChannelConfig(AD_PORT, AD_TEMP2_CHANNEL, 2, AD_TIME);
     
    ADC_Cmd(AD_PORT, ENABLE);
    
    ADC_DMACmd(AD_PORT, ENABLE);

    //校准
    ADC_ResetCalibration(AD_PORT);
    while(ADC_GetResetCalibrationStatus(AD_PORT));
    ADC_StartCalibration(AD_PORT);
    while(ADC_GetCalibrationStatus(AD_PORT));
    
    DMA_Cmd(AD_DMA_CHANNEL, ENABLE);
    ADC_Cmd(AD_PORT, ENABLE);
    AD_Start(); 
}

void AD_DmaInit(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit(AD_DMA_CHANNEL);												//将DMA的通道1寄存器重设为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(AD_PORT->DR));		//DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)g_aAdBuffer;		        //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//内存作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize = AD_CHANNEL_NUM;				        //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				    //内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		                        //工作在循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                        //DMA通道 x拥有高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;			                    //DMA通道x没有设置为内存到内存传输
    
    DMA_Init(AD_DMA_CHANNEL, &DMA_InitStructure);			                    //根据DMA_InitStruct中指定的参数初始化DMA的通道
    DMA_Cmd(AD_DMA_CHANNEL, ENABLE);
}


