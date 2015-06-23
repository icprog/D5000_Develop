#ifndef _FDIRRDB_H
#define _FDIRRDB_H
#include "dsbob.h"

//descr length
#define MAX_CLS_FDIR_CB_DESCR_LEN		64
#define MAX_FDIR_BZONE_DESCR_LEN	64
#define MAX_FDIR_RESTORE_DESCR_LEN	128
#define MAX_FDIR_ISOLATE_DESCR_LEN  128
#define MAX_CLS_FDIR_ZONE_DESCR_LEN		64
#define MAX_FDIR_FZONE_DESCR_LEN		64
#define MAX_FDIR_GLOBAL_STATUS_LEN    20
#define MAX_FDIR_GLOBAL_USERNAME_LEN  20
#define MAX_FDIR_GLOBAL_MACHINE_LEN	  20
#define	MAX_FDIR_GLOBAL_SPOR_LEN	20
#define MAX_FDIR_HISTORY_DESCR_LEN  64
#define MAX_FDIR_ERROR_DESCR_LEN	128
#define MAX_FDIR_GLOBAL_REASON_LEN   32

//table size
#define mx_fdir_bzone				50
#define mx_fdir_cb					25000
#define mx_fdir_isolate				50
#define mx_fdir_restore				100
#define mx_fdir_zone				10000
#define mx_fdir_zone_cb				25000
#define mx_fdir_global				1
#define mx_fdir_history				50
#define mx_fdir_error				1000

//mask bit value
#define FDIR_RESTORE_OPEN0			1
#define FDIR_RESTORE_OPEN			2
#define FDIR_RESTORE_TODO			4
#define FDIR_RESTORE_DONE			8
#define FDIR_RESTORE_SUCC			16

#define FDIR_ISOLATE_OPEN0			1
#define FDIR_ISOLATE_OPEN			2
#define FDIR_ISOLATE_TODO			4
#define FDIR_ISOLATE_DONE			8
#define FDIR_ISOLATE_SUCC			16
#define FDIR_ISOLATE_UP				128
#define FDIR_ISOLATE_NO_SO			256

#define FDIR_BZONE_UNPROC			1

#define FDIR_ZONE_UNPROC			1

#define FDIR_ZONE_DEAD				1
#define FDIR_ZONE_DEAD0				2
#define FDIR_ZONE_FAULT				4
#define FDIR_ZONE_FLASH				8
 
#define FDIR_CB_UNPROC				1
#define	FDIR_CB_BREAKER				2
#define	FDIR_CB_SWITCH				4
#define	FDIR_CB_AUTO				8
#define FDIR_CB_FDHCB				16//�����׶˿���
#define FDIR_CB_FDSCB				32
#define FDIR_CB_FDCCB				64
#define FDIR_CB_FDLCB				128//���߸��ɿ���
#define FDIR_CB_NOAVAIL				256

#define FDIR_CB_DEAD				1
#define FDIR_CB_DEAD0				2
#define FDIR_CB_FAULT				4
#define FDIR_CB_FLASH				8
#define FDIR_CB_OPEN0				16
#define FDIR_CB_OPEN				32
//�������������������϶�λʱ����������� by rongyiping 20130319
#define FDIR_CB_DETECT_OPEN				64	//�������¹����ź�©����û���¹���բ�����ķ�բ����
#define FDIR_CB_DETECT_FAULT			128	//�¹���բ����
#define FDIR_CB_DETECT_FI				512	//����ָʾ�������Ŀ���
#define FDIR_CB_BEFORE_OPEN				4096//��Сʱ֮��ͬһ���߷����ķ�բ�����¹���բ�Ŀ���
#define FDIR_BUS_DETECT				    8192	//ĸ�߹�����Ϣ
#define FDIR_FAULT_HEAD					1024	//���豸Ϊ������ʼ�豸
#define FDIR_FAULT_TAIL					2048	//���豸Ϊ���Ͻ����豸

//zone type
#define	_FDIR_BUS					1
#define _FDIR_ZN					2
#define _FDIR_SO					3
#define _FDIR_LD					4
#define _FDIR_XFMR					5
#define _FDIR_CP					6

class CLS_FDIR_CB
{
public:
    long id;
	char descr[MAX_CLS_FDIR_CB_DESCR_LEN];
    int ind;// �׶����κ�
    int iznd;//ĩ�����κ�
    int q;
	int topflags;
	long cb_id;	//��FDIR��CB��DMS��CB��������

public:
    CLS_FDIR_CB();
    CLS_FDIR_CB(const long m_id);
    const CLS_FDIR_CB& operator = (const CLS_FDIR_CB& aCLS_FDIR_CB);
    int operator == (const CLS_FDIR_CB& m_fdir_cb) const
    {
        return id == m_fdir_cb.id;
    }
    int operator < (const CLS_FDIR_CB& m_fdir_cb) const
    {
        return id < m_fdir_cb.id;
    }

    void GetKey(char *key) const {sprintf(key, "%ld", id);}
    bool IsRTable() const {return false;}	//�ж��Ƿ��ϵ��
	const long GetParentId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

    ~CLS_FDIR_CB(){}
};

class  CLS_FDIR_FZONE
{
public:
    long id;
    char descr[MAX_FDIR_FZONE_DESCR_LEN];
    int izone;

public:
    CLS_FDIR_FZONE();
    CLS_FDIR_FZONE(const long m_id);
    const CLS_FDIR_FZONE& operator = (const CLS_FDIR_FZONE& aFDIR_FZONE);
    int operator == (const CLS_FDIR_FZONE& m_fdir_fzone) const
    {
        return id == m_fdir_fzone.id;
    }
    int operator < (const CLS_FDIR_FZONE& m_fdir_fzone) const
    {
        return id < m_fdir_fzone.id;
    }

    void GetKey(char *key) const {sprintf(key, "%ld", id);}
    bool IsRTable() const {return false;}	//�ж��Ƿ��ϵ��
    const long GetParentId() const
    {
    	printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
    	return -1;
    }

    const long GetChildId() const
    {
    	printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
    	return -1;
    }

    ~CLS_FDIR_FZONE(){}
};

class CLS_FDIR_ZONE
{
public:
    long id;
    char descr[MAX_CLS_FDIR_ZONE_DESCR_LEN];
    int type;//������������
    int sub;// ���������豸���ͣ����豸��������
	int icb;// ���뱾���ε����Ŀ��������
    int cbnum;//���ظ���
    int cbptr;//�����κ͵�һ���������أ��ڹ����������ţ�
    int q;
	int topflags;
	long zn_id;	//��FDIR��ZN��DMS��ZN��������

public:
    CLS_FDIR_ZONE();
    CLS_FDIR_ZONE(const long m_id);
    const CLS_FDIR_ZONE& operator = (const CLS_FDIR_ZONE& aCLS_FDIR_ZONE);
    int operator == (const CLS_FDIR_ZONE& m_fdir_zone) const
    {
        return id == m_fdir_zone.id;
    }
    int operator < (const CLS_FDIR_ZONE& m_fdir_zone) const
    {
        return id < m_fdir_zone.id;
    }

    void GetKey(char *key) const {sprintf(key, "%ld", id);}
    bool IsRTable() const {return false;}	//�ж��Ƿ��ϵ��
	const long GetParentId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

    ~CLS_FDIR_ZONE(){}
};

class CLS_FDIR_ZONE_CB
{
public:
    long id;
    int icb;

public:
    CLS_FDIR_ZONE_CB();
    CLS_FDIR_ZONE_CB(const long m_id);
    const CLS_FDIR_ZONE_CB& operator = (const CLS_FDIR_ZONE_CB& aCLS_FDIR_ZONE_CB);
    int operator == (const CLS_FDIR_ZONE_CB& m_fdir_zone_cb) const
    {
        return id == m_fdir_zone_cb.id;
    }
    int operator < (const CLS_FDIR_ZONE_CB& m_fdir_zone_cb) const
    {
        return id < m_fdir_zone_cb.id;
    }

    void GetKey(char *key) const {sprintf(key, "%ld", id);}
    bool IsRTable() const {return false;}	//�ж��Ƿ��ϵ��
	const long GetParentId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

    ~CLS_FDIR_ZONE_CB(){}
};

class CLS_FDIR_RESTORE
{
public:
    int id;
    char descr[MAX_FDIR_RESTORE_DESCR_LEN];
    int icb;
    long cb;
    char cb_desc[MAX_CB_DESCR_LEN];
    int q;
    long so_id;

public:
    CLS_FDIR_RESTORE();
    CLS_FDIR_RESTORE(const int m_id);
    const CLS_FDIR_RESTORE& operator = (const CLS_FDIR_RESTORE& aFDIR_RESTORE);
    int operator == (const CLS_FDIR_RESTORE& m_fdir_isolate) const
    {
        return id == m_fdir_isolate.id;
    }
    int operator < (const CLS_FDIR_RESTORE& m_fdir_isolate) const
    {
        return id < m_fdir_isolate.id;
    }

    ~CLS_FDIR_RESTORE(){}
};

class CLS_FDIR_ISOLATE
{
public:
    int id;
    char descr[MAX_FDIR_ISOLATE_DESCR_LEN];
    int icb;
    long cb;
    char cb_desc[MAX_CB_DESCR_LEN];
    int q;

public:
    CLS_FDIR_ISOLATE();
    CLS_FDIR_ISOLATE(const int m_id);
    const CLS_FDIR_ISOLATE& operator = (const CLS_FDIR_ISOLATE& aFDIR_ISOLATE);
    int operator == (const CLS_FDIR_ISOLATE& m_fdir_isolate) const
    {
        return id == m_fdir_isolate.id;
    }
    int operator < (const CLS_FDIR_ISOLATE& m_fdir_isolate) const
    {
        return id < m_fdir_isolate.id;
    }

    void GetKey(char *key) const {sprintf(key, "%d", id);}
    bool IsRTable() const {return false;}	//�ж��Ƿ��ϵ��
	const long GetParentId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!���棺�Էǹ�ϵ����û�ȡ��ID,line:%d\r\n", __LINE__);
		return -1;
	}

    ~CLS_FDIR_ISOLATE(){}
};

#endif
