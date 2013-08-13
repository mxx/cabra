/*
 * flashfile.c
 *
 *  Created on: Jan 27, 2013
 *      Author: mxx
 *
 *  FLASH文件系统
 *  将FLASH地址空间，分为256个数据块，分别分配给不同的文件使用
 *  FLASH空间的大小通过FLASH_SIZE宏定义
 *  每个文件使用一个4bit标识，最多可处理16个文件
 *
 *  每个数据块头部，包含一个FlashBlockHead结构，定义此块属于那个文件，以及
 *  第一个时间标签的开始位置，和文件下一块的编号、块的擦写次数
 *
 *  flashFileBlockLimite
 *  定义各个文件的分配最大的数据块数
 *
 *  block_map是内存中记录文件分配表，类似FAT表
 *  系统启动后，需调用flashfile_system_init，从FLASH中回复数据
 *
 *  FlashFile类型的数组变量flashFile，
 *  记录每个文件相关参数的数据结构。
 *  使用文件系统前需调用flashfile_set_param设置文件的各项参数。
 *  文件内容是带时间标签的定长记录
 *  需要设置的参数：
 *  int record_size		记录的长度
 *  int time_unit		每条记录的代表多少秒
 *  int time_interval	时间标签的间隔时间，以秒为单位
 *
 *  寫文件調用 flashfile_append_record函數
 *  參數爲：文件標識，時間標籤，數據指針
 *  返回值：大於0，表示寫的字節數，小於0，表示出錯
 *
 *  讀文件調用 flashfile_get_record函數
 *  參數爲：文件標識，時間標籤，存儲回傳數據的指針
 *  返回值：0，表示沒有此數據，大於0，表示寫的字節數，小於0，表示出錯
 *
 */
#include "flashfile.h"
#include "flash_dev.h"

#define FLASH_SIZE 1024*1024
#define BLOCK_NUMBER 256
#define BLOCK_SIZE FLASH_SIZE/BLOCK_NUMBER

#ifndef _DEBUG_TEST_
static
#endif
FlashBlockHead block_map[BLOCK_NUMBER];

#ifndef _DEBUG_TEST_
static
#endif
unsigned char nextBlockChain[BLOCK_NUMBER] =
{ 0 };

#ifndef _DEBUG_TEST_
static
#endif
FlashFile flashFile[FileIDMax] =
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
int flashfile_get_first_time_tag_offset(const unsigned char block)
{
	return block_map[block].first_time_tag_offset;
}

int flashfile_get_time_tag_next(const unsigned char block, const int offset,
		TimeTag* ptrCurrentTag)
{
	flash_read(block, offset, (char *) ptrCurrentTag, sizeof(TimeTag));
	return ptrCurrentTag->next_time_tag_offset;
}

int flashfile_get_last_time_tag(const unsigned char block, TimeTag* ptrTag)
{
	int offset = flashfile_get_first_time_tag_offset(block);
	TRACE("[%u]Start offset->[:%d] ",block,offset);
	while (offset < BLOCK_SIZE)
	{
		short new_offset = 0;
		TimeTag tag;
		new_offset = flashfile_get_time_tag_next(block, offset, &tag);
		TRACE(",time:%u ",tag.time_tag);
		if (new_offset == -1)
		{
			TRACE(" -1End\n");
			if (offset == flashfile_get_first_time_tag_offset(block))
				return -1;
			return offset - ptrTag->next_time_tag_offset;
		}

		offset += new_offset;

		ptrTag->time_tag = tag.time_tag;
		ptrTag->next_time_tag_offset = new_offset;

		TRACE("->next[:%d]",offset);
		if (offset > BLOCK_SIZE)
		{
			TRACE(" End\n");
			return offset - ptrTag->next_time_tag_offset;
		}
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

	flashFile[file_id].last_time_tag_offset = -1;

	if (flashFile[file_id].total_block > 0)
	{
		unsigned char nextBlock = flashFile[file_id].start_block;
		for (int i = 1; i < flashFile[file_id].total_block; i++)
		{
			nextBlock = nextBlockChain[nextBlock];
		}

		int last_tag_block = flashFile[file_id].last_write_block;
		int last_tag_offset = flashfile_get_last_time_tag(last_tag_block,
				&(flashFile[file_id].last_time_tag));

		while (last_tag_offset < 0
				&& (last_tag_block != flashFile[file_id].start_block))
		{
			last_tag_block = block_map[last_tag_block].prev_block;
			last_tag_offset = flashfile_get_last_time_tag(last_tag_block,
					&(flashFile[file_id].last_time_tag));
		};

		flashFile[file_id].last_time_tag_offset = last_tag_offset;
		flashFile[file_id].last_time_tag_block = last_tag_block;
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
		nextBlockChain[i] = i;
	}
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

int flashfile_count_compare(const unsigned char x, const unsigned char y)
{
	if (x == 255 && y == 0)
		return -1;
	if (x == 0 && y == 255)
		return 255;
	return x - y;
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
			if (found_block == 256)
				found_block = i;
			if (flashfile_count_compare(found_write_count,
					block_map[i].write_count) > 0)
			{
				found_block = i;
				found_write_count = block_map[i].write_count;
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

int flashfile_block_write(const unsigned char block, const int offset,
		const char* ptrData, const int size)
{
	if (offset > BLOCK_SIZE || (offset + size) > BLOCK_SIZE)
	{
		TRACE("\n%s(%u,%u,%u)\n",__FUNCTION__,block,offset,size+offset);
		return -1;
	}

	FlashFile* ptrFile = &flashFile[block_map[block].file_id];
	ptrFile->last_write_block = block;
	ptrFile->last_write_offset = offset;
	return flash_write(block, offset, ptrData, size);
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

	flashfile_block_write(block, 0, (const char*) &block_map[block],
			sizeof(block_map[block]));
}

int flashfile_set_param(const FlashFileID file_id, int record_size,
		int time_unit, int time_interval)
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
	TRACE("%s(%u) =>",__FUNCTION__,file_id);
	int block = flashfile_find_first_freeblock();

	if (block < 256)
	{
		if (flashFile[file_id].total_block == 0)
		{
			flashfile_append_file_tail_block(file_id, block);
			TRACE("%d\n",block);
			return block;
		}
		flashfile_round_file_struct(file_id, block);
	}TRACE("%d\n",block);
	return block;
}

int flashfile_read(const unsigned char block, const int offset, char* ptrData,
		const int size)
{
	return flash_read(block, offset, ptrData, size);
}

int flashfile_write(const unsigned char block, const int offset,
		const char* ptrData, const int size)
{
	int write_block = block;
	int new_offset = offset;
	if ((offset + size) > (2 * BLOCK_SIZE))
	{
		TRACE("error,too big\n");
		return -1;
	}

	if (offset > BLOCK_SIZE)
	{
		if (write_block != nextBlockChain[write_block])
		{
			TRACE(" shift to block %d ",nextBlockChain[write_block]);
			write_block = nextBlockChain[write_block];
			new_offset = offset - BLOCK_SIZE + sizeof(FlashBlockHead);
		}
		else
			return -1;
	}

	int wrote_size = 0;
	if (new_offset < BLOCK_SIZE)
	{
		int write = size;
		if ((new_offset + size) > BLOCK_SIZE)
		{
			write = BLOCK_SIZE - offset;
		}
		wrote_size = flashfile_block_write(write_block, new_offset, ptrData,
				write);
	}

	if (wrote_size < 0 || wrote_size > size)
	{
		TRACE("write fail.\n");
		return -1;
	}

	if (wrote_size == size)
		return size;

	TRACE(" part write[%d] ",wrote_size);

	write_block = flashfile_alloc_block(block_map[block].file_id);
	new_offset = sizeof(FlashBlockHead);

	if (write_block > 255)
	{
		TRACE("no block to write.\n");
		return -1;
	}

	return flashfile_block_write(write_block, new_offset, ptrData + wrote_size,
			size - wrote_size);
}

int flashfile_time_tag_offset(const FlashFileID file_id)
{
	int unit = flashFile[file_id].time_tag_unit;
	int interval = flashFile[file_id].time_tag_interval;
	int rec_size = flashFile[file_id].record_size;
	return sizeof(TimeTag) + (interval / unit) * rec_size;
}

void flashfile_store_last_time_tag(const FlashFileID file_id,
		const TimeTag* ptrTag)
{
	FlashFile* ptrFile = &flashFile[file_id];
	ptrFile->last_time_tag.time_tag = ptrTag->time_tag;
	ptrFile->last_time_tag.next_time_tag_offset = ptrTag->next_time_tag_offset;
	ptrFile->last_time_tag_block = ptrFile->last_write_block;
	ptrFile->last_time_tag_offset = ptrFile->last_write_offset;
}

int flashfile_append_data_offset_to_last_time_tag(const FlashFileID file_id,
		const unsigned int time_tag)
{
	int offset = sizeof(TimeTag);
	offset += ((time_tag - flashFile[file_id].last_time_tag.time_tag)
			/ flashFile[file_id].time_tag_unit)
			* flashFile[file_id].record_size;
	return offset;
}

int flashfile_init_record(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData)
{
	unsigned int block = flashfile_alloc_block(file_id);
	FlashFile* ptrFile = &flashFile[file_id];
	if (block > 256)
		return -1;
	block_map[block].first_time_tag_offset = sizeof(FlashBlockHead);
	block_map[block].prev_block = block;
	flashfile_write(block, 0, (char*) &block_map[block],
			sizeof(block_map[block]));
	TimeTag tag;
	int interval = ptrFile->time_tag_interval;
	tag.time_tag = (time_tag / interval) * interval;
	tag.next_time_tag_offset = flashfile_time_tag_offset(file_id);
	flashfile_write(block, sizeof(block_map[block]), (char*) &tag, sizeof(tag));
	flashfile_store_last_time_tag(file_id, &tag);

	int offset = sizeof(block_map[block]);
	offset += flashfile_append_data_offset_to_last_time_tag(file_id, time_tag);
	flashfile_write(block, offset, ptrData, flashFile[file_id].record_size);
	return 0;
}

int flashfile_append_time_tag(const FlashFileID file_id,
		const unsigned int time_tag)
{
	TRACE("%s(%u,%u) ",__FUNCTION__,file_id,time_tag);
	unsigned char block;
	TimeTag timeTag;
	timeTag.time_tag = time_tag;
	timeTag.next_time_tag_offset = flashfile_time_tag_offset(file_id);

	block = flashFile[file_id].last_time_tag_block;
	int offset = flashFile[file_id].last_time_tag_offset;

	if (offset < 0)
	{
		offset = sizeof(FlashBlockHead);
	}
	else
	{
		offset += timeTag.next_time_tag_offset;
	}

	TRACE(" @%d->%d\n",block,offset);
	if (flashfile_write(block, offset, (const char*) &timeTag, sizeof(timeTag))
			< 0)
	{
		TRACE("flash write fail\n");
		return -1;
	}
	flashfile_store_last_time_tag(file_id, &timeTag);
	return 0;
}

int flashfile_append_data(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData)
{
	unsigned char block;
	TRACE("Time:%u[%d] ",time_tag,time_tag - flashFile[file_id].last_time_tag.time_tag);
	block = flashFile[file_id].last_time_tag_block;
	int offset = 0;
	int relative_offset = 0;
	do
	{
		block = flashFile[file_id].last_time_tag_block;
		offset = flashFile[file_id].last_time_tag_offset;

		relative_offset = flashfile_append_data_offset_to_last_time_tag(file_id,
				time_tag);
		TRACE(" data offset %d",relative_offset);
		if (relative_offset < flashfile_time_tag_offset(file_id))
			break;

		int new_tag = time_tag / flashFile[file_id].time_tag_interval;
		new_tag *= flashFile[file_id].time_tag_interval;
		TRACE(" Insert new Tag:%u",new_tag);
		flashfile_append_time_tag(file_id, new_tag);

	} while (1);

	offset += relative_offset;
	TRACE(" @%d->%d\n",block,offset);
	return flashfile_write(block, offset, ptrData,
			flashFile[file_id].record_size);
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
