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
} FlashFileID;

struct record_block_head
{
	unsigned int StartTimeTag;
	unsigned short RecorderNumber;
};

int flashfile_system_init(void);
int flashfile_append_record(const FlashFileID file_id,
		const unsigned int time_tag, const char* ptrData);

#endif /* FLASHFILE_H_ */
