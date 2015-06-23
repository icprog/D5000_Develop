/*
 * RTDB.h
 *
 *  Created on: 2012-1-9
 *      Author: BetaKDE
 */

#ifndef RTDB_H_
#define RTDB_H_

#include "oodb.h"
#include <map>
#include <string>
#include <assert.h>

using namespace std;

class RTDB
{
public:
	RTDB();
	virtual ~RTDB();
	virtual int Init(const char * db_name, const char* sys_name, const char *bob_name, const char *server, const int mode);

	inline const DB_DESCR* GetDB()
	{
		return m_db;
	}

	inline const TB_DESCR* GetTB(const int tb_id)
	{
		if (tb_id <= 0)	return NULL;

		char tb_name[ENG_NAME_LEN] = { 0 };

		int ret = GetTableNameByID(tb_id, tb_name);

		if (ret >= 0)
		{
			return GetTB(tb_name);
		}

		return NULL;
	}

	inline const TB_DESCR* GetTB(const char *tb_name)
	{
		assert(tb_name != NULL);

//		printf("%s %s(%d):%s\r\n", __FILE__, __FUNCTION__, __LINE__, tb_name);

		map<string, TB_DESCR*>::iterator it = m_map_tb.find(tb_name);

//		printf("tablename:%s\r\n", it->first.c_str());

		if (it == m_map_tb.end())
		{
			//printf("态:%s,应用:%s下不存在%s表!!!\r\n", systemname, bobname, tb_name);
			return NULL;
		}

		if (it->second->ptab == NULL && it->second->ptabnet == NULL)
		{
			OpenTableByName(m_db->server, m_db->sys_name, m_db->bob_name, const_cast<char*>(tb_name), it->second);
		}

		return it->second;
	}

	inline int ResetAllTables()
	{
		for (map<string, TB_DESCR*>::iterator it = m_map_tb.begin(); it != m_map_tb.end(); it++)
		{
			ResetTable(it->first.c_str());
		}

		return OO_SUCCEED;
	}

	inline int ResetTable(const char *tb_name)
	{
		const TB_DESCR* tb_descr = GetTB(tb_name);

		if (tb_descr == NULL)
			return OO_FAIL;

		if (tb_descr->ptab != NULL)
			tb_descr->ptab->TableClear();
		else if (tb_descr->ptabnet != NULL)
			tb_descr->ptabnet->TableClear();

		return OO_SUCCEED;
	}

	inline map<string, TB_DESCR*> *GetTables()
	{
		return &m_map_tb;
	}

protected:
	DB_DESCR 	*m_db;
	TB_DESCR 	**m_tb_descr;
	char		**m_szTables;
	int 		m_cnt_Tables;
	map<string, TB_DESCR*> m_map_tb;

	char systemname[256];
	char bobname[256];
	char databasename[256];
	char servername[256];
};

#endif /* RTDB_H_ */
