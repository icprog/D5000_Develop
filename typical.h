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
typedef enum{TYPICAL_CLOSE = 0/*��բ*/, TYPICAL_OPEN/*��բ*/} CB_STATUS;
//������Ϣ
typedef struct
{
	long id; //����ID
	float Ia; //A�����ֵ
	float Ib; // B�����ֵ
	float Ic; // C�����ֵ
	long dv_id;
	CB_STATUS status; //���ؿ�����״̬0��բ��1��բ
	CB_TYPE type;	//��������
} CB_INFO, *PCB_INFPO;

//����Ϣ
typedef struct _BAND_INFO
{
	_BAND_INFO()
	{
		valid = 1;
	}
	map<long, CB_INFO> map_band_cb; //���ڵ�ǰ��ʽ�µı߽翪����ϢstringΪ���ص�ID
	map<long, CB_INFO> map_band_cb_typical;//���ڵ��ͷ�ʽ�µı߽翪����Ϣ
	long so_cb; //��ǰ��ʽ�µĹ�����߿���
	long so_cb_typical; //���ͷ�ʽ�µĹ�����߿���
	int valid;//��־�Ƿ���Ч0��Ч��1��Ч,��ʼ��Ϊ1
	long supply_cb; //�öε�ǰ��ʽ�ĵ������뿪��
	long supply_cb_typical; //�öε��ͷ�ʽ�µĵ������뿪��
} BAND_INFO, *PBAND_INFO;

//�ָ���������
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

	long cb; //����ID
	bool bopen; //���ز�������բtrue,��բfalse
	bool boptional; //�ò������Ƿ�Ϊ��ѡ
} TYPICAL_STEP, *PTYPICAL_STEP;

typedef PBAND_INFO BAND_TREE_DATA;	//�����ڵ�����

typedef struct _BAND_TREE_POS
{
	_BAND_TREE_POS()
	{
		pNode = NULL;
		pTypicalNode = NULL;
	}

	TreeNode<BAND_TREE_DATA>  *pNode;	//�ڵ�ǰ��ʽ�����µ�λ��
	TreeNode<BAND_TREE_DATA>	*pTypicalNode;//�ڵ���ǰ��ʽ�����µ�λ��
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
	//����������з�ʽ,cb:�˻����͵Ľ��߿���ID���ɹ�����true,ʧ�ܷ���false
	bool typical_save(const long cb_id, const char *machine, const char *userid);
	//�ָ����������з�ʽ,cb:�˻����͵Ľ��߿���ID���ɹ�����true,ʧ�ܷ���false
	bool typical_restore(const long cb_id, const char* machine, const char *userid, string &id, time_t &restore_time);

	//��ȡĳ�������ϵ����п�����Ϣ
	int get_dv_cb(int izone, const long fathercb, TreeNode<CB_INFO> *father, Tree<CB_INFO> &tree_cb);

	//��ȡ���ص���Ϣ�������������ֵ���������͡�����״̬��
	bool get_cb_info(CB_INFO &cb);

	//�ѱ����m_map_dv_cb�е����ݴ����ϵ��
	bool typical_write_cb();

	//�����Ͻ���γ�sql��䣬���ڰ�����д�����
	void formsql_from_tree(const char* id,const long dv, TreeNode<CB_INFO> *child);

	//��ȡָ�����ߵ������߿���,�ɹ�����true,ʧ�ܷ���false
	bool typical_get_main_cb();

	bool typical_get_all_cb(const long cb_id);

	//�ӹ�ϵ����ȡ�������з�ʽ�Ŀ�����Ϣ���γ���
	bool typical_read_cb();

	//�����߽��зֶΣ����ӽ��߿��������������
	void typical_band();

	void tree_to_band(TreeNode<CB_INFO>* pParent, Tree<CB_INFO> &pTree, BAND_INFO &bandnode, list<BAND_INFO> &lst_band);

	void typical_deal_lwcb(map<long, CB_BAND_POS> *map_cb_band, list<long> *lst_lwcb);

	//���������򽫵������з�ʽ�͵�ǰ���з�ʽ���γ���
	bool typical_form_band_tree();
	//�γɶ����ĺ��ĵݹ麯�����ݹ齫һ����բ���߿����γɹ�������ж��γ�һ����
	void typical_form_band_tree_core(Tree<BAND_TREE_DATA>* pTree, TreeNode<BAND_TREE_DATA>* pParent, map<long, CB_BAND_POS>* p_map_cb_band, const long cb_id, BAND_TREE_DATA band, const long cb_jxcb, bool btypical);
	bool typical_deal_band(list<TYPICAL_STEP> & lst_step);
	bool typical_verify(list<TYPICAL_STEP> & lst_step);
	bool typical_deal_break(list<TYPICAL_STEP> & lst_step,const long cb_id);
protected:
public:
	char m_id[100];	//���������ĵ������з�ʽID
	FDIR_R2 *m_fdir;
	
	map<long, Tree<CB_INFO> >	m_map_dv_cb;
	map<long, list<BAND_INFO> > m_map_dv_band;				//���й��������µĶ���Ϣ
	map<long, Tree<BAND_TREE_DATA> > m_map_tree_band; 		//��ǰ���з�ʽ�µĶ���������ǰ��ʽ�µĵ��������γɣ�,stringΪ���ÿ�������Ľ��߿���
	map<long, Tree<BAND_TREE_DATA> > m_map_tree_band_typical; //�������з�ʽ�µĶ����������ͷ�ʽ�µĵ��������γɣ�,stringΪ���ÿ�������Ľ��߿���
	map<BAND_TREE_DATA, BAND_TREE_POS> m_map_band_pos;			//ÿ�����ڶ����ϵ�λ��
	map<long, CB_INFO> m_map_cb; //�����ϵĿ�����Ϣ
	map<long, CB_INFO> all_map_cb; //ȫ���Ŀ�����Ϣ
};
extern int ExecSQL(const char *sql);
extern char *sql;
extern char sqltemp[1024];
#endif
