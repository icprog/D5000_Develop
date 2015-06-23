/*
 * test.cpp
 *
 *  Created on: 2013-4-19
 *      Author: BetaKDE
 */

#include<c++/4.1.1/ext/algorithm>
#include"DBPSBOB.h"
#include <db_struct_scada.h>
#include <sys_menu_info.h>
#include <rtdb_api.h>
#include<iostream>
#include<list>
#include<stack>
#include<map>
#include<assert.h>

typedef subcontrolarea_scada 	DV;
typedef energyconsumer_scada 	LD;
typedef aclinesegment_scada		SEC;
//typedef subctrlarea_substation_scada OO_R_DV_ST;
typedef breaker_scada			CB;
//typedef company_scada			CO;
typedef substation_scada		ST;
typedef busbarsection_scada		BUS;
typedef disconnector_scada		SWITCH;

using namespace std;

typedef enum
{
	_CB, _LD, _BUS, _SEC, _SWITCH
} EQ_TYPE;

typedef struct
{
	union
	{
		CB cb;
		LD ld;
		BUS bus;
		SEC sec;
		SWITCH disconnector;
	} data;

	EQ_TYPE type;
} NODE, *PNODE;

//简易内存自动管理类
class MyBufManager
{
	typedef void* 	PVOID;
	typedef CB*   	PCB;
	typedef LD*		PLD;
	typedef SEC*	PSEC;
	typedef BUS*	PBUS;
	typedef SWITCH*	PSWITCH;
public:
	MyBufManager(int size)
	{
		p = malloc(size);
	}
	~MyBufManager()
	{
		free(p);
	}
	operator PVOID() const
	{
		return (PVOID)p;
	}
	operator PCB() const
	{
		return (PCB)p;
	}
	operator PSEC() const
	{
		return (PSEC)p;
	}
	operator PBUS() const
	{
		return (PBUS)p;
	}
	operator PLD() const
	{
		return (PLD)p;
	}
	operator PSWITCH() const
	{
		return (PSWITCH)p;
	}

protected:
	void *p;
};

DB_PSBOB *g_db_psbob = NULL;
stack<NODE> g_lstPath;
list<long> g_lstFound;
NODE node = { 0 };
bool bDebug = false;
map<long, list<stack<NODE> > >	g_map_nd_path;
const int max_record = 10;

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

int FindCB(CB* buf, const long nd)
{
	int cnt = 0;

	int ret = GetRecsByAttr(g_db_psbob->GetTB("breaker"), "ind", (const char*)&nd, buf, sizeof(CB) * max_record);//buf, "ind", (char*)&nd, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(CB) * max_record);

	if (ret > 0)
	{
		cnt = ret / sizeof(CB);
	}

	ret = GetRecsByAttr(g_db_psbob->GetTB("breaker"), "jnd", (const char*)&nd, buf + cnt, sizeof(CB) * (max_record - cnt));

	if (ret > 0)
	{
		cnt += ret / sizeof(CB);
	}

	return cnt;
}

int FindSWITCH(SWITCH* buf, const long nd)
{
	int cnt = 0;

	int ret = GetRecsByAttr(g_db_psbob->GetTB("disconnector"), "ind", (const char*)&nd, buf, sizeof(SWITCH) * max_record);

	if (ret > 0)
	{
		cnt = ret / sizeof(SWITCH);
	}

	ret = GetRecsByAttr(g_db_psbob->GetTB("disconnector"), "jnd", (const char*)&nd, buf + cnt, sizeof(SWITCH) * (max_record - cnt));

	if (ret > 0)
	{
		cnt += ret / sizeof(SWITCH);
	}

	return cnt;
}

int FindLD(LD* buf, const long nd)
{
	int cnt = 0;

	int ret = GetRecsByAttr(g_db_psbob->GetTB("energyconsumer"), "nd", (const char*)&nd, buf, sizeof(LD) * max_record);

	if (ret > 0)
	{
		cnt = ret / sizeof(LD);
	}

	return cnt;
}

int FindSEC(SEC* buf, const long nd)
{
	int cnt = 0;

	MyBufManager bf_end(max_record * sizeof(aclineend_scada));

	int ret = GetRecsByAttr(g_db_psbob->GetTB("aclineend"), "nd", (const char*) &nd, (void*)bf_end, sizeof(aclineend_scada) * max_record);

	if (ret > 0)
	{
		int max = ret / sizeof(SEC);

		aclineend_scada *p = (void*) (bf_end);
		SEC* pbuf = buf;
		for (int i = 0; i < max; i++)
		{
			if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), (char*) &p->aclnseg_id, sizeof(SEC), (char*) pbuf) >= 0)
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

int FindBUS(BUS* buf, const long nd)
{
	int cnt = 0;

	MyBufManager bf_bus(max_record * sizeof(BUS));

	int ret = GetRecsByAttr(g_db_psbob->GetTB("busbarsection"), "nd", (const char*)&nd, (void*)bf_bus, sizeof(BUS) * max_record);

	if(ret > 0)
	{
		cnt = ret / sizeof(BUS);

		int j = 0;

		for (int i = 0; i < cnt; i++)
		{
			substation_scada psbob_st = { 0 };
			if (oodbread_rk(&psbob_st, &(((BUS*) bf_bus)[i].st_id), const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
			{
				if (psbob_st.st_type == MENU_FAC_TYPE_CABLEBRANCHBOX)	continue;
				if (psbob_st.st_type == MENU_FAC_TYPE_DISTRIBUTEBOX)	continue;
				if (psbob_st.st_type == MENU_FAC_TYPE_RINGMAINUNIT)		continue;
				if (psbob_st.st_type == MENU_FAC_TYPE_SWITCHING)		continue;
			}
			else
			{
				continue;
			}

			memcpy(&buf[j], &((BUS*) bf_bus)[i], sizeof(BUS));
			j++;
		}

		cnt = j;
	}

	return cnt;
}

void Find(const long nd, const long org_nd, NODE &data)
{
	//assert(nd > 0 && id > 0);

	g_lstPath.push(data);

	MyBufManager bf_bus(max_record * sizeof(BUS));
	MyBufManager bf_cb(max_record * sizeof(CB));
	MyBufManager bf_ld(max_record * sizeof(LD));
	MyBufManager bf_sec(max_record * sizeof(SEC));
	MyBufManager bf_switch(max_record * sizeof(SWITCH));
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

	if (cnt > 0)
	{
		memcpy(&node.data.bus, &((BUS*) bf_bus)[0], sizeof(BUS));
		node.type = _BUS;
		g_lstPath.push(node);
		g_map_nd_path[org_nd].push_back(g_lstPath);
		g_lstPath.pop();	//弹出母线
		g_lstPath.pop();	//弹出其它设备

		return;
	}

	//SEC
	cnt = FindSEC(bf_sec, nd);

	for (i = 0; i < cnt; i++)
	{
		if (std::find(g_lstFound.begin(), g_lstFound.end(), ((SEC*) bf_sec)[i].id) == g_lstFound.end())
		{
			if (bDebug)
				printf("PUSH->sec %-20ld[%-30s]%-20ld%-20ld[%-20ld]\r\n", ((SEC*) bf_sec)[i].id, ((SEC*) bf_sec)[i].name, ((SEC*) bf_sec)[i].ind, ((SEC*) bf_sec)[i].jnd, nd);
			g_lstFound.push_back(((SEC*) bf_sec)[i].id);
			memcpy(&node.data.sec, &((SEC*) bf_sec)[i], sizeof(SEC));
			node.type = _SEC;

			if (nd == ((SEC*) bf_sec)[i].ind)
			{
				Find(((SEC*) bf_sec)[i].jnd, org_nd, node);
			}
			else
			{
				Find(((SEC*) bf_sec)[i].ind, org_nd, node);
			}
		}
	}

	//CB
	cnt = FindCB(bf_cb, nd);

	for (i = 0; i < cnt; i++)
	{
		if (std::find(g_lstFound.begin(), g_lstFound.end(), ((CB*) bf_cb)[i].id) == g_lstFound.end())
		{
			if (bDebug)
				printf("PUSH->cb  %-20ld[%-30s]%-20ld%-20ld[%-20ld]\r\n", ((CB*) bf_cb)[i].id, ((CB*) bf_cb)[i].name, ((CB*) bf_cb)[i].ind, ((CB*) bf_cb)[i].jnd, nd);
			g_lstFound.push_back(((CB*) bf_cb)[i].id);

			memcpy(&node.data.cb, &((CB*) bf_cb)[i], sizeof(CB));
			node.type = _CB;

			if (nd == ((CB*) bf_cb)[i].ind)
			{
				Find(((CB*) bf_cb)[i].jnd, org_nd, node);
			}
			else
			{
				Find(((CB*) bf_cb)[i].ind, org_nd, node);
			}
		}
	}

	//SWITCH
	cnt = FindSWITCH(bf_switch, nd);

	for (i = 0; i < cnt; i++)
	{
		if (std::find(g_lstFound.begin(), g_lstFound.end(), ((SWITCH*) bf_switch)[i].id) == g_lstFound.end())
		{
			if (bDebug)
				printf("PUSH->swh  %-20ld[%-30s]%-20ld%-20ld[%-20ld]\r\n", ((SWITCH*) bf_switch)[i].id, ((SWITCH*) bf_switch)[i].name, ((SWITCH*) bf_switch)[i].ind, ((SWITCH*) bf_switch)[i].jnd, nd);
			g_lstFound.push_back(((SWITCH*) bf_switch)[i].id);

			memcpy(&node.data.cb, &((SWITCH*) bf_switch)[i], sizeof(SWITCH));
			node.type = _SWITCH;

			if (nd == ((SWITCH*) bf_switch)[i].ind)
			{
				Find(((SWITCH*) bf_switch)[i].jnd, org_nd, node);
			}
			else
			{
				Find(((SWITCH*) bf_switch)[i].ind, org_nd, node);
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
		int size = sizeof(DV);
		int cnt = ret / size;

		DV dv = { 0 };

		for (int i = 0; i < cnt; i++)
		{
			while (g_lstPath.size() > 0)
			{
				g_lstPath.pop();
			}
			memcpy(&dv, buf + i * size, size);

			if (dv.dtype & (1 << (MENU_DV_TYPE_FD - 1)))
			{
				if(dv.nd_id > 0)
				{
					NODE data = { 0 };
					data.type = _LD;

					data.data.ld.nd = dv.nd_id;

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
		int size = sizeof(LD);
		int cnt = ret / size;

		LD ld = { 0 };
		ST st = { 0 };

		for (int i = 0; i < cnt; i++)
		{
			while(g_lstPath.size() > 0)
			{
				g_lstPath.pop();
			}
			memcpy(&ld, buf + i * size, size);

			if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), (char*) &ld.st_id, sizeof(st), (char*) &st) >= 0)
			{
				if(st.st_type == MENU_FAC_TYPE_SUBSTN)
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

int main(int argc, char *argv[])
{
	g_db_psbob = new DB_PSBOB;

	if (g_db_psbob->Init() == OO_FAIL)
	{
		printf("初始化实时库失败，错误号：%d\r\n", _oodb_errno);
		return 1;
	}

	printf("初始化……\r\n");

	build_map2();

	printf("共建立%d对路径映射关系！\r\n", g_map_nd_path.size());
	while (true)
	{
		long nd, cb;

		printf("\r\n请入结点和馈线开关ID：");
		scanf("%ld %ld", &nd, &cb);

		map<long, list<stack<NODE> > >::const_iterator it = g_map_nd_path.find(nd);

		if (it != g_map_nd_path.end())
		{
			printf("%-20s%-20s|%-20s|%-20s|%s\r\n", "EQ", "ID", "ND", "ZND", "DESCR");

			bool bOpen = true;

			for (list<stack<NODE> >::const_iterator it_lst = it->second.begin(); it_lst != it->second.end(); it_lst++)
			{
				g_lstPath = *it_lst;
				cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
				while (g_lstPath.size() > 0)
				{
					NODE *p = &g_lstPath.top();

					if (p->type == _CB)
					{
						printf("%-20s%-20ld|%-20ld|%-20ld|%s\r\n", "breaker", p->data.cb.id, p->data.cb.ind, p->data.cb.jnd, p->data.cb.name);

						if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), (char*) &(p->data.cb.id), sizeof(CB), (char*) &(p->data.cb)) < 0)
						{
							printf("读取ID为%ld的开关记录失败！\r\n", p->data.cb.id);
						}

						if (p->data.cb.point <= 0)
						{
							printf("该开关为分闸状态，路径无效！\r\n");
							break;
						}
					}
					else if (p->type == _SWITCH)
					{
						printf("%-20s%-20ld|%-20ld|%-20ld|%s\r\n", "disconnector", p->data.disconnector.id, p->data.disconnector.ind, p->data.disconnector.jnd, p->data.disconnector.name);

						if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("disconnector")), (char*) &(p->data.disconnector.id), sizeof(SWITCH), (char*) &(p->data.disconnector)) < 0)
						{
							printf("读取ID为%ld的刀闸记录失败！\r\n", p->data.disconnector.id);
						}

						if (p->data.disconnector.point <= 0)
						{
							printf("该刀闸为分闸状态，路径无效！\r\n");
							break;
						}
					}
					else if (p->type == _LD)
					{
						printf("%-20s%-20ld|%-20ld|%-20s|%s\r\n", "energyconsumer", p->data.ld.id, p->data.ld.nd, "null", p->data.ld.name);
					}
					else if (p->type == _BUS)
					{
						printf("%-20s%-20ld|%-20ld|%-20s|%s\r\n", "busbarsection", p->data.bus.id, p->data.bus.nd, "null", p->data.bus.name);

						if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), (char*) &(p->data.bus.id), sizeof(BUS), (char*) &(p->data.bus)) < 0)
						{
							printf("读取ID为%ld的母线记录失败！\r\n", p->data.bus.id);
						}

						if (p->data.bus.v <= 0)
						{
							printf("该母线无电压，路径无效！\r\n");
							break;
						}
					}
					else
					{
						printf("%-20s%-20ld|%-20ld|%-20ld|%s\r\n", "aclinesegment", p->data.sec.id, p->data.sec.ind, p->data.sec.jnd, p->data.sec.name);
					}

					g_lstPath.pop();
				}

				if (g_lstPath.size() == 0)
				{
					bOpen = true;
					printf("该供电路径有效！\r\n");
				}
			}

			printf("\r\n=============馈线开关%ld为%s状态！=============\r\n", cb, bOpen ? "合闸" : "分闸");
		}
	}
}
