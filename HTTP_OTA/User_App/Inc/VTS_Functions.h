/******************** (C) COPYRIGHT 2018 EDS INDIA ****************************
* File Name     : VTS_Function.h					       	                  *
* Author        : DILEEP SINGH                                                *
* Date          : 21/02/2018                                                  *
* Description   : This File Defines variables & misc functions                *
* Revision		: Rev0						      	                          *
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Functions_H
#define __Functions_H



void mSec_Delay(unsigned int);
void Get_Firmware(void);
void Program_Flash(void);
void StoreSPI_Flash(void);
void Serial_PutString_232(char *s, uint8_t usart_no);
void Get_Firmware_Serial(void);
void GPRS_Module_ON(void);
void GPRS_Module_OFF(void);
#endif
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/