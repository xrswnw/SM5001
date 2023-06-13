#include "AnyID_Boot_FLASH_HL.h"

const PORT_INF FLASH_CS_COM     = {GPIOB, GPIO_Pin_12};
const PORT_INF FLASH_PORT_SPI   = {GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15};
void Flash_InitInterface(void)
{
    SPI_InitTypeDef   SPI_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //cs
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = FLASH_CS_COM.Pin;
    GPIO_Init(FLASH_CS_COM.Port, &GPIO_InitStructure);
    Flash_CsHigh();

    //SPI
    GPIO_InitStructure.GPIO_Pin = FLASH_PORT_SPI.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(FLASH_PORT_SPI.Port, &GPIO_InitStructure);

    SPI_Cmd(FLASH_PORT, DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex ;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master ;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b ;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low ;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge ;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft ;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  // 最大支持20M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB ;
    SPI_InitStructure.SPI_CRCPolynomial = 7 ;
    SPI_Init(FLASH_PORT, &SPI_InitStructure);

    SPI_Cmd(FLASH_PORT, ENABLE);
}

u8 Flash_WriteByte(u8 byte)
{
    if(SPI_I2S_GetFlagStatus(FLASH_PORT, SPI_I2S_FLAG_RXNE) == SET)
    {
        SPI_I2S_ReceiveData(FLASH_PORT);
    }

    while(SPI_I2S_GetFlagStatus(FLASH_PORT, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(FLASH_PORT, byte);
    while(SPI_I2S_GetFlagStatus(FLASH_PORT, SPI_I2S_FLAG_RXNE) == RESET);

    return SPI_I2S_ReceiveData(FLASH_PORT);
}


void Flash_Delayms(u32 n)
{
    n *= 0x3800;        //72MHZ
    n++;
    while(n--);
}

void Flash_Delay16us(u32 us)
{
    u32 delay = 0;
    while(us--)
    {
        delay = 0x120;
        while(delay--);
    }
}