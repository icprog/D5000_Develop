/*
 * FHZG.h
 *
 *  Created on: 2012-3-12
 *      Author: Administrator
 */
#ifndef FHZG_H_
#define FHZG_H_
#include <list>
#include <stdlib.h>
//#include "dms.h"
#include "tmptable.h"
#include "fdirdb.h"
#include "FDIR_R.h"
#include "netmom.h"
#include "tmpodrtb.h"
#include <map>
#include <iostream>
#include <sstream>
#include "Tree.h"
//#include "pwrflow2.h"

#define DEV_STATUS_UNDEFINE	0	//��Ӧ��״̬��D5000���ﻹ��δ���壬�ȶ���ú����滻

using namespace std;

#define _NODE_ZN 0
#define _NODE_SO 1

#define FHZG_TYPE_ALL	1	//����ת��
#define FHZG_TYPE_SHARE	2	//��̯ת��

#define FHZG_FAIL_NO_SO			-1		//û�е�Դ�ɹ�ת��
#define FHZG_FAIL_SO_FULL		-2		//���п������ĸ���������󣬵�Դʣ�������Բ���
#define FHZG_FAIL_NO_IVALUE		-3		//û�л�ȡ��Ҫת���Ŀ��صĵ���ֵ

#define FHZG_CAPACTIY_K 0.8 //��Դ�������ж�ϵ���������Դʣ������ʱʹ��

#define FHZG_LD_DV 0 //0:���ɷ���
#define FHZG_CTRL_I 1 //1����������(ң�ؿ���Ϊ���α�־����Ҫ������)
#define FHZG_MIN_I 2 //2:��������������ң�صĿ��صĵ���ֵ���߶��϶�ȡ��

enum STATUS_PROTECT{NONE, LDPROTECT, CBPROTECT};

typedef struct _DATA_NODE_ZN
{
	_DATA_NODE_ZN()
	{
		ldamount = 0;
		ld_list.clear();
		zn_id =0;
		cb_id = 0;
		izone = -1;
		prio = 3;
		protect = NONE;
		map_zn_ld.clear();
	}

	const char *GetText()
	{
		string code;
		ostringstream ss;

		GetCode(cb_id, code);

		ss << code.c_str() << "(" << ldamount << ")";

		return ss.str().c_str();
	}

	float ldamount; //��������
	list<CLS_LD> ld_list;
//	list<CLS_ZN> zn_list;//�������������������2012��6��14��
	map<int ,list<CLS_LD> > map_zn_ld;
	long zn_id; //����id
	long cb_id; //�������뿪��ID
	int izone;//
	int prio; //���ȼ�
	STATUS_PROTECT protect;//�Ƿ���Ҫ���磬����Ϊtrue������Ϊfalse
} _DATA_NODE_ZN;


typedef struct _OPERATION_STEP
{
	long open_cb;
	long close_cb;
	long so_id;
}_OPERATION_STEP;

typedef struct _DATA_NODE_SO
{
	_DATA_NODE_SO()
	{
		full_capacity = 0;
		left_capacity = 0;
		take_ld_scale = 0;
		cb_id = 0;
		so_name = 0;
		so_descr[0] = 0;
		dv_id = 0;
		org_wloss = 0;
		new_wloss = 0;
		lst_zones.clear();
	}

	const _DATA_NODE_SO& operator =(const _DATA_NODE_SO &so_data)
	{
		full_capacity = so_data.full_capacity;
		left_capacity = so_data.left_capacity;
		take_ld_scale = so_data.take_ld_scale;
		cb_id = so_data.cb_id;
		so_name = so_data.so_name;
		strcpy(so_descr, so_data.so_descr);
		dv_id = so_data.dv_id;
		org_wloss = so_data.org_wloss;
		new_wloss = so_data.new_wloss;
		lst_zones = so_data.lst_zones;

		return *this;
	}

	const char *GetText()
	{
		string code;
		ostringstream ss;

		GetCode(cb_id, code);

		ss << code.c_str() << "(" << left_capacity << ")";

		return ss.str().c_str();
	}

	float full_capacity; //������
	float left_capacity; //ʣ������
	float take_ld_scale;//������Ӧ�е��ĸ�����
	long cb_id;
	long so_name;
	char so_descr[MAX_SO_DESCR_LEN];
	long dv_id;//��Դ�Ľ��߿������ڵ�����id
	float org_wloss;	//ת��ǰ�õ�Դ������
	float new_wloss;	//����øõ�Դת��ʱ������
	list<long>	lst_zones;//�����翪�ص���Դ����������
} _DATA_NODE_SO;

typedef struct DataType
{
	DataType()
	{
		dist = 0;
		cnt_node = 0;
//		flag = 0;
		type = 0;
	}

	const char* GetText()
	{
		if (type == _NODE_ZN)
		{
			return node_zn.GetText();
		}
		else
		{
			return node_so.GetText();
		}
	}

	int GetType()
	{
		return type;
	}

	int type;//���ͣ�ZN��SO��
	int dist; //�õ�Դ�����޿��ؼ�ľ���
	int cnt_node;//�õ�Դ�����޿��ؼ�Ľ����
//	int flag;	//���״̬��־
	_DATA_NODE_ZN node_zn;
	_DATA_NODE_SO node_so;
}DataType;

ostream& operator <<(ostream &os, const DataType &data);

typedef list<TreeNode<DataType>*> LIST_TREENODE;
class FHZG: public FDIR_R
{
private:
	long		fhzg_cb;	//������ת���Ŀ���ID
//	PWRFLOW		*pwrflow;
	TP			*tp;
	Tree<DataType>* m_pTree;

	int zg_type;//0:���ɷ�����1����������(ң�ؿ���Ϊ���α�־����Ҫ������),2:��������������ң�صĿ��صĵ���ֵ���߶��϶�ȡ��tianyq 2012,6,14
	int m_Type; //0:ת��ʧ��, 1:����ת�� 2:��̯ת��
	map<TreeNode<DataType>*, LIST_TREENODE> m_map_so_zn;
	map<long, Tree<DataType> > m_map_cb_tree;
	map<long, float> zone_value_map;//���κ͵���ֵ��ӳ��
	LIST_TREENODE so_list; //�ɹ���ĵ�Դ�б�
	LIST_TREENODE down_zn_list; //������Ҫת���������б�
	LIST_TREENODE sel_so_list; //��ѡ��������̯�ĵ�Դ�б�
	LIST_TREENODE so_list_swp; //��so_list�е����ݿ�����so_list_swp��
	LIST_TREENODE::iterator so_pos;
	list<_OPERATION_STEP> m_operations; //����ת���Ĳ�������
	list<long> m_cut_operations;//��������Ҫ�Ͽ��Ŀ����б�

	LIST_TREENODE cut_leaf_list;//�������ĸ����б�
	LIST_TREENODE lst_so_removed; //һ�������ж����Դʱ���Ƴ����ĵ�Դ
public:
	list<DEV_STATUS>	lst_protected;
	float fhzg_capacity;
public:
	FHZG(DSBOB* pds) :
			FDIR_R(pds)
	{
		m_operations.clear();
		m_pTree = new Tree<DataType>;
//		pwrflow = new PWRFLOW(pds);
		tp = new TP(pds);
	}
	~FHZG()
	{
		delete m_pTree;
//		delete pwrflow;
		delete tp;
	}
protected:

	float GetMaxHisValue(char *command);
	//��ȡ���صĵ���ֵ
	float GetCbIVlaue(long cb_id);

	//��ȡ��Դ���ĵ�Դ������
	float GetFullcap(int izone);

	//���θ��ݵ�������������tianyq�����2012��6��14��
	void ReBuildTree(TreeNode<DataType> *father, Tree<DataType> *pTree, TreeNode<DataType> *old_father, Tree<DataType> *old_pTree);

	//�������Σ��γ���
	void ErgodicTree(int p, TreeNode<DataType> *father, Tree<DataType> *pTree,  bool b_so_tree, LIST_TREENODE &main_tree, map<long, int> &m_so_zone);

	//��ȡ���������ϵĸ���
	void GetZoneLd(LIST_TREENODE tree_list, bool is_so_tree);

	//������
	int BuildDB();

	//�����翪�ؿ�ʼ�������ҵ���Դ�������Σ�Ȼ�����ErgodicTree�γ�һ����
	int GetLwcbSo(int icb, DataType &node_so);

	//�����˻����صĿ��أ��ҵ���������id
	int GetDownZoneId(const long cb);

	//�õ���������id����FindUpZone���ƣ���������δ����жϣ�Ϊ�˷�ֹ���翪�����������λ����������
	int GetUpZone(int icb);

	//�������
	void displayTree(LIST_TREENODE listcoll);

	//�ҵ���������id
	int FindDownZone(int icb);

	//ͳ��һ�����еĸ�������
	float LdSum(Tree<DataType>* pTree, TreeNode<DataType>* pNode = NULL);

	//����ת����̯�㷨
	bool LdTransfer(Tree<DataType>* pTree);

	//�е�Դ���ϵ�������
	void LdByOne(float ldsum, LIST_TREENODE &lstSO);

	//�����Դ����
	float MultiSo(float ldsum);

	//ѡ�񹩵��Դ
	float SelectSo(LIST_TREENODE &solist, float ldsum);

	/////////////��ӡָ�����εı߽翪��
	void DisplayZoneCB(int izone);

	//������������֮�䲻����Դ�Ľڵ�
	void NodeOfRegion(TreeNode<DataType>* node1, TreeNode<DataType>* node2, LIST_TREENODE &nodelist);

	//�ж������Ƿ���ڵ�Դ
	bool JudgeSo(LIST_TREENODE &temlist);

	bool GetLD(Tree<DataType> *pTree, TreeNode<DataType>* node1, TreeNode<DataType>* node2, float &num, float offset, LIST_TREENODE &lstGet);

	//�жϼ������е�Ҷ�ӽڵ�ĸ��ɣ��Ƿ����㸺��������Ҫ��
	float JudgeLeafLD(float ldsum, int pri, LIST_TREENODE &leaflist);

	//�ҵ����ʺϵ����
	void GetCombination(TreeNode<DataType>* nums[], int cnt_nums, int cnt_need, float ldsum, TreeNode<DataType>* result[], LIST_TREENODE &result_ld);

	//�����б��еĵ�Դ�����޿��صľ���
	void CalcSODist(LIST_TREENODE &lstSO);

	float FindLeafNode(int pri, LIST_TREENODE &leaflist_tem);

	bool SelectCutLD(float ldsum, int max_pri);
public:
	const list<long> * GetLdCutOperations()
	{
		for (list<long>::iterator it = m_cut_operations.begin(); it != m_cut_operations.end(); it++)
		{
			id_to_pid(*it);
		}

		return &m_cut_operations;
	}

	//���ر��������ĸ����б�
	const LIST_TREENODE * GetLdCut()
	{
		return &cut_leaf_list;
	}

	const LIST_TREENODE* GetAllSO()
	{
		for (LIST_TREENODE::iterator it = so_list.begin(); it != so_list.end(); it++)
		{
			id_to_pid((*it)->data.node_so.so_name);
			id_to_pid((*it)->data.node_so.cb_id);
		}

		return &so_list;
	}

	//���ط�̯ת��ʱ��Դ��Ӧ��ת������
	const map<TreeNode<DataType>*, LIST_TREENODE> *GetSoFhzgZone()
	{
		return &m_map_so_zn;
	}

	//���ز��������б�
	const list<_OPERATION_STEP> *GetOperation()
	{
		for (list<_OPERATION_STEP>::iterator it = m_operations.begin(); it != m_operations.end(); it++)
		{
			id_to_pid(it->close_cb);
			id_to_pid(it->open_cb);
			id_to_pid(it->so_id);
		}

		return &m_operations;
	}

	//����ת������
	int GetType()
	{
		return m_Type;
	} //0:ת��ʧ��, 1:����ת�� 2:��̯ת��

	//��ȡָ�����εı߽翪��
	void GetZoneCB(int izone, list<long> &lst_cb);

	//������Ҫת���������б�
	const LIST_TREENODE * GetDownZones()
	{
		return &down_zn_list;
	}

	//��ȡ�ṩ�������̯ת���ĵ�Դ�б�
	const LIST_TREENODE * GetSO();

	//���ݵ�ԴID��ȡ�õ�Դ��̯�����ת�������ڵ��б�
	const LIST_TREENODE * GetZoneBySO(const long so_dv);

	//����ת��ִ�к���
	int Do(const long cb_id, bool bbuild_db = true);
};

#endif

//���ڴ洢���ص���ƽ��ֵ
extern std::map<long,float> g_mapIValue;
extern pthread_mutex_t avg_i_mutex;