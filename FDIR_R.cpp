/*
 * FDIR_R.cpp
 *
 *  Created on: 2011-11-8
 *      Author: BetaKDE
 */
#include "FDIR_R.h"
#include "warn_define.h"

bool bDisplay = false;

FDIR_R::FDIR_R(DSBOB* pdsbob)
{

#if PROCESS_ONE_BY_ONE
	dsbob = pdsbob;
#else
	dsbob = new DSBOB; //pdsbob;

	dsbob->analog_tb = pdsbob->analog_tb;
	dsbob->bs_tb = pdsbob->bs_tb;
	dsbob->bus_tb = pdsbob->bus_tb;
	dsbob->cb_tb = pdsbob->cb_tb;
	dsbob->co_tb = pdsbob->co_tb;
	dsbob->cp_tb = pdsbob->cp_tb;
	dsbob->digital_tb = pdsbob->digital_tb;
	dsbob->global_tb = pdsbob->global_tb;
	dsbob->island_tb = pdsbob->island_tb;
	dsbob->ld_tb = pdsbob->ld_tb;
	dsbob->nd_tb = pdsbob->nd_tb;
	dsbob->r_co_st_tb = pdsbob->r_co_st_tb;
	dsbob->r_nd_cb_tb = pdsbob->r_nd_cb_tb;
	dsbob->r_nd_sec_tb = pdsbob->r_nd_sec_tb;
	dsbob->r_nd_xf_tb = pdsbob->r_nd_xf_tb;
	dsbob->r_nd_zn_tb = pdsbob->r_nd_zn_tb;
	dsbob->r_st_bus_tb = pdsbob->r_st_bus_tb;
	dsbob->r_st_cb_tb = pdsbob->r_st_cb_tb;
	dsbob->r_st_cp_tb = pdsbob->r_st_cp_tb;
	dsbob->r_st_ld_tb = pdsbob->r_st_ld_tb;
	dsbob->r_st_so_tb = pdsbob->r_st_so_tb;
	dsbob->r_st_xfmr_tb = pdsbob->r_st_xfmr_tb;
	dsbob->r_st_zn_tb = pdsbob->r_st_zn_tb;
	dsbob->r_xfmr_xf_tb = pdsbob->r_xfmr_xf_tb;
	dsbob->r_zn_ld_tb = pdsbob->r_zn_ld_tb;
	dsbob->r_zn_sec_tb = pdsbob->r_zn_sec_tb;
	dsbob->sec_tb = pdsbob->sec_tb;
	dsbob->so_tb = pdsbob->so_tb;
	dsbob->st_tb = pdsbob->st_tb;
	dsbob->tapty_tb = pdsbob->tapty_tb;
	dsbob->xf_tb = pdsbob->xf_tb;
	dsbob->xfmr_tb = pdsbob->xfmr_tb;
	dsbob->zn_tb = pdsbob->zn_tb;
#endif
	tp = new TP(dsbob);

	fdir_cb_tb = new TTable<CLS_FDIR_CB>(GetTableSize(g_db_fdir->GetTB("FDIR_CB")));
	fdir_zone_tb = new TTable<CLS_FDIR_ZONE>(GetTableSize(g_db_fdir->GetTB("FDIR_ZONE")));
	fdir_zone_cb_tb = new TTable<CLS_FDIR_ZONE_CB>(GetTableSize(g_db_fdir->GetTB("FDIR_ZONE_CB")));
//	fdir_restore_tb = new TTable<CLS_FDIR_RESTORE>(mx_fdir_restore);
	fdir_isolate_tb = new TTable<CLS_FDIR_ISOLATE>(mx_fdir_isolate);
	fdir_isolate_tb2 = new TTable<CLS_FDIR_ISOLATE>(mx_fdir_isolate);
	lFaultStId = 0;
	cur_father = NULL;
}

FDIR_R::~FDIR_R()
{
#if !PROCESS_ONE_BY_ONE
	delete dsbob;
#endif
	delete tp;

	delete fdir_cb_tb;
	delete fdir_zone_tb;
	delete fdir_zone_cb_tb;
//	delete fdir_restore_tb;
	delete fdir_isolate_tb;
	delete fdir_isolate_tb2;
}


const FDIR_R& FDIR_R::operator =(const FDIR_R& fdir_r)
{
	memcpy(&data_start, &fdir_r.data_start, sizeof(FAULT_START));

	lst_restore = fdir_r.lst_restore;
	lst_dev = fdir_r.lst_dev;
	lst_point = fdir_r.lst_point;
	lst_err_point = fdir_r.lst_err_point;
	lst_mis_point = fdir_r.lst_mis_point;
	lst_warning = fdir_r.lst_warning;
	lst_iso_failed = fdir_r.lst_iso_failed;
	lst_isolated = fdir_r.lst_isolated;

	lv_cb = fdir_r.lv_cb;
	lv_bus = fdir_r.lv_bus;
	lv_so = fdir_r.lv_so;

	faultzone = fdir_r.faultzone;
	faulticb = fdir_r.faulticb;
	faultbus = fdir_r.faultbus;
	fault_head = fdir_r.fault_head;
	fault_tail = fdir_r.fault_tail;

	dsbob = fdir_r.dsbob;
	tp = fdir_r.tp;

	fdir_cb_tb = fdir_r.fdir_cb_tb;
	fdir_zone_tb = fdir_r.fdir_zone_tb;
	fdir_zone_cb_tb = fdir_r.fdir_zone_cb_tb;
	fdir_restore_tb = fdir_r.fdir_restore_tb;
	fdir_isolate_tb = fdir_r.fdir_isolate_tb;
	fdir_restore_tb2 = fdir_r.fdir_restore_tb2;
	fdir_isolate_tb2 = fdir_r.fdir_isolate_tb2;
	lst_up_iso_cb = fdir_r.lst_up_iso_cb;
	lst_edge_cb = fdir_r.lst_edge_cb;

	map_cb_restore = fdir_r.map_cb_restore;

	return *this;
}

void FDIR_R::setfaultzone(int newzone)
{
	assert(newzone >= 0);
	faultzone = newzone;
}

int FDIR_R::getfaultzone()
{
	assert(faultzone >= 0);

	//return fdir_zone_tb->Data[faultzone].id;
	return faultzone;
}

int FDIR_R::getfaultjxcb()
{
//	assert(faultjxcb >= 0);
	return faultjxcb;
}
void FDIR_R::setfaultjxcb(int icb)
{
	//杨勇 20150204 防止因母线故障执行失败后导致DA挂掉
	//assert(icb >= 0);
	faultjxcb = icb;
}
int FDIR_R::getisozone()
{
	if (isozone >= 0)
	{
		return fdir_zone_tb->Data[isozone].id;
	}
	else
		return -1;
}

int FDIR_R::getfaultautoopenzone()
{
	if (faultauto_open >= 0)
	{
		return fdir_zone_tb->Data[faultauto_open].id;
	}
	else
		return -1;
}
//获取上游区段，tozone是下游的终止区段，fmzone是上游的开始区段，遍历从fmzone开始的下游，直到断开的开关或者tozone区段为止tianyq
void FDIR_R::getlstupzone(int fmzone, int tozone, list<int> &lstzone)
{
	assert(fmzone >= 0);
	assert(tozone >= 0);
	int cbptr, cbnum, icb, i;
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();

	lstzone.push_back(fmzone);

	cbptr = (fdir_zone + fmzone)->cbptr;
	cbnum = (fdir_zone + fmzone)->cbnum;

	for (i = 0; i < cbnum; i++)
	{
		icb = (fdir_zone_cb + cbptr + i)->icb;

		if (icb != (fdir_zone + fmzone)->icb)
		{
			//开关是合的状态时才会深度优先递归查找区段
			if(!((fdir_cb + icb)->topflags & FDIR_CB_OPEN))
			{
				int nextzone = FindDownZone(icb);
				if((nextzone != tozone) && (nextzone != -1))
				{
					getlstupzone(nextzone, tozone, lstzone);
				}
			}
		}
	}
}
void FDIR_R::getlstfaultzone(int zone, list<int> &lstzone)
{
	assert(zone >= 0);
	int cbptr, cbnum, icb, i;
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	list<int> lst_tmp_iso_cb;

	lstzone.push_back(zone);

	lst_tmp_iso_cb.clear();

	cbptr = (fdir_zone + zone)->cbptr;
	cbnum = (fdir_zone + zone)->cbnum;

	for (i = 0; i < cbnum; i++)
	{
		icb = (fdir_zone_cb + cbptr + i)->icb;

		if (icb != (fdir_zone + zone)->icb)
		{
			lst_tmp_iso_cb.push_back(icb);
		}
	}

	for (list<int>::iterator it = lst_tmp_iso_cb.begin(); it != lst_tmp_iso_cb.end(); it++)
	{
		CLS_CB *tmp_cb = dsbob->cb_tb->Find(CLS_CB((fdir_cb + *it)->cb_id));
		PSBOB::CB psbob_cb = { 0 };

		if (((oodbread_rk(&psbob_cb, &tmp_cb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL) || !is_controllable(tmp_cb->pid) || IsCBFailed(tmp_cb->pid)) && (psbob_cb.point == 1))
		{
			FDIR_CB_INFO info = { 0 };

			if (psbob_cb.id == 0)
			{
				info.cb_id= tmp_cb->id;
				strcpy(info.cb_descr, tmp_cb->descr);
				info.reason = NO_POINT;
				lst_cb_info.push_back(info);
			}
			else if (!is_controllable(tmp_cb->pid))
			{
				info.cb_id = tmp_cb->id;
				info.pt_id=tmp_cb->pid;
				strcpy(info.cb_descr, tmp_cb->descr);
//				info.pt_control = psbob_point.control;
//				info.pt_quality = psbob_point.quality;
//				info.pt_timexc = psbob_cb.psbob_point.timexc;
				info.reason = NO_CONTROL;
				lst_cb_info.push_back(info);
			}
			else
			{
				//不需要处理
			}

			TRACE("开关%ld(%s)不存在或非可遥控开关！\r\n", tmp_cb->id, tmp_cb->descr);

			int idownzone = FindDownZone(*it);

			if (idownzone >= 0)
			{
				getlstfaultzone(idownzone, lstzone);
			}
		}
		else
		{
		}
	}

}

//获取断路器表[遥信值]所在列号
int GetPntIDColumn()
{
	int col = -1;
	FIELD_PARA_STRU *pfps = NULL;

	int ret = GetFieldsPara(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), &pfps);
	if (ret >= 0)
	{
		int cnt = ret / sizeof(FIELD_PARA_STRU);

		for (int i = 0; i < cnt; i++)
		{
			if (strcmp(pfps[i].field_name_eng, "point") == 0)
			{
				col = pfps[i].column_id;
				break;
			}
		}

		BufFree(pfps);
	}

	return col;
}

//判断一个开关是否可以遥控，可遥控返回true，否则返回false
bool is_controllable(long cb_id)
{
	KEY_STRU ks = { 0 };
	long kid = cb_id;

	id_to_pid(kid);

	CCommon::long_to_keyid(kid, &ks);

	ks.field_id = GetPntIDColumn();

	unsigned long pt_id = 0;
	CCommon::keyid_to_long(&ks, &pt_id);

	PSBOB::CD data = { 0 };

	if (oodbread_ra(&data, "pnt_id", &pt_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("controldigital")), sizeof(data)) < 0)
	{
		TRACE("读取controldigital（801）表pnt_id为%ld的记录失败,对应的开关设备ID为%ld\r\n", pt_id, kid);
		return false;
	}
	else
	{
		TRACE("读取开关设备%ld在表中的记录成功！控制位%d\n", kid,data.exp_state);
	}

	return data.exp_state > 0;
}

const list<int>* FDIR_R::get_edge_cb()
{
	return &lst_edge_cb;
}

long FDIR_R::getfaultst()
{
	assert(faultzone >= 0);

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE_CB *fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_CB *cb = dsbob->cb_tb->GetTable();

	int cbptr = (fdir_zone + faultzone)->cbptr;
	int icb = (fdir_zone_cb + cbptr)->icb;

	return (cb + icb)->stid;
}

long FDIR_R::getfaultcb()
{
	FAULT_ITEM head;
	list<FAULT_ITEM> tail;
	PSBOB::CB cb = {0};

	get_fault_area(head, tail);

	if(strcmp(head.table.c_str(), "CB") == 0)
	{
		if (oodbread_rk(&cb, &head.eq_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) == OO_FAIL)
		{
			printf("%s(%d):读取PSBOB CB表关键字为%ld的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, head.eq_id, _oodb_errno);
		}
		else
		{
			if(HasMask(g_db_psbob->GetTB("breaker"), "brk_type", cb.brk_type , MENU_BRK_TYPE_JX))
			{
			}
			else
			{
				if(HasMask(g_db_psbob->GetTB("breaker"), "brk_type", cb.brk_type , MENU_BRK_TYPE_LW))
				{
				}
				else
				{
					if(HasMask(g_db_psbob->GetTB("breaker"), "brk_prop", cb.brk_prop , MENU_BRK_PROP_AUTO))
					{
						id_to_pid(head.eq_id);

						return head.eq_id;
					}
				}
			}
		}
	}

	for(list<FAULT_ITEM>::iterator it_tail = tail.begin(); it_tail != tail.end(); it_tail++)
	{
		if(strcmp(it_tail->table.c_str(), "CB") == 0)
		{
			if (oodbread_rk(&cb, &it_tail->eq_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) == OO_FAIL)
			{
				printf("%s(%d):读取PSBOB CB表关键字为%ld的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, it_tail->eq_id, _oodb_errno);
			}
			else
			{
				if(HasMask(g_db_psbob->GetTB("breaker"), "brk_type", cb.brk_type , MENU_BRK_TYPE_JX))
				{
					continue;
				}
				if(HasMask(g_db_psbob->GetTB("breaker"), "brk_type", cb.brk_type , MENU_BRK_TYPE_LW))
				{
					continue;
				}
				if(HasMask(g_db_psbob->GetTB("breaker"), "brk_prop", cb.brk_prop , MENU_BRK_PROP_AUTO))
				{
					id_to_pid(it_tail->eq_id);

					return it_tail->eq_id;
				}
			}
		}
	}

	id_to_pid(head.eq_id);

	return head.eq_id;
}

long FDIR_R::getfaultsec()
{
	assert(faultzone >= 0);

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();

	if ((fdir_zone + faultzone)->type == _FDIR_ZN)
	{
		CLS_ZN *zone = dsbob->zn_tb->Find(CLS_ZN(fdir_zone_tb->Data[faultzone].zn_id));

		if (zone->r_sec >= 0)
		{
			CLS_R_ZN_SEC *r_zn_sec = dsbob->r_zn_sec_tb->GetTable();

			return (r_zn_sec + zone->r_sec)->sec_id;
		}
		else
		{
			return 0;
		}
	}
	else
		return 0;
}

//获取转供馈线的开关信息
int FDIR_R::getfhzglstcb(int izone, list<CLS_CB> &lst_cb)
{
	if (izone == -1)
	{
		TRACE("查找电源树的区段为-1\r\n");
		return FDIR_R_FAIL;
	}

	int cnt_cb = dsbob->cb_tb->GetCount();
	int cnt_fdir_zone = fdir_zone_tb->GetCount();
	bool *bvisited = new bool[cnt_cb];
	bool *bvisited_zn = new bool[cnt_fdir_zone];
	memset(bvisited, false, sizeof(bool) * cnt_cb);
	memset(bvisited_zn, false, sizeof(bool) * cnt_fdir_zone);
	m_p_jxcb_Node = NULL;

	Tree<FDIR_TREE_NODE_DATA> ptree;
	Tree<FDIR_TREE_NODE_DATA> tree1, tree2;
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_nodes;
	list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lst;

	int cbptr = fdir_zone_tb->GetDataByPos(izone)->cbptr;
	int cbnum = fdir_zone_tb->GetDataByPos(izone)->cbnum;
	int icb = -1;
	int i = 0;
	CLS_CB *key_cb = NULL;
	for (i = 0; i < cbnum; i++)
	{
		icb = fdir_zone_cb_tb->GetDataByPos(cbptr + i)->icb;
		key_cb = dsbob->cb_tb->GetDataByLog(icb);
		if (!(key_cb->q & CB_OPEN))
		{
			break;
		}
	}

	if (i == cbnum)
	{
		TRACE("负荷转供区段中没有符合建立树条件的cb，查找负荷转供开关失败\r\n");
		delete bvisited;
		delete bvisited_zn;
		return FDIR_R_FAIL;
	}
	int ind = fdir_cb_tb->GetDataByPos(icb)->ind;
	int iznd = fdir_cb_tb->GetDataByPos(icb)->iznd;
	bool b_in_ind = false;

	bvisited[icb] = true;

	if (key_cb->q & CB_FDHCB)
	{
		int tmp_zone = -1;

		if (fdir_zone_tb->GetDataByPos(ind)->type == _FDIR_BUS)
		{
			tmp_zone = iznd;
		}
		else if (fdir_zone_tb->GetDataByPos(iznd)->type == _FDIR_BUS)
		{
			tmp_zone = ind;
		}
		else
		{
			TRACE("找不到进线开关！");
			delete[] bvisited;
			delete[] bvisited_zn;
			return FDIR_R_FAIL;
		}

		BuildTree(bvisited, bvisited_zn, tmp_zone, ptree.TreeAddLeaf(NULL, key_cb), &ptree);
	}
	else
	{
		BuildTree(bvisited, bvisited_zn, ind, tree1.TreeAddLeaf(NULL, key_cb), &tree1);
		if (m_p_jxcb_Node != NULL)
		{
			b_in_ind = true;
		}
		TRACE("=============================================================\r\n");
		BuildTree(bvisited, bvisited_zn, iznd, tree2.TreeAddLeaf(NULL, key_cb), &tree2);

		if (m_p_jxcb_Node == NULL)
		{
			TRACE("未找到进线开关！\r\n");
			delete[] bvisited;
			delete[] bvisited_zn;
			return FDIR_R_FAIL;
		}

		Tree<FDIR_TREE_NODE_DATA> *pMainTree = NULL, *pSubTree = NULL;
		if (b_in_ind)
		{
			pMainTree = &tree1;
			pSubTree = &tree2;
		}
		else
		{
			pMainTree = &tree2;
			pSubTree = &tree1;
		}

		pMainTree->ChangeRoot(m_p_jxcb_Node, ptree, ptree.root);
		ptree.BreadthFirstErgodic(ptree.root, lst_nodes);

		for (it_lst = lst_nodes.begin(); it_lst != lst_nodes.end(); it_lst++)
		{
			if ((*it_lst)->data.m_pData == key_cb)
			{
				TreeNode<FDIR_TREE_NODE_DATA> *p = ptree.GetParent(*it_lst);
				ptree.TreeAddSubTree(p, pSubTree);
				ptree.TreeRemoveSubTree(*it_lst);
				break;
			}
		}
	}

	delete[] bvisited;
	delete[] bvisited_zn;

	lst_nodes.clear();
	ptree.BreadthFirstErgodic(ptree.root, lst_nodes);

	lst_cb.clear();

	for (it_lst = lst_nodes.begin(); it_lst != lst_nodes.end(); it_lst++)
	{
		CLS_CB cb_info;
		PSBOB::CB point = { 0 };

		if ((*it_lst)->data.m_type != _CB)
		{
			continue;
		}

		cb_info = *((CLS_CB*)(*it_lst)->data.m_pData);
		cb_info.q = 0;

		if (oodbread_rk(&point, &(((CLS_CB*)(*it_lst)->data.m_pData)->pid), const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) == OO_FAIL)
		{
			TRACE("读取PSBOB CB表关键字为%ld的记录失败，错误号：%d\r\n", ((CLS_CB*)(*it_lst)->data.m_pData)->pid, _oodb_errno);
			cb_info.q |= CB_UNKNOWN;
		}
		if (point.point <= 0)
		{
			cb_info.q |= CB_OPEN;
		}
		else
		{
			cb_info.q &= ~CB_OPEN;
		}
		if (((CLS_CB*)(*it_lst)->data.m_pData)->q & CB_FDHCB)
		{
			cb_info.q |= CB_FDHCB;

			//进线开关的带电状态
			if (point.tpcolor <= 0)
			{
				cb_info.topflags |= CB_DEAD;
			}
			else
			{
				cb_info.topflags |= CB_LIVE;
			}
		}
		if (((CLS_CB*)(*it_lst)->data.m_pData)->q & CB_LWCB)
		{
			cb_info.q |= CB_LWCB;
		}

		lst_cb.push_back(cb_info);
	}

	return FDIR_R_SUCCEED;
}

int FDIR_R::setisolate(list<FDIR_ISO_CB> &lst_cb)
{
	fdir_isolate_tb->Empty();
	list<FDIR_ISO_CB>::iterator it_step;
	CLS_FDIR_ISOLATE* fdir_isolate = new CLS_FDIR_ISOLATE;

	for (it_step = lst_cb.begin(); it_step != lst_cb.end(); it_step++)
	{
		bzero(fdir_isolate, sizeof(CLS_FDIR_ISOLATE));

		long id = it_step->cb;

		pid_to_id(id);

		fdir_isolate->id = fdir_isolate_tb->GetCount(); // 分闸开关在隔离表的记录id，设为隔离表当前个数
		CLS_CB *cb = dsbob->cb_tb->Find(CLS_CB(id));

		sprintf(fdir_isolate->descr, "%ld 分闸", id);

		TRACE("%s\r\n", fdir_isolate->descr);

		fdir_isolate->icb = dsbob->cb_tb->FindLog(cb->id); // 开关对应的开关物理号

		if (it_step->q & FDIR_ISOLATE_UP)
			fdir_isolate->q |= FDIR_ISOLATE_UP;	//此开关为故障上游开关，不需要对些开关做负荷转供
		else
			fdir_isolate->q &= ~FDIR_ISOLATE_UP;

		fdir_isolate->q |= FDIR_ISOLATE_OPEN;
		fdir_isolate->cb= cb->pid;
		strcpy(fdir_isolate->cb_desc, cb->descr);
		TRACE("把隔离开关：%s 恢复到隔离列表中\r\n", fdir_isolate->descr);

		if (fdir_isolate_tb->Insert(*fdir_isolate) == -1) // 插入隔离操作表
		{
			delete fdir_isolate;
			return -1;
		}
	}
	delete fdir_isolate;
	return 1;
}
TTable<CLS_FDIR_ISOLATE>* FDIR_R::getisolate_cbid() const
{
	//D5000不需要转换
	/*int lv_isolate = fdir_isolate_tb->GetCount();

	for (int i = 0; i < lv_isolate; i++)
	{
		PSBOB::POINT point = { 0 };

		if (oodbread_rk(&point, const_cast<char*>(fdir_isolate_tb->Data[i].cb), const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), sizeof(PSBOB::POINT)) > 0)
		{
			if (point.groupid[0] == 0)
			{
				strcpy(fdir_isolate_tb->Data[i].cb, point.classid);
			}
			else
			{
				strcpy(fdir_isolate_tb->Data[i].cb, point.groupid);
			}
		}
		else
		{
			printf("读取开关表%s数据失败，错误号：%d，line：%d\r\n", fdir_isolate_tb->Data[i].cb, _oodb_errno, __LINE__);
		}
	}
*/
	return fdir_isolate_tb;
}

TTable<CLS_FDIR_ISOLATE>* FDIR_R::getisolate() const
{
//	int lv_isolate = fdir_isolate_tb->GetCount();
//
//	for (int i = 0; i < lv_isolate; i++)
//	{
//		PSBOB::CB psbob_cb = { 0 };
//
//		if (oodbread_rk(&psbob_cb, const_cast<char*>(fdir_isolate_tb->Data[i].cb), const_cast<TB_DESCR*>(g_db_psbob->GetTB("cb")), sizeof(PSBOB::CB)) > 0)
//		{
//			strcpy(fdir_isolate_tb->Data[i].cb, psbob_cb.pointid);
//		}
//		else
//		{
//			printf("读取开关表%s数据失败，错误号：%d，line：%d\r\n", fdir_isolate_tb->Data[i].cb, _oodb_errno, __LINE__);
//		}
//	}

	return fdir_isolate_tb;
}

TTable<CLS_FDIR_ISOLATE>* FDIR_R::getisolate2() const
{
//	int lv_isolate = fdir_isolate_tb2->GetCount();
//
//	for (int i = 0; i < lv_isolate; i++)
//	{
//		PSBOB::CB psbob_cb = { 0 };
//
//		if (oodbread_rk(&psbob_cb, const_cast<char*>(fdir_isolate_tb2->Data[i].cb), const_cast<TB_DESCR*>(g_db_psbob->GetTB("cb")), sizeof(PSBOB::CB)) > 0)
//		{
//			strcpy(fdir_isolate_tb2->Data[i].cb, psbob_cb.pointid);
//		}
//		else
//		{
//			printf("读取开关表%s数据失败，错误号：%d，line：%d\r\n", fdir_isolate_tb2->Data[i].cb, _oodb_errno, __LINE__);
//		}
//	}

	return fdir_isolate_tb2;
}

MAP_FDIR_RESTORE FDIR_R::getrestore() const
{
	return fdir_restore_tb;
}

MAP_FDIR_RESTORE FDIR_R::getrestore2() const
{
	return fdir_restore_tb2;
}

int FDIR_R::get_zone_cb(const int izone, string &lst_cb) const
{
	CLS_FDIR_ZONE_CB *fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_CB *cb = dsbob->cb_tb->GetTable();

	lst_cb.clear();

	for (int i = 0; i < (fdir_zone + izone)->cbnum; i++)
	{
		char temp[60] = {0};

		sprintf(temp, "%ld;", (cb + (fdir_zone_cb + (fdir_zone + izone)->cbptr + i)->icb)->pid);

		lst_cb += temp;
	}

	return FDIR_R_SUCCEED;
}

int FDIR_R::get_zone_devs(const int izone, list<FAULT_ITEM> &devs)
{
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_ZONE_CB *fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_CB *cb = dsbob->cb_tb->GetTable();
	int i = 0, j = 0;

	devs.clear();

	if(izone < 0)
	{
		return FDIR_R_FAIL;
	}

	//获取区段的开关
	int cbptr = (fdir_zone + izone)->cbptr;
	int cbnum = (fdir_zone + izone)->cbnum;

	for (i = 0; i < cbnum; i++)
	{
		PSBOB::CB psbob_cb = { 0 };
		CLS_CB *cb = dsbob->cb_tb->GetDataByPos((fdir_zone_cb + cbptr + i)->icb);

		devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
	}

	list<long> lst_dev;
	list<long>::iterator it;

	//获取区段内的负荷
	get_zone_ld(izone, lst_dev);

	for (it = lst_dev.begin(); it != lst_dev.end(); it++)
	{
		CLS_LD *ld = dsbob->ld_tb->Find(CLS_LD(*it));

		devs.push_back(FAULT_ITEM(0, ld->pid, "LD", ld != NULL ? ld->descr : ""));
	}

	//获取区段内的线段
	get_zone_sec(izone, lst_dev);

	for (it = lst_dev.begin(); it != lst_dev.end(); it++)
	{
		CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(*it));

		devs.push_back(FAULT_ITEM(0, sec->pid, "SEC", sec != NULL ? sec->descr : ""));

	}

	//查找母线及刀闸
	lst_dev.clear();

	CLS_ZN* zn = dsbob->zn_tb->GetTable();
	for(j = 0; j < (zn+izone)->type; j++)
	{
		//printf("获取设备的区段名:%s结点是:%s\n",(zn+izone)->id, (zn+izone)->nds[j].nd_id);
		map<long, list<long> >::iterator it;
		//查找母线设备
		it = g_map_nd_bus.find((zn+izone)->nds[j].nd_id);
		if(it != g_map_nd_bus.end())
		{
			for(list<long>::iterator vi = it->second.begin(); vi != it->second.end(); vi++)
			{
				PSBOB::BUS psbob_bus = {0};

				if(oodbread_rk(&psbob_bus, &(*vi), const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(psbob_bus)) <= 0)
				{
					TRACE("读取ID为%ld的母线记录失败！", *vi);
				}
				else
					devs.push_back(FAULT_ITEM(0, psbob_bus.id, "BUS", psbob_bus.describe));
			}
		}
		//查找刀闸设备
		it = g_map_nd_switch.find((zn+izone)->nds[j].nd_id);
		if (it != g_map_nd_switch.end())
		{
			for (list<long>::iterator vi = it->second.begin(); vi != it->second.end(); vi++)
			{
				lst_dev.push_back(*vi);
			}
		}
	}

	lst_dev.sort();
	lst_dev.unique();

	for (it = lst_dev.begin(); it != lst_dev.end(); it++)
	{
		PSBOB::SWITCH psbob_switch = { 0 };

		if (oodbread_rk(&psbob_switch, &(*it), const_cast<TB_DESCR*>(g_db_psbob->GetTB("disconnector")), sizeof(psbob_switch)) <= 0)
		{
			TRACE("读取ID为%ld的刀闸记录失败！", *it);
		}
		else
			devs.push_back(FAULT_ITEM(0, psbob_switch.id, "SWITCH", psbob_switch.describe));
	}

	return FDIR_R_SUCCEED;
}

//根据区段边界开关遥信获取对应的区段号
int FDIR_R::GetZoneByCB(list<long> lst_cb)
{
	if (lst_cb.size() <= 0)
		return -1;

	list<long> tmp_lst_cb;
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE_CB *fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	int pos = dsbob->cb_tb->FindPos(CLS_CB(lst_cb.front()));

	if (pos < 0)
		return -1;

	int izone = (fdir_cb + pos)->ind;
	int zzone = (fdir_cb + pos)->iznd;

	if (izone >= 0)
	{
		int icb = (fdir_zone + izone)->cbptr;
		int cbnum = (fdir_zone + izone)->cbnum;

		for (int i = 0; i < cbnum; i++)
		{
			tmp_lst_cb.push_back((fdir_cb + (fdir_zone_cb + icb + i)->icb)->cb_id);
		}

		if (tmp_lst_cb.size() == lst_cb.size())
		{
			lst_cb.sort();
			tmp_lst_cb.sort();

			list<long>::iterator it1, it2;

			for (it1 = lst_cb.begin(), it2 = tmp_lst_cb.begin(); it1 != lst_cb.end(); it1++, it2++)
			{
				if (*it1 != *it2)
					break;
			}

			if (it1 == lst_cb.end())
			{
				return izone;
			}
		}
	}

	tmp_lst_cb.clear();

	if (zzone >= 0)
	{
		int icb = (fdir_zone + zzone)->cbptr;
		int cbnum = (fdir_zone + zzone)->cbnum;

		for (int i = 0; i < cbnum; i++)
		{
			tmp_lst_cb.push_back((fdir_cb + (fdir_zone_cb + icb + i)->icb)->cb_id);
		}

		if (tmp_lst_cb.size() == lst_cb.size())
		{
			lst_cb.sort();
			tmp_lst_cb.sort();

			list<long>::iterator it1, it2;

			for (it1 = lst_cb.begin(), it2 = tmp_lst_cb.begin(); it1 != lst_cb.end(); it1++, it2++)
			{
				if (*it1 != *it2)
					break;
			}

			if (it1 == lst_cb.end())
			{
				return zzone;
			}
		}
	}

	return -1;
}
void FDIR_R::GetDVByZone(int izone, PSBOB::DV &dv)
{
	list<FAULT_ITEM> lst_dev;
	get_zone_devs(izone, lst_dev);
	PSBOB::CB psbob_cb = { 0 };
	for (list<FAULT_ITEM>::iterator dev_it = lst_dev.begin(); dev_it != lst_dev.end(); dev_it++)
	{
		//id_to_pid(dev_it->id);

		if (dev_it->table == "CB")
		{
			if (oodbread_rk(&psbob_cb, const_cast<long*>(&dev_it->eq_id), const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) != OO_FAIL)
			{
				if ((HasMask(g_db_psbob->GetTB("breaker"), "brk_type", psbob_cb.brk_type , MENU_BRK_TYPE_JX)) || (HasMask(g_db_psbob->GetTB("breaker"), "brk_type", psbob_cb.brk_type , MENU_BRK_TYPE_LW)))
				{
					cout << "此开关为进线开关或是联络开关" << dev_it->eq_id << endl;
					continue;
				}
				else
				{
					if (GetDv(psbob_cb.id, "cb", dv) > 0)
					{
						break;
					}
					else
					{
						continue;
					}
				}
			}
			else
			{
				cout << "读取此开关的cb表失败" << dev_it->eq_id << endl;
				continue;
			}
		}

		if (dev_it->table == "SWITCH")
		{
			continue;
		}

		if (GetDv(dev_it->eq_id, dev_it->table.c_str(), dv) > 0)
		{
			break;
		}
		else
		{
			continue;
		}

	}
}
int FDIR_R::get_zone_ld(const int izone, list<long> &lst_ld)
{
	lst_ld.clear();

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_ZN *zn = dsbob->zn_tb->GetTable();
	CLS_LD *ld = dsbob->ld_tb->GetTable();

	ChildPtr *ptr1, *ptr2;

	ptr1 = dsbob->r_zn_ld_tb->FindChildPtr((fdir_zone + izone)->zn_id);
	ptr2 = ptr1;

	while (ptr1 != NULL)
	{
		lst_ld.push_back((ld + ptr1->Ptr)->id);

		ptr1 = ptr1->Next;
	}

	FreeChildPtr(ptr2);

	return FDIR_R_SUCCEED;
}

int FDIR_R::get_zone_sec(const int izone, list<long> &lst_sec)
{
	lst_sec.clear();

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_ZN *zn = dsbob->zn_tb->GetTable();
	CLS_SEC *sec = dsbob->sec_tb->GetTable();

	ChildPtr *ptr1, *ptr2;

	ptr1 = dsbob->r_zn_sec_tb->FindChildPtr((fdir_zone + izone)->zn_id);
	ptr2 = ptr1;

	while (ptr1 != NULL)
	{
		lst_sec.push_back((sec + ptr1->Ptr)->id);

		ptr1 = ptr1->Next;
	}

	FreeChildPtr(ptr2);

	return FDIR_R_SUCCEED;
}

void FDIR_R::FindFaultTail(int izone, list<FAULT_ITEM> &tail)
{
	assert(izone >= 0);

	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	CLS_R_ZN_LD *r_zn_ld = dsbob->r_zn_ld_tb->GetTable();
	list<int> lst_tmp_iso_cb;

	lst_tmp_iso_cb.clear();

	int cbptr, cbnum;
	int icb;
	int i;

	cbptr = (fdir_zone + izone)->cbptr;
	cbnum = (fdir_zone + izone)->cbnum;

	for (i = 0; i < cbnum; i++)
	{
		icb = (fdir_zone_cb + cbptr + i)->icb;

		if (icb != (fdir_zone + izone)->icb)
		{
			lst_tmp_iso_cb.push_back(icb);

		}
	}

	list<long> lst_ld;

	get_zone_ld(izone, lst_ld);

	for (list<long>::iterator it_lst_ld = lst_ld.begin(); it_lst_ld != lst_ld.end(); it_lst_ld++)
	{
		CLS_LD *ld = dsbob->ld_tb->Find(CLS_LD(*it_lst_ld));
		tail.push_back(FAULT_ITEM(0, *it_lst_ld, "LD", ld != NULL ? ld->descr : ""));
	}

	for (list<int>::iterator it = lst_tmp_iso_cb.begin(); it != lst_tmp_iso_cb.end(); it++)
	{
		PSBOB::CB psbob_cb = { 0 };
		CLS_CB *tmp_cb = dsbob->cb_tb->Find(CLS_CB((fdir_cb + *it)->cb_id));

		if (tmp_cb == NULL)
			continue;

		if (oodbread_rk(&psbob_cb, &tmp_cb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) == OO_FAIL)
		{
			printf("%s(%d):读取ID为%ld的CB记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, tmp_cb->pid, _oodb_errno);
			continue;
		}
		else
		{
			//开关合闸
			if (psbob_cb.point > 0)
			{
				//开关可以跳闸但没有跳
				if (HasMask(g_db_psbob->GetTB("breaker"), "brk_prop", psbob_cb.brk_prop , MENU_BRK_PROP_AUTO))
				{
					tail.push_back(FAULT_ITEM(0, psbob_cb.id, "CB", psbob_cb.describe));
				}
				//开关不可以跳闸
				else
				{
					bool b_has_ppft = false; //该开关上是否有故障指示器

					char *buf = NULL;
					int buf_size = 0;

					const TB_DESCR *tb_relaysig = g_db_psbob->GetTB("relaysig");

					if(tb_relaysig != NULL)
					{
						int ret = tb_relaysig->ptab->TableGet(&buf, buf_size);

						if (ret >= 0)
						{
							int cnt = buf_size / sizeof(PSBOB::PROTECT);

							PSBOB::PROTECT *relay_buf = (PSBOB::PROTECT *)buf;

							for (i = 0; i < cnt; i++)
							{
								if (relay_buf->rely_brk1 == psbob_cb.id || relay_buf->rely_brk2 == psbob_cb.id || relay_buf->rely_brk3 == psbob_cb.id || relay_buf->rely_brk4 == psbob_cb.id
										|| relay_buf->rely_brk5 == psbob_cb.id || relay_buf->rely_brk6 == psbob_cb.id)
								{
									b_has_ppft = true;
									break;
								}

								relay_buf++;
							}

							BufFree(buf);
						}
						else
						{
							printf("%s(%d):读取relaysig表记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, _oodb_errno);
						}

						//该开关有没有故障指示器,故障可能跨过该开关
						if (!b_has_ppft)
						{
							TRACE("开关%ld[%s]上没有事故总，故障可能跨过该开关，往下游继续搜索\r\n", tmp_cb->id, tmp_cb->descr);

							FDIR_CB_INFO info = { 0 };

							info.cb_id = tmp_cb->id;
							info.pt_id = tmp_cb->pid;
							strcpy(info.cb_descr, tmp_cb->descr);
//							info.pt_control = tmp_point.control;
//							info.pt_quality = tmp_point.quality;
//							info.pt_timexc = tmp_point.timexc;
//							info.cb_q = psbob_cb.q;
							info.cb_status = psbob_cb.status;
							info.reason = NO_AUTO;
							lst_cb_info.push_back(info);

							int idownzone = FindDownZone(*it);

							if (idownzone >= 0)
							{
								FindFaultTail(idownzone, tail);
							}
						}
						else
						{
							tail.push_back(FAULT_ITEM(0, psbob_cb.id, "CB", psbob_cb.describe));
						}
					}
				}
			}
			//开关分闸
			else
			{
				tail.push_back(FAULT_ITEM(0, psbob_cb.id, "CB", psbob_cb.describe));
			}
		}
	}
}

void FDIR_R::set_faultcb_edge(const char *faultid)
{
	list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_tail;

	if (fault_head->data.m_type == _CB)
	{
		sprintf(sql, "update FDIR_FAULT_DEV set edge=1, flag=1 where id = \'%s\' and dev_id = \'%ld\'", faultid, ((CLS_CB *)fault_head->data.m_pData)->pid);
		ExecSQL(sql);
	}
	else if (fault_head->data.m_type == _BUS)
	{
		sprintf(sql, "update FDIR_FAULT_DEV set edge=1, flag=1 where id = \'%s\' and dev_id = \'%ld\'", faultid, ((CLS_BUS *)fault_head->data.m_pData)->pid);
		ExecSQL(sql);
	}
	else if (fault_head->data.m_type == _FI)
	{
		sprintf(sql, "update FDIR_FAULT_DEV set edge=1, flag=1 where id = \'%s\' and dev_id = \'%ld\'", faultid, ((CLS_FI *)fault_head->data.m_pData)->pid);
		ExecSQL(sql);
	}

	for (it_tail = fault_tail.begin(); it_tail != fault_tail.end(); it_tail++)
	{
		if ((*it_tail)->data.m_type == _CB)
		{
			sprintf(sql, "update FDIR_FAULT_DEV set edge=1, flag=2 where id = \'%s\' and dev_id = \'%ld\'", faultid, ((CLS_CB *)(*it_tail)->data.m_pData)->pid);
			ExecSQL(sql);
		}
		else if ((*it_tail)->data.m_type == _FI)
		{
			sprintf(sql, "update FDIR_FAULT_DEV set edge=1, flag=2 where id = \'%s\' and dev_id = \'%ld\'", faultid, ((CLS_FI *)(*it_tail)->data.m_pData)->pid);
			ExecSQL(sql);
		}
	}

}

void FDIR_R::get_fault_devs(list<FAULT_ITEM> &up_devs, list<FAULT_ITEM> &fault_devs, list<FAULT_ITEM> &down_devs)
{
//	for (list <FAULT_ITEM>::iterator it = lst_fault_devs.begin(); it != lst_fault_devs.end(); it++)
//	{
//		lst_devs.push_back(*it);
//	}
	up_devs.clear();
	up_devs.merge(lst_up_devs);

	fault_devs.clear();
	fault_devs.merge(lst_fault_devs);

	down_devs.clear();
	down_devs.merge(lst_down_devs);
}

void FDIR_R::get_fault_area(FAULT_ITEM &head, list<FAULT_ITEM> &tail)
{
	if (fault_head == NULL)
	{
		return;
	}

	if (fault_head->data.m_type == _CB)
	{
		head.eq_id = ((CLS_CB *) fault_head->data.m_pData)->pid;
		head.table = "CB";
		head.eq_descr = ((CLS_CB *) fault_head->data.m_pData)->descr;
	}
	else if (fault_head->data.m_type == _FI)
	{
		CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(((CLS_FI *) fault_head->data.m_pData)->sec));
		if (sec != NULL)
		{
			head.eq_id = sec->pid;
			head.table = "FI";
			head.eq_descr = ((CLS_FI *) fault_head->data.m_pData)->descr;
			head.id = ((CLS_FI *) fault_head->data.m_pData)->pid;
		}
	}
	else if (fault_head->data.m_type == _BUS)
	{
		head.eq_id = ((CLS_BUS *) fault_head->data.m_pData)->pid;
		head.table = "BUS";
		head.eq_descr = ((CLS_BUS *) fault_head->data.m_pData)->descr;
	}
	else if (fault_head->data.m_type == _LD)
	{
		head.eq_id = ((CLS_LD *) fault_head->data.m_pData)->pid;
		head.table = "LD";
		head.eq_descr = ((CLS_LD *) fault_head->data.m_pData)->descr;
	}
	if (fault_head->data.m_type != _BUS)
	{
		for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_tail = fault_tail.begin(); it_tail != fault_tail.end(); it_tail++)
		{
			FAULT_ITEM tp_item;
			if ((*it_tail)->data.m_pData == NULL)
			{
				continue;
			}
			if ((*it_tail)->data.m_type == _CB)
			{
				tp_item.eq_id = ((CLS_CB *) (*it_tail)->data.m_pData)->pid;
				tp_item.table = "CB";
				tp_item.eq_descr = ((CLS_CB *) (*it_tail)->data.m_pData)->descr;
			}
			else if ((*it_tail)->data.m_type == _FI)
			{
				CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(((CLS_FI *) (*it_tail)->data.m_pData)->sec));
				if (sec != NULL)
				{
					tp_item.eq_id = sec->pid;
					tp_item.table = "FI";
					tp_item.eq_descr = ((CLS_FI *) (*it_tail)->data.m_pData)->descr;
					tp_item.id = ((CLS_FI *) (*it_tail)->data.m_pData)->pid;
				}
			}
			else if ((*it_tail)->data.m_type == _LD)
			{
				tp_item.eq_id = ((CLS_LD *) (*it_tail)->data.m_pData)->pid;
				tp_item.table = "LD";
				tp_item.eq_descr = ((CLS_LD *) (*it_tail)->data.m_pData)->descr;
			}
			tail.push_back(tp_item);
		}
	}
	else
		tail.clear();
}

//判断某个开关是否为拒动开关
bool FDIR_R::IsCBFailed(const long point_name)
{
	TRACE("判断开关%ld是否为拒动开关！\r\n", point_name);

	if(std::find(lst_failed.begin(), lst_failed.end(), point_name) == lst_failed.end())
		return false;
	else
	{
		TRACE("开关%ld在拒动列表中！\r\n", point_name);
		return true;
	}
//	for (list<long>::iterator it = lst_failed.begin(); it != lst_failed.end(); it++)
//	{
//		if (*it == point_name)
//		{
//			return true;
//		}
//	}
//
//	return false;
}

int FDIR_R::FormFdirDB()
{
//	if(BuildDms() == FDIR_R_FAIL)
//	{
//		printf("failed in build dms (fdir_r)!\n");
//		return FDIR_R_FAIL;
//	}
	if (BuildCb() == FDIR_R_FAIL)
	{
		printf("failed in build cb (fdir_r)!\n");
		return FDIR_R_FAIL;
	}
	if (BuildZone() == FDIR_R_FAIL)
	{
		printf("failed in build zone (fdir_r)!\n");
		return FDIR_R_FAIL;
	}

//	WriteFile();
	return FDIR_R_SUCCEED;
}

int FDIR_R::BuildCb()
{
	int i;
	CLS_CB* cb;
	int lv_cb;
	CLS_FDIR_CB* fdir_cb = new CLS_FDIR_CB;

	fdir_cb_tb->Empty();
	cb = dsbob->cb_tb->GetTable();
	lv_cb = dsbob->cb_tb->GetCount();
	for (i = 0; i < lv_cb; i++)
	{
		fdir_cb->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_CB")), i + 1);
		strcpy(fdir_cb->descr, (cb + i)->descr);
		fdir_cb->ind = -1;
		fdir_cb->iznd = -1;
		fdir_cb->q = 0;
		fdir_cb->cb_id = (cb + i)->id;

		if ((cb + i)->q & CB_BREAKER)
		{
			fdir_cb->q |= FDIR_CB_BREAKER;
		}
		if ((cb + i)->q & CB_SWITCH)
		{
			fdir_cb->q |= FDIR_CB_SWITCH;
		}
		if ((cb + i)->q & CB_AUTO)
		{
			fdir_cb->q |= FDIR_CB_AUTO;
		}
		if ((cb + i)->q & CB_FDHCB)
		{
			fdir_cb->q |= FDIR_CB_FDHCB;
		}
		if ((cb + i)->q & CB_FDSCB)
		{
			fdir_cb->q |= FDIR_CB_FDSCB;
		}
		if ((cb + i)->q & CB_FDCCB)
		{
			fdir_cb->q |= FDIR_CB_FDCCB;
		}
		if ((cb + i)->q & CB_FDLCB)
		{
			fdir_cb->q |= FDIR_CB_FDLCB;
		}
		if (fdir_cb_tb->Insert(*fdir_cb) == -1)
		{
			delete fdir_cb;
			return FDIR_R_FAIL;
		}
	}
	delete fdir_cb;

	return FDIR_R_SUCCEED;
}

int FDIR_R::BuildBusZone(bool *bvisited, const long nd_id, int &k)
{
	assert(bvisited != NULL);
	assert(nd_id > 0);

	int ind = dsbob->nd_tb->FindPos(CLS_ND(nd_id));
	CLS_ND *pnd = dsbob->nd_tb->GetDataByPos(ind);

	if (ind < 0)
		return FDIR_R_FAIL;
	if (bvisited[ind])
		return FDIR_R_SUCCEED;

	bvisited[ind] = true;

	int cnt = GetCbCountToNd(ind, _FDIR_BUS);
	int icb = pnd->r_cb;

	k += cnt;

	if (Insert_Zone_Cb(icb, cnt, _FDIR_BUS) == -1)
	{
		TRACE("Insert_Zone_Cb失败, nd_id=%ld\r\n", nd_id);
		return FDIR_R_FAIL;
	}

	if (pnd->r_sec >= 0)
	{
		ChildPtr *ptr1 = dsbob->r_nd_sec_tb->FindChildPtr(pnd->r_sec);
		ChildPtr *ptr2 = ptr1;

		while (ptr1 != NULL)
		{
			CLS_SEC *sec = dsbob->sec_tb->GetDataByPos(ptr1->Ptr);

			if (sec->nd ==  nd_id)
			{
				if (BuildBusZone(bvisited, sec->znd, k) == FDIR_R_FAIL)
				{
					return FDIR_R_FAIL;
				}
			}
			else
			{
				if (BuildBusZone(bvisited, sec->nd, k) == FDIR_R_FAIL)
				{
					return FDIR_R_FAIL;
				}
			}

			ptr1 = ptr1->Next;
		}

		FreeChildPtr(ptr2);
	}

	if (pnd->r_switch >= 0)
	{
		ChildPtr *ptr1 = dsbob->r_nd_switch_tb->FindChildPtr(pnd->r_switch);
		ChildPtr *ptr2 = ptr1;

		while (ptr1 != NULL)
		{
			CLS_SWITCH *swh = dsbob->switch_tb->GetDataByPos(ptr1->Ptr);

			if(swh->q & CB_OPEN)
			{
			}
			else
			{
				if (swh->nd == nd_id)
				{
					if (BuildBusZone(bvisited, swh->znd, k) == FDIR_R_FAIL)
					{
						return FDIR_R_FAIL;
					}
				}
				else
				{
					if (BuildBusZone(bvisited, swh->nd, k) == FDIR_R_FAIL)
					{
						return FDIR_R_FAIL;
					}
				}
			}
			ptr1 = ptr1->Next;
		}

		FreeChildPtr(ptr2);
	}

	return FDIR_R_SUCCEED;
}

int FDIR_R::BuildZone()
{
	int i, j, k, k1, k2, k3, k4, k5, index;
	int ind1, ind2, ind3, ind4, ind5, icb, type;

	//get nd and r_nd_cb from dsbob
	int lv_nd = dsbob->nd_tb->GetCount();
	CLS_ND* nd = dsbob->nd_tb->GetTable();
	int lv_r_nd_cb = dsbob->r_nd_cb_tb->GetCount();
	CLS_R_ND_CB* r_nd_cb = dsbob->r_nd_cb_tb->GetTable();

	CLS_FDIR_ZONE* fdir_zone = new CLS_FDIR_ZONE;
	CLS_FDIR_ZONE_CB* fdir_zone_cb = NULL;//new CLS_FDIR_ZONE_CB;
	fdir_zone_tb->Empty();
	fdir_zone_cb_tb->Empty();

	//form zone from dms_zn  cuilizhong 根据zn（区段）生成zone（避免重复，称区间），类型是_FDIR_ZN
	int lv_zn = dsbob->zn_tb->GetCount();
	CLS_ZN* zn = dsbob->zn_tb->GetTable();

	for (i = 0; i < lv_zn; i++)
	{
		///////////////////////////////////////////////////
		///////////////荣义平添加于20111130为适应超过5个结节的ZN
#if ZN_ND_USE_ARRAY
		fdir_zone->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE")), fdir_zone_tb->GetCount() + 1);
		strcpy(fdir_zone->descr, (zn + i)->descr);
		fdir_zone->type = _FDIR_ZN;
		fdir_zone->sub = i;
		fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
		fdir_zone->zn_id = (zn + i)->id;

		fdir_zone->cbnum = 0;
		for(index = 0; index < (zn+i)->type; index++)
		{
			fdir_zone->cbnum += GetCbCountToNd((zn + i)->nds[index].ind, _FDIR_ZN);
		}

		if (fdir_zone_tb->Insert(*fdir_zone) == -1)
		{
			TRACE("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			delete fdir_zone;
			return FDIR_R_FAIL;
		}

		for (index = 0; index < (zn + i)->type; index++)
		{
			if (Insert_Zone_Cb((nd + (zn + i)->nds[index].ind)->r_cb, GetCbCountToNd((zn + i)->nds[index].ind, _FDIR_ZN), _FDIR_ZN) == -1)
			{
				TRACE("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				delete fdir_zone;
				return FDIR_R_FAIL;
			}
		}
#else
		type = (zn + i)->type;
		if (type == 1) // 单端点区段
		{
			fdir_zone->id = fdir_zone_tb->GetCount(); // 区段id从0开始排
			strcpy(fdir_zone->descr, (zn + i)->descr);
			fdir_zone->type = _FDIR_ZN;
			fdir_zone->sub = i; // zn表物理号
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount(); //指向fdir_zone_cb_tb关系表中相关联的首个记录位置
			ind1 = (zn + i)->ind; // 区段对应的节点
			k1 = GetCbCountToNd(ind1); //通过节点找到相连开关个数
			fdir_zone->cbnum = k1; //相连开关个数

			if (fdir_zone_tb->Insert(*fdir_zone) == -1) // 保存本区段至fdir_zone表中
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb; //nd对应的首个开关在关系表中的物理号
			if (Insert_Zone_Cb(icb, k1) == -1) // 将区段和关联开关插入关系表
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
		}
		else if (type == 2)
		{
			fdir_zone->id = fdir_zone_tb->GetCount();
			strcpy(fdir_zone->descr, (zn + i)->descr);
			fdir_zone->type = _FDIR_ZN;
			fdir_zone->sub = i;
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
			ind1 = (zn + i)->ind;
			ind2 = (zn + i)->iznd;
			k1 = GetCbCountToNd(ind1);
			k2 = GetCbCountToNd(ind2);
			k = k1 + k2;
			fdir_zone->cbnum = k;
			if (fdir_zone_tb->Insert(*fdir_zone) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb;
			if (Insert_Zone_Cb(icb, k1) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind2)->r_cb;
			if (Insert_Zone_Cb(icb, k2) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
		}
		else if (type == 3)
		{
			fdir_zone->id = fdir_zone_tb->GetCount();
			strcpy(fdir_zone->descr, (zn + i)->descr);
			fdir_zone->type = _FDIR_ZN;
			fdir_zone->sub = i;
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
			ind1 = (zn + i)->ind;
			ind2 = (zn + i)->iznd;
			ind3 = (zn + i)->ind3;
			k1 = GetCbCountToNd(ind1);
			k2 = GetCbCountToNd(ind2);
			k3 = GetCbCountToNd(ind3);
			k = k1 + k2 + k3;
			fdir_zone->cbnum = k;

			if (fdir_zone_tb->Insert(*fdir_zone) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb;
			if (Insert_Zone_Cb(icb, k1) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind2)->r_cb;
			if (Insert_Zone_Cb(icb, k2) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind3)->r_cb;
			if (Insert_Zone_Cb(icb, k3) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
		}
		else if (type == 4)
		{
			fdir_zone->id = fdir_zone_tb->GetCount();
			strcpy(fdir_zone->descr, (zn + i)->descr);
			fdir_zone->type = _FDIR_ZN;
			fdir_zone->sub = i;
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
			ind1 = (zn + i)->ind;
			ind2 = (zn + i)->iznd;
			ind3 = (zn + i)->ind3;
			ind4 = (zn + i)->ind4;
			k1 = GetCbCountToNd(ind1);
			k2 = GetCbCountToNd(ind2);
			k3 = GetCbCountToNd(ind3);
			k4 = GetCbCountToNd(ind4);
			k = k1 + k2 + k3 + k4;
			fdir_zone->cbnum = k;

			if (fdir_zone_tb->Insert(*fdir_zone) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb;
			if (Insert_Zone_Cb(icb, k1) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind2)->r_cb;
			if (Insert_Zone_Cb(icb, k2) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind3)->r_cb;
			if (Insert_Zone_Cb(icb, k3) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind4)->r_cb;
			if (Insert_Zone_Cb(icb, k4) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

		}
		else if (type == 5)
		{
			fdir_zone->id = fdir_zone_tb->GetCount();
			strcpy(fdir_zone->descr, (zn + i)->descr);
			fdir_zone->type = _FDIR_ZN;
			fdir_zone->sub = i;
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
			ind1 = (zn + i)->ind;
			ind2 = (zn + i)->iznd;
			ind3 = (zn + i)->ind3;
			ind4 = (zn + i)->ind4;
			ind5 = (zn + i)->ind5;
			k1 = GetCbCountToNd(ind1);
			k2 = GetCbCountToNd(ind2);
			k3 = GetCbCountToNd(ind3);
			k4 = GetCbCountToNd(ind4);
			k5 = GetCbCountToNd(ind5);
			k = k1 + k2 + k3 + k4 + k5;
			fdir_zone->cbnum = k;

			if (fdir_zone_tb->Insert(*fdir_zone) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb;
			if (Insert_Zone_Cb(icb, k1) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind2)->r_cb;
			if (Insert_Zone_Cb(icb, k2) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind3)->r_cb;
			if (Insert_Zone_Cb(icb, k3) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind4)->r_cb;
			if (Insert_Zone_Cb(icb, k4) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind5)->r_cb;
			if (Insert_Zone_Cb(icb, k5) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
		}
		else
		{
			printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}
#endif
	}
#if 0
	//form zone from xfmr   根据变压器生成zone ，类型是_FDIR_XFMR
	int lv_xfmr = dsbob->xfmr_tb->GetCount();
	CLS_XFMR* xfmr = dsbob->xfmr_tb->GetTable();
	for (i = 0; i < lv_xfmr; i++)
	{
		type = (xfmr + i)->type;
		if (type == 2)
		{
			fdir_zone->id = fdir_zone_tb->GetCount();
			strcpy(fdir_zone->descr, (xfmr + i)->descr);
			fdir_zone->type = _FDIR_XFMR;
			fdir_zone->sub = i; //变压器表物理号
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
			ind1 = dsbob->nd_tb->FindPos(CLS_ND((xfmr + i)->hnd));
			ind2 = dsbob->nd_tb->FindPos(CLS_ND((xfmr + i)->lnd));
			k1 = GetCbCountToNd(ind1, _FDIR_XFMR);
			k2 = GetCbCountToNd(ind2, _FDIR_XFMR);
			k = k1 + k2;
			fdir_zone->cbnum = k;
			strcpy(fdir_zone->zn_id, (xfmr + i)->id);

			if (fdir_zone_tb->Insert(*fdir_zone) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb;
			if (Insert_Zone_Cb(icb, k1, _FDIR_XFMR) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind2)->r_cb;
			if (Insert_Zone_Cb(icb, k2, _FDIR_XFMR) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
		}
		else if (type == 3)
		{
			fdir_zone->id = fdir_zone_tb->GetCount();
			strcpy(fdir_zone->descr, (xfmr + i)->descr);
			fdir_zone->type = _FDIR_XFMR;
			fdir_zone->sub = i;
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
			ind1 = dsbob->nd_tb->FindPos(CLS_ND((xfmr + i)->hnd));
			ind2 = dsbob->nd_tb->FindPos(CLS_ND((xfmr + i)->mnd));
			ind3 = dsbob->nd_tb->FindPos(CLS_ND((xfmr + i)->lnd));
			k1 = GetCbCountToNd(ind1, _FDIR_XFMR);
			k2 = GetCbCountToNd(ind2, _FDIR_XFMR);
			k3 = GetCbCountToNd(ind3, _FDIR_XFMR);
			k = k1 + k2 + k3;
			fdir_zone->cbnum = k;

			if (fdir_zone_tb->Insert(*fdir_zone) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb;
			if (Insert_Zone_Cb(icb, k2, _FDIR_XFMR) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind2)->r_cb;
			if (Insert_Zone_Cb(icb, k2, _FDIR_XFMR) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
			icb = (nd + ind3)->r_cb;
			if (Insert_Zone_Cb(icb, k3, _FDIR_XFMR) == -1)
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}
		}
		else
		{
			printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}
	}
#endif
	//form zone from dms_bus  根据母线生成zone ，类型是_FDIR_BUS
	int lv_bus = dsbob->bus_tb->GetCount();
	CLS_BUS* bus = dsbob->bus_tb->GetTable();
	CLS_ND* tmp_nd = NULL;

	bool *bvisited = new bool[lv_nd];

	for (i = 0; i < lv_bus; i++)
	{
		fdir_zone->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE")), fdir_zone_tb->GetCount() + 1);
		strcpy(fdir_zone->descr, (bus + i)->descr);
		fdir_zone->type = _FDIR_BUS;
		fdir_zone->sub = i; //母线表物理号
		fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
		k = 0;
		bzero(bvisited, lv_nd * sizeof(bool));

		if (BuildBusZone(bvisited, (bus + i)->nd, k) == FDIR_R_FAIL)
		{
			TRACE("BuildBusZone failed!i=%d\r\n", i);
			delete bvisited;
			delete fdir_zone;
			return FDIR_R_FAIL;
		}

		fdir_zone->cbnum = k;

		fdir_zone->zn_id = (bus + i)->id;

		if (fdir_zone_tb->Insert(*fdir_zone) == -1)
		{
			TRACE("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			delete bvisited;
			delete fdir_zone;
			return FDIR_R_FAIL;
		}

		//利用BUS和SO的对应关系，将BUS的ID转换成SO的ID
		KEY_STRU ks = { 0 };
		unsigned long so_id;
		CCommon::long_to_keyid((bus + i)->id, &ks);
		ks.table_no = g_db_dms->GetTB("DMS_SO")->table_id;
		CCommon::keyid_to_long(&ks, &so_id);

		int index_so = dsbob->so_tb->FindPos(CLS_SO((long)so_id));
		if (index_so >= 0)
		{
			fdir_zone->type = _FDIR_SO;
			fdir_zone->sub = index_so;
			fdir_zone->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE")), fdir_zone_tb->GetCount() + 1);

			if (fdir_zone_tb->Insert(*fdir_zone) == -1)
			{
				TRACE("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				delete bvisited;
				delete fdir_zone;
				return FDIR_R_FAIL;
			}
		}
	}

	delete[] bvisited;
#if 0					//前面形成母线区段时已经形成相应的电源区段
	//form zone from so
	int lv_so = dsbob->so_tb->GetCount();
	CLS_SO* so = dsbob->so_tb->GetTable();
	for (i = 0; i < lv_so; i++)
	{
		fdir_zone->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE")), fdir_zone_tb->GetCount() + 1);
		strcpy(fdir_zone->descr, (so + i)->descr);
		fdir_zone->type = _FDIR_SO;
		fdir_zone->sub = i;
		fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
		ind1 = (so + i)->ind;
		k1 = GetCbCountToNd(ind1, _FDIR_SO);
		fdir_zone->cbnum = k1;
		fdir_zone->zn_id= (so + i)->id;
		if (fdir_zone_tb->Insert(*fdir_zone) == -1)
		{
			TRACE("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}

		icb = (nd + ind1)->r_cb;
		if (Insert_Zone_Cb(icb, k1, _FDIR_SO) == -1)
		{
			TRACE("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}
	}
#endif
	delete fdir_zone;
	fdir_zone = NULL;
#if 0
	//form zone from ld
	int lv_ld = dsbob->ld_tb->GetCount();
	CLS_LD* ld = dsbob->ld_tb->GetTable();
	for (i = 0; i < lv_ld; i++)
	{
		fdir_zone->id = fdir_zone_tb->GetCount();
		strcpy(fdir_zone->descr, (ld + i)->descr);
		fdir_zone->type = _FDIR_LD;
		fdir_zone->sub = i;
		fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
		ind1 = (ld + i)->ind;
		k1 = GetCbCountToNd(ind1, _FDIR_LD);
		fdir_zone->cbnum = k1;
		strcpy(fdir_zone->zn_id, (ld + i)->id);
		if (fdir_zone_tb->Insert(*fdir_zone) == -1)
		{
			printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}

		icb = (nd + ind1)->r_cb;
		if (Insert_Zone_Cb(icb, k1, _FDIR_LD) == -1)
		{
			printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}
	}

	//form zone from cp
	int lv_cp = dsbob->cp_tb->GetCount();
	CLS_CP* cp = dsbob->cp_tb->GetTable();
	for (i = 0; i < lv_cp; i++)
	{
		fdir_zone->id = fdir_zone_tb->GetCount();
		strcpy(fdir_zone->descr, (cp + i)->descr);
		fdir_zone->type = _FDIR_CP;
		fdir_zone->sub = i;
		fdir_zone->cbptr = fdir_zone_cb_tb->GetCount();
		ind1 = (cp + i)->ind;
		k1 = GetCbCountToNd(ind1, _FDIR_CP);
		fdir_zone->cbnum = k1;
		strcpy(fdir_zone->zn_id, (cp + i)->id);
		if (fdir_zone_tb->Insert(*fdir_zone) == -1)
		{
			printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}

		icb = (nd + ind1)->r_cb;
		if (Insert_Zone_Cb(icb, k1, _FDIR_CP) == -1)
		{
			printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
			return FDIR_R_FAIL;
		}
	}
#endif
	//set izone and izzone to cb
	int count, cbptr, cbnum;

	int lv_fdir_zone = fdir_zone_tb->GetCount();
	fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	for (j = 0; j < lv_fdir_cb; j++) // 开关表循环
	{
		count = 0;
		for (i = 0; i < lv_fdir_zone; i++) //zone表循环
		{
			//_FDIR_BUS和_FDIR_SO对应，两个区段的信息信息一样
			if((fdir_zone + i)->type == _FDIR_BUS)
			{
				continue;
			}

			cbptr = (fdir_zone + i)->cbptr; // 关联表首记录号
			cbnum = (fdir_zone + i)->cbnum; // 关联开关个数
			for (k = 0; k < cbnum; k++)
			{
				if ((fdir_zone_cb + cbptr + k)->icb == j)
				{
					if (count == 0)
						(fdir_cb + j)->ind = i;
					if (count == 1)
					{
						(fdir_cb + j)->iznd = i;
						break;
					}
					count++;
				}
			}

			if (k < cbnum)
			{
				break;
			}
		}
	}
	return FDIR_R_SUCCEED;
}

int FDIR_R::SetFaultInfo()
{
	if (tp->tp_allnew() == TP_FAIL) // 在这重新做拓扑，有意义吗？
	{
		return FDIR_R_FAIL;
	}
//	else
//	{
//		if (!dsbob->WriteReDB(NULL, NULL, NULL))
//		{
//			cout << __FUNCTION__ << "fdir_r->WriteRtdb失败！" << endl;
//
//			return FDIR_FAIL_DIRECT;
//		}
//	}
	//set fdir_zone status
	CLS_BUS* bus = dsbob->bus_tb->GetTable();
	CLS_ZN* zn = dsbob->zn_tb->GetTable();
	CLS_LD* ld = dsbob->ld_tb->GetTable();
	CLS_XFMR* xfmr = dsbob->xfmr_tb->GetTable();
	CLS_CP* cp = dsbob->cp_tb->GetTable();

	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int sub,type;
	for(int i = 0;i < lv_fdir_zone; i++)
	{
		sub = (fdir_zone + i)->sub;
		type = (fdir_zone + i)->type;
		if(type == _FDIR_BUS)
		{
			if((bus + sub)->topflags & BUS_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_XFMR)
		{
			if((xfmr + sub)->topflags & XFMR_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_CP)
		{
			if((cp + sub)->topflags & CP_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_ZN)
		{
			if((zn + sub)->topflags & ZN_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_LD)
		{
			if((ld + sub)->topflags & LD_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_SO)
		{
			(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
		}
		else
		{
			return FDIR_R_FAIL;
		}
	}

	//for display
#if 0
	//write to rtap
	rtChar	tbName[255];
	strcpy(tbName, "dms_zn");
	if(WriteTable(dbConnection, dsbob->zn_tb->Data, dsbob->zn_tb->Count, sizeof(ZN), tbName) == rtFAILED)
	{
		return FDIR_R_FAIL;
	}
	//write to rtap
	strcpy(tbName, "dms_bus");
	if(WriteTable(dbConnection, dsbob->bus_tb->Data,dsbob->bus_tb->Count, sizeof(BUS), tbName) == rtFAILED)
	{
		return FDIR_R_FAIL;
	}
	//write to rtap
	strcpy(tbName, "dms_sec");
	if(WriteTable(dbConnection, dsbob->sec_tb->Data,dsbob->sec_tb->Count, sizeof(SEC), tbName) == rtFAILED)
	{
		return FDIR_R_FAIL;
	}
	//write to rtap
	strcpy(tbName, "fdir_r_zone");
	if(WriteTable(dbConnection, fdir_zone_tb->Data,fdir_zone_tb->Count, sizeof(FDIR_ZONE), tbName) == rtFAILED)
	{
		return FDIR_R_FAIL;
	}
#endif
	return FDIR_R_SUCCEED;
}

int FDIR_R::ReadDvFiBef(FDIR_TREE_NODE_DATA key, list<FAULT_ITEM> &lst_bef_fi)
{
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	PSBOB::DV dv = { 0 };
	switch (key.m_type)
	{
	case _LD:
		GetDv(((CLS_LD*) key.m_pData)->id, "ld", dv);
		break;

	case _CB:
		GetDv(((CLS_CB*) key.m_pData)->id, "cb", dv);
		break;

	case _FI:
		GetDv(((CLS_FI*) key.m_pData)->sec, "sec", dv);
		break;

	default:
		return FDIR_R_FAIL;
		break;
	}

	if (dv.id <= 0)
	{
		return FDIR_R_FAIL;
	}

	sprintf(sql, "SELECT ID,RECV_POINT FROM FDIR_FDIRECT WHERE FD=\'%ld\' AND STATUS=1", dv.id);

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	cout << sql << endl;

	timeval tm_now;
	gettimeofday(&tm_now, NULL);

	if (ret != OCI_ERROR)
	{
		if (rec_num > 0)
		{
			for (int i = 0; i < rec_num; i++)
			{
				char recv_point[1000];
				char fault_id[FAULT_ID_LENGTH];
				bzero(fault_id, sizeof(fault_id));
				bzero(recv_point, sizeof(recv_point));

				memcpy(fault_id, buf + i * (attrs[0].col_width + attrs[1].col_width), attrs[0].col_width);
				memcpy(recv_point, buf + i * (attrs[0].col_width + attrs[1].col_width) + attrs[0].col_width, attrs[1].col_width);

				struct tm tm_fault = {0};
				long usec = 0;
				long sec = 0;
				sscanf(fault_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", &tm_fault.tm_year, &tm_fault.tm_mon, &tm_fault.tm_mday, &tm_fault.tm_hour, &tm_fault.tm_min, &tm_fault.tm_sec, &usec);
				tm_fault.tm_mon = tm_fault.tm_mon - 1;
				tm_fault.tm_year = tm_fault.tm_year - 1900;
				sec = mktime(&tm_fault);
				//故障发生时间在半小时之前则不考虑
				if (tm_now.tv_sec - sec > 1800)
				{
					continue;
				}
				printf("故障时间：%ld，旧故障时间：%ld\r\n", tm_now.tv_sec, sec);
				printf("recv_point :%s\r\n", recv_point);

				char *pbuf1, *pbuf2;

				pbuf1 = pbuf2 = recv_point;

				while ((pbuf2 = strchr(pbuf1, ';')) != NULL)
				{
					char cb[MAX_CB_ID_LEN + 5] = { 0 };

					bzero(cb, sizeof(cb));

					strncpy(cb, pbuf1, pbuf2 - pbuf1);

					char *pTemp = strstr(cb, "[CB]");

					if (pTemp != NULL)
					{
						*pTemp = 0;
						CLS_CB *open_cb = dsbob->cb_tb->Find(CLS_CB(atol(cb)));
						lst_bef_fi.push_back(FAULT_ITEM(0, open_cb->pid, "CB", open_cb->descr));
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
		return FDIR_R_FAIL;
	}

	return FDIR_R_SUCCEED;
}

int FDIR_R::SetBefFaultflags(list<FAULT_ITEM> lst_fi)
{
	for (list<FAULT_ITEM>::iterator it_lst = lst_fi.begin(); it_lst != lst_fi.end(); it_lst++)
	{
		CLS_CB *cb = dsbob->cb_tb->Find(CLS_CB(it_lst->eq_id));
		if (cb == NULL)
		{
			continue;
		}
		cb->topflags |= FDIR_CB_BEFORE_OPEN;
	}
	return FDIR_R_SUCCEED;
}
int FDIR_R::SetFaultTopflags(FDIR_POINT_TRIG_SECTION &lstTrig, FDIR_TREE_NODE_DATA &key_dev, map<long, FDIR_POINT_TRIG_SECTION> &map_cb_faults)
{
	int i;

	int lv_cb = dsbob->cb_tb->GetCount();
	int lv_ld = dsbob->ld_tb->GetCount();
	int lv_fi = dsbob->fi_tb->GetCount();
	int lv_bus = dsbob->bus_tb->GetCount();

	for (i = 0; i < lv_cb; i++)
	{
		CLS_CB *cb = dsbob->cb_tb->GetDataByPos(i);

		cb->topflags &= ~FDIR_CB_DETECT_OPEN;
		cb->topflags &= ~FDIR_CB_DETECT_FAULT;
		cb->topflags &= ~FDIR_CB_DETECT_FI;
		cb->topflags &= ~FDIR_CB_BEFORE_OPEN;
		cb->topflags &= ~FDIR_FAULT_HEAD;
		cb->topflags &= ~FDIR_FAULT_TAIL;
	}

	for (i = 0; i < lv_ld; i++)
	{
		CLS_LD *ld = dsbob->ld_tb->GetDataByPos(i);

		ld->topflags &= ~FDIR_CB_DETECT_FI;
		ld->topflags &= ~FDIR_FAULT_HEAD;
		ld->topflags &= ~FDIR_FAULT_TAIL;
	}

	for (i = 0; i < lv_fi; i++)
	{
		CLS_FI *fi = dsbob->fi_tb->GetDataByPos(i);

		fi->topflags &= ~FDIR_CB_DETECT_FI;
		fi->topflags &= ~FDIR_FAULT_HEAD;
		fi->topflags &= ~FDIR_FAULT_TAIL;
	}

	for (i = 0; i < lv_bus; i++)
	{
		CLS_BUS *bus = dsbob->bus_tb->GetDataByPos(i);

		bus->topflags &= ~FDIR_BUS_DETECT;
		bus->topflags &= ~FDIR_FAULT_HEAD;
		bus->topflags &= ~FDIR_FAULT_TAIL;
	}

	lst_point.clear();
	lst_fault_open_cb.clear();
	//设置各故障信号对应开关的标志位
	for (FDIR_POINT_TRIG_SECTION::iterator it = lstTrig.begin(); it != lstTrig.end(); it++)
	{
		TRACE("key_id:%ld st_id:%ld\r\n", (*it)->alarm.key_id, (*it)->alarm.st_id);

		if ((*it)->alarm.dev == ALARM_FAULTINFO_DL || (*it)->alarm.dev == ALARM_FAULTINFO_JD)
		{
			long pid = (*it)->psbob_data.faultinfo.id;
			long id = pid;

			pid_to_id(id);

			CLS_FI *fi = dsbob->fi_tb->Find(CLS_FI(id));

			if (fi == NULL)
			{
				TRACE("获取遥信报警%ld对应的故障指示器设备失败！\r\n", pid);
				if (it == lstTrig.begin())
				{
					//故障定位失败，要将启动信号设置成已经使用过
					lstTrig.erase(++it, lstTrig.end());
					return FDIR_R_FAIL;
				}
			}

			if (key_dev.m_pData == NULL)
			{
				key_dev = fi;
			}

			lst_point.push_back(FAULT_ITEM(pid, fi->sec, "FI", fi->descr));
			fi->topflags |= FDIR_CB_DETECT_FI;
			map_cb_faults[fi->id].push_back(*it);
		}
		else if ((*it)->alarm.dev == ALARM_BREAKER || (*it)->alarm.dev == ALARM_SGZ)
		{
			unsigned long pid = (*it)->psbob_data.point.id;
			long id = 0;

			KEY_STRU ks = { 0 };

			//报警有可能是遥信ID，统一转成开关ID进行处理
			CCommon::long_to_keyid(pid, &ks);
			ks.field_id = 0;

			unsigned long uid = 0;
			CCommon::keyid_to_long(&ks, &uid);
			id = uid;
			pid_to_id(id);

			CLS_CB *cb = NULL;

			cb = dsbob->cb_tb->Find(CLS_CB(id));

			if (cb == NULL)
			{
				TRACE("获取遥信报警%ld对应的开关设备失败！\r\n", pid);
				if (it == lstTrig.begin())
				{
					//故障定位失败，要将启动信号设置成已经使用过
					lstTrig.erase(++it, lstTrig.end());
					return FDIR_R_FAIL;
				}
			}
			else
			{
				if (key_dev.m_pData == NULL)
				{
					key_dev = cb;
				}

				TRACE("key_id:%ld st_id:%ld\r\n", (*it)->alarm.key_id, (*it)->alarm.st_id);

				if (/*(*it)->alarm.type == SG_WARN_TYPE ||*/ D_SG_WARN_TYPE == (*it)->alarm.type)	//事故跳闸
				{
					lst_point.push_back(FAULT_ITEM(0, pid, "CB", cb->descr));
					cb->topflags |= FDIR_CB_DETECT_FAULT;
					map_cb_faults[cb->id].push_back(*it);
					lst_fault_open_cb.push_back(dsbob->cb_tb->FindPos(*cb));
				}
				else
				{
					lst_point.push_back(FAULT_ITEM(0, pid, "CB", cb->descr));
					cb->topflags |= FDIR_CB_DETECT_OPEN;
					map_cb_faults[cb->id].push_back(*it);
					lst_fault_open_cb.push_back(dsbob->cb_tb->FindPos(*cb));
				}
			}
		}
		// 判断是母线故障
		else if (strcasecmp((*it)->alarm.tabname, "bus") == 0)
		{
			unsigned long id = (*it)->alarm.key_id;
			CLS_BUS *bus = NULL;
			bus = dsbob->bus_tb->Find(CLS_BUS(id));

			if (bus == NULL)
			{
				TRACE("获取母线%ld失压信号对应的设备失败！\r\n", id);
				if (it == lstTrig.begin())
				{
					//故障定位失败，要将启动信号设置成已经使用过
					lstTrig.erase(++it, lstTrig.end());
					return FDIR_R_FAIL;
				}
			}
			else
			{
				if (key_dev.m_pData == NULL)
				{
					key_dev = bus;
				}

				TRACE("key_id:%ld st_id:%ld\r\n", (*it)->alarm.key_id, (*it)->alarm.st_id);
				lst_point.push_back(FAULT_ITEM(0, id, "BUS", bus->descr));
				bus->topflags |= FDIR_BUS_DETECT;
				map_cb_faults[bus->id].push_back(*it);
				TRACE("获取母线%ld故障成功！\r\n", (*it)->alarm.key_id);
			}
		}
	}

	return FDIR_R_SUCCEED;
}

//获取节点pnode与节点childnode之间的所有设备
void FDIR_R::GetDevsBetweenNodes(TreeNode<FDIR_TREE_NODE_DATA>* pnode, TreeNode<FDIR_TREE_NODE_DATA>* childnode, list<FAULT_ITEM> &devs)
{
	if (m_Tree.root == NULL || pnode == NULL)
	{
		return;
	}

	TreeNode<FDIR_TREE_NODE_DATA>* pchild = pnode->childnode;
	while(pchild != NULL)
	{
		if (pchild->data.m_type == _CB)
		{
			TRACE("路过开关：%ld(%s)\r\n", ((CLS_CB*)pchild->data.m_pData)->id, ((CLS_CB*)pchild->data.m_pData)->descr);
		}
		else if (pchild->data.m_type == _FI)
		{
			TRACE("路过故障指示器：%ld(%s)\r\n", ((CLS_FI*)pchild->data.m_pData)->id, ((CLS_FI*)pchild->data.m_pData)->descr);
		}
		else if (pchild->data.m_type == _LD)
		{
			TRACE("路过负荷：%ld(%s)\r\n", ((CLS_LD*)pchild->data.m_pData)->id, ((CLS_LD*)pchild->data.m_pData)->descr);
		}
		if (pchild != childnode && pchild->data.m_type != _LD)
		{
			GetDevsBetweenNodes(pchild, childnode, devs);
		}
		if (pchild->data.m_type == _LD)
		{
			devs.push_back(FAULT_ITEM(0, ((CLS_LD*)pchild->data.m_pData)->pid, "ld", ((CLS_LD*)pchild->data.m_pData)->descr));
			TRACE("找到负荷：%ld(%s)\r\n", ((CLS_LD*)pchild->data.m_pData)->id, ((CLS_LD*)pchild->data.m_pData)->descr);
		}
		pchild = pchild->next;
	}
}

int FDIR_R::BuildTree(FDIR_TREE_NODE_DATA &key_dev, Tree<FDIR_TREE_NODE_DATA> &tree)
{
	int lv_sec = dsbob->sec_tb->GetCount();
	int lv_nd = dsbob->nd_tb->GetCount();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	int lv_cb = dsbob->cb_tb->GetCount();
	int lv_switch = dsbob->switch_tb->GetCount();
	int jxcb_ret = 0;
	map_jxcb_bus.clear();

	for(int i = 0; i < lv_cb; i++)
	{
		dsbob->cb_tb->GetDataByPos(i)->topflags |= FDIR_CB_NOAVAIL;
	}

	for(int i = 0; i < lv_switch; i++)
	{
		dsbob->switch_tb->GetDataByPos(i)->topflags |= FDIR_CB_NOAVAIL;
	}

	//保存树上所有进线开关
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_jxcb;

	bool *bvisited_sec = new bool[lv_sec];
	bool *bvisited_nd = new bool[lv_nd];
	memset(bvisited_sec, false, sizeof(bool) * lv_sec);
	memset(bvisited_nd, false, sizeof(bool) * lv_nd);

	m_p_jxcb_Node = NULL;
	m_p_bus_Node = NULL;

	TRACE("查找供电母线，并分析拓扑连接关系！\r\n");

	//以故障启动开关为根结点建立树
	tree.TreeRemoveSubTree(tree.root);

	Tree<FDIR_TREE_NODE_DATA> tree1, tree2;
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_nodes;
	list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lst;
	map<TreeNode<FDIR_TREE_NODE_DATA>*, TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_jxcb_bus;

	//启动信号关联设备为开关
	if (key_dev.m_type == _CB)
	{
		CLS_CB *key_cb = (CLS_CB*) key_dev.m_pData;
		int key_icb = dsbob->cb_tb->FindPos(*key_cb);
		bool b_in_ind = false;

		key_cb->topflags &= ~FDIR_CB_NOAVAIL;
		bvisited_nd[key_cb->ind] = true;
		bvisited_nd[key_cb->iznd] = true;
		TreeNode<FDIR_TREE_NODE_DATA> *befather1, *befather2;
		befather1 = tree1.TreeAddLeaf(NULL, key_cb);
		befather2 = tree2.TreeAddLeaf(NULL, key_cb);

		if (key_cb->q & CB_FDHCB)
		{
			lst_jxcb.push_back(befather1);
		}

		build_fi_tree_BFS(key_cb->ind, bvisited_sec, bvisited_nd, befather1, tree1, true, lst_jxcb, true,true);
		tree1.SaveTree("tree1.xml");
		TRACE("=============================================================\r\n");
		build_fi_tree_BFS(key_cb->iznd, bvisited_sec, bvisited_nd, befather2, tree2, true, lst_jxcb, true,true);
		tree2.SaveTree("tree2.xml");

		cout << "建立起进线开关和母线映射的个数对为" << map_jxcb_bus.size() << endl;
		m_p_jxcb_Node = GetValidJXCBNode(map_jxcb_bus, jxcb_ret);

		if (m_p_jxcb_Node == NULL)
		{
			TRACE("未找到进线开关！\r\n");
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_NO_JXCB;
		}

		it_jxcb_bus = map_jxcb_bus.find(m_p_jxcb_Node);
		m_p_bus_Node = it_jxcb_bus->second; // 找到进线开关对应的母线

		TreeNode<FDIR_TREE_NODE_DATA> *pRoot = m_p_bus_Node;

		while (pRoot->fathernode != NULL)
		{
			pRoot = pRoot->fathernode;
		}

		if (pRoot == tree1.root)
		{
			b_in_ind = true;
			TRACE("母线在IND(%ld)端！\r\n", key_cb->nd);
		}
		else
		{
			b_in_ind = false;
			TRACE("母线在IZND(%ld)端！\r\n", key_cb->znd);
		}

		Tree<FDIR_TREE_NODE_DATA> *pMainTree = NULL, *pSubTree = NULL;
		if (b_in_ind)
		{
			pMainTree = &tree1;
			pSubTree = &tree2;
		}
		else
		{
			pMainTree = &tree2;
			pSubTree = &tree1;
		}

		pMainTree->ChangeRoot(m_p_bus_Node, tree, tree.root);
		tree.BreadthFirstErgodic(tree.root, lst_nodes);

		for (it_lst = lst_nodes.begin(); it_lst != lst_nodes.end(); it_lst++)
		{
			if ((*it_lst)->data.m_type == key_dev.m_type && (*it_lst)->data.m_pData == key_dev.m_pData)
			{
				TRACE("找到启动信号开关在树上的位置, 准备插入子树。\r\n");
				TreeNode<FDIR_TREE_NODE_DATA> *p = tree.GetParent(*it_lst);
				tree.TreeAddSubTree(p, pSubTree);
				if (key_cb->q & CB_FDHCB)
				{
					if (((CLS_CB*) ((*it_lst)->data.m_pData))->id == ((CLS_CB*) (m_p_jxcb_Node->data.m_pData))->id)
					{
						m_p_jxcb_Node = pSubTree->root;
					}
				}
				tree.TreeRemoveSubTree(*it_lst);
				break;
			}
		}
	}
	//启动信号关联设备为负荷
	else if (key_dev.m_type == _LD)
	{
		CLS_LD *ld = (CLS_LD*) key_dev.m_pData;

		TreeNode<FDIR_TREE_NODE_DATA> *proot = tree1.TreeAddLeaf(NULL, ld);

		bvisited_nd[ld->ind] = true;

		build_fi_tree_BFS(ld->ind, bvisited_sec, bvisited_nd, proot, tree1, true, lst_jxcb, true,true);

		tree1.SaveTree("tree1.xml");
		m_p_jxcb_Node = GetValidJXCBNode(map_jxcb_bus, jxcb_ret);

		if (m_p_jxcb_Node == NULL)
		{
			TRACE("未找到进线开关！\r\n");
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_NO_JXCB;
		}

		it_jxcb_bus = map_jxcb_bus.find(m_p_jxcb_Node);
		m_p_bus_Node = it_jxcb_bus->second; // 找到进线开关对应的母线

		tree1.ChangeRoot(m_p_bus_Node, tree, tree.root);
	}
	else if (key_dev.m_type == _BUS)
	{
		TRACE("故障启动信号为母线！\r\n");
		CLS_BUS *bus = (CLS_BUS*) key_dev.m_pData;

		TreeNode<FDIR_TREE_NODE_DATA> *proot = tree.TreeAddLeaf(NULL, bus);

		bvisited_nd[bus->ind] = true;
		build_fi_tree_BFS(bus->ind, bvisited_sec, bvisited_nd, proot, tree, true, lst_jxcb, true,true);
		m_p_bus_Node = tree.root; // 树的根结点即为母线结点

	}
	else if (key_dev.m_type == _FI)
	{
		CLS_FI *fi = (CLS_FI*) key_dev.m_pData;
		CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(fi->sec));
		if (sec == NULL)
		{
			TRACE("未找到故障指示器%ld关联的线段！\r\n", fi->id);
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_FAIL;
		}
		int pos = dsbob->sec_tb->FindPos(*sec);
		bool b_in_ind = false;

		//解决故障指示器短路或接地故障中故障区域定位错误的问题，目前的想法是将tree1的根节点确定为触发故障的故障指示器，
		//这样貌似可以确保故障树连接点顺序与实际相符
		bvisited_sec[pos] = false;
		bvisited_nd[sec->ind] = true;
		bvisited_nd[sec->iznd] = false;

		//下面的操作是为了避免从故障指示器两端同时查找，导致故障树生成错误
		CLS_ND *pnd = dsbob->nd_tb->GetTable() + sec->ind;
		ChildPtr *tmp_sec_ptr = NULL;
		if (pnd != NULL)
		{
			ChildPtr *sec_ptr = dsbob->r_nd_sec_tb->FindChildPtr(pnd->id);
			tmp_sec_ptr = sec_ptr;
			if (tmp_sec_ptr != NULL && tmp_sec_ptr->Next != NULL)
			{
				bvisited_sec[tmp_sec_ptr->Next->Ptr] = true;
			}		
		}
		
		build_fi_tree_BFS(sec->ind, bvisited_sec, bvisited_nd, NULL, tree1, true, lst_jxcb, true, true);

		FDIR_TREE_NODE_DATA root = tree1.root->data;
		CLS_CB* fcb = (CLS_CB*)(root.m_pData);
		TRACE("\033[40;33m$$$$$$$father:%s id:%ld\033[0m\r\n", fcb->descr, fcb->pid);

		//把一端堵死，从另一端开始查找
		bvisited_sec[pos] = true;
		if (tmp_sec_ptr != NULL && tmp_sec_ptr->Next != NULL)
		{
			bvisited_sec[tmp_sec_ptr->Next->Ptr] = false;
		}	
		//bvisited_nd[sec->ind] = false;
		//bvisited_nd[sec->iznd] = true;
		TRACE("=============================================================\r\n");
		build_fi_tree_BFS(sec->ind, bvisited_sec, bvisited_nd, tree2.TreeAddLeaf(NULL, root), tree2, true, lst_jxcb, true, true);
		cur_father = NULL;
		tree1.SaveTree("tree1.xml");
		tree2.SaveTree("tree2.xml");
		m_p_jxcb_Node = GetValidJXCBNode(map_jxcb_bus, jxcb_ret);

		if (m_p_jxcb_Node == NULL)
		{
			TRACE("未找到进线开关！\r\n");
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_NO_JXCB;
		}

		it_jxcb_bus = map_jxcb_bus.find(m_p_jxcb_Node);
		m_p_bus_Node = it_jxcb_bus->second; // 找到进线开关对应的母线
		TreeNode<FDIR_TREE_NODE_DATA> *pRoot = m_p_bus_Node;

		while(pRoot->fathernode != NULL)
		{
			pRoot = pRoot->fathernode;
		}

		if(pRoot == tree1.root)
		{
			b_in_ind = true;
			TRACE("母线在IND(%ld)端！\r\n", sec->nd);
		}
		else
		{
			b_in_ind = false;
			TRACE("母线在IZND(%ld)端！\r\n", sec->znd);
		}

		Tree<FDIR_TREE_NODE_DATA> *pMainTree = NULL, *pSubTree = NULL;
		if (b_in_ind)
		{
			pMainTree = &tree1;
			pSubTree = &tree2;
		}
		else
		{
			pMainTree = &tree2;
			pSubTree = &tree1;
		}

		pMainTree->ChangeRoot(m_p_bus_Node, tree, tree.root);
		tree.BreadthFirstErgodic(tree.root, lst_nodes);
		//
		for (it_lst = lst_nodes.begin(); it_lst != lst_nodes.end(); it_lst++)
		{
			if ((*it_lst)->data.m_type == root.m_type && (*it_lst)->data.m_pData == root.m_pData)
			{
				TRACE("找到启动信号开关在树上的位置, 准备插入子树。\r\n");
				TreeNode<FDIR_TREE_NODE_DATA> *p = tree.GetParent(*it_lst);
				tree.TreeAddSubTree(p, pSubTree);
				tree.TreeRemoveSubTree(*it_lst);
				break;
			}
		}

	}
	else
	{
		TRACE("不支持的故障启动设备类型：%d\r\n", key_dev.m_type);
		delete[] bvisited_sec;
		delete[] bvisited_nd;
		return FDIR_R_FAIL;
	}

	if(key_dev.m_type != _BUS)
	{
		faultjxcb = dsbob->cb_tb->FindPos(*((CLS_CB*) m_p_jxcb_Node->data.m_pData));
	}
	else
	{
		//母线故障的时候以母线为根节点建立故障树
		faultjxcb = -1;
	}

	delete[] bvisited_sec;
	delete[] bvisited_nd;

	return FDIR_R_SUCCEED;
}

int FDIR_R::GetFaultPath(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FDIR_TREE_NODE_DATA &key_dev)
{
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_nodes;
	list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lst;

	//在树上将有故障信号的结点找出来
	lst_nodes.clear();
	map<TreeNode<FDIR_TREE_NODE_DATA>*, list<TreeNode<FDIR_TREE_NODE_DATA>*> > map_nodes;	//故障信号与故障信号到树根结点路径映射

	m_Tree.BreadthFirstErgodic(m_Tree.root, lst_nodes);
	lst_edge_cb.clear();
	//查找故障馈线的边界开关，后面对故障进行分组时用到
	TRACE("边界开关：\r\n========================================\r\n");
	for (it_lst = lst_nodes.begin(); it_lst != lst_nodes.end(); it_lst++)
	{
		if ((*it_lst)->data.m_type == _CB)
		{
			CLS_CB *cb = (CLS_CB*) ((*it_lst)->data.m_pData);
			TRACE("树节点是：%ld %s\r\n", cb->id, cb->descr);
			if (cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI))
			{
				TRACE("树上发现故障信号开关%ld[%s]\r\n", cb->id, cb->descr);

				map_nodes[*it_lst] = list<TreeNode<FDIR_TREE_NODE_DATA>*>();
			}

			if (m_Tree.GetChildNode(*it_lst) == NULL)
			{
				TRACE("%ld[%s]\r\n", cb->id, cb->descr);
				lst_edge_cb.push_back(dsbob->cb_tb->FindPos(*cb));
			}
		}
		else if ((*it_lst)->data.m_type == _FI)
		{
			CLS_FI *fi = (CLS_FI*) ((*it_lst)->data.m_pData);
			TRACE("树节点是：%ld %s\r\n", fi->id, fi->descr);

			if (fi->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI))
			{
				TRACE("树上发现故障信号故障指示器%ld[%s]\r\n", fi->id, fi->descr);

				map_nodes[*it_lst] = list<TreeNode<FDIR_TREE_NODE_DATA>*>();
			}
		}
		else if ((*it_lst)->data.m_type == _BUS)
		{
			CLS_BUS *bus = (CLS_BUS*) ((*it_lst)->data.m_pData);

			if (bus->topflags & FDIR_BUS_DETECT)
			{
				TRACE("树上发现故障信号母线%ld[%s]\r\n", bus->id, bus->descr);

				map_nodes[*it_lst] = list<TreeNode<FDIR_TREE_NODE_DATA>*>();
			}
		}
		else
		{
			CLS_LD *ld = (CLS_LD*) ((*it_lst)->data.m_pData);
			TRACE("树节点是：%ld %s\r\n", ld->id, ld->descr);

			if (ld->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI))
			{
				TRACE("树上发现故障信号负荷%ld[%s]\r\n", ld->id, ld->descr);

				map_nodes[*it_lst] = list<TreeNode<FDIR_TREE_NODE_DATA>*>();
			}
		}
	}

	cout << "\r\n=======================================" << endl;

	map<TreeNode<FDIR_TREE_NODE_DATA>*, list<TreeNode<FDIR_TREE_NODE_DATA>*> >::iterator it_map;

	//计算出各故障信号结点到树根结点的路径
	for (it_map = map_nodes.begin(); it_map != map_nodes.end(); it_map++)
	{
		//通过反复获取父结点来计算路径
		TreeNode<FDIR_TREE_NODE_DATA> *pParent = it_map->first;

		do
		{
			it_map->second.push_back(pParent);

			pParent = m_Tree.GetParent(pParent);

			if (pParent != NULL)
			{
				//判断某个故障信号到根结点的路径上是否包含其它故障信号
				map<TreeNode<FDIR_TREE_NODE_DATA>*, list<TreeNode<FDIR_TREE_NODE_DATA>*> >::iterator it_find = map_nodes.find(pParent);
				if (it_find != map_nodes.end())
				{
					map_nodes.erase(it_find);
				}
			}
		} while (pParent != NULL);
	}

	//找出一个包含启动故障信号的故障路径作为新的故障
	for (it_map = map_nodes.begin(); it_map != map_nodes.end(); it_map++)
	{
		TRACE("故障路径：\r\n");

		for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_cbs = it_map->second.begin(); it_cbs != it_map->second.end(); it_cbs++)
		{
			if ((*it_cbs)->data.m_type == _CB)
			{
				TRACE("(CB)%ld[%s]\r\n", ((CLS_CB*) (*it_cbs)->data.m_pData)->id, ((CLS_CB*) (*it_cbs)->data.m_pData)->descr);
			}
			else if ((*it_cbs)->data.m_type == _FI)
			{
				TRACE("(FI)%ld[%s]\r\n  ", ((CLS_FI*) (*it_cbs)->data.m_pData)->id, ((CLS_FI*) (*it_cbs)->data.m_pData)->descr);
			}
			else if ((*it_cbs)->data.m_type == _BUS)
			{
				TRACE("(BUS)%ld[%s]\r\n  ", ((CLS_BUS*) (*it_cbs)->data.m_pData)->id, ((CLS_BUS*) (*it_cbs)->data.m_pData)->descr);
			}
			else
			{
				TRACE("(LD)%ld[%s]\r\n", ((CLS_LD*) (*it_cbs)->data.m_pData)->id, ((CLS_LD*) (*it_cbs)->data.m_pData)->descr);
			}

			if (key_dev.m_type == (*it_cbs)->data.m_type && key_dev.m_pData == (*it_cbs)->data.m_pData)
			{
				lstPath = it_map->second;
			}
		}

		cout << endl;
	}

	if (lstPath.size() <= 0)
	{
		if (key_dev.m_type == _CB)
		{
			TRACE("找不到一条包含启动信号%ld[%s]的故障路径\r\n", ((CLS_CB*) key_dev.m_pData)->id, ((CLS_CB*) key_dev.m_pData)->descr);
		}
		else if (key_dev.m_type == _FI)
		{
			TRACE("找不到一条包含启动信号%ld[%s]的故障路径\r\n", ((CLS_FI*) key_dev.m_pData)->id, ((CLS_FI*) key_dev.m_pData)->descr);
		}
		else if (key_dev.m_type == _BUS)
		{
			TRACE("找不到一条包含启动信号%ld[%s]的故障路径\r\n", ((CLS_BUS*) key_dev.m_pData)->id, ((CLS_BUS*) key_dev.m_pData)->descr);
		}
		else
		{
			TRACE("找不到一条包含启动信号%ld[%s]的故障路径\r\n", ((CLS_LD*) key_dev.m_pData)->id, ((CLS_LD*) key_dev.m_pData)->descr);
		}

		return FDIR_R_FAIL;
	}

	return FDIR_R_SUCCEED;
}


void FDIR_R::GetFaultType(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FAULT_TYPE_CONTEXT &type, bool &ld_fault)
{
	/*
	 * 	判断逻辑
	 *
	 * 	故障信号收集完成后判断故障上游是否有最终状态为分闸的开关。
	 * 	1、	如果有则启动故障方案的计算；
	 *	2、	如果无
	 *		a)	所有开关状态均采集，判断是否有开关事故跳闸
	 *			i.	若有，则判断为瞬时故障，重合闸成功。
	 *			ii.	否则，将故障信息以误报的形式提示给用户；
	 *		b)	有不采集遥信的开关，则在库中写入一条故障信息，该故障与明确定位的故障进行区分（可能有故障）；
	 */
	bool bHasOpen = false;	//是否有最终状态为分闸的开关
	bool bHasFault = false;	//是否有事故跳闸 的开关
	bool bHasNoValue = false;	//是否有状态不采集的开关

	ld_fault = false;
	//获取故障路径上的开关状态采集状况
	for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lstPath = lstPath.begin(); it_lstPath != lstPath.end(); it_lstPath++)
	{
		if ((*it_lstPath)->data.m_type == _CB)
		{
			CLS_CB *cb = (CLS_CB*) ((*it_lstPath)->data.m_pData);
			time_t tmxc = 0;	//最后一次上遥信的时间
			PSBOB::CB psbob_cb = { 0 };

			if (oodbread_rk(&psbob_cb, &cb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) <= 0
					|| HasMask(g_db_psbob->GetTB("breaker"), "status", psbob_cb.status, MENU_STATE_YX_REAL))
			{
				FDIR_CB_INFO cb_info = { 0 };
				strcpy(cb_info.cb_descr, cb->descr);
				cb_info.cb_id = cb->pid;
				cb_info.pt_id = cb->pid;
				cb_info.pt_timexc = tmxc;
				cb_info.reason = NO_VALUE;
				lst_cb_info.push_back(cb_info);

				bHasNoValue = true;
				TRACE("开关%ld(%s)状态不采集\r\n", cb->id, cb->descr);
			}

			//有事故跳闸的开关
			if (cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN))
			{
				bHasFault = true;
				TRACE("开关%ld(%s)为跳闸开关\r\n", cb->id, cb->descr);
			}

			//有最终状态为分闸的开关
			if (psbob_cb.point == 0)
			{
				bHasOpen = true;
				TRACE("开关%ld(%s)为分闸状态\r\n", cb->id, cb->descr);
			}
		}
		else if ((*it_lstPath)->data.m_type == _LD)
		{
			ld_fault = true;
		}
	}

	if (bHasOpen)	//有分闸开关
	{
		type = FAULT_TYPE_SURE;
	}
	else
	{
		if (bHasNoValue)	//有状态不采集的开关
		{
			type = FAULT_TYPE_POSSIBLE;
		}
		else
		{
			if (bHasFault) //有事故跳闸的开关
			{
				type = FAULT_TYPE_WINK;
			}
			else
			{
				type = FAULT_TYPE_NONE;
			}
		}
	}
}

//判断开关上是否有过流，事故总信号等
bool FDIR_R::find_cb_fi(long cb_id)
{
	relaysig_dscada *sig = NULL;
	TB_DESCR *tb = const_cast<TB_DESCR*>(g_db_psbob->GetTB("relaysig"));

	int ret = GetTableRecs(tb, 1, (char**) &sig);
	if (ret > 0)
	{
		for (int i = 0; i < ret / sizeof(relaysig_dscada); i++)
		{
			if (sig[i].bay_id == cb_id && (HasMask(tb, "pnt_type", sig[i].pnt_type, MENU_TYPE_RELAY_ACT) || HasMask(tb, "pnt_type", sig[i].pnt_type, MENU_TYPE_RELAY_SGZ)))
			{
				BufFree(sig);
				return true;
			}
		}
	}

	return false;
}

//获取从pnode开始的子树中的结点，直到故障指示器或者树的末端结束
void FDIR_R::GetDownDevs(TreeNode<FDIR_TREE_NODE_DATA>* pnode, list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_dev)
{
	TreeNode<FDIR_TREE_NODE_DATA>* pchild_node = m_Tree.GetChildNode(pnode);

	if(pchild_node == NULL)
	{
		return;
	}

	while(pchild_node != NULL)
	{
		if ((pchild_node->data.m_type == _FI) || (pchild_node->data.m_type == _LD))
		{
			lst_dev.push_back(pchild_node);
		}
		//如果结点为开关类型，判断开关上是否有故障信号，或者是否能够自动跳闸
		else
		{
			CLS_CB *cb = (CLS_CB *) (pchild_node->data.m_pData);
			PSBOB::CB psbob_cb = { 0 };

			//崔工要求考故障最未端信号漏报时的故障定位（故障下游的设备数据非实测，即认为链路不通，该信号可能漏报）
			if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), (char*) &cb->pid, sizeof(PSBOB::CB), (char*) &psbob_cb) > 0
					&& HasMask(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), "status", psbob_cb.status, MENU_STATE_YX_REAL))
			{
				lst_mis_point.push_back(psbob_cb.id);
				TRACE("断路器%ld[%s]采集状态为非实测，该信号可能为漏报，扩大故障定位范围。\r\n", psbob_cb.id, psbob_cb.name);
				GetDownDevs(pchild_node, lst_dev);
			}
			else
			{
				if (find_cb_fi(cb->id))
				{
					lst_dev.push_back(pchild_node);
				}
				else
				{
					if (cb->q & CB_AUTO)
					{
						lst_dev.push_back(pchild_node);
					}
					else
					{
						GetDownDevs(pchild_node, lst_dev);
					}
				}
			}
		}

		pchild_node = m_Tree.GetNextBrotherNode(pchild_node);
	}
}

void FDIR_R::FillUsedFaultInfo(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FDIR_POINT_TRIG_SECTION &lstTrig, map<long, FDIR_POINT_TRIG_SECTION> &map_cb_faults)
{
	g_lstSecTrig.clear();
	fault_fi_type = FAULT_UNKNOW;
	for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_path = lstPath.begin(); it_path != lstPath.end(); it_path++)
	{
		if ((*it_path)->data.m_type == _CB)
		{
			map<long, FDIR_POINT_TRIG_SECTION>::iterator it_find = map_cb_faults.find(((CLS_CB*) (*it_path)->data.m_pData)->id);

			if (it_find != map_cb_faults.end())
			{
				//D5000故障指示器报警还不清楚，无法判断故障类型
				for (std::list<FDIR_POINT_TRIG*>::iterator it_lst_trig = it_find->second.begin(); it_lst_trig != it_find->second.end(); it_lst_trig++)
				{
					//单相接地故障可能会不存在分闸的开关
					if ((*it_lst_trig)->alarm.type == ALARM_FAULTINFO_JD)
					{
						if (fault_type != FAULT_TYPE_WINK)
						{
							fault_type = FAULT_TYPE_SURE;
						}
						fault_fi_type = FAULT_JDFAULT;
					}
					else if ((*it_lst_trig)->alarm.type == ALARM_FAULTINFO_DL)
					{
						fault_fi_type = FAULT_DLFAULT;
					}
				}

				g_lstSecTrig.merge(it_find->second);

				CLS_CB* cb = dsbob->cb_tb->Find(CLS_CB(it_find->first));

				//合闸事故总漏报的开关
				if ((cb != NULL) && (cb->topflags & FDIR_CB_DETECT_OPEN))
				{
					cb->q &= ~CB_OPEN;
					TRACE("合闸事故总漏报的开关%ld[%s]\r\n", cb->id, cb->descr);
				}
			}
		}
		else if ((*it_path)->data.m_type == _LD)
		{
			map<long, FDIR_POINT_TRIG_SECTION>::iterator it_find = map_cb_faults.find(((CLS_LD*) (*it_path)->data.m_pData)->id);

			if (it_find != map_cb_faults.end())
			{
				g_lstSecTrig.merge(it_find->second);
			}
		}
		else if ((*it_path)->data.m_type == _FI)
		{
			map<long, FDIR_POINT_TRIG_SECTION>::iterator it_find = map_cb_faults.find(((CLS_FI*) (*it_path)->data.m_pData)->id);

			if (it_find != map_cb_faults.end())
			{
				//D5000故障指示器报警还不清楚，无法判断故障类型
				for (std::list<FDIR_POINT_TRIG*>::iterator it_lst = it_find->second.begin(); it_lst != it_find->second.end(); it_lst++)
				{
					//单相接地故障可能会不存在分闸的开关
					if ((*it_lst)->alarm.type == ALARM_FAULTINFO_JD)
					{
						if (fault_type != FAULT_TYPE_WINK)
						{
							fault_type = FAULT_TYPE_SURE;
						}
						fault_fi_type = FAULT_JDFAULT;
					}
					else if ((*it_lst)->alarm.type == ALARM_FAULTINFO_DL)
					{
						fault_fi_type = FAULT_DLFAULT;
					}
				}

				g_lstSecTrig.merge(it_find->second);
			}
		}
		else if ((*it_path)->data.m_type == _BUS)
		{
			map<long, FDIR_POINT_TRIG_SECTION>::iterator it_find = map_cb_faults.find(((CLS_BUS*) (*it_path)->data.m_pData)->id);

			if (it_find != map_cb_faults.end())
			{
				g_lstSecTrig.merge(it_find->second);
			}
		}
	}
}

TreeNode<FDIR_TREE_NODE_DATA>* FDIR_R::GetValidJXCBNode(map<TreeNode<FDIR_TREE_NODE_DATA>*, TreeNode<FDIR_TREE_NODE_DATA>*> mapJXCB_BUS, int &jxcb_tp_type)
{
	if (mapJXCB_BUS.size() <= 0)
	{
		jxcb_tp_type = FDIR_R_NO_JXCB;
		return NULL;
	}
	else
	{
		TreeNode<FDIR_TREE_NODE_DATA>* pJXCB = NULL;

		//判断查找到的进线开关中哪个是带电的，找到有效的进线开关

		for (map<TreeNode<FDIR_TREE_NODE_DATA>*, TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_jxcb = mapJXCB_BUS.begin(); it_jxcb != mapJXCB_BUS.end(); it_jxcb++)
		{
			CLS_CB *temp_jxcb = (CLS_CB*) (it_jxcb->first)->data.m_pData;
			if (!(temp_jxcb->q & CB_OPEN) || (temp_jxcb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI | FDIR_CB_BEFORE_OPEN)))
			{
				if (pJXCB == NULL)
				{
					pJXCB = it_jxcb->first;
				}
				else if (pJXCB != it_jxcb->first)
				{
					TRACE("找到多个闭合的进线开关：%ld，%ld\r\n", ((CLS_CB*)(pJXCB->data.m_pData))->id, temp_jxcb->id);
					jxcb_tp_type = FDIR_R_MUTI_JXCB;
					return NULL;
				}
			}
		}

		if (pJXCB == NULL && mapJXCB_BUS.size() == 1)
		{
			pJXCB = mapJXCB_BUS.begin()->first;
		}

		if (pJXCB == NULL)
		{
			TRACE("没有找到供电的进线开关\r\n");
			for (map<TreeNode<FDIR_TREE_NODE_DATA>*, TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_jxcb = mapJXCB_BUS.begin(); it_jxcb != mapJXCB_BUS.end(); it_jxcb++)
			{
				CLS_CB *temp_jxcb = (CLS_CB*) (it_jxcb->first)->data.m_pData;
				if (temp_jxcb != NULL)
				{
					TRACE("断开的进线开关有：%ld\r\n", temp_jxcb->id);
				}
			}

			jxcb_tp_type = FDIR_R_NO_JXCB;
			return NULL;
		}

		jxcb_tp_type = FDIR_R_SUCCEED;
		return pJXCB;
	}
}

void FDIR_R::MaskFaultOnTree()
{
	switch (fault_head->data.m_type)
	{
	case _CB:
		((CLS_CB*) fault_head->data.m_pData)->topflags |= FDIR_FAULT_HEAD;
		TRACE("故障起始开关为:%ld[%s]\r\n", ((CLS_CB*) fault_head->data.m_pData)->id, ((CLS_CB*) fault_head->data.m_pData)->descr);
		break;

	case _BUS:
		((CLS_BUS*) fault_head->data.m_pData)->topflags |= FDIR_FAULT_HEAD;
		TRACE("故障起始母线为:%ld[%s]\r\n", ((CLS_BUS*) fault_head->data.m_pData)->id, ((CLS_BUS*) fault_head->data.m_pData)->descr);
		break;

	case _LD:
		((CLS_LD*) fault_head->data.m_pData)->topflags |= FDIR_FAULT_HEAD;
		TRACE("故障起始负荷为:%ld[%s]\r\n", ((CLS_LD*) fault_head->data.m_pData)->id, ((CLS_LD*) fault_head->data.m_pData)->descr);
		break;

	case _FI:
		((CLS_FI*) fault_head->data.m_pData)->topflags |= FDIR_FAULT_HEAD;
		TRACE("故障起始故障指示器为:%ld[%s]\r\n", ((CLS_FI*) fault_head->data.m_pData)->id, ((CLS_FI*) fault_head->data.m_pData)->descr);
		break;

	default:
		break;
	}

	for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it = fault_tail.begin(); it != fault_tail.end(); it++)
	{
		switch ((*it)->data.m_type)
		{
		case _CB:
			TRACE("故障末端开关:%ld[%s]\r\n", ((CLS_CB*) (*it)->data.m_pData)->id, ((CLS_CB*) (*it)->data.m_pData)->descr);
			((CLS_CB*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			break;

		case _LD:
			((CLS_LD*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			TRACE("故障末端负荷为:%ld[%s]\r\n", ((CLS_LD*) (*it)->data.m_pData)->id, ((CLS_LD*) (*it)->data.m_pData)->descr);
			break;

		case _BUS:
			((CLS_BUS*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			TRACE("故障末端也为该母线:%ld[%s]\r\n", ((CLS_BUS*) (*it)->data.m_pData)->id, ((CLS_BUS*) (*it)->data.m_pData)->descr);
			break;

		case _FI:
			((CLS_FI*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			TRACE("故障末端故障指示器为:%ld[%s]\r\n", ((CLS_FI*) (*it)->data.m_pData)->id, ((CLS_FI*) (*it)->data.m_pData)->descr);
			break;

		default:
			break;
		}
	}
}

// 获取故障母线和故障设备,分母线故障和其他故障分开处理
int FDIR_R::FindFaultBusAndFaultDevs(FDIR_TREE_NODE_DATA &key_dev, int faultjxcb)
{
	lst_fault_devs.clear();
	map_node_downnode.clear();
	//故障启动设备是母线时   faultjxcb为-1
	if (key_dev.m_type == _BUS)
	{
		//故障类型为母线故障
		if (fault_head->data.m_type == _BUS)
		{
			lst_up_devs.clear();
			lst_fault_devs.push_back(FAULT_ITEM(0, ((CLS_BUS*) key_dev.m_pData)->pid, "BUS", ((CLS_BUS*) key_dev.m_pData)->descr));
			if (lst_fault_devs.size() <= 0)
			{
				TRACE("lst_fault_devs.size is %d\r\n", lst_fault_devs.size());
				return FDIR_R_FAIL;
			}
			else
			{
				return FDIR_R_SUCCEED;
			}
		}
		else//母线失压，但是非母线故障，馈线故障导致母线失压
		{
			int lv_sec = dsbob->sec_tb->GetCount();
			int lv_nd = dsbob->nd_tb->GetCount();

			bool *bvisited_sec = new bool[lv_sec];
			bool *bvisited_nd = new bool[lv_nd];

			memset(bvisited_sec, false, sizeof(bool) * lv_sec);
			memset(bvisited_nd, false, sizeof(bool) * lv_nd);

			for (int i = 0; i < lv_cb; i++)
			{
				dsbob->cb_tb->GetDataByPos(i)->topflags |= FDIR_CB_NOAVAIL;
			}
			b_pass_fault_head = false;
			b_pass_fault_tail = false;
			b_pass_head_tail = false;
			bvisited_nd[((CLS_BUS *)key_dev.m_pData)->ind] = true;

			Find_fault_devs(((CLS_BUS *)key_dev.m_pData)->ind, bvisited_sec, bvisited_nd, true);
			if (faultbus < 0 || lst_fault_devs.size() <= 0)
			{
				TRACE("FAULTBUS is %d, lst_fault_devs.size is %d\r\n", faultbus, lst_fault_devs.size());
				return FDIR_R_FAIL;
			}
			else
			{
				return FDIR_R_SUCCEED;
			}
		}
	}
	else
	{
#if 1
//		rte_disconnect();
		CLS_CB* cb = dsbob->cb_tb->GetDataByPos(faultjxcb);
		int lv_cb = dsbob->cb_tb->GetCount();

		faultbus = (CLS_BUS*) (m_p_bus_Node->data.m_pData) - dsbob->bus_tb->GetTable();

		int lv_sec = dsbob->sec_tb->GetCount();
		int lv_nd = dsbob->nd_tb->GetCount();

		vector<bool> bVisited;
		bVisited.assign(lv_nd, false);
//		bool *bvisited_sec = new bool[lv_sec];
//		bool *bvisited_nd = new bool[lv_nd];
//
//		memset(bvisited_sec, false, sizeof(bool) * lv_sec);
//		memset(bvisited_nd, false, sizeof(bool) * lv_nd);

		//两端搜索，防止CB一端直接连接母线设备时map_node_downnode里没有内容
		Find_fault_devs(cb->ind, bVisited, FAULT_ITEM(0, cb->pid, "CB", cb->descr));
		TRACE("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");
		Find_fault_devs(cb->iznd, bVisited, FAULT_ITEM(0, cb->pid, "CB", cb->descr));

		lst_up_devs.clear();
		lst_fault_devs.clear();
		lst_down_devs.clear();

		for (map<FAULT_ITEM, list<FAULT_ITEM> >::iterator it = map_node_downnode.begin(); it != map_node_downnode.end(); it++)
		{
			it->second.unique();
			if (fault_head->data.GetType() != _FI ?
					it->first.eq_id == fault_head->data.GetPid() : it->first.id == fault_head->data.GetPid())
			{
				TRACE("====故障区段设备%ld[%s]=========\r\n", it->first.eq_id, it->first.eq_descr.c_str());
				for (list<FAULT_ITEM>::const_iterator it_list = it->second.begin(); it_list != it->second.end(); it_list++)
				{
					lst_fault_devs.push_back(*it_list);
				}
			}
			else
			{
				list<TreeNode<FDIR_TREE_NODE_DATA>*>::const_iterator it_tail;

				for (it_tail = fault_tail.begin(); it_tail != fault_tail.end(); it_tail++)
				{
					if (fault_head->data.GetType() != _FI ?
							it->first.eq_id == (*it_tail)->data.GetPid() : it->first.id == (*it_tail)->data.GetPid())
//					if (it->first.eq_id == (*it_tail)->data.GetPid())
						break;
				}

				if (it_tail == fault_tail.end())
				{
					TRACE("====故障上游设备%ld[%s]=========\r\n", it->first.eq_id, it->first.eq_descr.c_str());
					for (list<FAULT_ITEM>::const_iterator it_list = it->second.begin(); it_list != it->second.end(); it_list++)
					{
						lst_up_devs.push_back(*it_list);
					}
				}
				else
				{
					TRACE("====故障下游设备%ld[%s]=========\r\n", it->first.eq_id, it->first.eq_descr.c_str());
					for (list<FAULT_ITEM>::const_iterator it_list = it->second.begin(); it_list != it->second.end(); it_list++)
					{
						lst_down_devs.push_back(*it_list);
					}
				}
			}

			for (list<FAULT_ITEM>::const_iterator it_list = it->second.begin(); it_list != it->second.end(); it_list++)
			{
				TRACE("%ld[%s]\r\n", it_list->eq_id, it_list->eq_descr.c_str());
			}
		}
		return FDIR_R_SUCCEED;
#else
		if (fault_head->data.m_type == _CB || fault_head->data.m_type == _FI)
		{
			CLS_CB* cb = dsbob->cb_tb->GetDataByPos(faultjxcb);
			int lv_cb = dsbob->cb_tb->GetCount();

			for (int i = 0; i < lv_cb; i++)
			{
				dsbob->cb_tb->GetDataByPos(i)->topflags |= FDIR_CB_NOAVAIL;
			}
			faultbus = -1;

			int lv_sec = dsbob->sec_tb->GetCount();
			int lv_nd = dsbob->nd_tb->GetCount();

			bool *bvisited_sec = new bool[lv_sec];
			bool *bvisited_nd = new bool[lv_nd];

			memset(bvisited_sec, false, sizeof(bool) * lv_sec);
			memset(bvisited_nd, false, sizeof(bool) * lv_nd);

			cb->topflags &= ~FDIR_CB_NOAVAIL;
			bvisited_nd[cb->ind] = true;
			bvisited_nd[cb->iznd] = true;

			if (fault_head->data.m_type == _CB && (((CLS_CB *) fault_head->data.m_pData)->id == cb->id))
			{
				TRACE("故障起始设备为进线开关！\r\n");
				b_pass_fault_head = true;
				b_pass_fault_tail = false;
				b_pass_head_tail = false;

				Find_fault_devs(cb->ind, bvisited_sec, bvisited_nd, true);

				//ind端找到供电母线,由于b_pass_fault_head为true后找到的所有设备均会被存入故障设备列表
				if (faultbus >= 0)
				{
					lst_fault_devs.clear();
				}
				else
				{
					b_pass_fault_head = false;
				}
				b_pass_fault_tail = false;
				b_pass_head_tail = false;
				Find_fault_devs(cb->iznd, bvisited_sec, bvisited_nd, true);
				lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
			}
			else
			{
				TRACE("故障起始设备非进线开关！\r\n");
				b_pass_fault_head = false;
				b_pass_fault_tail = false;
				b_pass_head_tail = false;

				Find_fault_devs(cb->ind, bvisited_sec, bvisited_nd, true);

				b_pass_fault_head = false;
				b_pass_fault_tail = false;
				b_pass_head_tail = false;

				Find_fault_devs(cb->iznd, bvisited_sec, bvisited_nd, true);

				delete bvisited_sec;
				delete bvisited_nd;
			}
			if (faultbus < 0 || lst_fault_devs.size() <= 0)
			{
				TRACE("FAULTBUS is %d, lst_fault_devs.size is %d\r\n", faultbus, lst_fault_devs.size());
				return FDIR_R_FAIL;
			}
			else
			{
				return FDIR_R_SUCCEED;
			}
		}
		else if (fault_head->data.m_type == _LD)
		{

		}
		else
		{
			cout << "未知的故障起始设备" << endl;
			return FDIR_R_FAIL;
		}
#endif
	}
}

int FDIR_R::FaultDetection()
{
	int i;
	int sub,type;
	faultjxcb = -1;

	//get real cb and zone status
	int lv_cb = dsbob->cb_tb->GetCount();
	CLS_CB* cb = dsbob->cb_tb->GetTable();
	int lv_bus = dsbob->bus_tb->GetCount();
	CLS_BUS* bus = dsbob->bus_tb->GetTable();
	int lv_zn = dsbob->zn_tb->GetCount();
	CLS_ZN* zn = dsbob->zn_tb->GetTable();
	int lv_sec = dsbob->sec_tb->GetCount();
	CLS_SEC* sec = dsbob->sec_tb->GetTable();
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	int lv_ld = dsbob->ld_tb->GetCount();
	CLS_LD* ld = dsbob->ld_tb->GetTable();
	int lv_so = dsbob->so_tb->GetCount();
	CLS_SO* so = dsbob->so_tb->GetTable();
	int lv_xfmr = dsbob->xfmr_tb->GetCount();
	CLS_XFMR* xfmr = dsbob->xfmr_tb->GetTable();
	int lv_cp = dsbob->cp_tb->GetCount();
	CLS_CP* cp = dsbob->cp_tb->GetTable();

	//get fdir_cb,fdir_zone and fdir_zone_cb
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	//set fdir_cb status
	for(i = 0; i < lv_cb; i++)
	{
		if((cb + i)->q & CB_OPEN)// cuilizhong 保存开关在隔离之前的状态，恢复时处理受影响的区段
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

	//set fdir_zone status
	for(i = 0;i < lv_fdir_zone; i++)
	{
		sub = (fdir_zone + i)->sub;// 区段所包含设备在其设备表的物理号
		type = (fdir_zone + i)->type;
		if(type == _FDIR_BUS)
		{
			if((bus + sub)->topflags & BUS_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_ZN)
		{
			if((zn + sub)->topflags & ZN_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_XFMR)
		{
			if((xfmr + sub)->topflags & XFMR_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_CP)
		{
			if((cp + sub)->topflags & CP_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_LD)
		{
			if((ld + sub)->topflags & LD_DEAD)
			{
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags |= FDIR_ZONE_DEAD;
			}
			else
			{
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
				(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
			}
		}
		else if(type == _FDIR_SO)
		{
			(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD0;
			(fdir_zone + i)->topflags &= ~FDIR_ZONE_DEAD;
		}
		else
		{
			printf("区段类型有误!\n");
			return FDIR_R_FAIL;
		}
	}
	//set all cbs and zones to no fault and no flash
	for(i = 0;i < lv_cb;i++)
	{
		(cb + i)->topflags &= ~FDIR_CB_FLASH;
	}
	for(i = 0; i < lv_zn; i++)
	{
		(zn + i)->topflags &= ~ZN_FLASH;
//		(zn + i)->topflags &= ~ZN_FAULT;同一电源下不同故障时标记是否定位了故障,不能在这里清楚标记，多重故障时调用此函数调用，并使用到了这位的值
	}
	for(i = 0; i < lv_bus; i++)
	{
		(bus + i)->topflags &= ~BUS_FLASH;
		(bus + i)->topflags &= ~BUS_FAULT;
	}
	for(i = 0;i < lv_sec; i++)
	{
		(sec + i)->topflags &= ~SEC_FLASH;
		(sec + i)->topflags &= ~SEC_FAULT;
	}

	int j,izone;
	int cbptr,cbnum,icb;

	FDIR_TREE_NODE_DATA key_dev;
	map<long, FDIR_POINT_TRIG_SECTION> map_cb_faults;	//开关与关联的故障信号映射。key为开关ID， second为与开关关联的故障信号列表

	m_Tree.TreeRemoveSubTree(m_Tree.root);
	m_p_jxcb_Node = NULL;
	fault_head = NULL;
	fault_tail.clear();

	cout<<"开始进行故障定位"<<endl;
	//将故障信号标记到关联设备上，并填充key_dev
	if (SetFaultTopflags(g_lstSecTrig, key_dev, map_cb_faults) == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}

	//只保留启动信号相关的故障信号，后面定位失败时要将启动信号使用次数加1
	map<long, FDIR_POINT_TRIG_SECTION>::iterator it_faults;
	switch (key_dev.m_type)
	{
	case _LD:
		it_faults = map_cb_faults.find(((CLS_LD*) key_dev.m_pData)->id);
		break;

	case _CB:
		it_faults = map_cb_faults.find(((CLS_CB*) key_dev.m_pData)->id);
		break;

	case _BUS:
		it_faults = map_cb_faults.find(((CLS_BUS*) key_dev.m_pData)->id);
		break;

	case _FI:
		it_faults = map_cb_faults.find(((CLS_FI*) key_dev.m_pData)->id);
		break;

	default:
		it_faults = map_cb_faults.end();
		break;
	}

	if (it_faults == map_cb_faults.end())
	{
		TRACE("未找到启动信号设备关联的故障信号！\r\n");
		g_lstSecTrig.erase(++g_lstSecTrig.begin(), g_lstSecTrig.end());
	}
	else
	{
		g_lstSecTrig = it_faults->second;
	}

	TRACE("g_lstSecTrig.size() = %d\r\n", g_lstSecTrig.size());

	b_pass_fault_head = false;
	b_pass_fault_tail = false;
	b_pass_head_tail = false;
	//建立故障树
	cout<<"开始建立故障树"<<endl;
	int ret_bdtree = BuildTree(key_dev, m_Tree);
	if (ret_bdtree == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}
	//如果定位时找不到进行开关，则读取故障汇总表在此馈线上已经定位成功故障，并且时间间隔在30分钟内，把故障信号综合起来继续定位此故障
	else if (ret_bdtree == FDIR_R_NO_JXCB)
	{
		list<FAULT_ITEM> lst_bef_fi;
		if (ReadDvFiBef(key_dev, lst_bef_fi) == FDIR_R_SUCCEED)
		{
			SetBefFaultflags(lst_bef_fi);
			m_Tree.TreeRemoveSubTree(m_Tree.root);
			m_p_jxcb_Node = NULL;
			ret_bdtree = BuildTree(key_dev, m_Tree);
			if (ret_bdtree == FDIR_R_SUCCEED)
			{
			}
			else
			{
				return FDIR_R_FAIL;
			}
		}
		else
		{
	        return FDIR_R_FAIL;
		}
	}
	else if (ret_bdtree == FDIR_R_MUTI_JXCB)
	{
		 return FDIR_R_FAIL;
	}


	m_Tree.SaveTree("fault_tree.xml");
	//获取故障路径
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lstPath;
	cout<<"开始获取故障路径"<<endl;
	if (GetFaultPath(lstPath, key_dev) == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}

	lst_mis_point.clear();
	//将故障路径上漏报的故障报警信息添加到lst_mis_point列表中
	list<TreeNode<FDIR_TREE_NODE_DATA>*> ::iterator it_lstpath;
	for(it_lstpath = lstPath.begin();it_lstpath != lstPath.end();it_lstpath ++)
	{
		if ((*it_lstpath)->data.m_type == _FI)
		{
			CLS_FI* misfi = (CLS_FI*)(*it_lstpath)->data.m_pData;

			if(!(misfi->topflags & FDIR_CB_DETECT_FI))
			{
				lst_mis_point.push_back(misfi->pid);
			}
		}
		else if ((*it_lstpath)->data.m_type == _CB)
		{
			CLS_CB* misCb = (CLS_CB*)(*it_lstpath)->data.m_pData;

			if (!(misCb->topflags & FDIR_CB_DETECT_OPEN))
			{
				lst_mis_point.push_back(misCb->pid);
			}
		}
	}

	//获取故障类型
	GetFaultType(lstPath, fault_type, b_ld_fault);

	if (b_ld_fault)
	{
		CLS_LD *ld = (CLS_LD*) ((*lstPath.begin())->data.m_pData);

		faultzone = -1;
		TRACE("负荷故障，故障负荷为：%ld[%s]\r\n", ld->id, ld->descr);

		fault_head = *lstPath.begin();

		fault_tail.clear();

//		//负荷单点故障不需要分组
//		lst_edge_cb.clear();

		//获取负荷所在的区段
		int r_zn = ld->r_zn;

		if (r_zn >= 0)
		{
			int zn_id_ptr = dsbob->r_zn_ld_tb->GetDataByPos(r_zn)->zn_id_ptr;

			for (i = 0; i < lv_fdir_zone; i++)
			{
				if (fdir_zone_tb->GetDataByPos(i)->sub == zn_id_ptr)
				{
					TRACE("故障所在的区段号i=%d\r\n", i);
					faultzone = i;
					break;
				}
			}
		}
		else
		{
			TRACE("负荷%ld对应的r_zn为-1\r\n", ld->id);
		}
	}
	else
	{
		//获取故障区段号(在开关两端的区段中通过开关匹配的方式)
		faultzone = 0;
		list<string> lstfault, lstind, lstznd;

		fault_head = lstPath.front();

		//根据故障起始结点fault_head找出故障结束结点fault_tail

		if (fault_head->data.m_type == _BUS)
		{
          fault_tail.push_back(fault_head);
		}
		else
	        	GetDownDevs(fault_head, fault_tail);

	}

	//设置故障起始、结束位到相应设备的topflags上
	MaskFaultOnTree();

	cout<<"开始获取故障设备"<<endl;
	if(FindFaultBusAndFaultDevs(key_dev,faultjxcb) == FDIR_R_FAIL)
	{
		TRACE("获取故障母线和故障设备失败！");
		return FDIR_R_FAIL;
	}

	if (key_dev.m_type == _CB)
	{
		TRACE("\033[42m启动信号开关为%ld的新故障定位成功！故障区段:%d,进线开关:%d\033[0m\r\n", ((CLS_CB*)key_dev.m_pData)->id, faultzone, faultjxcb);
	}
	if (key_dev.m_type == _FI)
	{
		TRACE("\033[42m启动信号故障指示器为%ld的新故障定位成功！故障区段:%d,进线开关:%d\033[0m\r\n", ((CLS_FI*)key_dev.m_pData)->id, faultzone, faultjxcb);
	}
	else if (key_dev.m_type == _BUS)
	{
		fault_type = FAULT_TYPE_BUS;
		TRACE("\033[42m启动信号母线为%ld的新故障定位成功！故障区段:%d,进线开关:%d\033[0m\r\n", ((CLS_BUS*)key_dev.m_pData)->id,faultzone, faultjxcb);
	}

	else if (key_dev.m_type == _LD)
	{
		TRACE("\033[42m启动信号负荷为%ld的新故障定位成功！故障区段:%d,进线开关:%d\033[0m\r\n", ((CLS_LD*)key_dev.m_pData)->id, faultzone, faultjxcb);
	}

	//故障定位成功，填充本故障所使用的故障信号列表
	FillUsedFaultInfo(lstPath, g_lstSecTrig, map_cb_faults);

#if 0	//D5000暂时不判断不对称电流源
	//如果故障类型是接地故障，需要判断不对称电流源信号
	if (fault_fi_type == FAULT_JDFAULT)
	{
		CLS_BUS *jd_bus = dsbob->bus_tb->GetDataByLog(faultbus);
		if (jd_bus == NULL)
		{
			TRACE("没有找到故障馈线连接的母线,faultbus:%d\r\n", faultbus);
			return FDIR_R_FAIL;
		}
		printf("电源的id是： %s\r\n", (jd_bus->id));

		PSBOB::POINT psbob_point[2] = { 0 };

		if (oodbread_ra(&psbob_point, "classid", jd_bus->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), 2*sizeof(PSBOB::POINT)) > 0)
		{
			printf("电源的对应的不对称电流源是： %s， %s \r\n", psbob_point[0].id, psbob_point[1].id);
			if(psbob_point[0].value <= 0 && psbob_point[1].value <= 0)
			{
				printf("不对称电流源无故障信号！\r\n");
				return FDIR_R_FAIL;
			}
		}
		else
		{
			//有的电源没有不对称电源的遥信，不考虑不对称电流源
			printf("读取POINT表CLASSID为%s的记录失败，错误号:%ld\r\n",jd_bus->id, _oodb_errno);
		}
	}
#endif
	//合闸了事故总漏报的开关，重新形成ICB
	FormIcbOfZone();

	return FDIR_R_SUCCEED;
}

void FDIR_R::build_fi_tree_BFS(int ind, bool *bvisited, bool *bvisited_nd, TreeNode<FDIR_TREE_NODE_DATA>* father, Tree<FDIR_TREE_NODE_DATA> &tree, bool save_ld, list<TreeNode<FDIR_TREE_NODE_DATA>* > &lst_jxcb, bool b_build_tree, bool Isputjxcb)
{
	TreeNode<FDIR_TREE_NODE_DATA>* tmp_father = father;

	CLS_ND *pnd = dsbob->nd_tb->GetTable() + ind;

	map<long, list<long> >::iterator it_nd_bus = g_map_nd_bus.find(pnd->id);

	if (it_nd_bus != g_map_nd_bus.end() && it_nd_bus->second.size() > 0)
	{
		for (list<long>::iterator it_bus = it_nd_bus->second.begin(); it_bus != it_nd_bus->second.end(); it_bus++)
		{
			long id = *it_bus;
			pid_to_id(id);

			CLS_BUS *bus = dsbob->bus_tb->Find(CLS_BUS(id));
			if (bus != NULL)
			{
				//树上可能有多个进线开关，从根结点开始查找，有可能找到多条母线，只有未跨过
				//故障结束设备找到的母线才是供电母线
				if(!b_pass_fault_tail)
				{
					if (Isputjxcb)
					{
						faultbus = dsbob->bus_tb->FindPos(bus->id);
						TRACE("发现母线：%ld\r\n", bus->id);
					}
				}
				if (b_build_tree)
				{
					if (lst_jxcb.size() > 0)
					{
						tmp_father = tree.TreeAddLeaf(tmp_father, bus);
						if (Isputjxcb)
						{
							CLS_CB* p_jxcb =(CLS_CB*) lst_jxcb.back()->data.m_pData;
							map_jxcb_bus[lst_jxcb.back()] = tmp_father; //建立进线开关和母线的对应关系
							Isputjxcb = false;
							cout << "母线信息" << bus->id << endl;
							cout << "进线开关信息" << p_jxcb->id << endl;
						}
					}
					else
					{

					}
				}
				else
				{
					if (b_pass_fault_head)
					{
						lst_fault_devs.push_back(FAULT_ITEM(0, bus->pid, "BUS", bus->descr));
						TRACE("故障区域母线设备为%ld[%s]\r\n", bus->id, bus->descr);
					}

				}
			}
			//分线箱母线
			else
			{
				if (b_pass_fault_head)
				{
					PSBOB::BUS psbob_bus = { 0 };
					if (oodbread_rk(&psbob_bus, &(*it_bus), const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(PSBOB::BUS)) == OO_FAIL)
					{
						printf("%s(%d):读取PSBOB BUS表关键字为%ld的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, *it_bus, _oodb_errno);
						continue;
					}
					lst_fault_devs.push_back(FAULT_ITEM(0, psbob_bus.id, "BUS", psbob_bus.describe));
					TRACE("故障区域找到设备分线箱母线%ld[%s]\r\n", psbob_bus.id, psbob_bus.describe);
				}

			}
		}

	}

	if (pnd->r_cb != -1)
	{
		ChildPtr *cb_ptr = dsbob->r_nd_cb_tb->FindChildPtr(pnd->id);
		ChildPtr *tmp_cb_ptr = cb_ptr;
		CLS_CB *cb = NULL;
		while (tmp_cb_ptr != NULL)
		{
			cb = dsbob->cb_tb->GetDataByPos(tmp_cb_ptr->Ptr);

			if (!(cb->topflags & FDIR_CB_NOAVAIL))
			{
			}
			else
			{
				if (!b_build_tree)
				{
					if (b_pass_head_tail)
					{
						return;
					}
				}

				//cout << "处理： " << cb->id << endl;
				TRACE("处理开关%ld[%s]\r\n", cb->id, cb->descr);
				cb->topflags &= ~FDIR_CB_NOAVAIL;

				if(cb->topflags & FDIR_FAULT_TAIL)
				{
					TRACE("发现故障未端开关%ld[%s]\r\n", cb->id, cb->descr);
					b_pass_fault_tail = true;

					if (!b_build_tree)
					{
						lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
						TRACE("故障区域开关设备开关%ld[%s]\r\n", cb->id, cb->descr);
						tmp_cb_ptr = tmp_cb_ptr->Next;
						continue;
					}
				}

				if(cb->topflags & FDIR_FAULT_HEAD)
				{
					b_pass_fault_head = true;
					TRACE("发现故障起始开关%ld[%s]\r\n", cb->id, cb->descr);
				}

				if(b_build_tree)
				{
					father = tree.TreeAddLeaf(tmp_father, cb);			
					//添加当前故障树根结点，防止递归调用返回时故障树生成错误，该错误可能导致故障树生成失败，下同
					if (cur_father == NULL && father != NULL)
					{
						cur_father = father;
					}
				}					
				else
				{
					if(b_pass_fault_head)
					{
						lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
						TRACE("故障区域开关设备开关%ld[%s]\r\n", cb->id, cb->descr);
					}
				}

				if (cb->q & CB_FDHCB)
				{
					if(Isputjxcb)
					{
						TRACE("！！！！！！！！！发现进线开关%ld[%s]\r\n", cb->id, cb->descr);
						lst_jxcb.push_back(father);
					}
					else
					{

					}

				}
				//碰到断开的非进线开关或联络开关即终止本条路径的搜索
			    if (!(cb->q & CB_FDHCB) && (cb->q & CB_OPEN) && !(cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI)))
				{

				}
				else
				{
					if (cb->ind == ind)
					{
						if (!bvisited_nd[cb->iznd])
						{
							bvisited_nd[cb->iznd] = true;
							build_fi_tree_BFS(cb->iznd, bvisited, bvisited_nd, father, tree, save_ld, lst_jxcb, b_build_tree, Isputjxcb);
						}

					}
					else
					{
						if (!bvisited_nd[cb->ind])
						{
							bvisited_nd[cb->ind] = true;
							build_fi_tree_BFS(cb->ind, bvisited, bvisited_nd, father, tree, save_ld, lst_jxcb, b_build_tree, Isputjxcb);
						}
					}
				}

				if(cb->topflags & FDIR_FAULT_HEAD)
				{
					b_pass_head_tail = true;
					return;
				}
			}

			tmp_cb_ptr = tmp_cb_ptr->Next;
		}

		FreeChildPtr(cb_ptr);
	}

	//刀闸设备
	if(pnd->r_switch >= 0)
	{
		ChildPtr *switch_ptr = dsbob->r_nd_switch_tb->FindChildPtr(pnd->id);
		ChildPtr *tmp_switch_ptr = switch_ptr;
		CLS_SWITCH* swh = NULL;
		while (tmp_switch_ptr != NULL)
		{
			if (!b_build_tree)
			{
				if (b_pass_head_tail)
				{
					return;
				}
			}

			swh = dsbob->switch_tb->GetDataByPos(tmp_switch_ptr->Ptr);

			if (!(swh->topflags & FDIR_CB_NOAVAIL) || (swh->q & CB_OPEN))
			{
			}
			else
			{
				TRACE("处理刀闸%ld[%s]\r\n", swh->id, swh->descr);

				if (!b_build_tree)
				{
					if (b_pass_fault_head)
					{
						TRACE("故障区域刀闸设备%ld[%s]\r\n", swh->id, swh->descr);
						lst_fault_devs.push_back(FAULT_ITEM(0, swh->pid, "SWITCH", swh->descr));
					}
				}

				swh->topflags &= ~FDIR_CB_NOAVAIL;

				if (swh->ind == ind)
				{
					if (!bvisited_nd[swh->iznd])
					{
						bvisited_nd[swh->iznd] = true;
						build_fi_tree_BFS(swh->iznd, bvisited, bvisited_nd, father, tree, save_ld, lst_jxcb, b_build_tree, Isputjxcb);
					}
				}
				else
				{
					if (!bvisited_nd[swh->ind])
					{
						bvisited_nd[swh->ind] = true;
						build_fi_tree_BFS(swh->ind, bvisited, bvisited_nd, father, tree, save_ld, lst_jxcb, b_build_tree, Isputjxcb);
					}
				}
			}

			tmp_switch_ptr = tmp_switch_ptr->Next;
		}

		FreeChildPtr(switch_ptr);
	}

	//线段上的故障指示器
	if (pnd->r_sec != -1)
	{
		ChildPtr *sec_ptr = dsbob->r_nd_sec_tb->FindChildPtr(pnd->id);
		ChildPtr *tmp_sec_ptr = sec_ptr;
		CLS_SEC* sec = NULL;
		while (tmp_sec_ptr != NULL)
		{
			if (!b_build_tree)
			{
				if (b_pass_head_tail)
				{
					return;
				}
			}

			sec = dsbob->sec_tb->GetDataByPos(tmp_sec_ptr->Ptr);

			if (!bvisited[tmp_sec_ptr->Ptr])
			{
				father = tmp_father;
				
				bvisited[tmp_sec_ptr->Ptr] = true;
				TRACE("处理线段%ld[%s]\r\n", sec->id, sec->descr);

				//故障开始结点是否是此线段，如果是此线段，结点连接的其他设备不需要在查找故障设备
				bool first_head  = false;
				bool is_tail = false;
				if (sec->r_fi == -1)
				{
					if (!b_build_tree)
					{
						if(b_pass_fault_head)
						{
							TRACE("故障区域线段设备%ld[%s]\r\n", sec->id, sec->descr);
							lst_fault_devs.push_back(FAULT_ITEM(0, sec->pid, "SEC", sec->descr));
						}
					}
				}
				else
				{
					//判断当前线段上所有故障指示器的状态
					list<CLS_FI*> lst_fi;
					ChildPtr *ptr2 = dsbob->r_sec_fi_tb->FindChildPtr(sec->id);

					ChildPtr *tmp_ptr2 = ptr2;

					lst_fi.clear();

					while (tmp_ptr2 != NULL)
					{
						CLS_FI *fi = dsbob->fi_tb->GetDataByPos(tmp_ptr2->Ptr);

						for (list<CLS_FI*>::iterator it = lst_fi.begin();; it++)
						{
							//电流从fi开始编号的ND流入，lst_fi中的故障指示器按位置从小到大排序
							if (pnd->id == fi->nd)
							{
								if (it == lst_fi.end() || fi->pos < (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
							//电流从fi的ZND流入，lst_fi中的故障指示器按位置从大到小排序
							else
							{
								if (it == lst_fi.end() || fi->pos > (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
						}
						tmp_ptr2 = tmp_ptr2->Next;
					}

					for (list<CLS_FI*>::iterator it = lst_fi.begin(); it != lst_fi.end(); it++)
					{
						if ((*it)->topflags & FDIR_FAULT_TAIL)
						{
							b_pass_fault_tail = true;

							TRACE("发现故障未端故障指示器%ld[%s]\r\n", (*it)->id, (*it)->descr);

							if (!b_build_tree)
							{
								is_tail = true;
								TRACE("故障区域线段设备开关%ld[%s]\r\n", sec->id, sec->descr);
								lst_fault_devs.push_back(FAULT_ITEM((*it)->pid, sec->pid, "FI", (*it)->descr));
								lst_fault_devs.back().whole = 0;
								break;
							}
						}

						if ((*it)->topflags & FDIR_FAULT_HEAD)
						{
							b_pass_fault_head = true;
							TRACE("发现故障起始故障指示器%ld[%s]\r\n", (*it)->id, (*it)->descr);
							first_head = true;
						}

						if(b_build_tree)
						{
							father = tree.TreeAddLeaf(father, *it);
							if (cur_father == NULL && father != NULL)
							{
								cur_father = father;
							}
						}							
						else
						{
							if(b_pass_fault_head)
							{
								TRACE("故障区域线段设备开关%ld[%s]\r\n", sec->id, sec->descr);
								lst_fault_devs.push_back(FAULT_ITEM((*it)->pid, sec->pid, "FI", (*it)->descr));
								lst_fault_devs.back().whole = 0;
							}
						}
					}
				}

				if (is_tail && first_head)
				{
					b_pass_head_tail = true;
					return;
				}

				//如果找到故障结束的故障指示器，停止往下搜索，遍历此结点连接的下一条线段
				if (is_tail)
				{
					tmp_sec_ptr = tmp_sec_ptr->Next;
					continue;
				}
				if (sec->ind == ind)
				{
					if (!bvisited_nd[sec->iznd])
					{
						bvisited_nd[sec->iznd] = true;
						build_fi_tree_BFS(sec->iznd, bvisited, bvisited_nd, father, tree, save_ld, lst_jxcb, b_build_tree, Isputjxcb);
					}
				}
				else
				{
					if (!bvisited_nd[sec->ind])
					{
						bvisited_nd[sec->ind] = true;
						build_fi_tree_BFS(sec->ind, bvisited, bvisited_nd, father, tree, save_ld, lst_jxcb, b_build_tree, Isputjxcb);
					}
				}

				if (first_head)
				{
					b_pass_head_tail = true;
					return;
				}

			}
			tmp_sec_ptr = tmp_sec_ptr->Next;
		}

		FreeChildPtr(sec_ptr);
	}
	if (b_pass_head_tail)
	{
		return;
	}
	if (save_ld)
	{
		map<long, list<CLS_LD> >::iterator it = dsbob->m_map_nd_ld.find(pnd->id);

		if (it != dsbob->m_map_nd_ld.end())
		{
			for (list<CLS_LD>::iterator it_ld = it->second.begin(); it_ld != it->second.end(); it_ld++)
			{
				if(b_build_tree)
				{
					CLS_LD *temp_ld = dsbob->ld_tb->Find(*it_ld);
					if (temp_ld != NULL)
					{
						//如果是以负荷为根节点建立树，此负荷会再次被搜索到，所以要判断负荷是否是根节点，如果是就跳过，避免结点重复
						if (tmp_father != NULL && tmp_father->data.m_type == _LD)
						{
							if (((CLS_LD *)tmp_father->data.m_pData)->id == temp_ld->id)
							{
								continue;
							}

							//当父节点为空且要在故障树上要添加负荷时，以之前储存的节点作为根节点，防止递归算法返回到最底层时，因父节点为空，
							//添加负荷后，父节点就为负荷，这样会导致后面找不到故障路径从而使定位失败，下同
							if (tmp_father == NULL && tree.root != NULL)
							{
								tmp_father = cur_father;
							}

							father = tree.TreeAddLeaf(tmp_father, temp_ld);
						}
						else
						{
							if (tmp_father == NULL && tree.root != NULL)
							{
								tmp_father = cur_father;
							}

							father = tree.TreeAddLeaf(tmp_father, temp_ld);
						}
						//TRACE("找到负荷：%s\r\n", temp_ld->id);
					}

				}
				else
				{
					CLS_LD *temp_ld = dsbob->ld_tb->Find(*it_ld);
					if(temp_ld->topflags & FDIR_FAULT_HEAD)
					{
						b_pass_fault_head = true;
						b_pass_head_tail = true;
						lst_fault_devs.push_back(FAULT_ITEM(0, it_ld->pid, "LD", it_ld->descr));
						TRACE("故障区域负荷设备%ld[%s]\r\n", it_ld->id, it_ld->descr);
						return;
					}

					if (b_pass_fault_head)
					{
						lst_fault_devs.push_back(FAULT_ITEM(0, it_ld->pid, "LD", it_ld->descr));
						TRACE("故障区域负荷设备%ld[%s]\r\n", it_ld->id, it_ld->descr);
					}
				}
			}
		}
	}

	////D5000补故障区域的刀闸
//	if (!b_build_tree && b_pass_fault_head)
//	{
//		std::deque<Element_Info> dq_elems;
//		std::deque<Element_Info>::iterator it_dq;
//		map<string, list<string> >::iterator it;
//
//		//如果是跳过刀闸的结点，通过此结点把刀闸补齐
//		if (Switch_Manager::instance()->find_switch_by_shared_point(pnd->id, dq_elems))
//		{
//
//			PSBOB::CB switch_cb = { 0 };
//			for (it_dq = dq_elems.begin(); it_dq != dq_elems.end(); it_dq++)
//			{
//				if (oodbread_rk(&switch_cb, (void *) it_dq->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("cb")), sizeof(PSBOB::CB)) < 0)
//				{
//					printf("oodbread_rk(%s) failed, errno:%d, line:%d\r\n", it_dq->id, _oodb_errno, __LINE__);
//				}
//
//				lst_fault_devs.push_back(FAULT_ITEM("", switch_cb.pointid[0] == 0 ? it_dq->id : switch_cb.pointid, "SWITCH", switch_cb.descr));
//				TRACE("故障区域负荷设备刀闸%s[%s]\r\n", switch_cb.pointid[0] == 0 ? it_dq->id : switch_cb.pointid, switch_cb.descr);
//			}
//		}
//	}

}

void FDIR_R::Find_fault_devs(int ind, vector<bool> &bVisited_nd, FAULT_ITEM parent)
{
	if (bVisited_nd[ind])
		return;

	CLS_ND *pnd = dsbob->nd_tb->GetDataByLog(ind);
	bVisited_nd[ind] = true;
	int next_nd;

	map<long, list<long> >::iterator it_nd_bus = g_map_nd_bus.find(pnd->id);
	if (it_nd_bus != g_map_nd_bus.end() && it_nd_bus->second.size() > 0)
	{
		for (list<long>::iterator it_bus = it_nd_bus->second.begin(); it_bus != it_nd_bus->second.end(); it_bus++)
		{
			long id = *it_bus;
			pid_to_id(id);

			CLS_BUS *bus = dsbob->bus_tb->Find(CLS_BUS(id));

			if (bus != NULL)
			{
				//解决进线开关下游故障描述不正确的问题
				if (map_node_downnode.size() <= 0)
				{
					bVisited_nd[ind] = false;
				}
				return;
			}
			//分线箱母线
			else
			{
				PSBOB::BUS psbob_bus = { 0 };

				id_to_pid(id);

				if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), (char*) &id, sizeof(PSBOB::BUS), (char*) &psbob_bus) > 0)
				{
					map_node_downnode[parent].push_back(FAULT_ITEM(0, id, "BUS", psbob_bus.name));
				}
				else
				{
					TRACE("读取ID为%ld的母线记录失败！\r\n", id);
				}
			}
		}
	}

	if (pnd->r_sec >= 0)
	{
		ChildPtr *ptr = dsbob->r_nd_sec_tb->FindChildPtr(pnd->id);
		ChildPtr *ptr2 = ptr;
		
		while (ptr != NULL)
		{
			FAULT_ITEM cur_parent = parent;

			CLS_SEC * sec = dsbob->sec_tb->GetDataByLog(ptr->Ptr);

			if (sec->ind == ind)
			{
				next_nd = sec->iznd;
			}
			else
			{
				next_nd = sec->ind;
			}

			if (!bVisited_nd[next_nd])
			{
				FAULT_ITEM item(0, sec->pid, "SEC", sec->descr);

				if (sec->r_fi >= 0)
				{
					//判断当前线段上所有故障指示器的状态
					list<CLS_FI*> lst_fi;
					ChildPtr *tmp_ptr = dsbob->r_sec_fi_tb->FindChildPtr(sec->id);
					ChildPtr *tmp_ptr2 = tmp_ptr;

					lst_fi.clear();
					while (tmp_ptr != NULL)
					{
						CLS_FI *fi = dsbob->fi_tb->GetDataByPos(tmp_ptr->Ptr);

						for (list<CLS_FI*>::iterator it = lst_fi.begin();; it++)
						{
							//电流从fi开始编号的ND流入，lst_fi中的故障指示器按位置从小到大排序
							if (pnd->id == fi->nd)
							{
								if (it == lst_fi.end() || fi->pos < (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
							//电流从fi的ZND流入，lst_fi中的故障指示器按位置从大到小排序
							else
							{
								if (it == lst_fi.end() || fi->pos > (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
						}

						tmp_ptr = tmp_ptr->Next;
					}

					FreeChildPtr(tmp_ptr2);

					for (list<CLS_FI*>::iterator it = lst_fi.begin(); it != lst_fi.end(); it++)
					{
						FAULT_ITEM item_fi((*it)->pid, sec->pid, "FI", (*it)->descr);

						if (((*it)->topflags & FDIR_FAULT_TAIL) || ((*it)->topflags & FDIR_FAULT_HEAD))
						{
							map_node_downnode[parent].push_back(item_fi);

							cur_parent = item_fi;
							map_node_downnode[cur_parent].push_back(cur_parent);

							//杨勇 20150330 解决故障指示器故障中故障指示器所属线段着色异常问题
							map_node_downnode[cur_parent].push_back(item);
							map_node_downnode[cur_parent].back().whole = 0;
						}
						else
						{
							map_node_downnode[cur_parent].push_back(item_fi);
						}
					}
				}
				else
				{
					map_node_downnode[cur_parent].push_back(item);
				}

				Find_fault_devs(next_nd, bVisited_nd, cur_parent);
			}

			ptr = ptr->Next;
		}

		FreeChildPtr(ptr2);
	}

	if (pnd->r_cb >= 0)
	{
		ChildPtr *ptr = dsbob->r_nd_cb_tb->FindChildPtr(pnd->id);
		ChildPtr *ptr2 = ptr;

		while (ptr != NULL)
		{
			//杨勇 20150130 解决主干线故障区域包含负荷的问题
			FAULT_ITEM cur_parent = parent;

			CLS_CB *cb = dsbob->cb_tb->GetDataByLog(ptr->Ptr);

			FAULT_ITEM item(0, cb->pid, "CB", cb->descr);

			if (ind == cb->ind)
			{
				next_nd = cb->iznd;
			}
			else
			{
				next_nd = cb->ind;
			}

			if (!bVisited_nd[next_nd])
			{
				map_node_downnode[parent].push_back(item);

				if ((cb->topflags & FDIR_FAULT_HEAD) || (cb->topflags & FDIR_FAULT_TAIL))
				{
					cur_parent = item;
					map_node_downnode[cur_parent].push_back(item);
				}

				if (cb->q & CB_FDHCB)
				{
					TRACE("%ld[%s]为进线开关，设置成边界开关。\r\n", map_node_downnode[parent].back().eq_id, map_node_downnode[parent].back().eq_descr.c_str());
					map_node_downnode[parent].back().border = true;
				}

				if ((cb->q & CB_OPEN) && !(cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI)))
				{
					TRACE("%ld[%s]开关，设置成边界开关。\r\n", map_node_downnode[parent].back().eq_id, map_node_downnode[parent].back().eq_descr.c_str());
					map_node_downnode[parent].back().border = true;
				}
				else
				{
					Find_fault_devs(next_nd, bVisited_nd, cur_parent);
				}
			}

			ptr = ptr->Next;
		}

		FreeChildPtr(ptr2);
	}

	if (pnd->r_switch >= 0)
	{
		ChildPtr *ptr = dsbob->r_nd_switch_tb->FindChildPtr(pnd->id);
		ChildPtr *ptr2 = ptr;

		while (ptr != NULL)
		{
			FAULT_ITEM cur_parent = parent;

			CLS_SWITCH *swh = dsbob->switch_tb->GetDataByLog(ptr->Ptr);

			if (ind == swh->ind)
			{
				next_nd = swh->iznd;
			}
			else
			{
				next_nd = swh->ind;
			}

			map_node_downnode[cur_parent].push_back(FAULT_ITEM(0, swh->pid, "SWITCH", swh->descr));

			if (!bVisited_nd[next_nd])
			{
				Find_fault_devs(next_nd, bVisited_nd, cur_parent);
			}

			ptr = ptr->Next;
		}

		FreeChildPtr(ptr2);
	}

	if (pnd->r_ld >= 0)
	{
		ChildPtr *ptr = dsbob->r_nd_ld_tb->FindChildPtr(pnd->id);
		ChildPtr *ptr2 = ptr;

		while (ptr != NULL)
		{
			CLS_LD *ld = dsbob->ld_tb->GetDataByLog(ptr->Ptr);

			map_node_downnode[parent].push_back(FAULT_ITEM(0, ld->pid, "LD", ld->descr));

			ptr = ptr->Next;
		}

		FreeChildPtr(ptr2);
	}
}

/////////////////// 搜索故障设备
void FDIR_R::Find_fault_devs(int ind, bool *bvisited, bool *bvisited_nd, bool save_ld)
{

	CLS_ND *pnd = dsbob->nd_tb->GetTable() + ind; //  该节点的指针

	map<long, list<long> >::iterator it_nd_bus = g_map_nd_bus.find(pnd->id);

	if (it_nd_bus != g_map_nd_bus.end() && it_nd_bus->second.size() > 0)
	{
		for (list<long>::iterator it_bus = it_nd_bus->second.begin(); it_bus != it_nd_bus->second.end(); it_bus++)
		{
			long id = *it_bus;
			pid_to_id(id);

			CLS_BUS *bus = dsbob->bus_tb->Find(CLS_BUS(id));

			if (bus != NULL)
			{
				//树上可能有多个进线开关，从根结点开始查找，有可能找到多条母线，只有未跨过故障结束设备找到的母线才是供电母线
				if (!b_pass_fault_tail)
				{
					faultbus = dsbob->bus_tb->FindPos(bus->id);
					TRACE("发现母线：%ld\r\n", bus->id);
				}
			}
			//分线箱母线
			else
			{
				if (b_pass_fault_head)
				{
					PSBOB::BUS psbob_bus = { 0 };
					if (oodbread_rk(&psbob_bus, &(*it_bus), const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(PSBOB::BUS)) == OO_FAIL)
					{
						printf("%s(%d):读取PSBOB BUS表关键字为%s的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, *it_bus, _oodb_errno);
						continue;
					}
					lst_fault_devs.push_back(FAULT_ITEM(0, psbob_bus.id, "BUS", psbob_bus.describe));
					TRACE("故障区域找到设备分线箱母线%ld[%s]\r\n", psbob_bus.id, psbob_bus.describe);
				}

			}
		}

	}

	if (pnd->r_cb != -1)
	{
		ChildPtr *cb_ptr = dsbob->r_nd_cb_tb->FindChildPtr(pnd->id);
		ChildPtr *tmp_cb_ptr = cb_ptr;
		CLS_CB *cb = NULL;
		while (tmp_cb_ptr != NULL)
		{
			cb = dsbob->cb_tb->GetDataByPos(tmp_cb_ptr->Ptr);

			if (!(cb->topflags & FDIR_CB_NOAVAIL))
			{
			}
			else
			{
				if (b_pass_head_tail)
				{
					return;
				}
				cout << "处理： " << cb->id << endl;
				cb->topflags &= ~FDIR_CB_NOAVAIL;

				if (cb->topflags & FDIR_FAULT_TAIL)
				{
					TRACE("发现故障未端开关%ld[%s]\r\n", cb->id, cb->descr);
					b_pass_fault_tail = true;
					lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
					TRACE("故障区域开关设备开关%ld[%s]\r\n", cb->id, cb->descr);
					tmp_cb_ptr = tmp_cb_ptr->Next;
					continue;
				}

				if (cb->topflags & FDIR_FAULT_HEAD)
				{
					b_pass_fault_head = true;
					TRACE("发现故障起始开关%ld[%s]\r\n", cb->id, cb->descr);
				}

				if (b_pass_fault_head)
				{
					lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
					TRACE("故障区域开关设备开关%ld[%s]\r\n", cb->id, cb->descr);
				}

				//碰到断开的开关或联络开关即终止本条路径的搜索
				if ((cb->q & CB_OPEN) && !(cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI)))
				{
				}
				else
				{
					if (cb->ind == ind)
					{
						if (!bvisited_nd[cb->iznd])
						{
							bvisited_nd[cb->iznd] = true;
							Find_fault_devs(cb->iznd, bvisited, bvisited_nd, save_ld);
						}

					}
					else
					{
						if (!bvisited_nd[cb->ind])
						{
							bvisited_nd[cb->ind] = true;
							Find_fault_devs(cb->ind, bvisited, bvisited_nd, save_ld);
						}
					}
				}

				if (cb->topflags & FDIR_FAULT_HEAD)
				{
					b_pass_head_tail = true;
					return;
				}
			}

			tmp_cb_ptr = tmp_cb_ptr->Next;
		}

		FreeChildPtr(cb_ptr);
	}

	//线段上的故障指示器
	if (pnd->r_sec != -1)
	{
		ChildPtr *sec_ptr = dsbob->r_nd_sec_tb->FindChildPtr(pnd->id);
		ChildPtr *tmp_sec_ptr = sec_ptr;
		CLS_SEC* sec = NULL;
		while (tmp_sec_ptr != NULL)
		{
			if (b_pass_head_tail)
			{
				return;
			}

			sec = dsbob->sec_tb->GetDataByPos(tmp_sec_ptr->Ptr);

			if (!bvisited[tmp_sec_ptr->Ptr])
			{
				bvisited[tmp_sec_ptr->Ptr] = true;

				//故障开始结点是否是此线段，如果是此线段，结点连接的其他设备不需要再查找故障设备
				bool first_head = false;
				bool is_tail = false;
				if (sec->r_fi == -1)
				{
					if (b_pass_fault_head)
					{
						TRACE("故障区域线段设备开关%ld[%s]\r\n", sec->id, sec->descr);
						lst_fault_devs.push_back(FAULT_ITEM(0, sec->pid, "SEC", sec->descr));
					}
				}
				else
				{
					//判断当前线段上所有故障指示器的状态
					list<CLS_FI*> lst_fi;
					ChildPtr *ptr2 = dsbob->r_sec_fi_tb->FindChildPtr(sec->id);
					ChildPtr *tmp_ptr2 = ptr2;

					lst_fi.clear();
					while (tmp_ptr2 != NULL)
					{
						CLS_FI *fi = dsbob->fi_tb->GetDataByPos(tmp_ptr2->Ptr);

						for (list<CLS_FI*>::iterator it = lst_fi.begin();; it++)
						{
							//电流从fi开始编号的ND流入，lst_fi中的故障指示器按位置从小到大排序
							if (pnd->id == fi->nd)
							{
								if (it == lst_fi.end() || fi->pos < (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
							//电流从fi的ZND流入，lst_fi中的故障指示器按位置从大到小排序
							else
							{
								if (it == lst_fi.end() || fi->pos > (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
						}
						tmp_ptr2 = tmp_ptr2->Next;
					}

					for (list<CLS_FI*>::iterator it = lst_fi.begin(); it != lst_fi.end(); it++)
					{
						if ((*it)->topflags & FDIR_FAULT_TAIL)
						{
							b_pass_fault_tail = true;
							TRACE("发现故障未端故障指示器%ld[%s]\r\n", (*it)->id, (*it)->descr);
							is_tail = true;
							TRACE("故障区域线段设备开关%ld[%s]\r\n", sec->id, sec->descr);
							lst_fault_devs.push_back(FAULT_ITEM((*it)->pid, sec->pid, "FI", (*it)->descr));
							lst_fault_devs.back().whole = 0;
							break;
						}

						if ((*it)->topflags & FDIR_FAULT_HEAD)
						{
							b_pass_fault_head = true;
							TRACE("发现故障起始故障指示器%ld[%s]\r\n", (*it)->id, (*it)->descr);
							first_head = true;
						}

						if (b_pass_fault_head)
						{
							TRACE("故障区域线段设备开关%ld[%s]\r\n", sec->id, sec->descr);
							lst_fault_devs.push_back(FAULT_ITEM((*it)->pid, sec->pid, "FI", (*it)->descr));
							lst_fault_devs.back().whole = 0;
						}

					}
				}

				//如果找到故障结束的故障指示器，停止往下搜索，遍历此结点连接的下一条线段
				if (is_tail)
				{
					tmp_sec_ptr = tmp_sec_ptr->Next;
					continue;
				}
				if (sec->ind == ind)
				{
					if (!bvisited_nd[sec->iznd])
					{
						bvisited_nd[sec->iznd] = true;
						Find_fault_devs(sec->iznd, bvisited, bvisited_nd, save_ld);
					}
				}
				else
				{
					if (!bvisited_nd[sec->ind])
					{
						bvisited_nd[sec->ind] = true;
						Find_fault_devs(sec->ind, bvisited, bvisited_nd, save_ld);
					}
				}

				if (first_head)
				{
					b_pass_head_tail = true;
					return;
				}

			}
			tmp_sec_ptr = tmp_sec_ptr->Next;
		}

		FreeChildPtr(sec_ptr);
	}
	if (b_pass_head_tail)
	{
		return;
	}
	if (save_ld)
	{
		map<long, list<CLS_LD> >::iterator it = dsbob->m_map_nd_ld.find(pnd->id);

		if (it != dsbob->m_map_nd_ld.end())
		{
			for (list<CLS_LD>::iterator it_ld = it->second.begin(); it_ld != it->second.end(); it_ld++)
			{
				CLS_LD *temp_ld = dsbob->ld_tb->Find(*it_ld);
				if (temp_ld->topflags & FDIR_FAULT_HEAD)
				{
					b_pass_fault_head = true;
					b_pass_head_tail = true;
					lst_fault_devs.push_back(FAULT_ITEM(0, it_ld->pid, "LD", it_ld->descr));
					TRACE("故障区域负荷设备%ld[%s]\r\n", it_ld->id, it_ld->descr);
					return;
				}
				if (b_pass_fault_head)
				{
					lst_fault_devs.push_back(FAULT_ITEM(0, it_ld->pid, "LD", it_ld->descr));
					TRACE("故障区域负荷设备%ld[%s]\r\n", it_ld->id, it_ld->descr);
				}
			}
		}
	}

	////补故障区域的刀闸
//	if (b_pass_fault_head)
//	{
//		std::deque<Element_Info> dq_elems;
//		std::deque<Element_Info>::iterator it_dq;
//		map<string, list<string> >::iterator it;
//
//		//如果是跳过刀闸的结点，通过此结点把刀闸补齐
//		if (Switch_Manager::instance()->find_switch_by_shared_point(pnd->id, dq_elems))
//		{
//
//			PSBOB::CB switch_cb = { 0 };
//			for (it_dq = dq_elems.begin(); it_dq != dq_elems.end(); it_dq++)
//			{
//				if (oodbread_rk(&switch_cb, (void *) it_dq->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("cb")), sizeof(PSBOB::CB)) < 0)
//				{
//					printf("oodbread_rk(%s) failed, errno:%d, line:%d\r\n", it_dq->id, _oodb_errno, __LINE__);
//				}
//
//				lst_fault_devs.push_back(FAULT_ITEM("", switch_cb.pointid[0] == 0 ? it_dq->id : switch_cb.pointid, "SWITCH", switch_cb.descr));
//				TRACE("故障区域负荷设备刀闸%s[%s]\r\n", switch_cb.pointid[0] == 0 ? it_dq->id : switch_cb.pointid, switch_cb.descr);
//			}
//		}
//	}

}


void FDIR_R::BuildTree(bool *bvisited, bool *bvisited_zn, int pos, TreeNode<FDIR_TREE_NODE_DATA> *pParent, Tree<FDIR_TREE_NODE_DATA> *pTree)
{
	if (pos < 0 || bvisited_zn[pos])
		return;
	bvisited_zn[pos] = true;

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetDataByPos(pos);

	//在树上添加开关结点
	int cbptr = fdir_zone->cbptr;
	int cbnum = fdir_zone->cbnum;

	for (int i = 0; i < cbnum; i++)
	{
		int icb = fdir_zone_cb_tb->GetDataByPos(cbptr + i)->icb;

		int izone = fdir_cb_tb->GetDataByPos(icb)->ind;
		int zzone = fdir_cb_tb->GetDataByPos(icb)->iznd;

		if (icb < 0 || bvisited[icb])
			continue;

		bvisited[icb] = true;

		CLS_CB *cb = dsbob->cb_tb->GetDataByPos(icb);

		TreeNode<FDIR_TREE_NODE_DATA> *p = pTree->TreeAddLeaf(pParent, cb);

		TRACE("开关%ld[%s]的上一级开关为%ld\r\n", cb->id, cb->descr, pParent == NULL ? 0 : ((CLS_CB*)pParent->data.m_pData)->id);

		if (cb->q & CB_FDHCB)
		{
			TRACE("发现进线开关%ld[%s]\r\n", cb->id, cb->descr);
			faultjxcb = dsbob->cb_tb->FindPos(CLS_CB(cb->id));
			m_p_jxcb_Node = p;
			continue;
		}

		if (!(cb->q & CB_OPEN) || (cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI)))
		{
			if (izone == pos)
			{
				BuildTree(bvisited, bvisited_zn, zzone, p, pTree);
			}
			else
			{
				BuildTree(bvisited, bvisited_zn, izone, p, pTree);
			}
		}
	}
}

//查找指定结点上连接的设备（CB，XF，LD，BUS）
void FDIR_R::GetDevsByND(CLS_ND *pnd, list<FAULT_ITEM> &devs)
{
	assert(pnd != NULL);

	devs.clear();
	ChildPtr *ptr, *ptr1;

	if (pnd->r_cb != -1)
	{
		ptr1 = ptr = dsbob->r_nd_cb_tb->FindChildPtr(pnd->id);

		while (ptr != NULL)
		{
			CLS_CB *tmp_cb = dsbob->cb_tb->GetDataByPos(ptr->Ptr);
			devs.push_back(FAULT_ITEM(0, tmp_cb->pid, "CB", tmp_cb->descr));

			ptr = ptr->Next;
		}

		FreeChildPtr(ptr1);
	}
	else if (pnd->r_xf != -1)
	{
		ptr1 = ptr = dsbob->r_nd_xf_tb->FindChildPtr(pnd->id);

		while (ptr != NULL)
		{
			CLS_XF *tmp_xf = dsbob->xf_tb->GetDataByPos(ptr->Ptr);
			devs.push_back(FAULT_ITEM(0, tmp_xf->pid, "XF", tmp_xf->descr));

			ptr = ptr->Next;
		}

		FreeChildPtr(ptr1);
	}
	else
	{
		int lv_ld = dsbob->ld_tb->GetCount();
		int lv_bus = dsbob->bus_tb->GetCount();
		int index = 0;

		if (pnd->r_ld >= 0)
		{
			ptr1 = ptr = dsbob->r_nd_ld_tb->FindChildPtr(pnd->id);

			while (ptr != NULL)
			{
				CLS_LD *tmp_ld = dsbob->ld_tb->GetDataByPos(ptr->Ptr);
				devs.push_back(FAULT_ITEM(0, tmp_ld->pid, "LD", tmp_ld->descr));

				ptr = ptr->Next;
			}

			FreeChildPtr(ptr1);
		}
		//该结点上未连接LD
		else
		{
			for (index = 0; index < lv_bus; index++)
			{
				if (dsbob->bus_tb->GetDataByPos(index)->nd == pnd->id)
				{
					CLS_BUS *bus = dsbob->bus_tb->GetDataByPos(index);

					devs.push_back(FAULT_ITEM(0, bus->pid, "BUS", bus->descr));
				}
			}
		}
	}
}

//D5000可能要根据实际信号调整
void FDIR_R::EraseNotUseFaultsng(std::list<long> &faultsng, bool ispointid)
{
	for (FDIR_POINT_TRIG_SECTION::iterator it_sec = g_lstSecTrig.begin(); it_sec != g_lstSecTrig.end(); it_sec++)
	{
		cout << "故障信号id是： " << (*it_sec)->alarm.key_id << "故障stid是： " << (*it_sec)->alarm.st_id << endl;
//		if (strcmp((*it_sec)->alarm.tabname, "point") == 0)		//D5000暂时只考虑开关事故跳闸
		{
			std::list<long>::iterator it_vi;
			for (it_vi = faultsng.begin(); it_vi != faultsng.end(); it_vi++)
			{
				if (ispointid)
				{
					if (*it_vi == (*it_sec)->alarm.key_id)
					{
						break;
					}
				}
				else
				{
					if (*it_vi == (*it_sec)->psbob_data.point.id)
					{
						break;
					}
				}
			}
			if (it_vi == faultsng.end())
			{
				g_lstSecTrig.erase(it_sec);
				cout << "擦除未使用的故障指示器信号： " << (*it_sec)->alarm.key_id << endl;
				it_sec = g_lstSecTrig.begin();
			}
		}
	}
}

int FDIR_R::ReadCbOpen(list<long> &lstopen)
{
	cout << __LINE__ << __FUNCTION__ << endl;

	for (list<FAULT_ITEM>::iterator it_lst = lst_point.begin(); it_lst != lst_point.end(); it_lst++)
	{
		if (strcasecmp(it_lst->table.c_str(), "cb") == 0)
		{
			PSBOB::CB cb = { 0 };

			if (oodbread_rk(&cb, &it_lst->eq_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(cb)) == OO_FAIL)
			{
				TRACE("读取数据出错：%ld，errno：%d,%s\r\n", it_lst->eq_id, _oodb_errno, __FUNCTION__);
			}

			if (cb.point >= 1)
			{
				continue;
			}
			lstopen.push_back(it_lst->eq_id);
			cout <<it_lst->eq_id<<endl;
		}
	}

	return FDIR_R_SUCCEED;
}

int FDIR_R::GetCbCountToNd(int ind, int zn_type)
{
	CLS_ND *nd = dsbob->nd_tb->GetDataByPh(ind);

	////////////////////////////////////////////
	//荣义平添加于20111218，已连接母线的结点不再统计其连接的开关
	if (zn_type != _FDIR_SO && zn_type != _FDIR_BUS)
	{
		map<long, list<long> >::iterator it = g_map_nd_bus.find(nd->id);

		if (it != g_map_nd_bus.end())
		{
			for (list<long>::iterator it_lst = it->second.begin(); it_lst != it->second.end(); it_lst++)
			{
				CLS_BUS *tempbus = dsbob->bus_tb->Find(CLS_BUS(*it_lst));
				if (tempbus != NULL)
				{
					return 0;
				}
			}
		}
	}
	//荣义平添加于20111218，已连接母线的结点不再统计其连接的开关
	////////////////////////////////////////////

	return dsbob->r_nd_cb_tb->FindChildNum(nd->id);
}

int FDIR_R::Insert_Zone_Cb(int icb, int k, int zn_type)
{
	if(icb < 0) return 0;

	CLS_R_ND_CB* r_nd_cb = dsbob->r_nd_cb_tb->GetTable();
	CLS_ND* nd = dsbob->nd_tb->GetTable();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = new CLS_FDIR_ZONE_CB;
	CLS_CB *tmp_cb = NULL;
	CLS_ND *tmp_nd = NULL;
//	ChildPtr *ptr = NULL, *tmp_ptr = NULL;
	int ind = -1;

#if 1	//D5000开关表中没有刀闸
	ChildPtr *ptr = dsbob->r_nd_cb_tb->FindChildPtr((r_nd_cb + icb)->nd_id);
	ChildPtr *ptr1 = ptr;

	while (ptr != NULL)
	{
		fdir_zone_cb->icb = ptr->Ptr;
		fdir_zone_cb->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE_CB")), fdir_zone_cb_tb->GetCount() + 1);

		if (fdir_zone_cb_tb->Insert(*fdir_zone_cb) == -1) // 插入表中
		{
			FreeChildPtr(ptr1);
			delete fdir_zone_cb;
			return -1;
		}

		ptr = ptr->Next;
	}

	FreeChildPtr(ptr1);
#else
	for (int j = 0; j < k; j++)
	{
		fdir_zone_cb->icb = -1;
		fdir_zone_cb->id = -1;
		//20111218荣义平，前面形成区段的时候忽略了刀闸，所以这里计算区段关联的开关时，刀闸不计算入内
		tmp_cb = dsbob->cb_tb->Find(CLS_CB((r_nd_cb + icb + j)->cb_id));
		if (tmp_cb->q & CB_SWITCH)
		{
			if (zn_type == _FDIR_SO)
			{
				ChildPtr *ptr = NULL, *tmp_ptr = NULL;

				//查找刀闸另外一端连接的进线开关
				ind = (r_nd_cb + icb + j)->nd_id_ptr;

				if (tmp_cb->ind == ind)
				{
					tmp_nd = dsbob->nd_tb->Find(CLS_ND(tmp_cb->znd));
				}
				else
				{
					tmp_nd = dsbob->nd_tb->Find(CLS_ND(tmp_cb->nd));
				}

				ptr = dsbob->r_nd_cb_tb->FindChildPtr(tmp_nd->id);

				tmp_ptr = ptr;

				while (ptr)
				{
					if (ptr->Ptr != icb)
					{
						fdir_zone_cb->icb = ptr->Ptr;
						break;
					}

					ptr = ptr->Next;
				}

				FreeChildPtr(tmp_ptr);
			}
			else
				continue;
		}

		////////////////////////////////////////////
		//荣义平添加于20111218，已连接母线的结点不再统计其连接的开关
		if (zn_type != _FDIR_SO && zn_type != _FDIR_BUS)
		{
			int lv_bus = dsbob->bus_tb->GetCount();
			CLS_BUS *bus = dsbob->bus_tb->GetTable();

			for (int i = 0; i < lv_bus; i++)
			{
				if ((bus + i)->nd == (r_nd_cb + icb + j)->nd_id)
					break;
			}
		}
		//荣义平添加于20111218，已连接母线的结点不再统计其连接的开关
		////////////////////////////////////////////

		fdir_zone_cb->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE_CB")), fdir_zone_cb_tb->GetCount() + 1);//fdir_zone_cb_tb->GetCount(); //关联表记录id为当前记录数
		if(fdir_zone_cb->icb == -1)
			fdir_zone_cb->icb = (r_nd_cb + icb + j)->cb_id_ptr; // 保存icb=开关表物理号
		if (fdir_zone_cb_tb->Insert(*fdir_zone_cb) == -1) // 插入表中
		{
			delete fdir_zone_cb;
			return -1;
		}
	}
#endif
	delete fdir_zone_cb;
	return 0;
}

int FDIR_R::FindDownZone(int icb)
{
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();

	for (int i = 0; i < lv_fdir_zone; i++) //循环所有的区段
	{
		if ((fdir_zone + i)->icb == icb)
			return i;
	}
	return -1;
}

int FDIR_R::FindUpZone(int icb)
{
	int i,j,k;
	int cbptr,cbnum;
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	for(i = 0;i < lv_fdir_zone;i++)//循环所有的区段
	{
		//无电流流入并且不是供电区段的区段何以成为上游区段
		if((fdir_zone + i)->icb == -1 && (fdir_zone +i)->type != _FDIR_SO
				&& (fdir_zone +i)->type != _FDIR_BUS)
		{
			continue;
		}

		cbptr = (fdir_zone + i)->cbptr;//得到区段起始开关的编号
		cbnum = (fdir_zone + i)->cbnum;
		for(j = 0;j < cbnum;j++)
		{
			k = (fdir_zone_cb + cbptr + j)->icb;

			if( k == icb && k != (fdir_zone + i)->icb /* && !((fdir_cb + k)->topflags & FDIR_CB_OPEN)*/)
				return i;
		}
	}
	return -1;
}

bool FDIR_R::WriteRtdb()
{
	//D5000
	cout << "写入FDIR RTDB库…" << endl;

	g_db_fdir->ResetAllTables();

	int i = 0, size = 0, count = 0;

	cout << "//1 - write FDIR_CB table" << endl;

	CLS_FDIR_CB* fdir_cb;
	FDIR_CB* fdir_cb_set = new FDIR_CB;
	size = sizeof(FDIR_CB);

	fdir_cb = fdir_cb_tb->GetTable();
	count = fdir_cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		fdir_cb_set->id = (fdir_cb + i)->id;
		fdir_cb_set->izone = (fdir_cb + i)->ind;
		fdir_cb_set->zzone = (fdir_cb + i)->iznd;
		fdir_cb_set->q = (fdir_cb + i)->q;
		fdir_cb_set->topflags = (fdir_cb + i)->topflags;
		strcpy(fdir_cb_set->descr, (fdir_cb + i)->descr);
		fdir_cb_set->cb_id = (fdir_cb + i)->cb_id;

		if (oodbinsert(fdir_cb_set, const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_CB")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert FDIR_CB error!errno=" << _oodb_errno << endl;

			delete fdir_cb_set;

			return false;
		}
	}

	delete fdir_cb_set;

	cout << "//2 - write FDIR_ZONE table" << endl;

	CLS_FDIR_ZONE* fdir_zone;
	FDIR_ZONE* fdir_zone_set = new FDIR_ZONE;
	size = sizeof(FDIR_ZONE);

	fdir_zone = fdir_zone_tb->GetTable();
	count = fdir_zone_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		fdir_zone_set->id = (fdir_zone + i)->id;
		fdir_zone_set->type = (fdir_zone + i)->type;
		fdir_zone_set->sub = (fdir_zone + i)->sub;
		fdir_zone_set->q = (fdir_zone + i)->q;
		fdir_zone_set->topflags = (fdir_zone + i)->topflags;
		fdir_zone_set->cbnum = (fdir_zone + i)->cbnum;
		fdir_zone_set->cbptr = (fdir_zone + i)->cbptr;
		fdir_zone_set->icb = (fdir_zone + i)->icb;
		strcpy(fdir_zone_set->descr, (fdir_zone + i)->descr);
		fdir_zone_set->zn_id = (fdir_zone + i)->zn_id;

		if (oodbinsert(fdir_zone_set, const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert FDIR_ZONE error!errno=" << _oodb_errno << endl;

			delete fdir_cb_set;

			return false;
		}
	}

	delete fdir_zone_set;

	cout << "//3 - write FDIR_ZONE_CB table" << endl;

	CLS_FDIR_ZONE_CB* fdir_zone_cb;
	FDIR_ZONE_CB* fdir_zone_cb_set = new FDIR_ZONE_CB;
	size = sizeof(FDIR_ZONE_CB);

	fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	count = fdir_zone_cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		fdir_zone_cb_set->id = (fdir_zone_cb + i)->id;
		fdir_zone_cb_set->icb = (fdir_zone_cb + i)->icb;

		if (oodbinsert(fdir_zone_cb_set, const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE_CB")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert FDIR_ZONE_CB error!errno=" << _oodb_errno << endl;

			delete fdir_cb_set;

			return false;
		}
	}

	delete fdir_zone_cb_set;

	return true;
}

//将电源区段延伸到该区段的所有相邻区段
void FDIR_R::spread_so(int izone)
{
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();

	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();

	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	if ((fdir_zone + izone)->q & FDIR_ZONE_UNPROC)
	{
		(fdir_zone + izone)->q &= ~FDIR_ZONE_UNPROC;

		for (int i = 0; i < (fdir_zone + izone)->cbnum; i++)
		{
			int icb = (fdir_zone_cb + (fdir_zone + izone)->cbptr + i)->icb;

			if ((fdir_cb + icb)->q & FDIR_CB_UNPROC)
			{
				(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;

				if (!((fdir_cb + icb)->topflags & FDIR_CB_OPEN)) //开关是合状态
				{
//					if(bDisplay) TRACE("电流流过开关%s\r\n", (fdir_cb + icb)->descr);
					int zone1 = -1;

					if (izone == (fdir_cb + icb)->ind)
					{
						zone1 = (fdir_cb + icb)->iznd;
					}
					else
					{
						zone1 = (fdir_cb + icb)->ind;
					}

					if ((fdir_zone + zone1)->type != _FDIR_SO)
					{
						(fdir_zone + zone1)->icb = icb;
						spread_so(zone1);
					}
				}
				else
				{
//					if(bDisplay) TRACE("开关%s为断开状态！\r\n", (fdir_cb + icb)->descr);
				}
			}
			else
			{
//				if(bDisplay) TRACE("开关%s已经被处理过！\r\n", (fdir_cb + icb)->descr);
			}
		}
	}
	else
	{
//		if(bDisplay) TRACE("区段%d已经被处理过！\r\n", izone);
	}
}

int FDIR_R::FormIcbOfZone()
{
	//get fdir_cb,fdir_zone and fdir_zone_cb
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	int i;
	//set fdir_cb status
	int lv_cb = dsbob->cb_tb->GetCount();
	CLS_CB* cb = dsbob->cb_tb->GetTable();

	//将事故导致的跳闸、分闸的开关闭合
	for (list<int>::iterator it = lst_fault_open_cb.begin(); it != lst_fault_open_cb.end(); it++)
	{
		(cb + *it)->q &= ~CB_OPEN;
		TRACE("%s:合闸开关%ld[%s]\r\n", __FUNCTION__, (cb + *it)->id, (cb + *it)->descr);
	}

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

	//set icb of fdir_zone
	for (i = 0; i < lv_fdir_cb; i++)
		(fdir_cb + i)->q |= FDIR_CB_UNPROC;
	for (i = 0; i < lv_fdir_zone; i++)
	{
		(fdir_zone + i)->q |= FDIR_ZONE_UNPROC;
		(fdir_zone + i)->icb = -1;
	}

	int j, k, m, k0, current;
	int tzone[100];
	int izone, zone1, zone2;
	int cbptr, cbnum, icb, type;
	for (i = 0; i < lv_fdir_zone; i++) //循环每一个区段
	{
		type = (fdir_zone + i)->type; // 区段类型
		if (type == _FDIR_SO) // 电源类型
		{

//			if (strcmp((fdir_zone + i)->descr, "西宁.测试厂站/10kV.母线") == 0)
//			{
//				TRACE("!!!!!!!!!!!!!!!!处理母线%ld\r\n", (fdir_zone + i)->zn_id);
//				bDisplay = true;
//			}
//			else
//			{
//				bDisplay = false;
//			}

			(fdir_zone + i)->icb = -1; // 当前区段的icb置-1
#if 0
			tzone[0] = i; // i为当前区段号
			k = current = 1;
			while (current) // 新的相邻区段个数为current，不为0则继续查找
			{
				k0 = k; // k为tzone里面有效的区段个数，
				for (j = k0 - current; j < k0; j++) // j=0,j<1;
				{
					izone = tzone[j]; //未处理的相邻区段的区段号
					(fdir_zone + izone)->q &= ~FDIR_ZONE_UNPROC; //表示已处理
					cbnum = (fdir_zone + izone)->cbnum; // 区段对应开关数
					cbptr = (fdir_zone + izone)->cbptr; //区段对应的第一个开关的物理号（在fdir_zone_cb）
					for (m = 0; m < cbnum; m++) // 循环处理每一个开关
					{
						icb = (fdir_zone_cb + cbptr + m)->icb; // 从关联表取每一个开关的物理号
						if ((fdir_cb + icb)->q & FDIR_CB_UNPROC)
						{
							if (!((fdir_cb + icb)->topflags & FDIR_CB_OPEN)) // 开关是合状态
							{
								zone1 = (fdir_cb + icb)->ind; // 开关首端区段号
								zone2 = (fdir_cb + icb)->iznd; //开关末端区段号
								if (izone == zone1) // zone1==当前电源区段
								{
									tzone[k++] = zone2; //则zone2为相邻的区段
									(fdir_zone + zone2)->icb = icb; //icb为相邻区段流入电流的开关
									(fdir_zone + zone2)->q &= ~FDIR_ZONE_UNPROC;
								}
								else if (izone == zone2) //与上面功能相同
								{
									tzone[k++] = zone1;
									(fdir_zone + zone1)->icb = icb;
									(fdir_zone + zone1)->q &= ~FDIR_ZONE_UNPROC;
								}
							}
							(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
						}
					}
				}
				current = k - k0;
			}
#else
			spread_so(i);
#endif
		}
	}
	return FDIR_R_SUCCEED;
}


//////////////////////////////
//NAME:	FormResSteps
//FUNCTION:	form restoring steps
//生成非故障停电区域恢复步骤
///////////////////////////////
int FDIR_R::FormResSteps()
{
	//form restore steps
	CLS_FDIR_RESTORE* fdir_restore = new CLS_FDIR_RESTORE;

	int lv_fdir_cb = fdir_cb_tb->GetCount();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount(); //cuilizhong,is a bug? fdir_zone_tb?
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_SO *so = dsbob->so_tb->GetTable();

	int zone1, zone2, izone, iizone, icb;
	std::vector<int> tempzone;
	tempzone.assign(GetTableSize(g_db_fdir->GetTB("FDIR_ZONE")), 0);
//	int tempzone[GetTableSize(g_db_fdir->GetTB("FDIR_ZONE"))];
	std::vector<int> upbzones, downbzones;
	int lv_upbzones, lv_downbzones;
	int current, k0, m, n, i, j, k;
	int cbptr0, cbnum0, cbptr, cbnum;
//	CString temp;

	//form steps for up zones restoring
	if (!((fdir_cb + (fdir_zone + faultzone)->icb)->q & FDIR_CB_FDHCB)) //if icb of faultzone is not FDHCB, then restore up streams
	{
		//合馈线首端开关
		// cuilizhong 如果是联络开关合闸、故障段和馈线首端之间有开关A分闸，
		// cuilizhong 这种情况下发生故障，首端开关就没有分闸，合首端开关没有意义，应该把开关A合闸才对。
		fdir_restore->id = fdir_restore_tb2.size();
//		temp = (fdir_cb + faulticb)->descr;//faulticb首端开关号
//		temp = temp + "合闸";
//		strcpy(fdir_restore->descr,temp);//
		sprintf(fdir_restore->descr, "%s 合闸", (fdir_cb + faulticb)->descr);

		cout << fdir_restore->descr << endl;

		fdir_restore->icb = faulticb;
		fdir_restore->q = 0;
		if ((fdir_cb + faulticb)->q & FDIR_CB_OPEN0)
		{
			fdir_restore->q |= FDIR_RESTORE_OPEN0;
		}
		else
		{
			fdir_restore->q &= ~FDIR_RESTORE_OPEN0;
		}

		fdir_restore->q |= FDIR_RESTORE_OPEN;
#if 0		//荣义平20111213-进线开关不能直接合闸
		if (fdir_restore_tb->Insert(*fdir_restore) == -1)
		{
			return FDIR_R_FAIL;
		}
#endif
		//搜索所有上游供电路径
		/*		icb = (fdir_zone + faultzone)->icb;
		 upbzones.clear();

		 //begin to search from izone
		 for(j = 0;j < lv_fdir_zone;j++)
		 (fdir_zone + j)->q |= FDIR_ZONE_UNPROC;
		 for(j = 0;j < lv_fdir_cb;j++)
		 (fdir_cb + j)->q |= FDIR_CB_UNPROC;

		 izone = FindUpZone(icb);
		 tempzone[0] = izone;
		 upbzones.push_back(izone);

		 (fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
		 current = k = 1;
		 while(1)
		 {//7
		 k0 = k;
		 for(j = k0 - current; j < k0; j++)
		 {//6
		 iizone = tempzone[j];
		 (fdir_zone + iizone)->q &= ~FDIR_ZONE_UNPROC;
		 cbnum = (fdir_zone + iizone)->cbnum;
		 cbptr = (fdir_zone + iizone)->cbptr;
		 for(m = 0; m < cbnum; m++)
		 {//5
		 icb = (fdir_zone_cb + cbptr + m)->icb;
		 if((fdir_cb + icb)->q & FDIR_CB_FDLCB)
		 continue;
		 if((fdir_cb + icb)->q & FDIR_CB_UNPROC)
		 {//4
		 if(!((fdir_cb + icb)->topflags & FDIR_CB_OPEN))
		 {//3
		 zone1 = (fdir_cb + icb)->ind;
		 zone2 = (fdir_cb + icb)->iznd;
		 if(iizone == zone1)
		 {//1
		 tempzone[k++] = zone2;
		 upbzones.push_back(zone2);
		 }//1
		 else if(iizone == zone2)
		 {//2
		 tempzone[k++] = zone1;
		 upbzones.push_back(zone1);
		 }//2
		 }//3

		 (fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
		 }//4
		 }//5
		 }//6
		 current = k - k0;
		 if(current == 0) break;
		 }//7
		 lv_upbzones = upbzones.size();
		 for(n = 0;n < lv_upbzones;n++)
		 {
		 printf("upbzones:%s,\n",(fdir_zone + upbzones[n])->descr);
		 }
		 int uprestore = 0;//record number of up streams restoring
		 for(k = 0;k < upbzones.size();k++)
		 {//12
		 if(uprestore > 0)
		 break;
		 iizone = upbzones[k];
		 cbptr = (fdir_zone + iizone)->cbptr;
		 cbnum = (fdir_zone + iizone)->cbnum;
		 for(j = 0;j < cbnum;j++)
		 {//11
		 icb = (fdir_zone_cb + cbptr + j)->icb;
		 if((fdir_cb + icb)->q & FDIR_CB_FDLCB)
		 continue;

		 //由于湖州工程无法对馈线首端开关进行控制，所以馈线首端开关无法作为恢复开关
		 if((fdir_cb + icb)->q & FDIR_CB_FDHCB)
		 continue;

		 zone1 = (fdir_cb + icb)->ind;
		 zone2 = (fdir_cb + icb)->iznd;
		 if((fdir_cb + icb)->topflags & FDIR_CB_OPEN)
		 {//10
		 if(zone1 == iizone)
		 if(!((fdir_zone + zone2)->topflags & FDIR_ZONE_DEAD))
		 {//8
		 fdir_restore->id = fdir_restore_tb->GetCount();
		 temp = (fdir_cb + icb)->descr;
		 temp = temp + "合闸";
		 strcpy(fdir_restore->descr,temp);//
		 fdir_restore->icb = icb;
		 fdir_restore->q = 0;
		 if((fdir_cb + j)->q & FDIR_CB_OPEN0)
		 {
		 fdir_restore->q |= FDIR_RESTORE_OPEN0;
		 }
		 else
		 {
		 fdir_restore->q &= ~FDIR_RESTORE_OPEN0;
		 }

		 fdir_restore->q |= FDIR_RESTORE_OPEN;

		 if(fdir_restore_tb->Insert(*fdir_restore) == -1)
		 {
		 return FDIR_R_FAIL;
		 }
		 uprestore++;
		 break;
		 }//8
		 if(zone2 == iizone)
		 if(!((fdir_zone + zone1)->topflags & FDIR_ZONE_DEAD))
		 {//9
		 fdir_restore->id = fdir_restore_tb->GetCount();
		 temp = (fdir_cb + icb)->descr;
		 temp = temp + "合闸";
		 strcpy(fdir_restore->descr,temp);//
		 fdir_restore->icb = icb;
		 fdir_restore->q = 0;
		 if((fdir_cb + j)->q & FDIR_CB_OPEN0)
		 {
		 fdir_restore->q |= FDIR_RESTORE_OPEN0;
		 }
		 else
		 {
		 fdir_restore->q &= ~FDIR_RESTORE_OPEN0;
		 }

		 fdir_restore->q |= FDIR_RESTORE_OPEN;

		 if(fdir_restore_tb->Insert(*fdir_restore) == -1)
		 {
		 FDIR_R_FAIL;
		 }
		 uprestore++;
		 break;
		 }//9
		 }//10
		 }//11
		 }//12
		 */}

	//form steps for down zones steps
	cbptr0 = (fdir_zone + faultzone)->cbptr;
	cbnum0 = (fdir_zone + faultzone)->cbnum;
	map_cb_restore.clear();

	for (i = 0; i < cbnum0; i++) // 对故障区段每个边界开关对应的下游区段，分别进行恢复
	{ //14
		int border_icb = 0;	//故障区段边界开关

		icb = (fdir_zone_cb + cbptr0 + i)->icb;
		border_icb = icb;

		if ((fdir_zone + faultzone)->icb == icb) // cuilizhong  (fdir_zone + faultzone)->icb是连接上游区段的开关？
			continue;

		printf("边界开关：%s下游区段恢复方案：\r\n", (fdir_cb + icb)->descr);
		map_cb_restore[(fdir_cb + border_icb)->cb_id] = false;

		downbzones.clear();
		if (!((fdir_cb + icb)->q & FDIR_CB_FDLCB) && !((fdir_cb + icb)->topflags & FDIR_CB_OPEN0)) //如果该开关原来是合的
		{ //13
			zone1 = (fdir_cb + icb)->ind; // 开关对应的两个区段
			zone2 = (fdir_cb + icb)->iznd; // 开关对应的两个区段
			if (zone1 == faultzone)
				izone = zone2;
			if (zone2 == faultzone)
				izone = zone1; // izone为开关对应的另一个区段，加入到下游区段
			//begin to search from izone
			for (j = 0; j < lv_fdir_zone; j++)
				(fdir_zone + j)->q |= FDIR_ZONE_UNPROC;
			for (j = 0; j < lv_fdir_cb; j++)
				(fdir_cb + j)->q |= FDIR_CB_UNPROC;

			tempzone[0] = izone;
			downbzones.push_back(izone);

			vector<CLS_FDIR_RESTORE> tmp;
			tmp.clear();
			fdir_restore_tb2[izone] = tmp;

			(fdir_cb + (fdir_zone + izone)->icb)->q &= ~FDIR_CB_UNPROC;
			current = k = 1;
			while (1)
			{ //7
				k0 = k;
				for (j = k0 - current; j < k0; j++)
				{ //6
					iizone = tempzone[j];
					(fdir_zone + iizone)->q &= ~FDIR_ZONE_UNPROC;
					cbnum = (fdir_zone + iizone)->cbnum;
					cbptr = (fdir_zone + iizone)->cbptr;
					for (m = 0; m < cbnum; m++)
					{ //5
						icb = (fdir_zone_cb + cbptr + m)->icb;
						if ((fdir_cb + icb)->q & FDIR_CB_FDLCB)
							continue;
						if ((fdir_cb + icb)->q & FDIR_CB_UNPROC)
						{ //4
							if (!((fdir_cb + icb)->topflags & FDIR_CB_OPEN))
							{ //3
								zone1 = (fdir_cb + icb)->ind;
								zone2 = (fdir_cb + icb)->iznd;
								if (iizone == zone1)
								{ //1
									tempzone[k++] = zone2;
									downbzones.push_back(zone2);
								} //1
								else if (iizone == zone2)
								{ //2
									tempzone[k++] = zone1;
									downbzones.push_back(zone1);
								} //2
							} //3

							(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
						} //4
					} //5
				} //6
				current = k - k0;
				if (current == 0)
					break;
			} //7
			lv_downbzones = downbzones.size();
			for (n = 0; n < lv_downbzones; n++)
			{
				printf("downbzones:%s,\n", (fdir_zone + downbzones[n])->descr);

#if 0
				for (int index = 0; index < dsbob->zn_tb->GetCount(); index++)
				{
					if (strcmp(dsbob->zn_tb->Data[index].descr, (fdir_zone + downbzones[n])->descr)
							== 0)
					{
						for (int index1 = 0; index1 < dsbob->zn_tb->Data[index].type; index1++)
						{
							cout << dsbob->zn_tb->Data[index].nds[index1] << ",";
						}
					}
				}

				cout << endl;
#endif
			}
			int downrestore = 0; //record number of down streams restoring
			for (k = 0; k < downbzones.size(); k++) // 对下游区段进行循环
			{ //12
//				if (downrestore > 0) // 找到一个可恢复操作（合闸）即停止
//					break;

				//该边界开关对应的下游区段有恢复方案
//				if(downrestore > 0)
//					map_cb_restore[(fdir_cb + border_icb)->cb_id] = true;

				iizone = downbzones[k]; // 取一个下游区段
				cbptr = (fdir_zone + iizone)->cbptr;
				cbnum = (fdir_zone + iizone)->cbnum;
				for (j = 0; j < cbnum; j++) // 对下游区段的边界开关进行循环
				{ //11
					icb = (fdir_zone_cb + cbptr + j)->icb;
					if ((fdir_cb + icb)->q & FDIR_CB_FDLCB)
						continue;
					zone1 = (fdir_cb + icb)->ind; // 开关本侧区段
					zone2 = (fdir_cb + icb)->iznd; // 开关另一侧区段
					if ((fdir_cb + icb)->topflags & FDIR_CB_OPEN) // 如果是开关是分闸状态
					{ //10
#if 0	//荣义平于20111202注释掉
						if (zone1 == iizone) // zone2为另一侧侧区段
							if (!((fdir_zone + zone2)->topflags & FDIR_ZONE_DEAD)) // 区段非故障状态
							{ //8
								fdir_restore->id = fdir_restore_tb->GetCount();
//								temp = (fdir_cb + icb)->descr;
//								temp = temp + "合闸";
//								strcpy(fdir_restore->descr,temp);//
								sprintf(fdir_restore->descr, "%s 合闸", (fdir_cb + icb)->descr);

								cout << fdir_restore->descr << endl;

								fdir_restore->icb = icb;
								fdir_restore->q = 0;
								if ((fdir_cb + j)->q & FDIR_CB_OPEN0)
								{
									fdir_restore->q |= FDIR_RESTORE_OPEN0;
								}
								else
								{
									fdir_restore->q &= ~FDIR_RESTORE_OPEN0;
								}

								fdir_restore->q |= FDIR_RESTORE_OPEN;

								if (fdir_restore_tb->Insert(*fdir_restore) == -1)
								{
									return FDIR_R_FAIL;
								}
								downrestore++;
								break;
							} //8
						if (zone2 == iizone)
							if (!((fdir_zone + zone1)->topflags & FDIR_ZONE_DEAD))
							{ //9
								fdir_restore->id = fdir_restore_tb->GetCount();
//								temp = (fdir_cb + icb)->descr;
//								temp = temp + "合闸";
//								strcpy(fdir_restore->descr,temp);//
								sprintf(fdir_restore->descr, "%s 合闸", (fdir_cb + icb)->descr);

								cout << fdir_restore->descr << endl;

								fdir_restore->icb = icb;
								fdir_restore->q = 0;
								if ((fdir_cb + j)->q & FDIR_CB_OPEN0)
								{
									fdir_restore->q |= FDIR_RESTORE_OPEN0;
								}
								else
								{
									fdir_restore->q &= ~FDIR_RESTORE_OPEN0;
								}

								fdir_restore->q |= FDIR_RESTORE_OPEN;

								if (fdir_restore_tb->Insert(*fdir_restore) == -1)
								{
									return FDIR_R_FAIL;
								}
								downrestore++;
								break;
							} //9
#else	//荣义平于20111202添加
						int idownzone = -1; //与下游区段相邻的区段

						if (iizone == zone2)
							idownzone = zone1;
						else if (iizone == zone1)
							idownzone = zone2;
						else
							idownzone = -1;

						if (idownzone != -1)
						{
#if 0
							if (!((fdir_zone + idownzone)->topflags & FDIR_ZONE_DEAD)) // 区段非故障状态
#else
							if((fdir_zone + idownzone)->icb != -1)
#endif
							{
								fdir_restore->id = 0;//fdir_restore_tb[izone].GetCount();

								sprintf(fdir_restore->descr, "%s 合闸", (fdir_cb + icb)->descr);

								cout << fdir_restore->descr << endl;

								while ((fdir_zone + idownzone)->type != _FDIR_SO
										&& (fdir_zone + idownzone)->type != _FDIR_BUS
										&& idownzone < lv_fdir_zone)
								{
									if((fdir_zone + idownzone)->icb == -1)
									{
										printf("%s 区段无电流流入开关！！！\r\n", (fdir_zone + idownzone)->descr);
										break;
									}

									idownzone = FindUpZone((fdir_zone + idownzone)->icb);
								}

								if ((fdir_zone + idownzone)->type == _FDIR_SO)
								{
									fdir_restore->so_id = (so + (fdir_zone + idownzone)->sub)->id;
									cout << "此恢复方案使用电源："	<< (so + (fdir_zone + idownzone)->sub)->descr << endl;
								}
								else if ((fdir_zone + idownzone)->type == _FDIR_BUS)
								{
									CLS_BUS *bus = dsbob->bus_tb->GetTable();

									fdir_restore->so_id = (bus + (fdir_zone + idownzone)->sub)->id;
									cout << "此恢复方案使用电源："	<< (bus +(fdir_zone + idownzone)->sub)->descr << endl;
								}
								else
								{
									cout << "未追踪到此恢复方案的电源！" << endl;
								}

								fdir_restore->icb = icb;
								fdir_restore->cb= dsbob->cb_tb->Data[icb].id;
								strcpy(fdir_restore->cb_desc, dsbob->cb_tb->Data[icb].descr);
								fdir_restore->q = 0;
								if ((fdir_cb + j)->q & FDIR_CB_OPEN0)
								{
									fdir_restore->q |= FDIR_RESTORE_OPEN0;
								}
								else
								{
									fdir_restore->q &= ~FDIR_RESTORE_OPEN0;
								}

								fdir_restore->q |= FDIR_RESTORE_OPEN;

								fdir_restore_tb2[izone].push_back(*fdir_restore);

								map_cb_restore[(fdir_cb + border_icb)->cb_id] = true;

								downrestore++;
//								break;
							}

//							downrestore++;
//							break;
						}
#endif
					} //10
				} //11
			} //12
		} //13
	} //14
#if 0
	//write to rtap
	rtChar tbName[255];
	strcpy(tbName, "fdir_r_restore");
	if(WriteTable(dbConnection, fdir_restore_tb->Data, fdir_restore_tb->Count, sizeof(FDIR_RESTORE), tbName) == rtFAILED)
	{
		return FDIR_R_FAIL;
	}
#endif
	delete fdir_restore;
	return FDIR_R_SUCCEED;
}

extern stack<NODE> get_virtual_path(long nd);
bool FDIR_R::update_cb_state(bool bfhzg)
{
	cout << "FDIR_R更新开关状态！" << endl;

	lv_cb = dsbob->cb_tb->GetCount();

	//更新开关状态
	for (int i = 0; i < lv_cb; i++)
	{
		CLS_CB *cb = dsbob->cb_tb->GetDataByPos(i);
		cb->q &= ~CB_FAULT_ISOLATE;

		if(cb->q & CB_VIRTUAL)
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
			if (oodbread_rk(&psbob_cb, &cb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
			{
				cout << "更新开关" << cb->id << "状态失败,errno:" << _oodb_errno << endl;

				return false;
			}

			cb->q &= ~CB_OPEN;

			if (psbob_cb.point <= 0)
				cb->q |= CB_OPEN;
		}

		if (version == VERSION_REALTIME)
		{
			if (!bfhzg)
			{
				//20111201荣义平
				//将事故跳闸开关置合，以便计算区段的电流流入开关
				pthread_mutex_lock(&trig_list_mutex);
				for (FDIR_POINT_TRIG_SECTION::iterator it = g_lstSecTrig.begin(); it != g_lstSecTrig.end(); it++)
				{
					if ((*it)->alarm.dev == ALARM_BREAKER && cb->pid == (*it)->psbob_data.point.id)
					{
						TRACE("合跳闸开关，cb:%ld(%s)\r\n", cb->id, cb->descr);
						cb->q &= ~CB_OPEN;
						break;
					}
				}
				pthread_mutex_unlock(&trig_list_mutex);

				//20111201荣义平
				//将事故跳闸开关置合，以便计算区段的电流流入开关
				for (list<FAULT_ITEM>::iterator it_lst_p = lst_point.begin(); it_lst_p != lst_point.end(); it_lst_p++)
				{
					if (it_lst_p->eq_id == cb->pid)
					{
						TRACE("合跳闸开关，cb:%ld, pointid:%s, point_name:%ld\r\n", cb->pid, cb->descr, it_lst_p->id);
						cb->q &= ~CB_OPEN;
						break;
					}
				}
			}
		}
		else
		{
			if (!bfhzg)
			{

			}
		}
	}


	int lv_switch = dsbob->switch_tb->GetCount();

	for(int i = 0; i < lv_switch; i++)
	{
		CLS_SWITCH *swh = dsbob->switch_tb->GetDataByPos(i);

		PSBOB::SWITCH psbob_switch = { 0 };
		PSBOB::ST psbob_st = { 0 };

		KEY_STRU ks = { 0 };

		CCommon::long_to_keyid(swh->pid, &ks);

		if (oodbread_rk(&psbob_switch, &swh->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB(ks.table_no)), sizeof(psbob_switch)) == OO_FAIL)
		{
			TRACE("更新刀闸%ld(%ld)[%s]状态失败！\r\n", swh->id, swh->pid, swh->descr);

			return false;
		}

		if (psbob_switch.st_id <= 0)
		{
			TRACE("刀闸%ld的厂站ID错误！", psbob_switch.id);
			return false;
		}

		if (oodbread_rk(&psbob_st, &psbob_switch.st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) == OO_FAIL)
		{
			TRACE("读取%ld厂站信息失败！\r\n", psbob_switch.st_id);

			return false;
		}

		//馈线厂站的刀闸全部视为闭合状态
		if (HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type , MENU_FAC_TYPE_FEEDVIR) || psbob_switch.point > 0)
		{
			swh->q &= ~CB_OPEN;
		}
		else
		{
			swh->q |= CB_OPEN;
		}
	}

	return true;
}

int FDIR_R::add_so()
{
	cout << "FDIR_R增加电源！" << endl;

	dsbob->so_tb->Empty();

	lv_bus = dsbob->bus_tb->GetCount();

	for (int i = 0; i < lv_bus; i++)
	{
		int ind = dsbob->bus_tb->Data[i].ind;

		if (ind < 0)
			continue; //无节点

		PSBOB::BUS psbob_bus = {0};

		//读取母线对应的遥测信息
		if (oodbread_rk(&psbob_bus, &dsbob->bus_tb->Data[i].pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(psbob_bus)) <= 0)
		{
			cout << "读取母线" << dsbob->bus_tb->Data[i].id << "电压信息失败,errno:" << _oodb_errno << endl;

			continue; //物理母线无电压信息
		}

		if (psbob_bus.v < 1)
		{
			//是否需要补母线电压量测
			bool isset = false;

			for (list<long>::iterator it_lst = lst_loss_vl_bus.begin(); it_lst != lst_loss_vl_bus.end(); it_lst++)
			{
				if ((*it_lst) == dsbob->bus_tb->Data[i].id)
				{
					TRACE("补失压母线：%ld\r\n", *it_lst);
					psbob_bus.v = 10.0;
					isset = true;
					break;
				}
			}
			if (!isset)
			{
				pthread_mutex_lock(&trig_list_mutex);
				for (FDIR_POINT_TRIG_SECTION::iterator it = g_lstSecTrig.begin(); it != g_lstSecTrig.end(); it++)
				{
					if ((strcmp((*it)->alarm.fault_info, "母线失压") == 0) && (dsbob->bus_tb->Data[i].id == (*it)->alarm.key_id))
					{
						TRACE("补失压母线：%ld\r\n", dsbob->bus_tb->Data[i].id);
						psbob_bus.v = 10.0;
						isset = true;
						break;
					}
				}
				pthread_mutex_unlock(&trig_list_mutex);
			}
			if (!isset)
			{
				continue;
			}
		}

		CLS_SO tmp_so;

		KEY_STRU ks = { 0 };

		CCommon::long_to_keyid(dsbob->bus_tb->GetDataByPos(i)->id, &ks);

		tmp_so.id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SO")), ks.key);//dsbob->bus_tb->Data[i].id;
		tmp_so.stid = dsbob->bus_tb->Data[i].stid;
		tmp_so.pid = dsbob->bus_tb->Data[i].pid;

		tmp_so.ind = ind;
		tmp_so.nd = dsbob->bus_tb->Data[i].nd;
		strcpy(tmp_so.descr, dsbob->bus_tb->Data[i].descr);

		dsbob->so_tb->Insert(tmp_so);

		dsbob->nd_tb->Find(CLS_ND(dsbob->bus_tb->Data[i].nd))->q |= ND_SO;
	}

//	if (data_start.type == TASK_FAULT_BUS)
//	{
//		CLS_BUS *bus = dsbob->bus_tb->Find(CLS_BUS(data_start.source.bus_id));
//
//		if (bus != NULL)
//		{
//			CLS_SO tmp_so;
//
//			tmp_so.id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SO")), dsbob->so_tb->GetCount() + 1);//data_start.source.bus_id;
//
//			tmp_so.ind = bus->ind;
//			tmp_so.nd = bus->nd;
//			strcpy(tmp_so.descr, bus->descr);
//
//			dsbob->so_tb->Insert(tmp_so);
//
//			dsbob->nd_tb->Find(CLS_ND(bus->nd))->q |= ND_SO;
//		}
//		else
//		{
//			printf("电压越下限的母线ID为%lds\r\n", data_start.source.bus_id);
//		}
//	}

	lv_so = dsbob->so_tb->GetCount();
	cout << "增加电源end！" << endl;
	return true;
}

void FDIR_R::FindFaultIslandEdgeCB(int icb)
{
	assert(icb >= 0);
	if(icb < 0) return;

	CLS_CB *cb = dsbob->cb_tb->GetDataByPos(icb);
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE_CB *fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	int lv_fdir_cb_zone = fdir_zone_cb_tb->GetCount();
	int i, j;

	if (cb->q & CB_OPEN)
	{
		printf("%ld(%s),q:%d;", cb->id, cb->descr, cb->q);
		lst_edge_cb.push_back(icb);
	}
	else
	{
		for (i = 0; i < lv_fdir_zone; i++)
		{
			if ((fdir_zone + i)->icb == icb)
				break;
		}

		if (i < lv_fdir_zone)
		{
			int cbnum = (fdir_zone + i)->cbnum;
			int cbptr = (fdir_zone + i)->cbptr;

			for (j = 0; j < cbnum; j++)
			{
				if ((fdir_zone_cb + cbptr + j)->icb != icb && (fdir_zone_cb + cbptr + j)->icb >= 0)
				{
					CLS_CB *temp_cb = dsbob->cb_tb->GetDataByPos((fdir_zone_cb + cbptr + j)->icb);
					if (!(temp_cb->q & CB_FDHCB))
					{
						FindFaultIslandEdgeCB((fdir_zone_cb + cbptr + j)->icb);
					}
				}
			}
		}
	}
}
