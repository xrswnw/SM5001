#include "AnyID_Boot_FlashM25.h"

u8 g_nFlashUpData[FLASH_UPDATA_LEN] = {0};
u8 g_nFlashUpTempData[FLASH_UPDATA_LEN] = {0};
void Flash_Demo()
{
    Flash_ReadBuffer(FLASH_DATA_OPEN_ADDR , FLASH_UPDATA_LEN, g_nFlashUpData);
    
}