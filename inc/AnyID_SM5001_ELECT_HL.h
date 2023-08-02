#ifndef _ANYID_SM5001_ELECT_HL_L
#define _ANYID_SM5001_ELECT_HL_L

#include "AnyID_SM5001_Config.h"

#define ELECT_PORT                       USART2
#define ELECT_IRQHandler                 USART2_IRQHandler
#define Elect_ChkTxOver()                while(((ELECT_PORT)->SR & USART_FLAG_TC) == (u16)RESET)
                                             
#define ELECT_BAUDRARE                   9600

#define Elect_ReadByte()                  ((u16)(ELECT_PORT->DR & (u16)0x01FF))
extern const PORT_INF ELECT_PORT_CTRL;
#define Elect_EnableRx()                     (ELECT_PORT_CTRL.Port->BRR  = ELECT_PORT_CTRL.Pin)
#define Elect_EnableTx()                     (ELECT_PORT_CTRL.Port->BSRR = ELECT_PORT_CTRL.Pin)

#define ELECT_DMA                               DMA1
#define ELECT_TXDMA_CH                          DMA1_Channel7
#define ELECT_TXDMA_INT                         DMA1_Channel7_IRQn
#define ELECT_TxDMAIRQHandler                   DMA1_Channel7_IRQHandler
#define ELECT_TXDMA_TC_FLAG                     DMA1_FLAG_TC7

#define Elect_EnableTxDma(p,s)                  do{\
                                                    (ELECT_DMA)->IFCR = ELECT_TXDMA_TC_FLAG; \
                                                    (ELECT_TXDMA_CH)->CMAR = ((u32)(p)); \
                                                    (ELECT_TXDMA_CH)->CNDTR = (s); \
                                                    (ELECT_TXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)

#define Elect_DisableTxDma()                    do{\
                                                    (ELECT_DMA)->IFCR = ELECT_TXDMA_TC_FLAG;\
                                                    (ELECT_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                }while(0)

   

#define ELECT_RXDMA_CH                          DMA1_Channel6
#define ELECT_RXDMA_INT                         DMA1_Channel6_IRQn
#define ELECT_RxDMAIRQHandler                   DMA1_Channel6_IRQHandler
#define ELECT_RXDMA_TC_FLAG                     DMA1_FLAG_TC6                       

#define Elect_EnableRxDma()                  do{\
                                                (ELECT_DMA)->IFCR = ELECT_RXDMA_TC_FLAG; \
                                                (ELECT_RXDMA_CH)->CNDTR = ELECT_BUFFER_MAX_LEN; \
                                                (ELECT_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)

#define Elect_DisableRxDma()                    do{\
                                                     (ELECT_DMA)->IFCR = ELECT_RXDMA_TC_FLAG;\
                                                    (ELECT_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                }while(0)
#define Elect_GetRxLen()                     (ELECT_BUFFER_MAX_LEN - (ELECT_RXDMA_CH)->CNDTR)  
                                                  
#define ELECT_MODE_MODBUS_8_2           0x02 
#define ELECT_MODE_MODBUS_8_1           0x01 
#define ELECT_MODE_645                  0x00 


void Elect_InitInterface(u32 baudrate, u8 mode);
void Elect_ConfigInt(FunctionalState state);
void Elect_InitTxDma(u8 *pTxBuffer, u32 len);
void Elect_InitRxDma(u8 *pRxBuffer, u32 len);
void Elect_Stop();
void Elect_EnableInt(FunctionalState rxState, FunctionalState txState);
void Elect_WriteByte(u8 ch);
void Elect_Delayms(u32 n);
void Elect_WriteBuffer(u8 *pFrame, u16 len);

#endif