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
#define SE         0xD8  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0x00
#define SPI_FLAG_TXE SPI_I2S_FLAG_TXE
#define SPI_FLAG_RXNE SPI_I2S_FLAG_RXNE

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Select DATAFLASH Card: ChipSelect pin low  */
#define DATAFLASH_CS_LOW()    GPIO_ResetBits(GPIOA, GPIO_Pin_4)
/* Deselect DATAFLASH Card: ChipSelect pin high */
#define DATAFLASH_CS_HIGH()   GPIO_SetBits(GPIOA, GPIO_Pin_4)
/* Select SPI FLASH: ChipSelect pin low  */
#define SPI_FLASH_CS_LOW()     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
/* Deselect SPI FLASH: ChipSelect pin high */
#define SPI_FLASH_CS_HIGH()    GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define DATAFLASH_SPI         SPI1
#define DATAFLASH_RCC_SPI     RCC_APB2Periph_SPI1
#define DATAFLASH_Manufacturer_ATMEL 0x1F
#define DATAFLASH_READSUCCESS(x)  (((x>>8) & 0x00FF)!=0)
#define DATAFLASH_READ_BYTE(x)    (u8)(x&0x00FF)
/* Private function prototypes -----------------------------------------------*/
static void SPI_Config(void);
/* Private functions ---------------------------------------------------------*/

void dataflash_delay(u16 n)
{
	u16 i;
	for(i=0;i<n;i++);
}
/*******************************************************************************
 * Function Name  : DATAFLASH_Init
 * Description    : Initializes the DATAFLASH/SD communication.
 * Input          : None
 * Output         : None
 * Return         : The DATAFLASH Response: - DATAFLASH_RESPONSE_FAILURE: Sequence failed
 *                                    - DATAFLASH_RESPONSE_NO_ERROR: Sequence succeed
 *******************************************************************************/
u8 DATAFLASH_Init(void)
{
        /* Initialize SPI */
	SPI_Config();
	/* DATAFLASH chip select high */
	DATAFLASH_CS_HIGH();
	/* Send dummy byte 0xFF, 10 times with CS high*/
	/* rise CS and MOSI for 80 clocks cycles */

	/*------------Put DATAFLASH in SPI mode--------------*/
	/* DATAFLASH initialized and set to SPI mode properly */
	sDATAFLASH_CID cid;
	return (DATAFLASH_GetDeviceID(&cid));
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
u8 DATAFLASH_GetDeviceID(sDATAFLASH_CID* DATAFLASH_cid)
{
	u8 rvalue = DATAFLASH_RESPONSE_FAILURE;
	u8 CID_Tab[4];

	DATAFLASH_CS_HIGH();
	dataflash_delay(2000);
	/* DATAFLASH chip select low */
	DATAFLASH_CS_LOW();
	/* Send CMD10 (CID register) */
	dataflash_delay(1000);
	DATAFLASH_WriteByte(DATAFLASH_READ_CID);
	DATAFLASH_ReadByte();
	/* Store CID register value on CID_Tab */
	/* Wait until a data is received */
	u16 i;
	for (i = 0; i < 4; i++)
	{

		DATAFLASH_WriteByte(Dummy_Byte);
		u16 data = DATAFLASH_ReadByte();
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
	DATAFLASH_CS_HIGH();
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
void DATAFLASH_WriteByte(u8 Data)
{
        /* Wait until the transmit buffer is empty */
        while (SPI_I2S_GetFlagStatus(DATAFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
                ;
        /* Send the byte */
        SPI_I2S_SendData(DATAFLASH_SPI, Data);
}


/*******************************************************************************
 * Function Name  : DATAFLASH_ReadByte
 * Description    : Read a byte from the DATAFLASH.
 * Input          : None.
 * Output         : None
 * Return         : The received byte.
 *******************************************************************************/
u16 DATAFLASH_ReadByte(void)
{
	u8 Data = 0;
	u8 count=0xFF;
	/* Wait until a data is received */
	while ((--count) && SPI_I2S_GetFlagStatus(DATAFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)
		;
	/* Get the received data */
	Data = SPI_I2S_ReceiveData(DATAFLASH_SPI);

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
void SPI_Config(void)
{
	uint32_t delay;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	/* GPIOA and GPIOC Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* SPI Periph clock enable */
	RCC_APB2PeriphClockCmd(DATAFLASH_RCC_SPI, ENABLE);

	/* Configure SPI pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

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
	SPI_Init(DATAFLASH_SPI, &SPI_InitStructure);

	/* SPI enable */
	SPI_Cmd(DATAFLASH_SPI, ENABLE);

	for (delay = 0; delay < 0xfffff; delay++)
		;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Sector Erase instruction */
  SPI_FLASH_SendByte(SE);
  /* Send SectorAddr high nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(void)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  /* Bulk Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Bulk Erase instruction  */
  SPI_FLASH_SendByte(BE);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
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
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(WRITE);
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  /* while there is data to be written on the FLASH */
  while(NumByteToWrite--)
  {
    /* Send the current byte */
    SPI_FLASH_SendByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
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
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if(Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if(NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
   }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if(NumOfPage== 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if(NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
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
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(READ);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while(NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
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
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(READ);

/* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
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
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_FLASH_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  /* Loop while DR register in not emplty */
  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET);

  /* Send Half Word through the SPI1 peripheral */
  SPI_SendData(SPI1, HalfWord);

  /* Wait to receive a Half Word */
  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET);

  /* Return the Half Word read from the SPI bus */
  return SPI_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(WREN);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
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
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(RDSR);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);

  } while((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
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
	if (DATAFLASH_Init() == DATAFLASH_RESPONSE_NO_ERROR)
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
}
