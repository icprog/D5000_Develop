/*
 * test.cpp
 *
 *  Created on: 2013-4-19
 *      Author: BetaKDE
 */

#include<algorithm>
#include"DBPSBOB.h"
#ifdef _SCADA
#include <db_struct_scada.h>
#else
#include <db_struct_dscada.h>
#endif
#include <sys_menu_info.h>
#include <rtdb_api.h>
#include<iostream>
#include<list>
#include<stack>
#include<assert.h>

#ifdef _SCADA
typedef subcontrolarea_scada 	DV;
typedef energyconsumer_scada 	LD;
typedef aclinesegment_scada		SEC;
typedef aclineend_scada 		END;
typedef subctrlarea_substation_scada OO_R_DV_ST;
typedef breaker_scada			CB;
typedef company_scada			CO;
typedef substation_scada		ST;
typedef busbarsection_scada		BUS;
typedef disconnector_scada		SWITCH;
#else
typedef subcontrolarea_dscada 	DV;
typedef energyconsumer_dscada 	LD;
typedef aclinesegment_dscada		SEC;
typedef aclineend_dscada 		END;
typedef subctrlarea_substation_dscada OO_R_DV_ST;
typedef breaker_dscada			CB;
typedef company_dscada			CO;
typedef substation_dscada		ST;
typedef busbarsection_dscada		BUS;
typedef disconnector_dscada		SWITCH;
#endif

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

	MyBufManager bf_end(max_record * sizeof(END));

	int ret = GetRecsByAttr(g_db_psbob->GetTB("aclineend"), "nd", (const char*) &nd, (void*)bf_end, sizeof(END) * max_record);

	if (ret > 0)
	{
		int max = ret / sizeof(SEC);

		END *p = (void*) (bf_end);
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
				printf("aclinesegment中找不着ID为%ld的记录,nd=%ld\r\n", p->aclnseg_id, nd);
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
			ST psbob_st = { 0 };
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

bool Find(const long nd, const long id, NODE &data)
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
		return false;
	}


	//BUS
	cnt = FindBUS(bf_bus, nd);

	for (i = 0; i < cnt; i++)
	{
		if (id == ((BUS*) bf_bus)[i].id)
		{
			memcpy(&node.data.bus, &((BUS*) bf_bus)[i], sizeof(BUS));
			node.type = _BUS;
			g_lstPath.push(node);
			return true;
		}
	}

	if (cnt > 0 && i >= cnt)
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
		return false;
	}

	//LD
	cnt = FindLD(bf_ld, nd);

	for (i = 0; i < cnt; i++)
	{
		if (id == ((LD*) bf_ld)[i].id)
		{
			memcpy(&node.data.ld, &((LD*) bf_ld)[i], sizeof(LD));
			node.type = _LD;
			g_lstPath.push(node);
			return true;
		}
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

			if (id == ((SEC*) bf_sec)[i].id)
			{
				g_lstPath.push(node);
				return true;
			}

			if (nd == ((SEC*) bf_sec)[i].ind)
			{
				if (Find(((SEC*) bf_sec)[i].jnd, id, node))
				{
					return true;
				}
			}
			else
			{
				if (Find(((SEC*) bf_sec)[i].ind, id, node))
				{
					return true;
				}
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

			if (id == ((CB*) bf_cb)[i].id)
			{
				g_lstPath.push(node);
				return true;
			}

			if (nd == ((CB*) bf_cb)[i].ind)
			{
				if (Find(((CB*) bf_cb)[i].jnd, id, node))
				{
					return true;
				}
			}
			else
			{
				if (Find(((CB*) bf_cb)[i].ind, id, node))
				{
					return true;
				}
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

			if (id == ((SWITCH*) bf_switch)[i].id)
			{
				g_lstPath.push(node);
				return true;
			}

			if (nd == ((SWITCH*) bf_switch)[i].ind)
			{
				if (Find(((SWITCH*) bf_switch)[i].jnd, id, node))
				{
					return true;
				}
			}
			else
			{
				if (Find(((SWITCH*) bf_switch)[i].ind, id, node))
				{
					return true;
				}
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

	return false;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("usage:FindTest dev1 dev2\r\n");
		return 0;
	}

	if (argc > 3)
	{
		bDebug = true;
	}
	else
	{
		bDebug = false;
	}

	g_db_psbob = new DB_PSBOB;
	long id1 = atol(argv[1]);
	long id2 = atol(argv[2]);

#ifdef _SCADA
	if (g_db_psbob->Init("psbob", "realtime", "scada") == OO_FAIL)
#else
	if (g_db_psbob->Init("psbob", "realtime", "dscada") == OO_FAIL)
#endif
	{
		printf("初始化实时库失败，错误号：%d\r\n", _oodb_errno);
		return 1;
	}

	KEY_STRU ks1 = { 0 }, ks2 = { 0 };

	CCommon::long_to_keyid(id1, &ks1);
	CCommon::long_to_keyid(id2, &ks2);

	TB_DESCR* tb1 = g_db_psbob->GetTB(ks1.table_no);
	TB_DESCR* tb2 = g_db_psbob->GetTB(ks2.table_no);

	if(tb1 == NULL || tb2 == NULL)
	{
		printf("设备1(表号:%d)或设备2(表号:%d)的ID不存在，请检查\r\n", ks1.table_no, ks2.table_no);
		return -1;
	}

	NODE data = { 0 };

	std::list<long> lst_nd;

	if(strcasecmp(tb1->table_name, "breaker") == 0)
	{
		int ret = oodbread_rk(&data.data.cb, (char*) &id1, const_cast<TB_DESCR*>(tb1), sizeof(CB));
		if (ret == OO_FAIL)
		{
			printf("读取CB表ID为%ld的记录失败\r\n", id1);
			return 2;
		}

		data.type = _CB;
		lst_nd.push_back(data.data.cb.ind);
		lst_nd.push_back(data.data.cb.jnd);
	}
	else if(strcasecmp(tb1->table_name, "busbarsection") == 0)
	{
		int ret = oodbread_rk(&data.data.bus, (char*) &id1, const_cast<TB_DESCR*>(tb1), sizeof(BUS));
		if (ret == OO_FAIL)
		{
			printf("读取BUS表ID为%ld的记录失败\r\n", id1);
			return 2;
		}

		data.type = _BUS;
		lst_nd.push_back(data.data.bus.nd);
	}
	else if(strcasecmp(tb1->table_name, "aclinesegment") == 0)
	{
		int ret = oodbread_rk(&data.data.sec, (char*) &id1, const_cast<TB_DESCR*>(tb1), sizeof(SEC));
		if (ret == OO_FAIL)
		{
			printf("读取SEC表ID为%ld的记录失败\r\n", id1);
			return 2;
		}

		data.type = _SEC;
		lst_nd.push_back(data.data.sec.ind);
		lst_nd.push_back(data.data.sec.jnd);
	}
	else if(strcasecmp(tb1->table_name, "energyconsumer") == 0)
	{
		int ret = oodbread_rk(&data.data.ld, (char*) &id1, const_cast<TB_DESCR*>(tb1), sizeof(LD));
		if (ret == OO_FAIL)
		{
			printf("读取LD表ID为%ld的记录失败\r\n", id1);
			return 2;
		}

		data.type = _LD;
		lst_nd.push_back(data.data.ld.nd);
	}
	else if(strcasecmp(tb1->table_name, "disconnector") == 0)
	{
		int ret = oodbread_rk(&data.data.disconnector, (char*) &id1, const_cast<TB_DESCR*>(tb1), sizeof(SWITCH));
		if (ret == OO_FAIL)
		{
			printf("读取SWITCH表ID为%ld的记录失败，错误号：%ld\r\n", id1, _oodb_errno);
			return 2;
		}

		data.type = _SWITCH;
		lst_nd.push_back(data.data.disconnector.ind);
		lst_nd.push_back(data.data.disconnector.jnd);
	}
	else
	{
		printf("不支持的设备类型%s\r\n", tb1->table_name);
		return -2;
	}

	cout << "正在查找路径，请稍候……" << endl;
	time_t tb = time(NULL);

	g_lstFound.push_back(id1);

	for (std::list<long>::const_iterator it = lst_nd.begin(); it != lst_nd.end(); it++)
	{
		if (Find(*it, id2, data))
		{
			cout << "/************路径如下****************/" << endl;
			printf("%-20s%-20s|%-20s|%-20s|%s\r\n", "EQ", "ID", "ND", "ZND", "DESCR");
			cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
			while (g_lstPath.size() > 0)
			{
				NODE *p = &g_lstPath.top();

				if (p->type == _CB)
				{
					printf("%-20s%-20ld|%-20ld|%-20ld|%s\r\n", "breaker", p->data.cb.id, p->data.cb.ind, p->data.cb.jnd, p->data.cb.name);
				}
				else if (p->type == _SWITCH)
				{
					printf("%-20s%-20ld|%-20ld|%-20ld|%s\r\n", "disconnector", p->data.disconnector.id, p->data.disconnector.ind, p->data.disconnector.jnd, p->data.disconnector.name);
				}
				else if (p->type == _LD)
				{
					printf("%-20s%-20ld|%-20ld|%-20s|%s\r\n", "energyconsumer", p->data.ld.id, p->data.ld.nd, "null", p->data.ld.name);
				}
				else if (p->type == _BUS)
				{
					printf("%-20s%-20ld|%-20ld|%-20s|%s\r\n", "busbarsection", p->data.bus.id, p->data.bus.nd, "null", p->data.bus.name);
				}
				else
				{
					printf("%-20s%-20ld|%-20ld|%-20ld|%s\r\n", "aclinesegment", p->data.sec.id, p->data.sec.ind, p->data.sec.jnd, p->data.sec.name);
				}

				g_lstPath.pop();
			}

			printf("total cost:%lds\r\n", time(NULL) - tb);
			return 0;
		}
	}

	cout << "!!!!!!!!!!!!!!!!!!!无路径！" << endl;

	printf("total cost:%lds\r\n", time(NULL) - tb);
}
