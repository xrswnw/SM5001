#include "AnyID_Boot_FlashM25.h"

u8 g_nFlashUpData[FLASH_UPDATA_LEN] = {0};
void Flash_Demo()
{
    Flash_ReadBuffer(FLASH_DATA_OPEN_ADDR + FLASE_PAGE_SIZE , FLASH_UPDATA_LEN, g_nFlashUpData);
    
}