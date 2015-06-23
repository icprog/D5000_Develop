#include <stdlib.h>
#include <iostream>
#include"typical.h"

inline ostream& operator <<(ostream &os, const CB_INFO &data)
{
	os << data.id << "," << data.type << endl;
	return os;
}

inline ostream& operator <<(ostream &os, const BAND_TREE_DATA &data)
{
	os << "[段边界开关：";

	for (map<long, CB_INFO>::iterator it = data->map_band_cb.begin(); it != data->map_band_cb.end(); it++)
	{
		os << it->first << ",";
	}

	os << "]";

	return os;
}

//inline bool operator <(const BAND_TREE_DATA &left, const BAND_TREE_DATA &right)
//{
//	return &(*left) < &(*right);
//}
//inline bool operator >(const BAND_TREE_DATA &left, const BAND_TREE_DATA &right)
//{
//	return &(*left) > &(*right);
//}

typical::typical(FDIR_R2 *fdir_r2, const char* id)
{
	assert(id != NULL);

	strcpy(m_id, id);
	m_fdir = fdir_r2;
}

typical::~typical()
{

}

void build_tree(long &cb, map<long, list<long> > &map_up_cb, map<long, CB_INFO> &map_cb_info, Tree<CB_INFO> *pTree, TreeNode<CB_INFO>* pParent)
{
	TreeNode<CB_INFO>* pnewParent = pTree->TreeAddLeaf(pParent, map_cb_info.find(cb)->second);

	map<long, list<long> >::iterator it = map_up_cb.find(cb);

	if(it != map_up_cb.end())
	{
		for(list<long>::iterator it_tmp = it->second.begin(); it_tmp != it->second.end(); it_tmp++)
		{
			build_tree(*it_tmp, map_up_cb, map_cb_info, pTree, pnewParent);
		}
	}
}

bool typical::typical_read_cb()
{
	assert(m_id != NULL);

	map<long, long> map_dv_cb;			//馈线与对应的进线开关映射
	map<long, list<long> > map_up_cb;	//开关string与其所有下游开关list<string>映射
	map<long, CB_INFO> map_cb_info;		//开关与开关信息映射

	char *buf = NULL;
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;

	sprintf(sql, "SELECT CB, UP_CB, DV, STATUS, IA, IB, IC, TYPE FROM TYPICAL_CB WHERE ID=\'%s\'", m_id);

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);

	if (ret != OCI_ERROR)
	{
		int attr_len = 0;

		for (int j = 0; j < attr_num; j++)
		{
			attr_len += attrs[j].col_width;
		}

		for (int i = 0; i < rec_num; i++)
		{
			CB_INFO info = { 0 };
			char dv[100] = { 0 };
			char cb[100] = { 0 };
			char up_cb[100] = { 0 };
			char status[10] = { 0 };
			char Ia[10] = { 0 };
			char Ib[10] = { 0 };
			char Ic[10] = { 0 };
			char Type[10] = { 0 };

			memcpy(cb, buf + i * attr_len, attrs[0].col_width);
			memcpy(up_cb, buf + i * attr_len + attrs[0].col_width, attrs[1].col_width);
			memcpy(dv, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width, attrs[2].col_width);
			memcpy(status, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width, attrs[3].col_width);
			memcpy(Ia, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width, attrs[4].col_width);
			memcpy(Ib, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width + attrs[4].col_width, attrs[5].col_width);
			memcpy(Ic, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width + attrs[4].col_width + attrs[5].col_width, attrs[6].col_width);
			memcpy(Type, buf + i * attr_len + attrs[0].col_width + attrs[1].col_width + attrs[2].col_width + attrs[3].col_width + attrs[4].col_width + attrs[5].col_width + attrs[6].col_width, attrs[7].col_width);

			if (up_cb[0] == 0)
			{
				map_dv_cb[*(long*)dv] = *(long*)cb;
			}
			else
			{
				map_up_cb[*(long*)up_cb].push_back(*(long*)cb);
			}

			info.id = *(long*)cb;
			info.Ia = *(float*)(Ia);
			info.Ib = *(float*)(Ib);
			info.Ic = *(float*)(Ic);
			info.status = (CB_STATUS)(*(int*)status);
			info.type = (CB_TYPE)(*(int*)Type);

			map_cb_info[*(long*)cb] = info;
		}

		g_oci->Readdata_Free();
	}
	else
	{
		return false;
	}

	for(map<long, long>::iterator it = map_dv_cb.begin(); it != map_dv_cb.end(); it++)
	{
		build_tree(it->second, map_up_cb, map_cb_info, &m_map_dv_cb[it->first], NULL);
	}
	return true;
}
bool typical::typical_write_cb()
{
	map<long, Tree<CB_INFO> >::iterator map_vi;
	//TreeNode<CB_INFO> *pchild = NULL;

	for (map_vi = m_map_dv_cb.begin(); map_vi != m_map_dv_cb.end(); map_vi++)
	{
		long cb = map_vi->second.root->data.id;
		long up_cb = map_vi->first;

		id_to_pid(cb);
		id_to_pid(up_cb);
		
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_CB (ID, CB, UP_CB, DV, STATUS, IA, IB, IC, TYPE) values(\'%s\', \'%ld\', \'\', \'%ld\', %d, %f, %f, %f, 1)",
				m_id, cb, up_cb, map_vi->second.root->data.status, map_vi->second.root->data.Ia, map_vi->second.root->data.Ib, map_vi->second.root->data.Ic);

		ExecSQL(sql);

		formsql_from_tree(m_id, map_vi->first, map_vi->second.root);
	}
	return true;
}
void typical::formsql_from_tree(const char* id, const long dv, TreeNode<CB_INFO> *child)
{
	if (child == NULL)
	{
		TRACE("树为空！遍历结束。");
		return;
	}
	if (child->childnode == NULL)
	{
		return;
	}
	TreeNode<CB_INFO>* ptem = child->childnode;
	while (ptem)
	{
		long cb = ptem->data.id;
		long up_cb = child->data.id;

		id_to_pid(cb);
		id_to_pid(up_cb);

		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_CB (ID, CB, UP_CB, DV, STATUS, IA, IB, IC, TYPE) values(\'%s\', \'%ld\', \'%ld\', \'%ld\', %d, %f, %f, %f, %d)",
				id, cb, up_cb, dv, ptem->data.status,ptem->data.Ia, ptem->data.Ib, ptem->data.Ic, (int)ptem->data.type);

		ExecSQL(sql);

		formsql_from_tree(id, dv, ptem);
		ptem = ptem->next;
	}
}

PSBOB::CB tmp_cb_info;
bool typical::get_cb_info(CB_INFO &cb)
{
	bzero(&tmp_cb_info, sizeof(PSBOB::CB));

	DSBOB *dsbob = m_fdir->get_dsbob();
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

int typical::get_dv_cb(int izone, const long fathercb, TreeNode<CB_INFO> *father, Tree<CB_INFO> &tree_cb)
{
	assert(izone != -1);
	int i, nextzone, ret, icb;
	CB_INFO cbnode;
	TreeNode<CB_INFO> *tmp_father;
	CLS_CB *cb;
	DSBOB *dsbob = m_fdir->get_dsbob();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = m_fdir->fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = m_fdir->fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = m_fdir->fdir_cb_tb->GetTable();
	//int lv_fdir_cb = m_fdir->fdir_cb_tb->GetCount();
	//int lv_fdir_zone = m_fdir->fdir_zone_tb->GetCount();
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

		cout<<"^^^^^^^^^^^^^^^^^^^^^^^^***"<<cb->id<<endl;
		//判断开关有没有被处理过
		if((fdir_cb + icb)->q & FDIR_CB_UNPROC)
		{
			//如果是进线开关停止搜索树,并且把进线开关作为这棵树的节点
			if(cb->q & CB_FDHCB)
			{
				//TreeNode<CB_INFO> *newroot = tmp_father;
				memset(&cbnode, 0, sizeof(cbnode));
				cbnode.id = cb->id;

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
					cbnode.id = cb->id;
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
					m_fdir->GetDVByZone(nextzone, dv);

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
						ret = get_dv_cb(nextzone, cb->id, m_map_dv_cb[dv.id].root, m_map_dv_cb[dv.id]);
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
						ret = get_dv_cb(nextzone, cb->id, tmp_father, tree_cb);
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
bool typical::typical_save(const long cb_id, const char *machine, const char *userid)
{
	assert(cb_id > 0);
	m_map_dv_cb.clear();
	//CLS_ZN *tempzone;
	int i, icb = -1, izone;
	map<string, Tree<CB_INFO> > temp_map;
	PSBOB::DV dv = { 0 };
	//PSBOB::ST st = { 0 };
	//PSBOB::OO_R_DV_ST oo_r_dv_st = { 0 };
	DSBOB *dsbob = m_fdir->get_dsbob();
	//CLS_FDIR_ZONE_CB* fdir_zone_cb = m_fdir->fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = m_fdir->fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = m_fdir->fdir_cb_tb->GetTable();
	int lv_fdir_cb = m_fdir->fdir_cb_tb->GetCount();
	int lv_fdir_zone = m_fdir->fdir_zone_tb->GetCount();

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
	TRACE("典型保存获取到开关:%ld的记录 获取到的icb: %d      %d \n", id,icb,(fdir_zone + (fdir_cb + icb)->ind)->type);
	if (icb == -1)
	{
		TRACE("没有获取到开关:%ld的记录\n", id);
		return false;
	}
	//cout<<"======"<<(fdir_cb + icb)->id<<(fdir_cb + icb)->descr <<"--"<<(fdir_zone + (fdir_cb + icb)->ind)->type <<endl;
	//获取进线开关的下游区段
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

	//设置进线开关为处理过的开关
	(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
	if (izone != -1)
	{
		//设置此区段izone为已经处理过的，并调用处理函数
		(fdir_zone + izone)->q &= ~FDIR_ZONE_UNPROC;
		//获取馈线ID
		m_fdir->GetDVByZone(izone, dv);
		if (dv.id <= 0)
		{
			TRACE("\033[31m********************LWCB id is %s getdv failed**************\033[0m\r\n",cb_id);
			return false;
		}

		if (get_dv_cb(izone, id, m_map_dv_cb[dv.id].root, m_map_dv_cb[dv.id]) == 0)
		{
			//打印信息
			for(map<long, Tree<CB_INFO> >::iterator print_it = m_map_dv_cb.begin(); print_it != m_map_dv_cb.end(); print_it++)
			{
				list<TreeNode<CB_INFO>*> so_tree;
				list<TreeNode<CB_INFO>*>::iterator list_vi;

				print_it->second.DepthFirstErgodic(print_it->second.root,so_tree);//深度优先遍历
				for(list_vi = so_tree.begin(); list_vi != so_tree.end(); list_vi++)
				{
					TRACE("树的开关id是%ld,馈线是%ld\n",(*list_vi)->data.id, print_it->first);
				}
				TRACE("!!!!!!!!!!!!!!!!!!newtree\n");
			}
			list<TreeNode<CB_INFO>*> so_tree;
			//获取正确的DV值，替换m_map_dv_cb中的key值
			for (map<long, Tree<CB_INFO> >::iterator it = m_map_dv_cb.begin(); it != m_map_dv_cb.end(); it++)
			{
				//获取开关所在的馈线信息

				list<TreeNode<CB_INFO>*>::iterator list_vi;
				so_tree.clear();
				it->second.DepthFirstErgodic(it->second.root, so_tree);
				list<TreeNode<CB_INFO>*>::iterator it_vi;
				//如果根节点是进线开关则不需要翻转树
				if (it->second.root->data.type != JXCB)
				{
					
					//查找进线开关的位置，并且进线翻转
					for (it_vi = so_tree.begin(); it_vi != so_tree.end(); it_vi++)
					{
						TRACE("树的开关id是%ld,馈线是%ld\n",(*it_vi)->data.id, it->first);
						//不能通过进线开关查找开关所在的馈线,联络开关的馈线可能是开关另一侧的馈线,不能通过分线箱开关查找馈线
						if ((*it_vi)->data.type == JXCB)
						{
							//以进线开关为根节点新生成树
							Tree<CB_INFO> newtree;

							it->second.ChangeRoot(*it_vi, newtree, newtree.root);
							m_map_dv_cb[it->first] = newtree;
							break;
						}
					}
					if ( it_vi == so_tree.end())
					{
						TRACE("馈线是 %ld 不存在进线开关\n", it->first);
					}	
				}
				else
				{
					//把典型方式信息写入TYPICAL_INFO表中
					sprintf(sql, "SELECT ID FROM PMS_PARAM.TYPICAL_INFO WHERE CB = \'%ld\'", cb_id);

					char *buf = NULL;
					char typical_id[100];
					int rec_num, attr_num;
					struct ORA_ATTR *attrs = NULL;

					pthread_mutex_lock(&oci_mutex);
					int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
					pthread_mutex_unlock(&oci_mutex);
					if (ret != OCI_ERROR && rec_num > 0)
					{
						memset(typical_id, 0, sizeof(typical_id));
						memcpy(typical_id, buf, attrs[0].col_width);

						g_oci->Readdata_Free();

						//先删除上次保存的此开关相关的馈线开关信息
						sprintf(sql, "DELETE FROM PMS_PARAM.TYPICAL_INFO WHERE CB = \'%ld\'", cb_id);
						ExecSQL(sql);

						sprintf(sql, "DELETE FROM PMS_PARAM.TYPICAL_CB WHERE id = \'%s\'", typical_id);
						ExecSQL(sql);
					}

					sprintf(sql, "Insert into PMS_PARAM.TYPICAL_INFO (ID, DV_ID, DV_DESCR, MACHINE, USERID, CB) Values (\'%s\', \'%ld\', \'%s\', \'%s\', \'%s\', \'%ld\')",
							m_id, dv.id, dv.name, machine, userid, cb_id);

					ExecSQL(sql);
				}

			}

			typical_write_cb();
			return true;
		}
		else
		{
			TRACE("获取馈线上的开关失败 \n");
			return false;
		}
	}
	else
	{
		TRACE("获取进线开关的下游区段失败\n");
		return false; 
	}
}

bool typical::typical_restore(const long cb_id, const char *machine, const char *userid, string &id, time_t &restore_time)
{
	assert(cb_id > 0);
	//首先获取开关对应的dv恢复的id，查历史库
	char *buf = NULL;
	PSBOB::CB psbob_cb = {0};
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	restore_time = time(NULL);
	struct tm tmtime;
	list<TYPICAL_STEP> lst_step;
	localtime_r(&restore_time,&tmtime);
	lst_step.clear();

	if (oodbread_rk(&psbob_cb, (void *)&cb_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) == OO_FAIL)
	{
		TRACE("%s(%d):读取ID为%s的CB记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, cb_id, _oodb_errno);
		return false;
	}

	sprintf(sql, "SELECT ID, DV_ID, CB FROM TYPICAL_INFO WHERE CB=\'%ld\'", cb_id);

	pthread_mutex_lock(&oci_mutex);
	int ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
	if(rec_num <= 0)
	{
		TRACE("获取典型保存状态失败,没有查找当当前CB的记录!!!\n");	
		return false;
	}

	if (ret != OCI_ERROR)
	{
		memset(m_id, 0, sizeof(m_id));
		strncpy(m_id, buf, attrs[0].col_width);
		id = m_id;
		g_oci->Readdata_Free();
	}
	else
	{
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
				m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, machine, userid, 0);

		ExecSQL(sql);
		return false;
	}

	if (!typical_read_cb())
	{
		TRACE("读取典型方式开关信息失败！\r\n");
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
				m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, machine, userid, 0);

		ExecSQL(sql);
		return false;
	}

	if (!typical_get_main_cb())
	{
		TRACE("获取主干线开关信息失败！\r\n");
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
				m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, machine, userid, 0);
		ExecSQL(sql);
		return false;
	}

	typical_band();//对馈线进行分段（按从进线开关往外进行排序）

	if (!typical_form_band_tree())
	{
		TRACE("形成段树失败！\r\n");
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
				m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, machine, userid, 0);

		ExecSQL(sql);
		return false;
	}

//	if(typical_verify(lst_step))
//	{
//		TRACE("形成恢复方案失败！\r\n");
//		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
//				m_id, (tmtime->tm_mon + 1), tmtime->tm_mday, (tmtime->tm_year + 1900), tmtime->tm_hour, tmtime->tm_min, tmtime->tm_sec, machine, userid, 0);
//
//		ExecSQL(sql);
//		return false;
//	}

	typical_get_all_cb(cb_id);

	if (!typical_deal_break(lst_step,cb_id))
	{
		TRACE("形成恢复非主干开关方案失败！\r\n");
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
				m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, machine, userid, 0);

		ExecSQL(sql);
		return false;
	}

	if (!typical_deal_band(lst_step))
	{
		TRACE("形成恢复方案失败！\r\n");
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
				m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, machine, userid, 0);

		ExecSQL(sql);
		return false;
	}

	
	cout<<"----------------打印恢复方案!@!"<<endl;
	list<TYPICAL_STEP> ::iterator list_it;
	for(list_it = lst_step.begin();list_it != lst_step.end();list_it++)
	{
		cout<<list_it->cb<<endl;
		if(list_it->bopen)
			cout<<"分闸"<<endl;
		else
			cout<<"合闸"<<endl;
		
		if(list_it->boptional)
			cout<<"可选"<<endl;
		else
			cout<<"不可选"<<endl;

	}

	int step_num = 0;
	for (list<TYPICAL_STEP>::iterator it = lst_step.begin(); it != lst_step.end(); it++)
	{
		TRACE("%ld,%s,%s\r\n", it->cb, it->bopen ? "开" : "合", it->boptional ? "可选" : "必选");
		sprintf(sql, "Insert into PMS_PARAM.TYPICAL_STEP (ID, TIME, STEP, CB, OPERATION, OPTIONAL, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), %d, \'%ld\', %d, %d, %d)",
				m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, step_num, it->cb, it->bopen ? 2 : 1,it->boptional ? 1 : 0, 0);

		ExecSQL(sql);
		step_num++;
	}

	sprintf(sql, "Insert into PMS_PARAM.TYPICAL_RESTORE (ID, TIME, MACHINE, USERID, STATUS) Values (\'%s\', TO_DATE(\'%d/%d/%d %d:%d:%d\', \'MM/DD/YYYY HH24:MI:SS\'), \'%s\', \'%s\', %d)",
			m_id, (tmtime.tm_mon + 1), tmtime.tm_mday, (tmtime.tm_year + 1900), tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec, machine, userid, 0);

	ExecSQL(sql);

	return true;
}

bool typical::typical_deal_break(list<TYPICAL_STEP> & lst_step,const long cb_id)
{
	int rec_num, attr_num;
	struct ORA_ATTR *attrs = NULL;
	char *tbuf = NULL;
	int ret = 0;
		
	CB_INFO cbnode;
	for(map<long, CB_INFO>::iterator it = all_map_cb.begin();it != all_map_cb.end();it++)
	{
		map<long, CB_INFO>::iterator m_it = m_map_cb.find(it->first);

		if(m_it == m_map_cb.end())//非主干线开关
		{
			cbnode.id = it->first;
			if(!get_cb_info(cbnode))
			{
				cout<<"获取开关信息失败:"<<cbnode.id<<endl;
				continue;
			}

			if(cbnode.status != it->second.status )
			{
				if(it->second.status)//如果是合闸需要考虑线路是否过载问题
				{
					lst_step.push_back(TYPICAL_STEP(cbnode.id, false, true));
				}
				else//如果典型是断开，那么当断开时就有限考虑是否过载，如果过载那么就非可选的
				{
					float max_i = max(cbnode.Ia,max(cbnode.Ib,cbnode.Ic));
					float typical_max_i = 0;
					long id = it->second.dv_id;
					//获取典型状态下的馈线额定电流和使用电流
					sprintf(sql, "SELECT ia,ib,ic FROM EMS.PMS_PARAM.TYPICAL_CB WHERE  DV = %ld and type = 1",id);

					pthread_mutex_lock(&oci_mutex);
					ret = g_oci->ReadWithBind(sql, &tbuf, &rec_num, &attr_num, &attrs);
					pthread_mutex_unlock(&oci_mutex);
					if(rec_num <= 0)
					{
						TRACE("获取进线开关的开关状态失败失败！%s\r\n",sql);
						return false;
					}else if(ret != OCI_ERROR)
					{
						if(tbuf == NULL)
							cout<<"返回数据为空!!!"<<endl;

						double ia = *(double*)tbuf;
						tbuf = tbuf + attrs[0].col_width;
						
						double ib = *(double*)tbuf;
						tbuf = tbuf + attrs[1].col_width;
						
						double ic = *(double*)tbuf;

						typical_max_i = max(ia,max(ib,ic));

						g_oci->Readdata_Free();
					}
					
					subcontrolarea_dscada subcontrolarea;
					memset(&subcontrolarea, '\0', sizeof(subcontrolarea_dscada));

					if ((ret = GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), (char*) &id, sizeof(subcontrolarea_dscada), (char*) &subcontrolarea)) < 0)
					{
						TRACE("Update 读取subcontrolarea表记录失败！id = %ld\r\n", id);
						continue;
					}
					TRACE("获取进线开关最大电流: %f  获取开关的最大电流:%f  馈电额定电流: %f \n", typical_max_i,max_i,subcontrolarea.amprating);

					if((typical_max_i + max_i) >= subcontrolarea.amprating)
					{
						lst_step.push_back(TYPICAL_STEP(cbnode.id, true, false));
					}else
					{
						lst_step.push_back(TYPICAL_STEP(cbnode.id, true, true));
					}	
					
				}
			}
		}

	}
	
	return true;
}

bool typical::typical_get_all_cb(const long cb_id)
{
		int rec_num, attr_num;
		struct ORA_ATTR *attrs = NULL;
		char *buf = NULL,*pbuf = NULL;
		

		long ret = 0;
		
		char time[50]="";
	
		sprintf(sql, "SELECT ID FROM TYPICAL_INFO WHERE CB=\'%ld\'", cb_id);
		
		pthread_mutex_lock(&oci_mutex);
		ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
		pthread_mutex_unlock(&oci_mutex);
		if(rec_num <= 0)
	 	{ 	
		 	return false;
		}
	
		if (ret != OCI_ERROR && rec_num > 0)
		{
			memset(time, 0, sizeof(time));
			memcpy(time, buf, attrs[0].col_width);
	
			g_oci->Readdata_Free();
			
		}else
		{
			return false;
		}
		
		sprintf(sql, "SELECT CB,STATUS,ia,ib,ic,type,dv FROM TYPICAL_CB WHERE ID=\'%s\'", time);
		
		pthread_mutex_lock(&oci_mutex);
		ret = g_oci->ReadWithBind(sql, &buf, &rec_num, &attr_num, &attrs);
		pthread_mutex_unlock(&oci_mutex);
	
		all_map_cb.clear();
		CB_INFO cb;
		if (ret != OCI_ERROR)
		{
			pbuf = buf;
			for (int i = 0; i < rec_num; i++)
			{
				cb.id = *(long*)pbuf;
				pbuf = pbuf + attrs[0].col_width;
				cb.status = *(int*)pbuf;
				pbuf = pbuf + attrs[1].col_width;
				cb.Ia= *(float*)pbuf;
				pbuf = pbuf + attrs[2].col_width;
				cb.Ib= *(float*)pbuf;
				pbuf = pbuf + attrs[3].col_width;
				cb.Ic = *(float*)pbuf;
				pbuf = pbuf + attrs[4].col_width;
				cb.type= *(int*)pbuf;
				pbuf = pbuf + attrs[5].col_width;
				cb.dv_id= *(long*)pbuf;
				pbuf = pbuf + attrs[6].col_width;

				all_map_cb[cb.id] = cb;
			}
			g_oci->Readdata_Free();
		}
		else
		{
			TRACE("获取所有开关信息失败!! %s \n",sql);
			return false;
		}

	return true;
}

bool typical::typical_get_main_cb()
{
	map<long, Tree<CB_INFO> >::iterator dv_it;
	list<TreeNode<CB_INFO>*> tree_cb;
	list<TreeNode<CB_INFO>*>::iterator tree_vi;
	list<TreeNode<CB_INFO>*> lst_main_cb;
	m_map_cb.clear();
	//获取每条馈线上的主干线开关
	for(dv_it = m_map_dv_cb.begin(); dv_it != m_map_dv_cb.end(); dv_it++)
	{
		tree_cb.clear();
		//遍历这个馈线的开关树
		dv_it->second.DepthFirstErgodic(dv_it->second.root, tree_cb);
		for (tree_vi = tree_cb.begin(); tree_vi != tree_cb.end(); tree_vi++)
		{
			lst_main_cb.clear();
			if((*tree_vi)->data.type == LWCB)
			{
	
				if(dv_it->second.GetPath(dv_it->second.root, *tree_vi, lst_main_cb))
				{
					for(list<TreeNode<CB_INFO>*>::iterator path_vi = lst_main_cb.begin(); path_vi != lst_main_cb.end(); path_vi++)
					{
						if((*path_vi)->flag & _NODE_UNPROC)
						{
							(*path_vi)->flag &= ~_NODE_UNPROC;
							
							TRACE("the main cb is %ld\n",(*path_vi)->data.id);
							
							CB_INFO cb;
							cb.id = (*path_vi)->data.id;
							cb.status = (*path_vi)->data.status;
							cb.Ia= (*path_vi)->data.Ia;
							cb.Ib = (*path_vi)->data.Ib;
							cb.Ic = (*path_vi)->data.Ic;
							cb.type= (*path_vi)->data.type;
							m_map_cb[cb.id] = cb;
						}
					}
				}
				else
				{
					TRACE("p1 data is %s , p2 data is %s \n",dv_it->second.root->data.id, (*tree_vi)->data.id);
				}
			}
		}
	}

	return true;
}

void typical::typical_band()
{
	map<long, Tree<CB_INFO> >::iterator dv_it;
	m_map_dv_band.clear();

	//分别对每条馈线进行分段,断开的主干线开关、进线开关、联络开关为边界
	for(dv_it = m_map_dv_cb.begin(); dv_it != m_map_dv_cb.end(); dv_it++)
	{
		BAND_INFO tempb;
		CB_INFO cbnode;
		cout<<m_map_dv_cb.size()<<endl;
		cbnode.id = dv_it->second.root->data.id;
		
		if(!get_cb_info(cbnode))
		{
			TRACE("获取进线开关的开关信息失败，开关id是%s,分段失败!\n", cbnode.id);
			return;
		}
		cout<<"获取进线开关，进行分段  "<<cbnode.id<<endl;
		tempb.map_band_cb[cbnode.id] = cbnode;
		tempb.map_band_cb_typical[cbnode.id] = dv_it->second.root->data;
		
		tree_to_band(dv_it->second.root, dv_it->second, tempb, m_map_dv_band[dv_it->first]);
		m_map_dv_band[dv_it->first].push_back(tempb);
		TRACE("新段 %d  \n",tempb.map_band_cb.size());
		//for(map<long, CB_INFO>::iterator cb_vi = tempb.map_band_cb.begin(); cb_vi != tempb.map_band_cb.end(); cb_vi++)
		//{
		//	TRACE("++++++++此段的边界是%ld\n",cb_vi->first);
		//}

	}

	/*map<long, list<BAND_INFO> >::iterator band_it;
	
	for(band_it = m_map_dv_band.begin(); band_it != m_map_dv_band.end(); band_it++)
	{
		TRACE("馈线是 %ld \n", band_it->first);

		for(list<BAND_INFO>::iterator vi = band_it->second.begin(); vi != band_it->second.end(); vi++)
		{
			TRACE("段的边界开关是 \n");
			for(map<long, CB_INFO>::iterator cb_vi = vi->map_band_cb.begin(); cb_vi != vi->map_band_cb.end(); cb_vi++)
			{
				printf("%ld ", cb_vi->first);
			}
			printf("\n");
		}
	}*/
}
void typical::tree_to_band(TreeNode<CB_INFO>* pParent, Tree<CB_INFO> &pTree, BAND_INFO &bandnode, list<BAND_INFO> &lst_band)
{
	assert(pParent != NULL);
	BAND_INFO tband;
	CB_INFO cbnode;
	TreeNode<CB_INFO>* pChild = pTree.GetChildNode(pParent);
	while(pChild != NULL)
	{
		//只处理是主干线的开关
		if(!(pChild->flag & _NODE_UNPROC))
		{
			//获取当前状态下开关的状态信息
			cbnode.id = pChild->data.id;
			get_cb_info(cbnode);
			if(pChild->data.type == LWCB)
			{
				bandnode.map_band_cb[cbnode.id] = cbnode;
				bandnode.map_band_cb_typical[cbnode.id] = pChild->data;
			}
			else
			{
				if(cbnode.status == pChild->data.status)
				{
					tree_to_band(pChild, pTree, bandnode, lst_band);
				}
				else
				{
					bandnode.map_band_cb[cbnode.id] = cbnode;
					bandnode.map_band_cb_typical[cbnode.id] = pChild->data;
					tband.map_band_cb[cbnode.id] = cbnode;
					tband.map_band_cb_typical[cbnode.id] = pChild->data;
					tree_to_band(pChild, pTree, tband, lst_band);
					lst_band.push_back(tband);
					TRACE("1新段\n");
					for(map<long, CB_INFO>::iterator cb_vi = tband.map_band_cb.begin(); cb_vi != tband.map_band_cb.end(); cb_vi++)
					{
						TRACE("1++++++++此段的边界是%ld\n",cb_vi->first);
					}
				}
			}
		}
		pChild = pChild->next;
	}
}

void typical::typical_form_band_tree_core(Tree<BAND_TREE_DATA>* pTree, TreeNode<BAND_TREE_DATA>* pParent, map<long, CB_BAND_POS>* p_map_cb_band, const long cb_id, BAND_TREE_DATA band, const long cb_jxcb, bool btypical)
{
	assert(pTree != NULL);
	assert(p_map_cb_band != NULL);
	assert(cb_id > 0);

	map<long, CB_BAND_POS>::iterator it = p_map_cb_band->find(cb_id);

	if (it != p_map_cb_band->end())
	{
		BAND_TREE_DATA iBand;

		if (band == it->second.iBand)
			iBand = it->second.zBand;
		else
			iBand = it->second.iBand;

		if(iBand == NULL) return;

		TreeNode<BAND_TREE_DATA>* pnewParent = pTree->TreeAddLeaf(pParent, iBand);

		if (btypical)//典型方式
		{
			m_map_band_pos[iBand].pTypicalNode = pnewParent;
			iBand->so_cb_typical = cb_jxcb;
			TRACE("典型方式下的供电进线开关 %ld \n ",cb_jxcb);
			iBand->supply_cb_typical = it->first;

			for (map<long, CB_INFO>::iterator it_map = iBand->map_band_cb_typical.begin(); it_map != iBand->map_band_cb_typical.end(); it_map++)
			{
				if(it_map->first == cb_id) continue;

				if (it_map->second.status == TYPICAL_OPEN) //开关合闸
				{
					typical_form_band_tree_core(pTree, pnewParent, p_map_cb_band, it_map->first, iBand, cb_jxcb, btypical);
				}
			}
		}
		else	//当前方式
		{
			m_map_band_pos[iBand].pNode = pnewParent;
			iBand->so_cb = cb_jxcb;
			TRACE("当前方式下的供电进线开关 %ld \n",cb_jxcb);
			iBand->supply_cb = it->first;

			for (map<long, CB_INFO>::iterator it_map = iBand->map_band_cb.begin(); it_map != iBand->map_band_cb.end(); it_map++)
			{
				cout<<"++++++"<<it_map->first<<"---------"<<it_map->second.id<<endl;
				if(it_map->first == cb_id) continue;
	
				if (it_map->second.status == TYPICAL_OPEN) //开关合闸
				{
					typical_form_band_tree_core(pTree, pnewParent, p_map_cb_band, it_map->first, iBand, cb_jxcb, btypical);
				}
			}
		}
	}
}

bool typical::typical_form_band_tree()
{
	map<long, CB_BAND_POS> map_cb_band;	//开关与所在的段映射关系
	list<long> lst_jxcb;	//进线开关列表
	list<long> lst_lwcb;  //联络开关列表
	map_cb_band.clear();

	//将开关与其所在的段形成映射方便通过给出开关名快速找到该开关所在的段，并将进线开关记录下来
	for(map<long, list<BAND_INFO> >::iterator it_map = m_map_dv_band.begin(); it_map != m_map_dv_band.end(); it_map++)
	{
		for(list<BAND_INFO>::iterator it_list = it_map->second.begin(); it_list != it_map->second.end(); it_list++)
		{
			for(map<long, CB_INFO>::iterator it_cb = it_list->map_band_cb_typical.begin(); it_cb != it_list->map_band_cb_typical.end(); it_cb++)
			{
				map<long, CB_BAND_POS>::iterator it_cb_band = map_cb_band.find(it_cb->first);

				if(it_cb_band == map_cb_band.end())
					map_cb_band[it_cb->first].iBand = &(*it_list);
				else
					it_cb_band->second.zBand = &(*it_list);

				if(it_cb->second.type == JXCB)
				{
					lst_jxcb.push_back(it_cb->first);
				}
				else if(it_cb->second.type == LWCB)
				{
					lst_lwcb.push_back(it_cb->first);
				}
			}
		}
	}

	//处理特殊的联络开关（典型和当前联络开关都是闭合状态时）
	lst_lwcb.sort();
	lst_lwcb.unique();
	//typical_deal_lwcb(&map_cb_band, &lst_lwcb);

	//以每个合闸的进线开关（合闸的进线开关可供电）为供电点搜索通过合闸的边界开关关联的段
	for(list<long>::iterator it = lst_jxcb.begin(); it != lst_jxcb.end(); it++)
	{	
		map<long, CB_BAND_POS>::iterator it_band = map_cb_band.find(*it); //找到该开关所在的段

		//形成当前方式下的树
		map<long, CB_INFO>::iterator it_cb = it_band->second.iBand->map_band_cb.find(*it); //再找到对应的开关信息

		if (it_cb->second.status == TYPICAL_OPEN)
		{
			typical_form_band_tree_core(&m_map_tree_band[*it], NULL, &map_cb_band, *it, NULL, *it, false);
		}

		//形成典型方式下的树
		map<long, CB_INFO>::iterator it_cb_typical = it_band->second.iBand->map_band_cb_typical.find(*it); //再找到对应的开关信息

		if (it_cb_typical->second.status == TYPICAL_OPEN)
		{
			typical_form_band_tree_core(&m_map_tree_band_typical[*it], NULL, &map_cb_band, *it, NULL, *it, true);
		}
	}

	return true;
}

void typical::typical_deal_lwcb(map<long, CB_BAND_POS> *map_cb_band, list<long> *lst_lwcb)
{
	list<long>::iterator it_lwcb;
	map<long, CB_BAND_POS>::iterator it_cb_band;		//联络开关对应的段
	map<long, CB_INFO>::iterator node;
	map<long, CB_INFO>::iterator typical_node;
	for(it_lwcb = lst_lwcb->begin(); it_lwcb != lst_lwcb->end(); it_lwcb++)
	{
		it_cb_band = map_cb_band->find(*it_lwcb);
		if(it_cb_band != map_cb_band->end())
		{
			node = it_cb_band->second.iBand->map_band_cb.find(*it_lwcb);
			typical_node = it_cb_band->second.iBand->map_band_cb_typical.find(*it_lwcb);

			//考虑联络开关一端空挂的情况
			if (it_cb_band->second.zBand == NULL)
			{
				continue;
			}

			//只处理典型状态和当前状态都是闭合状态下的联络开关
			if(typical_node != it_cb_band->second.iBand->map_band_cb_typical.end() && node != it_cb_band->second.iBand->map_band_cb.end() && node->second.status == TYPICAL_OPEN && typical_node->second.status == TYPICAL_OPEN)
			{
				for(node = it_cb_band->second.iBand->map_band_cb.begin(); node != it_cb_band->second.iBand->map_band_cb.end(); node++)
				{
					it_cb_band->second.zBand->map_band_cb[node->first] = node->second;
				}
				for(node = it_cb_band->second.iBand->map_band_cb_typical.begin(); node != it_cb_band->second.iBand->map_band_cb_typical.end(); node++)
				{
					it_cb_band->second.zBand->map_band_cb_typical[node->first] = node->second;

					//map_cb_band中的iband，zband指针也要相应的更改，两个段共有的联络开关就不需要重新设置
					if(node->first == *it_lwcb)
					{
						continue;
					}
					else
					{
						map<long, CB_BAND_POS>::iterator node_it;
						node_it = map_cb_band->find(node->first);
						if(node_it != map_cb_band->end())
						{
							if(node_it->second.iBand == it_cb_band->second.iBand)
							{
								(*map_cb_band)[node->first].iBand = it_cb_band->second.zBand;
							}
							else
							{
								(*map_cb_band)[node->first].zBand = it_cb_band->second.zBand;
							}
						}
					}
				}

				it_cb_band->second.zBand->map_band_cb.erase(*it_lwcb);
				it_cb_band->second.zBand->map_band_cb_typical.erase(*it_lwcb);
				map_cb_band->erase(*it_lwcb);

				it_cb_band->second.iBand->valid = 0;
			}
		}
	}
}

bool typical::typical_deal_band(list<TYPICAL_STEP> & lst_step)
{
	cout << "============================================================" << endl;

	bool bflag = false;	//标志本次调用是否有新的段被成功处理

	for(map<long, list<BAND_INFO> >::iterator it_dv_band = m_map_dv_band.begin(); it_dv_band != m_map_dv_band.end(); it_dv_band++)
	{
		for(list<BAND_INFO>::iterator it_band = it_dv_band->second.begin(); it_band != it_dv_band->second.end(); it_band++)
		{
			if(it_band->valid)
			{
				map<BAND_TREE_DATA, BAND_TREE_POS>::iterator pos = m_map_band_pos.find(&(*it_band));
				bool flag1 = true;//当前方式下的下游是否全部处理
				bool flag2 = true;//典型方式下的上游是否全部处理
				bool flag3 = false;//是否存在
				if(pos == m_map_band_pos.end())
				{
					TRACE("pos 没有find 到相关的数据   ！ \n");
					continue;
				}

				if (pos == m_map_band_pos.end())
				{
					TRACE("m_map_band_pos没有找到段,");
					cout << &(*it_band) << endl;
					continue;
				}

				cout << ">>>>>>>开始分析段" << pos->first << "……" << endl;
				
				if (pos->second.pNode == NULL && pos->second.pTypicalNode == NULL)
				{
					cout << "该段在典型方式和当前方式下均无电！！！！" << endl;
					continue;
				}

				//先判断段在当前方式下的下游段是否存在或全部被处理
				if(pos->second.pNode != NULL)
				{
					TRACE("%d, %ld\r\n", pos->second.pNode->flag, Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->so_cb);
					if (pos->second.pNode->flag & _NODE_UNPROC)
					{
						//先找到这个结点所在的树
						map<long, Tree<BAND_TREE_DATA> >::iterator it_tree = m_map_tree_band.find(Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->so_cb);

						TreeNode<BAND_TREE_DATA> *pNode = it_tree->second.GetChildNode(pos->second.pNode);

						if(pNode == NULL)
						{
							TRACE("该段在当前方式下无下游 \n");
						}

						while (pNode != NULL)
						{
							if (pNode->flag & _NODE_UNPROC)
							{
								cout << "2该段在当前方式下的的下游段" << pNode->data << "未处理！！！！" << endl;
								flag1 = false;
								break;
							}

							pNode = it_tree->second.GetNextBrotherNode(pNode);
						}
					}
					else
					{
						TRACE("2该段已经被处理过！\n");
						continue;
					}
				}
				else
				{
					TRACE("2该段在当前方式下无电！！！！\n");
					map<long, CB_INFO>::iterator it_typical;
					list<TYPICAL_STEP>::iterator it_temp;
					for(map<long, CB_INFO>::iterator it = pos->first->map_band_cb.begin();it != pos->first->map_band_cb.end();it++)
					{
						cout<<">>>>  "<<it->first<<endl;
						if(it->second.type ==LWCB)//如果是联络开关
						{
							it_typical = pos->first->map_band_cb_typical.find(it->second.id);
							if(it_typical != pos->first->map_band_cb_typical.end())
							{
								if(it_typical->second.status != it->second.status)
								{
									bool flag = false;
									for(it_temp = lst_step.begin();it_temp != lst_step.end();it_temp++)
									{	
										if(it->second.id == it_temp->cb)
											flag = true;
									}
									if(!flag)
									{
										if(it_typical->second.status)
											lst_step.push_back(TYPICAL_STEP(it->second.id,false,false));
										else
											lst_step.push_back(TYPICAL_STEP(it->second.id,true,false));
									}
								}
							}
						}
					}
				}

				//再判断段在典型方式下的上游是否存在或被处理
				if(pos->second.pTypicalNode != NULL)
				{
//					TRACE("%d, %s\r\n", pos->second.pTypicalNode->flag, Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->so_cb_typical.c_str());
					if (pos->second.pTypicalNode->flag & _NODE_UNPROC)
					{
						map<long, Tree<BAND_TREE_DATA> >::iterator it_tree = m_map_tree_band_typical.find(Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->so_cb);

						TreeNode<BAND_TREE_DATA> *pNode = it_tree->second.GetParent(pos->second.pTypicalNode);

						if(pNode == NULL)
						{
							TRACE("该段在典型方式下无上游。\n");
						}

						if (pNode != NULL && (pNode->flag & _NODE_UNPROC))
						{
							cout << "该段在典型方式下的的上游段" << pNode->data << "未处理！！！！" << endl;

							flag2 = false;
						}
					}
					else
					{
						TRACE("3该段已经被处理过！\n");
						continue;
					}
				}
				else
				{
					TRACE("3该段在典型方式下无电！！！！\n");
					flag3 = true;
				}

				//当前方式下无未处理的下游并且典型方式下无未处理上游，则该段可以处理了
				if(flag1 && flag2)
				{
					TRACE("该段可以处理。\n");
					CB_INFO cb;

					if (pos->second.pNode != NULL && Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->supply_cb != Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->supply_cb_typical)
					{
						list<TYPICAL_STEP>::iterator it;
						bool flag = false;
						for(it = lst_step.begin();it != lst_step.end();it++)
						{	
							if(Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->supply_cb == it->cb)
								flag = true;
						}
						if(!flag)
						{
							long id = Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->supply_cb;
							map<long, CB_INFO>::iterator it_cb = m_map_cb.find(id);
							cb.id = id;
							get_cb_info(cb);
							//cout<<id<<"<><><>"<<it_cb->second<<"<><>"<<cb.status<<endl;
							
							if(it_cb->second.status != cb.status)//开关状态是否发生变化
							{
								if(it_cb != m_map_cb.end())//判断是否是主线ID
									lst_step.push_back(TYPICAL_STEP(id, true,false));
								else
									lst_step.push_back(TYPICAL_STEP(id, true,true));
							}
						}
					}
					if (pos->second.pTypicalNode != NULL
							&& Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->supply_cb != Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->supply_cb_typical)
					{
						list<TYPICAL_STEP>::iterator it;
						bool flag = false;
						for(it = lst_step.begin();it != lst_step.end();it++)
						{	
							if(Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->supply_cb_typical == it->cb)
								flag = true;//恢复方案中是否存在
						}
						if(!flag)
						{
							long id = Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->supply_cb_typical;
							map<long, CB_INFO>::iterator it_cb = m_map_cb.find(id);
							cb.id = id;
							get_cb_info(cb);
							//cout<<id<<"<><><>"<<it_cb->second<<"<><>"<<cb.status<<endl;
							if(it_cb->second.status != cb.status)//开关状态是否发生变化
							{
								if(it_cb != m_map_cb.end())
									lst_step.push_back(TYPICAL_STEP(id, false,false));	
								else
									lst_step.push_back(TYPICAL_STEP(id, false,true));
							}
						}
					}

					//会出现当电源开关为发生变化时，不会给出恢复方案问题
					if(pos->second.pNode != NULL && Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->supply_cb == Tree<BAND_TREE_DATA>::GetData(pos->second.pNode)->supply_cb_typical &&
						pos->second.pTypicalNode != NULL&& Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->supply_cb == Tree<BAND_TREE_DATA>::GetData(pos->second.pTypicalNode)->supply_cb_typical)
					{
						TRACE("pTypicalNode-和-pNode-不为空----并且供电开关没有发生变化-------。\n");
						map<long, CB_INFO>::iterator it;
						list<TYPICAL_STEP>::iterator it_temp;
						for(map<long, CB_INFO>::iterator it_typical = pos->first->map_band_cb_typical.begin();it_typical != pos->first->map_band_cb_typical.end();it_typical++)
						{
							it = pos->first->map_band_cb.find(it_typical->second.id);
							if(it != pos->first->map_band_cb.end())
							{
								if(it->second.status != it_typical->second.status)
								{
									bool flag_size = false;
									for(it_temp = lst_step.begin();it_temp != lst_step.end();it_temp++)
									{	
										if(it_typical->second.id == it_temp->cb)
										{
											flag_size = true;
											break;
										}
									}
									if(!flag_size)
									{
										if(it_typical->second.status == 1)
											lst_step.push_back(TYPICAL_STEP(it_typical->second.id,false,false));
										else if(it_typical->second.status == 0)
											lst_step.push_back(TYPICAL_STEP(it_typical->second.id,true,false));
									}
								}
							}
						}
					}

					
					if (pos->second.pNode != NULL)
						pos->second.pNode->flag &= ~_NODE_UNPROC;
					if (pos->second.pTypicalNode != NULL)
						pos->second.pTypicalNode->flag &= ~_NODE_UNPROC;

						bflag = true;
				}

				if(flag3)//因为区段只能处理一次，而当联络开关在典型方式下无电，而在当前方式下有电，必须在当前区段断开后才能恢复避免短路问题
				{
					map<long, CB_INFO>::iterator it;
					list<TYPICAL_STEP>::iterator it_temp;
					bool flag = true;//是否已经有恢复方案
					bool flag_1 = false;//边界非联络开关是否闭合
					bool flag_2 = false;//是否存在联络开关
					long id = 0;
					int status = 2;
					for(map<long, CB_INFO>::iterator it_typical = pos->first->map_band_cb_typical.begin();it_typical != pos->first->map_band_cb_typical.end();it_typical++)
					{
						cout<<"<<<  "<<it_typical->first<<"<<<"<<it_typical->second.status<<endl;

						if(it_typical->second.type ==LWCB)//如果是联络开关
						{
							flag_2 = true;
							it = pos->first->map_band_cb.find(it_typical->second.id);
							if(it != pos->first->map_band_cb.end())
							{
								if(it->second.status != it_typical->second.status)
								{
									flag = false;
									for(it_temp = lst_step.begin();it_temp != lst_step.end();it_temp++)
									{	
										if(it_typical->second.id == it_temp->cb)
										{
											flag = true;
											break;
										}
									}
									if(!flag)
									{
										id = it_typical->second.id;
										status = it_typical->second.status;
									}
								}
							}
						}
						else
						{
							it = pos->first->map_band_cb.find(it_typical->second.id);
							if(it != pos->first->map_band_cb.end())
							{
								if(it->second.status == TYPICAL_CLOSE)
									flag_1 = true;
								else
									flag_1 = false;
								
								for(it_temp = lst_step.begin();it_temp != lst_step.end();it_temp++)
								{	
									if(it->second.id == it_temp->cb)
									{
										if(it_temp->bopen)
										{
											flag_1 = true;
											break;
										}
									}
								}
							}
						}
					}
					
					/*if(flag_1)
						cout<<"111"<<endl;
					if(flag_2)
						cout<<"222"<<endl;
					if(!flag)
						cout<<"333"<<endl;*/

					if(!flag && flag_1 && flag_2)
					{
						if(status == 1)
							lst_step.push_back(TYPICAL_STEP(id,false,false));
						else if(status == 0)
							lst_step.push_back(TYPICAL_STEP(id,true,false));

					}
				}
				
				list<TYPICAL_STEP> ::iterator list_it;
				for(list_it = lst_step.begin();list_it != lst_step.end();list_it++)
				{
					cout<<list_it->cb<<endl;
					if(list_it->bopen)
						cout<<"分闸"<<endl;
					else
						cout<<"合闸"<<endl;
					
					if(list_it->boptional)
						cout<<"可选"<<endl;
					else
						cout<<"不可选"<<endl;
				
				}

				cout << endl;
			}
			else	//该段被合并
			{
			}
		}
	}

	if(bflag)
	{
		typical_deal_band(lst_step);
	}

	return true;
}

bool typical::typical_verify(list<TYPICAL_STEP> & lst_step)
{
	return true;
}
