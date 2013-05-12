#ifndef menu_h
#define menu_h
#include"lcd.h"
////////////////////////////////////////////////////////
//                        menu                        //
////////////////////////////////////////////////////////
//

#define NULL 0
#define Normal		0
#define Node 		1
#define BackLeaf 	2
#define ActLeaf 	3
#define Action		4
#define USBComm		5


#define PRINT		0
#define USB_SLAVE	1
#define USB_HOST	2
#define SHOW		3


typedef struct
{

	FONT_MATRIX * * content;
	short ChildrenList;
	short FatherList;
	short FatherNB;
	void (* handler)(void);
	
} MENU_NODE;

typedef struct
{
	MENU_NODE * ListPt;//����б��׵�ַ
	unsigned short NodeNumber;//������
	
} NODE_LIST;

typedef struct
{//LCD��ʾ��ǰ���ƿ�
    unsigned char enter;
	unsigned char mode;
	unsigned char ListNb;
	unsigned char NodeNb;
	unsigned int  KeepTime;
	
} LCDTCB;

typedef struct
{
	unsigned char type;
	unsigned char IfActionEnd;//��0δ����1��������
	unsigned char LineNumber;
	unsigned char CurLine;
} ACTION_TCB;

extern void SaveDatatoUdisk();
extern void PrintAllData();
extern void DisplayAutoCode();
extern void DisplayDriverNumber();
extern void DisplayDriverCode();
extern void Displaywheel();
extern void DisplayStatusPolarity();
extern FONT_MATRIX *AutoCodeHZ2LCM(unsigned short data);
extern FONT_MATRIX *ASCII2LCM(unsigned char data);
extern void OKKeyHandler();
extern void SelectKeyHandler( unsigned char  dir);
extern void MenutKeyHandler();
extern void DisplayNormalUI();
extern FONT_MATRIX *BCD2LCM(unsigned char data, unsigned char type);
extern void DisplayErrorCard();
extern void Display2DayOTDR();
extern void Display15MinAverageSpeed();
extern void WriteDataToUDiskMenu();
extern void DisplayProductVersion();
extern void DisplayTotalDistance();
extern void DisplayInteger(unsigned long integer,unsigned char row,unsigned char end_column,unsigned char len);
extern void DisplayAlarm();
extern void DisplayNormalUI();
extern void DisplayEraseDataFlash();
#endif /* menu_h */
