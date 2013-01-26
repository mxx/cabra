/*
 * Config.cpp
 *
 *  Created on: 2010-1-9
 *      Author: mxx
 */

#include "Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GetValue(const char* szFile,LPCTSTR szKey, char *pBuf, int nLen)
{
	FILE *confFile;
	int nRet,nLine,nCmp;
	int nszKey = strlen(szKey);
	char line[1024]={0};
	char szKeyCmp[1024]={0};
	char* moveEof=NULL;

	int nSpace=0;
	int nEnd=0;
	int i;

	pBuf[0]=0;

	if( (confFile  = fopen((const char *)szFile, "r" )) == NULL )
		nRet = -1;
	else
	{
		nRet = -2;
		while (!feof (confFile))
		{
			memset(line,0,1024);
			memset(szKeyCmp,0,1024);
			fgets(line, 1024,confFile);

			if (line[0]=='#')
				continue;

			nLine = strlen(line);
			if(line[nLine-1]=='\n')
				nEnd = 1;
			else
				nEnd=0;
			moveEof=strtok(line,"\n");
			if(moveEof!=NULL)
			{
				while(*moveEof == ' ')
				{
					nSpace++;
					moveEof--;
				}
			}
			if(nSpace!=0)
				nLine = nLine-nSpace-1;
			strcpy(szKeyCmp,szKey);
			strcat(szKeyCmp,"=");
			nCmp=strncmp(szKeyCmp,line,nszKey+1);
			if(!nCmp)
			{
				int temp = nLine-nszKey;
				if(nLen<temp)
					nRet = temp;
				else
				{
					if(nEnd>0)
					{
						for(i=0;i<temp-1;i++)
							pBuf[i]=line[i+nszKey+1];
					}
					else
					{
						for(i=0;i<temp;i++)
							pBuf[i]=line[i+nszKey+1];
					}
					nRet = 0;
				}
				break;
			}
		}
		fclose (confFile);
	}
	return nRet;
}

Config::Config(const char* szFileName)
{
	strFileName = szFileName;
}

Config::~Config()
{

}


double Config::GetMF_A(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"A",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return 0.000;
}

double Config::GetMF_B(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"B",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return 0.000;
}
double Config::GetMF_C(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"C",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return 2.900;
}

double Config::GetMF_D(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"D",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return 1.491;
}

double Config::GetMF_a(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"a",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return 0.000;
}
double Config::GetMF_b(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"b",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return 0.000;
}
double Config::GetMF_c(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"c",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return -2.0;
}

double Config::GetMF_d(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName,"d",tmp,1024)==0)
	{
		return atof(tmp);
	}
	return -1.0;
}

bool Config::ReadKey(LPCTSTR szKey, LPCTSTR szVal, int nLen)
{
	if (GetValue(strFileName,szKey,(char*)szVal,nLen)==0)
		return true;
	else
		return false;

}
