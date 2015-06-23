/*
 * FHZG.cpp
 *
 *  Created on: 2012-3-13
 *      Author: Administrator
 */

#include "FHZG.h"
//#include <intr_oci.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>

int CNT = 0;
map<string, float> map_so_capacity;
using namespace std;

bool IsNodeInList(TreeNode<DataType> *pNode, LIST_TREENODE lstNode);

ostream& operator <<(ostream &os, const DataType &data)
{
	return os;
	os << "*********FHZG node type:" << data.type << endl;

	if (data.type == _NODE_SO)
	{
		os << data.node_so.cb_id << endl;
		os << data.node_so.so_descr << endl;
		os << data.node_so.so_name << endl;
	}
	else
	{
		os << data.node_zn.cb_id << endl;
	}

	os << "**********************";

	return os;
}

float FHZG::GetMaxHisValue(char *command)
{
	char * buffer;
	int result;
	int rec_num, attr_num;
	float max_value = 0;//存放一天前电流的最大值
	struct ORA_ATTR * attrs = NULL;

	pthread_mutex_lock(&oci_mutex);
	result = g_oci->ReadWithBind(command,&buffer,&rec_num, &attr_num, &attrs);
	pthread_mutex_unlock(&oci_mutex);
				//buffer为首字段地址，rec_num记录行数，attr_num记录列数，attrs[0].col_width为偏移
	if(OCI_ERROR == result)
	{
		printf("读取数据失败\n");
		g_oci->Readdata_Free();
		return -1;
	}

	if(rec_num == 0)
	{
		printf("前一天的历史电流表为空\n");
		g_oci->Readdata_Free();
		return -1;
	}

	max_value = *(float*)buffer;
	g_oci->Readdata_Free();
	printf("HISDB::Read(DATA * pdata) END\n");
	return max_value;
}
float FHZG::GetFullcap(int izone)
{
	return 0;
	//D5000 读取逻辑有变化，等田燕青有空修改
//	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable(); //得到FDIR_ZONE表中的第一条数据
//	//该区段的电流流入开关
//	CLS_CB *so_cb = dsbob->cb_tb->GetDataByPos((fdir_zone + izone)->icb);
//	CLS_ND *so_cb_nd = dsbob->nd_tb->GetDataByPos(so_cb->ind);
//	CLS_ND *so_cb_znd = dsbob->nd_tb->GetDataByPos(so_cb->iznd);
//	float max_value;
//	float content;
//	char temp_id[MAX_ANALOG_ID_LEN] = {0};
//	int sec_pos;//标识线段的哪一端连接进线开关，0为首段，1为末端
//	CLS_SEC *so_sec;
//
//
//	sprintf(temp_id, "%sIa",so_cb->id);
//	//读取前一天的最大值
//	PSBOB::ANALOG psbob_analog = { 0 };
//	if (oodbread_rk(&psbob_analog, temp_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("analog")), sizeof(psbob_analog)) <= 0)
//	{
//		cout << "读取表dms的analog出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//		psbob_analog.limithigh1 = -1;
//		printf("analog 的id是%s\n",temp_id);
//	}
//	content = psbob_analog.limithigh1;
//	sprintf(temp_id, "%sIb",so_cb->id);
//	//读取前一天的最大值
//	if (oodbread_rk(&psbob_analog, temp_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("analog")), sizeof(psbob_analog)) <= 0)
//	{
//		cout << "读取表dms的analog出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//		psbob_analog.limithigh1 = -1;
//		printf("analog 的id是%s\n",temp_id);
//	}
//	if(content < psbob_analog.limithigh1)
//	{
//		content = psbob_analog.limithigh1;
//	}
//	sprintf(temp_id, "%sIc",so_cb->id);
//	//读取前一天的最大值
//	if (oodbread_rk(&psbob_analog, temp_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("analog")), sizeof(psbob_analog)) <= 0)
//	{
//		cout << "读取表dms的analog出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//		psbob_analog.limithigh1 = -1;
//		printf("analog 的id是%s\n",temp_id);
//	}
//	if(content < psbob_analog.limithigh1)
//	{
//		content = psbob_analog.limithigh1;
//	}
//
//	if(content < 0)
//	{
//		if(so_cb_nd->r_sec != -1)
//		{
//			so_sec = dsbob->sec_tb->GetDataByPos((dsbob->r_nd_sec_tb->GetDataByPos(so_cb_nd->r_sec))->sec_id_ptr);
//			if(so_sec->ind == so_cb->ind)
//			{
//				sec_pos = 0;
//			}
//			else
//			{
//				sec_pos = 1;
//			}
//		}
//		else
//		{
//			so_sec = dsbob->sec_tb->GetDataByPos((dsbob->r_nd_sec_tb->GetDataByPos(so_cb_znd->r_sec))->sec_id_ptr);
//			if(so_sec->ind == so_cb->iznd)
//			{
//				sec_pos = 0;
//			}
//			else
//			{
//				sec_pos = 1;
//			}
//		}
//		//从psbob中读取线段的结点号
//		PSBOB::SEC* sec_set = new PSBOB::SEC;
//		int size = sizeof(PSBOB::SEC);
//		if(oodbread_rk(sec_set, so_sec->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("sec")), size) <= 0)
//		{
//			delete sec_set;
//			cout << "读取表sec出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//		}
//		PSBOB::LD psbob_ld = {0};
//		if(sec_pos == 0)//然后再去ld表中读取ld的关键字，然后再读取analog表中的电流值
//		{
//			size = sizeof(PSBOB::LD);
//			if(oodbread_ra(&psbob_ld, "nd", sec_set->nd,const_cast<TB_DESCR*>(g_db_psbob->GetTB("ld")), size) <= 0)
//			{
//				cout << "读取表ld出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//			}
//		}
//		else
//		{
//			//PSBOB::LD psbob_ld = {0};
//			size = sizeof(PSBOB::LD);
//			if(oodbread_ra(&psbob_ld, "nd", sec_set->znd,const_cast<TB_DESCR*>(g_db_psbob->GetTB("ld")), size) <= 0)
//			{
//				cout << "读取表ld出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//			}
//		}
//		sprintf(temp_id, "0%s0050",psbob_ld.id);
//
//		PSBOB::ANALOG psbob_analog = { 0 };
//		if (oodbread_rk(&psbob_analog, temp_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("analog")), sizeof(psbob_analog)) <= 0)
//		{
//			cout << "读取表psbob的analog出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//		}
//		content = psbob_analog.limithigh1;
//	}
//	return content;
//
}
float FHZG::GetCbIVlaue(long cb_id)
{
	PSBOB::CB psbob_cb = { 0 };

	if (oodbread_rk(&psbob_cb, &cb_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) <= 0)
	{
		TRACE("读取breaker表关键字为%ld的记录失败，错误号%ld\r\n", cb_id, _oodb_errno);
		return -1;
	}

	pthread_mutex_lock(&avg_i_mutex);
	std::map<long,float>::iterator itVI = g_mapIValue.find(cb_id);
	if (itVI == g_mapIValue.end())
	{
		pthread_mutex_unlock(&avg_i_mutex);
		TRACE("查询开关%s id:%ld的历史电流值失败！\r\n", psbob_cb.name, cb_id);
		return -1;
	}
	else
	{
		float value = itVI->second;
		pthread_mutex_unlock(&avg_i_mutex);

		return value;
	}

	//杨勇 20141114 根据巴州项目实际情况，继续读取三相电流幅值最大值作为电流最大值
	//return max(max(psbob_cb.i_a_value, psbob_cb.i_b_value), psbob_cb.i_c_value);
	//杨勇 20141010 读取开关电流值，不再读取三相电流幅值的最大值
	//return psbob_cb.i;

	//D5000 读取逻辑有变化，等田燕青有空修改
//	float max_value;
//
//	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
//	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable(); //得到FDIR_ZONE表中的第一条数据
//
//	//判断检修开关的类型是否为进线开关，如果是进线开关要读取ld的电流值
//	CLS_CB *cb_temp = dsbob->cb_tb->Find(CLS_CB(cb_id));
//
//	if(cb_temp->q & CB_FDHCB)
//	{
//		CLS_ND *so_cb_nd = dsbob->nd_tb->GetDataByPos(cb_temp->ind);
//		CLS_ND *so_cb_znd = dsbob->nd_tb->GetDataByPos(cb_temp->iznd);
//		int sec_pos;//标识线段的哪一端连接进线开关，0为首段，1为末端
//		CLS_SEC *so_sec;
//		char temp_id[MAX_ANALOG_ID_LEN] = {0};
//
//		sprintf(temp_id, "%sIa",cb_id);
//		//读取前一天的最大值
//		DMS::ANALOG dms_analog = { 0 };
//		if (oodbread_rk(&dms_analog, temp_id, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), sizeof(dms_analog)) <= 0)
//		{
//			dms_analog.value = -1;
//			printf("读取dms的analog表 的id是%s失败err:%d，函数为%s，line是%d\n",temp_id,_oodb_errno,__FUNCTION__,__LINE__);
//		}
//		max_value = dms_analog.value;
//		sprintf(temp_id, "%sIb",cb_id);
//		//读取前一天的最大值
//		if (oodbread_rk(&dms_analog, temp_id, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), sizeof(dms_analog)) <= 0)
//		{
//			printf("读取dms的analog表 的id是%s失败err:%d，函数为%s，line是%d\n",temp_id, _oodb_errno, __FUNCTION__,__LINE__);
//			dms_analog.value = -1;
//		}
//		if(max_value < dms_analog.value)
//		{
//			max_value = dms_analog.value;
//		}
//		sprintf(temp_id, "%sIc",cb_id);
//		//读取前一天的最大值
//		if (oodbread_rk(&dms_analog, temp_id, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), sizeof(dms_analog)) <= 0)
//		{
//			printf("读取dms的analog表 的id是%s失败err:%d，函数为%s，line是%d\n",temp_id,_oodb_errno,__FUNCTION__,__LINE__);
//			dms_analog.value = -1;
//		}
//		if(max_value < dms_analog.value)
//		{
//			max_value = dms_analog.value;
//		}
//
//		if(max_value < 0)
//		{
//			if(so_cb_nd->r_sec != -1)
//			{
//				so_sec = dsbob->sec_tb->GetDataByPos((dsbob->r_nd_sec_tb->GetDataByPos(so_cb_nd->r_sec))->sec_id_ptr);
//				if(so_sec->ind == cb_temp->ind)
//				{
//					sec_pos = 0;
//				}
//				else
//				{
//					sec_pos = 1;
//				}
//			}
//			else
//			{
//				so_sec = dsbob->sec_tb->GetDataByPos((dsbob->r_nd_sec_tb->GetDataByPos(so_cb_znd->r_sec))->sec_id_ptr);
//				if(so_sec->ind == cb_temp->iznd)
//				{
//					sec_pos = 0;
//				}
//				else
//				{
//					sec_pos = 1;
//				}
//			}
//			printf("开关相连的线段id是%s\n",so_sec->id);
//			//从psbob中读取线段的结点号
//			PSBOB::SEC* sec_set = new PSBOB::SEC;
//			int size = sizeof(PSBOB::SEC);
//			if(oodbread_rk(sec_set, so_sec->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("sec")), size) <= 0)
//			{
//				delete sec_set;
//				printf("读取表 sec的id是%s失败err:%d，函数为%s，line是%d\n",so_sec->id,_oodb_errno,__FUNCTION__,__LINE__);
//			}
//			PSBOB::LD psbob_ld = {0};
//			if(sec_pos == 0)//然后再去ld表中读取ld的关键字，然后再读取analog表中的电流值
//			{
//				size = sizeof(PSBOB::LD);
//				if(oodbread_ra(&psbob_ld, "nd", sec_set->nd,const_cast<TB_DESCR*>(g_db_psbob->GetTB("ld")), size) <= 0)
//				{
//					printf("读取表ld失败err:%d,结点为%s，函数为%s，line是%d\n",_oodb_errno,sec_set->nd,__FUNCTION__,__LINE__);
//				}
//			}
//			else
//			{
//				size = sizeof(PSBOB::LD);
//				if(oodbread_ra(&psbob_ld, "nd", sec_set->znd,const_cast<TB_DESCR*>(g_db_psbob->GetTB("ld")), size) <= 0)
//				{
//					printf("读取表ld失败err:%d,结点为%s，函数为%s，line是%d\n",_oodb_errno,sec_set->znd,__FUNCTION__,__LINE__);
//				}
//			}
//			sprintf(temp_id, "0%s0050",psbob_ld.id);
//			//读取前一天的最大值
//			DMS::ANALOG dms_analog = { 0 };
//			if (oodbread_rk(&dms_analog, temp_id, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), sizeof(dms_analog)) <= 0)
//			{
//				dms_analog.value = -1;
//				printf("读取dms的analog表 的id是%s失败err:%d，函数为%s，line是%d\n",temp_id,_oodb_errno,__FUNCTION__,__LINE__);
//			}
//
//			max_value = dms_analog.value;
//		}
//
//		printf("the cb_id %s max_value is %f\n",cb_id,max_value);
//		return max_value;
//
//	}
//	else
//	{
//		char temp_id[MAX_ANALOG_ID_LEN] = {0};
//		PSBOB::CB psbob_cb = {0};
//		DMS::ANALOG dms_analog = {0};
//		char temp_cb_id[MAX_CB_ID_LEN] = {0};
//		sprintf(temp_cb_id, "%s", cb_id);
//
//		if(oodbread_rk(&psbob_cb, temp_cb_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("cb")), sizeof(psbob_cb)) <= 0)
//		{
//			printf("%s(%d):读取开关%s记录信息失败，错误号:%ld\r\n", __FUNCTION__, __LINE__, cb_id, _oodb_errno);
//			return -1;
//		}
//		else
//		{
//			strcpy(temp_id, psbob_cb.algi);
//			//读取前一天的最大值
//
//			if (oodbread_rk(&dms_analog, temp_id, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), sizeof(dms_analog)) <= 0)
//			{
//				printf("读取dms的analog表 的id是%s失败err:%d，函数为%s，line是%d\n",temp_id,_oodb_errno,__FUNCTION__,__LINE__);
//				dms_analog.value = -1;
//			}
//			max_value = dms_analog.value;
//			strcpy(temp_id, psbob_cb.algib);
//			//读取前一天的最大值
//			if (oodbread_rk(&dms_analog, temp_id, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), sizeof(dms_analog)) <= 0)
//			{
//				printf("读取dms的analog表 的id是%s失败err:%d，函数为%s，line是%d\n",temp_id,_oodb_errno,__FUNCTION__,__LINE__);
//				dms_analog.value = -1;
//			}
//			if(max_value < dms_analog.value)
//			{
//				max_value = dms_analog.value;
//			}
//			strcpy(temp_id, psbob_cb.algic);
//			//读取前一天的最大值
//			if (oodbread_rk(&dms_analog, temp_id, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), sizeof(dms_analog)) <= 0)
//			{
//				printf("读取dms的analog表 的id是%s失败err:%d，函数为%s，line是%d\n",temp_id,_oodb_errno,__FUNCTION__,__LINE__);
//				dms_analog.value = -1;
//			}
//			if(max_value < dms_analog.value)
//			{
//				max_value = dms_analog.value;
//			}
//			if(zg_type == FHZG_MIN_I && max_value < 0)
//			{
//				CLS_ND *cb_nd = dsbob->nd_tb->GetDataByPos(cb_temp->ind);
//				CLS_ND *cb_znd = dsbob->nd_tb->GetDataByPos(cb_temp->iznd);
//				CLS_SEC *so_sec = NULL;
//				if(cb_nd->r_sec == -1 && cb_znd->r_sec == -1)
//				{
//					cout << "读取开关连接线段上的电流值出错,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
//					printf("开关的id是%s\n",temp_id);
//					return -1;
//				}
//				else
//				{
//					if(cb_nd->r_sec != -1)
//					{
//						so_sec = dsbob->sec_tb->GetDataByPos((dsbob->r_nd_sec_tb->GetDataByPos(cb_nd->r_sec))->sec_id_ptr);
//					}
//					else
//					{
//						so_sec = dsbob->sec_tb->GetDataByPos((dsbob->r_nd_sec_tb->GetDataByPos(cb_znd->r_sec))->sec_id_ptr);
//					}
//					return /*abs(so_sec->i)*/so_sec->i < 0 ? so_sec->i *= -1 : so_sec->i ;
//				}
//
//			}
//			printf("the cb_id %s max_value is %f\n",cb_id,max_value);
//			return max_value;
//		}
//	}
}

//根据电流重新生成树,传入值为新树的根结点，新树指针，旧树要处理的结点，旧树
//递归调用这个函数，知道旧的树遍历完,father是新生成的树的父节点，old_father是旧树遍历到的结点
void FHZG::ReBuildTree(TreeNode<DataType> *father, Tree<DataType> *pTree, TreeNode<DataType> *old_father, Tree<DataType> *old_pTree)
{
	cout<<__FUNCTION__<<endl;

	float max_value = 0;
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable(); //得到FDIR_ZONE表中的第一条数据
	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	TreeNode<DataType> *tmp_father = father;
	TreeNode<DataType> *pChild = old_father;
	map<long, float>::iterator zone_value_vi;

	//遍历树结点
	//判断该区段的电流流入开关是否有电流值
	if(old_father->data.type == _NODE_SO)
	{
		if (old_pTree == m_pTree)
		{
			TreeNode<DataType> *tmp_nnd;
			list<TreeNode<DataType>*>::iterator ss_vi;

			pChild = pTree->GetChildNode(tmp_father);

			while (pChild)
			{
				if (pChild->data.type == _NODE_SO)
					break;
				pChild = pChild->next;
			}
			if (pChild)
			{
				//该区段已经存在剩余容量比较大的电源，则不进行新电源结点的插入
				if (pChild->data.node_so.left_capacity >= old_father->data.node_so.left_capacity)
				{
					tmp_nnd = pTree->TreeAddLeaf(father, old_father->data);
					tmp_nnd->flag |= _NODE_DELETE_;
					m_map_cb_tree.erase(old_father->data.node_so.cb_id);
					lst_so_removed.push_back(tmp_nnd);
				}
				//否则移除容量小的电源结点，插入新的电源结点
				else
				{
					pChild->flag |= _NODE_DELETE_;
					so_list.remove(pChild);
					m_map_cb_tree.erase(pChild->data.node_so.cb_id);
					tmp_nnd = pTree->TreeAddLeaf(father, old_father->data);
					so_list.push_back(tmp_nnd);
					lst_so_removed.push_back(pChild);
				}
			}
			else
			{
				tmp_nnd = pTree->TreeAddLeaf(father, old_father->data);
				so_list.push_back(tmp_nnd);
//				for (ss_vi = so_list.begin(); ss_vi != so_list.end(); ss_vi++)
//				{
//					if (strcmp(old_father->data.node_so.cb_id, (*ss_vi)->data.node_so.cb_id) == 0)
//					{
//						*ss_vi = tmp_nnd;
//					}
//				}
			}

//			printf("the so_nd flag is %d\n",(pChild->flag & _NODE_DELETE_));
		}
		else
		{
			pTree->TreeAddLeaf(father, pChild->data);
		}
		return;
	}

	max_value = GetCbIVlaue(pChild->data.node_zn.cb_id);

	printf("开关%ld上的电流值为%f\n",pChild->data.node_zn.cb_id,max_value);
	if(max_value < 0)
	{
		//此区段的电流流入开关不是可遥测的,不需要创建新的结点，把当前结点的信息存入父节点中
		father->data.node_zn.map_zn_ld[pChild->data.node_zn.izone] = pChild->data.node_zn.ld_list;
		pChild = old_pTree->GetChildNode(pChild);
		while(pChild)
		{
			ReBuildTree(father, pTree, pChild, old_pTree);
			pChild = pChild->next;
		}
	}
	else
	{
		zone_value_vi = zone_value_map.find((fdir_zone + father->data.node_zn.izone)->zn_id);
		if(zone_value_vi != zone_value_map.end())
		{
			father->data.node_zn.ldamount = (*zone_value_vi).second;
		}
		else
		{
			father->data.node_zn.ldamount = father->data.node_zn.ldamount - max_value;
		}
		pChild->data.node_zn.ldamount = max_value;
		tmp_father = pTree->TreeAddLeaf(father, pChild->data);
		tmp_father->data.node_zn.map_zn_ld[pChild->data.node_zn.izone] = pChild->data.node_zn.ld_list;

		pChild = old_pTree->GetChildNode(pChild);
		while(pChild)
		{
			ReBuildTree(tmp_father, pTree, pChild, old_pTree);
			pChild = pChild->next;
		}
	}
}

void FHZG::GetZoneLd(LIST_TREENODE tree_list, bool is_so_tree)
{
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();

	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable(); //得到FDIR_ZONE表中的第一条数据

	list<TreeNode<DataType>*>::iterator list_it;
	ChildPtr *ptr1 = NULL, *ptr2 = NULL;
	CLS_ZN *zn = NULL;

	for(list_it = tree_list.begin(); list_it != tree_list.end(); list_it++)
	{
		zn = dsbob->zn_tb->Find(CLS_ZN((fdir_zone + (*list_it)->data.node_zn.izone)->zn_id));
		//根据ZN表中的r_ld找到r_ZN_LD表中的的相关记录（可能有多条记录）
		ptr1 = dsbob->r_zn_ld_tb->FindChildPtr(zn->r_ld);

		ptr2 = ptr1;

		while (ptr1)
		{
			//根据r_ZN_LD表中的Ld_id，找到LD表中相关记录。
			PSBOB::LD psbob_ld = { 0 };

			CLS_LD *ld = dsbob->ld_tb->GetDataByPos(ptr1->Ptr);

			if (oodbread_rk(&psbob_ld, &ld->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), sizeof(psbob_ld)) <= 0)
			{
				printf("%s(%d) 读取负荷%ld有功负荷量测%ld对应的值失败，错误号:%d\r\n", __FILE__, __LINE__, ld->id, ld->algp, _oodb_errno);
			}

			ld->w = psbob_ld.pnom/*D5000*//*psbob_analog.value*/;
			if (zg_type == FHZG_LD_DV) //以负荷作为参考值做负荷转供，tianyq 2012,6,14
			{
				(*list_it)->data.node_zn.ldamount += psbob_ld.pnom/*D5000*//*psbob_analog.value*/; //找到区段内每个元件的负荷，求和得到区段的最大负荷
			}
			(*list_it)->data.node_zn.ld_list.push_back(*ld); //将区段内的所有负荷存入链表

			//读取负荷的状态标志位，挂保电、检修牌的负荷所在区段的开关不允许操作
			//（将其优先级设置为最高，这样减负荷的时候就不会被减到）
//			PSBOB::LD psbob_ld = { 0 };
//			if (oodbread_rk(&psbob_ld, ld->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("ld")), sizeof(psbob_ld)) <= 0)
//			{
//				printf("%s(%d):读取负荷%s记录信息失败，错误号:%d\r\n", __FUNCTION__, __LINE__, psbob_ld.id, _oodb_errno);
//			}

			if ((psbob_ld.status & DEV_STATUS_UNDEFINE/*负荷保电*/)/* || (psbob_ld.status & CB_REPAIR)*/)
			{
				printf("*************负荷%ld需要保电！！！！！！！\r\n", ld->id);

				//该节点没有保电标志，则将该节点设置成负荷保电
				if ((*list_it)->data.node_zn.protect == NONE)
				{
					(*list_it)->data.node_zn.protect = LDPROTECT;
				}
				//该节点已经有开关保电或负荷保电则不修改
				//尤其是当该节点已经有开关保电标志时切莫修改成负荷保电
				//因为开关保电为影响其下游的整理个区域
				else
				{
				}

				//将需要保电的设备存入列表lst_protected
				if (!is_so_tree)
				{
					lst_protected.push_back(DEV_STATUS(psbob_ld.id, "ld", psbob_ld.status));
				}
			}

			ptr1 = ptr1->Next;
		}

		FreeChildPtr(ptr2);
		ptr1 = NULL;
		ptr2 = NULL;
		zn = NULL;
	}

}

//遍历区段，形成树
void FHZG::ErgodicTree(int izone, TreeNode<DataType> *father, Tree<DataType> *pTree,  bool b_so_tree, LIST_TREENODE &main_tree, map<long, int> &m_so_zone)
{
//	cout << __FUNCTION__ << endl;

	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();

	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable(); //得到FDIR_ZONE表中的第一条数据

	int lv_fdir_zone_cb = fdir_zone_cb_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	TreeNode<DataType> *tmp_father = father;

	//判定区段有没有被处理过
	if ((fdir_zone + izone)->q & FDIR_ZONE_UNPROC)
	{
		(fdir_zone + izone)->q &= ~FDIR_ZONE_UNPROC;

		if ((fdir_zone + izone)->type == _FDIR_ZN)
		{
			/////////////////////////////////////////////////
			DataType node_zn /*= { 0 }*/;
			//根据FDIR_ZONE表中的zn_id找到ZN表中相应记录中的r_ld
			CLS_ZN *zn = dsbob->zn_tb->Find(CLS_ZN((fdir_zone + izone)->zn_id));
			//该区段的电流流入开关
			CLS_FDIR_CB *tmp_cb = fdir_cb_tb->GetDataByPos((fdir_zone + izone)->icb);

			long cb_id = tmp_cb->cb_id;

			id_to_pid(cb_id);

//			node_zn.flag |= _NODE_UNPROC;
//			node_zn.flag &= ~_NODE_DELETE_;
			node_zn.dist = 0;
			node_zn.cnt_node = 0;
			node_zn.type = _NODE_ZN;
			node_zn.node_zn.izone = izone;
			node_zn.node_zn.zn_id= zn->id;
			node_zn.node_zn.cb_id= cb_id;

			//读取开关的状态标志位，挂保电、检修牌的开关不允许操作
			//（将其优先级设置为最高，这样减负荷的时候就不会被减到）
			PSBOB::CB psbob_cb = { 0 };
			if (oodbread_rk(&psbob_cb, &cb_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) <= 0)
			{
				TRACE("读取开关%ld记录信息失败，错误号:%d\r\n", cb_id, _oodb_errno);
			}

			if ((psbob_cb.status & DEV_STATUS_UNDEFINE/*开关保电*/)/* || (psbob_cb.status & CB_REPAIR)*/)
			{
				TRACE("*************开关%ld需要保电！！！！！！！\r\n", cb_id);
				node_zn.node_zn.protect = CBPROTECT;

				//将需要保电的设备存入列表lst_protected
				if (!b_so_tree)
				{
					lst_protected.push_back(DEV_STATUS(psbob_cb.id, "cb", psbob_cb.status));
				}
			}

//			ChildPtr *ptr1 = NULL, *ptr2 = NULL;
//			//根据ZN表中的r_ld找到r_ZN_LD表中的的相关记录（可能有多条记录）
//			ptr1 = dsbob->r_zn_ld_tb->FindChildPtr(zn->r_ld);
//
//			ptr2 = ptr1;
//
//			while (ptr1)
//			{
//				//根据r_ZN_LD表中的Ld_id，找到LD表中相关记录。
//				PSBOB::ANALOG psbob_analog = { 0 };
//
//				CLS_LD *ld = dsbob->ld_tb->GetDataByPos(ptr1->Ptr);
//
//				if (oodbread_rk(&psbob_analog, ld->algp, const_cast<TB_DESCR*>(g_db_psbob->GetTB("analog")), sizeof(psbob_analog)) <= 0)
//				{
//					printf("%s(%d) 读取负荷%s有功负荷量测%s对应的值失败，错误号:%d\r\n", __FILE__, __LINE__, ld->id, ld->algp, _oodb_errno);
//				}
//
//				ld->w = psbob_analog.value;
//				if (zg_type == FHZG_LD_DV) //以负荷作为参考值做负荷转供，tianyq 2012,6,14
//				{
//					node_zn.node_zn.ldamount += psbob_analog.value; //找到区段内每个元件的负荷，求和得到区段的最大负荷
//				}
//				node_zn.node_zn.ld_list.push_back(*ld); //将区段内的所有负荷存入链表
//				//printf("负荷%s被发现！\r\n", ld->id);
//
//				//读取负荷的状态标志位，挂保电、检修牌的负荷所在区段的开关不允许操作
//				//（将其优先级设置为最高，这样减负荷的时候就不会被减到）
//				PSBOB::LD psbob_ld = { 0 };
//				if (oodbread_rk(&psbob_ld, ld->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("ld")), sizeof(psbob_ld)) <= 0)
//				{
//					printf("%s(%d):读取负荷%s记录信息失败，错误号:%d\r\n", __FUNCTION__, __LINE__, psbob_ld.id, _oodb_errno);
//				}
//
//				if ((psbob_ld.status & LD_PRESERVE)/* || (psbob_ld.status & CB_REPAIR)*/)
//				{
//					printf("*************负荷%s需要保电！！！！！！！\r\n", ld->id);
//
//					//该节点没有保电标志，则将该节点设置成负荷保电
//					if (node_zn.node_zn.protect == NONE)
//					{
//						node_zn.node_zn.protect = LDPROTECT;
//					}
//					//该节点已经有开关保电或负荷保电则不修改
//					//尤其是当该节点已经有开关保电标志时切莫修改成负荷保电
//					//因为开关保电为影响其下游的整理个区域
//					else
//					{
//					}
//
//					//将需要保电的设备存入列表lst_protected
//					if (!b_so_tree)
//					{
//						lst_protected.push_back(DEV_STATUS(psbob_ld.id, "ld", psbob_ld.status));
//					}
//				}
//
//				ptr1 = ptr1->Next;
//			}
//
//			FreeChildPtr(ptr2);

			//由于开关挂保电牌意思着本开关下游的所整个区域都需要保电
			//所以如果父节点为开关保电类型，则将本结点也设置成开关保电
			if (father != NULL && father->data.node_zn.protect == CBPROTECT)
			{
				TRACE("节点%d父节点为开关保电，帮将其也设置成开关保电！\r\n", node_zn.node_zn.izone);
				node_zn.node_zn.protect = CBPROTECT;
			}

			if (zg_type == FHZG_LD_DV)
			{
				node_zn.node_zn.map_zn_ld[node_zn.node_zn.izone] = node_zn.node_zn.ld_list;
			}

			/*down_zn_list.push_back(*/
			tmp_father = pTree->TreeAddLeaf(father, node_zn)/*)*/;

			main_tree.push_back(tmp_father);

			//////////////////////////////////////////////////////////////
			if (zg_type == FHZG_MIN_I)
			{
				tmp_father->data.node_zn.ldamount = GetCbIVlaue(tmp_father->data.node_zn.cb_id);
			}
		}
		for (int i = 0; i < (fdir_zone + izone)->cbnum; i++)
		{
			//分别得到区段内所有开关的物理ID，cbptr为本区段开关物理号的起始编号
			int icb = (fdir_zone_cb + (fdir_zone + izone)->cbptr + i)->icb; //此处的icb为开关的物理号，在FDIR_ZONE_CB表中

			if ((fdir_cb + icb)->q & FDIR_CB_UNPROC) //判断开关是否被处理过，如果处理过则不再进行处理，防止递归而产生死循环
			{
				CLS_CB *cb = dsbob->cb_tb->Find(CLS_CB((fdir_cb + icb)->cb_id));
//				//如果是联络开关，则添加电源节点
//				if (cb->q & CB_LWCB)	//联络开关不设置已经处理标记，以防在生成该联络开关电源节点子树时此开关会因为有已处理标记而被忽略
//				{}
//				else
//				{
//
//				}

				if (!((fdir_cb + icb)->topflags & FDIR_CB_OPEN)) //开关是合状态
				{
					if (zg_type == FHZG_MIN_I)
					{
						tmp_father->data.node_zn.ldamount = tmp_father->data.node_zn.ldamount - GetCbIVlaue(cb->pid);
					}
					(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;

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
						ErgodicTree(zone1, tmp_father, pTree, b_so_tree, main_tree, m_so_zone);
					}
				}
				else //find so
				{
					//拒动的开关不生成电源
					if (IsCBFailed(dsbob->cb_tb->GetDataByPos(icb)->pid))
					{
					}
					//不是拒动开关
					else
					{
						DataType node_so/* = { 0 }*/;

						node_so.type = _NODE_SO;
//					node_so.flag |= _NODE_UNPROC;
						node_so.dist = 0;

						node_so.node_so.cb_id= cb->id;

						if (b_so_tree)
						{
							pTree->TreeAddLeaf(tmp_father, node_so);
						}
						else
						{
							int jxcb_zone = GetLwcbSo(icb, node_so);
							//获取断开开关另一端对应的电源失败
							if (-1 == jxcb_zone)
							{
								m_map_cb_tree.erase(cb->id);
								TRACE("获取联络开关%ld[%s]对应的电源失败！\r\n", cb->id, cb->descr);
								continue;
							}

							if (zg_type == FHZG_CTRL_I || zg_type == FHZG_MIN_I) //电流作为负荷转供的参考对象，获取电源的最大容量和剩余容量，田艳青加于2012年6月16日
							{
								//该区段的电流流入开关
								CLS_CB *so_cb = dsbob->cb_tb->GetDataByPos((fdir_zone + jxcb_zone)->icb);
								float max_value;
								float content;
								PSBOB::DV dv = { 0 };

								GetDVByZone(jxcb_zone, dv);
								node_so.node_so.dv_id= dv.id;
								if (dv.id == 0)
								{
									TRACE("\033[31m********************zone is %d getdv failed**************\033[0m\r\n",jxcb_zone);
									continue;
								}
								else
								{
									printf("馈线%ld（%s）总容量lmtnorm为%d\r\n", dv.id, dv.name, dv.lmtnorm);
								}

#if 1	//D5000获取馈线容量
								content = dv.lmtnorm;
#else
								//读取电源的总容量
								content = GetFullcap(jxcb_zone);
#endif
								TRACE("开关 %ld另一端的电源总容量是 %f\n", cb->id, content);
								//读取前一天的最大值
								max_value = GetCbIVlaue(so_cb->pid);
								TRACE("读取开关%ld 上电流使用值为 %f\n", so_cb->id, max_value);
								//计算电源的容量和剩余容量tianyq
								node_so.node_so.left_capacity = (FHZG_CAPACTIY_K * content) - max_value;
								if (node_so.node_so.left_capacity < 0)
								{
									node_so.node_so.left_capacity = 0;
								}
								node_so.node_so.full_capacity = FHZG_CAPACTIY_K * content;
								pTree->TreeAddLeaf(tmp_father, node_so);
								m_so_zone[cb->id] = jxcb_zone;
//								TreeNode<DataType> *pChild = pTree->GetChildNode(tmp_father);
//								while (pChild)
//								{
//									if (pChild->data.type == _NODE_SO)
//										break;
//									pChild = pChild->next;
//								}
//								if (pChild)
//								{
//									//该区段已经存在剩余容量比较大的电源，则不进行新电源结点的插入
//									if (pChild->data.node_so.left_capacity >= node_so.node_so.left_capacity)
//									{
//										tmp_father = pTree->TreeAddLeaf(tmp_father, node_so);
//										tmp_father->flag |= _NODE_DELETE_;
//										//so_list.push_back(tmp_father);
//									}
//									//否则移除容量小的电源结点，插入新的电源结点
//									else
//									{
//										pChild->flag |= _NODE_DELETE_;
//										so_list.remove(pChild);
//										m_so_zone.erase(pChild->data.node_so.cb_id);
//										m_so_zone[cb->id] = jxcb_zone;
////									pTree->TreeRemoveSubTree(pChild);
//										so_list.push_back(pTree->TreeAddLeaf(tmp_father, node_so));
//										printf("添加电源结点%s,%d\n", node_so.node_so.so_name, __LINE__);
//									}
//								}
//								else
//								{
//									so_list.push_back(pTree->TreeAddLeaf(tmp_father, node_so));
//									m_so_zone[cb->id] = jxcb_zone;
//									printf("添加电源结点%s,%d\n", node_so.node_so.so_name, __LINE__);
//								}
							}
							else
							{
								node_so.node_so.left_capacity = node_so.node_so.full_capacity - LdSum(&m_map_cb_tree[cb->id]);

								//计算本电源的网损
								CLS_SO *so = dsbob->so_tb->Find(CLS_SO(node_so.node_so.so_name));

								if (so != NULL)
								{
									dsbob->co_tb->Data[0].wloss = 0;
//									pwrflow->GetAnalogValue();				//D5000潮流计算还未移植
//									pwrflow->ExecutePwrflow(so->iisland);
									node_so.node_so.org_wloss = dsbob->co_tb->Data[0].wloss;

									TRACE("电源%ld所在岛的网损为%f\r\n", node_so.node_so.so_name, node_so.node_so.org_wloss);
								}
								///////////////////
								TRACE("***********以母线%ld对应的电源总容量为%f,剩余容量为%f\r\n", node_so.node_so.so_name, node_so.node_so.full_capacity, node_so.node_so.left_capacity);
								//将电源节点的指针存入链表
								//tmp_father = pTree->TreeAddLeaf(tmp_father, node_so);

								TreeNode<DataType> *pChild = pTree->GetChildNode(tmp_father);

								while (pChild)
								{
									if (pChild->data.type == _NODE_SO)
										break;
									pChild = pChild->next;
								}

								if (pChild)
								{
									//该区段已经存在剩余容量比较大的电源，则不进行新电源结点的插入
									if (pChild->data.node_so.left_capacity >= node_so.node_so.left_capacity)
									{
										m_map_cb_tree.erase(cb->id);
									}
									//否则移除容量小的电源结点，插入新的电源结点
									else
									{
										so_list.remove(pChild);
										m_map_cb_tree.erase(pChild->data.node_so.cb_id);
										pTree->TreeRemoveSubTree(pChild);
										so_list.push_back(pTree->TreeAddLeaf(tmp_father, node_so));
									}
								}
								else
									so_list.push_back(pTree->TreeAddLeaf(tmp_father, node_so));
							}

						}
					}
				}
			}
		}
	}
}
//建立fdir库
int FHZG::BuildDB()
{
	cout<<"*****************************************"<<endl;
	cout<<__FUNCTION__<<endl;
#if 0
	FILE *fp = fopen("/data/users/osa/bin/so_capacity", "r");

	if (fp)
	{
		char buf[1024] = { 0 };
		while (fgets(buf, 1024, fp))
		{
			char bus_id[MAX_BUS_ID_LEN] = { 0 };
			float capacity = 0;
			//sscanf(buf, "%s,%f", cb_id, &capacity);

			char *p = strchr(buf, ',');

			if (p)
			{
				strncpy(bus_id, buf , p - buf);
				capacity = atof(p + 1);
			}

			printf("母线%s的电源容量为%f\r\n", bus_id, capacity);

			map_so_capacity[bus_id] = capacity;
		}

		fclose(fp);
	}
	else
	{
		cout << strerror(errno) << endl;

		map_so_capacity["cszCB201"] = 300;
		map_so_capacity["cszCB202"] = 430;
		map_so_capacity["cszCB204"] = 110;
		map_so_capacity["cszCB203"] = 100;
	}
#endif
	if (!update_cb_state(true) || !add_so())
	{
		cout << "更新开关遥信状态或自动添加电源失败！" << endl;
		return FDIR_FAIL_DIRECT;
	}

//	TP tp(dsbob);
	if (FormFdirDB() == FDIR_R_FAIL)
	{
		cout << "FormFdirDB failed!" << endl;

		return FDIR_FAIL_DIRECT;
	}

	if (FormIcbOfZone() == FDIR_R_FAIL)
	{
		cout << "形成区段电流流入开关失败！" << endl;
		return FDIR_FAIL_DIRECT;
	}

	if (!WriteRtdb())
	{
		cout << __FUNCTION__ << "fdir_r->WriteRtdb失败！" << endl;

		return FDIR_FAIL_DIRECT;
	}

	return FDIR_R_SUCCEED;
}

////////////////////////////////////
//根据联络开关找到母线，根据母线建树//
///////////////////////////////////
//建树的目的是为了计算剩余负荷（这个需要遍历树，然后找到所有的负荷数，求和得之）。再者在负荷转供时，需要减掉树的时候，
//需要知道父子关系,防止减掉时影响其他的地方。
//根据联络开关，找到给其供电的电源，也就是找到第一个区段，然后再以此区段为首区段，形成一棵树
//返回值为电源进线开关为电流入流开关的区段号

int FHZG::GetLwcbSo(int icb,DataType &node_so)
{
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int pos = icb;
	int curbus; //给联络开关供电的母线或电源
	int tmp_zone = -1, pre_zone = -1;

	while (true)
	{
		if (pos < 0)
		{
			printf("区段%d无电流流入开关！\r\n", pre_zone);
			DisplayZoneCB(pre_zone);
			cout << __FUNCTION__ << "失败！" << endl;
			return -1;
		}

		if(dsbob->cb_tb->GetDataByPos(pos)->q & CB_FAULT_ISOLATE)
		{
			printf("开关%ld已经故障隔离！！！！！！！！！！！！\r\n", dsbob->cb_tb->GetDataByPos(pos)->id);
			return -1;
		}

		//找到上游区段，返回区段id
		tmp_zone = GetUpZone(pos);

		if(tmp_zone == -1) return -1;

		//取区段内的任一条线段，获取该线段所属的DV信息，最后把该DV对应馈线容量作为node_so的总容量
		if ((fdir_zone + tmp_zone)->type == _FDIR_ZN && node_so.node_so.full_capacity <= 0)
		{
			CLS_ZN *zn = dsbob->zn_tb->Find((fdir_zone + tmp_zone)->zn_id);

			if (zn != NULL && zn->r_sec != -1) //区段内有线段
			{
				CLS_R_ZN_SEC *r_zn_sec = dsbob->r_zn_sec_tb->GetDataByPos(zn->r_sec);
				PSBOB::SEC psbob_sec = { 0 };

				long tmp_sec_id = r_zn_sec->sec_id;

				id_to_pid(tmp_sec_id);

				if (oodbread_rk(&psbob_sec, &tmp_sec_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), sizeof(psbob_sec)) <= 0)
				{
					printf("%s(%d):根据SEC关键字%ld读取PSBOB库SEC表记录失败，错误号：%d\r\n", __FILE__, __LINE__, r_zn_sec->sec_id, _oodb_errno);
				}
				else
				{
					//D5000逻辑有变化，等田燕青修改
//					if (psbob_sec.oo_dv >= 0)
//					{
//						PSBOB::OO_R_DV_SEC psbob_oo_r_dv_sec = { 0 };
//
//						if (oodbread_rph(&psbob_oo_r_dv_sec, psbob_sec.oo_dv, const_cast<TB_DESCR*>(g_db_psbob->GetTB("oo_r_dv_sec")), sizeof(psbob_oo_r_dv_sec)) <= 0)
//						{
//							printf("%s(%d):读取PSBOB库表oo_r_dv_sec物理号为%d的记录失败，错误号：%d\r\n", __FILE__, __LINE__, psbob_sec.oo_dv, _oodb_errno);
//						}
//						else
//						{
//							PSBOB::DV psbob_dv = { 0 };
//
//							if (oodbread_rk(&psbob_dv, psbob_oo_r_dv_sec.dv_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("dv")), sizeof(psbob_dv)) <= 0)
//							{
//								printf("%s(%d):根据DV关键字%s读取PSBOB库DV表记录失败，错误号：%d\r\n", __FILE__, __LINE__, psbob_oo_r_dv_sec.dv_id, _oodb_errno);
//							}
//							else
//							{
//								node_so.node_so.full_capacity = psbob_dv.lmtnorm;
//							}
//						}
//					}
				}
			}
		}

		cout << __FUNCTION__ << ":" << (fdir_zone + tmp_zone)->id << endl;


		if ((fdir_zone + tmp_zone)->type == _FDIR_SO|| (fdir_zone + tmp_zone)->type == _FDIR_BUS)
		{

			(fdir_cb + pos)->q &= ~FDIR_CB_UNPROC;

			long cbid = (fdir_cb + pos)->cb_id;
			id_to_pid(cbid);
			long busid = (fdir_zone + tmp_zone)->zn_id;
			id_to_pid(busid);
			long soid = 0;

			KEY_STRU ks_cb = { 0 };
			CCommon::long_to_keyid(cbid, &ks_cb);
			KEY_STRU ks_bus = { 0 };
			CCommon::long_to_keyid(busid, &ks_bus);

			soid = ks_cb.key;
			soid |= ((long(ks_bus.key)) << 32);

//			node_so.node_so.full_capacity = map_so_capacity[(fdir_zone + tmp_zone)->zn_id];
			timeval tv = { 0 };
			gettimeofday(&tv, NULL);
			node_so.node_so.so_name = soid;//tv.tv_usec; //(fdir_zone + tmp_zone)->zn_id;
			strcpy(node_so.node_so.so_descr, (fdir_zone + tmp_zone)->descr);

			printf("!!!!!!!!!!!!!!!发现电源节点:%ld(%s)\r\n", node_so.node_so.so_name, node_so.node_so.so_descr);
			curbus = tmp_zone;
			break;
		}

		node_so.node_so.lst_zones.push_back(tmp_zone);

		pre_zone = tmp_zone;

		pos = (fdir_zone + pre_zone)->icb;
	}

	DisplayZoneCB(pre_zone);

	//ErgodicTree(pre_zone, pTree->root, pTree, true, so_tree, main_tree);

	return pre_zone;
}

void FHZG::GetZoneCB(int izone, list<long> &lst_cb)
{
	lst_cb.clear();

	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_ZONE_CB *fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	int cbnum = (fdir_zone + izone)->cbnum;
	int cbptr = (fdir_zone + izone)->cbptr;

	printf("===============zone:%ld,izone:%d,cbnum:%d================\r\n", (fdir_zone + izone)->zn_id, izone, cbnum);

	for (int i = 0; i < cbnum; i++)
	{
		PSBOB::CB psbob_cb = {0};

//		if (oodbread_rk(&psbob_cb, &(fdir_cb + (fdir_zone_cb + cbptr + i)->icb)->cb_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("cb")), sizeof(PSBOB::CB)) > 0)
//		{
//			lst_cb.push_back(psbob_cb.pointid);
//		}
//		else
//		{
//			printf("读取开关表%s数据失败，错误号：%d，line：%d\r\n", (fdir_cb + (fdir_zone_cb + cbptr + i)->icb)->cb_id, _oodb_errno, __LINE__);
//		}
//		lst_cb.push_back((fdir_cb + (fdir_zone_cb + cbptr + i)->icb)->cb_id);//D5000
		lst_cb.push_back(dsbob->cb_tb->GetDataByPos((fdir_zone_cb + cbptr + i)->icb)->pid);//D5000
	}
}

void FHZG::DisplayZoneCB(int izone)
{
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_ZONE_CB *fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	int cbnum = (fdir_zone + izone)->cbnum;
	int cbptr = (fdir_zone + izone)->cbptr;

	printf("===============zone:%ld================\r\n", (fdir_zone + izone)->zn_id);

	for (int i = 0; i < cbnum; i++)
	{
		cout << (fdir_cb + (fdir_zone_cb + cbptr + i)->icb)->descr << endl;
	}
}

int FHZG::GetDownZoneId(const long id)
{
//	CLS_DIGITAL *digital = dsbob->digital_tb->Find(CLS_DIGITAL(id));
	int icb = dsbob->cb_tb->FindPos(CLS_CB(id));

	TRACE("获取ICB=%d开关的下游区段！\r\n", icb);

	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	int i;
	//将所有的区段设置为未处理状态
	for (i = 0; i < lv_fdir_cb; i++)
	{
		(fdir_cb + i)->q |= FDIR_CB_UNPROC;
	}
	for (i = 0; i < lv_fdir_zone; i++)
	{
		(fdir_zone + i)->q |= FDIR_ZONE_UNPROC;
	}

	(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
	//得到下游区段id
	int dzoneid = FindDownZone(icb);
	return dzoneid;
}

int FHZG::GetUpZone(int icb)
{
	int i,j,k;
	int cbptr,cbnum;
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	for(i = 0;i < lv_fdir_zone;i++)//循环所有的区段
	{
		cbptr = (fdir_zone + i)->cbptr;//得到区段起始开关的编号
		cbnum = (fdir_zone + i)->cbnum;
		for(j = 0;j < cbnum;j++)
		{
			k = (fdir_zone_cb + cbptr + j)->icb;
			if( k == icb && k != (fdir_zone + i)->icb)
			{
				if((fdir_zone + i)->q & FDIR_ZONE_UNPROC)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

void FHZG::displayTree(LIST_TREENODE listcoll)
{
	cout<<__FUNCTION__<<endl;
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	int cbptr,cbnum;
	int i,icb;
	long zn_id,cb_id;
	while(!listcoll.empty())
	{
		if (listcoll.front()->data.type == _NODE_ZN)
		{
			zn_id = listcoll.front()->data.node_zn.zn_id;
			int zn = dsbob->zn_tb->FindPos(CLS_ZN(zn_id));

			cbnum = (fdir_zone + zn)->cbnum;
			cbptr = (fdir_zone + zn)->cbptr;
			for (i = 0; i < cbnum; i++)
			{
				cout << "-------------------" << endl;
				cout<<"ldamount = "<<listcoll.front()->data.node_zn.ldamount<<endl;
				icb = (fdir_zone_cb + (cbptr + i))->icb;
				cout<<dsbob->cb_tb->GetDataByPos(icb)->id<<endl;
			}
		}
		else
		{
			cout<<"^^^^^^^^^^so^^^^^^^^^^"<<endl;
			LIST_TREENODE so_tree;
			Tree<DataType> *pTree = &m_map_cb_tree[listcoll.front()->data.node_so.cb_id];
			pTree->BreadthFirstErgodic(pTree->root,so_tree);
			for(so_pos = so_tree.begin();so_pos != so_tree.end();so_pos++)
			{
				cout<<(*so_pos)->data.node_zn.zn_id<<endl;

				zn_id = (*so_pos)->data.node_zn.zn_id;
				int zn = dsbob->zn_tb->FindPos(CLS_ZN(zn_id));

				cbnum = (fdir_zone + zn)->cbnum;
				cbptr = (fdir_zone + zn)->cbptr;

				for (i = 0; i < cbnum; i++)
				{
					cout << "-------------------" << endl;
					icb = (fdir_zone_cb + (cbptr + i))->icb;
					cout<<dsbob->cb_tb->GetDataByPos(icb)->id<<endl;
				}

			}
		}
		listcoll.pop_front();
		cout<<"========================="<<endl;
	}
}

//找到下游区段id
int FHZG::FindDownZone(int icb)
 {
	int i, j, k;
	int cbptr, cbnum;
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	for (i = 0; i < lv_fdir_zone; i++) //循环所有的区段
	{
		if((fdir_zone + i)->icb == icb)
		{
			return i;//返回逻辑号
		}
	}
	return -1;
}

//统计一棵树中的负荷总数
float FHZG::LdSum(Tree<DataType>* pTree, TreeNode<DataType>* pNode)
{
	cout << __FUNCTION__ << endl;

	down_zn_list.clear();
	float amount = 0;
	LIST_TREENODE temcoll;
	pTree->BreadthFirstErgodic(pNode == NULL ? pTree->root : pNode, temcoll);

	for (LIST_TREENODE::iterator it = temcoll.begin(); it != temcoll.end(); it++)
	{
		if ((*it)->data.type == _NODE_ZN)
		{
			cout << __FUNCTION__ << ":" << (*it)->data.node_zn.cb_id << "," << (*it)->data.node_zn.ldamount << endl;
//			DisplayZoneCB((*it)->data.node_zn.izone);
			amount += ((*it)->data.node_zn.ldamount);
//			cout << amount << endl;

			if(pTree == m_pTree)	//down_zn_list为储存需要转供的区段列表，所以只有当计算主树负荷总数时才操作它
				down_zn_list.push_back(*it);
		}
	}
//	while(!temcoll.empty())
//	{
//		amount += (temcoll.front()->data.node_zn.ldamount);
//		temcoll.pop_front();
//	}
	return amount;
}

//负荷转供算法
bool FHZG::LdTransfer(Tree<DataType>* pTree)
{
	char *so_id = NULL;
	float ldsum = 0;

	while (true)
	{
		//总负荷数
		ldsum = LdSum(pTree);

		printf("需要转供的负荷总数为：%f\r\n", ldsum);
		fhzg_capacity = ldsum;
		LdByOne(ldsum, sel_so_list);

		if (sel_so_list.size() > 0)
		{
			m_Type = FHZG_TYPE_ALL;
			for (LIST_TREENODE::iterator it = lst_so_removed.begin(); it != lst_so_removed.end(); it++)
			{
				cout << __FUNCTION__ << (*it)->data.node_so.so_name << endl;
				sel_so_list.push_back(*it);
			}
//		cout<<"单独供电电源ID = "<<so_id<<endl;
			return true;
		}
		else //多电源转供
		{

			if (MultiSo(ldsum) > 0)
			{
				//前面消减
				return false;
			}
			else
			{
				m_Type = FHZG_TYPE_SHARE;

				m_map_so_zn.clear();

				for (LIST_TREENODE::iterator it = sel_so_list.begin(); it != sel_so_list.end(); it++)
				{
					LIST_TREENODE lstGet;
					float num = (*it)->data.node_so.take_ld_scale;
					TRACE("take ld is %f\n", num);
					TreeNode<DataType>* pNode = NULL;
					int dist = 9999;

					GetLD(m_pTree, (*it), m_pTree->root, num, (*it)->data.node_so.left_capacity - num, lstGet);

					cout << "电源（  " << (*it)->data.node_so.so_name << "  )分摊的负荷为：" << (*it)->data.node_so.take_ld_scale << endl;

					if (lstGet.size() <= 0)
						continue;

					for (LIST_TREENODE::iterator it_get = lstGet.begin(); it_get != lstGet.end(); it_get++)
					{
						//计算距离检修开关最近的一个区段结点
						int tmp_dist = m_pTree->LengthOfRegion(m_pTree->root, *it_get);

						if (tmp_dist < dist)
						{
							pNode = *it_get;
							dist = tmp_dist;
						}

						TRACE("区段电流放入开关%s, 区段负荷%f\r\n", (*it_get)->data.node_zn.GetText(), (*it_get)->data.node_zn.ldamount);
					}

					m_map_so_zn[(*it)] = lstGet;

					_OPERATION_STEP step = { 0 };

					step.close_cb= (*it)->data.node_so.cb_id;
					step.open_cb= pNode->data.node_zn.cb_id;
					step.so_id= (*it)->data.node_so.so_name;
					m_operations.push_front(step);
				}

//			return true;
			}

			//查找主树上没有被分摊掉的结点
			LIST_TREENODE lst_tree_node;
			bool bhas_unproc_node = false; //主树上是否有没有被分摊掉的结点
			LIST_TREENODE::iterator it_temp;

			m_pTree->BreadthFirstErgodic(m_pTree->root, lst_tree_node);

			for (it_temp = lst_tree_node.begin(); it_temp != lst_tree_node.end(); it_temp++)
			{
				if ((*it_temp)->flag & _NODE_UNPROC)
					bhas_unproc_node = true;
				else
					(*it_temp)->flag |= _NODE_UNPROC;
			}

			//分摊成功
			if (!bhas_unproc_node)
			{
				return true;
			}

			//找一个负荷数最小的结点消减负荷
			FindLeafNode(3, lst_tree_node);

			//无可消减负荷并且仍然无法分摊，直接返回失败
			if(lst_tree_node.size() <=0 ) return false;

			TreeNode<DataType> *pNode = lst_tree_node.back();

//			for (it_temp = lst_tree_node.begin(); it_temp != lst_tree_node.end(); it_temp++)
//			{
//				cout << (*it_temp)->data.node_zn.cb_id<<endl;
//				if ((*it_temp)->data.type == _NODE_SO)
//					continue;
//				if ((*it_temp)->data.node_zn.ldamount <= 0)
//					continue;
//
//				if ((*it_temp)->data.node_zn.ldamount < pNode->data.node_zn.ldamount)
//					pNode = *it_temp;
//			}



			//主树上仍然有没有被分摊掉的结点，分摊失败
			if (bhas_unproc_node)
			{
				printf("!!!!!!!!!!!!!!!!!消减负荷%ld(%f)\r\n", pNode->data.node_zn.cb_id, pNode->data.node_zn.ldamount);
				pNode->flag |= _NODE_DELETE_;
				cut_leaf_list.push_back(pNode);

				if (!IsNodeInList(pNode->fathernode, cut_leaf_list))
				{
					m_cut_operations.push_back(pNode->data.node_zn.cb_id);
				}
				m_operations.clear();
				continue;
			}
		}
	}

	return false;
}

//有电源符合单独供电
void FHZG::LdByOne(float ldsum, LIST_TREENODE &lstSO)
{
	lstSO.clear();

	int index = 0;
	int temmax = 0;


	LIST_TREENODE temlist;
	char* tem = NULL;
	char* so_id_tem = NULL;

	LIST_TREENODE::const_iterator pos;
	for(so_pos = so_list.begin();so_pos != so_list.end();so_pos ++)
	{
		if((*so_pos)->data.node_so.left_capacity > ldsum)
		{
			//将所有满足全部供电的电源存起来
			lstSO.push_back(*so_pos);
		}
	}

	if(/*lstSO.size() > 0*/0)   //潮流暂时不能计算，先注释掉2012年6月19日
	{
		CLS_CB *cb = dsbob->cb_tb->Find(CLS_CB(fhzg_cb));

		if(cb == NULL)
		{
			printf("未找到开关%ld---------------\n",fhzg_cb);
		}

		cb->q |= CB_OPEN;
		if(tp->tp_cb_changed(fhzg_cb) != TP_FAIL)
		{
			for(so_pos = lstSO.begin(); so_pos != lstSO.end(); so_pos++)
			{
				cb = dsbob->cb_tb->Find(CLS_CB((*so_pos)->data.node_so.cb_id));
				CLS_SO *so = dsbob->so_tb->Find(CLS_SO((*so_pos)->data.node_so.so_name));

				cb->q &= ~CB_OPEN;
				if (tp->tp_cb_changed(cb->id) != TP_FAIL)
				{
					if (so != NULL)
					{
						dsbob->co_tb->Data[0].wloss = 0;
//						pwrflow->GetAnalogValue();			//D5000潮流计算还未移植
//						pwrflow->ExecutePwrflow(so->iisland);
						(*so_pos)->data.node_so.new_wloss = dsbob->co_tb->Data[0].wloss;

						printf("若用电源%ld独立转供，则其所在岛的网损为%f\r\n", (*so_pos)->data.node_so.so_name, (*so_pos)->data.node_so.new_wloss);
					}

					cb->q |= CB_OPEN;
					tp->tp_cb_changed(cb->id);
				}
			}
		}

		cb->q &= ~CB_OPEN;
		tp->tp_cb_changed(fhzg_cb);
	}
#if 0
	if (temlist.empty())return NULL;
	for (pos = temlist.begin(); pos != temlist.end(); pos++)
	{
		so_id_tem = (*temlist.begin())->data.node_so.cb_id;
		index = temlist.front()->data.node_so.left_capacity;
		if (index > temmax)
		{
			temmax = index;
			so_id_tem = (*pos)->data.node_so.cb_id;
		}
	}
	return so_id_tem;
#endif
}

//多个电源供电
float FHZG::MultiSo(float ldsum)
{
	float index = 0;
	int i=0;

	//将so_list中的数据拷贝到so_list_swp中
	for(so_pos = so_list.begin();so_pos != so_list.end();so_pos ++)
	{
		so_list_swp.push_back((*so_pos));
	}
	//调用选择电源函数，将合适的电源组合存到sel_so_list链表中
	float left_ld = SelectSo(sel_so_list,ldsum);
	if(left_ld > 0) return left_ld;

	for(so_pos = sel_so_list.begin();so_pos != sel_so_list.end();so_pos ++)
	{
		i++;
		index += (*so_pos)->data.node_so.left_capacity;
		cout<<"提供分摊转供的电源"<<i<<"为"<<(*so_pos)->data.node_so.cb_id<<endl;
		cout<<"电源的剩余容量是："<<(*so_pos)->data.node_so.left_capacity<<endl;
	}

	for(so_pos = sel_so_list.begin();so_pos != sel_so_list.end();so_pos ++)
	{
		float test = 0;
		test = ((*so_pos)->data.node_so.left_capacity * ldsum * 1.00)/index;
		(*so_pos)->data.node_so.take_ld_scale = test;
	}

	return 0;
}

//选择供电电源
float FHZG::SelectSo(LIST_TREENODE &solist,float ldsum)
{
//	int max = 0;
	solist.clear();
	float ld_sum = 0;
	TreeNode<DataType>* temnode = new TreeNode<DataType>;
	LIST_TREENODE::iterator ptem;
	while(ld_sum<=ldsum)
	{
		if(so_list_swp.empty())
		{
			cout<<"超负荷，请消减负荷！"<<endl;
			return ldsum - ld_sum;
		}
		ptem = so_list_swp.begin();
		for (so_pos = so_list_swp.begin(); so_pos != so_list_swp.end();so_pos++)
		{
			if ((*so_pos)->data.node_so.left_capacity > (*ptem)->data.node_so.left_capacity)
			{
				ptem = so_pos;
			}
		}
		//将需要的电源按距离检修开关从近到远的距离放入到solist链表中
		LIST_TREENODE::iterator it;
		for (it = solist.begin(); it != solist.end(); it++)
		{
			if ((*it)->data.dist > (*ptem)->data.dist)
			{
				solist.insert(it, (*ptem));
				break;
			}
			else if ((*it)->data.dist == (*ptem)->data.dist)	//如果距离相等，则比较结点个数
			{
				if ((*it)->data.cnt_node > (*ptem)->data.cnt_node)
				{
					solist.insert(it, (*ptem));
					break;
				}
			}
		}

		if(it == solist.end()) solist.push_back(*ptem);

		ld_sum += (*ptem)->data.node_so.left_capacity;
		//然后，将so_list_swp中这个元素删除，意为此电源已被使用
		so_list_swp.erase(ptem);
	}

	//将剩下的电源从树中删掉
	while(!so_list_swp.empty())
	{
		//m_pTree->TreeRemoveSubTree(so_list_swp.front());
		so_list_swp.pop_front();
	}
	return 0;
}

//判断结点是否在链表中
bool IsNodeInList(TreeNode<DataType> *pNode, LIST_TREENODE lstNode)
{
	for(LIST_TREENODE::iterator it = lstNode.begin(); it != lstNode.end(); it++)
	{
		if((*it)->data.node_zn.cb_id== pNode->data.node_zn.cb_id)
			return true;
	}

	return false;
}

/*
 * 在树的指定结点的孩子结点中查找电源类结点
 *
 * lstNode:树指定结点的所有孩子结点
 * lstSO:孩子结点中的电源类结点
 *
 */
void FindSO(Tree<DataType> *pTree, TreeNode<DataType> *pNode, LIST_TREENODE &lstNode, LIST_TREENODE &lstSO)
{
	lstNode.clear();
	lstSO.clear();

	pTree->BreadthFirstErgodic(pNode, lstNode);

	for(LIST_TREENODE::iterator it = lstNode.begin(); it != lstNode.end(); it++)
	{
		if((*it)->data.type == _NODE_SO)
		{
			lstSO.push_back(*it);
		}
	}
}


/*
 * 在树pTree的结点node1,node2间获取最大数额为num的结点负荷存入lstGet中
 *
 * offset为允许实际获取到的负荷数额与num的最大偏差
 *
 */
bool FHZG::GetLD(Tree<DataType> *pTree, TreeNode<DataType>* node1, TreeNode<DataType>* node2, float &num, float offset, LIST_TREENODE &lstGet)
{
	cout<<__FUNCTION__<<lstGet.size()<<endl;
	assert(pTree != NULL);
	assert(node1 != NULL);
	assert(node2 != NULL);

	LIST_TREENODE  lstPath;//保存node1到node2间的结点
	map<TreeNode<DataType> *, LIST_TREENODE >  map_node_so;//保存下游有电源的孩子结点

	pTree->GetPath(node1, node2, lstPath);

	for(LIST_TREENODE::iterator it = lstPath.begin(); it != lstPath.end(); it++)
	{
		int cnt_so = 0;	//结点的孩子结点的子树中有电源结点的孩子结点个数
		if ((*it)->data.type == _NODE_SO)
		{
			(*it)->flag &= ~_NODE_UNPROC;
			continue;
		}

		if(!((*it)->flag & _NODE_UNPROC)) break;

		TreeNode<DataType> *pChild = pTree->GetChildNode((*it));
		LIST_TREENODE lstChildren;
		lstChildren.clear();
		float total_ld = (*it)->data.node_zn.ldamount;

		while(pChild != NULL)
		{
			if(/*!IsNodeInList(pChild, lstPath)*/pChild->flag & _NODE_UNPROC)
			{
				LIST_TREENODE tmp_Children;
				LIST_TREENODE  lstSO;

				FindSO(pTree, pChild, tmp_Children, lstSO);

				if(lstSO.size() > 0)
				{
					map_node_so[pChild] = lstSO;
					cnt_so ++;
//					lstChildren.clear();
				}
				else
				{
					while (!tmp_Children.empty())
					{
						lstChildren.push_back(tmp_Children.front());
						tmp_Children.pop_front();
					}

					total_ld += LdSum(pTree, pChild);
				}
			}

			pChild = pTree->GetNextBrotherNode(pChild);
		}

		if (cnt_so < 2)
		{
			num -= total_ld;

			if (num < 0)
			{
				if (num * -1 < offset)
				{

					lstGet.push_back(*it);
					(*it)->flag &= ~_NODE_UNPROC;

					for (LIST_TREENODE::iterator it_child = lstChildren.begin(); it_child != lstChildren.end(); it_child++)
					{
						lstGet.push_back(*it_child);
						(*it_child)->flag &= ~_NODE_UNPROC;
					}

					return true;
				}
				else
				{
					return true;
				}
			}
			else
			{
				lstGet.push_back(*it);
				(*it)->flag &= ~_NODE_UNPROC;

				for (LIST_TREENODE::iterator it_child = lstChildren.begin(); it_child != lstChildren.end(); it_child++)
				{
					lstGet.push_back(*it_child);
					(*it_child)->flag &= ~_NODE_UNPROC;
				}
			}
		}
	}

	for(map<TreeNode<DataType> *, LIST_TREENODE >::iterator it_map = map_node_so.begin(); it_map != map_node_so.end(); it_map++)
	{
		GetLD(pTree, it_map->first, *(it_map->second.begin()), num, offset, lstGet);
	}

	return false;
}

//保存两个区段之间不带电源的节点
void FHZG::NodeOfRegion(TreeNode<DataType>* node1,TreeNode<DataType>* node2,LIST_TREENODE &nodelist)
{

	/*
	 * nodelist-------->用来存放最终节点
	 * temp------------->用来存放node1到node2之间的路径所经过节点
	 * templist-------->用来存放temp中数据的孩子节点
	 * tempchild------->用来存放以templist中节点为根节点的树的节点
	*/
	TreeNode<DataType>* tempnode = new TreeNode<DataType>;
	LIST_TREENODE temlist,temp,tempchild;
	LIST_TREENODE::iterator pos,pos1;
	if(m_pTree->JudgeFather(node1,node2))//node2为祖先节点
	{
		//将两点之间经过的节点存放到nodelist中
		temp.push_back(node1);
		while(node1->fathernode != node2)
		{
			tempnode = node1->fathernode;
			temp.push_back(node1->fathernode);
			node1 = tempnode;
		}
	}
	//然后判断nodelist中的节点的孩子节点中是否有电源,如果不存在，则放入nodelist中
	for(so_pos = temp.begin();so_pos != temp.end();so_pos++)
	{
		if((*so_pos)->childnode == NULL)
		{
			nodelist.push_back(*so_pos);
		}
		else
		{
			tempnode = (*so_pos)->childnode;
			if(tempnode->next == NULL)
			{
				nodelist.push_back(*so_pos);
			}
			else
			{
				temlist.push_back(tempnode);//temlist用来存放temp中的节点的孩子节点
				while(tempnode->next)
				{
					temlist.push_back(tempnode->next);
					tempnode = tempnode->next;
				}

				for(pos = temlist.begin();pos != temlist.end();pos ++)
				{
					for(pos1 = nodelist.begin();pos1 != nodelist.end();pos1 ++)
					{
						if((*pos1)->data.type == _NODE_ZN && (*pos)->data.type == _NODE_ZN)
						{
							if((*pos1)->data.node_zn.cb_id==(*pos)->data.node_zn.cb_id)
							{
								//将temlist中与temp中相同的孩子结点删除
								temlist.erase(pos1);
							}
						}
					}
					//删除之后，以剩下的孩子节点为根节点遍历，然后判断是否存在电源
					m_pTree->BreadthFirstErgodic((*pos),tempchild);
					if(!JudgeSo(tempchild))
					{
						nodelist.push_back((*pos)->fathernode);
						while(!tempchild.empty())
						{
							nodelist.push_back(tempchild.front());
							tempchild.pop_front();
						}
					}
				}
			}
		}
	}
}

//判断树中是否存在电源
bool FHZG::JudgeSo(LIST_TREENODE &tempchild)
{
	tempchild.pop_front();//删除链表中的第一个节点，也就是根节点
	for(so_pos = tempchild.begin();so_pos != tempchild.end();so_pos ++)
	{
		if((*so_pos)->data.type == _NODE_SO)
		{
			while(!tempchild.empty())
			{
				tempchild.pop_back();
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void FHZG::CalcSODist(LIST_TREENODE &lstSO)
{
	LIST_TREENODE::iterator it;
	//计算每个结点的计数值
	for (it = lstSO.begin(); it != lstSO.end(); it++)
	{
		TreeNode<DataType> *pNode = (*it);

		while ((pNode = m_pTree->GetParent(pNode)) != NULL)
		{
			pNode->data.dist++;
		}
	}

	//计算每个结点到根结点的距离
	for (it = lstSO.begin(); it != lstSO.end(); it++)
	{
		TreeNode<DataType> *pNode = (*it);

		while ((pNode = m_pTree->GetParent(pNode)) != NULL)
		{
			if (pNode->data.dist > 1)
				(*it)->data.dist++;
			(*it)->data.cnt_node++;
		}

		printf("电源（  %ld  ）到检修开关的距离为%d，结点数为%d\r\n", (*it)->data.node_so.so_name, (*it)->data.dist, (*it)->data.cnt_node);
	}
}

/*
 * 在所有树中找优先级为pri的叶子结点按负荷数从大到小的顺序存入leaflist_tem，
 * 并将所有叶子结点的负荷总数作为函数的返回值返回
 */
float FHZG::FindLeafNode(int pri, LIST_TREENODE &nodelist)
{
	nodelist.clear();
	LIST_TREENODE tmp_nodelist;
	float sum = 0;

	//先遍历主树上的所有节点
	m_pTree->BreadthFirstErgodic(m_pTree->root, tmp_nodelist);

	while (!tmp_nodelist.empty())
	{
		LIST_TREENODE::iterator it_tmp = tmp_nodelist.begin();

		//判断孩子结点是否都被消减掉
		bool child_del_all = true;
		TreeNode<DataType>* next_node = (*it_tmp)->childnode;
		while (next_node != NULL)
		{
			if (next_node->flag & _NODE_DELETE_)
			{
				next_node = next_node->next;
				continue;
			}
			{
				child_del_all = false;
				break;
			}
		}

		if ((*it_tmp)->data.type == _NODE_ZN //区段节点
			&& ((*it_tmp)->childnode == NULL || child_del_all) //没有孩子节点或者孩子节点已删除
			&& (*it_tmp)->data.node_zn.prio == pri && (*it_tmp)->data.node_zn.protect == NONE) //优先级满足并且不需要保电
		{
			LIST_TREENODE::iterator it;

			for (it = nodelist.begin(); it != nodelist.end(); it++)
			{
				if ((*it)->data.node_zn.ldamount < (*it_tmp)->data.node_zn.ldamount)
				{
					nodelist.insert(it, (*it_tmp));
					sum += (*it_tmp)->data.node_zn.ldamount;
					break;
				}
			}

			if (it == nodelist.end())
			{
				nodelist.push_back(*it_tmp);
				sum += (*it_tmp)->data.node_zn.ldamount;
			}
		}

		tmp_nodelist.pop_front();
	}

#if 0			//只消减需要转供区段的负荷
	//再遍历电源节点树上的节点
	for (map<string, Tree<DataType> >::iterator it = m_map_cb_tree.begin(); it != m_map_cb_tree.end(); it++)
	{
		it->second.BreadthFirstErgodic(it->second.root, tmp_nodelist);

		while (!tmp_nodelist.empty())
		{
			LIST_TREENODE::iterator it_tmp = tmp_nodelist.begin();

			if ((*it_tmp)->data.type == _NODE_ZN //区段节点
				&&((*it_tmp)->childnode == NULL || ((*it_tmp)->childnode->flag & _NODE_DELETE_)) //没有孩子节点或者孩子节点已删除
				&& (*it_tmp)->data.node_zn.prio == pri) //优先级满足
			{
				LIST_TREENODE::iterator it;

				for (it = nodelist.begin(); it != nodelist.end(); it++)
				{
					if ((*it)->data.node_zn.ldamount < (*it_tmp)->data.node_zn.ldamount)
					{
						nodelist.insert(it, (*it_tmp));
						sum += (*it_tmp)->data.node_zn.ldamount;
						break;
					}
				}

				if (it == nodelist.end())
				{
					nodelist.push_back(*it_tmp);
					sum += (*it_tmp)->data.node_zn.ldamount;
				}
			}
			tmp_nodelist.pop_front();
		}
	}
#endif
	return sum;
}

bool FHZG::SelectCutLD(float ldsum, int max_pri)
{
	if(ldsum <= 0) return true;

	const int min_pri = 3;
	int cur_pri = min_pri;
	LIST_TREENODE nodelist;

	while (true)
	{
		float sum = FindLeafNode(cur_pri, nodelist);

		if (nodelist.size() <= 0)	//没找到满足条件的叶子节点
		{
			cur_pri--;

			if (cur_pri <= max_pri)
				return false;
		}

		if (sum < ldsum)	//所有找到的叶子节点负荷总数仍不满足需要消减的负荷数
		{
			for (LIST_TREENODE::iterator it = nodelist.begin(); it != nodelist.end(); it++)
			{
				(*it)->flag |= _NODE_DELETE_;
				cut_leaf_list.push_back((*it));
			}

			ldsum -= sum;
		}
		else
		{
			CNT = 1;
			float tmp_ld = 0;

			for (LIST_TREENODE::iterator it = nodelist.begin(); it != nodelist.end(); it++, CNT++)
			{
				tmp_ld += (*it)->data.node_zn.ldamount;

				if (tmp_ld > ldsum)
					break;
			}

			int size = nodelist.size();
			TreeNode<DataType>** result = new TreeNode<DataType>*[CNT];
			LIST_TREENODE result_ld;
			int j = 0;

			TreeNode<DataType>** nums = new TreeNode<DataType>*[size];
			cout << "可消减负荷为：" << endl;
			for (so_pos = nodelist.begin(); so_pos != nodelist.end(); so_pos++)
			{
				cout << (*so_pos)->data.node_zn.ldamount << "  ";
				nums[j] = (*so_pos);
				++j;
			}

			cout << endl;
			GetCombination(nums, size, CNT, ldsum, result, result_ld);
			cout << "需要消减的负荷为：";
			while (!result_ld.empty())
			{
				cout << result_ld.front()->data.node_zn.ldamount << " ";
				cut_leaf_list.push_back(result_ld.front()); //将要消减的负荷存到cut_leaf_list中
				result_ld.front()->flag |= _NODE_DELETE_;
				result_ld.pop_front();
			}

			cout << endl;

			break;
		}
	}

	return true;
}

//判断减掉所有的叶子节点的负荷，是否满足负荷消减的要求
float FHZG::JudgeLeafLD(float ldsum,int pri,LIST_TREENODE &leaflist)
{
	float temsum = 0;
	int tem_pri = 0;
	for(so_pos = leaflist.begin();so_pos != leaflist.end();so_pos ++)
	{
		temsum += (*so_pos)->data.node_zn.ldamount;
	}
	if(temsum>ldsum)
	{
		return -1;
	}
	else
		return (ldsum-temsum);
}

void FHZG::GetCombination(TreeNode<DataType>* nums[], int cnt_nums, int cnt_need,float ldsum,TreeNode<DataType>* result[],LIST_TREENODE &result_ld)
{
	float index = 0.0;
	float temp = (unsigned int)(-1)*1.0;//将temp设为int的最大值
	int flag = 1;
	if (cnt_need <= 0)
	{
		//测试用
//		for (int i = 0; i < CNT-1; i++)
//		{
//			cout<<result[i]->data.node_zn.ldamount<<",";
//		}
//		cout<<result[CNT-1]->data.node_zn.ldamount<<endl;

		for (int i = 0; i < CNT; i++)
		{
			index += result[i]->data.node_zn.ldamount;
		}
		if(index > ldsum)
		{
			if(index < temp)
			{
				temp = index;
				while(!result_ld.empty())
				{
					result_ld.pop_front();
				}
				for(int i = 0;i< CNT;i++)
				{
					result_ld.push_back(result[i]);
				}
			}
		}
		return;
	}


	TreeNode<DataType>** p = nums;
//	TreeNode<DataType>* ptem = new TreeNode<DataType>;
	while (p + cnt_need - 1 <= nums + cnt_nums - 1)
	{
		result[CNT - cnt_need] = *p;
		GetCombination(p + 1, cnt_nums - (p - nums) - 1, cnt_need - 1,ldsum,result,result_ld);
		p++;
	}
}

const LIST_TREENODE *FHZG::GetSO()
{
	for(LIST_TREENODE::iterator it = sel_so_list.begin(); it != sel_so_list.end(); it++)
	{
		id_to_pid((*it)->data.node_so.so_name);
	}

	return &sel_so_list;
}

const LIST_TREENODE *FHZG::GetZoneBySO(const long so_dv)
{
	printf("the so dv is %ld __________________________\n", so_dv);

	for (map<TreeNode<DataType>*, LIST_TREENODE>::iterator it = m_map_so_zn.begin(); it != m_map_so_zn.end(); it++)
	{
		if (it->first->data.node_so.dv_id== so_dv)
		{
			return &(it->second);
		}
	}

	return NULL;
}

int FHZG::Do(const long cb_id, bool bbuild_db)
{
	printf("\033[42m负荷转供开关为：%ld\033[0m\r\n", cb_id);

	float total_ld;
	fhzg_cb= cb_id;

	down_zn_list.clear();
	sel_so_list.clear();
	so_list.clear();
	cut_leaf_list.clear();
	m_pTree->TreeRemoveSubTree(m_pTree->root);
	m_map_cb_tree.clear();
	so_list_swp.clear();
	m_operations.clear();
	m_cut_operations.clear();
	lst_so_removed.clear();
	fhzg_capacity = 0.0;

	if(bbuild_db)
		BuildDB();

	if (tp->tp_allnew() == TP_FAIL)
	{
		printf("tp->tp_allnew() failed!\r\n");
		return FDIR_FAIL_DIRECT;
	}

//	dsbob->WriteReDB(NULL, NULL, NULL);
	long tmp_cb_id = cb_id;

	pid_to_id(tmp_cb_id);

	int zoneid = GetDownZoneId(tmp_cb_id);
	if(zoneid == -1)
	{
		printf("找不到转供开关%ld的下游区段！\r\n", tmp_cb_id);
		return FDIR_FAIL_DIRECT;
	}
	else
	{
		DisplayZoneCB(zoneid);
	}
	//zg_type 为1时用电流计算负荷转供,tianyq2012年6月16日添加
	zg_type = FHZG_CTRL_I;
	lst_protected.clear();
	//将需要转供的区段和联络开关形成树
	LIST_TREENODE so_list_tree;
	LIST_TREENODE main_list_tree;
	map<long, int> map_so_zone;
	ErgodicTree(zoneid, m_pTree->root, m_pTree, false, main_list_tree, map_so_zone);

	for(map<long, int>::iterator map_vi = map_so_zone.begin(); map_vi != map_so_zone.end(); map_vi++)
	{
		ErgodicTree((*map_vi).second, m_map_cb_tree[(*map_vi).first].root, &m_map_cb_tree[(*map_vi).first], true, so_list_tree, map_so_zone);
	}
	GetZoneLd(so_list_tree, true);
	GetZoneLd(main_list_tree, false);

	//以电流作为转供参考对象
	if(zg_type == FHZG_CTRL_I)
	{
		_DATA_NODE_SO *cb_node = new _DATA_NODE_SO;
		CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
		Tree<DataType>* temp_pTree;
		temp_pTree = new Tree<DataType>;
		temp_pTree->TreeRemoveSubTree(temp_pTree->root);
		TreeNode<DataType> *pChild = m_pTree->root;
		TreeNode<DataType> *father;
		TreeNode<DataType> *new_father;
		map<long, float>::iterator zone_value_vi;

		father = temp_pTree->root;
		new_father = temp_pTree->TreeAddLeaf(father, pChild->data);
		new_father->data.node_zn.map_zn_ld[pChild->data.node_zn.izone] = pChild->data.node_zn.ld_list;
		//要从历史库中取出根节点电流入流开关的电流遥测值
		new_father->data.node_zn.ldamount = GetCbIVlaue(cb_id);
		TRACE("要断开的开关是:%ld\r\n", cb_id);
		if(new_father->data.node_zn.ldamount <= 0)
		{
			zone_value_vi = zone_value_map.find((fdir_zone + zoneid)->zn_id);
			if(zone_value_vi != zone_value_map.end())
			{
				new_father->data.node_zn.ldamount = (*zone_value_vi).second;
				printf("111要断开的开关的电流值%f\n",new_father->data.node_zn.ldamount);
			}
			else
			{
				if(new_father->data.node_zn.ldamount < 0)
				{
					printf("%s要断开的%ld开关的电流值没有获取到line%d\n",__FUNCTION__,cb_id,__LINE__);
					return FHZG_FAIL_NO_IVALUE;
				}
				printf("222要断开的开关的电流值%f\n",new_father->data.node_zn.ldamount);
			}
		}
		else
		{
			printf("333要断开的开关的电流值%f\n",new_father->data.node_zn.ldamount);
		}
		fhzg_capacity = new_father->data.node_zn.ldamount;
		pChild = m_pTree->GetChildNode(pChild);
		while(pChild)
		{
			ReBuildTree(new_father, temp_pTree, pChild, m_pTree);
			pChild = pChild->next;
		}
		delete m_pTree;
		m_pTree = temp_pTree;

		LIST_TREENODE so_tree;
		list<TreeNode<DataType>*>::iterator list_vi;
		map<int ,list<CLS_LD> >::iterator zone_ld_vi;

		m_pTree->DepthFirstErgodic(m_pTree->root,so_tree);
		for(list_vi = so_tree.begin(); list_vi != so_tree.end(); list_vi++)
		{
			if((*list_vi)->data.type == _NODE_ZN)
			{
				for(zone_ld_vi = (*list_vi)->data.node_zn.map_zn_ld.begin();zone_ld_vi != (*list_vi)->data.node_zn.map_zn_ld.end();zone_ld_vi++)
				{

					zone_value_vi = zone_value_map.find((fdir_zone + (*zone_ld_vi).first)->zn_id);
					if(zone_value_vi != zone_value_map.end())
					{
						continue;
					}
					else
					{
						zone_value_map[(fdir_zone + (*zone_ld_vi).first)->zn_id] = (*list_vi)->data.node_zn.ldamount;
					}
				}
				printf("树的区段的入流开关id是%ld\n",(*list_vi)->data.node_zn.cb_id);
			}
			else
			{
				printf("树的区段的联络开关id是%ld,父区段的入流开关是%ld\n",(*list_vi)->data.node_so.cb_id,m_pTree->GetParent((*list_vi))->data.node_zn.cb_id);
			}
		}

		map<long, Tree<DataType> >::iterator map_vi;
		list<TreeNode<DataType>*>::iterator so_vi;
		for(map_vi = m_map_cb_tree.begin(); map_vi != m_map_cb_tree.end(); map_vi++)
		{
			temp_pTree = NULL;
			temp_pTree = new Tree<DataType>;
			temp_pTree->TreeRemoveSubTree(temp_pTree->root);
			pChild = map_vi->second.root;
			for(so_vi = so_list.begin(); so_vi != so_list.end(); so_vi++)
			{
				if(map_vi->first == (*so_vi)->data.node_so.cb_id)
				{
					printf("发现的可转供的电源为%ld\n",(*so_vi)->data.node_so.so_name);
					printf("可转供的电源剩余的电流值为%f\n",(*so_vi)->data.node_so.left_capacity);
					break;
				}
			}
			if(so_vi == so_list.end())
			{
				continue;
			}
			father = temp_pTree->root;
			new_father = temp_pTree->TreeAddLeaf(father, pChild->data);
			new_father->data.node_zn.map_zn_ld[pChild->data.node_zn.izone] = pChild->data.node_zn.ld_list;
			new_father->data.node_zn.ldamount = GetCbIVlaue(pChild->data.node_zn.cb_id);
			//new_father->data.node_zn.ldamount = (*so_vi)->data.node_so.full_capacity - (*so_vi)->data.node_so.left_capacity;

			pChild = map_vi->second.GetChildNode(pChild);
			while(pChild)
			{
				ReBuildTree(new_father, temp_pTree, pChild, &(map_vi->second));
				pChild = pChild->next;
			}

			map_vi->second.TreeRemoveSubTree(map_vi->second.root);
			map_vi->second.root = temp_pTree->root;
			LIST_TREENODE so_tree1;
			temp_pTree->DepthFirstErgodic(temp_pTree->root,so_tree1);
			for(list_vi = so_tree1.begin(); list_vi != so_tree1.end(); list_vi++)
			{
				if((*list_vi)->data.type == _NODE_ZN)
				{
					for(zone_ld_vi = (*list_vi)->data.node_zn.map_zn_ld.begin();zone_ld_vi != (*list_vi)->data.node_zn.map_zn_ld.end();zone_ld_vi++)
					{

						zone_value_vi = zone_value_map.find((fdir_zone + (*zone_ld_vi).first)->zn_id);
						if(zone_value_vi != zone_value_map.end())
						{
							continue;
						}
						else
						{
							zone_value_map[(fdir_zone + (*zone_ld_vi).first)->zn_id] = (*list_vi)->data.node_zn.ldamount;
						}
					}
					printf("电源树的区段的入流开关id是%ld\n",(*list_vi)->data.node_zn.cb_id);
				}
				else
				{
					printf("电源树的区段的联络开关id是%ld,父区段的入流开关是%ld\n",(*list_vi)->data.node_so.cb_id,m_pTree->GetParent((*list_vi))->data.node_zn.cb_id);
					//printf("电源的剩余结点")
				}
			}

			//map_vi->second.TreeRemoveSubTree(map_vi->second.root);
			//map_vi->second.TreeAddSubTree(map_vi->second.root, temp_pTree->root);
		}

	}

	//减负荷
	float total_left_capacity = 0;//能够提供的剩余容量总数

	m_pTree->SaveTree("FHZG.xml");

	total_ld = LdSum(m_pTree, m_pTree->root);//需要转供的负荷总数
	TRACE("需要转供的负荷总数为%f\r\n", total_ld);

	for(LIST_TREENODE::iterator it = so_list.begin(); it != so_list.end(); it++)
	{
		total_left_capacity += (*it)->data.node_so.left_capacity;
	}
	printf("可转供的电源剩余的电流值为%f\n",total_left_capacity);
	if (total_left_capacity <= 0)
	{
		cout << "没有可供转供的电源，转供失败！" << endl;
		if (so_list.size() > 0)
		{
			return FHZG_FAIL_SO_FULL;
		}
		else
		{
			return FHZG_FAIL_NO_SO;
		}
	}

	float over_ld = total_ld - total_left_capacity;	//需要消减掉的负荷总数

	if(over_ld > 0)
	{
		printf("所有电源能够提供的剩余容量总和为:%f,需要转供的负荷总数为:%f,需要消减%f的负荷\r\n", total_left_capacity, total_ld, over_ld);
//		LIST_TREENODE lst_treenode;
//
//		if(FindLeafNode(3, over_ld, lst_treenode))
//		{
//			total_ld = LdSum(m_pTree, m_pTree->root);
//			SelectCutLD(total_ld, 1, lst_treenode);
//
//			for (LIST_TREENODE::iterator it = so_list.begin(); it != so_list.end(); it++)
//			{
//				(*it)->data.node_so.left_capacity = (*it)->data.node_so.full_capacity - LdSum(&(m_map_cb_tree[(*it)->data.node_so.cb_id]));
//				printf("调整后电源%s的剩余容量为%f\r\n", (*it)->data.node_so.so_name, (*it)->data.node_so.left_capacity);
//			}
//		}

		if (SelectCutLD(over_ld, 0))
		{
			m_cut_operations.clear();
			cout << "最终消减负荷结果：" << endl;
			for (LIST_TREENODE::const_iterator _it = cut_leaf_list.begin(); _it != cut_leaf_list.end(); _it++)
			{
				if(!IsNodeInList((*_it)->fathernode, cut_leaf_list))
				{
					m_cut_operations.push_back((*_it)->data.node_zn.cb_id);
				}
				cout << (*_it)->data.node_zn.ldamount << ",";
			}

			//重新统计电源的剩余容量
			for(LIST_TREENODE::iterator it_so = so_list.begin(); it_so != so_list.end(); it_so++)
			{
				Tree<DataType>* pTree = &m_map_cb_tree[(*it_so)->data.node_so.cb_id];

				(*it_so)->data.node_so.left_capacity = (*it_so)->data.node_so.full_capacity - LdSum(pTree, pTree->root);
			}
			cout << endl;
		}
		else
			return FHZG_FAIL_SO_FULL;
	}

	//计算每个联络开关到检修开关的距离
	CalcSODist(so_list);

	if(!LdTransfer(m_pTree))
	{
		return FHZG_FAIL_SO_FULL;
	}

	return 0;
}
