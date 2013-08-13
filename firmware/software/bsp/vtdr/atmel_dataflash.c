/******************** (C) COPYRIGHT 2012 M ********************
 * File Name          : atmel_dataflash.c
 * Author             : MXX
 * Description        : atmel data flash driver source file.
 *                      Pin assignment:
 *             ----------------------------------------------
 *             |  STM32F10x    |     DATAFLASH    Pin        |
 *             ----------------------------------------------
 *             | PA.4          |   ChipSelect      1         |
 *             | PA.6 / MISO   |   DataOut         2         |
 *             |               |   WP              3 (3.3 V) |
 *             |               |   GND             4 (0 V)   |
 *             | PA.7 / MOSI   |   DataIn          5         |
 *             | PA.5 / SCLK   |   Clock           6         |
 *             |               |   Hold            7 (3.3V)  |
 *             |               |   VDD             8         |
 *             -----------------------------------------------
 ********************************************************************************
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
 *******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "atmel_dataflash.h"
#include <stm32f10x_spi.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize 256

#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE4K       0x20  /*Sector 4k Erase instruction */
#define SE         0xD8  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0x00
#define SPI_FLAG_TXE SPI_I2S_FLAG_TXE
#define SPI_FLAG_RXNE SPI_I2S_FLAG_RXNE

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Select DATAFLASH Card: ChipSelect pin low  */
#define DATAFLASH_CS_LOW()    GPIO_ResetBits(GPIOA, GPIO_Pin_3)
/* Deselect DATAFLASH Card: ChipSelect pin high */
#define DATAFLASH_CS_HIGH()   GPIO_SetBits(GPIOA, GPIO_Pin_3)
/* Select SPI FLASH: ChipSelect pin low  */
#define SPI_FLASH_CS_LOW()     GPIO_ResetBits(GPIOA, GPIO_Pin_3)
/* Deselect SPI FLASH: ChipSelect pin high */
#define SPI_FLASH_CS_HIGH()    GPIO_SetBits(GPIOA, GPIO_Pin_3)
/* Deselect ADATAFLASH : ChipSelect pin high */
#define ADATAFLASH_CS_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_12)
/* Deselect ADATAFLASH : ChipSelect pin Low */
#define ADATAFLASH_CS_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_12)


#define DATAFLASH_SPI         SPI1
#define DATAFLASH_RCC_SPI     RCC_APB2Periph_SPI1
#define DATAFLASH_Manufacturer_ATMEL 0x1F
#define DATAFLASH_READSUCCESS(x)  (((x>>8) & 0x00FF)!=0)
#define DATAFLASH_READ_BYTE(x)    (u8)(x&0x00FF)
/* Private function prototypes -----------------------------------------------*/
static void SPI_Config(SPI_TypeDef* SPIx);
/* Private functions ---------------------------------------------------------*/

void dataflash_delay(u16 n)
{
	u16 i;
	for(i=0;i<n;i++);
}
/*******************************************************************************
 * Function Name  : Flah_Chip_Select
 * Description    : change and select the spi chip bus
 * Input          : SPIx:it can be SPI1,SPI2;
 *                  status: 1,set high ;0,set low
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Flah_Chip_Select( SPI_TypeDef* SPIx ,u8 status)
{
	if (SPIx == SPI1)
	{
		if (status)
		{
			DATAFLASH_CS_HIGH();
		}
		else
		{

			DATAFLASH_CS_LOW();
		}

	}
	else if (SPIx == SPI2)
	{
		if(status)
		{
			ADATAFLASH_CS_HIGH();
		}
		else
		{
			ADATAFLASH_CS_LOW();
		}
	}


}
/*******************************************************************************
 * Function Name  : DATAFLASH_Init
 * Description    : Initializes the DATAFLASH/SD communication.
 * Input          : None
 * Output         : None
 * Return         : The DATAFLASH Response: - DATAFLASH_RESPONSE_FAILURE: Sequence failed
 *                                    - DATAFLASH_RESPONSE_NO_ERROR: Sequence succeed
 *******************************************************************************/
u8 DATAFLASH_Init(SPI_TypeDef* SPIx)
{
        /* Initialize SPI */
	SPI_Config(SPIx);
	/* DATAFLASH chip select high */
	Flah_Chip_Select(SPIx,1);
	/* Send dummy byte 0xFF, 10 times with CS high*/
	/* rise CS and MOSI for 80 clocks cycles */

	/*------------Put DATAFLASH in SPI mode--------------*/
	/* DATAFLASH initialized and set to SPI mode properly */
	sDATAFLASH_CID cid;
	DATAFLASH_ChangProtectStatus(SPIx,0x01);
	DATAFLASH_GetStatusREG(SPIx);
	return (DATAFLASH_GetDeviceID(SPIx,&cid));



}

/*******************************************************************************
* Function Name  : DATAFLASH_ChangProtectStatus
* Description    : change the global protect status.
* Input          : - pBuffer : the status you want to change.
*
* Output         : None
* Return         : None
*******************************************************************************/
void DATAFLASH_ChangProtectStatus(SPI_TypeDef* SPIx, u8 pBuffer)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable(SPIx);
  dataflash_delay(2000);
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx,0);
  dataflash_delay(1000);
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(SPIx,WRSR);
    SPI_FLASH_SendByte(SPIx,pBuffer);
  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx,1);
  dataflash_delay(2000);
  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(SPIx);
}

u8 DATAFLASH_GetStatusREG( SPI_TypeDef* SPIx )
{
	u8 rvalue = DATAFLASH_RESPONSE_FAILURE;
	u8 CID_Tab[2];
	Flah_Chip_Select(SPIx,1);
    dataflash_delay(2000);

	  /* Sector Erase */
	  /* Select the FLASH: Chip Select low */
    Flah_Chip_Select(SPIx,0);

	  dataflash_delay(1000);
	  /* Send Sector Erase instruction */
	  SPI_FLASH_SendByte( SPIx, RDSR );

		u16 i;
		for (i = 0; i < 2; i++)
		{

			DATAFLASH_WriteByte(SPIx,Dummy_Byte);
			u16 data = DATAFLASH_ReadByte(SPIx);
			if (DATAFLASH_READSUCCESS(data))
			{
				CID_Tab[i] = DATAFLASH_READ_BYTE(data);
			}
			else
			{
				rvalue = DATAFLASH_RESPONSE_FAILURE;
				break;
			}
			rvalue = DATAFLASH_RESPONSE_NO_ERROR;

		}

		Flah_Chip_Select(SPIx ,1);
		return rvalue;


}


/*******************************************************************************
 * Function Name  : DATAFLASH_GetCIDRegister
 * Description    : Read the CID card register.
 *                  Reading the contents of the CID register in SPI mode
 *                  is a simple read-block transaction.
 * Input          : - DATAFLASH_cid: pointer on an CID register structure
 * Output         : None
 * Return         : The DATAFLASH Response: - DATAFLASH_RESPONSE_FAILURE: Sequence failed
 *                                    - DATAFLASH_RESPONSE_NO_ERROR: Sequence succeed
 *******************************************************************************/
u8 DATAFLASH_GetDeviceID(SPI_TypeDef* SPIx ,sDATAFLASH_CID* DATAFLASH_cid)
{
	u8 rvalue = DATAFLASH_RESPONSE_FAILURE;
	u8 CID_Tab[4];

	Flah_Chip_Select(SPIx ,1);
	dataflash_delay(2000);
	/* DATAFLASH chip select low */
	Flah_Chip_Select(SPIx ,0);
	/* Send CMD10 (CID register) */
	dataflash_delay(1000);
	DATAFLASH_WriteByte(SPIx,DATAFLASH_READ_CID);
	DATAFLASH_ReadByte(SPIx);
	/* Store CID register value on CID_Tab */
	/* Wait until a data is received */
	u16 i;
	for (i = 0; i < 4; i++)
	{

		DATAFLASH_WriteByte(SPIx,Dummy_Byte);
		u16 data = DATAFLASH_ReadByte(SPIx);
		if (DATAFLASH_READSUCCESS(data))
		{
			CID_Tab[i] = DATAFLASH_READ_BYTE(data);
		}
		else
		{
			rvalue = DATAFLASH_RESPONSE_FAILURE;
			break;
		}
		rvalue = DATAFLASH_RESPONSE_NO_ERROR;

	}

	/* DATAFLASH chip select high */
	Flah_Chip_Select(SPIx,1);
	/* Byte 0 */
	DATAFLASH_cid->ManufacturerID = CID_Tab[0];
	/* Byte 1 */
	u8* p = (u8*) &(DATAFLASH_cid->ManufacturerID);
	p[1] = CID_Tab[1];
	/* Byte 2 */
	p[2] = CID_Tab[2];

	/* Return the reponse */
	return rvalue;
}

/*******************************************************************************
 * Function Name  : DATAFLASH_WriteByte
 * Description    : Write a byte on the DATAFLASH.
 * Input          : Data: byte to send.
 * Output         : None
 * Return         : None.
 *******************************************************************************/
void DATAFLASH_WriteByte(SPI_TypeDef* SPIx ,u8 Data)
{
        /* Wait until the transmit buffer is empty */
        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
        /* Send the byte */
        SPI_I2S_SendData(SPIx, Data);
}


/*******************************************************************************
 * Function Name  : DATAFLASH_ReadByte
 * Description    : Read a byte from the DATAFLASH.
 * Input          : None.
 * Output         : None
 * Return         : The received byte.
 *******************************************************************************/
u16 DATAFLASH_ReadByte( SPI_TypeDef* SPIx )
{
	u8 Data = 0;
	u8 count=0xFF;
	/* Wait until a data is received */
	while ((--count) && SPI_I2S_GetFlagStatus( SPIx, SPI_I2S_FLAG_RXNE) == RESET)
		;
	/* Get the received data */
	Data = SPI_I2S_ReceiveData(SPIx);

	/* Return the shifted data */
	return ((count << 8)&0xFF00) | (0x00FF & Data);
}


/*******************************************************************************
 * Function Name  : SPI_Config
 * Description    : Initializes the SPI and CS pins.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SPI_Config(SPI_TypeDef* SPIx)
{
	uint32_t delay;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	if (SPIx==SPI1 )
	{
		/* GPIOA and GPIOC Periph clock enable */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		/* SPI Periph clock enable */
		RCC_APB2PeriphClockCmd(DATAFLASH_RCC_SPI, ENABLE);

		/* Configure SPI pins: SCK, MISO and MOSI */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if(SPIx == SPI2)
	{
		/* GPIOA and GPIOC Periph clock enable */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		/* SPI Periph clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

		/* Configure SPI pins: SCK, MISO and MOSI */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}

	/* SPI Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	//SPI_Init(DATAFLASH_SPI, &SPI_InitStructure);
	SPI_Init(SPIx, &SPI_InitStructure);
	/* SPI enable */
	//SPI_Cmd(DATAFLASH_SPI, ENABLE);
	SPI_Cmd(SPIx, ENABLE);

	for (delay = 0; delay < 0xfffff; delay++);

}

/*******************************************************************************
* Function Name  : SPI_FLASH_Sector4kErase
* Description    : Erases the specified 4k FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_Sector4kErase(SPI_TypeDef* SPIx ,u32 SectorAddr)
{
 if (SPIx==SPI2 )
  {
	  rt_public_pin_init(0);
  }
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable(SPIx);
  dataflash_delay(2000);
  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);

  dataflash_delay(1000);
  /* Send Sector Erase instruction */
  SPI_FLASH_SendByte(SPIx, SE4K);
  /* Send SectorAddr high nibble address byte */
  SPI_FLASH_SendByte(SPIx,(SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte(SPIx,(SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(SPIx, SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx ,1);

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(SPIx);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(SPI_TypeDef* SPIx ,u32 SectorAddr)
{
  if (SPIx==SPI2 )
  {
	rt_public_pin_init(0);
  }
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable(SPIx);

  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);
  /* Send Sector Erase instruction */
  SPI_FLASH_SendByte(SPIx,SE);
  /* Send SectorAddr high nibble address byte */
  SPI_FLASH_SendByte(SPIx, (SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte(SPIx, (SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(SPIx, SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx ,1);

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(SPIx);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase( SPI_TypeDef* SPIx )
{
	if (SPIx==SPI2 )
	{
		rt_public_pin_init(0);
	}
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable(SPIx);
  dataflash_delay(2000);
  /* Bulk Erase */
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);
  dataflash_delay(1000);
  /* Send Bulk Erase instruction  */
  SPI_FLASH_SendByte(SPIx,BE);
  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx ,1);

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(SPIx);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(SPI_TypeDef* SPIx ,u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable(SPIx);
  dataflash_delay(2000);
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);
  dataflash_delay(1000);
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(SPIx,WRITE);
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte(SPIx, (WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte(SPIx, (WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(SPIx, WriteAddr & 0xFF);

  /* while there is data to be written on the FLASH */
  while(NumByteToWrite--)
  {
    /* Send the current byte */
    SPI_FLASH_SendByte(SPIx, *pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx ,1);
  dataflash_delay(2000);
  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(SPIx);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferWrite(SPI_TypeDef* SPIx ,u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
  if (SPIx==SPI2 )
  {
	  rt_public_pin_init(0);
  }

  if(Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if(NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(SPIx,pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, NumOfSingle);
   }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if(NumOfPage== 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if(NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(SPIx, pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferRead(SPI_TypeDef* SPIx ,u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
 if (SPIx==SPI2 )
  {
	  rt_public_pin_init(0);
  }
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);
  dataflash_delay(1000);
  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(SPIx,READ);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte(SPIx,(ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte(SPIx,(ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(SPIx, ReadAddr & 0xFF);

  while(NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(SPIx,Dummy_Byte);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx ,1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(SPI_TypeDef* SPIx, u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(SPIx,READ);

/* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte(SPIx,(ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte(SPIx,(ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(SPIx, ReadAddr & 0xFF);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_FLASH_ReadByte(SPI_TypeDef* SPIx)
{
  return (SPI_FLASH_SendByte(SPIx, Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_FLASH_SendByte(SPI_TypeDef* SPIx, u8 byte)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPIx, byte);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPIx);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
u16 SPI_FLASH_SendHalfWord(SPI_TypeDef* SPIx, u16 HalfWord)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_FLAG_TXE) == RESET);

  /* Send Half Word through the SPI1 peripheral */
  SPI_SendData(SPIx, HalfWord);

  /* Wait to receive a Half Word */
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_FLAG_RXNE) == RESET);

  /* Return the Half Word read from the SPI bus */
  return SPI_ReceiveData(SPIx);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(SPI_TypeDef* SPIx )
{
  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);
  dataflash_delay(1000);
  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(SPIx,WREN);

  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx ,1);

}

/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd( SPI_TypeDef* SPIx )
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  Flah_Chip_Select(SPIx ,0);

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(SPIx, RDSR);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(SPIx, Dummy_Byte);

  } while((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  Flah_Chip_Select(SPIx ,1);
}


/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

/*
 * RT-Thread SD Card Driver
 * 20090417 Bernard
 */
#include <rtthread.h>
#include <dfs_fs.h>

static struct rt_device dataflash_device;
static struct dfs_partition part;
#define SECTOR_SIZE 512

/* RT-Thread Device Driver Interface */
static rt_err_t rt_dataflash_init(rt_device_t dev)
{

	return RT_EOK;
}

static rt_err_t rt_dataflash_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_dataflash_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t rt_dataflash_read(rt_device_t dev, rt_off_t pos, void* buffer,
		rt_size_t size)
{
	rt_uint8_t status;
	rt_uint32_t i;

	status = DATAFLASH_RESPONSE_NO_ERROR;
	// rt_kprintf("read: 0x%x, size %d\n", pos, size);

	/* read all sectors */
	for (i = 0; i < size / SECTOR_SIZE; i++)
	{
//		status = DATAFLASH_ReadBlock((rt_uint8_t*) ((rt_uint8_t*) buffer + i
//				* SECTOR_SIZE), (part.offset + i) * SECTOR_SIZE + pos,
//				SECTOR_SIZE);
//		if (status != DATAFLASH_RESPONSE_NO_ERROR)
//		{
//			rt_kprintf("dataflash read failed\n");
//			return 0;
//		}
	}

	if (status == DATAFLASH_RESPONSE_NO_ERROR)
		return size;

	rt_kprintf("read failed: %d\n", status);
	return 0;
}

static rt_size_t rt_dataflash_write(rt_device_t dev, rt_off_t pos,
		const void* buffer, rt_size_t size)
{
	rt_uint8_t status;
	rt_uint32_t i;

	status = DATAFLASH_RESPONSE_NO_ERROR;
	// rt_kprintf("write: 0x%x, size %d\n", pos, size);

	/* read all sectors */
	for (i = 0; i < size / SECTOR_SIZE; i++)
	{
//		status = DATAFLASH_WriteBuffer((rt_uint8_t*) ((rt_uint8_t*) buffer + i
//				* SECTOR_SIZE), (part.offset + i) * SECTOR_SIZE + pos,
//				SECTOR_SIZE);
//		if (status != DATAFLASH_RESPONSE_NO_ERROR)
//		{
//			rt_kprintf("dataflash write failed\n");
//			return 0;
//		}
	}

	if (status == DATAFLASH_RESPONSE_NO_ERROR)
		return size;

	rt_kprintf("write failed: %d\n", status);
	return 0;
}

static rt_err_t rt_dataflash_control(rt_device_t dev, rt_uint8_t cmd,
		void *args)
{
	return RT_EOK;
}

void rt_hw_dataflash_init()
{
	if (DATAFLASH_Init(SPI1) == DATAFLASH_RESPONSE_NO_ERROR)
	{
		rt_uint8_t status;
		rt_uint8_t *sector;

		/* register dataflash device */
		dataflash_device.init = rt_dataflash_init;
		dataflash_device.open = rt_dataflash_open;
		dataflash_device.close = rt_dataflash_close;
		dataflash_device.read = rt_dataflash_read;
		dataflash_device.write = rt_dataflash_write;
		dataflash_device.control = rt_dataflash_control;

		/* no private */
		//dataflash_device.private = RT_NULL;
		/* get the first sector to read partition table */
		sector = (rt_uint8_t*) rt_malloc(512);
		if (sector == RT_NULL)
		{
			rt_kprintf("allocate partition sector buffer failed\n");
			return;
		}

		/* release sector buffer */
		rt_free(sector);

		rt_device_register(&dataflash_device, "sd0", RT_DEVICE_FLAG_RDWR
				| RT_DEVICE_FLAG_REMOVABLE | RT_DEVICE_FLAG_STANDALONE);
	}
	else
	{
		rt_kprintf("dataflash init failed\n");
	}
	DATAFLASH_Init(SPI2);

}
