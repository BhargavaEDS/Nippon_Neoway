/**
  ******************************************************************************
  * @file    stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    16-January-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "App_Include.h"

    
extern uint8_t Data_WaitTime;
extern char 
SER_USN[11],
SER_PWD[11];

FlagStatus 
Flag_logRST,
Flag_PWD_Req,
Flag_logRST;

extern FlagStatus 
Flag_USN,
Flag_Ser_Err,
Flag_FWFail,
Flag_LogIn,
Flag_FW_Start,
Flag_GetFile_Serial;

static uint8_t ser_cnt = 0;
/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DMA_FLASHRAMTransfer
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
  RCC_ClearITPendingBit(RCC_IT_CSS);
  SYSTEM_SW_RESET; 
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
    if(IAP_Status == 'S')GP_Counter = 255 << 8 | 'E'; 
    else GP_Counter = IAP_Counter << 8 | 'E'; 
    RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);
    SYSTEM_SW_RESET;                                                            // System Sw reset in case of hard fault
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
    
    
void SysTick_Handler(void)
{
 Flag_mSec = SET;                                                                // SET MILI SECOND FLAG
 if(Flag_FWFail == SET)
 {
  if((Sec_Tick % 400) == 0)STATUS_LED_TOGGLE;
 }
 else if(Flag_Download)
 {
   if((Sec_Tick % 200) == 0)STATUS_LED_TOGGLE;
 }
 else if(Flag_SaveFlash)
 {
   if((Sec_Tick % 100) == 0)STATUS_LED_TOGGLE;
 }
 else if(Flag_LoadProg)
 {
   if((Sec_Tick % 50) == 0)STATUS_LED_TOGGLE;
 }
 
  
 if(Sec_Tick)Sec_Tick--; 
 else {
       Sec_Tick = 999;
       Flag_Second = SET;
      };
}

void USART1_IRQHandler(void)
{
  char *GPRSPtr;
  unsigned char RcvIn;
 
  GPRSPtr = NULL;
  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    RcvIn = (u16)(USART1->RDR & (u16)0x01FF);
        
    if((Flag_GetFile == SET) && (Flag_GetFile_Serial == RESET)) 
    {
       Data_WaitTime = 30;
       Rev_Buffer[Rev_Count] = RcvIn;
       Rev_Count ++;
       if((Rev_Count % FOTA_PKT)==0)
         Flag_WriteFile = SET;
     
    }
    else
    {  
    if(RcvIn != 0)
     {
      if(RcvIn == 10)                                                           // LF
      {                                                                        // INITIAL MODEM READY
        if(Flag_ModemRdy == RESET)
        {
          if(GPRSPtr = strstr((char*)GPRS_Buffer,"PB DONE"))Flag_ModemRdy = SET;
          else if(GPRSPtr = strstr((char*)GPRS_Buffer,"OK"))Flag_NoResp = RESET;
          GPRS_Count = 0;
        }
        else
        {  
           // OK Response  
           if(strstr((char*)GPRS_Buffer,"OK")){Flag_ModemOK = SET;GPRS_Count = 0;} 
           // PDP Deactivated
           if(strstr((char*)GPRS_Buffer,"pdpdeact")){VTS_State = 0; Init_Time = 25; GPRS_Module_OFF(); } 
           // all other erroe\r
           else if(GPRSPtr = strstr((char*)GPRS_Buffer,"ERROR")){Flag_ModemError = SET;GPRS_Count = 0;}
        }// End of Else
    
        if(GPRSPtr != NULL){*GPRSPtr = ' '; GPRSPtr++;*GPRSPtr = ' '; GPRSPtr = NULL;GPRS_Count = 0;};
    }// if not 10
    else 
    {
     GPRS_Buffer[GPRS_Count] = RcvIn;
     GPRS_Count++;
     if(GPRS_Count >= GPRSBuffer_Size)GPRS_Count = 0;
    }; 
   }// End of chr not 0
  };// end of else progr data
 
    
    {
        TxData[0] = RcvIn;   
        DMA2_Channel1->CNDTR = 1;
        DMA_Cmd(DMA2_Channel1, ENABLE);
       } 
  }
   else
   {
     USART_ClearITPendingBit(USART1,USART_IT_CTS);
     USART_ClearITPendingBit(USART1,USART_IT_TC);
     USART_ClearITPendingBit(USART1,USART_IT_RXNE);
     USART_ClearITPendingBit(USART1,USART_IT_ORE);
     RcvIn = (u16)(USART1->RDR & (u16)0x01FF);
   };
}

void USART3_8_IRQHandler(void)
{
  char *SerPtr;
  unsigned char RcvIn;
 
 
  SerPtr = NULL;
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);
    RcvIn = (u16)(USART3->RDR & (u16)0x01FF);
    
    if((Flag_GetFile == RESET) && (Flag_GetFile_Serial == SET))
    {
       Data_WaitTime = 30;
       Rev_Buffer[Rev_Count] = RcvIn;
       Rev_Count ++;
       if((Rev_Count % 4096)==0)Flag_WriteFile = SET;
    }
    else
    {  
    if(RcvIn != 0)
     {
      if(RcvIn == 10)                                                           // LF
       {                                                                        // INITIAL MODEM READY
 
         if(strstr((char*)Ser_Buffer,"START")){Flag_START = SET; Flag_FW_Start = SET; ser_cnt = 0;}
         if(SerPtr = strstr((char*)Ser_Buffer,"LENGTH"))
         {
           Flag_START = SET; Flag_FW_Start = SET; ser_cnt = 0;
           SerPtr += 8; FileLength = atoi(SerPtr); 
           if(FileLength > 0){Flag_GetFile = RESET;Flag_GetFile_Serial = SET;}
         }
         else if(strstr((char*)Ser_Buffer,"FLASH"))
         {
            Flag_USN = SET;// send ok
         }
         else if(strstr((char*)Ser_Buffer,"LOG_RST"))
         {
            Flag_logRST = SET;
            Flag_USN = SET;
            Flag_LogIn = RESET;
            Flag_PWD_Req = RESET;
          }
         else if(Flag_LogIn == SET)
         {
               if(strstr((char*)Ser_Buffer,"FLASH"))
                {
                  Flag_USN = SET;// send ok
                }
           }
         else 
         {
            if(SerPtr = strstr((char*)Ser_Buffer,"\r"))
            {
              *SerPtr = 0;
            
            if(Flag_PWD_Req == SET)
            {
              if(strcmp((char*)Ser_Buffer,SER_PWD) == 0)
              {
                Flag_PWD_Req = RESET;
                Flag_LogIn = SET;
                Flag_USN = SET;
              }
              else 
              {
                Flag_Ser_Err = SET;
              }
            }
            else if(strcmp((char*)Ser_Buffer,SER_USN) == 0)
            {
              Flag_USN = SET;
              Flag_PWD_Req = SET;
            }
            else 
            {
              if(Flag_logRST == SET)Flag_logRST = RESET;
              else if(Flag_FW_Start == RESET)Flag_Ser_Err = SET;
            }
          }
            
          }
         
         ser_cnt = 0;
         memset(Ser_Buffer,0,50);
    }// if not 10
    else 
    {
     Ser_Buffer[ser_cnt] = RcvIn;
     if(RcvIn != 0)ser_cnt++;
     if(ser_cnt > 48)ser_cnt = 0;
    }; 
   }// End of chr not 0
  };// end of else progr data
 
  }
   else
   {
     USART_ClearITPendingBit(USART3,USART_IT_CTS);
     USART_ClearITPendingBit(USART3,USART_IT_TC);
     USART_ClearITPendingBit(USART3,USART_IT_RXNE);
     USART_ClearITPendingBit(USART3,USART_IT_ORE);
     RcvIn = (unsigned short)(USART3->RDR & (unsigned short)0x01FF);
   };
  
}

void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA2_IT_TC1) == SET)
    { 
       /* Disable DMA */
       DMA_ClearFlag(DMA2_IT_TC1);
       DMA_Cmd(DMA2_Channel1, DISABLE);
       
   
    }
}
/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */