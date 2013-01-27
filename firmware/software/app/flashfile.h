/*
 * flashfile.h
 *
 *  Created on: Sep 15, 2012
 *      Author: mxx
 */

#ifndef FLASHFILE_H_
#define FLASHFILE_H_


typedef struct _time_tag
{
	unsigned int time_tag;
	unsigned short next_time_tag_offset;
} TimeTag;

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

struct record_block_head {
	unsigned int StartTimeTag;
	unsigned short RecorderNumber;
};

int flashfile_system_init(void);
int flashfile_append(const char file_id, const char* ptrData, const int size);
int flashfile_read(const char file_id, const int offset, char* ptrData,
		const int size);
int flashfile_get_first_write_size(const char file_id);

#endif /* FLASHFILE_H_ */
