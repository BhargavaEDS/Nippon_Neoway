/******************** (C) COPYRIGHT 2018 EDS INDIA *****************************
* File Name     : Main.c					               *
* Author        : DILEEP SINGH                                                 *
* Date          : 21/02/2018                                                   *
* Description   : This is main file of Micro-controller     		       *
* Revision	: Rev0			                                       *
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "App_Include.h"
FlagStatus 
Flag_USN,
Flag_Ser_Err,
Flag_FWFail,
Flag_LogIn,
Flag_FW_Start;


uint8_t respond_time = 0;
int main(void)
{  
  
  INIT_GPIO();              							// INITIALIZE GPIO PORTS
  
  INIT_BKP();                                                                   // INITIALISE RCC
  INIT_SysTick();                                                               // INITIALISE SYSTICK TIMER
  
  IAP_Status  = RTC_ReadBackupRegister(RTC_BKP_DR4) & 0x00ff;                   // READ IAP STATUS 
  IAP_Counter = RTC_ReadBackupRegister(RTC_BKP_DR4) >> 8;                       // READ IAP COUNTER
  
  
  INIT_USART();
  INIT_USART_GPRS();             						// INITIALIZE USART & BAUD RATE
  SPI_FLASH_Init();                                                             // FLASH INITIALIZATION 
  INIT_IWDG();                                                                  // INITIALIZE IWDT
  INIT_NVIC();              							// INITIALIZE NVIC
  
  if(((IAP_Status == 'E') && (IAP_Counter == 255)) || (IAP_Status == 'P'))
  {
    goto APP_FW;
  }
  if((IAP_Status == 'I') || (IAP_Status == 'S'))Flag_FlashProg = SET;
  
  if(IAP_Status == 'S')
  {
    Flag_SerialFlash = SET;
    VTS_State = SERIAL;
    GPRS_Module_ON();  
  }
  
  INIT_APP();                                                                   // INITIALIZE APPLICATION
  
  if((Flag_FlashProg == SET) && (IAP_Counter < 10))
  {
    Flag_LoadProg = RESET;  
    if(Flag_SerialFlash){ GP_Counter = IAP_Counter << 8 | 'S'; }
    else GP_Counter = IAP_Counter << 8 | 'I'; 
    RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);                            // will be cleared after sucess programming
  }
  else if(IAP_Status == 'E')
  {
    if(Flag_SerialFlash){ GP_Counter = IAP_Counter << 8 | 'S'; }
    else GP_Counter = IAP_Counter << 8 | 'I';
    RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);
    SYSTEM_SW_RESET;
   }  
  else if(IAP_Status == 'R')Flag_SaveFlash = SET;
  else if(IAP_Status == 'D')Program_Flash();
 
  /* Keep the user application running */
  else
  {   
  APP_FW:   
    
    if((IAP_Status == 'E') && (IAP_Counter == 255))
    {
      Serial_PutString_232("FAIL",3); 
    }
   /* Test if user code is programmed starting from address "ApplicationAddress" */
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
     { 
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) ApplicationAddress);
      Jump_To_Application();
     }
    else 
    {
        if(IAP_Status == 'D')
        {
          IAP_Counter = 5;
          Flag_FlashProg = RESET;
          Flag_SaveFlash = RESET;
          Flag_LoadProg = SET;
          VTS_State = NORMAL;
        }
        else 
        {
          if(IAP_Status == 0)Flag_SerialFlash = SET;
          if(Flag_SerialFlash){ GP_Counter = IAP_Counter << 8 | 'S'; }
          else GP_Counter = IAP_Counter << 8 | 'I'; 
          RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);
          Flag_FWFail = SET;
          if(Flag_SerialFlash)VTS_State = SERIAL;
          else VTS_State = MODEM_INIT; 
          Flag_FlashProg = SET;
        }
    };                                                                          // return to IAP if Flash is not correct
  }
  
  
  //Flag_PowerON = SET;
 if(IAP_Status == 'S')
 {
  IAP_Counter += 2;
  memset(Rev_Buffer,0,sizeof(Rev_Buffer));
  Rev_Count = 0;
  
   memset(GPRS_Buffer,0,50);
   GPRS_Count = 0;
    
   if(Flag_SPIFail == SET)
    {
     Serial_PutString_232("SPI FLASH FAIL",3); 
     RTC_WriteBackupRegister(RTC_BKP_DR4,0);
     mSec_Delay(1000);
     SYSTEM_SW_RESET;
    }
   else 
   {
     Serial_PutString_232("OK",3); 
   }
 }
  
  /* Infinite loop */
  for (;;)
  {
    
    if(Flag_START == SET)
    {
        Flag_FWFail = RESET;
        Flag_START = RESET;
        
        IAP_Counter = 0;
    }
    else if(Flag_USN == SET)
    {
      Flag_USN = RESET;
      Serial_PutString_232("OK",3);
    }
    else if(Flag_Ser_Err == SET)
    {
      Flag_Ser_Err = RESET;
      Serial_PutString_232("INVALID ENTRY",3);
    }
    /**************************************************************************/
    /*              EVERY SECOND CYCLE WORK                                   */
    /**************************************************************************/
    if(Flag_Second == SET)             
    {
      Flag_Second = RESET;
      
      IWDG_ReloadCounter();                                                     // RELOAD IWDT

      
      
      switch(VTS_State)                                                         // DO ACT AS PER VEHICLE STATUS
      {
        
      case MODEM_INIT : if(Init_Time)Init_Time--;                               // Modem Initialization Time
                        else 
                        {
                          RTC_WriteBackupRegister(RTC_BKP_DR4,0);
                          SYSTEM_SW_RESET;
                        }
                        if(Init_Time == 20)GPRS_Module_ON();
                        else if(Init_Time < 20)
                        {
                          if(Init_Time == 1)Flag_ModemRdy = SET;                  // if modem does not gives call ready
                          if(Flag_NoResp == SET) Modem_PutString("AT");           // Sent AT for Autobauding 
                          if(Flag_ModemRdy == SET)
                          {
                            Init_Time = 0;mSec_Delay(100);INIT_GPRS_Modem();     //if call ready 
                            VTS_State = NORMAL; 
                            if(Flag_SPIFail == SET)
                            {
                             Send_SMS(SPI_FAIL);
                             RTC_WriteBackupRegister(RTC_BKP_DR4,0);
                             SYSTEM_SW_RESET;
                            };
                          };
                        }
                        break;
                        
      case NORMAL     :   if(IAP_Counter >= 10){Send_SMS(FAIL);  SYSTEM_SW_RESET;  }
                          else if(Flag_WrongFile){Send_SMS(WRONG_FILE);RTC_WriteBackupRegister(RTC_BKP_DR4,0);SYSTEM_SW_RESET;}
                          else if(NoData_Count > 3){Send_SMS(UNABLE_TO_DWLOAD);RTC_WriteBackupRegister(RTC_BKP_DR4,0);SYSTEM_SW_RESET;}
                          else if(Modem_NoResp >= 8)INIT_APP();
                          else if(Flag_FlashProg)
                          {
                            IAP_Counter +=1;
                            Get_Firmware();
                          }
                         break;
                         
                         
      case SERIAL      :  if(IAP_Counter >= 10)
                          {
                            if(IAP_Status == 'S')
                            {
                              Serial_PutString_232("FAIL",3);
                            }
                            else 
                            {
                            Send_SMS(FAIL);
                            }
                            RTC_WriteBackupRegister(RTC_BKP_DR4,0);
                            SYSTEM_SW_RESET;
                          }
                        
                          GPRS_Module_ON();   
                          if(Flag_FlashProg)
                          {
                            IAP_Counter +=1;
                            Get_Firmware_Serial();
                            
                          }
                          else if(Flag_SaveFlash)
                          {
                            IAP_Counter +=1;
                            StoreSPI_Flash();
                          }
                          else if(Flag_LoadProg)
                          {
                            IAP_Counter +=1;
                            Program_Flash();
                          } 
                      else Flag_FW_Start = RESET;
                         break;
       default :   if(Flag_FWFail == RESET)
                   {
                       if(IAP_Counter >= 10)
                        {
                          if(IAP_Status == 'S')
                          {
                            Serial_PutString_232("FAIL",3);
                          }
                          else 
                          {
                            Send_SMS(FAIL);
                          }
                          RTC_WriteBackupRegister(RTC_BKP_DR4,0);
                          SYSTEM_SW_RESET;
                        }
                       IAP_Counter++; 
                   }
                   else 
                   {
                     if(Flag_LogIn == RESET)
                     {
                       respond_time++;
                       if(respond_time > 4)
                       {
                         respond_time = 0;
                         Serial_PutString_232("Please Flash The Device",3);
                         SYSTEM_SW_RESET;
                       }
                     }
                     Flag_FlashProg = SET;
                   }
                   
                    break;
        
      };// End of switch statement
      
    };// End of Second Flag
    
  };// End of for loop
  
  
  
}// End of Main

void assert_failed(char* file, long line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

   printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* Infinite loop */
  while (1)
  {
  }
}
/******************* (C) COPYRIGHT 2015 EDS INDIA *********END OF FILE*********/