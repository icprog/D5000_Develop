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
#define FDIR_R_NO_JXCB		-1 //û�бպϵĽ��߿���
#define FDIR_R_MUTI_JXCB	-2 //����պϵĽ��߿���

#define FDIR_FAIL_DIRECT	-1
#define FDIR_FAIL_ISOLATE	-2
#define FDIR_FAIL_RESTORE	-3

#define FDIR_TYPE_CONTROL	1			//ȫң�����Ϳ���
#define FDIR_TYPE_PWRFLOW	2			//���ó������㲻��ң�ؿ��صĵ���
#define FDIR_TYPE_IGNORE	3			//���Ե����Ϳ��ؿ�ң����

#define PROCESS_ONE_BY_ONE	1
#define FAULT_DESC_FILE	"/data/users/osa/adm/alarm/realtime/faultfile/fault_desc.xml"

#define DETECT_ONLY	false	//ֻ��λ����

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

//��Χ���Ͼ�ȷ��λ�ĸ��˵�
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

	long id;//����ָʾ����id
	long eq_id;//����ָʾ���������豸id
	string table;
	string eq_descr;
	bool 	border;
	int ind;//����ָʾ���������豸��������Ľ���
	int whole;
	int flag;//1��ʾ�ǹ�����ʼ�豸��2��ʾ�ǹ��Ͻ����豸
};

typedef struct FDIR_ISO_CB
{
	long cb;
	int  q; //�Ƿ��ǹ����������εĸ��뿪��
} FDIR_ISO_CB;

typedef std::map<int, vector<CLS_FDIR_RESTORE> > MAP_FDIR_RESTORE, *PMAP_FDIR_RESTORE;//intΪ�������ε�ֱ���������Σ�TTableΪ��ֱ���������εĻָ�����
typedef enum {FAULT_TYPE_NONE/*�޹��ϣ�����*/, FAULT_TYPE_SURE/*����*/, FAULT_TYPE_WINK/*˲ʱ����*/, FAULT_TYPE_POSSIBLE/*���ܹ���*/,FAULT_TYPE_BUS /*ĸ�߹���*/} FAULT_TYPE_CONTEXT;
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
	 //  ���ĸ������
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
	int FormIcbOfZone();//�γ����εĵ������뿪��
	int ExecuteFDIR_R();//ִ�й��ϴ������
	bool WriteRtdb();//�����ϴ�������д�����ݿ�
	int FormFdirDB();//�γɹ��ϴ������ݿ�
	int FormResSteps();//���ɻָ�����
	int FormIsoSteps();//���ɸ��벽��
	int ExecFdir();
	int ReIsolate(const long cb, const char* fault_id);
	FAULT_START data_start;

	const FDIR_R& operator =(const FDIR_R& fdir_r);
	//��ȡ�������Σ�ũ����������������ʹ��
	int getfaultzone();

	//�ѹ������λָ����ڴ������У��ڴ����ݻָ�ʱʹ��
	void setfaultzone(int newzone);

	//��ȡ�������ߵĽ��߿��أ�д��ʱ���ã��������ݣ��ڴ�ָ�ʱʹ��
	int getfaultjxcb();

	//���ù������ߵĽ��߿��أ��ڴ�ָ�ʱʹ��
	void setfaultjxcb(int icb);

	//��ȡ�Զ���բ�Ŀ������ڵ����Σ��������λָ�����ʱʹ��
	int getfaultautoopenzone();
	int getisozone();
	void getlstupzone(int fmzone, int tozone, list<int> &lstzone);

	//��ȡ���ϵ����Σ����Ͽ�����������Σ���������д�����豸ʱʹ��
	void getlstfaultzone(int zone, list<int> &lstzone);

	//�Ѹ��뿪����Ϣ���������У��ڴ�ָ�ʱʹ��
	int setisolate(list<FDIR_ISO_CB> &lst_cb);

	//��ȡת�����ߵĿ�����Ϣ���������ʹ�õ����ߣ�����У��ʱʹ��
	int getfhzglstcb(int izone, list<CLS_CB> &lst_cb);

	//��ȡ���ϵı߽翪�أ����Ϸ���ʱʹ��
	const list<int>* get_edge_cb();
	long getfaultst();
	long getfaultcb();
	long getfaultsec();

	//��ȡ���뿪�ص�cb��id����point��idת��Ϊcb��id�����¼���ʱʹ��
	TTable<CLS_FDIR_ISOLATE>* getisolate_cbid() const;

	//��ȡ���뿪�ص�point��id
	TTable<CLS_FDIR_ISOLATE>* getisolate() const;

	TTable<CLS_FDIR_ISOLATE>* getisolate2() const;
	MAP_FDIR_RESTORE getrestore() const;
	MAP_FDIR_RESTORE getrestore2() const;

	//��ȡ���ε������豸����д���ʱ�����
	int get_zone_devs(const int izone, list<FAULT_ITEM> &devs);

	//��ȡ���εĿ��أ��ڻ�ȡ���������豸ʱ����
	int get_zone_cb(const int izone, string &lst_cb) const;

	//��ȡ���ε����и��ɣ��ڻ�ȡ���ε������豸ʱ����
	int get_zone_ld(const int izone, list<long> &lst_ld);

	//��ȡ���ε������߶Σ��ڻ�ȡ���ε������豸ʱ����
	int get_zone_sec(const int izone, list<long> &lst_sec);

	//��ȡ���ϵĿ�ʼ�ͽ����豸�����ϵı߽��豸������д���������ı�ʱʹ�ã���д��ʱҲʹ��
	void get_fault_area(FAULT_ITEM &head, list<FAULT_ITEM> &tail);

	//ũ������ȡ���ϵ��豸��д���ϵ��豸ʱ����
		void get_fault_devs(list<FAULT_ITEM> &up_devs, list<FAULT_ITEM> &fault_devs, list<FAULT_ITEM> &down_devs);

	//���ù����豸�ı߽磨��ǹ��Ͽ�ʼ���豸�����Ͻ������豸����д��ʱ����
	void set_faultcb_edge(const char *faultid);

	//��ĳ����㿪ʼ��������豸,���õݹ麯����ѭ�����ҹ��ϵ��豸,д���ȡ�����豸ʱ����
//	void get_head_tail_dev(int ind, bool *bvisited, list<FAULT_ITEM> &lst_devs);

	//��ʱûʹ��
	int GetZoneByCB(list<long> lst_cb);

	//�������λ�ȡ���ߣ��������з�ʽ��ȡ����ʱʹ��
	void GetDVByZone(int izone, PSBOB::DV &dv);

	//�γɹ������򣬹��϶�λ��ɺ���ã��ѹ�����Ϣд�������
	void FormFaultArea();

	DSBOB *get_dsbob(){return dsbob;}

public:
	list<long>		lst_restore;	//��Ҫ��ӵ��������������ֹ��ָ����裨�Զ���բ�Ŀ��أ�
	list<DEV_STATUS> 	lst_dev;		/*�������ڳ�վ�¹Ҽ��޻�����ƵĿ��ػ򸺺��б�*/
	list<FAULT_ITEM>	lst_point;		/*�ռ����Ĺ������ڳ�վ���й����ź�*/
	list<long>		lst_err_point;	/*�󱨹����ź�*/
	list<long>		lst_mis_point;	/*©�������ź�*/
	list<DEV_STATUS>	lst_warning;	//�޷�������Ҫ��ʾ���û����豸
	map<int, long>	lst_iso_failed;	//����ʧ�ܵĿ���ң��,intΪICB��stringΪң��ID
	list<long>		lst_isolated;	//�Ѿ�����ɹ��Ŀ���
	list<long>		lst_failed;		//ң��ʧ�ܵĿ���
	list<FDIR_CB_INFO>	lst_cb_info;	//����ң�ء������Զ���բ�������ݡ����ɼ�״̬�Ŀ����б�
	FAULT_TYPE_CONTEXT	fault_type;		//��������
	FAULT_TYPE 			fault_fi_type;				//�������ͣ��ӵع��ϻ��·����
	TreeNode<FDIR_TREE_NODE_DATA> 		*fault_head;//���Ͽ�ʼ�Ľ��
	list<TreeNode<FDIR_TREE_NODE_DATA>*> fault_tail;//���Ͻ����Ľ��
	list<FAULT_ITEM>	m_ld;//���ϵ���ʧ�粢�Ҳ��ָܻ�����ĸ���
	bool 				b_ld_fault;		//�Ƿ�ΪLD���ϣ������ϵ��¹����źŶ����������豸ֻ��LD����
	list<long> lst_loss_vl_bus; //�뱾�ι�����ص�ʧѹ��ĸ��

protected:
	void FindFaultTail(int izone, list<FAULT_ITEM> &tail);

	//�ж�ĳ�������Ƿ�Ϊ�ܶ����أ�����ʱ����
	bool IsCBFailed(const long point_name);

	//��ȡ���ص���������
	int FindUpZone(int icb);

	//��ȡ���ص���������
	int FindDownZone(int icb);
	void FindAllDownZones(int izone, list<int> &lst_iso_cb);
	int BuildCb();
	int BuildZone();
	int BuildBusZone(bool *bvisited, const long nd_id, int &k);
	int GetCbCountToNd(int ind, int zn_type);
	int Insert_Zone_Cb(int icb ,int k, int zn_type);

	//ũ������ȡ�����ź��з�բ���¹���բ����ؿ��أ��������θ�������λָ�����ʱʹ��
	int ReadCbOpen(list<long> &lstopen);

	//ũ��������g_lstSecTrig��û��ʹ�õĹ����źţ����϶�λʧ�ܻ��߳ɹ�ʱ����
	void EraseNotUseFaultsng(std::list<long> &faultsng, bool ispointid);

	int FaultDetection();

	void BuildTree(bool *bvisited, bool *bvisited_zn, int pos, TreeNode<FDIR_TREE_NODE_DATA> *pParent, Tree<FDIR_TREE_NODE_DATA> *pTree);

	//�������ź����õ���Ӧ�豸��topflags��
	int SetFaultTopflags(FDIR_POINT_TRIG_SECTION &lstTrig, FDIR_TREE_NODE_DATA &key, map<long, FDIR_POINT_TRIG_SECTION> &map_cb_faults);

	//����Сʱ�ڴ����ߵĹ����ź����õ���Ӧ���豸��topflags��
	int SetBefFaultflags(list<FAULT_ITEM> lst_fi);

	//���������źŽ���������
	int BuildTree(FDIR_TREE_NODE_DATA &key, Tree<FDIR_TREE_NODE_DATA> &tree);

	//ũ���������������µ����й���ָʾ���γ����ṹ,tianyq��ӻ�ȡ��բ���صĽ�㣬���϶�λʱ���������������ߵ��������Ұѹ������õ�ĸ�߱�������
	void build_fi_tree_BFS(int ind, bool *bvisited, bool *bvisited_nd, TreeNode<FDIR_TREE_NODE_DATA>* father, Tree<FDIR_TREE_NODE_DATA> &tree, bool save_ld,list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_jxcb, bool b_build_tree, bool Isputjxcb);
    // ���������豸�͹���ĸ��
	void Find_fault_devs(int ind, bool *bvisited, bool *bvisited_nd, bool save_ld);
	void Find_fault_devs(int ind, vector<bool> &bvisited_nd, FAULT_ITEM parent);
	//�������ҳ�����·��
	int GetFaultPath(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FDIR_TREE_NODE_DATA &key_dev);

	//�жϹ��ϵ����ͣ�˲ʱ���ϡ����ܹ��ϡ��󱨵ȣ�
	void GetFaultType(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FAULT_TYPE_CONTEXT &type, bool &ld_fault);

	//ũ������ȡ��m_tree_fi��ָ�����pnode���εĹ���ָʾ����δ�˿��أ�δ�˸����б����϶�λʱ���ã���ϸ��λʱ���ô˺���
	void GetDownDevs(TreeNode<FDIR_TREE_NODE_DATA>* pnode, list<TreeNode<FDIR_TREE_NODE_DATA>*> &lst_dev);

	//�жϿ������Ƿ�����Ź����źţ��ڽ�������ʱ�����
	bool find_cb_fi(long cb_id);

	//���ñ��ι����õ��Ĺ����ź�
	void FillUsedFaultInfo(list<TreeNode<FDIR_TREE_NODE_DATA>*> &lstPath, FDIR_POINT_TRIG_SECTION &lstTrig, map<long, FDIR_POINT_TRIG_SECTION> &map_cb_faults);

	//��Խ���״̬���Ե�������ڹ����ź��������������н��߿������ҳ�һ�������Ĺ�����߿���
	TreeNode<FDIR_TREE_NODE_DATA>* GetValidJXCBNode(map<TreeNode<FDIR_TREE_NODE_DATA>*,TreeNode<FDIR_TREE_NODE_DATA>* > mapJXCB_BUS, int &jxcb_tp_type);

	//�����Ͻ�������ʼ����������ʶ����
	void MaskFaultOnTree();

	//�ҳ���������Ĺ���ĸ�ߺ͹�������������豸
	int FindFaultBusAndFaultDevs(FDIR_TREE_NODE_DATA &key_dev,int faultjxcb);

	//���϶�λû���ҵ����߿��أ���Ѱ��Сʱ�ڵĴ������ϵĹ����źŶ�ȡ�������¶�λ
	int ReadDvFiBef(FDIR_TREE_NODE_DATA key, list<FAULT_ITEM> &lst_bef_fi);

	//��ȡ�ڵ�pnode��ڵ�childnode֮��������豸
	void GetDevsBetweenNodes(TreeNode<FDIR_TREE_NODE_DATA>* pnode, TreeNode<FDIR_TREE_NODE_DATA>* childnode, list<FAULT_ITEM> &devs);


	int SetFaultInfo();
	void spread_so(int izone);
	bool update_cb_state(bool bfhzg = false);
	int add_so();
	bool Find_BFS(CLS_ND *pnd, bool *bvisited);
	void GetDevsByND(CLS_ND *pnd, list<FAULT_ITEM> &devs);
	void FindFaultIslandEdgeCB(int icb);	//���ҹ��ϵ������ı߽翪��

protected:
	int lv_cb;
	int lv_bus;
	int lv_so;

protected:
	int faultzone;//��������
	int faulticb;//�׶˿���
	int faultbus;//���ι������ӵĵ�Դ��ĸ��
	int faultjxcb;//�������εĽ��߿���
	int faultauto_open;//�����Զ���բ������
	FAULT_ITEM head_ppft;//��ӽ��������ε���ʼ����ָʾ��ID
	list<FAULT_ITEM> tail_ppft;//��ӽ��������ε�δ�˹���ָʾ��ID��
	int isozone; //���θ��뿪�ص���������

	Tree<FDIR_TREE_NODE_DATA>	m_Tree;
	TreeNode<FDIR_TREE_NODE_DATA> *m_p_jxcb_Node;
	TreeNode<FDIR_TREE_NODE_DATA> *m_p_bus_Node;
	map<TreeNode<FDIR_TREE_NODE_DATA>*,TreeNode<FDIR_TREE_NODE_DATA>* > map_jxcb_bus; // �������߿��غ͵�Դĸ�ߵ�ӳ���ϵ
	list<int>	lst_fault_open_cb;	//�뱾�ι�����صĴ��ڷ�բ״̬�Ŀ���

	bool b_pass_head_tail;
	bool b_pass_fault_head;
	bool b_pass_fault_tail;	//���϶�λ�ɹ����ٴε���build_fi_tree_BFS�ӽ��߿��ؿ�ʼ�������Բ���faultbus�͹��������ڵ������豸��
								//�ñ�������ڲ��ҵĹ������Ƿ��Ѿ��������Ͻ����豸
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
	MAP_FDIR_RESTORE     		  fdir_restore_tb2;			//��ң�ػָ�����
	TTable<CLS_FDIR_ISOLATE>*     fdir_isolate_tb2;			//��ң�ظ��뷽��
	TTable<CLS_FDIR_FZONE>*		  fdir_fzone_tb;
	list<int> 					  lst_up_iso_cb;			//���ڽ����θ��뿪�����������������������α߽翪��
	list<int>					  lst_edge_cb;				//�������ڵ������ı߽�Ͽ�����

	map<long, bool> map_cb_restore;	//string�������α߽翪��id�� bool�ñ߽翪�����������Ƿ��лָ�����
	map<FAULT_ITEM, list<FAULT_ITEM> > map_node_downnode;

protected:
	map<long, CLS_BUS> map_nd_bus;//����ƽ20111218��ӣ����ڿ�������һ������Ƿ���ĸ����

public:
	long lFaultStId;			//��ӹ�������������վID�����ڽ�����߿��ع��ϲ����Ƶ�����ͼ���޷��ƻ���ͼ������
	//��ӱ������洢���������ڵ㣬��ֹ�ݹ��㷨����ʱ��fatherΪ�յ��µĹ�����ƴ�Ӵ��󣬸ô�����ܵ��¹���·������ʧ��
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
