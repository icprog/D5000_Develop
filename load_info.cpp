/*
 * load_info.cpp
 *
 *  Created on: 2012-11-7
 *      Author: tianyq
 */

#include <iostream>
#include "dsbob.h"
#include "dsbob2.h"
#include "Nmd.h"
#include "Tp.h"
#include "FDIRR2.h"
#include "comm.h"
#include "DBPSBOB.h"
#include "DBDMS.h"
#include "DBFDIR.h"
#include "netmom.h"
#include <db_struct_scada.h>
#include <sys_menu_info.h>
#include "CntLdDv.h"

#define		MAX_SQL_LEN				40960
#define   pthread_mutexattr_default	NULL

DSBOB2 *g_bob2 = NULL;
DSBOB *g_bob=NULL;
NMD *nmd = NULL;
DB_PSBOB *g_db_psbob = NULL;
DB_DMS *g_db_dms = NULL;
DB_FDIR *g_db_fdir = NULL;
FDIR_R2 *g_fdir_r = NULL;
FDIR_CONFIG g_FdirConfig = {0};
FDIR_TASK_LIST g_lstTask;
FDIR_POINT_TRIG_LIST g_lstTrig;//�Ϳ��ع����Ĺ����ź��б�
FDIR_POINT_TRIG_SECTION	g_lstSecTrig;//�߶��ϵĹ����ź��б�
pthread_mutex_t oci_mutex;
pthread_mutex_t	trig_list_mutex;
INTR_OCI *g_oci = NULL;
//char sql[30720] = {0};
char *sql = new char[MAX_SQL_LEN];
int version = 0;
//���ڹ���ָʾ���ź�Ϊ�����������϶�λ
map<long, long> g_map_fi_dv;	//��һ��stringΪ����ָʾ����ID����һ��stringΪ�ù���ָʾ����Ӧ��DV��ID
//map<long, list<long> > g_map_trig_fi;	//stringΪDV��ID��list<string>Ϊ��DV���������źŵĹ���ָʾ���б�
//���ڹ���ָʾ���ź�Ϊ�����������϶�λ
//////////////////////////////////////

map<long, list<long> > g_map_nd_bus, g_map_nd_switch;//����nd��bus��nd�͵�բ�Ĺ�ϵ��������ٵ�ͨ��nd��id�鵽������ĸ�ߺ͵�բ20120918
list<long> lst_lwcbs;//�������翪�ص�id���ڶ����ʱ��ͱ������

int GetStbyDv(long dvid, PSBOB::ST &st);
int GetDv(long devid, const char *devtb, PSBOB::DV &dv);
bool pid_to_id(long &id);
bool id_to_pid(long &id);

int ExecSQL(const char *sql)
{
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->Exec_noBind(sql);
	pthread_mutex_unlock(&oci_mutex);

	usleep(50);

	if (ret == OCI_ERROR)
	{
		cout << "\033[31m********************ExecSQL failed**************\033[0m" << endl;
	}

	return ret;
}

int GetDv(long devid, const char *devtb, PSBOB::DV &dv)
{
	//D5000��ȡDV
	PSBOB::ST psbob_st = { 0 };
	id_to_pid(devid);

	if (strcasecmp(devtb, "ld") == 0)
	{
		PSBOB::LD psbob_ld = { 0 };

		if (oodbread_rk(&psbob_ld, &devid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), sizeof(psbob_ld)) > 0)
		{
			if (oodbread_rk(&psbob_st, &psbob_ld.st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
			{
				return oodbread_rk(&dv, &psbob_st.subarea_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(dv));
			}
			else
			{
				printf("%s(%d):��ȡsubstation��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, psbob_ld.st_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):��ȡenergyconsumer��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
		}
	}
	else if (strcasecmp(devtb, "bus") == 0)
	{
		PSBOB::BUS psbob_bus = { 0 };

		if (oodbread_rk(&psbob_bus, &devid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(psbob_bus)) > 0)
		{
			if (oodbread_rk(&psbob_st, &psbob_bus.st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
			{
				return oodbread_rk(&dv, &psbob_st.subarea_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(dv));
			}
			else
			{
				printf("%s(%d):��ȡsubstation��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, psbob_bus.st_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):��ȡbusbarsection��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
		}
	}
	else if (strcasecmp(devtb, "sec") == 0)
	{
		PSBOB::SEC psbob_sec = { 0 };

		if (oodbread_rk(&psbob_sec, &devid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), sizeof(psbob_sec)) > 0)
		{
			if (oodbread_rk(&psbob_st, &psbob_sec.ist_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
			{
				return oodbread_rk(&dv, &psbob_st.subarea_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(dv));
			}
			else
			{
				printf("%s(%d):��ȡsubstation��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, psbob_sec.ist_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):��ȡaclinesegment��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
		}
	}
	else if (strcasecmp(devtb, "cb") == 0)
	{
		PSBOB::CB psbob_cb = { 0 };

		if (oodbread_rk(&psbob_cb, &devid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) > 0)
		{
			if (oodbread_rk(&psbob_st, &psbob_cb.st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
			{
				return oodbread_rk(&dv, &psbob_st.subarea_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(dv));
			}
			else
			{
				printf("%s(%d):��ȡsubstation��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, psbob_cb.st_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):��ȡbreaker��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ������:%ld\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
		}
	}

	return -1;
}

int GetStbyDv(long dvid, PSBOB::ST &st)
{
	//D5000��ȡDV
	_oodb_errno = OO_SUCCEED;
	PSBOB::ST psbob_st[20] = { 0 };
	//��ȡst
	id_to_pid(dvid);
	int re_size = oodbread_ra(psbob_st, "subarea_id", &dvid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st));
	if (re_size <= 0)
	{
		TRACE("��ȡsubstation��subarea_id����Ϊ:%ld�ļ�¼ʧ�ܣ�errno:%d\r\n", dvid, _oodb_errno);
		return OO_FAIL;
	}
	else
	{
		int st_num = 0;
		st_num = re_size / sizeof(PSBOB::ST);
		for (int j = 0; j < st_num; j++)
		{

			if ((psbob_st[j].st_type == MENU_FAC_TYPE_FEEDVIR) || (psbob_st[j].st_type == MENU_FAC_TYPE_SUBSTN))
			{
				memcpy(&st, &psbob_st[j], sizeof(PSBOB::ST));

				printf("the st id is %ld\n", st.id);
				return OO_SUCCEED;
			}
		}

		return OO_FAIL;
	}
}

void abrt(int param)
{
	cout << "���򼴽��˳���" << param << endl;
	cout << "����ID��" << getpid() << endl;

	//g_bob2->WriteReDB(NULL, NULL, NULL);

	delete g_db_dms;
	delete g_db_fdir;
	delete g_db_psbob;

	exit(0);
}

bool pid_to_id(long &id)
{
	map<string, TB_DESCR*> map_table;

	map_table["breaker"] = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CB"));
	map_table["aclinesegment"] = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SEC"));
	map_table["energyconsumer"] = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_LD"));
	map_table["busbarsection"] = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_BUS"));
	map_table["company"] = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CO"));
	map_table["substation"] = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_ST"));

	KEY_STRU ks = { 0 };

	CCommon::long_to_keyid(id, &ks);

	TB_DESCR* tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB(ks.table_no));

	if (tb_descr == NULL)
	{
		return false;
	}

	map<string, TB_DESCR*>::iterator it = map_table.find(tb_descr->table_name);

	if (it == map_table.end())
		return false;

	long value;

	if (oodbread_aa(&value, "pid", &id, "id", it->second, sizeof(long)) <= 0)
	{
		return false;
	}

	id = value;

	return true;
}

bool id_to_pid(long &id)
{
	KEY_STRU ks = { 0 };

	CCommon::long_to_keyid(id, &ks);

	TB_DESCR* tb_descr = const_cast<TB_DESCR*>(g_db_dms->GetTB(ks.table_no));

	if (tb_descr == NULL)
	{
		return false;
	}

	long value;

	if (oodbread_ak(&value, &id, "pid", tb_descr, sizeof(long)) != OO_SUCCEED)
	{
		return false;
	}

	id = value;

	return true;
}

//���ݼ�¼ID��ȡ��¼��Ӣ�ı�ʶ
bool GetCode(const long id, string &code)
{
	if (id <= 0) return false;

	KEY_STRU ks = {0};
	long key = 0;

	CCommon::long_to_keyid(id, &ks);

	if(ks.table_no <= 0) return false;


	TB_DESCR *tb_descr = const_cast<TB_DESCR*>(g_db_dms->GetTB(ks.table_no));
	if(tb_descr == NULL)
	{
		tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB(ks.table_no));

		if(tb_descr == NULL) return false;

		key = id;
	}
	else
	{
		if(oodbread_ak(&key, const_cast<long *>(&id), "pid", tb_descr, sizeof(long)) != OO_SUCCEED)
		{
			return false;
		}
	}

	if(key <= 0) return false;

	char buf[32];

	CCommon::long_to_keyid(key, &ks);

	if (oodbread_ak(buf, &key, "code", const_cast<TB_DESCR*>(g_db_psbob->GetTB(ks.table_no)), sizeof(char) * 32) != OO_SUCCEED)
	{
		return false;
	}

	code = buf;

	return true;
}


int main(int _argc, char *_argv[])
{
	timeval stimer = {0};
	timeval etimer = {0};

	signal(SIGABRT, abrt);
	signal(SIGINT, abrt);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCONT, SIG_IGN);


	g_db_psbob = new DB_PSBOB;
	g_db_dms = new DB_DMS;
	g_db_fdir = new DB_FDIR;
	if (g_db_psbob->Init() == OO_FAIL || g_db_dms->Init() == OO_FAIL || g_db_fdir->Init() == OO_FAIL)
	{
		delete g_db_psbob;
		delete g_db_dms;
		delete g_db_fdir;

		return -1;
	}

	g_oci = new INTR_OCI;

	if (g_oci->connect(getenv("DMS_OCI_USER"), getenv("DMS_OCI_PWD"), getenv("DMS_OCI_SRV")) == OCI_ERROR)
	{
		TRACE("�������ݿ�ʧ�ܣ�user:%s,pwd:%s,srv:%s\r\n", getenv("DMS_OCI_USER"), getenv("DMS_OCI_PWD"), getenv("DMS_OCI_SRV"));
		return -1;
	}

	g_bob2 = new DSBOB2;

	if (!g_bob2->ReadPSBOB())
	{
		cout << "��ȡPSBOB��ʧ��!" << endl;

		return -1;
	}

	nmd = new NMD(g_bob2);

	if (!nmd->DoValidate())
	{
		cout << __LINE__<<":NMD ����ʧ�ܣ�" << endl;

		return -1;
	}

	//����ZN��
	MAP_SO_PATH so_path;

	if (!nmd->find_so(NULL, NULL, so_path, 4))
	{
		cout << __LINE__<<":����ZNʧ�ܣ�" << endl;

		return -1;
	}

	//���ϻָ�
	g_bob = new DSBOB;

	g_bob->ReadFromDsbob2(g_bob2);

	if (pthread_mutex_init(&oci_mutex, pthread_mutexattr_default))
	{
		perror("Init oci_mutex error");

		return -1;
	}

	Cntld *g_cnt = new Cntld(g_bob);

	g_cnt->Init();

	while(1)
	{
		gettimeofday(&stimer, NULL);

		//���¿���״̬,������ĸ�ߵ�ң����Ϣ���SO
		if (!g_cnt->count_ld())
		{
		//	break;
		}
		if (g_cnt->count_dv())
		{
			cout << "get dv success" << endl;
		}

		gettimeofday(&etimer, NULL);
		float cost = etimer.tv_sec * 1000 * 1000 + etimer.tv_usec - (stimer.tv_sec * 1000 * 1000 + stimer.tv_usec);
		TRACE(">>>>>>>> total cost:%.2fs\r\n",  cost / 1000 / 1000);

//		sleep(30);
		break;
	}

	delete g_db_psbob;
	delete g_db_dms;
	delete g_db_fdir;
	delete g_fdir_r;

	cout <<"ALL DONE" << endl;
	return 1;
}




