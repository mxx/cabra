#ifndef application_h
#define application_h

#define TRUE 1
#define FALSE 0


#define GetSpeedSelf    	0
#define POWERON_LINE_EN     0
#define StatusPol           0
#define StationStamp_EN     0
#define guizhoufile    	    0
#define guizhou             1
#define GPS                 0


#define OpenDoorDeal    	1
#define RTC8025             1     //ʱ��оƬѡ�񿪹�
#define GetSpeedStatusBy232 0     //ͨ��ں�������ͨѶ��ȡ�ٶȺ�ȫ��״̬����
//#define WATCH_DOG_EN    	1     // ���Ź�����
#define RPM_EN				0     //������ת�ٿ���
#define SectionAlarm_EN     0     //��·�α�������
#define OpenDoorAlarm		0     //������ʻ��������
#define Test                0	  //���Թ�װ
#define Status14			0     //����8��״̬
//**************************************//
#define AlarmRecord         0     //���ٺ�ƣ�ͼ�ʻ������¼
////////////////////////////////////////////////////////////////////////

//2004.03.02 panhui �人�������߸�Ϊ2Сʱ���п�����ʻ������
#if OpenDoorAlarm
#define GotoSleepTime 36000     //����ʱ�䣽120����
#else
#define GotoSleepTime 1500     //����ʱ�䣽300�룽5����
#endif

#define DoorNBLimit   6       //200ms�п����ź���Ч�ĸ�������

//�����ж����ȼ�
#define USART1_INT_PRI	4
#define IRQ0_INT_PRI	7       //USB
#define IRQ1_INT_PRI    3       //ENGINE SPEED
#define IRQ2_INT_PRI    6       //SPEED
#define TC0_INT_PRI     0       //TICK
#define Clock_PRI     	5       //CLOCK 
#define DriverCard_PRI 	5		//DRIVERCARD
#define TC2_INT_PRI     6       //��ʱ��
#define TC1_INT_PRI     5       //door��ʱ��


typedef struct
{

	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	
} CLOCK;
typedef struct
{

	int year;
	int month;
	int day;
	int time;
	
} DateTime;

typedef struct
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char type;
	unsigned char reserved;
} UpDownCLOCK;

typedef struct
{

	unsigned short type;
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	
} Record_CLOCK;

typedef struct
{
	unsigned char speed;
	unsigned long bound;
} RoadSection;
typedef struct
{
	unsigned char AutoCode[17];
	unsigned char AutoVincode[12];
	unsigned char AutoSort[12];

}AutoInfo;
typedef struct
{
	unsigned char d0[10];
	unsigned char d1[10];
	unsigned char d2[10];
	unsigned char d3[10];
	unsigned char d4[10];
	unsigned char d5[10];
	unsigned char d6[10];
	unsigned char d7[10];
	unsigned char d8[10];
}StatusSingal;
typedef struct
{
	unsigned char productccc[7];
	unsigned char type[16];
	unsigned char productdate[3];
	unsigned char productline[4];
	unsigned char reserve[5];
}ProductType;
typedef struct
{

	unsigned short mark;//*�����֡���2
	unsigned char  sn[22];//��Ʒ���кš���22(24)
	unsigned char standeryear;
	unsigned char modifyNb;
	AutoInfo AutoInfodata;
	unsigned char signalstatus;
	StatusSingal singalname;
	ProductType  typedata;
	unsigned char  DriverLisenseCode[18];//��ʻ֤���롪��20(108)
	unsigned char  AlarmSound;//��������ѡ��0x00-����0xFF-������1(114)
	CLOCK   time;
	CLOCK   InstallTime;
	unsigned long  DriverDistace;
	unsigned long  StarDistance;
	unsigned char  PulseNumber;
	unsigned short  PulseCoff;

	unsigned char SectionNumber;
	RoadSection section[20];

	unsigned char  reserved1[92];//Ϊ�˱�֤��վ������·�߷ֶ����ôӵ�ַ0x200��ʼд
	unsigned char SectionNumber1;
	RoadSection section1[20];
} StructPara;

typedef struct
{
	unsigned long  DriverCode;//��ʻԱ���롪��4(88)
	unsigned char  DriverLisenseCode[18];//��ʻ֤���롪��20(108)
} DRIVER;

typedef struct
{

	unsigned long  BaseAddr;
	unsigned long  EndAddr;//�����ַ
	unsigned long  CurPoint;//��ǰָ���ַ,ָ����һ����ݿɴ�ŵ�λ��
	unsigned long  BakPoint;
	
} StructPT;

typedef struct
{

	unsigned short  Available;//=0��ʾû��ʹ�ô������>0��ʾ�������Ч
	StructPT DoubtPointData;// 疑点数据
	StructPT LocationData;
	StructPT OverSpeedRecord;//超速记录
	StructPT PowerOffRunRecord;
	StructPT ModifyRecord;
	StructPT BaseData;//行驶速度记录
	StructPT DriverReRecord;
	StructPT journalRecord;

	unsigned long	 TotalDistance;
	CLOCK	 LastUploadTime;
	unsigned long	 DriverCode;//��ʻԱ����
	unsigned char   DriverLisenseCode[20];//��ʻ֤����
} PartitionTable;

/* ���������Ч�ָ�λ���� */
#define DOUBTPOINTDATA  	0
#define OVERSPEEDRECORD    	1
#define POWEROFFRUNRECORD   2
#define DOUBLTPOINT 		3
#define BASEDATA 			4
#define LOCATIONDATA  		5
#define RPMDATA  			6
#define JOURNALDATA  	   7
#define OILPRESSDATA 		8
#define STATUS14DATA        9
/*  */
#define DoubtDataBlockSize 210

/* ������ݷ���� */
#define DOUBTPOINTDATA_EN  		1

#if AlarmRecord
#define OVERSPEEDRECORD_EN    	1
#else
#define OVERSPEEDRECORD_EN    	0
#endif

#define POWEROFFRUNRECORD_EN   	0	
#define RUNRECORD360h_EN 		1
#define BASEDATA_EN 			1
#define OILDATA_EN  			0

#if RPM_EN
#define RPMDATA_EN  			1
#else
#define RPMDATA_EN				0
#endif

#if Status14
#define Status14DATA_EN  			1
#else
#define Status14DATA_EN				0
#endif

#define TEMPERATUREDATA_EN  	0
#define OILPRESSDATA_EN 		0

#define PartitionTableFlag  (DOUBTPOINTDATA_EN << DOUBTPOINTDATA)|(OVERSPEEDRECORD_EN << OVERSPEEDRECORD)|(RUNRECORD360h_EN << DOUBLTPOINT)|(BASEDATA_EN << BASEDATA)|(RPMDATA_EN << RPMDATA)|(Status14DATA_EN << STATUS14DATA)
#define DATAFLASH_BASE    0x00000000
#define PARAMETER_BASE    (DATAFLASH_BASE+0x00000000)
#define PartitionTable_BASE (DATAFLASH_BASE+0x00001000)

#define BASEDATA_BASE DATAFLASH_BASE+0x02000 //48h data 126byte/block
#define BASEDATA_END  DATAFLASH_BASE+0x164fff

#define DPD_BASE      DATAFLASH_BASE+0x165000 //100条
#define DPD_END       DATAFLASH_BASE+0x17bfff

#define OVERDRV_BASE  DATAFLASH_BASE+0x17c000 //100条
#define OVERDRV_END   DATAFLASH_BASE+0x180fff

#define LOCATION_BASE DATAFLASH_BASE+0x181000//360个小时
#define LOCATION_END  DATAFLASH_BASE+0x26bfff

#define DRVRG_BASE    DATAFLASH_BASE+0x26c000//200条
#define DRVRG_END     DATAFLASH_BASE+0x270fff

#define POWER_BASE    DATAFLASH_BASE+0x271000//100条
#define POWER_END     DATAFLASH_BASE+0x271fff

#define PARA_BASE     DATAFLASH_BASE+0x272000//100条
#define PARA_END      DATAFLASH_BASE+0x272fff

#define JN_BASE       DATAFLASH_BASE+0x273000//10条
#define JN_END    	  DATAFLASH_BASE+0x274fff

//2004.07.23
#define DOWNLOADTIME_BASE DATAFLASH_BASE+0x00400
#define DOWNLOADTIME_END DATAFLASH_BASE+0x007f4
#define UPLOADTIME_BASE DATAFLASH_BASE+0x00800
#define UPLOADTIME_END DATAFLASH_BASE+0x00bf4
#define SETCHCOTIME_BASE DATAFLASH_BASE+0x00c00
#define SETCHCOTIME_END DATAFLASH_BASE+0x00ff4
//2004.07.23

#define DoubtDataSpace 110 

/*�����ɵ���ݸ�ʽ*/
typedef struct{
	unsigned char speed;//ÿ0.2���ٶ�
	unsigned char status;//ÿ0.2��8λ״̬
}DoubtData;
typedef struct{
	unsigned char longtitude[4];
	unsigned char latitude[4];
	unsigned char altitude[2];
}SizeData;
typedef struct{
	unsigned char  DriverLisenseCode[18];
	CLOCK   StopTime;
	DoubtData data[100];
	SizeData vaildlocation;
}DoubtDataBlock;
typedef struct{
	CLOCK basedataclk;
	unsigned char speed[60];
	unsigned char status[60];
}BaseDataBlock;
typedef struct{
	unsigned char  DriverLisenseCode[18];
	CLOCK startdrivertime;
	CLOCK enddrivertime;
	SizeData startlocation;
	SizeData endlocation;
}OverDriverBlock;
typedef struct{
	unsigned char  DriverLisenseCode[18];
	CLOCK happentime;
	unsigned char registerstatus;

}DriverRegisterBlock;
typedef struct{
	CLOCK powertime;
	unsigned char powerstatus;
}PowerBlock;
typedef struct{
	CLOCK modifytime;
	unsigned char modifystatus;
}ParaModifyBlock;
typedef struct{
	unsigned char speedstatus;
	CLOCK journalstartime;
	CLOCK jouranlendtime;
	unsigned char speed[60];
	unsigned char rspeed[60];
}JournalBlock;

/*����ƣ�ͼ�ʻ��ݸ�ʽ*/
//#define DriveMinuteLimit 5
//#define RestMinuteLimit 1
//#define DriveMinuteLimit 180
//#define RestMinuteLimit 20
typedef struct{
	Record_CLOCK  dt;
}OTDR_start;
typedef struct{
	Record_CLOCK  dt;
	unsigned long  TotalDistance;
	unsigned long MinuteNb;
	DRIVER driver;
}OTDR_end;
typedef struct{
	OTDR_start start;
	OTDR_end end;
}OTDR;

typedef struct{
	Record_CLOCK  dt;
	DRIVER driver;
}AnAlarmData;

/*����״̬�ֽ���ÿλ�ĺ���*/
#define DOOR	   8
#define AIRPRESS   7
#define STATION    6
#define HEADLIGHT  5
#define RIGHTLIGHT 4
#define HORN	   3
#define BREAK      2
#define LEFTLIGHT  1
#define POWERON	   0
/* ����TIMECHANGE�� */
#define YEAR_CHANGE		0
#define MONTH_CHANGE	1
#define DAY_CHANGE		2
#define HOUR_CHANGE		3
#define MINUTE_CHANGE	4
#define SECOND_CHANGE	5
//
#define DRIVING_STAR  0x01
#define DRIVING_STOP  0X02
#define DRIVING_STOP_DRIVER 0x04
#define DRIVING_OVERTIME 0x08

//
#define DRIVER_REG_IN   0x01
#define DRIVER_REG_OUT  0x02

//
#define POWER_ON   0x01
#define POWER_OFF  0x02

#define SpeedPowerOff    0
enum
{
	NONE_OPR, //木有操作
	NORMAL,   //速度正常
	ABORT,    //速度异常
	RECORD_STARTTIME,   //记录数据
	RECORD_DATA
}CMD_JOURNAL;
enum
{
	NONE_DB,
	DBRECORD_START,
	DBRECORD_END
}CMD_DB;

typedef struct
{
	Record_CLOCK dt;
}RecordData_start;
typedef struct
{//��ʻ��¼
	Record_CLOCK dt;
	unsigned long DistancePulse;
	unsigned long DriverCode;
}RecordData_end;

//�����ӡ����ʾ��ÿ����ƽ���ٶ�
typedef struct
{//����ʱ��

	unsigned char hour;
	unsigned char minute;
	unsigned char speed;
	unsigned long  DriverCode;
	
} PrintSpeed;


#define MAX_BUFF 2048
#define RECVHEADSIZE 4
#define SENDHEADSIZE 8
#define SENDFILEBUFF 1000
#define SENDFILESUM	 2

//232
#define BAUDS4800 427
#define BAUDS9600 213
#define BAUDS38400 53
#define DataLength  256
#define CmdLength   17


#endif
