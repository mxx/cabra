//*----------------------------------------------------------------------------
//*      閿熸枻鎷疯嵔閿熸枻鎷烽敓鏂ゆ嫹鏄犻敓鏂ゆ嫹閿燂拷
//*----------------------------------------------------------------------------
//* File Name           : DataManager.c
//* Object              : 閿熸枻鎷峰綍閿熻閲囩》鎷烽敓鏂ゆ嫹閿熷姭璁规嫹鐘舵�閿熸枻鎷疯幐鎷囬敓鏂ゆ嫹閿熼叺鎲嬫嫹閿熸枻鎷�
//*
//* 1.0 24/02/03 PANHUI : Creation
//*----------------------------------------------------------------------------
//
#include	"DataManager.h"
#include   "atmel_dataflash.h"
#include "application.h"
#include    "lcd.h" 
#include <sys/types.h>
#include  <time.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <string.h>

////time value
extern unsigned char Time10sCnts;
extern unsigned char Time10sCnte;
extern unsigned char Time30mincnt1;
extern unsigned char Time30mincnt2;
extern unsigned char Time30mincnt3;
extern unsigned char Time30mincnt4;
extern unsigned char DisplayMin;

extern unsigned long CurSpeed;
extern unsigned long RsSpeed;
extern CLOCK curTime;
extern unsigned char CurStatus;
extern unsigned long AddupSpeed;
extern unsigned short SpeedNb;
extern unsigned long Curspeed1min;
extern unsigned long Curspeed1s;
PartitionTable pTable;
StructPara Parameter;
extern unsigned long PulseTotalNumber;	/*閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽┒閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹*/

unsigned char TimeChange;	//时锟斤拷浠拷锟街�
unsigned short DriveMinuteLimit;       //疲锟酵硷拷驶锟斤拷驶时锟斤拷锟斤拷锟斤拷
unsigned short RestMinuteLimit;        //疲锟酵硷拷驶锟斤拷锟斤拷锟斤拷息时锟斤拷锟斤拷锟斤拷
unsigned char AlarmFlag;

DoubtDataBlock ddb;			//閿熸枻鎷峰墠閿熺即纰夋嫹閿熸枻鎷疯彉閿燂拷
BaseDataBlock basedata;
LocationBlock Locationdata;
SizeData location;
OverDriverBlock overdriverdata;
DriverRegisterBlock driverregisterdata;
PowerBlock powerdata;
ParaModifyBlock modifydata;
JournalBlock journaldata;
CLOCK startdriverclk;
CLOCK enddriverclk;

unsigned char InRecordCycle=0;		//閿熻鍑ゆ嫹閿熻妭纭锋嫹褰曢敓鏂ゆ嫹鑾ㄩ敓鏂ゆ嫹閿熸枻鎷�
unsigned char speeddel;
Datastatus Datastatusdata;
unsigned char InFlashWriting=0;	//閿熸枻鎷稦LASH鍐欓敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
unsigned long Tick02NB;				//閿熸枻鎷烽敓鏂ゆ嫹鍋滈敓鏂ゆ嫹涔嬮敓鏂ゆ嫹0.2s閿熶茎闈╂嫹閿熸枻鎷�
OTDR_end otdrEND;			//鐤查敓閰电》鎷烽┒閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
unsigned long AddupSpeed = 0;		//1閿熸枻鎷烽敓鏂ゆ嫹閿熷姭璁规嫹閿熸纭锋嫹
unsigned short SpeedNb = 0;		//1閿熸枻鎷烽敓鏂ゆ嫹閿熷姭璁规嫹鍊奸敓鏂ゆ嫹閿熸枻鎷�
unsigned char PowerOnTime=0;		//閿熻緝纰夋嫹閿熸枻鎷烽敓缁炴唻鎷烽敓锟�unsigned char OTRecordType=0;		//閿熸枻鎷烽敓鏂ゆ嫹鐤查敓閰电》鎷烽┒閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹
unsigned long LastDistance;			//閿熻緝杈炬嫹鐤查敓閰电》鎷烽┒閿熸枻鎷峰綍閿熸纭锋嫹閿熸枻鎷烽敓锟�unsigned char STATUS1min;			//1閿熸枻鎷烽敓鏂ゆ嫹鐘舵�閿熸枻鎷�
unsigned char DriverStatus = 0;
unsigned char DriverRegstatus = 0;
unsigned char powerstatus = 0;
unsigned char paramodifystatus = 0;
unsigned char JournalRecordstatus = 0;
unsigned char DoubltPointstatus = 0;
DRIVER CurrentDriver;		//閿熸枻鎷峰墠鍙搁敓鏂ゆ嫹
DRIVER RecordDriver;		//閿熸枻鎷峰綍鍙搁敓鏂ゆ嫹
Record_CLOCK PowerOnDT;     //閿熻緝纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹
RecordData_end rec_end;
unsigned char FinishFlag=0;

extern unsigned char LargeDataBuffer[];
unsigned short *DoubtData_4k = (unsigned short *)(&(LargeDataBuffer[12*1024]));//[2*1024];
unsigned char *OTDR_4k = &(LargeDataBuffer[16*1024]);//[4*1024];
unsigned short *BaseData_4k = (unsigned short *)(&(LargeDataBuffer[20*1024]));//[2*1024];
unsigned char *Location_4k = &(LargeDataBuffer[28*1024]);
unsigned char *temp_4k=&(LargeDataBuffer[32*1024]);

//*----------------------------------------------------------------------------
//* Function Name       : Task_GetData
//* Object              : 閿熺殕纭锋嫹閿熸枻鎷烽敓锟�//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
void SelfCheck()
{
	int clock_succ = 1;//GetCurrentDateTime();
	int sl811_succ = sl811s_check();
	if(clock_succ && sl811_succ)
		DisplayOK();
	else if(!clock_succ)
		DisplayClockError();
	else
		DisplayError();
		
	int i;
	for(i=0;i<200000;i++);
	lcd_clear(lineall);
}

//*----------------------------------------------------------------------------
//* Function Name       : StrCmp
//* Object              : 閿熼ズ鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熻鍑ゆ嫹閿熸枻鎷峰悓
//* Input Parameters    : 閿熸枻鎷峰唹绯婚敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍧�敓绲猼r1,str2,閿熸枻鎷烽敓鏂ゆ嫹length
//* Output Parameters   : 1閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸枻鎷峰悓閿熸枻鎷�閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹鍚�
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
char StrCmp(unsigned char *str1, unsigned char *str2, short length)
{
	short i;
	char ret;
	ret=1;
	for(i=0;i<length;i++)
	{
		if(str1[i]!=str2[i]){
			ret = 0;
			break;
		}
	}
	return(ret);
}
//*----------------------------------------------------------------------------
//* Function Name       : BCD2Char
//* Object              : BCD閿熸枻鎷疯浆閿熸枻鎷蜂负鍗侀敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
//* Input Parameters    : bcd閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰啓杞敓鏂ゆ嫹閿熸枻鎷稡CD閿熸枻鎷�
//* Output Parameters   : 杞敓鏂ゆ嫹閿熸枻鎷烽敓缁烆噯鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
unsigned char BCD2Char(unsigned char bcd)
{
	unsigned char ch;
	unsigned char d1,d0;
	d1=((bcd & 0xf0)>>4);
	if(d1>9)
		return (0xff);
	d0=bcd & 0x0f;
	if(d0>9)
		return (0xff);
	ch = d1*10 + d0;
	return(ch);
}
time_t timechange(CLOCK time)
{
	time_t now;
	struct tm ti;
	ti.tm_year =   BCD2Char(time.year) +100;
	ti.tm_mon 	= BCD2Char(time.month )- 1; /* ti->tm_mon 	= month; 0~11 */
	ti.tm_mday = BCD2Char(time.day);
	ti.tm_hour = BCD2Char(time.hour);
	ti.tm_min 	= BCD2Char(time.minute);
	ti.tm_sec 	= 0;
    now = mktime(&ti);
    return now;
}
//*----------------------------------------------------------------------------
//* Function Name       : WriteParameterTable
//* Object              : 鍐欓敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* Input Parameters    : none
//* Output Parameters   : 閿熻鍑ゆ嫹鏅掗敓锟�//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int WriteParameterTable(StructPara *para)
{
	unsigned long *data;
	int i,size;

	SPI_FLASH_Sector4kErase(SPI1,PARAMETER_BASE);
	
	data = (unsigned long *)(para);
	size=sizeof(StructPara);

	SPI_FLASH_BufferWrite( SPI1 ,(u8 *)data, PARAMETER_BASE, size);

	
	return ( TRUE ) ;
}
//*----------------------------------------------------------------------------
//* Function Name       : WritePartitionTable
//* Object              : 鍐欓敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* Input Parameters    : none
//* Output Parameters   : 閿熻鍑ゆ嫹鏅掗敓锟�//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int WritePartitionTable(PartitionTable *ptt)
{
	unsigned long *data;
	int i,size;

	SPI_FLASH_Sector4kErase(SPI1,PartitionTable_BASE);
	
	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙揪鎷烽敓绱絃ASH
	if(ptt->TotalDistance==0xffffffff)
		ptt->TotalDistance = 0;
	data = (unsigned long *)(ptt);
	size=sizeof(PartitionTable);
	
	SPI_FLASH_BufferWrite( SPI1 ,(u8 *)data, PartitionTable_BASE, size);
		
	return ( TRUE ) ;
}
//*----------------------------------------------------------------------------
//* Function Name       : IsPartitionTableCorrect
//* Object              : 閿熸枻鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* Input Parameters    : none
//* Output Parameters   : 鍐欓敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹娆犻敓缂寸櫢鎷�
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int IsPartitionTableCorrect()
{
	unsigned long *data;
	unsigned long *p;
	int i,size;
	
	/////////*******2003.10.06 panhui*********////////
	unsigned short temp = PartitionTableFlag;
	if( pTable.Available != temp)
		return(0);
	/////////*******2003.10.06 panhui*********////////
	if(	pTable.DoubtPointData.BaseAddr != DPD_BASE)
		return(0);
	if(	pTable.DoubtPointData.EndAddr != DPD_END)
		return(0);

	if(	pTable.BaseData.BaseAddr != BASEDATA_BASE)
		return(0);
	if(	pTable.BaseData.EndAddr != BASEDATA_END)
		return(0);

	/////////////modified by panhui 2003.10.20////////////
	if((pTable.DoubtPointData.CurPoint < DPD_BASE)||(pTable.DoubtPointData.CurPoint > DPD_END-110))
		return(-1);

	if((pTable.BaseData.CurPoint < BASEDATA_BASE)||(pTable.BaseData.CurPoint > BASEDATA_END))
		return(-3);
	/////////////閿熸枻鎷锋鎸囬敓鏂ゆ嫹閿熸澃鍑ゆ嫹/////////////////////////////

	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙揪鎷烽敓绱絃ASH
	return (1);
	

}
//*----------------------------------------------------------------------------
//* Function Name       : InitializeTable
//* Object              : 閿熸枻鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* Input Parameters    : unsigned char parti閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸枻鎷烽敓閾版仮闈╂嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//                        unsigned char para閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸枻鎷烽敓閾版仮闈╂嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//                        unsigned char change_set閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鐭鎷烽敓鐙″彿锝忔嫹
//                                             閿熸枻鎷烽敓鏂ゆ嫹閿熸纭锋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//* Output Parameters   : 閿熸枻鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閰佃鎷烽敓鏂ゆ嫹閿熸枻鎷锋瑺閿熺即鐧告嫹
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int InitializeTable()
{
	int i;
	SPI_FLASH_BufferRead(SPI1,(unsigned char *)&pTable,PartitionTable_BASE,sizeof(pTable));
	SPI_FLASH_BufferRead(SPI1,(unsigned char *)&Parameter,PARAMETER_BASE,sizeof(Parameter));
	Parameter.DriverDistace = 0x45678961;
	//Parameter.PulseCoff = 5000;
	if(Parameter.mark != 0xaeae)
	{
		//
		Parameter.mark = 0xaeae;
		Parameter.AutoInfodata.AutoCode[0] = 0xbe;
		Parameter.AutoInfodata.AutoCode[1] = 0xa9;
		Parameter.AutoInfodata.AutoCode[2] = 'A';
		Parameter.AutoInfodata.AutoCode[3] = '1';
		Parameter.AutoInfodata.AutoCode[4] = '2';
		Parameter.AutoInfodata.AutoCode[5] = '3';
		Parameter.AutoInfodata.AutoCode[6] = '4';
		Parameter.AutoInfodata.AutoCode[7] = '5';
		memcpy(Parameter.AutoInfodata.AutoSort,"EQ1195GX24D1",12);
		memcpy(Parameter.AutoInfodata.AutoVincode,"XTA210900N1093188",17);
		memcpy(Parameter.DriverLisenseCode ,"440781198602126403",18);
		if( !WriteParameterTable(&Parameter) )
			return (0);
	}
	
	if(pTable.Available !=PartitionTableFlag)
	{
		//鍒濆鍖杙Table
		pTable.Available = PartitionTableFlag;
		pTable.DoubtPointData.BaseAddr = DPD_BASE;
		pTable.DoubtPointData.EndAddr = DPD_END;
		pTable.DoubtPointData.CurPoint = DPD_BASE;

		pTable.BaseData.BaseAddr = BASEDATA_BASE;
		pTable.BaseData.EndAddr = BASEDATA_END;
		pTable.BaseData.CurPoint = BASEDATA_BASE;

		pTable.DriverReRecord.BaseAddr = DRVRG_BASE;
		pTable.DriverReRecord.EndAddr = DRVRG_END;
		pTable.DriverReRecord.CurPoint = DRVRG_BASE;

		pTable.LocationData.BaseAddr = LOCATION_BASE;
		pTable.LocationData.EndAddr = LOCATION_END;
		pTable.LocationData.CurPoint = LOCATION_BASE;

		pTable.OverSpeedRecord.BaseAddr = OVERDRV_BASE;
		pTable.OverSpeedRecord.EndAddr = OVERDRV_END;
		pTable.OverSpeedRecord.CurPoint = OVERDRV_BASE;

		pTable.ModifyRecord.BaseAddr = PARA_BASE;
		pTable.ModifyRecord.EndAddr = PARA_END;
		pTable.ModifyRecord.CurPoint = PARA_BASE;

		pTable.PowerOffRunRecord.BaseAddr = POWER_BASE;
		pTable.PowerOffRunRecord.EndAddr = POWER_END;
		pTable.PowerOffRunRecord.CurPoint = POWER_BASE;

		pTable.journalRecord.BaseAddr = JN_BASE;
		pTable.journalRecord.EndAddr = JN_END;
		pTable.journalRecord.CurPoint = JN_BASE;

		if( !WritePartitionTable(&pTable) )
			return(0);
	}
	
	return (1);
}
void InitialValue()
{
	curTime.day = 0x20;
	curTime.hour = 0x11;
	curTime.minute = 0x40;
	curTime.year = 0x13;
	curTime.month = 0x05;
	CurSpeed =124;
	//AlarmFlag =1;
}
//*----------------------------------------------------------------------------
//* Function Name       : UpdateParameterPartition
//* Object              : 閿熸枻鎷烽敓閾拌鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//* Input Parameters    : none
//* Output Parameters   : 閿熼摪璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹娆犻敓缂寸櫢鎷�
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int UpdateParameterPartition()
{
	unsigned long sector_addr;
	sector_addr = 0;
	unsigned long *data;
	unsigned long *p;
	int i,size;
	
	//閿熸枻鎷烽敓鏂ゆ嫹FLASH
	SPI_FLASH_Sector4kErase(SPI1,*p);
	//鍒烽敓閾拌鎷烽敓鏂ゆ嫹閿燂拷
	p = (unsigned long *)PARAMETER_BASE;
	data = (unsigned long *)(&Parameter);
	size=sizeof(StructPara);
	
	SPI_FLASH_BufferWrite( SPI1 ,(u8 *)data, *p, size);
	
	//鍒烽敓閾板嚖鎷烽敓鏂ゆ嫹閿燂拷
	p = (unsigned long *)PartitionTable_BASE;
	data = (unsigned long *)(&pTable);
	size=sizeof(PartitionTable);
	
	SPI_FLASH_BufferWrite( SPI1 ,(u8 *)data, *p, size);
	return(1);
}
//*----------------------------------------------------------------------------
//* Function Name       : Update4k
//* Object              : FLASH閿熸枻鎷�K閿熸枻鎷烽敓鏂ゆ嫹閿熻妭瀛橈紝閿熸枻鎷烽敓鏂ゆ嫹鍓�k閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓楗哄嚖鎷烽敓鏂ゆ嫹娆犻敓锟�//                        閿熸枻鎷锋寚閿熸枻鎷蜂箣鍓嶉敓鏂ゆ嫹閿熸枻鎷烽敓鍙揪鎷烽敓绱絃ASH
//* Input Parameters    : p閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰墠閿熸枻鎷烽敓琛楅潻鎷烽敓锟�//*                       Buffer閿熸枻鎷烽敓鏂ゆ嫹閿熻妭杈炬嫹閿熷彨杈炬嫹閿熸枻鎷风閿燂拷k閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋閿熻锟�//*                       type閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int Update4k(unsigned long p,unsigned char *Buffer,unsigned char type)
{
	unsigned short i;
	unsigned long sector_addr;
	sector_addr = (p & 0xfffff000);
	SPI_FLASH_BufferRead(SPI1,Buffer,sector_addr,4096);
	if(type == UpdateFlashOnce)
		return(1); 
		
	//閿熸枻鎷烽敓鏂ゆ嫹FLASH
	SPI_FLASH_Sector4kErase(SPI1,sector_addr);
	//鎸囬敓鏂ゆ嫹涔嬪墠閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨杈炬嫹閿熺唇LASH
	if(type == UpdateFlashTimes)
	{
		i=0;
		while(sector_addr<p)
		{
			SPI_FLASH_BufferWrite( SPI1 ,(u8 *)(&Buffer[i]), sector_addr, 2);
			sector_addr++;
			i++;
		}
	}
	return(1);
}
int erase4kflash(unsigned long p ,unsigned long num)
{
	if (num == 0)
		return 0;
	if((p &0x00000fff)== 0)
	{			//Update4k( p,&Location_4k,UpdateFlashTimes);
		SPI_FLASH_Sector4kErase(SPI1,p);

	}
	else if(((p+num)&0x00000fff)<num)
	{
		p = (p+num)&0xfffff000;
		SPI_FLASH_Sector4kErase(SPI1,p);
	}
	return 1;
}
//*----------------------------------------------------------------------------
//* Function Name       : WriteDoubtDataToFlash
//* Object              : 鍐欎竴閿熸枻鎷烽敓缂寸鎷烽敓鏂ゆ嫹鑾搁敓绱絃ASH閿熸枻鎷�
//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : curTime
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------


#if 0
unsigned Char2BCD(unsigned char ch)
{
	unsigned char bcd;
	if(ch >99)
		return (0xff);
	bcd = (((ch/10)<< 4)&0xf0)+(ch%10);
	return (bcd);

}
#endif
//*----------------------------------------------------------------------------
//* Function Name       : IfOneAfterAotherDay
//* Object              : 閿熷彨璁规嫹閿熻鍑ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* Input Parameters    : time閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纰岄敓缁炴唻鎷烽敓琛楅潻鎷烽敓锟�//*                       end閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴閿熸枻鎷风柌閿熼叺纭锋嫹褰曢敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋寚閿熸枻鎷�
//* Output Parameters   : 0閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓瑙掞綇鎷�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int IfOneAfterAotherDay(CLOCK *time,OTDR_end *end)
{
	unsigned char day,month,year;
	day = BCD2Char(time->day);
	if(day>1){
		day --;
		if((day==BCD2Char(end->dt.day))&&(time->month==end->dt.month)&&(time->year==end->dt.year))
			return(1);
		else
			return(0);
	}
	else{
		year = BCD2Char(time->year);
		month = BCD2Char(time->month);
		if(month>1)
			month--;
		else{
			month = 12;
			if(year>1)
				year--;
			else
				year=99;
		}
			
		switch(month){
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				day = 31;break;
			case 4:
			case 6:
			case 9:
			case 11:
				day = 30;break;
			case 2:
				if((year&0x03)==0)
					day = 29;
				else
					day = 28;
				break;
			default:
				return(0);	
		}
		
		if((day==BCD2Char(end->dt.day))&&(month==BCD2Char(end->dt.month))&&(year==BCD2Char(end->dt.year)))
			return(1);
		else
			return(0);

		
	}
	
}
//*----------------------------------------------------------------------------
//* Function Name       : JudgeTimeSpace
//* Object              : 閿熷彨璁规嫹鏃堕敓鏂ゆ嫹閿熸枻鎷�
//* Input Parameters    : time閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纰岄敓缁炴唻鎷烽敓琛楅潻鎷烽敓锟�//*                       end閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴閿熸枻鎷风柌閿熼叺纭锋嫹褰曢敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋寚閿熸枻鎷�
//* Output Parameters   : 鏃堕敓鏂ゆ嫹閿熸枻鎷峰�閿熸枻鎷烽敓鏂ゆ嫹閿熸帴锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹20閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷稦F
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
unsigned char JudgeTimeSpace(CLOCK *time,OTDR_end *end)
{
	int space,conti;
	unsigned char ret;
	if((time->year==end->dt.year)&&(time->month==end->dt.month)&&(time->day==end->dt.day))
	{
		space = BCD2Char(time->hour)*60 + BCD2Char(time->minute) - BCD2Char(end->dt.hour)*60 - BCD2Char(end->dt.minute);
		if((space<0)||(space>20))
			ret=0xff;
		else
			ret=(unsigned char)space;
			
	}
	else
	{
		conti = IfOneAfterAotherDay(time,end);
		if(conti){
			space = 24*60 + BCD2Char(time->hour)*60 + BCD2Char(time->minute) - BCD2Char(end->dt.hour)*60 - BCD2Char(end->dt.minute);
			if((space<0)||(space>20))
				ret=0xff;
			else
				ret=(unsigned char)space;
		}
		else
			ret = 0xff;
	}
	return ret;
}
//*----------------------------------------------------------------------------
//* Function Name       : AddPointer
//* Object              : 閿熸枻鎷烽敓鏂ゆ嫹鎸囬敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍋忛敓鏂ゆ嫹閿熸枻鎷�
//* Input Parameters    : pt閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓缁撴瀯
//*                       inc閿熸枻鎷烽敓鏂ゆ嫹閿熸纭锋嫹鍊�
//* Output Parameters   : 閿熸枻鎷烽敓鏂ゆ嫹閿熶茎鏂ゆ嫹閿燂拷
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
unsigned long AddPointer(StructPT *pt, int inc)
{
	unsigned long result;
	if(inc>=0)
	{
		result = pt->CurPoint + inc;
		if(result > pt->EndAddr)
			result = pt->BaseAddr + ( result - pt->EndAddr ) - 1 ;
	}
	else
	{
		result = pt->CurPoint + inc;
		if(result < pt->BaseAddr)
			result = pt->EndAddr - (pt->BaseAddr - result) + 1 ;
	}
	return(result);
}
//*----------------------------------------------------------------------------
//* Function Name       : Get_otdrEND
//* Object              : 閿熸枻鎷峰彇鐤查敓閰电》鎷烽┒閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎹疯鎷烽敓鎻潻鎷烽敓鏂ゆ嫹閿熸枻鎷疯幖閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//* Input Parameters    : none
//* Output Parameters   : none
//* Global Parameters   : otdrEND,InFlashWriting
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int Get_otdrEND(OTDR_start *start,OTDR_end *end)
{

		return(2);

}
//*----------------------------------------------------------------------------
//* Function Name       : Write4kToFlashOTDR
//* Object              : 閿熼ズ璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹鍐�k閿熻妭杈炬嫹閿熸枻鎷疯幐閿熺唇LASH閿熸枻鎷�
//* Input Parameters    : p閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰墠閿熸枻鎷烽敓琛楅潻鎷烽敓锟�//*                       Buffer閿熸枻鎷烽敓鏂ゆ嫹閿熻妭杈炬嫹閿熷彨杈炬嫹閿熸枻鎷风閿燂拷k閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋閿熻锟�//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int Write4kToFlashOTDR(unsigned short *p,unsigned short *Buffer)
{
	//閿熸枻鎷烽敓鏂ゆ嫹FLASH
	if(!flash_sst39_erase_sector((unsigned long *)DATAFLASH_BASE,(unsigned long *)p))
		return (0);

	return(Write4kToFlash(p,Buffer));
}
//*----------------------------------------------------------------------------
//* Function Name       : Write4kToFlash
//* Object              : 鍐�k閿熻妭杈炬嫹閿熸枻鎷疯幐閿熺唇LASH閿熸枻鎷�
//* Input Parameters    : p閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰墠閿熸枻鎷烽敓琛楅潻鎷烽敓锟�//*                       Buffer閿熸枻鎷烽敓鏂ゆ嫹閿熻妭杈炬嫹閿熷彨杈炬嫹閿熸枻鎷风閿燂拷k閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋閿熻锟�//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int Write4kToFlash(unsigned short *p,unsigned short *Buffer)
{
	unsigned short *inside_p,*flash_p;
	unsigned short data;
	inside_p = (unsigned short *)((unsigned long)p & 0x00fff);//4k閿熻妭纰夋嫹鍧�寚閿熸枻鎷�
	flash_p = p;
	while((unsigned long)inside_p<0x01000)
	{
		data = *((unsigned short *)((unsigned long)Buffer + (unsigned long)inside_p));
		SPI_FLASH_BufferWrite( SPI1 ,(u8 *)&data, *flash_p, 2);
		inside_p++;
		flash_p++;
	}
	return ( 1 );
}

//*----------------------------------------------------------------------------
//* Function Name       : WriteAverageSpeed
//* Object              : 鍐欎竴閿熸枻鎷峰钩閿熸枻鎷烽敓鍔鎷烽敓鏂ゆ嫹鑾搁敓绱絃ASH閿熸枻鎷�
//* Input Parameters    : V閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰啓閿熸枻鎷烽敓鐙℃枻鎷烽敓鏂ゆ嫹淇ｉ敓琛楋拷
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
void WriteAverageSpeed(unsigned char v)
{
	unsigned char *p;
	unsigned long pos,pt,last_pos;
#if 0
	pos = 0x00fff & pTable.RunRecord360h.CurPoint;
	p = (unsigned char *)((unsigned long)(OTDR_4k) + pos);
	*p = v;
	pos++;	

	if(pos==0x01000)
	{//閿熷彨浼欐嫹閿熸枻鎷烽敓鏂ゆ嫹涓�敓鏂ゆ嫹4k
		//閿熸枻鎷烽敓閾扮鎷峰潃鎸囬敓鏂ゆ嫹
		p=(unsigned char *)(OTDR_4k);
		last_pos = pTable.RunRecord360h.CurPoint&0xfffff000;
		pos = (pTable.RunRecord360h.CurPoint&0xff000) + 0x01000;
		if(pos>(pTable.RunRecord360h.EndAddr&0xff000))
			pos = pTable.RunRecord360h.BaseAddr;
		else
			pos += (unsigned long)DATAFLASH_BASE;
		pTable.RunRecord360h.CurPoint = pos;
	
		//鍐欓敓鏂ゆ嫹鍓�k閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷稦LASH閿熸枻鎷�
		Write4kToFlashOTDR((unsigned short *)last_pos,(unsigned short *)OTDR_4k);
		//閿熸枻鎷烽敓閾颁紮鎷烽敓鏂ゆ嫹閿熸枻鎷�
		//Update4k((unsigned short *)pos,(unsigned short *)OTDR_4k,UpdateFlashOnce);
	}
	else
		pTable.RunRecord360h.CurPoint++;	 
#endif

}
//*----------------------------------------------------------------------------
//* Function Name       : FinishOTDRToFlash
//* Object              :閿熸枻鎷烽敓鏂ゆ嫹鍓嶉敓鏂ゆ嫹褰�60灏忔椂骞抽敓鏂ゆ嫹閿熷姭璁规嫹閿熸枻鎷烽┒閿熸枻鎷峰綍閿熸枻鎷风粺閿熸枻鎷风柌閿熼叺纭锋嫹椹堕敓鑺傝揪鎷�
//                       閿熸枻鎷烽敓鑺ュ埌FLASH閿熸枻鎷�
//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
void FinishOTDRToFlash()
{
	unsigned long p;
	otdrEND.driver = RecordDriver;
	if((InRecordCycle&(1<<DOUBLTPOINT))!=0)
	{
		//閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯幐閿燂拷
		otdrEND.dt.type = 0xeaea;
		otdrEND.dt.year = curTime.year;
		otdrEND.dt.month = curTime.month;
		otdrEND.dt.day = curTime.day;
		otdrEND.dt.hour = curTime.hour;
		otdrEND.dt.minute = curTime.minute;
		otdrEND.dt.second = curTime.second;
		otdrEND.TotalDistance = PulseTotalNumber - otdrEND.TotalDistance;
		//if(OTRecordType == MergeLastData)
		//	otdrEND.TotalDistance += LastDistance;
	}
	
	WriteOTDREndData(&otdrEND);
#if 0
	if(pTable.RunRecord360h.CurPoint!=pTable.RunRecord360h.CurPoint)
	{
		p = pTable.RunRecord360h.CurPoint;
		p &= 0xfffff000;
		Write4kToFlashOTDR((unsigned short *)p,(unsigned short *)OTDR_4k);
	}
#endif
}
//*----------------------------------------------------------------------------
//* Function Name       : ModifyUnknownDriver
//* Object              : 姣忛敓鏂ゆ嫹閿熸枻鎷烽┒閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷峰綍閿熷彨纭锋嫹褰曢敓鏂ゆ嫹鏈煡鍙搁敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻棄鍗￠敓鏂ゆ嫹
//						  閿熸枻鎷烽敓鏂ゆ嫹閿熻顎拝鎷烽敓渚ョ》鎷峰綍閿熺潾闈╂嫹閿熸枻鎷峰徃閿熸枻鎷烽敓鏂ゆ嫹
//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//*----------------------------------------------------------------------------
void ModifyUnknownDriver()
{
	unsigned long dpp;
	unsigned short data;
	Record_CLOCK stoptime,PowerOffDT;
	int cmp1,cmp2;
	Record_CLOCK *temp;
	unsigned char i=0;
	
	PowerOffDT.year = curTime.year;
	PowerOffDT.month = curTime.month;
	PowerOffDT.day = curTime.day;
	PowerOffDT.hour = curTime.hour;
	PowerOffDT.minute = curTime.minute;
	PowerOffDT.second = curTime.second;
	
	//閿熺即纰夋嫹閿熼摪锟�	dpp=pTable.DoubtPointData.CurPoint;
	do
	{
		if(dpp==pTable.DoubtPointData.BaseAddr)
			dpp = pTable.DoubtPointData.BaseAddr+96*210+2;
		else
			dpp = dpp-210+2;
		temp = (Record_CLOCK *)dpp;
		stoptime.type = temp->type;
		stoptime.year = temp->year;
		stoptime.month = temp->month;
		stoptime.day = temp->day;
		stoptime.hour = temp->hour;
		stoptime.minute = temp->minute;
		stoptime.second = temp->second;
//		stoptime=*temp;
		dpp = dpp - 2;
		cmp1=CompareDateTime(stoptime,PowerOnDT);
		cmp2=CompareDateTime(PowerOffDT,stoptime); 
		if((cmp1==1)&&(cmp2==1)&&(stoptime.type==0xffff))
		{
			data = (unsigned short)(RecordDriver.DriverCode);
			SPI_FLASH_BufferWrite( SPI1 ,(u8 *)&data, dpp, 2);
			data = (unsigned short)(RecordDriver.DriverCode>>16);
			SPI_FLASH_BufferWrite( SPI1 ,(u8 *)&data, dpp+2, 2);
		}
		i++;//2003.11.11,panhui(閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹)
	}while((cmp1>0)&&(cmp2>=0)&&(i<97));
	
	//360灏忔椂骞抽敓鏂ゆ嫹閿熷姭搴︾》鎷峰綍
}
//*----------------------------------------------------------------------------
//* Function Name       : WriteBaseDataToFlash
//* Object              : 閿熸枻鎷烽敓鏂ゆ嫹椹堕敓鏂ゆ嫹褰曞啓閿熸枻鎷烽敓鏂ゆ嫹閿熺唇LASH
//* Input Parameters    : buf閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯幖閿熸枻鎷烽敓鏂ゆ嫹閿熻闈╂嫹閿燂拷
//                        len閿熸枻鎷烽敓鏂ゆ嫹unsigned short閿熸枻鎷烽敓鏂ゆ嫹鑾╅敓鏂ゆ嫹閿燂拷
//						  type閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* Output Parameters   : if data write TRUE or FALSE
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int WriteBaseDataToFlash(unsigned short *buf,unsigned char len,unsigned char type)
{

	unsigned char i;
	unsigned long pos,UpdatePos;
	
	pos = pTable.BaseData.CurPoint;
	
	for(i=0;i<len;i++)
	{
		SPI_FLASH_BufferWrite( SPI1 ,(u8 *)(&buf[i]), pos, 2);
		pos+=2;
		////////////add by panhui 2003.10.28////////
		if(pos>pTable.BaseData.EndAddr)
			pos = pTable.BaseData.BaseAddr;
		////////////////////////////////////////////
		UpdatePos = pos + RecordFlagByte*2;
		if(UpdatePos > pTable.BaseData.EndAddr)
			UpdatePos = pTable.BaseData.BaseAddr + (UpdatePos-pTable.BaseData.EndAddr)-1;

		if(((pos&0xff000)!=(UpdatePos&0xff000))&&((UpdatePos&0x00fff)==0))
		{
			if(!Update4k(UpdatePos,(unsigned char *)BaseData_4k,UpdateFlashAll))
				return(0);
		}
		
	}
	pTable.BaseData.CurPoint = pos;
	return(1);

}

#if 0
void WritelocationData2Flash(CMD_LOCATION type)
{
	unsigned long p,i;
	p = pTable.LocationData.BakPoint;
	if(type == TIME)
	{
		erase4kflash(p,6);
		for(i= 0;i++;i<6)
		{
			Location_4k[i] = (unsigned char )*((unsigned char * )(&curTime+i));
		}
		SPI_FLASH_BufferWrite(SPI1,Location_4k , p, 6);
		pTable.LocationData.BakPoint =6 ;

	}
	else if(type == DATA)
	{
		erase4kflash(p,11);
		for(i = 0;i++;i<10)
		{
			Location_4k[i] = (unsigned char )*((unsigned char * )(&location+i));
		}
		//Location_4k[i] = Curspeed1min;
		SPI_FLASH_BufferWrite(SPI1,Location_4k , p, 10);
		pTable.LocationData.BakPoint = p+11;
	}

}
#endif

#if 0
void WriteOverDriverData2Flash()
{
	unsigned long p,i;
	p = pTable.OverSpeedRecord.CurPoint;

		erase4kflash(p,50);
		SPI_FLASH_BufferWrite(SPI1,(unsigned char *)overdriverdata , p, 50);

}

void WriteDriverRegData2Flash()
{
	unsigned long p,i;
	p = pTable.DriverReRecord.CurPoint;
	erase4kflash(p,50);
	SPI_FLASH_BufferWrite(SPI1,(unsigned char *)driverregisterdata , p, 50);

}
#endif

void MovePtablePtr(StructPT ptr,unsigned long movesize)
{

	    ptr.CurPoint =  ptr.CurPoint+movesize;
	    ptr.finshflag |= 0xea;
		if((ptr.CurPoint+movesize) >ptr.EndAddr)
		{
			ptr.CurPoint=ptr.BaseAddr;
			 ptr.finshflag = 0xeaea;
		}
	    WritePartitionTable(&pTable);


}
void WriteRecordData2Flash(StructPT ptbl ,unsigned char *buff,unsigned long num)
{
	unsigned long num_addr;
	unsigned long bnum_addr;
	unsigned long star_addr;
	unsigned long p;
	p = ptbl.CurPoint;
	//erase4kflash(p,num);

		if(((p&0x00000fff)+num)<0x1000)
		{
			Update4k(p,temp_4k,UpdateFlashTimes);
			SPI_FLASH_BufferWrite(SPI1,(unsigned char *)buff , p, num);
			num_addr =4096-(p&0x00000fff)-num;
			SPI_FLASH_BufferWrite(SPI1,(unsigned char *)&temp_4k[p+num] , p+num, num_addr);
		}
		else
		{
			Update4k(p,temp_4k,UpdateFlashTimes);
			num_addr = 4096-(p&0x00000fff);
			SPI_FLASH_BufferWrite(SPI1,(unsigned char *)buff , p,num_addr);
			Update4k((p+num)&0x00000fff,temp_4k,UpdateFlashTimes);
			SPI_FLASH_BufferWrite(SPI1,(unsigned char *)&buff[num_addr] , (p+num)&0x00000fff,num-num_addr);
			bnum_addr = 4096 +num_addr-num;
			star_addr = (p+num)&0x00000fff +num-num_addr;
			SPI_FLASH_BufferWrite(SPI1,(unsigned char *)&temp_4k[num-num_addr] , star_addr,bnum_addr);
		}
		MovePtablePtr(ptbl,num);

}

//*----------------------------------------------------------------------------
//* Function Name       : BaseDataHandler
//* Object              : 閿熸枻鎷烽敓鏂ゆ嫹缁嗛敓鏂ゆ嫹鑽介敓鏂ゆ嫹?閿熸枻鎷烽敓閾帮拷
//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : PowerOnTime,STATUS1min,TimeChange,InRecordCycle,curTime
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : PowerOnTime,STATUS1min,InRecordCycle
//*----------------------------------------------------------------------------
void ValueStatusHandler()
{
	unsigned long timenowmin,timedriverstartmin,timestopmin;
	//驾驶状态变化处理
	if((CurSpeed >0)&&(Datastatusdata.keepstart10s ==0))
	{
		if(Time10sCnts == 0)
		{
			Time10sCnts = 10;
			Time10sCnte = 0;
			Datastatusdata.keepstop10s = 0;
		}

	}
	else if ((CurSpeed == 0)&&(Datastatusdata.keepstop10s ==0))
	{
		if(Time10sCnte == 0)
		{
			Time10sCnte = 10;
			Time10sCnts = 0;
			Datastatusdata.keepstart10s = 0;
		}
	}
	if(Datastatusdata.keepstart10s)
	{
		if((DriverStatus & DRIVING_STAR)==0)
		{
			memcpy((unsigned char *)&startdriverclk,(unsigned char *)&curTime,strlen((unsigned char *)&curTime));
		}
		//Datastatusdata.keepstart10s = 0;
		DriverStatus |= DRIVING_STAR;//连续驾驶开始
		DriverStatus &= ~DRIVING_STOP;
		DriverStatus &= ~DRIVING_STOP_DRIVER;
		timedriverstartmin = timechange(startdriverclk);
		timenowmin = timechange(curTime);
		if((timenowmin-timedriverstartmin)>225)
		{
			DisplayMin = timenowmin-timedriverstartmin;
			if((AlarmFlag&ALARM_OVER_TIME)!= ALARM_OVER_TIME)
				AlarmFlag |= ALARM_OVER_TIME;
		}
		if((timenowmin-timedriverstartmin)>240)
		{
			DriverStatus |= DRIVING_OVERTIME;
		}
	}
	else if(Datastatusdata.keepstop10s)
	{
		if((DriverStatus & DRIVING_STOP)==0)
		{
			memcpy((unsigned char *)&enddriverclk,(unsigned char *)&curTime,strlen((unsigned char *)&curTime));
		}
		DriverStatus |= DRIVING_STOP;
		//DriverStatus &= ~DRIVING_STAR;
		timedriverstartmin = timechange(startdriverclk);
		timestopmin = timechange(curTime);
		if( (timenowmin-timestopmin)>20)//并且同一个驾驶人
		{
			DriverStatus &= ~DRIVING_STAR;
			DriverStatus |= DRIVING_STOP_DRIVER;//连续驾驶结束
			DriverStatus &= ~DRIVING_OVERTIME;
			AlarmFlag &= ~ALARM_OVER_TIME;
			Time30mincnt1 = 0;

		}
	}
	//鐤戠偣鏁版嵁鍒ゆ柇澶勭悊鐘舵�鍙樺寲
	if((Datastatusdata.locationchagestatus == 1)&&(DriverStatus & DRIVING_STAR ))//鎴栬�璁板綍浠柇鐢�
	{
		DoubltPointstatus = DBRECORD_END;
	}
	else if((DriverStatus & DRIVING_STAR ))
	{
		DoubltPointstatus = DBRECORD_START;
	}
	else
	{
		DoubltPointstatus= NONE_DB;
	}
	//鍙告満鐧昏鐘舵�鍙樺寲
	IckaHandler();

	//鏃ュ織鐘舵�杞崲
	if(CurSpeed >40 )
	{
		if(Datastatusdata.keepthespeed == 0)
		{
			Datastatusdata.keepthespeed =1;
			JournalRecordstatus = RECORD_STARTTIME;
		}
		else if((Datastatusdata.keepthespeed == 1)&&(Datastatusdata.keepspeed5min))
		{
			if(speeddel <11)
			{
				JournalRecordstatus = NORMAL;
				AlarmFlag &= ~ALARM_SPEED_ABOR;
				Time30mincnt4 = 0;
			}
			else
			{
				JournalRecordstatus = ABORT;
				AlarmFlag |= ALARM_SPEED_ABOR;
			}
		}
	}
	//速度超过限定数值
	if(CurSpeed > (Parameter.LimitSpeed-5))
	{
		if((AlarmFlag&ALARM_OVER_SPEED)!= ALARM_OVER_SPEED)
						AlarmFlag |= ALARM_OVER_SPEED;
	}
	else
	{
		AlarmFlag &= ~ALARM_OVER_SPEED;
	}


}
void AlarmHandler()
{
	if(((AlarmFlag&ALARM_OVER_TIME)== ALARM_OVER_TIME)&&(Time30mincnt1 ==0))
	{
		Time30mincnt1 = 6;
	}
	if(((AlarmFlag&ALARM_NOT_RE)== ALARM_NOT_RE)&&(Time30mincnt2 ==0))
	{
		Time30mincnt2 = 6;
	}
	if(((AlarmFlag&ALARM_OVER_SPEED)== ALARM_OVER_SPEED)&&(Time30mincnt3 ==0))
	{
		Time30mincnt3 = 6;
	}
	if(((AlarmFlag&ALARM_SPEED_ABOR)== ALARM_SPEED_ABOR)&&(Time30mincnt4 ==0))
	{
		Time30mincnt4 = 6;
	}

}

void BaseDataHandler()
{
	int i,pt;
	if(DriverStatus & DRIVING_STAR )
	{

		//STATUS1min = 0;//2003.10.23,panhui
		//STATUS1min |= CurStatus;
		if(TimeChange & (0x01<<SECOND_CHANGE))//閿熸枻鎷蜂竴閿熸枻鎷烽敓鏂ゆ嫹
		{
			TimeChange &= ~(0x01<<SECOND_CHANGE);
			if (curTime.second == 0)
			{

				if(( InRecordCycle&(1<<BASEDATA))==BASEDATA)
				{
					//basedata.speed[59] = (unsigned char)Curspeed1s;
					basedata.status[59] = (unsigned char)CurStatus;
					WriteRecordData2Flash(pTable.BaseData,(unsigned char *)(&basedata),sizeof(BaseDataBlock));
				}
				basedata.basedataclk.year = curTime.year;
				basedata.basedataclk.month = curTime.month;
				basedata.basedataclk.day = curTime.day;
				basedata.basedataclk.hour = curTime.hour;
				basedata.basedataclk.minute = curTime.minute;
				basedata.basedataclk.second = curTime.second;
				InRecordCycle |= 1<<BASEDATA;
			}
			else
			{
				//basedata.speed[curTime.second-1] = (unsigned char)Curspeed1s;
				basedata.status[curTime.second-1] = (unsigned char)CurStatus;

			}

			//STATUS1min = 0;
		}
	}
	else
	{
		if(( InRecordCycle&(1<<BASEDATA))==BASEDATA)
		{
			if(TimeChange & (0x01<<SECOND_CHANGE))
			{
				TimeChange &= ~(0x01<<SECOND_CHANGE);
				if (curTime.second != 0)
				{
					basedata.speed[curTime.second] = 0xff;
					basedata.status[curTime.second] = 0xff;
				}
				else
				{
					WriteRecordData2Flash(pTable.BaseData,(unsigned char *)(&basedata),sizeof(BaseDataBlock));
					InRecordCycle &= ~(1<<BASEDATA);
				}

			}

		}

	}
	
}

void LocationHandler()
{
	int i,pt;
	if((DriverStatus & DRIVING_STAR ))
	{
		if(TimeChange & (0x01<<MINUTE_CHANGE))//閿熸枻鎷蜂竴閿熸枻鎷烽敓鏂ゆ嫹
		{
			TimeChange &= ~(0x01<<MINUTE_CHANGE);
			if (curTime.minute == 0)
			{

				if(( InRecordCycle&(1<<LOCATIONDATA))==LOCATIONDATA)
				{
					for(i = 0;i<10;i++)
					{
						*((unsigned char *)&Locationdata.Lsizedata[59]+i)=*((unsigned char *)&location+i);
					}
					Locationdata.speed1min[59]= Curspeed1min;
					WriteRecordData2Flash(pTable.LocationData,(unsigned char *)(&Locationdata),sizeof(LocationBlock));
				}
				Locationdata.lclk.year = curTime.year;
				Locationdata.lclk.month = curTime.month;
				Locationdata.lclk.day = curTime.day;
				Locationdata.lclk.hour = curTime.hour;
				Locationdata.lclk.minute = curTime.minute;
				Locationdata.lclk.second = curTime.second;
				InRecordCycle |= 1<<LOCATIONDATA;

			}


			else
			{
				for(i = 0;i<10;i++)
				{
					*((unsigned char *)&Locationdata.Lsizedata[curTime.minute-1]+i)=*((unsigned char *)&location+i);
				}
				Locationdata.speed1min[curTime.minute-1]= Curspeed1min;
			}

		}
	}
	else
	{
		if(( InRecordCycle&(1<<LOCATIONDATA))==LOCATIONDATA)
		{
			if(TimeChange & (0x01<<MINUTE_CHANGE))
			{
				TimeChange &= ~(0x01<<MINUTE_CHANGE);
				if (curTime.minute != 0)
				{
					for(i = 0;i<10;i++)
					{
						*((unsigned char *)&Locationdata.Lsizedata[curTime.minute-1]+i) =0xff;
					}
					Locationdata.speed1min[curTime.minute-1]= 0x7f;
				}
				else
				{
					WriteRecordData2Flash(pTable.LocationData,(unsigned char *)(&Locationdata),sizeof(LocationBlock));
					InRecordCycle &= ~(1<<BASEDATA);
				}


			}

		}

	}

}
void OverDriverHandler()
{
	int i,pt;
	if((DriverStatus & DRIVING_OVERTIME ))
	{
		overdriverdata.startdrivertime.year =startdriverclk.year;
		overdriverdata.startdrivertime.month =startdriverclk.month;
		overdriverdata.startdrivertime.day =startdriverclk.day;
		overdriverdata.startdrivertime.hour =startdriverclk.hour;
		overdriverdata.startdrivertime.minute =startdriverclk.minute;
		overdriverdata.startdrivertime.second =startdriverclk.second;

	    if (DriverStatus & DRIVING_STOP_DRIVER )
		{
			overdriverdata.enddrivertime.year =enddriverclk.year;
			overdriverdata.enddrivertime.month =enddriverclk.month;
			overdriverdata.enddrivertime.day =enddriverclk.day;
			overdriverdata.enddrivertime.hour =enddriverclk.hour;
			overdriverdata.enddrivertime.minute =enddriverclk.minute;
			overdriverdata.enddrivertime.second =enddriverclk.second;
			WriteRecordData2Flash(pTable.OverSpeedRecord,(unsigned char *)&overdriverdata,50);//write the data
		}

	}
}

//*----------------------------------------------------------------------------
//* Function Name       : RunRecord360Handler
//* Object              :360灏忔椂骞抽敓鏂ゆ嫹閿熷姭璁规嫹閿熸枻鎷烽┒閿熸枻鎷峰綍閿熸枻鎷风粺閿熸枻鎷风柌閿熼叺纭锋嫹椹�
//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : CurSpeed,curTime
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
//0.2绉掕窇涓�亶
void DoubltPointHandler()
{
	unsigned char i,cnt;
	static unsigned DBcnt = 0;
	unsigned char *ptr ;
	switch(DoubltPointstatus)
	{
		case DBRECORD_START:
			ddb.data[DBcnt].speed = CurSpeed;
			ddb.data[DBcnt].status = CurStatus;
			DBcnt++;
			if(DBcnt == 100)
				DBcnt = 0;
			break;
		case DBRECORD_END:
			for(i= 0;i++;i<100)
			{
				cnt = DBcnt+i;
				if(cnt>99)
				{
					cnt = cnt-100;
				}
				ddb.data[i].speed= ddb.data[cnt].speed;
				ddb.data[i].status= ddb.data[cnt].status;
			}
			ptr = (unsigned char *)&ddb.StopTime;
			for(i = 0;i++;i<6)
			{
				ptr[i] = (unsigned char )*((unsigned char * )(&curTime+i));
			}
			ptr = (unsigned char *)&ddb.vaildlocation;
			for(i = 0;i++;i<10)
			{
				ptr[i]= (unsigned char )*((unsigned char * )&location+i);
			}
			WriteRecordData2Flash(pTable.DoubtPointData,(unsigned char *)&ddb,234);
		break;
		default:
			break;
	}
}
void DrvierRegisterHandler()
{
	unsigned char i;
	unsigned char *ptr;
		if((DriverRegstatus & DRIVER_REG_IN)||(DriverRegstatus & DRIVER_REG_OUT))
		{
			ptr = (unsigned char *)&driverregisterdata.happentime;
			for(i=0;i++;i<18)
			{
				driverregisterdata.DriverLisenseCode[i] = CurrentDriver.DriverLisenseCode[i];
			}
			if(DriverRegstatus &DRIVER_REG_IN)
			{

				for(i=0;i++;i<6)
				{
					ptr[i] = (unsigned char )*((unsigned char * )(&startdriverclk+i));
				}
				driverregisterdata.registerstatus = 0x01;
			}
			else
			{
				for(i=0;i++;i<6)
				{
					ptr[i] = (unsigned char )*((unsigned char * )&enddriverclk+i);
				}
				driverregisterdata.registerstatus = 0x02;

			}
			WriteRecordData2Flash(pTable.DriverReRecord,(unsigned char *)&driverregisterdata,25);
		}
}

void PowerHandle()
{
	unsigned char i;
	unsigned char *ptr = (unsigned char *)&powerdata.powertime;

	if((powerstatus & POWER_ON)||(DriverRegstatus & POWER_OFF))
	{
		for(i=0;i++;i<6)
		{
			ptr[i] = (unsigned char )*((unsigned char * )(&curTime+i));
		}
		powerdata.powerstatus = paramodifystatus;
		WriteRecordData2Flash(pTable.PowerOffRunRecord,(unsigned char *)&modifydata,7);
	}

}

void ModifyHandle()
{
	unsigned char i;
	unsigned char *ptr = (unsigned char *)&modifydata.modifytime;
	if(paramodifystatus)
	{
		for(i=0;i++;i<6)
		{
			ptr[i] = (unsigned char )*((unsigned char * )(&curTime+i));
		}
		modifydata.modifystatus = powerstatus;
		WriteRecordData2Flash(pTable.ModifyRecord,(unsigned char *)&modifydata,7);
	}

}

//姝ゅ嚱鏁颁竴绉掗挓鎵ц涓�
void JournalHandle()
{
	unsigned char i;
	static unsigned JNcnt;
	unsigned char *ptr;
	switch(JournalRecordstatus)
	{
		case NORMAL://璁板綍姝ｅ父鏁版嵁
		case ABORT://璁板綍寮傚父鏁版嵁
			journaldata.speedstatus = JournalRecordstatus;
			JournalRecordstatus = NONE_OPR;
			ptr = (unsigned char *)&journaldata.jouranlendtime;
			for(i = 0;i++;i<6)
			{
				ptr[i]= (unsigned char )*((unsigned char * )(&curTime+i));
			}
			WriteRecordData2Flash(pTable.journalRecord,(unsigned char *)&journaldata,133);
		break;
		case RECORD_STARTTIME://婊¤冻閫熷害>40,骞朵笖寮傚父涓庢甯镐氦鏇�
			ptr = (unsigned char *)&journaldata.journalstartime;
			for(i = 0;i++;i<6)
			{
				ptr[i] = (unsigned char )*((unsigned char * )(&curTime+i));
			}
			journaldata.rspeed[0] = RsSpeed;
			journaldata.speed[0]  = CurSpeed;
			JNcnt = 1;
			JournalRecordstatus = RECORD_DATA;
			break;
		case RECORD_DATA://婊¤冻閫熷害>40,骞朵笖璇樊鐜囬檺鏁板�淇濇寔鍦ㄤ竴涓按骞�
			journaldata.rspeed[JNcnt] = RsSpeed;
			journaldata.speed[JNcnt]  = CurSpeed;
			if(JNcnt== 59)
			{
				JNcnt = 0;
				JournalRecordstatus = NONE_OPR;
			}
			break;
		default:
			break;
	}

}
//*----------------------------------------------------------------------------
//* Function Name       : FinishAllRecord
//* Object              : 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙》鎷峰綍
//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : PowerOnTime,STATUS1min,TimeChange,InRecordCycle,curTime
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : PowerOnTime,STATUS1min,InRecordCycle
//*----------------------------------------------------------------------------
void FinishAllRecord()
{
	if(FinishFlag)
	{
		//閿熸枻鎷烽┒閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熷彨杈炬嫹閿熺禌ataflash
		WriteBaseDataToFlash((unsigned short *)(&rec_end),(sizeof(RecordData_end))/2,END);
		RecordDriver = CurrentDriver;//閿熻闈╂嫹閿熸枻鎷峰綍鍙搁敓鏂ゆ嫹
		FinishFlag = 0;
	}
}
//*----------------------------------------------------------------------------
//* Function Name       : GetOTDRDataFromFlash
//* Object              : 閿熸帴闈╂嫹鐗￠敓琛楀嚖鎷烽敓楗侯摨nc閿熸枻鎷烽敓楗虹鎷风柌閿熼叺纭锋嫹椹堕敓鏂ゆ嫹閿燂拷
//*                       閿熸枻鎷烽敓鏂ゆ嫹璇洪敓绱筓F閿熸枻鎷�
//* Input Parameters    : p閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风墶閿熻锟�//*                       inc閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯帺閿熸枻鎷烽緥閿熸枻鎷烽敓鎹峰嚖鎷烽敓楗哄嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎹风殑鍑ゆ嫹閿熸枻鎷�
//*                       娉ㄩ敓瑙ｏ細inc閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹閿熸枻鎷烽敓鑴氱》鎷烽敓锟�//*                       buf閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
void GetOTDRDataFromFlash(unsigned short *p, int inc,unsigned char *buf)
{
	int  i;
	unsigned short data;

	if(inc==0)
		return;

}
//*----------------------------------------------------------------------------
//* Function Name       : IsCorrectCLOCK
//* Object              : 閿熷彨璁规嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹娆犻敓鏂ゆ嫹閿熼ズ锟�//* Input Parameters    : 鏃堕敓鏂ゆ嫹鎸囬敓鏂ゆ嫹
//* Output Parameters   : 閿熻鍑ゆ嫹閿熸枻鎷蜂竴閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int IsCorrectCLOCK(CLOCK *dt)
{
	unsigned char data;
	data = BCD2Char(dt->year);
	if(data>99)
		return(0);
	data = BCD2Char(dt->month);
	if((data>12)||(data<=0))
		return(0);
	data = BCD2Char(dt->day);
	if((data>31)||(data<=0))
		return(0);
	data = BCD2Char(dt->hour);
	if(data>23)
		return(0);
	data = BCD2Char(dt->minute);
	if(data>59)
		return(0);
	data = BCD2Char(dt->second);
	if(data>59)
		return(0);
		
	return (1);
}
//*----------------------------------------------------------------------------
//* Function Name       : IsCorrectClock
//* Object              : 閿熷彨璁规嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹娆犻敓鏂ゆ嫹閿熼ズ锟�//* Input Parameters    : 鏃堕敓鏂ゆ嫹鎸囬敓鏂ゆ嫹
//* Output Parameters   : 閿熻鍑ゆ嫹閿熸枻鎷蜂竴閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓锟�//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int IsCorrectClock(Record_CLOCK *dt)
{
	unsigned char data;
	data = BCD2Char(dt->year);
	if(data>99)
		return(0);
	data = BCD2Char(dt->month);
	if((data>12)||(data<=0))
		return(0);
	data = BCD2Char(dt->day);
	if((data>31)||(data<=0))
		return(0);
	data = BCD2Char(dt->hour);
	if(data>23)
		return(0);
	data = BCD2Char(dt->minute);
	if(data>59)
		return(0);
//	data = BCD2Char(dt->second);
//	if(data>59)
//		return(0);
		
	return (1);
}
//*----------------------------------------------------------------------------
//* Function Name       : GetOTDR
//* Object              : 閿熸枻鎷峰彇閿熸枻鎷峰墠閿熸枻鎷风柌閿熼叺纭锋嫹椹堕敓鏂ゆ嫹褰�
//* Input Parameters    : p閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰墠鎸囬敓鏂ゆ嫹;
//*                       s閿熸枻鎷烽敓鏂ゆ嫹鐤查敓閰电》鎷烽┒閿熸枻鎷峰綍閿熸枻鎷峰閿熸枻鎷烽敓锟�
//*                       e閿熸枻鎷烽敓鏂ゆ嫹鐤查敓閰电》鎷烽┒閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
//* Output Parameters   : 閿熻鍑ゆ嫹閿熸枻鎷蜂竴閿熸枻鎷风柌閿熼叺纭锋嫹椹堕敓鏂ゆ嫹褰�
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     :
//*----------------------------------------------------------------------------
int GetOTDR( unsigned long p, OTDR_start *s, OTDR_end *e )
{
	int offset;
	offset = 0 - sizeof(OTDR_end);
	GetOTDRDataFromFlash((unsigned short *)p, offset,(unsigned char *)e);
	if(e->dt.type!=0xeaea)
		return (0);
	if(!IsCorrectClock(&(e->dt)))
		return (0);
	
	StructPT temp;
	temp.CurPoint = (unsigned long)p;
	offset = 0 - (sizeof(OTDR_start)+sizeof(OTDR_end)+e->MinuteNb);
	p = AddPointer(&temp, offset);
	offset = sizeof(OTDR_start);
	GetOTDRDataFromFlash((unsigned short *)p, offset,(unsigned char *)s);
	if(s->dt.type!=0xafaf)
		return (0);
	if(!IsCorrectClock(&(s->dt)))
		return (0);

	return(1);
	
}
//*----------------------------------------------------------------------------
//* Function Name       : ComputeDistance100m
//* Object              : 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼叺绛规嫹閿熸枻鎷风郴閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂綅涓洪敓鏂ゆ嫹閿熸枻鎷�
//* Input Parameters    : pulseNb閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//* Output Parameters   : 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�-5804009閿熸枻鎷�
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//*----------------------------------------------------------------------------
unsigned long ComputeDistance100m(unsigned long pulseNb)
{
	unsigned long result;
	unsigned char PP =8;
	result = (pulseNb/(PP*Parameter.PulseCoff/10));
	return result;
}
//*----------------------------------------------------------------------------
//* Function Name       : CompareDateTime
//* Object              : 閿熼ズ鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹鎷囬敓鍙拷
//* Input Parameters    : dt1閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹1閿熸枻鎷穌t2閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹2
//* Output Parameters   : 1 閿熸枻鎷烽敓鏂ゆ嫹 dt1 > dt2;
//                       -1 閿熸枻鎷烽敓鏂ゆ嫹 dt1 < dt2;
//                        0 閿熸枻鎷烽敓鏂ゆ嫹 dt1 = dt2;
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//*----------------------------------------------------------------------------
int CompareDateTime(Record_CLOCK dt1,Record_CLOCK dt2)
{
	int res1,res2;
	res1 = IsCorrectClock(&dt1);
	res2 = IsCorrectClock(&dt2);
	if((res1==0)&&(res2==0))
		return 0;
	else if((res1==1)&&(res2==0))
		return 1;
	else if((res1==0)&&(res2==1))
		return -1;

	if(dt1.year > dt2.year)
		return 1;
	if(dt1.year < dt2.year)
		return -1;
		
	//dt1.yea == dt2.year	
	if(dt1.month >  dt2.month)
		return 1;
	if(dt1.month < dt2.month)
		return -1;
	
	//dt1.month == dt2.month
	if(dt1.day > dt2.day)
		return 1;
	if(dt1.day < dt2.day)
		return -1;
	
	//dt1.day == dt2.day
	if(dt1.hour > dt2.hour)
		return 1;
	if(dt1.hour < dt2.hour)
		return -1;

	//dt1.hour == dt2.hour
	if(dt1.minute > dt2.minute)
		return 1;
	if(dt1.minute < dt2.minute)
		return -1;

	//dt1.minute == dt2.minute
	if(dt1.second > dt2.second)
		return 1;
	if(dt1.second < dt2.second)
		return -1;
		
	//dt1.second == dt2.second
	return 0;
}
//*----------------------------------------------------------------------------
//* Function Name       : DataPointerSeek
//* Object              : 閿熸枻鎷烽敓琛楅潻鎷锋數銊庯拷
//* Input Parameters    : none
//* Output Parameters   : none
//* 閿熸枻鎷烽敓鐭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//* 閿熺潾鏀圭鎷峰叏閿熻鎲嬫嫹閿熸枻鎷�     : none
//*----------------------------------------------------------------------------
void DataPointerSeek()
{
	unsigned long DataPoint;
	unsigned short data;
	unsigned char *p;
	unsigned char temp,f1,f2;
	unsigned char update = 0;
	int i;
	DataPoint = pTable.BaseData.CurPoint;
	p = (unsigned char *)DataPoint;
	//閿熸枻鎷烽敓鎻鎷峰墠鎸囬敓鏂ゆ嫹浣嶉敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸枻鎷疯瘨閿熻锟�
	for(i=0;i<RecordFlagByte;i++)
	{
		if(*p!=0xff)
			break;
		if((unsigned long)p==pTable.BaseData.EndAddr)
			p=(unsigned char *)pTable.BaseData.BaseAddr;
		else
			p++;
	}
	if(i==RecordFlagByte)//閿熸枻鎷峰織閿熸枻鎷烽敓鑺傦綇鎷烽敓鏂ゆ嫹閿熸枻鎷锋寚閿熻瀹氫綅閿熸枻鎷�
		return;
		
	//閿熸枻鎷烽敓閾拌鎷蜂綅閿熸枻鎷烽敓鏂ゆ嫹鑾搁敓琛楅潻鎷烽敓锟�
	do
	{
		if((*p == 0xff)&&(((unsigned long)p&1)==0))
		{//閿熸枻鎷烽敓鏂ゆ嫹閿熸彮纰夋嫹閿熸枻鎷峰織
			i=0;
			do
			{
				if((unsigned long)p == pTable.BaseData.EndAddr)
					p = (unsigned char *)pTable.BaseData.BaseAddr;
				else
					p++;
				i++;
			}while((*p==0xff)&&(i<RecordFlagByte));
			if(i == RecordFlagByte)//閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹鎸囬敓鏂ゆ嫹浣嶉敓鏂ゆ嫹
			{
				//閿熶粙鐪嬮敓鏂ゆ嫹蹇椾箣鍓嶉敓瑙掑嚖鎷烽敓鍙枻鎷烽敓鏂ゆ嫹閿熻缍欵AE
				//1)閿熷彨璁规嫹閿熻鍑ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓敓鍙揪鎷烽敓绲榚ae閿熸枻鎷峰織閿熶茎鍑ゆ嫹閿熸枻鎷风郴閿熸枻鎷峰綍
				DataPoint = (unsigned long)p;
				DataPoint -= (RecordFlagByte+2);
				if(DataPoint<pTable.BaseData.BaseAddr)
					DataPoint = pTable.BaseData.EndAddr - (pTable.BaseData.BaseAddr - DataPoint)+1;
				f1 = *((unsigned char *)DataPoint);
				f2 = *((unsigned char *)(DataPoint+1));
				if((f1==0xae)&&(f2==0xae))
				{//閿熸枻鎷烽敓鏂ゆ嫹绯婚敓鏂ゆ嫹褰曢敓鏂ゆ嫹閿熸枻鎷锋寚閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
					pTable.BaseData.CurPoint = (unsigned long)p;
					pTable.BaseData.CurPoint = AddPointer(&(pTable.BaseData),-2);
					update = 1;
					break;
				}
				
				//2)閿熷彨璁规嫹閿熻鍑ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻緝纰夋嫹閿熼摪锟�
				DataPoint = (unsigned long)p;
				DataPoint -= RecordFlagByte*2;
				if(DataPoint<pTable.BaseData.BaseAddr)
					DataPoint = pTable.BaseData.EndAddr - (pTable.BaseData.BaseAddr - DataPoint)+1;
				f1 = *((unsigned char *)DataPoint);
				f2 = *((unsigned char *)(DataPoint+1));
					
				//閿熸枻鎷烽敓鏂ゆ嫹閿熻鎾呮嫹閿熺粸鍢変紮鎷烽敓锟�				/////////*******2003.10.06 panhui*********////////
				DataPoint = (unsigned long)p;
				DataPoint -= RecordFlagByte;
				if(DataPoint<pTable.BaseData.BaseAddr)
					DataPoint = pTable.BaseData.EndAddr - (pTable.BaseData.BaseAddr - DataPoint)+1;
				if((f1==0xae)&&(f2==0xae))//閿熸枻鎷烽敓閾扮》鎷烽敓鏂ゆ嫹閿熸嵎锝忔嫹閿熸枻鎷锋寚閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
					pTable.BaseData.CurPoint = DataPoint;
				else
				{//閿熸枻鎷烽敓鏂ゆ嫹绯婚敓锟芥湭閿熸枻鎷烽敓鐭》鎷峰啓閿熸枻鎷烽敓鏂ゆ嫹閿熻缍欵AE
					//鍐欓敓鏂ゆ嫹钀侀敓鏂ゆ嫹閿熸枻鎷峰織
					data = 0xaeae;
					SPI_FLASH_BufferWrite(SPI1,(u8 *)(&data),(unsigned long )DataPoint,2);
					//閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿燂拷
					pTable.BaseData.CurPoint = (unsigned long)p;
				}
				/////////*******2003.10.06 panhui*********////////
				update = 1;
				break;
			}
		}
		else
		{//鏈敓鎻鎷烽敓鏂ゆ嫹蹇楅敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺嫛璁规嫹鎸囬敓鏂ゆ嫹
			if((unsigned long)p == pTable.BaseData.EndAddr)
				p = (unsigned char *)pTable.BaseData.BaseAddr;
			else
				p++;
		}
	}while((unsigned long)p != DataPoint);
	
	if(update)
	{
		//閿熸枻鎷烽敓閾拌鎷蜂綅閿熺即纰夋嫹閿熸枻鎷疯幐閿熻闈╂嫹閿燂拷
		Record_CLOCK LastDT,CurDT;
		unsigned char *dp;
		DataPoint = pTable.DoubtPointData.CurPoint;
		int result;
		
		if(DataPoint == pTable.DoubtPointData.BaseAddr)
			dp = (unsigned char *)(DataPoint+96*210+4);
		else
			dp = (unsigned char *)(DataPoint-210+4);
		LastDT.year = *dp;dp++;
		LastDT.month = *dp;dp++;
		LastDT.day = *dp;dp++;
		LastDT.hour = *dp;dp++;
		LastDT.minute = *dp;dp++;
		LastDT.second = *dp;
		do{
		
			dp = (unsigned char *)(DataPoint+4);
			CurDT.year = *dp;dp++;
			CurDT.month = *dp;dp++;
			CurDT.day = *dp;dp++;
			CurDT.hour = *dp;dp++;
			CurDT.minute = *dp;dp++;
			CurDT.second = *dp;dp++;
	
			result = CompareDateTime( CurDT, LastDT);
			if(result!=1)
				break;
			DataPoint += 210;
			//////////modified by panhui 2003.10.20////////////
			if(DataPoint > (pTable.DoubtPointData.EndAddr-110))
				DataPoint = pTable.DoubtPointData.BaseAddr;
			//////////閿熸埅鐨囩鎷�鍙伴敓鏂ゆ嫹//////////////////////////////
			LastDT = CurDT;
				
		}while(DataPoint != pTable.DoubtPointData.CurPoint);	
		
		pTable.DoubtPointData.CurPoint = DataPoint;
		
		WritePartitionTable(&pTable);
	}
}



