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

	long 	id;	//故障指示器的ID
	long 	equ_id;//该故障指示器关联的设备ID（线段或开关ID）
	EQU_TYPE equ_type;
	void *pdata;//保存设备相关信息，如cls_cb，cls_sec
	bool 	bfault;//true：该故障指示器发生故障，false:没发生故障
	int 	dist;//该结点到根结点的距离
	int		ind;//电流流入的结点号
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
	int q; //是否已经设置过dsbob对应开关的状态
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
	list<_DATA_NODE_ZN> lst_zones;	//转供的区段列表
	list<_DATA_NODE_ZN> lst_cut_zones;	//消减负荷区段列表
}TreeData;

//联络开关的id作为key
typedef map<long, TreeData> MAP_SO_NODE;

class FDIR_R2: public FHZG
{
private:
	long m_sz_sec;	//人机推图后将会把这条线段放在画面中心位置

public:
	FDIR_R2(DSBOB *bob) :
			FHZG(bob)
	{
		m_sz_sec = -1;
	}
	virtual ~FDIR_R2();

	int Init();	//初始化，包括更新开关状态、生成电源、形成FDIR_DB库、生成区段的电流流入开关

	//获取故障馈线的开关信息，主要是开关id，point的id，开关开合状态,保存开关状态，数据校验时使用
	void Get_Fdir_Cbs(list<CLS_CB> &lst_cb_info);

	int FaultDetection();
	int ExecFdir();
	int ExecFdir(long id, const char *table);
	int ReIsolate(const long cb, const char* fault_id);

	//上游隔离开关隔离失败时调用此函数，重新获取上游隔离所在的区段,重新计算时使用
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

	//根据进线开关建立树结构，为重新计算隔离使用，数据校验发现策略失效，用户选择重新计算时调用此函数把数据恢复
	void ReBuildFaultTree(const char* fault_id);

	//恢复故障的起始和结束的设备，数据校验发现策略失效，用户选择重新计算时调用此函数把数据恢复
	void ReCoHeadTail(const char* fault_id);

	//农牧区定位函数中使用，详细定位故障开始和结束的设备
	void fdir_detect_more(TreeNode<FDIR_TREE_NODE_DATA>* node);

	//农牧区故障隔离函数
	int FormIsoSteps();

	//农牧区故障隔离中调用，递归查找能够隔离的开关
	void GetIsoCbs(TreeNode<FDIR_TREE_NODE_DATA> *node, list<int> &lst_cb, list<TreeNode<FDIR_TREE_NODE_DATA> *> &lst_node);

	//农牧区清除故障所在的馈线树，暂时没有调用
	void clear_tree();

	//母线负荷转供
	int FHZG_Bus(long bus_id, list<list<MAP_SO_NODE> > &fdir_steps, list<long> &lstIsolate);

	inline long getfaultsec()
	{
		return m_sz_sec;
	}

	inline string getfaultst()
	{
		//获取故障所在的厂站
		char zn_id[MAX_ZN_ID_LEN] = { 0 };

		if (oodbread_aa(zn_id, "SEC_id", (void*) &m_sz_sec, "ZN_id", const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_SEC")), sizeof(zn_id)) <= 0)
		{
			printf("%s(%d):读取r_zn_sec表sec_id为%ld对应的zn_id失败，错误号：%d\r\n", __FUNCTION__, __LINE__, m_sz_sec, _oodb_errno);
			return FDIR_R_FAIL;
		}

		char st_id[MAX_ST_ID_LEN] = { 0 };

		if (oodbread_aa(st_id, "ZN_id", zn_id, "ST_id", const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_ZN")), sizeof(st_id)) <= 0)
		{
			printf("%s(%d):读取r_st_zn表zn_id为%s对应的st_id失败，错误号：%d\r\n", __FUNCTION__, __LINE__, zn_id, _oodb_errno);
			return FDIR_R_FAIL;
		}

		return st_id;
	}

protected:
	//获取线段上的故障指示器所在区段，暂时没有使用
	int GetZoneOfFI(const long id);
//	int FaultDetection(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_other_fault);
//	void fdir_detect_more(TreeNode<FDIR_TREE_NODE_DATA>* node);

	//农牧区查找以father为根节点的故障开始的故障指示器或者开关，定位故障开始的设备，并保持在lst_fault_node中，在定位时调用
	void find_fault_node(TreeNode<FDIR_TREE_NODE_DATA>* father, list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_fault_node);

	//判断故障信号是否为误报，农牧区暂时去掉了误报的判断
	bool is_wrong_fi(const char *fi_id);

	//暂时没有使用
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
extern int calc_restore_step(list<FDIR_R2*>& lst_fdir_r, list<list<MAP_SO_NODE> > &lst_step, bool bStation = true/*是否主站全遥控方案*/);
extern int re_calc_restore_step(const char *group_id, list<FDIR_R2*>& lst_fdir_r, list<list<MAP_SO_NODE> > &lst_steps);
#endif /* FDIRR2_H_ */
