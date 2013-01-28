/*
 * flashfile.c
 *
 *  Created on: Jan 27, 2013
 *      Author: mxx
 */
#include "flashfile.h"
#include "flash_dev.h"
typedef struct flashfile
{
	unsigned char file_id;
	unsigned char start_block;
	unsigned char total_block;
	unsigned char block_limit;
	unsigned char last_write_block;
	unsigned int last_time_tag;
	int time_tag_offset;
	int record_size;				//bytes per record
	int time_tag_unit;              //one record for how may seconds
	int time_tag_interval;			//how many seconds on tag placed
} FlashFile;

typedef struct _time_tag
{
	unsigned int time_tag;
	unsigned short next_time_tag_offset;
} TimeTag;

typedef struct block_info
{
	unsigned short file_id :4;
	unsigned short first_time_tag_offset :12;
	unsigned char prev_block;
	unsigned char write_count;
} FlashBlockHead;

#define FLASH_SIZE 1024*1024
#define BLOCK_NUMBER 256
#define BLOCK_SIZE FLASH_SIZE/BLOCK_NUMBER

static FlashBlockHead block_map[BLOCK_NUMBER];
static unsigned char nextBlockChain[BLOCK_NUMBER] =
{ 0 };

static FlashFile flashFile[FileIDMax] =
{ 0 };

const unsigned char flashFileBlockLimite[FileIDMax] =
{
//define maximum file size in block
		15,//Suspicious Point
		136, 	//Speed
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
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		unsigned char prev = block_map[i].prev_block;

		if (block_map[i].file_id == file_id)
		{
			nextBlockChain[prev] = i;
			flashFile[file_id].total_block++;

			if (block_map[prev].file_id != file_id
					|| block_map[block_map[i].prev_block] == i)
				flashFile[file_id].start_block = i;
		}
	}

	flashFile[file_id].last_write_block = flashFile[file_id].start_block;
	if (flashFile[file_id].total_block > 1)
	{
		unsigned char nextBlock = flashFile[file_id].start_block;
		for (int i = 1; i < flashFile[file_id].total_block; i++)
		{
			nextBlock = nextBlockChain[nextBlock];
		}
		flashFile[file_id].last_write_block = nextBlock;
	}
}

void flashfile_init_file_struct(FlashFileID file_id)
{
	flashFile[file_id].total_block = 0;
	flashFile[file_id].file_id = file_id;
	flashFile[file_id].block_limit = flashFileBlockLimite[file_id];
	flashFile[file_id].start_block = 0;
	flashFile[file_id].time_tag_interval = 60;
	flashFile[file_id].time_tag_offset = flashfile_scan_file_block(file_id);
}

int flashfile_update_blockmap(void)
{
	int rt = 0;
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		if (sizeof(FlashBlockHead)
				!= flash_block_read(i, 0, &block_map[i],
						sizeof(FlashBlockHead)))
		{
			rt = -1;
			break;
		}
	}
	return rt;
}
// if return value > 255, means that no block found
int flashfile_find_first_freeblock(void)
{
	int found_block = 256;
	unsigned char found_write_count = 255;
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		if (block_map[i].file_id == NoFile)
		{
			if ((block_map[i].write_count + 1) < found_write_count)
			{
				found_block = i;
				found_write_count = block_map[i].write_count + 1;
			}
		}
	}
	return found_block;
}

int flashfile_system_init(void)
{
	flashfile_memset(block_map, 0, sizeof(block_map));
	if (flashfile_update_blockmap())
		return -1;

	for (int i = 0; i < FileIDMax; i++)
		flashfile_init_file_struct(i);

	return 0;
}

void flashfile_remove_file_head_block(const FlashFileID file_id)
{
	unsigned char block = flashFile[file_id].start_block;
	block_map[block].file_id = NoFile;
	block_map[block].write_count++;
	unsigned char nextblock = nextBlockChain[block];
	nextBlockChain[block] = block;
	flashFile[file_id].start_block = nextblock;
	flashFile[file_id].total_block--;
	flashfile_erase(block);
}

void flashfile_append_file_tail_block(const FlashFileID file_id,
		const unsigned char block)
{
	block_map[block].file_id = file_id;
	block_map[block].prev_block = flashFile[file_id].last_write_block;
	nextBlockChain[flashFile[file_id].last_write_block] = block;
	flashFile[file_id].last_write_block = block;
	flashFile[file_id].total_block++;
}

void flashfile_round_file_struct(const FlashFileID file_id,
		const unsigned char block)
{
	if ((flashFile[file_id].total_block + 1) > (flashFile[file_id].block_limit))
	{
		flashfile_remove_file_head_block(file_id);
	}
	flashfile_append_file_tail_block(file_id, block);
}

int flashfile_alloc_block(const FlashFileID file_id)
{
	int block = flashfile_find_first_freeblock();

	if (block < 256)
	{
		if (flashFile[file_id].total_block == 0)
		{
			flashfile_append_file_tail_block(file_id, block);
			return block;
		}
		flashfile_round_file_struct(file_id, block);
	}

	return block;
}
// return the time_tag structure offset from block head;
int flashfile_get_first_time_tag(const unsigned char block)
{
	return block_map[block].first_time_tag_offset;
}

int flashfile_get_next_time_tag(const unsigned char block, const int offset)
{
	TimeTag tag;
	flash_read(block, offset, &tag, sizeof(tag));
	return offset + tag.next_time_tag_offset;
}

int flashfile_get_last_time_tag(const unsigned char block, int* next_offset)
{
	int offset = flashfile_get_first_time_tag(block);
	while (offset < 0xFFF)
	{
		int new_offset;
		new_offset = flashfile_get_next_time_tag(block,offset);
		if (new_offset > 0x1000)
		{
			*next_offset = new_offset - offset;
			return offset;
		}
		offset = new_offset;
	}
	return -1;
}

int flashfile_append_time_tag(const FlashFileID file_id,
		const unsigned int time_tag)
{
	unsigned char block;
	TimeTag timeTag;
	timeTag.next_time_tag_offset = 0;
	if (flashFile[file_id].total_block)
	{
		block = flashFile[file_id].last_write_block;

		int start_offset = block_map[block].first_time_tag_offset;
		int offset = start_offset + timeTag.next_time_tag_offset;
		while (offset < BLOCK_SIZE)
		{
			flash_read(block, offset, &timeTag, sizeof(timeTag));
			if (!(timeTag.time_tag + 1))
				break;
			offset += timeTag.next_time_tag_offset;
		};
	}

	if (offset > BLOCK_SIZE)
	{
		int new_block = flashfile_alloc_block(file_id);
		if (new_block < 256)
		{
			block_map[new_block].first_time_tag_offset = (offset - BLOCK_SIZE);
			flashfile_write(new_block, &block_map[new_block],
					sizeof(block_map[block])
							- sizeof(block_map[block].write_count));
			block = new_block;
		}
	}

	flashfile_write(block, &timeTag, sizeof(timeTag));
}

int flashfile_append_record(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData)
{
	if ((time_tag % flashFile[file_id].time_tag_interval) == 0)
	{
		flashfile_append_time_tag(file_id, time_tag);
	}

	flashfile_append_data(file_id, time_tag, ptrData);

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
