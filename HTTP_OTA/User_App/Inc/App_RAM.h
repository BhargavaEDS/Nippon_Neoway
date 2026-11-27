/******************** (C) COPYRIGHT 2018 EDS INDIA ****************************
* File Name     : App_RAM.h					       	      *
* Author        : DILEEP SINGH                                                *
* Date          : 21/02/2018                                                  *
* Description   : This file Define all Variables used                         *
* Revision      : Rev0						       	      *
*******************************************************************************/

/* DEFINE TO PREVENT RECURSIVE INCLUSION -------------------------------------*/
#ifndef __Application_RAM_H
#define __Application_RAM_H

#define MODEM_INIT              0                                               // MODEM INITIALIZATION
#define NORMAL                  1                                               // VTS NORMAL RUNNING MODE
#define SERIAL                  2

#define Reveive_Buffer_Size 12300
/*----- FLASH PROGRAMMING FOR WRITTING CONFIG TABLE --------------------------*/
#define ApplicationAddress      ((u32)0x08005000)
#define EndAddr                 ((u32)0x08040000)                               // MICRO FLASH LAST ADDRESS BUT ONE PAGE 
#define FLASH_PAGE_SIZE         ((u16)0x800)                                    // MICRO FLASH PAGE SIZE

/*----- SECONDARY SPI FLASH WRITTING DATA TABLE ------------------------------*/
#define SPIFLASH_FOTA           0xF00000                                        // FOR STORING ONLY PROGRAM DATA
#define SPIFLASH_DATA           0xFB0000                                        // FOR STORING ALL HEX 

#define FOTA_PKT                8192                                            // FOTA Packet Download size


typedef  void (*pFunction)(void);

extern pFunction Jump_To_Application;

extern FlagStatus
Flag_FRTM,
Flag_SerialFlash,
Flag_START,
Flag_FileSize,                                                                  // INDICATE FILE SIZE IS ABOUT TO RECEIVE                                                   
Flag_LoadProg,                                                                  // LOAD PROGRAM TO FLASH MEMORY OF MICROCONTROLLER 
Flag_FlashProg,                                                                 // DOWNLOAD VTS PROGRAM FROM FTP SERVER 
Flag_SaveFlash,                                                                 // SAVE VTS PROGRAM IN SPI FLASH MEMORY
Flag_Download,                                                                  // DOWNLOAD VTS PROGRAM IN MODEM

Flag_NoResp,                                                                    // MODEM NO RESPONSE FLAG
Flag_ModemRdy,                                                                  // INIT MODEM BOOT SUCCESS
Flag_SPIFail,                                                                   // SPI FAIL FLAG
Flag_mSec,                                                                      // Flag Set Every Milli Second
Flag_Second;                                                                    // FLAG SET EVERY SECOND

extern char
Ser_Buffer[50],
Curr_SWVer[10],
SW_VER[10];                                                                      // SOFTWARE VERSION INPUT 

extern unsigned char
Get_CSQ,
Save_Record_Try,
Program_Byte[17],                                                               // PROGRAM BYTE TO SAVE A 2 BYTES INTO FLASH
VTS_State;                                                                      // VTS OPERATING STATE

extern unsigned short
ErrorCode,
Check_Systick,
Write_Loc,                                                                      // WRITE LOCATION OF FLASH DATA FROM RECEIVING BUFFER 
Rev_Count,                                                                      // COUNTER OF RECEIVING BYTES @ GPRS USART INTERRUPT
Loc,                                                                            // SAVE LAST LOCATION OF RECEIVE COUNT AT THE END OF RECEIVING FTP FILE 
Sec_Tick,                                                                       // TICK @ 1 MILI SECOND 
GP_Counter;                                                                     // GENERAL PURPOSE COUNTER 

extern long
FileLength,
StartAddress,                                                                   // START ADDRESS FLASH DATA
SPIFlashAddress,                                                                // SPI FLASH ADDRESS
FlashAddress;                                                                   // FLASH PAGE ADDRESS

extern uint32_t JumpAddress;

extern uint8_t TxData[2];                                                                      // TxData packet

#endif
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/