/******************** (C) COPYRIGHT 2018 EDS INDIA ****************************
* File Name     : GPRS.h					       	      *
* Author        : DILEEP SINGH                                                *
* Date          : 21/02/2018                                                  *
* Description   : This File Defines variables & function used in GPRS         *
* Revision		: Rev0						      	      *
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPRS_H
#define __GPRS_H

/* FUNCTIONS -----------------------------------------------------------------*/
void INIT_USART_GPRS(void);
void Serial_SendData_GPRS(char);
void Serial_PutData_GPRS(char *, char);
void Serial_PutString_GPRS(char *);
void Modem_PutString(char *);
void INIT_GPRS_Modem(void);
void Set_APN(void);
void Check_Signal_Nw_GPRS(void);
void WAIT_MODEM_RESP(unsigned char);
void WAIT_MODEM_CONNECT(void);
void Send_SMS(unsigned char Subject);

/* DEFINATIONS----------------------------------------------------------------*/
// GPRS BAUD RATE
#define BAUD_RATE_GPRS          115200                                          // GPRS BAUD RATE
#define BAUD_RATE_COM           115200

// USART 1 used for GPRS/GSM - GPRS Modem Auto Buad Rate
#define GPRS_USART              USART1                   
#define GPRS_PORT               GPIOA
#define GPRS_Tx                 GPIO_Pin_9                                      // USART1(GPRS) TRANSMITTER  PIN
#define GPRS_Rx                 GPIO_Pin_10                                     // USART1(GPRS) RECEIVER PIN



// GPRS PIN & PORT 
#define PORT_GPRS               GPIOC
#define PIN_GPRS                GPIO_Pin_12                                     // TO ENABLE/DISABLE GPRS POWER SUPPLY                             // TO ENABLE/DISABLE GPRS POWER SUPPLY


// GPRS ON/OFF
#define GPRS_ON                PORT_GPRS->BRR   = PIN_GPRS                      // INVERSE LOGIC TRANSISTOR
#define GPRS_OFF               PORT_GPRS->BSRR  = PIN_GPRS                      // INVERSE LOGIC TRANSISTOR

#define PORT_PWRKEY                GPIOB
#define PIN_PWRKEY                 GPIO_Pin_10                                    // TO ENABLE/DISABLE GPS POWER SUPPLY

#define PWRKEY_ENB                 PORT_PWRKEY->BSRR    = PIN_PWRKEY                   // INVERSE LOGIC TRANSISTOR
#define PWRKEY_DSB                 PORT_PWRKEY->BRR   = PIN_PWRKEY                   // INVERSE LOGIC TRANSISTOR


#define GPRSBuffer_Size     500
#define VALID_SIGNAL        10                                                  // MINIMUM CSQ LEVEL

#define FAIL                0
#define UNABLE_TO_CONNECT   1
#define UNABLE_TO_DWLOAD    2
#define WRONG_FILE          3
#define SPI_FAIL            4
#define ERROR               5
#define RECORD_FAIL         6
#define ERRORCODE           7 // ftp error code
/* VARIABLES -----------------------------------------------------------------*/

typedef struct									// DATA STRUCTURE FOR GPRS
  {
    char IMEI_No[17];                                                           // IMEI NUMBER 
    char Operator[20];								// OPERATOR
    char APN_Name[33];                                                          // ACCESS POINT NAME 
    char SET_APN[33];                                                           // SET APN BY USER MANUALLY
    char Signal_Strength;							// SIGNAL STRENGTH 
   }GPRS_InitTypeDef;
extern GPRS_InitTypeDef    GPRSData;

typedef struct                                                                  // DATA STRUCTURE FOR DFU
 {
  char IP[4];                                                                   // DFU FTP SERVER IP ADDRESS
  char PATH[50];                                                            // DFU FTP SERVER USERNAME 
  char FILENAME[15];                                                            // DFU FTP SERVER PASSWORD
  char URL[200]; 
 }HTTP_InitTypeDef; 
extern HTTP_InitTypeDef DFU_HTTP;

extern FlagStatus
Flag_GetFile,                                                                   // DOWNLOADING STARTING FROM FTP SERVER
Flag_WriteFile,                                                                 // SAVE RECEIVED DATA INTO FLASH
Flag_ModemOK,                                                                   // GPRS DATA READY           
Flag_ModemError,                                                                // ERROR GENERATED 
Flag_ValidNetwork,                                                              // VALID NETWORK
Flag_WrongIMEI,                                                                 // WRONG IMEI NUMBER                  
Flag_WrongFile,                                                                 // WRONG FILE NAME REQUESTED TO DOWNLOAD 
Flag_APNSet;                                                                    // APN NAME SET

extern char 
Rev_Buffer[Reveive_Buffer_Size],                                                // BUFFER TO RECEIVE BYTES FROM FTP SERVER 
IAP_Status,                                                                     // IAP STATUS 
IAP_Counter,                                                                    // IAP COUNTER 
NoData_Count,                                                                   // NO DATA  AFTER CONNECTION COUNTER        
Storage_Data[4096],                                                             // PROGRAMMING DATA
GPRS_SendBuffer[100],                                                           // GENERAL PURPOSE SENDING BUFFER
GPRS_Buffer[GPRSBuffer_Size],                                                   // GPRS DATA BUFFER 
DFU_Mob[15];                                                                    // DFU REQUESTING MOBILE NUMBER 

extern unsigned char
Reg_Error,                                                                      // SIM REGISTRATION ERROR                 
Modem_NoResp,                                                                   // MODEM NO RESPONSE COUNT
Init_Time;                                                                      // MODEM INITIALIZATION TIME

extern unsigned int
GPRS_Count;                                                                     // GPRS DATA COUNTER                   
//UpLoad_Time;                                                                    // GPRS DATA UPLOAD TIME

extern unsigned long 
File_Size;                                                                      // RECEIVED FILE SIZE 
#endif
/******************* (C) COPYRIGHT 2015 EDS INDIA *********END OF FILE*********/