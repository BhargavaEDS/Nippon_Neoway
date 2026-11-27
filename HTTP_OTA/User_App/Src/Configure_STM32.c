/******************** (C) COPYRIGHT 2018 EDS INDIA *****************************
* File Name     : Configure_STM32.c					       *
* Author        : DILEEP SINGH                                                 *
* Date          : 21/02/2018                                                   *
* Description   : This file Configures the STM32 Microcontroller & Periferals  *
* Revision	: Rev0			                                       *
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "App_Include.h"

/*******************************************************************************
* Function Name  : INIT_RCC
* Description    : Initialize the System Clock Configure PLL 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void INIT_BKP(void)
{
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);                           // BACKUP DOMAIN POWER ENABLE  
   PWR_BackupAccessCmd(ENABLE);                                                  // ALLOW TO ACCESS BACKUP DOMAIN RTC REGISTER
}
/*******************************************************************************
* Function Name  : INIT_GPIO
* Description    : Initialize the GPIO Registers
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void INIT_GPIO(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  
  /*----- GPIO PERIPHERAL CLOCK ENABLE ---------------------------------------*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD, ENABLE);
  
  /*----- DISABLE PERIFERAL RESET FOR GPIO -----------------------------------*/ 
  RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD,DISABLE);

  /*----- DEFAULT STATE INITIALIZE -------------------------------------------*/
  GPIO_DeInit(GPIOA);                                                           // RESET GPIOA TO DEFAULT STATE  
  GPIO_DeInit(GPIOB);                                                           // RESET GPIOB TO DEFAULT STATE  
  GPIO_DeInit(GPIOC);                                                           // RESET GPIOC TO DEFAULT STATE  
  GPIO_DeInit(GPIOD);
 /*----- CONFIGURE GPIO PIN SPEED -------------------------------------------*/ 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   
 /*----- CONFIGURE GPIO PIN MODE --------------------------------------------*/  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = PIN_DEVICE_ON; GPIO_Init(PORT_DEVICE_ON, &GPIO_InitStructure);
  POWER_ON; 
  GPIO_InitStructure.GPIO_Pin = PIN_PWRKEY;
  GPIO_Init(PORT_PWRKEY, &GPIO_InitStructure);
 //  GPIO_InitStructure.GPIO_Pin =  PIN_CHRG; GPIO_Init(PORT_CHRG, &GPIO_InitStructure);
  /*----- ON/OFF CONTROL GPS, GPRS, DEVICE -----------------------------------*/
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
                                                  // GNSS OFF
  GPIO_InitStructure.GPIO_Pin = PIN_GPRS;GPIO_Init(PORT_GPRS, &GPIO_InitStructure);
  GPRS_OFF;                                                                     // GPRS OFF
  
  /*----- CONFIGURE STATUS LED & DEVICE ON/OFF -------------------------------*/ 
  GPIO_InitStructure.GPIO_Pin = PIN_STATUS_LED;GPIO_Init(PORT_STATUS_LED, &GPIO_InitStructure);
  STATUS_LED_ON;                                                                // STATUS LED SWITCH ON 
  
  /*----- CONFIGURE EXTERNAL BATTERY PRESENCE SENSE --------------------------*/ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin  = PIN_EB; 
  GPIO_Init(PORT_EB, &GPIO_InitStructure);
  
  
}
/*******************************************************************************
* Function Name  : INIT_SysTick
* Description    : Initialize the 1ms Timer
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void INIT_SysTick(void)
{
 SysTick_Config(SystemCoreClock / 1000);                                       // 1 MILI SECOND TICK 
}
/*******************************************************************************
* Function Name  : INIT_USART for RS-232
* Description    : Initialize the USART 3
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void INIT_USART(void)
{
GPIO_InitTypeDef              GPIO_InitStructure;
  USART_InitTypeDef             USART_InitStructure;
  DMA_InitTypeDef               DMA_InitStructure;
  
  
    /*---------- GPIO CLOCK ENABLE ---------------------------------------------*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);     // ENABLE GPIO CLOCK 
  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_1);                           // PIN CONFIGURE TO ALTERNATE FUNCTION 
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_1);                           // PIN CONFIGURE TO ALTERNATE FUNCTION 
 
  /* CONFIGURE USART3_Tx AS ALTERNATE FUNCTION -------------------------------*/
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                 // PIN IS DEFINED AS ALTERNATE FUNCTION 
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                             // NO INTERNALLY PULL UP/PULL DOWN  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                             // PIN SPEED MAXIMUM = 50MHz                             
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                // OUTPUT TYPE PUSH PULL
  GPIO_InitStructure.GPIO_Pin   = COM_Tx;                                       // TX PIN OF MICRO
  GPIO_Init(COM_PORT, &GPIO_InitStructure);                                     // INITIALIZE COM PORT FOR RS-232 COMMUNICATION
  
   /*----- CONFIGURE USART3 Rx AS ALTERNATE FUNCTION  -------------------------*/
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                 // INTERNALLY PULL UP 
  GPIO_InitStructure.GPIO_Pin   = COM_Rx ;                                      // RX PIN OF MICRO
  GPIO_Init(COM_PORT, &GPIO_InitStructure);                                     // INITIALIZE COM PORT FOR RS-232 COMMUNICATION
 
  /* USART3 CONFIGURATION - FOR RS-232----------------------------------------*/
  /* USART3 CONFIGURED AS FOLLOW:
        - BAUDRATE = DEFINE IN BOARD DEFINATION
        - WORD LENGTH = 8 BITS 
        - ONE STOP BIT 
        - NO PARITY 
        - HARDWARE FLOW CONTROL DISABLED (RTS & CTS SIGNALS)
        - RECIEVE AND TRANSMIT ENABLED 
  ----------------------------------------------------------------------------*/

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);                        // ENABLE USART CLOCK
  USART_DeInit(COM_USART);                                                      // RESET TO DEFAULT STATE 

  USART_InitStructure.USART_BaudRate   = BAUD_RATE_COM;                         // SET RS-232 BAUDRATE DEFINED IN HEADER FILE 
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                   // 8 BIT
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                        // 1 STOP BIT
  USART_InitStructure.USART_Parity   = USART_Parity_No;                         // PARITY NONE
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// HARDWARE FLOW CONTROL NONE
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                // Tx & Rx ENABLED 

  /*----- INITIALIZE USART 3 -------------------------------------------------*/
  USART_Init(COM_USART, &USART_InitStructure);                                  // INITIALIZE USART 3

  /*----- ENABLE USART3 RECIEVE INTERRUPT : THIS INTERRUPT IS GENERATED WHEN 
  USART RECIEVE DATA REGISTER IS NOT EMPTY -----------------------------------*/
  USART_ITConfig(COM_USART, USART_IT_RXNE, ENABLE);                            // ENABLE RECEIVE INTERRUPT

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2 , ENABLE);                           // ENABLE DMA FOR ADC 


   /* Common DMA configuration */
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  /* DMA2 Channel1 configuration */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxData;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004828;                        // USART 3 TDR 
  DMA_Init(DMA2_Channel1, &DMA_InitStructure);
  
  /* DMA remap Channel1 to USART1 Tx */
  DMA_RemapConfig(DMA2, DMA2_CH1_USART3_TX);
  
     
  /* Enable the USART2 Rx and Tx DMA2 requests */
  USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
  
  /* Enable Usart */
  USART_Cmd(USART3, ENABLE);
  
  
  DMA_ITConfig(DMA2_Channel1, DMA_IT_TC, ENABLE);
  
}
/*******************************************************************************
* Function Name  : INIT_NVIC
* Description    : Initialize the Interrupt section
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void INIT_NVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /*----- ENABLE USART1 INTERRUPT - GPRS/GSM ---------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			        // USART 1 INTERRUPT 			
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;				// SUB GROUP PRIORITY 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// ENABLE INTERRUPT
  NVIC_Init(&NVIC_InitStructure);						// WRITE CONFIGURATION

   /*----- ENABLE USART3 INTERRUPT RS-232 -------------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel =  USART3_8_IRQn;                          // USART 3 INTERRUPT     
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;			 	// SUB PRIORITY   
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			        // ENABLE INTERRUPT 
  NVIC_Init(&NVIC_InitStructure);
  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch2_3_DMA2_Ch1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* Function Name  : INIT_IWGD
* Description    : Initialize the Watchdog Timer for 25 Sec
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void INIT_IWDG(void)
{
  /*---------- ENABLE WRITE ACCESS TO IWDG_PR & IWDG_RLR REGISTER ------------*/
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);					

  /*---------- STOP THE WATCH DOG TIMER WHEN MCU ENTER INTO DEBUD MODE -------*/
  DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP,ENABLE);
  
  /*---------- IWDG COUNTER CLOCK : 40KHz(LSI) / 256 = 156 Hz i.e 6.4 ms------*/
  IWDG_SetPrescaler(IWDG_Prescaler_256);					// SET PRESCALER

  /*---------- Set counter reload value to 1563 i.e 6.4 ms * 1563 ~ 10 Sec----*/
  IWDG_SetReload(3906);		        //25 sec						// SET RELOAD VALUE 

  /*---------- RELOAD IWDG COUNTER -------------------------------------------*/
  IWDG_ReloadCounter();								// Reload Counter

  /*---------- ENABLE IWDG (LSI OSCILLATOR WILL BE ENABLED BY HW )------------*/
  IWDG_Enable();								// ENABLE IWDT
}
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/