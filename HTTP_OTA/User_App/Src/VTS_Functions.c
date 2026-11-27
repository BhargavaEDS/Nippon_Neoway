/******************** (C) COPYRIGHT 2018 EDS INDIA *****************************
* File Name     : VTS_Functions.c					       *
* Author        : DILEEP SINGH                                                 *
* Date          : 21/02/2018                                                   *
* Description   : This file contains all misc functions of VTS                 *
* Revision	: Rev0    						       *
********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "App_Include.h"


static void DataErr(void);
unsigned int NOLs =0, WrnData = 0;
int Wait = 80;
long FileLength = 0;
long FileLoc = 0;
int SPI_Write_Byte = 0;  

uint8_t 
Data_WaitTime;
  

FlagStatus Flag_GetFile_Serial;

/*******************************************************************************
* Function Name  : GPRS_Module_ON
* Description    : Powers ON the GPRS module. First enables the module power
*                  supply and waits 3 sec for the supply to stabilize. Then
*                  asserts the PWRKEY line for 2 sec (power-on pulse) and
*                  releases it. The module boots after the pulse is released.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPRS_Module_ON(void)
{
  GPRS_ON;
  mSec_Delay(3000);
  PWRKEY_ENB;
  mSec_Delay(2000);
  PWRKEY_DSB;
}

/*******************************************************************************
* Function Name  : GPRS_Module_OFF
* Description    : Powers OFF the GPRS module. First asserts the PWRKEY line
*                  for 3 sec (power-off pulse) so the module shuts down
*                  gracefully and detaches from the network. Then releases
*                  PWRKEY and cuts the module power supply.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPRS_Module_OFF(void)
{
  PWRKEY_ENB;
  mSec_Delay(3000);
  PWRKEY_DSB;
  GPRS_OFF;
}


/*******************************************************************************
* Function Name  : mSec_Delay
* Description    : Insert Delay of milli Second
* Input          : val Number of cycle
* Output         : None
* Return         : None
*******************************************************************************/
void mSec_Delay(unsigned int nTime)
{

 while(nTime != 0)                                                              // Msec Delay 
  { 
    if(Flag_mSec == SET){Flag_mSec = RESET;nTime--;};                           // MSec Flag is set by Sys tick timer
  };
 
}
/*******************************************************************************
* Function Name : Get Firmware
* Description   : Get firmware file from FTP
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void Get_Firmware(void)
{
  char* PTR, CMD_Buff[512];
  uint32_t Present_Pointer = 0;
uint16_t wait;
  /*----- CHECK SIGNALS & NETWORK --------------------------------------------*/
  Check_Signal_Nw_GPRS();                                                       // CHECK SIGNAL STRENGTH NETWORK REGISTRATION GPRS ATTACHMENT 
  Get_CSQ++;
  if(Get_CSQ > 31)SYSTEM_SW_RESET;
  if(Flag_ValidNetwork == SET && Flag_APNSet == RESET){Get_CSQ = 0; Set_APN();}               // IF NETWORK VALID SET APN
  
  if(Flag_APNSet)
  {
      char buff[64];
      //Serial_PutString_GPRS("AT+HTTPPARA=url,");                                    // AT+OHTTPURL - set URL address
//      memset(buff,0,100);
//      if(DFU_HTTP.URL[0] != 'h')
//        sprintf(DFU_HTTP.URL,"http://%d.%d.%d.%d/%s/%s.bin",DFU_HTTP.IP[0],DFU_HTTP.IP[1],DFU_HTTP.IP[2],DFU_HTTP.IP[3],&DFU_HTTP.PATH[0],&DFU_HTTP.FILENAME[0]);
//      sprintf(DFU_HTTP.URL,
//      sprintf(buff,"%d,10",strlen(DFU_HTTP.URL));
//      Serial_PutString_GPRS(buff);                                              // URL address data length
//      Serial_SendData_GPRS(13); Serial_SendData_GPRS(10);                                                 // Address upload time
//      WAIT_MODEM_CONNECT();           
//      // PUT URL & Wait for connect
//      Modem_PutString(DFU_HTTP.URL);
//      WAIT_MODEM_RESP(2);      
//      // Get Request
//      Modem_PutString("AT+QHTTPGET=80");WAIT_MODEM_RESP(5); 
      strcpy((char*)DFU_HTTP.FILENAME,"NN1.bin");
      sprintf(DFU_HTTP.URL,"http://13.234.46.63/FOTA/Download/NN1.bin");
      memset(CMD_Buff,0,sizeof(CMD_Buff));
      sprintf(CMD_Buff,"AT+HTTPPARA=url,%s",DFU_HTTP.URL);
      Modem_PutString(CMD_Buff);
      WAIT_MODEM_RESP(5);
      mSec_Delay(1000);
      
      Modem_PutString("AT+XIIC=1");
      WAIT_MODEM_RESP(5);
      mSec_Delay(1000);
      
      Modem_PutString("AT+HTTPPARA=port,80");
      WAIT_MODEM_RESP(5);
      mSec_Delay(1000);
      
      Modem_PutString("AT+HTTPSETUP");
      WAIT_MODEM_RESP(5);
      mSec_Delay(1000);
      
      Modem_PutString("AT+HTTPGET=0");
      WAIT_MODEM_RESP(5);
      mSec_Delay(3000);
      
      Modem_PutString("AT+FSLIST?");
      WAIT_MODEM_RESP(5);
      mSec_Delay(3000);
      
      char *Ptr;
      while(Wait)
      {
        mSec_Delay(100);
        if(Flag_Second){IWDG_ReloadCounter(); Flag_Second = RESET; Wait--;}
        if(Wait == 1){ Flag_APNSet = RESET; return;} // try again         
//        if(Ptr = strstr(GPRS_Buffer,"GET: "))
//        {
//          mSec_Delay(500);
//          Ptr += 5;
//          if(*Ptr != '0'){ Flag_APNSet = RESET; return;} // try again
//          Ptr += 6;
//          FileLength = atoi(Ptr);
//          Wait = 0;
//       }
        if(Ptr = strstr(GPRS_Buffer,DFU_HTTP.FILENAME))
        {
          mSec_Delay(500);
          Ptr = strchr(Ptr,',');
          FileLength = atoi(Ptr+1);
          Wait = 0;
          Flag_APNSet = SET;
        }
        else Flag_APNSet = RESET;
      }

//      // Read in File
//      Modem_PutString("AT+QHTTPREADFILE=\"FOTA.txt\",80");WAIT_MODEM_RESP(2);
//      Wait = 80;
//      while(Wait)
//      {
//        mSec_Delay(100);
//        if(Flag_Second){Flag_Second = RESET;IWDG_ReloadCounter(); Wait--;}
//        if(Wait == 1){ Flag_APNSet = RESET; return;} // try again
//        if(Ptr = strstr(GPRS_Buffer,"READFILE: 0"))Wait = 0;
//      }
//      
//       // Open File
//      int FileHandel = 0;Wait = 80;
//      Modem_PutString("AT+QFOPEN=\"FOTA.txt\",2");WAIT_MODEM_RESP(2); // Open in Readonly mode
//      while(Wait)
//      {
//        mSec_Delay(100);
//        if(Flag_Second){Flag_Second = RESET;IWDG_ReloadCounter(); Wait--;}
//        if(Wait == 1){ Flag_APNSet = RESET; return;} // try again
//        if(Ptr = strstr(GPRS_Buffer,"OPEN: "))
//        {
//          Ptr += 6;
//          Wait = 0;
//          FileHandel = atoi(Ptr);
//        }
//      }
      
      // Start Download in SPI
      FlashAddress = SPIFLASH_DATA;Write_Loc = 0;
      Flag_ModemError = RESET;Modem_NoResp = 0;
      Flag_Download = SET;
      while(FileLength)
      {
//        sprintf(buff,"AT+QFSEEK=%d,%d,0",FileHandel,FileLoc);
//        Modem_PutString(buff);WAIT_MODEM_RESP(2); // Open in Readonly mode
        
        if(FileLength > FOTA_PKT)
        {
          if(!Flag_FRTM)
          {
            sprintf(buff,"AT+FSRF=%s,%d,%d",DFU_HTTP.FILENAME,Present_Pointer, FOTA_PKT);
            Flag_FRTM = SET;
            
          }
          else sprintf(buff,"AT+FSRF=%s,1,%d,%d",DFU_HTTP.FILENAME,Present_Pointer, FOTA_PKT);
          
         // mSec_Delay(1000);
          Modem_PutString(buff);
        }
        else
        {
          //mSec_Delay(1000);
          sprintf(buff,"AT+FSRF=%s,1,%d,%d",DFU_HTTP.FILENAME,Present_Pointer,FileLength);
          Modem_PutString(buff);
        } 
        
        Flag_GetFile_Serial = RESET;Flag_WriteFile = RESET;
        Data_WaitTime = 30; 
        Flag_GetFile = SET;
        while(Data_WaitTime > 0)
        {
          if(Flag_Second){Flag_Second = RESET; Data_WaitTime--;}
          
//          while (!(Flag_WriteFile || strstr(Rev_Buffer, ":00000001FF")) && (wait < 3000))
//          {
//            mSec_Delay(10);
//            IWDG_ReloadCounter();   // keep the watchdog happy while waiting
//            wait += 10;
//          }
          //Flag_WriteFile = SET;
         // if (!Flag_WriteFile && strstr(Rev_Buffer, ":00000001FF")) return;
          if(Flag_WriteFile || strstr(Rev_Buffer,":00000001FF"))
          {
            
            mSec_Delay(50);
            PTR = strstr(&Rev_Buffer[0],"+FSRF: ");
            if(PTR == NULL) return;
            
            char *c = strchr(PTR, ',');
            if(c == NULL) return;
            PTR = c + 1;
            
            Flag_WriteFile = RESET;
            SPI_FLASH_SectorErase(FlashAddress);
            SPI_FLASH_SectorErase(FlashAddress + 4096);
            SPI_FLASH_BufferWrite(PTR,FlashAddress,8192);
            Rev_Count = 0;
            if(FileLength < 8192 && strstr(Rev_Buffer,":00000001FF"))
            {
              mSec_Delay(50);
              StoreSPI_Flash();
              if(Flag_LoadProg)Program_Flash();
              Flag_GetFile = RESET;
              return;
            }
            else FileLength -= 8192;
            Present_Pointer += FOTA_PKT;
            Data_WaitTime = 0;
            FileLoc += 8192;  FlashAddress += 8192; 
            memset(Rev_Buffer,0,8192);
          }
        }
       
      }
      
      
    }// APN SET
}
/*******************************************************************************
* Function Name  : StoreSPI_Flash
* Description    : Store download data in SPI flash
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void StoreSPI_Flash(void)
{
  Flag_Download = RESET;
  Flag_LoadProg = RESET;
  Flag_SaveFlash = SET;
  uint16_t
  Current_Add = 0, Last_Add = 0;
  unsigned char Calc_ChkSum = 0;
  unsigned char No_OfBytes = 0,Record,CheckSum = 0;
  char Byt_Count[4];
  FlagStatus
  Flag_DataError = RESET,
  Flag_Prog = SET;
  char *StrPtr;
  StrPtr = NULL;
 
  NOLs = 0;
 
  SPIFlashAddress = SPIFLASH_DATA;
  FlashAddress  = SPIFLASH_FOTA;
  
  for(GP_Counter = 0; GP_Counter < 64 ; GP_Counter++,FlashAddress +=4096){SPI_FLASH_SectorErase(FlashAddress); mSec_Delay(10);} // 256 KB
 
  
  
  Current_Add = 0x0000;
  Last_Add    = 0xFFFF;
NEXT:  
  while(Flag_Prog)
  {
  memset(GPRS_SendBuffer,0,100);
  SPI_FLASH_BufferRead(Byt_Count,SPIFlashAddress,4);
  if(StrPtr = strstr(Byt_Count,":"))StrPtr +=1;
  No_OfBytes = CONVERTHEX(*StrPtr) *16 + CONVERTHEX(*(StrPtr+1));
  SPI_FLASH_BufferRead(GPRS_SendBuffer,SPIFlashAddress,(13+ 2*No_OfBytes));
  StrPtr = GPRS_SendBuffer;

    
   for(GP_Counter = 0; GP_Counter < 30; GP_Counter++,StrPtr++)
     if(*StrPtr == ':'){StrPtr += 1; break;}
   
   if(GP_Counter == 30){Flag_DataError = SET;};    
       
   if(StrPtr)
   {
   /*----- NUMBER OF BYTES ---------------------------------------------------*/
   No_OfBytes = CONVERTHEX(*StrPtr) *16 + CONVERTHEX(*(StrPtr+1));StrPtr += 2;
   Calc_ChkSum = No_OfBytes;
  
   /*----- FLASH ADDRESS CHECKSUM --------------------------------------------*/
   FlashAddress = CONVERTHEX(*StrPtr) *16 + CONVERTHEX(*(StrPtr+1));StrPtr += 2;
   Calc_ChkSum += FlashAddress;
   FlashAddress = CONVERTHEX(*StrPtr) *16 + CONVERTHEX(*(StrPtr+1));StrPtr += 2;
   Calc_ChkSum += FlashAddress;StrPtr -= 4;
 
   
   
   /*----- FLASH ADDRESS FOR WRITTING ----------------------------------------*/   
   Current_Add = (CONVERTHEX(*StrPtr) * 4096) + (CONVERTHEX(*(StrPtr+1)) * 256) + (CONVERTHEX(*(StrPtr+2)) * 16) + CONVERTHEX(*(StrPtr+3));
   //FlashAddress = SPIFLASH_FOTA + StartAddress + (CONVERTHEX(*StrPtr) * 4096) + (CONVERTHEX(*(StrPtr+1)) * 256) + (CONVERTHEX(*(StrPtr+2)) * 16) + CONVERTHEX(*(StrPtr+3));
   FlashAddress = SPIFLASH_FOTA + StartAddress + Current_Add;
   //FlashAddress -= 0x5000;                                                      // application start address in program is 0x4C00;
   FlashAddress  -= (ApplicationAddress - 0x08000000);
   StrPtr+=4;
    
   /*----- RECORD TYPE -------------------------------------------------------*/
   Record = CONVERTHEX(*StrPtr) *16 + CONVERTHEX(*(StrPtr+1));StrPtr +=2;
   Calc_ChkSum += Record;
  
   if(Record == 0x04)
     {
      FlashAddress = CONVERTHEX(*StrPtr) * 16 + CONVERTHEX(*(StrPtr+1));
      StartAddress =  FlashAddress * 0x1000;StrPtr += 2;
      FlashAddress = CONVERTHEX(*StrPtr) * 16 + CONVERTHEX(*(StrPtr+1));
      FlashAddress *= 0x10;
      StartAddress +=  FlashAddress;
      StartAddress *= 0x1000;
      StartAddress -= 0x08000000;
      
      SPIFlashAddress += (13 + (2*No_OfBytes));
      goto NEXT;
      };
   
   /*----- DATA BYTES --------------------------------------------------------*/
   for(GP_Counter = 0; GP_Counter < No_OfBytes;GP_Counter++)
    {
     Program_Byte[GP_Counter] = CONVERTHEX(*StrPtr) *16 + CONVERTHEX(*(StrPtr+1)) ; 
     StrPtr += 2;
     Calc_ChkSum += Program_Byte[GP_Counter];
    }
   
   /*----- CHECKSUM ----------------------------------------------------------*/
   CheckSum = CONVERTHEX(*StrPtr) *16 + CONVERTHEX(*(StrPtr+1));StrPtr +=2;
   
   /*----- CALCULATE CHECKSUM ------------------------------------------------*/
   Calc_ChkSum = 0x100 - Calc_ChkSum;
   if(Record == 0x00)                                                           // DATA VALUE
    {
       if(Calc_ChkSum == CheckSum)
       { SPI_FLASH_BufferWrite((char*)Program_Byte,FlashAddress,No_OfBytes); NOLs++;}
       else {Flag_Prog = RESET;Flag_DataError = SET;}
      };
     
     
     // CHECK ADDRESS DIFFERENCE == 16 BYTE
     if(Record == 0x00)
     {
     if(Last_Add == 0xFFFF)
     {
       if(Current_Add == (ApplicationAddress - 0x08000000))
       {
         Last_Add = Current_Add;
       }
       else 
       {
         DataErr();
       }
     }
     else 
     {
       if(Last_Add == 0xFFF0)
       {
         if(Current_Add != 0)
         {
           DataErr();
         }
         else 
         {
           Last_Add = Current_Add;
         }
       }
       else 
       {
         if((Current_Add - Last_Add) == 0x10)
         {
           Last_Add = Current_Add;
         }
         else 
         {
           DataErr();           
           
         }
       }
     }
   }
   
  }; // END OF StrPtr
   
   if(*StrPtr == 0x0D)
    {
     if(No_OfBytes == 0 && CheckSum == 0xFF && Record == 0x01)
     {
       Flag_Prog = RESET;Flag_SaveFlash = RESET;Flag_LoadProg = SET;

       GP_Counter = IAP_Counter << 8 | 'D'; 
       RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);
       
     } 
     // VTS PROGRAM DATA HAVE BEEN EXTRACTED 
     else {Flag_Prog = SET;SPIFlashAddress += (13 + (2*No_OfBytes));}
    }
   else {Flag_Prog = RESET;Flag_DataError = SET;}
   
   if(Flag_Second == SET){ Flag_Second = RESET; IWDG_ReloadCounter();}
  }
     
 mSec_Delay(50);
 if(Flag_DataError)
 {
   if(IAP_Status != 'S')
   {
     Send_SMS(ERROR);GP_Counter = IAP_Counter << 8 | 'E'; RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);
   }
   else 
   { 
     GP_Counter = 255 << 8 | 'E';
     RTC_WriteBackupRegister(RTC_BKP_DR4,GP_Counter);
   }
   SYSTEM_SW_RESET;
 }
}
/*******************************************************************************
* Function Name  : Program_Flash
* Description    : Prepare the flash for new Program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Program_Flash(void)
{
  Flag_Download = RESET;
  Flag_SaveFlash = RESET;
  Flag_LoadProg = SET;
  
  volatile FLASH_Status FLASHStatus;
  unsigned short FlashData;
  short Prog_Loc;
  char Prog_Data[2048];
  
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();								// UNLOCK FLASH

  /* Erase the FLASH pages */
  for(FlashAddress = ApplicationAddress;FlashAddress <(EndAddr-(FLASH_PAGE_SIZE *2));FlashAddress += FLASH_PAGE_SIZE)
  {
    if(Flag_Second == SET){Flag_Second = RESET;  IWDG_ReloadCounter();}
    FLASHStatus = FLASH_ErasePage(FlashAddress);				// ERASE FLASH PAGE
  }
  WrnData = 0;
  FlashAddress = ApplicationAddress;
  SPIFlashAddress = SPIFLASH_FOTA;
  
  for(GP_Counter = 0;GP_Counter < (EndAddr-ApplicationAddress-(FLASH_PAGE_SIZE*2))/FLASH_PAGE_SIZE; GP_Counter++)                            // UPTO CONFIGURATION LOCATION 
   {
      SPI_FLASH_BufferRead((char*)Prog_Data,SPIFlashAddress,FLASH_PAGE_SIZE);
      for(Prog_Loc = 0; Prog_Loc < FLASH_PAGE_SIZE; Prog_Loc++)
       {
        FlashData =   (Prog_Data[Prog_Loc+1]<<8) | Prog_Data[Prog_Loc];		// GET RAM DATA           
        FLASH_ProgramHalfWord(FlashAddress, FlashData);
        FlashAddress += 2;Prog_Loc ++; 
        WrnData++;
        if(WrnData >= (NOLs*8))break;
       };
   
      SPIFlashAddress += FLASH_PAGE_SIZE;
      IWDG_ReloadCounter();
    
    if(WrnData >= (NOLs*8))break;
   };

  FLASH_Lock();
   
  IAP_Status = 'P';
  RTC_WriteBackupRegister(RTC_BKP_DR4,'P');
  mSec_Delay(1000);
  
  
  Save_Record_Try = 0;
  if(Flag_SerialFlash == SET)
  {
    Serial_PutString_232("SUCCEED OK",3);
  }
  
  
  FLASH_Unlock();
  FLASH_OB_Unlock();
  FLASH_EraseOptionBytes();
  FLASH_OB_RDPConfig(OB_RDP_Level_1);
  /* Enable the pages write protection */
  FLASH_EnableWriteProtection(OB_WRP_Pages0to1 | OB_WRP_Pages2to3 | OB_WRP_Pages4to5 | OB_WRP_Pages6to7 | OB_WRP_Pages8to9);
  FLASH_OB_Lock();
  FLASH_Lock(); 

  FLASH_OB_Launch();
  
 
  SYSTEM_SW_RESET;
 
}


/*******************************************************************************
* Function Name  : Serial_PutString_232
* Description    : Print a string on the GPRS Modem
* Input          : A String till null character
* Output         : None
* Return         : None
*******************************************************************************/
void Serial_PutString_232(char *s, uint8_t usart_no)                                              // THIS FUNCTION SEND STRING'S CHARACTER TILL NULL CHARACTER
{  
  mSec_Delay(10);
  if(usart_no == 3) 
  {
     while (*s != '\0')                                                            // PUT STRING OVER USART TILL NULL CHARACTER 
      {
        while (USART_GetFlagStatus(COM_USART, USART_FLAG_TXE) == RESET);               // WAIT UNTILL LAST TRANSMISSION COMPLETES
        COM_USART->TDR = (*s & (uint16_t)0x01FF);                                    // WRITE IN DATA REGISTER OF GPRS FOR SENDING NEXT DATA
        s++;                                                                        // INCREMENT POINTER
        mSec_Delay(2);
      };
       mSec_Delay(5);
   
      while (USART_GetFlagStatus(COM_USART, USART_FLAG_TXE) == RESET);	                // WAIT FOR COMPLETE TRANSMISSION TILL FLAG IS 0
      COM_USART->TDR = (10 & (uint16_t)0x01FF);					// SEND DATA BYTE 13 
      COM_USART->TDR = (13 & (uint16_t)0x01FF);					// SEND DATA BYTE 13 
  }
  else __NOP();
 
}
/*******************************************************************************
* Function Name : Get Firmware
* Description   : Get firmware file from FTP
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void Get_Firmware_Serial(void)
{
    char  buff[100];
    char* PTR;

    /*----- SUCCESSFULLY OPEN - DATA CONNECTION ----------------------------*/  
    mSec_Delay(10);
    
    FlashAddress = SPIFLASH_DATA;Write_Loc = 0;
    Flag_ModemError = RESET;
    
    Serial_PutString_232("OK",3); 
    Loc = 0;Write_Loc = 0;
    Wait = 30;
    // Read Length
    while(Wait && FileLength == 0)
    {
      mSec_Delay(100);
      if(Flag_Second){Flag_Second = RESET; Wait--;}
      if(Wait == 1){ return;} // try again         
      if(PTR = strstr(Ser_Buffer,"LENGTH-"))
      {
        mSec_Delay(500);
        PTR += 7;
        FileLength = atoi(PTR);
        Wait = 0;
     }       
    }
    
    Flag_GetFile_Serial = SET;
    Flag_GetFile = RESET;
    // Start Download in SPI
    FlashAddress = SPIFLASH_DATA;Write_Loc = 0;
    Flag_ModemError = RESET;Modem_NoResp = 0;
    Flag_Download = SET;
    while(FileLength)
    {
      
      sprintf(buff,"NEXT=%d",FileLoc);
      Serial_PutString_232(buff,3);
      
      Flag_WriteFile = RESET;
      Data_WaitTime = 30; 
      while(Data_WaitTime > 0)
      {
        if(Flag_Second)
        {
          Flag_Second = RESET;IWDG_ReloadCounter(); Data_WaitTime--;
          if(FileLength < 4096 && strstr(Rev_Buffer,":00000001FF"))Flag_WriteFile = SET;
        }
        
        if(Flag_WriteFile)
        {          
          mSec_Delay(100);
          PTR = strstr(&Rev_Buffer[0],"CONNECT");
          PTR = strstr(PTR,"\r\n") + 2;
          
          Flag_WriteFile = RESET;
          SPI_FLASH_SectorErase(FlashAddress);
          SPI_FLASH_BufferWrite(PTR,FlashAddress,4096);
          Rev_Count = 0;
          if(FileLength < 4096 && strstr(Rev_Buffer,":00000001FF"))
          {
            StoreSPI_Flash();
            if(Flag_LoadProg)Program_Flash();
            Flag_GetFile = RESET;
            return;
          }
          else FileLength -= 4096;
          Data_WaitTime = 0;
          FileLoc += 4096;  FlashAddress += 4096; 
          memset(Rev_Buffer,0,4096);
          
        }
      }   
    }
}


void DataErr(void)
{
  Serial_PutString_232("MISSING Data",3);
  RTC_WriteBackupRegister(RTC_BKP_DR4,0);
  SYSTEM_SW_RESET;
}
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/