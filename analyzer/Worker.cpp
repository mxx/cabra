/*
 * User.cpp
 *
 *  Created on: Dec 1, 2012
 *      Author: mxx
 */

#include "Worker.h"
#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/Dbo/Impl>

DBO_INSTANTIATE_TEMPLATES(Worker);


Worker::Worker() :
		role(Guest)
{

}

Worker::~Worker()
{

}

