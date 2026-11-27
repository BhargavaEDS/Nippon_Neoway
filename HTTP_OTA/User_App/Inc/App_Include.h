/******************** (C) COPYRIGHT 2018 EDS INDIA ****************************
* File Name     : App_Include.h					       	       				  *
* Author        : DILEEP SINGH                                                *
* Date          : 21/02/2018                                                  *
* Description   : This File Defines the application includes requirements     *
*                 This file needs to be modified as per hardware changes      *
* Revision      : Rev0						       							  *
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Application_H
#define __Application_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>


#include "stm32f0xx.h"								// HARDWARE DEFINITION
#include "stm32f0xx_conf.h"							// CPU CONFIGURATION
#include "HW_Config.h"								// HARDWARE DEFINATION
#include "Config_STM32.h"							// CONFIGURATION FILE OF MCU
#include "App_RAM.h"								// RAM VARIABLE DEFINATION
#include "GPRS.h"                                                               // GPRS FUNCTIONS
#include "VTS_Functions.h"							// MISC FUNCTIONS
#include "App_Init.h"								// INITIALIZE APPLICATION	
#include "SPI_Flash.h"                                                          // SPI FOR DATA WRITE

/****************************** ON/OFF CONTROL ********************************/
#define POWER_ON               PORT_DEVICE_ON->BSRR = PIN_DEVICE_ON             // EXTERNAL SUPPLY ON
#define POWER_OFF              PORT_DEVICE_ON->BRR  = PIN_DEVICE_ON             // EXTERNAL SUPPLY OFF

#define STATUS_LED_ON          PORT_STATUS_LED->BRR  = PIN_STATUS_LED           // STATUS LED ON
#define STATUS_LED_OFF         PORT_STATUS_LED->BSRR = PIN_STATUS_LED           // STATUS LED OFF

#define STATUS_LED_TOGGLE      PORT_STATUS_LED->ODR ^= PIN_STATUS_LED           // STATUS LED TOGGLE



#define CHRG_ON                PORT_CHRG->BSRR  = PIN_CHRG           // STATUS LED ON
#define CHRG_OFF               PORT_CHRG->BRR = PIN_CHRG           // STATUS LED OFF

#define IB_ENB                PORT_BON->BSRR  = PIN_BON           // STATUS LED ON
#define IB_DSB                PORT_BON->BRR = PIN_BON           // STATUS LED OFF

 

/******************************************************************************/

#define SYSTEM_SW_RESET       NVIC_SystemReset()// SCB->AIRCR = 0x05FA0000 | (u32)0x04              // SYSTEM SOFTWARE RESET REQUEST GENERATION


#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#endif
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/