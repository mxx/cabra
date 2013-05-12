
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include<stm32f10x_tim.h>
#include<application.h>


unsigned char CurStatus;
#define SpeedSpace 4

extern StructPara Parameter;

unsigned long CurPulse = 0;

unsigned long CurPN = 0;
unsigned long LastPN20ms = 0;
unsigned long LastPN1s = 0;
unsigned long LastPN1min = 0;
unsigned long CurSpeed = 0;	//当前速度（0.2秒平均速度）
unsigned long Curspeed1s = 0;//1s平均速度
unsigned long Curspeed1min = 0;//1分钟平均速度
unsigned char radionum = 0;

int DeltaSpeed = 0;

unsigned long Distance = 0;
typedef struct
{
	unsigned char Time200msflag:1;
	unsigned char Time1sflag:1;
	unsigned char Time1minflag:1;
}Timeflag;
typedef struct
{
	unsigned char Time200msCnt;
	unsigned char Time1sCnt:3;
	unsigned char Time1minCnt:5;
}TimeCnt;
Timeflag timeflag;
TimeCnt  timecnt;

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
	unsigned char PP = 8;
	T = 7200000 / Parameter.PulseCoff;
	T = (T * 10)/ PP;

	
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
void GetSpeed(void)
{
 	unsigned long pulse,timer;
	unsigned long temp;
	unsigned long p,t;     //200Ϊ10ms,250Ϊ12.5ms,ͬ��ͷΪ12ms
	if(timeflag.Time200msflag == 1)
	{
		if(LastPN20ms == 0)
			pulse = CurPN;
		else
			pulse = CurPN-LastPN20ms;
		timeflag.Time200msflag = 0;
		LastPN20ms = CurPN;
		CurSpeed = ComputeSpeed(pulse);
	}
	if(timeflag.Time1sflag == 1)
	{
		if(LastPN1s == 0)
			pulse = CurPN;
		else
			pulse = CurPN-LastPN1s;
		LastPN1s = CurPN;
		timeflag.Time1sflag = 0;
		Curspeed1s = ComputeSpeed(pulse);
	}
	if(timeflag.Time1minflag == 1)
	{
		if(LastPN1min == 0)
			pulse = CurPN;
		else
			pulse = CurPN-LastPN1min;
		LastPN1min = CurPN;
		timeflag.Time1minflag = 0;
		Curspeed1min = ComputeSpeed(pulse);
	}


	//////////////////////////////////////////////////////////////////////
}
void Time3_irg_handler()
{

	  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	  {
	      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	      timecnt.Time200msCnt++;
	      if( timecnt.Time200msCnt >199)
	      {
	    	  timecnt.Time200msCnt = 0;
	    	  timeflag.Time200msflag = 1;

	    	  timecnt.Time1sCnt++;
	    	  if(timecnt.Time1sCnt > 4)
	    	  {
	    		  timeflag.Time1sflag = 1;
	    		  timecnt.Time1sCnt = 0;
	    		  timecnt.Time1minCnt ++;
	    		  if(timecnt.Time1minCnt >59)
	    		  {
	    			  timeflag.Time1minflag = 1;
	    			  timecnt.Time1minCnt = 0;
	    		  }
	    	  }
	      }

	  }
}
 
 
