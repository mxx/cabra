//*----------------------------------------------------------------------------
//* File Name           : RS232.h
//* Object              : H File of RS232 communication
//*----------------------------------------------------------------------------

#ifndef RS232_h
#define RS232_h
#include "application.h"
/* Functions Prototyping */

//����ݲɼ���ص�����
extern void UpLoad_DriverCode();          
extern void UpLoad_RealTime();          
extern void UpLoad_TotalDistance360h();         
extern void UpLoad_CHCO();   
extern void UpLoad_Speed360h();
extern void ComputeTimeBefore360h();         
extern void UpLoad_AutoVIN();               
extern void UpLoad_DoubtPoint();         
extern void UpLoad_DistanceinTwoDays();     
extern void UpLoad_SpeedinTwoDays();     
extern void UpLoad_OverThreeHours();    
extern void UpLoad_OverEightHours();     
extern void UpLoad_ALL_PARA();   
extern void ComputeTimeBeforeX(CLOCK *ct,CLOCK *dt,unsigned long timeinterval);
extern unsigned char GetOneOTDRandModifyPointer(unsigned long *p,unsigned long *old_p, OTDR_start *s, OTDR_end *e);
extern void RefreshCurTime(CLOCK *s,CLOCK *d);
extern void Write65ByteToSRAM(unsigned short hourNB,CLOCK *t,unsigned char *buf);

//�����������صĺ���
extern void Set_DriverCode();       
extern void Set_AutoVIN();               
extern void Set_RealTime();         
extern void Set_CHCO();  
extern void Set_ALL_PARA();  

//��ͨѶ�����صĺ���
extern void RS232SetError();
extern void RS232UploadError();
extern void RS232SetSuccess(unsigned char CorrespondCmdWord);
                
extern void Modify_LastUploadTime();

#endif /* RS232_h */
