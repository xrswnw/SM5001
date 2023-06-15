#include "AnyID_Boot_FRam.h"

FRAM_BOOTPARAMS g_sFramBootParamenter = {0};

void Fram_ReadBootParamenter(void)
{
    BOOL b = FALSE, bBackUp = FALSE;
    FRAM_BOOTPARAMS backupParams = {0};
    u8 repeat = 0;
    do{
        b = FRam_ReadBuffer(FRAME_BOOT_INFO_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
        if(b)
        {
            u16 crc1 = 0, crc2 = 0;

            crc1 = a_GetCrc((u8 *)(&g_sFramBootParamenter), (sizeof(FRAM_BOOTPARAMS)) - 2);
            crc2 = g_sFramBootParamenter.crc;

            //检测参数是否正确，如果不正确，使用默认参数操作
            if(crc1 != crc2)
            {
                b = FALSE;
            }
            else
            {
                b = TRUE;
            }
            break;
        }
        else
        {
            FRam_Delayms(50);
            repeat++;
        }
    }while(repeat < 3);

    repeat = 0;
    do{
        bBackUp = FRam_ReadBuffer(FRAME_BOOT_INFO_BACKUP_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&backupParams));
        if(bBackUp)
        {
            u16 crc1 = 0, crc2 = 0;

            crc1 = a_GetCrc((u8 *)(&backupParams), (sizeof(FRAM_BOOTPARAMS)) - 2);
            crc2 = backupParams.crc;

            //检测参数是否正确，如果不正确，使用默认参数操作
            if(crc1 != crc2)
            {
                bBackUp = FALSE;
            }
            else
            {
                bBackUp = TRUE;
            }
            break;
        }
        else
        {
            FRam_Delayms(50);
            repeat++;
        }
    }while(repeat < 3);
    
    if(bBackUp == FALSE && b == FALSE)
    {
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
        
        Fram_WriteBootParamenter();
        Fram_WriteBackupBootParamenter();
    }
    else if(bBackUp == FALSE)
    {
        Fram_WriteBackupBootParamenter();
    }
    else if(b == FALSE)
    {
        memcpy(&g_sFramBootParamenter, &backupParams, sizeof(FRAM_BOOTPARAMS));
        Fram_WriteBootParamenter();
    }
    
}

BOOL Fram_WriteBootParamenter(void)
{
    BOOL b = FALSE;
    u8 repeat = 0;

    g_sFramBootParamenter.crc = 0;
    g_sFramBootParamenter.crc = a_GetCrc((u8 *)(&g_sFramBootParamenter), (sizeof(FRAM_BOOTPARAMS)) - 2);
    do{
        b = FRam_WriteBuffer(FRAME_BOOT_INFO_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
        if(b)
        {
            b = FRam_WriteBuffer(FRAME_BOOT_INFO_BACKUP_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
        }
        if(b == FALSE)
        {
            FRam_Delayms(50);
            repeat++;
        }
        else
        {
            break;
        }
    }while(repeat < 3);

    return b;
}

BOOL Fram_WriteBackupBootParamenter(void)
{
    BOOL b = FALSE;
    u8 repeat = 0;

    g_sFramBootParamenter.crc = 0;
    g_sFramBootParamenter.crc = a_GetCrc((u8 *)(&g_sFramBootParamenter), (sizeof(FRAM_BOOTPARAMS)) - 2);
    do{
        b = FRam_WriteBuffer(FRAME_BOOT_INFO_BACKUP_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
        if(b == FALSE)
        {
            FRam_Delayms(50);
            repeat++;
        }
        else
        {
            break;
        }
    }while(repeat < 3);

    return b;
}

u32 Fram_GetUartBaudrate(void)
{

    return 115200;

}