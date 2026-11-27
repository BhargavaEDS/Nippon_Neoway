/******************** (C) COPYRIGHT 2018 EDS INDIA *****************************
* File Name     : HW_Config.h					       	       *
* Author        : DILEEP SINGH                                                 *
* Date          : 21/02/2018                                                   *
* Description   : This file provides all Application hardware Details          *
*                 This file needs to be modified as per hardware changes       *
* Revision		: Rev0							       *
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Configure_HW_H
#define __Configure_HW_H



/******************************************************************************/
/******************************  ON/OFF CONTROL  ******************************/
/*
#define PORT_PPS                GPIOB
#define PIN_PPS                 GPIO_Pin_6                                      // Ignition LED

#define PORT_NL                 GPIOC
#define PIN_NL                  GPIO_Pin_6                                      // Ignition LED
*/
#define PORT_STATUS_LED         GPIOD
#define PIN_STATUS_LED          GPIO_Pin_2                                     // TO INDICATE VEHICLE STATUS

#define PORT_DEVICE_ON          GPIOB
#define PIN_DEVICE_ON           GPIO_Pin_8                                      // TO ENABLE/DISABLE VEHICLE POWER SUPPLY

#define PORT_EB                 GPIOA
#define PIN_EB                  GPIO_Pin_1


/**************************  USART PIN Definations ****************************/
  // USART 3 used for RS-232
#define COM_USART               USART3
#define COM_PORT                GPIOC
#define COM_Tx                  GPIO_Pin_10                                     // USART3(RS-232) TRANSMITTER PIN
#define COM_Rx                  GPIO_Pin_11                                     // USART3(RS-232) RECEIVER PIN

/*************************************************************************/

#endif
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/