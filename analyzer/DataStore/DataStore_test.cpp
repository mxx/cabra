/*
 * DataStore_test.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: mxx
 */

#include "DataStore.h"

int main(int argc ,const char** argv)
{
	DataStore ds;
	ds.Open();
	VTDRSpeedRecord rec;
	try
	{
		ds.SaveSpeedRecord("1234",rec);
	}
	catch(DataStoreException& ex)
	{
		printf("%s\n",ex.what());
	}
}
