/******************** (C) COPYRIGHT 2018 EDS INDIA *****************************
* File Name     : GPRS.c					               *
* Author        : DILEEP SINGH                                                 *
* Date          : 21/02/2018                                                   *
* Description   : This file Configures GPRS & Deals with ITs functions         *
* Revision	: Rev0			                                       *
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "App_Include.h"

GPRS_InitTypeDef        GPRSData;                                               // DATA STRUCTURE FOR GPRS
HTTP_InitTypeDef        DFU_HTTP;                                               // DATA STRUCTURE FOR DFU HTTP

FlagStatus
Flag_GetFile,                                                                   // DOWNLOADING STARTING FROM FTP SERVER
Flag_WriteFile,                                                                 // SAVE RECEIVED DATA INTO FLASH
Flag_ModemOK,                                                                   // GPRS DATA READY           
Flag_ModemError,                                                                // ERROR GENERATED 
Flag_ValidNetwork,                                                              // VALID NETWORK
Flag_WrongIMEI,                                                                 // WRONG IMEI NUMBER                  
Flag_WrongFile,                                                                 // WRONG FILE NAME REQUESTED TO DOWNLOAD 
Flag_APNSet;                                                                    // APN NAME SET

char 
Rev_Buffer[Reveive_Buffer_Size],                                                // BUFFER TO RECEIVE BYTES FROM FTP SERVER 
IAP_Status,                                                                     // IAP STATUS 
IAP_Counter,                                                                    // IAP COUNTER 
NoData_Count,                                                                   // NO DATA  AFTER CONNECTION COUNTER        
Storage_Data[4096],                                                             // PROGRAMMING DATA
GPRS_SendBuffer[100],                                                           // GENERAL PURPOSE SENDING BUFFER
GPRS_Buffer[GPRSBuffer_Size],                                                   // GPRS DATA BUFFER 
DFU_Mob[15];                                                                    // DFU REQUESTING MOBILE NUMBER

unsigned char
Reg_Error,                                                                      // SIM REGISTRATION ERROR                 
Modem_NoResp,                                                                   // MODEM NO RESPONSE COUNT
Init_Time = 25;                                                                      // MODEM INITIALIZATION TIME

unsigned int
GPRS_Count;                                                                     // GPRS DATA COUNTER                   
//UpLoad_Time;                                                                    // GPRS DATA UPLOAD TIMEe

unsigned long 
File_Size;                                                                      // RECEIVED FILE SIZE 
/*******************************************************************************
* Function Name : GPRS USART Initialization
* Description   : Initialization USART communication
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void INIT_USART_GPRS(void)
{
  GPIO_InitTypeDef              GPIO_InitStructure;
  USART_InitTypeDef             USART_InitStructure;
    
  GPIO_PinAFConfig(GPRS_PORT,GPIO_PinSource9,GPIO_AF_1);                           // PIN MAPPED TO ALTERNATRE FUNCTION 
  GPIO_PinAFConfig(GPRS_PORT,GPIO_PinSource10, GPIO_AF_1);                           // PIN MAPPED TO ALTERNATRE FUNCTION   
  
  /*---------- CONFIGURE USART1 AS ALTERNATE FUNCTION MODE & OPEN DRAIN ------*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                  // PIN MODE ALTERNATE FUNCTION 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                             // PIN @ MAX SPEED 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;                                // OUTPUT TYPE OPEN DRAIN                                       
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                             // NO PULL UP / NO PULL DOWN 
  GPIO_InitStructure.GPIO_Pin   = GPRS_Tx;                                      // TX PIN OF MICRO
  GPIO_Init(GPRS_PORT, &GPIO_InitStructure);                                    // INITIALIZE GPRS PORT
  
 /*---------- CONFIGURE USART1 RX AS ALTERNATE FUNCTION MODE & PULL UP ------*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                  // PIN IS INTERNALLY PULLED UP
  GPIO_InitStructure.GPIO_Pin = GPRS_Rx ;                                       // RX PIN OF MICRO
  GPIO_Init(GPRS_PORT, &GPIO_InitStructure);                                    // INITIALIZE GPRS PORT

  
  
  /* USART1 configuration - for GPRS/GSM-------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = Define in Board Defination
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);                        // ENABLE USRAT 1 CLOCK
  USART_DeInit(GPRS_USART);                                                     // RESET TO DEFAULT STATE 

  USART_InitStructure.USART_BaudRate = BAUD_RATE_GPRS;                          // SET GPRS BAUDRATE  DEFINED IN HEADER FILE
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                   // 8 BIT
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                        // 1 STOP
  USART_InitStructure.USART_Parity = USART_Parity_No;                           // PARITY NONE
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// HARDWARE FLOW NONE
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;               // Rx & Tx ENABLED

  /* INITIALIZE USART 1 */
  USART_Init(GPRS_USART, &USART_InitStructure);                                 // INITIALIZE USART 1 

  /* Enable the USART Receive interrupt: this interrupt is generated when the
   USART1 receive data register is not empty */
  USART_ITConfig(GPRS_USART, USART_IT_RXNE, ENABLE);                            // ENABLE RECEIVE INTERRUPT

  /* Enable USART1 */
  USART_Cmd(GPRS_USART, ENABLE);                                                // ENABLE USART 1
}
/*******************************************************************************
* Function Name : Serial_SendData_GPRS
* Description   : Send Data to Modem
* Input         : one hex data only
* Output        : None
* Return        : None
*******************************************************************************/
void Serial_SendData_GPRS(char SerialData)					// THIS FUNCTION SEND ONLY ONE HEX VALUE AT A TIME	
 {
   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// WAIT FOR COMPLETE TRANMISSION TILL FLAG IS 0 
     
   GPRS_USART->TDR = (SerialData & (uint16_t)0x01FF);                           // WRITE IN DATA REGISTER OF GPRS FOR SENDING NEXT DATA                          
 }
/*******************************************************************************
* Function Name  : Serial_PutData
* Description    : Print a string on the GPRS Modem till place define
* Input          : A String up to given place
* Output         : None
* Return         : None
*******************************************************************************/
void Serial_PutData_GPRS(char *s, char Place)                                   // THIS FUNCTION SEND A STRING'S CHARACTERS UPTO GIVEN PLACE 
{  
  mSec_Delay(10);
  while (*s != '\0' && Place)
  {
    Place--;                                                                    // PUT DATA TO USART UPTO NUMBER OF PLACE AS POINTER BY POINTER 
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);                // WAIT UNTILL LAST TRANSMISSION COMPLETES
    GPRS_USART->TDR = (*s & (uint16_t)0x01FF);                                  // WRITE IN DATA REGISTER OF GPRS FOR SENDING NEXT DATA
    s++;                                                                        // INCREMENT POINTER
    mSec_Delay(2);                                                              // WAIT FOR 2 MILI SECOND
  };
   mSec_Delay(5);    
}
/*******************************************************************************
* Function Name  : Serial_PutString
* Description    : Print a string on the GPRS Modem
* Input          : A String till null character
* Output         : None
* Return         : None
*******************************************************************************/
void Serial_PutString_GPRS(char *s)                                             // THIS FUNCTION SEND STRING'S CHARACTER TILL NULL CHARACTER
{  
   mSec_Delay(10);
   while (*s != '\0')                                                            // PUT STRING OVER USART TILL NULL CHARACTER 
   {
     while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);               // WAIT UNTILL LAST TRANSMISSION COMPLETES
     GPRS_USART->TDR = (*s & (uint16_t)0x01FF);                                  // WRITE IN DATA REGISTER OF GPRS FOR SENDING NEXT DATA
     s++;                                                                        // INCREMENT POINTER
     mSec_Delay(2);                                                                     
   };
}
/*******************************************************************************
* Function Name : Modem_PutString_GPRS
* Description   : Send data to Modem and add 13(LF) at the end
* Input         : string
* Output        : None
* Return        : None
*******************************************************************************/
void Modem_PutString(char *s)                                                   // THIS FUNCTION SENDS A STRING TO GPRS MODEM AND ADD LF AT THE END 
{
  
 memset(GPRS_Buffer,0,GPRSBuffer_Size);
 GPRS_Count = 0;   
 mSec_Delay(10);
 Flag_ModemOK = RESET;
  while (*s != '\0')
  {
   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// WAIT FOR COMPLETE TRANSMISSION TILL FLAG IS 0
     
   GPRS_USART->TDR = (*s & (uint16_t)0x01FF);                                   // SEND NEXT STRING VALUE
   s++;                                                                         // INCREMENT POINTER 
   mSec_Delay(2);
  };
  mSec_Delay(5);
  
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	                // WAIT FOR COMPLETE TRANSMISSION TILL FLAG IS 0
  GPRS_USART->TDR = (13 & (uint16_t)0x01FF);					// SEND DATA BYTE 13
  
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	                // WAIT FOR COMPLETE TRANSMISSION TILL FLAG IS 0
  GPRS_USART->TDR = (10 & (uint16_t)0x01FF);					// SEND DATA BYTE 13
   
}
/*******************************************************************************
* Function Name : INIT_GPRS_MODEM
* Description   : Initialize GPRS Modem
* Input         : string
* Output        : None
* Return        : None
*******************************************************************************/
void INIT_GPRS_Modem(void)                                                      // THIS FUNCTION INITIALIZE THE MODEM FOR WORKING 
{
  char* RPtr;
  //Modem_PutString("AT+IFC=0,0");WAIT_MODEM_RESP(5);                              // HANDSHAKING NONE
  mSec_Delay(50); 
  Modem_PutString("AT+CMGF=1");WAIT_MODEM_RESP(5);                               // ENABLE TEXT MODE
 
  memset(GPRS_Buffer,0,50);GPRS_Count = 0;
  Flag_WrongIMEI = SET; 
  Modem_PutString("AT+GSN");                                                 // REQUEST FOR IMEI NUMBER   
  memset(GPRS_Buffer,0,sizeof(GPRS_Buffer));
  mSec_Delay(1500);
  if(strstr((char*)GPRS_Buffer,"OK"))                           
  {
    if(RPtr = strstr((char*)GPRS_Buffer,"GSN:"))   
    {
      Flag_WrongIMEI = RESET;
      RPtr += 5;
      if(*RPtr < '0' || *RPtr > '9') RPtr += 2;                                // SET POINTER TO READ IMEI NUMBER 
      for(GP_Counter = 0;GP_Counter <=14; GP_Counter++,RPtr++)                    // EXTRACT 15 DIGIT IMEI NUMBER 
      {
        if(*RPtr < '0' || *RPtr > '9'){Flag_WrongIMEI = SET; break;}             // IF CHAR OTHER THAN NUMERIC IT MEANS WRONG IMEI NUMBER 
        GPRSData.IMEI_No[GP_Counter] = *RPtr;                                    // SAVE IMEI NUMBER                               
      };
      GPRSData.IMEI_No[GP_Counter] = 0;  
    };
  }                                                     // IF NOT GETTING CORRECT IMEI NUMBER  
  if(Flag_WrongIMEI == SET)SYSTEM_SW_RESET;                                      // RESET IF IMEI NOT VALID
  Modem_PutString("AT+CSCS=\"IRA\"");     WAIT_MODEM_RESP(2);                 // Set character set as GSM   
  Modem_PutString("AT+CSMP=17,167,0,241");WAIT_MODEM_RESP(5);                    // SET SMS TEXT MODE PARAMETERS DEFAULT VALUES
  mSec_Delay(100);
  Modem_PutString("AT+IPR=115200");WAIT_MODEM_RESP(5); 
  mSec_Delay(300);
}
/*******************************************************************************
* Function Name : Send_SMS
* Description   : Send SMS via GPRS Module 
* Input         : string
* Output        : None
* Return        : None
*******************************************************************************/
void Send_SMS(unsigned char Subject)
{
  
 unsigned char UpLoad_Time; 
 char *StrPtr;
 char buff[10];
 StrPtr = NULL;
 
 
 if((Flag_SerialFlash == RESET) && (DFU_Mob[0] > '3') && (DFU_Mob[0] <= '9'))
 {
 Serial_PutString_GPRS("AT+CMGS=");
 Serial_SendData_GPRS(0x22);                                                    // INPUT MOBILE NUMBER    
 Serial_SendData_GPRS('0');                                                     // FOR ALL MOBILE SMS ROAMING 
 Serial_PutData_GPRS(DFU_Mob,strlen(DFU_Mob));                                  // DFU MOBILE NUMBER 
 Serial_SendData_GPRS(0x22);Serial_SendData_GPRS(13);
 
 Serial_PutString_GPRS("Device-");Serial_PutString_GPRS(GPRSData.IMEI_No);
 
 switch(Subject)
   {
     default                : break;
     case FAIL              : Serial_PutString_GPRS("-DFU FAIL");break;
     case UNABLE_TO_CONNECT : Serial_PutString_GPRS("-Unable to connect DFU Server");break;
     case UNABLE_TO_DWLOAD  : Serial_PutString_GPRS("-Unable to Download");break;
     case WRONG_FILE        : Serial_PutString_GPRS("-Wrong File on Server,");break;
     case SPI_FAIL          : Serial_PutString_GPRS("-SPI FLASH FAIL");
                              Serial_PutString_GPRS("\n DFU can not be performed");break;
     case ERROR             : Serial_PutString_GPRS("-Download Error");break;
     case RECORD_FAIL       : Serial_PutString_GPRS("-Failed to save status file on server");break;
     case ERRORCODE         : memset(buff,0,10);
                              sprintf(buff,"%d",ErrorCode);
                              Serial_PutString_GPRS("-Error Code : "); 
                              Serial_PutString_GPRS(buff);
                              IAP_Counter +=2;
                              break;
   }

   IAP_Status = RTC_ReadBackupRegister(RTC_BKP_DR4) & 0x00ff;
   GP_Counter = IAP_Counter << 8 | IAP_Status; 
   RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);
      
   if(IAP_Counter == 9)Serial_PutString_GPRS("\n LAST RETRY FROM DEVICE");
   if(IAP_Counter >= 10)
    {
     Serial_PutString_GPRS("\n Returning to Prev. Ver");
    }
     
   mSec_Delay(100);Serial_SendData_GPRS(26);                                      // send SMS

   UpLoad_Time = 30;StrPtr = NULL;Flag_ModemError = RESET;                      // wait 10 Sec for SMS sending
   while(StrPtr == NULL && (UpLoad_Time > 0) && Flag_ModemError == RESET)       // wait till SMS Sent
    {
      if(StrPtr = strstr((char*)GPRS_Buffer,"+CMGS:")){memset(GPRS_Buffer,0,GPRSBuffer_Size);break;}
      else if(Flag_Second == SET){Flag_Second = RESET;UpLoad_Time--;IWDG_ReloadCounter();};
    };
   
   if(IAP_Counter >= 10)
    {
     IAP_Counter = 0;
     RTC_WriteBackupRegister(RTC_BKP_DR4,0);                                       // RETURNING TO PREVIOUS VERSION
     SYSTEM_SW_RESET;
    }
 }
}                        
/*******************************************************************************
* Function Name : Set_APN
* Description   : Set Access Point Name based on service provider
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void Set_APN(void)
{
  char *Ptr;
  Ptr = 0;
  
  memset(GPRS_Buffer,0,50);GPRS_Count = 0;
  
  if(Flag_ModemOK == SET){ memset(GPRS_Buffer,0,50);GPRS_Count = 0; Modem_PutString("AT+COPS?");WAIT_MODEM_RESP(2);}
 
  if(Ptr = strstr((char*)GPRS_Buffer,"OPS:")) 
  {
//     Ptr += 4;
//     if(*Ptr == 0x22)
//     {
//       Ptr += 1;
//       for(GP_Counter = 0;GP_Counter < 10; GP_Counter ++,Ptr++)
//       {
//          if(*Ptr == 0x22){GPRSData.Operator[GP_Counter] = 0; break;}
//          else 
//            {
//             GPRSData.Operator[GP_Counter] = *Ptr;
//             if(islower(GPRSData.Operator[GP_Counter]))GPRSData.Operator[GP_Counter] = toupper(GPRSData.Operator[GP_Counter]);
//            };
//        };
//    }; 
      Ptr = strchr(Ptr, '"');                    // find opening quote
      if (Ptr != NULL)
      {
        Ptr++;                                 // first char of operator name
        
        for (GP_Counter = 0; GP_Counter < (sizeof(GPRSData.Operator) - 1); GP_Counter++, Ptr++)
        {
          if (*Ptr == '"' || *Ptr == '\0') break;     // end of name
          GPRSData.Operator[GP_Counter] = toupper((unsigned char)*Ptr);
        }
        GPRSData.Operator[GP_Counter] = '\0';
        
        // compare only after the full name is copied
        if (strcmp(GPRSData.Operator, "IND AIRTEL") == 0)
        {
          strcpy(GPRSData.Operator, "AIRTEL");        // shorter, so it always fits
        }
      }    
    
   memset(GPRS_Buffer,0,50);GPRS_Count = 0;
   char APN[100] = {'\0'};
   
   // GET APN Name from Operator
   if((strstr((char*)GPRSData.SET_APN,"no-apn")) || (strstr((char*)GPRSData.SET_APN,"NO-APN")))  
   {
    if(strstr((char*)GPRSData.Operator,"AIRTEL"))sprintf(APN,"taisysnet");
    else if(strstr((char*)GPRSData.Operator,"BSNL"))sprintf(APN,"bsnlnet");
    else if(strstr((char*)GPRSData.Operator,"VODAFONE"))sprintf(APN,"m2misafe");
   }
   else  sprintf(APN,"%s",GPRSData.SET_APN);
   
   // Request APN Name to check
   Modem_PutString("AT+CGDCONT?");WAIT_MODEM_RESP(2); mSec_Delay(1000);
   if(Ptr = strstr((char*)GPRS_Buffer,"CONT: "))
   {
      if(Ptr = strstr((char*)GPRS_Buffer,APN))
      {
        Flag_APNSet = SET;
        memset(GPRSData.APN_Name,0,19);
        for(GP_Counter = 0;GP_Counter < 19; GP_Counter ++,Ptr++)
        {
          if (*Ptr == 0x22){GPRSData.APN_Name[GP_Counter] = 0; break;}
          else GPRSData.APN_Name[GP_Counter] = *Ptr;
        };
      }
    };
  
   // If not set then SET APN Name
   if(Flag_APNSet == RESET)
   {
     char GPRS_MsgBuffer[100];
     memset(GPRS_MsgBuffer,0,sizeof(GPRS_MsgBuffer));
     sprintf(GPRS_MsgBuffer,"AT+CGDCONT=1,\"IP\",\"%s\"",APN);
     Modem_PutString(GPRS_MsgBuffer); WAIT_MODEM_RESP(2);
   }
 }
  
  Flag_ModemOK = RESET;

}
/*******************************************************************************
* Function Name : Wait Modem Response
* Description   : Waits for the Modem Response
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void WAIT_MODEM_RESP(unsigned char UpLoad_Time)                           
{  
  char *StrPtr;
  StrPtr = NULL;
 
  Flag_ModemError = RESET;Flag_ModemOK = RESET;                                 // RESET ALL MODEM RELATED FLAGS
  while(Flag_ModemOK == RESET && UpLoad_Time > 0 && Flag_ModemError == RESET)
   {
     if(Flag_Second == SET)
        {
         Flag_Second = RESET; UpLoad_Time--;IWDG_ReloadCounter();               // DECREMENT UPLOAD TIME 
        };
     
     if(StrPtr = strstr((char*)GPRS_Buffer,"OK")){Flag_ModemOK = SET;GPRS_Count = 0;Modem_NoResp = 0;}
     if(StrPtr != NULL){*StrPtr = ' '; StrPtr++;*StrPtr = ' '; StrPtr = NULL;}; // INSERT 2 BLANKS
       
  };
  
  if(UpLoad_Time == 0){Modem_NoResp++;};                       // UPLOAD ERROR 
  mSec_Delay(50);
}
/*******************************************************************************
* Function Name : Wait Modem Connect
* Description   : Waits for the Modem connect Response
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void WAIT_MODEM_CONNECT(void)
{
  unsigned char UpLoad_Time;
 
  UpLoad_Time = 75;Flag_ModemOK = RESET;                           // SET UPLOAD TIME 
  while(UpLoad_Time > 0 )
    {
      if(Flag_Second == SET)
      {
         Flag_Second = RESET;UpLoad_Time--;IWDG_ReloadCounter();
      };
      
     
      if(strstr((char*)GPRS_Buffer,"CONNECT")){Flag_ModemOK = SET; Modem_NoResp = 0; break;}
      if(strstr((char*)GPRS_Buffer,"ERROR")){Flag_ModemError = SET; Flag_ModemOK = RESET;break;} 
      
    };
  if(UpLoad_Time == 0) Flag_ModemOK = RESET;

}
/*******************************************************************************
* Function Name : Check_Signal_Nw
* Description   : Check Signal Strength & Network Parameters
* Input         : Parameter name & place
* Output        : Parameter new value
* Return        : Paremeter
*******************************************************************************/
void Check_Signal_Nw_GPRS(void)
{     
  char *StrPtr;
  StrPtr = NULL;
 
  Flag_ValidNetwork = RESET;                                                    // TO AUTHENTICATE CORRECT EXECUTION
  memset(GPRS_Buffer,0,50); GPRS_Count = 0;
  Modem_PutString("AT+CSQ");WAIT_MODEM_RESP(5);                                 // CHECK SIGNAL STRENGTH 
 
  /*----- GET SIGNAL STRENGTH & REQUEST NETWORK INFORMATION ------------------*/
  if(Flag_ModemOK == SET)
    {
      Flag_ModemOK = RESET;
      if(StrPtr = strstr((char*)GPRS_Buffer,"Q:"))
       {
         StrPtr += 3;GPRSData.Signal_Strength = atoi(StrPtr);        // EXTRACT SIGNAL STRENGTH
         if(GPRSData.Signal_Strength >= 35 )GPRSData.Signal_Strength = 0;       // SET SIGNAL STRENGTH TO 0 IF INVALID/GARBAGE
         if(GPRSData.Signal_Strength >= VALID_SIGNAL )                          // CHECK AUTHENTICATION OF SIGNAL STRENGTH
         {
          memset(GPRS_Buffer,0,50); GPRS_Count = 0; 
          Modem_PutString("AT+CGREG?");WAIT_MODEM_RESP(5);
         } 
       }
     
      if(StrPtr = strstr((char*)GPRS_Buffer,"Q:")){*StrPtr = ' ';StrPtr++;*StrPtr = ' ';};// INSERT BLANK 
    };
  
  /*----- GET LOCATION & GPRS AVAILABILITY -----------------------------------*/
  if(Flag_ModemOK == SET)                                                                       
     {  
      if(StrPtr = strstr((char*)GPRS_Buffer,"EG:"))
        {
          StrPtr += 6;
          if((*StrPtr == '1') || (*StrPtr == '5'))
          {memset(GPRS_Buffer,0,50); GPRS_Count = 0;Modem_PutString("AT+CGATT?");WAIT_MODEM_RESP(5);}
          else {Flag_ModemOK = RESET;Reg_Error++;};
        }
      else{Flag_ModemOK = RESET;Reg_Error++;};
    }
  
  /*----- CHECK NETWORK VALIDATION -------------------------------------------*/
   if(Flag_ModemOK == SET)                                                                        
     { 
      if(StrPtr = strstr((char*)GPRS_Buffer,"TT:"))
      {
        StrPtr += 4;
        if(*StrPtr == '1'){Flag_ValidNetwork = SET;Reg_Error = 0;}
        else Reg_Error++;
      }
     };
  
  if(Reg_Error > 50){Reg_Error = 0;INIT_APP();};                                // RESET APPLICATION
  
  /*----- REDO APN SETTING IF NETWORK IS NOT VALID ---------------------------*/
  if(Flag_ValidNetwork == RESET)Flag_APNSet = RESET;
           
}
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/