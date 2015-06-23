/*
 * DBPSBOB.cpp
 *
 *  Created on: 2012-1-6
 *      Author: BetaKDE
 */

#include "DBPSBOB.h"

DB_PSBOB::DB_PSBOB()
:RTDB()
{
	// TODO Auto-generated constructor stub
}

DB_PSBOB::~DB_PSBOB()
{
	// TODO Auto-generated destructor stub
}

int DB_PSBOB::Init(const char* db_name, const char* sys_name, const char *bob_name, const char *server, const int mode)
{
	return RTDB::Init(db_name, sys_name, bob_name, server, mode);
}

