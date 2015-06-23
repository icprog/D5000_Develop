/*
 * FDIRR2.h
 *
 *  Created on: 2012-4-27
 *      Author: BetaKDE
 */

#ifndef FDIRR2_H_
#define FDIRR2_H_

#include "FHZG.h"
#include <stack>
#include <map>
#include "Tree.h"
#include "warn_define.h"

using namespace std;

enum EQU_TYPE
{
	EQU_CB, EQU_SEC, EQU_LD
};



typedef struct DATA_FI
{
	DATA_FI()
	{
	}

	DATA_FI(const long sz_fi_id, const long sz_equ_id, EQU_TYPE type,int iind,void *pvalue)
	{
		id = sz_fi_id;
		equ_id = sz_equ_id;
		equ_type = type;
		bfault = false;
		dist = 0;
		ind = iind;
		pdata = pvalue;
	}

	long 	id;	//����ָʾ����ID
	long 	equ_id;//�ù���ָʾ���������豸ID���߶λ򿪹�ID��
	EQU_TYPE equ_type;
	void *pdata;//�����豸�����Ϣ����cls_cb��cls_sec
	bool 	bfault;//true���ù���ָʾ���������ϣ�false:û��������
	int 	dist;//�ý�㵽�����ľ���
	int		ind;//��������Ľ���
}DATA_FI;


#define FDIR_STEP_PROCESS		1
#define FDIR_STEP_PROCESSED		2

typedef enum
{
	CUT_LD, RESTORE
} FDIR_STEP_STAGE;

typedef struct FDIR_STEP
{
	FDIR_STEP(const long cb_id, const bool b_cb_open, const FDIR_STEP_STAGE fstage, const int status = 0)
	{
		cb = cb_id;
		bOpen = b_cb_open;
		q = status;
		stage = fstage;
	}

	FDIR_STEP_STAGE stage;
	long cb;
	bool bOpen;
	int q; //�Ƿ��Ѿ����ù�dsbob��Ӧ���ص�״̬
} FDIR_STEP, *PFDIR_STEP;

ostream& operator<<(ostream& os, const DATA_FI data);

typedef struct TreeData
{
	TreeData()
	{
		lst_zones.clear();
		lst_steps.clear();
		lst_cut_zones.clear();
	}

	TreeData(list<FDIR_STEP> steps, list<_DATA_NODE_ZN> zones, list<_DATA_NODE_ZN> cut_zones, _DATA_NODE_SO node_so)
	{
		lst_zones = zones;
		lst_steps = steps;
		data_so = node_so;
		lst_cut_zones = cut_zones;
	}
	list<FDIR_STEP> lst_steps;
	_DATA_NODE_SO data_so;
	list<_DATA_NODE_ZN> lst_zones;	//ת���������б�
	list<_DATA_NODE_ZN> lst_cut_zones;	//�������������б�
}TreeData;

//���翪�ص�id��Ϊkey
typedef map<long, TreeData> MAP_SO_NODE;

class FDIR_R2: public FHZG
{
private:
	long m_sz_sec;	//�˻���ͼ�󽫻�������߶η��ڻ�������λ��

public:
	FDIR_R2(DSBOB *bob) :
			FHZG(bob)
	{
		m_sz_sec = -1;
	}
	virtual ~FDIR_R2();

	int Init();	//��ʼ�����������¿���״̬�����ɵ�Դ���γ�FDIR_DB�⡢�������εĵ������뿪��

	//��ȡ�������ߵĿ�����Ϣ����Ҫ�ǿ���id��point��id�����ؿ���״̬,���濪��״̬������У��ʱʹ��
	void Get_Fdir_Cbs(list<CLS_CB> &lst_cb_info);

	int FaultDetection();
	int ExecFdir();
	int ExecFdir(long id, const char *table);
	int ReIsolate(const long cb, const char* fault_id);

	//���θ��뿪�ظ���ʧ��ʱ���ô˺��������»�ȡ���θ������ڵ�����,���¼���ʱʹ��
	int Findupfaultzone();
/*

	inline string getfaultst()
	{
		assert(fdir_isolate_tb->GetCount() > 0);

		CLS_CB *cb = dsbob->cb_tb->GetTable();
		int icb = fdir_isolate_tb->GetDataByPos(0)->icb;

		return (cb + icb)->stid;
	}
*/

	//���ݽ��߿��ؽ������ṹ��Ϊ���¼������ʹ�ã�����У�鷢�ֲ���ʧЧ���û�ѡ�����¼���ʱ���ô˺��������ݻָ�
	void ReBuildFaultTree(const char* fault_id);

	//�ָ����ϵ���ʼ�ͽ������豸������У�鷢�ֲ���ʧЧ���û�ѡ�����¼���ʱ���ô˺��������ݻָ�
	void ReCoHeadTail(const char* fault_id);

	//ũ������λ������ʹ�ã���ϸ��λ���Ͽ�ʼ�ͽ������豸
	void fdir_detect_more(TreeNode<FDIR_TREE_NODE_DATA>* node);

	//ũ�������ϸ��뺯��
	int FormIsoSteps();

	//ũ�������ϸ����е��ã��ݹ�����ܹ�����Ŀ���
	void GetIsoCbs(TreeNode<FDIR_TREE_NODE_DATA> *node, list<int> &lst_cb, list<TreeNode<FDIR_TREE_NODE_DATA> *> &lst_node);

	//ũ��������������ڵ�����������ʱû�е���
	void clear_tree();

	//ĸ�߸���ת��
	int FHZG_Bus(long bus_id, list<list<MAP_SO_NODE> > &fdir_steps, list<long> &lstIsolate);

	inline long getfaultsec()
	{
		return m_sz_sec;
	}

	inline string getfaultst()
	{
		//��ȡ�������ڵĳ�վ
		char zn_id[MAX_ZN_ID_LEN] = { 0 };

		if (oodbread_aa(zn_id, "SEC_id", (void*) &m_sz_sec, "ZN_id", const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_SEC")), sizeof(zn_id)) <= 0)
		{
			printf("%s(%d):��ȡr_zn_sec��sec_idΪ%ld��Ӧ��zn_idʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, m_sz_sec, _oodb_errno);
			return FDIR_R_FAIL;
		}

		char st_id[MAX_ST_ID_LEN] = { 0 };

		if (oodbread_aa(st_id, "ZN_id", zn_id, "ST_id", const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_ZN")), sizeof(st_id)) <= 0)
		{
			printf("%s(%d):��ȡr_st_zn��zn_idΪ%s��Ӧ��st_idʧ�ܣ�����ţ�%d\r\n", __FUNCTION__, __LINE__, zn_id, _oodb_errno);
			return FDIR_R_FAIL;
		}

		return st_id;
	}

protected:
	//��ȡ�߶��ϵĹ���ָʾ���������Σ���ʱû��ʹ��
	int GetZoneOfFI(const long id);
//	int FaultDetection(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_other_fault);
//	void fdir_detect_more(TreeNode<FDIR_TREE_NODE_DATA>* node);

	//ũ����������fatherΪ���ڵ�Ĺ��Ͽ�ʼ�Ĺ���ָʾ�����߿��أ���λ���Ͽ�ʼ���豸����������lst_fault_node�У��ڶ�λʱ����
	void find_fault_node(TreeNode<FDIR_TREE_NODE_DATA>* father, list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_fault_node);

	//�жϹ����ź��Ƿ�Ϊ�󱨣�ũ������ʱȥ�����󱨵��ж�
	bool is_wrong_fi(const char *fi_id);

	//��ʱû��ʹ��
	void FindDownCbbySec(int ind, bool *bvisited, bool *bvisited_nd);

	int FormResSteps();
};



extern DSBOB *g_bob;

ostream& operator <<(ostream &os, const TreeData &data);
extern int ExecSQL(const char *sql);
extern int GetStbyDv(const char *dvid, PSBOB::ST &st);
extern int GetDv(const char *devid, const char *devtb, PSBOB::DV &dv);
extern void fill_fhzg_db(const long cb_id, const char *fhzg_id, FHZG *fhzg, bool has_fhzg_cb);
extern int GroupFault(list<FDIR_TASK> &lst_task);
extern int calc_restore_step(list<FDIR_R2*>& lst_fdir_r, list<list<MAP_SO_NODE> > &lst_step, bool bStation = true/*�Ƿ���վȫң�ط���*/);
extern int re_calc_restore_step(const char *group_id, list<FDIR_R2*>& lst_fdir_r, list<list<MAP_SO_NODE> > &lst_steps);
#endif /* FDIRR2_H_ */
