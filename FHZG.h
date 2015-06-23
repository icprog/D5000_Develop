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

#define DEV_STATUS_UNDEFINE	0	//对应的状态在D5000库里还暂未定义，等定义好后再替换

using namespace std;

#define _NODE_ZN 0
#define _NODE_SO 1

#define FHZG_TYPE_ALL	1	//独立转供
#define FHZG_TYPE_SHARE	2	//分摊转供

#define FHZG_FAIL_NO_SO			-1		//没有电源可供转供
#define FHZG_FAIL_SO_FULL		-2		//所有可消减的负荷消减完后，电源剩余容量仍不够
#define FHZG_FAIL_NO_IVALUE		-3		//没有获取到要转供的开关的电流值

#define FHZG_CAPACTIY_K 0.8 //电源容量的阈度系数，计算电源剩余容量时使用

#define FHZG_LD_DV 0 //0:负荷方案
#define FHZG_CTRL_I 1 //1：电流方案(遥控开关为区段标志，需要精简树)
#define FHZG_MIN_I 2 //2:电流方案（不能遥控的开关的电流值从线段上读取）

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

	float ldamount; //负荷总数
	list<CLS_LD> ld_list;
//	list<CLS_ZN> zn_list;//区段链表，田艳青添加于2012年6月14日
	map<int ,list<CLS_LD> > map_zn_ld;
	long zn_id; //区段id
	long cb_id; //电流流入开关ID
	int izone;//
	int prio; //优先级
	STATUS_PROTECT protect;//是否需要保电，保电为true，否则为false
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

	float full_capacity; //总容量
	float left_capacity; //剩余容量
	float take_ld_scale;//按比例应承担的负荷数
	long cb_id;
	long so_name;
	char so_descr[MAX_SO_DESCR_LEN];
	long dv_id;//电源的进线开关所在的馈线id
	float org_wloss;	//转供前该电源的网损
	float new_wloss;	//如果用该电源转供时的网损
	list<long>	lst_zones;//从联络开关到电源经过的区段
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

	int type;//类型（ZN，SO）
	int dist; //该电源到检修开关间的距离
	int cnt_node;//该电源到检修开关间的结点数
//	int flag;	//结点状态标志
	_DATA_NODE_ZN node_zn;
	_DATA_NODE_SO node_so;
}DataType;

ostream& operator <<(ostream &os, const DataType &data);

typedef list<TreeNode<DataType>*> LIST_TREENODE;
class FHZG: public FDIR_R
{
private:
	long		fhzg_cb;	//做负荷转供的开关ID
//	PWRFLOW		*pwrflow;
	TP			*tp;
	Tree<DataType>* m_pTree;

	int zg_type;//0:负荷方案，1：电流方案(遥控开关为区段标志，需要精简树),2:电流方案（不能遥控的开关的电流值从线段上读取）tianyq 2012,6,14
	int m_Type; //0:转供失败, 1:独立转供 2:分摊转供
	map<TreeNode<DataType>*, LIST_TREENODE> m_map_so_zn;
	map<long, Tree<DataType> > m_map_cb_tree;
	map<long, float> zone_value_map;//区段和电流值的映射
	LIST_TREENODE so_list; //可供电的电源列表
	LIST_TREENODE down_zn_list; //所有需要转供的区段列表
	LIST_TREENODE sel_so_list; //被选中用来分摊的电源列表
	LIST_TREENODE so_list_swp; //将so_list中的数据拷贝到so_list_swp中
	LIST_TREENODE::iterator so_pos;
	list<_OPERATION_STEP> m_operations; //负荷转供的操作步骤
	list<long> m_cut_operations;//减负荷需要断开的开关列表

	LIST_TREENODE cut_leaf_list;//被减掉的负荷列表
	LIST_TREENODE lst_so_removed; //一个区段有多个电源时被移除掉的电源
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
	//获取开关的电流值
	float GetCbIVlaue(long cb_id);

	//获取电源树的电源的容量
	float GetFullcap(int izone);

	//区段根据电流重新生成树tianyq添加于2012年6月14日
	void ReBuildTree(TreeNode<DataType> *father, Tree<DataType> *pTree, TreeNode<DataType> *old_father, Tree<DataType> *old_pTree);

	//遍历区段，形成树
	void ErgodicTree(int p, TreeNode<DataType> *father, Tree<DataType> *pTree,  bool b_so_tree, LIST_TREENODE &main_tree, map<long, int> &m_so_zone);

	//获取树上区段上的负荷
	void GetZoneLd(LIST_TREENODE tree_list, bool is_so_tree);

	//建立库
	int BuildDB();

	//以联络开关开始，遍历找到电源所在区段，然后调用ErgodicTree形成一棵树
	int GetLwcbSo(int icb, DataType &node_so);

	//根据人机返回的开关，找到上游区段id
	int GetDownZoneId(const long cb);

	//得到上游区段id，与FindUpZone类似，添加了区段处理判断，为了防止联络开关找上游区段会有两个结果
	int GetUpZone(int icb);

	//树的输出
	void displayTree(LIST_TREENODE listcoll);

	//找到下游区段id
	int FindDownZone(int icb);

	//统计一棵树中的负荷总数
	float LdSum(Tree<DataType>* pTree, TreeNode<DataType>* pNode = NULL);

	//负荷转供分摊算法
	bool LdTransfer(Tree<DataType>* pTree);

	//有电源符合单独供电
	void LdByOne(float ldsum, LIST_TREENODE &lstSO);

	//多个电源供电
	float MultiSo(float ldsum);

	//选择供电电源
	float SelectSo(LIST_TREENODE &solist, float ldsum);

	/////////////打印指定区段的边界开关
	void DisplayZoneCB(int izone);

	//保存两个区段之间不带电源的节点
	void NodeOfRegion(TreeNode<DataType>* node1, TreeNode<DataType>* node2, LIST_TREENODE &nodelist);

	//判断树中是否存在电源
	bool JudgeSo(LIST_TREENODE &temlist);

	bool GetLD(Tree<DataType> *pTree, TreeNode<DataType>* node1, TreeNode<DataType>* node2, float &num, float offset, LIST_TREENODE &lstGet);

	//判断减掉所有的叶子节点的负荷，是否满足负荷消减的要求
	float JudgeLeafLD(float ldsum, int pri, LIST_TREENODE &leaflist);

	//找到最适合的组合
	void GetCombination(TreeNode<DataType>* nums[], int cnt_nums, int cnt_need, float ldsum, TreeNode<DataType>* result[], LIST_TREENODE &result_ld);

	//计算列表中的电源到检修开关的距离
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

	//返回被消减掉的负荷列表
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

	//返回分摊转供时电源对应的转供区段
	const map<TreeNode<DataType>*, LIST_TREENODE> *GetSoFhzgZone()
	{
		return &m_map_so_zn;
	}

	//返回操作步骤列表
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

	//返回转供类型
	int GetType()
	{
		return m_Type;
	} //0:转供失败, 1:独立转供 2:分摊转供

	//获取指定区段的边界开关
	void GetZoneCB(int izone, list<long> &lst_cb);

	//返回需要转供的区段列表
	const LIST_TREENODE * GetDownZones()
	{
		return &down_zn_list;
	}

	//获取提供独立或分摊转供的电源列表
	const LIST_TREENODE * GetSO();

	//根据电源ID获取该电源分摊或独立转供的树节点列表
	const LIST_TREENODE * GetZoneBySO(const long so_dv);

	//负荷转供执行函数
	int Do(const long cb_id, bool bbuild_db = true);
};

#endif

//用于存储开关电流平均值
extern std::map<long,float> g_mapIValue;
extern pthread_mutex_t avg_i_mutex;