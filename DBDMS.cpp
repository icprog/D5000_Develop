/*
 * DBDMS.cpp
 *
 *  Created on: 2012-1-9
 *      Author: BetaKDE
 */

#include "DBDMS.h"

DB_DMS::DB_DMS()
:RTDB()
{
	// TODO Auto-generated constructor stub

}

DB_DMS::~DB_DMS()
{
	// TODO Auto-generated destructor stub
}

int DB_DMS::Init(const char*db_name, const char* sys_name, const char *bob_name, const char *server, const int mode)
{
	return RTDB::Init(db_name, sys_name, bob_name, server, mode);
}

