/*
 * flashfile.h
 *
 *  Created on: Sep 15, 2012
 *      Author: mxx
 */

#ifndef FLASHFILE_H_
#define FLASHFILE_H_

#define FLASH_SIZE 1024*1024
#define BLOCK_NUMBER 256
#define BLOCK_SIZE FLASH_SIZE/BLOCK_NUMBER

#ifdef _DEBUG_TEST_
#include <stdio.h>
#define TRACE(args...)  printf(args)
#else
#define TRACE(args...)
#endif

typedef enum _FlashFileID
{
	RealTimeFile,
	SpeedFile,
	PositionFile,
	OverTimeFile,
	DriverFile,
	PowerFile,
	ParamModiFile,
	InstallFile,
	FileIDMax,
	NoFile = 0x0F
} __attribute__ ((packed)) FlashFileID;



int flashfile_system_init(void);
int flashfile_set_param(const FlashFileID file_id,int record_size, int time_unit, int time_interval);
int flashfile_append_record(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData);

#endif /* FLASHFILE_H_ */
