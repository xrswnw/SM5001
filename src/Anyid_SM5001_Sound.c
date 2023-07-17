#include "AnyID_SM5001_Sound.h"
#include <math.h>
SOUND_INFO g_sSoundInfo = {0};

void Sound_Init()
{
    Sound_InitInterface(SOUND_BAUDRARE);
    Sound_ConfigInt();
    Sound_EnableInt(ENABLE,DISABLE);
    g_sSoundInfo.state = SOUND_STAT_IDLE;

}

void Sound_ReceiveFrame(u8 byte, SOUND_INFO *Info)
{
    switch(Info->flag)
    {
        case SOUND_FLAG_IDLE:
            if(byte == SOUND_FRAME_HEAD)
            {
                Info->rxBuf.buffer[0] = byte;
                Info->flag = SOUND_FLAG_HRAD;
            }
            break;
        case SOUND_FLAG_HRAD:
            if(byte == SOUND_FRAME_ADD)
            {
                Info->rxBuf.buffer[1] = byte;
                Info->flag = SOUND_FLAG_LEN;
            }
            else
            {
                Info->flag = SOUND_FLAG_IDLE;
            }
            break;
        case SOUND_FLAG_LEN:
            if(SOUND_BUF_MIN_LEN <= byte)
            {
                Info->flag = SOUND_FLAG_DATA;
                Info->rxBuf.len  = byte ;
                Info->rxBuf.buffer[2] = byte;
                Info->index = 3;
            }
            else
            {
                Info->flag = SOUND_FLAG_IDLE;
            }
            break;
        case SOUND_FLAG_DATA:
            Info->rxBuf.buffer[Info->index++] = byte;
            
            if(Info->index > (Info->rxBuf.len + 3))
            {
                Info->flag = SOUND_FLAG_IDLE;
                Info->state = SOUND_STAT_RCV;
            }
            else
            {
                Info->state = SOUND_STAT_IDLE;
            }

            break;
    }
}

u8 Sound_FormaFrame(u8 cmd,u8 feedback, u16 dat, u8 *pFrame)
{
	u8 pos = 0;
       
        pFrame[pos++] = SOUND_FRAME_HEAD;
	pFrame[pos++] = SOUND_FRAME_ADD;
	pFrame[pos++] = 0x06;
	pFrame[pos++] = cmd;
	pFrame[pos++] = feedback;
	pFrame[pos++] = (dat >> 8) & 0xFF;
	pFrame[pos++] = (dat >> 0) & 0xFF;
        //pFrame[pos++] = 0xEF;
	Sound_Sum(&pFrame[1],6); 
        pos += 3;
      
      /*  
        pFrame[pos++] = SOUND_FRAME_HEAD;
	pFrame[pos++] = SOUND_FRAME_ADD;
	pFrame[pos++] = 0x06;
	pFrame[pos++] = 0X0f;
	pFrame[pos++] = feedback;
	pFrame[pos++] = 60;
	pFrame[pos++] = 0X01;
        //pFrame[pos++] = 0xEF;
	Sound_Sum(&pFrame[1],6); 
        pos += 3;*/
     /*     pFrame[pos++] = 0x7E;
	pFrame[pos++] = 0xFF;
	pFrame[pos++] = 0x06;
	pFrame[pos++] = 0x49;
	pFrame[pos++] = 0x00;
	pFrame[pos++] = 0x00;
	pFrame[pos++] = 0x00;

        Sound_Sum(&pFrame[1],6); 
        pos += 3; */
	return pos;
}

u8 Sound_FormaFrame_ApoVoice(u8 cmd,u8 feedback, u8 folderName, u8 voiceName, u8 *pFrame)
{
	u8 pos = 0;
       
        pFrame[pos++] = SOUND_FRAME_HEAD;
	pFrame[pos++] = SOUND_FRAME_ADD;
	pFrame[pos++] = 0x06;
	pFrame[pos++] = cmd;
	pFrame[pos++] = feedback;
	pFrame[pos++] = folderName;
	pFrame[pos++] = voiceName;
	Sound_Sum(&pFrame[1],6); 
        pos += 3;
        
	return pos;
}

void Sound_Sum(u8 *pFrame, u8 len)
{
     u16 sum = 0;
     u8 i;
     for(i = 0; i < len; i++) 
     {
            sum = sum + pFrame[i];
     }
    sum = 0 - sum;
    *(pFrame+i)   = (sum >>8) & 0xFF;
    *(pFrame+i+1) = (sum >>0) & 0xFF;
    *(pFrame+i+2) = 0xEF;
}

u8 Sounde_Chk(u8 *pFrame)
{       
    u8 bOk = FALSE; 
    u8 i = 0;
    u16 sumS = 0;
    u16 sumG = 0;
    for(i=1; i <= (*(pFrame+2)); i++)
     {
        sumS = sumS + pFrame[i] ;
     }
     sumG = ((u16)((*(pFrame+i))<<8)) | (*(pFrame+i+1));
     sumS = sumS + sumG;
     if(!sumS)
     {
       bOk = TRUE;
     }

     return bOk;
}
void Sound_TransmitCmd(u8 feedback)
{
    g_sSoundInfo.txBuf.len = Sound_FormaFrame_ApoVoice(g_sSoundInfo.txBuf.cmd, feedback, g_sSoundInfo.txBuf.data >> 8, g_sSoundInfo.txBuf.data >> 0,  g_sSoundInfo.txBuf.buffer);
    Sound_WriteBuffer(g_sSoundInfo.txBuf.buffer, g_sSoundInfo.txBuf.len);
}




void Sound_CtrTxCmd(SOUND_TX_BUF *pCntOp, u32 sysTick)
{
    u8 op = 0;
    op = pCntOp->op[pCntOp->index];
    
    switch(op)
    {
        case SOUND_VOICE_RTU_BAT:
          
                Sound_Voice_Appoint(g_nSysTick, SOUND_FOLDER_RETUEN_BAT, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
                Sound_TransmitCmd(SOOUND_FEEDBACK_NO);

            break;
        case SOUND_VOICE_BRW_BAT:
          
            Sound_Voice_Appoint(g_nSysTick, SOUND_FOLDER_BRWORR_BAT, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
            Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
            break;
        case SOUND_VOICE_NO_BAT: 
          
            Sound_Voice_Appoint(g_nSysTick, SOUND_FOLDER_PROMPT, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
            Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
            break;
        case SOUND_VOICE_DI: 
            if(pCntOp->id[pCntOp->index] == SOUND_VOICE_STOP)
            {
                Sound_Voice_Appoint(g_nSysTick, SOUND_VOICE_STOP, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
                Sound_TransmitCmd(SOOUND_FEEDBACK_NEED);
            }
            else
            {
                Sound_Voice_Appoint(g_nSysTick, SOUND_FOLDER_PROMPT, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
                Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
            }
            //Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
            break;
        case SOUND_VOICE_RTU_BAT_OK: 
          
            Sound_Voice_Appoint(g_nSysTick, SOUND_FOLDER_PROMPT, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
            Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
            break;
        case SOUND_VOICE_BRW_BAT_OK:   
            break;
        case SOUND_VOICE_RTU_FAIL:   
            break;
        case SOUND_VOICE_BAT_IDI_FAIL:   
            break;
        case SOUND_VOICE_TEMP_MEMORY_OK:   
            break;
        case SOUND_VOICE_TEMP_MEMORY_FAIL:   
            break;
        case SOUND_VOICE_BAT_SN_FAIL:
          
            Sound_Voice_Appoint(g_nSysTick, SOUND_FOLDER_PROMPT, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
            Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
            break;
        case SOUND_VOICE_PAKEAGE_DIE:   
            break;
        case SOUND_VOICE_FIRE_WRAN: 

            Sound_Voice_Appoint(g_nSysTick, SOUND_FOLDER_PROMPT, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
            Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
            break;
        case SOUND_VOICE_CTR_STRENGH:
          
            Sound_Voice_Appoint(g_nSysTick, SOUND_VOC_FRAME_NULL, pCntOp->id[pCntOp->index], pCntOp->cd[pCntOp->index]);
            Sound_TransmitCmd(SOOUND_FEEDBACK_NEED);
          break;
        case SOUND_VOICE_GET_STRENGH:
          
            Sound_Voice_Appoint(g_nSysTick, SOUND_VOC_FRAME_NULL, SOUND_VOC_FRAME_NULL, SOUND_FRAME_CMD_GET_VOL);
            Sound_TransmitCmd(SOOUND_FEEDBACK_NO);
          break;
    }
    pCntOp->tick = sysTick;
}


void Sound_ConnectStep(SOUND_TX_BUF *pCntOp)
{
    u8 op = 0;
    
    op = pCntOp->op[pCntOp->index];
    switch(op)
    {
        case SOUND_VOICE_RTU_BAT:
        case SOUND_VOICE_BRW_BAT:
        case SOUND_VOICE_NO_BAT:
        case SOUND_VOICE_DI:
        case SOUND_VOICE_RTU_BAT_OK:
        case SOUND_VOICE_BRW_BAT_OK:
        case SOUND_VOICE_RTU_FAIL:
        case SOUND_VOICE_BAT_IDI_FAIL:
        case SOUND_VOICE_TEMP_MEMORY_OK:
        case SOUND_VOICE_TEMP_MEMORY_FAIL:
        case SOUND_VOICE_BAT_SN_FAIL:
        case SOUND_VOICE_PAKEAGE_DIE:
        case SOUND_VOICE_FIRE_WRAN:
        case SOUND_VOICE_CTR_STRENGH:
        case SOUND_VOICE_GET_STRENGH:
            if(pCntOp->result == SOUND_RESULT_OK)
            {
                pCntOp->repeat[pCntOp->index] = 0;
                pCntOp->index++;
            }
            else
            {
                pCntOp->repeat[pCntOp->index] = 0;
                pCntOp->index++;
            }
            break;
    }

}
