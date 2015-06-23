/*
 * DB_DMS.h
 *
 *  Created on: 2012-1-9
 *      Author: BetaKDE
 */

#ifndef DB_DMS_H_
#define DB_DMS_H_

#include "RTDB.h"

class DB_DMS: public RTDB
{
public:
	DB_DMS();
	virtual ~DB_DMS();

	int Init(const char*db_name = "pwrflow", const char* sys_name = "realtime", const char *bob_name = "scada_dms"
				, const char *server = "", const int mode = 1);
};

#endif /* DB_DMS_H_ */
