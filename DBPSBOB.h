/*
 * DBPSBOB.h
 *
 *  Created on: 2012-1-6
 *      Author: BetaKDE
 */

#ifndef DBPSBOB_H_
#define DBPSBOB_H_

#include "RTDB.h"

class DB_PSBOB:public RTDB
{
public:
	DB_PSBOB();
	virtual ~DB_PSBOB();
	int Init(const char*db_name = "psbob", const char* sys_name = "realtime", const char *bob_name = "scada"
			, const char *server = "", const int mode = 1);
};

#endif /* DBPSBOB_H_ */
