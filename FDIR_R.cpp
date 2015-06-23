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
	//���� 20150204 ��ֹ��ĸ�߹���ִ��ʧ�ܺ���DA�ҵ�
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
//��ȡ�������Σ�tozone�����ε���ֹ���Σ�fmzone�����εĿ�ʼ���Σ�������fmzone��ʼ�����Σ�ֱ���Ͽ��Ŀ��ػ���tozone����Ϊֹtianyq
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
			//�����Ǻϵ�״̬ʱ�Ż�������ȵݹ��������
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
				//����Ҫ����
			}

			TRACE("����%ld(%s)�����ڻ�ǿ�ң�ؿ��أ�\r\n", tmp_cb->id, tmp_cb->descr);

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

//��ȡ��·����[ң��ֵ]�����к�
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

//�ж�һ�������Ƿ����ң�أ���ң�ط���true�����򷵻�false
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
		TRACE("��ȡcontroldigital��801����pnt_idΪ%ld�ļ�¼ʧ��,��Ӧ�Ŀ����豸IDΪ%ld\r\n", pt_id, kid);
		return false;
	}
	else
	{
		TRACE("��ȡ�����豸%ld�ڱ��еļ�¼�ɹ�������λ%d\n", kid,data.exp_state);
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
			printf("%s(%d):��ȡPSBOB CB��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, head.eq_id, _oodb_errno);
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
				printf("%s(%d):��ȡPSBOB CB��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, it_tail->eq_id, _oodb_errno);
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

//��ȡת�����ߵĿ�����Ϣ
int FDIR_R::getfhzglstcb(int izone, list<CLS_CB> &lst_cb)
{
	if (izone == -1)
	{
		TRACE("���ҵ�Դ��������Ϊ-1\r\n");
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
		TRACE("����ת��������û�з��Ͻ�����������cb�����Ҹ���ת������ʧ��\r\n");
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
			TRACE("�Ҳ������߿��أ�");
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
			TRACE("δ�ҵ����߿��أ�\r\n");
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
			TRACE("��ȡPSBOB CB��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ�����ţ�%d\r\n", ((CLS_CB*)(*it_lst)->data.m_pData)->pid, _oodb_errno);
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

			//���߿��صĴ���״̬
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

		fdir_isolate->id = fdir_isolate_tb->GetCount(); // ��բ�����ڸ����ļ�¼id����Ϊ�����ǰ����
		CLS_CB *cb = dsbob->cb_tb->Find(CLS_CB(id));

		sprintf(fdir_isolate->descr, "%ld ��բ", id);

		TRACE("%s\r\n", fdir_isolate->descr);

		fdir_isolate->icb = dsbob->cb_tb->FindLog(cb->id); // ���ض�Ӧ�Ŀ��������

		if (it_step->q & FDIR_ISOLATE_UP)
			fdir_isolate->q |= FDIR_ISOLATE_UP;	//�˿���Ϊ�������ο��أ�����Ҫ��Щ����������ת��
		else
			fdir_isolate->q &= ~FDIR_ISOLATE_UP;

		fdir_isolate->q |= FDIR_ISOLATE_OPEN;
		fdir_isolate->cb= cb->pid;
		strcpy(fdir_isolate->cb_desc, cb->descr);
		TRACE("�Ѹ��뿪�أ�%s �ָ��������б���\r\n", fdir_isolate->descr);

		if (fdir_isolate_tb->Insert(*fdir_isolate) == -1) // ������������
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
	//D5000����Ҫת��
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
			printf("��ȡ���ر�%s����ʧ�ܣ�����ţ�%d��line��%d\r\n", fdir_isolate_tb->Data[i].cb, _oodb_errno, __LINE__);
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
//			printf("��ȡ���ر�%s����ʧ�ܣ�����ţ�%d��line��%d\r\n", fdir_isolate_tb->Data[i].cb, _oodb_errno, __LINE__);
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
//			printf("��ȡ���ر�%s����ʧ�ܣ�����ţ�%d��line��%d\r\n", fdir_isolate_tb2->Data[i].cb, _oodb_errno, __LINE__);
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

	//��ȡ���εĿ���
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

	//��ȡ�����ڵĸ���
	get_zone_ld(izone, lst_dev);

	for (it = lst_dev.begin(); it != lst_dev.end(); it++)
	{
		CLS_LD *ld = dsbob->ld_tb->Find(CLS_LD(*it));

		devs.push_back(FAULT_ITEM(0, ld->pid, "LD", ld != NULL ? ld->descr : ""));
	}

	//��ȡ�����ڵ��߶�
	get_zone_sec(izone, lst_dev);

	for (it = lst_dev.begin(); it != lst_dev.end(); it++)
	{
		CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(*it));

		devs.push_back(FAULT_ITEM(0, sec->pid, "SEC", sec != NULL ? sec->descr : ""));

	}

	//����ĸ�߼���բ
	lst_dev.clear();

	CLS_ZN* zn = dsbob->zn_tb->GetTable();
	for(j = 0; j < (zn+izone)->type; j++)
	{
		//printf("��ȡ�豸��������:%s�����:%s\n",(zn+izone)->id, (zn+izone)->nds[j].nd_id);
		map<long, list<long> >::iterator it;
		//����ĸ���豸
		it = g_map_nd_bus.find((zn+izone)->nds[j].nd_id);
		if(it != g_map_nd_bus.end())
		{
			for(list<long>::iterator vi = it->second.begin(); vi != it->second.end(); vi++)
			{
				PSBOB::BUS psbob_bus = {0};

				if(oodbread_rk(&psbob_bus, &(*vi), const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(psbob_bus)) <= 0)
				{
					TRACE("��ȡIDΪ%ld��ĸ�߼�¼ʧ�ܣ�", *vi);
				}
				else
					devs.push_back(FAULT_ITEM(0, psbob_bus.id, "BUS", psbob_bus.describe));
			}
		}
		//���ҵ�բ�豸
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
			TRACE("��ȡIDΪ%ld�ĵ�բ��¼ʧ�ܣ�", *it);
		}
		else
			devs.push_back(FAULT_ITEM(0, psbob_switch.id, "SWITCH", psbob_switch.describe));
	}

	return FDIR_R_SUCCEED;
}

//�������α߽翪��ң�Ż�ȡ��Ӧ�����κ�
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
					cout << "�˿���Ϊ���߿��ػ������翪��" << dev_it->eq_id << endl;
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
				cout << "��ȡ�˿��ص�cb��ʧ��" << dev_it->eq_id << endl;
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
			printf("%s(%d):��ȡIDΪ%ld��CB��¼ʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, tmp_cb->pid, _oodb_errno);
			continue;
		}
		else
		{
			//���غ�բ
			if (psbob_cb.point > 0)
			{
				//���ؿ�����բ��û����
				if (HasMask(g_db_psbob->GetTB("breaker"), "brk_prop", psbob_cb.brk_prop , MENU_BRK_PROP_AUTO))
				{
					tail.push_back(FAULT_ITEM(0, psbob_cb.id, "CB", psbob_cb.describe));
				}
				//���ز�������բ
				else
				{
					bool b_has_ppft = false; //�ÿ������Ƿ��й���ָʾ��

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
							printf("%s(%d):��ȡrelaysig���¼ʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, _oodb_errno);
						}

						//�ÿ�����û�й���ָʾ��,���Ͽ��ܿ���ÿ���
						if (!b_has_ppft)
						{
							TRACE("����%ld[%s]��û���¹��ܣ����Ͽ��ܿ���ÿ��أ������μ�������\r\n", tmp_cb->id, tmp_cb->descr);

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
			//���ط�բ
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

//�ж�ĳ�������Ƿ�Ϊ�ܶ�����
bool FDIR_R::IsCBFailed(const long point_name)
{
	TRACE("�жϿ���%ld�Ƿ�Ϊ�ܶ����أ�\r\n", point_name);

	if(std::find(lst_failed.begin(), lst_failed.end(), point_name) == lst_failed.end())
		return false;
	else
	{
		TRACE("����%ld�ھܶ��б��У�\r\n", point_name);
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
		TRACE("Insert_Zone_Cbʧ��, nd_id=%ld\r\n", nd_id);
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

	//form zone from dms_zn  cuilizhong ����zn�����Σ�����zone�������ظ��������䣩��������_FDIR_ZN
	int lv_zn = dsbob->zn_tb->GetCount();
	CLS_ZN* zn = dsbob->zn_tb->GetTable();

	for (i = 0; i < lv_zn; i++)
	{
		///////////////////////////////////////////////////
		///////////////����ƽ�����20111130Ϊ��Ӧ����5����ڵ�ZN
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
		if (type == 1) // ���˵�����
		{
			fdir_zone->id = fdir_zone_tb->GetCount(); // ����id��0��ʼ��
			strcpy(fdir_zone->descr, (zn + i)->descr);
			fdir_zone->type = _FDIR_ZN;
			fdir_zone->sub = i; // zn�������
			fdir_zone->cbptr = fdir_zone_cb_tb->GetCount(); //ָ��fdir_zone_cb_tb��ϵ������������׸���¼λ��
			ind1 = (zn + i)->ind; // ���ζ�Ӧ�Ľڵ�
			k1 = GetCbCountToNd(ind1); //ͨ���ڵ��ҵ��������ظ���
			fdir_zone->cbnum = k1; //�������ظ���

			if (fdir_zone_tb->Insert(*fdir_zone) == -1) // ���汾������fdir_zone����
			{
				printf("%s failed(%d),line:%d\r\n", __FUNCTION__, i, __LINE__);
				return FDIR_R_FAIL;
			}

			icb = (nd + ind1)->r_cb; //nd��Ӧ���׸������ڹ�ϵ���е������
			if (Insert_Zone_Cb(icb, k1) == -1) // �����κ͹������ز����ϵ��
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
	//form zone from xfmr   ���ݱ�ѹ������zone ��������_FDIR_XFMR
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
			fdir_zone->sub = i; //��ѹ���������
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
	//form zone from dms_bus  ����ĸ������zone ��������_FDIR_BUS
	int lv_bus = dsbob->bus_tb->GetCount();
	CLS_BUS* bus = dsbob->bus_tb->GetTable();
	CLS_ND* tmp_nd = NULL;

	bool *bvisited = new bool[lv_nd];

	for (i = 0; i < lv_bus; i++)
	{
		fdir_zone->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE")), fdir_zone_tb->GetCount() + 1);
		strcpy(fdir_zone->descr, (bus + i)->descr);
		fdir_zone->type = _FDIR_BUS;
		fdir_zone->sub = i; //ĸ�߱������
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

		//����BUS��SO�Ķ�Ӧ��ϵ����BUS��IDת����SO��ID
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
#if 0					//ǰ���γ�ĸ������ʱ�Ѿ��γ���Ӧ�ĵ�Դ����
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
	for (j = 0; j < lv_fdir_cb; j++) // ���ر�ѭ��
	{
		count = 0;
		for (i = 0; i < lv_fdir_zone; i++) //zone��ѭ��
		{
			//_FDIR_BUS��_FDIR_SO��Ӧ���������ε���Ϣ��Ϣһ��
			if((fdir_zone + i)->type == _FDIR_BUS)
			{
				continue;
			}

			cbptr = (fdir_zone + i)->cbptr; // �������׼�¼��
			cbnum = (fdir_zone + i)->cbnum; // �������ظ���
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
	if (tp->tp_allnew() == TP_FAIL) // �������������ˣ���������
	{
		return FDIR_R_FAIL;
	}
//	else
//	{
//		if (!dsbob->WriteReDB(NULL, NULL, NULL))
//		{
//			cout << __FUNCTION__ << "fdir_r->WriteRtdbʧ�ܣ�" << endl;
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
				//���Ϸ���ʱ���ڰ�Сʱ֮ǰ�򲻿���
				if (tm_now.tv_sec - sec > 1800)
				{
					continue;
				}
				printf("����ʱ�䣺%ld���ɹ���ʱ�䣺%ld\r\n", tm_now.tv_sec, sec);
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
						cout << "��ȡ����բ���أ�" << cb << endl;
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
	//���ø������źŶ�Ӧ���صı�־λ
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
				TRACE("��ȡң�ű���%ld��Ӧ�Ĺ���ָʾ���豸ʧ�ܣ�\r\n", pid);
				if (it == lstTrig.begin())
				{
					//���϶�λʧ�ܣ�Ҫ�������ź����ó��Ѿ�ʹ�ù�
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

			//�����п�����ң��ID��ͳһת�ɿ���ID���д���
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
				TRACE("��ȡң�ű���%ld��Ӧ�Ŀ����豸ʧ�ܣ�\r\n", pid);
				if (it == lstTrig.begin())
				{
					//���϶�λʧ�ܣ�Ҫ�������ź����ó��Ѿ�ʹ�ù�
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

				if (/*(*it)->alarm.type == SG_WARN_TYPE ||*/ D_SG_WARN_TYPE == (*it)->alarm.type)	//�¹���բ
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
		// �ж���ĸ�߹���
		else if (strcasecmp((*it)->alarm.tabname, "bus") == 0)
		{
			unsigned long id = (*it)->alarm.key_id;
			CLS_BUS *bus = NULL;
			bus = dsbob->bus_tb->Find(CLS_BUS(id));

			if (bus == NULL)
			{
				TRACE("��ȡĸ��%ldʧѹ�źŶ�Ӧ���豸ʧ�ܣ�\r\n", id);
				if (it == lstTrig.begin())
				{
					//���϶�λʧ�ܣ�Ҫ�������ź����ó��Ѿ�ʹ�ù�
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
				TRACE("��ȡĸ��%ld���ϳɹ���\r\n", (*it)->alarm.key_id);
			}
		}
	}

	return FDIR_R_SUCCEED;
}

//��ȡ�ڵ�pnode��ڵ�childnode֮��������豸
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
			TRACE("·�����أ�%ld(%s)\r\n", ((CLS_CB*)pchild->data.m_pData)->id, ((CLS_CB*)pchild->data.m_pData)->descr);
		}
		else if (pchild->data.m_type == _FI)
		{
			TRACE("·������ָʾ����%ld(%s)\r\n", ((CLS_FI*)pchild->data.m_pData)->id, ((CLS_FI*)pchild->data.m_pData)->descr);
		}
		else if (pchild->data.m_type == _LD)
		{
			TRACE("·�����ɣ�%ld(%s)\r\n", ((CLS_LD*)pchild->data.m_pData)->id, ((CLS_LD*)pchild->data.m_pData)->descr);
		}
		if (pchild != childnode && pchild->data.m_type != _LD)
		{
			GetDevsBetweenNodes(pchild, childnode, devs);
		}
		if (pchild->data.m_type == _LD)
		{
			devs.push_back(FAULT_ITEM(0, ((CLS_LD*)pchild->data.m_pData)->pid, "ld", ((CLS_LD*)pchild->data.m_pData)->descr));
			TRACE("�ҵ����ɣ�%ld(%s)\r\n", ((CLS_LD*)pchild->data.m_pData)->id, ((CLS_LD*)pchild->data.m_pData)->descr);
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

	//�����������н��߿���
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_jxcb;

	bool *bvisited_sec = new bool[lv_sec];
	bool *bvisited_nd = new bool[lv_nd];
	memset(bvisited_sec, false, sizeof(bool) * lv_sec);
	memset(bvisited_nd, false, sizeof(bool) * lv_nd);

	m_p_jxcb_Node = NULL;
	m_p_bus_Node = NULL;

	TRACE("���ҹ���ĸ�ߣ��������������ӹ�ϵ��\r\n");

	//�Թ�����������Ϊ����㽨����
	tree.TreeRemoveSubTree(tree.root);

	Tree<FDIR_TREE_NODE_DATA> tree1, tree2;
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_nodes;
	list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lst;
	map<TreeNode<FDIR_TREE_NODE_DATA>*, TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_jxcb_bus;

	//�����źŹ����豸Ϊ����
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

		cout << "��������߿��غ�ĸ��ӳ��ĸ�����Ϊ" << map_jxcb_bus.size() << endl;
		m_p_jxcb_Node = GetValidJXCBNode(map_jxcb_bus, jxcb_ret);

		if (m_p_jxcb_Node == NULL)
		{
			TRACE("δ�ҵ����߿��أ�\r\n");
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_NO_JXCB;
		}

		it_jxcb_bus = map_jxcb_bus.find(m_p_jxcb_Node);
		m_p_bus_Node = it_jxcb_bus->second; // �ҵ����߿��ض�Ӧ��ĸ��

		TreeNode<FDIR_TREE_NODE_DATA> *pRoot = m_p_bus_Node;

		while (pRoot->fathernode != NULL)
		{
			pRoot = pRoot->fathernode;
		}

		if (pRoot == tree1.root)
		{
			b_in_ind = true;
			TRACE("ĸ����IND(%ld)�ˣ�\r\n", key_cb->nd);
		}
		else
		{
			b_in_ind = false;
			TRACE("ĸ����IZND(%ld)�ˣ�\r\n", key_cb->znd);
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
				TRACE("�ҵ������źſ��������ϵ�λ��, ׼������������\r\n");
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
	//�����źŹ����豸Ϊ����
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
			TRACE("δ�ҵ����߿��أ�\r\n");
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_NO_JXCB;
		}

		it_jxcb_bus = map_jxcb_bus.find(m_p_jxcb_Node);
		m_p_bus_Node = it_jxcb_bus->second; // �ҵ����߿��ض�Ӧ��ĸ��

		tree1.ChangeRoot(m_p_bus_Node, tree, tree.root);
	}
	else if (key_dev.m_type == _BUS)
	{
		TRACE("���������ź�Ϊĸ�ߣ�\r\n");
		CLS_BUS *bus = (CLS_BUS*) key_dev.m_pData;

		TreeNode<FDIR_TREE_NODE_DATA> *proot = tree.TreeAddLeaf(NULL, bus);

		bvisited_nd[bus->ind] = true;
		build_fi_tree_BFS(bus->ind, bvisited_sec, bvisited_nd, proot, tree, true, lst_jxcb, true,true);
		m_p_bus_Node = tree.root; // ���ĸ���㼴Ϊĸ�߽��

	}
	else if (key_dev.m_type == _FI)
	{
		CLS_FI *fi = (CLS_FI*) key_dev.m_pData;
		CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(fi->sec));
		if (sec == NULL)
		{
			TRACE("δ�ҵ�����ָʾ��%ld�������߶Σ�\r\n", fi->id);
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_FAIL;
		}
		int pos = dsbob->sec_tb->FindPos(*sec);
		bool b_in_ind = false;

		//�������ָʾ����·��ӵع����й�������λ��������⣬Ŀǰ���뷨�ǽ�tree1�ĸ��ڵ�ȷ��Ϊ�������ϵĹ���ָʾ����
		//����ò�ƿ���ȷ�����������ӵ�˳����ʵ�����
		bvisited_sec[pos] = false;
		bvisited_nd[sec->ind] = true;
		bvisited_nd[sec->iznd] = false;

		//����Ĳ�����Ϊ�˱���ӹ���ָʾ������ͬʱ���ң����¹��������ɴ���
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

		//��һ�˶���������һ�˿�ʼ����
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
			TRACE("δ�ҵ����߿��أ�\r\n");
			delete[] bvisited_sec;
			delete[] bvisited_nd;
			return FDIR_R_NO_JXCB;
		}

		it_jxcb_bus = map_jxcb_bus.find(m_p_jxcb_Node);
		m_p_bus_Node = it_jxcb_bus->second; // �ҵ����߿��ض�Ӧ��ĸ��
		TreeNode<FDIR_TREE_NODE_DATA> *pRoot = m_p_bus_Node;

		while(pRoot->fathernode != NULL)
		{
			pRoot = pRoot->fathernode;
		}

		if(pRoot == tree1.root)
		{
			b_in_ind = true;
			TRACE("ĸ����IND(%ld)�ˣ�\r\n", sec->nd);
		}
		else
		{
			b_in_ind = false;
			TRACE("ĸ����IZND(%ld)�ˣ�\r\n", sec->znd);
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
				TRACE("�ҵ������źſ��������ϵ�λ��, ׼������������\r\n");
				TreeNode<FDIR_TREE_NODE_DATA> *p = tree.GetParent(*it_lst);
				tree.TreeAddSubTree(p, pSubTree);
				tree.TreeRemoveSubTree(*it_lst);
				break;
			}
		}

	}
	else
	{
		TRACE("��֧�ֵĹ��������豸���ͣ�%d\r\n", key_dev.m_type);
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
		//ĸ�߹��ϵ�ʱ����ĸ��Ϊ���ڵ㽨��������
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

	//�����Ͻ��й����źŵĽ���ҳ���
	lst_nodes.clear();
	map<TreeNode<FDIR_TREE_NODE_DATA>*, list<TreeNode<FDIR_TREE_NODE_DATA>*> > map_nodes;	//�����ź�������źŵ��������·��ӳ��

	m_Tree.BreadthFirstErgodic(m_Tree.root, lst_nodes);
	lst_edge_cb.clear();
	//���ҹ������ߵı߽翪�أ�����Թ��Ͻ��з���ʱ�õ�
	TRACE("�߽翪�أ�\r\n========================================\r\n");
	for (it_lst = lst_nodes.begin(); it_lst != lst_nodes.end(); it_lst++)
	{
		if ((*it_lst)->data.m_type == _CB)
		{
			CLS_CB *cb = (CLS_CB*) ((*it_lst)->data.m_pData);
			TRACE("���ڵ��ǣ�%ld %s\r\n", cb->id, cb->descr);
			if (cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI))
			{
				TRACE("���Ϸ��ֹ����źſ���%ld[%s]\r\n", cb->id, cb->descr);

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
			TRACE("���ڵ��ǣ�%ld %s\r\n", fi->id, fi->descr);

			if (fi->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI))
			{
				TRACE("���Ϸ��ֹ����źŹ���ָʾ��%ld[%s]\r\n", fi->id, fi->descr);

				map_nodes[*it_lst] = list<TreeNode<FDIR_TREE_NODE_DATA>*>();
			}
		}
		else if ((*it_lst)->data.m_type == _BUS)
		{
			CLS_BUS *bus = (CLS_BUS*) ((*it_lst)->data.m_pData);

			if (bus->topflags & FDIR_BUS_DETECT)
			{
				TRACE("���Ϸ��ֹ����ź�ĸ��%ld[%s]\r\n", bus->id, bus->descr);

				map_nodes[*it_lst] = list<TreeNode<FDIR_TREE_NODE_DATA>*>();
			}
		}
		else
		{
			CLS_LD *ld = (CLS_LD*) ((*it_lst)->data.m_pData);
			TRACE("���ڵ��ǣ�%ld %s\r\n", ld->id, ld->descr);

			if (ld->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI))
			{
				TRACE("���Ϸ��ֹ����źŸ���%ld[%s]\r\n", ld->id, ld->descr);

				map_nodes[*it_lst] = list<TreeNode<FDIR_TREE_NODE_DATA>*>();
			}
		}
	}

	cout << "\r\n=======================================" << endl;

	map<TreeNode<FDIR_TREE_NODE_DATA>*, list<TreeNode<FDIR_TREE_NODE_DATA>*> >::iterator it_map;

	//������������źŽ�㵽��������·��
	for (it_map = map_nodes.begin(); it_map != map_nodes.end(); it_map++)
	{
		//ͨ��������ȡ�����������·��
		TreeNode<FDIR_TREE_NODE_DATA> *pParent = it_map->first;

		do
		{
			it_map->second.push_back(pParent);

			pParent = m_Tree.GetParent(pParent);

			if (pParent != NULL)
			{
				//�ж�ĳ�������źŵ�������·�����Ƿ�������������ź�
				map<TreeNode<FDIR_TREE_NODE_DATA>*, list<TreeNode<FDIR_TREE_NODE_DATA>*> >::iterator it_find = map_nodes.find(pParent);
				if (it_find != map_nodes.end())
				{
					map_nodes.erase(it_find);
				}
			}
		} while (pParent != NULL);
	}

	//�ҳ�һ���������������źŵĹ���·����Ϊ�µĹ���
	for (it_map = map_nodes.begin(); it_map != map_nodes.end(); it_map++)
	{
		TRACE("����·����\r\n");

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
			TRACE("�Ҳ���һ�����������ź�%ld[%s]�Ĺ���·��\r\n", ((CLS_CB*) key_dev.m_pData)->id, ((CLS_CB*) key_dev.m_pData)->descr);
		}
		else if (key_dev.m_type == _FI)
		{
			TRACE("�Ҳ���һ�����������ź�%ld[%s]�Ĺ���·��\r\n", ((CLS_FI*) key_dev.m_pData)->id, ((CLS_FI*) key_dev.m_pData)->descr);
		}
		else if (key_dev.m_type == _BUS)
		{
			TRACE("�Ҳ���һ�����������ź�%ld[%s]�Ĺ���·��\r\n", ((CLS_BUS*) key_dev.m_pData)->id, ((CLS_BUS*) key_dev.m_pData)->descr);
		}
		else
		{
			TRACE("�Ҳ���һ�����������ź�%ld[%s]�Ĺ���·��\r\n", ((CLS_LD*) key_dev.m_pData)->id, ((CLS_LD*) key_dev.m_pData)->descr);
		}

		return FDIR_R_FAIL;
	}

	return FDIR_R_SUCCEED;
}


void FDIR_R::GetFaultType(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FAULT_TYPE_CONTEXT &type, bool &ld_fault)
{
	/*
	 * 	�ж��߼�
	 *
	 * 	�����ź��ռ���ɺ��жϹ��������Ƿ�������״̬Ϊ��բ�Ŀ��ء�
	 * 	1��	��������������Ϸ����ļ��㣻
	 *	2��	�����
	 *		a)	���п���״̬���ɼ����ж��Ƿ��п����¹���բ
	 *			i.	���У����ж�Ϊ˲ʱ���ϣ��غ�բ�ɹ���
	 *			ii.	���򣬽�������Ϣ���󱨵���ʽ��ʾ���û���
	 *		b)	�в��ɼ�ң�ŵĿ��أ����ڿ���д��һ��������Ϣ���ù�������ȷ��λ�Ĺ��Ͻ������֣������й��ϣ���
	 */
	bool bHasOpen = false;	//�Ƿ�������״̬Ϊ��բ�Ŀ���
	bool bHasFault = false;	//�Ƿ����¹���բ �Ŀ���
	bool bHasNoValue = false;	//�Ƿ���״̬���ɼ��Ŀ���

	ld_fault = false;
	//��ȡ����·���ϵĿ���״̬�ɼ�״��
	for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lstPath = lstPath.begin(); it_lstPath != lstPath.end(); it_lstPath++)
	{
		if ((*it_lstPath)->data.m_type == _CB)
		{
			CLS_CB *cb = (CLS_CB*) ((*it_lstPath)->data.m_pData);
			time_t tmxc = 0;	//���һ����ң�ŵ�ʱ��
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
				TRACE("����%ld(%s)״̬���ɼ�\r\n", cb->id, cb->descr);
			}

			//���¹���բ�Ŀ���
			if (cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN))
			{
				bHasFault = true;
				TRACE("����%ld(%s)Ϊ��բ����\r\n", cb->id, cb->descr);
			}

			//������״̬Ϊ��բ�Ŀ���
			if (psbob_cb.point == 0)
			{
				bHasOpen = true;
				TRACE("����%ld(%s)Ϊ��բ״̬\r\n", cb->id, cb->descr);
			}
		}
		else if ((*it_lstPath)->data.m_type == _LD)
		{
			ld_fault = true;
		}
	}

	if (bHasOpen)	//�з�բ����
	{
		type = FAULT_TYPE_SURE;
	}
	else
	{
		if (bHasNoValue)	//��״̬���ɼ��Ŀ���
		{
			type = FAULT_TYPE_POSSIBLE;
		}
		else
		{
			if (bHasFault) //���¹���բ�Ŀ���
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

//�жϿ������Ƿ��й������¹����źŵ�
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

//��ȡ��pnode��ʼ�������еĽ�㣬ֱ������ָʾ����������ĩ�˽���
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
		//������Ϊ�������ͣ��жϿ������Ƿ��й����źţ������Ƿ��ܹ��Զ���բ
		else
		{
			CLS_CB *cb = (CLS_CB *) (pchild_node->data.m_pData);
			PSBOB::CB psbob_cb = { 0 };

			//�޹�Ҫ�󿼹�����δ���ź�©��ʱ�Ĺ��϶�λ���������ε��豸���ݷ�ʵ�⣬����Ϊ��·��ͨ�����źſ���©����
			if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), (char*) &cb->pid, sizeof(PSBOB::CB), (char*) &psbob_cb) > 0
					&& HasMask(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), "status", psbob_cb.status, MENU_STATE_YX_REAL))
			{
				lst_mis_point.push_back(psbob_cb.id);
				TRACE("��·��%ld[%s]�ɼ�״̬Ϊ��ʵ�⣬���źſ���Ϊ©����������϶�λ��Χ��\r\n", psbob_cb.id, psbob_cb.name);
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
				//D5000����ָʾ����������������޷��жϹ�������
				for (std::list<FDIR_POINT_TRIG*>::iterator it_lst_trig = it_find->second.begin(); it_lst_trig != it_find->second.end(); it_lst_trig++)
				{
					//����ӵع��Ͽ��ܻ᲻���ڷ�բ�Ŀ���
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

				//��բ�¹���©���Ŀ���
				if ((cb != NULL) && (cb->topflags & FDIR_CB_DETECT_OPEN))
				{
					cb->q &= ~CB_OPEN;
					TRACE("��բ�¹���©���Ŀ���%ld[%s]\r\n", cb->id, cb->descr);
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
				//D5000����ָʾ����������������޷��жϹ�������
				for (std::list<FDIR_POINT_TRIG*>::iterator it_lst = it_find->second.begin(); it_lst != it_find->second.end(); it_lst++)
				{
					//����ӵع��Ͽ��ܻ᲻���ڷ�բ�Ŀ���
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

		//�жϲ��ҵ��Ľ��߿������ĸ��Ǵ���ģ��ҵ���Ч�Ľ��߿���

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
					TRACE("�ҵ�����պϵĽ��߿��أ�%ld��%ld\r\n", ((CLS_CB*)(pJXCB->data.m_pData))->id, temp_jxcb->id);
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
			TRACE("û���ҵ�����Ľ��߿���\r\n");
			for (map<TreeNode<FDIR_TREE_NODE_DATA>*, TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_jxcb = mapJXCB_BUS.begin(); it_jxcb != mapJXCB_BUS.end(); it_jxcb++)
			{
				CLS_CB *temp_jxcb = (CLS_CB*) (it_jxcb->first)->data.m_pData;
				if (temp_jxcb != NULL)
				{
					TRACE("�Ͽ��Ľ��߿����У�%ld\r\n", temp_jxcb->id);
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
		TRACE("������ʼ����Ϊ:%ld[%s]\r\n", ((CLS_CB*) fault_head->data.m_pData)->id, ((CLS_CB*) fault_head->data.m_pData)->descr);
		break;

	case _BUS:
		((CLS_BUS*) fault_head->data.m_pData)->topflags |= FDIR_FAULT_HEAD;
		TRACE("������ʼĸ��Ϊ:%ld[%s]\r\n", ((CLS_BUS*) fault_head->data.m_pData)->id, ((CLS_BUS*) fault_head->data.m_pData)->descr);
		break;

	case _LD:
		((CLS_LD*) fault_head->data.m_pData)->topflags |= FDIR_FAULT_HEAD;
		TRACE("������ʼ����Ϊ:%ld[%s]\r\n", ((CLS_LD*) fault_head->data.m_pData)->id, ((CLS_LD*) fault_head->data.m_pData)->descr);
		break;

	case _FI:
		((CLS_FI*) fault_head->data.m_pData)->topflags |= FDIR_FAULT_HEAD;
		TRACE("������ʼ����ָʾ��Ϊ:%ld[%s]\r\n", ((CLS_FI*) fault_head->data.m_pData)->id, ((CLS_FI*) fault_head->data.m_pData)->descr);
		break;

	default:
		break;
	}

	for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it = fault_tail.begin(); it != fault_tail.end(); it++)
	{
		switch ((*it)->data.m_type)
		{
		case _CB:
			TRACE("����ĩ�˿���:%ld[%s]\r\n", ((CLS_CB*) (*it)->data.m_pData)->id, ((CLS_CB*) (*it)->data.m_pData)->descr);
			((CLS_CB*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			break;

		case _LD:
			((CLS_LD*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			TRACE("����ĩ�˸���Ϊ:%ld[%s]\r\n", ((CLS_LD*) (*it)->data.m_pData)->id, ((CLS_LD*) (*it)->data.m_pData)->descr);
			break;

		case _BUS:
			((CLS_BUS*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			TRACE("����ĩ��ҲΪ��ĸ��:%ld[%s]\r\n", ((CLS_BUS*) (*it)->data.m_pData)->id, ((CLS_BUS*) (*it)->data.m_pData)->descr);
			break;

		case _FI:
			((CLS_FI*) (*it)->data.m_pData)->topflags |= FDIR_FAULT_TAIL;
			TRACE("����ĩ�˹���ָʾ��Ϊ:%ld[%s]\r\n", ((CLS_FI*) (*it)->data.m_pData)->id, ((CLS_FI*) (*it)->data.m_pData)->descr);
			break;

		default:
			break;
		}
	}
}

// ��ȡ����ĸ�ߺ͹����豸,��ĸ�߹��Ϻ��������Ϸֿ�����
int FDIR_R::FindFaultBusAndFaultDevs(FDIR_TREE_NODE_DATA &key_dev, int faultjxcb)
{
	lst_fault_devs.clear();
	map_node_downnode.clear();
	//���������豸��ĸ��ʱ   faultjxcbΪ-1
	if (key_dev.m_type == _BUS)
	{
		//��������Ϊĸ�߹���
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
		else//ĸ��ʧѹ�����Ƿ�ĸ�߹��ϣ����߹��ϵ���ĸ��ʧѹ
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

		//������������ֹCBһ��ֱ������ĸ���豸ʱmap_node_downnode��û������
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
				TRACE("====���������豸%ld[%s]=========\r\n", it->first.eq_id, it->first.eq_descr.c_str());
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
					TRACE("====���������豸%ld[%s]=========\r\n", it->first.eq_id, it->first.eq_descr.c_str());
					for (list<FAULT_ITEM>::const_iterator it_list = it->second.begin(); it_list != it->second.end(); it_list++)
					{
						lst_up_devs.push_back(*it_list);
					}
				}
				else
				{
					TRACE("====���������豸%ld[%s]=========\r\n", it->first.eq_id, it->first.eq_descr.c_str());
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
				TRACE("������ʼ�豸Ϊ���߿��أ�\r\n");
				b_pass_fault_head = true;
				b_pass_fault_tail = false;
				b_pass_head_tail = false;

				Find_fault_devs(cb->ind, bvisited_sec, bvisited_nd, true);

				//ind���ҵ�����ĸ��,����b_pass_fault_headΪtrue���ҵ��������豸���ᱻ��������豸�б�
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
				TRACE("������ʼ�豸�ǽ��߿��أ�\r\n");
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
			cout << "δ֪�Ĺ�����ʼ�豸" << endl;
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
		if((cb + i)->q & CB_OPEN)// cuilizhong ���濪���ڸ���֮ǰ��״̬���ָ�ʱ������Ӱ�������
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
		sub = (fdir_zone + i)->sub;// �����������豸�����豸��������
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
			printf("������������!\n");
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
//		(zn + i)->topflags &= ~ZN_FAULT;ͬһ��Դ�²�ͬ����ʱ����Ƿ�λ�˹���,���������������ǣ����ع���ʱ���ô˺������ã���ʹ�õ�����λ��ֵ
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
	map<long, FDIR_POINT_TRIG_SECTION> map_cb_faults;	//����������Ĺ����ź�ӳ�䡣keyΪ����ID�� secondΪ�뿪�ع����Ĺ����ź��б�

	m_Tree.TreeRemoveSubTree(m_Tree.root);
	m_p_jxcb_Node = NULL;
	fault_head = NULL;
	fault_tail.clear();

	cout<<"��ʼ���й��϶�λ"<<endl;
	//�������źű�ǵ������豸�ϣ������key_dev
	if (SetFaultTopflags(g_lstSecTrig, key_dev, map_cb_faults) == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}

	//ֻ���������ź���صĹ����źţ����涨λʧ��ʱҪ�������ź�ʹ�ô�����1
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
		TRACE("δ�ҵ������ź��豸�����Ĺ����źţ�\r\n");
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
	//����������
	cout<<"��ʼ����������"<<endl;
	int ret_bdtree = BuildTree(key_dev, m_Tree);
	if (ret_bdtree == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}
	//�����λʱ�Ҳ������п��أ����ȡ���ϻ��ܱ��ڴ��������Ѿ���λ�ɹ����ϣ�����ʱ������30�����ڣ��ѹ����ź��ۺ�����������λ�˹���
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
	//��ȡ����·��
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lstPath;
	cout<<"��ʼ��ȡ����·��"<<endl;
	if (GetFaultPath(lstPath, key_dev) == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}

	lst_mis_point.clear();
	//������·����©���Ĺ��ϱ�����Ϣ��ӵ�lst_mis_point�б���
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

	//��ȡ��������
	GetFaultType(lstPath, fault_type, b_ld_fault);

	if (b_ld_fault)
	{
		CLS_LD *ld = (CLS_LD*) ((*lstPath.begin())->data.m_pData);

		faultzone = -1;
		TRACE("���ɹ��ϣ����ϸ���Ϊ��%ld[%s]\r\n", ld->id, ld->descr);

		fault_head = *lstPath.begin();

		fault_tail.clear();

//		//���ɵ�����ϲ���Ҫ����
//		lst_edge_cb.clear();

		//��ȡ�������ڵ�����
		int r_zn = ld->r_zn;

		if (r_zn >= 0)
		{
			int zn_id_ptr = dsbob->r_zn_ld_tb->GetDataByPos(r_zn)->zn_id_ptr;

			for (i = 0; i < lv_fdir_zone; i++)
			{
				if (fdir_zone_tb->GetDataByPos(i)->sub == zn_id_ptr)
				{
					TRACE("�������ڵ����κ�i=%d\r\n", i);
					faultzone = i;
					break;
				}
			}
		}
		else
		{
			TRACE("����%ld��Ӧ��r_znΪ-1\r\n", ld->id);
		}
	}
	else
	{
		//��ȡ�������κ�(�ڿ������˵�������ͨ������ƥ��ķ�ʽ)
		faultzone = 0;
		list<string> lstfault, lstind, lstznd;

		fault_head = lstPath.front();

		//���ݹ�����ʼ���fault_head�ҳ����Ͻ������fault_tail

		if (fault_head->data.m_type == _BUS)
		{
          fault_tail.push_back(fault_head);
		}
		else
	        	GetDownDevs(fault_head, fault_tail);

	}

	//���ù�����ʼ������λ����Ӧ�豸��topflags��
	MaskFaultOnTree();

	cout<<"��ʼ��ȡ�����豸"<<endl;
	if(FindFaultBusAndFaultDevs(key_dev,faultjxcb) == FDIR_R_FAIL)
	{
		TRACE("��ȡ����ĸ�ߺ͹����豸ʧ�ܣ�");
		return FDIR_R_FAIL;
	}

	if (key_dev.m_type == _CB)
	{
		TRACE("\033[42m�����źſ���Ϊ%ld���¹��϶�λ�ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_CB*)key_dev.m_pData)->id, faultzone, faultjxcb);
	}
	if (key_dev.m_type == _FI)
	{
		TRACE("\033[42m�����źŹ���ָʾ��Ϊ%ld���¹��϶�λ�ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_FI*)key_dev.m_pData)->id, faultzone, faultjxcb);
	}
	else if (key_dev.m_type == _BUS)
	{
		fault_type = FAULT_TYPE_BUS;
		TRACE("\033[42m�����ź�ĸ��Ϊ%ld���¹��϶�λ�ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_BUS*)key_dev.m_pData)->id,faultzone, faultjxcb);
	}

	else if (key_dev.m_type == _LD)
	{
		TRACE("\033[42m�����źŸ���Ϊ%ld���¹��϶�λ�ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_LD*)key_dev.m_pData)->id, faultzone, faultjxcb);
	}

	//���϶�λ�ɹ�����䱾������ʹ�õĹ����ź��б�
	FillUsedFaultInfo(lstPath, g_lstSecTrig, map_cb_faults);

#if 0	//D5000��ʱ���жϲ��ԳƵ���Դ
	//������������ǽӵع��ϣ���Ҫ�жϲ��ԳƵ���Դ�ź�
	if (fault_fi_type == FAULT_JDFAULT)
	{
		CLS_BUS *jd_bus = dsbob->bus_tb->GetDataByLog(faultbus);
		if (jd_bus == NULL)
		{
			TRACE("û���ҵ������������ӵ�ĸ��,faultbus:%d\r\n", faultbus);
			return FDIR_R_FAIL;
		}
		printf("��Դ��id�ǣ� %s\r\n", (jd_bus->id));

		PSBOB::POINT psbob_point[2] = { 0 };

		if (oodbread_ra(&psbob_point, "classid", jd_bus->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), 2*sizeof(PSBOB::POINT)) > 0)
		{
			printf("��Դ�Ķ�Ӧ�Ĳ��ԳƵ���Դ�ǣ� %s�� %s \r\n", psbob_point[0].id, psbob_point[1].id);
			if(psbob_point[0].value <= 0 && psbob_point[1].value <= 0)
			{
				printf("���ԳƵ���Դ�޹����źţ�\r\n");
				return FDIR_R_FAIL;
			}
		}
		else
		{
			//�еĵ�Դû�в��ԳƵ�Դ��ң�ţ������ǲ��ԳƵ���Դ
			printf("��ȡPOINT��CLASSIDΪ%s�ļ�¼ʧ�ܣ������:%ld\r\n",jd_bus->id, _oodb_errno);
		}
	}
#endif
	//��բ���¹���©���Ŀ��أ������γ�ICB
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
				//���Ͽ����ж�����߿��أ��Ӹ���㿪ʼ���ң��п����ҵ�����ĸ�ߣ�ֻ��δ���
				//���Ͻ����豸�ҵ���ĸ�߲��ǹ���ĸ��
				if(!b_pass_fault_tail)
				{
					if (Isputjxcb)
					{
						faultbus = dsbob->bus_tb->FindPos(bus->id);
						TRACE("����ĸ�ߣ�%ld\r\n", bus->id);
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
							map_jxcb_bus[lst_jxcb.back()] = tmp_father; //�������߿��غ�ĸ�ߵĶ�Ӧ��ϵ
							Isputjxcb = false;
							cout << "ĸ����Ϣ" << bus->id << endl;
							cout << "���߿�����Ϣ" << p_jxcb->id << endl;
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
						TRACE("��������ĸ���豸Ϊ%ld[%s]\r\n", bus->id, bus->descr);
					}

				}
			}
			//������ĸ��
			else
			{
				if (b_pass_fault_head)
				{
					PSBOB::BUS psbob_bus = { 0 };
					if (oodbread_rk(&psbob_bus, &(*it_bus), const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(PSBOB::BUS)) == OO_FAIL)
					{
						printf("%s(%d):��ȡPSBOB BUS��ؼ���Ϊ%ld�ļ�¼ʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, *it_bus, _oodb_errno);
						continue;
					}
					lst_fault_devs.push_back(FAULT_ITEM(0, psbob_bus.id, "BUS", psbob_bus.describe));
					TRACE("���������ҵ��豸������ĸ��%ld[%s]\r\n", psbob_bus.id, psbob_bus.describe);
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

				//cout << "���� " << cb->id << endl;
				TRACE("������%ld[%s]\r\n", cb->id, cb->descr);
				cb->topflags &= ~FDIR_CB_NOAVAIL;

				if(cb->topflags & FDIR_FAULT_TAIL)
				{
					TRACE("���ֹ���δ�˿���%ld[%s]\r\n", cb->id, cb->descr);
					b_pass_fault_tail = true;

					if (!b_build_tree)
					{
						lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
						TRACE("�������򿪹��豸����%ld[%s]\r\n", cb->id, cb->descr);
						tmp_cb_ptr = tmp_cb_ptr->Next;
						continue;
					}
				}

				if(cb->topflags & FDIR_FAULT_HEAD)
				{
					b_pass_fault_head = true;
					TRACE("���ֹ�����ʼ����%ld[%s]\r\n", cb->id, cb->descr);
				}

				if(b_build_tree)
				{
					father = tree.TreeAddLeaf(tmp_father, cb);			
					//��ӵ�ǰ����������㣬��ֹ�ݹ���÷���ʱ���������ɴ��󣬸ô�����ܵ��¹���������ʧ�ܣ���ͬ
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
						TRACE("�������򿪹��豸����%ld[%s]\r\n", cb->id, cb->descr);
					}
				}

				if (cb->q & CB_FDHCB)
				{
					if(Isputjxcb)
					{
						TRACE("���������������������ֽ��߿���%ld[%s]\r\n", cb->id, cb->descr);
						lst_jxcb.push_back(father);
					}
					else
					{

					}

				}
				//�����Ͽ��ķǽ��߿��ػ����翪�ؼ���ֹ����·��������
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

	//��բ�豸
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
				TRACE("����բ%ld[%s]\r\n", swh->id, swh->descr);

				if (!b_build_tree)
				{
					if (b_pass_fault_head)
					{
						TRACE("��������բ�豸%ld[%s]\r\n", swh->id, swh->descr);
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

	//�߶��ϵĹ���ָʾ��
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
				TRACE("�����߶�%ld[%s]\r\n", sec->id, sec->descr);

				//���Ͽ�ʼ����Ƿ��Ǵ��߶Σ�����Ǵ��߶Σ�������ӵ������豸����Ҫ�ڲ��ҹ����豸
				bool first_head  = false;
				bool is_tail = false;
				if (sec->r_fi == -1)
				{
					if (!b_build_tree)
					{
						if(b_pass_fault_head)
						{
							TRACE("���������߶��豸%ld[%s]\r\n", sec->id, sec->descr);
							lst_fault_devs.push_back(FAULT_ITEM(0, sec->pid, "SEC", sec->descr));
						}
					}
				}
				else
				{
					//�жϵ�ǰ�߶������й���ָʾ����״̬
					list<CLS_FI*> lst_fi;
					ChildPtr *ptr2 = dsbob->r_sec_fi_tb->FindChildPtr(sec->id);

					ChildPtr *tmp_ptr2 = ptr2;

					lst_fi.clear();

					while (tmp_ptr2 != NULL)
					{
						CLS_FI *fi = dsbob->fi_tb->GetDataByPos(tmp_ptr2->Ptr);

						for (list<CLS_FI*>::iterator it = lst_fi.begin();; it++)
						{
							//������fi��ʼ��ŵ�ND���룬lst_fi�еĹ���ָʾ����λ�ô�С��������
							if (pnd->id == fi->nd)
							{
								if (it == lst_fi.end() || fi->pos < (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
							//������fi��ZND���룬lst_fi�еĹ���ָʾ����λ�ôӴ�С����
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

							TRACE("���ֹ���δ�˹���ָʾ��%ld[%s]\r\n", (*it)->id, (*it)->descr);

							if (!b_build_tree)
							{
								is_tail = true;
								TRACE("���������߶��豸����%ld[%s]\r\n", sec->id, sec->descr);
								lst_fault_devs.push_back(FAULT_ITEM((*it)->pid, sec->pid, "FI", (*it)->descr));
								lst_fault_devs.back().whole = 0;
								break;
							}
						}

						if ((*it)->topflags & FDIR_FAULT_HEAD)
						{
							b_pass_fault_head = true;
							TRACE("���ֹ�����ʼ����ָʾ��%ld[%s]\r\n", (*it)->id, (*it)->descr);
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
								TRACE("���������߶��豸����%ld[%s]\r\n", sec->id, sec->descr);
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

				//����ҵ����Ͻ����Ĺ���ָʾ����ֹͣ���������������˽�����ӵ���һ���߶�
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
						//������Ը���Ϊ���ڵ㽨�������˸��ɻ��ٴα�������������Ҫ�жϸ����Ƿ��Ǹ��ڵ㣬����Ǿ��������������ظ�
						if (tmp_father != NULL && tmp_father->data.m_type == _LD)
						{
							if (((CLS_LD *)tmp_father->data.m_pData)->id == temp_ld->id)
							{
								continue;
							}

							//�����ڵ�Ϊ����Ҫ�ڹ�������Ҫ��Ӹ���ʱ����֮ǰ����Ľڵ���Ϊ���ڵ㣬��ֹ�ݹ��㷨���ص���ײ�ʱ���򸸽ڵ�Ϊ�գ�
							//��Ӹ��ɺ󣬸��ڵ��Ϊ���ɣ������ᵼ�º����Ҳ�������·���Ӷ�ʹ��λʧ�ܣ���ͬ
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
						//TRACE("�ҵ����ɣ�%s\r\n", temp_ld->id);
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
						TRACE("�������򸺺��豸%ld[%s]\r\n", it_ld->id, it_ld->descr);
						return;
					}

					if (b_pass_fault_head)
					{
						lst_fault_devs.push_back(FAULT_ITEM(0, it_ld->pid, "LD", it_ld->descr));
						TRACE("�������򸺺��豸%ld[%s]\r\n", it_ld->id, it_ld->descr);
					}
				}
			}
		}
	}

	////D5000����������ĵ�բ
//	if (!b_build_tree && b_pass_fault_head)
//	{
//		std::deque<Element_Info> dq_elems;
//		std::deque<Element_Info>::iterator it_dq;
//		map<string, list<string> >::iterator it;
//
//		//�����������բ�Ľ�㣬ͨ���˽��ѵ�բ����
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
//				TRACE("�������򸺺��豸��բ%s[%s]\r\n", switch_cb.pointid[0] == 0 ? it_dq->id : switch_cb.pointid, switch_cb.descr);
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
				//������߿������ι�����������ȷ������
				if (map_node_downnode.size() <= 0)
				{
					bVisited_nd[ind] = false;
				}
				return;
			}
			//������ĸ��
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
					TRACE("��ȡIDΪ%ld��ĸ�߼�¼ʧ�ܣ�\r\n", id);
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
					//�жϵ�ǰ�߶������й���ָʾ����״̬
					list<CLS_FI*> lst_fi;
					ChildPtr *tmp_ptr = dsbob->r_sec_fi_tb->FindChildPtr(sec->id);
					ChildPtr *tmp_ptr2 = tmp_ptr;

					lst_fi.clear();
					while (tmp_ptr != NULL)
					{
						CLS_FI *fi = dsbob->fi_tb->GetDataByPos(tmp_ptr->Ptr);

						for (list<CLS_FI*>::iterator it = lst_fi.begin();; it++)
						{
							//������fi��ʼ��ŵ�ND���룬lst_fi�еĹ���ָʾ����λ�ô�С��������
							if (pnd->id == fi->nd)
							{
								if (it == lst_fi.end() || fi->pos < (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
							//������fi��ZND���룬lst_fi�еĹ���ָʾ����λ�ôӴ�С����
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

							//���� 20150330 �������ָʾ�������й���ָʾ�������߶���ɫ�쳣����
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
			//���� 20150130 ��������߹�������������ɵ�����
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
					TRACE("%ld[%s]Ϊ���߿��أ����óɱ߽翪�ء�\r\n", map_node_downnode[parent].back().eq_id, map_node_downnode[parent].back().eq_descr.c_str());
					map_node_downnode[parent].back().border = true;
				}

				if ((cb->q & CB_OPEN) && !(cb->topflags & (FDIR_CB_DETECT_FAULT | FDIR_CB_DETECT_OPEN | FDIR_CB_DETECT_FI)))
				{
					TRACE("%ld[%s]���أ����óɱ߽翪�ء�\r\n", map_node_downnode[parent].back().eq_id, map_node_downnode[parent].back().eq_descr.c_str());
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

/////////////////// ���������豸
void FDIR_R::Find_fault_devs(int ind, bool *bvisited, bool *bvisited_nd, bool save_ld)
{

	CLS_ND *pnd = dsbob->nd_tb->GetTable() + ind; //  �ýڵ��ָ��

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
				//���Ͽ����ж�����߿��أ��Ӹ���㿪ʼ���ң��п����ҵ�����ĸ�ߣ�ֻ��δ������Ͻ����豸�ҵ���ĸ�߲��ǹ���ĸ��
				if (!b_pass_fault_tail)
				{
					faultbus = dsbob->bus_tb->FindPos(bus->id);
					TRACE("����ĸ�ߣ�%ld\r\n", bus->id);
				}
			}
			//������ĸ��
			else
			{
				if (b_pass_fault_head)
				{
					PSBOB::BUS psbob_bus = { 0 };
					if (oodbread_rk(&psbob_bus, &(*it_bus), const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(PSBOB::BUS)) == OO_FAIL)
					{
						printf("%s(%d):��ȡPSBOB BUS��ؼ���Ϊ%s�ļ�¼ʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, *it_bus, _oodb_errno);
						continue;
					}
					lst_fault_devs.push_back(FAULT_ITEM(0, psbob_bus.id, "BUS", psbob_bus.describe));
					TRACE("���������ҵ��豸������ĸ��%ld[%s]\r\n", psbob_bus.id, psbob_bus.describe);
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
				cout << "���� " << cb->id << endl;
				cb->topflags &= ~FDIR_CB_NOAVAIL;

				if (cb->topflags & FDIR_FAULT_TAIL)
				{
					TRACE("���ֹ���δ�˿���%ld[%s]\r\n", cb->id, cb->descr);
					b_pass_fault_tail = true;
					lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
					TRACE("�������򿪹��豸����%ld[%s]\r\n", cb->id, cb->descr);
					tmp_cb_ptr = tmp_cb_ptr->Next;
					continue;
				}

				if (cb->topflags & FDIR_FAULT_HEAD)
				{
					b_pass_fault_head = true;
					TRACE("���ֹ�����ʼ����%ld[%s]\r\n", cb->id, cb->descr);
				}

				if (b_pass_fault_head)
				{
					lst_fault_devs.push_back(FAULT_ITEM(0, cb->pid, "CB", cb->descr));
					TRACE("�������򿪹��豸����%ld[%s]\r\n", cb->id, cb->descr);
				}

				//�����Ͽ��Ŀ��ػ����翪�ؼ���ֹ����·��������
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

	//�߶��ϵĹ���ָʾ��
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

				//���Ͽ�ʼ����Ƿ��Ǵ��߶Σ�����Ǵ��߶Σ�������ӵ������豸����Ҫ�ٲ��ҹ����豸
				bool first_head = false;
				bool is_tail = false;
				if (sec->r_fi == -1)
				{
					if (b_pass_fault_head)
					{
						TRACE("���������߶��豸����%ld[%s]\r\n", sec->id, sec->descr);
						lst_fault_devs.push_back(FAULT_ITEM(0, sec->pid, "SEC", sec->descr));
					}
				}
				else
				{
					//�жϵ�ǰ�߶������й���ָʾ����״̬
					list<CLS_FI*> lst_fi;
					ChildPtr *ptr2 = dsbob->r_sec_fi_tb->FindChildPtr(sec->id);
					ChildPtr *tmp_ptr2 = ptr2;

					lst_fi.clear();
					while (tmp_ptr2 != NULL)
					{
						CLS_FI *fi = dsbob->fi_tb->GetDataByPos(tmp_ptr2->Ptr);

						for (list<CLS_FI*>::iterator it = lst_fi.begin();; it++)
						{
							//������fi��ʼ��ŵ�ND���룬lst_fi�еĹ���ָʾ����λ�ô�С��������
							if (pnd->id == fi->nd)
							{
								if (it == lst_fi.end() || fi->pos < (*it)->pos)
								{
									lst_fi.insert(it, fi);
									break;
								}
							}
							//������fi��ZND���룬lst_fi�еĹ���ָʾ����λ�ôӴ�С����
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
							TRACE("���ֹ���δ�˹���ָʾ��%ld[%s]\r\n", (*it)->id, (*it)->descr);
							is_tail = true;
							TRACE("���������߶��豸����%ld[%s]\r\n", sec->id, sec->descr);
							lst_fault_devs.push_back(FAULT_ITEM((*it)->pid, sec->pid, "FI", (*it)->descr));
							lst_fault_devs.back().whole = 0;
							break;
						}

						if ((*it)->topflags & FDIR_FAULT_HEAD)
						{
							b_pass_fault_head = true;
							TRACE("���ֹ�����ʼ����ָʾ��%ld[%s]\r\n", (*it)->id, (*it)->descr);
							first_head = true;
						}

						if (b_pass_fault_head)
						{
							TRACE("���������߶��豸����%ld[%s]\r\n", sec->id, sec->descr);
							lst_fault_devs.push_back(FAULT_ITEM((*it)->pid, sec->pid, "FI", (*it)->descr));
							lst_fault_devs.back().whole = 0;
						}

					}
				}

				//����ҵ����Ͻ����Ĺ���ָʾ����ֹͣ���������������˽�����ӵ���һ���߶�
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
					TRACE("�������򸺺��豸%ld[%s]\r\n", it_ld->id, it_ld->descr);
					return;
				}
				if (b_pass_fault_head)
				{
					lst_fault_devs.push_back(FAULT_ITEM(0, it_ld->pid, "LD", it_ld->descr));
					TRACE("�������򸺺��豸%ld[%s]\r\n", it_ld->id, it_ld->descr);
				}
			}
		}
	}

	////����������ĵ�բ
//	if (b_pass_fault_head)
//	{
//		std::deque<Element_Info> dq_elems;
//		std::deque<Element_Info>::iterator it_dq;
//		map<string, list<string> >::iterator it;
//
//		//�����������բ�Ľ�㣬ͨ���˽��ѵ�բ����
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
//				TRACE("�������򸺺��豸��բ%s[%s]\r\n", switch_cb.pointid[0] == 0 ? it_dq->id : switch_cb.pointid, switch_cb.descr);
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

	//��������ӿ��ؽ��
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

		TRACE("����%ld[%s]����һ������Ϊ%ld\r\n", cb->id, cb->descr, pParent == NULL ? 0 : ((CLS_CB*)pParent->data.m_pData)->id);

		if (cb->q & CB_FDHCB)
		{
			TRACE("���ֽ��߿���%ld[%s]\r\n", cb->id, cb->descr);
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

//����ָ����������ӵ��豸��CB��XF��LD��BUS��
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
		//�ý����δ����LD
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

//D5000����Ҫ����ʵ���źŵ���
void FDIR_R::EraseNotUseFaultsng(std::list<long> &faultsng, bool ispointid)
{
	for (FDIR_POINT_TRIG_SECTION::iterator it_sec = g_lstSecTrig.begin(); it_sec != g_lstSecTrig.end(); it_sec++)
	{
		cout << "�����ź�id�ǣ� " << (*it_sec)->alarm.key_id << "����stid�ǣ� " << (*it_sec)->alarm.st_id << endl;
//		if (strcmp((*it_sec)->alarm.tabname, "point") == 0)		//D5000��ʱֻ���ǿ����¹���բ
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
				cout << "����δʹ�õĹ���ָʾ���źţ� " << (*it_sec)->alarm.key_id << endl;
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
				TRACE("��ȡ���ݳ���%ld��errno��%d,%s\r\n", it_lst->eq_id, _oodb_errno, __FUNCTION__);
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
	//����ƽ�����20111218��������ĸ�ߵĽ�㲻��ͳ�������ӵĿ���
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
	//����ƽ�����20111218��������ĸ�ߵĽ�㲻��ͳ�������ӵĿ���
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

#if 1	//D5000���ر���û�е�բ
	ChildPtr *ptr = dsbob->r_nd_cb_tb->FindChildPtr((r_nd_cb + icb)->nd_id);
	ChildPtr *ptr1 = ptr;

	while (ptr != NULL)
	{
		fdir_zone_cb->icb = ptr->Ptr;
		fdir_zone_cb->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE_CB")), fdir_zone_cb_tb->GetCount() + 1);

		if (fdir_zone_cb_tb->Insert(*fdir_zone_cb) == -1) // �������
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
		//20111218����ƽ��ǰ���γ����ε�ʱ������˵�բ����������������ι����Ŀ���ʱ����բ����������
		tmp_cb = dsbob->cb_tb->Find(CLS_CB((r_nd_cb + icb + j)->cb_id));
		if (tmp_cb->q & CB_SWITCH)
		{
			if (zn_type == _FDIR_SO)
			{
				ChildPtr *ptr = NULL, *tmp_ptr = NULL;

				//���ҵ�բ����һ�����ӵĽ��߿���
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
		//����ƽ�����20111218��������ĸ�ߵĽ�㲻��ͳ�������ӵĿ���
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
		//����ƽ�����20111218��������ĸ�ߵĽ�㲻��ͳ�������ӵĿ���
		////////////////////////////////////////////

		fdir_zone_cb->id = get_id(const_cast<TB_DESCR*>(g_db_fdir->GetTB("FDIR_ZONE_CB")), fdir_zone_cb_tb->GetCount() + 1);//fdir_zone_cb_tb->GetCount(); //�������¼idΪ��ǰ��¼��
		if(fdir_zone_cb->icb == -1)
			fdir_zone_cb->icb = (r_nd_cb + icb + j)->cb_id_ptr; // ����icb=���ر������
		if (fdir_zone_cb_tb->Insert(*fdir_zone_cb) == -1) // �������
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

	for (int i = 0; i < lv_fdir_zone; i++) //ѭ�����е�����
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
	for(i = 0;i < lv_fdir_zone;i++)//ѭ�����е�����
	{
		//�޵������벢�Ҳ��ǹ������ε����κ��Գ�Ϊ��������
		if((fdir_zone + i)->icb == -1 && (fdir_zone +i)->type != _FDIR_SO
				&& (fdir_zone +i)->type != _FDIR_BUS)
		{
			continue;
		}

		cbptr = (fdir_zone + i)->cbptr;//�õ�������ʼ���صı��
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
	cout << "д��FDIR RTDB�⡭" << endl;

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

//����Դ�������쵽�����ε�������������
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

				if (!((fdir_cb + icb)->topflags & FDIR_CB_OPEN)) //�����Ǻ�״̬
				{
//					if(bDisplay) TRACE("������������%s\r\n", (fdir_cb + icb)->descr);
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
//					if(bDisplay) TRACE("����%sΪ�Ͽ�״̬��\r\n", (fdir_cb + icb)->descr);
				}
			}
			else
			{
//				if(bDisplay) TRACE("����%s�Ѿ����������\r\n", (fdir_cb + icb)->descr);
			}
		}
	}
	else
	{
//		if(bDisplay) TRACE("����%d�Ѿ����������\r\n", izone);
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

	//���¹ʵ��µ���բ����բ�Ŀ��رպ�
	for (list<int>::iterator it = lst_fault_open_cb.begin(); it != lst_fault_open_cb.end(); it++)
	{
		(cb + *it)->q &= ~CB_OPEN;
		TRACE("%s:��բ����%ld[%s]\r\n", __FUNCTION__, (cb + *it)->id, (cb + *it)->descr);
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
	for (i = 0; i < lv_fdir_zone; i++) //ѭ��ÿһ������
	{
		type = (fdir_zone + i)->type; // ��������
		if (type == _FDIR_SO) // ��Դ����
		{

//			if (strcmp((fdir_zone + i)->descr, "����.���Գ�վ/10kV.ĸ��") == 0)
//			{
//				TRACE("!!!!!!!!!!!!!!!!����ĸ��%ld\r\n", (fdir_zone + i)->zn_id);
//				bDisplay = true;
//			}
//			else
//			{
//				bDisplay = false;
//			}

			(fdir_zone + i)->icb = -1; // ��ǰ���ε�icb��-1
#if 0
			tzone[0] = i; // iΪ��ǰ���κ�
			k = current = 1;
			while (current) // �µ��������θ���Ϊcurrent����Ϊ0���������
			{
				k0 = k; // kΪtzone������Ч�����θ�����
				for (j = k0 - current; j < k0; j++) // j=0,j<1;
				{
					izone = tzone[j]; //δ������������ε����κ�
					(fdir_zone + izone)->q &= ~FDIR_ZONE_UNPROC; //��ʾ�Ѵ���
					cbnum = (fdir_zone + izone)->cbnum; // ���ζ�Ӧ������
					cbptr = (fdir_zone + izone)->cbptr; //���ζ�Ӧ�ĵ�һ�����ص�����ţ���fdir_zone_cb��
					for (m = 0; m < cbnum; m++) // ѭ������ÿһ������
					{
						icb = (fdir_zone_cb + cbptr + m)->icb; // �ӹ�����ȡÿһ�����ص������
						if ((fdir_cb + icb)->q & FDIR_CB_UNPROC)
						{
							if (!((fdir_cb + icb)->topflags & FDIR_CB_OPEN)) // �����Ǻ�״̬
							{
								zone1 = (fdir_cb + icb)->ind; // �����׶����κ�
								zone2 = (fdir_cb + icb)->iznd; //����ĩ�����κ�
								if (izone == zone1) // zone1==��ǰ��Դ����
								{
									tzone[k++] = zone2; //��zone2Ϊ���ڵ�����
									(fdir_zone + zone2)->icb = icb; //icbΪ����������������Ŀ���
									(fdir_zone + zone2)->q &= ~FDIR_ZONE_UNPROC;
								}
								else if (izone == zone2) //�����湦����ͬ
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
//���ɷǹ���ͣ������ָ�����
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
		//�������׶˿���
		// cuilizhong ��������翪�غ�բ�����϶κ������׶�֮���п���A��բ��
		// cuilizhong ��������·������ϣ��׶˿��ؾ�û�з�բ�����׶˿���û�����壬Ӧ�ðѿ���A��բ�Ŷԡ�
		fdir_restore->id = fdir_restore_tb2.size();
//		temp = (fdir_cb + faulticb)->descr;//faulticb�׶˿��غ�
//		temp = temp + "��բ";
//		strcpy(fdir_restore->descr,temp);//
		sprintf(fdir_restore->descr, "%s ��բ", (fdir_cb + faulticb)->descr);

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
#if 0		//����ƽ20111213-���߿��ز���ֱ�Ӻ�բ
		if (fdir_restore_tb->Insert(*fdir_restore) == -1)
		{
			return FDIR_R_FAIL;
		}
#endif
		//�����������ι���·��
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

		 //���ں��ݹ����޷��������׶˿��ؽ��п��ƣ����������׶˿����޷���Ϊ�ָ�����
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
		 temp = temp + "��բ";
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
		 temp = temp + "��բ";
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

	for (i = 0; i < cbnum0; i++) // �Թ�������ÿ���߽翪�ض�Ӧ���������Σ��ֱ���лָ�
	{ //14
		int border_icb = 0;	//�������α߽翪��

		icb = (fdir_zone_cb + cbptr0 + i)->icb;
		border_icb = icb;

		if ((fdir_zone + faultzone)->icb == icb) // cuilizhong  (fdir_zone + faultzone)->icb�������������εĿ��أ�
			continue;

		printf("�߽翪�أ�%s�������λָ�������\r\n", (fdir_cb + icb)->descr);
		map_cb_restore[(fdir_cb + border_icb)->cb_id] = false;

		downbzones.clear();
		if (!((fdir_cb + icb)->q & FDIR_CB_FDLCB) && !((fdir_cb + icb)->topflags & FDIR_CB_OPEN0)) //����ÿ���ԭ���Ǻϵ�
		{ //13
			zone1 = (fdir_cb + icb)->ind; // ���ض�Ӧ����������
			zone2 = (fdir_cb + icb)->iznd; // ���ض�Ӧ����������
			if (zone1 == faultzone)
				izone = zone2;
			if (zone2 == faultzone)
				izone = zone1; // izoneΪ���ض�Ӧ����һ�����Σ����뵽��������
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
			for (k = 0; k < downbzones.size(); k++) // ���������ν���ѭ��
			{ //12
//				if (downrestore > 0) // �ҵ�һ���ɻָ���������բ����ֹͣ
//					break;

				//�ñ߽翪�ض�Ӧ�����������лָ�����
//				if(downrestore > 0)
//					map_cb_restore[(fdir_cb + border_icb)->cb_id] = true;

				iizone = downbzones[k]; // ȡһ����������
				cbptr = (fdir_zone + iizone)->cbptr;
				cbnum = (fdir_zone + iizone)->cbnum;
				for (j = 0; j < cbnum; j++) // ���������εı߽翪�ؽ���ѭ��
				{ //11
					icb = (fdir_zone_cb + cbptr + j)->icb;
					if ((fdir_cb + icb)->q & FDIR_CB_FDLCB)
						continue;
					zone1 = (fdir_cb + icb)->ind; // ���ر�������
					zone2 = (fdir_cb + icb)->iznd; // ������һ������
					if ((fdir_cb + icb)->topflags & FDIR_CB_OPEN) // ����ǿ����Ƿ�բ״̬
					{ //10
#if 0	//����ƽ��20111202ע�͵�
						if (zone1 == iizone) // zone2Ϊ��һ�������
							if (!((fdir_zone + zone2)->topflags & FDIR_ZONE_DEAD)) // ���ηǹ���״̬
							{ //8
								fdir_restore->id = fdir_restore_tb->GetCount();
//								temp = (fdir_cb + icb)->descr;
//								temp = temp + "��բ";
//								strcpy(fdir_restore->descr,temp);//
								sprintf(fdir_restore->descr, "%s ��բ", (fdir_cb + icb)->descr);

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
//								temp = temp + "��բ";
//								strcpy(fdir_restore->descr,temp);//
								sprintf(fdir_restore->descr, "%s ��բ", (fdir_cb + icb)->descr);

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
#else	//����ƽ��20111202���
						int idownzone = -1; //�������������ڵ�����

						if (iizone == zone2)
							idownzone = zone1;
						else if (iizone == zone1)
							idownzone = zone2;
						else
							idownzone = -1;

						if (idownzone != -1)
						{
#if 0
							if (!((fdir_zone + idownzone)->topflags & FDIR_ZONE_DEAD)) // ���ηǹ���״̬
#else
							if((fdir_zone + idownzone)->icb != -1)
#endif
							{
								fdir_restore->id = 0;//fdir_restore_tb[izone].GetCount();

								sprintf(fdir_restore->descr, "%s ��բ", (fdir_cb + icb)->descr);

								cout << fdir_restore->descr << endl;

								while ((fdir_zone + idownzone)->type != _FDIR_SO
										&& (fdir_zone + idownzone)->type != _FDIR_BUS
										&& idownzone < lv_fdir_zone)
								{
									if((fdir_zone + idownzone)->icb == -1)
									{
										printf("%s �����޵������뿪�أ�����\r\n", (fdir_zone + idownzone)->descr);
										break;
									}

									idownzone = FindUpZone((fdir_zone + idownzone)->icb);
								}

								if ((fdir_zone + idownzone)->type == _FDIR_SO)
								{
									fdir_restore->so_id = (so + (fdir_zone + idownzone)->sub)->id;
									cout << "�˻ָ�����ʹ�õ�Դ��"	<< (so + (fdir_zone + idownzone)->sub)->descr << endl;
								}
								else if ((fdir_zone + idownzone)->type == _FDIR_BUS)
								{
									CLS_BUS *bus = dsbob->bus_tb->GetTable();

									fdir_restore->so_id = (bus + (fdir_zone + idownzone)->sub)->id;
									cout << "�˻ָ�����ʹ�õ�Դ��"	<< (bus +(fdir_zone + idownzone)->sub)->descr << endl;
								}
								else
								{
									cout << "δ׷�ٵ��˻ָ������ĵ�Դ��" << endl;
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
	cout << "FDIR_R���¿���״̬��" << endl;

	lv_cb = dsbob->cb_tb->GetCount();

	//���¿���״̬
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

			//��ȡ���ض�Ӧ��ң����Ϣ  oodbread_apΪ�߼������� idigitalΪ���ز���
			if (oodbread_rk(&psbob_cb, &cb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
			{
				cout << "���¿���" << cb->id << "״̬ʧ��,errno:" << _oodb_errno << endl;

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
				//20111201����ƽ
				//���¹���բ�����úϣ��Ա�������εĵ������뿪��
				pthread_mutex_lock(&trig_list_mutex);
				for (FDIR_POINT_TRIG_SECTION::iterator it = g_lstSecTrig.begin(); it != g_lstSecTrig.end(); it++)
				{
					if ((*it)->alarm.dev == ALARM_BREAKER && cb->pid == (*it)->psbob_data.point.id)
					{
						TRACE("����բ���أ�cb:%ld(%s)\r\n", cb->id, cb->descr);
						cb->q &= ~CB_OPEN;
						break;
					}
				}
				pthread_mutex_unlock(&trig_list_mutex);

				//20111201����ƽ
				//���¹���բ�����úϣ��Ա�������εĵ������뿪��
				for (list<FAULT_ITEM>::iterator it_lst_p = lst_point.begin(); it_lst_p != lst_point.end(); it_lst_p++)
				{
					if (it_lst_p->eq_id == cb->pid)
					{
						TRACE("����բ���أ�cb:%ld, pointid:%s, point_name:%ld\r\n", cb->pid, cb->descr, it_lst_p->id);
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
			TRACE("���µ�բ%ld(%ld)[%s]״̬ʧ�ܣ�\r\n", swh->id, swh->pid, swh->descr);

			return false;
		}

		if (psbob_switch.st_id <= 0)
		{
			TRACE("��բ%ld�ĳ�վID����", psbob_switch.id);
			return false;
		}

		if (oodbread_rk(&psbob_st, &psbob_switch.st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) == OO_FAIL)
		{
			TRACE("��ȡ%ld��վ��Ϣʧ�ܣ�\r\n", psbob_switch.st_id);

			return false;
		}

		//���߳�վ�ĵ�բȫ����Ϊ�պ�״̬
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
	cout << "FDIR_R���ӵ�Դ��" << endl;

	dsbob->so_tb->Empty();

	lv_bus = dsbob->bus_tb->GetCount();

	for (int i = 0; i < lv_bus; i++)
	{
		int ind = dsbob->bus_tb->Data[i].ind;

		if (ind < 0)
			continue; //�޽ڵ�

		PSBOB::BUS psbob_bus = {0};

		//��ȡĸ�߶�Ӧ��ң����Ϣ
		if (oodbread_rk(&psbob_bus, &dsbob->bus_tb->Data[i].pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(psbob_bus)) <= 0)
		{
			cout << "��ȡĸ��" << dsbob->bus_tb->Data[i].id << "��ѹ��Ϣʧ��,errno:" << _oodb_errno << endl;

			continue; //����ĸ���޵�ѹ��Ϣ
		}

		if (psbob_bus.v < 1)
		{
			//�Ƿ���Ҫ��ĸ�ߵ�ѹ����
			bool isset = false;

			for (list<long>::iterator it_lst = lst_loss_vl_bus.begin(); it_lst != lst_loss_vl_bus.end(); it_lst++)
			{
				if ((*it_lst) == dsbob->bus_tb->Data[i].id)
				{
					TRACE("��ʧѹĸ�ߣ�%ld\r\n", *it_lst);
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
					if ((strcmp((*it)->alarm.fault_info, "ĸ��ʧѹ") == 0) && (dsbob->bus_tb->Data[i].id == (*it)->alarm.key_id))
					{
						TRACE("��ʧѹĸ�ߣ�%ld\r\n", dsbob->bus_tb->Data[i].id);
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
//			printf("��ѹԽ���޵�ĸ��IDΪ%lds\r\n", data_start.source.bus_id);
//		}
//	}

	lv_so = dsbob->so_tb->GetCount();
	cout << "���ӵ�Դend��" << endl;
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
