/*
 * FDIR_R.h
 *
 *  Created on: 2011-11-8
 *      Author: BetaKDE
 */

#ifndef FDIR_R_H_
#define FDIR_R_H_
//#include "oodb.h"
#include <vector>
#include "dsbob.h"
#include "Tp.h"
//#include <fdir.h>
#include <pthread.h>
#include <map>
#include "tmptable.h"
#include "fdirdb.h"
#include "netmom.h"
#include "Nmd.h"
#include "comm.h"
#include "INTROCI.h"
#include "DBFDIR.h"
#include "Tree.h"

extern bool GetCode(const long, string&);

#define FDIR_R_SUCCEED  1
#define FDIR_R_FAIL     0
#define FDIR_R_NO_JXCB		-1 //没有闭合的进线开关
#define FDIR_R_MUTI_JXCB	-2 //多个闭合的进线开关

#define FDIR_FAIL_DIRECT	-1
#define FDIR_FAIL_ISOLATE	-2
#define FDIR_FAIL_RESTORE	-3

#define FDIR_TYPE_CONTROL	1			//全遥控类型开关
#define FDIR_TYPE_PWRFLOW	2			//利用潮流计算不能遥控开关的电流
#define FDIR_TYPE_IGNORE	3			//忽略电流和开关可遥控性

#define PROCESS_ONE_BY_ONE	1
#define FAULT_DESC_FILE	"/data/users/osa/adm/alarm/realtime/faultfile/fault_desc.xml"

#define DETECT_ONLY	false	//只定位故障

struct FAULTSNG
{
	time_t		time;
	char		name[20];
	char		type[4];
	int			status;
};

struct FAULT_START
{
	int type;
	union
	{
		long bus_id;
		long cb_id;
		long ld_id;
	}source;
};

//包围故障精确定位的各端点
class FAULT_ITEM
{
public:
	FAULT_ITEM()
	{
		id = -1;
		table.clear();
		eq_id = -1;;
		eq_descr.clear();
		whole = 1;
		ind = -1;
		flag = 0;
		border = false;
	}

	~FAULT_ITEM()
	{
		table.clear();
	}

	FAULT_ITEM(const long szid, const long eq, const char *sztable, const char *descr)
	{
		id = szid;
		eq_id = eq;
		table = sztable;
		whole = 1;
		ind = -1;
		eq_descr = descr;
		flag = 0;
		border = false;
	}

	FAULT_ITEM(const long szid, const long eq, const string sztable, const string descr)
	{
		id = szid;
		eq_id = eq;
		table = sztable;
		whole = 1;
		eq_descr = descr;
		ind = -1;
		flag = 0;
		border = false;
	}

	const FAULT_ITEM& operator =(const FAULT_ITEM& item)
	{
		id = item.id;
		eq_id = item.eq_id;
		table = item.table;
		whole = item.whole;
		eq_descr = item.eq_descr;
		ind = item.ind;
		flag = item.flag;
		border = item.border;

		return *this;
	}

	bool operator ==(const FAULT_ITEM &fault_item) const
	{
		return id > 0 ? id == fault_item.id : eq_id == fault_item.eq_id;
	}

	bool operator <(const FAULT_ITEM& fault_item) const
	{
		return id > 0 ? id < fault_item.id : eq_id < fault_item.eq_id;
	}

	long id;//故障指示器的id
	long eq_id;//故障指示器关联的设备id
	string table;
	string eq_descr;
	bool 	border;
	int ind;//故障指示器关联的设备电流流入的结点号
	int whole;
	int flag;//1表示是故障起始设备，2表示是故障结束设备
};

typedef struct FDIR_ISO_CB
{
	long cb;
	int  q; //是否是故障区域上游的隔离开关
} FDIR_ISO_CB;

typedef std::map<int, vector<CLS_FDIR_RESTORE> > MAP_FDIR_RESTORE, *PMAP_FDIR_RESTORE;//int为故障区段的直接下游区段，TTable为该直接下游区段的恢复方案
typedef enum {FAULT_TYPE_NONE/*无故障，是误报*/, FAULT_TYPE_SURE/*故障*/, FAULT_TYPE_WINK/*瞬时故障*/, FAULT_TYPE_POSSIBLE/*可能故障*/,FAULT_TYPE_BUS /*母线故障*/} FAULT_TYPE_CONTEXT;
typedef enum {FAULT_UNKNOW, FAULT_JDFAULT, FAULT_DLFAULT} FAULT_TYPE;
typedef struct _FDIR_TREE_NODE_DATA
{
	_FDIR_TREE_NODE_DATA()
	{
		m_type = -1;
		m_pData = NULL;
	}

	_FDIR_TREE_NODE_DATA(CLS_CB *pcb)
	{
		assert(pcb != NULL);
		m_type = _CB;
		m_pData = pcb;
	}

	_FDIR_TREE_NODE_DATA(CLS_LD* pld)
	{
		assert(pld != NULL);
		m_type = _LD;
		m_pData = pld;
	}

	_FDIR_TREE_NODE_DATA(CLS_FI* pfi)
	{
		assert(pfi != NULL);
		m_type = _FI;
		m_pData = pfi;
	}
	 //  添加母线类型
	_FDIR_TREE_NODE_DATA(CLS_BUS* pbus)
	{
		assert(pbus != NULL);
		m_type = _BUS;
		m_pData = pbus;
	}

	_FDIR_TREE_NODE_DATA(int type, void* p)
	{
		assert(p != NULL);

		m_type = type;
		m_pData = p;
	}

	long GetPid()
	{
		if (m_type == _CB)
		{
			return ((CLS_CB*) (m_pData))->pid;
		}
		else if (m_type == _LD)
		{
			return ((CLS_CB*) (m_pData))->pid;
		}
		else if (m_type == _BUS)
		{
			return ((CLS_CB*) (m_pData))->pid;
		}
		else if (m_type == _FI)
		{
			return ((CLS_CB*) (m_pData))->pid;
		}
		else
		{
			return -1;
		}
	}

	const char* GetText()
	{
		if (m_type == _CB)
		{
			string code;

			GetCode(((CLS_CB*) (m_pData))->pid, code);
			return code.size() > 0 ? code.c_str() : ((CLS_CB*) (m_pData))->descr;
		}
		else if (m_type == _LD)
		{
			string code;

			GetCode(((CLS_LD*) (m_pData))->pid, code);
			return code.size() > 0 ? code.c_str() : ((CLS_LD*) (m_pData))->descr;
		}
		else if (m_type == _BUS)
		{
			string code;
			GetCode(((CLS_BUS*)(m_pData))->pid,code);
			return code.size() > 0 ? code.c_str() : ((CLS_BUS*) (m_pData))->descr;
		}
		else if (m_type == _FI)
		{
			string code;

			GetCode(((CLS_FI*) (m_pData))->pid, code);
			return code.size() > 0 ? code.c_str() : ((CLS_FI*) (m_pData))->descr;
		}
		else
		{
			return NULL;
		}
	}

	int GetType()
	{
		return m_type;
	}

	const _FDIR_TREE_NODE_DATA & operator=(const _FDIR_TREE_NODE_DATA &data)
	{
		m_type = data.m_type;
		m_pData = data.m_pData;

		return *this;
	}

	int m_type;
	void *m_pData;
}FDIR_TREE_NODE_DATA, *PFDIR_TREE_NODE_DATA;

class  FDIR_R
{
public:
	FDIR_R(DSBOB* pdsbob);
	virtual ~FDIR_R();
public:
	int FormIcbOfZone();//形成区段的电流流入开关
	int ExecuteFDIR_R();//执行故障处理程序
	bool WriteRtdb();//将故障处理数据写入数据库
	int FormFdirDB();//形成故障处理数据库
	int FormResSteps();//生成恢复步骤
	int FormIsoSteps();//生成隔离步骤
	int ExecFdir();
	int ReIsolate(const long cb, const char* fault_id);
	FAULT_START data_start;

	const FDIR_R& operator =(const FDIR_R& fdir_r);
	//获取故障区段，农牧区和西宁配网都使用
	int getfaultzone();

	//把故障区段恢复到内存数据中，内存数据恢复时使用
	void setfaultzone(int newzone);

	//获取故障馈线的进线开关，写库时调用，保存数据，内存恢复时使用
	int getfaultjxcb();

	//设置故障馈线的进线开关，内存恢复时使用
	void setfaultjxcb(int icb);

	//获取自动跳闸的开关所在的区段，计算上游恢复区段时使用
	int getfaultautoopenzone();
	int getisozone();
	void getlstupzone(int fmzone, int tozone, list<int> &lstzone);

	//获取故障的区段，故障可能扩大的区段，西宁配网写故障设备时使用
	void getlstfaultzone(int zone, list<int> &lstzone);

	//把隔离开关信息加入隔离表中，内存恢复时使用
	int setisolate(list<FDIR_ISO_CB> &lst_cb);

	//获取转供馈线的开关信息，保存故障使用的馈线，数据校验时使用
	int getfhzglstcb(int izone, list<CLS_CB> &lst_cb);

	//获取故障的边界开关，故障分组时使用
	const list<int>* get_edge_cb();
	long getfaultst();
	long getfaultcb();
	long getfaultsec();

	//获取隔离开关的cb的id，把point的id转换为cb的id，重新计算时使用
	TTable<CLS_FDIR_ISOLATE>* getisolate_cbid() const;

	//获取隔离开关的point的id
	TTable<CLS_FDIR_ISOLATE>* getisolate() const;

	TTable<CLS_FDIR_ISOLATE>* getisolate2() const;
	MAP_FDIR_RESTORE getrestore() const;
	MAP_FDIR_RESTORE getrestore2() const;

	//获取区段的所有设备，在写库的时候调用
	int get_zone_devs(const int izone, list<FAULT_ITEM> &devs);

	//获取区段的开关，在获取区段所有设备时调用
	int get_zone_cb(const int izone, string &lst_cb) const;

	//获取区段的所有负荷，在获取区段的所有设备时调用
	int get_zone_ld(const int izone, list<long> &lst_ld);

	//获取区段的所有线段，在获取区段的所有设备时调用
	int get_zone_sec(const int izone, list<long> &lst_sec);

	//获取故障的开始和结束设备（故障的边界设备），在写故障描述文本时使用，在写库时也使用
	void get_fault_area(FAULT_ITEM &head, list<FAULT_ITEM> &tail);

	//农牧区获取故障的设备，写故障的设备时调用
		void get_fault_devs(list<FAULT_ITEM> &up_devs, list<FAULT_ITEM> &fault_devs, list<FAULT_ITEM> &down_devs);

	//设置故障设备的边界（标记故障开始的设备，故障结束的设备），写库时调用
	void set_faultcb_edge(const char *faultid);

	//从某个结点开始保存故障设备,调用递归函数，循环查找故障的设备,写库获取故障设备时调用
//	void get_head_tail_dev(int ind, bool *bvisited, list<FAULT_ITEM> &lst_devs);

	//暂时没使用
	int GetZoneByCB(list<long> lst_cb);

	//根据区段获取馈线，典型运行方式获取馈线时使用
	void GetDVByZone(int izone, PSBOB::DV &dv);

	//形成故障区域，故障定位完成后调用，把故障信息写入变量中
	void FormFaultArea();

	DSBOB *get_dsbob(){return dsbob;}

public:
	list<long>		lst_restore;	//需要添加到操作步骤最后的手工恢复步骤（自动跳闸的开关）
	list<DEV_STATUS> 	lst_dev;		/*故障所在厂站下挂检修或隔离牌的开关或负荷列表*/
	list<FAULT_ITEM>	lst_point;		/*收集到的故障所在厂站所有过流信号*/
	list<long>		lst_err_point;	/*误报过流信号*/
	list<long>		lst_mis_point;	/*漏报过流信号*/
	list<DEV_STATUS>	lst_warning;	//无法保电需要提示给用户的设备
	map<int, long>	lst_iso_failed;	//隔离失败的开关遥信,int为ICB，string为遥信ID
	list<long>		lst_isolated;	//已经隔离成功的开关
	list<long>		lst_failed;		//遥控失败的开关
	list<FDIR_CB_INFO>	lst_cb_info;	//不能遥控、不能自动跳闸、老数据、不采集状态的开关列表
	FAULT_TYPE_CONTEXT	fault_type;		//故障类型
	FAULT_TYPE 			fault_fi_type;				//故障类型：接地故障或短路故障
	TreeNode<FDIR_TREE_NODE_DATA> 		*fault_head;//故障开始的结点
	list<TreeNode<FDIR_TREE_NODE_DATA>*> fault_tail;//故障结束的结点
	list<FAULT_ITEM>	m_ld;//故障导致失电并且不能恢复供电的负荷
	bool 				b_ld_fault;		//是否为LD故障（负荷上的事故总信号动作，故障设备只有LD本身）
	list<long> lst_loss_vl_bus; //与本次故障相关的失压的母线

protected:
	void FindFaultTail(int izone, list<FAULT_ITEM> &tail);

	//判断某个开关是否为拒动开关，隔离时调用
	bool IsCBFailed(const long point_name);

	//获取开关的上游区段
	int FindUpZone(int icb);

	//获取开关的下游区段
	int FindDownZone(int icb);
	void FindAllDownZones(int izone, list<int> &lst_iso_cb);
	int BuildCb();
	int BuildZone();
	int BuildBusZone(bool *bvisited, const long nd_id, int &k);
	int GetCbCountToNd(int ind, int zn_type);
	int Insert_Zone_Cb(int icb ,int k, int zn_type);

	//农牧区读取故障信号中分闸、事故跳闸的相关开关，计算上游隔离和上游恢复区段时使用
	int ReadCbOpen(list<long> &lstopen);

	//农牧区擦除g_lstSecTrig中没有使用的故障信号，故障定位失败或者成功时调用
	void EraseNotUseFaultsng(std::list<long> &faultsng, bool ispointid);

	int FaultDetection();

	void BuildTree(bool *bvisited, bool *bvisited_zn, int pos, TreeNode<FDIR_TREE_NODE_DATA> *pParent, Tree<FDIR_TREE_NODE_DATA> *pTree);

	//将故障信号设置到对应设备的topflags上
	int SetFaultTopflags(FDIR_POINT_TRIG_SECTION &lstTrig, FDIR_TREE_NODE_DATA &key, map<long, FDIR_POINT_TRIG_SECTION> &map_cb_faults);

	//将半小时内此馈线的故障信号设置到对应的设备的topflags上
	int SetBefFaultflags(list<FAULT_ITEM> lst_fi);

	//根据启动信号建立故障树
	int BuildTree(FDIR_TREE_NODE_DATA &key, Tree<FDIR_TREE_NODE_DATA> &tree);

	//农牧区将故障馈线下的所有故障指示器形成树结构,tianyq添加获取跳闸开关的结点，故障定位时建立故障所在馈线的树，并且把故障所用的母线保存下来
	void build_fi_tree_BFS(int ind, bool *bvisited, bool *bvisited_nd, TreeNode<FDIR_TREE_NODE_DATA>* father, Tree<FDIR_TREE_NODE_DATA> &tree, bool save_ld,list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_jxcb, bool b_build_tree, bool Isputjxcb);
    // 搜索故障设备和故障母线
	void Find_fault_devs(int ind, bool *bvisited, bool *bvisited_nd, bool save_ld);
	void Find_fault_devs(int ind, vector<bool> &bvisited_nd, FAULT_ITEM parent);
	//在树上找出故障路径
	int GetFaultPath(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FDIR_TREE_NODE_DATA &key_dev);

	//判断故障的类型（瞬时故障、可能故障、误报等）
	void GetFaultType(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FAULT_TYPE_CONTEXT &type, bool &ld_fault);

	//农牧区获取树m_tree_fi中指定结点pnode下游的故障指示器，未端开关，未端负荷列表，故障定位时调用，详细定位时调用此函数
	void GetDownDevs(TreeNode<FDIR_TREE_NODE_DATA>* pnode, list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_dev);

	//判断开关上是否关联着故障信号，在建立树的时候调用
	bool find_cb_fi(long cb_id);

	//设置本次故障用到的故障信号
	void FillUsedFaultInfo(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FDIR_POINT_TRIG_SECTION &lstTrig, map<long, FDIR_POINT_TRIG_SECTION> &map_cb_faults);

	//针对进线状态不对的情况下在故障信号两端搜索到所有进线开关中找出一个真正的供电进线开关
	TreeNode<FDIR_TREE_NODE_DATA>* GetValidJXCBNode(map<TreeNode<FDIR_TREE_NODE_DATA>*,TreeNode<FDIR_TREE_NODE_DATA>* > mapJXCB_BUS, int &jxcb_tp_type);

	//在树上将故障起始、结束结点标识出来
	void MaskFaultOnTree();

	//找出故障区域的供电母线和故障区域的所有设备
	int FindFaultBusAndFaultDevs(FDIR_TREE_NODE_DATA &key_dev,int faultjxcb);

	//故障定位没有找到进线开关，则把半个小时内的此馈线上的故障信号读取出来重新定位
	int ReadDvFiBef(FDIR_TREE_NODE_DATA key, list<FAULT_ITEM> &lst_bef_fi);

	//获取节点pnode与节点childnode之间的所有设备
	void GetDevsBetweenNodes(TreeNode<FDIR_TREE_NODE_DATA>* pnode, TreeNode<FDIR_TREE_NODE_DATA>* childnode, list<FAULT_ITEM> &devs);


	int SetFaultInfo();
	void spread_so(int izone);
	bool update_cb_state(bool bfhzg = false);
	int add_so();
	bool Find_BFS(CLS_ND *pnd, bool *bvisited);
	void GetDevsByND(CLS_ND *pnd, list<FAULT_ITEM> &devs);
	void FindFaultIslandEdgeCB(int icb);	//查找故障电气岛的边界开关

protected:
	int lv_cb;
	int lv_bus;
	int lv_so;

protected:
	int faultzone;//故障区段
	int faulticb;//首端开关
	int faultbus;//本次故障连接的电源或母线
	int faultjxcb;//故障区段的进线开关
	int faultauto_open;//故障自动跳闸的区段
	FAULT_ITEM head_ppft;//最接近故障区段的起始故障指示器ID
	list<FAULT_ITEM> tail_ppft;//最接近故障区段的未端故障指示器ID们
	int isozone; //上游隔离开关的下游区段

	Tree<FDIR_TREE_NODE_DATA>	m_Tree;
	TreeNode<FDIR_TREE_NODE_DATA> *m_p_jxcb_Node;
	TreeNode<FDIR_TREE_NODE_DATA> *m_p_bus_Node;
	map<TreeNode<FDIR_TREE_NODE_DATA>*,TreeNode<FDIR_TREE_NODE_DATA>* > map_jxcb_bus; // 建立进线开关和电源母线的映射关系
	list<int>	lst_fault_open_cb;	//与本次故障相关的处于分闸状态的开关

	bool b_pass_head_tail;
	bool b_pass_fault_head;
	bool b_pass_fault_tail;	//故障定位成功后再次调用build_fi_tree_BFS从进线开关开始搜索，以查找faultbus和故障区域内的所有设备，
								//该变量标记在查找的过程中是否已经遇到故障结束设备
	list<FAULT_ITEM> lst_fault_devs, lst_up_devs, lst_down_devs;
	DSBOB *dsbob;
	TP* tp;
public:
	TTable<CLS_FDIR_CB>*          fdir_cb_tb;
	TTable<CLS_FDIR_ZONE>*        fdir_zone_tb;
	TTable<CLS_FDIR_ZONE_CB>*     fdir_zone_cb_tb;
protected:
	MAP_FDIR_RESTORE     		  fdir_restore_tb;
	TTable<CLS_FDIR_ISOLATE>*     fdir_isolate_tb;
	MAP_FDIR_RESTORE     		  fdir_restore_tb2;			//非遥控恢复方案
	TTable<CLS_FDIR_ISOLATE>*     fdir_isolate_tb2;			//非遥控隔离方案
	TTable<CLS_FDIR_FZONE>*		  fdir_fzone_tb;
	list<int> 					  lst_up_iso_cb;			//由于将上游隔离开关往上扩而产生的上游区段边界开关
	list<int>					  lst_edge_cb;				//故障所在电气岛的边界断开开关

	map<long, bool> map_cb_restore;	//string故障区段边界开关id， bool该边界开关下游区段是否有恢复方案
	map<FAULT_ITEM, list<FAULT_ITEM> > map_node_downnode;

protected:
	map<long, CLS_BUS> map_nd_bus;//荣义平20111218添加，便于快速搜索一个结点是否在母线上

public:
	long lFaultStId;			//添加故障区域所属厂站ID，用于解决进线开关故障不能推到馈线图和无法推环网图的问题
	//添加变量，存储故障树根节点，防止递归算法返回时因father为空导致的故障树拼接错误，该错误可能导致故障路径查找失败
	TreeNode<FDIR_TREE_NODE_DATA>* cur_father;
};

extern bool bDisplay;

extern map<long, long> g_map_fi_dv;
extern map<long, list<long> > g_map_trig_fi;
extern FDIR_CONFIG g_FdirConfig;
extern FDIR_POINT_TRIG_LIST g_lstTrig;
extern FDIR_POINT_TRIG_SECTION	g_lstSecTrig;
extern pthread_mutex_t	trig_list_mutex;
extern pthread_mutex_t oci_mutex;
extern NMD *nmd;
extern INTR_OCI *g_oci;
extern char *sql;
extern DB_FDIR *g_db_fdir;
extern map<long, list<long> > g_map_nd_bus, g_map_nd_switch;
extern int GetDv(const long devid, const char *devtb, PSBOB::DV &dv);
extern bool is_controllable(long cb_id);
extern int GetPntIDColumn();
extern void rte_disconnect();
extern int ExecSQL(const char *sql);
extern const char *key[];
#endif /* FDIR_R_H_ */
