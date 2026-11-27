/******************** (C) COPYRIGHT 2018 EDS INDIA *****************************
* File Name     : App_init.c					       	       *
* Author        : DILEEP SINGH                                                 *
* Date          : 21/02/2018                                                   *
* Description   : This file provides all Application Initialization func.      *
* Revision	: Rev0						               *
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "App_Include.h"                                                        
char 
SER_USN[11],
SER_PWD[11];

void INIT_APP(void)
{
  unsigned short FlashData;
  POWER_ON;                                                                     // ENABLE EXTERNAL POWER SUPPLY
  
  /*----- CHECK SPI FLASH MEMORY ---------------------------------------------*/
  FlashAddress = SPI_FLASH_ReadID();                                            // GET FLASH STATUS WORKING 
  if(FlashAddress == 0xEF4018 || FlashAddress == 0xEF4017 || FlashAddress == 0x852018)Flag_SPIFail = RESET;
  else Flag_SPIFail = SET;  
  
  /****** FLASH READING *****************************************************/
  FlashAddress = EndAddr-FLASH_PAGE_SIZE;

  FlashData = (*(vu16*) FlashAddress);FlashAddress += 2;                      // VERIFY FLASH DATA 
  if(FlashData == 0x00ED)  
  {

    FlashAddress = 0x0803F9B2; // USER NAME SERIAL LOGIN
    for(GP_Counter = 0;GP_Counter < 10; GP_Counter++)                          // GET SET APN                                        
    {
        FlashData = (*(vu16*) FlashAddress);  
        SER_USN[GP_Counter+1] = FlashData >> 8;
        SER_USN[GP_Counter] = (char)FlashData; 
        FlashAddress += 2;GP_Counter++;
    };
    //0x0803F9BC // PASSWORD SERIAL LOGIN
    for(GP_Counter = 0;GP_Counter < 10; GP_Counter++)                          // GET SET APN                                        
    {
        FlashData = (*(vu16*) FlashAddress);  
        SER_PWD[GP_Counter+1] = FlashData >> 8;
        SER_PWD[GP_Counter] = (char)FlashData; 
        FlashAddress += 2;GP_Counter++;
    };

    if(Flag_SerialFlash == RESET)
    {

/*----- FLASH READING ------------------------------------------------------*/
  //FlashAddress = 0x0803F866; //0x0803F856 
   FlashAddress = (EndAddr-FLASH_PAGE_SIZE) + 1024-32; 
      
      
  for(GP_Counter = 0;GP_Counter < 32; GP_Counter++)                          // GET SET APN                                        
     {
      FlashData = (*(vu16*) FlashAddress);  
      GPRSData.SET_APN[GP_Counter+1] = FlashData >> 8;GPRSData.SET_APN[GP_Counter] = (char)FlashData; 
      if( GPRSData.SET_APN[0] == 255 || GPRSData.SET_APN[0] == 0){sprintf(GPRSData.SET_APN,"no-apn");break;}
      FlashAddress += 2;GP_Counter++;
     }; 
  
  FlashAddress = (EndAddr-FLASH_PAGE_SIZE) + 1024 - 32; 
  for(GP_Counter = 0;GP_Counter < 32; GP_Counter++)                          // GET SET APN                                        
     {
      FlashData = (*(vu16*) FlashAddress);  
      GPRSData.APN_Name[GP_Counter+1] = FlashData >> 8;GPRSData.APN_Name[GP_Counter] = (char)FlashData; 
      if(GPRSData.APN_Name[0] == 255 || GPRSData.APN_Name[0] == 0){break;}
      FlashAddress += 2;GP_Counter++;
     }; 

/*****************************************************************************/
/*                        DFU BLOCK                                          */
/*****************************************************************************/
  FlashAddress = (EndAddr-FLASH_PAGE_SIZE) + 1024;   

 /*----- HTTP IP ADDRESS --------------------------------------------------*/
 FlashData = (*(vu16*) FlashAddress);
 DFU_HTTP.IP[0] = (char)FlashData;DFU_HTTP.IP[1] = FlashData >> 8;FlashAddress += 2;                             // DFU IP TAB 1,2
 FlashData = (*(vu16*) FlashAddress);
 DFU_HTTP.IP[2] = (char)FlashData;DFU_HTTP.IP[3] = FlashData >> 8;FlashAddress += 2;                             // DFU IP TAB 3,4

 
 /*----- HTTP PATH ----------------------------------------------------*/
 for(GP_Counter = 0;GP_Counter <= 49; GP_Counter++)                             // GET FTP USERNAME   
 {
   FlashData = (*(vu16*) FlashAddress);
   DFU_HTTP.PATH[GP_Counter+1] = FlashData >> 8;DFU_HTTP.PATH[GP_Counter]  = (char)FlashData;  
   FlashAddress += 2;GP_Counter++;
  };

 /*----- HTTP FILE -----------------------------------------------------*/
 for(GP_Counter = 0;GP_Counter <= 15; GP_Counter++)                            // GET FTP USERNAME   
 {
   FlashData = (*(vu16*) FlashAddress);
   DFU_HTTP.FILENAME[GP_Counter+1] = FlashData >> 8;DFU_HTTP.FILENAME[GP_Counter]  = (char)FlashData;  
   FlashAddress += 2;GP_Counter++;
 };
 
 /*----- DFU NUMBER -------------------------------------------------------*/
 for(GP_Counter = 0;GP_Counter < 15; GP_Counter++)                             // GET DFU MOBILE NUMBER TO UPGRADE SOFTWARE VERSION  
 {
   FlashData = (*(vu16*) FlashAddress);
   DFU_Mob[GP_Counter+1] = FlashData >> 8;DFU_Mob[GP_Counter]  = (char)FlashData;  
   FlashAddress += 2;GP_Counter++;
 };

  /*----- NEXT SOFTWARE VERSION ----------------------------------------------*/
  FlashData = (*(vu16*) FlashAddress);
  SW_VER[0] = (char)FlashData;SW_VER[1] = FlashData >> 8;  FlashAddress += 2;    
  FlashData = (*(vu16*) FlashAddress);
  SW_VER[2] = (char)FlashData;SW_VER[3] = FlashData >> 8;  FlashAddress += 2;    
  FlashData = (*(vu16*) FlashAddress);
  SW_VER[4] = (char)FlashData;SW_VER[5] = FlashData >> 8;  FlashAddress += 2;    // SW Ver 
  FlashData = (*(vu16*) FlashAddress);
  SW_VER[6] = (char)FlashData;SW_VER[7] = FlashData >> 8;  FlashAddress += 2;
  FlashData = (*(vu16*) FlashAddress);
  SW_VER[8] = (char)FlashData;SW_VER[9] = FlashData >> 8;  FlashAddress += 2;


/*----- CURRENT SOFTWARE VERSION -------------------------------------------*/
FlashData = (*(vu16*) FlashAddress);
Curr_SWVer[0] = (char)FlashData;Curr_SWVer[1] = FlashData >> 8; FlashAddress += 2;  
FlashData = (*(vu16*) FlashAddress);
Curr_SWVer[2] = (char)FlashData;Curr_SWVer[3] = FlashData >> 8; FlashAddress += 2;  
FlashData = (*(vu16*) FlashAddress);
Curr_SWVer[4] = (char)FlashData;Curr_SWVer[5] = FlashData >> 8; FlashAddress += 2;  
FlashData = (*(vu16*) FlashAddress);
Curr_SWVer[6] = (char)FlashData;Curr_SWVer[7] = FlashData >> 8; 
FlashData = (*(vu16*) FlashAddress);
Curr_SWVer[8] = (char)FlashData;Curr_SWVer[9] = FlashData >> 8; 



}
}
  else 
  {
    sprintf(SER_USN,"MMI");
    sprintf(SER_PWD,"MMI@1234");
    
    DFU_HTTP.IP[0] = 13;DFU_HTTP.IP[1] = 234;
    DFU_HTTP.IP[2] = 46;DFU_HTTP.IP[3] = 63;
    
    sprintf(DFU_HTTP.PATH,"4GAIS_MMI");
    sprintf(DFU_HTTP.FILENAME,"V-1.0.00");
    sprintf(DFU_HTTP.URL,"http://%d.%d.%d.%d/%s/%s.bin",DFU_HTTP.IP[0],DFU_HTTP.IP[1],DFU_HTTP.IP[2],DFU_HTTP.IP[3],&DFU_HTTP.PATH[0],&DFU_HTTP.FILENAME[0]);
      
  }
  
  /*----- INITIALIZE DEVICE --------------------------------------------------*/
  if(VTS_State > 2)VTS_State = MODEM_INIT;                                                       // FIRST INITIALIZE GPRS MODEM   
  
  Flag_ModemRdy = RESET;                                                        // MODEM READY RESET       
  Flag_ValidNetwork = RESET;                                                    // VALID NETWORK RESET 
  Flag_APNSet = RESET;                                                          // APN NAME RESET 
  Flag_NoResp = SET;                                                            // INITIAL NO RESPONSE FROM MODEM 
  
  Init_Time = 25;                                                               // MODEM INITIALIZATION TIME 15 SECONDS
  Modem_NoResp = 0;                                                             // RESET MODEM NO RESPONSE 


}
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/