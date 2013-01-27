/*
 * flashfile.c
 *
 *  Created on: Jan 27, 2013
 *      Author: mxx
 */
#include "flashfile.h"

#define FLASH_SIZE 1024*1024
#define BLOCK_NUMBER 256
#define BLOCK_SIZE FLASH_SIZE/BLOCK_NUMBER

static FlashBlockHead block_map[BLOCK_NUMBER];

static FlashFile flashFile[FileIDMax] =
{ 0 };

const unsigned char flashFileBlockLimite[FileIDMax] =
{
		15, 	//Suspicious Point
		130, 	//Speed
		80, 	//Position
		2,		//Overtime Driving
		2,		//Driver information
		1,		//Power
		1,		//Parameter Modify
		1		//Installation Record
};

void flashfile_memset(char* ptr, char value, int size)
{
	for (int i = 0; i < size; i++)
		ptr[i] = c;
}

void flashfile_scan_fileblock(FlashFileID file_id)
{
	flashFile[file_id].total_block = 0;
	flashFile[file_id].file_id = file_id;
	flashFile[file_id].block_limit = flashFileBlockLimite[file_id];
	flashFile[file_id].start_block = 0;
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		if (block_map[i].file_id == file_id)
		{
			flashFile[file_id].total_block++;
			unsigned char prev = block_map[i].prev_block;
			if (block_map[prev].file_id != file_id
					|| block_map[block_map[i].prev_block] == i)
				flashFile[file_id].start_block = i;
		}
	}
}

void flashfile_init_file_struct(FlashFileID file_id)
{
	flashFile[file_id].file_id = file_id;
	flashfile_scan_file_block(file_id);
}

int flashfile_system_init(void)
{
	flashfile_memset(block_map, 0, sizeof(block_map));
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		flash_block_read(i, 0, &block_map[i], sizeof(FlashBlockHead));
	}

	return 0;
}

int flashfile_append(const char file_id, const char* ptrData, const int size)
{
	return 0;
}
int flashfile_read(const char file_id, const int offset, char* ptrData,
		const int size)
{
	return 0;
}

int flashfile_get_first_write_size(const char file_id)
{
	return 0;
}
