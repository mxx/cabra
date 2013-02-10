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

typedef struct _time_tag
{
	unsigned int time_tag;
	unsigned short next_time_tag_offset;
}__attribute__ ((packed)) TimeTag;

typedef struct flashfile
{
	unsigned char file_id;
	unsigned char start_block;
	unsigned char total_block;
	unsigned char block_limit;
	unsigned char last_write_block;
	int last_write_offset;
	TimeTag last_time_tag;
	int last_time_tag_block;
	int last_time_tag_offset;			//last time tag offset from block head
	int record_size;				//bytes per record .
	int time_tag_unit;              //one record stand for how may seconds
	int time_tag_interval;			//how many seconds between time tags
} FlashFile;

typedef struct block_info
{
	unsigned short file_id :4;
	unsigned short first_time_tag_offset :12;
	unsigned char prev_block;
	unsigned char write_count;
}__attribute__ ((packed)) FlashBlockHead;

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
