#ifndef _ANYID_SM5001_WIGHT_HL_H
#define _ANYID_SM5001_WIGHT_HL_H

#include "AnyID_SM5001_Config.h"

#define WATER_PORT                       USART3
#define Water_IRQHandler                 USART3_IRQHandler
#define Water_ChkTxOver()                while(((WATER_PORT)->SR & USART_FLAG_TC) == (u16)RESET)
                                             
#define WATER_BAUDRARE                   115200

#define Water_ReadByte()                  ((u16)(WATER_PORT->DR & (u16)0x01FF))
extern const PORT_INF WATER_PORT_CTRL;
#define Water_EnableRx()                     (WATER_PORT_CTRL.Port->BRR  = WATER_PORT_CTRL.Pin)
#define Water_EnableTx()                     (WATER_PORT_CTRL.Port->BSRR = WATER_PORT_CTRL.Pin)

#define WATER_DMA                               DMA1
#define WATER_TXDMA_CH                          DMA1_Channel2
#define WATER_TXDMA_INT                         DMA1_Channel2_IRQn
#define WATER_TxDMAIRQHandler                   DMA1_Channel2_IRQHandler
#define WATER_TXDMA_TC_FLAG                     DMA1_FLAG_TC2

#define Water_EnableTxDma(p,s)                  do{\
                                                    Water_EnableTx();\
                                                    (WATER_DMA)->IFCR = WATER_TXDMA_TC_FLAG; \
                                                    (WATER_TXDMA_CH)->CMAR = ((u32)(p)); \
                                                    (WATER_TXDMA_CH)->CNDTR = (s); \
                                                    (WATER_TXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)

#define Water_DisableTxDma()                    do{\
                                                    (WATER_DMA)->IFCR = WATER_TXDMA_TC_FLAG;\
                                                    (WATER_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                    Water_ChkTxOver();\
                                                    Water_EnableRx();\
                                                }while(0)

   

#define WATER_RXDMA_CH                          DMA1_Channel3
#define WATER_RXDMA_INT                         DMA1_Channel3_IRQn
#define WATER_RxDMAIRQHandler                   DMA1_Channel3_IRQHandler
#define WATER_RXDMA_TC_FLAG                     DMA1_FLAG_TC3                       

#define Water_EnableRxDma()                  do{\
                                                (WATER_DMA)->IFCR = WATER_RXDMA_TC_FLAG; \
                                                (WATER_RXDMA_CH)->CNDTR = WATER_BUFFER_MAX_LEN; \
                                                (WATER_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)

#define Water_DisableRxDma()                    do{\
                                                     (WATER_DMA)->IFCR = WATER_RXDMA_TC_FLAG;\
                                                    (WATER_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                }while(0)
#define Water_GetRxLen()                     (WATER_BUFFER_MAX_LEN - (WATER_RXDMA_CH)->CNDTR)                                               

                                                  
void Water_InitInterface(u32 baudrate);
void Water_ConfigInt(FunctionalState state);
void Water_InitTxDma(u8 *pTxBuffer, u32 len);
void Water_InitRxDma(u8 *pRxBuffer, u32 len);
void Water_Stop();
void Water_EnableInt(FunctionalState rxState, FunctionalState txState);
void Water_WriteByte(u8 ch);
void Water_Delayms(u32 n);
void Water_WriteBuffer(u8 *pFrame, u16 len);
void Water_WriteStr(char *str);


#endif