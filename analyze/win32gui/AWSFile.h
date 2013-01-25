// AWSFile.h: interface for the CAWSFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AWSFILE_H__B58D6B90_E4D1_474B_8BBC_35EC299DC2B7__INCLUDED_)
#define AFX_AWSFILE_H__B58D6B90_E4D1_474B_8BBC_35EC299DC2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <limits>
typedef struct _AWS_Sample
{
	double dA_ratio[10];
	double dA_CPM[10];
	double dB_ratio[10];
	double dB_A_CPM[10];
	double dB_CPM[10];
	_AWS_Sample()
	{
		double dbNaN = std::numeric_limits<double>::quiet_NaN();
		for(int i=0;i<10;i++) 
		{
			dA_ratio[i] = dbNaN;
			dA_CPM[i] = dbNaN;
			dB_ratio[i]=dbNaN;
			dB_A_CPM[i]=dbNaN;
			dB_CPM[i]=dbNaN;
		}
	};

} AWS_Sample;

typedef struct 
{
	double dA_Eff[10];
	double dB_Eff[10];
	double dBA_CPM[10];
} AWS_Cal;

typedef struct
{
	int nAch_LL;
	int nAch_UL;
	int nBch_LL;
	int nBch_UL;
	unsigned int nA_DPM;
	unsigned int nB_DPM;
	AWS_Sample sample;
} AWS_Setting;

typedef struct
{
	double dAch_co[4];
	double dBch_co[4];
	double d_BA_co[4];
	AWS_Cal cal;
} AWS_CalCo;

class CAWSFile  
{
public:
	bool CalculateCoefficient(AWS_Setting& set ,AWS_CalCo& co);
	bool WriteFile(LPCTSTR szPath, AWS_Setting& set);
	bool ReadData(LPCTSTR szFile,AWS_Setting& set);
	CAWSFile();
	virtual ~CAWSFile();

};

#endif // !defined(AFX_AWSFILE_H__B58D6B90_E4D1_474B_8BBC_35EC299DC2B7__INCLUDED_)
