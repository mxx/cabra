/*
 * current_state.h
 *
 *  Created on: Sep 15, 2012
 *      Author: mxx
 */

#ifndef CURRENT_STATE_H_
#define CURRENT_STATE_H_

struct current_state
{
	unsigned short index:4;
	unsigned short Speed:12;
	unsigned char state;
} ;

typedef struct current_state CurrentState;

struct gis_position
{
	int longitude;
	int latitude;
	short altidude;
};

typedef struct gis_position GISPosition;

struct current_state_record
{
	CurrentState state[5];
	GISPosition pos;
};

typedef struct current_state_record StateRecord;

#endif /* CURRENT_STATE_H_ */
