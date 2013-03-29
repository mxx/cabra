/*
 * DataStore.h
 *
 *  Created on: Mar 29, 2013
 *      Author: mxx
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include "sqlite3.h"
#include <exception>
#include <string>

using namespace std;

class DataStoreException: public exception
{
public:
	DataStoreException()
	{
	}
	;
	~DataStoreException() throw ()
	{

	}
	;
	DataStoreException(const char* szErr)
	{
		strErr = szErr;
	}
	;
	const char* what(void)
	{
		return strErr.c_str();
	}
	;
protected:
	string strErr;
};

class DataStore
{
public:
	DataStore();
	virtual ~DataStore();
	void Open(void);
protected:
	void initDataStore(void);
	sqlite3* db;
};

#endif /* DATASTORE_H_ */
