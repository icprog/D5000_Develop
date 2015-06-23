/*
 * DBFDIR.h
 *
 *  Created on: 2012-1-9
 *      Author: BetaKDE
 */

#ifndef DBFDIR_H_
#define DBFDIR_H_

#include "RTDB.h"

class DB_FDIR: public RTDB
{
public:
	DB_FDIR();
	virtual ~DB_FDIR();

	int Init(const char*db_name = "fdir", const char* sys_name = "realtime", const char *bob_name = "scada_fdir"
					, const char *server = "", const int mode = 1);
};

#endif /* DBFDIR_H_ */
