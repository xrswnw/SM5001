#include "AnyID_Boot_Flash.h"


FRAM_BOOTPARAMS g_sFramBootParamenter = {0};
FRAM_DEVPARAMS g_sFramDevParams;;

void FRam_InitInterface(void)
{
    
}

BOOL FRam_ReadBuffer(u32 addr, u16 len, u8 *pBuffer)
{
    memcpy(pBuffer, (u8 *)addr, len);
    return TRUE;
}
BOOL FRam_WriteBuffer(u32 addr, u16 len, u8 *pBuffer)
{
    u8 i = 0;
    BOOL b = FALSE;
    u16 *pDevicePar = NULL;
    FLASH_Status flashStatus = FLASH_BUSY;

    pDevicePar = (u16 *)pBuffer;
    len >>= 1;
    len += 1;
    FLASH_Unlock();
    flashStatus = FLASH_ErasePage(addr);
    if(flashStatus == FLASH_COMPLETE)
    {
        for(i = 0; i < len; i++)
        {
            if(flashStatus != FLASH_COMPLETE)
            {
                break;
            }
            flashStatus = FLASH_ProgramHalfWord(addr + (i << 1), pDevicePar[i]);
        }

        if(i == len)
        {
            b = TRUE;
        }
    }
    FLASH_Lock();
    return b;
}

void FRam_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}

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
        g_sFramBootParamenter.br = FRAM_BR_9600;                        
        
        Fram_WriteBootParamenter();
        Fram_WriteBackupBootParamenter();
    }
    else if(bBackUp == FALSE)
    {
        Fram_WriteBackupBootParamenter();   //b==TRUE表示g_sFramBootParamenter数据正确
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

void Fram_ReadDevParams()
{
    BOOL b = FALSE, bBackup = FALSE;
    //读取参数
    b = FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
    if(b)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sFramDevParams), (sizeof(FRAM_DEVPARAMS)) - 4);
        crc2 = g_sFramDevParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }
    //读取备份参数
    bBackup = FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
    if(bBackup)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sFramDevParams), (sizeof(FRAM_DEVPARAMS)) - 4);
        crc2 = g_sFramDevParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }

    //如果参数区和备份区都错误，恢复默认参数
    if((b == FALSE && bBackup == FALSE))
    {
        memset(&g_sFramDevParams, 0, sizeof(FRAM_DEVPARAMS));
        g_sFramDevParams.br = FRAM_BR_9600;
        g_sFramDevParams.crc = a_GetCrc((u8 *)(&g_sFramDevParams), (sizeof(FRAM_DEVPARAMS)) - 2);
        
        FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
        FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
    }
    //如果备份区错误，使用正常数据覆盖备份区
    else if(b == TRUE && bBackup == FALSE)
    {
        FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
        FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
    }
    //如果正常数据错误，使用备份区覆盖正常数据区
    else if(b == FALSE && bBackup == TRUE)
    {
        FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
        FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
    }
    
    Fram_ReadBootParamenter();
    if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_FAIL || (g_sFramDevParams.br != g_sFramBootParamenter.br))
    {
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
        g_sFramBootParamenter.br = g_sFramDevParams.br;
        
        Fram_WriteBootParamenter();
        Fram_WriteBackupBootParamenter();
    }
}

void Fram_WriteDevParams(void)
{
    g_sFramDevParams.crc = a_GetCrc((u8 *)(&g_sFramDevParams), (sizeof(FRAM_DEVPARAMS)) - 2);
    //写入数据，正常区和备份区都需要写入
    FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
    FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(FRAM_DEVPARAMS), (u8 *)(&g_sFramDevParams));
}

u32 Fram_GetUartBaudrate(void)
{
    if(g_sFramDevParams.br == FRAM_BR_9600)
    {
        return 9600;
    }
    else if(g_sFramDevParams.br == FRAM_BR_38400)
    {
        return 38400;
    }
    else if(g_sFramDevParams.br == FRAM_BR_115200)
    {
        return 115200;
    }
    else
    {
        return 9600;
    }
}
