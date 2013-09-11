
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include<stm32f10x_tim.h>
#include<application.h>
#include<DataManager.h>
#include<menu.h>
#include<RS232.h>

extern void playVolWarm1();
extern void playVolWarm2();
extern void playVolWarm3();
extern void playVolWarm4();
extern CLOCK curTime;
extern LCDTCB lcd_tcb;
extern SizeData location;
unsigned char CurStatus;
#define SpeedSpace 4

extern unsigned char TimeChange;
extern unsigned char DriverStatus ;
extern unsigned char AlarmFlag;
extern unsigned char DriverRegstatus;
extern Datastatus Datastatusdata;
extern StructPara Parameter;
extern CMD_VER Verificationstatus;
unsigned long CurPulse = 0;
///////////////////////////////////gps data and speed data////////////
unsigned long CurPN = 0;
unsigned long LastPN20ms = 0;
unsigned long LastPN1s = 0;
unsigned long LastPN1min = 0;
unsigned long RsSpeed;
unsigned short CurSpeed = 0;	//当前速度（0.2秒平均速度）
unsigned long Curspeed1s = 0;//1s平均速度
unsigned long Curspeed1min = 0;//1分钟平均速度
unsigned char radionum = 0;

int DeltaSpeed = 0;

////////////////time///////////////////////////////////
unsigned char Time6sCnt;
unsigned char Time30mincnt1;
unsigned char Time30mincnt2;
unsigned char Time30mincnt3;
unsigned char Time30mincnt4;
unsigned char Time10sCnts;
unsigned char Time10sCnte;
unsigned char Time20sCnt1;
unsigned char Time20sCnt2;

unsigned long Distance = 0;


////////////////ic卡data///////////////////////
///////////////////////////////////////////////
unsigned short insertcount; //防止误动作
////////////////////////////////key data////////
//////////////////////////////////////////////
Timeflag timeflag;
TimeCnt  timecnt;
typedef enum KEY_STATUS
{
	KEY_NONE,
	MENU_KEY,
	SELE_UP_KEY,
	SELE_DOWN_KEY,
	ENTER_KEY,
	PRIN_KEY,

}Key_status;
Key_status keyval = KEY_NONE,keyvalbak = KEY_NONE;
unsigned short keyvalcount;
void Getthepluse()
{
	CurPN++;
	Parameter.PulseNumber++;

}
//*----------------------------------------------------------------------------
//* Function Name       : GetStatus
//* Object              : ��ȡ״̬
//* Input Parameters    : none
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      : ״̬
//*----------------------------------------------------------------------------
void GetStatus()
{
	//PA0:点火信号；
	CurStatus |= (7<<GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12));//制动
	CurStatus |= (6<<GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8));//turn left
	CurStatus |= (5<<GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10));//turn right
	CurStatus |= (4<<GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11));//high beam
	CurStatus |= (3<<(~GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)));//low beam
	CurStatus |= (2<<GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9));//door
	CurStatus |= (1<<GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13));//鸣笛
	CurStatus |= (0<<GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));//点火
}


//*----------------------------------------------------------------------------
//* Function Name       : ComputeSpeed
//* Object              : �����ٶ�ֵ
//* Input Parameters    : ������
//* Output Parameters   : none
//* ���õ�ȫ�ֱ���      :
//* �޸ĵ�ȫ�ֱ���      : �������ٶ�ֵ��
//*----------------------------------------------------------------------------
unsigned long ComputeSpeed(unsigned long pulse)
{
	unsigned long p,t,temp_pulse;
	int i;
	unsigned long spe;
	unsigned long T;
	unsigned char PP = 2;
	T = 7200000 / Parameter.PulseCoff;
	T = (T * 10)/ PP;

	
	//spe = T*pulse/(2000);
	spe = T*pulse/(2000);

	if((spe % 10) >= 5)
		spe = spe/10 + 1;
	else
		spe = spe/10;
		
	if(spe>255)
		spe = 255;
	return spe;

}
//#if OpenDoorDeal
//*----------------------------------------------------------------------------
//* Function Name       : GetSpeed
//* Object              : ��ȡ�ٶȺͿ�����״̬
//* Input Parameters    : times��ʾ�ڼ��βɼ�
//* Output Parameters   : �ɼ��Ƿ�ɹ�
//*0.2s执行一次
//*----------------------------------------------------------------------------
void GetSpeedandTime(void)
{
 	unsigned long pulse,timer;
	unsigned long temp;
	unsigned long p,t;
	if(timeflag.Time200msflag == 1)
	{
		if(LastPN20ms == 0)
			pulse = CurPN;
		else
			pulse = CurPN-LastPN20ms;
		timeflag.Time200msflag = 0;
		LastPN20ms = CurPN;
		CurSpeed = ComputeSpeed(pulse);
		DoubltPointHandler();

	}
	if(timeflag.Time1sflag == 1)
	{
		GetCurrentDateTime(&curTime);
		if(( lcd_tcb.ListNb == 0 ) && (lcd_tcb.mode == 0))
		{
			DisplayNormalUI();
		}
		TimeChange |= (0x01<<SECOND_CHANGE);
		if(LastPN1s == 0)
			pulse = CurPN;
		else
			pulse = CurPN-LastPN1s;
		LastPN1s = CurPN;
		timeflag.Time1sflag = 0;
		Curspeed1s = ComputeSpeed(pulse)/5;
		JournalHandle();
	}
	if(timeflag.Time1minflag == 1)
	{
		if(LastPN1min == 0)
			pulse = CurPN;
		else
			pulse = CurPN-LastPN1min;
		LastPN1min = CurPN;
		timeflag.Time1minflag = 0;
		TimeChange |= (0x01<<MINUTE_CHANGE);
		Curspeed1min = ComputeSpeed(pulse)/300;
		temp = BCD2Char(Parameter.DriverDistace)+100*BCD2Char(Parameter.DriverDistace>>8);
		temp = 10000*BCD2Char(Parameter.DriverDistace>>16)+temp;
		temp = 1000000*BCD2Char(Parameter.DriverDistace>>24)+temp;
		temp = Curspeed1min/6+temp;
		Parameter.DriverDistace = Char2BCD(temp%100)+Char2BCD((temp%10000)/100)*256+Char2BCD((temp%1000000)/10000)*65536
								+Char2BCD((temp%100000000)/1000000)*16777216;


	}


	//////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////key handler////////////////
/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void Time3_irg_handler()
{
	extern void keyscanhandler(void);
	  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	  {
	      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	      timecnt.Time1msCnt++;
	      if((timecnt.Time1msCnt )%10 ==0)//10ms扫描一次按键
	      {
	    	  keyscanhandler();
	      }
	      if( timecnt.Time1msCnt >199)
	      {
	    	  timecnt.Time1msCnt = 0;
	    	  timeflag.Time200msflag = 1;

	    	  timecnt.Time200msCnt++;
	    	  if(timecnt.Time200msCnt > 4)
	    	  {
	    		  timeflag.Time1sflag = 1;
	    		  timeflag.Ver1sflag = 1;
	    		  timecnt.Time200msCnt = 0;
	    		  timecnt.Time1sCnt ++;
	    		  if(Time6sCnt)
	    		  {
	    			  Time6sCnt--;
	    			  if((Time6sCnt == 0)&&(Verificationstatus == 1))
	    			  {
	    				  Verificationstatus = 0;
	    			  }

	    		  }
	    		  if(Time10sCnts)
	    		  {
	    			  Time10sCnts--;
					  if(Time10sCnts == 0)
					  {
						  Datastatusdata.keepstart10s = 1;
					  }
	    		  }
	    		  if(Time10sCnte)
				  {
					  Time10sCnte--;
					  if(Time10sCnte == 0)
					  {
						  Datastatusdata.keepstop10s = 1;
					  }
				  }
	    		  if(Time20sCnt1)
				  {
					  Time20sCnt1--;
				  }
	    		  if(Time20sCnt2)
				  {
	    			  Time20sCnt2--;
				  }
	    		  if(timecnt.Time1sCnt >59)
	    		  {
	    			  timeflag.Time1minflag = 1;
	    			  timecnt.Time1sCnt = 0;
					  if(Time30mincnt4)//alarm status handle
					  {
						  if(Time30mincnt4 != 1)
						  {
							  Time30mincnt4--;
						  }
					  }
	    			  timecnt.Time5minCnt++;
	    			  if(timecnt.Time5minCnt>4)
	    			  {
	    				  timecnt.Time5minCnt = 0;
						  if(Time30mincnt1!= 0xff )
						  {
							  if(Time30mincnt1)//alarm status handle
							  {
								  Time30mincnt1--;
								  Time20sCnt1 =20;

								  if(Time30mincnt1 == 0)
								  {
									  playVolWarm1();
									  playVolWarm1();
									  playVolWarm1();
									  Time30mincnt1 = 0xff;

								  }


							  }
						  }
						  if(Time30mincnt2!= 0xff )
						  {
							  if(Time30mincnt2)//alarm status handle
							  {
								  Time30mincnt2--;
								  Time20sCnt2 =20;
								  if(Time30mincnt2 == 0)
								  {
									  playVolWarm2();
									  playVolWarm2();
									  playVolWarm2();
									  Time30mincnt2 = 0xff;

								  }

							  }
						  }
						  if(Time30mincnt3!= 0xff )
						  {
							  if(Time30mincnt3)//alarm status handle
							  {
								  Time30mincnt3--;
								  if(Time30mincnt3 == 0)
								  {
									  playVolWarm3();
									  playVolWarm3();
									  playVolWarm3();
									  Time30mincnt3 = 0xff;
								  }

							  }
						  }

	    			  }


	    		  }
	    	  }
	      }

	  }
}
//////////////////////////////////////////////
//////////////////////////////////////////////
///GPS data////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
unsigned char ASCII2char(unsigned char number)
{
	if((number <58)&&(number>47))
	{
		number = number-48;
	}
	else
	{
		number = 0;
	}
	return number;
}
unsigned long CountTime(CLOCK ctime)
{
	unsigned long timesec;
	timesec = ctime.hour*3600+ctime.minute*60+ctime.second;
	return timesec;
}

void GetTheGPSTime(unsigned char *tbuf)
{
	CLOCK GPStime;
	unsigned long nwtime,gtime;
	unsigned char i,dtime;
	unsigned char *pbuf;
	unsigned char *buf;
	pbuf = &GPStime.hour;
	buf = &curTime.hour;
	for(i= 0;i<3;i++)
	{
		*(pbuf+i) = 16*ASCII2char(tbuf[i*2])+ ASCII2char(tbuf[i*2+1]);
	}
	nwtime =CountTime(curTime);
	gtime = CountTime(GPStime);
	if(gtime>nwtime)
	{
		dtime = gtime-nwtime;
	}
	else
	{
		dtime = nwtime-gtime;
	}
	if((dtime >10) &&(dtime<200))
	{
		for(i= 0;i<3;i++)
		{
			*(buf+i) = (*(pbuf+i));
		}
		//set the time
		SetCurrentDateTime(&curTime);
	}

}
void GetGPSLocation1(unsigned char *tbuf)
{
	unsigned char i;
	unsigned long *pbuf = (unsigned long *)&location.longtitude;
	*pbuf = 0;
	for(i= 0;i<9;i++)
	{
		if(i!=4)
		{
			*pbuf =ASCII2char(tbuf[i])+10*(*pbuf);
		}
	}

}
void GetGPSLocation2(unsigned char *tbuf)
{
	unsigned char i,j= 1;
	unsigned long *pbuf = (unsigned long *)&location.latitude;
	*pbuf = 0;
	for(i= 0;i<10;i++)
	{
		if(i!=5)
		{
			*pbuf =ASCII2char(tbuf[i])+10*(*pbuf);
		}
	}

}
void GetGPSLocation3(unsigned char *tbuf,unsigned short lenth)
{
	unsigned char i,k=0;
	unsigned short *pbuf = (unsigned short *)&location.altitude;
	*pbuf = 0;
	if(tbuf[0] == '-')
	{
				k = 1;
    }
	for(i= k;i<lenth;i++)
	{
		  if(tbuf[i] !='.')
		  {
			*pbuf =ASCII2char(tbuf[i])+10*(*pbuf);
		  }
	}
	if(k== 1)
	{
		*pbuf = 0-*pbuf;
	}

}
//精度1km/h
void GetGPSSpeed(unsigned char *tbuf,unsigned lenth)
{
	unsigned char i;
	RsSpeed = 0;
	for(i = 0;i<lenth;i++)
	{
		if(tbuf[i] == '.')
			break;
		RsSpeed = ASCII2char(tbuf[i])+10*RsSpeed;
	}
}
void GettheSinaldata(unsigned char *tbuf)
{

	radionum = 10*ASCII2char(tbuf[0])+ASCII2char(tbuf[1]);

}
//////////////////////////////////////////////////////
////////////////////////key press handle//////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
void KepPressHandler()
{
	switch(keyval)
	{
	case MENU_KEY:
		MenutKeyHandler();
		keyval = KEY_NONE;
		break;
	case SELE_UP_KEY:
		SelectKeyHandler(1);
		keyval = KEY_NONE;
		break;
	case SELE_DOWN_KEY:
		SelectKeyHandler(0);
		keyval = KEY_NONE;
		break;
	case ENTER_KEY:
		OKKeyHandler();
		keyval = KEY_NONE;
		break;
	case PRIN_KEY:
		if((lcd_tcb.ListNb == 0 )&&(lcd_tcb.mode ==0))
		{
			PrintAllData();
			printer();
			lcd_clear(lineall);
			DisplayNormalUI();
			keyval = KEY_NONE;
		}
	    break;
	default:
		break;
	}

}
void keyscanhandler(void)
{
	unsigned char tempkey = KEY_NONE;
	if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14) == 0)
	{
		tempkey = MENU_KEY;
	}
	else if(GPIO_ReadInputDataBit( GPIOD, GPIO_Pin_15) == 0 )
	{
		tempkey = SELE_UP_KEY;
	}
	else if(GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_6) == 0 )
	{
		tempkey = SELE_DOWN_KEY;
	}
	else if(GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_7) == 0 )
	{
		tempkey = ENTER_KEY;
	}
	if(tempkey)
	{
		if(tempkey == keyvalbak)
		{

			if(keyvalcount !=0xffff)
				keyvalcount++;
			if((keyvalcount ==300)&& (keyvalbak == ENTER_KEY))
			{
				keyval = PRIN_KEY;
			}
		}
		else
		{
			keyvalcount = 1;
			keyvalbak = tempkey;
		}
	}
	else
	{
		if(keyvalcount >15)
		{
			if((keyvalcount >300)&& (keyvalbak == ENTER_KEY))
			{
				if(keyval == KEY_NONE)
				{
					keyval = KEY_NONE;
				}
			}
			else
			{
				keyval = keyvalbak;
			}
			keyvalcount =0;
		}
		keyvalcount = 0;
		keyvalbak = 0;
	}

}
//////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/////////IC 卡数据处理//////////////////////////////////
void GetTheDriverNumber()
{

	I2C_Master_BufferRead(I2C1,
								  0xa0,
	                              32,
	                              18,
	                              Parameter.DriverLisenseCode);
}
void IckaHandler()
{
	if((GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4) == 1)&&(Datastatusdata.keeprestatus ==0))
	{
		insertcount++;
		if(insertcount == 100)
		{
			insertcount = 0;
			Datastatusdata.keeprestatus = 1;
			DriverRegstatus = DRIVER_REG_IN;
			GetTheDriverNumber();
			AlarmFlag &=~ ALARM_NOT_RE;
			Time30mincnt2 = 0;
		}
	}
	else if((Datastatusdata.keeprestatus ==  1) &&(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4) ==0))
	{
		Datastatusdata.keeprestatus = 0;
		DriverRegstatus = DRIVER_REG_OUT;
		DriverStatus &= ~DRIVING_STAR;
		DriverStatus |= DRIVING_STOP_DRIVER;//连续驾驶结束
		DriverStatus &= ~DRIVING_OVERTIME;
		AlarmFlag &= ~ALARM_OVER_TIME;
		Time30mincnt1 = 0;

	}
	else if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4) ==0)
	{
		insertcount = 0;
		if((AlarmFlag&ALARM_NOT_RE)!= ALARM_NOT_RE)
			AlarmFlag |= ALARM_NOT_RE;
	}

}


 
