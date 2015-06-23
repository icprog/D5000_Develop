/*
 * RTDB.cpp
 *
 *  Created on: 2012-1-9
 *      Author: BetaKDE
 */

#include "RTDB.h"

RTDB::RTDB()
{
	// TODO Auto-generated constructor stub
	m_db = new DB_DESCR;
	m_tb_descr = NULL;
	m_szTables = NULL;
	m_cnt_Tables = OO_FAIL;

	systemname[0] = 0;
	bobname[0] = 0;
	databasename[0] = 0;
	servername[0] = 0;

	oodbinit();
}

RTDB::~RTDB()
{
	// TODO Auto-generated destructor stub
	m_map_tb.clear();

	delete m_db;

	if (m_cnt_Tables != OO_FAIL)
	{
		for (int i = 0; i < m_cnt_Tables; i++)
		{
			if(m_tb_descr[i]->ptab != NULL || m_tb_descr[i]->ptabnet != NULL)
				CloseTable(m_tb_descr[i]);

			delete m_tb_descr[i];
			delete m_szTables[i];
		}

		delete m_tb_descr;
		delete m_szTables;
	}
}

int RTDB::Init(const char * db_name, const char* sys_name, const char *bob_name, const char *server, const int mode)
{
	assert(db_name != NULL);
	assert(sys_name != NULL);
	assert(bob_name != NULL);

	strcpy(systemname, sys_name);
	strcpy(bobname, bob_name);
	strcpy(databasename, db_name);
	if (server != NULL && server[0] != 0)
		strcpy(servername, server);
	else
		gethostname(servername, 256);

	if (oodbopen(const_cast<char*>(db_name), const_cast<char*>(sys_name), const_cast<char*>(bob_name), const_cast<char*>(servername), mode, m_db) != OO_SUCCEED)
	{
		printf("%s oodbopen failed, errno:%d, line:%d\r\n", __FUNCTION__, _oodb_errno, __LINE__);
		return OO_FAIL;
	}

	vector<int> vec;
	COdbSystem *ptab = new COdbSystem();
	int appno;
	int ret = ptab->GetAllOdbNoByAppName(appno, vec, bobname);

	if (ret == 0)
	{
		m_cnt_Tables = vec.size();

		int i = 0;

		m_tb_descr = new TB_DESCR*[m_cnt_Tables];
		m_szTables = new char*[m_cnt_Tables];

		for (i = 0; i < m_cnt_Tables; i++)
		{
			m_szTables[i] = new char[ENG_NAME_LEN];
			m_tb_descr[i] = new TB_DESCR;
			bzero(m_tb_descr[i], sizeof(TB_DESCR));
		}

		for (i = 0; i < m_cnt_Tables; i++)
		{
			ptab->GetTableNameByNo(m_szTables[i], vec[i], true);

//			printf("%s::%s\r\n", bobname, m_szTables[i]);

			m_map_tb[m_szTables[i]] = m_tb_descr[i];
		}
	}

	delete ptab;

#if 0
	printf("/********************************************/\r\n");
	printf("/*  态名：%s   应用名：%s\r\n", sys_name, bob_name);
	printf("/********************************************/\r\n");

	printf("%-32s%-10s%-20s%-20s\r\n", "table_name", "table_id", "CTableOp", "CTableNet");
	for (map<string, TB_DESCR*>::iterator it = m_map_tb.begin(); it != m_map_tb.end(); it++)
	{

		if (oodbuse(const_cast<char*>(it->first.c_str()), m_db, it->second) != OO_SUCCEED)
		{
			printf("%s use %s failed, errno:%d, line:%d\r\n", __FUNCTION__, it->first.c_str(), _oodb_errno, __LINE__);


			it->second = NULL;
			continue;
		}

		printf("%-32s%-10d0x%-20x0x%-20x\r\n", it->second->table_name, it->second->table_id, it->second->ptab, it->second->ptabnet);
	}
	cout << endl;
#endif

	return OO_SUCCEED;
}
