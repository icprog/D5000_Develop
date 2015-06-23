/*
 * cnt.cpp
 *
 *  Created on: 2013-9-23
 *      Author: tianyq
 */
#include "CntLdDv.h"
#include <iostream>
#include <list>
#include <map>
#include <string>
#include "netmom.h"
#include "fdirdb.h"
#include "FDIR_R.h"

extern list<long> lst_lwcbs;

int Cntld::Init()
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
	return FDIR_R_SUCCEED;
}

bool Cntld::count_dv()
{
	PSBOB::DV* dv_set = new PSBOB::DV;
	PSBOB::DV dv_st = { 0 };
	PSBOB::DV dv_fq = { 0 };
	PSBOB::ST st[10];
	int cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")));
	int size = sizeof(PSBOB::DV);

	bzero(st, sizeof(PSBOB::ST)*10);
	//�Ȱ�psbob�е�dv��Ϣ�������ÿ�
	for (int pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(dv_set, pos, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), 1, size) <= 0)
		{
			delete dv_set;
			cout << "��ȡ��dv����,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
			return false;
		}

		//���߲�д���
		if (dv_set->dtype == MENU_DV_TYPE_FD)
		{
			int retnum;
			if (dv_set->father_id != 0)
			{
				if (oodbread_rk(&dv_st, &(dv_set->father_id), const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(dv_st)) <= 0)
				{
					bzero(&dv_st, sizeof(PSBOB::DV));
					bzero(&dv_fq, sizeof(PSBOB::DV));
					cout << "��ȡ��dv����,errno:" << _oodb_errno << ",line:" << __LINE__ << endl;
				}
				else
				{
					if (dv_st.father_id != 0)
					{
						if (oodbread_rk(&dv_fq, &(dv_st.father_id), const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")),sizeof(dv_fq)) <= 0)
						{
							bzero(&dv_fq, sizeof(PSBOB::DV));
						}
					}
					else
					{
						bzero(&dv_fq, sizeof(PSBOB::DV));
					}
				}
			}
			else
			{
				bzero(&dv_st, sizeof(PSBOB::DV));
				bzero(&dv_fq, sizeof(PSBOB::DV));
			}
			int i = 0;
			retnum = oodbread_ra(st, "subarea_id", &dv_set->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")),sizeof(PSBOB::ST)*10);
			if ( retnum <= 0)
			{
				bzero(st, sizeof(PSBOB::ST)*10);

			}
			else
			{
				for ( i = 0; i < retnum/sizeof(PSBOB::ST); i++)
				{
					if (st[i].st_type & MENU_FAC_TYPE_SUBSTN)
					{
						break;
					}
				}
			}

			sprintf(sql, "Insert into HISDB.DV_INFO (ID, DESCR, ST, ST_DESCR, DV_FQ_DESCR, DV_FQ_ID) Values (\'%ld\', \'%s\', \'%ld\', \'%s\', \'%s\', \'%ld\')", dv_set->id, dv_set->name, i >= retnum/sizeof(PSBOB::ST) ?0:st[i].id, st[i].name, dv_fq.name, dv_fq.id);
			ExecSQL(sql);
			sprintf(sql, "update HISDB.DV_INFO set DESCR=\'%s\', ST=\'%ld\', ST_DESCR=\'%s\', DV_FQ_DESCR=\'%s\', DV_FQ_ID=\'%ld\' where id=\'%ld\'", dv_set->name, i >= retnum/sizeof(PSBOB::ST) ?0:st[i].id, st[i].name, dv_fq.name, dv_fq.id, dv_set->id);
			ExecSQL(sql);

		}
	}

	//Ȼ�����ͳ��dv�ĺ�������dv����ϸ��Ϣ���µ����ÿ���
	if (!get_info_dvs())
	{
		return false;
	}

	delete dv_set;
	return true;
}
//ͳ�����ߵĹ��ܣ�ͳ�����ߵĳ�վ�����翪�أ��������ߣ����糧վ�ȹ̶�����Ϣ
bool Cntld::get_info_dvs()
{
	int pos, izone, zzone;
	list<long>::iterator it_lst_lwcb;
	map<long, COUNT_DV> map_dv_info;
	map<long, COUNT_DV>::iterator it_map_dv;
	PSBOB::DV idv = { 0 };
	PSBOB::DV zdv = { 0 };
	PSBOB::ST ist = { 0 };
	PSBOB::ST zst = { 0 };
	CLS_CB *cb = NULL;
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();

	//ͨ�����翪�ص���ĩ�����λ�ȡdv����Ϣ���������е����翪�ؾ��ܰ�dv�������ϵ��ȡ��
	for (it_lst_lwcb = lst_lwcbs.begin(); it_lst_lwcb != lst_lwcbs.end(); it_lst_lwcb++)
	{
		pos = dsbob->cb_tb->FindPos(CLS_CB(*it_lst_lwcb));
		cb = dsbob->cb_tb->Find(CLS_CB(*it_lst_lwcb));
		if (pos == -1)
		{
			continue;
		}
		izone = (fdir_cb + pos)->ind;
		zzone = (fdir_cb + pos)->iznd;
		bzero(&idv, sizeof(PSBOB::DV));
		bzero(&zdv, sizeof(PSBOB::DV));
		bzero(&ist, sizeof(PSBOB::ST));
		bzero(&zst, sizeof(PSBOB::ST));
		GetDVByZone(izone, idv);
		GetDVByZone(zzone, zdv);

		//�е����翪����һ�˴������ε���û��dv
		GetStbyDv(idv.id, ist);
		GetStbyDv(zdv.id, zst);

		bool exiidv, exizdv, exiist, exizst;
		if (idv.id == 0)
		{
			exiidv = false;
		}
		else
		{
			exiidv = true;
		}
		if (zdv.id == 0)
		{
			exizdv = false;
		}
		else
		{
			exizdv = true;
		}
		if (ist.id == 0)
		{
			exiist = false;
		}
		else
		{
			exiist = true;
		}
		if (zst.id == 0)
		{
			exizst = false;
		}
		else
		{
			exizst = true;
		}
		if (exiidv)
		{
			it_map_dv = map_dv_info.find(idv.id);
			if (it_map_dv != map_dv_info.end())
			{
				it_map_dv->second.lwcb[*it_lst_lwcb] = cb->descr;
				if (exizdv)
				{
					it_map_dv->second.dv_lw[zdv.id] = zdv.name;
				}
				if (exizst)
				{
					it_map_dv->second.st_lw[zst.id] = zst.name;
				}
			}
			else
			{
				COUNT_DV idv_node;
				idv_node.descr = idv.name;
				idv_node.lwcb[*it_lst_lwcb] = cb->descr;
				if (exizdv)
				{
					idv_node.dv_lw[zdv.id] = zdv.name;
				}
				idv_node.st_id = ist.id;
				if (exizst)
				{
					idv_node.st_lw[zst.id] = zst.name;
				}
				map_dv_info[idv.id] = idv_node;
			}
		}

		if (idv.id == zdv.id)
		{
			continue;
		}
		if (exizdv)
		{
			it_map_dv = map_dv_info.find(zdv.id);
			if (it_map_dv != map_dv_info.end())
			{
				it_map_dv->second.lwcb[*it_lst_lwcb] = cb->descr;
				if (exiidv)
				{
					it_map_dv->second.dv_lw[idv.id] = idv.name;
				}

				if (exiist)
				{
					it_map_dv->second.st_lw[ist.id] = ist.name;
				}
			}
			else
			{
				COUNT_DV zdv_node;
				zdv_node.descr = zdv.name;
				zdv_node.lwcb[*it_lst_lwcb] = cb->descr;
				if (exiidv)
				{
					zdv_node.dv_lw[idv.id] = idv.name;
				}
				zdv_node.st_id = zst.id;
				if (exiist)
				{
					zdv_node.st_lw[ist.id] = ist.name;
				}
				map_dv_info[zdv.id] = zdv_node;
			}
		}
	}

	char temp_cb[500];
	char temp_cb_des[500];
	char temp_st[500];
	char temp_st_des[500];
	char temp_dv[500];
	char temp_dv_des[500];
	char temp[500];
	for (it_map_dv = map_dv_info.begin(); it_map_dv != map_dv_info.end(); it_map_dv++)
	{
		for(map<long, string>::iterator it_cb = it_map_dv->second.lwcb.begin(); it_cb != it_map_dv->second.lwcb.end(); it_cb++)
		{
			sprintf(temp, "%ld",it_cb->first);
			strcat(temp_cb, temp);
			strcat(temp_cb, ";");
			strcat(temp_cb_des, it_cb->second.c_str());
			strcat(temp_cb_des, ";");
		}
		for(map<long, string>::iterator it_st = it_map_dv->second.st_lw.begin(); it_st != it_map_dv->second.st_lw.end(); it_st++)
		{
			sprintf(temp, "%ld", it_st->first);
			strcat(temp_st, temp);
			strcat(temp_st, ";");
			strcat(temp_st_des, it_st->second.c_str());
			strcat(temp_st_des, ";");
		}
		for(map<long, string>::iterator it_dv = it_map_dv->second.dv_lw.begin(); it_dv != it_map_dv->second.dv_lw.end(); it_dv++)
		{
			sprintf(temp, "%ld", it_dv->first);
			strcat(temp_dv, temp);
			strcat(temp_dv, ";");
			strcat(temp_dv_des, it_dv->second.c_str());
			strcat(temp_dv_des, ";");
		}
		sprintf(sql,"UPDATE HISDB.DV_INFO SET CB=\'%s\', L_CB_ST=\'%s\', L_CB_DV=\'%s\', CB_DESCR=\'%s\', L_CB_ST_DESCR=\'%s\', L_CB_DV_DESCR=\'%s\'  WHERE ID=\'%ld\'", temp_cb, temp_st, temp_dv, temp_cb_des, temp_st_des, temp_dv_des, it_map_dv->first);
		ExecSQL(sql);

		memset(temp_cb, 0, 500);
		memset(temp_st, 0, 500);
		memset(temp_dv, 0, 500);
		bzero(temp_cb_des, 500);
		bzero(temp_st_des, 500);
		bzero(temp_dv_des, 500);
	}

	return true;
}

//ͳ�Ƹ��ɵĹ��ܣ�ͳ�Ƹ��ɵĳ�վ��ʵ�ʳ�վ�����ߣ����翪�ص�
bool  Cntld::count_ld()
{
	map<long, COUNT_LD> map_ld_info;
	map<long, COUNT_LD>::iterator it_map_ld_info;
	PSBOB::DV dv = { 0 };
	int i = 0, br, ind, znd;
	ChildPtr *br_p, *br_p0;
	char lwcb_id[MAX_CB_ID_LEN] = { 0 };
	if (!update_cb_state() || !add_so())
	{
		cout << "���¿���ң��״̬���Զ���ӵ�Դʧ�ܣ�" << endl;
		return false;
	}
	lv_so = dsbob->so_tb->GetCount();	//��Դ����
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();
	CLS_FDIR_ZONE* fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB* fdir_cb = fdir_cb_tb->GetTable();
	CLS_LD *ld;
	int lv_ld = dsbob->ld_tb->GetCount();
	CLS_CB *cb;
	int lv_fdir_cb = fdir_cb_tb->GetCount();
	int lv_fdir_zone = fdir_zone_tb->GetCount();
	int izone;

	//�����е���������Ϊδ����״̬
	for (i = 0; i < lv_fdir_cb; i++)
	{
		(fdir_cb + i)->q |= FDIR_CB_UNPROC;
	}
	for (i = 0; i < lv_fdir_zone; i++)
	{
		(fdir_zone + i)->q |= FDIR_ZONE_UNPROC;
	}

	//�������еĵ�Դ���ӵ�Դ��ʼ�������ɣ����ҹ���ĸ���
	for (i = 0; i < lv_so; i++)
	{
		ind = dsbob->so_tb->Data[i].ind;
		if (dsbob->nd_tb->Data[ind].r_cb == -1)
		{
			continue;
		}
		//ÿ����Դ�����Ӷ�����߿��أ������պϵĽ��߿��أ���������
		br_p = dsbob->r_nd_cb_tb->FindChildPtr(dsbob->nd_tb->Data[ind].r_cb);
		br_p0 = br_p;

		while (br_p)
		{
			br = br_p->Ptr;
			if (br == -1)
			{
				br_p = br_p->Next;
				continue;
			}
			cb = dsbob->cb_tb->GetDataByPos(br);

			if (cb->q & CB_OPEN)
			{
				cout << cb->id << "���ضϿ���" << endl;
			}
			else if ((fdir_cb + br)->q & FDIR_CB_UNPROC)
			{
				cout << cb->id << "���رպϣ�" << endl;
				(fdir_cb + br)->q &= ~FDIR_CB_UNPROC;
				if ((fdir_zone + (fdir_cb + br)->ind)->type == _FDIR_ZN)
				{
					izone = (fdir_cb + br)->ind;
					(fdir_zone + (fdir_cb + br)->iznd)->q &= ~FDIR_ZONE_UNPROC;
				}
				else
				{
					izone = (fdir_cb + br)->iznd;
					(fdir_zone + (fdir_cb + br)->ind)->q &= ~FDIR_ZONE_UNPROC;
				}
				if ((fdir_zone + izone)->q & FDIR_ZONE_UNPROC)
				{
					(fdir_zone + izone)->q &= ~FDIR_ZONE_UNPROC;
					search_ld(map_ld_info, izone, NULL, 0);
				}
			}
			else
			{
			}
			br_p = br_p->Next;
		}
		FreeChildPtr(br_p0);
	}

	//��û�й���ĺ͹���ĸ��ɶ�ͳ�Ƴ�����û�й���ĸ����޷�ͳ��ʵ�ʹ��糧վ
	for (i = 0; i < lv_ld; i++)
	{
		//�Ƿ��ǹ���ĸ���
		it_map_ld_info = map_ld_info.find(dsbob->ld_tb->Data[i].id);
		if (it_map_ld_info != map_ld_info.end())
		{
			id_to_pid(it_map_ld_info->second.id);
			sprintf(sql, "INSERT INTO PMS_PARAM.LD_INFO (ID,ST_ID,DV_ID) VALUES (\'%ld\',\'%ld\',\'%ld\') ", it_map_ld_info->second.id, it_map_ld_info->second.st, it_map_ld_info->second.dv_id);
			ExecSQL(sql);
			sprintf(sql, "UPDATE PMS_PARAM.LD_INFO SET REAL_ST=\'%ld\', CB_ID=\'%s\' WHERE ID=\'%ld\'", it_map_ld_info->second.rel_st, it_map_ld_info->second.lwcb_id.c_str(), it_map_ld_info->second.id);
			ExecSQL(sql);
			printf("the ld is %ld st %ld real st %ld lwcb %s\n", it_map_ld_info->second.id, it_map_ld_info->second.st, it_map_ld_info->second.rel_st, it_map_ld_info->second.lwcb_id.c_str());
			continue;
		}
		else
		{
			COUNT_LD ld_node;
			ld_node.id = dsbob->ld_tb->Data[i].id;
			ld_node.descr = dsbob->ld_tb->Data[i].descr;
			if (GetDv(ld_node.id, "ld", dv) >= 0)
			{
				ld_node.dv_id = dv.id;
			}
			ld_node.st = dsbob->ld_tb->Data[i].stid;
			id_to_pid(ld_node.id);
			id_to_pid(ld_node.st);
			sprintf(sql, "INSERT INTO PMS_PARAM.LD_INFO (ID,ST_ID,DV_ID) VALUES (\'%ld\',\'%ld\',\'%ld\') ", ld_node.id, ld_node.st, ld_node.dv_id);
			ExecSQL(sql);
			sprintf(sql, "UPDATE PMS_PARAM.LD_INFO SET REAL_ST=\'\', CB_ID=\'\' WHERE ID=\'%ld\'", ld_node.id);
			ExecSQL(sql);
			printf("the ld is %ld dv %ld \n", ld_node.id, ld_node.dv_id);
			//map_ld_info[ld_node.id] = ld_node;
		}
	}

	return true;
}

//�������ɵĵݹ麯����st_idΪʵ�ʵĹ��糧վ��lwcb_idΪ���羭�������翪�أ�izoneΪ����������
void  Cntld::search_ld(map<long, COUNT_LD> &ld_info, int izone, char* lwcb_id, long st_id)
{
	int icb, nextzone;
	CLS_FDIR_ZONE *fdir_zone = fdir_zone_tb->GetTable();
	CLS_FDIR_CB *fdir_cb = fdir_cb_tb->GetTable();
	CLS_FDIR_ZONE_CB* fdir_zone_cb = fdir_zone_cb_tb->GetTable();

	ChildPtr *ptr1 = NULL, *ptr2 = NULL;
	CLS_ZN *zn = NULL;
	CLS_CB *cb = NULL;
	PSBOB::DV dv = { 0 };
	PSBOB::ST st = { 0 };

	//��Ҫ���ҹ�����LD
	zn = dsbob->zn_tb->Find(CLS_ZN((fdir_zone + izone)->zn_id));
	//����ZN���е�r_ld�ҵ�r_ZN_LD���еĵ���ؼ�¼�������ж�����¼��
	ptr1 = dsbob->r_zn_ld_tb->FindChildPtr(zn->r_ld);

	ptr2 = ptr1;

	while (ptr1)
	{
		//����r_ZN_LD���е�Ld_id���ҵ�LD������ؼ�¼��

		CLS_LD *ld = dsbob->ld_tb->GetDataByPos(ptr1->Ptr);
		//cout<<"***"<<ld->id<<endl;
		COUNT_LD ld_node;
		ld_node.id = ld->id;
		ld_node.descr = ld->descr;
		ld_node.st = ld->stid;
		id_to_pid(ld_node.st);
		if (GetDv(ld->id, "ld", dv) >= 0)
		{
			ld_node.dv_id = dv.id;
		}
		else
		{
			ld_node.dv_id = 0;
		}

		//ʵ�ʹ��糧վ���ΪNULL����ĳ���µ����ߣ�Ҫ���»�ȡ���߶�Ӧ�ĵ�Դ��վ
		if (st_id == 0)
		{
			if(GetStbyDv(dv.id, st) == 0)
			{
				ld_node.rel_st = st.id;
				st_id = st.id;
			}
		}
		else
		{
			ld_node.rel_st = st_id;
		}
		if (lwcb_id != NULL)
		{
			ld_node.lwcb_id = lwcb_id;
		}
		ld_info[ld_node.id] = ld_node;

		ptr1 = ptr1->Next;
	}

	FreeChildPtr(ptr2);
	ptr1 = NULL;
	ptr2 = NULL;
	zn = NULL;

	//������߿������ڵ����β����ڸ��ɣ���ô�������εĿ��ػ�ȡst
	if (st_id == 0)
	{
		GetDVByZone(izone, dv);
		if(dv.id != 0)
		{
			if (GetStbyDv(dv.id, st) == 0)
			{
				st_id = st.id;
			}
		}
	}
//	cout << izone << endl;
	//��������������������п���
	for (int i = 0; i < (fdir_zone + izone)->cbnum; i++)
	{
		icb = (fdir_zone_cb + (fdir_zone + izone)->cbptr + i)->icb; //�˴���icbΪ���ص�����ţ���FDIR_ZONE_CB����
		cb = dsbob->cb_tb->Find(CLS_CB((fdir_cb + icb)->cb_id));

		//ֻ����պϵĿ���
		if (cb->q & CB_OPEN)
		{
			(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
			continue;
		}
		//�жϿ�����û�б������
		if((fdir_cb + icb)->q & FDIR_CB_UNPROC)
		{
			//cout<<"^^^^^^^^^^^^^^^^^^^^^^^^***"<<cb->id<<lwcb_id<<endl;
			(fdir_cb + icb)->q &= ~FDIR_CB_UNPROC;
			//�Ѿ����ıպϵ����翪��ͳ�Ƶ�������
			if (cb->q & CB_LWCB)
			{
				char tmp_lwcb[MAX_CB_ID_LEN*10]={ 0 };
				id_to_pid(cb->id);
				if (lwcb_id != NULL)
				{
					sprintf(tmp_lwcb, "%ld;", cb->id);
					strcat(tmp_lwcb, lwcb_id);
				}
				else
				{
					sprintf(tmp_lwcb, "%ld", cb->id);
				}
				if (izone == (fdir_cb + icb)->ind)
				{
					nextzone = (fdir_cb + icb)->iznd;
				}
				else
				{
					nextzone = (fdir_cb + icb)->ind;
				}
				if ((nextzone == -1) || (fdir_zone + nextzone)->type == _FDIR_SO)
				{
					continue;
				}
				else
				{
					if((fdir_zone + nextzone)->q & FDIR_ZONE_UNPROC)
					{
						(fdir_zone + nextzone)->q &= ~FDIR_ZONE_UNPROC;
						search_ld(ld_info, nextzone, tmp_lwcb, st_id);
					}
				}

			}
			else
			{
				if (izone == (fdir_cb + icb)->ind)
				{
					nextzone = (fdir_cb + icb)->iznd;
				}
				else
				{
					nextzone = (fdir_cb + icb)->ind;
				}
				if ((nextzone == -1) || (fdir_zone + nextzone)->type == _FDIR_SO)
				{
					continue;
				}
				else
				{
					if((fdir_zone + nextzone)->q & FDIR_ZONE_UNPROC)
					{
						(fdir_zone + nextzone)->q &= ~FDIR_ZONE_UNPROC;
						search_ld(ld_info, nextzone, lwcb_id, st_id);
					}
				}

			}

		}
	}
}




