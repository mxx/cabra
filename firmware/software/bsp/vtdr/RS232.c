//*----------------------------------------------------------------------------
//* File Name           : RS232.c
//* Object              : C program of RS232 communication
//*----------------------------------------------------------------------------

#include <stm32f10x_usart.h>
#include "atmel_dataflash.h"
#include "serial.h"
#include    "RS232.h"
#include    "lcd.h"
#include    "menu.h"
#include    "application.h"
#include    "DataManager.h"
#include <rtthread.h>
#define CMDLENGTH 0x20
#define FALSE 0
#define TRUE 1

CMD_VER Verificationstatus;
unsigned char RSCmdrxBuf[CMDLENGTH];
unsigned char RSCmdtxBuf[CMDLENGTH];
unsigned char RSDatarxBuf[DataLength];
unsigned char LargeDataBuffer[32*1024];//[360*65];
unsigned long DataLengthReceived;
unsigned char CheckSum;
unsigned char SendCheckSum;
unsigned char Schedule_Result = 0;
unsigned char CloseUSART1Time = 0xff;
extern struct rt_device uart2_device;
extern unsigned long AddPointer(StructPT *pt, int inc);
extern int WriteParameterTable(StructPara *para);
extern int InitializeTable(unsigned char parti,unsigned char para,unsigned char change_set);
extern void GetOTDRDataFromFlash(unsigned short *p, int inc,unsigned char *buf);
extern int WritePartitionTable(PartitionTable *ptt);
extern int IsCorrectCLOCK(CLOCK *dt);
//unsigned char SetTimeFlag = 0;

extern unsigned short CurSpeed;
extern unsigned char CurStatus;
extern Timeflag timeflag;
extern unsigned char BCD2Char(unsigned char bcd);
extern unsigned char GetOverTimeRecordIn2Days(OTDR *record);
extern unsigned char Time6sCnt;
extern CLOCK curTime;
extern PartitionTable pTable;
extern StructPara Parameter;
extern unsigned long PulseTotalNumber;
extern unsigned char InRecordCycle;		//�Ƿ��ڼ�¼��ݹ����
extern unsigned char InFlashWriting;	//��FLASHд������
extern unsigned char FinishFlag;
unsigned char *ep2_bufr;


//*----------------------------------------------------------------------------
//* Function Name            : Int2BCD
//* Object              	 : ʮ������ת��ΪBCD��
//* Input Parameters    	 : ch������ת����ʮ������
//* Output Parameters   	 : ת�����BCD��
//* Global  Variable Quoted  : none
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
void Int2BCD(unsigned long ch, unsigned char *buf)
{
	unsigned char d0,d1;
	unsigned long x;
	int i;
	x=ch;
	for(i=2;i>=0;i--)
	{
		d0=x%10;
		x=x/10;
		d1=x%10;
		x=x/10;
		buf[i]=(d1<<4)+d0;
		
	}

}
//*----------------------------------------------------------------------------
//* Function Name            : Char2BCD
//* Object              	 : ʮ������ת��ΪBCD��
//* Input Parameters    	 : ch������ת����ʮ������
//* Output Parameters   	 : ת�����BCD��
//* Global  Variable Quoted  : none
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
unsigned char Char2BCD(unsigned char ch)
{
	unsigned char bcd,d0,d1;
	d0 = ch%10;
	d1 = (ch/10)<<4;
	bcd = d1+d0;

	return(bcd);
}
//*----------------------------------------------------------------------------
//* ������IncreaseTime
//* ���ܣ�����ʱ�������һ���������������������ɸ���
//* ���룺dt���� ����ʱ�����
//*     ��inc��������(�Է��Ӽƣ�����ɸ�)
//* ������ѱ��޸ĵ�����ʱ�����ָ��ָ�����ݣ�dtָ�����ݣ�
//* ���õ�ȫ�ֱ���: none
//* �޸ĵ�ȫ�ֱ���: none
//*----------------------------------------------------------------------------
void IncreaseTime(DateTime *dt, int inc)
{
	int t=dt->time+inc;
	int d=dt->day;
	int m=dt->month;
	int y=dt->year;
	do{
		if(t>=24*60)
		{
			d++;
			t=t-24*60;
			switch(m)
			{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				if(d>31){
					m++;
					d=d-31;
				}
				break;
			case 2:
				if((d>29)&&(y%4==0)){
					m++;
					d=d-29;
				}
				else if ((d>28)&&(y%4!=0)){
					m++;
					d=d-28;
				}
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				if(d>30){
					m++;
					d=d-30;
				}
				break;
			}
			if(m>12){
				y++;
				m=m-12;
			}
		}
		else if(t<0)
		{
			d--;
			t=t+24*60;
			switch(m)
			{
			case 2:
			case 4:
			case 6:
			case 8:
			case 9:
			case 11:
			case 1:
				if(d<1){
					m--;
					d=d+31;
				}
				break;
			case 3:
				if((d<1)&&(y%4==0)){
					m--;
					d=d+29;
				}
				else if ((d<1)&&(y%4!=0)){
					m--;
					d=d+28;
				}
				break;
			case 5:
			case 7:
			case 10:
			case 12:
				if(d<1){
					m--;
					d=d+30;
				}
				break;
			}
			if(m<1){
				y--;
				m=m+12;
			}
		}

	}while((t>=24*60)||(t<0));
	dt->time=t;
	dt->day=d;
	dt->month=m;
	dt->year=y;
}
//*----------------------------------------------------------------------------
//* ������HaveTime
//* ���ܣ�������ʱ��֮�������ٷ���
//* ���룺bigtime�������������ʱ���нϴ��һ�����Ǻ���ĵ�һ������,
//*     ��smalltime�������������ʱ���н�С��һ�����Ǻ���ĵڶ�������
//* ����������(bigtime>smalltime)���bigtime��smalltime֮�����ķ�����
//*     : ���bigtime<smalltime,�����"-1"
//* ���õ�ȫ�ֱ���: none
//* �޸ĵ�ȫ�ֱ���: none
//*----------------------------------------------------------------------------
int HaveTime(DateTime bigtime,DateTime smalltime)
{
	int result = 0;

	if(bigtime.year < smalltime.year)
		return -1;
	else if(bigtime.year==smalltime.year)
	{
		if(bigtime.month < smalltime.month)
			return -1;
		else if(bigtime.month == smalltime.month)
		{
			if(bigtime.day < smalltime.day)
				return -1;
			else if(bigtime.day == smalltime.day)
			{
				if(bigtime.time < smalltime.time)
					return -1;
			}
		}
	}
	int timesmall = smalltime.time;
	smalltime.time = 0;
	while((bigtime.year != smalltime.year) || (bigtime.month != smalltime.month) || (bigtime.day != smalltime.day))
	{
		IncreaseTime(&smalltime,24*60);
		bigtime.time = bigtime.time+24*60;
	}

	result = bigtime.time-timesmall;
	return result;
}


//*----------------------------------------------------------------------------
//* Function Name            : PrepareTime
//* Object                   : ��BCD����ʽ��ʱ����ݣ�������ʱ�֣�ת����
//*                          : ʮ������ʽ�����
//* Input Parameters         : bcd_time����ָ����BCD����ʽ��ʱ����ݵ������׵�ַ��ָ��
//*                          : dt����ָ����ʮ������ʽʱ����ݵĽṹ��ָ��
//* Output Parameters        : none
//* Global  Variable Quoted  : none
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
void PrepareTime(unsigned char *bcd_time,DateTime *dt)
{

	dt->year = BCD2Char(bcd_time[0]);
	dt->month = BCD2Char(bcd_time[1]);
	dt->day = BCD2Char(bcd_time[2]);
	dt->time = BCD2Char(bcd_time[3])*60+BCD2Char(bcd_time[4]);
}

//*----------------------------------------------------------------------------
//* Function Name            : WriteDataTxTime
//* Object                   : ��232�ӿ�дʵʱʱ�䣨��������ʵʱʱ�䡢360Сʱ�ۼ���ʻ��̺�2���������ۼ���ʻ��̣�
//* Input Parameters         : cmd���������֣���������Ǻ����
//*                          : ��Ϊ0x02(����ʵʱʱ��),�����ء�������ʱ���롱
//*                          : ������0x02�������ء�������ʱ�֡�����Ӧ360Сʱ�ۼ���ʻ��̺�2���������ۼ���ʻ��̣�
//* Output Parameters        : none
//* Global  Variable Quoted  : curTime����ʵʱʱ�䣬�������н���д��232�ӿ�
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
void WriteDataTxTime(void)
{
	//����ʵʱʱ����ݿ�(BCD��
	rt_device_write(&uart2_device, 0, &curTime.year, 1);
	SendCheckSum = SendCheckSum^(curTime.year);

	rt_device_write(&uart2_device, 0, &curTime.month, 1);
	SendCheckSum = SendCheckSum^(curTime.month);

	rt_device_write(&uart2_device, 0, &curTime.day, 1);
	SendCheckSum = SendCheckSum^(curTime.day);

	rt_device_write(&uart2_device, 0, &curTime.hour, 1);
	SendCheckSum = SendCheckSum^(curTime.hour);

	rt_device_write(&uart2_device, 0, &curTime.minute, 1);
	SendCheckSum = SendCheckSum^(curTime.minute);
		rt_device_write(&uart2_device, 0, &curTime.second, 1);
		SendCheckSum = SendCheckSum^(curTime.second);
}
void UpLoad_DriverDistance()
{
	unsigned long i;
	StructPara *para = &Parameter;;
	PartitionTable *table = &pTable;
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x03;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x14;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x03^0x00^0x14^0x00;

	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

	WriteDataTxTime();

		rt_device_write(&uart2_device, 0, (const void *)(&(para->InstallTime) ), 6);
		rt_device_write(&uart2_device, 0, (const void *)(&(para->StarDistance) ), 4);
		rt_device_write(&uart2_device, 0, (const void *)(&(para->DriverDistace) ), 4);

	//	at91_usart_write(RS232,(u_char)(>> ((2-i)*8)));
		SendCheckSum = SendCheckSum^((unsigned char)((para->InstallTime.year) ))^((unsigned char)((para->InstallTime.month) ));
		SendCheckSum = SendCheckSum^((unsigned char)((para->InstallTime.day) ))^((unsigned char)((para->InstallTime.hour) ));
		SendCheckSum = SendCheckSum^((unsigned char)((para->InstallTime.minute) ))^((unsigned char)((para->InstallTime.second) ));
		for(i=0;i<4;i++)
		{
			SendCheckSum = SendCheckSum^((unsigned char)(((para->DriverDistace)>>(i*8)) ));
			SendCheckSum = SendCheckSum^((unsigned char)(((para->StarDistance)>>(i*8)) ));
		}


	//·¢ËÍÐ£ÑéºÍ
		rt_device_write(&uart2_device, 0, &SendCheckSum, 1);

	Modify_LastUploadTime();

}
void UpLoad_DriverAutoInfo()
{
	unsigned long i;
	StructPara *para = &Parameter;;
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x05;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x29;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x05^0x00^0x29^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	rt_device_write(&uart2_device, 0, (const void *)(&(para->AutoInfodata) ), 41);
	for(i = 0;i<41;i++)
	{
		SendCheckSum = SendCheckSum^((unsigned char) *((unsigned char *)(&(para->AutoInfodata)+i)));
	}
	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);

Modify_LastUploadTime();
}
void UpLoad_StatusInfo()
{
	unsigned long i;
	StructPara *para = &Parameter;
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x06;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x57;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x06^0x00^0x57^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

	WriteDataTxTime();
	rt_device_write(&uart2_device, 0, (const void *)(&(para->signalstatus) ), 1);
	SendCheckSum = SendCheckSum^(para->signalstatus);
	rt_device_write(&uart2_device, 0, (const void *)(&(para->singalname) ), 80);
	for(i = 0;i<80;i++)
	{
		SendCheckSum = SendCheckSum^((unsigned char) *((unsigned char *)(&(para->singalname)+i)));
	}
	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);

	Modify_LastUploadTime();
}

void UpLoad_Type()
{
	unsigned long i;
		StructPara *para = &Parameter;
		RSCmdtxBuf[0] = 0x55;
		RSCmdtxBuf[1] = 0x7a;
		RSCmdtxBuf[2] = 0x07;
		RSCmdtxBuf[3] = 0x00;
		RSCmdtxBuf[4] = 0x23;
		RSCmdtxBuf[5] = 0x00;
		SendCheckSum = 0x55^0x7a^0x07^0x00^0x23^0x00;
		rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

		rt_device_write(&uart2_device, 0, (const void *)(&(para->typedata) ), 35);
		for(i = 0;i<35;i++)
		{
			SendCheckSum = SendCheckSum^((unsigned char) *((unsigned char *)(&(para->typedata)+i)));
		}
		rt_device_write(&uart2_device, 0, &SendCheckSum, 1);

		Modify_LastUploadTime();
}
void UpLoad_BlockData( CLOCK Starttime, CLOCK Endtime,uint16_t lenth, uint16_t cmd)
{
	unsigned long i,j,k;
	unsigned long STOPp,STOPb,flag;
	unsigned short BlockSize;
	unsigned long startbase,endbase;
	unsigned long startime,endtime,readtime;
	unsigned char NumMaxBlockSize;
	unsigned char temp_data[1000];
	unsigned char Numcout = 0;
	unsigned char Sendflag = 0;
	CLOCK    temp_clock;
	startime = timechange(Starttime);
	endtime = timechange(Endtime);
	temp_data[0] = 0x55;
	temp_data[1] = 0x7a;
	temp_data[2] = cmd;
	switch (cmd)
	{
		case 0x08:
			BlockSize = DRV_SPEED_BLOCK;
			NumMaxBlockSize = 7;
			STOPp = pTable.BaseData.CurPoint;
			flag = pTable.BaseData.finshflag;
			startbase = BASEDATA_BASE;
			endbase = BASEDATA_END;
			break;
		case 0x09:
			BlockSize = LOCATION_BLOCK;
			NumMaxBlockSize = 1;
			STOPp = pTable.LocationData.CurPoint;
			flag = pTable.LocationData.finshflag;
			startbase = LOCATION_BASE;
			endbase = LOCATION_END;
			break;
		case 0x10:
			BlockSize = DOUBLT_BLOCK;
			NumMaxBlockSize = 4;
			STOPp = pTable.OverSpeedRecord.CurPoint;
			flag = pTable.OverSpeedRecord.finshflag;
			startbase = DPD_BASE;
			endbase = DPD_END;
			break;
		case 0x11:
			BlockSize = OVERDRV_BLOCK;
			NumMaxBlockSize = 19;
			STOPp = pTable.DoubtPointData.CurPoint;
			flag = pTable.DoubtPointData.finshflag;
			startbase = OVERDRV_BASE;
			endbase = OVERDRV_END;
			break;
		case 0x12:
			BlockSize = DRV_RG_BLOCK;
			NumMaxBlockSize = 39;
			STOPp = pTable.DriverReRecord.CurPoint;
			flag = pTable.DriverReRecord.finshflag;
			startbase = DRVRG_BASE;
			endbase = DRVRG_END;
			break;
		case 0x13:
			BlockSize = POW_BLOCK;
			NumMaxBlockSize = 141;
			STOPp = pTable.PowerOffRunRecord.CurPoint;
			flag = pTable.PowerOffRunRecord.finshflag;
			startbase = POWER_BASE;
			endbase = POWER_END;
			break;
		case 0x14:
			BlockSize = PARA_BLOCK;
			NumMaxBlockSize = 141;
			STOPp = pTable.ModifyRecord.CurPoint;
			flag = pTable.ModifyRecord.finshflag;
			startbase = PARA_BASE;
			endbase = PARA_END;
			break;
		case 0x15:
			BlockSize = JN_BLOCK;
			NumMaxBlockSize = 7;
			STOPp = pTable.journalRecord.CurPoint;
			flag = pTable.journalRecord.finshflag;
			startbase = JN_BASE;
			endbase =  JN_END;
			break;
		default:
			break;

	}
	//RSCmdtxBuf[3] = (BlockSize>>8)&0xff;
	//RSCmdtxBuf[4] =( unsigned char )BlockSize ;
	temp_data[5] = 0x00;
	j = 6;
	SendCheckSum = 0x55^0x7a^cmd^0x00;
	//rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	if((flag &0xea)!=0xea)
	{
		Numcout = 0;
	}
	else
	{
		for(i = 0;i< lenth;i++)
		{
			do
			{
				if(STOPp > startbase)
				{
					STOPp = STOPp-BlockSize;
				}
				else
				{
					STOPb = (endbase-startbase+1)/BlockSize;
					STOPp = STOPb*BlockSize+startbase;
				}
				if(BlockSize == OVERDRV_BLOCK)
				{
					SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp+18,6);
				}
				else
				{
					SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
				}
				readtime = timechange(temp_clock);

			}while((readtime >endtime) || (readtime == endtime ));
			if( readtime<startime )
			{
				break;
			}
			else
			{
				Numcout++;
				SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_data[j] ,STOPp, BlockSize);

				for(k =0;k<BlockSize;k++,j++)
				{
					SendCheckSum = SendCheckSum^(temp_data[j]);
				}
				if( Numcout == NumMaxBlockSize )
				{
					temp_data[3] = (NumMaxBlockSize*BlockSize)>>8;
					temp_data[4] = (unsigned char )NumMaxBlockSize*BlockSize;
					SendCheckSum = SendCheckSum^temp_data[4]^temp_data[3];
					temp_data[j] = SendCheckSum;
					j++;
					rt_device_write(&uart2_device, 0, temp_data, j);
					Sendflag = 1;
					Numcout = 0;
					j = 6;
				}
			}

		}
	}
	if( (Sendflag != 1) ||(Numcout !=0))
	{
		temp_data[3] = (Numcout*BlockSize)>>8;
		temp_data[4] = (unsigned char )Numcout*BlockSize;
		SendCheckSum = SendCheckSum^temp_data[4]^temp_data[3];
		temp_data[j] = SendCheckSum;
		j++;
		rt_device_write(&uart2_device, 0, temp_data, j);
		j = 6;
	}
	//Modify_LastUploadTime();
}
void upload_Miledata()
{
	unsigned long i;
	unsigned char temp_data[51];
	StructPara *para = &Parameter;
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0xe1;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x2c;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x07^0x00^0x2c^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

	rt_device_write(&uart2_device, 0, (const void *)(&(para->typedata) ), 35);
	for(i = 0;i<35;i++)
	{
		SendCheckSum = SendCheckSum^((unsigned char) *((unsigned char *)(&(para->typedata)+i)));
	}
	for(i = 0; i < 2;i++)
	{
		rt_device_write(&uart2_device, 0, (const void *)((para->PulseCoff) >> ((2-i)*8)), 1);
		SendCheckSum = SendCheckSum^((unsigned char)((para->PulseCoff) >> ((2-i)*8)));
	}
	for(i= 0;i<2;i++)
	{

		rt_device_write(&uart2_device, 0, (const void *)(&CurSpeed), 2);
	}
	SendCheckSum = SendCheckSum^((unsigned char )CurSpeed)^((unsigned char )(CurSpeed >>8));
	for(i= 0;i<4;i++)
	{
		rt_device_write(&uart2_device, 0, (const void *)((para->DriverDistace) >> ((4-i)*8)), 1);
		SendCheckSum = SendCheckSum^((unsigned char)((para->DriverDistace) >> ((4-i)*8)));
	}
	rt_device_write(&uart2_device, 0, (const void *)(&CurStatus), 1);
	SendCheckSum = SendCheckSum^CurStatus;
	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	Modify_LastUploadTime();
}
#if 0
void UpLoad_DriverSpeedInfo( CLOCK Starttime, CLOCK Endtime,uint16_t lenth )
{
	unsigned long i,j;
	unsigned long STOPp;
	unsigned long startime,endtime,readtime;
	unsigned  temp_data[120];
	CLOCK    temp_clock;
	startime = timechange(Starttime);
	endtime = timechange(Endtime);
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x08;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x1E;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x07^0x08^0x0c^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	for(i = 0;i< lenth;i++)
	{
		do
		{
			if(STOPp > BASEDATA_BASE)
			{
				STOPp = STOPp-DRV_SPEED_BLOCK;
			}
			else
			{
				STOPp = BASEDATA_END-DRV_SPEED_BLOCK;
			}
			SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
			readtime = timechange(temp_clock);

		}while((readtime >endtime) || (readtime == endtime ));
		if( readtime<startime )
		{
			break;
		}
		else
		{
			rt_device_write(&uart2_device, 0, &temp_data, 120);
			SendCheckSum = SendCheckSum^(temp_clock.year);
			SendCheckSum = SendCheckSum^(temp_clock.month);
			SendCheckSum = SendCheckSum^(temp_clock.day);
			SendCheckSum = SendCheckSum^(temp_clock.hour);
			SendCheckSum = SendCheckSum^(temp_clock.minute);
			SendCheckSum = SendCheckSum^(temp_clock.second);
			for(j =0;j++;j<120)
			{
				SendCheckSum = SendCheckSum^(temp_data[j]);

			}
		}

	}

	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	Modify_LastUploadTime();
}

void UpLoad_LocationInfo(CLOCK Starttime, CLOCK Endtime,uint16_t lenth)
{
		unsigned long i,j;
		unsigned long STOPp;
		unsigned long startime,endtime,readtime;
		unsigned speed;
		unsigned  temp_data[660];
		CLOCK    temp_clock;
		startime = timechange(Starttime);
		endtime = timechange(Endtime);
		RSCmdtxBuf[0] = 0x55;
		RSCmdtxBuf[1] = 0x7a;
		RSCmdtxBuf[2] = 0x09;
		RSCmdtxBuf[3] = 0x02;
		RSCmdtxBuf[4] = 0x9a;
		RSCmdtxBuf[5] = 0x00;
		SendCheckSum = 0x55^0x7a^0x07^0x08^0x0c^0x00;
		rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

		for(i = 0;i< lenth;i++)
		{
			do
			{
				if(STOPp > BASEDATA_BASE)
				{
					STOPp = STOPp-LOCATION_BLOCK;
				}
				else
				{
					STOPp = BASEDATA_END-LOCATION_BLOCK;
				}
				SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
				readtime = timechange(temp_clock);

			}while((readtime >endtime) || (readtime == endtime ));
			if( readtime<startime )
			{
				break;
			}
			else
			{
				SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_data ,STOPp+6, 660);
				rt_device_write(&uart2_device, 0, &temp_clock, 6);
				rt_device_write(&uart2_device, 0, &temp_data, 660);
				SendCheckSum = SendCheckSum^(temp_clock.year);
				SendCheckSum = SendCheckSum^(temp_clock.month);
				SendCheckSum = SendCheckSum^(temp_clock.day);
				SendCheckSum = SendCheckSum^(temp_clock.hour);
				SendCheckSum = SendCheckSum^(temp_clock.minute);
				SendCheckSum = SendCheckSum^(temp_clock.second);
				for(j =0;j++;j<660)
				{
					SendCheckSum = SendCheckSum^(temp_data[j]);
				}
			}

		}
		rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
		Modify_LastUploadTime();
}

void UpLoad_DoubltPointInfo(CLOCK Starttime, CLOCK Endtime,uint16_t lenth)
{
	unsigned long i,j;
	unsigned long STOPp;
	unsigned long startime,endtime,readtime;
	unsigned  temp_data[660];
	CLOCK    temp_clock;
	startime = timechange(Starttime);
	endtime = timechange(Endtime);
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x0a;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0xEA;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x07^0x08^0x0c^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

	for(i = 0;i< lenth;i++)
	{
		do
		{
			if(STOPp > BASEDATA_BASE)
			{
				STOPp = STOPp-DOUBLT_BLOCK;
			}
			else
			{
				STOPp = BASEDATA_END-DOUBLT_BLOCK;
			}
			SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
			readtime = timechange(temp_clock);

		}while((readtime >endtime) || (readtime == endtime ));
		if( readtime<startime )
		{
			break;
		}
		else
		{
			rt_device_write(&uart2_device, 0, &temp_data, 228);
			SendCheckSum = SendCheckSum^(temp_clock.year);
			SendCheckSum = SendCheckSum^(temp_clock.month);
			SendCheckSum = SendCheckSum^(temp_clock.day);
			SendCheckSum = SendCheckSum^(temp_clock.hour);
			SendCheckSum = SendCheckSum^(temp_clock.minute);
			SendCheckSum = SendCheckSum^(temp_clock.second);
			for(j =0;j++;j<228)
			{
				SendCheckSum = SendCheckSum^(temp_data[j]);

			}
		}

	}

	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	Modify_LastUploadTime();
}

void UpLoad_OverDriveInfo(CLOCK Starttime, CLOCK Endtime,uint16_t lenth)
{
	unsigned long i,j;
	unsigned long STOPp;
	unsigned long startime,endtime,readtime;
	unsigned  temp_data[660];
	CLOCK    temp_clock;
	startime = timechange(Starttime);
	endtime = timechange(Endtime);
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x0b;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x32;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x07^0x08^0x0c^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

	for(i = 0;i< lenth;i++)
	{
		do
		{
			if(STOPp > BASEDATA_BASE)
			{
				STOPp = STOPp-OVERDRV_BLOCK;
			}
			else
			{
				STOPp = BASEDATA_END-OVERDRV_BLOCK;
			}
			SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
			readtime = timechange(temp_clock);

		}while((readtime >endtime) || (readtime == endtime ));
		if( readtime<startime )
		{
			break;
		}
		else
		{
			rt_device_write(&uart2_device, 0, &temp_data, 44);
			SendCheckSum = SendCheckSum^(temp_clock.year);
			SendCheckSum = SendCheckSum^(temp_clock.month);
			SendCheckSum = SendCheckSum^(temp_clock.day);
			SendCheckSum = SendCheckSum^(temp_clock.hour);
			SendCheckSum = SendCheckSum^(temp_clock.minute);
			SendCheckSum = SendCheckSum^(temp_clock.second);
			for(j =0;j++;j<44)
			{
				SendCheckSum = SendCheckSum^(temp_data[j]);

			}
		}

	}

	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	Modify_LastUploadTime();
}
void UpLoad_DriverReInfo(CLOCK Starttime, CLOCK Endtime,uint16_t lenth)
{
	unsigned long i,j;
	unsigned long STOPp;
	unsigned long startime,endtime,readtime;
	unsigned  temp_data[660];
	CLOCK    temp_clock;
	startime = timechange(Starttime);
	endtime = timechange(Endtime);
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x0c;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x19;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x07^0x08^0x0c^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);

	for(i = 0;i< lenth;i++)
	{
		do
		{
			if(STOPp > BASEDATA_BASE)
			{
				STOPp = STOPp-DRV_RG_BLOCK;
			}
			else
			{
				STOPp = BASEDATA_END-DRV_RG_BLOCK;
			}
			SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
			readtime = timechange(temp_clock);

		}while((readtime >endtime) || (readtime == endtime ));
		if( readtime<startime )
		{
			break;
		}
		else
		{
			rt_device_write(&uart2_device, 0, &temp_data, 19);
			SendCheckSum = SendCheckSum^(temp_clock.year);
			SendCheckSum = SendCheckSum^(temp_clock.month);
			SendCheckSum = SendCheckSum^(temp_clock.day);
			SendCheckSum = SendCheckSum^(temp_clock.hour);
			SendCheckSum = SendCheckSum^(temp_clock.minute);
			SendCheckSum = SendCheckSum^(temp_clock.second);
			for(j =0;j++;j<19)
			{
				SendCheckSum = SendCheckSum^(temp_data[j]);

			}
		}

	}

	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	Modify_LastUploadTime();
}
UpLoad_PowerInfo
UpLoad_ParaModifyInfo
UpLoad_JournalInfo
#endif

void UpLoad_ExVersion()
{
	unsigned char i;
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x00;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x02;
	RSCmdtxBuf[5] = 0x00;
	RSCmdtxBuf[6] = Parameter.standeryear;
	RSCmdtxBuf[7] = Parameter.modifyNb;
	SendCheckSum = 0x55^0x7a^0x00^0x00^0x02^0x00^0x03^0x00;

	rt_device_write(&uart2_device, 0,RSCmdtxBuf, 8);
	rt_device_write(&uart2_device, 0,&SendCheckSum, 1);

}

//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_DriverCode
//* Object                   : ���ؼ�ʻԱ���뼰��Ӧ�Ļ���ʻ֤���루��������
//*                          : ���������ֱ�ӴӼ�¼����ݴ洢���Ĳ������ȡ�ã�
//*                          : ������Ϊ0x01
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵΪ���ؼ�ʻԱ���뼰��Ӧ�Ļ���ʻ֤�����Ӧ��֡
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*----------------------------------------------------------------------------
void UpLoad_DriverCode()
{
	unsigned long i;
	StructPara *para = &Parameter;
	
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x01;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x12;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x01^0x00^0x12^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	for(i = 0; i < 18;i++)
	{
		rt_device_write(&uart2_device, 0, &((para->DriverLisenseCode)[i]), 1);
		SendCheckSum = SendCheckSum^((para->DriverLisenseCode)[i]);
	}

	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	
	Modify_LastUploadTime();
}

//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_RealTime
//* Object                   : ����ʵʱʱ�䣬��ݴ�����ȫ�ֱ���curTime�У�
//*                          : ������Ϊ0x02
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵΪ����ʵʱʱ���Ӧ��֡
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*----------------------------------------------------------------------------
void UpLoad_RealTime()
{
	unsigned long i;
	
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x02;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x06;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x02^0x00^0x06^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
		
	WriteDataTxTime();
	
	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	
	Modify_LastUploadTime();
}

//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_TotalDistance360h
//* Object                   : �������360Сʱ�ڵ��ۼ���ʻ��̣���Ҫ���ڴ���
//*                          : ��¼�������ѡ���жϡ��ۼӡ��������������
//*                          : ���360Сʱ���ۼ������������ʱ����Buf�У�
//*                          : �����ó����ۼ���̴�����ʱ����disbuf�У�
//*                          : ���ۼ���̵ķ������ȸ��ֽں���ֽڣ�
//*                          : ������Ϊ0x03
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵΪ�������360Сʱ�ۼ���ʻ��̵�Ӧ��֡
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*----------------------------------------------------------------------------
/*void UpLoad_TotalDistance360h()
{
	int TimeIntervalSum,Nb;
	int i,j,TimeInterval;
	unsigned long curPointer;
	unsigned long TimeLimit;
	OTDR record;
	OTDR_start last_start;
	unsigned long Buf;
	StructPT spt;
	DateTime BigTime,SmallTime,BigTime1,SmallTime1;
	unsigned char disbuf[3];

	//�رտ��Ź�
#if WATCH_DOG_EN
	WD_OMR = 0x2340;
#endif	
	unsigned char StartTimeBuf[6];
	unsigned char StopTimeBuf[6];
	int offset;
	spt = pTable.RunRecord360h;

	//�ó�ֵ
	Buf = 0;
	SendCheckSum=0;
	TimeIntervalSum = 0;
	curPointer = pTable.RunRecord360h.CurPoint;
	TimeLimit = 360*60;	
	last_start.dt.type = 0;
	
	do
	{
		//ȡ����ǰ��¼�ǲ���ȷ��
		if(!GetOTDR(curPointer,&(record.start), &(record.end)))
		{
			offset = -1;
			curPointer = AddPointer(&spt, offset);
			spt.CurPoint = curPointer;
			continue;
		}		
		PrepareTime((unsigned char *)(&(record.end.dt.year)),&BigTime1);
		PrepareTime((unsigned char *)(&(record.start.dt.year)),&SmallTime1);
		TimeInterval = HaveTime(BigTime1,SmallTime1);
		if(TimeInterval<0)
		{
			//�޸�ָ��
			offset = 0 - (sizeof(OTDR_start)+sizeof(OTDR_end)+record.end.MinuteNb);
			curPointer = AddPointer(&spt, offset);
			spt.CurPoint = curPointer;
			last_start = record.start;	
			continue;
		}	

		if(last_start.dt.type==0xafaf)
			PrepareTime((unsigned char *)(&(last_start.dt.year)),&BigTime);
		else
			PrepareTime((unsigned char *)(&curTime),&BigTime);
		
		//���㵱ǰ��¼����һ����¼֮���ʱ���	
		PrepareTime((unsigned char *)(&(record.end.dt.year)),&SmallTime);
		TimeInterval = HaveTime(BigTime,SmallTime);
		if(TimeInterval < 0)
		{
			//�޸�ָ��
			offset = 0 - (sizeof(OTDR_start)+sizeof(OTDR_end)+record.end.MinuteNb);
			curPointer = AddPointer(&spt, offset);
			spt.CurPoint = curPointer;
			last_start = record.start;	
			continue;
		}	
		TimeIntervalSum  += TimeInterval;
		if(TimeIntervalSum >=TimeLimit)
			break;
		
		Nb = record.end.MinuteNb;
		Buf +=  record.end.TotalDistance;
		TimeIntervalSum += Nb;
		
		if(TimeIntervalSum >=TimeLimit)
			break;
		//�޸�ָ��
		offset = 0 - (sizeof(OTDR_start)+sizeof(OTDR_end)+record.end.MinuteNb);
		curPointer = AddPointer(&spt, offset);
		spt.CurPoint = curPointer;

		last_start = record.start;		
	}while((TimeIntervalSum <= 360*60)&&(pTable.RunRecord360h.CurPoint!=curPointer));
	
	//��������������
	Buf = ComputeDistance100m(Buf);
	disbuf[0]=(unsigned char)(Buf>>16);
	disbuf[1]=(unsigned char)(Buf>>8);
	disbuf[2]=(unsigned char)(Buf);
	
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x03;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x08;
	RSCmdtxBuf[5] = 0x00;
	for(i = 0; i < 6;i++)
	{
		while((at91_usart_get_status(RS232) & 0x02) != 0x02);
		at91_usart_write(RS232,RSCmdtxBuf[i]);
		SendCheckSum = SendCheckSum^RSCmdtxBuf[i];
	}
	//����������ݿ�
	for(i = 0;i < 3;i++)
	{
		while((at91_usart_get_status(RS232) & 0x02) != 0x02);
		at91_usart_write(RS232,disbuf[i]);
		SendCheckSum = SendCheckSum^(disbuf[i]);
	}
	//������ݶ���ʱ��
	WriteDataTxTime();
	
	//����У���
	while((at91_usart_get_status(RS232) & 0x02) != 0x02);
	at91_usart_write(RS232,SendCheckSum);
	
	Modify_LastUploadTime();
	
	//�������Ź�
#if WATCH_DOG_EN
    WD_CR = 0xc071;
    WD_OMR = 0x2343;
#endif
}*/

//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_CHCO
//* Object                   : ���س�������ϵ��������
//*                          : ���������ֱ�ӴӼ�¼����ݴ洢���Ĳ������ȡ�ã�
//*                          : ������Ϊ0x04
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵΪ���س�������ϵ���Ӧ��֡
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*----------------------------------------------------------------------------
void UpLoad_PulseCoff()
{
	unsigned long i;
	unsigned char temp[2];
	StructPara *para = &Parameter;
	temp[0] = para->PulseCoff>>8;
	temp[1] = para->PulseCoff;
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x04;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x08;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x04^0x00^0x03^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	WriteDataTxTime();
	rt_device_write(&uart2_device, 0, &temp ,2);
	for(i = 0; i < 2;i++)
	{
		SendCheckSum = SendCheckSum^((unsigned char)((para->PulseCoff) >> ((1-i)*8)));
	}

	//����У���
	rt_device_write(&uart2_device, 0, &SendCheckSum ,1);
	Modify_LastUploadTime();
}

//*----------------------------------------------------------------------------
//* Function Name            : ComputeTimeBeforeX
//* Object                   : ����ĳһʱ��֮ǰһ��ʱ���ʱ��ֵ
//* Input Parameters         : ct����ָ���ʱ�䣨������ʱ����Ľṹ����ָ��
//*                          : dt����ָ��������ʱ����������ʱ����Ľṹ����ָ��
//*                          : timeinterval����ʱ�������Է��Ӽƣ���
//*                          : ʱ����ǰ��Ϊ��,timeinterval����Ϊ��
//* Output Parameters        : none
//* Global  Variable Quoted  : none
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
void ComputeTimeBeforeX(CLOCK *ct,CLOCK *dt,unsigned long timeinterval)
{
	DateTime TimeBeforeXBuf;
		
	TimeBeforeXBuf.year = BCD2Char(ct->year);
	TimeBeforeXBuf.month = BCD2Char(ct->month);
	TimeBeforeXBuf.day = BCD2Char(ct->day);
	TimeBeforeXBuf.time = BCD2Char(ct->hour)*60+BCD2Char(ct->minute);
	
	int hour;
	hour = 0 - timeinterval;
	IncreaseTime(&TimeBeforeXBuf,hour);
	
	dt->year = Char2BCD(TimeBeforeXBuf.year);
	dt->month = Char2BCD(TimeBeforeXBuf.month);
	dt->day = Char2BCD(TimeBeforeXBuf.day);
	
	hour = (TimeBeforeXBuf.time)/60;
	dt->hour = Char2BCD((unsigned char)hour);
	dt->minute = Char2BCD(TimeBeforeXBuf.time-hour*60);
}





void Write65ByteToSRAM(unsigned short hourNB,CLOCK *t,unsigned char *buf)
{
	unsigned char i;
	unsigned long start = 65*hourNB;
	//дÿСʱ��ʼʱ���뻺����
	LargeDataBuffer[start] = t->year;
	LargeDataBuffer[start+1] = t->month;
	LargeDataBuffer[start+2] = t->day;
	LargeDataBuffer[start+3] = t->hour;
	LargeDataBuffer[start+4] = t->minute;
	
	//д60��������뻺����
	for(i=0;i<60;i++)
		LargeDataBuffer[start+5+i] = buf[i];
}


void RefreshCurTime(CLOCK *s,CLOCK *d)
{
	d->year = s->year;
	d->month = s->month;
	d->day = s->day;
	d->hour = s->hour;
	d->minute = s->minute;
}


//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_Speed360h
//* Object                   : �������360Сʱ�ڵ���ʻ�ٶȣ���Ҫ���ڴ���
//*                          : ��¼�������ѡ���жϡ��ۼӡ��������������
//*                          : ���360Сʱ����ʻ�ٶ���ݴ���ȫ�ֱ���LargeDataBuffer�У�
//*                          : ���ȷ��͵���ݿ����ٶ���ݶ�Ӧ����ʼʱ�䣬
//*                          : ����Ϊ360*60��360Сʱ��ʻ�ٶ���ݱ���Ϊ6�鷢�ͣ�
//*                          : ÿ��ĳ�����3607�ֽ�
//*                          : ����ʻ�ٶȵķ���������ʱ�����ʱ�䣬
//*                          : ������Ϊ0x05
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//*                          : RSCmdrxBuf[0]��RSCmdrxBuf[6]����������ռĴ���,
//*                          : �����ڴ��͸�ҳ֮�����PC���ͬ������
//*                          : CheckSum������PC����յ�������������У���
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ʱ��ҳ��ֵΪ�������360Сʱ�ٶȵ�Ӧ��֡��
//*                          : ���ҳ��ֵΪ�������360Сʱ�ٶȵ�Ӧ��֡�м���ҳ��
//*                          : RSCmdrxBuf[0]��RSCmdrxBuf[6]����������ռĴ���,
//*                          : ÿҳ���յ������ݶ����ҳ������ͬ
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*                          : LargeDataBuffer��������ڴ���ȡ�������360Сʱ�ٶ����
//*----------------------------------------------------------------------------
void UpLoad_Speed360h()
{
	int offset,i,j,TimeInterval;
	StructPT spt;
	unsigned char ReadOTDRFlag = 0;            //����¼��־
	unsigned char InOTDRFlag = 0;              //��һ��ƣ�ͼ�ʻ��¼�м��־
	unsigned char FilledNB = 0;                //60�ֽ����Ѿ������ֽ���
	unsigned char FirstRead = 1;               //����һ����¼��־
	unsigned long curPointer,Old_Pointer,Mid_Pointer;   //DataFlash�е�ָ��
	OTDR cur_record,Last_Record;
	unsigned short hourNB = 0;                 //360Сʱ������
	unsigned long CurRemainMinuteNB;            //��ǰʣ�������
	unsigned long temp;
	CLOCK StartTime;
	CLOCK current_time;
	unsigned char Buf60Bytes[60];              //60������ݻ�����
	DateTime BigTime,SmallTime;
	unsigned long status232;
	unsigned long rhr;

	//�رտ��Ź�
	#if WATCH_DOG_EN
	WD_OMR = 0x2340;
	#endif	

	Old_Pointer = curPointer;
	for(i=0;i<60;i++)
		Buf60Bytes[i] = 0;
	for(i=0;i<360*65;i++)
		LargeDataBuffer[i] = 0;

	//�����û�кϷ���¼
	if(GetOneOTDRandModifyPointer(&(curPointer), &(Old_Pointer), &(cur_record.start), &(cur_record.end)))
	{
		CurRemainMinuteNB = cur_record.end.MinuteNb;
		RefreshCurTime((CLOCK *)(&(cur_record.end.dt.year)),(CLOCK *)(&current_time));
		spt.CurPoint = Old_Pointer;
		
		do
		{
			//�ж���¼��־
			if(ReadOTDRFlag)
			{
				//û�гɹ�����ƣ�ͼ�¼�������65�ֽڲ�����
				if(!GetOneOTDRandModifyPointer(&(curPointer), &(Old_Pointer), &(Last_Record.start), &(Last_Record.end)))
				{
					ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
					//��ȡʣ�����������װ���ڴ�
					Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);
					hourNB++;
					break;
				}
				//�ɹ�����һ��ƣ�ͼ�ʻ��¼
				else
				{
					spt.CurPoint = Old_Pointer;
					FirstRead = 0;
					//����������¼֮���ʱ���
					PrepareTime((unsigned char *)(&(cur_record.start.dt.year)),&BigTime);
					PrepareTime((unsigned char *)(&(Last_Record.end.dt.year)),&SmallTime);
					TimeInterval = HaveTime(BigTime,SmallTime);
					//���ʣ��������ʱ��������60����
					if((TimeInterval+FilledNB)>=60)
					{
						ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
						Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);
						for(i=0;i<60;i++)
							Buf60Bytes[i] = 0;
						hourNB++;
						InOTDRFlag = 0;
						//����ʣ��������ʱ��
						CurRemainMinuteNB = Last_Record.end.MinuteNb;
						RefreshCurTime((CLOCK *)(&(Last_Record.end.dt.year)),&current_time);
						ReadOTDRFlag = 0;				
						continue;
					}
					//���ʣ��������ʱ����С��60����
					else if((TimeInterval+FilledNB)<60)
					{
						if((TimeInterval+FilledNB+Last_Record.end.MinuteNb)>=60)
						{
							ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
							temp = 60-TimeInterval-FilledNB;
							Old_Pointer = AddPointer(&spt, -sizeof(OTDR_end));
							spt.CurPoint = Old_Pointer;
							offset = 0-temp;
							GetOTDRDataFromFlash((unsigned short *)Old_Pointer,offset,Buf60Bytes);
							Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);			
							for(i=0;i<60;i++)
								Buf60Bytes[i] = 0;
							Old_Pointer = AddPointer(&spt, offset);
							Mid_Pointer = Old_Pointer;
							spt.CurPoint = Old_Pointer;
							hourNB++;
							InOTDRFlag = 1;
							//����ʣ��������ʱ��
							CurRemainMinuteNB = Last_Record.end.MinuteNb-temp;
							RefreshCurTime(&StartTime,&current_time);	
							ReadOTDRFlag = 0;
							continue;
						}
						else if((TimeInterval+FilledNB+Last_Record.end.MinuteNb)<60)
						{
							temp = 60-TimeInterval-FilledNB;
							Old_Pointer = AddPointer(&spt, -(sizeof(OTDR_end)));
							spt.CurPoint = Old_Pointer;
							offset = 0-Last_Record.end.MinuteNb;
							j = 60-TimeInterval-FilledNB-Last_Record.end.MinuteNb;
							GetOTDRDataFromFlash((unsigned short *)Old_Pointer,offset,&(Buf60Bytes[j]));
							offset = 0-sizeof(OTDR_start)-Last_Record.end.MinuteNb;
							Old_Pointer = AddPointer(&spt, offset);
							spt.CurPoint = Old_Pointer;
							ReadOTDRFlag = 1;
							//��ȡ��ǰ��¼����ʼʱ��
							RefreshCurTime((CLOCK *)(&(Last_Record.start.dt.year)),(CLOCK *)(&(cur_record.start.dt.year)));
							FilledNB = TimeInterval+FilledNB+Last_Record.end.MinuteNb;
							continue;
						}
					}
				}
			}
			//û�ж���¼��־
			if(!ReadOTDRFlag)
			{
				//��ǰʣ����������60
				if(CurRemainMinuteNB > 60)
				{
					ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
					//��ȡ��Сʱ��60������ݴ���Buf60Bytes
					if(!InOTDRFlag)
					{
						Old_Pointer = AddPointer(&spt, -(sizeof(OTDR_end)));
						spt.CurPoint = Old_Pointer;
					}
					GetOTDRDataFromFlash((unsigned short *)Old_Pointer,-60,Buf60Bytes);
					Old_Pointer = AddPointer(&spt, -60);
					Mid_Pointer = Old_Pointer;
					spt.CurPoint = Old_Pointer;
					Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);
					for(i=0;i<60;i++)
						Buf60Bytes[i] = 0;
					hourNB++;
					InOTDRFlag = 1;
					//����ʣ��������ʱ��
					CurRemainMinuteNB -= 60;
					RefreshCurTime(&StartTime,&current_time);				
					continue;
				}
				//��ǰʣ�������С��60
				else
				{
					//�ö���¼��־
					ReadOTDRFlag = 1;
					if(!FirstRead)//��ȡ��ǰ��¼����ʼʱ��
						RefreshCurTime((CLOCK *)(&(Last_Record.start.dt.year)),(CLOCK *)(&(cur_record.start.dt.year)));
					//���䲿�������60�ֽڻ�����
					if(!InOTDRFlag)
					{
						Old_Pointer = AddPointer(&spt, -(sizeof(OTDR_end)));
						spt.CurPoint = Old_Pointer;
					}
					offset = 0-CurRemainMinuteNB;
					if(offset!=0)
					{
						temp = 60-CurRemainMinuteNB;
						GetOTDRDataFromFlash((unsigned short *)Old_Pointer,offset,&(Buf60Bytes[temp]));
					}
					Old_Pointer = AddPointer(&spt, offset-sizeof(OTDR_start));
					spt.CurPoint = Old_Pointer;
					FilledNB = CurRemainMinuteNB;
					continue;
				}
			}
		}while((hourNB<=360));
	}
	
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x05;
	RSCmdtxBuf[3] = (unsigned char)((hourNB*65)>>8);
	RSCmdtxBuf[4] = (unsigned char)(hourNB*65);
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x05^((unsigned char)((hourNB*65)>>8))^((unsigned char)(hourNB*65))^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	

	for(j=0;j<hourNB;j++)
	{
		//����������ݿ飨360h֮ǰ��ʱ�䣩
		for(i = 0;i < 65;i++)
		{
			rt_device_write(&uart2_device, 0, &LargeDataBuffer[j*65+i],1);
			SendCheckSum = SendCheckSum^LargeDataBuffer[j*65+i];
		}
	}
	//����У���
	rt_device_write(&uart2_device, 0, &SendCheckSum,1);

	//�������Ź�
	#if WATCH_DOG_EN
    WD_CR = 0xc071;
    WD_OMR = 0x2343;
	#endif
}


//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_DoubtPoint
//* Object                   : �����¹��ɵ���ݣ�10���ɵ���ݴ�
//*                          : ��¼���ڴ��е��ɵ������ȡ��
//*                          : ������Ϊ0x07
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵΪ�����¹��ɵ��Ӧ��֡,
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������,
//*                          : LargeDataBuffer����ʹ�õ�0��2059���ֽڴ��
//*                          : 10���ɵ����
//*----------------------------------------------------------------------------
void UpLoad_DoubtPoint()
{
	unsigned long i,j;
	unsigned long *source,*des;
	unsigned long STOPp;
	unsigned short temp_data;
	
	for(i=0;i<2060;i++)
		LargeDataBuffer[i]=0;
	
	//�رտ��Ź�
	#if WATCH_DOG_EN
	WD_OMR = 0x2340;
	#endif	
	STOPp = pTable.DoubtPointData.CurPoint;
	for(i = 0;i < 10;i++)
	{
		STOPp -= 206;
		source = (unsigned long *)STOPp;
		des = (unsigned long *)(&(LargeDataBuffer[i*206]));
		for(j = 0;j < 103;j++)
		{
			des[j]=source[j];
		}
		//�ƶ��źŷ���״̬�����λ
		for(j = 3;j < 103;j++)
		{
			temp_data = (des[j])&0x8200;
			if(temp_data==0x0200)
				des[j]=((des[j])|0x8000)&0xfdff;
			if(temp_data==0x8000)
				des[j]=((des[j])|0x0200)&0x7fff;
		}
		STOPp -= 4;
		if(STOPp==pTable.DoubtPointData.BaseAddr)
			STOPp = pTable.DoubtPointData.EndAddr - 110 + 1;
	}
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x07;
	RSCmdtxBuf[3] = 0x08;
	RSCmdtxBuf[4] = 0x0c;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x07^0x08^0x0c^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
		
	//����2060�ֽڵ��ɵ����
	for(i = 0; i < 2060;i++)
	{
		rt_device_write(&uart2_device, 0, &LargeDataBuffer[i], 6);
		SendCheckSum = SendCheckSum^(LargeDataBuffer[i]);
	}
	//����У���
	rt_device_write(&uart2_device, 0, &SendCheckSum, 6);
	Modify_LastUploadTime();
	//�������Ź�
	#if WATCH_DOG_EN
    WD_CR = 0xc071;
    WD_OMR = 0x2343;
	#endif
}

//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_DistanceinTwoDays
//* Object                   : ����������2���������ڵ��ۼ���ʻ��̣���Ҫ���ڴ���
//*                          : ��¼�������ѡ���жϡ��ۼӡ��������������
//*                          : ���2����������ۼ���̴�����ʱ����Buf�У�
//*                          : ���ۼ���̵ķ������ȸ��ֽں���ֽڣ�
//*                          : ������Ϊ0x08
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵΪ�������360Сʱ�ۼ���ʻ��̵�Ӧ��֡
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*----------------------------------------------------------------------------
/*void UpLoad_DistanceinTwoDays()
{
	int offset,i,j,TimeInterval;
	StructPT spt;
	unsigned char ReadOTDRFlag = 0;            //����¼��־
	unsigned char InOTDRFlag = 0;              //��һ��ƣ�ͼ�ʻ��¼�м��־
	unsigned char FilledNB = 0;                //60�ֽ����Ѿ������ֽ���
	unsigned char FirstRead = 1;               //����һ����¼��־
	unsigned long curPointer,Old_Pointer,Mid_Pointer;   //DataFlash�е�ָ��
	OTDR cur_record,Last_Record,temp_record;
	unsigned short hourNB = 0;                 //360Сʱ������
	unsigned long CurRemainMinuteNB;            //��ǰʣ�������
	unsigned long temp;
	CLOCK StartTime;
	CLOCK current_time,temptime;
	DateTime BigTime,SmallTime;
	unsigned long rhr;
	unsigned long Buf;
	unsigned char disbuf[3];
	unsigned char HourLimit;

	//�رտ��Ź�
	#if WATCH_DOG_EN
	WD_OMR = 0x2340;
	#endif	

	spt = pTable.RunRecord360h;
	curPointer = pTable.RunRecord360h.CurPoint;
	Old_Pointer = curPointer;
	Buf = 0;
	SendCheckSum=0;

	do
	{
		if(!GetOTDR(curPointer,&(temp_record.start), &(temp_record.end)))
		{
			offset = -1;
			curPointer = AddPointer(&spt, offset);
			spt.CurPoint = curPointer;
			continue;
		}
		break;
	}while(pTable.RunRecord360h.CurPoint!=curPointer);
	
	temptime.year = temp_record.end.dt.year;
	temptime.month = temp_record.end.dt.month;
	temptime.day = temp_record.end.dt.day;
	temptime.hour = temp_record.end.dt.hour;
	temptime.minute = temp_record.end.dt.minute;
		

	//�ó�ֵ
	HourLimit = BCD2Char(temptime.hour) + 24 + 1;	//�����û�кϷ���¼
	if(!GetOneOTDRandModifyPointer(&(curPointer), &(Old_Pointer), &(cur_record.start), &(cur_record.end)))
	{
		RS232UploadError();
		//�������Ź�
		#if WATCH_DOG_EN
	    WD_CR = 0xc071;
	    WD_OMR = 0x2343;
		#endif
		return;
	}
	CurRemainMinuteNB = cur_record.end.MinuteNb;
	Buf += cur_record.end.TotalDistance;
	RefreshCurTime((CLOCK *)(&(cur_record.end.dt.year)),(CLOCK *)(&current_time));
	spt.CurPoint = Old_Pointer;
	
	do
	{
		//�ж���¼��־
		if(ReadOTDRFlag)
		{
			//û�гɹ�����ƣ�ͼ�¼�������65�ֽڲ�����
			if(!GetOneOTDRandModifyPointer(&(curPointer), &(Old_Pointer), &(Last_Record.start), &(Last_Record.end)))
			{
				ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
				//��ȡʣ�����������װ���ڴ�
//				Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);
				hourNB++;
				break;
			}
			//�ɹ�����һ��ƣ�ͼ�ʻ��¼
			else
			{
				Buf += Last_Record.end.TotalDistance;
				spt.CurPoint = Old_Pointer;
				FirstRead = 0;
				//����������¼֮���ʱ���
				PrepareTime((unsigned char *)(&(cur_record.start.dt.year)),&BigTime);
				PrepareTime((unsigned char *)(&(Last_Record.end.dt.year)),&SmallTime);
				TimeInterval = HaveTime(BigTime,SmallTime);
				//���ʣ��������ʱ��������60����
				if((TimeInterval+FilledNB)>=60)
				{
					ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
//					Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);
//					for(i=0;i<60;i++)
//						Buf60Bytes[i] = 0;
					hourNB++;
					InOTDRFlag = 0;
					//����ʣ��������ʱ��
					CurRemainMinuteNB = Last_Record.end.MinuteNb;
					RefreshCurTime((CLOCK *)(&(Last_Record.end.dt.year)),&current_time);
					ReadOTDRFlag = 0;				
					continue;
				}
				//���ʣ��������ʱ����С��60����
				else if((TimeInterval+FilledNB)<60)
				{
					if((TimeInterval+FilledNB+Last_Record.end.MinuteNb)>=60)
					{
						ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
						temp = 60-TimeInterval-FilledNB;
						Old_Pointer = AddPointer(&spt, -sizeof(OTDR_end));
						spt.CurPoint = Old_Pointer;
						offset = 0-temp;
//						GetOTDRDataFromFlash((unsigned short *)Old_Pointer,offset,Buf60Bytes);
//						Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);			
//						for(i=0;i<60;i++)
//							Buf60Bytes[i] = 0;
						Old_Pointer = AddPointer(&spt, offset);
						Mid_Pointer = Old_Pointer;
						spt.CurPoint = Old_Pointer;
						hourNB++;
						InOTDRFlag = 1;
						//����ʣ��������ʱ��
						CurRemainMinuteNB = Last_Record.end.MinuteNb-temp;
						RefreshCurTime(&StartTime,&current_time);	
						ReadOTDRFlag = 0;
						continue;
					}
					else if((TimeInterval+FilledNB+Last_Record.end.MinuteNb)<60)
					{
						temp = 60-TimeInterval-FilledNB;
						Old_Pointer = AddPointer(&spt, -(sizeof(OTDR_end)));
						spt.CurPoint = Old_Pointer;
//						offset = 0-Last_Record.end.MinuteNb;
//						j = 60-TimeInterval-FilledNB-Last_Record.end.MinuteNb;
//						GetOTDRDataFromFlash((unsigned short *)Old_Pointer,offset,&(Buf60Bytes[j]));
						offset = 0-sizeof(OTDR_start)-Last_Record.end.MinuteNb;
						Old_Pointer = AddPointer(&spt, offset);
						spt.CurPoint = Old_Pointer;
						ReadOTDRFlag = 1;
						//��ȡ��ǰ��¼����ʼʱ��
						RefreshCurTime((CLOCK *)(&(Last_Record.start.dt.year)),(CLOCK *)(&(cur_record.start.dt.year)));
						FilledNB = TimeInterval+FilledNB+Last_Record.end.MinuteNb;
						continue;
					}
				}
			}
		}
		//û�ж���¼��־
		if(!ReadOTDRFlag)
		{
			//��ǰʣ����������60
			if(CurRemainMinuteNB > 60)
			{
				ComputeTimeBeforeX(&current_time,&StartTime,60);  //���㱾Сʱ����ʼʱ��
				//��ȡ��Сʱ��60������ݴ���Buf60Bytes
				if(!InOTDRFlag)
				{
					Old_Pointer = AddPointer(&spt, -(sizeof(OTDR_end)));
					spt.CurPoint = Old_Pointer;
				}
//				GetOTDRDataFromFlash((unsigned short *)Old_Pointer,-60,Buf60Bytes);
				Old_Pointer = AddPointer(&spt, -60);
				Mid_Pointer = Old_Pointer;
				spt.CurPoint = Old_Pointer;
//				Write65ByteToSRAM(hourNB,&StartTime,Buf60Bytes);
//				for(i=0;i<60;i++)
//					Buf60Bytes[i] = 0;
				hourNB++;
				InOTDRFlag = 1;
				//����ʣ��������ʱ��
				CurRemainMinuteNB -= 60;
				RefreshCurTime(&StartTime,&current_time);				
				continue;
			}
			//��ǰʣ�������С��60
			else
			{
				//�ö���¼��־
				ReadOTDRFlag = 1;
				if(!FirstRead)//��ȡ��ǰ��¼����ʼʱ��
					RefreshCurTime((CLOCK *)(&(Last_Record.start.dt.year)),(CLOCK *)(&(cur_record.start.dt.year)));
				//���䲿�������60�ֽڻ�����
				if(!InOTDRFlag)
				{
					Old_Pointer = AddPointer(&spt, -(sizeof(OTDR_end)));
					spt.CurPoint = Old_Pointer;
				}
//				offset = 0-CurRemainMinuteNB;
//				if(offset!=0)
//				{
//					temp = 60-CurRemainMinuteNB;
//					GetOTDRDataFromFlash((unsigned short *)Old_Pointer,offset,&(Buf60Bytes[temp]));
//				}
				Old_Pointer = AddPointer(&spt, offset-sizeof(OTDR_start));
				spt.CurPoint = Old_Pointer;
				FilledNB = CurRemainMinuteNB;
				continue;
			}
		}
	}while((hourNB<=HourLimit)&&(pTable.RunRecord360h.CurPoint!=Old_Pointer));
	
	//��������������
	Buf = ComputeDistance100m(Buf);
	Int2BCD(Buf, disbuf);
//	disbuf[0]=(unsigned char)(Buf>>16);
//	disbuf[1]=(unsigned char)(Buf>>8);
//	disbuf[2]=(unsigned char)(Buf);
	
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x03;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x08;
	RSCmdtxBuf[5] = 0x00;
	for(i = 0; i < 6;i++)
	{
		while((at91_usart_get_status(RS232) & 0x02) != 0x02);
		at91_usart_write(RS232,RSCmdtxBuf[i]);
		SendCheckSum = SendCheckSum^RSCmdtxBuf[i];
	}
	//����������ݿ�
	for(i = 0;i < 3;i++)
	{
		while((at91_usart_get_status(RS232) & 0x02) != 0x02);
		at91_usart_write(RS232,disbuf[i]);
		SendCheckSum = SendCheckSum^(disbuf[i]);
	}
	//������ݶ���ʱ��
	WriteDataTxTime();
	
	//����У���
	while((at91_usart_get_status(RS232) & 0x02) != 0x02);
	at91_usart_write(RS232,SendCheckSum);
	
	Modify_LastUploadTime();
	
	//�������Ź�
#if WATCH_DOG_EN
    WD_CR = 0xc071;
    WD_OMR = 0x2343;
#endif
}
*/

//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_SpeedinTwoDays
//* Object                   : �������2���������ڵ���ʻ�ٶȣ���Ҫ���ڴ���
//*                          : ��¼�������ѡ���жϡ��ۼӡ��������������
//*                          : ���2�����������ʻ�ٶ���ݴ���ȫ�ֱ���LargeDataBuffer�У�
//*                          : ���ȷ��͵���ݿ����ٶ���ݶ�Ӧ����ʼʱ�䣨����Buf�У���
//*                          : ����ʻ�ٶȵķ���������ʱ�����ʱ�䣬
//*                          : ������Ϊ0x09
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵΪ�������2������������ʻ�ٶ���ݵ�Ӧ��֡��
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*                          : LargeDataBuffer��������ڴ���ȡ�������2���������ٶ����
//*----------------------------------------------------------------------------
/*void UpLoad_SpeedinTwoDays()
{
	int i,j,TimeInterval,TimeIntervalSum;
	int Nb;
	unsigned char buf[2];
	unsigned long TimeLimit;
	unsigned long curPointer,p;
	OTDR record;
	OTDR_start last_start;
	StructPT spt;
	DateTime BigTime,SmallTime;
	CLOCK TimeBefore2day;
	unsigned char Buf[5];

	//�رտ��Ź�
	#if WATCH_DOG_EN
	WD_OMR = 0x2340;
	#endif	
	unsigned char StartTimeBuf[6];
	unsigned char StopTimeBuf[6];
	int offset;
	spt = pTable.RunRecord360h;

	//�ó�ֵ
	TimeLimit = (BCD2Char(curTime.hour))*60+BCD2Char(curTime.minute)+24*60;
	j = TimeLimit-1;
	TimeIntervalSum = 0;
	curPointer = pTable.RunRecord360h.CurPoint;
	for(i = 0;i < 5;i++)
		Buf[i]=0;
		
	for(i = 0;i<TimeLimit;i++)
		LargeDataBuffer[i] = 0;	
	
	last_start.dt.type = 0;
	
	//�����360Сʱ֮ǰ��ʱ�䲢װ�뻺����
	ComputeTimeBeforeX(&curTime,&TimeBefore2day,TimeLimit);
	Buf[0] = TimeBefore2day.year;
	Buf[1] = TimeBefore2day.month;
	Buf[2] = TimeBefore2day.day;
	Buf[3] = TimeBefore2day.hour;
	Buf[4] = TimeBefore2day.minute;
	
	do
	{
		//ȡ����ǰ��¼�ǲ���ȷ��
		if(!GetOTDR(curPointer,&(record.start), &(record.end)))
		{
			offset = -1;
			curPointer = AddPointer(&spt, offset);
			spt.CurPoint = curPointer;
			continue;
		}
		
		if(last_start.dt.type==0xafaf)
			PrepareTime((unsigned char *)(&(last_start.dt.year)),&BigTime);
		else
			PrepareTime((unsigned char *)(&curTime),&BigTime);
		
		//���㵱ǰ��¼����һ����¼֮���ʱ���	
		PrepareTime((unsigned char *)(&(record.end.dt.year)),&SmallTime);
		TimeInterval = HaveTime(BigTime,SmallTime);
		if(TimeInterval < 0)
			break;
		TimeIntervalSum  += TimeInterval;
		if(TimeIntervalSum >=TimeLimit)
			break;
		
		j -= TimeInterval;
		
		Nb = record.end.MinuteNb;
		TimeIntervalSum += Nb;
		i = 0;
		p = AddPointer(&spt, -sizeof(OTDR_end));
		while((j>=0)&&(Nb>0))
		{
			offset = -2;
			GetOTDRDataFromFlash((unsigned short *)p, offset,buf);
			LargeDataBuffer[j] = buf[1];
			j--;
			Nb--;
			if(Nb<=0)
				break;
			if(j>=0){
				LargeDataBuffer[j] = buf[0];
				j--;
				Nb--;
				if(Nb<=0)
					break;	
			}
			i += 2;
			p = AddPointer(&spt, -sizeof(OTDR_end)-i);
		}
		
		if(TimeIntervalSum >=TimeLimit)
			break;
		//�޸�ָ��
		offset = 0 - (sizeof(OTDR_start)+sizeof(OTDR_end)+record.end.MinuteNb);
		curPointer = AddPointer(&spt, offset);
		spt.CurPoint = curPointer;

		last_start = record.start;		
	}while((j>0)&&(pTable.RunRecord360h.CurPoint!=curPointer));
	
	
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x09;
	RSCmdtxBuf[3] = (unsigned char)((TimeLimit+5)>>8);
	RSCmdtxBuf[4] = (unsigned char)(TimeLimit+5);
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0;
	for(i = 0; i < 6;i++)
	{
		while((at91_usart_get_status(RS232) & 0x02) != 0x02);
		at91_usart_write(RS232,RSCmdtxBuf[i]);
		SendCheckSum = SendCheckSum^RSCmdtxBuf[i];
	}
	//����������ݿ�
	for(i = 0;i < 5;i++)
	{
		while((at91_usart_get_status(RS232) & 0x02) != 0x02);
		at91_usart_write(RS232,Buf[i]);
		SendCheckSum = SendCheckSum^Buf[i];
	}
	for(i = 0;i < TimeLimit;i++)
	{
		while((at91_usart_get_status(RS232) & 0x02) != 0x02);
		at91_usart_write(RS232,LargeDataBuffer[i]);
		SendCheckSum = SendCheckSum^LargeDataBuffer[i];
	}
	
	//����У���
	while((at91_usart_get_status(RS232) & 0x02) != 0x02);
	at91_usart_write(RS232,SendCheckSum);
	Modify_LastUploadTime();
	//�������Ź�
	#if WATCH_DOG_EN
    WD_CR = 0xc071;
    WD_OMR = 0x2343;
	#endif
}
*/
//*----------------------------------------------------------------------------
//* Function Name            : TransmitOneOTDR
//* Object                   : ����һ��ƣ�ͼ�ʻ��¼
//* Input Parameters         : record����ָ��ƣ�ͼ�ʻ��¼��ָ��
//*                          : nb������¼�����
//* Output Parameters        : none
//* Global  Variable Quoted  : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*----------------------------------------------------------------------------
void TransmitOneOTDR(OTDR *record,unsigned char nb)
{
	unsigned char i;
	for(i=0;i<18;i++)
	{
		rt_device_write(&uart2_device, 0, &record[nb].end.driver.DriverLisenseCode[i], 1);
		SendCheckSum = SendCheckSum^record[nb].end.driver.DriverLisenseCode[i];
	}
	
	rt_device_write(&uart2_device, 0, &record[nb].start.dt.year, 1);
	SendCheckSum = SendCheckSum^record[nb].start.dt.year;

	rt_device_write(&uart2_device, 0, &record[nb].start.dt.month, 1);
	SendCheckSum = SendCheckSum^record[nb].start.dt.month;

	rt_device_write(&uart2_device, 0, &record[nb].start.dt.day, 1);
	SendCheckSum = SendCheckSum^record[nb].start.dt.day;

	rt_device_write(&uart2_device, 0, &record[nb].start.dt.hour, 1);
	SendCheckSum = SendCheckSum^record[nb].start.dt.hour;

	rt_device_write(&uart2_device, 0, &record[nb].start.dt.minute, 1);
	SendCheckSum = SendCheckSum^record[nb].start.dt.minute;
	
	rt_device_write(&uart2_device, 0, &record[nb].end.dt.year, 1);
	SendCheckSum = SendCheckSum^record[nb].end.dt.year;

	rt_device_write(&uart2_device, 0, &record[nb].end.dt.month, 1);
	SendCheckSum = SendCheckSum^record[nb].end.dt.month;

	rt_device_write(&uart2_device, 0, &record[nb].end.dt.day, 1);
	SendCheckSum = SendCheckSum^record[nb].end.dt.day;

	rt_device_write(&uart2_device, 0, &record[nb].end.dt.hour, 1);
	SendCheckSum = SendCheckSum^record[nb].end.dt.hour;

	rt_device_write(&uart2_device, 0, &record[nb].end.dt.minute, 1);
	SendCheckSum = SendCheckSum^record[nb].end.dt.minute;
}
//*----------------------------------------------------------------------------
//* Function Name            : UpLoad_OverThreeHours
//* Object                   : �������2����������ͬһ��ʻԱ�����ʻʱ�䳬��3Сʱ
//*                          : �����м�¼��ݣ�
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ���,
//*                          : ��ֵ��ͨ��232�ӿڽ����е�ֵ��Ӧ��֡�����͸�PC��
//*                          : SendCheckSum����У��ͣ��õ���������͸�PC��
//* Global  Variable Modified: RSCmdtxBuf[0]��RSCmdtxBuf[5]��������ͼĴ�����
//*                          : ��ֵΪ�������2����������ͬһ��ʻԱ�����ʻʱ�䳬��3Сʱ��Ӧ��֡��
//*                          : SendCheckSum����У��ͣ������͵���ݰ��ֽڽ������
//*----------------------------------------------------------------------------
void UpLoad_OverThreeHours()
{
	unsigned long i;
	unsigned char recordnb;
	unsigned short length;
	OTDR record[16];
	
	//�رտ��Ź�
	#if WATCH_DOG_EN
	WD_OMR = 0x2340;
	#endif	
	recordnb = GetOverTimeRecordIn2Days(record);
	length = recordnb*28;
	
	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0x11;
	RSCmdtxBuf[3] = (unsigned char)(length>>8);
	RSCmdtxBuf[4] = (unsigned char)length;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^0x11^((unsigned char)(length>>8))^((unsigned char)length)^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	//����������ݿ�
	if(recordnb!=0)
	{
		for(i=0;i<recordnb;i++)
		{
			TransmitOneOTDR(&(record[i]),i);
		}
	}
	//����У���
	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
	Modify_LastUploadTime();
	//�������Ź�
	#if WATCH_DOG_EN
    WD_CR = 0xc071;
    WD_OMR = 0x2343;
	#endif
}


//*----------------------------------------------------------------------------
//* Function Name            : Set_DriverCode
//* Object                   : ���ü�ʻԱ���롢��ʻ֤����
//*                          : ������Ϊ0x81(RSCmdrxBuf[2])
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdrxBuf[2]������PC����յ��������֣�ӦΪ0x81
//*                          : DataLengthReceived������PC����յ�����ݳ��ȣ�ӦΪ0x15��
//*                          : �������������ж���ݳ����Ƿ���ȷ
//*                          : RSDatarxBuf������PC����յ��ļ�ʻԱ���롢��ʻ֤�������
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
void Set_DriverAutoInfo(unsigned char *buf,unsigned short lenth)
{
	unsigned long i=0;
	unsigned char *ptr = (unsigned char *)&Parameter.AutoInfodata;
	if(lenth == 0x29)
	{
		RS232SetSuccess(0x82);
		for (i = 0;i<lenth;i++)
		{
			
			ptr[i] = buf[i];
		}
			WriteParameterTable(&Parameter);
	}
}
void Set_Installtime(unsigned char *buf,unsigned short lenth)
{
	unsigned long i=0;
	unsigned char *ptr = (unsigned char *)&Parameter.InstallTime;
	if(lenth == 0x06)
	{
		RS232SetSuccess(0x83);
		for (i = 0;i<lenth;i++)
		{

			ptr[i] = buf[i];
		}
			WriteParameterTable(&Parameter);
	}
}


void Set_SingalStatus(unsigned char *buf,unsigned short lenth)
{
	unsigned long i=0;
	unsigned char *ptr = (unsigned char *)&Parameter.singalname;
	if(lenth == 0x50)
	{
		RS232SetSuccess(0x84);
		for (i = 0;i<lenth;i++)
		{

			ptr[i] = buf[i];
		}
			WriteParameterTable(&Parameter);
	}
}

void Set_Curtime(unsigned char *buf,unsigned short lenth)
{
	unsigned long i=0;
	unsigned char *ptr = (unsigned char *)&curTime;
	if(lenth == 0x06)
	{
		RS232SetSuccess(0xc2);
		for (i = 0;i<lenth;i++)
		{

			ptr[i]= buf[i];
		}
		SetCurrentDateTime(&curTime);
			WriteParameterTable(&Parameter);
	}
}
void Set_PulseCoff(unsigned char *buf,unsigned short lenth)
{
	unsigned long i=0;
	if(lenth == 0x08)
	{
		RS232SetSuccess(0xc3);

		Parameter.PulseCoff =buf[7]+buf[6]*256;

			WriteParameterTable(&Parameter);
	}
}
void Set_StartDistance(unsigned char *buf,unsigned short lenth)
{
	unsigned long i=0,j= 1;
	if(lenth == 4)
	{
		Parameter.StarDistance = 0;
		RS232SetSuccess(0xc4);
		for (i = 4;i>0;i--)
		{

			Parameter.StarDistance =j*BCD2Char(buf[i-1])+Parameter.StarDistance;
			j= 100*j;
		}
		WriteParameterTable(&Parameter);
	}
}
void EnterVercationStatus()
{
	Time6sCnt = 6;
	Verificationstatus = VER_ENTER;
	RS232SetSuccess(0xe0);
}

void EnterMileStatus()
{
	if( Verificationstatus )
	{
		Verificationstatus = VER_MIL;
	}
}

void EnterPulsStatus()
{
	if( Verificationstatus )
	{
		Verificationstatus = VER_PLUS;
	}
	RS232SetSuccess(0xe2);
}
void EnterTimeStatus()
{

	if( Verificationstatus )
	{
		Verificationstatus = VER_TIME;
	}
	RS232SetSuccess(0xe3);
}
void GetBackNormalStatus()
{
	Verificationstatus = VER_NONE;
	RS232SetSuccess(0xe4);
}
//*----------------------------------------------------------------------------
//* Function Name            : Set_RealTime
//* Object                   : ����ʵʱʱ��
//*                          : ������Ϊ0xc2
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdrxBuf[2]������PC����յ��������֣�ӦΪ0x82
//*                          : DataLengthReceived������PC����յ�����ݳ��ȣ�ӦΪ0x06��
//*                          : �������������ж���ݳ����Ƿ���ȷ
//*                          : RSDatarxBuf������PC����յ���ʵʱʱ��
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
void Set_RealTime()
{
	unsigned long i,j;
	unsigned long *p;
	StructPara para;
	unsigned char CorrespondCmdWord;
	CorrespondCmdWord = RSCmdrxBuf[2];
	CLOCK ClockSet;
	
	//�ж���ݿ鳤���Ƿ�Ϊ6�ֽ�
	if(DataLengthReceived == 0x06)
	{
		//�жϽ��յ���ʱ���Ƿ��ںϷ���ݷ�Χ
	    ClockSet.year = RSDatarxBuf[0];
	    ClockSet.month = RSDatarxBuf[1];
	    ClockSet.day = RSDatarxBuf[2];
	    ClockSet.hour = RSDatarxBuf[3];
	    ClockSet.minute = RSDatarxBuf[4];
	    ClockSet.second = RSDatarxBuf[5];   
		if(!IsCorrectCLOCK(&ClockSet))
		{
			RS232SetError();
			return;
		}
		//����������ȷӦ��֡
		RS232SetSuccess(CorrespondCmdWord);

		//���Ƚ�DATAFLASH�ĵ�һ��4K��ķ����Ͳ���?�����ڴ���
		para = Parameter;
	
		//����Ҫ�޸ĵ���ݣ�realtime��д���ڴ����ѱ����4k�е���Ӧλ��
		para.time = ClockSet;
		
		//���ڴ��иĺõĲ����Copy��DATAFLASH��First4k��
		WriteParameterTable(&para);
			
		//��ʱ��оƬ����ʱ��

		SetCurrentDateTime(&ClockSet);

//		SetTimeFlag = 1;
			
	}
	else//��ݿ鳤�ȴ��󣬷���PC��ͨѶ����Ӧ��֡
		RS232SetError();
}




//*----------------------------------------------------------------------------
//* Function Name            : RS232SetError
//* Object                   : ��¼����PC�������ͨѶ��������֡
//*                          : ������Ϊ0xfb
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf�������͵�����ͨѶ����Ӧ��֡
//*                          : SendCheckSum�������͵�У���
//* Global  Variable Modified: RSCmdtxBuf������ֵΪ����ͨѶ����Ӧ��֡
//*                          : SendCheckSum���������͵�Ӧ��֡���ֽ����
//*----------------------------------------------------------------------------
void RS232SetError()
{
	unsigned long i;
	
	//��PC�������ͨѶ����Ӧ��֡
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0xfb;
	RSCmdtxBuf[3] = 0x00;
	SendCheckSum = 0x55^0x7a^0xfb^0x00;	
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 4);
	rt_device_write(&uart2_device, 0, &SendCheckSum, 6);
}

//*----------------------------------------------------------------------------
//* Function Name            : RS232UploadError
//* Object                   : ��¼����PC�������ͨѶ��������֡
//*                          : ������Ϊ0xfa
//* Input Parameters         : none
//* Output Parameters        : none
//* Global  Variable Quoted  : RSCmdtxBuf�������͵�����ͨѶ����Ӧ��֡
//*                          : SendCheckSum�������͵�У���
//* Global  Variable Modified: RSCmdtxBuf������ֵΪ����ͨѶ����Ӧ��֡
//*                          : SendCheckSum���������͵�Ӧ��֡���ֽ����
//*----------------------------------------------------------------------------
void RS232UploadError()
{
	unsigned long i;

	//��PC�������ͨѶ����Ӧ��֡
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = 0xfa;
	RSCmdtxBuf[3] = 0x00;
	SendCheckSum = 0x55^0x7a^0xfa^0x00;	
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 4);
	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
}

//*----------------------------------------------------------------------------
//* Function Name            : RS232SetSuccess
//* Object                   : ��¼����PC���������ȷӦ��֡
//* Input Parameters         : CorrespondCmdWord������PC����յ����������������Ӧ��������
//* Output Parameters        : none
//* Global  Variable Quoted  : CorrespondCmdWord������PC����յ����������������Ӧ��������
//*                          : RSCmdtxBuf�������͵�����ͨѶ��ȷӦ��֡
//*                          : SendCheckSum�������͵�У���
//* Global  Variable Modified: RSCmdtxBuf������ֵΪ����ͨѶ��ȷӦ��֡
//*                          : SendCheckSum���������͵�Ӧ��֡���ֽ����
//*----------------------------------------------------------------------------
void RS232SetSuccess(unsigned char CorrespondCmdWord)
{
	unsigned long i;

	//���ȷ���Ӧ��֡����ʼ��ͷ��������
	RSCmdtxBuf[0] = 0x55;
	RSCmdtxBuf[1] = 0x7a;
	RSCmdtxBuf[2] = CorrespondCmdWord;
	RSCmdtxBuf[3] = 0x00;
	RSCmdtxBuf[4] = 0x00;
	RSCmdtxBuf[5] = 0x00;
	SendCheckSum = 0x55^0x7a^CorrespondCmdWord^0x00^0x00^0x00;
	rt_device_write(&uart2_device, 0, RSCmdtxBuf, 6);
	rt_device_write(&uart2_device, 0, &SendCheckSum, 1);
}

//*----------------------------------------------------------------------------
//* Function Name            : Write4kDataToFlash
//* Object                   : ���ڴ��е�4k����д��FLASH��һ��4K�У�
//*                          : �����д��0��4k������?�������еĳ��ƺŷ����˱仯��
//*                          : �ͽ���ݴ洢����2��255��4kȫ������ͬʱ���·����
//* Input Parameters         : PageNb������д���4k����ݴ洢���е�ҳ�ţ�0��1��
//* Output Parameters        : "1"����д��ɹ�
//*                          : "0"����д��ʧ��
//* Global  Variable Quoted  : ep2_bufr����USB�豸���յ���һ��4k
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
int Write4kDataToFlash(unsigned char PageNb)
{
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned char *Buffer;
	StructPara old_para,new_para;
	
	unsigned long *sector_addr = (unsigned long *)(DATAFLASH_BASE + 0x1000*PageNb);
	unsigned long *word_addr = (unsigned long *)(DATAFLASH_BASE + 0x1000*PageNb);

	if(PageNb==0)
		old_para =Parameter;

	SPI_FLASH_Sector4kErase(SPI1,*sector_addr);
	
	//�ڴ��е�һ��4k�������д��FLASH
	Buffer = (unsigned char *)ep2_bufr;
	SPI_FLASH_BufferWrite(SPI1, Buffer,*word_addr,4096);

	if(PageNb==0)
	{
	    CLOCK clockset;
	    clockset=Parameter.time;

		SetCurrentDateTime(&clockset);
		
		new_para = Parameter;
		
	//	while((old_para.AutoCode[j]==new_para.AutoCode[j])&&(j<12))
	//		j++;
		
		//����ƺ����б仯���Ͳ���DATAFLASH��2��255����
		if(j < 12)	
		{
			DisplayEraseDataFlash();
			for(i=2;i<=255;i++)
			{
#ifdef WATCH_DOG_EN
				WD_CR = 0xc071;//reload watchdog
#endif
				sector_addr = (unsigned long *)(i<<12);
				SPI_FLASH_Sector4kErase(SPI1,*sector_addr);
//				if(!flash_sst39_erase_sector((unsigned long *)DATAFLASH_BASE, (unsigned long *)sector_addr))
//					return(0);
			}
			InitializeTable(1,0,1);
			PulseTotalNumber = 0;//��ǰ�������	
			//����־	
			InRecordCycle=0;	//�Ƿ��ڼ�¼��ݹ����
			InFlashWriting=0;	//��FLASHд������
			FinishFlag=0;


			lcd_clear(lineall);
			DisplayNormalUI();
		}	
	}		
	return(1);
}		
//*----------------------------------------------------------------------------
//* Function Name            : Modify_LastUploadTime
//* Object                   : ���ڴ��е�4k����д��FLASH��һ��4K�У�
//*                          : �����д��0��4k������?�������еĳ��ƺŷ����˱仯��
//*                          : �ͽ���ݴ洢����2��255��4kȫ������ͬʱ���·����
//* Input Parameters         : PageNb������д���4k����ݴ洢���е�ҳ�ţ�0��1��
//* Output Parameters        : "1"����д��ɹ�
//*                          : "0"����д��ʧ��
//* Global  Variable Quoted  : ep2_bufr����USB�豸���յ���һ��4k
//* Global  Variable Modified: none
//*----------------------------------------------------------------------------
void Modify_LastUploadTime()
{
	PartitionTable p;
	p = pTable;
	
	p.LastUploadTime = curTime;
	
    WritePartitionTable(&p);
}



void Reply_Schedule(void)
{
	unsigned char i;
	unsigned char sendchecksum = 0;
	switch(Schedule_Result)
	{
		case 1:
			RSCmdrxBuf[0] = 0x55;
			RSCmdrxBuf[1] = 0x7a;
			for(i = 0;i < 16;i++)
			{
				rt_device_write(&uart2_device, 0, &RSCmdtxBuf[i], 1);
				sendchecksum = sendchecksum^RSCmdrxBuf[i];
			}
			rt_device_write(&uart2_device, 0, &sendchecksum, 1);
		break;
		case 2 : 
			RSCmdrxBuf[0] = 0x55;
			RSCmdrxBuf[1] = 0x7a;
			RSCmdrxBuf[2] = 0xce;
			for(i = 0;i < 16;i++)
			{
				rt_device_write(&uart2_device, 0, &RSCmdtxBuf[i], 1);
				sendchecksum = sendchecksum^RSCmdrxBuf[i];
			}
			rt_device_write(&uart2_device, 0, &sendchecksum, 1);
		break;
		case 3 :
			RSCmdrxBuf[0] = 0x55;
			RSCmdrxBuf[1] = 0x7a;
			for(i = 0;i < 16;i++)
			{
				rt_device_write(&uart2_device, 0, &RSCmdtxBuf[i], 1);
				sendchecksum = sendchecksum^RSCmdrxBuf[i];
			}
			rt_device_write(&uart2_device, 0, &sendchecksum, 1);
		break;
		case 4 :
			RSCmdrxBuf[0] = 0x55;
			RSCmdrxBuf[1] = 0x7a;
			RSCmdrxBuf[2] = 0xbd;
			for(i = 0;i < 16;i++)
			{
				rt_device_write(&uart2_device, 0, &RSCmdtxBuf[i], 1);
				sendchecksum = sendchecksum^RSCmdrxBuf[i];
			}
			rt_device_write(&uart2_device, 0, &sendchecksum, 1);
		break;
	}
	Schedule_Result = 0;
#if 0
	uart->uart_device->US_CR = 0x20;
	uart2_device.user_data->uart_device->US_IDR = 0x01;
#endif
	CloseUSART1Time=0;

}

void rs232_handle_application(rt_device_t device)
{

	struct stm32_serial_device* uart = (struct stm32_serial_device*) device->user_data;
	unsigned short Datalenth;
	uint8_t DataCmd;
	CLOCK Startime,Endtime;
	uint16_t Blocklenth;
	if(( uart->int_rx->getcmd) > 0x7f)
	{
		uart->int_rx->getcmd = 0;
		RS232UploadError();

	}
	while (uart->int_rx->getcmd )
	{
		Datalenth = uart->int_rx->rx_buffer[(uart->int_rx->read_index +4) &UART_RX_BUFFER_MAX_SIZE];
		Datalenth = ((uart->int_rx->rx_buffer[(uart->int_rx->read_index +3) &UART_RX_BUFFER_MAX_SIZE])<<8)+Datalenth;
		DataCmd = uart->int_rx->rx_buffer[(uart->int_rx->read_index+2)& UART_RX_BUFFER_MAX_SIZE];
		if ((DataCmd < 0x16)&& (DataCmd > 0x07))
		{
			Startime.year = uart->int_rx->rx_buffer[(uart->int_rx->read_index +6) &UART_RX_BUFFER_MAX_SIZE];
			Startime.month = uart->int_rx->rx_buffer[(uart->int_rx->read_index +7) &UART_RX_BUFFER_MAX_SIZE];
			Startime.day = uart->int_rx->rx_buffer[(uart->int_rx->read_index +8) &UART_RX_BUFFER_MAX_SIZE];
			Startime.hour = uart->int_rx->rx_buffer[(uart->int_rx->read_index +9) &UART_RX_BUFFER_MAX_SIZE];
			Startime.minute = uart->int_rx->rx_buffer[(uart->int_rx->read_index +10) &UART_RX_BUFFER_MAX_SIZE];
			Startime.second = uart->int_rx->rx_buffer[(uart->int_rx->read_index +11) &UART_RX_BUFFER_MAX_SIZE];
			Endtime.year = uart->int_rx->rx_buffer[(uart->int_rx->read_index +12) &UART_RX_BUFFER_MAX_SIZE];
			Endtime.month = uart->int_rx->rx_buffer[(uart->int_rx->read_index +13) &UART_RX_BUFFER_MAX_SIZE];
			Endtime.day = uart->int_rx->rx_buffer[(uart->int_rx->read_index +14) &UART_RX_BUFFER_MAX_SIZE];
			Endtime.hour = uart->int_rx->rx_buffer[(uart->int_rx->read_index +15) &UART_RX_BUFFER_MAX_SIZE];
			Endtime.minute = uart->int_rx->rx_buffer[(uart->int_rx->read_index +16) &UART_RX_BUFFER_MAX_SIZE];
			Endtime.second = uart->int_rx->rx_buffer[(uart->int_rx->read_index +17) &UART_RX_BUFFER_MAX_SIZE];
			Blocklenth =((uart->int_rx->rx_buffer[(uart->int_rx->read_index +18) &UART_RX_BUFFER_MAX_SIZE]) <<8)
					+uart->int_rx->rx_buffer[(uart->int_rx->read_index +19) &UART_RX_BUFFER_MAX_SIZE];

		}
		switch(DataCmd)
		{
			case 0x00 : UpLoad_ExVersion();          break;
			case 0x01 : UpLoad_DriverCode();         break;
			case 0x02 : UpLoad_RealTime();           break;
			case 0x03 : UpLoad_DriverDistance();     break;
			case 0x04 : UpLoad_PulseCoff();               break;
			case 0x05 : UpLoad_DriverAutoInfo();            break;
			case 0x06 : UpLoad_StatusInfo();         break;
			case 0x07 : UpLoad_Type();               break;
			case 0x08 : //UpLoad_BlockData(Startime,Endtime,Blocklenth,0x08);        break;
			case 0x09 : //UpLoad_BlockData(Startime,Endtime,Blocklenth,0x09);     break;
			case 0x10 : //UpLoad_BlockData(Startime,Endtime,Blocklenth,0x10);     break;
			case 0x11 : //UpLoad_BlockData(Startime,Endtime,Blocklenth,0x11);    	   break;
			case 0x12 : //UpLoad_BlockData(Startime,Endtime,Blocklenth,0x12);     break;
			case 0x13 : //UpLoad_BlockData(Startime,Endtime,Blocklenth,0x13);     break;
			case 0x14 : //UpLoad_BlockData(Startime,Endtime,Blocklenth,0x14);    	   break;
			case 0x15 : UpLoad_BlockData(Startime,Endtime,Blocklenth,DataCmd);     break;
			case 0x82 : Set_DriverAutoInfo(&(uart->int_rx->rx_buffer[(uart->int_rx->read_index +6) &UART_RX_BUFFER_MAX_SIZE]),Datalenth); break;//设置车辆信息
			case 0x83 : Set_Installtime(&(uart->int_rx->rx_buffer[(uart->int_rx->read_index +6) &UART_RX_BUFFER_MAX_SIZE]),Datalenth); break;//设置记录仪初始安装日期
			case 0x84 : Set_SingalStatus(&(uart->int_rx->rx_buffer[(uart->int_rx->read_index +6) &UART_RX_BUFFER_MAX_SIZE]),Datalenth); break;//设置状态量配置信息
			case 0xc2 : Set_Curtime(&(uart->int_rx->rx_buffer[(uart->int_rx->read_index +6) &UART_RX_BUFFER_MAX_SIZE]),Datalenth); break;//设置记录仪的时间
			case 0xc3 : Set_PulseCoff(&(uart->int_rx->rx_buffer[(uart->int_rx->read_index +6) &UART_RX_BUFFER_MAX_SIZE]),Datalenth);  break;//设置记录仪的脉冲系数
			case 0xc4 : Set_StartDistance(&(uart->int_rx->rx_buffer[(uart->int_rx->read_index +6) &UART_RX_BUFFER_MAX_SIZE]),Datalenth); break;//设置记录仪的初始里程
			case 0xe0 : EnterVercationStatus(); break;//进入检定状态
			case 0xe1 : EnterMileStatus(); break;//进入里程误差测量
			case 0xe2 : EnterPulsStatus(); break;//进入脉冲系数测量
			case 0xe3 : EnterTimeStatus(); break;//进入时间误差测量
			case 0xe4 : GetBackNormalStatus(); break;//返回正常状态
			default   : RS232UploadError();              break;
		}
		uart->int_rx->read_index = (Datalenth+uart->int_rx->read_index+7)& UART_RX_BUFFER_MAX_SIZE;
		uart->int_rx->getcmd--;
	}
}
void Ver_handle_application()
{
	switch( Verificationstatus )
	{
		case VER_ENTER:
			break;
		case VER_MIL:
			  if( timeflag.Ver1sflag == 1)
			  {
				  timeflag.Ver1sflag = 0;
				  upload_Miledata();
				  //sendthe buff
			  }
			break;
		case VER_PLUS:
			//send the maichong xinhao
			break;

		case VER_TIME:
			//send the time
			break;
		default:break;

	}
}

