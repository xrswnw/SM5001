#ifndef _ANYID_BOOT_FLASHM25_
#define _ANYID_BOOT_FLASHM25_

#include "AnyID_FlashM25.h"


#define FLASH_UPDATA_LEN                1024       
#define FLASH_DOWNLOAD_LEN              1044     
#define FLASH_DATA_OPEN_ADDR            0


extern u8 g_nFlashUpData[FLASH_UPDATA_LEN]; 
extern u8 g_nFlashUpTempData[FLASH_UPDATA_LEN]; 
extern u8 g_nFlashDownData[FLASH_DOWNLOAD_LEN]; 
extern u8 g_nFlashDownTempData[FLASH_UPDATA_LEN]; 




void Flash_Demo();
#endif

