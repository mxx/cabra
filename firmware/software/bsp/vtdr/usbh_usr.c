/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include <application.h>
#include <DataManager.h>

#include <rthw.h>
#include <rtthread.h>

#include <ff.h>
/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
#define WRITE_LENTH_MAX 1024
#define WRITE_LENTH  1000
/**
* @}
*/ 
const unsigned char Namemap16[16][18] ={


};

/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern unsigned char CurStatus;
extern StructPara Parameter;
extern PartitionTable pTable;
extern CLOCK curTime;
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
#define NULL 0

/**
* @}
*/ 
extern void Fillthefilename();

/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
//uint16_t filenameString[]  = {0x3a30,0xcfc9,0xc2cf,0x542e,0x5458};
//uint8_t writeTextBuff[] = "STM32 Connectivity line Host Demo application using FAT_FS   ";

uint16_t filenameString[13];
FATFS fatfs; //modify by leiyq 20120219
FIL file;    //modify by leiyq 20120219
uint8_t line_idx = 0;   

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level);
static void    Toggle_Leds(void);
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 

//#define rt_kprintf   LOGOUT
/**
* @brief  USBH_USR_Init 
*         Displays the message for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
  static uint8_t startup = 0;  
  
  if(startup == 0 )
  {
    startup = 1;
    /* Configure the LEDs */
  }
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
	USBH_USR_ApplicationState = 0;
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 

}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
	USBH_USR_ApplicationState = 0;
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {

  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {

  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {

  }
  else
  {

  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  //LOGOUT("VID : %04Xh\n" , (uint32_t)(*hs).idVendor);
 // LOGOUT("PID : %04Xh\n" , (uint32_t)(*hs).idProduct);
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
    //LOGOUT((void *)MSG_MSC_CLASS);
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
    //LOGOUT((void *)MSG_HID_CLASS);
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
 // LOGOUT("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
 // LOGOUT("Product : %s\n", (char *)ProductString);
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
 // LOGOUT( "Serial Number : %s\n", (char *)SerialNumString);
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
  
  /* Enumeration complete */
  //LOGOUT((void *)MSG_DEV_ENUMERATED);
  
 // LOGOUT( "To see the root content of the disk : " );
  //LOGOUT("Press Key...");
  
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
  //LOGOUT ("> Device not supported.");
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
  
  usbh_usr_status = USBH_USR_NO_RESP;  
  
  /*Key B3 is in polling mode to detect user action */
  //if(STM_EVAL_PBGetState(Button_KEY) == RESET)
  {
    
    usbh_usr_status = USBH_USR_RESP_OK;
    
  } 
  return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
 // LOGOUT ("Overcurrent detected.");
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
  FRESULT res;//modify by leiyq 20120319
  uint16_t bytesWritten, bytesToWrite,i;
  switch(USBH_USR_ApplicationState)
  {
  case USH_USR_FS_INIT: 
	  TIM_Cmd(TIM3, ENABLE);
	  USBH_USR_ApplicationState = USH_USR_IDEL;
    break;
    
  case USH_USR_FS_READLIST:
	   TIM_Cmd(TIM3, DISABLE);
	    /* Initialises the File System*/
	    if ( f_mount( 0, &fatfs ) != FR_OK ) //modify by leiyq20120319
	    {
	      return(-1);
	    }
	    USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;//modify by leiyq 20120219
    
    break;
    
  case USH_USR_FS_WRITEFILE:
    

		USB_OTG_BSP_mDelay(100);
		if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
		{

		  //LOGOUT ( "> Disk flash is write protected \n");
		  USBH_USR_ApplicationState = USH_USR_FS_DRAW;
		  break;
		}

		f_mount(0, &fatfs);
		Fillthefilename();
		res = f_open(&file,( const TCHAR *)filenameString,FA_CREATE_ALWAYS | FA_WRITE);
		if(res == FR_OK)
		{
			/* Write buffer to file */
			for(i= 1;i<200;i++)
			WriteTheData(1);
			USBH_USR_ApplicationState = USH_USR_FS_DRAW;

		}
		else
		{
			//LOGOUT ("> STM32.TXT created in the disk\n");
			USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
			rt_kprintf("> STM32.TXT created in the disk\n");
		}
		f_close(&file);
		f_mount(0, NULL);
  
    break;
    
  case USH_USR_FS_DRAW:
	  USBH_USR_ApplicationState = USH_USR_IDEL;
	  TIM_Cmd(TIM3, ENABLE);
    /*Key B3 in polling*/
    break;
  default: break;
  }
  return(0);
}

/**
* @brief  Explore_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level)
{
	return 0;
}

unsigned char BCD2ASCLL(unsigned char data, unsigned char type)
{
	unsigned char ret;
	if(type)
	{
		ret = (data & 0xf0) >> 4;
	}
	else
		ret = data & 0x0f;
	ret = ret+0x30;
	return ret;
}
//DXXXXXX_XXXX_XXXXXXXX
//uint16_t filenameString[]  = {0x3a30,0xcfc9,0xc2cf,0x542e,0x5458};
void Fillthefilename()
{
	filenameString[0]= 0x3a30;
	filenameString[1]= BCD2ASCLL(curTime.year,1) +(BCD2ASCLL(curTime.year,0)<<8);
	filenameString[2]= BCD2ASCLL(curTime.month,1)+(BCD2ASCLL(curTime.month,0)<<8);
	filenameString[3]= BCD2ASCLL(curTime.day,1)	+(BCD2ASCLL(curTime.day,0)<<8);
	filenameString[4]= 0x005f+(BCD2ASCLL(curTime.hour,1)<<8);
	filenameString[5]= BCD2ASCLL(curTime.hour,0)+(BCD2ASCLL(curTime.minute,1)<<8);
	filenameString[6]= BCD2ASCLL(curTime.minute,0)+0x5f00;
	filenameString[7]= (Parameter.AutoInfodata.AutoCode[1]<<8)+Parameter.AutoInfodata.AutoCode[0];
	filenameString[8]= (Parameter.AutoInfodata.AutoCode[3]<<8)+Parameter.AutoInfodata.AutoCode[2];
	filenameString[9]= (Parameter.AutoInfodata.AutoCode[5]<<8)+Parameter.AutoInfodata.AutoCode[4];;
	filenameString[10]= (Parameter.AutoInfodata.AutoCode[7]<<8)+Parameter.AutoInfodata.AutoCode[6];
	filenameString[11]= 0x562e;
	filenameString[12]= 0x5244;

}
void WriteTheData(unsigned short num)
{
#if 0
		FRESULT res;//modify by leiyq 20120319
		uint16_t bytesWritten, bytesToWrite;
		bytesToWrite = sizeof(writeTextBuff);
		res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);

		if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
		{
			//rt_kprintf("> STM32.TXT CANNOT be writen.\n");
			res= 0;
			//f_close(&file);
		}
		f_lseek(&file,(bytesToWrite+2)*(num+1));

		/*close file and filesystem*/
		//f_close(&file);
		//f_mount(0, NULL);
#else
		unsigned char writeTextBuff[WRITE_LENTH_MAX];
		unsigned char Wcount,i;
		uint16_t bytesWritten, bytesToWrite;
		FRESULT res;//modify by leiyq 20120319
		bytesToWrite = FilltheTextBuff(writeTextBuff,0,&Wcount);
		res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);
		if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
		{
			rt_kprintf("> STM32.TXT CANNOT be writen.\n");
		}
		f_lseek(&file,bytesToWrite);
		for(i = 0x08;i<0x16;i++)
		{
			if((i==0x08) ||(i=0x09 )||(i<0x16&&i>0x0f))
			{
				Wcount =0;
				do
				{
					bytesToWrite = FilltheTextBuff(writeTextBuff,i,&Wcount);
					res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);
					if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
					{
						rt_kprintf("> STM32.TXT CANNOT be writen.\n");
					}
					f_lseek(&file,bytesToWrite);
				}while(Wcount);
			}
		}
		/*close file and filesystem*/
		f_close(&file);
		f_mount(0, NULL);

#endif
}
void GettheBlock(unsigned char *Nameptr,unsigned char NameNum,unsigned long lenth)
{
	unsigned char i;
	if(NameNum >0x09)
	{
		Nameptr[0] = NameNum+6;
	}
	else
	{
		Nameptr[0] = NameNum;
	}
	for (i = 1;i<19;i++)
	{
		Nameptr[i] = (unsigned char )Namemap16[NameNum][i-1];
	}
	for (i = 19;i<23;i++)
	{
		Nameptr[i] = (unsigned char )(lenth>>((22-i)*8));
	}
	switch(NameNum)
	{
		case 0:
			Nameptr[i] = Parameter.standeryear;
			Nameptr[i+1] = Parameter.modifyNb;
			break;
		case 1:
			for(i = 23;i<41;i++)
			{
				Nameptr[i] = pTable.DriverLisenseCode[i-23];
			}
			break;
		case 2:
			for(i = 23;i<29;i++)
			{
				Nameptr[i] = *((unsigned char *)&curTime+i-23);
			}
			break;
		case 3:
			for(i = 23;i<29;i++)
			{
				Nameptr[i] = *((unsigned char *)&curTime+i-23);
			}
			for(i = 29;i<35;i++)
			{
				Nameptr[i] = *((unsigned char *)&Parameter.InstallTime+i-29);
			}
			for(i = 35;i<39;i++)
			{
				Nameptr[i] = *((unsigned char *)&Parameter.DriverDistace+i-35);
			}
			for(i = 39;i<43;i++)
			{
				Nameptr[i] = *((unsigned char *)&Parameter.StarDistance+i-39);
			}
			break;
		case 4:
			for(i = 23;i<29;i++)
			{
				Nameptr[i] = *((unsigned char *)&curTime+i-23);
			}
			for(i = 29;i<31;i++)
			{
				Nameptr[i] = *((unsigned char *)&Parameter.PulseCoff+i-29);
			}
			break;
		case 5:
			for(i = 23;i<64;i++)
			{
				Nameptr[i] = *((unsigned char *)&Parameter.AutoInfodata+i-23);
			}
			break;
		case 6:
			for(i = 23;i<29;i++)
			{
				Nameptr[i] = *((unsigned char *)&curTime+i-23);
			}
			Nameptr[29] = CurStatus;
			for(i = 30;i<110;i++)
			{
				Nameptr[i] = *((unsigned char *)&Parameter.singalname+i-30);
			}

			break;
		case 7:
			for(i = 23;i<29;i++)
			{
				Nameptr[i] = *((unsigned char *)&Parameter.typedata+i-23);
			}
			break;
		default:
			break;


	}

}
void FilltheBlock(unsigned char *buf)
{
	unsigned char i;
	buf[0] = 0;
	buf[1] = 16;
	GettheBlock(&buf[2],0,2);
	GettheBlock(&buf[27],1,18);
	GettheBlock(&buf[68],2,6);
	GettheBlock(&buf[97],3,20);
	GettheBlock(&buf[140],4,8);
	GettheBlock(&buf[171],5,41);
	GettheBlock(&buf[235],6,87);
	GettheBlock(&buf[345],7,35);
}
unsigned short FillthedataBlock(unsigned char *buf,unsigned char block,unsigned char *count)
{
	static unsigned long readnum;
	static unsigned long readaddr;
	unsigned short renum;
	unsigned short BlockSize;
	unsigned long STOPp,STOPb,startbase,endbase;
	unsigned long flag;
	switch (block)
	{
		case 0x08:
			BlockSize = DRV_SPEED_BLOCK;
			STOPp = pTable.BaseData.CurPoint;
			flag = pTable.BaseData.finshflag;
			startbase = BASEDATA_BASE;
			endbase = BASEDATA_END;
			break;
		case 0x09:
			BlockSize = LOCATION_BLOCK;
			STOPp = pTable.LocationData.CurPoint;
			flag = pTable.LocationData.finshflag;
			startbase = LOCATION_BASE;
			endbase = LOCATION_END;
			break;
		case 0x10:
			BlockSize = DOUBLT_BLOCK;
			STOPp = pTable.OverSpeedRecord.CurPoint;
			flag = pTable.OverSpeedRecord.finshflag;
			startbase = DPD_BASE;
			endbase = DPD_END;
			break;
		case 0x11:
			BlockSize = OVERDRV_BLOCK;
			STOPp = pTable.DoubtPointData.CurPoint;
			flag = pTable.BaseData.finshflag;
			startbase = OVERDRV_BASE;
			endbase = OVERDRV_END;
			break;
		case 0x12:
			BlockSize = DRV_RG_BLOCK;
			STOPp = pTable.DriverReRecord.CurPoint;
			flag = pTable.DriverReRecord.finshflag;
			startbase = DRVRG_BASE;
			endbase = DRVRG_END;
			break;
		case 0x13:
			BlockSize = POW_BLOCK;
			STOPp = pTable.PowerOffRunRecord.CurPoint;
			flag = pTable.PowerOffRunRecord.finshflag;
			startbase = POWER_BASE;
			endbase = POWER_END;
			break;
		case 0x14:
			BlockSize = PARA_BLOCK;
			STOPp = pTable.ModifyRecord.CurPoint;
			flag = pTable.ModifyRecord.finshflag;
			startbase = PARA_BASE;
			endbase = PARA_END;
			break;
		case 0x15:
			BlockSize = JN_BLOCK;
			STOPp = pTable.journalRecord.CurPoint;
			flag = pTable.journalRecord.finshflag;
			startbase = JN_BASE;
			endbase =  JN_END;
			break;
		default:
			break;

	}
	if( flag == 0xeaea)
	{
		if((*count)==0)
		{
			readaddr = STOPp;
			if(readaddr > startbase)
			{
				readaddr = readaddr-BlockSize;
			}
			else
			{
				readaddr = (endbase-startbase+1)/BlockSize;
				readaddr = readaddr*BlockSize+startbase;
			}
			readnum = (endbase -startbase+1)/BlockSize;
			readnum = readnum*BlockSize;
			GettheBlock(buf,block,readnum);
			renum = WRITE_LENTH;
			if(readnum%renum)
			{
				*count = readnum/renum;
			}
			else
			{
				*count = readnum/renum-1;
			}
			readnum = readnum-renum;
			SPI_FLASH_BufferRead(SPI1 ,&buf[23] ,readaddr, renum);
			renum = renum+23;

		}
		else
		{
			if(readaddr > startbase)
			{
				readaddr = readaddr-BlockSize;
			}
			else
			{
				readaddr = (endbase-startbase+1)/BlockSize;
				readaddr = readaddr*BlockSize+startbase;
			}
			(*count)--;
			if((*count)!= 1)
			{
				renum = WRITE_LENTH;
				readnum = readnum-renum;
			}
			else
			{
				renum = readnum;
			}
			SPI_FLASH_BufferRead(SPI1 ,buf ,STOPp, renum);
		}
	}
	else
	{
		if((*count)==0)
		{
			readaddr = STOPp;
			readnum = (STOPp -startbase)/BlockSize;
			readnum = readnum*BlockSize;
			if(readnum  == 0)
			{
				renum = 0;
				GettheBlock(buf,block,0);
				return renum;
			}
			else
			{
				readaddr = readaddr-BlockSize;
				GettheBlock(buf,block,readnum);
				if(readnum%WRITE_LENTH)
				{
					*count = readnum/WRITE_LENTH;
					if((*count)!= 0)
					{
						renum = WRITE_LENTH;
						readnum = readnum-renum;
					}
					else
					{
						renum = readnum;
					}

				}
				else
				{
					*count = readnum/renum-1;
					renum = WRITE_LENTH;
					readnum = readnum-renum;
				}
				SPI_FLASH_BufferRead(SPI1 ,&buf[23] ,STOPp, renum);
				renum = renum+23;
			}

		}
		else
		{
			readaddr = readaddr-BlockSize;
			(*count)--;
			if((*count)!= 1)
			{
				renum = WRITE_LENTH;
				readnum = readnum-renum;
			}
			else
			{
				renum = readnum;
			}
			SPI_FLASH_BufferRead(SPI1 ,buf ,STOPp, renum);
		}
	}
	return renum;

}

unsigned short FilltheTextBuff(unsigned char *bufptr,unsigned char block,unsigned char *Count)
{
	unsigned short writenum =0;
	switch (block)
	{
		case 0:
			FilltheBlock(bufptr);
			writenum = 403;
			break;
		case 0x08:
		case 0x09:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
			writenum = FillthedataBlock(bufptr,block,Count);
			break;
		default:
			break;
	}
	return writenum;
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

