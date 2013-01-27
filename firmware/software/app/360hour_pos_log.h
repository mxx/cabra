/*
 * 360hour_pos_log.h
 *
 *  Created on: Sep 15, 2012
 *      Author: mxx
 */

#ifndef R360HOUR_POS_LOG_H_
#define R360HOUR_POS_LOG_H_
//This the record every one minute
#include "current_state.h"

struct position_record
{
	unsigned char speed;
	GISPosition pos;
};

typedef struct position_record PositionRecord;

struct position_record_block
{
	unsigned int TimeTag;
	unsigned short RecordNumber;
};

#endif /* 360HOUR_POS_LOH_H_ */
