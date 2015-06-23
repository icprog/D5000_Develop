/*
 * DBFDIR.cpp
 *
 *  Created on: 2012-1-9
 *      Author: BetaKDE
 */

#include "DBFDIR.h"

DB_FDIR::DB_FDIR()
:RTDB()
{
	// TODO Auto-generated constructor stub
}

DB_FDIR::~DB_FDIR()
{
	// TODO Auto-generated destructor stub
}

int DB_FDIR::Init(const char* db_name, const char* sys_name, const char *bob_name, const char *server, const int mode)
{
	return RTDB::Init(db_name, sys_name, bob_name, server, mode);
}

