/******************** (C) COPYRIGHT 2018 EDS INDIA *****************************
* File Name     : App_RAM.c					               *
* Author        : DILEEP SINGH                                                 *
* Date          : 21/02/2018                                                   *
* Description   : This file defines variables & data types                     *
* Revision	: Rev0			                                       *
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "App_Include.h"

pFunction Jump_To_Application;                                                  // JUMP TO MAIN VTS APPLICATION PROGRAM

/*----- FLAGS ----------------------------------------------------------------*/
FlagStatus
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

char
Ser_Buffer[50],
Curr_SWVer[10],
SW_VER[10];                                                                      // SOFTWARE VERSION INPUT 

uint8_t TxData[2];                                                                      // TxData packet

unsigned char
Get_CSQ,
Save_Record_Try,
Program_Byte[17],                                                               // PROGRAM BYTE TO SAVE A 2 BYTES INTO FLASH
VTS_State;                                                                      // VTS OPERATING STATE

unsigned short
ErrorCode,
Write_Loc,                                                                      // WRITE LOCATION OF FLASH DATA FROM RECEIVING BUFFER 
Rev_Count,                                                                      // COUNTER OF RECEIVING BYTES @ GPRS USART INTERRUPT
Loc,                                                                            // SAVE LAST LOCATION OF RECEIVE COUNT AT THE END OF RECEIVING FTP FILE 
Sec_Tick = 999,                                                                       // TICK @ 1 MILI SECOND 
GP_Counter;                                                                     // GENERAL PURPOSE COUNTER 

long
StartAddress,                                                                   // START ADDRESS FLASH DATA
SPIFlashAddress,                                                                // SPI FLASH ADDRESS
FlashAddress;                                                                   // FLASH PAGE ADDRESS

uint32_t JumpAddress;                                                           // JUMP TO VTS PROGRAM
uint8_t TxData[2];  
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/