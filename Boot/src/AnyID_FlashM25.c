#include "AnyID_FlashM25.h"


void Flash_Init(void)
{
    Flash_CsLow();
    Flash_WriteByte(FLASH_CMD_WRDI);
    Flash_CsHigh();
}

BOOL Flash_WritePage(u32 addr, u32 len, u8 *pBuffer)
{
    BOOL b = FALSE;
    u8 state = 0;
    u16 delay = 0;

    //等待上次的操作完成，一般页编程只要15ms
    //sector擦除使用特殊方法判断
    delay = 0;

    //等待WEL未变为0，最多15ms
    Flash_CsLow();
    Flash_WriteByte(FLASH_CMD_RDSR);
    do{
        Flash_Delay16us(10);
        delay++;
        state = Flash_WriteByte(0xFF);
        //Tcp232_WriteByte('A');
    }while((state & FLASH_STAT_WEL) == FLASH_STAT_WEL && delay < 200);
    Flash_CsHigh();

    //空闲状态才可以进行写操作
    if((state & FLASH_STAT_WEL) == 0x00)
    {
        //写使能
        Flash_CsLow();

        Flash_WriteByte(FLASH_CMD_WREN);

        Flash_CsHigh();
        Flash_Delay16us(10);            //添加10us延时，猜测芯片原因
        delay = 0;
        //等待WEL未变为1，最多15ms
        Flash_CsLow();
        Flash_WriteByte(FLASH_CMD_RDSR);
        do{
            Flash_Delay16us(10);
            delay++;
            state = Flash_WriteByte(0xFF);
            //Tcp232_WriteByte('B');
        }while((state & FLASH_STAT_WEL) == 0x00 && delay < 200);
        Flash_CsHigh();

        if((state & FLASH_STAT_WEL) == FLASH_STAT_WEL)
        {
            u16 i = 0;
            Flash_CsLow();
            Flash_WriteByte(FLASH_CMD_PP);
            Flash_WriteByte((addr >> 16) & 0xFF);
            Flash_WriteByte((addr >> 8) & 0xFF);
            Flash_WriteByte((addr >> 0) & 0xFF);
            for(i = 0; i < len; i++)
            {
                Flash_WriteByte(pBuffer[i]);
            }
            Flash_CsHigh();

            b = TRUE;
            //Tcp232_WriteByte('C');
        }
    }

    return b;
}

BOOL Flash_WriteBuffer(u32 addr, u16 len, u8 *pBuffer)
{
    u16 size = 0;
    u16 pos = 0;
    BOOL b = FALSE;

    //pageSize = 256
    if(addr & FLASH_PAGE_MASK)
    {
        size = FLASH_PAGE_SIZE - (addr & FLASH_PAGE_MASK);
        if(len < size)
        {
            size = len;
        }
        b = Flash_WritePage(addr + pos, size, pBuffer + pos);
        len -= size;
        pos += size;
    }
    else
    {
        b = TRUE;
    }

    if(b)
    {
        while(len > 0)
        {
            size = FLASH_PAGE_SIZE;
            if(len < size)
            {
                size = len;
            }
            b = Flash_WritePage(addr + pos, size, pBuffer + pos);
            len -= size;
            pos += size;

            if(b == FALSE)
            {
                break;
            }
        }
    }

    return b;
}
BOOL Flash_EraseSector(u32 addr)
{
    BOOL b = FALSE;
    u8 state = 0;
    u16 delay = 0;

    //等待上次的操作完成，一般页编程只要15ms
    //sector擦除使用特殊方法判断
    delay = 0;
    //等待WEL未变为0，最多15ms
    Flash_CsLow();
    Flash_WriteByte(FLASH_CMD_RDSR);
    do{
        Flash_Delay16us(10);
        delay++;
        state = Flash_WriteByte(0xFF);
        //Tcp232_WriteByte('D');
    }while((state & FLASH_STAT_WEL) == FLASH_STAT_WEL && delay < 200);
    Flash_CsHigh();

    //空闲状态才可以进行写操作
    if((state & FLASH_STAT_WEL) == 0x00)
    {
        //写使能
        Flash_CsLow();
        Flash_WriteByte(FLASH_CMD_WREN);
        Flash_CsHigh();
        Flash_Delay16us(10);            //添加10us延时，猜测芯片原因

        delay = 0;
        //等待WEL未变为1，最多15ms
        Flash_CsLow();
        Flash_WriteByte(FLASH_CMD_RDSR);
        do{
            Flash_Delay16us(10);
            delay++;
            state = Flash_WriteByte(0xFF);
            //Tcp232_WriteByte('E');
        }while((state & FLASH_STAT_WEL) == 0x00 && delay < 200);
        Flash_CsHigh();

        if((state & FLASH_STAT_WEL) == FLASH_STAT_WEL)
        {
            addr &= (~FLASH_SECTOR_MASK);
            Flash_CsLow();
            Flash_WriteByte(FLASH_CMD_SE);
            Flash_WriteByte((addr >> 16) & 0xFF);
            Flash_WriteByte((addr >> 8) & 0xFF);
            Flash_WriteByte((addr >> 0) & 0xFF);
            Flash_CsHigh();
            //Tcp232_WriteByte('F');
            b = TRUE;
        }
    }

    return b;
}
            
            
BOOL Flash_EraseAllmemory(u32 addr)
{
    BOOL b = FALSE;
    u8 state = 0;
    u16 delay = 0;

    //等待上次的操作完成，一般页编程只要15ms
    //sector擦除使用特殊方法判断
    delay = 0;
    //等待WEL未变为0，最多15ms
    Flash_CsLow();
    Flash_WriteByte(FLASH_CMD_RDSR);
    do{
        Flash_Delay16us(10);
        delay++;
        state = Flash_WriteByte(0xFF);
        //Tcp232_WriteByte('D');
    }while((state & FLASH_STAT_WEL) == FLASH_STAT_WEL && delay < 200);
    Flash_CsHigh();

    //空闲状态才可以进行写操作
    if((state & FLASH_STAT_WEL) == 0x00)
    {
        //写使能
        Flash_CsLow();
        Flash_WriteByte(FLASH_CMD_WREN);
        Flash_CsHigh();

        delay = 0;
        //等待WEL未变为1，最多15ms
        Flash_CsLow();
        Flash_WriteByte(FLASH_CMD_RDSR);
        do{
            Flash_Delay16us(10);
            delay++;
            state = Flash_WriteByte(0xFF);
            //Tcp232_WriteByte('E');
        }while((state & FLASH_STAT_WEL) == 0x00 && delay < 200);
        Flash_CsHigh();

        if((state & FLASH_STAT_WEL) == FLASH_STAT_WEL)
        {
            addr &= (~FLASH_SECTOR_MASK);
            Flash_CsLow();
            Flash_WriteByte(FLASH_CMD_BE);
           // Flash_WriteByte((addr >> 16) & 0xFF);
            //Flash_WriteByte((addr >> 8) & 0xFF);
            //Flash_WriteByte((addr >> 0) & 0xFF);
            Flash_CsHigh();
            //Tcp232_WriteByte('F');
            b = TRUE;
        }
    }

    return b;
}            

            
BOOL Flash_ReadBuffer(u32 addr, u16 len, u8 *pBuffer)
{
    u16 i = 0;
    BOOL b = FALSE;
    u8 state = 0;
    u16 delay = 0;

    //等待上次的操作完成，一般页编程只要15ms
    //sector擦除使用特殊方法判断
    delay = 0;
    //等待WEL未变为0，最多15ms
    Flash_CsLow();
    Flash_WriteByte(FLASH_CMD_RDSR);
    do{
        Flash_Delay16us(10);
        delay++;
        state = Flash_WriteByte(0xFF);
        //Tcp232_WriteByte('H');
    }while((state & FLASH_STAT_WEL) == FLASH_STAT_WEL && delay < 200);
    Flash_CsHigh();

    //空闲状态才可以进行读操作
    if((state & (FLASH_STAT_WEL | FLASH_STAT_WIP)) == 0x00)
    {

        Flash_CsLow();
        Flash_WriteByte(FLASH_CMD_READ);
        Flash_WriteByte((addr >> 16) & 0xFF);
        Flash_WriteByte((addr >> 8) & 0xFF);
        Flash_WriteByte((addr >> 0) & 0xFF);
        for(i = 0; i < len; i++)
        {
            pBuffer[i] = Flash_WriteByte(0xFF);
        }
        Flash_CsHigh();
        //Tcp232_WriteByte('I');
        b = TRUE;
    }

    return b;
}

void Flash_ReadId()
{

    
   static u16 id = 0;

    Flash_CsLow();
    Flash_Delay16us(10);
    Flash_WriteByte(FLASH_CMD_GET_ID);
	
    Flash_WriteByte(0);
    Flash_WriteByte(0);
    Flash_WriteByte(0);
	
    id=Flash_WriteByte(0xFF)<<8;
    id|=Flash_WriteByte(0xFF);


    Flash_CsHigh();

}




