/******************** (C) COPYRIGHT 2018 EDS INDIA ****************************
* File Name     : SPI_Flash.h					       	                  *
* Author        : DILEEP SINGH                                                *
* Date          : 21/02/2018                                                  *
* Description   : This File Defines functions of SPI Flash                    *
* Revision		: Rev0						      							  *
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

/* Exported macro ------------------------------------------------------------*/

#define PORT_FLASH_CS           GPIOA
#define PIN_FLASH_CS            GPIO_Pin_15                                     // SPI1 NSS CHIP SELECT PIN


#define Flash_ON                PORT_FLASH_CS->BRR = PIN_FLASH_CS
#define Flash_OFF               PORT_FLASH_CS->BSRR  = PIN_FLASH_CS

// Just for Verification
#define  FLASH_WriteAddress     0x700000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  FLASH_ID             


/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(char* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(char* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(char* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32 SPI_FLASH_ReadID(void);
void SPI_FLASH_PD(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);

/*----- Low layer function -----*/
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */
/******************* (C) COPYRIGHT 2018 EDS INDIA *********END OF FILE*********/
