/*
 * DataStore.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: mxx
 */

#include "DataStore.h"
#include <exception>

#define CREATE_DRIVER 			"CREATE  TABLE  IF NOT EXISTS main.driver (driver_license VARCHAR PRIMARY KEY  NOT NULL , driver_name VARCHAR)"
#define CREATE_DRIVE_RECORD 	"CREATE  TABLE  IF NOT EXISTS main.driving_record (vid VARCHAR NOT NULL,driver VARCHAR NOT NULL , time_tag INTEGER NOT NULL , state CHAR NOT NULL , update_time INTEGER NOT NULL , PRIMARY KEY (vid,time_tag))"
#define CREATE_ODERMETER 		"CREATE  TABLE  IF NOT EXISTS main.oder_meter (vid VARCHAR NOT NULL,time_tag INTEGER NOT NULL , odermeter DOUBLE NOT NULL , update_time INTEGER NOT NULL , PRIMARY KEY (vid,time_tag))"
#define CREATE_OVERDRIVE 		"CREATE  TABLE  IF NOT EXISTS main.over_drive (vid VARCHAR NOT NULL,driver VARCHAR NOT NULL , start_time INTEGER NOT NULL , end_time INTEGER NOT NULL , start_lng DOUBLE, start_lat DOUBLE, start_alt DOUBLE, end_lng DOUBLE, end_lat DOUBLE, end_alt DOUBLE, PRIMARY KEY (vid,driver, start_time, end_time))"
#define CREATE_POWERLOG 		"CREATE  TABLE  IF NOT EXISTS main.power_log (vid VARCHAR NOT NULL,time_tag INTEGER NOT NULL , type CHAR NOT NULL , update_time INTEGER NOT NULL , PRIMARY KEY (vid,time_tag))"
#define CREATE_MODIFYLOG 		"CREATE  TABLE  IF NOT EXISTS main.modify_log (vid VARCHAR NOT NULL,time_tag INTEGER NOT NULL , action CHAR NOT NULL , update_time INTEGER NOT NULL , PRIMARY KEY (vid,time_tag))"
#define CREATE_POSTION_RECORD 	"CREATE  TABLE  IF NOT EXISTS main.position_record (vid VARCHAR NOT NULL , time_tag INTEGER NOT NULL , lng DOUBLE, lat DOUBLE, alt DOUBLE, speed FLOAT NOT NULL , update_time INTEGER, PRIMARY KEY (vid, time_tag))"
#define CREATE_SPEED_RECORD 	"CREATE  TABLE  IF NOT EXISTS main.speed_record (vid VARCHAR NOT NULL , speed INTEGER, state CHAR, time_tag INTEGER NOT NULL , update_time INTEGER NOT NULL , PRIMARY KEY (vid, time_tag))"
#define CREATE_ABNORMAL_SPEED 	"CREATE  TABLE  IF NOT EXISTS main.abnormal_speed (vid VARCHAR NOT NULL , time_tag INTEGER NOT NULL , speed CHAR, speed_ref CHAR, end_time_tag INTEGER, update_time INTEGER, PRIMARY KEY (vid, time_tag))"
#define CREATE_VINFO 			"CREATE  TABLE  IF NOT EXISTS main.vehicle_info (vid VARCHAR NOT NULL , uniq_code VARCHAR NOT NULL , type VARCHAR NOT NULL , class VARCHAR NOT NULL , b0_name VARCHAR NOT NULL , b1_name VARCHAR NOT NULL , b2_name VARCHAR NOT NULL , b3_name VARCHAR NOT NULL , b4_name VARCHAR NOT NULL , b5_name VARCHAR NOT NULL , b6_name VARCHAR NOT NULL , b7_name VARCHAR NOT NULL , product_date INTEGER NOT NULL , sn VARCHAR NOT NULL , manufacture VARCHAR NOT NULL , auth_type VARCHAR NOT NULL , update_time , PRIMARY KEY (vid, uniq_code, product_date, sn, manufacture, auth_type))"

DataStore::DataStore()
{
	db = NULL;
}

DataStore::~DataStore()
{
	if (db)
		sqlite3_close(db);
}

void DataStore::Open(void)
{
	int rt = sqlite3_open("datastore.db", &db);
	if (rt)
	{
		DataStoreException ex(sqlite3_errmsg(db));
		throw(ex);
	}
	initDataStore();
}

void DataStore::initDataStore(void)
{
	char* err_msg = NULL;
	const char* create_sql[] =
	{ CREATE_DRIVER, CREATE_DRIVE_RECORD, CREATE_ODERMETER, CREATE_OVERDRIVE,
			CREATE_POWERLOG, CREATE_MODIFYLOG, CREATE_POSTION_RECORD,
			CREATE_SPEED_RECORD, CREATE_ABNORMAL_SPEED, CREATE_VINFO };
	for (int i = 0; i < sizeof(create_sql)/sizeof(const char*); i++)
	{
		if (SQLITE_OK != sqlite3_exec(db, create_sql[i], 0, 0, &err_msg))
		{
			DataStoreException ex(err_msg);
			throw(ex);
		}
	}
}

