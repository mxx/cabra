/*
 * flashfile.c
 *
 *  Created on: Jan 27, 2013
 *      Author: mxx
 */
#include "flashfile.h"
#include "flash_dev.h"

typedef struct _time_tag
{
	unsigned int time_tag;
	unsigned short next_time_tag_offset;
} __attribute__ ((packed)) TimeTag;

typedef struct flashfile
{
	unsigned char file_id;
	unsigned char start_block;
	unsigned char total_block;
	unsigned char block_limit;
	unsigned char last_write_block;
	TimeTag last_time_tag;
	int last_time_tag_offset;			//last time tag offset from block head
	int record_size;				//bytes per record .
	int time_tag_unit;              //one record stand for how may seconds
	int time_tag_interval;			//how many seconds between time tags
} __attribute__ ((packed)) FlashFile;

typedef struct block_info
{
	unsigned short file_id :4;
	unsigned short first_time_tag_offset :12;
	unsigned char prev_block;
	unsigned char write_count;
} __attribute__ ((packed)) FlashBlockHead;

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
		ptr[i] = value;
}

int flashfile_scan_last_block(FlashFileID file_id)
{
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		if (block_map[nextBlockChain[i]].file_id != file_id)
		{
			flashFile[file_id].last_write_block = i;
			return 0;
		}
	}
	return -1;
}
// return the time_tag structure offset from block head;
int flashfile_get_first_time_tag(const unsigned char block)
{
	return block_map[block].first_time_tag_offset;
}

int flashfile_get_next_time_tag(const unsigned char block, const int offset,
		TimeTag* ptrTag)
{
	flash_read(block, offset, (char *) ptrTag, sizeof(TimeTag));
	return ptrTag->next_time_tag_offset;
}

int flashfile_get_last_time_tag(const unsigned char block, TimeTag* ptrTag)
{
	int offset = flashfile_get_first_time_tag(block);
	while (offset < 0xFFF)
	{
		int new_offset;
		TimeTag tag;
		new_offset = flashfile_get_next_time_tag(block, offset, &tag);
		if (new_offset == 0xFFF)
		{
			return offset;
		}
		offset += new_offset;
		ptrTag->time_tag = tag.time_tag;
		ptrTag->next_time_tag_offset = new_offset;
	}
	return -1;
}

void flashfile_scan_file_block(FlashFileID file_id)
{
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		unsigned char prev = block_map[i].prev_block;

		if (block_map[i].file_id == file_id)
		{
			nextBlockChain[prev] = i;
			flashFile[file_id].total_block++;
			if (block_map[prev].file_id != file_id
					|| block_map[i].prev_block == i)
				flashFile[file_id].start_block = i;
		}
	}

	if (flashFile[file_id].total_block > 0)
	{
		unsigned char nextBlock = flashFile[file_id].start_block;
		for (int i = 1; i < flashFile[file_id].total_block; i++)
		{
			nextBlock = nextBlockChain[nextBlock];
		}
		flashFile[file_id].last_write_block = nextBlock;

		flashFile[file_id].last_time_tag_offset = flashfile_get_last_time_tag(
				flashFile[file_id].last_write_block,
				&(flashFile[file_id].last_time_tag));
	}

}

void flashfile_init_file_struct(FlashFileID file_id)
{
	flashfile_memset((char*) &flashFile[file_id], 0,
			sizeof(flashFile[file_id]));
	flashFile[file_id].file_id = file_id;
	flashFile[file_id].block_limit = flashFileBlockLimite[file_id];

	flashfile_scan_file_block(file_id);
}

int flashfile_update_blockmap(void)
{
	int rt = 0;
	for (int i = 0; i < BLOCK_NUMBER; i++)
	{
		if (sizeof(FlashBlockHead)
				!= flash_read(i, 0, (char *) &block_map[i],
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
			if ((unsigned char)(block_map[i].write_count + 1) < found_write_count)
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
	flashfile_memset((char*) block_map, 0, sizeof(block_map));
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
	flash_erase_block(block);
}

void flashfile_append_file_tail_block(const FlashFileID file_id,
		const unsigned char block)
{
	block_map[block].file_id = file_id;
	block_map[block].prev_block = flashFile[file_id].last_write_block;
	block_map[block].first_time_tag_offset =
			flashFile[file_id].last_time_tag.next_time_tag_offset
					+ flashFile[file_id].last_time_tag_offset - BLOCK_SIZE
					+ sizeof(FlashBlockHead);

	nextBlockChain[flashFile[file_id].last_write_block] = block;
	flashFile[file_id].last_write_block = block;
	flashFile[file_id].total_block++;
}

int flashfile_set_param(const FlashFileID file_id,int record_size, int time_unit, int time_interval)
{
	flashFile[file_id].record_size = record_size;
	flashFile[file_id].time_tag_interval = time_interval;
	flashFile[file_id].time_tag_unit = time_unit;
	return 0;
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

int flashfile_read(const unsigned char block, const int offset, char* ptrData,
		const int size)
{
	return flash_read(block, offset, ptrData, size);
}

int flashfile_block_write(const unsigned char block, const int offset,
		const char* ptrData, const int size)
{
	if (offset > BLOCK_SIZE)
		return -1;

	return flash_write(block, offset, ptrData, size);
}

int flashfile_write(const unsigned char block, const int offset,
		const char* ptrData, const int size)
{
	int write_block = block;
	if ((offset > 2 * BLOCK_SIZE) && ((offset + size) > 2 * BLOCK_SIZE))
		return -1;

	int wrote_size;
	if (offset < BLOCK_SIZE)
	{
		wrote_size = flashfile_block_write(write_block, offset, ptrData, size);
	}

	if (wrote_size < 0 || wrote_size > size)
		return -1;

	if (wrote_size > 0 && wrote_size == size)
		return size;

	write_block = flashfile_alloc_block(block_map[block].file_id);

	if (write_block > 255)
		return -1;

	return flashfile_block_write(write_block,
			offset - wrote_size + sizeof(FlashBlockHead), ptrData + wrote_size,
			size - wrote_size);
}

int flashfile_time_tag_offset(const FlashFileID file_id)
{
	int unit = flashFile[file_id].time_tag_unit;
	int interval = flashFile[file_id].time_tag_interval;
	int rec_size = flashFile[file_id].record_size;
	return sizeof(TimeTag) + (interval / unit) * rec_size;
}

int flashfile_record_offset_to_time_tag(const FlashFileID file_id,
		const TimeTag* ptrTag, const unsigned int record_time_tag)
{
	int offset = sizeof(TimeTag);
	offset += ((record_time_tag - ptrTag->time_tag)
			/ flashFile[file_id].time_tag_unit)*flashFile[file_id].record_size;
	return offset;
}

void flashfile_store_last_time_tag(const FlashFileID file_id,
		const TimeTag* ptrTag)
{
	flashFile[file_id].last_time_tag.time_tag = ptrTag->time_tag;
	flashFile[file_id].last_time_tag.next_time_tag_offset =
			ptrTag->next_time_tag_offset;
}

int flashfile_init_record(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData)
{
	unsigned char block = flashfile_alloc_block(file_id);
	if (block > 256)
		return -1;
	block_map[block].first_time_tag_offset = sizeof(FlashBlockHead);
	block_map[block].prev_block = block;
	flashfile_write(block, 0, (char*)&block_map[block], sizeof(block_map[block]));
	TimeTag tag;
	int interval = flashFile[file_id].time_tag_interval;
	tag.time_tag = (time_tag / interval) * interval;
	tag.next_time_tag_offset = flashfile_time_tag_offset(file_id);
	flashfile_write(block, sizeof(block_map[block]),(char*) &tag, sizeof(tag));

	flashfile_store_last_time_tag(file_id, &tag);

	int offset = sizeof(block_map[block]) ;
	offset += flashfile_record_offset_to_time_tag(file_id, &tag, time_tag);
	flashfile_write(block, offset, ptrData, flashFile[file_id].record_size);
	return 0;
}

int flashfile_append_time_tag(const FlashFileID file_id,
		const unsigned int time_tag)
{
	unsigned char block;
	TimeTag timeTag;
	timeTag.time_tag = time_tag;
	timeTag.next_time_tag_offset = flashfile_time_tag_offset(file_id);

	block = flashFile[file_id].last_write_block;
	int offset = 0;
	TimeTag lastTag;
	offset = flashfile_get_last_time_tag(block,&lastTag);
	flashfile_store_last_time_tag(file_id, &timeTag);
	return flashfile_write(block, offset, (const char*)&timeTag, sizeof(timeTag));
}

int flashfile_append_data(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData)
{
	unsigned char block;
	TimeTag timeTag;
	timeTag.time_tag = time_tag;
	timeTag.next_time_tag_offset = flashfile_time_tag_offset(file_id);

	block = flashFile[file_id].last_write_block;
	int offset = 0;
	TimeTag lastTag;
	offset = flashfile_get_last_time_tag(block,&lastTag);
	offset += flashfile_record_offset_to_time_tag(file_id, &timeTag, time_tag);

	return flashfile_write(block, offset, ptrData,
			flashFile[file_id].time_tag_unit);
}

int flashfile_append_record(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData)
{
	if (flashFile[file_id].total_block == 0)
		return flashfile_init_record(file_id, time_tag, ptrData);

	if ((time_tag % flashFile[file_id].time_tag_interval) == 0)
	{
		flashfile_append_time_tag(file_id, time_tag);
	}

	return flashfile_append_data(file_id, time_tag, ptrData);
}

int flashfile_get_first_write_size(const char file_id)
{
	return 0;
}
