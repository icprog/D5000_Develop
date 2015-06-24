#include <iostream>
#include <dcisg.h>
#include <rtdb_api.h>
#include <msg_api/message_inv.h>
#include <msg_api/messageheader.h>
#include <message_channel.h>
#include <proc_inv.h>
#include <message_type.h>
#include <priv_access_new.h>
#include <scada_api/scada_event_m.h>
#include <scada_api/scada_front_struct.h>
#include <scada_api/CScaComm.h>
#include <warn_service_message_m.h>
#include "dsbob.h"
#include "dsbob2.h"
#include "Nmd.h"
#include "Tp.h"
#include "dms_tcp.h"
#include "tinyxml.h"
#include "FDIRR2.h"
#include "comm.h"
//#include <commdata.h>
#include "INTROCI.h"
#include "DBPSBOB.h"
#include "DBDMS.h"
#include "DBFDIR.h"
#include "FHZG.h"
#include "typical.h"
#include <cmath>


#define   pthread_attr_default		NULL
#define   pthread_mutexattr_default	NULL
#define   pthread_condattr_default	NULL
#define   pthread_addr_t			void *
typedef void*(*pthread_startroutine_t)(void *);

using namespace std;
//#undef _ORACLE_

//////////////////////////////////////
//用于故障指示器信号为条件启动故障定位
map<long, long> g_map_fi_dv;	//第一个string为故障指示器的ID，后一个string为该故障指示器对应的DV的ID
map<long, list<long> > g_map_trig_fi;	//string为DV的ID，list<string>为该DV下所有有信号的故障指示器列表
//用于故障指示器信号为条件启动故障定位
//////////////////////////////////////

CPrivAccessNew		g_Priv;
/////用于生成虚拟母线和进线开关
stack<NODE> g_lstPath;
list<long> g_lstFound;
bool bDebug = false;
const int max_record = 10;
map<long, list<stack<NODE> > >	g_map_nd_path;	//馈线首端节点到母线间的路径
///////////////////////////////////////////


map<long, list<long> > g_map_nd_bus, g_map_nd_switch;//保存nd和bus，nd和刀闸的关系，方便快速的通过nd的id查到关联的母线和刀闸20120918
list<long> lst_lwcbs;//保存联络开关的id，在读库的时候就保存出来

TCPClass *tcptools = NULL;
DSBOB2 *g_bob2 = NULL;
DSBOB *g_bob=NULL;
NMD *nmd = NULL;
DB_PSBOB *g_db_psbob = NULL;
DB_DMS *g_db_dms = NULL;
DB_FDIR *g_db_fdir = NULL;

//////////////////////////////////
//haozeyun 测试 添加
FHZG* fhzg = NULL;

int listenfd;
int numthread = 0;
GDI_THREAD pthread_id[MAX_THREAD];
CScaComm	*g_sca_common;
time_t		*model_chg_ntf;
time_t 		tmp_val0 = 0;
time_t		last_model_chg_ntf;
pthread_mutex_t	mutex;
pthread_mutex_t	g_rq_mutex;
pthread_mutex_t mutex_array_fault;

char gdiservices[] = "tcp";
char hostname[256] = "127.0.0.1";
const char userid[] = "dmshan";

///////////////////////
//故障隔离、恢复
#define MAX_FAULT	20
#define GROUP_TIME 10
#define		MAX_SQL_LEN				40960

// 母线电压下线值
#define MIN_BUS_VALUE 1.0

const char *key[] = {"分", "事故跳闸", "动作", "复归", "合"};
FDIR_CONFIG g_FdirConfig = { 0 };
FDIR_TASK_LIST g_lstTask;
FDIR_POINT_TRIG_LIST g_lstTrig;//和开关关联的过流信号列表
FDIR_POINT_TRIG_SECTION	g_lstSecTrig;//线段上的过流信号列表
pthread_mutex_t	task_list_mutex;
pthread_mutex_t	trig_list_mutex;
pthread_mutex_t oci_mutex;
pthread_mutex_t simu_sync_mutex;	//用于故障模拟，提示人机交互线程向人机返回报文
FDIR_R2 *g_fdir_r = NULL;
INTR_OCI *g_oci = NULL;
//bob_invocation  *bob_inv = NULL;
proc_invocation *proc_inv = NULL;
char *sql = new char[MAX_SQL_LEN];
message_invocation *bob_inv = NULL;
int iMessage;
int iProcess;
//char sql[20480] = {0};
char sqltemp[1024] = {0};
FDIR_R2 *array_fdir_r[MAX_FAULT];		//故障数组，每个故障项对应数组中的一项
bool	map_idle_fdir_r[MAX_FAULT];		//索引故障数组中的每个项的占用情况
bool	g_is_write_to_lib = false;		//故障模拟写库标志

//杨勇 用于存储开关电流平均值
std::map<long,float> g_mapIValue;
std::map<long,std::list<float> > g_mapTempIValue;
pthread_mutex_t avg_i_mutex;
//故障隔离、恢复
/////////////////////////
typedef struct
{
	long point_id;
	bool bOpen;
	int stage;
}CB_CONTROL;

bool IsMaster();	//判断主备机
void write_fhzg_step(const char* groupid, const char* table_name, list<list<MAP_SO_NODE> > lst_step, FDIR_R2* fdir_r, list<FAULT_ITEM> &lst_ld, list<long> lstIsolate/*用于保存母线负荷转供时需要断开的进线开关*/, bool bStation);
void HandlerMessage(void *para);
void thread_get_ivalue(void *param);
void thread_simu_return(void* param);
void fill_fhzg_db(const long cb_id, const char *fhzg_id, FHZG *fhzg, bool has_fhzg_cb);	//写负荷转供关系库表
int GetFHZGLeftStep(const char *FHZG_id, list<CB_CONTROL> &lstcb);
bool AddTask(FDIR_TASK *newtask);
bool RemoveTask(const char* fault_id);
bool RemoveTasks(const char *group_id);
void UpdateExtStatus(const char *group_id);
int GetStbyDv(long dvid, PSBOB::ST &st);
int GetDv(long devid, const char *devtb, PSBOB::DV &dv);
void form_fdir_txt(const FDIR_TASK fault);
int GetJxcbDv(long cbid, PSBOB::DV &dv);
void RecoTaskItem(const char* groupid);
void RecoTaskStep(const char* groupid);
void RecoMemTask(const char* groupid);
int data_verify(const char* groupid);
void write_fault_tree(const char* groupid, FDIR_R2* fdir_r);
void write_fault_up_dev(const char* fault_id, char *table_name, FDIR_R2* fdir_r, bool upiso_fail, list<FAULT_ITEM> &lst_ld);
void write_fault_dev(const char *fault_id, char *table_name, FDIR_R2* fdir_r);
bool invalid_recalc(const char *group_id);
void EraseSameInfo(list<FAULT_ITEM> &lst_cont, list<FAULT_ITEM> lst_remove);
void write_res_and_disableres_ld(const char *groupid, list<FAULT_ITEM> lst_lose, list<FAULT_ITEM> lst_res);
void form_jxcb_control_txt(const char *group_id, const long point_name, const bool bOpen);
bool IsInRegion(MLang::Long cb_id);
bool pid_to_id(long &id);
bool id_to_pid(long &id);
bool GetCode(const long id, string &code);
long IDTransfor(long &id);
int Notify(const char *grp_id, const long dv_id, const long st_id, char op=-1);
int AddToken(const char *fault_id, const long dev_id, int token_type);

typedef struct
{
	char picturename[256];
	char contextname[20];
	char bobname[20];
	char st[100];
	char equipment_type[2];
} ORDER, *PORDER;


//故障模拟数据结构
typedef struct
{
	char plan_id[50];
	int  type;
	char plan_name[20];
	char machine_name[50];
	char usr_id[50];
}SIMU_PLAN;


typedef struct
{
	long cb_id;
	int  status;
	float Ia;
	float Ib;
	float Ic;
	float p;
	float q;
}CHECK_DATA;

typedef std::list<CHECK_DATA> CHECK_LIST;

typedef struct
{
	char plan_id[50];
	long sig_id;
	char sig_name[64];
	int	 status;
	int  type;
	int  sig_delay;
	char comment[50];
}SIMU_SIGNAL;

typedef std::list<SIMU_SIGNAL> SIMU_SIG_LIST;
typedef map<std::string, FDIR_POINT_TRIG_LIST>	MACHINE_TRIG_MAP;
MACHINE_TRIG_MAP simu_mac_trig;

//设备着色响应报文头
const char head_xml[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"Equipment_Color\"/>\r\n\
<Body>\r\n";

//设备着色响应报文头
const char head_xml4[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"study_fdir\"/>\r\n\
<Body>\r\n";

//响应故障执行步骤方案
const char head_xml5[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"FDIR_STEP\"/>\r\n\
<Body>\r\n";

//响应典型运行方式报文
const char head_xml6[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"typical\"/>\r\n\
<Body>\r\n";

//响应数据校验报文
const char head_xml7[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"check_data\"/>\r\n\
<Body>\r\n";

//响应故障重新计算报文
const char head_xml8[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"recalc\"/>\r\n\
<Body>\r\n";

//设备着色响应报文尾
const char tail_xml[] = "</Order>\r\n\
</Body>\r\n\
</Message>\r\n\
";

//防误闭锁判断响应报文头
const char head_xml2[] = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type=\"misoperation\"/>\r\n\
<Body>\r\n";

const char head_xml3[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"FHZG\"/>\r\n\
<Body>\r\n";

//防误闭锁判断响应报文尾
const char tail_xml2[] = "</Body>\r\n\
</Message>\r\n\
";

//测试数据
const char test_misoperation[] = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\
<Message>\
    <Header type=\"misoperation\" />\
    <Body>\
        <operation pictureName=\"FUJIAN/可视化测试/0001.epp\" contextname=\"realtime\" bobname=\"dms\">\
            <order cb=\"KSLCBKL44\" op_type=\"1\" />\
        </operation>\
    </Body>\
</Message>";


//故障模拟保存方案报文头
const char head_xml_simu_save[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
<Message>\r\n\
<Header type = \"fault_simu_save\"/>\r\n\
<Body>\r\n";

//故障模拟保存方案报文尾
const char tail_xml_simu[] = "</Simu_Plan>\r\n\
</Body>\r\n\
</Message>\r\n\
";

//故障模拟保存方案报文头
const char head_xml_simu_exec[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\<Message>\r\n\<Header type = \"fault_simu_exec\"/>\r\n\<Body>\r\n";


///////////////////////////////////////////////////////////////////////////////
int GetRecsByAttr(const TB_DESCR* tb_descr,  const char* attr_name, const char* attr_value, void* buf, long len_buf)
{
	int field_no[2] = { 0 };
	int fieldnum = 0;
	int ret_code = GetFieldNoByName(const_cast<TB_DESCR*>(tb_descr), const_cast<char*>(attr_name), field_no, &fieldnum);
	if (ret_code < 0)
	{
		return ret_code;
	}

	char* buf_ptr = NULL;
	ret_code = GetRecsByAttr(const_cast<TB_DESCR*>(tb_descr), field_no[0], EQU, const_cast<char*>(attr_value), 0, &buf_ptr);
	if (0 >= ret_code)
	{
		if (NULL != buf_ptr)
		{
			BufFree(buf_ptr);
			buf_ptr = NULL;
		}

		return ret_code;
	}

	if(ret_code > len_buf)
	{
		BufFree(buf_ptr);
		buf_ptr = NULL;
		return -1;
	}

	memcpy(buf, buf_ptr, ret_code);

	BufFree(buf_ptr);
	buf_ptr = NULL;

	return ret_code;
}

int FindCB(PSBOB::CB* buf, const long nd)
{
	int cnt = 0;

	int ret = GetRecsByAttr(g_db_psbob->GetTB("breaker"), "ind", (const char*)&nd, buf, sizeof(PSBOB::CB) * max_record);//buf, "ind", (char*)&nd, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(CB) * max_record);

	if (ret > 0)
	{
		cnt = ret / sizeof(PSBOB::CB);
	}

	ret = GetRecsByAttr(g_db_psbob->GetTB("breaker"), "jnd", (const char*)&nd, buf + cnt, sizeof(PSBOB::CB) * (max_record - cnt));

	if (ret > 0)
	{
		cnt += ret / sizeof(PSBOB::CB);
	}

	return cnt;
}

int FindSWITCH(PSBOB::SWITCH* buf, const long nd)
{
	int cnt = 0;

	int ret = GetRecsByAttr(g_db_psbob->GetTB("disconnector"), "ind", (const char*)&nd, buf, sizeof(PSBOB::SWITCH) * max_record);

	if (ret > 0)
	{
		cnt = ret / sizeof(PSBOB::SWITCH);
	}

	ret = GetRecsByAttr(g_db_psbob->GetTB("disconnector"), "jnd", (const char*)&nd, buf + cnt, sizeof(PSBOB::SWITCH) * (max_record - cnt));

	if (ret > 0)
	{
		cnt += ret / sizeof(PSBOB::SWITCH);
	}

	return cnt;
}

int FindLD(PSBOB::LD* buf, const long nd)
{
	int cnt = 0;

	int ret = GetRecsByAttr(g_db_psbob->GetTB("energyconsumer"), "nd", (const char*)&nd, buf, sizeof(PSBOB::LD) * max_record);

	if (ret > 0)
	{
		cnt = ret / sizeof(PSBOB::LD);
	}

	return cnt;
}

int FindSEC(PSBOB::SEC* buf, const long nd)
{
	int cnt = 0;

	MyBufManager bf_end(max_record * sizeof(PSBOB::ACLINEEND));

	int ret = GetRecsByAttr(g_db_psbob->GetTB("aclineend"), "nd", (const char*) &nd, (void*)bf_end, sizeof(PSBOB::ACLINEEND) * max_record);

	if (ret > 0)
	{
		int max = ret / sizeof(PSBOB::SEC);

		PSBOB::ACLINEEND *p = (void*) (bf_end);
		PSBOB::SEC* pbuf = buf;
		for (int i = 0; i < max; i++)
		{
			if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), (char*) &p->aclnseg_id, sizeof(PSBOB::SEC), (char*) pbuf) >= 0)
			{
				pbuf++;
				cnt++;
			}
			else
			{
				printf("aclinesegment中找不着ID为%ld的记录\r\n", p->aclnseg_id);
			}

			p++;
		}
	}

//	ret = GetRecsByAttr(g_db_psbob->GetTB("aclinesegment"), "jnd", (const char*)&nd, buf + cnt, sizeof(SEC) * (max_record - cnt));
//
//	if (ret > 0)
//	{
//		cnt += ret / sizeof(SEC);
//	}

	return cnt;
}

int FindBUS(PSBOB::BUS* buf, const long nd)
{
	int cnt = 0;

	MyBufManager bf_bus(max_record * sizeof(PSBOB::BUS));

	int ret = GetRecsByAttr(g_db_psbob->GetTB("busbarsection"), "nd", (const char*)&nd, (void*)bf_bus, sizeof(PSBOB::BUS) * max_record);

	if(ret > 0)
	{
		cnt = ret / sizeof(PSBOB::BUS);

		int j = 0;

		for (int i = 0; i < cnt; i++)
		{
			PSBOB::ST psbob_st = { 0 };
			if (oodbread_rk(&psbob_st, &(((PSBOB::BUS*) bf_bus)[i].st_id), const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
			{
				if (HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type , MENU_FAC_TYPE_CABLEBRANCHBOX))	continue;
				if (HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type , MENU_FAC_TYPE_DISTRIBUTEBOX))	continue;
				if (HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type , MENU_FAC_TYPE_RINGMAINUNIT))		continue;
				if (HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type , MENU_FAC_TYPE_SWITCHING))		continue;
			}
			else
			{
				continue;
			}

			memcpy(&buf[j], &((PSBOB::BUS*) bf_bus)[i], sizeof(PSBOB::BUS));
			j++;
		}

		cnt = j;
	}

	return cnt;
}

void Find(const long nd, const long org_nd, NODE &data)
{
	//assert(nd > 0 && id > 0);
	NODE node = { 0 };
	g_lstPath.push(data);

	MyBufManager bf_bus(max_record * sizeof(PSBOB::BUS));
	MyBufManager bf_cb(max_record * sizeof(PSBOB::CB));
	MyBufManager bf_ld(max_record * sizeof(PSBOB::LD));
	MyBufManager bf_sec(max_record * sizeof(PSBOB::SEC));
	MyBufManager bf_switch(max_record * sizeof(PSBOB::SWITCH));
	int cnt = 0, i = 0;

	//碰到终止条件
	if (nd <= 0 /*|| FindBUS(bf_bus, nd) > 0 || FindLD(bf_ld, nd) > 0*/)
	{
		if (bDebug)
		{
			if (g_lstPath.top().type == _BUS)
				printf("POP->%ld[%s]\r\n", g_lstPath.top().data.bus.id, g_lstPath.top().data.bus.name);
			else if (g_lstPath.top().type == _LD)
				printf("POP->%ld[%s]\r\n", g_lstPath.top().data.ld.id, g_lstPath.top().data.ld.name);
			else if (g_lstPath.top().type == _SWITCH)
				printf("POP->%ld[%s]\r\n", g_lstPath.top().data.disconnector.id, g_lstPath.top().data.disconnector.name);
			else if (g_lstPath.top().type == _CB)
				printf("POP->%ld[%s]\r\n", g_lstPath.top().data.cb.id, g_lstPath.top().data.cb.name);
			else
				printf("POP->%ld[%s]\r\n", g_lstPath.top().data.sec.id, g_lstPath.top().data.sec.name);
		}

		g_lstPath.pop();
		return;
	}


	//BUS
	cnt = FindBUS(bf_bus, nd);
	PSBOB::BUS *pbus = (PSBOB::BUS*)bf_bus;

	if (cnt > 0)
	{
		TRACE("通过结点%ld找到母线%ld[%s]\r\n", nd, pbus->id, pbus->name);
	}

	//考虑一个节点连接一条母线
	if ((cnt > 0) && !HasMask(g_db_psbob->GetTB("busbarsection"), "bs_type", pbus->bs_type, MENU_BS_TYPE_ANCILLBUS))
	{
		memcpy(&node.data.bus, pbus, sizeof(PSBOB::BUS));
		node.type = _BUS;
		g_lstPath.push(node);
		g_map_nd_path[org_nd].push_back(g_lstPath);
		g_lstPath.pop();	//弹出母线
		g_lstPath.pop();	//弹出其它设备

		return;
	}

	//SEC
	cnt = FindSEC(bf_sec, nd);
	PSBOB::SEC *psec = (PSBOB::SEC*)bf_sec;

	for (i = 0; i < cnt; i++, psec++)
	{
		if(!is_substn_dev(psec->ist_id) && !is_substn_dev(psec->jst_id)) continue;

		if (std::find(g_lstFound.begin(), g_lstFound.end(), psec->id) == g_lstFound.end())
		{
			if (bDebug)
				printf("PUSH->sec %-20ld[%-30s]%-20ld%-20ld[%-20ld]\r\n", psec->id, psec->name, psec->ind, psec->jnd, nd);
			g_lstFound.push_back(psec->id);
			memcpy(&node.data.sec, psec, sizeof(PSBOB::SEC));
			node.type = _SEC;

			if (nd == psec->ind)
			{
				Find(psec->jnd, org_nd, node);
			}
			else
			{
				Find(psec->ind, org_nd, node);
			}
		}
	}

	//CB
	cnt = FindCB(bf_cb, nd);
	PSBOB::CB *pcb = (PSBOB::CB*)bf_cb;

	for (i = 0; i < cnt; i++, pcb++)
	{
		if(!is_substn_dev(pcb->st_id)) continue;

		if (std::find(g_lstFound.begin(), g_lstFound.end(), pcb->id) == g_lstFound.end())
		{
			if (bDebug)
				printf("PUSH->cb  %-20ld[%-30s]%-20ld%-20ld[%-20ld]\r\n", pcb->id, pcb->name, pcb->ind, pcb->jnd, nd);
			g_lstFound.push_back(pcb->id);

			memcpy(&node.data.cb, pcb, sizeof(PSBOB::CB));
			node.type = _CB;

			if (nd == pcb->ind)
			{
				Find(pcb->jnd, org_nd, node);
			}
			else
			{
				Find(pcb->ind, org_nd, node);
			}
		}
	}

	//SWITCH
	cnt = FindSWITCH(bf_switch, nd);
	PSBOB::SWITCH *pswitch = (PSBOB::SWITCH*)bf_switch;

	for (i = 0; i < cnt; i++, pswitch++)
	{
		if(!is_substn_dev(pswitch->st_id)) continue;

		if (std::find(g_lstFound.begin(), g_lstFound.end(), pswitch->id) == g_lstFound.end())
		{
			if (bDebug)
				printf("PUSH->swh  %-20ld[%-30s]%-20ld%-20ld[%-20ld]\r\n", pswitch->id, pswitch->name, pswitch->ind, pswitch->jnd, nd);
			g_lstFound.push_back(pswitch->id);

			memcpy(&node.data.cb, pswitch, sizeof(PSBOB::SWITCH));
			node.type = _SWITCH;

			if (nd == pswitch->ind)
			{
				Find(pswitch->jnd, org_nd, node);
			}
			else
			{
				Find(pswitch->ind, org_nd, node);
			}
		}
	}

	if (bDebug)
	{
		if (g_lstPath.top().type == _BUS)
			printf("POP->%ld[%s]\r\n", g_lstPath.top().data.bus.id, g_lstPath.top().data.bus.name);
		else if (g_lstPath.top().type == _LD)
			printf("POP->%ld[%s]\r\n", g_lstPath.top().data.ld.id, g_lstPath.top().data.ld.name);
		else if (g_lstPath.top().type == _SWITCH)
			printf("POP->%ld[%s]\r\n", g_lstPath.top().data.disconnector.id, g_lstPath.top().data.disconnector.name);
		else if (g_lstPath.top().type == _CB)
			printf("POP->%ld[%s]\r\n", g_lstPath.top().data.cb.id, g_lstPath.top().data.cb.name);
		else
			printf("POP->%ld[%s]\r\n", g_lstPath.top().data.sec.id, g_lstPath.top().data.sec.name);
	}
	g_lstPath.pop();

	return;
}

//根据馈线首端节点ID建立映射
void build_map2()
{
	char *buf = NULL;

	int ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), 1/*读取有效记录*/, &buf);

	if (ret > 0)
	{
		int size = sizeof(PSBOB::DV);
		int cnt = ret / size;

		PSBOB::DV dv = { 0 };

		for (int i = 0; i < cnt; i++)
		{
			while (g_lstPath.size() > 0)
			{
				g_lstPath.pop();
			}
			memcpy(&dv, buf + i * size, size);

			if (HasMask(g_db_psbob->GetTB("subcontrolarea"), "dtype", dv.dtype , MENU_DV_TYPE_FD))
			{
				if (dv.nd_id > 0)
				{
					NODE data = { 0 };
					data.type = _LD;

					data.data.ld.nd = dv.nd_id;

					TRACE("建立结点%ld与母线的映射关系…\r\n", dv.nd_id);

					Find(dv.nd_id, dv.nd_id, data);
				}
			}
		}

		BufFree(buf);
	}
}

//根据进线开关负荷建立映射
void build_map()
{
	char *buf = NULL;

	int ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), 1/*读取有效记录*/, &buf);

	if (ret > 0)
	{
		int size = sizeof(PSBOB::LD);
		int cnt = ret / size;

		PSBOB::LD ld = { 0 };
		PSBOB::ST st = { 0 };

		for (int i = 0; i < cnt; i++)
		{
			while(g_lstPath.size() > 0)
			{
				g_lstPath.pop();
			}
			memcpy(&ld, buf + i * size, size);

			if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), (char*) &ld.st_id, sizeof(st), (char*) &st) >= 0)
			{
				if(HasMask(g_db_psbob->GetTB("substation"), "st_type", st.st_type , MENU_FAC_TYPE_SUBSTN))
				{
					NODE data = { 0 };
					data.type = _LD;
					memcpy(&data.data.ld, &ld, size);
					Find(ld.nd, ld.nd, data);
				}
			}
		}

		BufFree(buf);
	}
}
/////////////////////////////////////////////////////////////////////


int ExecSQL(const char *sql)
{
#ifndef _FAST_DEBUG
	//备机不写关系库
	if (!IsMaster())
	{
		cout << "备机不写关系库！" << endl;
		return 0;
	}
#endif

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

void splitstring(const char *str, const char ch, list<long> *plist)
{
	assert(str != NULL && plist != NULL);
	if(str == NULL || plist == NULL) return;

	plist->clear();

	const char *pstart = str;
	const char *pend = NULL;

	while(true)
	{
		pend = strchr(pstart, ch);

		if(pend != NULL)
		{
			int len = pend- pstart;

			char *tmp = new char[len + 1];

			strncpy(tmp, pstart, len);
			tmp[len] = 0;

			if(tmp[0] != 0)
			{
				plist->push_back(atol(tmp));
			}

			pstart = pend + 1;

			delete []tmp;
		}
		else
		{
			if(pstart[0] != 0)
			{
				plist->push_back(atol(pstart));
			}

			break;
		}
	}
}

bool DoConvert()
{
//	char *buf = NULL;
//	int rec_num, attr_num;
//	struct ORA_ATTR *attrs = NULL;
//
//	//////tianyq
//	pthread_mutex_lock(&oci_mutex);
//	int ret = g_oci->ReadWithBind("select id,zone_cb from fdir_fdirect", &buf, &rec_num, &attr_num, &attrs);
//	pthread_mutex_unlock(&oci_mutex);
//
//	if (ret != OCI_ERROR)
//	{
//		if (rec_num > 0)
//		{
//			int attr_len = 0;
//
//			for (int j = 0; j < attr_num; j++)
//			{
//				attr_len += attrs[j].col_width;
//			}
//
//			char id[60], cb_id[500];
//			int izone;
//
//			for (int i = 0; i < rec_num; i++)
//			{
//
//				list<long> lst_cb;
//
//				bzero(id, sizeof(id));
//				bzero(cb_id, sizeof(cb_id));
//
//				strncpy(id, buf + i * attr_len, attrs[0].col_width);
//				strncpy(cb_id, buf + i * attr_len + attrs[0].col_width, attrs[1].col_width);
//
//				splitstring(cb_id, ';', &lst_cb);
//
//				izone = g_fdir_r->GetZoneByCB(lst_cb);
//
//				if (izone >= 0)
//				{
//
//					list<int> lstfaultzone;
//					list<FAULT_ITEM> devs;
//
//					g_fdir_r->getlstfaultzone(izone, lstfaultzone);
//					map<string, string> faultzone_cb;
//					faultzone_cb.clear();
//					for (list<int>::iterator list_it = lstfaultzone.begin(); list_it != lstfaultzone.end(); list_it++)
//					{
//						g_fdir_r->get_zone_devs((*list_it), devs);
//
//						for (list<FAULT_ITEM>::iterator it = devs.begin(); it != devs.end(); it++)
//						{
//							if (strcmp(it->table.c_str(), "CB") == 0)
//							{
//								if (faultzone_cb.find(it->id) != faultzone_cb.end())
//								{
//									//设置边界开关
//									printf("update FDIR_FAULT_DEV set edge = 0 where id = \'%s\' and dev_id = \'%s\';\r\n", id, it->id.c_str());
//								}
//								else
//								{
//									faultzone_cb[it->id] = "CB";
//									printf("INSERT INTO FDIR_FAULT_DEV (ID, DEV_ID, DEV_TABLE, EDGE) VALUES (\'%s\', \'%s\', \'%s\', %d);\r\n", id, it->id.c_str(), it->table.c_str(), 1);
//								}
//							}
//							else
//							{
//								printf("INSERT INTO FDIR_FAULT_DEV (ID, DEV_ID, DEV_TABLE, EDGE) VALUES (\'%s\', \'%s\', \'%s\', %d);\r\n", id, it->id.c_str(), it->table.c_str(), 0);
//							}
//
//						}
//					}
//
//				}
//			}
//		}
//
//		g_oci->Readdata_Free();
//	}
//
//	//////tianyq
//
//	pthread_mutex_lock(&oci_mutex);
//	ret = g_oci->ReadWithBind("select id,cb_id,so_id from fhzg_zone", &buf, &rec_num, &attr_num, &attrs);
//	pthread_mutex_unlock(&oci_mutex);
//
//	if (ret != OCI_ERROR)
//	{
//		if (rec_num > 0)
//		{
//			int attr_len = 0;
//
//			for (int j = 0; j < attr_num; j++)
//			{
//				attr_len += attrs[j].col_width;
//			}
//
//			char id[60], cb_id[500], so_id[50];
//			int izone;
//
//			for (int i = 0; i < rec_num; i++)
//			{
//
//				list<string> lst_cb;
//
//				bzero(id, sizeof(id));
//				bzero(cb_id, sizeof(cb_id));
//				bzero(so_id, sizeof(so_id));
//
//				strncpy(id, buf + i * attr_len, attrs[0].col_width);
//				strncpy(cb_id, buf + i * attr_len + attrs[0].col_width, attrs[1].col_width);
//				strncpy(so_id, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width, attrs[2].col_width);
//
//				strcat(cb_id, ";");
//
//				splitstring(cb_id, ';', &lst_cb);
//
////				cout << cb_id << endl;
//
//				izone = g_fdir_r->GetZoneByCB(lst_cb);
//
//				if (izone >= 0)
//				{
//
//					list<FAULT_ITEM> devs;
//
//					g_fdir_r->get_zone_devs(izone, devs);
//
//					for (list<FAULT_ITEM>::iterator it = devs.begin(); it != devs.end(); it++)
//					{
//						printf("Insert into FHZG_ZONE(ID, IZONE, SO_ID, DEV_ID, DEV_TABLE, EDGE) Values(\'%s\', %d, \'%s\', \'%s\', \'%s\', 0);\r\n", id, izone, so_id, it->id.c_str(),
//								it->table.c_str());
//					}
//				}
//			}
//		}
//
//		g_oci->Readdata_Free();
//	}
//
//	pthread_mutex_lock(&oci_mutex);
//	ret = g_oci->ReadWithBind("select id,cb_id,so_id from fhzg_so_zone", &buf, &rec_num, &attr_num, &attrs);
//	pthread_mutex_unlock(&oci_mutex);
//
//	if (ret != OCI_ERROR)
//	{
//		if (rec_num > 0)
//		{
//			int attr_len = 0;
//
//			for (int j = 0; j < attr_num; j++)
//			{
//				attr_len += attrs[j].col_width;
//			}
//
//			char id[60], cb_id[500], so_id[50];
//			int izone;
//
//			for (int i = 0; i < rec_num; i++)
//			{
//
//				list<string> lst_cb;
//
//				bzero(id, sizeof(id));
//				bzero(cb_id, sizeof(cb_id));
//				bzero(so_id, sizeof(so_id));
//
//				strncpy(id, buf + i * attr_len, attrs[0].col_width);
//				strncpy(cb_id, buf + i * attr_len + attrs[0].col_width, attrs[1].col_width);
//				strncpy(so_id, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width, attrs[2].col_width);
//
//				if(strcmp(cb_id, "02200013860020;LSSCBL1441;LSSCBL1454;LSSCBL1491;LYLCBLL141") == 0)
//				{
//					sleep(0);
//				}
//				strcat(cb_id, ";");
//
//				splitstring(cb_id, ';', &lst_cb);
//
////					cout << cb_id << endl;
//
//				izone = g_fdir_r->GetZoneByCB(lst_cb);
//
//				if (izone >= 0)
//				{
//
//					list<FAULT_ITEM> devs;
//
//					g_fdir_r->get_zone_devs(izone, devs);
//
//					for (list<FAULT_ITEM>::iterator it = devs.begin(); it != devs.end(); it++)
//					{
//						printf("Insert into fhzg_so_zone(ID, IZONE, SO_ID, DEV_ID, DEV_TABLE, EDGE) Values(\'%s\', %d, \'%s\', \'%s\', \'%s\', 0);\r\n", id, izone, so_id, it->id.c_str(),
//								it->table.c_str());
//					}
//				}
//			}
//		}
//
//		g_oci->Readdata_Free();
//	}
//
////		pthread_mutex_lock(&oci_mutex);
////				ret = g_oci->ReadWithBind("select id,cb_id,so_id from fhzg_so_zone", &buf, &rec_num, &attr_num, &attrs);
////				pthread_mutex_unlock(&oci_mutex);
////
////				if (ret != OCI_ERROR)
////				{
////					if (rec_num > 0)
////					{
////						int attr_len = 0;
////
////						for (int j = 0; j < attr_num; j++)
////						{
////							attr_len += attrs[j].col_width;
////						}
////
////						char id[60], cb_id[500], so_id[50];
////						int izone;
////
////						for (int i = 0; i < rec_num; i++)
////						{
////
////							list<string> lst_cb;
////
////							bzero(id, sizeof(id));
////							bzero(cb_id, sizeof(cb_id));
////							bzero(so_id, sizeof(so_id));
////
////							strncpy(id, buf + i * attr_len, attrs[0].col_width);
////							strncpy(cb_id, buf + i * attr_len + attrs[0].col_width, attrs[1].col_width);
////							strncpy(so_id, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width, attrs[2].col_width);
////
////							strcat(cb_id, ";");
////
////							splitstring(cb_id, ';', &lst_cb);
////
////							cout << cb_id << endl;
////
////							izone = g_fdir_r->GetZoneByCB(lst_cb);
////
////							if (izone >= 0)
////							{
////
////								list<FAULT_ITEM> devs;
////
////								g_fdir_r->get_zone_devs(izone, devs);
////
////								for (list<FAULT_ITEM>::iterator it = devs.begin(); it != devs.end(); it++)
////								{
////									printf("Insert into fhzg_so_zone(ID, IZONE, SO_ID, DEV_ID, DEV_TABLE, EDGE) Values(\'%s\', %d, \'%s\', \'%s\', \'%s\', 0);\r\n", id, izone, so_id, it->id.c_str(),
////											it->table.c_str());
////								}
////							}
////						}
////					}
////
////					g_oci->Readdata_Free();
////				}
//
////	pthread_mutex_lock(&oci_mutex);
////		ret = g_oci->ReadWithBind("select id from FDIR_FDIRECT group by id", &buf, &rec_num, &attr_num, &attrs);
////		pthread_mutex_unlock(&oci_mutex);
////
////		if (ret != OCI_ERROR)
////		{
////			if (rec_num > 0)
////			{
////				int attr_len = 0;
////
////				for (int j = 0; j < attr_num; j++)
////				{
////					attr_len += attrs[j].col_width;
////				}
////
////				char id[60], izone[10], so_id[50];
////
////				for (int i = 0; i < rec_num; i++)
////				{
////					bzero(id, sizeof(id));
////
////					strncpy(id, buf + i * attr_len, attrs[0].col_width);
////
////					printf("update FDIR_FAULT_DEV set edge = 1 where id = \'%s\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FDIR_FAULT_DEV where id = \'%s\' and dev_table='CB' group by dev_id) where num =1);\r\n", id, id);
////				}
////			}
////
////			g_oci->Readdata_Free();
////		}
//
//	pthread_mutex_lock(&oci_mutex);
//	ret = g_oci->ReadWithBind("select id,so_id from fhzg_zone group by id,so_id", &buf, &rec_num, &attr_num, &attrs);
//	pthread_mutex_unlock(&oci_mutex);
//
//	if (ret != OCI_ERROR)
//	{
//		if (rec_num > 0)
//		{
//			int attr_len = 0;
//
//			for (int j = 0; j < attr_num; j++)
//			{
//				attr_len += attrs[j].col_width;
//			}
//
//			char id[60], izone[10], so_id[50];
//
//			for (int i = 0; i < rec_num; i++)
//			{
//				bzero(id, sizeof(id));
//				bzero(so_id, sizeof(so_id));
//
//				strncpy(id, buf + i * attr_len, attrs[0].col_width);
//				strncpy(so_id, buf + i * attr_len + attrs[0].col_width, attrs[1].col_width);
//
//				printf(
//						"update fhzg_zone set edge = 1 where id = \'%s\' and so_id = \'%s\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FHZG_ZONE where id = \'%s\' and so_id = \'%s\' and dev_table='CB' group by dev_id) where num =1);\r\n",
//						id, so_id, id, so_id);
//			}
//		}
//
//		g_oci->Readdata_Free();
//	}
//
//	pthread_mutex_lock(&oci_mutex);
//	ret = g_oci->ReadWithBind("select id,so_id from fhzg_so_zone group by id,so_id", &buf, &rec_num, &attr_num, &attrs);
//	pthread_mutex_unlock(&oci_mutex);
//
//	if (ret != OCI_ERROR)
//	{
//		if (rec_num > 0)
//		{
//			int attr_len = 0;
//
//			for (int j = 0; j < attr_num; j++)
//			{
//				attr_len += attrs[j].col_width;
//			}
//
//			char id[60], izone[10], so_id[50];
//
//			for (int i = 0; i < rec_num; i++)
//			{
//				bzero(id, sizeof(id));
//				bzero(so_id, sizeof(so_id));
//
//				strncpy(id, buf + i * attr_len, attrs[0].col_width);
//				strncpy(so_id, buf + i * attr_len + attrs[0].col_width, attrs[1].col_width);
//
//				printf(
//						"update fhzg_so_zone set edge = 1 where id = \'%s\' and so_id = \'%s\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  fhzg_so_zone where id = \'%s\' and so_id = \'%s\' and dev_table='CB' group by dev_id) where num =1);\r\n",
//						id, so_id, id, so_id);
//			}
//		}
//
//		g_oci->Readdata_Free();
//	}
//
	return true;
}

//从关系库读取dv的damode配置信息
int GetDvDamode(long dv_id)
{
	char *buf = NULL;
	int rec_num, attr_num, fdir_mode;
	struct ORA_ATTR *attrs = NULL;

	sprintf(sql,"SELECT DAMODE FROM ems.subcontrolarea WHERE ID=\'%ld\'", dv_id);
	TRACE("sql is %s\r\n",sql);
	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		fdir_mode = -1;
		if (rec_num > 0)
		{
			char tmp[100];
			char *pbuf = buf;

			memcpy(tmp, pbuf, attrs[0].col_width);
			fdir_mode = *(int*)(tmp);
			TRACE("the dv is %ld damode is %d\n", dv_id, fdir_mode);

			g_oci->Readdata_Free();
		}
		return fdir_mode;
	}
	else
	{
		return -1;
	}
}

/*
 * 从关系库获取故障隔离恢复的配置信息
 */
bool GetFdirConfig(FDIR_CONFIG &config)
{
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind("SELECT FDIR_MODE, USER2AUTO, TIMEOUT_CONTROL, TIMEOUT_FAULT, TIMEOUT_PROTPNT, TIMEOUT_AUTO, SIMULATE, TIMEOUT_JD_BUS, OP_USER, OP_NODE FROM FDIR_CONFIG", &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		if (rec_num > 0)
		{
			char tmp[100];
			char *pbuf = buf;

			bzero(tmp, sizeof(tmp));
			bzero(&config, sizeof(config));

			memcpy(tmp, pbuf, attrs[0].col_width);
			config.FDIR_MODE = *(int*)tmp;

			memcpy(tmp, pbuf += attrs[0].col_width, attrs[1].col_width);
			config.USER2AUTO = *(int*)tmp;

			memcpy(tmp, pbuf += attrs[1].col_width, attrs[2].col_width);
			config.TIMEOUT_CONTROL = *(int*)tmp;

			memcpy(tmp, pbuf += attrs[2].col_width, attrs[3].col_width);
			config.TIMEOUT_FAULT = *(int*)tmp;

			memcpy(tmp, pbuf += attrs[3].col_width, attrs[4].col_width);
			config.TIMEOUT_PROTPNT = *(int*)tmp;

			memcpy(tmp, pbuf += attrs[4].col_width, attrs[5].col_width);
			config.TIMEOUT_AUTO = *(int*)tmp;

			memcpy(tmp, pbuf += attrs[5].col_width, attrs[6].col_width);
			config.SIMULATE = *(int*)tmp;

			memcpy(tmp, pbuf += attrs[6].col_width, attrs[7].col_width);
			config.TIMEOUT_JD_FAULT = *(int*)tmp;

			memcpy(config.OP_USER, pbuf += attrs[7].col_width, attrs[8].col_width);
			memcpy(config.OP_NODE, pbuf += attrs[8].col_width, attrs[9].col_width);

//			printf("读取配置信息成功:%d, %d, %ds, %ds, %ds, %ds, %d, %d\r\n" ,config.FDIR_MODE, config.USER2AUTO
//					, config.TIMEOUT_CONTROL, config.TIMEOUT_FAULT, config.TIMEOUT_PROTPNT, config.TIMEOUT_AUTO, config.SIMULATE, config.TIMEOUT_JD_FAULT);
		}
		else
		{
			TRACE(">>>>>>>>>>>>>>>>>关系库里无配置信息\r\n");
			return false;
		}

		g_oci->Readdata_Free();
	}
	else
	{
		return false;
	}

	return true;
}

//判断一个开关是否在责任区内，在责任区内返回true,否则返回false
bool IsInRegion(MLang::Long cbid)
{
	PSBOB::ST psbob_st = { 0 };
	PSBOB::CB psbob_cb = { 0 };

	if (oodbread_rk(&psbob_cb, &cbid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) == OO_FAIL)
	{
		TRACE("读取id为%ld的开关记录失败\r\n", cbid);
		return false;
	}

	if (oodbread_rk(&psbob_st, &psbob_cb.st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(PSBOB::ST)) == OO_FAIL)
	{
		TRACE("读取id为%ld的厂站记录失败\r\n", psbob_cb.st_id);
		return false;
	}

	int ret = g_Priv.HasGivenRegion(g_FdirConfig.OP_USER, "", g_FdirConfig.OP_NODE, psbob_st.name, "st", 0);

	return ret == P_NORMAL;
}

//判断主备机，主机返回true，备机返回false
bool IsMaster()
{
	if(version == VERSION_STUDY)
	{
		return true;
	}

	//D5000判断主备机
	CServicesManage sermanager;
	COdbSystem odbsystem;
#ifdef _SCADA
	return sermanager.IsOnDuty(odbsystem.GetAppNoByName("scada_dms")) != 0;
#else
	return sermanager.IsOnDuty(odbsystem.GetAppNoByName("dscada_dms")) != 0;
#endif
}

void abrt(int param)
{
	cout << "程序即将退出…" << param << endl;
	cout << "进程ID：" << getpid() << endl;

	//g_bob2->WriteReDB(NULL, NULL, NULL);

	delete g_db_dms;
	delete g_db_fdir;
	delete g_db_psbob;

	rte_disconnect();

	delete bob_inv;
	delete proc_inv;

//	if (bob_inv != NULL)
//	{
////		bob_inv->bob_exit();
//
//		delete bob_inv;
//		bob_inv = NULL;
//	}

	exit(0);
}

//清除关系库指定故障的数据
void ClearFdirDB(const char *group_id)
{
	if (!IsMaster())
	{
		cout << "备机不写关系库！" << endl;
		return;
	}

	Notify(group_id, 0, 0, 0);

	assert(group_id != NULL);
	sprintf(sql,"PMS_PARAM.FIN_HIS(\'%s\')",group_id);
	ExecSQL(sql);
}

bool FormFdirStep(const char *group_id)
{
	assert(group_id != NULL);

	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	int pnt_column = GetPntIDColumn();

	int istep = 1, i;
	list<string> lst_fault_id, lst_manual_restore;
	char cb_id[MAX_CB_ID_LEN + 10];
	list<string>::iterator it;

	char machine[100];

	gethostname(machine, sizeof(machine));

	sprintf(sql, "delete from fdir_step where id=\'%s\' and status=%d", group_id, STATUS_FDIR_STEP_WAIT);
	ExecSQL(sql);

	sprintf(sql, "select step from fdir_step where id=\'%s\'order by step desc", group_id);

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	cout << sql << endl;

	if (ret != OCI_ERROR)
	{
		if(rec_num > 0)
		{
			char step[50];

			bzero(step, sizeof(step));
			memcpy(step, buf, attrs[0].col_width);

			istep = *(int*)step + 1;
		}
		else
		{
			istep = 1;
		}

		g_oci->Readdata_Free();
	}
	else
	{
		istep = 1;
	}

	sprintf(sql, "update fdir_fdirect set userid=\'%s\',machine=\'%s\' where groupid=\'%s\'", userid, machine, group_id);
	ExecSQL(sql);

	sprintf(sql, "select id,manual_restore from fdir_fdirect where groupid=\'%s\'", group_id);

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	cout << sql << endl;

	if (ret != OCI_ERROR)
	{
		for (i = 0; i < rec_num; i++)
		{
			char fault_id[100], manual_restore[1000];

			bzero(fault_id, sizeof(fault_id));
			bzero(manual_restore, sizeof(manual_restore));

			memcpy(fault_id, buf + i * (attrs[0].col_width + attrs[1].col_width), attrs[0].col_width);
			memcpy(manual_restore, buf + i * (attrs[0].col_width + attrs[1].col_width) + attrs[0].col_width, attrs[1].col_width);

			lst_fault_id.push_back(fault_id);

			if (manual_restore[0] != 0)
			{
				char *p1, *p2;

				p1 = p2 = manual_restore;

				while ((p2 = strchr(p1, ';')) != NULL)
				{
					bzero(cb_id, sizeof(cb_id));

					memcpy(cb_id, p1, p2 - p1);

					if((p1 = strstr(cb_id, "[0]")) != NULL)
					{
						*p1 = 0;

						lst_manual_restore.push_back(cb_id);
					}

					p1 = p2 + 1;
				}
			}

		}

		g_oci->Readdata_Free();
	}

	//插入隔离步骤到方案表
	char Temp[1024] = { 0 };

	for (it = lst_fault_id.begin(); it != lst_fault_id.end(); it++)
	{
		if (it == lst_fault_id.begin())
		{
			strcat(Temp, "id=\'");
		}
		else
		{
			strcat(Temp, "or id=\'");
		}

		strcat(Temp, it->c_str());
		strcat(Temp, "\' ");
	}

	sprintf(sql, "update fdir_isolate set userid=\'%s\',machine=\'%s\' where (%s) and ctrl_type=0", userid, machine, Temp);
	ExecSQL(sql);

	sprintf(sql, "select cb from fdir_isolate where (%s) and ctrl_type=0 order by id,step asc", Temp);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
//	cout << sql << endl;

	if (ret != OCI_ERROR)
	{
		for (i = 0; i < rec_num; i++)
		{
			bzero(cb_id, sizeof(cb_id));

			memcpy(cb_id, buf + i * attrs[0].col_width, attrs[0].col_width);

			long id = *(long*) cb_id;

			KEY_STRU ks = { 0 };

			CCommon::long_to_keyid(id, &ks);

			ks.field_id = pnt_column;

			unsigned long pt_id = 0;
			CCommon::keyid_to_long(&ks, &pt_id);

			sprintf(sql, "Insert into FDIR_STEP (ID, CB_ID, OPERATION, STEP, STATUS, CONTROL, STAGE) Values(\'%s\', \'%ld\', %d, %d, 0, %d, %d)", group_id, pt_id, 2, istep, 1, 1);
			ExecSQL(sql);

			istep++;
		}

		g_oci->Readdata_Free();
	}

	//插入转供步骤
	sprintf(sql, "select cb_id, operation, so_id, control from FHZG_STEP where id=\'%s_0\' order by step asc", group_id);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
//	cout << sql << endl;

	if (ret != OCI_ERROR)
	{
		for (i = 0; i < rec_num; i++)
		{
			char /*cb_id[100], */operation[100], so_id[100], control[100];

			bzero(cb_id, sizeof(cb_id));
			bzero(operation, sizeof(cb_id));
			bzero(so_id, sizeof(so_id));
			bzero(control, sizeof(control));

			memcpy(cb_id, buf + i * (attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width), attrs[0].col_width);
			memcpy(operation, buf + i * (attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width) + attrs[0].col_width, attrs[1].col_width);
			memcpy(so_id, buf + i * (attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width) + attrs[0].col_width + attrs[1].col_width, attrs[2].col_width);
			memcpy(control, buf + i * (attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width) + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width,
				   attrs[3].col_width);

			long id = *(long*) cb_id;

			KEY_STRU ks = { 0 };

			CCommon::long_to_keyid(id, &ks);

			ks.field_id = pnt_column;

			unsigned long pt_id = 0;
			CCommon::keyid_to_long(&ks, &pt_id);

			sprintf(sql, "Insert into FDIR_STEP (ID, CB_ID, OPERATION, STEP, STATUS, CONTROL, STAGE) Values(\'%s\', \'%ld\', %d, %d, 0, %d, %d)", group_id, pt_id, *(int*)operation == 1 ? 2 : 1, istep,
					*(int*)control, so_id[0] == 0 ? 3 : 4);
			ExecSQL(sql);
			istep++;
		}

		g_oci->Readdata_Free();
	}

	//插入恢复步骤
	for (it = lst_manual_restore.begin(); it != lst_manual_restore.end(); it++)
	{
		long id = atol(it->c_str());

		KEY_STRU ks = { 0 };

		CCommon::long_to_keyid(id, &ks);

		ks.field_id = pnt_column;

		unsigned long pt_id = 0;
		CCommon::keyid_to_long(&ks, &pt_id);

		sprintf(sql, "Insert into FDIR_STEP (ID, CB_ID, OPERATION, STEP, STATUS, CONTROL, STAGE) Values(\'%s\', \'%ld\', %d, %d, 0, %d, %d)", group_id, pt_id, 1, istep, 1, 5);
		ExecSQL(sql);
		istep++;
	}

	//更新fhzg_so中的select_status为1
	sprintf(sql, "update fhzg_so set select_status=1 where id=\'%s_0\'", group_id);
	ExecSQL(sql);

	UpdateExtStatus(group_id);

	return true;
}

void UpdateExtStatus(const char *group_id)
{
	assert(group_id != NULL);

	int cnt_total, cnt_wait, cnt_exec, cnt_succeed, cnt_failed;
	char status[100];
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	list<string> lst_point; //该组故障中跳闸的开关

	//更新隔离状态
	sprintf(sql, "SELECT STATUS FROM FDIR_STEP WHERE ID=\'%s\' AND (STAGE=%d OR STAGE=%d) AND CONTROL=1", group_id, STAGE_FDIR_STEP_ISOLATE, STAGE_FDIR_STEP_RE_ISOLATE);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		cnt_total = rec_num;
		cnt_wait = cnt_exec = cnt_succeed = cnt_failed = 0;

		for (int i = 0; i < rec_num; i++)
		{
			bzero(status, sizeof(status));

			memcpy(status, buf + i * attrs[0].col_width, attrs[0].col_width);

			switch (*(int*)status)
			{
				case STATUS_FDIR_STEP_WAIT:
					cnt_wait++;
					break;

				case STATUS_FDIR_STEP_EXEC:
					cnt_exec++;
					break;

				case STATUS_FDIR_STEP_FAIL:
					cnt_failed++;
					break;

				case STATUS_FDIR_STEP_PASS:
				case STATUS_FDIR_STEP_OK:
					cnt_succeed++;
					break;

				default:
					break;
			}
		}

		g_oci->Readdata_Free();

		printf("cnt_total:%d, cnt_wait:%d, cnt_exec:%d, cnt_succeed:%d, cnt_failed:%d\r\n", cnt_total, cnt_wait, cnt_exec, cnt_succeed, cnt_failed);

		if (cnt_total <= 0)
		{
			sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_ISOLATE=%d WHERE GROUPID=\'%s\'", -1, group_id);
		}
		else
		{
			if (cnt_wait > 0 || cnt_exec > 0)
			{
				sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_ISOLATE=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_WAIT, group_id);
			}
			else
			{
				if (cnt_failed > 0)
				{
					sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_ISOLATE=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_RECALC, group_id);
				}
				else
				{
					sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_ISOLATE=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_SUCCEED, group_id);
				}
			}
		}

		ExecSQL(sql);
	}
	else
	{
	}

	//更新转供状态
	sprintf(sql, "SELECT STATUS FROM FDIR_STEP WHERE ID=\'%s\' AND (STAGE=%d OR STAGE=%d) AND CONTROL=1", group_id, STAGE_FDIR_STEP_CUT_LD, STAGE_FDIR_STEP_TRANSFER);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		cnt_total = rec_num;
		cnt_wait = cnt_exec = cnt_succeed = cnt_failed = 0;

		for (int i = 0; i < rec_num; i++)
		{
			bzero(status, sizeof(status));

			memcpy(status, buf + i * attrs[0].col_width, attrs[0].col_width);

			switch (*(int*)status)
			{
				case STATUS_FDIR_STEP_WAIT:
					cnt_wait++;
					break;

				case STATUS_FDIR_STEP_EXEC:
					cnt_exec++;
					break;

				case STATUS_FDIR_STEP_FAIL:
					cnt_failed++;
					break;

				case STATUS_FDIR_STEP_PASS:
				case STATUS_FDIR_STEP_OK:
					cnt_succeed++;
					break;

				default:
					break;
			}
		}

		g_oci->Readdata_Free();

		printf("cnt_total:%d, cnt_wait:%d, cnt_exec:%d, cnt_succeed:%d, cnt_failed:%d\r\n", cnt_total, cnt_wait, cnt_exec, cnt_succeed, cnt_failed);

		if (cnt_total <= 0)
		{
			sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_FHZG=%d WHERE GROUPID=\'%s\'", -1, group_id);
		}
		else
		{
			if (cnt_wait > 0 || cnt_exec > 0)
			{
				sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_FHZG=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_WAIT, group_id);
			}
			else
			{
				if (cnt_failed > 0)
				{
					sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_FHZG=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_RECALC, group_id);
				}
				else
				{
					sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_FHZG=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_SUCCEED, group_id);
				}
			}
		}

		ExecSQL(sql);
	}
	else
	{
	}

	//更新恢复上游开关状态
	sprintf(sql, "SELECT STATUS FROM FDIR_STEP WHERE ID=\'%s\' AND STAGE=%d AND CONTROL=1", group_id, STAGE_FDIR_STEP_RESTORE);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		cnt_total = rec_num;
		cnt_wait = cnt_exec = cnt_succeed = cnt_failed = 0;

		for (int i = 0; i < rec_num; i++)
		{
			bzero(status, sizeof(status));

			memcpy(status, buf + i * attrs[0].col_width, attrs[0].col_width);

			switch (*(int*)status)
			{
				case STATUS_FDIR_STEP_WAIT:
					cnt_wait++;
					break;

				case STATUS_FDIR_STEP_EXEC:
					cnt_exec++;
					break;

				case STATUS_FDIR_STEP_FAIL:
					cnt_failed++;
					break;

				case STATUS_FDIR_STEP_PASS:
				case STATUS_FDIR_STEP_OK:
					cnt_succeed++;
					break;

				default:
					break;
			}
		}

		g_oci->Readdata_Free();

		printf("cnt_total:%d, cnt_wait:%d, cnt_exec:%d, cnt_succeed:%d, cnt_failed:%d\r\n", cnt_total, cnt_wait, cnt_exec, cnt_succeed, cnt_failed);

		if (cnt_total <= 0)
		{
			sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_RESTORE=%d WHERE GROUPID=\'%s\'", -1, group_id);
		}
		else
		{
			if (cnt_wait > 0 || cnt_exec > 0)
			{
				sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_RESTORE=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_WAIT, group_id);
			}
			else
			{
				if (cnt_failed > 0)
				{
					sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_RESTORE=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_RECALC, group_id);
				}
				else
				{
					sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS_RESTORE=%d WHERE GROUPID=\'%s\'", EXT_STATUS_FDIRECT_SUCCEED, group_id);
				}
			}
		}

		g_oci->Readdata_Free();

		ExecSQL(sql);
	}
	else
	{
	}
}

//遥控成功
bool ControlSucceed(const char *group_id, const long point_name, const int stage, bool jxcb)
{
	assert(group_id != NULL);

	TRACE("故障组%s遥控%ld成功！\r\n", group_id, point_name);

	KEY_STRU ks = {0};

	CCommon::long_to_keyid(point_name, &ks);
	ks.field_id = (ks.field_id == 0 ? 40 : 0);
	MLang::ULong uid = 0;
	CCommon::keyid_to_long(&ks, &uid);

	//写成功状态到库
	switch (stage)
	{
		case STAGE_FDIR_STEP_ISOLATE:
		{
			MLang::ULong dev_id;

			ks.field_id = 0;
			CCommon::keyid_to_long(&ks, &dev_id);

			if (AddToken(group_id, dev_id, 0) < 0)
			{
				TRACE("为故障%s的隔离开关%ld挂牌失败！\r\n", group_id, point_name);
			}

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and (cb=\'%ld\' or cb=\'%ld\') and ctrl_type=0", STATUS_ISOLATE_OK, group_id, point_name, uid);
			ExecSQL(sql);

			if (!jxcb)
			{
				//更新开关状态，校验时需要
				sprintf(sql, "update FDIR_CB_STATUS set status=%d where id=\'%s\' and (pt_id=\'%ld\' or pt_id=\'%ld\')", 0, group_id, point_name, uid);
				ExecSQL(sql);
			}

			break;
		}

		case STAGE_FDIR_STEP_RE_ISOLATE:
		{
			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and (cb=\'%ld\' or cb=\'%ld\') and ctrl_type=0", STATUS_ISOLATE_OK, group_id, point_name, uid);
			ExecSQL(sql);

			if (!jxcb)
			{
				//更新开关状态，校验时需要
				sprintf(sql, "update FDIR_CB_STATUS set status=%d where id=\'%s\' and (pt_id=\'%ld\' or pt_id=\'%ld\')", 0, group_id, point_name, uid);
				ExecSQL(sql);
			}

			break;
		}

		case STAGE_FDIR_STEP_CUT_LD:
		{
			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and (cb_id=\'%ld\' or cb_id=\'%ld\')", STATUS_FDIR_STEP_OK, group_id, point_name, uid);
			ExecSQL(sql);

			if (!jxcb)
			{
				//更新开关状态，校验时需要
				sprintf(sql, "update FDIR_CB_STATUS set status=%d where id=\'%s\' and (pt_id=\'%ld\' or pt_id=\'%ld\')", 0, group_id, point_name, uid);
				ExecSQL(sql);
			}

			break;
		}

		case STAGE_FDIR_STEP_TRANSFER:
		{
			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and (cb_id=\'%ld\' or cb_id=\'%ld\')", STATUS_FDIR_STEP_OK, group_id, point_name, uid);
			ExecSQL(sql);

			if (!jxcb)
			{
				//更新开关状态，校验时需要
				sprintf(sql, "update FDIR_CB_STATUS set status=%d where id=\'%s\' and (pt_id=\'%ld\' or pt_id=\'%ld\')", 1, group_id, point_name, uid);
				ExecSQL(sql);
			}

			break;
		}

		case STAGE_FDIR_STEP_RESTORE:
		{
			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and (cb_id=\'%ld\' or cb_id=\'%ld\')", STATUS_FDIR_STEP_OK, group_id, point_name, uid);
			ExecSQL(sql);

			if (!jxcb)
			{
				//更新开关状态，校验时需要
				sprintf(sql, "update FDIR_CB_STATUS set status=%d where id=\'%s\' and (pt_id=\'%ld\' or pt_id=\'%ld\')", 1, group_id, point_name, uid);
				ExecSQL(sql);
			}
			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[0]\',\'%ld[3]\') where groupid =\'%s\' and (manual_restore like \'%%%ld[_]%%\' or manual_restore like \'%%%ld[_]%%\')", uid, uid, group_id,
					point_name, uid);
			ExecSQL(sql);
			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[1]\',\'%ld[3]\') where groupid =\'%s\' and (manual_restore like \'%%%ld[_]%%\' or manual_restore like \'%%%ld[_]%%\')", uid, uid, group_id,
					point_name, uid);
			ExecSQL(sql);

			break;
		}

		default:
			break;
	}

	sprintf(sql, "update FDIR_STEP set status=%d,endtime=sysdate where id=\'%s\' and (cb_id=\'%ld\' or cb_id=\'%ld\') and stage=%d and status=%d", STATUS_FDIR_STEP_OK, group_id, point_name, uid, stage, STATUS_FDIR_STEP_EXEC);
	ExecSQL(sql);

	UpdateExtStatus(group_id);

	return true;
}

extern stack<NODE> get_virtual_path(long nd);
//遥控失败
bool ControlFailed(const char *group_id, const long point_name/*D5000后面的%s要改为%ld*/, const int stage)
{
	cout << __FUNCTION__ << endl;
	assert(group_id != NULL);

	KEY_STRU ks = { 0 };

	CCommon::long_to_keyid(point_name, &ks);
	ks.field_id = (ks.field_id == 0 ? 40 : 0);
	MLang::ULong uid = 0;
	CCommon::keyid_to_long(&ks, &uid);

	//合上游跳闸失败不用重新计算方案
	if (stage < STAGE_FDIR_STEP_RESTORE)
	{
		int i;

		//查询出该组故障的故障跳闸开关，获取开关的最新状态，并将跳闸开关设置为合闸
		char *buf = NULL;
		int rec_num, attr_num;
		struct ORA_ATTR *attrs = NULL;
		list<long> lst_point; //该组故障中跳闸的开关
		list<long> lst_up_point;//故障上游的主干开关
		list<FAULT_ITEM> lst_cb_item;

		sprintf(sql, "SELECT RECV_POINT FROM FDIR_FDIRECT WHERE GROUPID=\'%s\'", group_id);

		pthread_mutex_lock(&oci_mutex);
		int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
		pthread_mutex_unlock(&oci_mutex);
		cout << sql << endl;

		if (ret != OCI_ERROR)
		{
			if (rec_num > 0)
			{
				for (i = 0; i < rec_num; i++)
				{
					char recv_point[1000];

					bzero(recv_point, sizeof(recv_point));
					memcpy(recv_point, buf + i * attrs[0].col_width, attrs[0].col_width);

					cout << recv_point << endl;

					char *pbuf1, *pbuf2; // = recv_point;

					pbuf1 = pbuf2 = recv_point;

					while ((pbuf2 = strchr(pbuf1, ';')) != NULL)
					{
						char cb[MAX_CB_ID_LEN + 5] = { 0 };

						bzero(cb, sizeof(cb));

						memcpy(cb, pbuf1, pbuf2 - pbuf1);

						char *pTemp = strstr(cb, "[CB]");

						if (pTemp != NULL)
						{
							*pTemp = 0;

							lst_point.push_back(atol(cb));

							cout << "读取到跳闸开关：" << cb << endl;
						}

						pbuf1 = pbuf2 + 1;
					}
				}
			}
			g_oci->Readdata_Free();
		}
		else
		{
			return false;
		}

		//如果故障所在馈线的上游主干开关变化，合主干变化的开关
		sprintf(sql, "SELECT PT_ID FROM FDIR_CB_STATUS WHERE ID=\'%s\' AND ISFAULTUP=1 AND STATUS=0", group_id);

		pthread_mutex_lock(&oci_mutex);
		ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
		pthread_mutex_unlock(&oci_mutex);
		cout << sql << endl;

		if (ret != OCI_ERROR)
		{
			if (rec_num > 0)
			{
				for (i = 0; i < rec_num; i++)
				{
					long cb;

					cb = *(long*)(buf + i * attrs[0].col_width);
//					memcpy(recv_point, buf + i * attrs[0].col_width, attrs[0].col_width);
//					strncpy(cb, buf + i * attrs[0].col_width, attrs[0].col_width);
					cout << cb << endl;
					lst_up_point.push_back(cb);
				}
			}
			g_oci->Readdata_Free();
		}
		else
		{
			return false;
		}

		int lv_cb = g_bob->cb_tb->GetCount();
		CLS_CB *cb = g_bob->cb_tb->GetTable();

		for (i = 0; i < lv_cb; i++)
		{
			if (cb->q & CB_VIRTUAL)
			{
				if (get_virtual_path(cb->nd).size() > 0)
				{
					cb->q &= ~CB_OPEN;
				}
				else
				{
					cb->q |= CB_OPEN;
				}
			}
			else
			{
				PSBOB::CB psbob_cb = { 0 };

				//读取开关对应的遥信信息  oodbread_ap为逻辑读属性 idigital为开关测点号
				if (oodbread_rk(&psbob_cb, &(cb + i)->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
				{
					TRACE("读取开关%ld记录失败\r\n", (cb + i)->pid);

					return false;
				}

				(cb + i)->q &= ~CB_OPEN;

				if (psbob_cb.point == 0)
					(cb + i)->q |= CB_OPEN;
			}

			for (list<long>::iterator it = lst_point.begin(); it != lst_point.end(); it++)
			{
				if (g_bob->cb_tb->Data[i].pid == *it)
				{
					if ((cb + i)->q & CB_OPEN)
					{
						printf("合跳闸开关，cb:%ld, pointid:%s, point_name:%ld\r\n", (cb + i)->id, (cb + i)->descr, *it);
						(cb + i)->q &= ~CB_OPEN;
						lst_cb_item.push_back(FAULT_ITEM(*it, g_bob->cb_tb->Data[i].pid, "CB", g_bob->cb_tb->Data[i].descr));
						break;
					}
				}
			}

			for (list<long>::iterator it_point = lst_up_point.begin(); it_point != lst_up_point.end(); it_point++)
			{
				if (g_bob->cb_tb->Data[i].pid == *it_point)
				{
					printf("合故障上游主干线开关，cb:%ld, pointid:%s, point_name:%ld\r\n", (cb + i)->id, (cb + i)->descr, *it_point);

					(cb + i)->q &= ~CB_OPEN;
					break;
				}
			}
		}

		//把计算失败的故障树重新建立
		RecoTaskItem(group_id);

		//设置失败的故障为待重新计算方案状态
		for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
		{
			if (it->rq_type == TASK_TYPE_FAULT)
			{
				if (strcmp(it->task_data.fault.head.group_id, group_id) == 0)
				{
					it->task_data.fault.fdir_r->lst_point = lst_cb_item;

					it->task_data.fault.next_step = TASK_FAULT_STATUS_RE_CALC;

					it->task_data.fault.fail_point = point_name;

					//更新FDIR_R对象fdir_cb里开关的状态
					CLS_FDIR_CB *fdir_cb = it->task_data.fault.fdir_r->fdir_cb_tb->GetTable();

					for (i = 0; i < lv_cb; i++)
					{
						if ((cb + i)->q & CB_OPEN)
						{
							(fdir_cb + i)->topflags |= FDIR_CB_OPEN0;
							(fdir_cb + i)->topflags |= FDIR_CB_OPEN;
						}
						else
						{
							(fdir_cb + i)->topflags &= ~FDIR_CB_OPEN0;
							(fdir_cb + i)->topflags &= ~FDIR_CB_OPEN;
						}
					}

//				it->task_data.fault.fdir_r->lst_failed.push_back(point_name);
				}
			}
		}
	}
	else
	{
		for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
		{
			if (it->rq_type == TASK_TYPE_FAULT)
			{
				if (strcmp(it->task_data.fault.head.group_id, group_id) == 0)
				{
					//删除上游需要恢复的区域
					sprintf(sql, "DELETE FROM FDIR_UP_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
					ExecSQL(sql);
					break;
				}
			}
		}
	}
	//写失败状态到库
	switch (stage)
	{
		case STAGE_FDIR_STEP_ISOLATE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d,status_isolate=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_ISO, EXT_STATUS_FDIRECT_FAILED, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and (cb=\'%ld\' or cb=\'%ld\') and ctrl_type=0", STATUS_ISOLATE_FAIL, group_id, point_name, uid);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_RE_ISOLATE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d,status_isolate=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_ISO, EXT_STATUS_FDIRECT_FAILED, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and (cb=\'%ld\' or cb=\'%ld\') and ctrl_type=0", STATUS_ISOLATE_FAIL, group_id, point_name, uid);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_CUT_LD:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d,status_fhzg=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_TRANSFER, EXT_STATUS_FDIRECT_FAILED, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and (cb_id=\'%ld\' or cb_id=\'%ld\')", STATUS_FDIR_STEP_FAIL, group_id, point_name, uid);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_TRANSFER:
		{
			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and (cb_id=\'%ld\' or cb_id=\'%ld\')", STATUS_FDIR_STEP_FAIL, group_id, point_name, uid);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_FDIRECT set status=%d,status_fhzg=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_TRANSFER, EXT_STATUS_FDIRECT_FAILED, group_id);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_RESTORE:
		{
			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and (cb_id=\'%ld\' or cb_id=\'%ld\')", STATUS_FDIR_STEP_FAIL, group_id, point_name, uid);
			ExecSQL(sql);

			sprintf(sql, "update fdir_fdirect set status_restore=%d where groupid =\'%s\'", EXT_STATUS_FDIRECT_FAILED, group_id);
			ExecSQL(sql);

			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[0]\',\'%ld[2]\') where groupid =\'%s\' and (manual_restore like \'%%%ld[_]%%\' or manual_restore like \'%%%ld[_]%%\')"
					, uid, uid, group_id, point_name, uid);
			ExecSQL(sql);
			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[1]\',\'%ld[2]\') where groupid =\'%s\' and (manual_restore like \'%%%ld[_]%%\' or manual_restore like \'%%%ld[_]%%\')"
					, uid, uid, group_id, point_name, uid);
			ExecSQL(sql);

			break;
		}

		default:
			break;
	}

	sprintf(sql, "update FDIR_STEP set status=%d,endtime=sysdate where id=\'%s\' and (cb_id=\'%ld\' or cb_id=\'%ld\') and stage=%d and status=%d", STATUS_FDIR_STEP_FAIL, group_id, point_name, uid, stage, STATUS_FDIR_STEP_EXEC);
	ExecSQL(sql);

	return true;
}

//往任务列表中添加遥控等待项
bool AddReturnControl(const char *group_id, const long point_name, const bool bOpen, const int stage)
{
	assert(group_id != NULL && point_name > 0);

	FDIR_TASK data_task;

	//bzero(&data_task, sizeof(data_task));

	data_task.rq_type = TASK_TYPE_CB_CTRL;
	gettimeofday(&data_task.tv_creat, NULL);

	data_task.task_data.control.head.type = 0;
	data_task.task_data.control.head.size = sizeof(FDIR_TASK_DATA_CONTROL);
	strcpy(data_task.task_data.control.head.group_id, group_id);

	data_task.task_data.control.status = TASK_CTL_STATUS_RUN;

	data_task.task_data.control.bLast = false;
	data_task.task_data.control.point_name = point_name;
	data_task.task_data.control.bOpen = bOpen;
	data_task.task_data.control.stage = stage;

	switch (stage)
	{
		case STAGE_FDIR_STEP_ISOLATE:
		{
			sprintf(sql, "update fdir_fdirect set status=(case status when %d then %d else status end) where groupid=\'%s\'", STATUS_FDIRECT_DIRECT_OK, STATUS_FDIRECT_ISO, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and cb=\'%ld\' and ctrl_type=0", STATUS_ISOLATE_EXEC, group_id, point_name);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_RE_ISOLATE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_ISO, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and cb=\'%ld\' and ctrl_type=0", STATUS_ISOLATE_EXEC, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_CUT_LD:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", STATUS_FDIR_STEP_EXEC, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_TRANSFER:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", STATUS_FDIR_STEP_EXEC, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_RESTORE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", STATUS_FDIR_STEP_EXEC, group_id, point_name);
			ExecSQL(sql);

			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[0]\',\'%ld[1]\') where groupid =\'%s\' and manual_restore like \'%%%ld[_]%%\'", point_name, point_name, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		default:
			break;
	}

	sprintf(sql, "update FDIR_STEP set status=%d,starttime=sysdate where id=\'%s\' and cb_id=\'%ld\' and stage=%d and status=%d", STATUS_FDIR_STEP_EXEC, group_id, point_name, stage, STATUS_FDIR_STEP_WAIT);
	ExecSQL(sql);

	printf("故障组：%s 开关:%ld 操作:%s\r\n", group_id, point_name, bOpen ? "分闸" : "合闸");
	PSBOB::CB cb = { 0 };
	if (oodbread_rk(&cb, &point_name, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) < 0)
	{
		TRACE("oodbread_rk(%s) failed\r\n", point_name);
		return false;
	}
	else
	{
		if (!IsInRegion(cb.id))
		{
			TRACE("开关%ld[%s]不在用户%s的责任区内！\r\n", cb.id, cb.name, g_FdirConfig.OP_USER);

			//把进线开关遥控生成文件
			form_jxcb_control_txt(group_id, point_name, bOpen);
			data_task.task_data.control.status = TASK_CTL_STATUS_RUN;
		}
		else
		{
		}
	}

	printf("添加新的遥控任务项\r\n");
	g_lstTask.push_front(data_task);
	printf("OK!\r\n");
	return true;
}

bool SetPassControl(const char *group_id, const long point_name, int op_type, const int stage)
{
	assert(group_id != NULL && point_name > 0);

	switch (stage)
	{
		case STAGE_FDIR_STEP_ISOLATE:
		{
			sprintf(sql, "update fdir_fdirect set status=(case status when %d then %d else status end) where groupid=\'%s\'", STATUS_FDIRECT_DIRECT_OK, STATUS_FDIRECT_ISO, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and cb=\'%ld\' and ctrl_type=0", op_type, group_id, point_name);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_RE_ISOLATE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_ISO, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and cb=\'%ld\' and ctrl_type=0", op_type, group_id, point_name);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_CUT_LD:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", op_type, group_id, point_name);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_TRANSFER:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", op_type, group_id, point_name);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_RESTORE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", op_type, group_id, point_name);
			ExecSQL(sql);

			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[0]\',\'%ld[4]\') where groupid =\'%s\' and manual_restore like \'%%%ld[_]%%\'", point_name, point_name, group_id, point_name);
			ExecSQL(sql);
			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[1]\',\'%ld[4]\') where groupid =\'%s\' and manual_restore like \'%%%ld[_]%%\'", point_name, point_name, group_id, point_name);
			ExecSQL(sql);

			break;
		}

		default:
			break;
	}

	sprintf(sql, "update FDIR_STEP set status=%d,starttime=sysdate,endtime=sysdate where id=\'%s\' and cb_id=\'%ld\' and stage=%d", op_type, group_id, point_name, stage);
	ExecSQL(sql);

	return true;
}

//往任务列表中添加新的遥控项
bool AddControlItem(const char *group_id, const long point_name, const bool bOpen, const int stage)
{
	assert(group_id != NULL);

	for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
	{
		if (it->rq_type == TASK_TYPE_FAULT)
		{
			if (strcmp(it->task_data.fault.head.group_id, group_id) == 0)
			{
				it->task_data.fault.next_step = TASK_FAULT_STATUS_PROCESS;
			}
		}
	}

	FDIR_TASK data_task;

//	bzero(&data_task, sizeof(data_task));

	data_task.rq_type = TASK_TYPE_CB_CTRL;
	gettimeofday(&data_task.tv_creat, NULL);

	data_task.task_data.control.head.type = 0;
	data_task.task_data.control.head.size = sizeof(FDIR_TASK_DATA_CONTROL);
	strcpy(data_task.task_data.control.head.group_id, group_id);

	data_task.task_data.control.status = TASK_CTL_STATUS_WAIT;
	data_task.task_data.control.bLast = false;
	data_task.task_data.control.point_name= point_name;
	data_task.task_data.control.bOpen = bOpen;
	data_task.task_data.control.stage = stage;

	switch (stage)
	{
		case STAGE_FDIR_STEP_ISOLATE:
		{
			sprintf(sql, "update fdir_fdirect set status=(case status when %d then %d else status end) where groupid=\'%s\'", STATUS_FDIRECT_DIRECT_OK, STATUS_FDIRECT_ISO, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and cb=\'%ld\' and ctrl_type=0", STATUS_ISOLATE_EXEC, group_id, point_name);
			ExecSQL(sql);

			break;
		}

		case STAGE_FDIR_STEP_RE_ISOLATE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_ISO, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FDIR_ISOLATE set status=%d where id=\'%s\' and cb=\'%ld\' and ctrl_type=0", STATUS_ISOLATE_EXEC, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_CUT_LD:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", STATUS_FDIR_STEP_EXEC, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_TRANSFER:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", STATUS_FDIR_STEP_EXEC, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		case STAGE_FDIR_STEP_RESTORE:
		{
			sprintf(sql, "update FDIR_FDIRECT set status=%d where groupid=\'%s\'", STATUS_FDIRECT_RES, group_id);
			ExecSQL(sql);

			sprintf(sql, "update FHZG_STEP set status=%d where id like \'%s%%\' and cb_id=\'%ld\'", STATUS_FDIR_STEP_EXEC, group_id, point_name);
			ExecSQL(sql);

			sprintf(sql, "update fdir_fdirect set manual_restore=replace(manual_restore,\'%ld[0]\',\'%ld[1]\') where groupid =\'%s\' and manual_restore like \'%ld%%[_]%%\'", point_name, point_name, group_id, point_name);
			ExecSQL(sql);
			break;
		}

		default:
			break;
	}

	sprintf(sql, "update FDIR_STEP set status=%d,starttime=sysdate where id=\'%s\' and cb_id=\'%ld\' and stage=%d and status=%d", STATUS_FDIR_STEP_EXEC, group_id, point_name, stage, STATUS_FDIR_STEP_WAIT);
	ExecSQL(sql);

	printf("故障组：%s 开关:%ld 操作:%s\r\n", group_id, point_name, bOpen ? "分闸" : "合闸");
	if (version == VERSION_REALTIME)
	{
		PSBOB::CB cb = { 0 };
		if (oodbread_rk(&cb, &point_name, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) < 0)
		{
			TRACE("oodbread_rk(%ld) failed\r\n", point_name);
			return false;
		}
		else
		{
			if (!IsInRegion(cb.id))
			{
				TRACE("开关%ld[%s]不在用户%s的责任区内！\r\n", cb.id, cb.name, g_FdirConfig.OP_USER);

				//把进线开关遥控生成文件
				form_jxcb_control_txt(group_id, point_name, bOpen);
				data_task.task_data.control.status = TASK_CTL_STATUS_RUN;
			}
			else
			{

			}
		}
	}
	cout << "添加新的遥控任务项";
	g_lstTask.push_front(data_task);
	cout << "OK!" << endl;
	return true;
}

//进线开关形成遥控文件传给EMS
void form_jxcb_control_txt(const char *group_id, const long point_name, const bool bOpen)
{
	char *filename = new char[1024];
	char *jxcb_msg = new char[1024*1024];
	char *str = new char[1024];
	char *cmd = new char[1024];

	sprintf(jxcb_msg, "<?xml version = \"1.0\" encoding = \"ISO-8859-1\" ?>\r\n<Message>\r\n");
	sprintf(str, "<fault id=\"%s\" jxcb=\"%ld\" type=\"%d\"/>\r\n", group_id, point_name, bOpen==true ? 0:1);
	strcat(jxcb_msg, str);
	strcat(jxcb_msg, "</Message>\r\n");
	timeval tm_now;
	gettimeofday(&tm_now, NULL);
	sprintf(filename, "%s/dms_txt/%ld_%ld.txt", getenv("HOME"), tm_now.tv_sec, point_name);

	FILE *fp = fopen(filename, "wb+");
	if (fp == NULL)
	{
		TRACE("写入遥控进线开关信息失败,file:%s\r\n", filename);
	}
	else
	{
		fwrite(jxcb_msg, 1, strlen(jxcb_msg), fp);
		fclose(fp);
	}

	KEY_STRU ks;
	unsigned long uid;
	CCommon::long_to_keyid(point_name, &ks);
	ks.field_id = 0;
	CCommon::keyid_to_long(&ks, &uid);

	PSBOB::CB psbob_cb = {0};

	if(oodbread_rk(&psbob_cb, &uid, const_cast<TB_DESCR*>(g_db_psbob->GetTB(ks.table_no)), sizeof(psbob_cb)) == OO_FAIL)
	{
		TRACE("读取关键字为%ld的开关记录失败！\r\n", uid);
	}

	sprintf(sql, "insert into pms_param.jxcb_control (pnt_id, opter, descr, st_id) values(%ld, %d, \'%s\', %ld)", point_name, !bOpen, psbob_cb.name, psbob_cb.st_id);
	ExecSQL(sql);

//	sprintf(cmd, "scp %s d5000@ems:/home/d5000", filename);
//	system(cmd);

	delete filename;
	delete jxcb_msg;
	delete str;
	delete cmd;

}

void SetFdirectStatus(const char *group_id)
{
	assert(group_id != NULL);

	char *buf = NULL;
	char *pbuf = NULL;
	char *id = new char[50];
	int status = 0;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	sprintf(sql, "SELECT ID,STATUS FROM FDIR_FDIRECT WHERE GROUPID=\'%s\' ", group_id);

	cout << sql << endl;
	//读取未执行的隔离步骤
	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		pbuf = buf;
		for (int i = 0; i < rec_num; i++)
		{
			memset(id, 0, sizeof(50));
			memcpy(id, pbuf, attrs[0].col_width);
			pbuf = pbuf + attrs[0].col_width;
			status = *(int*)pbuf;
			pbuf = pbuf + attrs[1].col_width;
			if ((status == STATUS_FDIRECT_POSSIBLE) || (status == STATUS_FDIRECT_NONE)|| (status == STATUS_FDIRECT_WINK))
			{
				sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE ID=\'%s\'", status+4, id);
				if (ExecSQL(sql) != OCI_ERROR)
				{
					sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE ID=\'%s\'", STATUS_FDIRECT_STOP, id);
					ExecSQL(sql);
				}
			}
		}
		g_oci->Readdata_Free();
//		if ((status == STATUS_FDIRECT_POSSIBLE) || (status == STATUS_FDIRECT_NONE) || (status == STATUS_FDIRECT_WINK))
//		{
//			sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE GROUPID=\'%s\'", status+4, group_id);
//			if (ExecSQL(sql) != OCI_ERROR)
//			{
//				sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_STOP, group_id);
//				ExecSQL(sql);
//			}
//		}
	}
	else
	{
		delete id;
		return ;
	}
	delete id;
}

//获取指定故障尚未执行的遥控
int GetLeftStep(const char *group_id, list<CB_CONTROL> &lstcb)
{
	assert(group_id != NULL);

	lstcb.clear();

	cout << "本次故障还未完成的操作步骤：" << endl;
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	CB_CONTROL control = { 0 };

	sprintf(sql, "SELECT CB_ID, OPERATION, STAGE FROM FDIR_STEP WHERE ID=\'%s\' AND STATUS=%d AND CONTROL=%d ORDER BY STEP ASC", group_id, STATUS_FDIR_STEP_WAIT, 1/*可遥控*/);

	cout << sql << endl;
	//读取未执行的隔离步骤
	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		for (int i = 0; i < rec_num; i++)
		{
			char bOpen[50], stage[50], point_id[50];

			bzero(&control, sizeof(control));
			bzero(bOpen, sizeof(bOpen));
			bzero(stage, sizeof(stage));
			bzero(point_id, sizeof(point_id));

			memcpy(point_id, buf + (i * (attrs[0].col_width + attrs[1].col_width + attrs[2].col_width)), attrs[0].col_width);
			control.point_id = *(long*)point_id;

			memcpy(bOpen, buf + (i * (attrs[0].col_width + attrs[1].col_width + attrs[2].col_width)) + attrs[0].col_width, attrs[1].col_width);
			control.bOpen = *(int*)bOpen - 1;

			memcpy(stage, buf + (i * (attrs[0].col_width + attrs[1].col_width + attrs[2].col_width)) + attrs[0].col_width + attrs[1].col_width, attrs[2].col_width);
			control.stage = *(int*)stage;

			lstcb.push_back(control);
			printf("point:%ld, operation:%s, stage:%d\r\n", control.point_id, control.bOpen ? "分" : "合", control.stage);
		}
		g_oci->Readdata_Free();
	}
	else
	{
		return -1;
	}

	return lstcb.size();
}

int Test_Oci()
{
	return 0;
//	OCI_TOOLS oci;
//	_list_bind *first_bind = NULL, *next_bind = NULL;
//
//	int ret = oci.Connect("PSBOB_REALTIME", "psbob", "scd1");
//
//	if (ret > 0)
//	{
//		ret = oci.ORA_BindWithReturn(&first_bind, "select * from cp");
//
//		if (ret > 0)
//		{
//			next_bind = first_bind;
//
//			for (int i = 0; i < ret; i++)
//			{
//				cout << next_bind->bindbuf[0] << endl;
//
//				next_bind = next_bind->next;
//			}
//
//			oci.Free_list_bind(first_bind);
//		}
//
//		ret = oci.Disconnect();
//	}
//
//	return 0;
}

long GetTokenID()
{
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	long token_id = 0;

	sprintf(sql, "SELECT max(token_id)+1 from ems.ems.token_info");

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		token_id = *(long*) buf;
	}

	g_oci->Readdata_Free();

	return token_id;
}

int AddToken(const char *fault_id, const long dev_id, int token_type)
{
	//研究态无法挂牌
	if (version == VERSION_STUDY)
	{
		return 0;
	}

	TRACE("%s…\r\n", __FUNCTION__);
#if 0
	long st_id;

	PSBOB::CB psbob_cb = { 0 };

	int ret = GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), (char*) &dev_id, sizeof(PSBOB::CB), (char*) &psbob_cb);

	if (ret < 0)
	{
		TRACE("获取ID为%ld的断路器信息失败！\r\n", dev_id);
		return ret;
	}

	token_info_dscada token = { 0 };

	char *buf = NULL;

	ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_psbob->GetTB("token_define")), 1, &buf);

	if (ret < 0)
	{
		TRACE("读取标志牌定义表失败！\r\n");
		return ret;
	}
	token_define_dscada *p = (token_define_dscada *) buf;

	for (int i = 0; i < ret / sizeof(token_define_dscada); i++)
	{
		if (strcmp(p->name, "故障隔离") == 0)
		{
			token.token_no = p->token_no;
			break;
		}

		p++;
	}

	BufFree(buf);

	if (token.token_no == 0)
	{
		TRACE("故障隔离牌未定义，请在标志牌定义表里添加\r\n");
		return -1;
	}

	token.token_id = GetTokenID();
	token.device_id = dev_id;
	token.record_app = AP_DSCADA;
	token.st_id = psbob_cb.st_id;
	token.if_display = 1;
	token.father_token_id = -1;

	ret = InsertRec(const_cast<TB_DESCR*>(g_db_psbob->GetTB("token_info")), 1, sizeof(token_info_dscada), (char*) &token, 1);
	if (ret < 0)
	{
		TRACE("插入挂牌信息失败\r\n");
		return ret;
	}

	sprintf(sql, "insert into ems.ems.token_info (token_id, device_id, token_no, record_app,st_id) values (%ld,%ld,%ld,%d,%ld)", token.token_id, token.device_id, token.token_no, token.record_app,
		psbob_cb.st_id);
	ExecSQL(sql);

	sprintf(sql, "insert into pms_param.fdir_token (id, dev_id, st_id,token_type,add_flag,del_flag,token_id) values (\'%s\',%ld,%ld,%d,0,0,%ld)", fault_id, dev_id, psbob_cb.st_id, token_type,
		token.token_id);
	ExecSQL(sql);
#else
	TScaTokenOp token_op = { 0 };

	bzero(&token_op, sizeof(TScaTokenOp));

	token_op.dev_id = dev_id;
	token_op.father_token_id = -1;
	token_op.op_time = time(NULL);

	token_info_dscada token = { 0 };

	char *buf = NULL;

	int ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_psbob->GetTB("token_define")), 1, &buf);

	if (ret < 0)
	{
		TRACE("读取标志牌定义表失败！\r\n");
		return ret;
	}
	token_define_dscada *p = (token_define_dscada *) buf;

	for (int i = 0; i < ret / sizeof(token_define_dscada); i++)
	{
		if (strcmp(p->name, "故障隔离") == 0)
		{
			token_op.token_type_id = p->token_no;
			break;
		}

		p++;
	}

	BufFree(buf);

	if (token_op.token_type_id == 0)
	{
		TRACE("故障隔离牌未定义，请在标志牌定义表里添加\r\n");
		return -1;
	}

	Message msg = { 0 };

	msg.header.event = SCADA_OPT_TAG_SET;
	msg.header.serv = D_CH_SCADA_OP_CHANNEL;

	msg.header.ctxt = 1;
	memcpy(msg.Msg_buf, &token_op, sizeof(token_op));

	ret = bob_inv->messageSend(&msg, sizeof(token_op), NULL);

	TRACE("send_event %d ,fault_id:%s, dev_id:%ld, return:%d\r\n", msg.header.event, fault_id, dev_id, ret);
#endif
	return 0;
}

//通知人机推故障
int Notify(const char *grp_id, const long dv_id, const long st_id, char op)
{
	if(version==VERSION_STUDY)
	{
		return 0;
	}

	assert(grp_id != NULL);

	if(grp_id == NULL) return -1;

	FDIR_MSG_ISO_REC data = { 0 };
	data.operate = op;

	strcpy(data.fault_id, grp_id);
	data.dv_id = dv_id;
	data.st_id = st_id;

	Message msg = { 0 };

	msg.header.event = EVPS_FDIR_MSG_ISO_REC;
	msg.header.serv = WARN_INFORM_CHANNEL;

	msg.header.ctxt = 1;
	memcpy(msg.Msg_buf, &data, sizeof(data));
	int ret = bob_inv->messageSend(&msg, sizeof(data), NULL);
	TRACE("send_event %d ,group_id:%s, dv_id:%ld, st_id:%ld, return:%d\r\n", msg.header.event, grp_id, dv_id, st_id, ret);

	return ret;
}

//发送遥控命令
int DoControl(const long cb_id, bool bPre, bool bOpen)
{
	if (!g_FdirConfig.SIMULATE)
	{
		TScaBaseOp	op = {0};
		op.op_time = time(NULL);
		strcpy(op.op_user_name, g_FdirConfig.OP_USER);
		strcpy(op.op_node_name, g_FdirConfig.OP_NODE);
		strcpy(op.gua_node_name, g_FdirConfig.OP_NODE);
		strcpy(op.gua_user_name, g_FdirConfig.OP_USER);

		KEY_STRU ks = {0};
		op.id = cb_id;
		id_to_pid(op.id);
		CCommon::long_to_keyid(op.id, &ks);
		ks.field_id = 1;

		//获取遥信值所在域号
		FIELD_PARA_STRU *pfps = NULL;
		int ret = GetFieldsPara(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), &pfps);
		if (ret >= 0)
		{
			int cnt = ret / sizeof(FIELD_PARA_STRU);

			for (int i = 0; i < cnt; i++)
			{
				if (strcmp(pfps[i].field_name_eng, "point") == 0)
				{
					ks.field_id = pfps[i].column_id;
					break;
				}
			}

			BufFree(pfps);
		}

		unsigned long uid = op.id;
		CCommon::keyid_to_long(&ks, &uid);

		op.data_type = 4;
		op.int_value = bOpen ? 0 : 1;
		op.id = uid;

		Message msg = { 0 };
		msg.header.event = bPre ? SCADA_OPT_CTRL_PREV : SCADA_OPT_CTRL_EXEC;
#ifdef _SCADA
		msg.header.serv = CH_SCADA_OP_CHANNEL;
#else
		msg.header.serv = D_CH_SCADA_OP_CHANNEL;
#endif
		msg.header.ctxt = 1;
		memcpy(msg.Msg_buf, &op, sizeof(op));

		ret = bob_inv->messageSend(&msg, sizeof(op), NULL);
		TRACE("send_event %d ,key_id:%ld, return:%d\r\n", msg.header.event, uid, ret);
	}
	else
	{
		Message message = { 0 };
		ChangeYx yx = { cb_id, bOpen, bOpen, 0, 0 };

//		M_CODE(yx, message.Msg_buf, sizeof(yx));

		message.header.event = MT_YX_CHANGE;
		message.header.serv = CH_UP_REAL_DATA;
		memcpy(message.Msg_buf, &yx, sizeof(yx));

		cout << "bob_inv->messageSend,return:" << bob_inv->messageSend(&message, sizeof(message.header) + sizeof(yx), NULL) << endl;

		PSBOB::CB psbob_cb;
		psbob_cb.id = cb_id;
		psbob_cb.point = !bOpen;
		oodbupdate_ak(&psbob_cb.point, &psbob_cb.id, "point", const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb.id));
	}
	//D5000
	return 0;
}


//故障模拟程序
//校验开关信息
bool Check_Simu_Data(CHECK_LIST& lst_data)
{
	if (lst_data.size() > 0)
	{
		for (std::list<CHECK_DATA>::iterator it_chk = lst_data.begin(); it_chk != lst_data.end(); )
		{
			long cb_id = it_chk->cb_id;
			PSBOB::CB chk_cb;
			if (oodbread_rk(&chk_cb, &cb_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) > 0)
			{
				//还未添加挂牌信息的校验，待人机开始开发后商议
				if (chk_cb.point == it_chk->status && fabs(chk_cb.i_a_value - it_chk->Ia) < 0.01 && fabs(chk_cb.i_b_value - it_chk->Ib) < 0.01
					&& fabs(chk_cb.i_c_value - it_chk->Ic) < 0.01 && fabs(chk_cb.p - it_chk->p) < 0.01 && fabs(chk_cb.q - it_chk->q) < 0.01)
				{
					std::list<CHECK_DATA>::iterator it_chk_tmp = it_chk;
					it_chk++;
					lst_data.erase(it_chk_tmp);
					continue;
				}
				else
				{
					it_chk->status = chk_cb.point;
					it_chk->Ia = chk_cb.i_a_value;
					it_chk->Ib = chk_cb.i_b_value;
					it_chk->Ic = chk_cb.i_c_value;
					it_chk->p = chk_cb.p;
					it_chk->q = chk_cb.q;
					it_chk++;
				}
			}
			else
			{
				TRACE("在断路器表中查找关键字为%ld的记录失败！\r\n", cb_id);
				continue;
			}
		}

		return true;
	}
	else
	{
		TRACE("模拟信号对应的开关列表为空！校验失败！\r\n");
		return false;
	}
}


bool get_cb_info(CB_INFO &cb)
{
	PSBOB::CB tmp_cb_info;
	bzero(&tmp_cb_info, sizeof(PSBOB::CB));

	DSBOB *dsbob = g_fdir_r->get_dsbob();
	//CLS_CB *tempcb = dsbob->cb_tb->Find(CLS_CB(cb.id));

	long pid = cb.id;

	id_to_pid(pid);

	if (oodbread_rk(&tmp_cb_info, &pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) <= 0)
	{
		TRACE("%s(%d):读取开关%ld记录信息失败，错误号:%d\r\n", __FUNCTION__, __LINE__, pid, _oodb_errno);
		return false;
	}

	if (tmp_cb_info.point > 0)
	{
		cb.status = TYPICAL_OPEN;
	}
	else
	{
		cb.status = TYPICAL_CLOSE;
	}

	//获取父节点的电流值等信息
	if (tmp_cb_info.brk_type == MENU_BRK_TYPE_JX)
	{
		cb.type = JXCB;
	}
	else if (tmp_cb_info.brk_type == MENU_BRK_TYPE_LW)
	{
		cb.type = LWCB;
	}
	else
	{
		cb.type = NORMAL_CB;
	}

	cb.Ia = tmp_cb_info.i_a_value;
	cb.Ib = tmp_cb_info.i_b_value;
	cb.Ic = tmp_cb_info.i_c_value;

	return true;
}



int get_dv_cb(int izone, const long fathercb, TreeNode<CB_INFO> *father, Tree<CB_INFO> &tree_cb, map<long, Tree<CB_INFO> >& m_map_dv_cb)
{

	assert(izone != -1);
	int i, nextzone, ret, icb;
	CB_INFO cbnode;
	TreeNode<CB_INFO> *tmp_father;
	CLS_CB *cb;
	DSBOB *dsbob = g_fdir_r->get_dsbob();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = g_fdir_r->fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = g_fdir_r->fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = g_fdir_r->fdir_cb_tb->GetTable();
	//int lv_fdir_cb = g_fdir_r->fdir_cb_tb->GetCount();
	//int lv_fdir_zone = g_fdir_r->fdir_zone_tb->GetCount();
	//PSBOB::CB psbob_cb = {0};
	ret = -1;
	icb = -1;

	TRACE("the izone is %d\n",izone);

	//获取父节点的电流值等信息
	cbnode.id = fathercb;
	get_cb_info(cbnode);

	tmp_father = tree_cb.TreeAddLeaf(father, cbnode);

	for (i = 0; i < (fdir_zone + izone)->cbnum; i++)
	{
		icb = (fdir_zone_cb + (fdir_zone + izone)->cbptr + i)->icb; //此处的icb为开关的物理号，在FDIR_ZONE_CB表中
		cb = dsbob->cb_tb->Find(CLS_CB((fdir_cb + icb)->cb_id));

		TRACE("发现开关：%s, ID:%ld\r\n", cb->descr, cb->pid);
		//判断开关有没有被处理过
		if((fdir_cb + icb)->q & FDIR_CB_UNPROC)
		{
			//如果是进线开关停止搜索树,并且把进线开关作为这棵树的节点
			if(cb->q & CB_FDHCB)
			{
				//TreeNode<CB_INFO> *newroot = tmp_father;
				memset(&cbnode, 0, sizeof(cbnode));
				cbnode.id = cb->pid;

				get_cb_info(cbnode);
				tree_cb.TreeAddLeaf(tmp_father, cbnode);
				(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
			}
			else
			{
				//如果是联络开关添加联络开关到树，并且以联络开关开始建立一个新的树
				if(cb->q & CB_LWCB)
				{
					memset(&cbnode, 0, sizeof(cbnode));
					cbnode.id = cb->pid;
					get_cb_info(cbnode);

					tree_cb.TreeAddLeaf(tmp_father, cbnode);
					//联络开关对应另外一端首先判断是否被处理过，如果没被处理过获取dv信息建立新树
					if(izone == (fdir_cb + icb)->ind)
					{
						nextzone = (fdir_cb + icb)->iznd;
					}
					else
					{
						nextzone = (fdir_cb + icb)->ind;
					}

					if(nextzone == -1)
					{
						(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
						continue;
					}
					PSBOB::DV dv = { 0 };
					g_fdir_r->GetDVByZone(nextzone, dv);

					if(dv.id <= 0)
					{
						(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
						TRACE("\033[31m********************LWCB id is %ld getdv failed**************\033[0m\n",cb->id);
						continue;
					}

					if((fdir_zone + nextzone)->q & FDIR_ZONE_UNPROC)
					{
						if(m_map_dv_cb.find(dv.id) != m_map_dv_cb.end())
						{
							continue;
						}
						(fdir_zone + nextzone)->q &= ~FDIR_ZONE_UNPROC;
						(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
						ret = get_dv_cb(nextzone, cb->id, m_map_dv_cb[dv.id].root, m_map_dv_cb[dv.id], m_map_dv_cb);
						if(ret == -1)
						{
							return -1;
						}
					}

				}
					//普通开关递归调用遍历所有相连的开关
				else
				{

					(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
					if(izone == (fdir_cb + icb)->ind)
					{
						nextzone = (fdir_cb + icb)->iznd;
					}
					else
					{
						nextzone = (fdir_cb + icb)->ind;
					}

					if((fdir_zone + nextzone)->q & FDIR_ZONE_UNPROC)
					{
						(fdir_zone + nextzone)->q &= ~FDIR_ZONE_UNPROC;
						ret = get_dv_cb(nextzone, cb->id, tmp_father, tree_cb, m_map_dv_cb);
						if(ret == -1)
						{
							return -1;
						}
					}
				}
			}
		}
	}

	return 0;

}


bool save_simu_cb_info(long sig_id, char* plan_id)
{
	PSBOB::PROTECT protect = {0};
	PSBOB::CB sig_cb = {0};

	if (oodbread_rk(&protect, &sig_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("relaysig")), sizeof(PSBOB::PROTECT)) > 0 ||
		oodbread_rk(&sig_cb, &sig_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) > 0)
	{
		long cb_id = 0;
		if (protect.id > 0)
		{
			cb_id = protect.oo_dev;
		}
		else
		{
			cb_id = sig_cb.id;
		}

		map<long, Tree<CB_INFO> >	m_map_dv_cb;
		int i, icb = -1, izone;
		map<string, Tree<CB_INFO> > temp_map;
		PSBOB::DV dv = { 0 };
		DSBOB *dsbob = g_fdir_r->get_dsbob();
		CLS_FDIR_ZONE* fdir_zone = g_fdir_r->fdir_zone_tb->GetTable();
		CLS_FDIR_CB* fdir_cb = g_fdir_r->fdir_cb_tb->GetTable();
		int lv_fdir_cb = g_fdir_r->fdir_cb_tb->GetCount();
		int lv_fdir_zone = g_fdir_r->fdir_zone_tb->GetCount();

		//将所有的区段设置为未处理状态
		for (i = 0; i < lv_fdir_cb; i++)
		{
			(fdir_cb + i)->q |= FDIR_CB_UNPROC;
		}
		for (i = 0; i < lv_fdir_zone; i++)
		{
			(fdir_zone + i)->q |= FDIR_ZONE_UNPROC;
		}

		long id = cb_id;

		pid_to_id(id);

		icb = dsbob->cb_tb->FindPos(CLS_CB(id));
		TRACE("获取到开关:%ld的记录 获取到的icb: %d %d \n", id,icb,(fdir_zone + (fdir_cb + icb)->ind)->type);
		if (icb == -1)
		{
			TRACE("没有获取到开关:%ld的记录\n", id);
			return false;
		}
		//cout<<"======"<<(fdir_cb + icb)->id<<(fdir_cb + icb)->descr <<"--"<<(fdir_zone + (fdir_cb + icb)->ind)->type <<endl;
		//获取开关的下游区段
		if ((fdir_zone + (fdir_cb + icb)->ind)->type == _FDIR_ZN)
		{
			izone = (fdir_cb + icb)->ind;
			(fdir_zone + (fdir_cb + icb)->iznd)->q &= ~FDIR_ZONE_UNPROC;
		}
		else
		{
			izone = (fdir_cb + icb)->iznd;
			(fdir_zone + (fdir_cb + icb)->ind)->q &= ~FDIR_ZONE_UNPROC;
		}

		(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
		if (izone != -1)
		{
			//设置此区段izone为已经处理过的，并调用处理函数
			(fdir_zone + izone)->q &= ~FDIR_ZONE_UNPROC;
			//获取馈线ID
			g_fdir_r->GetDVByZone(izone, dv);
			if (dv.id <= 0)
			{
				TRACE("\033[31m********************LWCB id is %s getdv failed**************\033[0m\r\n",cb_id);
				return false;
			}

			int ret = get_dv_cb(izone, id, m_map_dv_cb[dv.id].root, m_map_dv_cb[dv.id], m_map_dv_cb);

			if (ret >= 0)
			{
				//把馈线和开关信息写入关系库
				for (std::map<long, Tree<CB_INFO> >::iterator it_dv = m_map_dv_cb.begin(); it_dv != m_map_dv_cb.end(); it_dv++)
				{
					PSBOB::DV simu_dv = {0};
					long dv_id = it_dv->first;
					if (oodbread_rk(&simu_dv, &dv_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(PSBOB::DV)) > 0)
					{
						sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_DV (ID, DV_ID, DV_NAME, LMTNORM) VALUES (\'%s\', %ld, \'%s\', %f)",
								plan_id, dv_id, simu_dv.name, simu_dv.lmtnorm);
						ExecSQL(sql);
					}

					list<TreeNode<CB_INFO>*> so_tree;
					list<TreeNode<CB_INFO>*>::iterator it_tree;

					it_dv->second.DepthFirstErgodic(it_dv->second.root, so_tree);
					for (it_tree = so_tree.begin(); it_tree != so_tree.end(); it_tree++)
					{
						long brk_id = (*it_tree)->data.id;
						id_to_pid(brk_id);
						PSBOB::CB tree_cb = {0};
						if (oodbread_rk(&tree_cb, &brk_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) > 0)
						{
							sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_BREAKER(ID, CB_ID, CB_NAME, STATUS, IA, IB, IC, P, Q) VALUES (\'%s\', %ld, \'%s\', %d, %f, %f, %f, %f, %f)",
									plan_id, brk_id, tree_cb.name, tree_cb.point, tree_cb.i_a_value, tree_cb.i_b_value, tree_cb.i_c_value, tree_cb.p, tree_cb.q);
							ExecSQL(sql);

							//添加挂牌信息
							TOKEN_INFO_DSCADA token_info = {0};
							if (oodbread_ra(&token_info, "device_id", &brk_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("token_info")), sizeof(TOKEN_INFO_DSCADA)) > 0)
							{
								int token_type = 0;
								TOKEN_DEFINE_DSCADA token_def = {0};
								if (oodbread_rk(&token_def, &token_info.token_no, const_cast<TB_DESCR*>(g_db_psbob->GetTB("token_define")), sizeof(TOKEN_INFO_DSCADA)) > 0)
								{
									sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_TOKEN(ID, CB_ID, TOKEN_TYPE, TOKEN_NAME)VALUES(\'%s\', %ld, %d, \'%s\')", plan_id, brk_id, token_def.token_type, token_def.name);
									ExecSQL(sql);
								}
							}
						}
					}
				}
			}

		}
		else
		{
			TRACE("获取进线开关的下游区段失败\n");
			return false;
		}
	}
	else
	{
		TRACE("查询保护信号表和断路器表关键字为%ld的记录失败！\r\n", sig_id);
		return false;
	}

	return true;
}



void thread_alarm_process(void *param)
{
	bool bCanScan = true;		//判断正在定位的故障是否定位完成，是否可以开始新一轮的扫描
	while (true)
	{
		bCanScan = true;
		pthread_mutex_lock(&task_list_mutex);
		//TRACE("$$$$$$$$$NO deadlock!!!\r\n");
		for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
		{
			//存在未定位完成的故障
			if (it->rq_type == TASK_TYPE_FAULT)
			{
				if ((it->task_data.fault.next_step == TASK_FAULT_STATUS_CALC) || (it->task_data.fault.next_step == TASK_FAULT_STATUS_SLEEP))
				{
					bCanScan = false;
					break;
				}
			}
		}
		pthread_mutex_unlock(&task_list_mutex);

		if (bCanScan)
		{
			pthread_mutex_lock(&trig_list_mutex);
			for (FDIR_POINT_TRIG_LIST::iterator it = g_lstTrig.begin(); it != g_lstTrig.end(); it++)
			{
				//发现未被定位的故障信号(故障跳闸，动作,母线)
				if (it->lst_fault.size() == 0/* && (strcmp(it->alarm.fault_info, key[1]) == 0 || strcmp(it->alarm.fault_info, key[2]) == 0)*/)	//D5000暂时只收集事故跳闸
				{
					if(it->alarm.dev == ALARM_SGZ || it->alarm.dev == ALARM_FAULTINFO_JD || it->alarm.dev == ALARM_FAULTINFO_DL	|| it->alarm.dev == ALARM_BUS//母线故障、事故总动作、故障指示器动作
					   /* ||it->alarm.type == SG_WARN_TYPE*/ || it->alarm.type == D_SG_WARN_TYPE || strcmp(it->alarm.tabname,"bus") == 0)	//开关事故跳闸
					{
					}
					else	//不是故障启动信号
					{
						continue;
					}

					//线程上锁，不让后台返回模拟故障执行报文
					//pthread_mutex_lock(&simu_sync_mutex);

					GetFdirConfig(g_FdirConfig);
					//启动一个故障
					FDIR_TASK data_task;
					data_task.rq_type = TASK_TYPE_FAULT;
					memcpy(&data_task.tv_creat, &it->tm_add, sizeof(timeval));
					data_task.task_data.fault.next_step = TASK_FAULT_STATUS_SLEEP;
					if (g_FdirConfig.AUTO_DEAL_HIS)
					{
						data_task.task_data.fault.bAuto = 2;
					}
					else
					{
						data_task.task_data.fault.bAuto = it->damode == FDIR_MODE_AUTO ? 1 : 0;
					}

					//20150514 标记模拟出的故障信号
					if (it->trig_type == TRIG_TYPE_SIMU)
					{
						data_task.task_data.fault.simu_plan_id = it->simu_plan_id;
						data_task.task_data.fault.signal_type = TRIG_TYPE_SIMU;
					}

					//D5000暂时只收集事故跳闸
					//		if (it->alarm.datatype & (POINT_BPCB | POINT_GCB | POINT_BPDS | POINT_GDS))
					//		{
					//			data_task.task_data.fault.type = TASK_FAULT_LD;
					//		}

					if (strcmp(it->alarm.tabname, "bus") == 0)
					{
						cout << "发现故障母线" << it->alarm.key_id << endl;
						data_task.task_data.fault.type = TASK_FAULT_BUS;
					}
					else
						data_task.task_data.fault.type = TASK_FAULT_LD;

					data_task.task_data.fault.head.type = 0;
					data_task.task_data.fault.head.size = sizeof(FDIR_TASK_DATA_FAULT);
					data_task.task_data.fault.point_name = it->psbob_data.point.id;
					data_task.task_data.fault.dv = it->dv_id;
					data_task.task_data.fault.st = it->alarm.st_id;

					tm *ptm = localtime(&it->tm_add.tv_sec);
					sprintf(data_task.task_data.fault.head.fault_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
							it->tm_add.tv_usec);

					TRACE("报警ID:%ld,启动故障,故障ID:%s\r\n", it->alarm.key_id, data_task.task_data.fault.head.fault_id);

					AddTask(&data_task);

//					rte_disconnect();
//					pthread_mutex_unlock(&trig_list_mutex);
//					return;

					break;
				}
			}
			pthread_mutex_unlock(&trig_list_mutex);
			//pthread_mutex_unlock(&simu_sync_mutex);
		}
		else
		{
			cout << "有未定位完成的故障，不能启动新的故障定位" << endl;
		}

		sleep(1);
	}
}

//扫描母线及其进线开关，判断其是否为母线故障
void thread_scan_bus(void *param)
{
//	cout<<"开启扫描故障母线线程"<<endl;
	int i;
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	PSBOB::ST psbob_st = { 0 };
	PSBOB::DV psbob_dv = { 0 };

	while (true)
	{
		int lv_bus = g_bob->bus_tb->GetCount();
		CLS_BUS* bus = g_bob->bus_tb->GetTable();

		for (i = 0; i < lv_bus; i++)
		{
			int ind = g_bob->bus_tb->Data[i].ind;
			if (ind < 0)
				continue;

			PSBOB::BUS psbob_bus = {0};
			if (oodbread_rk(&psbob_bus, &g_bob->bus_tb->Data[i].pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(PSBOB::BUS)) <= 0)
			{
				cout << "读取母线" << g_bob->bus_tb->Data[i].id << "电压信息失败,errno:" << _oodb_errno << endl;
				continue;
			}
			if (psbob_bus.v < MIN_BUS_VALUE)
			{
				CLS_FDIR_ZONE *fdir_zone = g_fdir_r->fdir_zone_tb->GetTable();
				int lv_fdir_zone = g_fdir_r->fdir_zone_tb->GetCount();

				//如果fdir_fdirect表中已经存在此母线故障，并且此故障并未处理，则不再生成这条母线的故障信息，首先判断母线是否被定位过
				sprintf(sql, "SELECT ID FROM PMS_PARAM.FDIR_FDIRECT WHERE STUDY_EQ = \'%ld\' AND TYPE = 0 AND STATUS in (0,-1,1,2,3,6,7,9,10,11,12,13,14,15,17,18,19)", (bus + i)->pid);
				strcat(sql, "\0");
				pthread_mutex_lock(&oci_mutex);
				int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
				pthread_mutex_unlock(&oci_mutex);
				if (ret != OCI_ERROR)
				{
					if (rec_num > 0)
					{
						g_oci->Readdata_Free();
						//	cout<<"发现故障失压母线"<<(bus + i)->pid<<"已经被定位处理"<<endl;
						continue;
					}
					g_oci->Readdata_Free();
				}
				else
				{
					continue;
				}

				for (int j = 0; j < lv_fdir_zone; j++)
				{
					if ((fdir_zone + j)->zn_id == (bus + i)->id) // zn_id和 bus的id对应不上
					{
						int cbptr = (fdir_zone + j)->cbptr;
						int cbnum = (fdir_zone + j)->cbnum;
						int jxcbnum = 0;
						int num = 0;
						for (int k = 0; k < cbnum; k++)
						{
							int icb = g_fdir_r->fdir_zone_cb_tb->GetDataByPos(cbptr + k)->icb;
							CLS_CB *cb = g_bob->cb_tb->GetDataByPos(icb);
							if (cb->q & CB_FDHCB)
							{
								jxcbnum++;
								float max_value;
								PSBOB::CB psbob_cb = { 0 };
								long cb_id = cb->pid;

								if (oodbread_rk(&psbob_cb, &cb_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) <= 0)
								{
									printf("%s(%ld):读取开关%ld记录信息失败，错误号:%ld\r\n", __FUNCTION__, __LINE__, cb_id, _oodb_errno);
									jxcbnum--;
									continue;
								}
								max_value = max(max(psbob_cb.i_a_value, psbob_cb.i_b_value), psbob_cb.i_c_value);

								if (max_value <= 0)
								{
									num++;
								}
							}
						}
						if (num > jxcbnum / 2)
						{
							FDIR_POINT_TRIG data_trig;

							data_trig.alarm.st_id = psbob_bus.st_id;
							if (GetDv(data_trig.alarm.st_id, "st", psbob_dv) == OO_FAIL)
							{
								TRACE("获取厂站%ld的馈线信息失败！\r\n", data_trig.alarm.st_id);
								continue;
							}

							data_trig.dv_id = psbob_dv.id;
							data_trig.damode = GetDvDamode(psbob_dv.id);
							gettimeofday(&data_trig.tm_add, NULL);

							if (psbob_dv.damode == FDIR_MODE_FORBID)
							{
								TRACE("馈线%ld(%s)为禁止DA处理(%d)\r\n", psbob_dv.id, psbob_dv.name, psbob_dv.damode);
								continue;
							}

							// 填充一些字段表示母线故障信息
							data_trig.alarm.dev = ALARM_BUS;
							data_trig.alarm.key_id = (bus + i)->id;
							strcpy(data_trig.alarm.fault_info,"母线故障");
							data_trig.alarm.name = (bus + i)->descr;
							data_trig.psbob_data.point.id = (bus + i)->pid;
							strcpy(data_trig.alarm.tabname, "bus");
							strcpy(data_trig.alarm.fault_info, "母线失压");

							//将该母线加入过流开关列表
							pthread_mutex_lock(&trig_list_mutex);
							g_lstTrig.push_back(data_trig);
							pthread_mutex_unlock(&trig_list_mutex);
							cout << "g_lstTrig.size()=" << g_lstTrig.size() << endl;
							sleep(30);
						}
						break;
					}

				}
			}
		}

		sleep(60);

	}
}

//清除信号列表中过时的故障信号
//逻辑是：先在任务列表中找到一个发生时间最早的故障，然后在故障信号列表中
//找到该故障的启动信号，最后删除该启动信号前面30秒外的故障信号
void clear_timeout_trig()
{
	timeval tm_anchor = { -1 };
	FDIR_TASK_LIST::iterator it = g_lstTask.begin();

	for (; it != g_lstTask.end(); it++)
	{
		if (it->rq_type == TASK_TYPE_FAULT)
		{
			tm_anchor.tv_sec = it->tv_creat.tv_sec;
			tm_anchor.tv_usec = it->tv_creat.tv_usec;

			break;
		}
	}

	if (tm_anchor.tv_sec >= 0)
	{
		pthread_mutex_lock(&trig_list_mutex);
		FDIR_POINT_TRIG_LIST::iterator it_trig_list;

		for (it_trig_list = g_lstTrig.begin(); it_trig_list != g_lstTrig.end(); it_trig_list++)
		{
			//找到该故障的启动故障信号在故障信号列表中的位置
			if (it_trig_list->tm_add.tv_sec == it->tv_creat.tv_sec
				&& it_trig_list->tm_add.tv_usec == it->tv_creat.tv_usec)
			{
				break;
			}
		}

		if (it_trig_list != g_lstTrig.end())
		{
			//前30秒
			FDIR_POINT_TRIG_LIST::iterator itTemp = it_trig_list;

			while (itTemp != g_lstTrig.begin())
			{
				itTemp--;

				int span = it_trig_list->tm_add.tv_sec - itTemp->tm_add.tv_sec;

				if (span > g_FdirConfig.TIMEOUT_PROTPNT)
				{
					break;
				}
				else
				{
				}
			}

			//删除信号列表中往前30秒外的故障信号
			if (itTemp != g_lstTrig.begin())
			{
				g_lstTrig.erase(g_lstTrig.begin(), itTemp);
				TRACE("删除故障列表中最早发生的故障30s前的故障信号\r\n");
			}

		}
		else
		{
			//TRACE("未找到故障启动信号：%s\r\n", it->task_data.fault.head.fault_id);
		}

		pthread_mutex_unlock(&trig_list_mutex);
	}
	else
	{
//		TRACE("任务列表里无故障项！\r\n");
	}
}

//写遥控日志信息
int conlog(const char *pointid, bool bOpen, const char *faultid, const char *operstyle)
{
	//D5000遥控日志
	return 1;
//	assert(pointid != NULL);
//	assert(faultid != NULL);
//	assert(operstyle != NULL);
//
//	timeval tv = { 0 };
//	char groupid[100] = { 0 };
//	struct tm fault_tm = { 0 };
//	PSBOB::POINT point;
//
//	TRACE("%s faultid:%s\r\n", __FUNCTION__, faultid);
//
//	sscanf(faultid, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", &fault_tm.tm_year, &fault_tm.tm_mon, &fault_tm.tm_mday, &fault_tm.tm_hour, &fault_tm.tm_min, &fault_tm.tm_sec, &tv.tv_usec);
//	sprintf(groupid, "%04d%02d%02d%02d%02d%02d_%s", fault_tm.tm_year, fault_tm.tm_mon, fault_tm.tm_mday, fault_tm.tm_hour, fault_tm.tm_min, fault_tm.tm_sec, pointid);
//
//	if (oodbread_rk(&point, const_cast<char*>(pointid), const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), sizeof(point)) <= 0)
//	{
//		TRACE("读取PSBOB-POINT表关键字为%s的记录失败，错误号：%d\r\n", pointid, _oodb_errno);
//		return OO_FAIL;
//	}
//
//	gettimeofday(&tv, NULL);
//	tm *ptm = localtime(&tv.tv_sec);
//
//	sprintf(sql,
//			"Insert into hisdb.VIORECORD (OPERTIME, OPERUSER, OPERNODE, CONTENT, POINTID, ENVIRONMENT, OPERNAME, OPERSTYLE, OPERMODE, OPERTYPE, OPERGUARDIAN, GROUPID) Values ('%04d-%02d-%02d %02d:%02d:%02d', '%s', '%s', '%s%s%s', '%s', '%s', '%s', '%s', NULL, 0, NULL, '%s')",
//			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, userid, hostname, point.descr, operstyle, bOpen ? "由闭合--拉开" : "由拉开--闭合", pointid,
//			"realtime,scada,psbobpoint", "遥控", operstyle, groupid);
//
//	return ExecSQL(sql);
}

int conlog(const char *pointid, const char *content, const char *faultid, const char *operstyle)
{
	//D5000遥控日志
	return 1;
//	assert(pointid != NULL);
//	assert(faultid != NULL);
//	assert(operstyle != NULL);
//	assert(content != NULL);
//
//	timeval tv = { 0 };
//	char groupid[100] = { 0 };
//	struct tm fault_tm = { 0 };
//	PSBOB::POINT point;
//
//	TRACE("%s faultid:%s\r\n", __FUNCTION__, faultid);
//
//	sscanf(faultid, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", &fault_tm.tm_year, &fault_tm.tm_mon, &fault_tm.tm_mday, &fault_tm.tm_hour, &fault_tm.tm_min, &fault_tm.tm_sec, &tv.tv_usec);
//	sprintf(groupid, "%04d%02d%02d%02d%02d%02d_%s", fault_tm.tm_year, fault_tm.tm_mon, fault_tm.tm_mday, fault_tm.tm_hour, fault_tm.tm_min, fault_tm.tm_sec, pointid);
//
//	if (oodbread_rk(&point, const_cast<char*>(pointid), const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), sizeof(point)) <= 0)
//	{
//		TRACE("读取PSBOB-POINT表关键字为%s的记录失败，错误号：%d\r\n", pointid, _oodb_errno);
//		return OO_FAIL;
//	}
//
//	gettimeofday(&tv, NULL);
//	tm *ptm = localtime(&tv.tv_sec);
//
//	sprintf(sql,
//			"Insert into hisdb.VIORECORD (OPERTIME, OPERUSER, OPERNODE, CONTENT, POINTID, ENVIRONMENT, OPERNAME, OPERSTYLE, OPERMODE, OPERTYPE, OPERGUARDIAN, GROUPID) Values ('%04d-%02d-%02d %02d:%02d:%02d', '%s', '%s', '%s', '%s', '%s', '%s', '%s', NULL, 0, NULL, '%s')",
//			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, userid, hostname, content, pointid, "realtime,scada,psbobpoint", "遥控", operstyle, groupid);
//
//	return ExecSQL(sql);
}

void restart_timer(int sig)
{
	TRACE("进入重启定时器，程序重启！\r\n");

	exit(0);
}

bool SetRestartTimer(int nsec)
{
	struct itimerval value = { 0 }, ovalue = { 0 };

	//////////////////////////
	//取消已有定时器
	if (setitimer(ITIMER_REAL, &value, &ovalue) == -1)
	{
		signal(SIGALRM, SIG_DFL );
		return false;
	}

	if (signal(SIGALRM, SIG_DFL ) == SIG_ERR )
	{
		return false;
	}

	//////////////////////////////////
	//设置新的定时器
	if (signal(SIGALRM, restart_timer) == SIG_ERR )
	{
		return false;
	}

	//距离下次整点的秒数
	value.it_value.tv_sec = nsec;
	value.it_value.tv_usec = 0;

	//1小时
	value.it_interval.tv_sec = nsec;
	value.it_interval.tv_usec = 0;

	if (setitimer(ITIMER_REAL, &value, &ovalue) == -1)
	{
		signal(SIGALRM, SIG_DFL );
		return false;
	}

	TRACE("将重启定时器设置在%d秒后……\r\n", value.it_value.tv_sec);

	return true;
}

void thread_process(void *param)
{
	cout << "命令处理线程启动……" << endl;

	while (true)
	{
		//模型结构发生变化后，程序退出，然后等待守护进程去重新启动。
		if (*model_chg_ntf != last_model_chg_ntf && *model_chg_ntf != 0)
		{
			TRACE("\033[31m模型结构变化。%ld\033[0m\r\n", *model_chg_ntf);

			last_model_chg_ntf = *model_chg_ntf;

			//exit(0);
			SetRestartTimer(5 * 60);
		}

		pthread_mutex_lock(&task_list_mutex);
		pthread_mutex_lock(&simu_sync_mutex);

		if (g_lstTask.size() > 0)
		{
			if (version == VERSION_REALTIME)
			{
				clear_timeout_trig();
			}

			FDIR_TASK_LIST::iterator it = g_lstTask.begin();

			for (; it != g_lstTask.end(); it++)
			{
				timeval tm_now;
				gettimeofday(&tm_now, NULL);

				float cost = tm_now.tv_sec * 1000 * 1000 + tm_now.tv_usec - (it->tv_creat.tv_sec * 1000 * 1000 + it->tv_creat.tv_usec);

				cost = cost / 1000 / 1000;

				switch (it->rq_type)
				{
					//做遥控
					case TASK_TYPE_CB_CTRL:
					{
						int ph = -1;

						if (it->task_data.control.status == TASK_CTL_STATUS_WAIT)
						{
							if (version == VERSION_REALTIME)
							{
								DoControl(it->task_data.control.point_name, true, it->task_data.control.bOpen);
//							if ((ph = oodbget_ph(const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), it->task_data.control.point_name)) == OO_FAIL)
//							{
//								printf("%s 获取%s物理号失败，errno:%d, line:%d\r\n", __FUNCTION__, it->task_data.control.point_name, _oodb_errno, __LINE__);
//
//								pthread_mutex_unlock(&task_list_mutex);
//
//								continue;
//							}
//
//							if (it->task_data.control.bOpen) //故障隔离
//							{
//								//发真实遥控指令
//								if (!g_FdirConfig.SIMULATE)
//								{
//									PS_TEL_DIGCTRL sel_cb = { 0 };
//
//									strcpy(sel_cb.id, it->task_data.control.point_name);
//									sel_cb.op = 2;
//									sel_cb.op_type = 2;
//									sel_cb.src_flag = 255;
//
//									printf("send_event(%d) %s return %d\r\n", EVPS_TEL_DIGCTRL_SELECT, sel_cb.id, bob_inv->send_event(EVPS_TEL_DIGCTRL_SELECT, (char *) &sel_cb, sizeof(sel_cb)));
//								}
//								//发遥信变位事件,模拟遥控
//								else
//								{
//									COM_POINT_DATA data = { ph, 0, 1, 0 };
//
//									printf("send_event(%d) return %d\r\n", EVPS_CPNTCHG, bob_inv->send_event(EVPS_CPNTCHG, (char *) &data, sizeof(data)));
//								}
//							}
//							else
//							{
//								//发真实遥控指令
//								if (!g_FdirConfig.SIMULATE)
//								{
//									PS_TEL_DIGCTRL sel_cb = { 0 };
//
//									strcpy(sel_cb.id, it->task_data.control.point_name);
//									sel_cb.op = 1;
//									sel_cb.op_type = 2;
//									sel_cb.src_flag = 255;
//
//									printf("send_event(%d) %s return %d\r\n", EVPS_TEL_DIGCTRL_SELECT, sel_cb.id, bob_inv->send_event(EVPS_TEL_DIGCTRL_SELECT, (char *) &sel_cb, sizeof(sel_cb)));
//								}
//								//发遥信变位事件,模拟遥控
//								else
//								{
//									COM_POINT_DATA data = { ph, 1, 1, 0 };
//
//									printf("send_event(%d) return %d\r\n", EVPS_CPNTCHG, bob_inv->send_event(EVPS_CPNTCHG, (char *) &data, sizeof(data)));
//								}
//							}
//
								it->task_data.control.status = TASK_CTL_STATUS_RUN;
							}
							else
							{
								PSBOB::CB psbob_cb = { 0 };

								if (oodbread_rk(&psbob_cb, &it->task_data.control.point_name, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) <= 0)
								{
									it->task_data.control.status = TASK_CTL_STATUS_RUN;
									printf("%s(%d):读取PSBOB库POINT表关键字为%ld的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, it->task_data.control.point_name, _oodb_errno);
								}
								else
								{
									if (HasMask(g_db_psbob->GetTB("breaker"), "status", psbob_cb.status , MENU_STATE_DEV_MANUAL))
									{
										it->task_data.control.status = TASK_CTL_STATUS_RUN;
									}
									else
									{
										if (it->task_data.control.bOpen)
										{
											psbob_cb.point = 0;
										}
										else
										{
											psbob_cb.point = 1;
										}

										if (oodbupdate_rk(&psbob_cb, &it->task_data.control.point_name, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
										{
											it->task_data.control.status = TASK_CTL_STATUS_RUN;
											printf("%s(%d):更新PSBOB库POINT表关键字为%ld的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, it->task_data.control.point_name, _oodb_errno);
										}
										else
										{
											ControlSucceed(it->task_data.control.head.group_id, it->task_data.control.point_name, it->task_data.control.stage, false);

											list<CB_CONTROL> lstCB;
											FDIR_TASK_LIST::iterator it_fault;
											char fault_id[FAULT_ID_LENGTH] = { 0 };

											strcpy(fault_id, it->task_data.control.head.group_id);

											if (GetLeftStep(fault_id, lstCB) == 0)
											{
												ClearFdirDB(fault_id);

												sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, fault_id);
												if (ExecSQL(sql) != OCI_ERROR)
												{
													sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, fault_id);
													ExecSQL(sql);
												}

												RemoveTasks(fault_id);

												printf("故障组%s处理完成！！！！\r\n", fault_id);
											}
											else
											{
												for (it_fault = g_lstTask.begin(); it_fault != g_lstTask.end(); it_fault++)
												{
													if (it_fault->rq_type == TASK_TYPE_FAULT)
													{
														if (strcmp(it_fault->task_data.fault.head.group_id, it->task_data.control.head.group_id) == 0)
														{
															it_fault->status = 1;
															if (it_fault->task_data.fault.bAuto == 1)
																AddControlItem(fault_id, lstCB.begin()->point_id, lstCB.begin()->bOpen, lstCB.begin()->stage);
															else
															{
																printf("遥控%ld所在故障组%s非自动执行模式！！\r\n", it->task_data.control.point_name, it->task_data.control.head.group_id);
															}

															for (FDIR_TASK_LIST::iterator it_temp = it_fault; it_temp != g_lstTask.end(); it_temp++)
															{
																if ((it_temp->rq_type == TASK_TYPE_FAULT) && (strcmp(it_temp->task_data.fault.head.group_id, it->task_data.control.head.group_id) == 0))
																{
																	it_temp->status = 1;
																}
															}

															break;
														}
													}
												}

												if (it_fault == g_lstTask.end())
												{
													printf("未找到遥控%ld所在故障组%s对应的故障项，无法判断其是否为自动执行模式！！！\r\n", it->task_data.control.point_name, it->task_data.control.head.group_id);
												}
											}

											g_lstTask.erase(it);
											it = g_lstTask.begin();
										}
									}
								}
							}
						}
						else if (it->task_data.control.status == TASK_CTL_STATUS_RUN)
						{
							if (cost > g_FdirConfig.TIMEOUT_CONTROL)
							{
								printf("[%.2fs]故障%s遥控%ld超时，从任务列表删除\r\n", cost, it->task_data.control.head.fault_id, it->task_data.control.point_name);

								ControlFailed(it->task_data.control.head.group_id, it->task_data.control.point_name, it->task_data.control.stage);

								g_lstTask.erase(it);
								it = g_lstTask.begin();
							}
							else
							{
								PSBOB::CB psbob_cb = { 0 };

								if (oodbread_rk(&psbob_cb, &it->task_data.control.point_name, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) <= 0)
								{
									TRACE("读取breaker表关键字为%ld的记录失败！\r\n", it->task_data.control.point_name);
								}
								else
								{
									//开关已经为遥控目标状态
									if ((psbob_cb.point > 0 && !it->task_data.control.bOpen) || (psbob_cb.point <= 0 && it->task_data.control.bOpen)
										|| !IsInRegion(psbob_cb.id))	//进线开关为主网开关，无法遥控
									{
										ControlSucceed(it->task_data.control.head.group_id, it->task_data.control.point_name, it->task_data.control.stage, false);

										list<CB_CONTROL> lstCB;
										FDIR_TASK_LIST::iterator it_fault;
										char fault_id[FAULT_ID_LENGTH] = { 0 };

										strcpy(fault_id, it->task_data.control.head.group_id);

										if (GetLeftStep(fault_id, lstCB) == 0)
										{
											ClearFdirDB(fault_id);
											sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, fault_id);
											if (ExecSQL(sql) != OCI_ERROR)
											{
												sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, fault_id);
												ExecSQL(sql);
											}

											RemoveTasks(fault_id);
											printf("故障组%s处理完成！！！！\r\n", fault_id);
										}
										else
										{
											for (it_fault = g_lstTask.begin(); it_fault != g_lstTask.end(); it_fault++)
											{
												if (it_fault->rq_type == TASK_TYPE_FAULT)
												{
													if (strcmp(it_fault->task_data.fault.head.group_id, it->task_data.control.head.group_id) == 0)
													{
														it_fault->status = 1;
														if (it_fault->task_data.fault.bAuto == 1)
															AddControlItem(fault_id, lstCB.begin()->point_id, lstCB.begin()->bOpen, lstCB.begin()->stage);
														else
														{
															printf("遥控%ld所在故障组%s非自动执行模式！！\r\n", it->task_data.control.point_name, it->task_data.control.head.group_id);
														}

														for (FDIR_TASK_LIST::iterator it_temp = it_fault; it_temp != g_lstTask.end(); it_temp++)
														{
															if ((it_temp->rq_type == TASK_TYPE_FAULT) && (strcmp(it_temp->task_data.fault.head.group_id, it->task_data.control.head.group_id) == 0))
															{
																it_temp->status = 1;
															}
														}
														break;
													}
												}
											}

											if (it_fault == g_lstTask.end())
											{
												printf("未找到遥控%ld所在故障组%s对应的故障项，无法判断其是否为自动执行模式！！！\r\n", it->task_data.control.point_name, it->task_data.control.head.group_id);
											}
										}

										it->task_data.control.status = TASK_CTL_STATUS_FINISH;
									}
								}
							}

						}
							//以遥信变位作为遥控执行完成的条件
						else if (it->task_data.control.status == TASK_CTL_STATUS_FINISH)
						{
							//接收所有返回消息超时
							if (cost > g_FdirConfig.TIMEOUT_CONTROL)
							{
								printf("[%.2fs]故障%s遥控%ld接收消息时限已到，从任务列表删除\r\n", cost, it->task_data.control.head.fault_id, it->task_data.control.point_name);
								g_lstTask.erase(it);
								it = g_lstTask.begin();
							}
						}

						break;
					}

						//故障项
					case TASK_TYPE_FAULT:
					{
						switch (it->task_data.fault.next_step)
						{
							case TASK_FAULT_STATUS_CALC:
							{
								if(it->task_data.fault.type == TASK_FAULT_LD || it->task_data.fault.type == TASK_FAULT_JXCB)
								{
									PSBOB::CB psbob_cb = {0};
									if(oodbread_rk(&psbob_cb, &it->task_data.fault.point_name, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) <= 0)
									{
										printf("%s(%d):读取PSBOB库断路器表关键字为%ld的记录失败，错误号为:%d\r\n", __FUNCTION__, __LINE__, it->task_data.fault.point_name, _oodb_errno);
									}

									//跳闸开关已经合闸，故障已经恢复
									if (it->task_data.fault.type == TASK_FAULT_JXCB)
									{
										if (psbob_cb.point == 1)
										{
											printf("+++++++++++++跳闸开关%ld已经合闸，故障已经恢复\n", it->task_data.fault.point_name);
											RemoveTask(it->task_data.fault.head.fault_id);
											it = g_lstTask.begin();
											printf("point.value is %d\n", psbob_cb.point);
											break;
										}
									}
								}
								string lst_cb;

								it->task_data.fault.fdir_r->data_start.type = it->task_data.fault.type;
								it->task_data.fault.fdir_r->data_start.source.bus_id= it->task_data.fault.point_name;

								cout << "定位故障:" << it->task_data.fault.head.fault_id << "，并计算隔离恢复方案。" << endl;

								gettimeofday(&it->tv_start, NULL);

								switch (it->task_data.fault.fdir_r->FaultDetection())
								{
									case FDIR_R_SUCCEED:
									{
										printf("故障定位成功\n");

										//将故障使用过的故障信号使用次数加1
										pthread_mutex_lock(&trig_list_mutex);
										for (FDIR_POINT_TRIG_SECTION::iterator it_used_trig = g_lstSecTrig.begin(); it_used_trig != g_lstSecTrig.end(); it_used_trig++)
										{
											printf("故障信号：key_id:%ld, st_id:%ld使用次数加1\r\n", (*it_used_trig)->alarm.key_id, (*it_used_trig)->alarm.st_id);
											(*it_used_trig)->lst_fault.push_back(it->task_data.fault.head.fault_id);

											it->task_data.fault.trig_list.push_back(**it_used_trig);
										}
										pthread_mutex_unlock(&trig_list_mutex);

										printf("故障定位成功\n");
										//为多重故障分组，故障先只计算到定位这一步
										it->task_data.fault.next_step = TASK_FAULT_STATUS_CALC_ISO;

										//生成故障描述文本文件
										form_fdir_txt(*it);
										//为多重故障分组，故障先只计算到定位这一步
										it->task_data.fault.next_step = TASK_FAULT_STATUS_CALC_ISO;

										break;
									}

									case FDIR_FAIL_DIRECT:
									{
										//将故障使用过的故障信号使用次数加1
										pthread_mutex_lock(&trig_list_mutex);
										for (FDIR_POINT_TRIG_SECTION::iterator it_used_trig = g_lstSecTrig.begin(); it_used_trig != g_lstSecTrig.end(); it_used_trig++)
										{
											printf("故障信号：key_id:%ld, st_id:%ld使用次数加1\r\n", (*it_used_trig)->alarm.key_id, (*it_used_trig)->alarm.st_id);
											(*it_used_trig)->lst_fault.push_back(it->task_data.fault.head.fault_id);

											it->task_data.fault.trig_list.push_back(**it_used_trig);
										}
										pthread_mutex_unlock(&trig_list_mutex);

										TRACE("it->task_data.fault.point_name:%ld\r\n", it->task_data.fault.point_name);
										PSBOB::DV dv = { 0 };
										oodbread_rk(&dv, &it->task_data.fault.dv, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(PSBOB::DV));

//							PSBOB::ST st_info = { 0 };
//
//							if (dv.id != 0)
//							{
//								GetStbyDv(dv.id, st_info);
//							}
//							if (st_info.id == 0)
//							{
//								printf("读取st失败，dv为：%ld, line:%d\r\n", dv.id, __LINE__);
//							}

										//写故障信号表
										TRACE("写故障信号表！\r\n");
										int pos = 0;
										for (FDIR_POINT_TRIG_LIST::iterator it_trig_list = it->task_data.fault.trig_list.begin(); it_trig_list != it->task_data.fault.trig_list.end(); it_trig_list++, pos++)
										{
											sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_ALARM (ID, TM_ADD, OCCUR_TIME, MSEC, KEY_ID, NAME, ST_ID, TYPE, STATUS) VALUES (\'%s\',%ld, %ld, %d, %ld, \'%s\', %ld, %d, %d)", it->task_data.fault.head.fault_id
													,it_trig_list->tm_add.tv_sec, it_trig_list->alarm.occur_time, it_trig_list->alarm.msec, it_trig_list->alarm.key_id, it_trig_list->alarm.name.c_str(), it_trig_list->alarm.st_id, it_trig_list->alarm.type, it_trig_list->alarm.status);

											ExecSQL(sql);
										}

										sprintf(sql, "Insert into PMS_PARAM.FDIR_FDIRECT (ID, GROUPID, STATUS, TYPE, STUDY_EQ, FD, FD_DESCR, ST_ID) Values (\'%s\', \'%s\', 0, 0, \'%ld\', \'%ld\', \'%s\', \'%ld\')",
												it->task_data.fault.head.fault_id, it->task_data.fault.head.fault_id, it->task_data.fault.point_name, dv.id, dv.name, it->task_data.fault.st);

										ExecSQL(sql);

										//写模拟态故障信号表
										TRACE("写入故障模拟关系库！\r\n");
										if ((it->task_data.fault.signal_type) == TRIG_TYPE_SIMU)
										{
											sprintf(sql, "UPDATE PMS_PARAM.FDIR_SIG_SUMMARY SET RUN_TIME=(SELECT RUN_TIME FROM PMS_PARAM.FDIR_SIG_SUMMARY WHERE ID=\'%s\')+1, \"INDEX\"=(SELECT RUN_TIME FROM PMS_PARAM.FDIR_SIG_SUMMARY WHERE ID=\'%s\')+1 WHERE ID=\'%s\' ",
													it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.simu_plan_id.c_str());
											ExecSQL(sql);

											sprintf(sql, "SELECT ID FROM PMS_PARAM.FDIR_SIG_FAULT WHERE ID LIKE \'%s\'",
													it->task_data.fault.simu_plan_id.c_str());
											char *buf = NULL;
											int rec_num, attr_num;
											struct ORA_ATTR *attrs = NULL;
											pthread_mutex_lock(&oci_mutex);
											int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
											pthread_mutex_unlock(&oci_mutex);
											if (ret != OCI_ERROR)
											{
												if (rec_num == 0)
												{
													sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_FAULT(ID, FAULT_INDEX, FAULT_ID)VALUES(\'%s\', 0, \'%s\')",
															it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.head.fault_id);
													ExecSQL(sql);
												}
											}

											sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_FAULT(ID, FAULT_INDEX, FAULT_ID)VALUES(\'%s\', (select max(FAULT_INDEX) from pms_param.fdir_sig_fault where id like \'%s%\')+1, \'%s\')",
													it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.head.fault_id);
											ExecSQL(sql);

											g_is_write_to_lib = true;
											pthread_mutex_unlock(&simu_sync_mutex);
										}

										TRACE("故障%s定位失败，将从任务列表中删除\r\n", it->task_data.fault.head.fault_id);

										if (version == VERSION_REALTIME)
										{
											//D5000发故障隔离恢复事件
											Notify(it->task_data.fault.head.fault_id, it->task_data.fault.dv, it->task_data.fault.st);
										}
										else if (version == VERSION_STUDY)
										{
										}

//							g_lstTask.erase(it);
										RemoveTask(it->task_data.fault.head.fault_id);
										it = g_lstTask.begin();
										break;
									}

									default:
										break;
								}

//						it->task_data.fault.next_step = TASK_FAULT_STATUS_ISO;

								break;
							}

							case TASK_FAULT_STATUS_WAIT:
							{
								if (cost > g_FdirConfig.TIMEOUT_FAULT) //故障启动超时，可能进线开关重合闸成功
								{
									printf("[%.2fs]故障%s启动超时，从任务列表删除，可能进线开关重合闸成功\r\n", cost, it->task_data.fault.head.fault_id);
//							g_lstTask.erase(it);
//							it = g_lstTask.begin();
									RemoveTask(it->task_data.fault.head.fault_id);
									it = g_lstTask.begin();
								}

								break;
							}

							case TASK_FAULT_STATUS_ISO:
							{
								if ((cost > g_FdirConfig.TIMEOUT_AUTO && g_FdirConfig.USER2AUTO) || it->task_data.fault.bAuto == 1)
								{
									//超时，允许转自动
									if (/*g_FdirConfig.USER2AUTO && */it->task_data.fault.bAuto == 0)
									{
										printf("[%.2fs]执行隔离恢复超时，转自动继续执行。\r\n", cost);

										it->task_data.fault.bAuto = 1;

										FormFdirStep(it->task_data.fault.head.group_id);

										list<CB_CONTROL> lstcb;

										GetLeftStep(it->task_data.fault.head.fault_id, lstcb);

										//还有步骤未执行
										if (lstcb.size() > 0)
										{
											AddControlItem(it->task_data.fault.head.fault_id, lstcb.begin()->point_id, lstcb.begin()->bOpen, lstcb.begin()->stage);
										}
										else
										{
										}
									}
								}

								break;
							}

							case TASK_FAULT_STATUS_PROCESS:
							{
//						cout << "故障:" << it->task_data.fault.head.fault_id << "正在恢复" << endl;

								break;
							}

							case TASK_FAULT_STATUS_SLEEP:
							{
								pthread_mutex_lock(&trig_list_mutex);
								FDIR_POINT_TRIG_LIST::iterator it_trig_list;

								for (it_trig_list = g_lstTrig.begin(); it_trig_list != g_lstTrig.end(); it_trig_list++)
								{
									//找到该故障的启动故障信号在故障信号列表中的位置
									if (it_trig_list->tm_add.tv_sec == it->tv_creat.tv_sec && it_trig_list->tm_add.tv_usec == it->tv_creat.tv_usec)
									{
										break;
									}
								}

								if (it_trig_list != g_lstTrig.end())
								{
									int sleeptime = g_FdirConfig.TIMEOUT_PROTPNT;
#ifdef _QingHai_
							if (strcmp(it_trig_list->psbob_data.point.devtype, "faultinfo") == 0)
							{
								PSBOB::FAULTINFO fault =
								{	0};
								if (oodbread_rk(&fault, it_trig_list->psbob_data.point.classid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo")), sizeof(PSBOB::FAULTINFO)) == OO_FAIL)
								{
									cout<<"读取故障指示器记录失败,故障指示器point id ： "<<it_trig_list->psbob_data.point.id<<" ,"<<__FUNCTION__<<endl;
								}
								else
								{
									if (strcmp(fault.jdpointid, it_trig_list->psbob_data.point.id) == 0)
									{
										sleeptime += g_FdirConfig.TIMEOUT_JD_FAULT;
									}
								}

							}
#endif
									if (cost > sleeptime) //故障启动超时，可能进线开关重合闸成功
									{
										printf("[%.2fs]故障%s所有开关过流信号已经送达\r\n", cost, it->task_data.fault.head.fault_id);

										//将故障启动信号前后30秒的故障信号准备好
										g_lstSecTrig.clear();

										//后30秒
										FDIR_POINT_TRIG_LIST::iterator itTemp = it_trig_list;

										while (itTemp != g_lstTrig.end())
										{
											int span = itTemp->tm_add.tv_sec - it_trig_list->tm_add.tv_sec;

											if (span > g_FdirConfig.TIMEOUT_PROTPNT)
											{
												break;
											}
											else
											{
												printf("故障信号：key_id:%ld, st_id:%ld添加到定位信号列表[-%ds]\r\n", itTemp->alarm.key_id, itTemp->alarm.st_id, span);
												g_lstSecTrig.push_back(&(*itTemp));
											}

											itTemp++;
										};

										//前30秒
										itTemp = it_trig_list;

										while (itTemp != g_lstTrig.begin())
										{
											itTemp--;

											int span = it_trig_list->tm_add.tv_sec - itTemp->tm_add.tv_sec;

											if (span > g_FdirConfig.TIMEOUT_PROTPNT)
											{
												break;
											}
											else
											{
												g_lstSecTrig.push_back(&(*itTemp));
												printf("故障信号：key_id:%ld, st_id:%ld添加到定位信号列表[%ds]\r\n", itTemp->alarm.key_id, itTemp->alarm.st_id, span);
											}

										}

										it->task_data.fault.next_step = TASK_FAULT_STATUS_CALC;
									}
									else
									{
										printf("[%.2fs]故障%s正在等待所有开关过流信号送达\r\n", cost, it->task_data.fault.head.fault_id);
									}
								}

								pthread_mutex_unlock(&trig_list_mutex);

								break;
							}

							case TASK_FAULT_STATUS_CALC_ISO:
							{
								//研究态下的分组
								if (version == VERSION_STUDY)
								{
									if (GroupFault(g_lstTask) == FDIR_R_SUCCEED)
									{
										printf("分组完成\n");
									}

									break;
								}

								bool bCanGroup = false;			//是否可以开始分组了
								bool bhas_unused_trig = false; 	//是否存在未定位完成的故障信号

								pthread_mutex_lock(&trig_list_mutex);
								for (FDIR_POINT_TRIG_LIST::iterator it_trig = g_lstTrig.begin(); it_trig != g_lstTrig.end(); it_trig++)
								{
									//先在信号列表中找到该故障的启动故障信号,该故障信号即为故障的启动故障信号
									if (it_trig->tm_add.tv_sec == it->tv_creat.tv_sec && it_trig->tm_add.tv_usec == it->tv_creat.tv_usec)
									{
										printf("key_id:%ld,st_id:%ld为故障%s的启动信号！！！！！\r\n", it_trig->alarm.key_id, it_trig->alarm.st_id, it->task_data.fault.head.fault_id);

										//以该信号为锚点
										timeval tm_anchor = { it_trig->tm_add.tv_sec, it_trig->tm_add.tv_usec };

										//检查该故障收集信号的30秒内是否有未定位的故障和其它故障
										FDIR_POINT_TRIG_LIST::iterator it_trig_tmp = it_trig;
										int span = 0;

										while (true)
										{
											if ((++it_trig_tmp) == g_lstTrig.end())
											{
												break;
											}

											//先判断该信号是否在30秒区间内
											span = it_trig_tmp->tm_add.tv_sec - tm_anchor.tv_sec;
											if (span > g_FdirConfig.TIMEOUT_PROTPNT)
											{
												break;
											}

											//存在未定位完成的故障信号，需要等待该故障信号定位完成
											if ((it_trig_tmp->lst_fault.size() <= 0) /*&& (strcmp(it_trig_tmp->alarm.fault_info, key[1]) == 0 || strcmp(it_trig_tmp->alarm.fault_info, key[2]) == 0)*/)
											{
												bhas_unused_trig = true;
												break;
											}
											//30秒内有其它故障B，重新检查故障B收集信号的30秒
											if (it_trig_tmp->lst_fault.size() > 0)
											{
												if (strcmp(it_trig_tmp->lst_fault.back().c_str(), it->task_data.fault.head.fault_id) != 0)
												{
													tm_anchor.tv_sec = it_trig_tmp->tm_add.tv_sec;
													tm_anchor.tv_usec = it_trig_tmp->tm_add.tv_usec;
												}
											}
										}

										if (!bhas_unused_trig)
										{
											bCanGroup = true;
										}

										break;
									}
								}
								pthread_mutex_unlock(&trig_list_mutex);

								if (bCanGroup)
								{
									TRACE("开始进行分组\r\n");
									if (GroupFault(g_lstTask) == FDIR_R_SUCCEED)
									{
										printf("分组完成\n");
									}
								}
								printf("TASK_FAULT_STATUS_CALC_ISO out\n");
								break;
							}

							case TASK_FAULT_STATUS_GROUPED:
							{
								FDIR_TASK_LIST::iterator itTemp;
								list<FDIR_R2*> lst_fdir_r2;
								map<FDIR_R2*, FDIR_TASK*> map_fdir_task;

								printf("开始计算组号为%s的操作方案，该组的故障有：\r\n", it->task_data.fault.head.group_id);

								for (itTemp = g_lstTask.begin(); itTemp != g_lstTask.end(); itTemp++)
								{
									if (itTemp->rq_type != TASK_TYPE_FAULT)
										continue;

									if (strcmp(itTemp->task_data.fault.head.group_id, it->task_data.fault.head.group_id) == 0)
									{
										lst_fdir_r2.push_back(itTemp->task_data.fault.fdir_r);
										map_fdir_task[itTemp->task_data.fault.fdir_r] = &(*itTemp);

										itTemp->task_data.fault.next_step = TASK_FAULT_STATUS_CALC_STEP;
										cout << it->task_data.fault.head.group_id << "->" << itTemp->task_data.fault.head.fault_id << endl;
									}
								}

								list<list<MAP_SO_NODE> > lst_steps,lst_steps2;

								int calc_ret = calc_restore_step(lst_fdir_r2, lst_steps);	//计算主站遥控方案
								int calc_ret2 = calc_restore_step(lst_fdir_r2, lst_steps2, false);	//计算最小停电方案

								char *table_name = new char[10];
								bzero(table_name, 10);

								if (it->task_data.fault.bAuto == 2)
								{
									sprintf(table_name, "_HIS");
								}

								////故障组中由于隔离或者跳闸导致失电的负荷，最终保存非故障不可恢复的负荷
								list<FAULT_ITEM> lst_group_ld;
								////故障组中由于隔离或者跳闸导致失电，存在恢复方案，可以恢复的负荷
								list<FAULT_ITEM> lst_able_res;

								//首先把故障定位信息写入数据库中,隔离和恢复步骤根据返回结果分别写入
								for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task = map_fdir_task.begin(); it_map_fdir_task != map_fdir_task.end(); it_map_fdir_task++)
								{
									lst_group_ld.splice(lst_group_ld.end(), it_map_fdir_task->first->m_ld);
									string lst_cb;
									char fault_area[1024] = { 0 };
//							PSBOB::ST st = { 0 };
									PSBOB::DV dv = { 0 };

									int faultzones = it_map_fdir_task->first->getfaultzone();
//								it_map_fdir_task->first->GetDVByZone(faultzones, dv);
									oodbread_rk(&dv, &(it_map_fdir_task->second->task_data.fault.dv), const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(PSBOB::DV));
									if (dv.id == 0)
									{
										printf("读取DV失败:故障区段为%d，errno:%d,line:%d\r\n", faultzones, _oodb_errno, __LINE__);
									}
									else
									{
//								if (GetStbyDv(dv.id, st) != 0)
//								{
//									printf("读取ST失败:DV为%ld，errno:%d,line:%d\r\n", dv.id, _oodb_errno, __LINE__);
//								}
									}

									it_map_fdir_task->first->get_zone_cb(it_map_fdir_task->first->getfaultzone(), lst_cb);

									//设置故障区域起始设备
									FAULT_ITEM head;
									list<FAULT_ITEM> tail;
									it_map_fdir_task->first->get_fault_area(head, tail);

									//负荷单点故障的故障设备只需写入负荷自己而已
									if (it_map_fdir_task->first->b_ld_fault)
									{
										sprintf(sql, "INSERT INTO FDIR_FAULT_DEV (ID, DEV_ID, DEV_TABLE, EDGE, FLAG) VALUES (\'%s\', \'%ld\', \'%s\', %d, 1)",
												it_map_fdir_task->second->task_data.fault.head.fault_id, head.eq_id, head.table.c_str(), 1);
										ExecSQL(sql);
									}
									else
									{
										//故障区域设备写入数据库中
										write_fault_dev(it_map_fdir_task->second->task_data.fault.head.fault_id, table_name, it_map_fdir_task->first);
									}

									//将故障信号、误报信号 、漏报信号拼接成字符串
									string str_point, str_err_point, str_mis_point;
									list<FAULT_ITEM>::iterator it_recv_point;
									for (it_recv_point = it_map_fdir_task->first->lst_point.begin(); it_recv_point != it_map_fdir_task->first->lst_point.end(); it_recv_point++)
									{
										char str_fault_item[100];
										if (strcmp(it_recv_point->table.c_str(), "FI") == 0)
										{
											sprintf(str_fault_item, "%ld[%s]", it_recv_point->id, it_recv_point->table.c_str());
										}
										else
										{
											sprintf(str_fault_item, "%ld[%s]", it_recv_point->eq_id, it_recv_point->table.c_str());
										}

										str_point += str_fault_item;
										str_point += ";";
									}

									list<long>::iterator it_point;
									char err_point[30];
									char mis_point[30];
									for (it_point = it_map_fdir_task->first->lst_err_point.begin(); it_point != it_map_fdir_task->first->lst_err_point.end(); it_point++)
									{
										sprintf(err_point, "%ld", *it_point);
										str_err_point += err_point;
										str_err_point += ";";
									}

									for (it_point = it_map_fdir_task->first->lst_mis_point.begin(); it_point != it_map_fdir_task->first->lst_mis_point.end(); it_point++)
									{
										sprintf(mis_point, "%ld", *it_point);
										str_mis_point += mis_point;
										str_mis_point += ";";
									}

									string manual_restore;

									for (it_point = it_map_fdir_task->first->lst_restore.begin(); it_point != it_map_fdir_task->first->lst_restore.end(); it_point++)
									{
										char sz_manual_point[30];

										sprintf(sz_manual_point, "%ld", *it_point);
										PSBOB::CB point = { 0 };
										manual_restore += sz_manual_point;

										if (is_controllable(*it_point))
											manual_restore += "[0];";
										else
											manual_restore += ";";
									}

									//西宁配网直接将故障区段开关转化为fault_area
									fault_area[0] = 0;

									long fault_cb = it_map_fdir_task->first->getfaultcb();

									sprintf(fault_area, "%ld[%s]", head.eq_id, head.table.c_str());

									for (list<FAULT_ITEM>::iterator it_tail = tail.begin(); it_tail != tail.end(); it_tail++)
									{
										char tmp[256] = { 0 };
										sprintf(tmp, ";%ld[%s]", it_tail->eq_id, it_tail->table.c_str());
										strcat(fault_area, tmp);
									}

									//写故障信号表
									TRACE("写故障信号表！\r\n");
									int pos = 0;
									for (FDIR_POINT_TRIG_LIST::iterator it_trig_list = it_map_fdir_task->second->task_data.fault.trig_list.begin();
										 it_trig_list != it_map_fdir_task->second->task_data.fault.trig_list.end(); it_trig_list++, pos++)
									{
										sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_ALARM (ID, TM_ADD, OCCUR_TIME, MSEC, KEY_ID, NAME, ST_ID, TYPE, STATUS) VALUES (\'%s\', %ld, %ld, %d, %ld, \'%s\', %ld, %d, %d)", it_map_fdir_task->second->task_data.fault.head.fault_id
												,it_trig_list->tm_add.tv_sec, it_trig_list->alarm.occur_time, it_trig_list->alarm.msec, it_trig_list->alarm.key_id, it_trig_list->alarm.name.c_str(), it_trig_list->alarm.st_id, it_trig_list->alarm.type, it_trig_list->alarm.status);

										ExecSQL(sql);
									}

									//写不能隔离、不能跳闸、老数据等开关
									TRACE("写不能隔离、不能跳闸、老数据等开关！\r\n");
									for (list<FDIR_CB_INFO>::iterator it_cb_info = it_map_fdir_task->first->lst_cb_info.begin(); it_cb_info != it_map_fdir_task->first->lst_cb_info.end(); it_cb_info++)
									{
										sprintf(sql,
												"INSERT INTO PMS_PARAM.FDIR_CB_INFO(ID, CB_ID, CB_DESCR, PT_ID, CB_Q, CB_STATUS, PT_CONTROL, PT_TIMEXC, PT_QUALITY, TYPE) VALUES(\'%s\', \'%ld\', \'%s\', \'%ld\', %d, %d, %d, %d, %d, %d)",
												it_map_fdir_task->second->task_data.fault.head.fault_id, it_cb_info->cb_id, it_cb_info->cb_descr, it_cb_info->pt_id, it_cb_info->cb_q, it_cb_info->cb_status,
												it_cb_info->pt_control, it_cb_info->pt_timexc, it_cb_info->pt_quality, (int) it_cb_info->reason);

										ExecSQL(sql);
									}

									//专家意见，有漏报故障信号的故障由需要由自动处理转为手动处理
									if(it_map_fdir_task->first->lst_mis_point.size() > 0)
									{
										it_map_fdir_task->second->task_data.fault.bAuto = 0;
									}

									//将漏报信息写入关系库PMS_PARAM.FDIR_DEV_STATUS表中，DEV_STATUS字段为0时为漏报
									for (it_point = it_map_fdir_task->first->lst_mis_point.begin(); it_point != it_map_fdir_task->first->lst_mis_point.end(); it_point++)
									{
										long lPrtId = *it_point;
										string strDevTable;
										string strDevDescr;
										PSBOB::CB misCb;
										PSBOB::FI misFi;
										if (oodbread_rk(&misCb, &lPrtId, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) > 0)
										{
											strDevTable = "CB";
											std::string strCb = string(misCb.name);
											strDevDescr = strCb.append(string("故障信号"));
										}
										else if(oodbread_rk(&misFi, &lPrtId, const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo")), sizeof(PSBOB::FI)) > 0)
										{
											strDevTable = "FI";
											strDevDescr = string(misFi.name) + "故障信号";
										}
										else
										{
											strDevTable = "未能识别的故障信号";
										}

										//将漏报信号写入实时库
										sprintf(sql, "INSERT into PMS_PARAM.FDIR_DEV_STATUS (FAULT_ID, DEV_ID, DEV_TABLE, DEV_STATUS, DEV_DESCR) VALUES (\'%s\', %ld, \'%s\', %d, \'%s\')",
												it_map_fdir_task->second->task_data.fault.head.fault_id, lPrtId, strDevTable.c_str(), 0, strDevDescr.c_str());
										ExecSQL(sql);
									}

									//获取实时库psbob的版本号
									PSBOB::GLOBAL global = { 0 };
									if (oodbread_rp(&global, 0, const_cast<TB_DESCR*>(g_db_psbob->GetTB("secinfo")), 1, sizeof(global)) <= 0)
									{
										TRACE("读取global记录失败\r\n");
									}

									//写故障定位表
									int type = -1;		//故障类型
									switch (it_map_fdir_task->first->fault_type)
									{
										case FAULT_TYPE_NONE:
											type = STATUS_FDIRECT_NONE;
											break;

										case FAULT_TYPE_SURE:
											type = STATUS_FDIRECT_DIRECT_OK;
											break;

										case FAULT_TYPE_WINK:
											type = STATUS_FDIRECT_WINK;
											break;

										case FAULT_TYPE_POSSIBLE:
											type = STATUS_FDIRECT_POSSIBLE;
											break;
										case FAULT_TYPE_BUS:
											type = STATUS_FDIRECT_DIRECT_OK;
											break;

										default:
											break;
									}

									//杨勇 20141216 修改故障汇总表数据来源，解决进线开关故障推不到馈线图和不能推环网图的问题
									long lStId = 0;
									long lDVId = 0;
									std::string strDVDescr;
									if (it->task_data.fault.fdir_r->lFaultStId > 0)
									{
										PSBOB::ST faultSt = {0};
										lStId = it->task_data.fault.fdir_r->lFaultStId;
										int iRet = oodbread_rk(&faultSt, &lStId, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(PSBOB::ST));
										if (iRet > 0)
										{
											lDVId = faultSt.subarea_id;
											PSBOB::DV faultDV = {0};
											iRet = oodbread_rk(&faultDV, &lDVId, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")),sizeof(PSBOB::DV));
											if (iRet <= 0)
											{
												TRACE("在区域表中查找关键字为%ld的记录失败啦！\r\n", lDVId);
											}
											else
											{
												TRACE("查询到的DVId：%ld\r\n", faultDV.id);
												strDVDescr = faultDV.name;
											}
										}
									}

									if (lStId > 0 && lDVId > 0)
									{
										TRACE("New DV!!!!!!!!!!!!!!\r\n");
										sprintf(sql,
												"Insert into PMS_PARAM.FDIR_FDIRECT%s (ID, GROUPID, FD, FD_DESCR, ZONE, ST_ID, CB_ID, STATUS, STARTTYPE, ZONE_CB, FAULT_AREA, RECV_POINT, ERR_POINT, MIS_POINT, type, MANUAL_RESTORE, STUDY_EQ, AUTO_DEAL,EXT_STATUS,JXCB,DB_VERSION) "
														"Values (\'%s\'/*ID*/, \'%s\'/*GROUPID*/, \'%ld\'/*FD*/, \'%s\'/*FD_DESCR*/, \'%d\'/*ZONE*/, \'%ld\'/*ST_ID*/, \'%ld\'/*CB_ID*/, %d/*STATUS*/, %d/*STARTTYPE*/, \'%s\'/*ZONE_CB*/, \'%s\'/*FAULT_AREA*/, \'%s\'/*RECV_POINT*/, \'%s\'/*ERR_POINT*/, \'%s\'/*MIS_POINT*/, %d/*type*/, \'%s\'/*MANUAL_RESTORE*/, \'%ld\'/*STUDY_EQ*/,%d/*AUTO_DEAL*/,%d/*EXT_STATUS*/,%d/*JXCB*/,%d/*DB_VERSION*/)",
												table_name, it_map_fdir_task->second->task_data.fault.head.fault_id, it_map_fdir_task->second->task_data.fault.head.group_id, lDVId, strDVDescr.c_str(),
												it_map_fdir_task->first->getfaultzone(), lStId, fault_cb, version == VERSION_REALTIME ? type : 1, 0, lst_cb.c_str(), fault_area, str_point.c_str(),
												str_err_point.c_str(), str_mis_point.c_str(), version == VERSION_REALTIME ? 0 : 1, manual_restore.c_str(), it_map_fdir_task->second->task_data.fault.point_name,
												it->task_data.fault.bAuto == 2 ? 1 : 0, version == VERSION_REALTIME ? type : 1, it_map_fdir_task->first->getfaultjxcb(), global.sec_no);
									}
									else
									{
										sprintf(sql,
												"Insert into PMS_PARAM.FDIR_FDIRECT%s (ID, GROUPID, FD, FD_DESCR, ZONE, ST_ID, CB_ID, STATUS, STARTTYPE, ZONE_CB, FAULT_AREA, RECV_POINT, ERR_POINT, MIS_POINT, type, MANUAL_RESTORE, STUDY_EQ, AUTO_DEAL,EXT_STATUS,JXCB,DB_VERSION) "
														"Values (\'%s\'/*ID*/, \'%s\'/*GROUPID*/, \'%ld\'/*FD*/, \'%s\'/*FD_DESCR*/, \'%d\'/*ZONE*/, \'%ld\'/*ST_ID*/, \'%ld\'/*CB_ID*/, %d/*STATUS*/, %d/*STARTTYPE*/, \'%s\'/*ZONE_CB*/, \'%s\'/*FAULT_AREA*/, \'%s\'/*RECV_POINT*/, \'%s\'/*ERR_POINT*/, \'%s\'/*MIS_POINT*/, %d/*type*/, \'%s\'/*MANUAL_RESTORE*/, \'%ld\'/*STUDY_EQ*/,%d/*AUTO_DEAL*/,%d/*EXT_STATUS*/,%d/*JXCB*/,%d/*DB_VERSION*/)",
												table_name, it_map_fdir_task->second->task_data.fault.head.fault_id, it_map_fdir_task->second->task_data.fault.head.group_id, dv.id, dv.name,
												it_map_fdir_task->first->getfaultzone(), it->task_data.fault.st, fault_cb, version == VERSION_REALTIME ? type : 1, 0, lst_cb.c_str(), fault_area, str_point.c_str(),
												str_err_point.c_str(), str_mis_point.c_str(), version == VERSION_REALTIME ? 0 : 1, manual_restore.c_str(), it_map_fdir_task->second->task_data.fault.point_name,
												it->task_data.fault.bAuto == 2 ? 1 : 0, version == VERSION_REALTIME ? type : 1, it_map_fdir_task->first->getfaultjxcb(), global.sec_no);
									}

									ExecSQL(sql);

									list<DEV_STATUS>::iterator it_dev;

									for (it_dev = it_map_fdir_task->first->lst_warning.begin(); it_dev != it_map_fdir_task->first->lst_warning.end(); it_dev++)
									{
										sprintf(sql, "INSERT into PMS_PARAM.FDIR_DEV_STATUS (FAULT_ID, DEV_ID, DEV_TABLE, DEV_STATUS) VALUES (\'%s\', \'%ld\', \'%s\', %d)",
												it_map_fdir_task->second->task_data.fault.head.fault_id, it_dev->id, it_dev->table.c_str(), it_dev->status);
										ExecSQL(sql);
									}

									if (it_map_fdir_task->first->lst_dev.size() > 0)
									{
										for (it_dev = it_map_fdir_task->first->lst_dev.begin(); it_dev != it_map_fdir_task->first->lst_dev.end(); it_dev++)
										{
											sprintf(sql, "INSERT into PMS_PARAM.FDIR_DEV_STATUS (FAULT_ID, DEV_ID, DEV_TABLE, DEV_STATUS) VALUES (\'%s\', \'%ld\', \'%s\', %d)",
													it_map_fdir_task->second->task_data.fault.head.fault_id, it_dev->id, it_dev->table.c_str(), it_dev->status);
											ExecSQL(sql);
										}
									}
								}
								lst_group_ld.sort();
								lst_group_ld.unique();
								TRACE("lst中负荷的个数是：%d\r\n", lst_group_ld.size());
								//恢复方案计算成功，把恢复方案、隔离方案、定位信息写入表中
								if (calc_ret == FDIR_R_SUCCEED)
								{
									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task = map_fdir_task.begin(); it_map_fdir_task != map_fdir_task.end(); it_map_fdir_task++)
									{
										//保存故障馈线开关信息，做数据校验用
										write_fault_tree(it_map_fdir_task->second->task_data.fault.head.group_id, it_map_fdir_task->first);
									}

									//写负荷转供步骤和负荷转供区段
									write_fhzg_step(it->task_data.fault.head.group_id, table_name, lst_steps, map_fdir_task.begin()->first, lst_able_res, list<long>(), true);

									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task_temp = map_fdir_task.begin(); it_map_fdir_task_temp != map_fdir_task.end(); it_map_fdir_task_temp++)
									{
										//写上游恢复区域设备
										write_fault_up_dev(it_map_fdir_task_temp->second->task_data.fault.head.fault_id, table_name, it_map_fdir_task_temp->first, false, lst_able_res);

										//写故障隔离步骤表
										PSBOB::CB psbob_point = { 0 };
										const TTable<CLS_FDIR_ISOLATE>* isolate = it_map_fdir_task_temp->first->getisolate();
										int i = 0;
										for (i = 0; i < isolate->GetCount(); i++)
										{
											int up_isolate = 0;
											if (isolate->Data[i].q & FDIR_ISOLATE_NO_SO)
											{
												TRACE("隔离开关%ld下游无可转供电源，不隔离！\r\n", isolate->Data[i].cb);
												continue; //下游不可恢复的开关不需要隔离
											}
											if (isolate->Data[i].q & FDIR_ISOLATE_UP)
											{
												up_isolate = 1;
											}

											bzero(&psbob_point, sizeof(psbob_point));

											if (oodbread_rk(&psbob_point, &isolate->Data[i].cb, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_point)) <= 0)
											{
												TRACE("读取PSBOB库POINT表ID为%ld的记录失败，错误号：%d\r\n", isolate->Data[i].cb, _oodb_errno);
											}
											sprintf(sql,
													"Insert into PMS_PARAM.FDIR_ISOLATE%s (ID, STEP, ICB, CB, CB_DESCR, IMODE, CTRL_TYPE, CONTROL, UP_ISOLATE) Values (\'%s\', %d, %d, \'%ld\', \'%s\', %d, 0, %d, %d)",
													table_name, it_map_fdir_task_temp->second->task_data.fault.head.fault_id/*故障标识符*/, i/*故障隔离序号*/, isolate->Data[i].icb/*要执行的开关逻辑号*/,
													isolate->Data[i].cb/*要执行的开关id*/, isolate->Data[i].cb_desc/*要执行的开关描述*/, it_map_fdir_task_temp->second->task_data.fault.bAuto == 1/*自动*/? 4 : 3,
													is_controllable(psbob_point.id) ? 1 : 0, up_isolate);

											ExecSQL(sql);
										}
									}


									//写模拟态故障信号表
									TRACE("写入故障模拟关系库！\r\n");
									if ((it->task_data.fault.signal_type) == TRIG_TYPE_SIMU)
									{
										sprintf(sql, "UPDATE PMS_PARAM.FDIR_SIG_SUMMARY SET RUN_TIME=(SELECT RUN_TIME FROM PMS_PARAM.FDIR_SIG_SUMMARY WHERE ID=\'%s\')+1, FAULT_INDEX=(SELECT RUN_TIME FROM PMS_PARAM.FDIR_SIG_SUMMARY WHERE ID=\'%s\')+1 WHERE ID=\'%s\' ",
												it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.simu_plan_id.c_str());
										ExecSQL(sql);

										sprintf(sql, "SELECT ID FROM PMS_PARAM.FDIR_SIG_FAULT WHERE ID LIKE \'%s\'",
												it->task_data.fault.simu_plan_id.c_str());
										char *buf = NULL;
										int rec_num, attr_num;
										struct ORA_ATTR *attrs = NULL;
										pthread_mutex_lock(&oci_mutex);
										int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
										pthread_mutex_unlock(&oci_mutex);
										if (ret != OCI_ERROR)
										{
											if (rec_num == 0)
											{
												sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_FAULT(ID, FAULT_INDEX, FAULT_ID)VALUES(\'%s\', 0, \'%s\')",
														it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.head.fault_id);
												ExecSQL(sql);
											}
										}

										g_oci->Readdata_Free();

										sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_FAULT(ID, FAULT_INDEX, FAULT_ID)VALUES(\'%s\', (select max(FAULT_INDEX) from pms_param.fdir_sig_fault where id like \'%s%\')+1, \'%s\')",
												it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.simu_plan_id.c_str(), it->task_data.fault.head.fault_id);
										ExecSQL(sql);

										g_is_write_to_lib = true;
										pthread_mutex_unlock(&simu_sync_mutex);
									}
								}
								else if (calc_ret == FDIR_FAIL_RESTORE) //恢复方案计算失败，把隔离和定位信息写入表中
								{
									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task = map_fdir_task.begin(); it_map_fdir_task != map_fdir_task.end(); it_map_fdir_task++)
									{
										//写上游恢复区段表
										//获取上游要恢复的区段号
										write_fault_up_dev(it_map_fdir_task->second->task_data.fault.head.fault_id, table_name, it_map_fdir_task->first, false, lst_able_res);

										//写故障隔离步骤表
										PSBOB::CB psbob_point = { 0 };
										const TTable<CLS_FDIR_ISOLATE>* isolate = it_map_fdir_task->first->getisolate();
										int i = 0;
										for (i = 0; i < isolate->GetCount(); i++)
										{
											int up_isolate = 0;
											if (isolate->Data[i].q & FDIR_ISOLATE_NO_SO)
											{
												TRACE("隔离开关%ld下游无可转供电源，不隔离！\r\n", isolate->Data[i].cb);
												continue; //下游不可恢复的开关不需要隔离
											}
											if (isolate->Data[i].q & FDIR_ISOLATE_UP)
											{
												up_isolate = 1;
											}

											bzero(&psbob_point, sizeof(psbob_point));

											if (oodbread_rk(&psbob_point, &isolate->Data[i].cb, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_point)) <= 0)
											{
												TRACE("读取PSBOB库POINT表ID为%ld的记录失败\r\n", isolate->Data[i].cb);
											}
											sprintf(sql,
													"Insert into PMS_PARAM.FDIR_ISOLATE%s (ID, STEP, ICB, CB, CB_DESCR, IMODE, CTRL_TYPE, CONTROL, UP_ISOLATE) Values (\'%s\', %d, %d, \'%ld\', \'%s\', %d, 0, %d, %d)",
													table_name, it_map_fdir_task->second->task_data.fault.head.fault_id/*故障标识符*/, i/*故障隔离序号*/, isolate->Data[i].icb/*要执行的开关逻辑号*/,
													isolate->Data[i].cb/*要执行的开关id*/, isolate->Data[i].cb_desc/*要执行的开关描述*/, it_map_fdir_task->second->task_data.fault.bAuto == 1/*自动*/? 4 : 3,
													is_controllable(psbob_point.id) ? 1 : 0, up_isolate);

											ExecSQL(sql);
										}
									}
								}
								else if (calc_ret == FDIR_FAIL_ISOLATE) //故障定位成功后计算隔离方案失败,把定位信息写入表中
								{

								}

								//最小停电恢复方案计算成功，把恢复方案、隔离方案、定位信息写入表中
								if (calc_ret2 == FDIR_R_SUCCEED)
								{
									//写负荷转供步骤和负荷转供区段
									write_fhzg_step(it->task_data.fault.head.group_id, table_name, lst_steps2, map_fdir_task.begin()->first, lst_able_res, list<long>(), false);

									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task_temp = map_fdir_task.begin(); it_map_fdir_task_temp != map_fdir_task.end(); it_map_fdir_task_temp++)
									{
										//写上游恢复区域设备
//								write_fault_up_dev(it_map_fdir_task_temp->second->task_data.fault.head.fault_id, table_name, it_map_fdir_task_temp->first, false, lst_able_res);

										//写故障隔离步骤表
										PSBOB::CB psbob_point = { 0 };
										const TTable<CLS_FDIR_ISOLATE>* isolate = it_map_fdir_task_temp->first->getisolate2();
										int i = 0;
										for (i = 0; i < isolate->GetCount(); i++)
										{
											int up_isolate = 0;
											if (isolate->Data[i].q & FDIR_ISOLATE_NO_SO)
											{
												TRACE("隔离开关%ld下游无可转供电源，不隔离！\r\n", isolate->Data[i].cb);
												continue; //下游不可恢复的开关不需要隔离
											}
											if (isolate->Data[i].q & FDIR_ISOLATE_UP)
											{
												up_isolate = 1;
											}

											bzero(&psbob_point, sizeof(psbob_point));

											if (oodbread_rk(&psbob_point, &isolate->Data[i].cb, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_point)) <= 0)
											{
												TRACE("读取PSBOB库POINT表ID为%ld的记录失败，错误号：%d\r\n", isolate->Data[i].cb, _oodb_errno);
											}
											sprintf(sql,
													"Insert into PMS_PARAM.FDIR_ISOLATE%s (ID, STEP, ICB, CB, CB_DESCR, IMODE, CTRL_TYPE, CONTROL, UP_ISOLATE) Values (\'%s\', %d, %d, \'%ld\', \'%s\', %d, 1, %d, %d)",
													table_name, it_map_fdir_task_temp->second->task_data.fault.head.fault_id/*故障标识符*/, i/*故障隔离序号*/, isolate->Data[i].icb/*要执行的开关逻辑号*/,
													isolate->Data[i].cb/*要执行的开关id*/, isolate->Data[i].cb_desc/*要执行的开关描述*/, it_map_fdir_task_temp->second->task_data.fault.bAuto == 1/*自动*/? 4 : 3,
													is_controllable(psbob_point.id) ? 1 : 0, up_isolate);

											ExecSQL(sql);
										}
									}
								}
								else if (calc_ret2 == FDIR_FAIL_RESTORE) //恢复方案计算失败，把隔离和定位信息写入表中
								{
									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task = map_fdir_task.begin(); it_map_fdir_task != map_fdir_task.end(); it_map_fdir_task++)
									{
										//写上游恢复区段表
										//获取上游要恢复的区段号
//								write_fault_up_dev(it_map_fdir_task->second->task_data.fault.head.fault_id, table_name, it_map_fdir_task->first, false, lst_able_res);

										//写故障隔离步骤表
										PSBOB::CB psbob_point = { 0 };
										const TTable<CLS_FDIR_ISOLATE>* isolate = it_map_fdir_task->first->getisolate2();
										int i = 0;
										for (i = 0; i < isolate->GetCount(); i++)
										{
											int up_isolate = 0;
											if (isolate->Data[i].q & FDIR_ISOLATE_NO_SO)
											{
												TRACE("隔离开关%ld下游无可转供电源，不隔离！\r\n", isolate->Data[i].cb);
												continue; //下游不可恢复的开关不需要隔离
											}
											if (isolate->Data[i].q & FDIR_ISOLATE_UP)
											{
												up_isolate = 1;
											}

											bzero(&psbob_point, sizeof(psbob_point));

											if (oodbread_rk(&psbob_point, &isolate->Data[i].cb, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_point)) <= 0)
											{
												TRACE("读取PSBOB库POINT表ID为%ld的记录失败\r\n", isolate->Data[i].cb);
											}
											sprintf(sql,
													"Insert into PMS_PARAM.FDIR_ISOLATE%s (ID, STEP, ICB, CB, CB_DESCR, IMODE, CTRL_TYPE, CONTROL, UP_ISOLATE) Values (\'%s\', %d, %d, \'%ld\', \'%s\', %d, 1, %d, %d)",
													table_name, it_map_fdir_task->second->task_data.fault.head.fault_id/*故障标识符*/, i/*故障隔离序号*/, isolate->Data[i].icb/*要执行的开关逻辑号*/,
													isolate->Data[i].cb/*要执行的开关id*/, isolate->Data[i].cb_desc/*要执行的开关描述*/, it_map_fdir_task->second->task_data.fault.bAuto == 1/*自动*/? 4 : 3,
													is_controllable(psbob_point.id) ? 1 : 0, up_isolate);

											ExecSQL(sql);
										}
									}
								}
								else if (calc_ret == FDIR_FAIL_ISOLATE) //故障定位成功后计算隔离方案失败,把定位信息写入表中
								{

								}

								lst_able_res.sort();
								lst_able_res.unique();
								EraseSameInfo(lst_group_ld, lst_able_res);
								//写入故障后可恢复的负荷和不可恢复的负荷
								write_res_and_disableres_ld(it->task_data.fault.head.group_id, lst_group_ld, lst_able_res);

								delete table_name;
								for (map<FDIR_R2*, FDIR_TASK*>::iterator it_fdir_task = map_fdir_task.begin(); it_fdir_task != map_fdir_task.end(); it_fdir_task++)
								{
									it_fdir_task->second->task_data.fault.next_step = TASK_FAULT_STATUS_ISO;
								}

								//向人机发送故障隔离恢复事件
								if (it->task_data.fault.bAuto == 0)
								{
									if (version == VERSION_REALTIME)
									{
										//D5000向人机发送故障隔离恢复事件
										Notify(it->task_data.fault.head.group_id, it->task_data.fault.dv, it->task_data.fault.st);
//								FDIR_MSG_ISO_REC fdata = { 0 };
//
//								strcpy(fdata.fault_id, it->task_data.fault.head.group_id);
//								strcpy(fdata.st_id, "INSERT");
//
//								printf("send_event(%d) group_id:%s,return %d\r\n", EVPS_FDIR_MSG_ISO_REC, fdata.fault_id, bob_inv->send_event(EVPS_FDIR_MSG_ISO_REC, (char *) &fdata, sizeof(fdata)));

										//计算完成后把故障从任务列表的内存中删除，在执行的时候再把内存数据恢复
										//允许超时转自动的情况得保留故障在内存中，否则无法判断是否超时
										if (!g_FdirConfig.USER2AUTO)
										{
											RemoveTasks(it->task_data.fault.head.group_id);
											it = g_lstTask.begin();
										}
									}
										//研究态时写完库，在接收报文的地方释放内存，为了更新模拟故障机器名
									else if (version == VERSION_STUDY)
									{
									}

								}
								else
								{
									if (g_FdirConfig.AUTO_DEAL_HIS == 0)
									{
										FormFdirStep(it->task_data.fault.head.group_id);

										list<CB_CONTROL> lst_cb;
										if (GetLeftStep(it->task_data.fault.head.group_id, lst_cb) > 0)
										{
											AddControlItem(it->task_data.fault.head.group_id, lst_cb.begin()->point_id, lst_cb.begin()->bOpen, lst_cb.begin()->stage);
										}
									}
									else //自动归档
									{

									}
								}

								break;
							}


							case TASK_FAULT_STATUS_RE_CALC:
							{

								FDIR_TASK_LIST::iterator itTemp;
								list<FDIR_R2*> lst_fdir_r2;
								map<FDIR_R2*, FDIR_TASK*> map_fdir_task;

								TRACE("开始计算组号为%s的操作方案，该组的故障有：\r\n", it->task_data.fault.head.group_id);

								for (itTemp = g_lstTask.begin(); itTemp != g_lstTask.end(); itTemp++)
								{
									if (itTemp->rq_type != TASK_TYPE_FAULT)
										continue;

									if (strcmp(itTemp->task_data.fault.head.group_id, it->task_data.fault.head.group_id) == 0)
									{
										lst_fdir_r2.push_back(itTemp->task_data.fault.fdir_r);
										map_fdir_task[itTemp->task_data.fault.fdir_r] = &(*itTemp);

										itTemp->task_data.fault.next_step = TASK_FAULT_STATUS_CALC_STEP;

										//后台自动执行失败后转为前台人工执行
										itTemp->task_data.fault.bAuto = 0;
										cout << it->task_data.fault.head.group_id << "->" << itTemp->task_data.fault.head.fault_id << endl;
									}
								}

								list<list<MAP_SO_NODE> > lst_steps;

								//判断执行失败的开关是否是上游隔离开关，如果是上游隔离开关，上游恢复区域失效
								bool up_iso_fail = false;
								const TTable<CLS_FDIR_ISOLATE>* old_isolate = it->task_data.fault.fdir_r->getisolate();

								for (int i = 0; i < old_isolate->GetCount(); i++)
								{
									if (old_isolate->Data[i].q & FDIR_ISOLATE_UP)
									{
										if (old_isolate->Data[i].cb == it->task_data.fault.fail_point)
										{
											up_iso_fail = true;
										}
									}
								}

								////故障组中由于隔离或者跳闸导致失电的负荷，最终保存非故障不可恢复的负荷
								list<FAULT_ITEM> re_lst_group_ld;
								////故障组中由于隔离或者跳闸导致失电，存在恢复方案，可以恢复的负荷
								list<FAULT_ITEM> re_lst_able_res;
								it->task_data.fault.fdir_r->Init();
								if (re_calc_restore_step(it->task_data.fault.head.group_id, lst_fdir_r2, lst_steps) == FDIR_R_SUCCEED)
								{
									//删除旧的恢复方案
									sprintf(sql, "DELETE FROM FHZG_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_SO_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_STEP WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_SO WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_CUT_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_LOAD WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "UPDATE FDIR_FDIRECT SET USERID=NULL,MACHINE=NULL WHERE GROUPID=\'%s\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									//删除选定的执行方案中未执行的步骤
									sprintf(sql, "DELETE FROM FDIR_STEP WHERE ID=\'%s\' AND STATUS=%d", it->task_data.fault.head.group_id, STATUS_FDIR_STEP_WAIT);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FDIR_CB_STATUS WHERE ID=\'%s\' AND ISFAULTDV=0", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FDIR_DISABLE_RES WHERE ID=\'%s\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FDIR_ENABLE_RES WHERE ID=\'%s\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);
									//string fault_cb = it->task_data.fault.fdir_r->getfaultcb();

									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_fdir_r = map_fdir_task.begin(); it_fdir_r != map_fdir_task.end(); it_fdir_r++)
									{
										re_lst_group_ld.splice(re_lst_group_ld.end(), it_fdir_r->first->m_ld);
										if (up_iso_fail)
										{
											sprintf(sql, "DELETE FROM FDIR_UP_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
											ExecSQL(sql);

											string manual_restore;

											for (list<long>::iterator it_point = it_fdir_r->second->task_data.fault.fdir_r->lst_restore.begin();
												 it_point != it_fdir_r->second->task_data.fault.fdir_r->lst_restore.end(); it_point++)
											{
												PSBOB::CB point = { 0 };

												char tmp[50];

												sprintf(tmp, "%ld", *it_point);

												manual_restore += tmp;

												if (is_controllable(*it_point))
													manual_restore += "[0];";
												else
													manual_restore += ";";
											}

											//更新上游开关
											sprintf(sql, "update FDIR_FDIRECT set MANUAL_RESTORE = \'%s\' where id = \'%s\'", manual_restore.c_str(), it_fdir_r->second->task_data.fault.head.fault_id);
											ExecSQL(sql);

											//have_up_iso标志故障区域是否有上游隔离开关，如果没上游隔离，则上游没有可以恢复的区域
											bool have_up_iso = false;
											const TTable<CLS_FDIR_ISOLATE>* new_isolate = it_fdir_r->first->getisolate();

											for (int i = 0; i < new_isolate->GetCount(); i++)
											{
												if (new_isolate->Data[i].q & FDIR_ISOLATE_UP)
												{
													have_up_iso = true;
												}
											}
											if (have_up_iso)
											{
												write_fault_up_dev(it->task_data.fault.head.group_id, "", it_fdir_r->first, true, re_lst_able_res);
											}
										}
									}
									re_lst_group_ld.sort();
									re_lst_group_ld.unique();

									//写入新的恢复方案
									write_fhzg_step(it->task_data.fault.head.group_id, "", lst_steps, map_fdir_task.begin()->first, re_lst_able_res, list<long>(), true);

									EraseSameInfo(re_lst_group_ld, re_lst_able_res);
									//写入故障后可恢复的负荷和不可恢复的负荷
									write_res_and_disableres_ld(it->task_data.fault.head.group_id, re_lst_group_ld, re_lst_able_res);
									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task = map_fdir_task.begin(); it_map_fdir_task != map_fdir_task.end(); it_map_fdir_task++)
									{
										sprintf(sql, "DELETE FROM FDIR_ISOLATE WHERE ID=\'%s\' and STATUS in(%d,%d)  and ctrl_type=0", it_map_fdir_task->second->task_data.fault.head.fault_id, STATUS_ISOLATE_FAIL,
												STATUS_ISOLATE_WAIT);
										ExecSQL(sql);

										sprintf(sql, "select count(cb) from fdir_isolate where id=\'%s\'", it_map_fdir_task->second->task_data.fault.head.fault_id);
										char *buf = NULL;
										int rec_num, attr_num;
										struct ORA_ATTR *attrs = NULL;
										int step_num = 0;

										pthread_mutex_lock(&oci_mutex);
										int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
										pthread_mutex_unlock(&oci_mutex);

										if (ret != OCI_ERROR)
										{
											step_num = *(int*)(buf);
										}
										g_oci->Readdata_Free();
										//写故障隔离步骤表
										PSBOB::CB psbob_point = { 0 };
										const TTable<CLS_FDIR_ISOLATE>* isolate = it_map_fdir_task->first->getisolate();
										int i = 0;
										for (i = 0; i < isolate->GetCount(); i++)
										{
											int up_isolate = 0;
											if (isolate->Data[i].q & FDIR_ISOLATE_NO_SO)
											{
												TRACE("隔离开关%ld下游无可转供电源，不隔离！\r\n", isolate->Data[i].cb);
												continue; //下游不可恢复的开关不需要隔离
											}
											if (isolate->Data[i].q & FDIR_ISOLATE_UP)
											{
												up_isolate = 1;
											}
											bzero(&psbob_point, sizeof(psbob_point));

											if (oodbread_rk(&psbob_point, &isolate->Data[i].cb, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_point)) <= 0)
											{
												TRACE("读取PSBOB库POINT表ID为%ld的记录失败\r\n", isolate->Data[i].cb);
											}
											sprintf(sql,
													"Insert into PMS_PARAM.FDIR_ISOLATE (ID, STEP, ICB, CB, CB_DESCR, IMODE, CTRL_TYPE, CONTROL, UP_ISOLATE) Values (\'%s\', %d, %d, \'%ld\', \'%s\', %d, 0, %d, %d)",
													it_map_fdir_task->second->task_data.fault.head.fault_id/*故障标识符*/, step_num + i/*故障隔离序号*/, isolate->Data[i].icb/*要执行的开关逻辑号*/,
													isolate->Data[i].cb/*要执行的开关id*/, isolate->Data[i].cb_desc/*要执行的开关描述*/, it_map_fdir_task->second->task_data.fault.bAuto == 1/*自动*/? 4 : 3,
													is_controllable(psbob_point.id) ? 1 : 0, up_isolate);

											ExecSQL(sql);
										}
									}
								}

								//向人机发送故障隔离恢复事件
								if (it->task_data.fault.bAuto == 0)
								{
									if (version == VERSION_REALTIME)
									{
										//D5000向人机发送故障隔离恢复事件
										Notify(it->task_data.fault.head.group_id, it->task_data.fault.dv, it->task_data.fault.st);
//								FDIR_MSG_ISO_REC fdata = { 0 };
//
//								strcpy(fdata.fault_id, it->task_data.fault.head.group_id);
//								strcpy(fdata.st_id, "UPDATE");
//
//								printf("send_event(%d) group_id:%s,return %d\r\n", EVPS_FDIR_MSG_ISO_REC, fdata.fault_id, bob_inv->send_event(EVPS_FDIR_MSG_ISO_REC, (char *) &fdata, sizeof(fdata)));
									}

									//计算完成后把故障从任务列表的内存中删除，在执行的时候再把内存数据恢复
									RemoveTasks(it->task_data.fault.head.group_id);
									it = g_lstTask.begin();
								}
								else
								{
									if (g_FdirConfig.AUTO_DEAL_HIS == 0)
									{
										FormFdirStep(it->task_data.fault.head.group_id);

										list<CB_CONTROL> lst_cb;
										if (GetLeftStep(it->task_data.fault.head.group_id, lst_cb) > 0)
										{
											AddControlItem(it->task_data.fault.head.group_id, lst_cb.begin()->point_id, lst_cb.begin()->bOpen, lst_cb.begin()->stage);
										}
									}

								}

								break;
							}

							case TASK_FAULT_STATUS_INVALID_RECALC:
							{
								FDIR_TASK_LIST::iterator itTemp;
								list<FDIR_R2*> lst_fdir_r2;
								map<FDIR_R2*, FDIR_TASK*> map_fdir_task;

								TRACE("开始计算组号为%s的操作方案，该组的故障有：\r\n", it->task_data.fault.head.group_id);

								for (itTemp = g_lstTask.begin(); itTemp != g_lstTask.end(); itTemp++)
								{
									if (itTemp->rq_type != TASK_TYPE_FAULT)
										continue;

									if (strcmp(itTemp->task_data.fault.head.group_id, it->task_data.fault.head.group_id) == 0)
									{
										lst_fdir_r2.push_back(itTemp->task_data.fault.fdir_r);
										map_fdir_task[itTemp->task_data.fault.fdir_r] = &(*itTemp);

										itTemp->task_data.fault.next_step = TASK_FAULT_STATUS_CALC_STEP;

										//后台自动执行失败后转为前台人工执行
										itTemp->task_data.fault.bAuto = 0;
										cout << it->task_data.fault.head.group_id << "->" << itTemp->task_data.fault.head.fault_id << endl;
									}
								}

								list<list<MAP_SO_NODE> > lst_steps;
								////故障组中由于隔离或者跳闸导致失电的负荷，最终保存非故障不可恢复的负荷
								list<FAULT_ITEM> in_re_lst_group_ld;
								////故障组中由于隔离或者跳闸导致失电，存在恢复方案，可以恢复的负荷
								list<FAULT_ITEM> in_re_lst_able_res;
								it->task_data.fault.fdir_r->Init();
								if (re_calc_restore_step(it->task_data.fault.head.group_id, lst_fdir_r2, lst_steps) == FDIR_R_SUCCEED)
								{
									//删除旧的恢复方案
									sprintf(sql, "DELETE FROM FHZG_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_SO_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_STEP WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_SO WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_CUT_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_LOAD WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FHZG_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "UPDATE FDIR_FDIRECT SET USERID=NULL,MACHINE=NULL,STATUS_ISOLATE=-1,STATUS_FHZG=-1,STATUS_RESTORE=-1 WHERE GROUPID=\'%s\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FDIR_CB_STATUS WHERE ID=\'%s\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									//删除选定的执行方案中未执行的步骤
									sprintf(sql, "DELETE FROM FDIR_STEP WHERE ID=\'%s\' AND STATUS=%d", it->task_data.fault.head.group_id, STATUS_FDIR_STEP_WAIT);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FDIR_DISABLE_RES WHERE ID=\'%s\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									sprintf(sql, "DELETE FROM FDIR_ENABLE_RES WHERE ID=\'%s\'", it->task_data.fault.head.group_id);
									ExecSQL(sql);

									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_fdir_r = map_fdir_task.begin(); it_fdir_r != map_fdir_task.end(); it_fdir_r++)
									{

										in_re_lst_group_ld.splice(in_re_lst_group_ld.end(), it_fdir_r->first->m_ld);
										sprintf(sql, "DELETE FROM FDIR_UP_ZONE WHERE ID LIKE \'%s%%\'", it->task_data.fault.head.group_id);
										ExecSQL(sql);

										string manual_restore;

										for (list<long>::iterator it_point = it_fdir_r->second->task_data.fault.fdir_r->lst_restore.begin();
											 it_point != it_fdir_r->second->task_data.fault.fdir_r->lst_restore.end(); it_point++)
										{
											char tmp[50];

											sprintf(tmp, "%ld", *it_point);

											manual_restore += tmp;

											if (is_controllable(*it_point))
												manual_restore += "[0];";
											else
												manual_restore += ";";
										}

										//更新上游开关
										sprintf(sql, "update FDIR_FDIRECT set MANUAL_RESTORE = \'%s\' where id = \'%s\'", manual_restore.c_str(), it_fdir_r->second->task_data.fault.head.fault_id);
										ExecSQL(sql);

										write_fault_up_dev(it->task_data.fault.head.group_id, "", it_fdir_r->first, false, in_re_lst_able_res);

										//保存故障馈线开关信息，做数据校验用
										write_fault_tree(it->task_data.fault.head.group_id, it_fdir_r->first);

									}

									in_re_lst_group_ld.sort();
									in_re_lst_group_ld.unique();
									//写入新的恢复方案
									write_fhzg_step(it->task_data.fault.head.group_id, "", lst_steps, map_fdir_task.begin()->first, in_re_lst_able_res, list<long>(), true);

									EraseSameInfo(in_re_lst_group_ld, in_re_lst_able_res);
									//写入故障后可恢复的负荷和不可恢复的负荷
									write_res_and_disableres_ld(it->task_data.fault.head.group_id, in_re_lst_group_ld, in_re_lst_able_res);
									for (map<FDIR_R2*, FDIR_TASK*>::iterator it_map_fdir_task = map_fdir_task.begin(); it_map_fdir_task != map_fdir_task.end(); it_map_fdir_task++)
									{
										sprintf(sql, "DELETE FROM FDIR_ISOLATE WHERE ID=\'%s\' and STATUS in(%d,%d,%d) and ctrl_type=0", it_map_fdir_task->second->task_data.fault.head.fault_id, STATUS_ISOLATE_FAIL,
												STATUS_ISOLATE_WAIT, STATUS_ISOLATE_EXEC);
										ExecSQL(sql);

										sprintf(sql, "select count(cb) from fdir_isolate where id=\'%s\'", it_map_fdir_task->second->task_data.fault.head.fault_id);
										char *buf = NULL;
										int rec_num, attr_num;
										struct ORA_ATTR *attrs = NULL;
										int step_num = 0;

										pthread_mutex_lock(&oci_mutex);
										int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
										pthread_mutex_unlock(&oci_mutex);

										if (ret != OCI_ERROR)
										{
											step_num = *(int*)(buf);
										}
										g_oci->Readdata_Free();

										//写故障隔离步骤表
										const TTable<CLS_FDIR_ISOLATE>* isolate = it_map_fdir_task->first->getisolate();
										int i = 0;
										for (i = 0; i < isolate->GetCount(); i++)
										{
											int up_isolate = 0;
											if (isolate->Data[i].q & FDIR_ISOLATE_NO_SO)
											{
												TRACE("隔离开关%ld下游无可转供电源，不隔离！\r\n", isolate->Data[i].cb);
												continue; //下游不可恢复的开关不需要隔离
											}
											if (isolate->Data[i].q & FDIR_ISOLATE_UP)
											{
												up_isolate = 1;
											}

											sprintf(sql,
													"Insert into PMS_PARAM.FDIR_ISOLATE (ID, STEP, ICB, CB, CB_DESCR, IMODE, CTRL_TYPE, CONTROL, UP_ISOLATE) Values (\'%s\', %d, %d, \'%ld\', \'%s\', %d, 0, %d, %d)",
													it_map_fdir_task->second->task_data.fault.head.fault_id/*故障标识符*/, step_num + i/*故障隔离序号*/, isolate->Data[i].icb/*要执行的开关逻辑号*/,
													isolate->Data[i].cb/*要执行的开关id*/, isolate->Data[i].cb_desc/*要执行的开关描述*/, it_map_fdir_task->second->task_data.fault.bAuto == 1/*自动*/? 4 : 3,
													is_controllable(isolate->Data[i].cb) ? 1 : 0, up_isolate);

											ExecSQL(sql);
										}
									}
								}

								//向人机发送故障隔离恢复事件
								if (it->task_data.fault.bAuto == 0)
								{
									if (version == VERSION_REALTIME)
									{
										//D5000向人机发送故障隔离恢复事件
										Notify(it->task_data.fault.head.group_id, it->task_data.fault.dv, it->task_data.fault.st);
//								FDIR_MSG_ISO_REC fdata = { 0 };
//
//								strcpy(fdata.fault_id, it->task_data.fault.head.group_id);
//								strcpy(fdata.st_id, "UPDATE");
//
//								printf("send_event(%d) group_id:%s,return %d\r\n", EVPS_FDIR_MSG_ISO_REC, fdata.fault_id, bob_inv->send_event(EVPS_FDIR_MSG_ISO_REC, (char *) &fdata, sizeof(fdata)));
									}

									//计算完成后把故障从任务列表的内存中删除，在执行的时候再把内存数据恢复
									RemoveTasks(it->task_data.fault.head.group_id);
									it = g_lstTask.begin();
								}
								else
								{
									if (g_FdirConfig.AUTO_DEAL_HIS == 0)
									{
										FormFdirStep(it->task_data.fault.head.group_id);

										list<CB_CONTROL> lst_cb;
										if (GetLeftStep(it->task_data.fault.head.group_id, lst_cb) > 0)
										{
											AddControlItem(it->task_data.fault.head.group_id, lst_cb.begin()->point_id, lst_cb.begin()->bOpen, lst_cb.begin()->stage);
										}
									}

								}

								break;
							}

							default:
								break;
						}

						break;
					}

						//转自动
					case TASK_TYPE_SET_AUTO:
						break;

					default:
						break;
				}
			}
		}
		pthread_mutex_unlock(&task_list_mutex);
		pthread_mutex_unlock(&simu_sync_mutex);

		sleep(1);
	}
}

//根据遥信ID获取该开关的目标开合状态
//-1: 获取失败 1: 开 0: 合
int get_op_type(const long id)
{
	assert(id > 0);

	int op_type = -1;

	pthread_mutex_lock(&task_list_mutex);

	for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
	{
		if (it->rq_type != TASK_TYPE_CB_CTRL)
			continue;

		if ((it->task_data.control.point_name == id)
			&& (TASK_CTL_STATUS_RUN == it->task_data.control.status))
		{
			op_type = it->task_data.control.bOpen ? 1 : 0;
			break;
		}
	}

	pthread_mutex_unlock(&task_list_mutex);

	return op_type;
}

//根据遥信ID获取该开关遥控所属的故障ID
const char * get_group_id(const long id)
{
	assert(id > 0);

	pthread_mutex_lock(&task_list_mutex);

	for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
	{
		if (it->rq_type != TASK_TYPE_CB_CTRL)
			continue;

		if ((it->task_data.control.point_name == id) &&
			((TASK_CTL_STATUS_RUN == it->task_data.control.status) || (TASK_CTL_STATUS_FINISH == it->task_data.control.status)))
		{
			pthread_mutex_unlock(&task_list_mutex);
			return it->task_data.control.head.group_id;
		}
	}

	pthread_mutex_unlock(&task_list_mutex);

	return NULL;
}

void thread_recv_rte(void *param)
{
	//D5000 这个函数需要根据D5000的故障事件重写
	int len = 0;
	const int sca_breaker_table_no = 407;
	KEY_STRU ks = { 0 };
	PSBOB::ST psbob_st = { 0 };
	PSBOB::DV psbob_dv = { 0 };
	PSBOB::CB psbob_cb = { 0 };
	FDIR_POINT_TRIG data_trig = { 0 };
	FDIR_TASK_LIST::iterator it;

	///////////////////
	///开始循环接收RTE事件
	while (true)
	{
		Message msg;

		len = bob_inv->messageReceive(&msg, NULL, 0);

		if (len < 0)
		{
			cout << "messageReceive fail!" << endl;
			pthread_exit(0);
			return;
		}
		else if (len == 0)
		{
			usleep(10);
			continue;
		}
		else
		{
			GetFdirConfig(g_FdirConfig);

			switch (msg.header.event)
			{
				case WARN_INFORM_TYPE:
				{
					break;
				}
				case MT_YX_CHANGE:
				{
//				int cnt = msg.header.len / sizeof(ChangeYx);
//				int pos = 1;
//
//				ChangeYx *yx = (ChangeYx *) msg.Msg_buf;
//
//				do
//				{
//					cout << "MT_YX_CHANGE,id:" << yx->keyid << endl;
//					///////////////////首先判断是否遥控返回////////////////
//					pthread_mutex_lock(&task_list_mutex);
//					FDIR_TASK_LIST::iterator it = g_lstTask.begin();
//					bool breturn = false;	//是否遥控返回项
//
//					for (; it != g_lstTask.end(); it++)
//					{
//						if (it->rq_type == TASK_TYPE_CB_CTRL) //为遥控项
//						{
//							if (it->task_data.control.point_name == yx->keyid)
//							{
//								breturn = true;
//								printf("遥控%ld返回报警事件：", yx->keyid);
//
//								cout << "成功！" << endl;
//
//								ControlSucceed(it->task_data.control.head.group_id, yx->keyid, it->task_data.control.stage);
//
//								list<CB_CONTROL> lstCB;
//								FDIR_TASK_LIST::iterator it_fault;
//								char fault_id[FAULT_ID_LENGTH] = { 0 };
//
//								strcpy(fault_id, it->task_data.control.head.group_id);
//
//								if (GetLeftStep(fault_id, lstCB) == 0)
//								{
//									ClearFdirDB(fault_id);
//									sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, fault_id);
//									if (ExecSQL(sql) != OCI_ERROR)
//									{
//										sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, fault_id);
//										ExecSQL(sql);
//									}
//
//									RemoveTasks(fault_id);
//									printf("故障组%s处理完成！！！！\r\n", fault_id);
//								}
//								else
//								{
//									for (it_fault = g_lstTask.begin(); it_fault != g_lstTask.end(); it_fault++)
//									{
//										if (it_fault->rq_type == TASK_TYPE_FAULT)
//										{
//											if (strcmp(it_fault->task_data.fault.head.group_id, it->task_data.control.head.group_id) == 0)
//											{
//												if (it_fault->task_data.fault.bAuto)
//													AddControlItem(fault_id, lstCB.begin()->point_id, lstCB.begin()->bOpen, lstCB.begin()->stage);
//												else
//												{
//													printf("遥控%ld所在故障组%s非自动执行模式！！\r\n", it->task_data.control.point_name, it->task_data.control.head.group_id);
//												}
//
//												break;
//											}
//										}
//									}
//
//									if (it_fault == g_lstTask.end())
//									{
//										printf("未找到遥控%ld所在故障组%s对应的故障项，无法判断其是否为自动执行模式！！！\r\n", it->task_data.control.point_name, it->task_data.control.head.group_id);
//									}
//								}
//
//								g_lstTask.erase(it);
//
//								break;
//							}
//						}
//					}
//
//					if (breturn)
//					{
//						pthread_mutex_unlock(&task_list_mutex);
//						break;
//					}
//
//					pthread_mutex_unlock(&task_list_mutex);
//
//					yx++;
//					pos++;
//				} while (pos < cnt);
//
					break;
				}
				case SCADA_OPT_PREV_ANSWER:
				{
					TScaOpAnswer *tsa = (TScaOpAnswer*)msg.Msg_buf;

					TRACE("key_id:%ld, time:%ld, ctrl_result:%i\r\n", tsa->key_id, tsa->time, tsa->ctrl_result);

					if(tsa->ctrl_result == 0){
						TRACE("遥控返校成功！\r\n");

						int op_type = get_op_type(tsa->key_id);

						switch(op_type){
							case 0:
							case 1:
								DoControl(tsa->key_id, false, op_type);
								break;

							default:
								TRACE("不是期望的遥控返回项！\r\n");
								break;
						}
					}
					break;
				}
				case MT_YK_REPLY:
				case MT_TAP_REPLY:
				{
//				TRACE("遥控telbob返校事件！\r\n");
					break;
				}
				case APP_TO_WARN_SERVICE_TYPE:
				{
					APP_TO_WARN_SERVICE_MESSAGE_STRU r_Dev;

					M_DECODE(r_Dev, msg.Msg_buf, msg.header.len);

					int len = r_Dev.seq_warn_message.length();

					for (int i = 0; i < len; i++)
					{
						TRACE("--------------------warntype:%d,app:%d\r\n", r_Dev.seq_warn_message[i].warn_type, r_Dev.seq_warn_message[i].app_no);
						if (r_Dev.seq_warn_message[i].warn_type == D_YX_BW_WARN_TYPE
							/*||	r_Dev.seq_warn_message[i].warn_type == YX_BW_WARN_TYPE*/
							|| r_Dev.seq_warn_message[i].warn_type == D_SG_WARN_TYPE
							/*|| r_Dev.seq_warn_message[i].warn_type == SG_WARN_TYPE*/)		//平台报警有问题，暂时报不上D_SG_WARN_TYPE事件，先收SG_WARN_TYPE做容错
						{
							if(r_Dev.seq_warn_message[i].app_no != AP_DSCADA) continue;

							data_trig.alarm.type = r_Dev.seq_warn_message[i].warn_type;

							data_trig.alarm.occur_time = r_Dev.seq_warn_message[i].seq_field_info[0].c_time();
							data_trig.alarm.msec = r_Dev.seq_warn_message[i].seq_field_info[1].c_short();
							data_trig.alarm.key_id = r_Dev.seq_warn_message[i].seq_field_info[2].c_long();
							data_trig.alarm.respid = r_Dev.seq_warn_message[i].seq_field_info[3].c_int();
							data_trig.alarm.st_id = r_Dev.seq_warn_message[i].seq_field_info[4].c_long();
							data_trig.alarm.bayid = r_Dev.seq_warn_message[i].seq_field_info[5].c_long();
							data_trig.alarm.status = r_Dev.seq_warn_message[i].seq_field_info[6].c_int();
							data_trig.alarm.str = r_Dev.seq_warn_message[i].seq_field_info[7].c_string();
							data_trig.alarm.bvid = r_Dev.seq_warn_message[i].seq_field_info[8].c_long();

							if (data_trig.alarm.msec == -1)
							{
								//研究态
								data_trig.trig_type = TRIG_TYPE_SIMU;
							}

							if (GetDv(data_trig.alarm.st_id, "st", psbob_dv) == OO_FAIL)
							{
								TRACE("获取厂站%ld的馈线信息失败！\r\n", data_trig.alarm.st_id);
								continue;
							}

							data_trig.dv_id = psbob_dv.id;
							data_trig.damode = GetDvDamode(psbob_dv.id);
							gettimeofday(&data_trig.tm_add, NULL);

							if (data_trig.damode <= FDIR_MODE_FORBID)
							{
								TRACE("馈线%ld(%s)为禁止DA处理(%d)\r\n", psbob_dv.id, psbob_dv.name, psbob_dv.damode);
								continue;
							}

							KEY_STRU ks = { 0 };

							CCommon::long_to_keyid(data_trig.alarm.key_id, &ks);
							ks.field_id = 0;
							CCommon::keyid_to_long(&ks, &data_trig.alarm.key_id);

							TB_DESCR *tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB(ks.table_no));

							if (ks.table_no == g_db_psbob->GetTB("breaker")->table_id)
							{
								data_trig.alarm.dev = ALARM_BREAKER;

								if (oodbread_rk(&data_trig.psbob_data.point, &data_trig.alarm.key_id, tb_descr, sizeof(data_trig.psbob_data.point)) == OO_FAIL)
								{
									TRACE("读取开关表关键字为%ld的记录失败！\r\n", data_trig.alarm.key_id);
									continue;
								}
								else
								{
									if (MENU_WARN_YX_BW_ON == data_trig.alarm.status)
									{
										TRACE("开关[%s]合闸(%d)\r\n", data_trig.psbob_data.point.name, data_trig.alarm.status);
									}
									else
									{
										if(r_Dev.seq_warn_message[i].warn_type == D_SG_WARN_TYPE
											/*||r_Dev.seq_warn_message[i].warn_type == SG_WARN_TYPE*/)
										{
											TRACE("开关[%s]事故跳闸(%d)\r\n", data_trig.psbob_data.point.name, data_trig.alarm.status);
										}
										else
										{
											TRACE("开关[%s]分闸(%d)\r\n", data_trig.psbob_data.point.name, data_trig.alarm.status);
										}

										data_trig.alarm.name = data_trig.psbob_data.point.name;

										//将该开关加入过流开关列表
										pthread_mutex_lock(&trig_list_mutex);
										g_lstTrig.push_back(data_trig);
										pthread_mutex_unlock(&trig_list_mutex);
									}

								}
							}
							else if (ks.table_no == g_db_psbob->GetTB("relaysig")->table_id)
							{
								PSBOB::PROTECT protect = { 0 };

								if (oodbread_rk(&protect, &data_trig.alarm.key_id, tb_descr, sizeof(protect)) == OO_FAIL)
								{
									TRACE("读取保护信号表关键字为%ld的记录失败！\r\n", data_trig.alarm.key_id);
									continue;
								}
								else
								{
									// 暂时修改读取保护信号策略，增加动作信号
									if (HasMask(tb_descr, "pnt_type", protect.pnt_type , MENU_TYPE_RELAY_SGZ) || HasMask(tb_descr, "pnt_type", protect.pnt_type , MENU_TYPE_RELAY_ACT))
									{
										data_trig.alarm.dev = ALARM_SGZ;

										if (MENU_WARN_YX_ACT_ON == data_trig.alarm.status)
										{
											unsigned long id = 0;
											CCommon::long_to_keyid(protect.oo_dev, &ks);
											ks.field_id = 0;
											CCommon::keyid_to_long(&ks, &id);

											if (oodbread_rk(&data_trig.psbob_data.point, &id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(data_trig.psbob_data.point)) == OO_FAIL)
											{
												TRACE("读取开关表关键字为%ld的记录失败！\r\n", id);
												continue;
											}
											else
											{
												TRACE("事故总[%s]动作(%d)\r\n", protect.name, data_trig.alarm.status);

												data_trig.alarm.name = protect.name;

												//将该开关加入过流开关列表
												pthread_mutex_lock(&trig_list_mutex);
												g_lstTrig.push_back(data_trig);
												pthread_mutex_unlock(&trig_list_mutex);
											}
										}
										else
										{
											TRACE("事故总[%s]复归(%d)\r\n", protect.name, data_trig.alarm.status);
										}

									}
									else if(HasMask(tb_descr, "pnt_type", protect.pnt_type , MENU_TYPE_RELAY_JD)
											|| HasMask(tb_descr, "pnt_type", protect.pnt_type , MENU_TYPE_RELAY_DL))
									{
										data_trig.alarm.dev = HasMask(tb_descr, "pnt_type", protect.pnt_type, MENU_TYPE_RELAY_DL) ? ALARM_FAULTINFO_DL : ALARM_FAULTINFO_JD;

										if (MENU_WARN_YX_ACT_ON == data_trig.alarm.status)
										{
											unsigned long id = 0;
											CCommon::long_to_keyid(protect.oo_dev, &ks);
											ks.field_id = 0;
											CCommon::keyid_to_long(&ks, &id);

											if (oodbread_rk(&data_trig.psbob_data.faultinfo, &id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo")), sizeof(data_trig.psbob_data.faultinfo)) == OO_FAIL)
											{
												TRACE("读取故障指示器表关键字为%ld的记录失败！\r\n", id);
												continue;
											}
											else
											{
												TRACE("故障指示器[%s]动作(%d)\r\n", protect.name, data_trig.alarm.status);

												data_trig.alarm.name = protect.name;
												//将该开关加入过流开关列表
												pthread_mutex_lock(&trig_list_mutex);
												g_lstTrig.push_back(data_trig);
												pthread_mutex_unlock(&trig_list_mutex);
											}
										}
										else
										{
											TRACE("故障指示器[%s]复归(%d)\r\n", protect.name, data_trig.alarm.status);
										}
									}
									else
									{
										TRACE("%s(%d)\r\n", protect.name, protect.pnt_type);
									}
								}
							}
							else
							{
								TRACE("未处理的报警设备%s(%d)\r\n", tb_descr->table_name, tb_descr->table_id);
							}

							//TRACE("(%ld %d %ld %d %ld %ld %d %ld)\r\n", tmp_sec, tmp_msec, tmp_keyid, tmp_respid, tmp_facid, tmp_bayid, tmp_status, tmp_bvid);
						}
						else
						{
#if 0
						int lenVal = r_Dev.seq_warn_message[i].seq_field_info.length();

						for (int j = 0; j < lenVal; j++)
						{
							short datatype = r_Dev.seq_warn_message[i].seq_field_info[j]._d();

							switch (datatype)
							{
							case C_DATATYPE_LONG:
							{
								unsigned long kid = r_Dev.seq_warn_message[i].seq_field_info[j].c_long();

								TRACE("warn_type:%d, app_no:%d, key_id:%ld\r\n", r_Dev.seq_warn_message[i].warn_type, r_Dev.seq_warn_message[i].app_no, kid);
								KEY_STRU ks;
								CCommon::long_to_keyid(kid, &ks);

								if (ks.table_no == g_db_psbob->GetTB("measpoint")->table_id)
								{
									PSBOB::POINT point = { 0 };

									if (oodbread_rk(&point, &kid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("measpoint")), sizeof(point)) == OO_FAIL)
									{

									}
									else
									{
										TRACE("%s(%ld)遥信变位！\r\n", point.name, point.id);
									}
								}

								break;
							}

							case C_DATATYPE_INT:
								TRACE("warn_type:%d, app_no:%d, int:%d\r\n", r_Dev.seq_warn_message[i].warn_type, r_Dev.seq_warn_message[i].app_no,
										r_Dev.seq_warn_message[i].seq_field_info[j].c_int());
								break;

							case C_DATATYPE_SHORT:
								TRACE("warn_type:%d, app_no:%d, short:%d\r\n", r_Dev.seq_warn_message[i].warn_type, r_Dev.seq_warn_message[i].app_no,
										r_Dev.seq_warn_message[i].seq_field_info[j].c_short());
								break;

							case C_DATATYPE_UCHAR:
								TRACE("warn_type:%d, app_no:%d, uchar:%d\r\n", r_Dev.seq_warn_message[i].warn_type, r_Dev.seq_warn_message[i].app_no,
										r_Dev.seq_warn_message[i].seq_field_info[j].c_uchar());
								break;

							case C_DATATYPE_DATETIME:
								TRACE("warn_type:%d, app_no:%d, time:%ld\r\n", r_Dev.seq_warn_message[i].warn_type, r_Dev.seq_warn_message[i].app_no,
										r_Dev.seq_warn_message[i].seq_field_info[j].c_time());
								break;

							case C_DATATYPE_STRING:
								TRACE("warn_type:%d, app_no:%d, string:%ld\r\n", r_Dev.seq_warn_message[i].warn_type, r_Dev.seq_warn_message[i].app_no,
										r_Dev.seq_warn_message[i].seq_field_info[j].c_string());
								break;

							default:
								TRACE("unknow datatype:%d\r\n", datatype);
								break;
							}
						}
#endif
						}
					}
					break;
				}
					//可用alarm_simu_tool命令模拟
				case MT_FAULT_EVENT:
				{
					break;
#if 0
				TransEventPkg receive_mes;

				M_DECODE(receive_mes, msg.Msg_buf, msg.header.len);

				for (int i = 0; i < receive_mes.trans_head.data_num; i++)
				{
					cout << "MT_FAULT_EVENT:key_id:" << receive_mes.event_info[i].key_id << ",stid:" << receive_mes.event_info[i].st_id << endl;

					long key_id = receive_mes.event_info[i].key_id;
					CCommon::long_to_keyid(key_id, &ks);

					if (ks.table_no != sca_breaker_table_no)
					{
						TRACE("不是开关事件！\r\n");
						continue;
					}

					if (GetDv(key_id, "cb", psbob_dv) < 0)
					{
						TRACE("获取开关%ld的DV信息失败！\r\n", key_id);
						continue;
					}

					if (psbob_dv.damode == FDIR_MODE_FORBID)
					{
						TRACE("馈线%ld(%s)为禁止DA处理(%d)\r\n", psbob_dv.id, psbob_dv.name, psbob_dv.damode);
						continue;
					}

					if (oodbread_rk(&data_trig.psbob_data.point, &key_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) <= 0)
					{
						TRACE("读取开关%ld信息失败！\r\n", key_id);
						continue;
					}

					gettimeofday(&data_trig.tm_add, NULL);
					data_trig.damode = psbob_dv.damode;
					data_trig.dv_id = psbob_dv.id;
					data_trig.lst_fault.clear();
					memcpy(&data_trig.alarm, &receive_mes.event_info[i], sizeof(OneEventStru));
					data_trig.alarm.st_id = data_trig.psbob_data.point.st_id;


					//将该开关加入过流开关列表
					pthread_mutex_lock(&trig_list_mutex);
					g_lstTrig.push_back(data_trig);
					pthread_mutex_unlock(&trig_list_mutex);
				}

				break;
#endif
				}
				case MT_YC_EVENT:
				{
//				TransEventPkg receive_mes;
//
//				M_DECODE(receive_mes, msg.Msg_buf, msg.header.len);
//
//				for (int i = 0; i < receive_mes.trans_head.data_num; i++)
//				{
//					cout << "MT_YC_EVENT:" << receive_mes.event_info[i].key_id << "," << receive_mes.event_info[i].st_id << endl;
//				}
					break;
				}
				case MT_YX_EVENT:
				{
//				TransEventPkg receive_mes;
//
//				M_DECODE(receive_mes, msg.Msg_buf, msg.header.len);
//
//				for (int i = 0; i < receive_mes.trans_head.data_num; i++)
//				{
//					cout << "MT_YX_EVENT:" << receive_mes.event_info[i].key_id << "," << receive_mes.event_info[i].st_id << endl;
//
//					long key_id = receive_mes.event_info[i].key_id;
//					CCommon::long_to_keyid(key_id, &ks);
//
//					switch (ks.table_no)
//					{
//					case sca_breaker_table_no:
//					{
//						if (oodbread_rk(&psbob_cb, &key_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) <= 0)
//						{
//							TRACE("读取开关%ld数据失败\r\n", key_id);
//							continue;
//						}
//
//						if(psbob_cb.point > 0)	//开关为合闸状态
//						{
//							pthread_mutex_lock(&trig_list_mutex);
//							for (FDIR_POINT_TRIG_LIST::iterator itTrig = g_lstTrig.begin(); itTrig != g_lstTrig.end(); itTrig++)
//							{
//								if (itTrig->id.cb_id == key_id)
//								{
//									g_lstTrig.erase(itTrig);
//									TRACE("从过流开关列表中删除%ld(%s)的过流项。\r\n", key_id, psbob_cb.name);
//									break;
//								}
//							}
//							pthread_mutex_unlock(&trig_list_mutex);
//						}
//						else	//开关为分闸状态
//						{
//							data_trig.bjxcb = (HasMask(NULL, "brk_type", psbob_cb.brk_type , MENU_BRK_TYPE_JX));
//							gettimeofday(&data_trig.tm_add, NULL);
//							data_trig.b_alarm_cb = true;
//							data_trig.id.cb_id = key_id;
//
//							//将该开关加入过流开关列表
//							pthread_mutex_lock(&trig_list_mutex);
//							g_lstTrig.push_back(data_trig);
//							pthread_mutex_unlock(&trig_list_mutex);
//
//							TRACE("将开关%ld(%s)加入到故障信号列表\r\n", key_id, psbob_cb.name);
//						}
//						break;
//					}
//					default:
//					{
//						break;
//					}
//					}
//				}
					break;
				}
				default:
				{
//				cout << "unknow message:" << msg.header.event << endl;
//				printf("msg.header.len:%d\r\n", msg.header.len);
//				printf("msg.header.serv:%d\r\n", msg.header.serv);
//				printf("msg.header.seq:%d\r\n", msg.header.seq);
//				printf("msg.header.event:%d\r\n", msg.header.event);
//				printf("msg.header.domain:%d\r\n", msg.header.domain);
//				printf("msg.header.ctxt:%d\r\n", msg.header.ctxt);
//				printf("msg.header.stid:%d\r\n", msg.header.stid);
//				printf("msg.header.dtid:%d\r\n", msg.header.dtid);
//				printf("msg.header.ver_coding:%d\r\n", msg.header.ver_coding);
//				printf("msg.header.remain:%d\r\n", msg.header.remain);
//				cout << endl;

					break;
				}
			}
		}
	}
}

//恢复故障的步骤列表，为自动执行时提供数据
void RecoTaskStep(const char* groupid)
{
	assert(groupid != NULL);
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	list<FDIR_TASK>::iterator it;
	for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
	{
		if (it->rq_type == TASK_TYPE_FAULT)
		{
			if (strcmp(it->task_data.fault.head.group_id, groupid) == 0)
			{
				//获取每条故障的隔离信息
				sprintf(sql, "select cb,up_isolate from fdir_isolate where id=\'%s\'", it->task_data.fault.head.fault_id);
				cout << sql << endl;

				pthread_mutex_lock(&oci_mutex);
				int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
				pthread_mutex_unlock(&oci_mutex);
				if (ret != OCI_ERROR)
				{
					list<FDIR_ISO_CB> lst_iso;
					for (int i = 0; i < rec_num; i++)
					{
						FDIR_ISO_CB fdir_cb;
						bzero(&fdir_cb,sizeof(fdir_cb));
						int isup = 0;
						fdir_cb.cb = *(long*) (buf+(i*(attrs[0].col_width+attrs[1].col_width)));//, attrs[0].col_width);
						isup = *(int*)(buf+(i*(attrs[0].col_width+attrs[1].col_width))+attrs[0].col_width);
						if (isup == 1)
						{
							fdir_cb.q |= FDIR_ISOLATE_UP;
						}
						else
						{
							fdir_cb.q &= ~FDIR_ISOLATE_UP;
						}
						lst_iso.push_back(fdir_cb);
						TRACE("隔离开关id是：%ld\r\n", fdir_cb.cb);
					}
					if (lst_iso.size() > 0)
					{
						///把隔离步骤添加到内存的isolate里
						it->task_data.fault.fdir_r->setisolate(lst_iso);

					}
				}
				g_oci->Readdata_Free();

				it->task_data.fault.fdir_r->setfaultzone(it->task_data.fault.fault_zone);


			}
		}
	}
}

//恢复故障的详细信息，主要是故障的进线开关，以进线开关为根节点的树，为重新计算方案时使用
void RecoTaskItem(const char* groupid)
{
	assert(groupid != NULL);
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
	{
		if (it->rq_type == TASK_TYPE_FAULT)
		{
			if (strcmp(it->task_data.fault.head.group_id, groupid) == 0)
			{
				//获取故障馈线的进线开关和故障信号，把分闸的开关合闸，并且建立以进线开关为根节点的树
				sprintf(sql, "select JXCB from fdir_fdirect where id=\'%s\'", it->task_data.fault.head.fault_id);
				cout << sql << endl;

				int jxcb = -1;

				pthread_mutex_lock(&oci_mutex);
				int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
				pthread_mutex_unlock(&oci_mutex);
				if (ret != OCI_ERROR)
				{
					if (rec_num < 0)
					{
						TRACE("获取故障记录数为0，故障id是：%s\r\n",it->task_data.fault.head.fault_id);
						return;
					}
					else
					{
						jxcb = *(int*)buf;

						it->task_data.fault.fdir_r->setfaultjxcb(jxcb);
					}
				}

				g_oci->Readdata_Free();
				//从接收的故障信号中把分闸的开关找出来，并且把分闸设置成合闸，重新形成电流流入方向，重新建树
				sprintf(sql, "select dev_id from fdir_alarm where id=\'%s\'", it->task_data.fault.head.fault_id);
				cout << sql << endl;

				//数据初始化
//				it->task_data.fault.fdir_r->RecalcInit();
				pthread_mutex_lock(&oci_mutex);
				ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
				pthread_mutex_unlock(&oci_mutex);

				if (ret != OCI_ERROR)
				{
					if (rec_num > 0)
					{
						for (int i = 0; i < rec_num; i++)
						{
							unsigned long pid = 0;
							pid =*(long*) (buf+(i*attrs[0].col_width));

							KEY_STRU ks = {0};

							CCommon::long_to_keyid(pid, &ks);

							if(ks.table_no != g_db_psbob->GetTB("breaker")->table_id) continue;

							long id = pid;

							pid_to_id(id);

							CLS_CB *cb = g_bob->cb_tb->Find(CLS_CB(id));
							cb->q &= ~CB_OPEN;

							TRACE("合闸开关%ld(%s)\r\n", cb->pid, cb->descr);
						}
					}
					g_oci->Readdata_Free();
				}

				it->task_data.fault.fdir_r->FormIcbOfZone();

				it->task_data.fault.fdir_r->ReBuildFaultTree(it->task_data.fault.head.fault_id);
				//故障起始和结束设备的恢复
				it->task_data.fault.fdir_r->ReCoHeadTail(it->task_data.fault.head.fault_id);
			}
		}
	}


}


void RecoMemTask(const char* groupid)
{
	assert(groupid != NULL);
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	list<long> lst_bus;
	//启动一个故障
	FDIR_TASK data_task;

	//获取报警信号中的报警源
	sprintf(sql, "SELECT KEY_ID FROM PMS_PARAM.FDIR_ALARM WHERE ID IN (SELECT ID FROM PMS_PARAM.FDIR_FDIRECT WHERE GROUPID=\'%s\')AND DEV_TABLE =\'bus\' ", groupid);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		for (int i = 0; i < rec_num; i++)
		{
			char bus_id[MAX_BUS_ID_LEN+10];
			bzero(bus_id,sizeof(bus_id));
			strncpy(bus_id, buf + i * (attrs[0].col_width), attrs[0].col_width);
			//	strcat(bus_id, '\0');
			lst_bus.push_back(atol(bus_id));
		}
		g_oci->Readdata_Free();
	}

	sprintf(sql, "select id,study_eq,fd,zone from fdir_fdirect where groupid=\'%s\'", groupid);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		for (int i = 0; i < rec_num; i++)
		{
			//启动一个故障
			//	FDIR_TASK data_task;
			data_task.rq_type = TASK_TYPE_FAULT;
			data_task.task_data.fault.next_step = TASK_FAULT_STATUS_ISO;
			data_task.task_data.fault.bAuto = 0;
			data_task.task_data.fault.type = TASK_FAULT_LD;
			strncpy(data_task.task_data.fault.head.fault_id, buf, attrs[0].col_width), buf += attrs[0].col_width;
			strcpy(data_task.task_data.fault.head.group_id, groupid);
			data_task.task_data.fault.head.type = 0;
			data_task.task_data.fault.head.size = sizeof(FDIR_TASK_DATA_FAULT);
			data_task.task_data.fault.point_name = *(long*) (buf), buf += attrs[1].col_width;
			data_task.task_data.fault.dv = *(long*) (buf), buf += attrs[2].col_width;
			data_task.task_data.fault.fault_zone = *(long*) (buf), buf += attrs[3].col_width;
			data_task.task_data.fault.st = *(long*) (buf), buf += attrs[4].col_width;

			struct tm tm_fault = {0};

			sscanf(data_task.task_data.fault.head.fault_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", &tm_fault.tm_year, &tm_fault.tm_mon, &tm_fault.tm_mday, &tm_fault.tm_hour, &tm_fault.tm_min, &tm_fault.tm_sec, &data_task.tv_creat.tv_usec);
			tm_fault.tm_year = tm_fault.tm_year - 1900;
			tm_fault.tm_mon = tm_fault.tm_mon - 1;
			data_task.tv_creat.tv_sec = mktime(&tm_fault);

			TRACE("恢复故障到内存中故障ID:%s\r\n", data_task.task_data.fault.head.fault_id);

			AddTask(&data_task);
			for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
			{
				if (it->rq_type == TASK_TYPE_FAULT)
				{
					if (strcmp(it->task_data.fault.head.fault_id, data_task.task_data.fault.head.fault_id) == 0)
					{
						it->task_data.fault.fdir_r->lst_loss_vl_bus.clear();
						for (list<long>::iterator it1 = lst_bus.begin(); it1 != lst_bus.end(); it1++)
						{
							it->task_data.fault.fdir_r->lst_loss_vl_bus.push_back(*it1);
						}
						it->task_data.fault.fdir_r->Init();
					}
				}
			}
		}
		g_oci->Readdata_Free();
	}

}


bool AddTask(FDIR_TASK *newtask)
{
	assert(newtask != NULL);
	assert(newtask->rq_type == TASK_TYPE_FAULT);

	int i;
	pthread_mutex_lock(&mutex_array_fault);
	for (i = 0; i < MAX_FAULT; i++)
	{
		if (!map_idle_fdir_r[i])
		{
			newtask->task_data.fault.fdir_r = array_fdir_r[i];
			map_idle_fdir_r[i] = true;

			printf("\033[42m故障%s使用第%d号故障处理器[0x%x]！\033[0m\r\n", newtask->task_data.fault.head.fault_id, i, array_fdir_r[i]);
			break;
		}
	}
	pthread_mutex_unlock(&mutex_array_fault);
	if(i < MAX_FAULT)
	{
		g_lstTask.push_back(*newtask);
		TRACE("故障列表中开关名字%ld\n",g_lstTask.back().task_data.fault.point_name);
		return true;
	}
	else
	{
		TRACE("查找故障处理器失败,释放部分处理器后重新查找\n");
		//释放创建时间比较早的，而且不是执行中的故障
		FDIR_TASK_LIST::iterator it = g_lstTask.begin();
		FDIR_TASK_LIST::iterator it_find;

		for (; it != g_lstTask.end(); it++)
		{
			if (it->rq_type == TASK_TYPE_FAULT && it->status == 0)
			{
				it_find = it;
				break;
			}
		}
		for (; it != g_lstTask.end(); it++)
		{
			if (it->rq_type == TASK_TYPE_FAULT && it->status == 0)
			{
				if (it->tv_creat.tv_sec < it_find->tv_creat.tv_sec)
				{
					it_find = it;
				}
			}
		}

		if (it_find != g_lstTask.end())
		{
			printf("释放故障groupid是：%s\r\n", it_find->task_data.fault.head.group_id);
			if (!RemoveTasks(it_find->task_data.fault.head.group_id))
			{
				return false;
			}
			printf("自动归档故障groupid是：%s的故障组，并且把归档位置位1\r\n", it_find->task_data.fault.head.group_id);
			ClearFdirDB(it_find->task_data.fault.head.group_id);
			sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d, AUTO_DEAL=1 WHERE GROUPID=\'%s\'", STATUS_FDIRECT_STOP, it_find->task_data.fault.head.group_id);
			if (ExecSQL(sql) != OCI_ERROR)
			{
				sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_STOP, it_find->task_data.fault.head.group_id);
				ExecSQL(sql);
			}
		}
		else
		{
			TRACE("查找故障处理器失败,没有可以调度释放的处理器\n");
			return false;
		}

		if (AddTask(newtask))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
bool RemoveTask(const char *faultid)
{
	assert(faultid != NULL);

	int i;
	FDIR_TASK_LIST::iterator it = g_lstTask.begin();
	for(;it != g_lstTask.end();it++)
	{
		if(strcmp(it->task_data.fault.head.fault_id, faultid) == 0)
		{
			break;
		}
	}
	if(it == g_lstTask.end())
	{
		return false;
	}
	else
	{
		pthread_mutex_lock(&mutex_array_fault);
		for (i = 0; i < MAX_FAULT; i++)
		{
			if (it->task_data.fault.fdir_r == array_fdir_r[i])
			{
				map_idle_fdir_r[i] = false;
				it->task_data.fault.fdir_r = NULL;
				printf("\033[42m第%d号故障处理器[0x%x]被释放！\033[0m\r\n", i,
					   array_fdir_r[i]);
				break;
			}
		}
		pthread_mutex_unlock(&mutex_array_fault);
		g_lstTask.erase(it);
		if(i < MAX_FAULT)
		{
			return true;
		}
		else
		{
			printf("\033[42m故障处理器释放失败,故障id是%s！\033[0m\r\n",faultid);
			return false;
		}
	}

}

bool RemoveTasks(const char *grpid)
{
	assert(grpid != NULL);

	int i;
	FDIR_TASK_LIST::iterator it = g_lstTask.begin();
	for(;it != g_lstTask.end();)
	{
		if(strcmp(it->task_data.fault.head.group_id, grpid) == 0)
		{
			pthread_mutex_lock(&mutex_array_fault);
			for (i = 0; i < MAX_FAULT; i++)
			{
				if (it->task_data.fault.fdir_r == array_fdir_r[i])
				{
					map_idle_fdir_r[i] = false;
					it->task_data.fault.fdir_r = NULL;
					printf("\033[42m第%d号故障处理器[0x%x]被释放！\033[0m\r\n", i,array_fdir_r[i]);
					break;
				}
			}
			pthread_mutex_unlock(&mutex_array_fault);
			g_lstTask.erase(it);
			it = g_lstTask.begin();
			if(i < MAX_FAULT)
			{
			}
			else
			{
				printf("\033[42m释放故障组号为%s的故障号为%s的故障处理器失败！\033[0m\r\n", it->task_data.fault.head.group_id,it->task_data.fault.head.fault_id);
				return false;
			}
		}
		else
		{
			it++;
		}
	}

	return true;
}

int rte_connect(int argc, char **argv)
{
	cout << "开始连接RTE..." << endl;
	//D5000连接消息总线
	iProcess = proc_inv->proc_init(argv[1], argv[2], argv[3]);

	if (iProcess >= 0)
	{
		TRACE("进程管理初始化成功，本进程号为:%d\r\n", iProcess);

		iMessage = bob_inv->messageInit(argv[1], argv[2], argv[3]);

		if (iMessage >= 0)
		{
			//for (int i = 0; i <= 255; i++)
			//	bob_inv->messageSubscribe(i);
			bob_inv->messageSubscribe(APP_TO_WARN_CHANNEL);
			bob_inv->messageSubscribe(WARN_INFORM_CHANNEL);
		}
		else
		{
			TRACE("连接消息总线失败！\r\n");
			return OO_FAIL;
		}
	}
	else
	{
		TRACE("进程管理初始化失败！\r\n");
		return OO_FAIL;
	}

	return OO_SUCCEED;
}

void rte_disconnect()
{
	for (int i = 0; i <= 255; i++)
		bob_inv->messageUnSubscribe(i);

	bob_inv->messageExit(iMessage);
	proc_inv->proc_exit(iProcess);
}

int build_fi_dv_map(map<long, long> &map_fi_dv)
{
	//D5000还没建故障指示器表
	return 0;
//	map_fi_dv.clear();
//
//	int count = oodbget_recnum(const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")));
//
//	PSBOB::POINT point = { 0 };
////	PSBOB::FAULTINFO faultinfo = { 0 };
////	PSBOB::DV dv = { 0 };
//	PSBOB::OO_R_ST_POINT oo_r_st_point = { 0 };
//	PSBOB::OO_R_DV_ST oo_r_dv_st = { 0 };
//
//	int size = sizeof(point);
//
//	for (int index = 0; index < count; index++)
//	{
//		bzero(&point, size);
//
//		if (oodbread_rp(&point, index, const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), 1, size) == OO_FAIL)
//		{
//			printf("%s:读取PSBOB库表POINT记录失败，错误号：%d，LINE：%d\r\n", __FILE__, _oodb_errno, __LINE__);
//			return OO_FAIL;
//		}
//
//		if ((point.datatype & POINT_PPFT) || (point.datatype & POINT_TFS) || (point.datatype & POINT_GDFT))
//		{
//#if 0
//			//该故障指示器在线段上
//			if (point.groupid[0] == 0)
//			{
//
//			}
//			//该故障指示器在开关上
//			else
//			{
//
//			}
//#endif
//			if (oodbread_rph(&oo_r_st_point, point.oo_st, const_cast<TB_DESCR*>(g_db_psbob->GetTB("oo_r_st_point")), sizeof(oo_r_st_point)) == OO_FAIL)
//			{
//				printf("%s:读取PSBOB库表oo_r_st_point物理号为%d的记录失败，错误号：%d，LINE:%d\r\n", __FILE__, point.oo_st, _oodb_errno, __LINE__);
//				return OO_FAIL;
//			}
//
//			if (oodbread_ra(&oo_r_dv_st, "st_id", oo_r_st_point.st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("oo_r_dv_st")), sizeof(oo_r_dv_st)) == OO_FAIL)
//			{
//				printf("%s:读取PSBOB库表oo_r_dv_st[st]为%s的记录失败，错误号：%d，LINE:%d\r\n", __FILE__, oo_r_st_point.st_id, _oodb_errno, __LINE__);
//				return OO_FAIL;
//			}
//
//			map_fi_dv[point.id] = oo_r_dv_st.dv_id;
//
////			printf("%s,%s\r\n", point.id, oo_r_dv_st.dv_id);
//		}
//		else
//		{
//			continue;
//		}
//	}
//
//	printf("map_fi_dv.size() = %d\r\n", map_fi_dv.size());
//
//	return 0;
}

bool IsRunning(const char *process_name)
{
	assert(process_name != NULL);

	char szFile[260];
	sprintf(szFile, "/var/tmp/%s.pid", process_name);

	int fd = open(szFile, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP);
	if (-1 == fd)
	{
		TRACE("创建进程锁文件%s失败: %s\n", szFile, strerror(errno));
		return true;
	}

	if (0 == flock(fd, LOCK_EX | LOCK_NB))
	{
		return false;
	}

	close(fd);
	return true;
}

void* log(void *param)
{
	const int buf_size = 1024 * 1024; //1M
	int pipefd[2];
	char *buf = new char[buf_size];
	char *p = NULL;
	char ch;
	tm last = { 0 };
	FILE *fp = NULL;
	char szLogPath[260];

	sprintf(szLogPath, "%s/var/log/dmshanlog", getenv("HOME"));

	struct stat st;
	if (stat(szLogPath, &st) == -1)
	{
		if (mkdir(szLogPath, S_IRWXU | S_IRWXG | S_IRWXO) != 0)
		{
			delete[] buf;
			return 0;
		}
	}

	cout << "注意：屏幕输出已写入日志文件，请用cat命令查看" << szLogPath << "目录下相应文件。" << endl;

	pipe(pipefd);

	//将输出缓存设置为行缓存模式
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ);

	dup2(pipefd[1], STDOUT_FILENO);
	dup2(pipefd[1], STDERR_FILENO);

	p = buf;

	while (true)
	{
		int ret = read(pipefd[0], p, 1);

		if (*p == '\n')
		{
			p++;

			time_t now = time(NULL);
			tm *ptm = localtime(&now);

			//如果是新的一天，则重新建立新的日志文件
			if (ptm->tm_mday != last.tm_mday)
			{
				memcpy(&last, ptm, sizeof(tm));

				if (fp != NULL)
				{
					fclose(fp);
				}

				char szFile[260] = { 0 };

				if (version == VERSION_STUDY)
				{
					sprintf(szFile, "%s/dmshan_study_%04d%02d%02d.log", szLogPath, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
				}
				else
				{
					sprintf(szFile, "%s/dmshan_realtime_%04d%02d%02d.log", szLogPath, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
				}

				fp = fopen(szFile, "a+");

				if (fp == NULL)
				{
					delete[] buf;
					return 0;
				}
			}

			char szTime[50] = { 0 };

			sprintf(szTime, "[%04d/%02d/%02d %02d:%02d:%02d]:", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			fwrite(szTime, 1, strlen(szTime), fp);

			fwrite(buf, 1, p - buf, fp);
			fflush(fp);

			p = buf;
			bzero(buf, buf_size);
		}
		else
		{
			p++;
		}

		if (p - buf >= buf_size)
		{
			//单行文本过长
		}
	}

	delete[] buf;
	return NULL;
}

int main(int _argc, char *_argv[])
{
	if (getopt(_argc, _argv, "c:") == 'c')
	{
		long dv = atol(optarg);

		KEY_STRU ks = { 0 };

		CCommon::long_to_keyid(dv, &ks);

		g_db_psbob = new DB_PSBOB();
		g_db_dms = new DB_DMS;

		if (g_db_psbob->Init("psbob", "realtime", "dscada") == OO_FAIL || g_db_dms->Init("pwrflow", "realtime", "dscada_dms") == OO_FAIL)
		{
			return -1;
		}

		if (dv > 0 && ks.table_no != g_db_psbob->GetTB("subcontrolarea")->table_id)
		{
			TRACE("参数区域ID不正确!\r\n");
			return -1;
		}

		build_map2();

		DSBOB2 bob2;
		if (!bob2.ReadPSBOB(dv))
		{
			return -1;
		}

		return 0;
	}

	//唯 一实例
	if (version == VERSION_STUDY)
	{
		if (IsRunning("dmshan_study"))
		{
			TRACE("进程已经在运行中！\r\n");
			return 0;
		}
	}
	else
	{
		if (IsRunning("dmshan"))
		{
			TRACE("进程已经在运行中！\r\n");
			return 0;
		}
	}

	gethostname(hostname, 256);
	cout << "hostname:" << hostname << endl;

#ifdef _LOG_
	//将屏幕输出都写入到日志文件
	pthread_t pt_log;
	if (pthread_create(&pt_log, pthread_attr_default, (pthread_startroutine_t) log, NULL) < 0)
	{
		return -1;
	}
#endif
	if(version == VERSION_STUDY)
		cout << "模拟态程序版本！"<<endl;
	else
		cout << "非模拟态程序版本！"<<endl;

#ifndef _NO_RTE_
#ifndef _FAST_DEBUG
	if (version == VERSION_REALTIME && _argc < 4)
	{
		cout << "Your input is wrong. The right input should be:" << endl;
		cout << "dmshan system_name(context_name) subsystem_name bobtemplate_name [sync_flag 0 1] CR>" << endl;
		cout << "eg. :dmshan realtime rts_scd1 dabob" << endl;

		return -1;
	}
#endif
#endif
#ifdef _NO_RTE_
	cout << "以非RTE事件调试模式启动程序"<<endl;
#endif
	signal(SIGABRT, abrt);
	signal(SIGINT, abrt);
//	signal(SIGSEGV, abrt);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCONT, SIG_IGN);
//	signal(SIGTERM, SIG_IGN);


	g_db_psbob = new DB_PSBOB;
	g_db_dms = new DB_DMS;
	g_db_fdir = new DB_FDIR;

	if (version == VERSION_STUDY)
	{
#ifdef	_SCADA
		if (g_db_psbob->Init("psbob", "study", "scada") == OO_FAIL || g_db_dms->Init("pwrflow", "study", "scada_dms") == OO_FAIL || g_db_fdir->Init("fdir", "study", "scada_fdir") == OO_FAIL)
#else
		if (g_db_psbob->Init("psbob", "study", "dscada") == OO_FAIL || g_db_dms->Init("pwrflow", "study", "dscada_dms") == OO_FAIL || g_db_fdir->Init("fdir", "study", "dscada_fdir") == OO_FAIL)
#endif
		{
			delete g_db_psbob;
			delete g_db_dms;
			delete g_db_fdir;

			return -1;
		}
	}
	else
	{
#ifdef _SCADA
		if (g_db_psbob->Init("psbob", "realtime", "scada") == OO_FAIL || g_db_dms->Init("pwrflow", "realtime", "scada_dms") == OO_FAIL || g_db_fdir->Init("pwrflow", "realtime", "scada_fdir") == OO_FAIL)
#else
		if (g_db_psbob->Init("psbob", "realtime", "dscada") == OO_FAIL || g_db_dms->Init("pwrflow", "realtime", "dscada_dms") == OO_FAIL || g_db_fdir->Init("pwrflow", "realtime", "dscada_fdir") == OO_FAIL)
#endif
		{
			delete g_db_psbob;
			delete g_db_dms;
			delete g_db_fdir;

			return -1;
		}
	}

	if (build_fi_dv_map(g_map_fi_dv) == OO_FAIL)
	{
		delete g_db_psbob;
		delete g_db_dms;
		delete g_db_fdir;

		return -1;
	}

//	PSBOB::POINT t_point;
//
//	oodbread_rp(&t_point, 0, const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), 1, sizeof(PSBOB::POINT));
//
//	delete g_db_psbob;

	g_oci = new INTR_OCI;


	if (g_oci->connect(getenv("DMS_OCI_USER"), getenv("DMS_OCI_PWD"), getenv("DMS_OCI_SRV")) == OCI_ERROR)
	{
		TRACE("连接数据库失败！user:%s,pwd:%s,srv:%s\r\n", getenv("DMS_OCI_USER"), getenv("DMS_OCI_PWD"), getenv("DMS_OCI_SRV"));
		return -1;
	}

	GetFdirConfig(g_FdirConfig);

	//建立馈线首端节点与母线的路径
	build_map2();
	TRACE("共建立%d对路径映射关系！\r\n", g_map_nd_path.size());

#ifndef _FAST_DEBUG
	g_bob2 = new DSBOB2;

	if (!g_bob2->ReadPSBOB())
//	if(!g_bob2->ReadReDB(NULL, NULL, NULL))
	{
		cout << "读取PSBOB库失败!" << endl;

		return -1;
	}

//	g_bob2->WriteReDB2(NULL, NULL, NULL);

	nmd = new NMD(g_bob2);

	if (!nmd->DoValidate())
	{
		cout << __LINE__<<":NMD 检验失败！" << endl;

		return -1;
	}

	//建立ZN表
	MAP_SO_PATH so_path;

	if (!nmd->find_so(0, NULL, so_path, FIND_TYPE_BUILD_ZN))
	{
		cout << __LINE__<<":建立ZN失败！" << endl;

		return -1;
	}

#if 0
	if (!nmd->update_cb_state()|| !nmd->add_so())
	{
		cout << "自动添加电源失败！" << endl;

		delete g_bob2;
		delete nmd;

		return 10;
	}
#endif
	//故障恢复
	g_bob = new DSBOB;

	g_bob->ReadFromDsbob2(g_bob2);
	if (!g_bob2->WriteReDB2(NULL, NULL, NULL))
	{
		cout << "写入DMS库失败！" << endl;

		return -1;
	}
#endif

//	if (!g_bob->ReadReDB(NULL, NULL, NULL))
//	{
//		cout << "DSBOB读取DMS库失败!" << endl;
//
//		return -1;
//	}
#ifndef _FAST_DEBUG
//	TP tp(g_bob);
//
//	if (tp.tp_allnew() == TP_FAIL)
//	{
//		return -1;
//	}
//
//	if (!g_bob->WriteReDB(NULL, NULL, NULL))
//	{
//		return -1;
//	}
#endif
#if 0
	if (fdir_r->FormFdirDB() == FDIR_R_FAIL)
	{
		cout <<"FormFdirDB failed!"<<endl;

		return -1;
	}

	if (fdir_r->FormIcbOfZone() == FDIR_R_FAIL)
	{
		cout <<"FormIcbOfZone failed!"<<endl;

		delete g_bob;
		delete g_bob2;
		delete nmd;
		delete fdir_r;

		return 8;
	}

	if (!fdir_r->WriteRtdb())
	{
		cout << "fdir_r->WriteRtdb失败！" << endl;

		return -1;
	}
#endif


	CScaCommonFactory *scacommFactory;
	GetScaCommonFactory(&scacommFactory);
	g_sca_common = scacommFactory->CreateScaCommon();
	scacommFactory->Release();

	model_chg_ntf = &tmp_val0;

	CContext::SetContextNo(_argc, _argv, AC_REALTIME_NO);
	g_sca_common->DbNotifyAt(SMNG_MODEL_NO, &model_chg_ntf, AP_DSCADA);

	last_model_chg_ntf = *model_chg_ntf;

	pthread_attr_t attr;
	size_t size_stack = 2 * 1024 * 1024; //2M

	pthread_attr_init(&attr);
	size_t size_org_stack = 0;

	pthread_attr_getstacksize(&attr, &size_org_stack);

	TRACE("系统默认线程堆栈大小为%d bytes", size_org_stack);

	if (size_org_stack < size_stack)
	{
		pthread_attr_setstacksize(&attr, size_stack);
		cout << "，调整为" << size_stack << " bytes";
	}

	cout << endl;

	if (pthread_mutex_init(&g_rq_mutex, pthread_mutexattr_default))
	{
		perror("Init g_rq_mutex error");

		return -1;
	}

	if (pthread_mutex_init(&avg_i_mutex, pthread_mutexattr_default))
	{
		perror("Init avg_i_mutex error");

		return -1;
	}

	if (pthread_mutex_init(&mutex_array_fault, pthread_mutexattr_default))
	{
		perror("Init mutex_array_fault error");

		return -1;
	}

	if (pthread_mutex_init(&task_list_mutex, pthread_mutexattr_default))
	{
		perror("Init task_list_mutex error");

		return -1;
	}

	if (pthread_mutex_init(&oci_mutex, pthread_mutexattr_default))
	{
		perror("Init oci_mutex error");

		return -1;
	}

	if (pthread_mutex_init(&simu_sync_mutex, pthread_mutexattr_default))
	{
		perror("Init simu_mutex error");

		return -1;
	}

	g_fdir_r = new FDIR_R2(g_bob);

	g_fdir_r->Init();
	//g_fdir_r->count_ld();
	for(int iFault = 0; iFault < MAX_FAULT; iFault++)
	{
		array_fdir_r[iFault] = new FDIR_R2(g_bob);
		map_idle_fdir_r[iFault] = false;
	}

//	g_fdir_r->Do(114560315538017527, true);
//	g_fdir_r->ExecFdir("CSCSEC18", "sec");
//	fill_fdir_db("1111111111111111111111", "CSCSEC18", 0);
//#define _NO_RTE_
#ifdef _NO_RTE_
	///////////////////故障定位测试代码
	cout<<"start loop"<<endl;
//	FDIR_R *fdir_r1=NULL;
//	while(true)
//	{
//	fdir_r1 = new FDIR_R(g_bob);
//	delete fdir_r1;
//
//	usleep(50);
//	}
//
//	fdir_r1 = new FDIR_R(g_bob);
//	delete fdir_r1;
//
//	fdir_r1 = new FDIR_R(g_bob);
//	delete fdir_r1;

//	fdir_r1 = new FDIR_R(g_bob);
//	delete fdir_r1;

	FDIR_POINT_TRIG data;

	data.bjxcb = false;
	data.id.evt.point_name=114560315538017527;// "CSCCBA7";
	g_lstTrig.push_back(data);

//	data.bjxcb = false;
//	strcpy(data.id.evt.point_name, "CSCRLYA8BCGL");
//	g_lstTrig.push_back(data);

//	data.bjxcb = false;
//	strcpy(data.id.evt.point_name, "22222_Rs");
//	g_lstTrig.push_back(data);

//	data.bjxcb = false;
//	strcpy(data.id.evt.point_name, "18731_Pos");
//	g_lstTrig.push_back(data);

	g_fdir_r->data_start.type = TASK_FAULT_LD;
	g_fdir_r->data_start.source.ld_id=114560315538017527;// "CSCCBA7");
//	g_map_trig_fi["GuZhangCeShi1"].push_back("GZ1TFS1");
//	g_map_trig_fi["GuZhangCeShi1"].push_back("GZ1TFS2");
//	g_map_trig_fi["GuZhangCeShi1"].push_back("GZ1TFS3");
//	g_map_trig_fi["GuZhangCeShi3"].push_back("GZ3TFS129");
//	g_map_trig_fi["GuZhangCeShi3"].push_back("GZ3TFS131");
//	g_map_trig_fi["GuZhangCeShi3"].push_back("GZ3TFS88");
//	g_lstSecTrig.push_back("GZCTFS6");

//	g_lstSecTrig.push_back("CSC4");

	g_fdir_r->ExecFdir();

//	cout << "**************模拟开关CSCCBA2拒动******************" << endl;
//
//	delete fdir_r1;
//
//	fdir_r1 = new FDIR_R(g_bob);
////	FDIR_POINT_TRIG data;
//
//	data.bjxcb = false;
//	strcpy(data.id.evt.point_name, "YSCCBA1");
//	g_lstTrig.push_back(data);
//
//	data.bjxcb = false;
//	strcpy(data.id.evt.point_name, "YSCCBB1");
//	g_lstTrig.push_back(data);
//
//	data.bjxcb = false;
//	strcpy(data.id.evt.point_name, "YSCCBS1");
//	g_lstTrig.push_back(data);
//
//
//	fdir_r1->data_start.type = TASK_FAULT_JXCB;
//	strcpy(fdir_r1->data_start.source.ld_id, "YSCCBS1");
//
//	fdir_r1->ExecFdir();
//
//	delete fdir_r1;
	//生成故障描述文本文件
//	map<string, string> keys;
//	FAULT_ITEM fault_head;
//	list<FAULT_ITEM> fault_tails;
//	time_t now; ///////
//
//	char str[4096] = { 0 };
//	char str_tmp[1024] = { 0 };
//
//	sprintf(str, "<?xml version = \"1.0\" encoding = \"ISO-8859-1\" ?>\r\n<Message>\r\n");
//	sprintf(str_tmp, "<fault fault_id = \"%s\"/>\r\n", "");
//	strcat(str, str_tmp);
//	strcat(str, "<Body>\r\n");
//	g_fdir_r->get_fault_area(fault_head, fault_tails);
//	PSBOB::FAULTINFO faultinfo = { 0 };
//	//写入故障区段首端结点
//	if (fault_head.table == "FI")
//	{
//		if (oodbread_rk(&faultinfo, const_cast<char*>(fault_head.id.c_str()), const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo")), sizeof(PSBOB::FAULTINFO)) == OO_FAIL)
//		{
//			printf("读取faultinfo表关键字为:%s的记录失败，errno:%d,line:%d\r\n", fault_head.id.c_str(), _oodb_errno, __LINE__);
//			return 0;
//		}
//		sprintf(str_tmp, "<Record id = \"%s\" type = \"%s\" pole = \"%s\" sec = \"%s\"/>\r\n", fault_head.id.c_str(), fault_head.table.c_str(), faultinfo.poleid,
//				faultinfo.secid);
//	}
//	else
//	{
//		sprintf(str_tmp, "<Record id = \"%s\" type = \"%s\" pole = \"\" sec = \"\"/>\r\n", fault_head.id.c_str(), fault_head.table.c_str());
//	}
//
//	strcat(str, str_tmp);
//
//	for (list<FAULT_ITEM>::iterator it_tail = fault_tails.begin(); it_tail != fault_tails.end(); it_tail++)
//	{
//		if (it_tail->table == "FI")
//		{
//			if (oodbread_rk(&faultinfo, const_cast<char*>(it_tail->id.c_str()), const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo")), sizeof(PSBOB::FAULTINFO)) == OO_FAIL)
//			{
//				printf("读取faultinfo表关键字为:%s的记录失败，errno:%d,line:%d\r\n", it_tail->id.c_str(), _oodb_errno, __LINE__);
//				break;
//			}
//			sprintf(str_tmp, "<Record id = \"%s\" type = \"%s\" pole = \"%s\" sec = \"%s\"/>\r\n", it_tail->id.c_str(), it_tail->table.c_str(), faultinfo.poleid, faultinfo.secid);
//		}
//		else
//		{
//			sprintf(str_tmp, "<Record id = \"%s\" type = \"%s\" pole = \"\" sec = \"\"/>\r\n", it_tail->id.c_str(), it_tail->table.c_str());
//		}
//
//		strcat(str, str_tmp);
//	}
//	strcat(str, "</Body>\r\n</Message>");
//	cout << "str=" << str << endl;
//
//	FILE *fp = fopen(FAULT_DESC_FILE, "w+");
//
//	if (fp != NULL)
//	{
//		fwrite(str, 1, strlen(str), fp);
//		fclose(fp);
//	}
	return 0;
	///////////////////故障定位测试代码
#endif
	if (pthread_mutex_init(&trig_list_mutex, pthread_mutexattr_default))
	{
		perror("Init trig_list_mutex error");

		return -1;
	}
#ifndef _FAST_DEBUG
	pthread_t pt_process;
	if (pthread_create(&pt_process, &attr, (pthread_startroutine_t) thread_process, 0) < 0)
	{
		cout << "创建故障处理线程失败！" << endl;
	}

	pthread_detach(pt_process);

	pthread_t pt_alarm_process;
	if (pthread_create(&pt_alarm_process, &attr, (pthread_startroutine_t) thread_alarm_process, 0) < 0)
	{
		cout << "创建故障信号扫描线程失败！" << endl;
	}

	pthread_detach(pt_alarm_process);

	pthread_t pt_iv;
	if (pthread_create(&pt_iv, &attr, (pthread_startroutine_t) thread_get_ivalue, 0) < 0)
	{
		cout<<"创建开关电流平均值读取线程失败！"<<endl;
	}
	pthread_detach(pt_iv);

	if (version == VERSION_REALTIME)
	{
		// 创建扫描母线故障线程
		pthread_t pt_scan_bus_process;
		if (pthread_create(&pt_scan_bus_process, pthread_attr_default, (pthread_startroutine_t) thread_scan_bus, 0) < 0)
		{
			cout << "创建母线故障扫描线程失败！" << endl;
		}
		pthread_detach(&pt_scan_bus_process);

		//连接RTE
		//D5000连接RTE
		bob_inv = new message_invocation;
		proc_inv = new proc_invocation;


		rte_connect(_argc, _argv);

		pthread_t pt_recv_rte;
		if (pthread_create(&pt_recv_rte, &attr, (pthread_startroutine_t) thread_recv_rte, 0) < 0)
		{
			cout << "创建RTE事件接收线程失败！" << endl;
		}

		pthread_detach(pt_recv_rte);
	}
	else if (version == VERSION_STUDY)
	{
		proc_inv = new proc_invocation;
		int Process = proc_inv->proc_init("study", "dscada_dms", "dmshan_study");

		if (Process >= 0)
		{
			TRACE("进程管理初始化成功，本进程号为:%d\r\n", Process);

		}
		else
		{
			TRACE("进程管理初始化失败！\r\n");
			return OO_FAIL;
		}
	}

#endif
	tcptools = new TCPClass;
	int ret = tcptools->Srv_Init(version == VERSION_REALTIME ? SERVER_PORT : SERVER_PORT_STUDY);

	if (ret < 0)
	{
		cout << " tcp Srv_init failed ... " << endl;

		return -1;
	}

	listenfd = tcptools->GetListenfd();

	//
	//  Accept client connect request
	//
	int newsockfd = 0;

	numthread = 0;

	for (int i = 0; i < MAX_THREAD; i++)
	{
		pthread_id[i].fdinThread = -1;
	}

	while (GDI_TRUE)
	{
		if (numthread > MAX_THREAD)
		{
			numthread = 0;
		}

		if ((newsockfd = tcptools->Srv_Accept()) < 0)
		{
		}

		numthread = newsockfd;

		//yangyong 20150623
		if (pthread_id[numthread].fdinThread == -1)
		{
			pthread_id[numthread].fdinThread = newsockfd;
			pthread_id[numthread + 1].fdinThread = newsockfd;
		}

		if (newsockfd == -1 && numthread == -1)
		{
			break;
		}

		int ret;

		if ((ret = pthread_create(&pthread_id[numthread].thread_id, &attr, (pthread_startroutine_t) HandlerMessage, (pthread_addr_t) &newsockfd)) < 0)
		{
#ifdef _DEBUG_
			printf("Create Failed!!!\n");
#endif
		}
		else if((ret = pthread_create(&pthread_id[numthread + 1].thread_id, &attr, (pthread_startroutine_t) thread_simu_return, (pthread_addr_t) &newsockfd)) < 0)
		{
#ifdef _DEBUG_
			TRACE("Create Failed!!!\n");
#endif
		}

		pthread_detach(pthread_id[numthread].thread_id);
		pthread_detach(pthread_id[numthread + 1].thread_id);
	}

	return 0;
}

void HandlerMessage(void *para)
{
	//
	//  Recv and Send client xml message init
	//
	int ret;
	int newsockfd;
	TiXmlDocument *pXmlDoc = new TiXmlDocument();

	struct timeval tpstart, tpend;
	long iTimeInterval;
	newsockfd = *((int*)para);
	cout << " ******** New Thread (" << newsockfd << ")********" << endl;

	char recv_buf[MAX_BUFLEN];
	bzero(recv_buf, MAX_BUFLEN);
	//
	//  Transact client xml message
	//
	int msgType = 0;
	timeval stimer = {0};
	time_t now = 0;

	//char recv_xml[40960] = {0};
	FILE *pfile = NULL;
	char filename[256];

	sprintf(filename, "dms_socket_%d.xml", newsockfd);

	while (GDI_TRUE)
	{
		RE_RECV:
		if ((ret = tcptools->ReadLine(newsockfd, recv_buf, MAX_BUFLEN)) <= 0)
		{
			if (ret == 0)
			{
			}

			break;
		}

		//接收到新的XML格式请求信息，创建新的临时文件储存接下来收到的XML文件数据
		if(strncasecmp(recv_buf, "<?xml", 5) == 0)
		{
			cout << ">>>>>>>>"<<ctime(&(now = time(NULL)))<<"new request!" << endl;

			gettimeofday(&stimer, NULL);

			pfile = fopen(filename, "w+");

//			fwrite(test_misoperation, 1, strlen(test_misoperation), pfile);

			if (pfile == NULL)
			{
				cout << "创建临时XML文件失败，" << strerror(errno) << endl;
				break;
			}
		}
		cout << recv_buf << endl;

		//将收到的数据写入临时文件
		if (pfile > 0)
			fputs(recv_buf, pfile);
		else
			cout << "错误的请求报文数据：" << recv_buf << endl;

		//XML传送结束
		if (strstr(recv_buf, "</Message>") != NULL)
		{
			try
			{
				fclose(pfile);
			} catch (...)
			{
			}
			pfile = NULL;

			MAP_SO_PATH paths;
			ORDER order = {0};
			char ret_buf[1024] = { 0 };

			paths.clear();

			cout << "下行报文结束！" << endl;

			int cmd = 0;

			//XML解析器载入临时XML文档并解析
			if (pXmlDoc->LoadFile(filename))
			{
				try
				{
					unlink(filename); //删除临时文件
				} catch (...)
				{
				}

				TiXmlNode *pnode = pXmlDoc->FirstChild(); //xml版本

				while ((pnode = pnode->NextSibling()) != NULL)
				{
					TiXmlElement *element = pnode->ToElement();

					if (element != NULL && strcasecmp(element->Value(), "Message") == 0)	//Message节点
					{
						TiXmlNode *pmsgnode = pnode->FirstChild();
						char Header[100] = {0};//请求类型

						while (pmsgnode != NULL)
						{
							TiXmlElement *msgelement = pmsgnode->ToElement();

							if (strcasecmp(msgelement->Value(), "Header") == 0)		//Header节点
							{
//								if (strcasecmp(msgelement->Attribute("type"), "Equipment_Color") != 0)	//判断是否着色请求,如果不是,将不响应本次请求
//									goto RE_RECV;
								strncpy(Header, msgelement->Attribute("type"), 100);
							}
							else if (strcasecmp(msgelement->Value(), "Body") == 0)	//Body节点
							{
								TiXmlNode *pbodynode = pmsgnode->FirstChild();

								while (pbodynode != NULL)
								{
									TiXmlElement *bodyelement = pbodynode->ToElement();

									//设备着色
									if (strcasecmp(Header, "Equipment_Color") == 0)
									{
										if (strcasecmp(bodyelement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.equipment_type, bodyelement->Attribute("equipment_type"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));
											strcpy(order.st, bodyelement->Attribute("st"));

											cmd = atoi(bodyelement->Attribute("equipment_type"));

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											//更新开关状态,并根据母线的遥测信息添加SO
											if (nmd->update_cb_state() && nmd->add_so()/* && nmd->DoValidate() && g_bob2->WriteReDB(NULL, NULL, NULL)*/)
											{
												TiXmlNode *pordernode = pbodynode->FirstChild();

												//循环查找Record项,并处理
												while (pordernode != NULL)
												{
													TiXmlElement *porderelement = pordernode->ToElement();

													if (strcasecmp(porderelement->Value(), "Record") == 0)
													{
														switch (cmd)
														{
															case 1: //电源跟踪
															{
																nmd->find_so(atol(porderelement->Attribute("id")), porderelement->Attribute("tablename"), paths, FIND_TYPE_SO);

																break;
															}
															case 2: //供电范围
															{
																nmd->find_so(atol(porderelement->Attribute("id")), porderelement->Attribute("tablename"), paths, FIND_TYPE_SO_AREA);

																break;
															}
															case 100: //检查设备连接状态
															{
																nmd->find_so(atol(porderelement->Attribute("id")), porderelement->Attribute("tablename"), paths, FIND_TYPE_CONNECT);

																break;
															}

															default:
																break;
														}

													}
													else
													{
													}

													pordernode = pordernode->NextSibling();
												}
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										//发送请求响应数据
										//发送XML响应头
										cout << head_xml << endl;
										ret = tcptools->Send(newsockfd, head_xml, strlen(head_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送响应请求上下文信息
										sprintf(ret_buf, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" equipment_type = \"%s\" >\r\n", order.picturename,
												order.contextname, order.bobname, order.st, order.equipment_type);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//逐项发送设备列表里的设备信息
										for (MAP_SO_PATH::iterator it = paths.begin(); it != paths.end(); it++)
										{
											while (it->second.path.size() > 0)
											{
												sprintf(ret_buf, "<Record id = \"%ld\" tablename = \"%s\" />\r\n", it->second.path.top().id, it->second.path.top().table.c_str());

												if(it->second.path.top().table == "busbarsection" || it->second.path.top().table == "energyconsumer")
												{
													int field_no[2] = { 0, 0 };
													int ret = 0;
													int field_num = 2;

													ret = GetFieldNoByName(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), "nd,name", field_no, &field_num);
													if (ret >= 0)
													{

														struct
														{
															long nd;
															char name[64];
														} value;

														ret = GetFieldsByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), (char*) &it->second.path.top().id, field_no,
																			field_num, sizeof(value), (char*) &value);
														if (ret == 0)
														{
															TRACE("nd:%ld,name:%s\r\n", value.nd, value.name);
														}
													}

												}
												else
												{
													int field_no[3] = { 0, 0, 0 };
													int ret = 0;
													int field_num = 3;

													ret = GetFieldNoByName(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), "ind,jnd,name", field_no, &field_num);
													if (ret >= 0)
													{

														struct
														{
															long ind;
															long jnd;
															char name[64];
														} value;

														ret = GetFieldsByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), (char*) &it->second.path.top().id, field_no,
																			field_num, sizeof(value), (char*) &value);
														if (ret == 0)
														{
															TRACE("ind:%ld,jnd:%ld,name:%s\r\n", value.ind, value.jnd, value.name);
														}
													}
												}

												cout << ret_buf << endl;
												ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

												if (ret <= 0)
												{
													cout << "发送数据失败，" << strerror(errno) << endl;
													goto RE_RECV;
												}

												//杨勇 20160614 添加对环网柜母线的着色
												DEV curDev = it->second.path.top();
												map<long, list<long> >::iterator itRMUbus = g_map_nd_bus.find(curDev.nd);
												if (itRMUbus != g_map_nd_bus.end() && strcasecmp(curDev.table.c_str(), "BUS") != 0
													&& strcasecmp(curDev.table.c_str(), "busbarsection") != 0)
												{
													for (list<long>::iterator itRMU = itRMUbus->second.begin(); itRMU != itRMUbus->second.end(); itRMU++)
													{
														sprintf(ret_buf, "<Record id = \"%ld\" tablename = \"%s\" />\r\n", *itRMU, "busbarsection");

														cout << ret_buf << endl;

														ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

														if (ret <= 0)
														{
															cout << "发送数据失败，" << strerror(errno) << endl;
															goto RE_RECV;
														}
													}
												}

												it->second.path.pop();
											}
										}

										//发送XML响应尾
										cout << tail_xml << endl;
										ret = tcptools->Send(newsockfd, tail_xml, strlen(tail_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}
									}
									else if(strcasecmp(Header,"fdir_step") == 0)
									{
										char fault_id[100] = {0};
										char fault_machine[100] = { 0 };
										if (strcasecmp(bodyelement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));
											strcpy(order.st, bodyelement->Attribute("st"));
											strcpy(fault_machine, bodyelement->Attribute("machine"));
											strcpy(fault_id, bodyelement->Attribute("fault_id"));

											int ret = FDIR_R_FAIL;

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											sprintf(sql, "DELETE FROM FDIR_STEP WHERE ID =\'%s\' and status=0", fault_id);
											ExecSQL(sql);

											TiXmlNode *pordernode = pbodynode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													sprintf(sql, "Insert into FDIR_STEP (ID, CB_ID, OPERATION, STEP, STATUS, CONTROL, STAGE) Values(\'%s\', \'%s\', %s, %s, 0, %s, %s)"
															,fault_id, porderelement->Attribute("cbId"), porderelement->Attribute("operation")
															,porderelement->Attribute("step"), porderelement->Attribute("control"), porderelement->Attribute("stage"));
													ExecSQL(sql);
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										UpdateExtStatus(fault_id);

										//发送请求响应数据
										//发送XML响应头
										cout << head_xml5 << endl;
										ret = tcptools->Send(newsockfd, head_xml5, strlen(head_xml5));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送响应请求上下文信息
										sprintf(ret_buf, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" machine=\"%s\">\r\n", order.picturename,
												order.contextname, order.bobname, order.st, fault_machine);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//逐项发送设备列表里的设备信息
										sprintf(ret_buf, "<Record id = \"%s\" result = \"0\" error=\"\"/>\r\n", fault_id);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送XML响应尾
										cout << tail_xml << endl;
										ret = tcptools->Send(newsockfd, tail_xml, strlen(tail_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}
									}
									else if (strcasecmp(Header, "study_fdir") == 0)
									{
										map<string, string> map_fault_group;
										list<string>  lst_group;
										list<string>  lst_remove_group;//故障模拟完成后需要从内存中删除的故障id

										char fault_id[100] = {0};
										long fault_sec;
										char fault_table[50] = { 0 };
										char fault_machine[100] = { 0 };
										if (strcasecmp(bodyelement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));
											strcpy(order.st, bodyelement->Attribute("st"));
											strcpy(fault_machine, bodyelement->Attribute("machine"));

											int ret = FDIR_R_FAIL;

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											pthread_mutex_lock(&task_list_mutex);
											TiXmlNode *pordernode = pbodynode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													fault_sec = atol(porderelement->Attribute("id"));
													strcpy(fault_table, porderelement->Attribute("tablename"));

													FDIR_TASK data_task;
													PSBOB::DV dv = { 0 };
													PSBOB::CB breaker = { 0 };
													PSBOB::FI faultinfo = { 0 };
													PSBOB::LD energyconsumer = { 0 };
													PSBOB::BUS busbarsection = { 0 };

													// 将可能是遥信ID转换成设备ID
													KEY_STRU ks = { 0 };
													CCommon::long_to_keyid(fault_sec, &ks);
													ks.field_id = 0;

													long dev_id = 0;
													CCommon::keyid_to_long(&ks, &dev_id);

													if (strcasecmp(fault_table, "faultinfo") == 0) //模拟故障的设备是故障指示器
													{
														if (oodbread_rk(&faultinfo, &fault_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo")), sizeof(PSBOB::FI)) < 0)
														{
															TRACE("oodbread_rk(%ld) failed!\r\n", fault_sec);
														}
														else
														{
															data_task.task_data.fault.st = faultinfo.st_id;
															GetDv(faultinfo.aclnseg_id, "sec", dv);
														}
													}
													else if (strcasecmp(fault_table, "breaker") == 0) //模拟故障的设备是开关设备
													{
														if (oodbread_rk(&breaker, &fault_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) < 0)
														{
															TRACE("oodbread_rk(%ld) failed\r\n", fault_sec);
														}
														else
														{
															data_task.task_data.fault.st = breaker.st_id;

															if (HasMask(g_db_psbob->GetTB("breaker"), "brk_type", breaker.brk_type, MENU_BRK_TYPE_JX))
															{
																GetJxcbDv(breaker.id, dv);
															}
															else
															{
																GetDv(breaker.id, "cb", dv);
															}
														}
													}
													else if (strcasecmp(fault_table, "energyconsumer") == 0) //模拟故障的设备是负荷设备
													{
														if (oodbread_rk(&energyconsumer, &fault_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), sizeof(PSBOB::LD)) < 0)
														{
															TRACE("oodbread_rk(%ld) failed\r\n", fault_sec);
														}
														else
														{
															data_task.task_data.fault.st = energyconsumer.st_id;
															GetDv(fault_sec, "ld", dv);
														}
													}
													else if (strcasecmp(fault_table, "busbarsection") == 0) //模拟故障的设备是母线
													{
														if (oodbread_rk(&busbarsection, &fault_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(PSBOB::BUS)) < 0)
														{
															TRACE("oodbread_rk(%ld) failed\r\n", fault_sec);
														}
														else
														{
															data_task.task_data.fault.st = busbarsection.st_id;
															GetDv(fault_sec, "bus", dv);
														}
													}

													if (dv.id != 0)
													{
														int fdir_damode = GetDvDamode(dv.id);
														if (fdir_damode == -1)
														{
															cout << "获取馈线的故障处理模式失败" << dv.id << endl;
															pordernode = pordernode->NextSibling();
															continue;
														}
														//禁止处理故障
														if (fdir_damode == FDIR_MODE_FORBID)
														{
															pordernode = pordernode->NextSibling();
															continue;
														}
														GetFdirConfig(g_FdirConfig);
														data_task.rq_type = TASK_TYPE_FAULT;
														gettimeofday(&data_task.tv_creat, NULL);

														data_task.task_data.fault.next_step = TASK_FAULT_STATUS_CALC_ISO;
														if (g_FdirConfig.AUTO_DEAL_HIS)
														{
															data_task.task_data.fault.bAuto = 2;
														}
														else
														{
															data_task.task_data.fault.bAuto = fdir_damode == FDIR_MODE_AUTO ? 1 : 0;
														}

														data_task.task_data.fault.dv = dv.id;
														data_task.task_data.fault.type = TASK_FAULT_STUDY;
														data_task.task_data.fault.head.type = 0;
														data_task.task_data.fault.head.size = sizeof(FDIR_TASK_DATA_FAULT);
														data_task.task_data.fault.point_name = atol(porderelement->Attribute("id"));

														tm *ptm = localtime(&data_task.tv_creat.tv_sec);
														sprintf(data_task.task_data.fault.head.fault_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday,
																ptm->tm_hour, ptm->tm_min, ptm->tm_sec, data_task.tv_creat.tv_usec);

														strcpy(fault_id, data_task.task_data.fault.head.fault_id);

														cout << "新故障,故障ID:" << data_task.task_data.fault.head.fault_id << endl;

														gettimeofday(&data_task.tv_start, NULL);

														if (!AddTask(&data_task))
														{
															cout << "添加新故障任务失败" << endl;
														}
														else
														{
															if (g_lstTask.back().task_data.fault.fdir_r->ExecFdir(dev_id, fault_table) == FDIR_R_SUCCEED)
															{
																map_fault_group[fault_id] = "";
																cout << "模拟故障成功" << endl;
															}
															else
															{
																sprintf(sql,
																		"Insert into FDIR_FDIRECT (ID, GROUPID, TYPE, FD, FD_DESCR, STATUS,STUDY_EQ,STUDY_MACHINE) Values (\'%s\', \'%s\', 1, \'%ld\', \'%s\', %d, \'%ld\',\'%s\')",
																		fault_id, fault_id, dv.id, dv.name, STATUS_FDIRECT_DIRECT_FAIL, fault_sec, fault_machine);
																ExecSQL(sql);

																RemoveTask(fault_id);
															}
														}
													}
													else
													{
														cout << "获取馈线信息失败" << porderelement->Attribute("id") << endl;
													}
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&task_list_mutex);
											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										map<string, string>::iterator it_map_fault_group;

										while (true)
										{
											pthread_mutex_lock(&task_list_mutex);

											for (list<FDIR_TASK>::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
											{
												if (it->rq_type != TASK_TYPE_FAULT)
													continue;
												if (it->task_data.fault.next_step != TASK_FAULT_STATUS_ISO)
													continue;

												it_map_fault_group = map_fault_group.find(it->task_data.fault.head.fault_id);

												if (it_map_fault_group != map_fault_group.end())
												{
													it_map_fault_group->second = it->task_data.fault.head.group_id;

													if (it->task_data.fault.bAuto == 0)
													{
														//计算完成后把故障从任务列表的内存中删除，在执行的时候再把内存数据恢复
														lst_remove_group.push_back(it->task_data.fault.head.fault_id);
													}
												}
											}

											pthread_mutex_unlock(&task_list_mutex);

											for (it_map_fault_group = map_fault_group.begin(); it_map_fault_group != map_fault_group.end(); it_map_fault_group++)
											{
												if(it_map_fault_group->second.size() < 10) break;
												else
												{
													lst_group.push_back(it_map_fault_group->second);
												}
											}

											if (it_map_fault_group == map_fault_group.end())
											{
												break;
											}

											sleep(1);
										}

										lst_group.sort();
										lst_group.unique();

										string str_group;

										for(list<string>::iterator it_lst_group = lst_group.begin(); it_lst_group != lst_group.end(); it_lst_group++)
										{
											str_group += *it_lst_group;
											str_group += ";";

											sprintf(sql, "UPDATE FDIR_FDIRECT SET TYPE=1,STUDY_MACHINE=\'%s\' WHERE GROUPID=\'%s\'", fault_machine, it_lst_group->c_str());
											ExecSQL(sql);
										}

										pthread_mutex_lock(&task_list_mutex);
										for (list<string>::iterator it_lst = lst_remove_group.begin(); it_lst != lst_remove_group.end(); it_lst++)
										{
											RemoveTask(it_lst->c_str());
										}
										pthread_mutex_unlock(&task_list_mutex);

										//发送请求响应数据
										//发送XML响应头
										cout << head_xml4 << endl;
										ret = tcptools->Send(newsockfd, head_xml4, strlen(head_xml4));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送响应请求上下文信息
										sprintf(ret_buf, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" machine=\"%s\">\r\n", order.picturename,
												order.contextname, order.bobname, order.st, fault_machine);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}


										sprintf(ret_buf, "<Record fault_id=\"%s\" restult = \"0\" error=\"0\"/>\r\n", str_group.c_str());

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送XML响应尾
										cout << tail_xml << endl;
										ret = tcptools->Send(newsockfd, tail_xml, strlen(tail_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}
									}
									else if (strcasecmp(Header, "fault_simu_save") == 0)
									{
										if(strcasecmp(bodyelement->Value(), "Simu_Plan") == 0)
										{
											//接收报文
											SIMU_PLAN simu = {0};
											strcpy(simu.plan_id, bodyelement->Attribute("id"));
											strcpy(simu.machine_name, bodyelement->Attribute("machine"));
											strcpy(simu.usr_id, bodyelement->Attribute("user"));
											strcpy(simu.plan_name, bodyelement->Attribute("name"));
											simu.type = atoi(bodyelement->Attribute("type"));

											TiXmlNode* simu_node = pbodynode->FirstChild();
											SIMU_SIG_LIST lst_sig;

											while (simu_node != NULL)
											{
												TiXmlElement* simu_element = simu_node->ToElement();

												if (strcasecmp(simu_element->Value(), "Record") == 0)
												{
													SIMU_SIGNAL simu_signal = {0};
													strcpy(simu_signal.plan_id, simu.plan_id);
													simu_signal.sig_id = atol(simu_element->Attribute("id"));
													simu_signal.status = atoi(simu_element->Attribute("status"));
													simu_signal.type = atoi(simu_element->Attribute("type"));
													simu_signal.sig_delay = atoi(simu_element->Attribute("delay"));
													strcpy(simu_signal.sig_name, simu_element->Attribute("name"));
													if (strlen(simu_element->Attribute("comment")) > 0)
													{
														strcpy(simu_signal.comment, simu_element->Attribute("comment"));
													}
													else
													{
														strcpy(simu_signal.comment, "");
													}

													lst_sig.push_back(simu_signal);
												}

												simu_node = simu_node->NextSibling();
											}

											sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_SUMMARY(ID, TYPE, NAME, MACHINE, USERID, RUN_TIME, FAULT_INDEX) VALUES(\'%s\', %d, \'%s\', \'%s\', \'%s\', 1, -1)",
													simu.plan_id, simu.type, simu.plan_name, simu.machine_name, simu.usr_id);
											ExecSQL(sql);

											std::list<SIMU_SIGNAL>::iterator it_sig;
											for(it_sig  = lst_sig.begin(); it_sig != lst_sig.end(); it_sig++)
											{
												sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_SIGNAL (ID, SIG_ID, SIG_NAME, TYPE, DELAY, STATUS, COMMENT) VALUES (\'%s\', %ld, \'%s\', %d, %d, %d, \'%s\')",
														it_sig->plan_id, it_sig->sig_id, it_sig->sig_name, it_sig->type, it_sig->sig_delay, it_sig->status, it_sig->comment);
												ExecSQL(sql);
											}

											//临时方案不校验
											bool bresult = true;
											if (simu.type == SIMU_PLAN_PERMANENT)
											{
												//分析拓扑，参考典型运行方式的代码
												it_sig = lst_sig.begin();
												SIMU_SIGNAL start_sig = {0};
												memcpy(&start_sig, &(*it_sig), sizeof(SIMU_SIGNAL));
												bresult = save_simu_cb_info(start_sig.sig_id, simu.plan_id);
											}

											//发送请求响应数据
											//发送XML响应头
											cout << head_xml_simu_save << endl;
											ret = tcptools->Send(newsockfd, head_xml_simu_save, strlen(head_xml_simu_save));

											if (ret <= 0)
											{
												cout << "发送数据失败，" << strerror(errno) << endl;
												goto RE_RECV;
											}

											//发送响应请求上下文信息
											sprintf(ret_buf, "<Simu_Plan  id = \"%s\" type=\"%d\" name=\'%s\' machine=\"%s\" user=\'%s\'>\r\n",
													simu.plan_id, simu.type, simu.plan_name, simu.machine_name, simu.usr_id);

											cout << ret_buf << endl;
											ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

											if (ret <= 0)
											{
												cout << "发送数据失败，" << strerror(errno) << endl;
												goto RE_RECV;
											}


											sprintf(ret_buf, "<Record result = \"%d\" error=\"0\"/>\r\n", bresult?0:-1);

											cout << ret_buf << endl;
											ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

											if (ret <= 0)
											{
												cout << "发送数据失败，" << strerror(errno) << endl;
												goto RE_RECV;
											}

											//发送XML响应尾
											cout << tail_xml_simu << endl;
											ret = tcptools->Send(newsockfd, tail_xml_simu, strlen(tail_xml_simu));

											if (ret <= 0)
											{
												cout << "发送数据失败，" << strerror(errno) << endl;
												goto RE_RECV;
											}
										}

									}
									else if (strcasecmp(Header, "fault_imitation_check") == 0)	//故障模拟校验
									{
										if (strcasecmp(bodyelement->Value(), "Simu_Plan") == 0)
										{
											//接收报文
											SIMU_PLAN simu = {0};
											strcpy(simu.plan_id, bodyelement->Attribute("id"));
											strcpy(simu.machine_name, bodyelement->Attribute("machine"));
											strcpy(simu.usr_id, bodyelement->Attribute("user"));
											simu.type = atoi(bodyelement->Attribute("type"));

											TiXmlNode* simu_node = pbodynode->FirstChild();
											CHECK_LIST check_cbs;
											//循环查找Record项,并处理
											while (simu_node != NULL)
											{
												TiXmlElement* simu_element = simu_node->ToElement();

												if (strcasecmp(simu_element->Value(), "Record") == 0)
												{
													long record_id = atol(simu_element->Attribute("id"));
													KEY_STRU ks = {0};
													CCommon::long_to_keyid(record_id, &ks);
													ks.field_id = 0;
													CCommon::keyid_to_long(&ks, &record_id);

													pthread_mutex_lock(&task_list_mutex);

													if (ks.table_no == g_db_psbob->GetTB("breaker")->table_id)
													{
														CHECK_DATA data_check = {0};
														data_check.cb_id = record_id;
														data_check.status = atoi(simu_element->Attribute("status"));
														data_check.Ia = atof(simu_element->Attribute("ia"));
														data_check.Ib = atof(simu_element->Attribute("ib"));
														data_check.Ic = atof(simu_element->Attribute("ic"));
														data_check.p = atof(simu_element->Attribute("p"));
														data_check.q = atof(simu_element->Attribute("q"));

														check_cbs.push_back(data_check);
													}
													else
													{
														TRACE("无法校验的设备ID：%ld\r\n", record_id);
														simu_node = simu_node->NextSibling();
														continue;
													}

													pthread_mutex_unlock(&task_list_mutex);
												}

												simu_node = simu_node->NextSibling();
											}

											//向人机返回校验结果
											if (Check_Simu_Data(check_cbs))
											{
												//发送请求响应数据
												//发送XML响应头
												cout << head_xml4 << endl;
												ret = tcptools->Send(newsockfd, head_xml4, strlen(head_xml4));

												if (ret <= 0)
												{
													cout << "发送数据失败，" << strerror(errno) << endl;
													goto RE_RECV;
												}

												//发送响应请求上下文信息
												sprintf(ret_buf, "<Simu_Plan  id = \"%s\" type = \"%d\" name=\"%s\" machine=\"%s\" user=\"%s\">\r\n",
														simu.plan_id, simu.type, simu.plan_name, simu.machine_name, simu.usr_id);

												cout << ret_buf << endl;
												ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

												if (ret <= 0)
												{
													cout << "发送数据失败，" << strerror(errno) << endl;
													goto RE_RECV;
												}

												//返回开关记录信息
												for (std::list<CHECK_DATA>::iterator it_chk = check_cbs.begin(); it_chk != check_cbs.end();it_chk++)
												{
													sprintf(ret_buf, "<Record id=\"%ld\" status=\"%d\" ia=\"%.2f\" ib=\"%.2f\" ic=\"%.2f\" p=\"%.2f\" q=\"%.2f\"/>\r\n",
															it_chk->cb_id, it_chk->status, it_chk->Ia, it_chk->Ib, it_chk->Ic, it_chk->p, it_chk->q);

													cout << ret_buf << endl;
													ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

													if (ret <= 0)
													{
														cout << "发送数据失败，" << strerror(errno) << endl;
														goto RE_RECV;
													}
												}

												//发送XML响应尾
												cout << tail_xml_simu << endl;
												ret = tcptools->Send(newsockfd, tail_xml_simu, strlen(tail_xml_simu));

												if (ret <= 0)
												{
													cout << "发送数据失败，" << strerror(errno) << endl;
													goto RE_RECV;
												}

											}
										}
									}
									else if (strcasecmp(Header, "fault_imitation_preexec") == 0)
									{
										SIMU_PLAN simu = {0};
										strcpy(simu.plan_id, bodyelement->Attribute("id"));
										strcpy(simu.machine_name, bodyelement->Attribute("machine"));
										strcpy(simu.usr_id, bodyelement->Attribute("user"));

										simu.type = atoi(bodyelement->Attribute("type"));

										//不想保存的临时方案，先保存，返回报文之后再删除
										if (strstr(simu.plan_id, "1970/01/01") != NULL)
										{
											//临时建立的方案
											sprintf(sql, "INSERT INTO PMS_PARAM.FDIR_SIG_SUMMARY(ID, TYPE, NAME, MACHINE, USERID, RUN_TIME, FAULT_INDEX) VALUES(\'%s\', %d, \'%s\', \'%s\', \'%s\', 1, -1)",
													simu.plan_id, 1, simu.plan_name, simu.machine_name, simu.usr_id);
											ExecSQL(sql);
										}

										//清空历史故障模拟信号
										simu_mac_trig.clear();

										TiXmlNode* simu_node = pbodynode->FirstChild();
										//循环查找Record项,并处理
										while (simu_node != NULL)
										{
											TiXmlElement* simu_element = simu_node->ToElement();

											if (strcasecmp(simu_element->Value(), "Record") == 0)
											{
												long sig_id = atol(simu_element->Attribute("id"));
												int type = atoi(simu_element->Attribute("type"));
												int status = atoi(simu_element->Attribute("status"));

												if (type == 1 && status == 1)
												{
													//保护信号动作
													PSBOB::PROTECT relay = {0};
													if (oodbread_rk(&relay, &sig_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("relaysig")), sizeof(PSBOB::PROTECT)) > 0)
													{
														FDIR_POINT_TRIG data_trig = {0};
														data_trig.trig_type = TRIG_TYPE_SIMU;
														PSBOB::DV psbob_dv = {0};
														if (GetDv(relay.st_id, "st", psbob_dv) == OO_FAIL)
														{
															TRACE("获取厂站%ld的馈线信息失败！\r\n", data_trig.alarm.st_id);
															continue;
														}

														data_trig.dv_id = psbob_dv.id;
														data_trig.alarm.key_id = sig_id;
														data_trig.damode = GetDvDamode(psbob_dv.id);

														if (data_trig.damode <= FDIR_MODE_FORBID)
														{
															TRACE("馈线%ld(%s)为禁止DA处理(%d)\r\n", psbob_dv.id, psbob_dv.name, psbob_dv.damode);
															continue;
														}

														unsigned long id = 0;
														KEY_STRU ks = {0};
														CCommon::long_to_keyid(relay.oo_dev, &ks);
														ks.field_id = 0;
														CCommon::keyid_to_long(&ks, &id);

														if (oodbread_rk(&data_trig.psbob_data.point, &id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(data_trig.psbob_data.point)) < 0)
														{
															TRACE("读取开关表关键字为%ld的记录失败！\r\n", id);
															continue;
														}

														TB_DESCR* tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("relaysig"));
														if (!(HasMask(tb_descr, "pnt_type", relay.pnt_type , MENU_TYPE_RELAY_SGZ) || HasMask(tb_descr, "pnt_type", relay.pnt_type , MENU_TYPE_RELAY_ACT)))
														{
															continue;
														}

														gettimeofday(&data_trig.tm_add, NULL);
														data_trig.alarm.dev = ALARM_SGZ;
														data_trig.alarm.name = relay.name;
														data_trig.simu_plan_id = simu.plan_id;

														std::string machine = simu.machine_name;
														simu_mac_trig[machine].push_back(data_trig);

														TRACE("模拟故障信号%ld已加入待处理故障信号列表！\r\n", data_trig.alarm.key_id);
													}
													else
													{
														TRACE("读取保护信号表关键字为%ld的记录失败！\r\n", sig_id);
													}
												}
												else if (type == 0 && status == 0)
												{
													//开关分闸
													PSBOB::CB cb = {0};
													if (oodbread_rk(&cb, &sig_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) > 0)
													{
														FDIR_POINT_TRIG data_trig = {0};
														data_trig.trig_type = TRIG_TYPE_SIMU;
														PSBOB::DV psbob_dv = {0};
														if (GetDv(cb.st_id, "st", psbob_dv) == OO_FAIL)
														{
															TRACE("获取厂站%ld的馈线信息失败！\r\n", data_trig.alarm.st_id);
															continue;
														}

														data_trig.dv_id = psbob_dv.id;
														data_trig.alarm.key_id = sig_id;
														data_trig.damode = GetDvDamode(psbob_dv.id);
														if (data_trig.damode <= FDIR_MODE_FORBID)
														{
															TRACE("馈线%ld(%s)为禁止DA处理(%d)\r\n", psbob_dv.id, psbob_dv.name, psbob_dv.damode);
															continue;
														}

														gettimeofday(&data_trig.tm_add, NULL);

														KEY_STRU ks = { 0 };
														CCommon::long_to_keyid(data_trig.alarm.key_id, &ks);
														ks.field_id = 0;
														CCommon::keyid_to_long(&ks, &data_trig.alarm.key_id);
														if (oodbread_rk(&data_trig.psbob_data.point, &data_trig.alarm.key_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(data_trig.psbob_data.point)) == OO_FAIL)
														{
															TRACE("读取开关表关键字为%ld的记录失败！\r\n", data_trig.alarm.key_id);
															continue;
														}

														data_trig.alarm.dev = ALARM_BREAKER;
														data_trig.alarm.name = data_trig.psbob_data.point.name;
														data_trig.simu_plan_id = simu.plan_id;
														std::string machine = simu.machine_name;
														simu_mac_trig[machine].push_back(data_trig);
														TRACE("模拟故障信号%ld已加入待处理故障信号列表！\r\n", data_trig.alarm.key_id);
													}
													else
													{
														TRACE("读取断路器表关键字为%ld的记录失败！\r\n", sig_id);
													}
												}
												else
												{
													TRACE("信号%ld,status:%d, type:%d被过滤！\r\n", sig_id, status, type);
												}
											}

											simu_node = simu_node->NextSibling();
										}

									}
									else if (strcasecmp(Header, "fault_imitation_exec") == 0)
									{
										if (strcasecmp(bodyelement->Value(), "Simu_Plan") == 0)
										{
											SIMU_PLAN simu = {0};
											strcpy(simu.plan_id, bodyelement->Attribute("id"));
											strcpy(simu.machine_name, bodyelement->Attribute("machine"));
											strcpy(simu.usr_id, bodyelement->Attribute("user"));
											simu.type = atoi(bodyelement->Attribute("type"));

											TiXmlNode* simu_node = pbodynode->FirstChild();
											//循环查找Record项,并处理
											while (simu_node != NULL)
											{
												TiXmlElement* simu_element = simu_node->ToElement();

												if (strcasecmp(simu_element->Value(), "Record") == 0)
												{
													long sig_id = atol(simu_element->Attribute("id"));
													int type = atoi(simu_element->Attribute("type"));
													int status = atoi(simu_element->Attribute("status"));

													//实时态转发为告警，研究态接收报文
													if (version == VERSION_REALTIME)
													{
														Message message = { 0 };
														RealDataPkg pkg={0};
														KEY_STRU ks = {0};
														CCommon::long_to_keyid(sig_id, &ks);
														if (ks.table_no == 407)
														{
															ks.field_id = 40;
														}
														else if(ks.table_no == 434)
														{
															ks.field_id = 20;
														}
														CCommon::keyid_to_long(&ks, &sig_id);

														ChangeYx yx = { sig_id, status, true, 222, -1 };

														memcpy(pkg.mes, &yx, sizeof(ChangeYx));
														pkg.package_head.data_num=1;
														pkg.package_head.package_type = MT_YC_CHANGE;
														pkg.package_head.second = time(NULL);
														pkg.package_head.msecond = 0;
														memcpy(message.Msg_buf, &pkg, sizeof(pkg));
														message.header.event = MT_YX_CHANGE;
														message.header.serv = D_CH_UP_REAL_DATA;
														cout << "bob_inv->messageSend,return:" << bob_inv->messageSend(&message, sizeof(message.header) +sizeof(yx), NULL) << endl;
													}
													else if(version == VERSION_STUDY)
													{
														if (type == 1 && status == 1)
														{
															//保护信号动作
															PSBOB::PROTECT relay = {0};
															if (oodbread_rk(&relay, &sig_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("relaysig")), sizeof(PSBOB::PROTECT)) > 0)
															{
																FDIR_POINT_TRIG data_trig = {0};
																data_trig.trig_type = TRIG_TYPE_SIMU;
																PSBOB::DV psbob_dv = {0};
																if (GetDv(relay.st_id, "st", psbob_dv) == OO_FAIL)
																{
																	TRACE("获取厂站%ld的馈线信息失败！\r\n", data_trig.alarm.st_id);
																	continue;
																}

																data_trig.dv_id = psbob_dv.id;
																data_trig.alarm.key_id = sig_id;
																data_trig.damode = GetDvDamode(psbob_dv.id);

																if (data_trig.damode <= FDIR_MODE_FORBID)
																{
																	TRACE("馈线%ld(%s)为禁止DA处理(%d)\r\n", psbob_dv.id, psbob_dv.name, psbob_dv.damode);
																	continue;
																}

																unsigned long id = 0;
																KEY_STRU ks = {0};
																CCommon::long_to_keyid(relay.oo_dev, &ks);
																ks.field_id = 0;
																CCommon::keyid_to_long(&ks, &id);

																if (oodbread_rk(&data_trig.psbob_data.point, &id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(data_trig.psbob_data.point)) < 0)
																{
																	TRACE("读取开关表关键字为%ld的记录失败！\r\n", id);
																	continue;
																}

																TB_DESCR* tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("relaysig"));
																if (!(HasMask(tb_descr, "pnt_type", relay.pnt_type , MENU_TYPE_RELAY_SGZ) || HasMask(tb_descr, "pnt_type", relay.pnt_type , MENU_TYPE_RELAY_ACT)))
																{
																	continue;
																}

																gettimeofday(&data_trig.tm_add, NULL);
																data_trig.alarm.dev = ALARM_SGZ;
																data_trig.alarm.name = relay.name;
																data_trig.simu_plan_id = simu.plan_id;

																//将该信号加入过流信号列表
																pthread_mutex_lock(&trig_list_mutex);
																g_lstTrig.push_back(data_trig);
																pthread_mutex_unlock(&trig_list_mutex);

																TRACE("模拟故障信号%ld已加入过流信号列表！\r\n", data_trig.alarm.key_id);
															}
															else
															{
																TRACE("读取保护信号表关键字为%ld的记录失败！\r\n", sig_id);
															}
														}
														else if (type == 0 && status == 0)
														{
															//开关分闸
															PSBOB::CB cb = {0};
															if (oodbread_rk(&cb, &sig_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) > 0)
															{
																FDIR_POINT_TRIG data_trig = {0};
																data_trig.trig_type = TRIG_TYPE_SIMU;
																PSBOB::DV psbob_dv = {0};
																if (GetDv(cb.st_id, "st", psbob_dv) == OO_FAIL)
																{
																	TRACE("获取厂站%ld的馈线信息失败！\r\n", data_trig.alarm.st_id);
																	continue;
																}

																data_trig.dv_id = psbob_dv.id;
																data_trig.alarm.key_id = sig_id;
																data_trig.damode = GetDvDamode(psbob_dv.id);
																if (data_trig.damode <= FDIR_MODE_FORBID)
																{
																	TRACE("馈线%ld(%s)为禁止DA处理(%d)\r\n", psbob_dv.id, psbob_dv.name, psbob_dv.damode);
																	continue;
																}

																gettimeofday(&data_trig.tm_add, NULL);

																KEY_STRU ks = { 0 };
																CCommon::long_to_keyid(data_trig.alarm.key_id, &ks);
																ks.field_id = 0;
																CCommon::keyid_to_long(&ks, &data_trig.alarm.key_id);
																if (oodbread_rk(&data_trig.psbob_data.point, &data_trig.alarm.key_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(data_trig.psbob_data.point)) == OO_FAIL)
																{
																	TRACE("读取开关表关键字为%ld的记录失败！\r\n", data_trig.alarm.key_id);
																	continue;
																}

																data_trig.alarm.dev = ALARM_BREAKER;
																data_trig.alarm.name = data_trig.psbob_data.point.name;
																data_trig.simu_plan_id = simu.plan_id;

																//将该开关加入过流开关列表
																pthread_mutex_lock(&trig_list_mutex);
																g_lstTrig.push_back(data_trig);
																pthread_mutex_unlock(&trig_list_mutex);
																TRACE("模拟故障信号%ld已加入过流信号列表！\r\n", data_trig.alarm.key_id);
															}
															else
															{
																TRACE("读取断路器表关键字为%ld的记录失败！\r\n", sig_id);
															}
														}
														else
														{
															TRACE("信号%ld,status:%d, type:%d被过滤！\r\n", sig_id, status, type);
														}
													}
													else
													{
													}

												}

												simu_node = simu_node->NextSibling();
											}

										}
									}
									else if (strcasecmp(Header, "pntcontrol") == 0) //故障隔离恢复遥控
									{
										if (strcasecmp(bodyelement->Value(), "operation") == 0)
										{
											TiXmlNode *pordernode = pbodynode->FirstChild();

											//循环查找order项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "order") == 0)
												{
													if (strcmp(porderelement->Attribute("op_type"), "finish") == 0)	//故障处理完成
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));
													}
													else if (strcmp(porderelement->Attribute("op_type"), "auto") == 0)//手工转自动
													{
//														if (!g_FdirConfig.USER2AUTO)
//														{
//															cout << "当前配置为禁止手工转自动" << endl;
//															continue;
//														}

														cout << "**********手动转自动执行" << endl;

//
														pthread_mutex_lock(&task_list_mutex);

														list<FDIR_TASK>::iterator it;
														for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
														{
															if (it->rq_type == TASK_TYPE_FAULT)
															{
																if (strcmp(it->task_data.fault.head.group_id, porderelement->Attribute("fault_id")) == 0)
																{
																	it->task_data.fault.bAuto = 1;
																	break;
																}
															}
														}

														//如果不在任务列表，把故障重新加载到内存中，然后再把故障详细加载进去
														if (it == g_lstTask.end())
														{
															RecoMemTask(porderelement->Attribute("fault_id"));
															RecoTaskStep(porderelement->Attribute("fault_id"));
															for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
															{
																if (it->rq_type == TASK_TYPE_FAULT)
																{
																	if (strcmp(it->task_data.fault.head.group_id, porderelement->Attribute("fault_id")) == 0)
																	{
																		it->task_data.fault.bAuto = 1;
																	}
																}
															}
														}

														list<CB_CONTROL> lstcb;

														GetLeftStep(porderelement->Attribute("fault_id"), lstcb);

														//还有步骤执行
														if (lstcb.size() > 0)
														{
															AddControlItem(porderelement->Attribute("fault_id"), lstcb.begin()->point_id, lstcb.begin()->bOpen, lstcb.begin()->stage);
														}
															//
														else
														{
														}

														pthread_mutex_unlock(&task_list_mutex);
													}
													else if(strcmp(porderelement->Attribute("op_type"), "stop") == 0)	//人工停止故障处理
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));
														//更新故障状态为人工停止故障处理,首先判断故障信息是否已经转入his表里
														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_STOP, porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_STOP, porderelement->Attribute("fault_id"));
															ExecSQL(sql);
														}

														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);
#if 0
														for (FDIR_TASK_LIST::iterator it_task = g_lstTask.begin(); it_task != g_lstTask.end(); it_task++)
														{
															if (it_task->rq_type == TASK_TYPE_FAULT)
															{
																if (strcmp(it_task->task_data.fault.head.group_id, porderelement->Attribute("fault_id")) == 0)
																{
																	g_lstTask.erase(it_task);
																	it_task = g_lstTask.begin();
																}
															}
															else if (it_task->rq_type == TASK_TYPE_CB_CTRL)
															{
																if (strcmp(it_task->task_data.control.head.group_id, porderelement->Attribute("fault_id")) == 0)
																{
																	g_lstTask.erase(it_task);
																	it_task = g_lstTask.begin();
																}
															}
															else
															{

															}
														}
#else
														RemoveTasks(porderelement->Attribute("fault_id"));
#endif
														pthread_mutex_unlock(&task_list_mutex);
													}
													else if(strcmp(porderelement->Attribute("op_type"), "manposer") == 0) //人工处理故障完成
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));

														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d, USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", STATUS_FDIRECT_MANPOSER,
																porderelement->Attribute("userName"), porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d, USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", STATUS_FDIRECT_MANPOSER,
																	porderelement->Attribute("userName"), porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
															ExecSQL(sql);

															//更新故障步骤状态为未知-1
															sprintf(sql, "UPDATE FDIR_STEP SET STATUS=%d WHERE ID=\'%s\'", STATUS_FDIRECT_UNKOWN, porderelement->Attribute("fault_id"));
															ExecSQL(sql);

															//更新故障步骤状态为未知-1
															sprintf(sql, "UPDATE FDIR_STEP_HIS SET STATUS=%d WHERE ID=\'%s\'", STATUS_FDIRECT_UNKOWN, porderelement->Attribute("fault_id"));
															ExecSQL(sql);
														}

														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);

														RemoveTasks(porderelement->Attribute("fault_id"));

														pthread_mutex_unlock(&task_list_mutex);

													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "yes_Fault") == 0) //误报、可能故障确认为真实故障
													{
														sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d, USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", STATUS_FDIRECT_DIRECT_OK,
																porderelement->Attribute("userName"), porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
														ExecSQL(sql);
													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "no_Fault") == 0) //误报、可能故障确认为非故障
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));

														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", porderelement->Attribute("userName"),
																porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															SetFdirectStatus(porderelement->Attribute("fault_id"));
														}
														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);

														RemoveTasks(porderelement->Attribute("fault_id"));

														pthread_mutex_unlock(&task_list_mutex);
													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "invalid") == 0)	//故障执行步骤失效
													{
														sprintf(sql, "UPDATE FDIR_STEP SET STATUS=%d, STARTTIME=SYSDATE, ENDTIME=SYSDATE WHERE ID=\'%s\' AND STAGE=\'%s\' AND CB_ID=\'%s\'",
																STATUS_FDIR_STEP_INVALID, porderelement->Attribute("fault_id"), porderelement->Attribute("stage"), porderelement->Attribute("cb"));
														ExecSQL(sql);
														ClearFdirDB(porderelement->Attribute("fault_id"));
														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_INVALID, porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_INVALID, porderelement->Attribute("fault_id"));
															ExecSQL(sql);
														}

														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);

														RemoveTasks(porderelement->Attribute("fault_id"));

														pthread_mutex_unlock(&task_list_mutex);
													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "pass") == 0)		//跳过指定遥控步骤
													{
														SetPassControl(porderelement->Attribute("fault_id"), atol(porderelement->Attribute("cb")), STATUS_FDIR_STEP_PASS,
																	   atoi(porderelement->Attribute("stage")));
														UpdateExtStatus(porderelement->Attribute("fault_id"));

														//置处理完成标志
														list<CB_CONTROL> lstCB;

														if (GetLeftStep(porderelement->Attribute("fault_id"), lstCB) == 0)
														{
															ClearFdirDB(porderelement->Attribute("fault_id"));
															sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, porderelement->Attribute("fault_id"));
															if (ExecSQL(sql) != OCI_ERROR)
															{
																sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, porderelement->Attribute("fault_id"));
																ExecSQL(sql);
															}

															RemoveTasks(porderelement->Attribute("fault_id"));

															printf("故障组%s处理完成！！！！\r\n", porderelement->Attribute("fault_id"));
														}
													}
													else
													{
														list<FDIR_TASK>::iterator it;
														for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
														{
															if (it->rq_type == TASK_TYPE_FAULT)
															{
																if (strcmp(it->task_data.fault.head.group_id, porderelement->Attribute("fault_id")) == 0)
																{
																	break;
																}
															}
														}

														//如果不在任务列表，把故障重新加载到内存中，然后再把故障详细加载进去
														if (it == g_lstTask.end())
														{
															RecoMemTask(porderelement->Attribute("fault_id"));
															RecoTaskStep(porderelement->Attribute("fault_id"));

														}

														if (strcasecmp("realtime", bodyelement->Attribute("contextname")) == 0)
														{
															pthread_mutex_lock(&task_list_mutex);
															AddReturnControl(porderelement->Attribute("fault_id"), atol(porderelement->Attribute("cb")), atoi(porderelement->Attribute("op_type")) - 1,
																			 atoi(porderelement->Attribute("stage")));
															pthread_mutex_unlock(&task_list_mutex);
														}
														else
														{
															pthread_mutex_lock(&task_list_mutex);
															AddControlItem(porderelement->Attribute("fault_id"), atol(porderelement->Attribute("cb")), atoi(porderelement->Attribute("op_type")) - 1,
																		   atoi(porderelement->Attribute("stage")));
															pthread_mutex_unlock(&task_list_mutex);
														}
													}
												}

												pordernode = pordernode->NextSibling();
											}
										}
									}
										//防误闭锁判断
									else if (strcasecmp(Header, "misoperation") == 0)
									{
										if (strcasecmp(bodyelement->Value(), "operation") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											//更新开关状态,并根据母线的遥测信息添加SO
											if (nmd->update_cb_state() && nmd->add_so()/* && nmd->DoValidate() && g_bob2->WriteReDB(NULL, NULL, NULL)*/)
											{
												TiXmlNode *pordernode = pbodynode->FirstChild();

												//循环查找Record项,并处理
												while (pordernode != NULL)
												{
													TiXmlElement *porderelement = pordernode->ToElement();

													if (strcasecmp(porderelement->Value(), "order") == 0)
													{
														DATA_MISOPERATION data_mis;
														int op_type = atoi(porderelement->Attribute("op_type"));

														nmd->find_so(atol(porderelement->Attribute("cb")), "breaker", paths, FIND_TYPE_MISOPERAT, &data_mis);

														cout << "*******DV1******" << endl;
														{
															for (list<PSBOB::DV>::iterator it = data_mis.dv1.begin(); it != data_mis.dv1.end(); it++)
															{
																cout << it->id << "," << it->name << endl;
															}
														}
														cout << "*******LD1******" << endl;
														{
															for (list<CLS_LD>::iterator it = data_mis.lst_ld1.begin(); it != data_mis.lst_ld1.end(); it++)
															{
																cout << it->id << "," << it->descr << endl;
															}
														}
														cout << "*******DV2******" << endl;
														{
															for (list<PSBOB::DV>::iterator it = data_mis.dv2.begin(); it != data_mis.dv2.end(); it++)
															{
																cout << it->id << "," << it->name << endl;
															}
														}
														cout << "*******LD2******" << endl;
														{
															for (list<CLS_LD>::iterator it = data_mis.lst_ld2.begin(); it != data_mis.lst_ld2.end(); it++)
															{
																cout << it->id << "," << it->descr << endl;
															}
														}

														int Result = -1;
														bool bhasdv[2] = {false, false};
														list<CLS_LD> lst_ld;	//充电或失电的LD

														bhasdv[0] = data_mis.dv1.size() > 0 ? true : false;
														bhasdv[1] = data_mis.dv2.size() > 0 ? true : false;

														if (op_type == 1) //合闸操作
														{
															if (bhasdv[0]) //一端有电源
															{
																if (bhasdv[1]) Result = 1; //另一端也有电源，合环了
																else
																{
																	Result = 3; //另一端无电源，充电

																	lst_ld = data_mis.lst_ld2;
																}
															}
															else //一端无电源
															{
																if (bhasdv[1])
																{
																	Result = 3; //另一端有电源， 充电了

																	lst_ld = data_mis.lst_ld1;
																}
																else ; //另一端也无电源，啥都不是
															}
														}
														else if (op_type == 2) //分闸操作
														{
															if (bhasdv[0]) //一端有电源
															{
																if (bhasdv[1]) Result = 2; //另一端也有电源，解环了
																else
																{
																	Result = 4; //另一端无电源，失电

																	lst_ld = data_mis.lst_ld2;
																}
															}
															else //一端无电源
															{
																if (bhasdv[1])
																{
																	Result = 4; //另一端有电源， 失电了

																	lst_ld = data_mis.lst_ld1;
																}
																else
																	; //另一端也无电源，啥都不是
															}
														}
														else	//其它操作无效
														{
															cout << "无效操作！" << endl;
															nmd->m_Errors.push_back("无效操作！");
														}

														//发送请求响应数据
														//发送XML响应头
														cout << head_xml2 << endl;
														ret = tcptools->Send(newsockfd, head_xml2, strlen(head_xml2));
														if (ret <= 0)
														{
															cout << "发送数据失败，" << strerror(errno) << endl;
															goto RE_RECV;
														}

														//发送响应请求上下文信息
														sprintf(ret_buf, "<operation pictureName=\"%s\" contextname=\"%s\" bobname=\"%s\">\r\n<order cb=\"%s\" op_type=\"%d\" />\r\n</operation>\r\n",
																order.picturename, order.contextname, order.bobname, porderelement->Attribute("cb"), op_type);

														cout << ret_buf << endl;
														ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));
														if (ret <= 0)
														{
															cout << "发送数据失败，" << strerror(errno) << endl;
															goto RE_RECV;
														}

														//发送错误信息
														string strError = "<ErrorMsg>\r\n";

														//逐条处理错误信息
														{
															for (list<string>::iterator it = nmd->m_Errors.begin(); it != nmd->m_Errors.end(); it++)
															{
																strError += "<error msg=\"";//" />
																strError += *it;
																strError += "\" />";
																strError += "\r\n";
															}
														}
														strError += "</ErrorMsg>\r\n";

														cout << strError.c_str() << endl;

														ret = tcptools->Send(newsockfd, strError.c_str(), strError.size());
														if (ret <= 0)
														{
															cout << "发送数据失败，" << strerror(errno) << endl;
															goto RE_RECV;
														}

														//有合解环数据
														if (Result != -1)
														{
															//发送合解环结果
															sprintf(ret_buf, "<Result type=\"%d\" level=\"1\">\r\n", Result);
															cout << ret_buf << endl;
															ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));
															if (ret <= 0)
															{
																cout << "发送数据失败，" << strerror(errno) << endl;
																goto RE_RECV;
															}

															//发送设备信息
															switch (Result)
															{
																//合、解环
																case 1:
																case 2:
																{
																	list<PSBOB::DV>::iterator it;

																	for (it = data_mis.dv1.begin(); it != data_mis.dv1.end(); it++)
																	{
																		sprintf(ret_buf, "<Record id=\"%ld\" descr=\"%s\" tablename=\"subcontrolarea\" />\r\n", it->id, it->name);
																		cout << ret_buf << endl;
																		ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));
																		if (ret <= 0)
																		{
																			cout << "发送数据失败，" << strerror(errno) << endl;
																			goto RE_RECV;
																		}

																	}

																	for (it = data_mis.dv2.begin(); it != data_mis.dv2.end(); it++)
																	{
																		sprintf(ret_buf, "<Record id=\"%ld\" descr=\"%s\" tablename=\"subcontrolarea\" />\r\n", it->id, it->name);
																		cout << ret_buf << endl;
																		ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));
																		if (ret <= 0)
																		{
																			cout << "发送数据失败，" << strerror(errno) << endl;
																			goto RE_RECV;
																		}
																	}
																	break;
																}
																	//充、失电
																case 3:
																case 4:
																{
																	for (list<CLS_LD>::iterator it = lst_ld.begin(); it != lst_ld.end(); it++)
																	{
																		sprintf(ret_buf, "<Record id=\"%ld\" descr=\"%s\" tablename=\"energyconsumer\" />\r\n", it->id, it->descr);
																		cout << ret_buf << endl;
																		ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));
																		if (ret <= 0)
																		{
																			cout << "发送数据失败，" << strerror(errno) << endl;
																			goto RE_RECV;
																		}
																	}
																	break;
																}
																default:
																	break;
															}

															strcpy(ret_buf, "</Result>\r\n");
															cout << ret_buf << endl;
															ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));
															if (ret <= 0)
															{
																cout << "发送数据失败，" << strerror(errno) << endl;
																goto RE_RECV;
															}
														}
														//发送XML响应尾
														cout << tail_xml2 << endl;
														ret = tcptools->Send(newsockfd, tail_xml2, strlen(tail_xml2));

														if (ret <= 0)
														{
															cout << "发送数据失败，" << strerror(errno) << endl;
															goto RE_RECV;
														}
													}

													pordernode = pordernode->NextSibling();
												}
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
									}
									else if (strcasecmp(Header, "fhzg") == 0)
									{
										char cb_id[MAX_CB_ID_LEN] = { 0 };
										char fhzg_id[50] = { 0 };
										char error[500] = { 0 };
										int ret_err = -1;

										if (strcasecmp(bodyelement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));
											strcpy(order.st, bodyelement->Attribute("st"));

											TiXmlNode *pordernode = pbodynode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													timeval tval = { 0 };
													strcpy(cb_id, porderelement->Attribute("id"));

													gettimeofday(&tval, NULL);

													tm *ptm = localtime(&tval.tv_sec);
													sprintf(fhzg_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
															tval.tv_usec);

													if (strcasecmp("busbarsection", porderelement->Attribute("tablename")) == 0)
													{
														list<list<MAP_SO_NODE> > fdir_steps;

														FDIR_R2 *fdir_r2 = new FDIR_R2(g_bob);

														list<long> lstIsolate;

														if (fdir_r2->FHZG_Bus(atol(porderelement->Attribute("id")), fdir_steps, lstIsolate) == FDIR_R_FAIL)
														{
															strcpy(error, "没有联络电源，转供失败！");
															ret_err = -1;
														}
														else
														{
															list<FAULT_ITEM> lst;
															write_fhzg_step(fhzg_id, "", fdir_steps, fdir_r2, lst, lstIsolate,true);
															ret_err = 0;
														}

														delete fdir_r2;
													}
													else if (strcasecmp("breaker", porderelement->Attribute("tablename")) == 0)
													{
														FHZG *fhzg = new FHZG(g_bob);

														int fhzg_ret = fhzg->Do(atol(porderelement->Attribute("id")));

														switch (fhzg_ret)
														{
															case 0: //转供成功
															{
																//写负荷转供关系库表
																fill_fhzg_db(atol(cb_id), fhzg_id, fhzg, true);
																sprintf(error, "%f", fhzg->fhzg_capacity);
																ret_err = 0;
																break;
															}

															case FHZG_FAIL_NO_SO:
															{
																strcpy(error, "没有可供转供的电源，转供失败！");
																ret_err = -1 ;
																break;
															}

															case FHZG_FAIL_SO_FULL:
															{
																strcpy(error, "所有可转供的电源剩余容量不足，转供失败！");
																ret_err = -1;
																break;
															}
															case FHZG_FAIL_NO_IVALUE:
															{
																strcpy(error, "没有获取到要转供开关的电流值，转供失败！");
																ret_err = -1;
																break;
															}
														}

														delete fhzg;
													}

													else
													{
														TRACE("不支持的设备类型%s\r\n", porderelement->Attribute("tablename"));
													}
												}

												pordernode = pordernode->NextSibling();
											}
										}
										else
										{
										}

										//发送请求响应数据
										//发送XML响应头
										cout << head_xml3 << endl;
										ret = tcptools->Send(newsockfd, head_xml3, strlen(head_xml3));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送响应请求上下文信息
										sprintf(ret_buf, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" >\r\n", order.picturename, order.contextname, order.bobname,
												order.st);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										sprintf(ret_buf, "<Record id = \"%s\" tablename = \"cb\" fhzg_id=\"%s\" result=\"%d\" error=\"%s\"/>\r\n", cb_id, fhzg_id, ret_err, error);

										//发送故障转供标识符
										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));
										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送XML响应尾
										cout << tail_xml << endl;
										ret = tcptools->Send(newsockfd, tail_xml, strlen(tail_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}
									}

									else if(strcasecmp(Header, "typical") == 0)
									{
										//D5000 典型运行方式暂未移植
										char userid[100] = { 0 };
										char machine[100] = { 0 };
										char id[50] = { 0 };
										char str_time[50] = { 0 };
										char ch_time[40] = { 0 };
										time_t res_time;
										string dv_id;
										int result = -1;

										if (strcasecmp(bodyelement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));
											strcpy(machine, bodyelement->Attribute("machine"));
											strcpy(userid, bodyelement->Attribute("userid"));

											int ret = FDIR_R_FAIL;
											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											TiXmlNode *pordernode = pbodynode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													cmd = atoi(porderelement->Attribute("operate_type"));
													switch (cmd)
													{
														case 0: //保存典型方式
														{
															timeval tval = { 0 };
															gettimeofday(&tval, NULL);
															tm *ptm = localtime(&tval.tv_sec);
															sprintf(id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
																	tval.tv_usec);
															typical *typi = new typical(g_fdir_r, id);

															if(typi->typical_save(atol(porderelement->Attribute("cb_id")), machine, userid))
															{
																result = 0;
															}

															break;
														}
														case 1: //恢复典型方式
														{
															typical *typi = new typical(g_fdir_r, id);

															if(typi->typical_restore(atol(porderelement->Attribute("cb_id")), machine, userid, dv_id, res_time))
															{
																result = 0;
															}
															struct tm tmtime;
															localtime_r(&res_time,&tmtime);
															sprintf(ch_time,"%04d/%02d/%02d %02d:%02d:%02d",(tmtime.tm_year + 1900),(tmtime.tm_mon + 1), tmtime.tm_mday, tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec);
															strcpy(id, dv_id.c_str());
															break;
														}

														default:
															break;
													}
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										//发送请求响应数据
										//发送XML响应头
										cout << head_xml6 << endl;
										ret = tcptools->Send(newsockfd, head_xml6, strlen(head_xml6));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送响应请求上下文信息
										sprintf(ret_buf, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  userid = \"%s\" machine=\"%s\">\r\n", order.picturename,
												order.contextname, order.bobname, userid, machine);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//逐项发送设备列表里的设备信息
										sprintf(ret_buf, "<Record id = \"%s\" time = \"%s\" result = \"%d\" error=\"\"/>\r\n", id, ch_time, result);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送XML响应尾
										cout << tail_xml << endl;
										ret = tcptools->Send(newsockfd, tail_xml, strlen(tail_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

									}
									else if (strcasecmp(Header, "check_data") == 0)
									{
										char userid[100] = { 0 };
										char machine[100] = { 0 };
										char groupid[100] = { 0 };
										const char *errmsg[] = { "校验通过，数据一致", "实时库版本号变化", "故障相关开关状态发生变化", "读取保存开关状态的数据库失败", "解析报文中groupid失败" };
										if (strcasecmp(bodyelement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));
											strcpy(machine, bodyelement->Attribute("machine"));
											strcpy(userid, bodyelement->Attribute("userid"));

											int ret = FDIR_R_FAIL;
											//加多请求线程互斥锁
											pthread_mutex_lock (&g_rq_mutex);

											TiXmlNode *pordernode = pbodynode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													strcpy(groupid, porderelement->Attribute("fault_id"));

												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										int result = CHECK_DATA_ID_ERR;
										if (strlen(groupid) != 0)
										{
											//校验数据是否跟故障时一致，如果一致返回0，实时库版本号变化返回1，如果相关数据发生变化返回2，读库失败返回3
											result = data_verify(groupid);
											if (result == CHECK_DATA_CB_ERR)
											{
												//方案已经失效，设置故障标识为失效状态
												sprintf(sql, "UPDATE FDIR_FDIRECT SET EXT_STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_INVALID, groupid);
												ExecSQL(sql);
											}
											else if (result == CHECK_DATA_PSBOB_ERR)
											{
												//实时库版本号变化设置故障标识为版本号变化
												sprintf(sql, "UPDATE FDIR_FDIRECT SET EXT_STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_VERSION_ERR, groupid);
												ExecSQL(sql);
											}
										}
										//发送请求响应数据
										//发送XML响应头
										cout << head_xml7 << endl;
										ret = tcptools->Send(newsockfd, head_xml7, strlen(head_xml7));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送响应请求上下文信息
										sprintf(ret_buf, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  userid = \"%s\" machine=\"%s\">\r\n", order.picturename,
												order.contextname, order.bobname, userid, machine);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//逐项发送设备列表里的设备信息
										sprintf(ret_buf, "<Record id = \"%s\" result = \"%d\" error=\"%s\"/>\r\n", groupid, result, errmsg[result]);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送XML响应尾
										cout << tail_xml << endl;
										ret = tcptools->Send(newsockfd, tail_xml, strlen(tail_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}
									}
									else if (strcasecmp(Header, "recalc") == 0)
									{
										//方案失效，重新计算隔离和恢复方案

										char userid[100] = { 0 };
										char machine[100] = { 0 };
										char groupid[100] = { 0 };
										if (strcasecmp(bodyelement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, bodyelement->Attribute("contextname"));
											strcpy(order.bobname, bodyelement->Attribute("bobname"));
											strcpy(order.picturename, bodyelement->Attribute("pictureName"));
											strcpy(machine, bodyelement->Attribute("machine"));
											strcpy(userid, bodyelement->Attribute("userid"));

											int ret = FDIR_R_FAIL;
											//加多请求线程互斥锁
											pthread_mutex_lock (&g_rq_mutex);

											TiXmlNode *pordernode = pbodynode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													strcpy(groupid, porderelement->Attribute("fault_id"));
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										int result = -1;
										char errmsg[50] = { 0 };

										if (strlen(groupid) != 0)
										{

											pthread_mutex_lock(&task_list_mutex);

											list<FDIR_TASK>::iterator it;
											for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
											{
												if (it->rq_type == TASK_TYPE_FAULT)
												{
													if (strcmp(it->task_data.fault.head.group_id, groupid) == 0)
													{
														break;
													}
												}
											}

											//如果不在任务列表，把故障重新加载到内存中，然后再把故障详细加载进去
											if (it == g_lstTask.end())
											{
												RecoMemTask(groupid);
												RecoTaskStep(groupid);
											}
											pthread_mutex_unlock(&task_list_mutex);

											if (invalid_recalc(groupid))
											{
												result = 0;
											}
										}
										else
										{
											sprintf(errmsg, "报文中解析groupid有问题！");
										}
										//发送请求响应数据
										//发送XML响应头
										cout << head_xml8 << endl;
										ret = tcptools->Send(newsockfd, head_xml8, strlen(head_xml8));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送响应请求上下文信息
										sprintf(ret_buf, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  userid = \"%s\" machine=\"%s\">\r\n", order.picturename,
												order.contextname, order.bobname, userid, machine);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//逐项发送设备列表里的设备信息
										sprintf(ret_buf, "<Record id = \"%s\" result = \"%d\" error=\"%s\"/>\r\n", groupid, result, errmsg);

										cout << ret_buf << endl;
										ret = tcptools->Send(newsockfd, ret_buf, strlen(ret_buf));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

										//发送XML响应尾
										cout << tail_xml << endl;
										ret = tcptools->Send(newsockfd, tail_xml, strlen(tail_xml));

										if (ret <= 0)
										{
											cout << "发送数据失败，" << strerror(errno) << endl;
											goto RE_RECV;
										}

									}
									else
									{
										TRACE("不识别的类型[%s]\r\n", Header);
									}

									pbodynode = pbodynode->NextSibling();
								}
							}
							else
							{
							}

							pmsgnode = pmsgnode->NextSibling();
						}
					}
					else
					{
					}
				}

			}
			else
			{
				cout << "请求报文XML语法错误！" << endl;
			}
			timeval etimer = {0};

			gettimeofday(&etimer, NULL);

			float cost = etimer.tv_sec * 1000 * 1000 + etimer.tv_usec - (stimer.tv_sec * 1000 * 1000 + stimer.tv_usec);

			printf(">>>>>>>>%sprocessed, total cost:%.2fs\r\n", ctime(&(now = time(NULL))), cost / 1000 / 1000);
		}

	} //while(GDI_TRUE)


	printf("********** Exit Thread (%d) **********\n", newsockfd);

	close(newsockfd);

	delete pXmlDoc;

	int exitThread = -1;

	for (int i = 0; i < MAX_THREAD; i++)
	{
		if (pthread_id[i].fdinThread == newsockfd)
		{
			exitThread = i;
			pthread_id[i].fdinThread = -1;
			break;
		}
	}

	pthread_exit(0);
}

void fill_fhzg_db(const long cb_id, const char *fhzg_id, FHZG *fhzg, bool bhas_fhzg_cb)
{
	assert(fhzg != NULL);

	int fhzg_type = fhzg->GetType();

	if (fhzg_type > 0)
	{
		//写消减负荷表
		const LIST_TREENODE *lst_cut_zn = fhzg->GetLdCut();
		int istep = 0; //开关操作步骤序号
		const list<long> *cut_step = fhzg->GetLdCutOperations();
		map<int, list<CLS_LD> >::iterator it_map_zn_ld;
		char buf[50];

		for (LIST_TREENODE::const_iterator it_cut_zn = lst_cut_zn->begin(); it_cut_zn != lst_cut_zn->end(); it_cut_zn++)
		{
			char zone_cb_id[500] = { 0 };

			list<long> lst_zone_cb;

			for (it_map_zn_ld = (*it_cut_zn)->data.node_zn.map_zn_ld.begin(); it_map_zn_ld != (*it_cut_zn)->data.node_zn.map_zn_ld.end(); it_map_zn_ld++)
			{
//				zone_cb_id[0] = 0;
//
//				fhzg->GetZoneCB(it_map_zn_ld->first, lst_zone_cb);
//
//				for (list<long>::iterator it_zone_cb = lst_zone_cb.begin(); it_zone_cb != lst_zone_cb.end(); it_zone_cb++)
//				{
//					if (zone_cb_id[0] != 0)
//						strcat(zone_cb_id, ";");
//
//					sprintf(buf, "%ld", *it_zone_cb);
//					strcat(zone_cb_id, buf);
//				}
//
//				sprintf(sql, "Insert into FHZG_CUT_ZONE (ID, IZONE, CB_ID) Values (\'%s\', %ld, \'%s\')", fhzg_id, it_map_zn_ld->first, zone_cb_id);
//				ExecSQL(sql);
				list<FAULT_ITEM> lst_dev;
				fhzg->get_zone_devs(it_map_zn_ld->first, lst_dev);
				for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
				{
					sprintf(sql, "Insert into FHZG_CUT_ZONE (ID, IZONE, DEV_ID, DEV_TABLE) Values (\'%s\', %d, \'%ld\', \'%s\')", fhzg_id, it_map_zn_ld->first, it_devs->eq_id, it_devs->table.c_str());
					ExecSQL(sql);
				}

				list<CLS_LD>::const_iterator it_ld;

				for (it_ld = it_map_zn_ld->second.begin(); it_ld != it_map_zn_ld->second.end(); it_ld++)
				{
					sprintf(sql, "Insert into FHZG_LOAD (ID, LD_ID, LD_DESCR, LD_PRIOR, LD_AMOUNT, IZONE) Values (\'%s\', \'%ld\', \'%s\', %d, %f, %d)", fhzg_id, it_ld->pid, it_ld->descr, (*it_cut_zn)->data.node_zn.prio, it_ld->w, it_map_zn_ld->first);

					ExecSQL(sql);
				}
			}
		}
		switch (fhzg_type)
		{
			case FHZG_TYPE_ALL: //独立转供
			{
				cout << "独立转供……" << endl;

				//写可提供独立转供的电源表
				const LIST_TREENODE *lst_so = fhzg->GetSO();

				LIST_TREENODE::const_iterator it;

				for (it = lst_so->begin(); it != lst_so->end(); it++)
				{
					long so_name = (*it)->data.node_so.so_name;
					long cb_to_so = (*it)->data.node_so.cb_id;

					id_to_pid(so_name);
					id_to_pid(cb_to_so);

					sprintf(sql, "Insert into FHZG_SO (ID, SO_ID, SO_DESCR, TYPE, FULL_CAPACITY, LEFT_CAPACITY, CB_ID, ORG_WLOSS, NEW_WLOSS,ZG_CAPACITY) Values (\'%s\', \'%ld\', \'%s\', %d, %f, %f, \'%ld\', %f, %f, %f)", fhzg_id, so_name, (*it)->data.node_so.so_descr, 1/*独立转供*/,
							(*it)->data.node_so.full_capacity, (*it)->data.node_so.left_capacity, cb_to_so, (*it)->data.node_so.org_wloss, (*it)->data.node_so.new_wloss,fhzg->fhzg_capacity);

					ExecSQL(sql);
					istep = 0;
					for (list<long>::const_iterator it_cut_step = cut_step->begin(); it_cut_step != cut_step->end(); it_cut_step++)
					{
						long id = *it_cut_step;

						id_to_pid(id);

						sprintf(sql, "Insert into FHZG_STEP (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values ('%s', '%ld', %d, %d, 0, '%ld', 1)", fhzg_id, *it_cut_step, 1, istep, so_name);
						ExecSQL(sql);
						istep++;
					}

					//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
				sprintf(sql, "Insert into FHZG_SO_ZONE (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE, CB_TO_SO) ");
				memset(sqltemp, 0, sizeof(sqltemp));
				for (list<int>::iterator it_lst_so_zones = (*it)->data.node_so.lst_zones.begin(); it_lst_so_zones != (*it)->data.node_so.lst_zones.end(); it_lst_so_zones++)
				{
					list<FAULT_ITEM> lst_dev;

					fhzg->get_zone_devs(*it_lst_so_zones, lst_dev);
					for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
					{
						strcat(sql, sqltemp);
						sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', \'%s\', \'%s\' from dual union all ", fhzg_id, *it_lst_so_zones, so_name, it_devs->id.c_str(), it_devs->table.c_str(), cb_to_so);
					}
				}
				strncat(sql, sqltemp, (strlen(sqltemp)-10));
				ExecSQL(sql);
#else
					for (list<long>::iterator it_lst_so_zones = (*it)->data.node_so.lst_zones.begin(); it_lst_so_zones != (*it)->data.node_so.lst_zones.end(); it_lst_so_zones++)
					{
						list<FAULT_ITEM> lst_dev;

						fhzg->get_zone_devs(*it_lst_so_zones, lst_dev);

						for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
						{
							sprintf(sql, "Insert into FHZG_SO_ZONE (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) Values (\'%s\', %ld, \'%ld\', \'%ld\', \'%s\')", fhzg_id, *it_lst_so_zones, so_name, it_devs->eq_id, it_devs->table.c_str());
							ExecSQL(sql);
						}
					}
#endif
					//设置边界开关
					sprintf(sql, "update fhzg_so_zone set edge = 1 where id = \'%s\' and so_id = \'%ld\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FHZG_SO_ZONE where id = \'%s\' and dev_table='CB' and so_id = \'%ld\' group by dev_id) where num =1)", fhzg_id,
							so_name, fhzg_id, so_name);
					ExecSQL(sql);

					if (bhas_fhzg_cb)
					{
						sprintf(sql, "Insert into FHZG_STEP (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values (\'%s\', \'%ld\', %d, %d, 0, \'%ld\', 1)", fhzg_id, cb_id, 1, istep, so_name);
						ExecSQL(sql);
					}

					sprintf(sql, "Insert into FHZG_STEP (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values (\'%s\', \'%ld\', %d, %d, 0, \'%ld\', 1)", fhzg_id, cb_to_so, 2, istep + 1, so_name);
					ExecSQL(sql);

				}

				const LIST_TREENODE *lst_down_zone = fhzg->GetDownZones();

				for (it = lst_down_zone->begin(); it != lst_down_zone->end(); it++)
				{
					list<FAULT_ITEM> lst_dev;

					for (it_map_zn_ld = (*it)->data.node_zn.map_zn_ld.begin(); it_map_zn_ld != (*it)->data.node_zn.map_zn_ld.end(); it_map_zn_ld++)
					{
						fhzg->get_zone_devs(it_map_zn_ld->first, lst_dev);

						//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
					sprintf(sql, "Insert into FHZG_ZONE (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) ");
					memset(sqltemp, 0, sizeof(sqltemp));
					for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
					{
						strcat(sql, sqltemp);
						sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', \'%s\' from dual union all ", fhzg_id, it_map_zn_ld->first, "", it_devs->id.c_str(), it_devs->table.c_str());
					}
					strncat(sql, sqltemp, (strlen(sqltemp)-10));
					ExecSQL(sql);

#else
						for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
						{
							sprintf(sql, "Insert into FHZG_ZONE (ID, IZONE, DEV_ID, DEV_TABLE) Values (\'%s\', %d, \'%ld\', \'%s\')", fhzg_id, it_map_zn_ld->first, it_devs->eq_id, it_devs->table.c_str());
							ExecSQL(sql);
						}
#endif
						list<CLS_LD>::const_iterator it_ld;

						//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
					if(it_map_zn_ld->second.size() > 0)
					{
						sprintf(sql, "Insert into FHZG_LOAD (ID, LD_ID, LD_DESCR, LD_PRIOR, LD_AMOUNT, IZONE) ");
						memset(sqltemp, 0, sizeof(sqltemp));
						for (it_ld = it_map_zn_ld->second.begin(); it_ld != it_map_zn_ld->second.end(); it_ld++)
						{
							strcat(sql, sqltemp);
							sprintf(sqltemp, " select \'%s\', \'%s\', \'%s\', %d, %f, %d from dual union all ", fhzg_id, it_ld->id,
									it_ld->descr, (*it)->data.node_zn.prio, it_ld->w, it_map_zn_ld->first);
						}
						strncat(sql, sqltemp, (strlen(sqltemp)-10));
						ExecSQL(sql);
					}
#else
						for (it_ld = it_map_zn_ld->second.begin(); it_ld != it_map_zn_ld->second.end(); it_ld++)
						{
							sprintf(sql, "Insert into FHZG_LOAD (ID, LD_ID, LD_DESCR, LD_PRIOR, LD_AMOUNT, IZONE) Values (\'%s\', \'%ld\', \'%s\', %d, %f, %d)", fhzg_id, it_ld->pid, it_ld->descr, (*it)->data.node_zn.prio, it_ld->w, it_map_zn_ld->first);

							ExecSQL(sql);
						}
#endif
					}
				}

				sprintf(sql, "update fhzg_zone set edge = 1 where id = \'%s\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FHZG_ZONE where id = \'%s\' and dev_table='CB' group by dev_id) where num =1)", fhzg_id, fhzg_id);
				ExecSQL(sql);

				break;
			}

			case FHZG_TYPE_SHARE:
			{
				cout << "分摊转供……" << endl;

				//写可提供分摊转供的电源表
//			const LIST_TREENODE *lst_so = fhzg->GetSO();
//
//			LIST_TREENODE::const_iterator it;

				const map<TreeNode<DataType>*, LIST_TREENODE> *m_so_zone = fhzg->GetSoFhzgZone();
				map<TreeNode<DataType>*, LIST_TREENODE>::const_iterator it_m_so_zone;

				//写操作步骤表
				const list<_OPERATION_STEP> *opera_step = fhzg->GetOperation();

				list<_OPERATION_STEP>::const_iterator it_step;

				for (list<long>::const_iterator it_cut_step = cut_step->begin(); it_cut_step != cut_step->end(); it_cut_step++)
				{
					long id = *it_cut_step;

					id_to_pid(id);

					sprintf(sql, "Insert into FHZG_STEP (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values ('%s', '%ld', %d, %d, 0, 0, 1)", fhzg_id, id, 1, istep);
					ExecSQL(sql);
					istep++;
				}

				map<long, long> m_tmp_close_steps;
				map<long, long>::iterator it_m_close;
				for (it_step = opera_step->begin(); it_step != opera_step->end(); it_step++, istep++)
				{
					long id = it_step->open_cb;

					id_to_pid(id);

					sprintf(sql, "Insert into FHZG_STEP (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values ('%s', '%ld', %d, %d, 0, '%ld', 1)", fhzg_id, id, 1, istep, it_step->so_id);
					ExecSQL(sql);
					istep++;
					m_tmp_close_steps[it_step->close_cb] = it_step->so_id;
				}

				for (it_m_close = m_tmp_close_steps.begin(); it_m_close != m_tmp_close_steps.end(); it_m_close++)
				{
					long id = it_m_close->first;

					id_to_pid(id);

					sprintf(sql, "Insert into FHZG_STEP (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values ('%s', '%ld', %d, %d, 0, '%ld', 1)", fhzg_id, id, 2, istep, it_m_close->second);
					ExecSQL(sql);
					istep++;
				}


				for (it_m_so_zone = m_so_zone->begin(); it_m_so_zone != m_so_zone->end(); it_m_so_zone++)
				{
					long so_name = it_m_so_zone->first->data.node_so.so_name;
					long cb_to_so = cb_to_so;
					float zg_capacity = 0.0;
					id_to_pid(so_name);
					id_to_pid(cb_to_so);

					list<TreeNode<DataType>*> ::const_iterator it_m_so_zone_data;
					for(it_m_so_zone_data = it_m_so_zone->second.begin();it_m_so_zone_data != it_m_so_zone->second.end();it_m_so_zone_data++)
					{
						zg_capacity += (*it_m_so_zone_data)->data.node_zn.ldamount;

					}

					sprintf(sql, "Insert into FHZG_SO (ID, SO_ID, SO_DESCR, TYPE, FULL_CAPACITY, LEFT_CAPACITY, CB_ID,ZG_CAPACITY) Values (\'%s\', \'%ld\', \'%s\', %d, %f, %f, \'%ld\',%f)", fhzg_id, so_name, it_m_so_zone->first->data.node_so.so_descr, 2/*分摊转供*/,
							it_m_so_zone->first->data.node_so.full_capacity, it_m_so_zone->first->data.node_so.left_capacity, cb_to_so,zg_capacity);

					ExecSQL(sql);

					//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
				sprintf(sql, "Insert into FHZG_SO_ZONE (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE, CB_TO_SO) ");
				memset(sqltemp, 0, sizeof(sqltemp));
				for (list<int>::iterator it_lst_so_zones = it_m_so_zone->first->data.node_so.lst_zones.begin(); it_lst_so_zones != it_m_so_zone->first->data.node_so.lst_zones.end(); it_lst_so_zones++)
				{
					list<FAULT_ITEM> lst_dev;

					fhzg->get_zone_devs(*it_lst_so_zones, lst_dev);
					for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
					{
						strcat(sql, sqltemp);
						sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', \'%s\', \'%s\' from dual union all ", fhzg_id, *it_lst_so_zones, so_name, it_devs->id.c_str(), it_devs->table.c_str(), cb_to_so);
					}
				}
				strncat(sql, sqltemp, (strlen(sqltemp)-10));
				ExecSQL(sql);
#else
					for (list<long>::iterator it_lst_so_zones = it_m_so_zone->first->data.node_so.lst_zones.begin(); it_lst_so_zones != it_m_so_zone->first->data.node_so.lst_zones.end(); it_lst_so_zones++)
					{
						list<FAULT_ITEM> lst_dev;

						fhzg->get_zone_devs(*it_lst_so_zones, lst_dev);

						for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
						{
							sprintf(sql, "Insert into FHZG_SO_ZONE (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) Values (\'%s\', %ld, \'%ld\', \'%ld\', \'%s\')", fhzg_id, *it_lst_so_zones, so_name, it_devs->eq_id, it_devs->table.c_str());
							ExecSQL(sql);
						}
					}
#endif

					//设置边界开关
					sprintf(sql, "update fhzg_so_zone set edge = 1 where id = \'%s\' and so_id = \'%ld\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FHZG_SO_ZONE where id = \'%s\' and dev_table='CB' and so_id = \'%ld\' group by dev_id) where num =1)", fhzg_id,
							so_name, fhzg_id, so_name);
					ExecSQL(sql);

//				const LIST_TREENODE *lst_so_zone = fhzg->GetZoneBySO((*it)->data.node_so.dv_id);
//
//				if (lst_so_zone == NULL)
//					continue;

					LIST_TREENODE::const_iterator it_zone;

					for (it_zone = it_m_so_zone->second.begin(); it_zone != it_m_so_zone->second.end(); it_zone++)
					{
						list<FAULT_ITEM> lst_dev;

						for (it_map_zn_ld = (*it_zone)->data.node_zn.map_zn_ld.begin(); it_map_zn_ld != (*it_zone)->data.node_zn.map_zn_ld.end(); it_map_zn_ld++)
						{
							fhzg->get_zone_devs(it_map_zn_ld->first, lst_dev);

							//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
						sprintf(sql, "Insert into FHZG_ZONE (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) ");
						memset(sqltemp, 0, sizeof(sqltemp));
						for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
						{
							strcat(sql, sqltemp);
							sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', \'%s\' from dual union all ", fhzg_id, it_map_zn_ld->first, so_name, it_devs->id.c_str(), it_devs->table.c_str());
						}
						strncat(sql, sqltemp, (strlen(sqltemp)-10));
						ExecSQL(sql);

#else
							for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
							{
								sprintf(sql, "Insert into FHZG_ZONE (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) Values (\'%s\', %d, \'%ld\', \'%ld\', \'%s\')", fhzg_id, it_map_zn_ld->first, so_name, it_devs->eq_id, it_devs->table.c_str());
								ExecSQL(sql);
							}
#endif
							list<CLS_LD>::const_iterator it_ld;

							//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
						if(it_map_zn_ld->second.size() > 0)
						{
							sprintf(sql, "Insert into FHZG_LOAD (ID, LD_ID, LD_DESCR, LD_PRIOR, LD_AMOUNT, IZONE) ");
							memset(sqltemp, 0, sizeof(sqltemp));
							for (it_ld = it_map_zn_ld->second.begin(); it_ld != it_map_zn_ld->second.end(); it_ld++)
							{
								strcat(sql, sqltemp);
								sprintf(sqltemp, " select \'%s\', \'%s\', \'%s\', %d, %f, %d from dual union all ", fhzg_id, it_ld->id,
										it_ld->descr, (*it_zone)->data.node_zn.prio, it_ld->w, it_map_zn_ld->first);
							}
							strncat(sql, sqltemp, (strlen(sqltemp)-10));
							ExecSQL(sql);
						}
#else

							for (it_ld = it_map_zn_ld->second.begin(); it_ld != it_map_zn_ld->second.end(); it_ld++)
							{
								sprintf(sql, "Insert into FHZG_LOAD (ID, LD_ID, LD_DESCR, LD_PRIOR, LD_AMOUNT, IZONE) Values (\'%s\', \'%ld\', \'%s\', %d, %f, %d)", fhzg_id, it_ld->pid, it_ld->descr, (*it_zone)->data.node_zn.prio, it_ld->w, it_map_zn_ld->first);

								ExecSQL(sql);
							}
#endif
						}
					}

					sprintf(sql, "update fhzg_zone set edge = 1 where id = \'%s\' and so_id = \'%ld\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FHZG_ZONE where id = \'%s\' and dev_table='CB' and so_id = \'%ld\' group by dev_id) where num =1)", fhzg_id,
							so_name, fhzg_id, so_name);
					ExecSQL(sql);

				}
				break;
			}

		}
	}
}

int GetFHZGLeftStep(const char *FHZG_id, list<CB_CONTROL> &lstcb)
{
	assert(FHZG_id != NULL);

	char *buf1 = NULL, *buf2 = NULL, *buf3 = NULL;
	int rec_num1, rec_num2, rec_num3, attr_num;
	struct ORA_ATTR *attrs1 = NULL, *attrs2 = NULL, *attrs3 = NULL;
	CB_CONTROL control = { 0 };

	//获取消减负荷的操作步骤
	sprintf(sql, "select CB_ID from FHZG_STEP where id=\'%s\' and status=0 and so_id=\'none\'", FHZG_id);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf1, &rec_num1, &attr_num, &attrs1);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		for (int i = 0; i < rec_num1; i++)
		{
			char point_id[50];
			bzero(point_id, sizeof(point_id));

			memcpy(point_id, buf1 + (i * attrs1[0].col_width), attrs1[0].col_width);
			control.bOpen = true;
			control.point_id = *(long*)point_id;

			lstcb.push_back(control);
			cout << control.point_id << "分" << endl;
		}
		g_oci->Readdata_Free();
	}
	else
	{
		return -1;
	}

	//获取转供类型
	sprintf(sql, "select so_id, type from FHZG_SO where id = \'%s\' order by LEFT_CAPACITY DESC", FHZG_id);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf1, &rec_num1, &attr_num, &attrs1);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		if(rec_num1 <= 0) return 0;

		char type[25];

		bzero(type, sizeof(type));

		memcpy(type, buf1 + attrs1[0].col_width, attrs1[1].col_width);

		switch (*(int*)type)
		{
			case 1:	//独立转供
			{
				int cur_sel;
				char so_id[21] = {0};

				int i = 0;

				//判断是否有方案已经执行成功
				for (i = 0; i < rec_num1; i++)
				{
					bzero(so_id, sizeof(so_id));

					memcpy(so_id, buf1 + i * (attrs1[0].col_width + attrs1[1].col_width), attrs1[0].col_width);

					sprintf(sql, "select so_id from FHZG_STEP where ID=\'%s\' AND STATUS=3 AND SO_ID=\'%s\'", FHZG_id, so_id);
					cout << sql << endl;

					pthread_mutex_lock(&oci_mutex);
					ret = g_oci->ReadWithBind(sql, &buf2, &rec_num2, &attr_num, &attrs2);
					pthread_mutex_unlock(&oci_mutex);

					if (ret != OCI_ERROR)
					{
						g_oci->Readdata_Free();

						if (rec_num2 > 0)
						{
							return 0;
						}
					}
					else
					{
						g_oci->Readdata_Free();
						return -1;
					}
				}

				//找一个正在执行中的独立转供方案
				for(i = 0; i < rec_num1; i++)
				{
					bzero(so_id, sizeof(so_id));

					memcpy(so_id, buf1 + i * (attrs1[0].col_width + attrs1[1].col_width), attrs1[0].col_width);

					sprintf(sql, "select so_id from FHZG_STEP where ID=\'%s\' AND STATUS=1 AND SO_ID=\'%s\'", FHZG_id, so_id);
					cout << sql << endl;

					pthread_mutex_lock(&oci_mutex);
					ret = g_oci->ReadWithBind(sql, &buf2, &rec_num2, &attr_num, &attrs2);
					pthread_mutex_unlock(&oci_mutex);

					if(ret != OCI_ERROR)
					{
						g_oci->Readdata_Free();

						if(rec_num2 > 0)
						{
							break;
						}
					}
					else
					{
						g_oci->Readdata_Free();
						return -1;
					}
				}

				if(rec_num2 > 0)
				{
					cur_sel = i;
				}
					//没有执行中的方案，则找一个还没有开始执行的方案
				else
				{
					for (i = 0; i < rec_num1; i++)
					{
						bzero(so_id, sizeof(so_id));

						memcpy(so_id, buf1 + i * (attrs1[0].col_width + attrs1[1].col_width), attrs1[0].col_width);

						sprintf(sql, "select so_id from FHZG_STEP where ID=\'%s\' AND STATUS=0 AND SO_ID=\'%s\'", FHZG_id, so_id);
						cout << sql << endl;

						pthread_mutex_lock(&oci_mutex);
						ret = g_oci->ReadWithBind(sql, &buf2, &rec_num2, &attr_num, &attrs2);
						pthread_mutex_unlock(&oci_mutex);

						if (ret != OCI_ERROR)
						{
							g_oci->Readdata_Free();

							if (rec_num2 > 0)
							{
								break;
							}
						}
						else
						{
							g_oci->Readdata_Free();
							return -1;
						}
					}

					if(rec_num2 > 0)
					{
						cur_sel = i;
					}
						//已经没有方案可选了（可能已经全部执行完成或执行失败）
					else
						return 0;
				}

//				//如果有正在执行中的方案则选中执行中的方案，如果没有则选中第一套方案（目前是剩余容量最多的方案）
//				int cur_sel = (rec_num2 == 0) ? 0 : i;

				bzero(type, sizeof(type));

				memcpy(type, buf1 + cur_sel * (attrs1[0].col_width + attrs1[1].col_width), attrs1[0].col_width);

				printf("第%d套方案被选中，该方案对应的电源名为%s\r\n", cur_sel, type);

				sprintf(sql, "select CB_ID,OPERATION from FHZG_STEP where ID=\'%s\' and SO_ID=\'%s\' and status=0  order by step asc", FHZG_id, type);
				cout << sql << endl;

				pthread_mutex_lock(&oci_mutex);
				ret = g_oci->ReadWithBind(sql, &buf2, &rec_num2, &attr_num, &attrs2);
				pthread_mutex_unlock(&oci_mutex);

				if(ret != OCI_ERROR)
				{
					for(i = 0; i < rec_num2; i++)
					{
						bzero(&control , sizeof(control));
						bzero(type, sizeof(type));
						char point_id[50];

						bzero(point_id, sizeof(point_id));

						memcpy(point_id, buf2 + i * (attrs2[0].col_width + attrs2[1].col_width), attrs2[0].col_width);
						memcpy(type, buf2 + i * (attrs2[0].col_width + attrs2[1].col_width) + attrs2[0].col_width, attrs2[1].col_width);

						control.bOpen = *(int*)type == 1 ? true : false;
						control.point_id = *(long*)point_id;

						lstcb.push_back(control);
						cout << control.point_id << (control.bOpen ? "分" : "合") << endl;
					}

					g_oci->Readdata_Free();
				}
				else
				{
					g_oci->Readdata_Free();
					return -1;
				}

				break;
			}

			case 2:	//分摊转供
			{
				int i = 0;
				sprintf(sql, "select CB_ID,OPERATION from FHZG_STEP where FHZG_ID=\'%s\' and status=0  order by step asc", FHZG_id);
				cout << sql << endl;

				pthread_mutex_lock(&oci_mutex);
				ret = g_oci->ReadWithBind(sql, &buf2, &rec_num2, &attr_num, &attrs2);
				pthread_mutex_unlock(&oci_mutex);

				if (ret != OCI_ERROR)
				{
					for (i = 0; i < rec_num2; i++)
					{
						char point_id[50];

						bzero(point_id, sizeof(point_id));
						bzero(&control, sizeof(control));
						bzero(type, sizeof(type));

						memcpy(point_id, buf2 + i * (attrs2[0].col_width + attrs2[1].col_width), attrs2[0].col_width);
						memcpy(type, buf2 + i * (attrs2[0].col_width + attrs2[1].col_width) + attrs2[0].col_width, attrs2[1].col_width);

						control.bOpen = *(int*)type == 1 ? true : false;
						control.point_id = *(long*)point_id;

						lstcb.push_back(control);
						cout << control.point_id << (control.bOpen ? "分" : "合") << endl;
					}

					g_oci->Readdata_Free();
				}
				else
				{
					g_oci->Readdata_Free();
					return -1;
				}
				break;
			}

			default:
			{
				break;
			}
		}

		g_oci->Readdata_Free();
	}
	else
	{
		return -1;
	}

	return lstcb.size();
}

int GetJxcbDv(long cbid, PSBOB::DV &dv)
{
	int pnd = -1;
	long id = cbid;

	pid_to_id(id);

	CLS_CB *cb = g_bob->cb_tb->Find(CLS_CB(id));

	CLS_ND *pnd_nd = g_bob->nd_tb->GetDataByPos(cb->ind);
	CLS_ND *pnd_znd = g_bob->nd_tb->GetDataByPos(cb->iznd);
	list<CLS_ND *> lst_nd;
	lst_nd.push_back(pnd_nd);
	lst_nd.push_back(pnd_znd);

	for (list<CLS_ND *>::iterator it_lst = lst_nd.begin(); it_lst != lst_nd.end(); it_lst++)
	{
		//查找连接的负荷设备
		map<long, list<CLS_LD> >::iterator it = g_bob->m_map_nd_ld.find((*it_lst)->id);

		if (it != g_bob->m_map_nd_ld.end())
		{
			for (list<CLS_LD>::iterator it_ld = it->second.begin(); it_ld != it->second.end(); it_ld++)
			{
				if (GetDv(it_ld->id, "ld", dv) >= 0)
				{
					printf("结点：%ld连接的负荷是：%ld\r\n", (*it_lst)->id, it_ld->id);
					if (HasMask(g_db_psbob->GetTB("subcontrolarea"), "dtype", dv.dtype , MENU_DV_TYPE_FD))
					{
						return OO_SUCCEED;
					}
					else
					{
						bzero(&dv, sizeof(dv));
						break;
					}
				}
			}
		}

		//结点连接的线段
		if ((*it_lst)->r_sec != -1)
		{
			ChildPtr *sec_ptr = g_bob->r_nd_sec_tb->FindChildPtr((*it_lst)->id);
			ChildPtr *tmp_sec_ptr = sec_ptr;
			CLS_SEC* sec = NULL;
			while (tmp_sec_ptr != NULL)
			{
				sec = g_bob->sec_tb->GetDataByPos(tmp_sec_ptr->Ptr);

				printf("结点：%ld连接的线段是：%ld\r\n", (*it_lst)->id, sec->id);
				if (GetDv(sec->id, "sec", dv) >= 0)
				{
					if (HasMask(g_db_psbob->GetTB("subcontrolarea"), "dtype", dv.dtype, MENU_DV_TYPE_FD))
					{
						return OO_SUCCEED;
					}
					else
					{
						bzero(&dv, sizeof(dv));
						break;
					}
				}
				tmp_sec_ptr = tmp_sec_ptr->Next;
			}

			FreeChildPtr(sec_ptr);
		}

		//结点连接的开关查找dv
		if ((*it_lst)->r_cb != -1)
		{

			ChildPtr *cb_ptr = g_bob->r_nd_cb_tb->FindChildPtr((*it_lst)->id);
			ChildPtr *tmp_cb_ptr = cb_ptr;
			while (tmp_cb_ptr != NULL)
			{
				cb = g_bob->cb_tb->GetDataByPos(tmp_cb_ptr->Ptr);
				if (cb->pid == cbid)
				{
					if (!(cb->q & CB_FDHCB) && !(cb->q & CB_LWCB))
					{
						printf("结点：%ld连接的开关是：%ld\r\n", (*it_lst)->id, cb->id);
						if (GetDv(cb->id, "cb", dv) >= 0)
						{
							if (HasMask(g_db_psbob->GetTB("subcontrolarea"), "dtype", dv.dtype, MENU_DV_TYPE_FD))
							{
								return OO_SUCCEED;
							}
							else
							{
								bzero(&dv, sizeof(dv));
								break;
							}
						}
					}
				}

				tmp_cb_ptr = tmp_cb_ptr->Next;
			}

			FreeChildPtr(cb_ptr);
		}
	}

	return OO_FAIL;
}

int GetDv(long devid, const char *devtb, PSBOB::DV &dv)
{
	id_to_pid(devid);

	//D5000获取DV
	PSBOB::ST psbob_st = { 0 };

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
				printf("%s(%d):读取substation表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, psbob_ld.st_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):读取energyconsumer表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
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
				printf("%s(%d):读取substation表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, psbob_bus.st_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):读取busbarsection表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
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
				printf("%s(%d):读取substation表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, psbob_sec.ist_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):读取aclinesegment表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
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
				printf("%s(%d):读取substation表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, psbob_cb.st_id, _oodb_errno);
			}
		}
		else
		{
			printf("%s(%d):读取breaker表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
		}
	}
	else if(strcasecmp(devtb, "st") == 0)
	{
		if (oodbread_rk(&psbob_st, &devid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
		{
			return oodbread_rk(&dv, &psbob_st.subarea_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(dv));
		}
		else
		{
			printf("%s(%d):读取substation表关键字为%ld的记录失败，错误号:%d\r\n", __FUNCTION__, __LINE__, devid, _oodb_errno);
		}
	}

	return OO_FAIL;
}

int GetStbyDv(long dvid, PSBOB::ST &st)
{
	//D5000获取DV
	_oodb_errno = OO_SUCCEED;
	PSBOB::ST psbob_st[20] = { 0 };
	//读取st
	int re_size = oodbread_ra(psbob_st, "subarea_id", &dvid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st));
	if (re_size < 0)
	{
		TRACE("读取substation表subarea_id属性为:%ld的记录失败，errno:%d\r\n", dvid, _oodb_errno);
		return OO_FAIL;
	}
	else
	{
		int st_num = 0;
		st_num = re_size / sizeof(PSBOB::OO_R_DV_ST);
		for (int j = 0; j < st_num; j++)
		{

			if (HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st[j].st_type, MENU_FAC_TYPE_FEEDVIR)
				|| HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st[j].st_type, MENU_FAC_TYPE_SUBSTN))
			{
				memcpy(&st, &psbob_st[j], sizeof(PSBOB::ST));

				printf("the st id is %ld\n", st.id);
				return OO_SUCCEED;
			}
		}

		return OO_FAIL;
	}
}

//校验数据是否跟故障定位时一致，如果一致返回0，如果相关数据发生变化返回2，实时库版本号变化返回1，读库失败返回3
int data_verify(const char* groupid)
{
	assert(groupid != NULL);
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	bool dbnochange = true;
	int old_version = 0;

	//首先判断psbob实时库版本号是否一致，如果不一致返回false
	//D5000暂时不知道从哪获取实时库的版本号
	/*sprintf(sql, "select db_version from fdir_fdirect where id=\'%s\'", groupid);
	cout << sql << endl;

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		if (rec_num <= 0)
		{
			g_oci->Readdata_Free();
			return CHECK_DATA_READ_ERR;
		}
		else
		{
			old_version = *(int*)(buf);
		}
	}
	else
	{
		g_oci->Readdata_Free();
		return CHECK_DATA_READ_ERR;
	}
	g_oci->Readdata_Free();

	PSBOB::GLOBAL global = { 0 };
	if (oodbread_rp(&global, 0, const_cast<TB_DESCR*>(g_db_psbob->GetTB("secinfo")), 1, sizeof(global)) <= 0)
	{
		printf("读取global记录失败，errno:%d,line:%d\r\n", _oodb_errno, __LINE__);
		return CHECK_DATA_READ_ERR;
	}
	if (!(global.sec_no == old_version))
	{
		TRACE("实时库的版本号不一致，old是：%d,now是:%d\r\n", old_version, global.sec_no);
		return CHECK_DATA_PSBOB_ERR;
	}*/

	//判断开关状态是否改变，如果改变返回false
	sprintf(sql, "select pt_id,status,isjxcb,topflags from fdir_cb_status where id=\'%s\'", groupid);
	cout << sql << endl;

	long point_id;
	int status = CHECK_DATA_OK;
	int old_status = 0;
	int isjxcb = 0;
	int bus_status = 0;

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		for (int i = 0; i < rec_num; i++)
		{
			int len_row = attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width;

			point_id = *(long*) (buf + i * len_row);
			old_status = *(int*) (buf + i * len_row + attrs[0].col_width);
			isjxcb = *(int*) (buf + i * len_row + attrs[0].col_width + attrs[1].col_width);
			bus_status = *(int*) (buf + i * len_row + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width);

			TRACE("pt_id=%ld, status=%d, isjxcb=%d, topflags=%d\r\n", point_id, old_status, isjxcb, bus_status);

			PSBOB::CB point = { 0 };
			if (oodbread_rk(&point, &point_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(point)) == OO_FAIL)
			{
				TRACE("读取breaker表关键字为:%ld的记录失败\r\n", point_id);
				status = CHECK_DATA_READ_ERR;
			}
			else
			{
				if (point.point > 0)
				{
					status = CHECK_DATA_PSBOB_ERR;
				}
				else
				{
					status = CHECK_DATA_OK;
				}

				if (isjxcb == 1)
				{
					TRACE("进线开关point id：%ld , topflags is :%d\r\n", point.id, point.tpcolor);
					if ((point.tpcolor <= 0 && (bus_status == 0)) || (point.tpcolor > 0 && (bus_status == 1)))
					{
						sprintf(sql, "update fdir_cb_status set change=1 where id=\'%s\' and pt_id=\'%ld\'", groupid, point_id);
						ExecSQL(sql);
						dbnochange = false;
					}
				}
			}

			if (status != old_status)
			{
				sprintf(sql, "update fdir_cb_status set change=1 where id=\'%s\' and pt_id=\'%ld\'", groupid, point_id);
				ExecSQL(sql);
				dbnochange = false;
			}
		}
	}
	else
	{
		g_oci->Readdata_Free();
		return CHECK_DATA_READ_ERR;
	}
	g_oci->Readdata_Free();

	if (dbnochange == false)
	{
		return CHECK_DATA_CB_ERR;
	}

	return CHECK_DATA_OK;
}

//故障恢复方案
void write_fhzg_step(const char* groupid, const char* table_name, list<list<MAP_SO_NODE> > lst_step, FDIR_R2* fdir_r, list<FAULT_ITEM> &lst_ld, list<long> lstIsolate, bool bStation)
{
	int fhzg_index = 0;
	char fhzg_id[100] = { 0 };

	for (list<list<MAP_SO_NODE> >::iterator it_lst_steps = lst_step.begin(); it_lst_steps != lst_step.end(); it_lst_steps++)
	{
		sprintf(fhzg_id, "%s%c%d", groupid, bStation ? '_' : '#', fhzg_index);

		int istep = 0;
		map<long, _DATA_NODE_SO> map_so_data;
		map<long, long> map_so_zone;

		TRACE("list<MAP_SO_NODE>->size() = %d\r\n", it_lst_steps->size());

		for(list<long>::const_iterator it_Isolate = lstIsolate.begin(); it_Isolate != lstIsolate.end(); it_Isolate++)
		{
			CLS_CB *tmp_open_cb = g_bob->cb_tb->Find(CLS_CB(*it_Isolate));

			sprintf(sql,
					"Insert into PMS_PARAM.FHZG_STEP%s (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values ('%s', %ld, %d, %d, 0, %d, 1)",
					table_name, fhzg_id, tmp_open_cb->pid, 1, istep, 0);
			ExecSQL(sql);

			istep++;
		}

		for (list<MAP_SO_NODE>::iterator it_data = it_lst_steps->begin(); it_data != it_lst_steps->end(); it_data++)
		{
			map<long, _DATA_NODE_SO> m_tmp_close_steps;
			map<long, _DATA_NODE_SO>::iterator it_m_close;

			for(MAP_SO_NODE::iterator it_so_node = it_data->begin(); it_so_node != it_data->end(); it_so_node++)
			{
				//////////////////////////////////////
				//将后面步骤的电源联络开关替换成初次用该电源时的联络开关
				map<long, _DATA_NODE_SO>::iterator it_map_so_data = map_so_data.find(it_so_node->second.data_so.so_name);

				if(it_map_so_data != map_so_data.end())
				{
					it_so_node->second.data_so.cb_id = it_map_so_data->second.cb_id;
					it_so_node->second.data_so.lst_zones.splice(it_so_node->second.data_so.lst_zones.end(),it_map_so_data->second.lst_zones);
					it_so_node->second.data_so.lst_zones.sort();
					it_so_node->second.data_so.lst_zones.unique();
					//it_data->data_so.lst_zones = it_map_so_data->second.lst_zones;
				}
				//将后面步骤的电源联络开关替换成初次用该电源时的联络开关
				//////////////////////////////////////

				map_so_data[it_so_node->second.data_so.so_name] = it_so_node->second.data_so;
				for (list<FDIR_STEP>::iterator it_step = it_so_node->second.lst_steps.begin(); it_step != it_so_node->second.lst_steps.end(); it_step++)
				{
					if(it_step->bOpen)
					{
						long pid = it_step->cb;
						id_to_pid(pid);

						sprintf(sql, "Insert into PMS_PARAM.FHZG_STEP%s (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values ('%s', '%ld', %d, %d, 0, '%ld', 1)", table_name, fhzg_id, pid,
								it_step->bOpen ? 1 : 2, istep, it_step->stage == CUT_LD ? 0 : it_so_node->second.data_so.so_name);
						ExecSQL(sql);

						istep++;
					}
					else
					{
						m_tmp_close_steps[it_step->cb] = it_so_node->second.data_so;
					}

				}

				for (list<_DATA_NODE_ZN>::iterator it_zones = it_so_node->second.lst_zones.begin(); it_zones != it_so_node->second.lst_zones.end(); it_zones++)
				{
					list<FAULT_ITEM> lst_dev;

					for (map<int, list<CLS_LD> >::iterator it_map_zn_ld = it_zones->map_zn_ld.begin(); it_map_zn_ld != it_zones->map_zn_ld.end(); it_map_zn_ld++)
					{
						fdir_r->get_zone_devs(it_map_zn_ld->first, lst_dev);
						//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
						sprintf(sql, "Insert into FHZG_ZONE%s (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) ", table_name);
						memset(sqltemp, 0, sizeof(sqltemp));
						for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
						{
							strcat(sql, sqltemp);
							sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', \'%s\' from dual union all ", fhzg_id, it_map_zn_ld->first, it_so_node->second.data_so.so_name, it_devs->eq_id.c_str(), it_devs->table.c_str());
						}
						strncat(sql, sqltemp, (strlen(sqltemp)-10));
						strcat(sql,"\0");
						ExecSQL(sql);

#else
						for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
						{
							sprintf(sql, "Insert into PMS_PARAM.FHZG_ZONE%s (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) Values (\'%s\', %d, \'%ld\', \'%ld\', \'%s\')", table_name, fhzg_id, it_map_zn_ld->first, it_so_node->second.data_so.so_name, it_devs->eq_id, it_devs->table.c_str());
							strcat(sql,"\0");
							ExecSQL(sql);
						}
#endif
						list<CLS_LD>::const_iterator it_ld;
						//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
						if(it_map_zn_ld->second.size() > 0)
						{
							sprintf(sql, "Insert into FHZG_LOAD%s (ID, LD_ID, LD_DESCR, LD_PRIOR, LD_AMOUNT, IZONE) ", table_name);
							memset(sqltemp, 0, sizeof(sqltemp));
							for (it_ld = it_map_zn_ld->second.begin(); it_ld != it_map_zn_ld->second.end(); it_ld++)
							{
								strcat(sql, sqltemp);
								sprintf(sqltemp, " select \'%s\', \'%s\', \'%s\', %d, %f, %d from dual union all ", fhzg_id, it_ld->id,
										it_ld->descr, it_zones->prio, it_ld->w, it_map_zn_ld->first);
								if (fhzg_index == 0)
								lst_ld.push_back(FAULT_ITEM(NULL, it_ld->id, "ld", it_ld->descr));
							}
							strncat(sql, sqltemp, (strlen(sqltemp)-10));
							strcat(sql,"\0");
							ExecSQL(sql);
						}
#else
						for (it_ld = it_map_zn_ld->second.begin(); it_ld != it_map_zn_ld->second.end(); it_ld++)
						{
							sprintf(sql, "Insert into PMS_PARAM.FHZG_LOAD%s (ID, LD_ID, LD_DESCR, LD_PRIOR, LD_AMOUNT, IZONE) Values (\'%s\', \'%ld\', \'%s\', %d, %f, %d)", table_name, fhzg_id, it_ld->pid, it_ld->descr, it_zones->prio, it_ld->w, it_map_zn_ld->first);
							strcat(sql,"\0");
							ExecSQL(sql);
							if (fhzg_index == 0)
								lst_ld.push_back(FAULT_ITEM(0, it_ld->pid, "ld", it_ld->descr));
						}
#endif
					}

				}

				sprintf(sql, "update PMS_PARAM.fhzg_zone%s set edge = 1 where id = \'%s\' and so_id = \'%ld\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FHZG_ZONE where id = \'%s\' and so_id = \'%ld\' and dev_table='CB' and so_id = \'%ld\' group by dev_id) where num =1)", table_name, fhzg_id, it_so_node->second.data_so.so_name, fhzg_id, it_so_node->second.data_so.so_name, it_so_node->second.data_so.so_name);
				ExecSQL(sql);

			}


			for (list<_DATA_NODE_ZN>::iterator it_zones = it_data->begin()->second.lst_cut_zones.begin(); it_zones != it_data->begin()->second.lst_cut_zones.end(); it_zones++)
			{
				list<FAULT_ITEM> lst_dev;

				for (map<int, list<CLS_LD> >::iterator it_map_zn_ld = it_zones->map_zn_ld.begin(); it_map_zn_ld != it_zones->map_zn_ld.end(); it_map_zn_ld++)
				{
					fdir_r->get_zone_devs(it_map_zn_ld->first, lst_dev);
					//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
					sprintf(sql, "Insert into FHZG_CUT_ZONE%s (ID, IZONE, DEV_ID, DEV_TABLE) ", table_name);
					memset(sqltemp, 0, sizeof(sqltemp));
					for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
					{
						strcat(sql, sqltemp);
						sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\' from dual union all ", fhzg_id, it_map_zn_ld->first, it_devs->id.c_str(), it_devs->table.c_str());
					}
					strncat(sql, sqltemp, (strlen(sqltemp)-10));
					ExecSQL(sql);

#else
					for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
					{
						sprintf(sql, "Insert into PMS_PARAM.FHZG_CUT_ZONE%s (ID, IZONE, DEV_ID, DEV_TABLE) Values (\'%s\', %d, \'%ld\', \'%s\')", table_name, fhzg_id, it_map_zn_ld->first, it_devs->eq_id, it_devs->table.c_str());
						ExecSQL(sql);
					}
#endif

				}

			}

			//分摊转供时会有非隔离开关的断开，断开步骤先执行，闭合步骤最后写入库
			for(it_m_close = m_tmp_close_steps.begin(); it_m_close != m_tmp_close_steps.end(); it_m_close++)
			{
				long pid = it_m_close->first;

				id_to_pid(pid);

				sprintf(sql, "Insert into PMS_PARAM.FHZG_STEP%s (ID, CB_ID, OPERATION, STEP, STATUS, SO_ID, CONTROL) Values ('%s', '%ld', %d, %d, 0, '%ld', 1)", table_name, fhzg_id, pid, 2, istep, it_m_close->second.so_name);
				ExecSQL(sql);

				istep++;
			}

		}

		TRACE("map_so_data.size()=%d\r\n", map_so_data.size());

		for (map<long, _DATA_NODE_SO>::iterator it_map_so_data = map_so_data.begin(); it_map_so_data != map_so_data.end(); it_map_so_data++)
		{
			long id = it_map_so_data->second.cb_id;

			id_to_pid(id);

			sprintf(sql, "Insert into PMS_PARAM.FHZG_SO%s (ID, SO_ID, SO_DESCR, TYPE, FULL_CAPACITY, LEFT_CAPACITY, CB_ID, ORG_WLOSS, NEW_WLOSS) Values (\'%s\', \'%ld\', \'%s\', %d, %f, %f, \'%ld\', %f, %f)",
					table_name, fhzg_id, it_map_so_data->second.so_name, it_map_so_data->second.so_descr, map_so_data.size() > 1 ? 2 : 1/*有两个以上电源时是分摊转供，少于两个电源是独立转供*/,
					it_map_so_data->second.full_capacity, it_map_so_data->second.left_capacity, id, it_map_so_data->second.org_wloss, it_map_so_data->second.new_wloss);
			strcat(sql,"\0");
			ExecSQL(sql);

			//分摊转供时方案中电源树可能会重复，下面通过map_so_zone避免重复写库
			map<long, long>::iterator it_so_zone;
			it_so_zone = map_so_zone.find(it_map_so_data->second.so_name);
			if (it_so_zone == map_so_zone.end())
			{
				//针对每个电源生成电源树，把电源树上的开关信息写入表中，为数据校验用
				list<long>::iterator it_so = it_map_so_data->second.lst_zones.begin();
				if (it_so != it_map_so_data->second.lst_zones.end())
				{
					list<CLS_CB> lst_info_cbs;
					fdir_r->getfhzglstcb(*it_so, lst_info_cbs);
					for (list<CLS_CB>::iterator it_lst_info_cbs = lst_info_cbs.begin(); it_lst_info_cbs != lst_info_cbs.end(); it_lst_info_cbs++)
					{
						//开关的状态，1表示闭合，0表示断开，3表示状态未知
						int cb_status = 1;
						int isjxcb = 0;
						int bus_status = 0;

						sprintf(sql, "select count(pt_id) from fdir_cb_status where id=\'%s\' and pt_id=\'%ld\'", groupid, it_lst_info_cbs->pid);
						cout<<sql<<endl;
						char *buf = NULL;
						int rec_num, attr_num;
						struct ORA_ATTR *attrs = NULL;
						int num = 0;

						pthread_mutex_lock(&oci_mutex);
						int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
						pthread_mutex_unlock(&oci_mutex);

						if (ret != OCI_ERROR)
						{
							num = *(int*)(buf);
						}
						g_oci->Readdata_Free();
						if (num > 0)
						{
							if (it_lst_info_cbs->q & CB_LWCB)
							{
								continue;
							}
							else
							{
								break;
							}
						}

						if (it_lst_info_cbs->q & CB_OPEN)
						{
							cb_status = 0;
						}
						if (it_lst_info_cbs->q & CB_UNKNOWN)
						{
							cb_status = 3;
						}
						if (it_lst_info_cbs->q & CB_FDHCB)
						{
							isjxcb = 1;
							if (it_lst_info_cbs->topflags & CB_DEAD)
							{
								bus_status = 1;
							}
						}
						sprintf(sql, "INSERT into PMS_PARAM.FDIR_CB_STATUS (ID,PT_ID,CB_DESCR,STATUS,ISJXCB,TOPFLAGS) VALUES (\'%s\',\'%ld\',\'%s\',%d,%d,%d)",
								groupid, it_lst_info_cbs->pid, it_lst_info_cbs->descr, cb_status, isjxcb, bus_status);
						strcat(sql,"\0");
						ExecSQL(sql);
					}
				}

				map_so_zone[it_map_so_data->second.so_name] = it_map_so_data->second.so_name;

			}


			//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
			sprintf(sql, "Insert into FHZG_SO_ZONE%s (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) ", table_name);
			memset(sqltemp, 0, sizeof(sqltemp));
			for (list<int>::iterator it_lst_so_zones = it_map_so_data->second.lst_zones.begin(); it_lst_so_zones != it_map_so_data->second.lst_zones.end(); it_lst_so_zones++)
			{
				list<FAULT_ITEM> lst_dev;

				map_fdir_task.begin()->first->get_zone_devs(*it_lst_so_zones, lst_dev);
				for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
				{
					strcat(sql, sqltemp);
					sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', \'%s\' from dual union all ", fhzg_id, *it_lst_so_zones, it_map_so_data->second.so_name, it_devs->eq_id.c_str(), it_devs->table.c_str());
				}
			}
			strncat(sql, sqltemp, (strlen(sqltemp)-10));
			strcat(sql,"\0");
			ExecSQL(sql);
#else
			for (list<long>::iterator it_lst_so_zones = it_map_so_data->second.lst_zones.begin(); it_lst_so_zones != it_map_so_data->second.lst_zones.end(); it_lst_so_zones++)
			{
				list<FAULT_ITEM> lst_dev;
				fdir_r->get_zone_devs(*it_lst_so_zones, lst_dev);

				for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
				{
					sprintf(sql, "Insert into PMS_PARAM.FHZG_SO_ZONE%s (ID, IZONE, SO_ID, DEV_ID, DEV_TABLE) Values (\'%s\', %ld, \'%ld\', \'%ld\', \'%s\')", table_name, fhzg_id, *it_lst_so_zones, it_map_so_data->second.so_name, it_devs->eq_id, it_devs->table.c_str());
					strcat(sql,"\0");
					ExecSQL(sql);
				}
			}
#endif
			//设置边界开关
			sprintf(sql, "update PMS_PARAM.fhzg_so_zone%s set edge = 1 where id = \'%s\' and so_id = \'%ld\' and dev_id in (select dev_id from (select dev_id,count(dev_id) as num from  FHZG_SO_ZONE where id = \'%s\' and dev_table='CB' and so_id = \'%ld\' group by dev_id) where num =1)", table_name, fhzg_id, it_map_so_data->second.so_name, fhzg_id, it_map_so_data->second.so_name);
			strcat(sql,"\0");
			ExecSQL(sql);
		}

		fhzg_index++;

	}
}

void write_fault_dev(const char *fault_id, char *table_name, FDIR_R2* fdir_r)
{
	std::map<long, int> map_st_type;	//故障区域开关的厂站与其厂站类型的映射
	list<FAULT_ITEM> lst_fault_devs, lst_up_devs, lst_down_devs;
	fdir_r->get_fault_devs(lst_up_devs, lst_fault_devs, lst_down_devs);

	//故障上游设备
	for (list<FAULT_ITEM>::iterator it_devs = lst_up_devs.begin(); it_devs != lst_up_devs.end(); it_devs++)
	{
		if (strcmp(it_devs->table.c_str(), "CB") == 0)
		{
			sprintf(sql, "INSERT INTO FDIR_FAULT_DEV%s (ID, DEV_ID, DEV_TABLE, EDGE, WHOLE,DEV_DESCR, REL_DEV_ID, FLAG, AREA_TYPE) VALUES (\'%s\', \'%ld\', \'%s\', %d, %d, \'%s\', \'%ld\', %d, 0)",
					table_name, fault_id, it_devs->eq_id, it_devs->table.c_str(), it_devs->border, it_devs->whole, it_devs->eq_descr.c_str(), it_devs->eq_id, it_devs->flag);
			ExecSQL(sql);
		}
		else
		{
			sprintf(sql, "INSERT INTO FDIR_FAULT_DEV%s (ID, DEV_ID, DEV_TABLE, EDGE, WHOLE,DEV_DESCR, REL_DEV_ID, FLAG, AREA_TYPE) VALUES (\'%s\', \'%ld\', \'%s\', %d, %d, \'%s\', \'%ld\',%d, 0)",
					table_name, fault_id, it_devs->id > 0 ? it_devs->id : it_devs->eq_id, it_devs->table.c_str(), it_devs->border, it_devs->whole, it_devs->eq_descr.c_str(), it_devs->eq_id, it_devs->flag);
			ExecSQL(sql);
		}
	}

	//故障设备
	for (list<FAULT_ITEM>::iterator it_devs = lst_fault_devs.begin(); it_devs != lst_fault_devs.end(); it_devs++)
	{
		if (strcmp(it_devs->table.c_str(), "CB") == 0)
		{
			sprintf(sql, "INSERT INTO FDIR_FAULT_DEV%s (ID, DEV_ID, DEV_TABLE, EDGE, WHOLE,DEV_DESCR, REL_DEV_ID, FLAG, AREA_TYPE) VALUES (\'%s\', \'%ld\', \'%s\', %d, %d, \'%s\', \'%ld\', %d, 1)",
					table_name, fault_id, it_devs->eq_id, it_devs->table.c_str(), it_devs->border, it_devs->whole, it_devs->eq_descr.c_str(), it_devs->eq_id, it_devs->flag);
			ExecSQL(sql);
		}
		else
		{
			sprintf(sql, "INSERT INTO FDIR_FAULT_DEV%s (ID, DEV_ID, DEV_TABLE, EDGE, WHOLE,DEV_DESCR, REL_DEV_ID, FLAG, AREA_TYPE) VALUES (\'%s\', \'%ld\', \'%s\', %d, %d, \'%s\', \'%ld\',%d, 1)",
					table_name, fault_id, it_devs->id > 0 ? it_devs->id : it_devs->eq_id, it_devs->table.c_str(), it_devs->border, it_devs->whole, it_devs->eq_descr.c_str(), it_devs->eq_id, it_devs->flag);
			ExecSQL(sql);
		}
	}

	// 添加故障区域所属厂站ID，用于解决进线开关故障不能推到馈线图和无法推环网图的问题
	for (list<FAULT_ITEM>::iterator itFaultDevs = lst_fault_devs.begin(); itFaultDevs != lst_fault_devs.end(); itFaultDevs++)
	{
		if (strcmp(itFaultDevs->table.c_str(), "CB") == 0)
		{
			PSBOB::CB faultCB = {0};
			int iRet = oodbread_rk(&faultCB, &(itFaultDevs->eq_id), const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB));
			if (iRet <= 0)
			{
				TRACE("在断路器表中查找关键字为%ld的记录失败啦！\r\n", itFaultDevs->eq_id);
				continue;
			}
			else
			{
				PSBOB::ST faultST = {0};
				iRet = oodbread_rk(&faultST, &(faultCB.st_id), const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(PSBOB::ST));
				if (iRet <= 0)
				{
					TRACE("在厂站表中查找关键字为%ld的记录失败啦！\r\n", faultCB.st_id);
					continue;
				}
				else
				{
					map_st_type.insert(std::make_pair(faultST.id, faultST.st_type));
				}
			}
		}
	}
	//从map中取出厂站ID，如果有类型为馈线类型为虚拟馈线厂站的ID，将其赋到lFaultStId中，否则如果有类型为环网柜的ID，赋到lFaultStId中，否则填0
	std::map<long, int>::iterator it_st = map_st_type.begin();
	for (; it_st != map_st_type.end(); it_st++)
	{
		if (it_st->second == MENU_FAC_TYPE_FEEDVIR)
		{
			fdir_r->lFaultStId = it_st->first;
			break;
		}
	}
	if (it_st == map_st_type.end())
	{
		for (it_st = map_st_type.begin(); it_st != map_st_type.end(); it_st++)
		{
			if (it_st->second == MENU_FAC_TYPE_RINGMAINUNIT)
			{
				fdir_r->lFaultStId = it_st->first;
				break;
			}
		}
	}

	if (it_st == map_st_type.end())
	{
		fdir_r->lFaultStId = 0;
	}

	//故障下游设备
	for (list<FAULT_ITEM>::iterator it_devs = lst_down_devs.begin(); it_devs != lst_down_devs.end(); it_devs++)
	{
		if (strcmp(it_devs->table.c_str(), "CB") == 0)
		{
			sprintf(sql, "INSERT INTO FDIR_FAULT_DEV%s (ID, DEV_ID, DEV_TABLE, EDGE, WHOLE,DEV_DESCR, REL_DEV_ID, FLAG, AREA_TYPE) VALUES (\'%s\', \'%ld\', \'%s\', %d, %d, \'%s\', \'%ld\', %d, 2)",
					table_name, fault_id, it_devs->eq_id, it_devs->table.c_str(), it_devs->border, it_devs->whole, it_devs->eq_descr.c_str(), it_devs->eq_id, it_devs->flag);
			ExecSQL(sql);
		}
		else
		{
			sprintf(sql, "INSERT INTO FDIR_FAULT_DEV%s (ID, DEV_ID, DEV_TABLE, EDGE, WHOLE,DEV_DESCR, REL_DEV_ID, FLAG, AREA_TYPE) VALUES (\'%s\', \'%ld\', \'%s\', %d, %d, \'%s\', \'%ld\',%d, 2)",
					table_name, fault_id, it_devs->id > 0 ? it_devs->id : it_devs->eq_id, it_devs->table.c_str(), it_devs->border, it_devs->whole, it_devs->eq_descr.c_str(), it_devs->eq_id, it_devs->flag);
			ExecSQL(sql);
		}
	}

	fdir_r->set_faultcb_edge(fault_id);
}

void write_fault_tree(const char* groupid, FDIR_R2* fdir_r)
{
	list<CLS_CB> fault_cbs;
	fdir_r->Get_Fdir_Cbs(fault_cbs);
	for (list<CLS_CB>::iterator it_fault_cb = fault_cbs.begin(); it_fault_cb != fault_cbs.end(); it_fault_cb++)
	{
		//开关的状态，1表示闭合，0表示断开，3表示状态未知
		int cb_status = 1;
		if (it_fault_cb->q & CB_OPEN)
		{
			cb_status = 0;
		}
		if (it_fault_cb->q & CB_UNKNOWN)
		{
			cb_status = 3;
		}
		int up_pos = 0;
		if (it_fault_cb->q & CB_FAULT)
		{
			up_pos = 1;
		}
		sprintf(sql, "INSERT into PMS_PARAM.FDIR_CB_STATUS (ID, PT_ID,CB_DESCR,STATUS,ISFAULTUP,ISFAULTDV) VALUES (\'%s\',\'%ld\',\'%s\',%d,%d,1)",
				groupid, it_fault_cb->pid, it_fault_cb->descr, cb_status, up_pos);
		ExecSQL(sql);
	}
}

void write_fault_up_dev(const char* fault_id, char *table_name, FDIR_R2* fdir_r, bool upiso_fail, list<FAULT_ITEM> &lst_ld)
{
	map<long, string> faultzone_cb;
	list<FAULT_ITEM> lst_dev;
	int isozone;
	char uptempsql[1024] = { 0 };
	char upsql[1024] = { 0 };

	//写上游恢复区段表
	//获取上游要恢复的区段号

	isozone = fdir_r->getisozone();


	int upzone = fdir_r->getfaultautoopenzone();

	printf("上游分闸的区段是 %d, the 隔离区段是 %d\n", upzone, isozone);
	//如果上游要恢复的区段和故障区段一样说明没有要恢复的上游区段
	if(upzone != isozone && isozone != -1 && upzone != -1)
	{
		list<int> uplstzone;
		fdir_r->getlstupzone(upzone, isozone, uplstzone);

		faultzone_cb.clear();
//只有oracle数据库支持这种插入库的sql语句，其他数据库还是一条一条的记录插入tianyq
#ifdef _ORACLE_
		sprintf(sql, "INSERT INTO FDIR_UP_ZONE%s (ID, IZONE, DEV_ID, DEV_TABLE, EDGE) ", table_name);
		memset(sqltemp, 0, sizeof(sqltemp));
		memset(uptempsql, 0, sizeof(uptempsql));
		int upcount = 0;
		sprintf(upsql, "update FDIR_UP_ZONE%s set edge = 0 where ", table_name);
		for(list<int>::iterator it_up = uplstzone.begin(); it_up != uplstzone.end(); it_up++)
		{
			lst_dev.clear();
			fdir_r->get_zone_devs(*it_up, lst_dev);

			for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
			{
				if(strcmp(it_devs->table.c_str(), "CB") == 0)
				{
					if(faultzone_cb.find(it_devs->eq_id) != faultzone_cb.end())
					{
						//设置边界开关
						upcount++;
						strcat(upsql, uptempsql);
						sprintf(uptempsql, "(id = \'%s\' and dev_id = \'%s\') or ", fault_id, it_devs->eq_id.c_str());
					}
					else
					{
						faultzone_cb[it_devs->eq_id] = "CB";
						strcat(sql, sqltemp);
						sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', %d from dual union all ", fault_id, *it_up, it_devs->eq_id.c_str(), it_devs->table.c_str(), 1);
					}
				}
				else
				{
					strcat(sql, sqltemp);
					sprintf(sqltemp, " select \'%s\', %d, \'%s\', \'%s\', %d from dual union all ", fault_id, *it_up, it_devs->eq_id.c_str(), it_devs->table.c_str(), 0);
					if (strcasecmp(it_devs->table.c_str(), "ld") == 0)
					{
						lst_ld.push_back(*it_devs);
					}
				}
			}
		}
		strncat(sql, sqltemp, (strlen(sqltemp)-10));
		ExecSQL(sql);
		if(upcount > 0)
		{
			strncat(upsql, uptempsql, (strlen(uptempsql)-3));
			ExecSQL(upsql);
		}
#else
		for(list<int>::iterator it_up = uplstzone.begin(); it_up != uplstzone.end(); it_up++)
		{
			lst_dev.clear();
			fdir_r->get_zone_devs(*it_up, lst_dev);

			for (list<FAULT_ITEM>::iterator it_devs = lst_dev.begin(); it_devs != lst_dev.end(); it_devs++)
			{
				if(strcmp(it_devs->table.c_str(), "CB") == 0)
				{
					if(faultzone_cb.find(it_devs->eq_id) != faultzone_cb.end())
					{
						//设置边界开关
						sprintf(sql, "update FDIR_UP_ZONE%s set edge = 0 where id = \'%s\' and dev_id = \'%ld\'",
								table_name, fault_id, it_devs->eq_id);
						ExecSQL(sql);
					}
					else
					{
						faultzone_cb[it_devs->eq_id] = "CB";
						sprintf(sql, "INSERT INTO FDIR_UP_ZONE%s (ID, IZONE, DEV_ID, DEV_TABLE, EDGE) VALUES (\'%s\', %d, \'%ld\', \'%s\', %d)", table_name, fault_id, *it_up, it_devs->eq_id, it_devs->table.c_str(), 1);
						ExecSQL(sql);
					}
				}
				else
				{
					sprintf(sql, "INSERT INTO FDIR_UP_ZONE%s (ID, IZONE, DEV_ID, DEV_TABLE, EDGE) VALUES (\'%s\', %d, \'%ld\', \'%s\', %d)", table_name, fault_id, *it_up, it_devs->eq_id, it_devs->table.c_str(), 0);
					ExecSQL(sql);
					if (strcasecmp(it_devs->table.c_str(), "ld") == 0)
					{
						lst_ld.push_back(*it_devs);
					}
				}
			}
		}
#endif
	}

}

bool invalid_recalc(const char *group_id)
{
	cout << __FUNCTION__ << endl;
	assert(group_id != NULL);

	int i;

	//查询出该组故障的故障跳闸开关，获取开关的最新状态，并将跳闸开关设置为合闸
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	list<long> lst_point; //该组故障中跳闸的开关
	list<long> lst_up_cb; //故障上游的主干开关
	list<FAULT_ITEM> lst_cb_item;

	sprintf(sql, "SELECT RECV_POINT FROM FDIR_FDIRECT WHERE GROUPID=\'%s\'", group_id);

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	cout << sql << endl;

	if (ret != OCI_ERROR)
	{
		if (rec_num > 0)
		{
			for (i = 0; i < rec_num; i++)
			{
				char recv_point[1000];

				bzero(recv_point, sizeof(recv_point));
				memcpy(recv_point, buf + i * attrs[0].col_width, attrs[0].col_width);

				cout << recv_point << endl;

				char *pbuf1, *pbuf2; // = recv_point;

				pbuf1 = pbuf2 = recv_point;

				while ((pbuf2 = strchr(pbuf1, ';')) != NULL)
				{
					char cb[MAX_CB_ID_LEN + 5] = { 0 };

					bzero(cb, sizeof(cb));

					memcpy(cb, pbuf1, pbuf2 - pbuf1);

					char *pTemp = strstr(cb, "[CB]");

					if (pTemp != NULL)
					{
						*pTemp = 0;

						lst_point.push_back(*(long*)cb);

						cout << "读取到跳闸开关：" << cb << endl;
					}

					pbuf1 = pbuf2 + 1;
				}
			}
		}
		g_oci->Readdata_Free();
	}
	else
	{
		return false;
	}

	//如果故障所在馈线的上游主干开关变化，合主干变化的开关
	sprintf(sql, "SELECT PT_ID FROM FDIR_CB_STATUS WHERE ID=\'%s\' AND ISFAULTUP=1 AND ((STATUS=1 AND CHANGE=1)OR(STATUS=0 AND CHANGE=0))", group_id);

	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	cout << sql << endl;

	if (ret != OCI_ERROR)
	{
		if (rec_num > 0)
		{
			for (i = 0; i < rec_num; i++)
			{
				char cb[MAX_CB_ID_LEN + 5] = { 0 };

				bzero(cb, sizeof(cb));
				memcpy(cb, buf + i * attrs[0].col_width, attrs[0].col_width);
				cout << cb << endl;
				lst_up_cb.push_back(*(long*)cb);
			}
		}
		g_oci->Readdata_Free();
	}
	else
	{
		return false;
	}

	int lv_cb = g_bob->cb_tb->GetCount();
	CLS_CB *cb = g_bob->cb_tb->GetTable();

	for (i = 0; i < lv_cb; i++)
	{
		// add by spf 20140507
		if (cb->q & CB_VIRTUAL)
		{
			if (get_virtual_path(cb->nd).size() > 0)
			{
				cb->q &= ~CB_OPEN;
			}
			else
			{
				cb->q |= CB_OPEN;
			}
		}
		else
		{
			PSBOB::CB psbob_cb = { 0 };
			long pid = (cb + i)->id;

			id_to_pid(pid);

			//读取开关对应的遥信信息  oodbread_ap为逻辑读属性 idigital为开关测点号
			if (oodbread_rk(&psbob_cb, &pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
			{
				TRACE("读取%ld的开关记录失败！\r\n", pid);

				return false;
			}

			(cb + i)->q &= ~CB_OPEN;

			if (psbob_cb.point <= 0)
				(cb + i)->q |= CB_OPEN;
		}

		//g_bob->digital_tb->Data[(cb + i)->idigital].value = value; //更新DIGITAL表信息

		for (list<long>::iterator it = lst_point.begin(); it != lst_point.end(); it++)
		{
			if ((cb + i)->pid == *it)
			{
				//断开的开关才需要合闸，如果是闭合的开关不需要加入lst_point中
				if ((cb + i)->q & CB_OPEN)
				{
					TRACE("合跳闸开关，cb:%ld, pointid:%s, point_name:%ld\r\n", (cb + i)->id, (cb + i)->descr, *it);
					(cb + i)->q &= ~CB_OPEN;
					lst_cb_item.push_back(FAULT_ITEM(0, (cb + i)->pid, "CB", g_bob->cb_tb->Data[i].descr));
					break;
				}
			}
		}

		for (list<long>::iterator it_cb = lst_up_cb.begin(); it_cb != lst_up_cb.end(); it_cb++)
		{
			if ((cb + i)->pid == *it_cb)
			{
				TRACE("合故障上游的主干开关，cb:%ld, pointid:%s, point_name:%ld\r\n", (cb + i)->id, (cb + i)->descr, *it_cb);
				(cb + i)->q &= ~CB_OPEN;
				break;
			}
		}
	}

	sprintf(sql, "UPDATE FDIR_CB_STATUS SET STATUS=0,CHANGE=0 WHERE ID=\'%s\' AND CHANGE=1 AND STATUS=1", group_id);
	ExecSQL(sql);

	sprintf(sql, "UPDATE FDIR_CB_STATUS SET STATUS=1,CHANGE=0 WHERE ID=\'%s\' AND CHANGE=1 AND STATUS=0", group_id);
	ExecSQL(sql);


	//把计算失败的故障树重新建立
	RecoTaskItem(group_id);

	//设置方案失效的故障为待重新计算方案状态
	for (FDIR_TASK_LIST::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
	{
		if (it->rq_type == TASK_TYPE_FAULT)
		{
			if (strcmp(it->task_data.fault.head.group_id, group_id) == 0)
			{

				it->task_data.fault.fdir_r->lst_point = lst_cb_item;

				it->task_data.fault.next_step = TASK_FAULT_STATUS_INVALID_RECALC;

				//更新FDIR_R对象fdir_cb里开关的状态
				CLS_FDIR_CB *fdir_cb = it->task_data.fault.fdir_r->fdir_cb_tb->GetTable();

				for (i = 0; i < lv_cb; i++)
				{
					if ((cb + i)->q & CB_OPEN)
					{
						(fdir_cb + i)->topflags |= FDIR_CB_OPEN0;
						(fdir_cb + i)->topflags |= FDIR_CB_OPEN;
					}
					else
					{
						(fdir_cb + i)->topflags &= ~FDIR_CB_OPEN0;
						(fdir_cb + i)->topflags &= ~FDIR_CB_OPEN;
					}
				}
			}
		}
	}

	//写状态到库
	sprintf(sql, "update FDIR_FDIRECT set status=%d,status_isolate=%d,status_fhzg=%d where groupid=\'%s\'", STATUS_FDIRECT_RE_CALC, EXT_STATUS_FDIRECT_WAIT,EXT_STATUS_FDIRECT_WAIT, group_id);
	ExecSQL(sql);

	return true;
}

void EraseSameInfo(list<FAULT_ITEM> &lst_cont, list<FAULT_ITEM> lst_remove)
{
	if (lst_cont.size() <= 0 || lst_remove.size() <= 0)
	{
		return;
	}
	map<long, FAULT_ITEM> m_cont;
	map<long, FAULT_ITEM>::iterator it_mc;
	list<FAULT_ITEM>::iterator it_lst;
	for (it_lst = lst_cont.begin(); it_lst != lst_cont.end(); it_lst++)
	{
		m_cont[it_lst->eq_id] = *it_lst;
	}
	for (it_lst = lst_remove.begin(); it_lst != lst_remove.end(); it_lst++)
	{
		it_mc = m_cont.find(it_lst->eq_id);
		if (it_mc != m_cont.end())
		{
			m_cont.erase(it_mc);
		}
	}
	lst_cont.clear();
	for (it_mc = m_cont.begin(); it_mc != m_cont.end(); it_mc++)
	{
		lst_cont.push_back(it_mc->second);
	}
}

void write_res_and_disableres_ld(const char *groupid, list<FAULT_ITEM> lst_lose, list<FAULT_ITEM> lst_res)
{
	if (groupid == NULL)
	{
		TRACE("传入的组号为空\r\n");
		return;
	}

	list<FAULT_ITEM>::iterator it_lst;

	for (it_lst = lst_lose.begin(); it_lst != lst_lose.end(); it_lst++)
	{
		sprintf(sql, "INSERT into PMS_PARAM.FDIR_DISABLE_RES (ID,DEV_ID,DEV_TYPE,DEV_DESCR) VALUES (\'%s\',\'%ld\',\'%s\',\'%s\')",groupid, it_lst->eq_id, it_lst->table.c_str(), it_lst->eq_descr.c_str());
		ExecSQL(sql);
	}

	for (it_lst = lst_res.begin(); it_lst != lst_res.end(); it_lst++)
	{
		sprintf(sql, "INSERT into PMS_PARAM.FDIR_ENABLE_RES (ID,DEV_ID,DEV_TYPE,DEV_DESCR) VALUES (\'%s\',\'%ld\',\'%s\',\'%s\')",groupid, it_lst->eq_id, it_lst->table.c_str(), it_lst->eq_descr.c_str());
		ExecSQL(sql);
	}
}

void form_fdir_txt(const FDIR_TASK fault)
{
	//D5000形成故障文本
}

long IDTransfor(long &id)
{
	map<int, int> map_table;

	map_table[g_db_psbob->GetTB("breaker")->table_id] = g_db_dms->GetTB("DMS_CB")->table_id;
	map_table[g_db_psbob->GetTB("aclinesegment")->table_id] = g_db_dms->GetTB("DMS_SEC")->table_id;
	map_table[g_db_psbob->GetTB("energyconsumer")->table_id] = g_db_dms->GetTB("DMS_LD")->table_id;
	map_table[g_db_psbob->GetTB("busbarsection")->table_id] = g_db_dms->GetTB("DMS_BUS")->table_id;
	map_table[g_db_psbob->GetTB("company")->table_id] = g_db_dms->GetTB("DMS_ST")->table_id;
	map_table[g_db_psbob->GetTB("substation")->table_id] = g_db_dms->GetTB("DMS_FI")->table_id;

	map<int, int>::const_iterator it;

	for (it = map_table.begin(); it != map_table.end(); it++)
	{
		map_table[it->second] = it->first;
	}

	KEY_STRU ks = { 0 };

	CCommon::long_to_keyid(id, &ks);

	it = map_table.find(ks.table_no);

	if (it != map_table.end())
	{
		ks.table_no = it->second;
		CCommon::keyid_to_long(&ks, (unsigned long*)&id);

		return id;
	}

	return 0;
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
	map_table["faultinfo"] = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_FI"));

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

//根据记录ID获取记录的英文标识
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

//返回故障模拟方案执行后向人机返回的报文
void thread_simu_return(void* param)
{
	int newsockfd = *((int*)param);

	while (true)
	{
		pthread_mutex_lock(&simu_sync_mutex);
		if (g_is_write_to_lib)
		{
			int rec_num, attr_num;
			char* buf = NULL;
			struct ORA_ATTR *attrs = NULL;
			int his_count = 0;
			sprintf(sql, "SELECT ID FROM PMS_PARAM.FDIR_FDIRECT ORDER BY ID DESC");
			pthread_mutex_lock(&oci_mutex);
			int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
			pthread_mutex_unlock(&oci_mutex);
			std::string cur_fault_id;
			int query_count = 0;

			if (ret != OCI_ERROR && rec_num > 0)
			{
				cur_fault_id = buf;
			}

			//通过故障ID查找故障信号，若其对应的故障信号包含了机器名发出的所有故障信号，视为故障处理方案计算完成，向人机返回报文
			buf = NULL;
			attrs = NULL;
			char return_str[1024] = {0};
			char simu_sql[1024] = {0};
			sprintf(simu_sql, "SELECT TM_ADD, KEY_ID FROM PMS_PARAM.FDIR_ALARM WHERE ID=\'%s\'", cur_fault_id.c_str());
			FDIR_POINT_TRIG_LIST pt_trig;
			pthread_mutex_lock(&oci_mutex);
			ret = g_oci->ReadWithBind(simu_sql, &buf, &rec_num, &attr_num, &attrs);
			pthread_mutex_unlock(&oci_mutex);

			if (ret >= 0 && rec_num > 0)
			{
				for (int i = 0; i < rec_num; i++)
				{
					FDIR_POINT_TRIG his_trig = {0};
					his_trig.tm_add.tv_sec = *(long*)(buf + i * (attrs[0].col_width + attrs[1].col_width));
					his_trig.alarm.key_id = *(long*)(buf + i * (attrs[0].col_width + attrs[1].col_width) + attrs[0].col_width);
					pt_trig.push_back(his_trig);
				}
			}

			if (simu_mac_trig.size() > 0 && pt_trig.size() > 0)
			{
				std::map<std::string, FDIR_POINT_TRIG_LIST>::iterator it_map = simu_mac_trig.begin();
				std::list<FDIR_POINT_TRIG>::iterator it_trig = pt_trig.begin();
				for (;it_map != simu_mac_trig.end(); it_map++)
				{
					std::list<FDIR_POINT_TRIG>::iterator it_map_trig = it_map->second.begin();
					for (; it_trig != pt_trig.end(); it_trig++)
					{
						for (; it_map_trig != it_map->second.end();)
						{
							if (it_trig->tm_add.tv_sec == it_map_trig->tm_add.tv_sec && it_trig->alarm.key_id == it_map_trig->alarm.key_id)
							{
								it_map->second.erase(it_map_trig++);
							}
							else
							{
								it_map_trig++;
								continue;
							}
						}

						if (it_trig != pt_trig.end() && it_map->second.size() > 0)
						{
							it_map_trig = it_map->second.begin();
							continue;
						}

						string cur_machine = it_map->first;
						if (it_map->second.size() == 0)
						{
							//获取模拟方案信息
							buf = NULL;
							attrs = NULL;
							sprintf(simu_sql, "SELECT T1.ID, T1.TYPE, T1.NAME, T1.USERID FROM PMS_PARAM.FDIR_SIG_SUMMARY T1, PMS_PARAM.FDIR_SIG_FAULT T2 WHERE T2.FAULT_ID=\'%s\' AND T1.ID=T2.ID",
									cur_fault_id.c_str());
							pthread_mutex_lock(&oci_mutex);
							ret = g_oci->ReadWithBind(simu_sql, &buf, &rec_num, &attr_num, &attrs);
							pthread_mutex_unlock(&oci_mutex);
							if (ret >= 0 && rec_num > 0)
							{
								char cur_plan_id[50] = {0};
								memcpy(cur_plan_id, buf, attrs[0].col_width);
								int plan_type = *(int*)(buf + attrs[0].col_width);
								char plan_name[100] = {0};
								char cur_usr[50] = {0};
								memcpy(plan_name, buf + attrs[0].col_width + attrs[1].col_width, attrs[2].col_width);
								memcpy(cur_usr, buf + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width, attrs[3].col_width);
								//发送XML响应头
								cout << head_xml_simu_exec << endl;
								ret = tcptools->Send(newsockfd, head_xml_simu_exec, strlen(head_xml_simu_exec));
								if (ret <= 0)
								{
									cout << "发送数据失败，" << strerror(errno) << endl;
								}

								//查找group_id
								sprintf(simu_sql, "SELECT GROUPID FROM PMS_PARAM.FDIR_FDIRECT WHERE ID=\'%s\'", cur_fault_id.c_str());
								buf = NULL;
								attrs = NULL;
								pthread_mutex_lock(&oci_mutex);
								int ret = g_oci->ReadWithBind(simu_sql, &buf, &rec_num, &attr_num, &attrs);
								pthread_mutex_unlock(&oci_mutex);
								std::string cur_group_id;

								if (ret != OCI_ERROR && rec_num > 0)
								{
									cur_group_id = buf;
								}

								//发送响应请求上下文信息
								sprintf(return_str, "<Simu_Plan  id=\"%s\" group_id=\"%s\" type=\"%d\" name=\"%s\" machine=\"%s\" user=\"%s\">\r\n",
										cur_plan_id, cur_group_id.c_str(), plan_type, plan_name, cur_machine.c_str(), cur_usr);

								cout << return_str << endl;
								ret = tcptools->Send(newsockfd, return_str, strlen(return_str));
								if (ret <= 0)
								{
									cout << "发送数据失败，" << strerror(errno) << endl;
								}

								//发送XML响应尾
								cout << tail_xml_simu << endl;
								ret = tcptools->Send(newsockfd, tail_xml_simu, strlen(tail_xml_simu));
								if (ret <= 0)
								{
									cout << "发送数据失败，" << strerror(errno) << endl;
								}

								g_is_write_to_lib = false;

								//删除临时创建但不保存的方案
								if (strstr(cur_plan_id, "1970/01/01") != NULL)
								{
									sprintf(simu_sql, "DELETE FROM PMS_PARAM.FDIR_SIG_SUMMARY WHERE ID=\'%s\'", cur_plan_id);
									ExecSQL(simu_sql);
								}

								pthread_mutex_unlock(&simu_sync_mutex);
								pthread_exit(0);

							}
							else
							{
								TRACE("故障模拟失败！\r\n");
							}

						}
					}

				}
			}
		}
		pthread_mutex_unlock(&simu_sync_mutex);
		usleep(1000);

	}
}


//创建开关电流值读取线程，用于计算恢复策略时读取开关电流平均值,
//具体方法是每10秒读取一次开关电流瞬时值，该值为开关三相电流值的最大值，随后取这6个值的平均值
void thread_get_ivalue(void *param)
{
	TRACE("计算平均电流值线程启动…\r\n");

	TB_DESCR *tb_breaker = const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker"));
	TB_DESCR * tb_dms_breaker = const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CB"));
	int size_cb = sizeof(PSBOB::CB);
	int size_dms_cb = sizeof(DMS::CB);

	if (tb_breaker == NULL || tb_dms_breaker == NULL)
	{
		TRACE("获取表指针失败！\r\n");
		return;
	}

	g_mapTempIValue.clear();
	list<long> lst_cb;
	char *buf = NULL;
	int index = 0;

	//先将pwrflow里的cb表所有开关保存起来，方便后面计算平均值时过滤掉一些多余的开关
	int ret = GetTableRecs(tb_dms_breaker, 1, &buf);
	if (ret < 0)
	{
		TRACE("读取表DMS_CB记录失败！\r\n");
		return;
	}

	int cnt_record = ret / size_dms_cb;
	DMS::CB *ptr = reinterpret_cast<DMS::CB *>(buf);
	for (index = 0; index < cnt_record; index++)
	{
		lst_cb.push_back(ptr->pid);
		ptr++;
	}

	BufFree(buf);
	buf = NULL;

	while (true)
	{
		ret = GetTableRecs(tb_breaker, 1, &buf);
		if (ret < 0)
		{
			TRACE("读取表breaker记录失败！\r\n");
			continue;
		}

		cnt_record = ret / size_cb;
		PSBOB::CB *ptr2 = reinterpret_cast<PSBOB::CB *>(buf);

		for (index = 0; index < cnt_record; index++)
		{
			//开关在pwrflow库的CB表里不存在
			if (::find(lst_cb.begin(), lst_cb.end(), ptr2->id) == lst_cb.end())
			{
				ptr2++;
				continue;
			}

			g_mapTempIValue[ptr2->id].push_back(ptr2->i);

			//最多只计算6个点的平均值，多余的点从列表删除
			if (g_mapTempIValue[ptr2->id].size() > 6)
			{
				g_mapTempIValue[ptr2->id].pop_front();
			}

			ptr2++;
		}

		BufFree(buf);
		buf = NULL;

		//求取电流平均值
		for (map<long, list<float> >::const_iterator it = g_mapTempIValue.begin(); it != g_mapTempIValue.end(); it++)
		{
			float fIVSum = 0;
			for (list<float>::const_iterator it_value = it->second.begin(); it_value != it->second.end(); it_value++)
			{
				fIVSum += fabs(*it_value);
			}

			pthread_mutex_lock(&avg_i_mutex);
			g_mapIValue[it->first] = fIVSum / it->second.size();
			pthread_mutex_unlock(&avg_i_mutex);
		}

		//10秒钟查询一次
		sleep(10);
	}
}
