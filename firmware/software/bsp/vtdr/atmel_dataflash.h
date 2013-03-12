/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : msd.h
* Author             : MCD Application Team
* Version            : V2.1
* Date               : 05/30/2008
* Description        : Header for msd.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATAFLASH_H
#define __DATAFLASH_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>

/* Private define ------------------------------------------------------------*/
/* Block Size */
#define BLOCK_SIZE    512

/* Dummy byte */
#define DUMMY   0xFF

/* Start Data tokens  */
/* Tokens (necessary because at nop/idle (and CS active) only 0xff is on the data/command line) */
#define DATAFLASH_START_DATA_SINGLE_BLOCK_READ 0xFE  /* Data token start byte, Start Single Block Read */
#define DATAFLASH_START_DATA_MULTIPLE_BLOCK_READ  0xFE  /* Data token start byte, Start Multiple Block Read */
#define DATAFLASH_START_DATA_SINGLE_BLOCK_WRITE 0xFE  /* Data token start byte, Start Single Block Write */
#define DATAFLASH_START_DATA_MULTIPLE_BLOCK_WRITE 0xFD  /* Data token start byte, Start Multiple Block Write */
#define DATAFLASH_STOP_DATA_MULTIPLE_BLOCK_WRITE 0xFD  /* Data toke stop byte, Stop Multiple Block Write */

/* DATAFLASH functions return */
#define DATAFLASH_SUCCESS       0x00
#define DATAFLASH_FAIL          0xFF

/* DATAFLASH reponses and error flags */
#define DATAFLASH_RESPONSE_NO_ERROR      0x00
#define DATAFLASH_IN_IDLE_STATE          0x01
#define DATAFLASH_ERASE_RESET            0x02
#define DATAFLASH_ILLEGAL_COMMAND        0x04
#define DATAFLASH_COM_CRC_ERROR          0x08
#define DATAFLASH_ERASE_SEQUENCE_ERROR   0x10
#define DATAFLASH_ADDRESS_ERROR          0x20
#define DATAFLASH_PARAMETER_ERROR        0x40
#define DATAFLASH_RESPONSE_FAILURE       0xFF

/* Data response error */
#define DATAFLASH_DATA_OK                0x05
#define DATAFLASH_DATA_CRC_ERROR         0x0B
#define DATAFLASH_DATA_WRITE_ERROR       0x0D
#define DATAFLASH_DATA_OTHER_ERROR       0xFF

/* Commands */
#define DATAFLASH_GO_IDLE_STATE          0   /* CMD0=0x40 */
#define DATAFLASH_SEND_OP_COND           1   /* CMD1=0x41 */
#define DATAFLASH_SEND_CSD               9   /* CMD9=0x49 */
#define DATAFLASH_READ_CID               0x9F
#define DATAFLASH_STOP_TRANSMISSION      12  /* CMD12=0x4C */
#define DATAFLASH_SEND_STATUS            13  /* CMD13=0x4D */
#define DATAFLASH_SET_BLOCKLEN           16  /* CMD16=0x50 */
#define DATAFLASH_READ_SINGLE_BLOCK      17  /* CMD17=0x51 */
#define DATAFLASH_READ_MULTIPLE_BLOCK    18  /* CMD18=0x52 */
#define DATAFLASH_SET_BLOCK_COUNT        23  /* CMD23=0x57 */
#define DATAFLASH_WRITE_BLOCK            24  /* CMD24=0x58 */
#define DATAFLASH_WRITE_MULTIPLE_BLOCK   25  /* CMD25=0x59 */
#define DATAFLASH_PROGRAM_CSD            27  /* CMD27=0x5B */
#define DATAFLASH_SET_WRITE_PROT         28  /* CMD28=0x5C */
#define DATAFLASH_CLR_WRITE_PROT         29  /* CMD29=0x5D */
#define DATAFLASH_SEND_WRITE_PROT        30  /* CMD30=0x5E */
#define DATAFLASH_TAG_SECTOR_START       32  /* CMD32=0x60 */
#define DATAFLASH_TAG_SECTOR_END         33  /* CMD33=0x61 */
#define DATAFLASH_UNTAG_SECTOR           34  /* CMD34=0x62 */
#define DATAFLASH_TAG_ERASE_GROUP_START  35  /* CMD35=0x63 */
#define DATAFLASH_TAG_ERASE_GROUP_END    36  /* CMD36=0x64 */
#define DATAFLASH_UNTAG_ERASE_GROUP      37  /* CMD37=0x65 */
#define DATAFLASH_ERASE                  38  /* CMD38=0x66 */
#define DATAFLASH_READ_OCR               39  /* CMD39=0x67 */
#define DATAFLASH_CRC_ON_OFF             40  /* CMD40=0x68 */

/* Exported types ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct _DATAFLASH_CSD      /*Card Specific Data*/
{
  vu8  CSDStruct;            /* CSD structure */
  vu8  SysSpecVersion;       /* System specification version */
  vu8  Reserved1;            /* Reserved */
  vu8  TAAC;                 /* Data read access-time 1 */
  vu8  NSAC;                 /* Data read access-time 2 in CLK cycles */
  vu8  MaxBusClkFrec;        /* Max. bus clock frequency */
  vu16 CardComdClasses;      /* Card command classes */
  vu8  RdBlockLen;           /* Max. read data block length */
  vu8  PartBlockRead;        /* Partial blocks for read allowed */
  vu8  WrBlockMisalign;      /* Write block misalignment */
  vu8  RdBlockMisalign;      /* Read block misalignment */
  vu8  DSRImpl;              /* DSR implemented */
  vu8  Reserved2;            /* Reserved */
  vu16 DeviceSize;           /* Device Size */
  vu8  MaxRdCurrentVDDMin;   /* Max. read current @ VDD min */
  vu8  MaxRdCurrentVDDMax;   /* Max. read current @ VDD max */
  vu8  MaxWrCurrentVDDMin;   /* Max. write current @ VDD min */
  vu8  MaxWrCurrentVDDMax;   /* Max. write current @ VDD max */
  vu8  DeviceSizeMul;        /* Device size multiplier */
  vu8  EraseGrSize;          /* Erase group size */
  vu8  EraseGrMul;           /* Erase group size multiplier */
  vu8  WrProtectGrSize;      /* Write protect group size */
  vu8  WrProtectGrEnable;    /* Write protect group enable */
  vu8  ManDeflECC;           /* Manufacturer default ECC */
  vu8  WrSpeedFact;          /* Write speed factor */
  vu8  MaxWrBlockLen;        /* Max. write data block length */
  vu8  WriteBlockPaPartial;  /* Partial blocks for write allowed */
  vu8  Reserved3;            /* Reserded */
  vu8  ContentProtectAppli;  /* Content protection application */
  vu8  FileFormatGrouop;     /* File format group */
  vu8  CopyFlag;             /* Copy flag (OTP) */
  vu8  PermWrProtect;        /* Permanent write protection */
  vu8  TempWrProtect;        /* Temporary write protection */
  vu8  FileFormat;           /* File Format */
  vu8  ECC;                  /* ECC code */
  vu8  msd_CRC;                  /* CRC */
  vu8  Reserved4;            /* always 1*/
}
sDATAFLASH_CSD;

typedef struct _DATAFLASH_CID      /*Chip Identification Data*/
{
  vu8  ManufacturerID;       /* ManufacturerID */
  vu8 FamilyCode:3;
  vu8 DensityCode:5;
  vu8 MLCCode:3;
  vu8 ProductVersionCode:5;
}
sDATAFLASH_CID;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/*----- High layer function -----*/
u8 DATAFLASH_Init(void);

u8 DATAFLASH_GetDeviceID(sDATAFLASH_CID* DATAFLASH_cid);
u8 DATAFLASH_GetStatusREG( void );
void DATAFLASH_ChangProtectStatus(u8 pBuffer);

/*----- Medium layer function -----*/
/*----- High layer function -----*/
void SPI_FLASH_Init(void);
void SPI_FLASH_Sector4kErase(u32 SectorAddr);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);

/*----- Low layer function -----*/
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

/*----- Low layer function -----*/
void DATAFLASH_WriteByte(u8 byte);
u16 DATAFLASH_ReadByte(void);

#endif /* __DATAFLASH_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
