/*
 * FDIRR2.cpp
 *
 *  Created on: 2012-4-27
 *      Author: BetaKDE
 */

#include "FDIRR2.h"

ostream& operator<<(ostream& os, const DATA_FI data)
{
	os << data.id;

	return os;
}

ostream& operator <<(ostream &os, const TreeData &data)
{
	return os;
}

FDIR_R2::~FDIR_R2()
{
	// TODO Auto-generated destructor stub
}

int FDIR_R2::ReIsolate(const long cb, const char* fault_id)
{
	cout << __FUNCTION__ << "......" << endl;

	assert(fault_id != NULL);

	fdir_restore_tb.clear();
	fdir_isolate_tb->Empty();
	fdir_isolate_tb2->Empty();
	map_cb_restore.clear();
	lst_iso_failed.clear();

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();
	int i = 0;

	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	char *buf = NULL;

	sprintf(sql, "SELECT ZONE FROM FDIR_FDIRECT WHERE ID=\'%s\'", fault_id);
	cout << sql <<endl;

	//��ȡ�ù��ϵ�ԭʼ��������
	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		if (rec_num <= 0)
			return FDIR_R_FAIL;
		else
		{
			faultzone = *(int*)buf;
			g_oci->Readdata_Free();
		}
	}
	else
	{
		cout << "ExecSQL failed:" << sql << endl;
		return FDIR_R_FAIL;
	}

	sprintf(sql, "SELECT ICB,CB FROM FDIR_ISOLATE_HIS WHERE ID=\'%s\' AND STATUS=%d", fault_id, STATUS_ISOLATE_FAIL);
	cout << sql <<endl;

	//��ȡ�ù��ϸ���ʧ�ܵĿ���
	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		if (rec_num <= 0)
			return FDIR_R_FAIL;
		else
		{
			for(i = 0; i < rec_num; i++)
			{
				char icb_buf[50], cb_buf[50];

				bzero(icb_buf, sizeof(icb_buf));
				memcpy(icb_buf, buf + i * (attrs[0].col_width + attrs[1].col_width), attrs[0].col_width);
				bzero(cb_buf, sizeof(cb_buf));
				memcpy(cb_buf, buf + i * (attrs[0].col_width + attrs[1].col_width) + attrs[0].col_width, attrs[1].col_width);

				cout << "����" << cb_buf << "ң��ʧ�ܣ�" << *(int*)icb_buf << endl;
				lst_iso_failed[*(int*)icb_buf] = *(long*)cb_buf;
			}
		}
	}
	else
	{
		cout << "ExecSQL failed:" << sql << endl;
		return FDIR_R_FAIL;
	}

	if (FormIsoSteps() == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}

	if (FormResSteps() == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}

	return FDIR_R_SUCCEED;
}

int FDIR_R2::GetZoneOfFI(const long id)
{
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	int i = 0;

	//��ȡ�ù����ź����ڵ�����
	CLS_FI *fi = dsbob->fi_tb->Find(CLS_FI(id));
	if (fi == NULL)
		return -1;

	CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(fi->sec));

	int lv_fdir_zone = fdir_zone_tb->GetCount();

	for (i = 0; i < lv_fdir_zone; i++)
	{
		if ((fdir_zone + i)->zn_id == sec->zn_id)
			break;
	}

	return i < lv_fdir_zone ? i : -1;
}

//������ṹ
void FDIR_R2::clear_tree()
{
	m_Tree.TreeRemoveSubTree(m_Tree.root);
	m_p_jxcb_Node = NULL;
	fault_head = NULL;
	fault_tail.clear();
}
#if 0
//�ж�ĳ�������ź��Ƿ���
bool FDIR_R2::is_wrong_fi(const char *fi_id)
{
	assert(fi_id != NULL);

	map<string, TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it = m_fi_node.find(fi_id);

	if(it == m_fi_node.end()) return true;

	//���ж������������Ϲ������Ƿ�û�й����ź�
	TreeNode<DATA_FI>* pnode = it->second;
	int cnt_up = 0;//����û�й����źŵĹ���ָʾ������

	for (int i = 0; /*i < 2*/; i++)
	{
		pnode = m_tree_fi.GetParent(pnode);

		if(pnode == NULL) return false;
		if(pnode->data.id.size() <= 0) continue;//������û�й���ָʾ��
		if(pnode->data.bfault) return false;
		else
		{
			cnt_up ++;

			if(cnt_up >= 2) break;
		}
	}

	//���ж�����һ������ָʾ���Ƿ�û�й����ź�
	list<TreeNode<DATA_FI>*> lst_dev;

	GetDownDevs(it->second, lst_dev);

	for (list<TreeNode<DATA_FI>*>::iterator it_fi = lst_dev.begin(); it_fi != lst_dev.end(); it_fi++)
	{
		if ((*it_fi)->data.equ_type == EQU_CB && (*it_fi)->data.id.size() > 0 && (*it_fi)->data.bfault)
			return false;
	}

	return true;
}

void FDIR_R2::GetDownDevs(TreeNode<DATA_FI>* pnode, list<TreeNode<DATA_FI>*> &lst_dev)
{
	TreeNode<DATA_FI>* pchild_node = m_tree_fi.GetChildNode(pnode);

	if(pchild_node == NULL)
	{
		//δ�˸���
		if(pnode->data.equ_type == EQU_LD)
		{
			lst_dev.push_back(pnode);

			return;
		}
		//δ���޹���ָʾ���Ŀ���
		else if(pnode->data.equ_type == EQU_CB && pnode->data.id.size() <= 0)
		{
			lst_dev.push_back(pnode);

			return;
		}
	}

	while(pchild_node != NULL)
	{
		if ((pchild_node->data.equ_type == EQU_CB && pchild_node->data.id.size() > 0)
				|| (pchild_node->data.equ_type == EQU_SEC))
		{
			lst_dev.push_back(pchild_node);
		}
		else
		{
			GetDownDevs(pchild_node, lst_dev);
		}

		pchild_node = m_tree_fi.GetNextBrotherNode(pchild_node);
	}
}
#endif

#if 0
int FDIR_R2::FaultDetection()
{
	lst_point.clear();
	lst_mis_point.clear();
	lst_err_point.clear();
	lst_protected.clear();

	int lv_cb = dsbob->cb_tb->GetCount();
	int lv_zone = fdir_zone_tb->GetCount();
	PSBOB::POINT point = { 0 };
	int i = 0;

	for(i = 0; i < lv_cb; i++)
	{
		dsbob->cb_tb->GetDataByPos(i)->topflags |= FDIR_CB_UNPROC;
	}

	for(i = 0; i < lv_zone; i++)
	{
		fdir_zone_tb->GetDataByPos(i)->topflags |= FDIR_ZONE_UNPROC;
	}

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();

	if (oodbread_rk(&point, data_start.source.bus_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("point")), sizeof(PSBOB::POINT)) < 0)
	{
		printf("oodbread_rk(%s) failed, errno:%d, line:%d\r\n", data_start.source.bus_id, _oodb_errno, __LINE__);

		return FDIR_R_FAIL;
	}

	int pos = -1;

	if (point.groupid[0] == 0) //�߶��ϵĹ����ź�
	{
		//��ȡ�ù����ź����ڵ�����
		CLS_FI *fi = dsbob->fi_tb->Find(CLS_FI(data_start.source.bus_id));
		CLS_SEC *sec = dsbob->sec_tb->Find(CLS_SEC(fi->sec));

		int lv_fdir_zone = fdir_zone_tb->GetCount();

		for (i = 0; i < lv_fdir_zone; i++)
		{
			if (strcmp((fdir_zone + i)->zn_id, sec->zn_id) == 0)
				break;
		}

		pos = (fdir_zone + i)->icb;

	}
	else //�����ϵ��¹����ź�
	{
		pos = dsbob->cb_tb->FindPos(point.groupid);
	}

	int tmp_zone = -1;
	int pre_zone = -1; //������������ĸ�����ε�ֱ����������
	int tmp_pos = pos;

	//��λ�ù������ڵ�ĸ��
	while (true)
	{
		if (tmp_pos < 0)
		{
			cout << "���϶�λʧ��" << endl;
			return FDIR_R_FAIL;
		}

		tmp_zone = FindUpZone(tmp_pos); //Ѱ���������Σ��������κ�

		if (tmp_zone == -1)
		{
			cout << __FUNCTION__ << __LINE__<<"���϶�λʧ��" << endl;
			return FDIR_R_FAIL;
		}

		DisplayZoneCB(tmp_zone);

		cout << (fdir_zone + tmp_zone)->id << endl;
		if ((fdir_zone + tmp_zone)->type == _FDIR_SO || (fdir_zone + tmp_zone)->type == _FDIR_BUS)
		{
			faultbus = tmp_zone;
			break;
		}

		pre_zone = tmp_zone;

		tmp_pos = (fdir_zone + tmp_zone)->icb;
	}

	string fault_dv = g_map_fi_dv[data_start.source.bus_id];

	printf("!!!!!!!!!!!����%s��������[%s]!!!!!!!!!!\r\n", fault_dv.c_str(), data_start.source.bus_id);

	list<string> lst_fi(g_map_trig_fi[fault_dv]); //�ù������������µ����й����ź�

	lst_point = lst_fi;

	int lv_sec = dsbob->sec_tb->GetCount();
	bool *bvisited = new bool[lv_sec];

	memset(bvisited, false, lv_sec * sizeof(bool));

	CLS_BUS *bus = dsbob->bus_tb->Find(CLS_BUS((fdir_zone + faultbus)->zn_id));
	int ind = dsbob->nd_tb->FindPos(bus->nd);

	int icb = -1;

	if (pre_zone != -1)
		icb = fdir_zone_tb->GetDataByPos(pre_zone)->icb;
	else
		icb = pos;

	CLS_CB *cb = dsbob->cb_tb->GetDataByPos(icb);

	//���߿����Ͽ����й���ָʾ��
	TreeNode<DATA_FI>* father = m_tree_fi.root;
	if (oodbread_aa(point.id, "groupid", cb->id, "id", const_cast<TB_DESCR *>(g_db_psbob->GetTB("point")), sizeof(point.id)) > 0)
	{
		father = m_tree_fi.TreeAddLeaf(father, DATA_FI(point.id, cb->id, EQU_CB));
		m_fi_node[point.id] = father;
	}

	cb->topflags &= ~FDIR_CB_UNPROC;

	if(cb->ind == ind)
	{
		build_fi_tree_BFS(cb->iznd, bvisited, father);
	}
	else
	{
		build_fi_tree_BFS(cb->ind, bvisited, father);
	}

	list<string>::iterator it;
	//�������ź��б��е����й���ָʾ����Ӧ���������Ϊ����
	for (it = lst_fi.begin(); it != lst_fi.end(); it++)
	{
		map<string, TreeNode<DATA_FI>*>::iterator it_map = m_fi_node.find(*it);

		if (it_map != m_fi_node.end())
		{
			it_map->second->data.bfault = true;
		}
	}

	//���˵������ź��б���������
	list<string> tmp_lst_fi(lst_fi);
	lst_fi.clear();

	for (it = tmp_lst_fi.begin(); it != tmp_lst_fi.end(); it++)
	{
		if(is_wrong_fi(it->c_str()))
		{
			printf("����ָʾ��%sΪ��!\r\n", it->c_str());

			lst_err_point.push_back(*it);
		}
		else
		{
			lst_fi.push_back(*it);
		}
	}

	//������©���Ĺ����ź���ӵ������ź��б�
	for (it = lst_fi.begin(); it != lst_fi.end(); it++)
	{
		map<string, TreeNode<DATA_FI>*>::iterator it_map = m_fi_node.find(*it);
		TreeNode<DATA_FI>* pnode = it_map->second;

		if (it_map != m_fi_node.end())
		{
			TreeNode<DATA_FI>* pnode = it_map->second;

			do
			{
				if (pnode->data.id.size() > 0 && !pnode->data.bfault)
				{
					lst_fi.push_front(pnode->data.id);
					pnode->data.bfault = true;
					printf("����ָʾ��%s©��\r\n", pnode->data.id.c_str());
					lst_mis_point.push_back(pnode->data.id);
				}

				it_map->second->data.dist++;

				pnode = m_tree_fi.GetParent(pnode);
			} while (pnode != NULL);
		}
	}

	if(lst_fi.size() <= 0) return FDIR_R_FAIL;

	m_sz_sec.clear();

	//�ҳ�����������Զ�Ĺ����źţ����źż�Ϊ���ϵ���ʼ��
	TreeNode<DATA_FI>* pfast_node = m_fi_node[lst_fi.front()];

	for (it = lst_fi.begin(); it != lst_fi.end(); it++)
	{
		TreeNode<DATA_FI>* ptmp_node = m_fi_node[*it];

		if (ptmp_node->data.dist > pfast_node->data.dist)
			pfast_node = ptmp_node;
	}

	//��һ���߶��������˻���ͼ����λ���߶ε���������
	if (pfast_node->data.equ_type == EQU_SEC)
	{
		m_sz_sec = pfast_node->data.equ_id;
	}
	else
	{
		cb = dsbob->cb_tb->Find(CLS_CB(pfast_node->data.equ_id.c_str()));
		CLS_ND *nd = NULL;

		nd = dsbob->nd_tb->GetDataByPos(cb->ind);

		if (nd->r_sec != -1)
		{
			m_sz_sec = dsbob->r_nd_sec_tb->GetDataByPos(nd->r_sec)->sec_id;
		}
		else
		{
			nd = dsbob->nd_tb->GetDataByPos(cb->iznd);

			if (nd->r_sec != -1)
			{
				m_sz_sec = dsbob->r_nd_sec_tb->GetDataByPos(nd->r_sec)->sec_id;
			}
			else
			{
				return FDIR_R_FAIL;
			}
		}
	}

	head_ppft.id = (pfast_node->data.equ_type == EQU_SEC ? pfast_node->data.id : pfast_node->data.equ_id);
	head_ppft.table = (pfast_node->data.equ_type == EQU_SEC ? "FI" : "CB");

	//�����к��ӽ����Ϊ���ϵ��յ�
	list<TreeNode<DATA_FI>*> lst_dev;

	GetDownDevs(pfast_node, lst_dev);

	for (list<TreeNode<DATA_FI>*>::iterator it_dev = lst_dev.begin(); it_dev != lst_dev.end(); it_dev++)
	{
		if ((*it_dev)->data.equ_type == EQU_LD)
		{
			tail_ppft.push_back(FAULT_ITEM((*it_dev)->data.id, "LD"));
		}
		else if ((*it_dev)->data.equ_type == EQU_SEC)
		{
			tail_ppft.push_back(FAULT_ITEM((*it_dev)->data.id, "FI"));
		}
		else if ((*it_dev)->data.equ_type == EQU_CB)
		{
			tail_ppft.push_back(FAULT_ITEM((*it_dev)->data.equ_id, "CB"));
		}
	}


#if 0
	//�˶δ��������������ɵĹ���ָʾ�����Ƿ���ȷ
	while (true)
	{
		char fi_id[30] = { 0 };

		cout<<"input fi:";
		scanf("%s", fi_id);

		if (fi_id[0] == 'q')
			break;

		TreeNode<string> *pnode = m_fi_node[fi_id];

		if(pnode == NULL) continue;

		do
		{
			cout << pnode->data.c_str() << endl;

			pnode = m_tree_fi.GetParent(pnode);

		} while (pnode != NULL);
	}
#endif
#if 0
	//�ӹ����������ߵ�ĸ�߿�ʼ���ŵ����������������������ź�
	for (i = 0; i < lv_cb; i++)
	{
		dsbob->cb_tb->GetDataByPos(i)->topflags |= FDIR_CB_UNPROC;
	}

	memset(bvisited, false, lv_sec * sizeof(bool));

	head_ppft = FAULT_ITEM(bus->id, "BUS");
	Find_BFS(ind, bvisited, &lst_fi);
#endif
	return FDIR_R_SUCCEED;
}
#endif

//�ָ����ϵ���ʼ�ͽ������豸
void FDIR_R2::ReCoHeadTail(const char* fault_id)
{
	char *tempbuf = new char[50];
	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	//��ȡ���ϵ���ʼ�豸
	sprintf(sql, "select DEV_ID,REL_DEV_ID,DEV_TABLE from fdir_fault_dev where id=\'%s\' and flag=1", fault_id);
	cout<<sql<<endl;
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_node;
	m_Tree.DepthFirstErgodic(m_Tree.root, lst_node);
	fault_tail.clear();
	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	if (ret != OCI_ERROR)
	{
		if (rec_num < 0)
		{
			TRACE("��ȡ����:%s�豸����ʼ�豸����\r\n", fault_id);
		}
		else
		{
			FAULT_ITEM temp_item;
			bzero(tempbuf, 50);
			memcpy(tempbuf, buf, attrs[0].col_width);
			temp_item.id = *(long*)tempbuf;
			pid_to_id(temp_item.id);

			bzero(tempbuf, 50);
			memcpy(tempbuf, buf+(attrs[0].col_width), attrs[1].col_width);
			temp_item.eq_id = *(long*)tempbuf;
			pid_to_id(temp_item.eq_id);

			bzero(tempbuf, 50);
			memcpy(tempbuf, buf+(attrs[0].col_width+attrs[1].col_width), attrs[2].col_width);
			temp_item.table = tempbuf;

			for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it = lst_node.begin(); it != lst_node.end(); it++)
			{
				if ((*it)->data.m_type == _CB)
				{
					if (((CLS_CB *)(*it)->data.m_pData)->id == temp_item.id)
					{
						fault_head = *it;
						break;
					}
				}
				else if ((*it)->data.m_type == _FI)
				{
					if (((CLS_FI *)(*it)->data.m_pData)->id == temp_item.id)
					{
						fault_head = *it;
						break;
					}
				}
				else if ((*it)->data.m_type == _BUS)
				{
					if (((CLS_BUS *)(*it)->data.m_pData)->id ==  temp_item.id)
					{
						TRACE("���Ͽ�ʼ�ͽ������豸����ĸ��:%ld\r\n", temp_item.id);
						fault_head = *it;
						fault_tail.push_back(*it);
						break;
					}
				}
				else if ((*it)->data.m_type == _LD)
				{
					if ((((CLS_LD *)(*it)->data.m_pData)->id) == temp_item.id)
					{
						fault_head = *it;
						break;
					}
				}
			}
		}
	}
	else
	{
		TRACE("��ȡ����:%s�豸����ʼ�豸����\r\n", fault_id);
	}
	g_oci->Readdata_Free();

	//��ȡ���ϵĽ����豸
	sprintf(sql, "select DEV_ID,REL_DEV_ID,DEV_TABLE from fdir_fault_dev where id=\'%s\' and flag=2", fault_id);
	cout<<sql<<endl;

	fault_tail.clear();
	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	if (ret != OCI_ERROR)
	{
		for (int i = 0; i < rec_num; i++)
		{
			FAULT_ITEM temp_item;
			bzero(tempbuf, 50);
			memcpy(tempbuf, buf+i*(attrs[0].col_width+attrs[1].col_width+attrs[2].col_width), attrs[0].col_width);
			temp_item.id = *(long*)tempbuf;
			pid_to_id(temp_item.id);

			bzero(tempbuf, 50);
			memcpy(tempbuf, buf+i*(attrs[0].col_width+attrs[1].col_width+attrs[2].col_width)+(attrs[0].col_width), attrs[1].col_width);
			temp_item.eq_id = *(long*)tempbuf;
			pid_to_id(temp_item.eq_id);

			bzero(tempbuf, 50);
			memcpy(tempbuf, buf+i*(attrs[0].col_width+attrs[1].col_width+attrs[2].col_width)+(attrs[0].col_width+attrs[1].col_width), attrs[2].col_width);
			temp_item.table = tempbuf;

			for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it = lst_node.begin(); it != lst_node.end(); it++)
			{
				if ((*it)->data.m_type == _CB)
				{
					if (((CLS_CB *)(*it)->data.m_pData)->id == temp_item.id)
					{
						fault_tail.push_back(*it);
						break;
					}
				}
				else if ((*it)->data.m_type == _FI)
				{
					if (((CLS_FI *)(*it)->data.m_pData)->id == temp_item.id)
					{
						fault_tail.push_back(*it);
						break;
					}
				}
				else if ((*it)->data.m_type == _LD)
				{
					if ((((CLS_LD *)(*it)->data.m_pData)->id) == temp_item.id)
					{
						fault_tail.push_back(*it);
						break;
					}
				}
				else if ((*it)->data.m_type == _BUS)
				{
					if ((((CLS_BUS *)(*it)->data.m_pData)->id) == temp_item.id)
					{
						fault_tail.push_back(*it);
						break;
					}
				}
			}

		}
	}
	else
	{
		TRACE("��ȡ����:%s���Ͻ������豸����\r\n", fault_id);
	}
	g_oci->Readdata_Free();
	delete tempbuf;
}

int FDIR_R2::FormResSteps()
{
	int lv_isolate = fdir_isolate_tb->GetCount();
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();

	for (int i = 0; i < lv_isolate; i++)
	{
		CLS_FDIR_ISOLATE * fdir_isolate = fdir_isolate_tb->GetDataByPos(i);

		if(fdir_isolate->q & FDIR_ISOLATE_UP)
		{
			cout << "����" << fdir_isolate->cb << "Ϊ�����������ο��أ���������ת����" << endl;
			continue;	//�����������ο��أ�����Ҫ������ת��
		}

		if (Do(fdir_isolate->cb, false) == 0)
		{
			char fhzg_id[50] = { 0 };
			timeval tval = { 0 };

			gettimeofday(&tval, NULL);

			tm *ptm = localtime(&tval.tv_sec);
			sprintf(fhzg_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1
					, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, tval.tv_usec);

//			fill_fhzg_db(fdir_isolate->cb, fhzg_id, this, false);

			int lv_fdir_zone = fdir_zone_tb->GetCount();

			for(int j = 0; j < lv_fdir_zone; j++)
			{
				if (fdir_zone_tb->GetDataByPos(j)->icb == fdir_isolate->icb)
				{
					CLS_FDIR_RESTORE fdir_restore/* = { 0 }*/;

					strcpy(fdir_restore.descr, fhzg_id);
					fdir_restore.cb= fdir_isolate->cb;

					fdir_restore_tb[j] = vector<CLS_FDIR_RESTORE>(1, fdir_restore);

					break;
				}
			}
		}
		else
		{
			if (GetAllSO()->size() <= 0)
			{
				cout << "����" << fdir_isolate->cb << "��ת��������" << endl;
				fdir_isolate->q |= FDIR_ISOLATE_NO_SO;
			}

			//���ÿ������������޷�������豸��ӵ����ι����޷�������豸�ܱ�
			while (lst_protected.size() > 0)
			{
				lst_warning.push_back(lst_protected.front());

				printf("!!!!!%s:%ld�޷�����!!!!!!!!!!\r\n", lst_protected.front().table.c_str(), lst_protected.front().id);
				lst_protected.pop_front();
			}

		}
	}

	return FDIR_R_SUCCEED;
}

void FDIR_R2::ReBuildFaultTree(const char* fault_id)
{
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_CB* cb_tb = dsbob->cb_tb->GetTable();
	int lv_sec = dsbob->sec_tb->GetCount();
	int lv_nd = dsbob->nd_tb->GetCount();
	int cnt_cb = dsbob->cb_tb->GetCount();
	int lv_bus = dsbob->bus_tb->GetCount();
	int ind, znd;
	int lv_fi = dsbob->fi_tb->GetCount();
	int lv_ld = dsbob->ld_tb->GetCount();
	int i = 0;

	for (i = 0; i < cnt_cb; i++)
	{
		CLS_CB *cb = dsbob->cb_tb->GetDataByPos(i);

		cb->topflags &= ~FDIR_CB_DETECT_OPEN;
		cb->topflags &= ~FDIR_CB_DETECT_FAULT;
		cb->topflags &= ~FDIR_CB_DETECT_FI;
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

	b_pass_fault_head = false;
	b_pass_fault_tail = false;
	b_pass_head_tail = false;
	FDIR_TREE_NODE_DATA key_dev;

	//ĸ�߹�����Ҫ��ĸ��Ϊ���ڵ㽨��������
	if (faultjxcb == -1)
	{
		char *buf = NULL;
		int rec_num, attr_num;
		struct ORA_ATTR *attrs = NULL;
		char bus_id[50];

		sprintf(sql, "SELECT STUDY_EQ FROM FDIR_FDIRECT WHERE ID=\'%s\'", fault_id);
		pthread_mutex_lock(&oci_mutex);
		int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
		pthread_mutex_unlock(&oci_mutex);

		if (ret != OCI_ERROR)
		{
			if (rec_num > 0)
			{
				bzero(bus_id, 50);
				strncpy(bus_id, buf, attrs[0].col_width);
				CLS_BUS *bus = g_bob->bus_tb->Find(CLS_BUS(atol(bus_id)));
				key_dev = bus;
			}
			g_oci->Readdata_Free();
		}
	}
	else
	{
		CLS_CB *cb = dsbob->cb_tb->GetDataByPos(faultjxcb);
		key_dev = cb;
	}

	//����������
	BuildTree(key_dev, m_Tree);
	m_Tree.SaveTree("fault_retree.xml");
}

int FDIR_R2::FormIsoSteps()
{
	list<int> lst_iso_cb,lst_iso_cb2;	//������Ҫ����Ŀ����б�
	int icb = -1;
	int izone = -1;
	int ret = -1;
	int lv_fdir_zone = fdir_cb_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ISOLATE* fdir_isolate = new CLS_FDIR_ISOLATE;
	bool b_has_up_cb = false;//��ʾ�����б����Ƿ�������ο���
	bool b_has_up_cb2 = false;//��ʾ��Сͣ������б����Ƿ�������ο���
	lst_iso_cb.clear();
	lst_iso_cb2.clear();
	lst_up_iso_cb.clear();
	lst_restore.clear();

	fdir_isolate_tb->Empty();
	fdir_isolate_tb2->Empty();

	//�������������ʼ���ػ���ʼ����ָʾ�����εĸ��뿪��
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_tree_node;
	list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lst_tree_node;
	//�������ҵ����Ͽ�ʼ��λ�ã����Ҹ���λ�ü����������
	m_Tree.BreadthFirstErgodic(m_Tree.root, lst_tree_node);
	if (fault_head == NULL)
	{
		TRACE("���Ͽ�ʼ�Ľ��Ϊ��\r\n");
		return FDIR_R_FAIL;
	}
	//ѭ������������Ҫ����Ŀ���
	TreeNode<FDIR_TREE_NODE_DATA> *tempnode;
	tempnode = fault_head;
	PSBOB::CB psbob_cb = { 0 };
	bool isfind = false;
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_iso_node;
	if (fault_head->data.m_type != _BUS)
	{
		if (tempnode->data.m_type == _CB)
		{
			CLS_CB* pcb = (CLS_CB*) tempnode->data.m_pData;

			if (oodbread_rk(&psbob_cb, &pcb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
			{
				TRACE("���Ͻ��������豸id�ǣ�%ld��ȡң��ʧ��\r\n", ((CLS_CB* ) tempnode->data.m_pData)->pid);
			}
			else
			{
				if (psbob_cb.point > 0)
				{
					b_has_up_cb2 = true;
					lst_iso_cb2.push_back(dsbob->cb_tb->FindPos(CLS_CB(pcb->id)));
				}
			}
		}

		while (tempnode)
		{
			if (tempnode->data.m_type == _CB)
			{
				if (oodbread_rk(&psbob_cb, &((CLS_CB*) tempnode->data.m_pData)->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
				{
					TRACE("���Ͻ��������豸id�ǣ�%ld��ȡң��ʧ��\r\n", ((CLS_CB*) tempnode->data.m_pData)->pid);
				}
				else
				{
					if (((is_controllable(psbob_cb.id)) && !IsCBFailed(psbob_cb.id)) || psbob_cb.point <= 0 || (m_Tree.GetParent(tempnode) == NULL))
					{
						isfind = true;
						icb = dsbob->cb_tb->FindPos(CLS_CB(((CLS_CB*) tempnode->data.m_pData)->id));
						if (icb == -1)
						{
							TRACE("���ϸ������ε�Դ���뿪��Ϊ-1\r\n");
							delete fdir_isolate;
							return FDIR_R_FAIL;
						}
						TRACE("���뿪��%ld[%s](�������뿪��)��Ӧ��ң��ֵΪ%d\r\n", psbob_cb.id, psbob_cb.describe, psbob_cb.point);
						if (psbob_cb.point >= 1)
						{
							lst_up_iso_cb.push_back(icb);
							b_has_up_cb = true;
						}
						else
						{
							((CLS_CB*) tempnode->data.m_pData)->q |= CB_FAULT_ISOLATE;
						}
						isozone = FindDownZone(icb);
						faultauto_open = isozone;
					}
					else
					{
						TRACE("����%ld��Ӧ�ļ�¼�����ڻ��޿ɿر�־λ���ǿ�ң�ؿ��أ�\r\n", ((CLS_CB*) tempnode->data.m_pData)->id);
						FDIR_CB_INFO info = { 0 };

						if (psbob_cb.id == 0)
						{
							info.cb_id = ((CLS_CB*) tempnode->data.m_pData)->id;
							strcpy(info.cb_descr, ((CLS_CB*) tempnode->data.m_pData)->descr);
							info.reason = NO_POINT;
							lst_cb_info.push_back(info);
						}
						else if (!is_controllable(psbob_cb.id))
						{
							info.cb_id = ((CLS_CB*) tempnode->data.m_pData)->id;
							//strcpy(info.pt_id, psbob_point.id);
							strcpy(info.cb_descr, ((CLS_CB*) tempnode->data.m_pData)->descr);
							//info.pt_control = psbob_point.control;
							//info.pt_quality = psbob_point.quality;
							//info.pt_timexc = psbob_point.timexc;
							info.reason = NO_CONTROL;
							lst_cb_info.push_back(info);
						}
						else
						{
							//����Ҫ����
						}
					}
				}
			}

			//����˽�㲻�ܸ�����������ο��أ����Ҹ������ָʾ���ֵܽڵ�
			if (!isfind)
			{
				TreeNode<FDIR_TREE_NODE_DATA> *pchild_node;
				TreeNode<FDIR_TREE_NODE_DATA> *parentnode;
				parentnode = m_Tree.GetParent(tempnode);
				if (tempnode->data.m_type == _CB)
				{
					TRACE("���ܸ���Ľ���豸�ǣ�%ld\r\n", ((CLS_CB*) tempnode->data.m_pData)->id);
				}
				else if (tempnode->data.m_type == _LD)
				{
					TRACE("���ܸ���Ľ���豸�ǣ�%ld\r\n", ((CLS_LD*) tempnode->data.m_pData)->id);
				}
				else if (tempnode->data.m_type == _FI)
				{
					TRACE("���ܸ���Ľ���豸�ǣ�%ld\r\n", ((CLS_FI*) tempnode->data.m_pData)->id);
				}
				if (parentnode != NULL)
				{
					pchild_node = m_Tree.GetChildNode(parentnode);
					while (pchild_node != NULL)
					{
						if (pchild_node != tempnode)
						{
							GetIsoCbs(pchild_node, lst_iso_cb, lst_iso_node);
						}
						pchild_node = m_Tree.GetNextBrotherNode(pchild_node);
					}
				}
				else
				{

				}
				tempnode = parentnode;
			}
			else
			{
				break;
			}
		}
		while (lst_up_iso_cb.size() > 0)
		{
			cout << "################################" << lst_up_iso_cb.front() << endl;
			lst_iso_cb.push_front(lst_up_iso_cb.front());
			lst_up_iso_cb.pop_front();
		}
		//�����ҵ����뿪�أ�������뿪�ص����߿�����Ҫ��բ������
		if (tempnode != NULL)
		{
			list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_path_node;
			list<long> fault_cb;
			ReadCbOpen(fault_cb);
			m_Tree.GetPath(m_Tree.root, tempnode, lst_path_node);
			for (it_lst_tree_node = lst_path_node.begin(); it_lst_tree_node != lst_path_node.end(); it_lst_tree_node++)
			{
				if ((*it_lst_tree_node) == tempnode)
				{
					continue;
				}
				if ((*it_lst_tree_node)->data.m_type == _CB)
				{
					for (list<long>::iterator lst_cb_it = fault_cb.begin(); lst_cb_it != fault_cb.end(); lst_cb_it++)
					{
						if (((CLS_CB*) (*it_lst_tree_node)->data.m_pData)->pid == *lst_cb_it)
						{
							lst_restore.push_back(((CLS_CB*) (*it_lst_tree_node)->data.m_pData)->pid);
							icb = dsbob->cb_tb->FindPos(CLS_CB(*lst_cb_it));

							//����lst_path_node������ݴ洢˳���Ǵӽ��߿��ؿ�ʼ�����ϵ��豸�������Զ���բ��ʼ���������ʼ����բ����豸
							if (faultauto_open == isozone)
							{
								faultauto_open = FindDownZone(icb);
							}
							break;
						}
					}
				}
			}
		}

		//��ȡ���θ��뿪�ص����Ͽ�ʼ�豸ֱ�Ӳ��ָܻ��ĸ���
		m_ld.clear();
		if (tempnode != NULL)
		{
			GetDevsBetweenNodes(tempnode, fault_head, m_ld);
		}
		for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lst_node = lst_iso_node.begin(); it_lst_node != lst_iso_node.end(); it_lst_node++)
		{
			GetDevsBetweenNodes((*it_lst_node), NULL, m_ld);
		}
	}
	else
	{

	}
	//���������Ͻ������ϵ�����
	for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it = fault_tail.begin(); it != fault_tail.end(); it++)
	{
		if ((*it)->data.m_type == _CB)
		{
			CLS_CB* pcb = (CLS_CB*) (*it)->data.m_pData;

			if (oodbread_rk(&psbob_cb, &pcb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
			{
				TRACE("���Ͻ��������豸id�ǣ�%ld��ȡң��ʧ��\r\n", ((CLS_CB* ) tempnode->data.m_pData)->pid);
			}
			else
			{
				if (psbob_cb.point > 0)
				{
					lst_iso_cb2.push_back(dsbob->cb_tb->FindPos(CLS_CB(pcb->id)));
				}
			}

		}

		if ((*it)->data.m_type == _CB || (*it)->data.m_type == _FI || (*it)->data.m_type == _BUS)
		{
			lst_iso_node.clear();
			GetIsoCbs((*it), lst_iso_cb, lst_iso_node);
			//��ȡ���Ͻ����豸������ĩ�˽������и��ɣ����ܻ�������Իָ��ĸ���
			if (tempnode != NULL)
			{
				GetDevsBetweenNodes((*it), NULL, m_ld);
			}
			else
			{
				for (list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_lst_node = lst_iso_node.begin(); it_lst_node != lst_iso_node.end(); it_lst_node++)
				{
					GetDevsBetweenNodes((*it_lst_node), NULL, m_ld);
				}
			}
		}
	}

	for (list<int>::iterator it_iso = lst_iso_cb.begin(); it_iso != lst_iso_cb.end(); it_iso++)
	{
		bzero(fdir_isolate, sizeof(CLS_FDIR_ISOLATE));

		fdir_isolate->id = fdir_isolate_tb->GetCount(); // ��բ�����ڸ����ļ�¼id����Ϊ�����ǰ����
		//strcpy(fdir_isolate->descr, (fdir_cb + *it_iso)->descr);

		fdir_isolate->icb = *it_iso; // ���ض�Ӧ�Ŀ��������

		if (it_iso == lst_iso_cb.begin() && b_has_up_cb)
			fdir_isolate->q |= FDIR_ISOLATE_UP;	//�˿���Ϊ�������ο��أ�����Ҫ��Щ����������ת��
		else
			fdir_isolate->q &= ~FDIR_ISOLATE_UP;

		if ((fdir_cb + *it_iso)->topflags & FDIR_CB_OPEN0)
		{
			fdir_isolate->q |= FDIR_ISOLATE_OPEN0; // ������������բ�Ŀ��أ��ı�־
		}
		else
		{
			fdir_isolate->q &= ~FDIR_ISOLATE_OPEN0;
		}
		fdir_isolate->q |= FDIR_ISOLATE_OPEN;
		fdir_isolate->cb = dsbob->cb_tb->Data[*it_iso].pid;
		strcpy(fdir_isolate->cb_desc, dsbob->cb_tb->Data[*it_iso].descr);

		TRACE("��Ҫ���뿪��%ld[%s]\r\n", fdir_isolate->cb, fdir_isolate->cb_desc);

		if (fdir_isolate_tb->Insert(*fdir_isolate) == -1) // ������������
		{
			delete fdir_isolate;
			return FDIR_R_FAIL;
		}
	}

	////////////��Сͣ����뷽��
	for (list<int>::iterator it_iso = lst_iso_cb2.begin(); it_iso != lst_iso_cb2.end(); it_iso++)
	{
		bzero(fdir_isolate, sizeof(CLS_FDIR_ISOLATE));

		fdir_isolate->id = fdir_isolate_tb2->GetCount(); // ��բ�����ڸ����ļ�¼id����Ϊ�����ǰ����
		//strcpy(fdir_isolate->descr, (fdir_cb + *it_iso)->descr);

		fdir_isolate->icb = *it_iso; // ���ض�Ӧ�Ŀ��������

		if (it_iso == lst_iso_cb2.begin() && b_has_up_cb2)
			fdir_isolate->q |= FDIR_ISOLATE_UP;	//�˿���Ϊ�������ο��أ�����Ҫ��Щ����������ת��
		else
			fdir_isolate->q &= ~FDIR_ISOLATE_UP;

		if ((fdir_cb + *it_iso)->topflags & FDIR_CB_OPEN0)
		{
			fdir_isolate->q |= FDIR_ISOLATE_OPEN0; // ������������բ�Ŀ��أ��ı�־
		}
		else
		{
			fdir_isolate->q &= ~FDIR_ISOLATE_OPEN0;
		}
		fdir_isolate->q |= FDIR_ISOLATE_OPEN;
		fdir_isolate->cb = dsbob->cb_tb->Data[*it_iso].pid;
		strcpy(fdir_isolate->cb_desc, dsbob->cb_tb->Data[*it_iso].descr);

		TRACE("��Ҫ���뿪��%ld[%s]\r\n", fdir_isolate->cb, fdir_isolate->cb_desc);

		if (fdir_isolate_tb2->Insert(*fdir_isolate) == -1) // ������������
		{
			delete fdir_isolate;
			return FDIR_R_FAIL;
		}
	}

	delete fdir_isolate;
	return FDIR_R_SUCCEED;

}
void FDIR_R2::GetIsoCbs(TreeNode<FDIR_TREE_NODE_DATA> *node, list<int> &lst_cb, list<TreeNode<FDIR_TREE_NODE_DATA> *> &lst_node)
{
	PSBOB::CB psbob_cb = { 0 };
	bool isfind = false;
	if (node->data.m_type == _CB)
	{
		if (oodbread_rk(&psbob_cb, &((CLS_CB*)node->data.m_pData)->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
		{
			printf("���Ͻ��������豸id�ǣ�%ld��ȡң��ʧ��\r\n", ((CLS_CB*)node->data.m_pData)->pid);
		}
		else
		{
			if (((is_controllable(psbob_cb.id)) && !IsCBFailed(psbob_cb.id)) || psbob_cb.point <= 0)
			{
				lst_node.push_back(node);
				printf(">>>>>>>>>>>>>>>>>>>>>>>>>���뿪��%ld��Ӧ��ң��valueֵΪ%d\r\n", psbob_cb.id, psbob_cb.point);
				if (psbob_cb.point > 0)
				{
					int pos = dsbob->cb_tb->FindPos(CLS_CB(((CLS_CB*)node->data.m_pData)->id));
					lst_cb.push_back(pos);
				}
				else
				{
					//���ø����־λ
					((CLS_CB*)node->data.m_pData)->q |= CB_FAULT_ISOLATE;
				}
				isfind = true;
			}
			else
			{
				FDIR_CB_INFO info = { 0 };

				if (psbob_cb.id == 0)
				{
					info.cb_id = ((CLS_CB*)node->data.m_pData)->id;
					strcpy(info.cb_descr, ((CLS_CB*)node->data.m_pData)->descr);
					info.reason = NO_POINT;
					lst_cb_info.push_back(info);
				}
				else if (!is_controllable(psbob_cb.id))
				{
					info.cb_id = ((CLS_CB*)node->data.m_pData)->id;
					//strcpy(info.pt_id, psbob_point.id);
					strcpy(info.cb_descr, ((CLS_CB*)node->data.m_pData)->descr);
					//info.pt_control = psbob_point.control;
					//info.pt_quality = psbob_point.quality;
					//info.pt_timexc = psbob_point.timexc;
					info.reason = NO_CONTROL;
					lst_cb_info.push_back(info);
				}
				else
				{
					//����Ҫ����
				}
			}
		}
	}
	if (!isfind)
	{
		TreeNode<FDIR_TREE_NODE_DATA> *pchild = m_Tree.GetChildNode(node);
		while (pchild != NULL)
		{
			GetIsoCbs(pchild, lst_cb, lst_node);
			pchild = m_Tree.GetNextBrotherNode(pchild);
		}
	}
}

int FDIR_R2::Init()
{
	if (!update_cb_state() || !add_so())
	{
		cout << "���¿���ң��״̬���Զ���ӵ�Դʧ�ܣ�" << endl;
		return FDIR_FAIL_DIRECT;
	}

	//	dsbob->WriteReDB(NULL, NULL, NULL);

	if (FormFdirDB() == FDIR_R_FAIL)
	{
		cout << "FormFdirDB failed!" << endl;

		return FDIR_FAIL_DIRECT;
	}

	if (FormIcbOfZone() == FDIR_R_FAIL)
	{
		cout << "�γ����ε������뿪��ʧ�ܣ�" << endl;
		return FDIR_FAIL_DIRECT;
	}

	if (!WriteRtdb())
	{
		cout << __FUNCTION__ << "fdir_r->WriteRtdbʧ�ܣ�" << endl;

		return FDIR_FAIL_DIRECT;
	}

	fdir_isolate_tb->Empty();
	fdir_restore_tb.clear();
	fdir_isolate_tb2->Empty();
	fdir_restore_tb2.clear();
	lst_iso_failed.clear();
	lst_dev.clear();
	//���� 20141226 ���û�����λָ�����������
	//lst_point.clear();
	lst_err_point.clear();
	//lst_mis_point.clear();
	lst_warning.clear();
	lst_iso_failed.clear();
	lst_isolated.clear();
	lst_failed.clear();

	lst_failed.clear();
	lst_cb_info.clear();
	cur_father = NULL;

	return FDIR_R_SUCCEED;
}

void FDIR_R2::Get_Fdir_Cbs(list<CLS_CB> &lst_cb_info)
{
	lst_cb_info.clear();

	list<TreeNode<FDIR_TREE_NODE_DATA>*> lst_nodes;
	list<TreeNode<FDIR_TREE_NODE_DATA>*>::iterator it_node;
	map<long, long> mpath;
	map<long, long>::iterator it_mpath;
	PSBOB::CB psbob_cb = { 0 };

	m_Tree.BreadthFirstErgodic(m_Tree.root, lst_nodes);
	for (it_node = lst_nodes.begin(); it_node != lst_nodes.end(); it_node++)
	{
		if ((*it_node)->data.m_type != _CB)
		{
			continue;
		}

		CLS_CB cb ;
//		cb.id = ((CLS_CB*)(*it_node)->data.m_pData)->id;
//		cb.digital = ((CLS_CB*)(*it_node)->data.m_pData)->id;
//		strcpy(cb.descr, ((CLS_CB*)(*it_node)->data.m_pData)->descr);
		 cb = *((CLS_CB*)(*it_node)->data.m_pData);

		if (oodbread_rk(&psbob_cb, &((CLS_CB*)(*it_node)->data.m_pData)->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) == OO_FAIL)
		{
			cb.q |= CB_UNKNOWN;
		}
		if (psbob_cb.point <= 0)
		{
			cb.q |= CB_OPEN;
		}
		else
		{
			cb.q &= ~CB_OPEN;
		}
		lst_cb_info.push_back(cb);
	}

	lst_nodes.clear();
	if (fault_head == NULL)
	{
		TRACE("û���ҵ����Ͽ�ʼ���豸\r\n");
		return;
	}

	//���ҹ��Ͻ��߿��ص����Ͽ�ʼ���豸��·��
	m_Tree.GetPath(m_Tree.root, fault_head, lst_nodes);
	if (lst_nodes.size() <= 0)
	{
		lst_nodes.push_back(m_Tree.root);
	}

	for (it_node = lst_nodes.begin(); it_node != lst_nodes.end(); it_node++)
	{
		if ((*it_node)->data.m_type != _CB)
		{
			continue;
		}
		mpath[((CLS_CB*)(*it_node)->data.m_pData)->id] = ((CLS_CB*)(*it_node)->data.m_pData)->id;
		TRACE("�����������豸�ǣ�%ld(%s)\r\n", ((CLS_CB*)(*it_node)->data.m_pData)->id, ((CLS_CB*)(*it_node)->data.m_pData)->descr);
	}
	for (list<CLS_CB>::iterator it_cb = lst_cb_info.begin(); it_cb != lst_cb_info.end(); it_cb++)
	{
		it_mpath = mpath.find(it_cb->id);
		if (it_mpath != mpath.end())
		{
			it_cb->q |= CB_FAULT;
		}
	}
}

int FDIR_R2::FaultDetection()
{
	if (Init() == FDIR_R_FAIL)
	{
		printf("���϶�λ��ʼ��ʧ��\n");
		return FDIR_FAIL_DIRECT;
	}

	if (FDIR_R::FaultDetection() == FDIR_R_FAIL)
	{
		printf("���϶�λʧ��\n");
		return FDIR_FAIL_DIRECT;
	}

	return FDIR_R_SUCCEED;
}

//��ȡ�������Σ����ε����������ǿ���ң�ص�
int FDIR_R2::Findupfaultzone()
{
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();

	while (true)
	{
		int icb = (fdir_zone + isozone)->icb;

		if (icb == -1)
		{
			return FDIR_FAIL_DIRECT;
		}

		isozone = FindUpZone(icb);

		if (isozone == -1)
		{
			return FDIR_FAIL_DIRECT;
		}
		CLS_CB *tmp_cb = dsbob->cb_tb->Find(CLS_CB((fdir_cb + ((fdir_zone + isozone)->icb))->cb_id));

		if(is_controllable(tmp_cb->pid) || (tmp_cb->q & CB_FDHCB))
		{
			break;
		}

		FDIR_CB_INFO info = { 0 };
		info.cb_id = tmp_cb->id;
		//strcpy(info.pt_id, tmp_cb->digital);
		strcpy(info.cb_descr, tmp_cb->descr);
		//info.pt_control = psbob_point.control;
		//info.pt_quality = psbob_point.quality;
		//info.pt_timexc = psbob_point.timexc;
		info.reason = NO_CONTROL;
		lst_cb_info.push_back(info);
	}
	return isozone;
}

int FDIR_R2::ExecFdir(const long id, const char *table)
{
	assert(table != NULL);
	assert(table[0] != 0);

	if (!update_cb_state() || !add_so())
	{
		cout << "���¿���ң��״̬���Զ���ӵ�Դʧ�ܣ�" << endl;
		return FDIR_FAIL_DIRECT;
	}

	if (FormFdirDB() == FDIR_R_FAIL)
	{
		cout << "FormFdirDB failed!" << endl;

		return FDIR_FAIL_DIRECT;
	}

	if (FormIcbOfZone() == FDIR_R_FAIL)
	{
		cout << "�γ����ε������뿪��ʧ�ܣ�" << endl;
		return FDIR_FAIL_DIRECT;
	}

	if (!WriteRtdb())
	{
		cout << __FUNCTION__ << "fdir_r->WriteRtdbʧ�ܣ�" << endl;

		return FDIR_FAIL_DIRECT;
	}

	fdir_isolate_tb->Empty();
	fdir_restore_tb.clear();
	fdir_isolate_tb2->Empty();
	fdir_restore_tb2.clear();
	lst_iso_failed.clear();
	lst_dev.clear();
	lst_point.clear();
	lst_err_point.clear();
	lst_mis_point.clear();
	lst_warning.clear();
	lst_iso_failed.clear();
	lst_isolated.clear();
	lst_failed.clear();
	lst_restore.clear();
	fault_head = NULL;
	fault_tail.clear();
	m_Tree.TreeRemoveSubTree(m_Tree.root);
	m_p_jxcb_Node = NULL;

	faultzone = -1;
	faultbus = -1;
	faultjxcb = -1;
	faultauto_open = -1;
	isozone = -1;
	b_ld_fault = false;

	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	int lv_nd = dsbob->nd_tb->GetCount();
	int lv_sec = dsbob->sec_tb->GetCount();
	int lv_cb = dsbob->cb_tb->GetCount();
	int lv_fi = dsbob->fi_tb->GetCount();
	int lv_ld = dsbob->ld_tb->GetCount();
	int i = 0;

	for (i = 0; i < lv_cb; i++)
	{
		CLS_CB *cb = dsbob->cb_tb->GetDataByPos(i);

		cb->topflags &= ~FDIR_CB_DETECT_OPEN;
		cb->topflags &= ~FDIR_CB_DETECT_FAULT;
		cb->topflags &= ~FDIR_CB_DETECT_FI;
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

	long dev_id = id;
	pid_to_id(dev_id);

	//�����Ҫģ����豸�Ľ��
	FDIR_TREE_NODE_DATA key_dev;
	if (strcasecmp(table, "breaker") == 0)
	{
		CLS_CB *fault_cb = dsbob->cb_tb->Find(CLS_CB(dev_id));
		if (fault_cb == NULL)
		{
			TRACE("��ȡģ�⿪�أ�%ld��Ӧ��cb��¼ʧ��\r\n", id);
			return FDIR_FAIL_DIRECT;
		}
		key_dev = fault_cb;
		fault_cb->topflags |= FDIR_CB_DETECT_FI;
	}
	else if (strcasecmp(table, "faultinfo") == 0)
	{
		CLS_FI *fault_fi = dsbob->fi_tb->Find(CLS_FI(dev_id));
		if (fault_fi == NULL)
		{
			TRACE("��ȡģ�����ָʾ����%ld��Ӧ��faultinfo��¼ʧ��\r\n", id);
			return FDIR_FAIL_DIRECT;
		}
		key_dev = fault_fi;
		fault_fi->topflags |= FDIR_CB_DETECT_FI;
	}
	else if (strcasecmp(table, "busbarsection") == 0)
	{
		CLS_BUS *fault_bus = dsbob->bus_tb->Find(CLS_BUS(dev_id));
		if (fault_bus == NULL)
		{
			TRACE("��ȡģ��ĸ�߹��ϣ�%ld��Ӧ��bus��¼ʧ��\r\n", id);
			return FDIR_FAIL_DIRECT;
		}
		key_dev = fault_bus;
		fault_bus->topflags |= FDIR_BUS_DETECT;
	}
	else if (strcasecmp(table, "energyconsumer") == 0)
	{
		CLS_LD *fault_ld = dsbob->ld_tb->Find(CLS_LD(dev_id));
		if (fault_ld == NULL)
		{
			TRACE("��ȡģ�⸺�ɣ�%ld��Ӧ��ld��¼ʧ��\r\n", id);
			return FDIR_FAIL_DIRECT;
		}
		key_dev = fault_ld;
		b_ld_fault = true;
		fault_ld->topflags |= FDIR_CB_DETECT_FI;
	}
	else
	{
		TRACE("�豸���ͣ�%s������ģ��ı�׼���豸id�ǣ�%ld\r\n", table, id);
		return FDIR_FAIL_DIRECT;
	}

	b_pass_fault_head = false;
	b_pass_fault_tail = false;
	b_pass_head_tail = false;

	//����������
	if (BuildTree(key_dev, m_Tree) != FDIR_R_SUCCEED)
	{
		return FDIR_R_FAIL;
	}

	m_Tree.SaveTree("fault_tree.xml");
	//��ȡ����·��
	list<TreeNode<FDIR_TREE_NODE_DATA>*> lstPath;
	if (GetFaultPath(lstPath, key_dev) == FDIR_R_FAIL)
	{
		return FDIR_R_FAIL;
	}

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

			for (int i = 0; i < lv_fdir_zone; i++)
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
		GetDownDevs(fault_head, fault_tail);

	}

	//���ù�����ʼ������λ����Ӧ�豸��topflags��
	MaskFaultOnTree();

	if (FindFaultBusAndFaultDevs(key_dev,faultjxcb) == FDIR_R_FAIL)
	{
		TRACE("��ȡ����ĸ�ߺ͹����豸ʧ�ܣ�");
		return FDIR_R_FAIL;
	}

	/////////////////ģ��������ο�����բ���¹��ܻ��߹���ָʾ���Ķ�·�źŶ���//////////////
	TreeNode<FDIR_TREE_NODE_DATA> *father_node = fault_head;
	bool find_open = false; //��־�Ƿ��ҵ����ο�����բ�Ŀ���
	int retnum, num;
	while (father_node != NULL)
	{
		if (father_node->data.m_type == _CB)
		{
			if (!find_open)
			{
				//�ҵ������Զ���բ���߽��߿��أ�ģ�⿪����բ
				if ((((CLS_CB *) father_node->data.m_pData)->q & CB_AUTO) || (((CLS_CB *) father_node->data.m_pData)->q & CB_FDHCB))
				{
					PSBOB::CB psbob_cb;
					psbob_cb.point = 0;
					lst_point.push_back(FAULT_ITEM(((CLS_CB *) father_node->data.m_pData)->pid, ((CLS_CB *) father_node->data.m_pData)->pid, "CB", ((CLS_CB *) father_node->data.m_pData)->descr));
					printf("������%ld����ģ������ź��б�\r\n", ((CLS_CB *) father_node->data.m_pData)->pid);

					//	ģ�⿪�صı�λ��Ϣ   ����ʵʱ���п��ص� ң��ֵ
//					if (oodbupdate_ak(&psbob_cb.point, &((CLS_CB *) father_node->data.m_pData)->pid, "point", const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb.point)) == OO_FAIL)
//					{
//						printf("%s(%d):����·����ؼ���Ϊ%ld��ң��ֵ����Ϊ%dʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, ((CLS_CB *) father_node->data.m_pData)->pid, psbob_cb.point, _oodb_errno);
//					}
					find_open = true;
					break;
				}
			}
		}
		else if (father_node->data.m_type == _LD)
		{
			PSBOB::PROTECT protect[10] = { 0 };
			retnum = oodbread_ra(protect, "oo_dev", &((CLS_LD *) father_node->data.m_pData)->pid, const_cast<TB_DESCR *>(g_db_psbob->GetTB("relaysig")), sizeof(PSBOB::PROTECT) * 10);
			if (retnum <= 0)
			{
				father_node = father_node->fathernode;
				continue;
			}
			else
			{
				num = retnum / sizeof(PSBOB::PROTECT);
				for (i = 0; i < num; i++)
				{
					if (protect[i].pnt_type & (D_SG_WARN_TYPE | SG_WARN_TYPE))
					{
						lst_point.push_back(FAULT_ITEM(((CLS_LD *) father_node->data.m_pData)->pid, ((CLS_LD *) father_node->data.m_pData)->pid, "LD", ((CLS_LD *) father_node->data.m_pData)->descr));
						break;
					}
				}
			}
		}
		else if (father_node->data.m_type == _FI)
		{
			PSBOB::PROTECT protect[10] = { 0 };
			retnum = oodbread_ra(protect, "oo_dev", &((CLS_FI *) father_node->data.m_pData)->pid, const_cast<TB_DESCR *>(g_db_psbob->GetTB("relaysig")), sizeof(PSBOB::PROTECT) * 10);
			if (retnum <= 0)
			{
				father_node = father_node->fathernode;
				continue;
			}
			else
			{
				num = retnum / sizeof(PSBOB::PROTECT);
				for (i = 0; i < num; i++)
				{
					if (protect[i].pnt_type & (D_SG_WARN_TYPE | SG_WARN_TYPE))
					{
						lst_point.push_back(FAULT_ITEM(((CLS_FI *) father_node->data.m_pData)->pid, ((CLS_FI *) father_node->data.m_pData)->sec, "FI", ((CLS_FI *) father_node->data.m_pData)->descr));
						break;
					}
				}
			}
		}
		else if (father_node->data.m_type == _BUS)
		{
			PSBOB::BUS psbob_bus;
			psbob_bus.v = 0.0;
			lst_point.push_back(FAULT_ITEM(((CLS_BUS *) father_node->data.m_pData)->pid, ((CLS_BUS *) father_node->data.m_pData)->pid, "BUS", ((CLS_BUS *) father_node->data.m_pData)->descr));
			printf("��ĸ��%ld����ģ������ź��б�\r\n", ((CLS_BUS *) father_node->data.m_pData)->pid);

			//	ģ�����ĸ�ߵ���Ϣ, ����ʵʱ����ĸ�ߵ��ߵ�ѹΪ0
//			if (oodbupdate_ak(&psbob_bus.v, &((CLS_BUS *) father_node->data.m_pData)->pid, "v", const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(psbob_bus.v)) == OO_FAIL)
//			{
//				printf("%s(%d):��ĸ�߱��йؼ���Ϊ%ld���ߵ�ѹ����Ϊ%0.2fʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, ((CLS_BUS *) father_node->data.m_pData)->pid, psbob_bus.v, _oodb_errno);
//			}
			break;
		}

		father_node = father_node->fathernode;
	}

	if (key_dev.m_type == _CB)
	{
		TRACE("\033[42mģ�⿪��Ϊ%ld���¹���ģ��ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_CB*) key_dev.m_pData)->id, faultzone, faultjxcb);
	}
	else if (key_dev.m_type == _LD)
	{
		TRACE("\033[42mģ�⸺��Ϊ%ld���¹���ģ��ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_LD*) key_dev.m_pData)->id, faultzone, faultjxcb);
	}
	else if (key_dev.m_type == _FI)
	{
		TRACE("\033[42mģ�����ָʾ��Ϊ%ld���¹���ģ��ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_FI*) key_dev.m_pData)->id, faultzone, faultjxcb);
	}
	else if (key_dev.m_type == _BUS)
	{
		TRACE("\033[42mģ��ĸ��Ϊ%ld���¹���ģ��ɹ�����������:%d,���߿���:%d\033[0m\r\n", ((CLS_BUS*) key_dev.m_pData)->id, faultzone, faultjxcb);
	}

	FormIcbOfZone();

	return FDIR_R_SUCCEED;
}

int GroupFault(list<FDIR_TASK> &lst_task)
{
	FDIR_TASK_LIST::iterator task_vi = lst_task.begin();
	FDIR_TASK_LIST::iterator sec_vi;
	for(;task_vi != lst_task.end();task_vi++)
	{
		if (((*task_vi).rq_type == TASK_TYPE_FAULT) &&
				(task_vi->task_data.fault.next_step == TASK_FAULT_STATUS_CALC_ISO))
		{
			sec_vi = task_vi;
			sec_vi++;
			for(;sec_vi != lst_task.end();sec_vi++)
			{
				if(((*sec_vi).rq_type == TASK_TYPE_FAULT) &&(sec_vi->task_data.fault.next_step == TASK_FAULT_STATUS_CALC_ISO))
				{
					list<int>::const_iterator cb_vi1;
					list<int>::const_iterator cb_vi2;
					const list<int>* temp1 = (*task_vi).task_data.fault.fdir_r->get_edge_cb();
					const list<int>* temp2 = (*sec_vi).task_data.fault.fdir_r->get_edge_cb();
					bool is_find = false;//����Ƿ�����һ����

					for(cb_vi1 = temp1->begin();cb_vi1 != temp1->end();cb_vi1++)
					{
						for(cb_vi2 = temp2->begin();cb_vi2 != temp2->end();cb_vi2++)
						{
							if((*cb_vi1) == (*cb_vi2))
							{
								is_find = true;
								break;
							}
						}
						if(is_find == true)
						{
							break;
						}
					}

					if(is_find == true)
					{
						//�ж��Ƿ���Ҫ���ӷ��飬����µ����
						if((*task_vi).task_data.fault.head.group_id[0] == 0 || strcmp((*task_vi).task_data.fault.head.group_id,"null") == 0)
						{
							strcpy((*task_vi).task_data.fault.head.group_id, (*task_vi).task_data.fault.head.fault_id);
							strcpy((*sec_vi).task_data.fault.head.group_id, (*task_vi).task_data.fault.head.group_id);
						}
						else//�Ѿ�������ţ�����Ҫ�µķ��飬ֱ�Ӱ����д���µĹ�����
						{
							strcpy((*sec_vi).task_data.fault.head.group_id, (*task_vi).task_data.fault.head.group_id);
						}
					}
				}
				else
				{
					continue;
				}
			}
			if((*task_vi).task_data.fault.head.group_id[0] == 0 || strcmp((*task_vi).task_data.fault.head.group_id,"null") == 0)
			{
				strcpy((*task_vi).task_data.fault.head.group_id, (*task_vi).task_data.fault.head.fault_id);
			}
			printf("���������%s\n",(*task_vi).task_data.fault.head.group_id);
			(*task_vi).task_data.fault.next_step = TASK_FAULT_STATUS_GROUPED;
		}
		else
		{
			continue;
		}
	}
	//printf("�������\n");
	//���dsbob��zn��Ĺ��ϱ�־λZN_FAULT
	int lv_zn = g_bob->zn_tb->GetCount();
	CLS_ZN* zn = g_bob->zn_tb->GetTable();
	for(int i = 0; i < lv_zn; i++)
	{
		(zn + i)->topflags &= ~ZN_FAULT;
	}

	return FDIR_R_SUCCEED;
}


int recu_calc_restore(list<FDIR_R2*>& lst_fdir_r, map<long, FDIR_R2*> &map_iso, list<list<MAP_SO_NODE> > &fdir_steps, map<long, FDIR_R2*> tmp_map_iso)
{
	list<_DATA_NODE_ZN> lst_zones, lst_cut_zones;
	list<FDIR_STEP>::iterator it_step;
	list<TreeNode<MAP_SO_NODE>*>::iterator it_tail;
	map<long, TreeData>::iterator it_so_node;
	map<long, FDIR_R2*>::iterator it_iso_cb;
	list<FDIR_R2*>::iterator it_fdir_r;
	LIST_TREENODE::const_iterator it_zones;
	TreeNode<MAP_SO_NODE>* pTreeNode = NULL;
	list<TreeNode<MAP_SO_NODE>*> lst_tree_tails;
	list<TreeNode<MAP_SO_NODE>*> lst_tmp_tree_tails;
	list<FDIR_STEP> lst_steps;
	Tree<MAP_SO_NODE> tree_lst_steps;
	list<long> lst_restored;	//�Ѿ��лָ������ĸ��뿪���б�
//	map<string, FDIR_R2*> tmp_map_iso = map_iso;//�����뿪�ؽ��б��ݣ�����ϸ��뿪�ص�ʱ��Ҫ�õ�

	lst_tmp_tree_tails.push_back(tree_lst_steps.root);

	while(true)
	{
		lst_restored.clear();

		for (map<long, FDIR_R2*>::iterator it = map_iso.begin(); it != map_iso.end(); it++)
		{
			lst_tree_tails.clear();

			for (it_tail = lst_tmp_tree_tails.begin(); it_tail != lst_tmp_tree_tails.end(); it_tail++)
			{
				//�ȼ���ǰ��Ĳ��趼�ɹ�
				pTreeNode = *it_tail;

				while (pTreeNode != NULL)
				{
					for(it_so_node = pTreeNode->data.begin(); it_so_node != pTreeNode->data.end(); it_so_node++)
					{
						for (it_step = it_so_node->second.lst_steps.begin(); it_step != it_so_node->second.lst_steps.end(); it_step++)
						{
							//���� 20141205 ��Ϊ��ѯDMS_CB���ID
							long lPwrId = it_step->cb;
							pid_to_id(lPwrId);
							CLS_CB *cb = g_bob->cb_tb->Find(lPwrId);

							if(cb == NULL)
							{
								TRACE("�Ҳ���IDΪ%ld�Ŀ��ؼ�¼��\r\n", lPwrId);

								return FDIR_R_FAIL;
							}

							if (it_step->bOpen)
							{
								cb->q |= CB_OPEN;
								TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>���ÿ���%ld[%s]Ϊ�Ͽ�״̬��\r\n", it_step->cb, cb->descr);
							}
							else
							{
								cb->q &= ~CB_OPEN;
								TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>���ÿ���%ld[%s]Ϊ�պ�״̬��\r\n", it_step->cb, cb->descr);
							}
						}
					}
					pTreeNode = tree_lst_steps.GetParent(pTreeNode);
				}


				for (it_iso_cb = tmp_map_iso.begin(); it_iso_cb != tmp_map_iso.end(); it_iso_cb++)
				{
					long cb_id = it_iso_cb->first;

					pid_to_id(cb_id);

					CLS_CB *cb = g_bob->cb_tb->Find(CLS_CB(cb_id));

					if (cb == NULL)
					{
						TRACE("�Ҳ���IDΪ%ld�Ŀ��ؼ�¼��\r\n", it_step->cb);

						return FDIR_R_FAIL;
					}

					//������Ҫ������ת���Ŀ�������Ϊ�պϣ����жϸÿ��ص������Σ���������Ҫת��������
					if (it_iso_cb->second == it->second)
					{
						cb->q &= ~CB_OPEN;
						TRACE("���ø��뿪��%ldΪ�պ�״̬������\r\n", it_iso_cb->first, cb->descr);
					}
					//�����еĸ��뿪������Ϊ�Ͽ�
					else
					{
						cb->q |= CB_OPEN;
						TRACE("���ø��뿪��%ldΪ�Ͽ�״̬������\r\n", it_iso_cb->first, cb->descr);
					}
				}

				//��ǰ�沽�趼�ɹ��Ļ������γɵ������뿪��
				for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
				{
					if ((*it_fdir_r)->FormIcbOfZone() == FDIR_R_FAIL)
					{
						return FDIR_R_FAIL;
					}
				}

				long tmp_id = it->first;
				id_to_pid(tmp_id);

				//����ǰ�沽�趼�ɹ��Ļ����ϵĸ���ת������
				if (it->second->Do(tmp_id, false) == 0) //ת���ɹ�
				{
					lst_restored.push_back(it->first);

					//����ת��
					if (it->second->GetType() == FHZG_TYPE_ALL)
					{
						lst_zones.clear();
						lst_cut_zones.clear();

						const LIST_TREENODE *lst_down_zone = it->second->GetDownZones();

						for (it_zones = lst_down_zone->begin(); it_zones != lst_down_zone->end(); it_zones++)
						{
							lst_zones.push_back((*it_zones)->data.node_zn);
						}

						const LIST_TREENODE *lst_so = it->second->GetSO();

						for (LIST_TREENODE::const_iterator it_lst_so = lst_so->begin(); it_lst_so != lst_so->end(); it_lst_so++)
						{
							lst_steps.clear();

							MAP_SO_NODE m_so_fhzg;
							//�ȼ�¼�������ɵĲ���
							const list<long> *cut_step = it->second->GetLdCutOperations();

							for (list<long>::const_iterator it_cut_step = cut_step->begin(); it_cut_step != cut_step->end(); it_cut_step++)
							{
								long value = *it_cut_step;
								lst_steps.push_back(FDIR_STEP(value, true, CUT_LD));

								TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>�������ɿ���%ld\r\n", value);
							}

							const LIST_TREENODE *lst_cut_zone = it->second->GetLdCut();

							for (it_zones = lst_cut_zone->begin(); it_zones != lst_cut_zone->end(); it_zones++)
							{
								lst_cut_zones.push_back((*it_zones)->data.node_zn);
							}

							lst_steps.push_back(FDIR_STEP((*it_lst_so)->data.node_so.cb_id, false, RESTORE));
							try
							{
								TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>�ָ����翪��%ld\r\n", (*it_lst_so)->data.node_so.cb_id);
							}
							catch(...)
							{
								cout << "errorerrorerrorerrorerrorerrorerrorerrorerrorerrorerrorerrorerrorerror"<<endl;
							}

							m_so_fhzg[(*it_lst_so)->data.node_so.cb_id] = TreeData(lst_steps, lst_zones, lst_cut_zones, (*it_lst_so)->data.node_so);
							lst_tree_tails.push_back(tree_lst_steps.TreeAddLeaf(*it_tail, m_so_fhzg));
						}
					}
					//��̯ת��
					else
					{
						//���ݿ�ṹ��ʱ��֧��

#if 1
						const list<_OPERATION_STEP> *opera_step = it->second->GetOperation();

						lst_steps.clear();
						MAP_SO_NODE m_so_fhzg;
						//�ȼ�¼�������ɵĲ���
						const list<long> *cut_step = it->second->GetLdCutOperations();
						const map<TreeNode<DataType>*, LIST_TREENODE> *m_so_zone = it->second->GetSoFhzgZone();

						for (list<long>::const_iterator it_cut_step = cut_step->begin(); it_cut_step != cut_step->end(); it_cut_step++)
						{
							long value = *it_cut_step;
							lst_steps.push_back(FDIR_STEP(value, true, CUT_LD));
							TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>�������ɿ���%ld\r\n", value);
						}

						for (list<_OPERATION_STEP>::const_iterator it_opera_step = opera_step->begin(); it_opera_step != opera_step->end(); it_opera_step++)
						{
							if (it_opera_step->open_cb!= it->first)
							{
								lst_steps.push_back(FDIR_STEP(it_opera_step->open_cb, true, RESTORE));
							}

							lst_steps.push_back(FDIR_STEP(it_opera_step->close_cb, false, RESTORE));
							TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>�ָ����翪��%ld��������%ld���ϡ�\r\n", it_opera_step->open_cb, it_opera_step->close_cb);

							lst_zones.clear();
							for (map<TreeNode<DataType>*, LIST_TREENODE>::const_iterator it_so_zn = m_so_zone->begin(); it_so_zn != m_so_zone->end(); it_so_zn++)
							{
								long cb_id = it_opera_step->close_cb;
								long so_id = it_opera_step->so_id;

								pid_to_id(cb_id);
								pid_to_id(so_id);

								if ((it_so_zn->first->data.node_so.so_name == so_id) && (it_so_zn->first->data.node_so.cb_id == cb_id))
								{
									for (LIST_TREENODE::const_iterator it_list_treenode = it_so_zn->second.begin(); it_list_treenode != it_so_zn->second.end(); it_list_treenode++)
									{
										lst_zones.push_back((*it_list_treenode)->data.node_zn);
									}

									const LIST_TREENODE *lst_cut_zone = it->second->GetLdCut();

									for (it_zones = lst_cut_zone->begin(); it_zones != lst_cut_zone->end(); it_zones++)
									{
										lst_cut_zones.push_back((*it_zones)->data.node_zn);
									}
									m_so_fhzg[it_so_zn->first->data.node_so.cb_id] = TreeData(lst_steps, lst_zones, lst_cut_zones, it_so_zn->first->data.node_so);

									break;
								}
							}
							lst_steps.clear();
						}
						lst_tree_tails.push_back(tree_lst_steps.TreeAddLeaf(*it_tail, m_so_fhzg));
						//lst_tree_tails.push_back(tree_lst_steps.TreeAddLeaf(*it_tail, TreeData(lst_steps, ));
#endif
					}
				}
				else
				{
				}

				//��ԭǰ��ļ���
				pTreeNode = *it_tail;

				while (pTreeNode != NULL)
				{
					for(it_so_node = pTreeNode->data.begin(); it_so_node != pTreeNode->data.end(); it_so_node++)
					{
						for (it_step = it_so_node->second.lst_steps.begin(); it_step != it_so_node->second.lst_steps.end(); it_step++)
						{
							//���� 20141205 ��Ϊ��ѯDMS_CB���ID
							long lPwrId = it_step->cb;
							pid_to_id(lPwrId);

							if (!it_step->bOpen)
							{	
								g_bob->cb_tb->Find(CLS_CB(lPwrId))->q |= CB_OPEN;
								TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>��ԭ����%ldΪ�Ͽ�״̬��\r\n", it_step->cb);
							}
							else
							{
								g_bob->cb_tb->Find(CLS_CB(lPwrId))->q &= ~CB_OPEN;
								TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>��ԭ����%ldΪ�պ�״̬��\r\n", it_step->cb);
							}
						}
					}
					pTreeNode = tree_lst_steps.GetParent(pTreeNode);
				}

				//���㻹ԭ�����ĵ������뿪��
				for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
				{
					if ((*it_fdir_r)->FormIcbOfZone() == FDIR_R_FAIL)
					{
						return FDIR_FAIL_RESTORE;
					}
				}
			}

			if (lst_tree_tails.size() > 0)
				lst_tmp_tree_tails = lst_tree_tails;
		}

		if(lst_restored.size() <= 0) break;

		lst_restored.sort();
		lst_restored.unique();

		while(lst_restored.size() > 0)
		{
			map_iso.erase(lst_restored.front());
			lst_restored.pop_front();
		}
	}

	cout << "\033[42m��ѡ�Ļָ�������\033[0m" << endl;
	for (it_tail = lst_tmp_tree_tails.begin(); it_tail != lst_tmp_tree_tails.end(); it_tail++)
	{
		list<MAP_SO_NODE> lst_data;
		//��ԭǰ��ļ���
		pTreeNode = *it_tail;

		while (pTreeNode != NULL)
		{
			lst_data.push_front(pTreeNode->data);

			for(it_so_node = pTreeNode->data.begin(); it_so_node != pTreeNode->data.end(); it_so_node++)
			{
				for (it_step = it_so_node->second.lst_steps.begin(); it_step != it_so_node->second.lst_steps.end(); it_step++)
				{
					string code;

					GetCode(it_step->cb, code);

					if (it_step->bOpen)
					{
						TRACE("\033[42m%ld(%s) ��\033[0m\r\n", it_step->cb, code.c_str());
					}
					else
					{
						TRACE("\033[42m%ld(%s) ��\033[0m\r\n", it_step->cb, code.c_str());
					}
				}
			}

			pTreeNode = tree_lst_steps.GetParent(pTreeNode);
		}

		fdir_steps.push_back(lst_data);
		cout << "\033[42m=========================================\033[0m" << endl;
	}

	return FDIR_R_SUCCEED;
}


int calc_restore_step(list<FDIR_R2*>& lst_fdir_r, list<list<MAP_SO_NODE> > &lst_steps, bool bStation)
{
	/*����*/
//	rte_disconnect();
	/**/
	cout << __FUNCTION__ << endl;

	if ((*lst_fdir_r.begin())->Init() == FDIR_R_FAIL)
	{
		return FDIR_FAIL_ISOLATE;
	}

	list<FDIR_R2*>::iterator it;
	list<FDIR_STEP> lst_step;
	map<long, FDIR_R2*> map_cb_fdir; //���뿪����ø��뿪�����ڵĹ��ϴ�����ӳ���ϵ,���������Щ���صĸ���ת��
	int i;

//	lst_steps.push_back(lst_step);

	if (version == VERSION_STUDY)
	{
		list<FAULT_ITEM> lst_point;

		for (it = lst_fdir_r.begin(); it != lst_fdir_r.end(); it++)
		{
//			lst_point.merge((*it)->lst_point);
			lst_point.splice(lst_point.end(), (*it)->lst_point);
		}

		for (list<FAULT_ITEM>::iterator it_lst_point = lst_point.begin(); it_lst_point != lst_point.end(); it_lst_point++)
		{
			if (strcasecmp(it_lst_point->table.c_str(), "breaker") == 0)
			{
				int lv_cb = g_bob->cb_tb->GetCount();

				for (i = 0; i < lv_cb; i++)
				{
					CLS_CB *cb = g_bob->cb_tb->GetDataByPos(i);

					if (cb->pid == it_lst_point->eq_id)
					{
						cb->q &= ~CB_OPEN;
						break;
					}
				}
			}
		}

		for (it = lst_fdir_r.begin(); it != lst_fdir_r.end(); it++)
		{
			(*it)->lst_point = lst_point;
			(*it)->FormIcbOfZone();
		}
	}

	//�ȷֱ������뷽��
	for (it = lst_fdir_r.begin(); it != lst_fdir_r.end(); it++)
	{
		if ((*it)->FormIsoSteps() == FDIR_R_FAIL)
		{
			return FDIR_FAIL_ISOLATE;
		}
		//����Ҫ����Ŀ��ؼ�¼������������Ҫ������������ת��
		else
		{
			const TTable<CLS_FDIR_ISOLATE>* isolate = NULL;

			if (bStation)
				isolate = (*it)->getisolate();
			else
				isolate = (*it)->getisolate2();

			for (i = 0; i < isolate->GetCount(); i++)
			{
				g_bob->cb_tb->Data[isolate->Data[i].icb].q |= CB_FAULT_ISOLATE;

				if (isolate->Data[i].q & FDIR_ISOLATE_UP)
				{
					cout << "����" << isolate->Data[i].cb << "Ϊ�����������ο��أ���������ת����" << endl;
				}
				else
				{
					cout << "��Ҫת���ĸ��뿪�أ�" << isolate->Data[i].cb << endl;
					map_cb_fdir[g_bob->cb_tb->GetDataByPos(isolate->Data[i].icb)->id] = *it;
				}
			}
		}
	}

	return recu_calc_restore(lst_fdir_r, map_cb_fdir, lst_steps, map_cb_fdir);
}

int re_calc_restore_step(const char *group_id, list<FDIR_R2*>& lst_fdir_r, list<list<MAP_SO_NODE> > &lst_steps)
{
	/*����*/
//	rte_disconnect();
	/**/
	assert(group_id != NULL);

	map<long, FDIR_R2*> map_cb_fdir, tmp_map_cb_fdir; //���뿪����ø��뿪�����ڵĹ��ϴ�����ӳ���ϵ,���������Щ���صĸ���ת��
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	char *buf = NULL;
	int i, j, k;
	list<FDIR_R2*>::iterator it_fdir_r;
	map<long, int> lst_isolated;	//�Ѿ����ɹ�����Ŀ���
	lst_isolated.clear();
	map<long, int>::iterator it;
	map<long, FDIR_R2*>::iterator it_map_cb_fdir;

	for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
	{
		(*it_fdir_r)->lst_failed.clear();
	}

	//�Ȼ�ȡ�Ѿ�ִ��ʧ�ܲ��裬������Ϊ�ܶ�
	sprintf(sql, "SELECT CB_ID FROM FDIR_STEP WHERE ID=\'%s\' AND STATUS=%d", group_id, STATUS_FDIR_STEP_FAIL);

	cout << sql << endl;
	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		for (i = 0; i < rec_num; i++)
		{
			char point_name[MAX_DIGITAL_ID_LEN];

			bzero(point_name, sizeof(point_name));

			memcpy(point_name, buf + i * attrs[0].col_width, attrs[0].col_width);

			unsigned long pid = *(long*)point_name;
			KEY_STRU ks = {0};

			CCommon::long_to_keyid(pid, &ks);
			ks.field_id = 0;
			CCommon::keyid_to_long(&ks, &pid);

			for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
			{
				(*it_fdir_r)->lst_failed.push_back(pid);

				printf("\033[41m%ld���ؾܶ�!!\033[0m\r\n", pid);
			}
		}

		g_oci->Readdata_Free();
	}
	else
	{
		cout << "ExecSQL failed:" << sql << endl;
		return FDIR_R_FAIL;
	}

	//�Ȼ�ȡ�Ѿ�ִ�гɹ��ĸ��벽�裬������Ϊ��բ
	sprintf(sql, "SELECT CB_ID FROM FDIR_STEP WHERE ID=\'%s\' AND STATUS=%d AND (STAGE=%d OR STAGE=%d)", group_id, STATUS_FDIR_STEP_OK, STAGE_FDIR_STEP_ISOLATE, STAGE_FDIR_STEP_RE_ISOLATE);

	cout << sql << endl;
	pthread_mutex_lock(&oci_mutex);
	ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		for (i = 0; i < rec_num; i++)
		{
			char point_name[MAX_DIGITAL_ID_LEN];

			bzero(point_name, sizeof(point_name));

			memcpy(point_name, buf + i * attrs[0].col_width, attrs[0].col_width);

			unsigned long pid = *(long*) point_name;
			KEY_STRU ks = { 0 };

			CCommon::long_to_keyid(pid, &ks);
			ks.field_id = 0;
			CCommon::keyid_to_long(&ks, &pid);

			int lv_cb = g_bob->cb_tb->GetCount();

			for (j = 0; j < lv_cb; j++)
			{
				CLS_CB *cb = g_bob->cb_tb->GetDataByPos(j);

				if (cb->pid == (long)pid)
				{
					lst_isolated[cb->id] = j;
					cb->q &= ~CB_OPEN;
					cb->q |= CB_FAULT_ISOLATE;

					printf("���ø��뿪��%ldΪ��բ!!!!\r\n", cb->id);
					break;
				}
			}

			if (j >= lv_cb)
			{
				TRACE("δ�ڿ����б����ҵ��ɹ�����Ŀ��أ�%ld\r\n", pid);
				g_oci->Readdata_Free();
				return FDIR_R_FAIL;
			}

			//������ɹ��Ŀ��ش�����뿪���б�������Ҫ���㸺��ת��
			for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
			{
				//�Ѿ�����ɹ��Ŀ��أ��´��ټ�������ʱ�򣬲�������ڸ��뿪���б��У�����������Ҫ�������������εĻָ�����������Ҫ�ȴ���������
				const TTable<CLS_FDIR_ISOLATE>* isolate = (*it_fdir_r)->getisolate_cbid();

				for (k = 0; k < isolate->GetCount(); k++)
				{
					if (isolate->Data[k].icb == j)
					{
						//���θ��뿪�ز���Ҫ���¼���ָ�����
						if (isolate->Data[k].q & FDIR_ISOLATE_UP)
						{
							break;
						}

						map_cb_fdir[isolate->Data[k].cb] = *it_fdir_r;
						printf("��Ӹ���ɹ��Ŀ���%ld�����뿪���б�������\r\n", isolate->Data[k].cb);
						break;
					}
				}

				//�Ѿ��ҵ��ø��뿪�������ĸ�������
				if (k < isolate->GetCount())
				{
					break;
				}
			}
		}

		g_oci->Readdata_Free();
	}
	else
	{
		cout << "ExecSQL failed:" << sql << endl;
		return FDIR_R_FAIL;
	}

	//�ֱ������뷽��
	for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
	{
		(*it_fdir_r)->FormIcbOfZone();

		if ((*it_fdir_r)->FormIsoSteps() == FDIR_R_FAIL)
		{
			return FDIR_R_FAIL;
		}
		//����Ҫ����Ŀ��ؼ�¼������������Ҫ������������ת��
		else
		{
			TTable<CLS_FDIR_ISOLATE>* isolate = (*it_fdir_r)->getisolate();

			for (i = 0; i < isolate->GetCount(); i++)
			{
				g_bob->cb_tb->Data[isolate->Data[i].icb].q |= CB_FAULT_ISOLATE;

				if (isolate->Data[i].q & FDIR_ISOLATE_UP)
				{
					TRACE("����%ld[%s]Ϊ�����������ο��أ���������ת����\r\n",  isolate->Data[i].cb,  isolate->Data[i].cb_desc);
				}
				else
				{
					TRACE("��Ҫת���ĸ��뿪��%ld[%s]\r\n", isolate->Data[i].cb, isolate->Data[i].cb_desc);
					map_cb_fdir[g_bob->cb_tb->GetDataByPos(isolate->Data[i].icb)->id] = *it_fdir_r;
				}
			}

			//������ɹ��Ŀ��ؼ��뵽���ԵĴ�������Ӧ�ĸ��뿪�ر��Ա��´��Ҵ˿��ض�Ӧ�Ĵ�����
			for (it = lst_isolated.begin(); it != lst_isolated.end(); it++)
			{
				map<long, FDIR_R2*>::iterator it_map_cb = map_cb_fdir.find(it->first);
				if (it_map_cb != map_cb_fdir.end())
				{
					if ((it_map_cb->second) == *it_fdir_r)
					{
						CLS_FDIR_ISOLATE fdir_isolate/* = { 0 }*/;

						fdir_isolate.q |= FDIR_ISOLATE_NO_SO;
						fdir_isolate.id = isolate->GetCount();
						fdir_isolate.icb = it->second;
						fdir_isolate.cb = it->first;

						isolate->Insert(fdir_isolate);

						printf("������ɹ��Ŀ���%ld���뵽���ϴ�����0x%x�ĸ����б�\r\n", fdir_isolate.cb, (unsigned int)*it_fdir_r);
					}
				}
			}
		}
	}

	//�ٽ�����ɹ��Ŀ�������Ϊ�Ͽ�״̬
	for (it = lst_isolated.begin(); it != lst_isolated.end(); it++)
	{
#if 0
		for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
		{
			(*it_fdir_r)->fdir_cb_tb->GetDataByPos(it->second)->topflags |= ~FDIR_CB_OPEN0;
			(*it_fdir_r)->fdir_cb_tb->GetDataByPos(it->second)->topflags |= ~FDIR_CB_OPEN;
		}
#else
		g_bob->cb_tb->GetDataByPos(it->second)->q |= CB_OPEN;
#endif
	}

	//�γɶϿ����뿪�غ�ĵ������뿪��
	for (it_fdir_r = lst_fdir_r.begin(); it_fdir_r != lst_fdir_r.end(); it_fdir_r++)
	{
		(*it_fdir_r)->FormIcbOfZone();
	}

	tmp_map_cb_fdir = map_cb_fdir;

	//�жϸ��뿪�ص����������Ƿ��Ѿ��ָ������Ѿ��ָ��������εĸ��뿪��ɾ�������ټ���ָ�������
	for (it_map_cb_fdir = map_cb_fdir.begin(); it_map_cb_fdir != map_cb_fdir.end(); )
	{
		it = lst_isolated.find(it_map_cb_fdir->first);

		//�ø��뿪��Ϊ�Ѿ��ɹ�������Ŀ��أ�Ҫ�ж������������Ƿ��Ѿ��ָ�
		if(it != lst_isolated.end())
		{
			if (it->second < 0)
			{
				printf("���뿪��%ld���߼���Ϊ-1������\r\n", it_map_cb_fdir->first);
				map_cb_fdir.erase(it_map_cb_fdir);
				it_map_cb_fdir = map_cb_fdir.begin();
				continue;
			}
			TRACE("���뿪���߼����ǣ�%d\r\n", it->second);
			CLS_FDIR_CB *fdir_cb = it_map_cb_fdir->second->fdir_cb_tb->GetDataByPos(it->second);
			CLS_FDIR_ZONE *fdir_izone;
			CLS_FDIR_ZONE *fdir_zzone;

			fdir_izone = it_map_cb_fdir->second->fdir_zone_tb->GetDataByPos(fdir_cb->ind);

			fdir_zzone = it_map_cb_fdir->second->fdir_zone_tb->GetDataByPos(fdir_cb->iznd);


			//�жϸø��뿪�طǹ�������һ�ˣ���ԭ����Ҫ�ָ����������Σ��Ƿ��е�
			//�е������뿪�أ��ø��뿪�������Ѿ��ָ������ø��뿪��ɾ��
			if(fdir_izone->icb != -1  && (fdir_zzone->icb != -1))
			{
				printf("���뿪��%ld�����Ѿ��ָ�������\r\n", it_map_cb_fdir->first);
				map_cb_fdir.erase(it_map_cb_fdir);
				it_map_cb_fdir = map_cb_fdir.begin();
			}
			else
			{
				it_map_cb_fdir++;
			}
		}
		else
		{
			it_map_cb_fdir++;
		}
	}

	//�ٽ�����ɹ��Ŀ�������Ϊ��բ״̬���Լ�����Ҫת�����ص��������Σ�����Ҫת�������Σ�
	for (it = lst_isolated.begin(); it != lst_isolated.end(); it++)
	{
		g_bob->cb_tb->GetDataByPos(it->second)->q &= ~CB_OPEN;
	}

	return recu_calc_restore(lst_fdir_r, map_cb_fdir, lst_steps, tmp_map_cb_fdir);
}

int FDIR_R2::FHZG_Bus(long bus_id, list<list<MAP_SO_NODE> > &fdir_steps, list<long> &lstIsolate)
{
	if (bus_id == 0)
		return -1;

	pid_to_id(bus_id);

	if (BuildDB() != FDIR_R_SUCCEED)
	{
		TRACE("BuildDB failed!\r\n");
		return FDIR_R_FAIL;
	}

//	CLS_FDIR_ZONE *zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount();

	int index;

	for (index = 0; index < lv_fdir_zone; index++)
	{
		CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetDataByPos(index);

		if (fdir_zone->type != _FDIR_SO)
			continue;

		if (fdir_zone->zn_id == bus_id)
		{
			break;
		}
	}

	if (index >= lv_fdir_zone)
	{
		TRACE("ĸ��%ld�Ҳ�����Ӧ�ĵ�Դ���ݣ����ܲ��������ɵ�Դ��������\r\n", bus_id);
		return FDIR_R_FAIL;
	}

	int cbptr = fdir_zone_tb->GetDataByPos(index)->cbptr;
	int cbnum = fdir_zone_tb->GetDataByPos(index)->cbnum;

	TRACE("index = %d, cbptr = %d, cbnum = %d\r\n", index, cbptr, cbnum);

	list<FDIR_R2*> lst_fdir_r;
	lst_fdir_r.push_back(this);

	map<long, FDIR_R2*> map_iso, tmp_map_iso;

	for (int i = 0; i < cbnum; i++)
	{
		int icb = fdir_zone_cb_tb->GetDataByPos(cbptr + i)->icb;
		CLS_CB *cb = dsbob->cb_tb->GetDataByPos(icb);

		if ((cb->q & CB_FDHCB) && !(cb->q & CB_OPEN))	//ֻ�Ժ�բ�Ľ��߿��ؽ��и���ת��
		{
			map_iso[cb->id] = this;
			lstIsolate.push_back(cb->id);
		}
		else
		{
			TRACE("����%ld[%s]��������ת����q:%d\r\n", cb->id, cb->descr, cb->q);
		}
	}

	tmp_map_iso = map_iso;

	return recu_calc_restore(lst_fdir_r, map_iso, fdir_steps, tmp_map_iso);
}


