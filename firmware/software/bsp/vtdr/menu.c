#include 	"menu.h"
#include 	"lcd.h"
#include 	"lcd_word_model.h"
#include    "application.h"
#include <stm32f10x.h>
extern unsigned char LargeDataBuffer[28*1024];
extern uint8_t USBH_USR_ApplicationState;
extern CLOCK curTime;

extern unsigned char Time30mincnt3;
extern unsigned char Time20sCnt1;
extern unsigned char Time20sCnt2;
extern PartitionTable pTable;
extern StructPara Parameter;
extern unsigned char radionum;
unsigned char gpssinaldata=0;
extern unsigned long CurSpeed;
extern char ClockVL;
extern unsigned char CurStatus;

extern unsigned char AlarmFlag;

unsigned char DisplayMin= 0;//显示连续驾驶的时间
unsigned char displaystatus = 0;
unsigned char otdr2dayNum;
unsigned char otdr2daydisplaynum = 0;
LCDTCB lcd_tcb;
LCDTCB last_lcd_tcb;
ACTION_TCB act_tcb;
OTDR *OTDR_Array = (OTDR *)(&(LargeDataBuffer[24*1024]));

const FONT_MATRIX * content00[] = {
	auto_che,
	auto_liang,
	auto_ji,
	driver_jia,
	driver_shi,
	driver_yuan,
	xin,
	data_xi,
	NULL
};

const FONT_MATRIX * content01[] = {
	over_chao,
	time_shi,
	driver_jia,
	driver_shi,
	record_ji,
	record_lu,
	NULL	
};//��ʾ��¼���

const FONT_MATRIX * content02[] = {
	print_da,
	print_yin,
	NULL
};//��ӡ

const FONT_MATRIX * content04[] = {
	data_shu,
	data_ju,
	save_bao,
	save_cun,
	to_dao,
	udisk_u,
	udisk_pan,
	NULL
};//��ݱ��浽����

const FONT_MATRIX * content03[] = {
	other_qi,
	other_ta,
	operate_cao,
	operate_zuo,
	NULL
};//�������

const FONT_MATRIX * content30[] = {
	door_jian,
	door_ce,
	door_kai,
	door_guan,
	door_men,
	NULL
};//��⿪����
const FONT_MATRIX * content31[] = {

	door_kai,
	door_guan,
	door_men,
	can,
	data_shu,
	NULL
};//�����Ų���

const FONT_MATRIX * back[] = {
	back_fan,
	back_hui,
	NULL
};//����

const FONT_MATRIX * content10[] = {
	auto_che,
	auto_pai,
	number_hao,
	code_ma,
	NULL
};//���ƺ���

const FONT_MATRIX * content11[] = {
	number_hao,
	auto_pai,
	minute_fen,
	number_lei,
	NULL
};//��ʻԱ����

const FONT_MATRIX * content12[] = {
	driver_jia,
	driver_shi,
	zheng,
	number_hao,
	code_ma,
	NULL
};//��ʻ֤����

const FONT_MATRIX * content13[] = {
	door_mai,
	door_chong,
	xi,
	data_shu,
	NULL
};//��������ϵ��

const FONT_MATRIX * content14[] = {
	status_zhuang,
	status_tai,
	xin,
	number_hao,
	NULL
};//״̬����

const FONT_MATRIX * content15[] = {
	product_chan,
	product_pin,
	version_ban,
	version_ben,
	number_hao,
	NULL
};//��Ʒ�汾��

const FONT_MATRIX * content20[] = {
	every_mei,
	minute_fen,
	minute_zhong,
	auto_che,
	speed_su,
	NULL
};//ÿ���ӳ���

const FONT_MATRIX * content21[] = {
	lian,
	xu,
	driver_jia,
	driver_shi,
	record_ji,
	record_lu,
	NULL
};//�����ʻ��¼

const FONT_MATRIX * content22[] = {
	addup_lei,
	addup_ji,
	all_zong,
	distance_li,
	distance_cheng,
	NULL
};
const FONT_MATRIX * content23[] = {
	over_chao,
	time_shi,
	driver_jia,
	driver_shi,
	record_ji,
	record_lu,

	NULL
};//�޼�¼

const FONT_MATRIX * content40[] ={
	driver_jia,
	driver_shi,
	zheng,
	number_hao,
	code_ma,
	NULL

};
const FONT_MATRIX * content41[] ={
	driver_lian,
	driver_xu,
	driver_jia,
	driver_shi,
	time_kai,
	time_shi2,
	time_shi,
	time_jian,
	NULL

};
const FONT_MATRIX * content42[] ={
		driver_lian,
		driver_xu,
		driver_jia,
		driver_shi,
		time_jie,
		time_su,
		time_shi,
		time_jian,
	NULL

};

const FONT_MATRIX * none[] = {
	none_wu,
	record_ji,
	record_lu,
	NULL
};//�޼�¼

const FONT_MATRIX * being_stat[] = {
	being_zheng,
	being_zai,
	stat_tong,
	stat_ji,
	NULL
};//����ͳ��

const FONT_MATRIX * working_ok[] = {
	work_gong,
	work_zuo,
	being_zheng,
	normal_chang,
	NULL
};

const MENU_NODE list0[4]={
	{
		(FONT_MATRIX **)content00,
		1,
		-1,
		-1,
		NULL
	},
	{
		(FONT_MATRIX **)content01,
		2,
		-1,
		-1,
		NULL
	},
#if 0
	{
		(FONT_MATRIX **)content02,
		-1,
		-1,
		-1,
		PrintAllData
	},
#endif
	{
		(FONT_MATRIX **)content04,
		-1,
		-1,
		-1,
		SaveDatatoUdisk
	},
	{
		(FONT_MATRIX **)content03,
		3,
		-1,
		-1,
		NULL
	},

};
const MENU_NODE list1[6]={
	{
		(FONT_MATRIX **)content10,
		-1,
		0,
		0,
		DisplayAutoCode
	},
	{
		(FONT_MATRIX **)content11,
		-1,
		0,
		0,
		DisplayAutoCodesort
	},
	{
		(FONT_MATRIX **)content12,
		-1,
		0,
		0,
		DisplayDriverCode1
	},
	{
		(FONT_MATRIX **)content13,
		-1,
		0,
		0,
		DisplayMaichongxishu
	},
	{
		(FONT_MATRIX **)content14,
		-1,
		0,
		0,
		Displaystatus
	},
	{
		(FONT_MATRIX **)content15,
		-1,
		0,
		0,
		DisplayProductVersion
	},
};
const MENU_NODE list2[4]={

		{
			(FONT_MATRIX **)content23,
			4,
			1,
			1,
			Display15MinAverageSpeed
		},
};
#if 0
const MENU_NODE list2[3]={
	{
		(FONT_MATRIX **)content20,
		-1,
		0,
		1,
		Display15MinAverageSpeed
	},
	{
		(FONT_MATRIX **)content21,
		-1,
		0,
		1,
		Display2DayOTDR
	},
	{
		(FONT_MATRIX **)content22,
		-1,
		0,
		1,
		DisplayTotalDistance
	},
};
#endif
#if OpenDoorDeal
const MENU_NODE list3[2]={
	{
		(FONT_MATRIX **)content30,
		-1,
		0,
		3,
		JudgeDoorType
	},
	{
		(FONT_MATRIX **)content31,
		-1,
		0,
		3,
		DoorType
	},
};
#else
const MENU_NODE list3[2]={
	{
		(FONT_MATRIX **)content30,
		-1,
		0,
		3,
		NULL
	},
	{
		(FONT_MATRIX **)content31,
		-1,
		0,
		3,
		NULL
	},
};
#endif
const MENU_NODE list4[4]={

		{
			(FONT_MATRIX **)content40,
			-1,
			2,
			0,
			DisplayDriverCode
		},
		{
			(FONT_MATRIX **)content41,
			-1,
			2,
			0,
			DisplayStartTime
		},
		{
			(FONT_MATRIX **)content42,
			-1,
			2,
			0,
			DisplayEndTime
		}

};
NODE_LIST NodeListTable[5] = {
	{
		(MENU_NODE *)list0,
		4
	},
	{
		(MENU_NODE *)list1,
		6
	},
	{
		(MENU_NODE *)list2,
		1
	},
	{
		(MENU_NODE *)list3,
		2
	},
	{
		(MENU_NODE *)list4,
		3
	}
};
void Displaybaocun()
{
	lcd_clear(lineall);
	lcd_write_matrix(line3,0,(FONT_MATRIX *)being_zheng,HAN_ZI);
	lcd_write_matrix(line3,16,(FONT_MATRIX *)being_zai,HAN_ZI);
	lcd_write_matrix(line3,32,(FONT_MATRIX *)save_bao,HAN_ZI);
	lcd_write_matrix(line3,48,(FONT_MATRIX *)save_cun,HAN_ZI);
}

//*----------------------------------------------------------------------------
//* Function Name       : SaveDatatoUdisk
//* Object              : ������ݵ����̣�������ɺ󷵻ص�ͨ������
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : none
//*----------------------------------------------------------------------------
void SaveDatatoUdisk()
{
  // usb_host_init();
   //Udisk_handler();
	if( USBH_USR_ApplicationState == 4)
	{
		Displaybaocun();
		USBH_USR_ApplicationState = 1;
	}
	else
	{
		MenutKeyHandler();
	}
}
//*----------------------------------------------------------------------------
//* Function Name       : PrintAllData
//* Object              : ��ӡ������ݣ�������ɺ󷵻ص�ͨ������
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : none
//*----------------------------------------------------------------------------
void PrintAllData()
{
	lcd_clear(lineall);
	lcd_write_matrix(line3,0,(FONT_MATRIX *)being_zheng,HAN_ZI);
	lcd_write_matrix(line3,16,(FONT_MATRIX *)being_zai,HAN_ZI);
	lcd_write_matrix(line3,32,(FONT_MATRIX *)print_da,HAN_ZI);
	lcd_write_matrix(line3,48,(FONT_MATRIX *)print_yin,HAN_ZI);
}
//*----------------------------------------------------------------------------
//* Function Name       : WriteDataToUDiskMenu
//* Object              : ������ݵ�U�̣�������ɺ󷵻ص�ͨ������
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : none
//*----------------------------------------------------------------------------
void WriteDataToUDiskMenu()
{

	int i;

	lcd_clear(0);
	lcd_write_matrix(line3,0,(FONT_MATRIX *)scan_sao,HAN_ZI);
	lcd_write_matrix(line3,16,(FONT_MATRIX *)scan_miao,HAN_ZI);
	lcd_write_matrix(line3,32,(FONT_MATRIX *)udisk_u,HAN_ZI);
	lcd_write_matrix(line3,48,(FONT_MATRIX *)udisk_pan,HAN_ZI);

	if(Scan_UDisk())		
	{	
		lcd_clear(line3);
		lcd_write_matrix(line3,0,(FONT_MATRIX *)being_zheng,HAN_ZI);
		lcd_write_matrix(line3,16,(FONT_MATRIX *)being_zai,HAN_ZI);
		lcd_write_matrix(line3,32,(FONT_MATRIX *)save_bao,HAN_ZI);
		lcd_write_matrix(line3,64,(FONT_MATRIX *)save_cun,HAN_ZI);
	
		if(!WriteDataToUDisk())
		{
			//ʧ��
			lcd_clear(line3);
			lcd_write_matrix(line3,0,(FONT_MATRIX *)operate_cao,HAN_ZI);
			lcd_write_matrix(line3,16,(FONT_MATRIX *)operate_zuo,HAN_ZI);
			lcd_write_matrix(line3,32,(FONT_MATRIX *)fail_shi,HAN_ZI);
			lcd_write_matrix(line3,48,(FONT_MATRIX *)fail_bai,HAN_ZI);
			for(i=0;i<100000;i++);
		}
				
	}
	
	Modify_LastUploadTime();
	
	
	DisplayNormalUI();
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayAutoCode
//* Object              : ��ʾ���ƺ���
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : act_tcb
//*----------------------------------------------------------------------------
void DisplayAutoCode()
{
	lcd_clear(lineall);
	unsigned short hz ;
	unsigned char j=0,col=0,type,count=0;
	unsigned char buf=Parameter.AutoInfodata.AutoCode[0];
	type = 0;
	while((buf!='\0')&&(col<20)&&(j<12))
	{
		if(buf>127)
		{

			hz = hz+buf;
			if(count == 1)
			{
				lcd_write_matrix(line2,type,AutoCodeHZ2LCM(hz),HAN_ZI);
				type = HAN_ZI+type;
				col++;
				count = 0;
			}
			else
			{
				hz = hz<<8;
				count++;
			}
			j++;
		    buf=Parameter.AutoInfodata.AutoCode[j];
		}
		else
		{
			if(buf<60){
				lcd_write_matrix(line2,type,ASCII2LCM(buf),NUM);
				type = NUM+type;
			}
			else{
				lcd_write_matrix(line2,type,ASCII2LCM(buf),ZIMU);
				type = ZIMU+type;
			}
			col++;
			j++;
			buf=Parameter.AutoInfodata.AutoCode[j];
		}
	}

}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayDriverNumber
//* Object              : ��ʾ˾����
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : act_tcb
//*----------------------------------------------------------------------------
void DisplayAutoCodesort()
{
	lcd_clear(lineall);
		unsigned short hz ;
		unsigned char j=0,col=0,type,count;
		unsigned char buf=Parameter.AutoInfodata.AutoSort[0];
		hz = buf;
		type = 0;
		while((buf!='\0')&&(col<20)&&(j<12))
		{
			if(buf>127)
			{
				hz = hz+buf;
				if(count == 1)
				{
					lcd_write_matrix(line2,type,AutoCodeHZ2LCM(hz),HAN_ZI);
					type = HAN_ZI+type;
					col++;
					count = 0;
				}
				else
				{
					hz = hz<<8;
					count++;
				}
				j++;
				buf=Parameter.AutoInfodata.AutoSort[j];


			}
			else
			{
				if(buf<60){
					lcd_write_matrix(line2,type,ASCII2LCM(buf),NUM);
					type = NUM+type;
				}
				else{
					lcd_write_matrix(line2,type,ASCII2LCM(buf),ZIMU);
					type = ZIMU+type;
				}
				col++;
				j++;
				buf=Parameter.AutoInfodata.AutoSort[j];
			}
		}
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayDriverCode
//* Object              : ��ʾ˾���ʻ֤����
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : act_tcb
//*----------------------------------------------------------------------------
void DisplayDriverCode1()
{
	int i;
	unsigned char buf;
	lcd_clear(lineall);
	for(i=0;i<18;i++)
	{
		buf=Parameter.DriverLisenseCode[i];
		if(buf==0)
			break;
		lcd_write_matrix(line3,i*NUM,ASCII2LCM(buf),NUM);
	}

}
void DisplayDriverCode()
{
	int i;
	unsigned char buf;
	for(i=0;i<18;i++)
	{
		buf=Parameter.DriverLisenseCode[i];
		if(buf==0)
			break;
		lcd_write_matrix(line3,i*NUM,ASCII2LCM(buf),NUM);
	}

}
void displayOverDriver()
{
	unsigned long STOPp;
	unsigned long startime,endtime,readtime;
	unsigned char numofdisplay;
	unsigned short BlockSize;
	unsigned char i = 0,j;
	unsigned char temp_data[6];
	unsigned long startbase = OVERDRV_BASE,endbase = OVERDRV_END;
	otdr2dayNum = 0;
	numofdisplay = otdr2daydisplaynum;
	FONT_MATRIX **p;
	p = (FONT_MATRIX **)content01;

	BlockSize = 50;
	readtime = 0;
	CLOCK    temp_clock;
	STOPp = pTable.OverSpeedRecord.CurPoint;
	endtime = timechange(curTime)-BCD2Char(curTime.hour)*360-BCD2Char(curTime.minute)*60;
	startime = endtime-19440;
	do
	{
		if(STOPp > startbase)
		{
			STOPp = STOPp-BlockSize;
		}
		else
		{
			STOPp = startbase-BlockSize;
		}
		SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
		readtime = timechange(temp_clock) -BCD2Char(temp_clock.hour)*360-BCD2Char(temp_clock.minute)*60;
		otdr2dayNum ++;
	}while((readtime >startime)|| (readtime = startime));
	do{
				lcd_write_matrix(line1,i*HAN_ZI,p[i],HAN_ZI);
				lcd_write_matrix(line2,i*HAN_ZI,p[i],HAN_ZI);
				lcd_write_matrix(line3,i*HAN_ZI,p[i],HAN_ZI);
				lcd_write_matrix(line4,i*HAN_ZI,p[i],HAN_ZI);
				i++;
			}while(p[i]!=NULL);
	if(otdr2dayNum > 3)
	{
		for(j = 0;j<4;j++)
		{
			if( (j ==0 )||(j== 2))
			{
				numofdisplay = numofdisplay+2;
			}
			numofdisplay = numofdisplay+j+1;
			if(numofdisplay >otdr2dayNum )
			{
				numofdisplay = numofdisplay-otdr2dayNum;
			}

			lcd_write_matrix(j*2+1,i*HAN_ZI,BCD2LCM(numofdisplay,0),NUM);
		}

	}
	else
	{
		switch (otdr2dayNum)
		{
		case 0:
			lcd_write_matrix(line1,0,(FONT_MATRIX *)none_wu,HAN_ZI);
			break;
		case 1:
			lcd_clear(line2);
			lcd_clear(line3);
			lcd_clear(line4);
			lcd_write_matrix(line1,i*HAN_ZI,BCD2LCM((numofdisplay+1),0),NUM);
			break;
		case 2:
			lcd_clear(line3);
			lcd_clear(line4);
			lcd_write_matrix(line1,i*HAN_ZI,BCD2LCM((numofdisplay+1),0),NUM);
			if((numofdisplay+2)==otdr2dayNum)
			{
				numofdisplay = 0;
			}
			lcd_write_matrix(line2,i*HAN_ZI,BCD2LCM((numofdisplay+1),0),NUM);
			break;
		case 3:
			lcd_clear(line4);
			numofdisplay++;
			lcd_write_matrix(line1,i*HAN_ZI,BCD2LCM(numofdisplay,0),NUM);
			numofdisplay++;
			if(numofdisplay >otdr2dayNum)
			{
				numofdisplay = numofdisplay-otdr2dayNum;
			}
			lcd_write_matrix(line2,i*HAN_ZI,BCD2LCM(numofdisplay,0),NUM);
			numofdisplay++;
			lcd_write_matrix(line3,i*HAN_ZI,BCD2LCM(numofdisplay,0),NUM);
            break;
		default:
			break;
		}
	}

}
//*----------------------------------------------------------------------------
//* Function Name       : Displaywheel
//* Object              : ��ʾ����ϵ��
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : act_tcb
//*----------------------------------------------------------------------------
void Displaywheel()
{
	act_tcb.type = SHOW;
	act_tcb.LineNumber = 1;
	if(act_tcb.CurLine == act_tcb.LineNumber)
		return;
	
	lcd_clear(line2);
	
	StructPara *para ;
	para = &Parameter;
	unsigned int y;
	unsigned int x=y/10;
	unsigned char i=0;
	unsigned char buf[10];
	do{
		buf[i] = y-x*10;
		y=y/10;
		x=y/10;
		i++;
	}while(y!=0);
	
	i--;
	int k,m=0;
	for(k=i;k>=0;k--){
		lcd_write_matrix(line3,(i-k)*NUM,BCD2LCM(buf[k],0),NUM);
		m++;
	}
	
	act_tcb.IfActionEnd = 1;
	act_tcb.CurLine = 1;
}
void DisplayMaichongxishu()
{
	lcd_clear(lineall);
	lcd_write_matrix(line2,0,(FONT_MATRIX *)door_mai,12);
	lcd_write_matrix(line2,12,(FONT_MATRIX *)door_chong,12);
	lcd_write_matrix(line2,24,(FONT_MATRIX *)xi,12);
	lcd_write_matrix(line2,36,(FONT_MATRIX *)data_shu,12);
	lcd_write_matrix(line3,24,BCD2LCM((Parameter.PulseCoff/1000),0),6);
	lcd_write_matrix(line3,30,BCD2LCM((Parameter.PulseCoff%1000/100 >> 0),0),6);
	lcd_write_matrix(line3,36,BCD2LCM((Parameter.PulseCoff%100)/10 ,0),6);
	lcd_write_matrix(line3,42,BCD2LCM(Parameter.PulseCoff%10,0),6);
}
void Displaystatus()
{
	lcd_clear(lineall);
	lcd_write_matrix(line2,0,(FONT_MATRIX *)status_zhuang,12);
	lcd_write_matrix(line2,12,(FONT_MATRIX *)status_tai,12);
	lcd_write_matrix(line2,24,(FONT_MATRIX *)xin,12);
	lcd_write_matrix(line2,36,(FONT_MATRIX *)number_hao,12);
	lcd_write_matrix(line3,24,BCD2LCM(((CurStatus&0x80 )>> 7),0),6);
	lcd_write_matrix(line3,24,BCD2LCM(((CurStatus&0x40 )>> 6),0),6);
	lcd_write_matrix(line3,24,BCD2LCM(((CurStatus&0x20 )>> 5),0),6);
	lcd_write_matrix(line3,24,BCD2LCM(((CurStatus&0x10) >> 4),0),6);
	lcd_write_matrix(line3,24,BCD2LCM(((CurStatus&0x08)>> 3),0),6);
	lcd_write_matrix(line3,24,BCD2LCM(((CurStatus&0x04)>> 2),0),6);
	lcd_write_matrix(line3,24,BCD2LCM(((CurStatus&0x02)>> 1),0),6);
	lcd_write_matrix(line3,24,BCD2LCM((CurStatus&0x01),0),6);
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayStatusPolarity
//* Object              : ��ʾ״̬����
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : act_tcb
//*----------------------------------------------------------------------------
void DisplayStatusPolarity()
{
	act_tcb.type = SHOW;
	act_tcb.LineNumber = 1;
	if(act_tcb.CurLine == act_tcb.LineNumber)
		return;
	
	lcd_clear(line2);
	
	StructPara *para = &Parameter;
	unsigned short s;
	unsigned short z;
	int j;
	for(j=15;j>=0;j--)
	{
		z=1<<j;
		if((s & z)==0)
			lcd_write_matrix(line3,(19-j)*NUM,(FONT_MATRIX *)digital_0,NUM);
		else
			lcd_write_matrix(line3,(19-j)*NUM,(FONT_MATRIX *)digital_1,NUM);
	}
	
	act_tcb.IfActionEnd = 1;
	act_tcb.CurLine = 1;
}

//*----------------------------------------------------------------------------
//* Function Name       : DisplayProductVersion
//* Object              : ��ʾ��Ʒ�汾��
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : act_tcb
//*----------------------------------------------------------------------------
void DisplayProductVersion()
{
	act_tcb.type = SHOW;
	act_tcb.LineNumber = 1;
	if(act_tcb.CurLine == act_tcb.LineNumber)
		return;
	
	lcd_clear(lineall);
	
	unsigned char col=0;
	lcd_write_matrix(line3,col*NUM,(FONT_MATRIX *)digital_0,NUM);
	lcd_write_matrix(line3,(col+1)*NUM,(FONT_MATRIX *)digital_6,NUM);
	lcd_write_matrix(line3,(col+2)*NUM,(FONT_MATRIX *)charater_point,NUM);
	lcd_write_matrix(line3,(col+3)*NUM,(FONT_MATRIX *)digital_0,NUM);
	lcd_write_matrix(line3,(col+4)*NUM,(FONT_MATRIX *)digital_8,NUM);
	lcd_write_matrix(line3,(col+5)*NUM,(FONT_MATRIX *)charater_point,NUM);
	lcd_write_matrix(line3,(col+6)*NUM,(FONT_MATRIX *)digital_2,NUM);
	lcd_write_matrix(line3,(col+7)*NUM,(FONT_MATRIX *)digital_8,NUM);
	lcd_write_matrix(line3,(col+8)*NUM,(FONT_MATRIX *)charater_point,NUM);
	lcd_write_matrix(line3,(col+9)*NUM,(FONT_MATRIX *)digital_1,NUM);

}

//*----------------------------------------------------------------------------
//* Function Name       : Get15MinuteSpeed
//* Object              : 15�����ڵ�ʱ����ٶ�
//* Input Parameters    : 
//* Output Parameters   : ͣ��ʱ�� ʱ,��,�ٶ�(3*15)
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
int Get15MinAverageSpeed(PrintSpeed *speed)
{
 	OTDR record;
	OTDR_start last_start;
	OTDR_end last_end,last_end_temp;
	CLOCK *cl;
	char temp;
	unsigned long i,Nb,p;
	int TimeInterval;
	unsigned long TimeIntervalSum;
	unsigned long TimeLimit;
	unsigned long curPointer;
	unsigned char buf[2];
	StructPT spt;
	DateTime BigTime,SmallTime,ti;	
	unsigned char StartTimeBuf[6];
	unsigned char StopTimeBuf[6];
	int offset,addup_offset=0;
	int j;
	
	TimeIntervalSum = 0;
	TimeLimit = 15;
	j = 15;	
	for(i = 0;i < 15;i++)
	{
		speed[i].hour = 0;
		speed[i].minute = 0;
		speed[i].speed = 0;
		speed[i].DriverCode = 0;
	}
		
	last_start.dt.type = 0;
	do
	{
		last_end = record.end;		
	
		if(last_start.dt.type==0xafaf)
			PrepareTime((unsigned char *)(&(last_start.dt.year)),&BigTime);
		else
		{
			PrepareTime((unsigned char *)(&(last_end.dt.year)),&BigTime);
			ti = BigTime;
			for(i=0;i<15;i++)
			{
				speed[i].hour = Char2BCD((ti.time)/(60));
				speed[i].minute = Char2BCD(ti.time%60);
				IncreaseTime(&ti, -1);
			}
		}
			
		//���㵱ǰ��¼����һ����¼֮���ʱ���	
		PrepareTime((unsigned char *)(&(record.end.dt.year)),&SmallTime);
		TimeInterval = HaveTime(BigTime,SmallTime);
		if(TimeInterval < 0)
			break;
		TimeIntervalSum  += TimeInterval;
		if(TimeIntervalSum >=TimeLimit)
			break;
			
		j=j-TimeInterval;//panhui,2003,7,12	
		Nb = record.end.MinuteNb;
		TimeIntervalSum += Nb;
		i = 0;
		p = AddPointer(&spt, -sizeof(OTDR_end));
		while((j>=0)&&(Nb>0))
		{
			offset = -2;
			speed[15-j].speed = buf[1];
			speed[15-j].DriverCode = record.end.driver.DriverCode;
			j--;
			Nb--;
			if(Nb<=0)
				break;
			if(j>=0){
				speed[15-j].speed = buf[0];
				speed[15-j].DriverCode = record.end.driver.DriverCode;
				j--;
				Nb--;
				if(Nb<=0)
					break;	
			}
			i += 2;
		}
			
		if(TimeIntervalSum >=TimeLimit)
			break;
		//�޸�ָ��
		offset = (sizeof(OTDR_start)+sizeof(OTDR_end)+record.end.MinuteNb);
		addup_offset += offset;
		offset = 0 - offset;
		curPointer = AddPointer(&spt, offset);
		spt.CurPoint = curPointer;
//		j -= TimeInterval;	
		last_start = record.start;		

	}while((j>=2)&&(addup_offset<200));
	return(15-j);
			
}
void DisplayStartTime()
{

	unsigned long STOPp;
	CLOCK    temp_clock;
	STOPp = pTable.OverSpeedRecord.CurPoint-otdr2daydisplaynum*50;
	SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
	lcd_clear(lineall);
	lcd_write_matrix(line2,0,(FONT_MATRIX *)date_ri,12);
	lcd_write_matrix(line2,12,(FONT_MATRIX *)date_qi,12);
	lcd_write_matrix(line2,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line2,30,BCD2LCM(2,0),6);
	lcd_write_matrix(line2,36,BCD2LCM(0,0),6);
	lcd_write_matrix(line2,42,BCD2LCM(temp_clock.year,1),6);
	lcd_write_matrix(line2,48,BCD2LCM(temp_clock.year,0),6);
	lcd_write_matrix(line2,54,(FONT_MATRIX *)charater_slash,NUM);
	lcd_write_matrix(line2,60,BCD2LCM(temp_clock.month,1),6);
	lcd_write_matrix(line2,66,BCD2LCM(temp_clock.month,0),6);
	lcd_write_matrix(line2,72,(FONT_MATRIX *)charater_slash,NUM);
	lcd_write_matrix(line2,78,BCD2LCM(temp_clock.day,1),6);
	lcd_write_matrix(line2,84,BCD2LCM(temp_clock.day,0),6);

	lcd_write_matrix(line3,0,(FONT_MATRIX *)time_shi,12);
	lcd_write_matrix(line3,12,(FONT_MATRIX *)time_jian,12);
	lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line3,42,BCD2LCM(temp_clock.hour,1),6);
	lcd_write_matrix(line3,48,BCD2LCM(temp_clock.hour,0),6);
	lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line3,60,BCD2LCM(temp_clock.minute,1),6);
	lcd_write_matrix(line3,66,BCD2LCM(temp_clock.minute,0),6);
	lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line3,78,BCD2LCM(temp_clock.second,1),6);
	lcd_write_matrix(line3,84,BCD2LCM(temp_clock.second,0),6);


}
void DisplayEndTime()
{

	unsigned long STOPp;
	CLOCK    temp_clock;
	STOPp = pTable.OverSpeedRecord.CurPoint-otdr2daydisplaynum*50+44;
	SPI_FLASH_BufferRead(SPI1 ,(uint8_t *)&temp_clock ,STOPp, 6);
	lcd_clear(lineall);
	lcd_write_matrix(line2,0,(FONT_MATRIX *)date_ri,12);
	lcd_write_matrix(line2,12,(FONT_MATRIX *)date_qi,12);
	lcd_write_matrix(line2,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line2,30,BCD2LCM(2,0),6);
	lcd_write_matrix(line2,36,BCD2LCM(0,0),6);
	lcd_write_matrix(line2,42,BCD2LCM(temp_clock.year,1),6);
	lcd_write_matrix(line2,48,BCD2LCM(temp_clock.year,0),6);
	lcd_write_matrix(line2,54,(FONT_MATRIX *)charater_slash,NUM);
	lcd_write_matrix(line2,60,BCD2LCM(temp_clock.month,1),6);
	lcd_write_matrix(line2,66,BCD2LCM(temp_clock.month,0),6);
	lcd_write_matrix(line2,72,(FONT_MATRIX *)charater_slash,NUM);
	lcd_write_matrix(line2,78,BCD2LCM(temp_clock.day,1),6);
	lcd_write_matrix(line2,84,BCD2LCM(temp_clock.day,0),6);

	lcd_write_matrix(line3,0,(FONT_MATRIX *)time_shi,12);
	lcd_write_matrix(line3,12,(FONT_MATRIX *)time_jian,12);
	lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line3,42,BCD2LCM(temp_clock.hour,1),6);
	lcd_write_matrix(line3,48,BCD2LCM(temp_clock.hour,0),6);
	lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line3,60,BCD2LCM(temp_clock.minute,1),6);
	lcd_write_matrix(line3,66,BCD2LCM(temp_clock.minute,0),6);
	lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line3,78,BCD2LCM(temp_clock.second,1),6);
	lcd_write_matrix(line3,84,BCD2LCM(temp_clock.second,0),6);


}
//*----------------------------------------------------------------------------
//* Function Name       : Display15MinAverageSpeed
//* Object              : ��ʾͣ��ʱ����ǰ��15����ƽ���ٶ�
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void Display15MinAverageSpeed()
{//��¼ѭ����ʾ
	unsigned char i;
	FONT_MATRIX ** p;
	PrintSpeed *pt;
	pt=(PrintSpeed *)OTDR_Array;
	act_tcb.type = SHOW;
//	act_tcb.LineNumber = 15;
//myw 2003.7.14
		lcd_write_matrix(line2,9*ZIMU,(FONT_MATRIX *)Letter_k,ZIMU);
		lcd_write_matrix(line2,10*ZIMU,(FONT_MATRIX *)Letter_m,ZIMU);
		lcd_write_matrix(line2,13*NUM,(FONT_MATRIX *)charater_slash,NUM);
		lcd_write_matrix(line2,12*ZIMU,(FONT_MATRIX *)Letter_h,ZIMU);
		
//
	if(act_tcb.CurLine == 0)//ȡ��¼
	{
		//��ʾ������ͳ�ơ�
		lcd_clear(line3);
		int i=0;
		p = (FONT_MATRIX **)being_stat;
		do{
			lcd_write_matrix(1,i,p[i],NUM);
			i++;
		}while(p[i]!=NULL);
		act_tcb.LineNumber = Get15MinAverageSpeed(pt);
	}
	if(act_tcb.LineNumber == 0)
	{//��ʾ���޼�¼��
		lcd_clear(line2);
		int i=0;
		p = (FONT_MATRIX **)none;
		do{
			lcd_write_matrix(line3,i*NUM,p[i],NUM);
			i++;
		}while(p[i]!=NULL);
	}
	else{	 

		act_tcb.CurLine ++;
		if(act_tcb.CurLine>15)
			act_tcb.CurLine = 1;
			
		lcd_clear(line2);
		unsigned char col=0;
		//ʱ�䣺ʱ
		lcd_write_matrix(line3,col*NUM,BCD2LCM(pt[act_tcb.CurLine-1].hour,1),NUM);
		lcd_write_matrix(line3,(col+1)*NUM,BCD2LCM(pt[act_tcb.CurLine-1].hour,0),NUM);
		lcd_write_matrix(line3,(col+2)*NUM,(FONT_MATRIX *)digital_,NUM);
		//��
		lcd_write_matrix(line3,(col+3)*NUM,BCD2LCM(pt[act_tcb.CurLine-1].minute,1),NUM);
		lcd_write_matrix(line3,(col+4)*NUM,BCD2LCM(pt[act_tcb.CurLine-1].minute,0),NUM);
		//�ٶ�
		DisplayInteger(pt[act_tcb.CurLine-1].speed,line3,col+8,3);
		
		/*lcd_write_matrix(row,8,(LCD_ZM *)Letter_k);//myw 2003.7.14
		lcd_write_matrix(row,9,(LCD_ZM *)Letter_m);
		lcd_write_matrix(row,12,(LCD_ZM *)charater_slash);
		lcd_write_matrix(row,11,(LCD_ZM *)Letter_h);
		*/
//˾���� 
		DisplayInteger(pt[act_tcb.CurLine-1].DriverCode,line3,19,0);
	}
	

}
//*----------------------------------------------------------------------------
//* Function Name       : Display2DayOTDR
//* Object              : ��ʾ2�������쳬��3Сʱ������ʻ��¼
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void Display2DayOTDR()
{//��¼ѭ����ʾ
	FONT_MATRIX ** p;
	act_tcb.type = SHOW;
	if(act_tcb.CurLine == 0)//ȡ��¼
	{
		//��ʾ������ͳ�ơ�
		lcd_clear(line2);
		int i=0;
		p = (FONT_MATRIX **)being_stat;
		do{
			lcd_write_matrix(line3,i*HAN_ZI,p[i],HAN_ZI);
			i++;
		}while(p[i]!=NULL);
	}

	if(act_tcb.LineNumber == 0)
	{//��ʾ���޼�¼��
		lcd_clear(line2);
		int i=0;
		p = (FONT_MATRIX **)none;
		do{
			lcd_write_matrix(line3,i*HAN_ZI,p[i],HAN_ZI);
			i++;
		}while(p[i]!=NULL);
	}
	else{	 
		act_tcb.CurLine ++;
		if(act_tcb.CurLine>act_tcb.LineNumber)
			act_tcb.CurLine = 1;
		
		lcd_clear(lineall);
		lcd_write_matrix(line2,0,BCD2LCM(Char2BCD(act_tcb.CurLine),1),NUM);
		lcd_write_matrix(line2,NUM,BCD2LCM(Char2BCD(act_tcb.CurLine),0),NUM);
		lcd_write_matrix(line2,2*NUM,(FONT_MATRIX *)digital_,NUM);
		unsigned char col=4;
		//��ʼʱ�䣺��
		lcd_write_matrix(line2,col*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.year,1),NUM);
		lcd_write_matrix(line2,(col+1)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.year,0),NUM);
		//��
		lcd_write_matrix(line2,(col+3)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.month,1),NUM);
		lcd_write_matrix(line2,(col+4)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.month,0),NUM);
		//��
		lcd_write_matrix(line2,(col+6)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.day,1),NUM);
		lcd_write_matrix(line2,(col+7)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.day,0),NUM);
		//ʱ
		lcd_write_matrix(line2,(col+9)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.hour,1),NUM);
		lcd_write_matrix(line2,(col+10)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.hour,0),NUM);
		lcd_write_matrix(line2,(col+11)*NUM,(FONT_MATRIX *)digital_,NUM);
		//��
		lcd_write_matrix(line2,(col+12)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.minute,1),NUM);
		lcd_write_matrix(line2,(col+13)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].start.dt.minute,0),NUM);

		//����ʱ�䣺��
		lcd_write_matrix(line3,col*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.year,1),NUM);
		lcd_write_matrix(line3,(col+1)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.year,0),NUM);
		//��
		lcd_write_matrix(line3,(col+3)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.month,1),NUM);
		lcd_write_matrix(line3,(col+4)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.month,0),NUM);
		//��
		lcd_write_matrix(line3,(col+6)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.day,1),NUM);
		lcd_write_matrix(line3,(col+7)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.day,0),NUM);
		//ʱ
		lcd_write_matrix(line3,(col+9)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.hour,1),NUM);
		lcd_write_matrix(line3,(col+10)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.hour,0),NUM);
		lcd_write_matrix(line3,(col+11)*NUM,(FONT_MATRIX *)digital_,NUM);
		//��
		lcd_write_matrix(line3,(col+12)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.minute,1),NUM);
		lcd_write_matrix(line3,(col+13)*NUM,BCD2LCM(OTDR_Array[act_tcb.CurLine-1].end.dt.minute,0),NUM);
			
		
	}
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayTotalDistance
//* Object              : ��ʾ�����
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void DisplayTotalDistance()
{
	act_tcb.type = SHOW;
	act_tcb.LineNumber = 1;
	if(act_tcb.CurLine == act_tcb.LineNumber)
		return;
	
	lcd_clear(line2);

	unsigned long x;
	unsigned long y;
	unsigned char i=0;
	unsigned char buf[10];

	do{
		buf[i] = y-x*10;
		y=y/10;
		x=y/10;
		i++;
	}while(y!=0);
	
	i--;
	unsigned char m=15;
	lcd_write_matrix(line3,m*NUM,BCD2LCM(buf[0],0),NUM);
	m--;
	lcd_write_matrix(line3,m*NUM,(FONT_MATRIX *)charater_point,NUM);
	m--;
	unsigned char k;
	for(k=1;k<=i;k++){
		lcd_write_matrix(line3,m*NUM,BCD2LCM(buf[k],0),NUM);
		m--;
	}
	
	lcd_write_matrix(line3,8*HAN_ZI,(FONT_MATRIX *)km_gong,HAN_ZI);
	lcd_write_matrix(line3,9*HAN_ZI,(FONT_MATRIX *)distance_li,HAN_ZI);

	act_tcb.IfActionEnd = 1;
	act_tcb.CurLine = 1;
}
void DisplayWarnReg()
{
	lcd_write_matrix(line4,0,(FONT_MATRIX *)bank,12);
	lcd_write_matrix(line4,12,(FONT_MATRIX *)bank,12);
	lcd_write_matrix(line4,24,(FONT_MATRIX *)bank,12);
	lcd_write_matrix(line4,36,(FONT_MATRIX *)Reg_qing,HAN_ZI);
	lcd_write_matrix(line4,48,(FONT_MATRIX *)Reg_deng,HAN_ZI);
	lcd_write_matrix(line4,60,(FONT_MATRIX *)Reg_lu,HAN_ZI);
	lcd_write_matrix(line4,72,(FONT_MATRIX *)Reg_shen,HAN_ZI);
	lcd_write_matrix(line4,84,(FONT_MATRIX *)Reg_fen,HAN_ZI);
	lcd_write_matrix(line4,96,(FONT_MATRIX *)bank,12);
	lcd_write_matrix(line4,108,(FONT_MATRIX *)bank,12);

}

//*----------------------------------------------------------------------------
//* Function Name       : SelectKeyHandler
//* Object              : ��ʾ��ǰ�˵����
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void DisplayCurrentNode()
{
	int i,j;
	j=0;
	FONT_MATRIX ** p1[4];

	lcd_clear(lineall);
	if(lcd_tcb.ListNb == 2)
	{
		//特殊处理
		displayOverDriver();
	}
	else
	{
		p1[0]=NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].content;
		if (NodeListTable[lcd_tcb.ListNb].NodeNumber<4)
		{
			for (i = 0;i<NodeListTable[lcd_tcb.ListNb].NodeNumber-1;i++)
			{
				if(lcd_tcb.NodeNb+i<NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
					p1[i+1]=NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb+1+i].content;
				else if((lcd_tcb.NodeNb+i) == NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
					p1[i+1]=NodeListTable[lcd_tcb.ListNb].ListPt[0].content;
				else if ((lcd_tcb.NodeNb+i) >  NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
					p1[i+1]=NodeListTable[lcd_tcb.ListNb].ListPt[((lcd_tcb.NodeNb+i+1)-NodeListTable[lcd_tcb.ListNb].NodeNumber)].content;
			}
		}
		else
		{
			for (i = 0;i<3;i++)
			{
				if(lcd_tcb.NodeNb+i<NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
					p1[i+1]=NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb+1+i].content;
				else if((lcd_tcb.NodeNb+i) == NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
					p1[i+1]=NodeListTable[lcd_tcb.ListNb].ListPt[0].content;
				else if ((lcd_tcb.NodeNb+i) >  NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
					p1[i+1]=NodeListTable[lcd_tcb.ListNb].ListPt[((lcd_tcb.NodeNb+i+1)-NodeListTable[lcd_tcb.ListNb].NodeNumber)].content;
			}
		}
		switch (i)
		{
			case 3:
				do{
					lcd_write_matrix(line4,j*HAN_ZI,p1[3][j],HAN_ZI);
					j++;
				}while(p1[3][j]!=NULL);
				j= 0;
			case 2:
				do{
					lcd_write_matrix(line3,j*HAN_ZI,p1[2][j],HAN_ZI);
						j++;
					}while(p1[2][j]!=NULL);
				j = 0;
			case 1:
				do{
					lcd_write_matrix(line2,j*HAN_ZI,p1[1][j],HAN_ZI);
					j++;
				}while(p1[1][j]!=NULL);
				j =0;
			case 0:
				do{
					lcd_write_matrix(line1,j*HAN_ZI,p1[0][j],HAN_ZI);
					j++;
				}while(p1[0][j]!=NULL);
			break;
				default:
			break;
	

		}
	}
	//��ʾָʾ��־
	lcd_write_matrix(line1,18*NUM,(FONT_MATRIX *)charater_arrow1,NUM);
	lcd_write_matrix(line1,19*NUM,(FONT_MATRIX *)charater_arrow2,NUM);
	i = 0;
}
//*----------------------------------------------------------------------------
//* Function Name       : MenutKeyHandler
//* Object              : ��ѡ�񡱼������
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void MenutKeyHandler()
{
	switch (lcd_tcb.mode )
	{
		case Normal://
				last_lcd_tcb.mode = lcd_tcb.mode;
				last_lcd_tcb.ListNb = lcd_tcb.ListNb;
				last_lcd_tcb.NodeNb = lcd_tcb.NodeNb;
				last_lcd_tcb.KeepTime = lcd_tcb.KeepTime;

				lcd_tcb.mode = Node;
				lcd_tcb.ListNb = 0;
				lcd_tcb.NodeNb = 0;
				DisplayCurrentNode();
				break;
		case Action:
			lcd_tcb.mode = ActLeaf;
			DisplayCurrentNode();
			//go back to upper
			break;
		case Node:
		case BackLeaf:
		case ActLeaf:
			last_lcd_tcb.mode = lcd_tcb.mode;
			last_lcd_tcb.ListNb = lcd_tcb.ListNb;
			last_lcd_tcb.NodeNb = lcd_tcb.NodeNb;
			last_lcd_tcb.KeepTime = lcd_tcb.KeepTime;
			if(lcd_tcb.ListNb == 0 )
			{
				DisplayNormalUI();
				if(lcd_tcb.mode != Normal)
				{
					lcd_tcb.mode = Normal;
				}

			}
			else
			{
				lcd_tcb.ListNb = NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].FatherList;
				lcd_tcb.NodeNb = NodeListTable[last_lcd_tcb.ListNb].ListPt[last_lcd_tcb.NodeNb].FatherNB;;
				lcd_tcb.mode = Node;
				if(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].ChildrenList == -1)
					lcd_tcb.mode = ActLeaf;
				if((lcd_tcb.mode == ActLeaf)
					&&(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].handler == NULL))
				lcd_tcb.mode = BackLeaf;
				DisplayCurrentNode();

			}

			break;


	}
}
//*----------------------------------------------------------------------------
//* Function Name       : SelectKeyHandler
//* Object              : ��ѡ�񡱼������
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void SelectKeyHandler( unsigned char  dir)
{

		lcd_tcb.KeepTime = 0;

		switch(lcd_tcb.mode)
		{

			case Node:
			case BackLeaf:
			case ActLeaf:
				last_lcd_tcb.mode = lcd_tcb.mode;
				last_lcd_tcb.ListNb = lcd_tcb.ListNb;
				last_lcd_tcb.NodeNb = lcd_tcb.NodeNb;
				last_lcd_tcb.KeepTime = lcd_tcb.KeepTime;
				last_lcd_tcb.enter = lcd_tcb.enter;

				if(dir == 1)
				{
					if(lcd_tcb.NodeNb==0)
						lcd_tcb.NodeNb=NodeListTable[lcd_tcb.ListNb].NodeNumber-1;
					else
						lcd_tcb.NodeNb--;
					lcd_tcb.mode = Node;
					if(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].ChildrenList == -1)
						lcd_tcb.mode = ActLeaf;
					if((lcd_tcb.mode == ActLeaf)
						&&(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].handler == NULL))
					lcd_tcb.mode = BackLeaf;
					if(lcd_tcb.ListNb == 2)
					{
						if(otdr2dayNum)
						{
							if(otdr2daydisplaynum  )
								otdr2daydisplaynum --;
							else
								otdr2daydisplaynum = otdr2dayNum-1;
						}
					}
				}
				else
				{

					if(lcd_tcb.NodeNb<NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
						lcd_tcb.NodeNb++;
					else if(lcd_tcb.NodeNb==NodeListTable[lcd_tcb.ListNb].NodeNumber-1)
						lcd_tcb.NodeNb = 0;
					lcd_tcb.mode = Node;
					if(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].ChildrenList == -1)
						lcd_tcb.mode = ActLeaf;
					if((lcd_tcb.mode == ActLeaf)
						&&(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].handler == NULL))
						lcd_tcb.mode = BackLeaf;
					if(lcd_tcb.ListNb == 1)
					{
						if(otdr2daydisplaynum == otdr2dayNum)
							otdr2daydisplaynum --;
						else
							otdr2daydisplaynum = otdr2dayNum;
					}
					if(lcd_tcb.ListNb == 2)
					{
						if(otdr2dayNum)
						{
							otdr2daydisplaynum ++;
							if( otdr2daydisplaynum == otdr2dayNum)
							{
								otdr2daydisplaynum = 0;
							}

						}
					}
				}
				DisplayCurrentNode();
				break;
			default:

				break;
		}
}
//*----------------------------------------------------------------------------
//* Function Name       : OKKeyHandler
//* Object              : ��ȷ�ϡ��������
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void OKKeyHandler()
{
	short TemplistNb;

	lcd_tcb.KeepTime = 0;
	switch(lcd_tcb.mode)
	{
		case Node://
			last_lcd_tcb = lcd_tcb;
			lcd_tcb.ListNb = NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].ChildrenList;
			if((lcd_tcb.ListNb == 4) && (otdr2dayNum == 0))
			{
				lcd_tcb.ListNb = -1;
			}
			lcd_tcb.NodeNb = 0;
			lcd_tcb.mode = Node;
			if(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].ChildrenList == -1)
				lcd_tcb.mode = ActLeaf;
			if((lcd_tcb.mode == ActLeaf)
				&&(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].handler == NULL))
				lcd_tcb.mode = BackLeaf;
			if(lcd_tcb.ListNb == 2)
			{
				otdr2daydisplaynum = 0;
			}

			DisplayCurrentNode();
			break;
		case BackLeaf://
			last_lcd_tcb = lcd_tcb;

			if(NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].FatherList==-1)
			{
				DisplayNormalUI();
			}
			else{
				lcd_tcb.mode = Node;
				TemplistNb = lcd_tcb.ListNb;
				lcd_tcb.ListNb = NodeListTable[TemplistNb].ListPt[lcd_tcb.NodeNb].FatherList;
				lcd_tcb.NodeNb = NodeListTable[TemplistNb].ListPt[lcd_tcb.NodeNb].FatherNB;
				if(lcd_tcb.ListNb == 2)
				{
					otdr2daydisplaynum = 0;
				}
				DisplayCurrentNode();
			}
			break;
		case ActLeaf://
			last_lcd_tcb = lcd_tcb;
			
			lcd_tcb.mode = Action;
			act_tcb.IfActionEnd = 0;
			act_tcb.CurLine = 0;
			NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].handler();
			break;
		case Action:
			if(act_tcb.type == SHOW)
				NodeListTable[lcd_tcb.ListNb].ListPt[lcd_tcb.NodeNb].handler();
			else if(act_tcb.IfActionEnd)
			{
				last_lcd_tcb = lcd_tcb;
				lcd_tcb.mode = Normal;
				DisplayNormalUI();
			}
			break;
		default  : 
			last_lcd_tcb = lcd_tcb;
			lcd_tcb.mode = Normal;
			DisplayNormalUI();
			break;
	}
}
//*----------------------------------------------------------------------------
//* Function Name       : ASCII2LCM
//* Object              : ASCII��ת��Ϊ��ģָ��
//* Input Parameters    : data������ת������ĸ
//* Output Parameters   : ��ģָ��
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
FONT_MATRIX *ASCII2LCM(unsigned char data)
{
	FONT_MATRIX *ret;
	
	switch(data)
	{
		case 48:
			ret=(FONT_MATRIX *)digital_0;break;
		case 49:
			ret=(FONT_MATRIX *)digital_1;break;
		case 50:
			ret=(FONT_MATRIX *)digital_2;break;
		case 51:
			ret=(FONT_MATRIX *)digital_3;break;
		case 52:
			ret=(FONT_MATRIX *)digital_4;break;
		case 53:
			ret=(FONT_MATRIX *)digital_5;break;
		case 54:
			ret=(FONT_MATRIX *)digital_6;break;
		case 55:
			ret=(FONT_MATRIX *)digital_7;break;
		case 56:
			ret=(FONT_MATRIX *)digital_8;break;
		case 57:
			ret=(FONT_MATRIX *)digital_9;break;
		
		case 65:
			ret=(FONT_MATRIX *)Letter_A;break;
		case 66:
			ret=(FONT_MATRIX *)Letter_B;break;
		case 67:
			ret=(FONT_MATRIX *)Letter_C;break;
		case 68:
			ret=(FONT_MATRIX *)Letter_D;break;
		case 69:
			ret=(FONT_MATRIX *)Letter_E;break;
		case 70:
			ret=(FONT_MATRIX *)Letter_F;break;
		case 71:
			ret=(FONT_MATRIX *)Letter_G;break;
		case 72:
			ret=(FONT_MATRIX *)Letter_H;break;
		case 73:
			ret=(FONT_MATRIX *)Letter_I;break;
		case 74:
			ret=(FONT_MATRIX *)Letter_J;break;
		case 75:
			ret=(FONT_MATRIX *)Letter_K;break;
		case 76:
			ret=(FONT_MATRIX *)Letter_L;break;
		case 77:
			ret=(FONT_MATRIX *)Letter_M;break;
		case 78:
			ret=(FONT_MATRIX *)Letter_N;break;
		case 79:
			ret=(FONT_MATRIX *)Letter_O;break;
		case 80:
			ret=(FONT_MATRIX *)Letter_P;break;
		case 81:
			ret=(FONT_MATRIX *)Letter_Q;break;
		case 82:
			ret=(FONT_MATRIX *)Letter_R;break;
		case 83:
			ret=(FONT_MATRIX *)Letter_S;break;
		case 84:
			ret=(FONT_MATRIX *)Letter_T;break;
		case 85:
			ret=(FONT_MATRIX *)Letter_U;break;
		case 86:
			ret=(FONT_MATRIX *)Letter_V;break;
		case 87:
			ret=(FONT_MATRIX *)Letter_W;break;
		case 88:
			ret=(FONT_MATRIX *)Letter_X;break;
		case 89:
			ret=(FONT_MATRIX *)Letter_Y;break;
		case 90:
			ret=(FONT_MATRIX *)Letter_Z;break;
			
		case 97:
			ret=(FONT_MATRIX *)Letter_a;break;
		case 98:
			ret=(FONT_MATRIX *)Letter_b;break;
		case 99:
			ret=(FONT_MATRIX *)Letter_c;break;
		case 100:
			ret=(FONT_MATRIX *)Letter_d;break;
		case 101:
			ret=(FONT_MATRIX *)Letter_e;break;
		case 102:
			ret=(FONT_MATRIX *)Letter_f;break;
		case 103:
			ret=(FONT_MATRIX *)Letter_g;break;
		case 104:
			ret=(FONT_MATRIX *)Letter_h;break;
		case 105:
			ret=(FONT_MATRIX *)Letter_i;break;
		case 106:
			ret=(FONT_MATRIX *)Letter_j;break;
		case 107:
			ret=(FONT_MATRIX *)Letter_k;break;
		case 108:
			ret=(FONT_MATRIX *)Letter_l;break;
		case 109:
			ret=(FONT_MATRIX *)Letter_m;break;
		case 110:
			ret=(FONT_MATRIX *)Letter_n;break;
		case 111:
			ret=(FONT_MATRIX *)Letter_o;break;
		case 112:
			ret=(FONT_MATRIX *)Letter_p;break;
		case 113:
			ret=(FONT_MATRIX *)Letter_q;break;
		case 114:
			ret=(FONT_MATRIX *)Letter_r;break;
		case 115:
			ret=(FONT_MATRIX *)Letter_s;break;
		case 116:
			ret=(FONT_MATRIX *)Letter_t;break;
		case 117:
			ret=(FONT_MATRIX *)Letter_u;break;
		case 118:
			ret=(FONT_MATRIX *)Letter_v;break;
		case 119:
			ret=(FONT_MATRIX *)Letter_w;break;
		case 120:
			ret=(FONT_MATRIX *)Letter_x;break;
		case 121:
			ret=(FONT_MATRIX *)Letter_y;break;
		case 122:
			ret=(FONT_MATRIX *)Letter_z;break;
		default:
			ret=(FONT_MATRIX *)space;break;
	}
	return ret;
}
//*----------------------------------------------------------------------------
//* Function Name       : AutoCodeHZ2LCM
//* Object              : ���ƺ��еĺ���ת��Ϊ��ģָ��
//* Input Parameters    : data������ת���ĺ���
//* Output Parameters   : ��ģָ��
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
FONT_MATRIX *AutoCodeHZ2LCM(unsigned short data)
{
	FONT_MATRIX *ret;


	switch(data)
	{
		case 0xbea9:
			ret = (FONT_MATRIX *)ch_jing1;//{"京}
			break; 
		case 0xbba6:
			ret = (FONT_MATRIX *)ch_hu;//{"��"}
			break; 
		case 0xbdf2:
			ret = (FONT_MATRIX *)ch_jin1;//{"��"}
			break; 
		case 0xcbd5:
			ret = (FONT_MATRIX *)ch_su;//{"��"}
			break; 
		case 0xcdee:
			ret = (FONT_MATRIX *)ch_wan;//{"��"}
			break; 
		case 0xb8d3:
			ret = (FONT_MATRIX *)ch_gan4;//{"��"}
			break; 
		case 0xc3f6:
			ret = (FONT_MATRIX *)ch_min;//{"��"}
			break; 
		case 0xc2b3:
			ret = (FONT_MATRIX *)ch_lu;//{"³"}
			break; 
		case 0xd5e3:
			ret = (FONT_MATRIX *)ch_zhe;//{"��"}
			break; 
		case 0xbdfa:
			ret = (FONT_MATRIX *)ch_jin4;//{"��"}
			break; 
		case 0xbcbd:
			ret = (FONT_MATRIX *)ch_ji4;//{"��"}
			break; 
		case 0xd4a5:
			ret = (FONT_MATRIX *)ch_yu4;//{"ԥ"}
			break; 
		case 0xc3c9:
			ret = (FONT_MATRIX *)ch_meng;//{"��"}
			break; 
		case 0xd0c2:
			ret = (FONT_MATRIX *)ch_xin;//{"��"}
			break; 
		case 0xc4fe:
			ret = (FONT_MATRIX *)ch_ning;//{"��"}
			break; 
		case 0xc1c9:
			ret = (FONT_MATRIX *)ch_liao;//{"��"}
			break; 
		case 0xbada:
			ret = (FONT_MATRIX *)ch_hei;//{"��"}
			break; 
		case 0xbcaa:
			ret = (FONT_MATRIX *)ch_ji2;//{"��"}
			break; 
		case 0xcfe6:
			ret = (FONT_MATRIX *)ch_xiang;//{"��"}
			break; 
		case 0xb6f5:
			ret = (FONT_MATRIX *)ch_e;//{"��"}
			break; 
		case 0xb9f0:
			ret = (FONT_MATRIX *)ch_gui;//{"��"}
			break; 
		case 0xd4c1:
			ret = (FONT_MATRIX *)ch_yue;//{"��"}
			break; 
		case 0xc7ed:
			ret = (FONT_MATRIX *)ch_qiong;//{"��"}
			break; 
		case 0xb2d8:
			ret = (FONT_MATRIX *)ch_zang;//{"��"}
			break; 
		case 0xc9c2:
			ret = (FONT_MATRIX *)ch_shan;//{"��"}
			break; 
		case 0xb8ca:
			ret = (FONT_MATRIX *)ch_gan1;//{"��"}
			break; 
		case 0xc7e0:
			ret = (FONT_MATRIX *)ch_qing;//{"��"}
			break; 
		case 0xb4a8:
			ret = (FONT_MATRIX *)ch_chuan;//{"��"}
			break; 
		case 0xc7ad:
			ret = (FONT_MATRIX *)ch_qian;//{"ǭ"}
			break; 
		case 0xd4c6:
			ret = (FONT_MATRIX *)ch_yun;//{"��"}
			break; 
		case 0xbaa3:
			ret = (FONT_MATRIX *)ch_hai3;//{"��"}
			break; 
		case 0xcca8:
			ret = (FONT_MATRIX *)ch_tai;//{"̨"}
			break; 
		case 0xd3e5:
			ret = (FONT_MATRIX *)ch_yu2;//{"��"}
			break; 
		case 0xb8db:
			ret = (FONT_MATRIX *)ch_gang;//{"��"}
			break; 
		case 0xb0c4:
			ret = (FONT_MATRIX *)ch_ao;//{"��"}
			break; 
		case 0xcab9:
			ret = (FONT_MATRIX *)ch_shi;//{"ʹ"}
			break; 
		case 0xbcd7:
			ret = (FONT_MATRIX *)ch_jia;//{"��"}
			break; 
		case 0xd2d2:
			ret = (FONT_MATRIX *)ch_yi;//{"��"}
			break; 
		case 0xb1fb:
			ret = (FONT_MATRIX *)ch_bing;//{"��"}
			break; 
		case 0xb6a1:
			ret = (FONT_MATRIX *)ch_ding;//{"��"}
			break; 
		case 0xceec:
			ret = (FONT_MATRIX *)ch_wu4;//{"��"}
			break; 
		case 0xbcba:
			ret = (FONT_MATRIX *)ch_ji3;//{"��"}
			break; 
		case 0xb8fd:
			ret = (FONT_MATRIX *)ch_geng;//{"��"}
			break; 
		case 0xd0c1:
			ret = (FONT_MATRIX *)ch_xin1;//{"��"}
			break; 
		case 0xd7d3:
			ret = (FONT_MATRIX *)ch_zi;//{"��"}
			break; 
		case 0xb3f3:
			ret = (FONT_MATRIX *)ch_chou;//{"��"}
			break; 
		case 0xd2fa:
			ret = (FONT_MATRIX *)ch_yin;//{"��"}
			break; 
		case 0xc3ae:
			ret = (FONT_MATRIX *)ch_mou;//{"î"}
			break; 
		case 0xb3bd:
			ret = (FONT_MATRIX *)ch_chen;//{"��"}
			break; 
		case 0xcee7:
			ret = (FONT_MATRIX *)ch_wu3;//{"��"}
			break; 
		case 0xceb4:
			ret = (FONT_MATRIX *)ch_wei;//{"δ"}
			break; 
		case 0xc9ea:
			ret = (FONT_MATRIX *)ch_shen;//{"��"}
			break; 
		case 0xd3cf:
			ret = (FONT_MATRIX *)ch_you;//{"��"}
			break; 
		case 0xbaa5:
			ret = (FONT_MATRIX *)ch_hai4;//{"��"}
			break; 
		case 0xc8c9:
			ret = (FONT_MATRIX *)ch_ren;//{"��"}
			break; 
		case 0xbeaf:
			ret = (FONT_MATRIX *)ch_jing3;//{"��"}
			break; 
		case 0xb9f3:
			ret = (FONT_MATRIX *)ch_gui4;//{"��"}
			break;
		case 0xc1ec:
			ret = (FONT_MATRIX *)ch_ling;//{"��"}
			break;
		case 0xd1a7:
			ret = (FONT_MATRIX *)ch_xue;//{"ѧ"}
			break;
		case 0xcad4:
			ret = (FONT_MATRIX *)ch_shi_try;//{"��"}
			break;
		case 0xbeb3:
			ret = (FONT_MATRIX *)ch_jing;//{"��"}
			break;
		default:
			ret = (FONT_MATRIX *)space;
			break;
	}
	return ret;
}
//*----------------------------------------------------------------------------
//* Function Name       : BCD2LCM
//* Object              : BCD��ʽ�����ת��Ϊ��ģָ��
//* Input Parameters    : data������ת����BCD��ʽ���
//*                       type����0������λ��1������λ
//* Output Parameters   : ��ģָ��
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
FONT_MATRIX *BCD2LCM(unsigned char data, unsigned char type)
{
	unsigned char temp;
	FONT_MATRIX *ret;
	if(type)
	{//����λ
		temp = (data & 0xf0) >> 4;
	}
	else
		temp = data & 0x0f;
	
	switch(temp)
	{
		case 0:
			ret = (FONT_MATRIX *)digital_0;break;
		case 1:
			ret = (FONT_MATRIX *)digital_1;break;
		case 2:
			ret = (FONT_MATRIX *)digital_2;break;
		case 3:
			ret = (FONT_MATRIX *)digital_3;break;
		case 4:
			ret = (FONT_MATRIX *)digital_4;break;
		case 5:
			ret = (FONT_MATRIX *)digital_5;break;
		case 6:
			ret = (FONT_MATRIX *)digital_6;break;
		case 7:
			ret = (FONT_MATRIX *)digital_7;break;
		case 8:
			ret = (FONT_MATRIX *)digital_8;break;
		case 9:
			ret = (FONT_MATRIX *)digital_9;break;
		default:
			ret = 0;break;
	}
	return ret;
} 
unsigned char radiotodn(unsigned char rdn)
{
	if(rdn == 0)
	{
		return 0;
	}
	else if(rdn <3)
	{
		return 4;
	}
	else if(rdn <5)
	{
		return 9;
	}
	else if(rdn<8)
	{
		return 14;
	}
	else if(rdn<10)
	{
		return 19;
	}
	else
		return 24;
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayInteger
//* Object              : ��ʾ����
//* Input Parameters    : row��������
//*                       end_column�������һλ������
//*                       Integer��������ʾ������
//*                       len������ʾ��ݵ������
//* Output Parameters   : none
//* Functions called    : 
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : none
//*----------------------------------------------------------------------------
void DisplayInteger(unsigned long integer,LINE_CMD row,unsigned char end_column,unsigned char len)
{
	unsigned long x=integer/10;
	unsigned long y=integer;
	unsigned char i=0;
	unsigned char buf;
	unsigned char m=end_column;

	do{
		buf = y-x*10;
		lcd_write_matrix(row,m*NUM,BCD2LCM(buf,0),NUM);
		y=y/10;
		x=y/10;
		i++;
		m--;
	}while(y!=0);
	
	int k;
	for(k=i;k<len;k++){
		lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)space,NUM);
		m--;
	}
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayFloat
//* Object              : ��ʾһλС��ĸ�����
//* Input Parameters    : row��������
//*                       end_column�������һλ������
//*                       Float��������ʾ������
//*                       len������ʾ��ݵ������
//* Output Parameters   : none
//* Functions called    : 
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : none
//*----------------------------------------------------------------------------
void DisplayFloat(unsigned long Float,LINE_CMD row,unsigned char end_column,unsigned char len)
{
	unsigned long x=Float/10;
	unsigned long y=Float;
	unsigned char i=0,m,k;
	unsigned char buf[10];

	do{
		buf[i] = y-x*10;
		y=y/10;
		x=y/10;
		i++;
	}while(y!=0);
	
	i--;
	m=end_column;
	lcd_write_matrix(row,m*NUM,BCD2LCM(buf[0],0),NUM);
	m--;
	lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)charater_point,NUM);
	m--;
	for(k=1;k<=i;k++){
		lcd_write_matrix(row,m*NUM,BCD2LCM(buf[k],0),NUM);
		m--;
	}
	for(k=i+1;k<len;k++){
		lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)space,NUM);
		m--;
	}
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplayDateTime
//* Object              : ��ʾ����ʱ��
//* Input Parameters    : row��������
//*                       end_column�������һλ������
//*                       flag���������ʾ��־�Ӹ�λ��ʼ
//*                       ���Σ����ʱ�����꣩��1��ʾ��ʾ0��ʾ����ʾ
//* Output Parameters   : none
//* Functions called    : 
//* ���õ�ȫ�ֱ���      : none
//* �޸ĵ�ȫ�ֱ���      : none
//*----------------------------------------------------------------------------
void DisplayDateTime(unsigned char flag,LINE_CMD row,unsigned char column)
{
	unsigned char m=column;
	if(ClockVL)
		lcd_write_matrix(row,(column-1)*NUM,(FONT_MATRIX *)charater_xing,NUM);
	else
		lcd_write_matrix(row,(column-1)*NUM,(FONT_MATRIX *)space,NUM);

	if((flag & 0x20)!=0)
	{	//��
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.year,1),NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.year,0),NUM);m++;
		lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)space,NUM);m++;
	}
	if((flag & 0x10)!=0)
	{	//��
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.month,1),NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.month,0),NUM);m++;
		lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)space,NUM);m++;
	}
	if((flag & 0x08)!=0)
	{	//��
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.day,1),NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.day,0),NUM);m++;
		lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)space,NUM);m++;
	}
	if((flag & 0x04)!=0)
	{	//ʱ
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.hour,1),NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.hour,0),NUM);m++;
	}
	if((flag & 0x02)!=0)
	{	//��
		lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)digital_,NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.minute,1),NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.minute,0),NUM);m++;
	}
	if((flag & 0x01)!=0)
	{	//��
		lcd_write_matrix(row,m*NUM,(FONT_MATRIX *)digital_,NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.second,1),NUM);m++;
		lcd_write_matrix(row,m*NUM,BCD2LCM(curTime.second,0),NUM);m++;
	}
}
//*----------------------------------------------------------------------------
//* Function Name       : DisplaySpeedDimensoin
//* Object              : ��ʾ���ý��棺ʱ����ٶ�
//* Input Parameters    : row������
//*                       column������
//*                       hz_pt������д������ģ����ָ��
//* Output Parameters   : none
//* Functions called    : lcm_write_command��lcm_control��lcm_write_data
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void DisplaySpeedDimensoin(unsigned char start,LINE_CMD row)
{
	lcd_write_matrix(row,start+14,(FONT_MATRIX *)charater_slash,NUM);
	lcd_write_matrix(row,start,(FONT_MATRIX *)Letter_k,ZIMU);
	lcd_write_matrix(row,start+7,(FONT_MATRIX *)Letter_m,ZIMU);
	lcd_write_matrix(row,start+20,(FONT_MATRIX *)Letter_h,ZIMU);

}
#if 0
//*----------------------------------------------------------------------------
//* Function Name       : DisplayNormalUI
//* Object              :
//* Input Parameters    :
//*
//*
//* Output Parameters   : none
//* Functions called    :
//*      :
//*      :
//*----------------------------------------------------------------------------
void DisplayNormalUI()
{
	if(( lcd_tcb.ListNb !=0)||(lcd_tcb.mode !=0))
	{
		lcd_clear(lineall);
	}

	if(AlarmFlag)
	{
		//line1


		if(((AlarmFlag&ALARM_SPEED_ABOR)== ALARM_SPEED_ABOR)&& (Time30mincnt3 >1))
		{
			lcd_write_matrix(line1,60,(FONT_MATRIX *)speed_su,12);
			lcd_write_matrix(line1,72,(FONT_MATRIX *)speed_du,12);
			lcd_write_matrix(line1,84,(FONT_MATRIX *)Reg_yi,12);
			lcd_write_matrix(line1,96,(FONT_MATRIX *)Reg_chang,12);

		}
		if(((AlarmFlag & ALARM_OVER_TIME) == ALARM_OVER_TIME)&&Time20sCnt1)
		{
			DisplayAlarm();
		}
		else if (((AlarmFlag & ALARM_NOT_RE) == ALARM_NOT_RE)&&Time20sCnt2)
		{
			DisplayWarnReg();
		}
		else
		{

			lcd_write_matrix(line1,0,(FONT_MATRIX *)over_chao,12);
			lcd_write_matrix(line1,12,(FONT_MATRIX *)time_shi,12);
			lcd_write_matrix(line1,24,(FONT_MATRIX *)driver_jia,12);
			lcd_write_matrix(line1,36,(FONT_MATRIX *)driver_shi,12);


			//line2
			lcd_write_matrix(line2,0,(FONT_MATRIX *)driver_jia,12);
			lcd_write_matrix(line2,12,(FONT_MATRIX *)driver_shi,12);
			lcd_write_matrix(line2,24,(FONT_MATRIX *)zheng,12);
			lcd_write_matrix(line2,36,(FONT_MATRIX *)number_hao,12);
			lcd_write_matrix(line2,48,(FONT_MATRIX *)code_ma,12);
			lcd_write_matrix(line2,60,(FONT_MATRIX *)charater_twopoint,NUM);
			//line3
			DisplayDriverCode();
			//line3
			lcd_write_matrix(line4,0,(FONT_MATRIX *)driver_jia,HAN_ZI);
			lcd_write_matrix(line4,12,(FONT_MATRIX *)driver_shi,HAN_ZI);
			lcd_write_matrix(line4,24,(FONT_MATRIX *)speed_su,HAN_ZI);
			lcd_write_matrix(line4,36,(FONT_MATRIX *)speed_du,HAN_ZI);
			lcd_write_matrix(line4,48,(FONT_MATRIX *)charater_twopoint,NUM);
			lcd_write_matrix(line4,54,BCD2LCM(CurSpeed/1000,0),NUM);
			lcd_write_matrix(line4,60,BCD2LCM(((CurSpeed%1000)/100),0),NUM);
			lcd_write_matrix(line4,66,BCD2LCM((CurSpeed%100)/10,0),NUM);
			lcd_write_matrix(line4,72,(FONT_MATRIX *)charater_point,NUM);
			lcd_write_matrix(line4,78,BCD2LCM((CurSpeed%10),0),NUM);
			DisplaySpeedDimensoin(84,line4);
		}
	}
	else
	{
		lcd_write_matrix(line1,0,(FONT_MATRIX *)charmapSinal,radiotodn(radionum));
		lcd_write_matrix(line1,30,BCD2LCM(curTime.hour,1),NUM);
		lcd_write_matrix(line1,36,BCD2LCM(curTime.hour,0),NUM);
		lcd_write_matrix(line1,42,(FONT_MATRIX *)charater_twopoint1,NUM);
		lcd_write_matrix(line1,48,BCD2LCM(curTime.minute,1),NUM);
		lcd_write_matrix(line1,54,BCD2LCM(curTime.minute,0),NUM);
		lcd_write_matrix(line1,60,(FONT_MATRIX *)charater_twopoint1,NUM);
		lcd_write_matrix(line1,66,BCD2LCM(curTime.second,1),NUM);
		lcd_write_matrix(line1,72,BCD2LCM(curTime.second,0),NUM);
		lcd_write_matrix(line1,80,(FONT_MATRIX *)Letter_V,ZIMU);
		lcd_write_matrix(line1,91,(FONT_MATRIX *)charmapRadio,16);
		lcd_write_matrix(line1,107,BCD2LCM(radionum,1),NUM);
		lcd_write_matrix(line1,113,BCD2LCM(radionum,0),NUM);
		//line2
		lcd_write_matrix(line2,0,(FONT_MATRIX *)date_ri,HAN_ZI);
		lcd_write_matrix(line2,12,(FONT_MATRIX *)date_qi,HAN_ZI);
		lcd_write_matrix(line2,24,(FONT_MATRIX *)charater_twopoint,NUM);
		lcd_write_matrix(line2,36,(FONT_MATRIX *)digital_2,NUM);
		lcd_write_matrix(line2,42,(FONT_MATRIX *)digital_0,NUM);
		lcd_write_matrix(line2,48,BCD2LCM(curTime.year,1),NUM);
		lcd_write_matrix(line2,54,BCD2LCM(curTime.year,0),NUM);
		lcd_write_matrix(line2,60,(FONT_MATRIX *)charater_slash,NUM);
		lcd_write_matrix(line2,66,BCD2LCM(curTime.month,1),NUM);
		lcd_write_matrix(line2,72,BCD2LCM(curTime.month,0),NUM);
		lcd_write_matrix(line2,78,(FONT_MATRIX *)charater_slash,NUM);
		lcd_write_matrix(line2,84,BCD2LCM(curTime.day,1),NUM);
		lcd_write_matrix(line2,90,BCD2LCM(curTime.day,0),NUM);

		//line3
		lcd_write_matrix(line3,0,(FONT_MATRIX *)speed_su,HAN_ZI);
		lcd_write_matrix(line3,12,(FONT_MATRIX *)speed_du,HAN_ZI);
		lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
		lcd_write_matrix(line3,36,BCD2LCM(CurSpeed/1000,0),NUM);
		lcd_write_matrix(line3,42,BCD2LCM(((CurSpeed%1000)/100),0),NUM);
		lcd_write_matrix(line3,48,BCD2LCM((CurSpeed%100)/10,0),NUM);
		lcd_write_matrix(line3,54,(FONT_MATRIX *)charater_point,NUM);
		lcd_write_matrix(line3,60,BCD2LCM((CurSpeed%10),0),NUM);
		DisplaySpeedDimensoin(66,line3);

		lcd_write_matrix(line4,0,(FONT_MATRIX *)Letter_G,ZIMU);
		lcd_write_matrix(line4,7,(FONT_MATRIX *)Letter_P,ZIMU);
		lcd_write_matrix(line4,14,(FONT_MATRIX *)Letter_S,ZIMU);
		lcd_write_matrix(line4,21,(FONT_MATRIX *)status_zhuang,HAN_ZI);
		lcd_write_matrix(line4,33,(FONT_MATRIX *)status_tai,HAN_ZI);
		lcd_write_matrix(line4,45,(FONT_MATRIX *)charater_twopoint,NUM);

	}
	//line4
} 
#endif
void DisplayNormalUI()
{
	if(( lcd_tcb.ListNb !=0)||(lcd_tcb.mode !=0))
	{
		lcd_clear(lineall);
	}

	lcd_write_matrix(line1,0,(FONT_MATRIX *)charmapSinal,radiotodn(radionum));
	lcd_write_matrix(line1,30,BCD2LCM(curTime.hour,1),NUM);
	lcd_write_matrix(line1,36,BCD2LCM(curTime.hour,0),NUM);
	lcd_write_matrix(line1,42,(FONT_MATRIX *)charater_twopoint1,NUM);
	lcd_write_matrix(line1,48,BCD2LCM(curTime.minute,1),NUM);
	lcd_write_matrix(line1,54,BCD2LCM(curTime.minute,0),NUM);
	lcd_write_matrix(line1,60,(FONT_MATRIX *)charater_twopoint1,NUM);
	lcd_write_matrix(line1,66,BCD2LCM(curTime.second,1),NUM);
	lcd_write_matrix(line1,72,BCD2LCM(curTime.second,0),NUM);
	lcd_write_matrix(line1,84,(FONT_MATRIX *)Letter_V,ZIMU);
	lcd_write_matrix(line1,97,(FONT_MATRIX *)charmapRadio,16);
	lcd_write_matrix(line1,113,BCD2LCM(radionum,1),NUM);
	lcd_write_matrix(line1,119,BCD2LCM(radionum,0),NUM);

//line3
	lcd_write_matrix(line2,0,(FONT_MATRIX *)distance_li,HAN_ZI);
	lcd_write_matrix(line2,12,(FONT_MATRIX *)distance_cheng,HAN_ZI);
	lcd_write_matrix(line2,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line2,30,BCD2LCM(Parameter.DriverDistace>>24,1),NUM);
	lcd_write_matrix(line2,36,BCD2LCM(Parameter.DriverDistace>>24,0),NUM);
	lcd_write_matrix(line2,42,BCD2LCM(Parameter.DriverDistace>>16,1),NUM);
	lcd_write_matrix(line2,48,BCD2LCM(Parameter.DriverDistace>>16,0),NUM);
	lcd_write_matrix(line2,54,BCD2LCM(Parameter.DriverDistace>>8,1),NUM);
	lcd_write_matrix(line2,60,BCD2LCM(Parameter.DriverDistace>>8,0),NUM);
	lcd_write_matrix(line2,66,BCD2LCM(Parameter.DriverDistace,1),NUM);
	lcd_write_matrix(line2,72,(FONT_MATRIX *)charater_point,NUM);
	lcd_write_matrix(line2,78,BCD2LCM(Parameter.DriverDistace,0),NUM);
	lcd_write_matrix(line2,84,(FONT_MATRIX *)Letter_k,ZIMU);
	lcd_write_matrix(line2,91,(FONT_MATRIX *)Letter_m,ZIMU);

	//line3
	lcd_write_matrix(line3,0,(FONT_MATRIX *)speed_su,HAN_ZI);
	lcd_write_matrix(line3,12,(FONT_MATRIX *)speed_du,HAN_ZI);
	lcd_write_matrix(line3,24,(FONT_MATRIX *)charater_twopoint,NUM);
	lcd_write_matrix(line3,30,BCD2LCM(CurSpeed/1000,0),NUM);
	lcd_write_matrix(line3,36,BCD2LCM(((CurSpeed%1000)/100),0),NUM);
	lcd_write_matrix(line3,42,BCD2LCM((CurSpeed%100)/10,0),NUM);
	lcd_write_matrix(line3,48,BCD2LCM((CurSpeed%10),0),NUM);
	DisplaySpeedDimensoin(54,line3);
	if(AlarmFlag)
	{
		if(((AlarmFlag&ALARM_SPEED_ABOR)== ALARM_SPEED_ABOR)&& (Time30mincnt3 >1))
		{
			lcd_write_matrix(line2,110,(FONT_MATRIX *)speed_su,12);
			lcd_write_matrix(line2,122,(FONT_MATRIX *)speed_du,12);
			lcd_write_matrix(line3,110,(FONT_MATRIX *)Reg_yi,12);
			lcd_write_matrix(line3,122,(FONT_MATRIX *)Reg_chang,12);

		}
		else
		{
			lcd_write_matrix(line2,110,(FONT_MATRIX *)bank,12);
			lcd_write_matrix(line2,122,(FONT_MATRIX *)bank,12);
			lcd_write_matrix(line3,110,(FONT_MATRIX *)bank,12);
			lcd_write_matrix(line3,122,(FONT_MATRIX *)bank,12);
		}
		if(((AlarmFlag & ALARM_OVER_TIME) == ALARM_OVER_TIME)&&Time20sCnt1)
		{
			DisplayAlarm();
		}
		else if (((AlarmFlag & ALARM_NOT_RE) == ALARM_NOT_RE)&&Time20sCnt2)
		{
			DisplayWarnReg();
		}
		else
		{
			lcd_write_matrix(line4,0,(FONT_MATRIX *)date_ri,HAN_ZI);
			lcd_write_matrix(line4,12,(FONT_MATRIX *)date_qi,HAN_ZI);
			lcd_write_matrix(line4,24,(FONT_MATRIX *)charater_twopoint,NUM);
			lcd_write_matrix(line4,30,(FONT_MATRIX *)digital_2,NUM);
			lcd_write_matrix(line4,36,(FONT_MATRIX *)digital_0,NUM);
			lcd_write_matrix(line4,42,BCD2LCM(curTime.year,1),NUM);
			lcd_write_matrix(line4,48,BCD2LCM(curTime.year,0),NUM);
			lcd_write_matrix(line4,54,(FONT_MATRIX *)charater_slash,NUM);
			lcd_write_matrix(line4,60,BCD2LCM(curTime.month,1),NUM);
			lcd_write_matrix(line4,66,BCD2LCM(curTime.month,0),NUM);
			lcd_write_matrix(line4,72,(FONT_MATRIX *)charater_slash,NUM);
			lcd_write_matrix(line4,78,BCD2LCM(curTime.day,1),NUM);
			lcd_write_matrix(line4,84,BCD2LCM(curTime.day,0),NUM);
			lcd_write_matrix(line2,90,(FONT_MATRIX *)bank,12);
			lcd_write_matrix(line2,102,(FONT_MATRIX *)bank,12);
			lcd_write_matrix(line2,114,(FONT_MATRIX *)bank,12);

		}
	}
	else
	{
			lcd_write_matrix(line4,0,(FONT_MATRIX *)date_ri,HAN_ZI);
			lcd_write_matrix(line4,12,(FONT_MATRIX *)date_qi,HAN_ZI);
			lcd_write_matrix(line4,24,(FONT_MATRIX *)charater_twopoint,NUM);
			lcd_write_matrix(line4,30,(FONT_MATRIX *)digital_2,NUM);
			lcd_write_matrix(line4,36,(FONT_MATRIX *)digital_0,NUM);
			lcd_write_matrix(line4,42,BCD2LCM(curTime.year,1),NUM);
			lcd_write_matrix(line4,48,BCD2LCM(curTime.year,0),NUM);
			lcd_write_matrix(line4,54,(FONT_MATRIX *)charater_slash,NUM);
			lcd_write_matrix(line4,60,BCD2LCM(curTime.month,1),NUM);
			lcd_write_matrix(line4,66,BCD2LCM(curTime.month,0),NUM);
			lcd_write_matrix(line4,72,(FONT_MATRIX *)charater_slash,NUM);
			lcd_write_matrix(line4,78,BCD2LCM(curTime.day,1),NUM);
			lcd_write_matrix(line4,84,BCD2LCM(curTime.day,0),NUM);
			lcd_write_matrix(line4,90,(FONT_MATRIX *)bank,12);
			lcd_write_matrix(line4,102,(FONT_MATRIX *)bank,12);
			lcd_write_matrix(line4,114,(FONT_MATRIX *)bank,12);
	}

}

//*----------------------------------------------------------------------------
//* Function Name       : DisplayEraseDataFlash
//* Object              : ��ʾ"���ڸ���"
//* Input Parameters    : 
//* Output Parameters   : none
//* Functions called    : 
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------
void DisplayEraseDataFlash()
{
	lcd_clear(lineall);
	lcd_write_matrix(line2,HAN_ZI,(FONT_MATRIX *)being_zheng,HAN_ZI);
	lcd_write_matrix(line2,2*HAN_ZI,(FONT_MATRIX *)being_zai,HAN_ZI);
	lcd_write_matrix(line2,3*HAN_ZI,(FONT_MATRIX *)update_geng,HAN_ZI);
	lcd_write_matrix(line2,4*HAN_ZI,(FONT_MATRIX *)update_xin,HAN_ZI);
	lcd_tcb.mode = Normal;
}
void DisplayOK()
{
	FONT_MATRIX **p;
	int i=0;
	lcd_clear(lineall);
		p = (FONT_MATRIX **)working_ok;
		do{
			lcd_write_matrix(line3,i,p[i],ZIMU);
			i++;
		}while(p[i]!=NULL);

}
void DisplayError()
{
	lcd_clear(lineall);
	lcd_write_matrix(line3,HAN_ZI,(FONT_MATRIX *)error_gu,HAN_ZI);
	lcd_write_matrix(line3,2*HAN_ZI,(FONT_MATRIX *)error_zhang,HAN_ZI);
}
void DisplayClockError()
{
	lcd_clear(lineall);
	lcd_write_matrix(line3,HAN_ZI,(FONT_MATRIX *)time_shi,HAN_ZI);
	lcd_write_matrix(line3,2*HAN_ZI,(FONT_MATRIX *)time_jian,HAN_ZI);
	lcd_write_matrix(line3,3*HAN_ZI,(FONT_MATRIX *)error_cuo,HAN_ZI);
	lcd_write_matrix(line3,4*HAN_ZI,(FONT_MATRIX *)error_wu,HAN_ZI);
}
void Display_Scan_Udisk()
{
	lcd_clear(lineall);
	lcd_write_matrix(line3,HAN_ZI,(FONT_MATRIX *)scan_sao,HAN_ZI);
	lcd_write_matrix(line3,2*HAN_ZI,(FONT_MATRIX *)scan_miao,HAN_ZI);
	lcd_write_matrix(line3,3*HAN_ZI,(FONT_MATRIX *)udisk_u,HAN_ZI);
	lcd_write_matrix(line3,4*HAN_ZI,(FONT_MATRIX *)udisk_pan,HAN_ZI);
}

void Display_Save()
{
	lcd_clear(lineall);
	lcd_write_matrix(line2,0,(FONT_MATRIX *)being_zheng,HAN_ZI);
	lcd_write_matrix(line2,HAN_ZI,(FONT_MATRIX *)being_zai,HAN_ZI);
	lcd_write_matrix(line2,2*HAN_ZI,(FONT_MATRIX *)save_bao,HAN_ZI);
	lcd_write_matrix(line2,3*HAN_ZI,(FONT_MATRIX *)save_cun,HAN_ZI);
	////////////
	lcd_write_matrix ( line2, 19*HAN_ZI, (FONT_MATRIX *)percent,HAN_ZI);
	lcd_write_matrix ( line3, 0, (FONT_MATRIX *)process_bar1,HAN_ZI);
	lcd_write_matrix ( line2, 18*HAN_ZI, (FONT_MATRIX *)digital_1,HAN_ZI);
	
	////////////
	
}

void Display_Fail()
{
	//lcm_clear_ram(LINE2);
	lcd_clear(lineall);
	lcd_write_matrix(line2,HAN_ZI,(FONT_MATRIX *)operate_cao,HAN_ZI);
	lcd_write_matrix(line2,2*HAN_ZI,(FONT_MATRIX *)operate_zuo,HAN_ZI);
	lcd_write_matrix(line2,3*HAN_ZI,(FONT_MATRIX *)fail_shi,HAN_ZI);
	lcd_write_matrix(line2,4*HAN_ZI,(FONT_MATRIX *)fail_bai,HAN_ZI);
}

void Display_udisk_full()
{
	lcd_clear(lineall);
	lcd_write_matrix(line3,HAN_ZI,(FONT_MATRIX *)udisk_u,HAN_ZI);
	lcd_write_matrix(line3,2*HAN_ZI,(FONT_MATRIX *)udisk_pan,HAN_ZI);
	lcd_write_matrix(line3,3*HAN_ZI,(FONT_MATRIX *)udisk_kong,HAN_ZI);
	lcd_write_matrix(line3,4*HAN_ZI,(FONT_MATRIX *)udisk_jian,HAN_ZI);
	lcd_write_matrix(line3,5*HAN_ZI,(FONT_MATRIX *)udisk_yi,HAN_ZI);
	lcd_write_matrix(line3,6*HAN_ZI,(FONT_MATRIX *)udisk_man,HAN_ZI);
}
void DisplayTestDoorFail()
{
	lcd_clear(lineall);

	lcd_write_matrix(line3,2*HAN_ZI,(FONT_MATRIX *)door_jian,HAN_ZI);
	lcd_write_matrix(line3,3*HAN_ZI,(FONT_MATRIX *)door_ce,HAN_ZI);
	lcd_write_matrix(line3,4*HAN_ZI,(FONT_MATRIX *)fail_shi,HAN_ZI);
	lcd_write_matrix(line3,5*HAN_ZI,(FONT_MATRIX *)fail_bai,HAN_ZI);
}
void DisplayTestDoorSucc()
{
	lcd_clear(lineall);
	lcd_write_matrix(line3,2*HAN_ZI,(FONT_MATRIX *)door_jian,HAN_ZI);
	lcd_write_matrix(line3,3*HAN_ZI,(FONT_MATRIX *)door_ce,HAN_ZI);
	lcd_write_matrix(line3,4*HAN_ZI,(FONT_MATRIX *)succ_cheng,HAN_ZI);
	lcd_write_matrix(line3,5*HAN_ZI,(FONT_MATRIX *)succ_gong,HAN_ZI);
}

void DisplayAlarm()
{
	lcd_write_matrix(line4,0,(FONT_MATRIX *)driver_lian,HAN_ZI);
	lcd_write_matrix(line4,1*HAN_ZI,(FONT_MATRIX *)driver_xu,HAN_ZI);
	lcd_write_matrix(line4,2*HAN_ZI,(FONT_MATRIX *)driver_jia,HAN_ZI);
	lcd_write_matrix(line4,3*HAN_ZI,(FONT_MATRIX *)driver_shi,HAN_ZI);
	lcd_write_matrix(line4,4*HAN_ZI,(FONT_MATRIX *)time_shi,HAN_ZI);
	lcd_write_matrix(line4,5*HAN_ZI,(FONT_MATRIX *)time_jian,HAN_ZI);
	lcd_write_matrix(line4,6*HAN_ZI,(FONT_MATRIX *)charater_twopoint,NUM);

	lcd_write_matrix(line4,78,BCD2LCM(DisplayMin/100,0),NUM);
	lcd_write_matrix(line4,84,BCD2LCM(DisplayMin%100/10,0),NUM);
	lcd_write_matrix(line4,90,BCD2LCM(DisplayMin%10,0),NUM);
	lcd_write_matrix(line4,96,(FONT_MATRIX *)minute_fen,HAN_ZI);
	lcd_write_matrix(line4,108,(FONT_MATRIX *)minute_zhong,HAN_ZI);
}
#if OpenDoorDeal
void Display_TestDoor()
{
	//lcm_clear_ram(LINE2);
	lcd_clear(lineall);
	lcd_write_matrix(line3,HAN_ZI,(FONT_MATRIX *)door_jin,HAN_ZI);
	lcd_write_matrix(line3,2*HAN_ZI,(FONT_MATRIX *)door_xing,HAN_ZI);
	lcd_write_matrix(line3,3*HAN_ZI,(FONT_MATRIX *)door_zhong,HAN_ZI);
}

void Display_DoorNB()
{
	lcd_write_matrix(line2,0,(FONT_MATRIX *)door_men,HAN_ZI);
	lcd_write_matrix(line2,2*ZIMU,(FONT_MATRIX *)digital_1,ZIMU);
	lcd_write_matrix(line2,3*ZIMU,(FONT_MATRIX *)space,ZIMU);
	lcd_write_matrix(line3,0,(FONT_MATRIX *)door_men,HAN_ZI);
	lcd_write_matrix(line3,2*ZIMU,(FONT_MATRIX *)digital_2,ZIMU);
	lcd_write_matrix(line3,3*ZIMU,(FONT_MATRIX *)space,ZIMU);
}

void Display_Test_No_Door(unsigned char doorNB)
{
	LINE_CMD raw;
	if(doorNB==1)
		raw =line2;
	if(doorNB==2)
		raw = line3;
	lcd_write_matrix(raw,2*HAN_ZI,(FONT_MATRIX *)door_wei,HAN_ZI);
	lcd_write_matrix(raw,3*HAN_ZI,(FONT_MATRIX *)door_jian,HAN_ZI);
	lcd_write_matrix(raw,4*HAN_ZI,(FONT_MATRIX *)door_ce,HAN_ZI);
	lcd_write_matrix(raw,5*HAN_ZI,(FONT_MATRIX *)door_dao,HAN_ZI);
}

void Display_DoubleKeyLevel(unsigned char doorNB)
{
	LINE_CMD raw;
	if(doorNB==1)
		raw = line2;
	if(doorNB==2)
		raw = line3;
	lcd_write_matrix(raw,2*HAN_ZI,(FONT_MATRIX *)door_shuang,HAN_ZI);
	lcd_write_matrix(raw,3*HAN_ZI,(FONT_MATRIX *)door_jian_key,HAN_ZI);
	lcd_write_matrix(raw,4*HAN_ZI,(FONT_MATRIX *)door_dian,HAN_ZI);
	lcd_write_matrix(raw,5*HAN_ZI,(FONT_MATRIX *)door_ping,HAN_ZI);
}

void Display_SingleKeyLevel(unsigned char doorNB)
{
	LINE_CMD raw;
	if(doorNB==1)
		raw = line2;
	if(doorNB==2)
		raw = line3;
	lcd_write_matrix(raw,2*HAN_ZI,(FONT_MATRIX *)door_dan,HAN_ZI);
	lcd_write_matrix(raw,3*HAN_ZI,(FONT_MATRIX *)door_jian_key,HAN_ZI);
	lcd_write_matrix(raw,4*HAN_ZI,(FONT_MATRIX *)door_dian,HAN_ZI);
	lcd_write_matrix(raw,5*HAN_ZI,(FONT_MATRIX *)door_ping,HAN_ZI);
}

void Display_DoubleKeyPulse(unsigned char doorNB)
{
	LINE_CMD raw;
	if(doorNB==1)
		raw = line2;
	if(doorNB==2)
		raw = line3;
	lcd_write_matrix(raw,2*HAN_ZI,(FONT_MATRIX *)door_shuang,HAN_ZI);
	lcd_write_matrix(raw,3*HAN_ZI,(FONT_MATRIX *)door_jian_key,HAN_ZI);
	lcd_write_matrix(raw,4*HAN_ZI,(FONT_MATRIX *)door_mai,HAN_ZI);
	lcd_write_matrix(raw,5*HAN_ZI,(FONT_MATRIX *)door_chong,HAN_ZI);
}

void Display_SingleKeyPulse(unsigned char doorNB)
{
	LINE_CMD raw;
	if(doorNB==1)
		raw = line2;
	if(doorNB==2)
		raw = line3;
	lcd_write_matrix(raw,2*HAN_ZI,(FONT_MATRIX *)door_dan,HAN_ZI);
	lcd_write_matrix(raw,3*HAN_ZI,(FONT_MATRIX *)door_jian_key,HAN_ZI);
	lcd_write_matrix(raw,4*HAN_ZI,(FONT_MATRIX *)door_mai,HAN_ZI);
	lcd_write_matrix(raw,5*HAN_ZI,(FONT_MATRIX *)door_chong,HAN_ZI);
}

//*----------------------------------------------------------------------------
//* Function Name       : DoorType
//* Object              : ��ʾ������״̬
//* Input Parameters    : 
//* Output Parameters   : none
//* Functions called    : 
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      :
//*----------------------------------------------------------------------------

void DoorType()
{
	unsigned char raw;
	act_tcb.type = SHOW;
	act_tcb.LineNumber = 1;
	if(act_tcb.CurLine == act_tcb.LineNumber)
		return;
	

	
	lcd_clear(lineall);
	Display_DoorNB();

	act_tcb.IfActionEnd = 1;
	act_tcb.CurLine = 1;
}
#endif
