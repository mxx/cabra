/*
 * 360hour_pos_loh.h
 *
 *  Created on: Sep 15, 2012
 *      Author: mxx
 */

#ifndef R360HOUR_POS_LOH_H_
#define R360HOUR_POS_LOH_H_

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
