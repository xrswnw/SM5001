#ifndef _ANYID_SM5001_GATE_HL_
#define _ANYID_SM5001_GATE_HL_

#include "AnyID_SM5001_Config.h"

#define GATE_TX_FRAME_LEN                   64
#define GATE_RX_FRAME_LEN                   (512 + 32)

#define GATE_PORT                           UART4
#define GATE_BAUDRARE                       115200
#define Gate_IRQHandler                     UART4_IRQHandler
#define Gate_ChkTxOver()                    while(((GATE_PORT)->SR & USART_FLAG_TC) == (u16)RESET)

extern const PORT_INF GATE_PORT_CTRL;
#define Gate_EnableRx()                     (GATE_PORT_CTRL.Port->BRR  = GATE_PORT_CTRL.Pin)
#define Gate_EnableTx()                     (GATE_PORT_CTRL.Port->BSRR = GATE_PORT_CTRL.Pin)

#define Gate_ReadByte()                     ((u16)(GATE_PORT->DR & (u16)0x01FF))

                                                
#define GATE_SR_IDLE                        0x0010  
#define GATE_DMA                            DMA2

#define GATE_TXDMA_CH                       DMA2_Channel5
#define GATE_TXDMA_INT                      DMA2_Channel4_5_IRQn
#define Gate_TxDMAIRQHandler                DMA2_Channel4_5_IRQHandler
#define GATE_TXDMA_TC_FLAG                  DMA2_FLAG_TC5
#define Gate_EnableTxDma(p, s)              do{\
                                                Gate_EnableTx();\
                                                (GATE_DMA)->IFCR = GATE_TXDMA_TC_FLAG; \
                                                (GATE_TXDMA_CH)->CMAR = ((u32)(p)); \
                                                (GATE_TXDMA_CH)->CNDTR = (s); \
                                                (GATE_TXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                             }while(0)
                                                                                             
#define Gate_DisableTxDma()                 do{\
                                                (GATE_DMA)->IFCR = GATE_TXDMA_TC_FLAG;\
                                                (GATE_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                Gate_ChkTxOver();\
                                                Gate_EnableRx();\
                                            }while(0)


#define GATE_RXDMA_CH                       DMA2_Channel3
#define GATE_RXDMA_INT                      DMA2_Channel3_IRQn
#define Gate_RxDMAIRQHandler                DMA2_Channel3_IRQHandler
#define GATE_RXDMA_TC_FLAG                  DMA2_FLAG_TC3
#define Gate_EnableRxDma()                  do{\
                                                (GATE_DMA)->IFCR = GATE_RXDMA_TC_FLAG; \
                                                (GATE_RXDMA_CH)->CNDTR = GATE_RX_FRAME_LEN; \
                                                (GATE_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                            }while(0)
                                            
#define Gate_GetRxLen()                     (GATE_RX_FRAME_LEN - (GATE_RXDMA_CH)->CNDTR)  

#define Gate_DisableRxDma()                 do{\
                                                (GATE_DMA)->IFCR = GATE_RXDMA_TC_FLAG;\
                                                (GATE_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                            }while(0)
                                            
void Gate_InitInterface(u32 baudrate);

void Gate_ConfigInt(FunctionalState state);

void Gate_InitTxDma(u8 *pTxBuffer, u32 len);
void Gate_InitRxDma(u8 *pRxBuffer, u32 len);

void Gate_Delayms(u32 n);             //œµÕ≥—” ±n∫¡√Î
void Gate_WriteBuffer(u8 *pBuffer, u32 len);

BOOL Gate_UartCheckErr(void);
#endif



