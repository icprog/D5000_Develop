#ifndef TYPICAL_H_
#define TYPICAL_H_
//#include <oodb.h>
#include <vector>
#include <map>
#include "dsbob.h"
#include "Tp.h"
#include "tmptable.h"
#include "fdirdb.h"
#include "netmom.h"
#include "Nmd.h"
#include "comm.h"
#include "FDIRR2.h"
//#include <intr_oci.h>
#include "DBFDIR.h"
#include "Tree.h"

typedef enum{NORMAL_CB = 0, JXCB, LWCB} CB_TYPE;
typedef enum{TYPICAL_CLOSE = 0/*分闸*/, TYPICAL_OPEN/*合闸*/} CB_STATUS;
//开关信息
typedef struct
{
	long id; //开关ID
	float Ia; //A相电流值
	float Ib; // B相电流值
	float Ic; // C相电流值
	long dv_id;
	CB_STATUS status; //开关开、合状态0分闸，1合闸
	CB_TYPE type;	//开关类型
} CB_INFO, *PCB_INFPO;

//段信息
typedef struct _BAND_INFO
{
	_BAND_INFO()
	{
		valid = 1;
	}
	map<long, CB_INFO> map_band_cb; //段在当前方式下的边界开关信息string为开关的ID
	map<long, CB_INFO> map_band_cb_typical;//段在典型方式下的边界开关信息
	long so_cb; //当前方式下的供电进线开关
	long so_cb_typical; //典型方式下的供电进线开关
	int valid;//标志是否有效0无效，1有效,初始化为1
	long supply_cb; //该段当前方式的电流流入开关
	long supply_cb_typical; //该段典型方式下的电流流入开关
} BAND_INFO, *PBAND_INFO;

//恢复操作步骤
typedef struct _TYPICAL_STEP
{
//	_TYPICAL_STEP(const char *param_cb, const bool param_bopen, const bool param_boptional)
//	{
//		strcpy(cb, param_cb);
//		bopen = param_bopen;
//		boptional = param_boptional;
//	}

	_TYPICAL_STEP(const long param_cb, const bool param_bopen, const bool param_boptional)
	{
		cb = param_cb;
		bopen = param_bopen;
		boptional = param_boptional;
	}

	long cb; //开关ID
	bool bopen; //开关操作，分闸true,合闸false
	bool boptional; //该步操作是否为可选
} TYPICAL_STEP, *PTYPICAL_STEP;

typedef PBAND_INFO BAND_TREE_DATA;	//段树节点数据

typedef struct _BAND_TREE_POS
{
	_BAND_TREE_POS()
	{
		pNode = NULL;
		pTypicalNode = NULL;
	}

	TreeNode<BAND_TREE_DATA>  *pNode;	//在当前方式段树下的位置
	TreeNode<BAND_TREE_DATA>	*pTypicalNode;//在典型前方式段树下的位置
}BAND_TREE_POS, *PBAND_TREE_POS;

typedef struct _CB_BAND_POS
{
	_CB_BAND_POS()
	{
		iBand = NULL;
		zBand = NULL;
	}

	BAND_TREE_DATA iBand;
	BAND_TREE_DATA zBand;
}CB_BAND_POS, *PCB_BAND_POS;

//inline bool operator <(const BAND_TREE_DATA &left, const BAND_TREE_DATA &right);
//inline bool operator >(const BAND_TREE_DATA &left, const BAND_TREE_DATA &right);
inline ostream& operator <<(ostream &os, const CB_INFO &data);
inline ostream& operator <<(ostream &os, const BAND_TREE_DATA &data);

class typical
{
public:
	typical(FDIR_R2 *fdir_r2, const char* id);
	~typical();
	//保存典型运行方式,cb:人机发送的进线开关ID。成功返回true,失败返回false
	bool typical_save(const long cb_id, const char *machine, const char *userid);
	//恢复到典型运行方式,cb:人机发送的进线开关ID。成功返回true,失败返回false
	bool typical_restore(const long cb_id, const char* machine, const char *userid, string &id, time_t &restore_time);

	//获取某条馈线上的所有开关信息
	int get_dv_cb(int izone, const long fathercb, TreeNode<CB_INFO> *father, Tree<CB_INFO> &tree_cb);

	//获取开关的信息包括：三相电流值、开关类型、开关状态等
	bool get_cb_info(CB_INFO &cb);

	//把保存的m_map_dv_cb中的数据存入关系库
	bool typical_write_cb();

	//把树上结点形成sql语句，便于把数据写入库中
	void formsql_from_tree(const char* id,const long dv, TreeNode<CB_INFO> *child);

	//获取指定馈线的主干线开关,成功返回true,失败返回false
	bool typical_get_main_cb();

	bool typical_get_all_cb(const long cb_id);

	//从关系库表读取典型运行方式的开关信息并形成树
	bool typical_read_cb();

	//对馈线进行分段（按从进线开关往外进行排序）
	void typical_band();

	void tree_to_band(TreeNode<CB_INFO>* pParent, Tree<CB_INFO> &pTree, BAND_INFO &bandnode, list<BAND_INFO> &lst_band);

	void typical_deal_lwcb(map<long, CB_BAND_POS> *map_cb_band, list<long> *lst_lwcb);

	//按电流方向将典型运行方式和当前运行方式段形成树
	bool typical_form_band_tree();
	//形成段树的核心递归函数，递归将一个合闸进线开关形成供电的所有段形成一棵树
	void typical_form_band_tree_core(Tree<BAND_TREE_DATA>* pTree, TreeNode<BAND_TREE_DATA>* pParent, map<long, CB_BAND_POS>* p_map_cb_band, const long cb_id, BAND_TREE_DATA band, const long cb_jxcb, bool btypical);
	bool typical_deal_band(list<TYPICAL_STEP> & lst_step);
	bool typical_verify(list<TYPICAL_STEP> & lst_step);
	bool typical_deal_break(list<TYPICAL_STEP> & lst_step,const long cb_id);
protected:
public:
	char m_id[100];	//该类对象处理的典型运行方式ID
	FDIR_R2 *m_fdir;
	
	map<long, Tree<CB_INFO> >	m_map_dv_cb;
	map<long, list<BAND_INFO> > m_map_dv_band;				//所有关联馈线下的段信息
	map<long, Tree<BAND_TREE_DATA> > m_map_tree_band; 		//当前运行方式下的段树（按当前方式下的电流方向形成）,string为给该棵树供电的进线开关
	map<long, Tree<BAND_TREE_DATA> > m_map_tree_band_typical; //典型运行方式下的段树（按典型方式下的电流方向形成）,string为给该棵树供电的进线开关
	map<BAND_TREE_DATA, BAND_TREE_POS> m_map_band_pos;			//每个段在段树上的位置
	map<long, CB_INFO> m_map_cb; //主线上的开关信息
	map<long, CB_INFO> all_map_cb; //全部的开关信息
};
extern int ExecSQL(const char *sql);
extern char *sql;
extern char sqltemp[1024];
#endif
