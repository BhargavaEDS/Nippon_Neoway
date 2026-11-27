/******************** (C) COPYRIGHT 2018 EDS INDIA ****************************
* File Name     : Config_STM32.h					       	                  *
* Author        : DILEEP SINGH                                                *
* Date          : 21/02/2018                                                  *
* Description   : This File Defines the configuration of micro-controller     *
* Revision		: Rev0						      							  *
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Configure_CPU_H
#define __Configure_CPU_H

void INIT_BKP(void);								// CONFIGURE CLOCKS
void INIT_GPIO(void);								// CONFIGURE GPIO PINS
void INIT_SysTick(void);							// CONFIGURE SYSTICK CLOCK 
void INIT_USART(void);
void INIT_NVIC(void);								// CONFIGURE INTERRUPT CONTROLLER
void INIT_IWDG(void);								// CONFIGURE IWDG TIMER

#endif
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/
