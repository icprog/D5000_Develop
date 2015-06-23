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
#define FDIR_CB_FDHCB				16//馈线首端开关
#define FDIR_CB_FDSCB				32
#define FDIR_CB_FDCCB				64
#define FDIR_CB_FDLCB				128//馈线负荷开关
#define FDIR_CB_NOAVAIL				256

#define FDIR_CB_DEAD				1
#define FDIR_CB_DEAD0				2
#define FDIR_CB_FAULT				4
#define FDIR_CB_FLASH				8
#define FDIR_CB_OPEN0				16
#define FDIR_CB_OPEN				32
//以下三个定义用作故障定位时计算故障区段 by rongyiping 20130319
#define FDIR_CB_DETECT_OPEN				64	//可能是事故总信号漏报而没有事故跳闸报警的分闸开关
#define FDIR_CB_DETECT_FAULT			128	//事故跳闸开关
#define FDIR_CB_DETECT_FI				512	//故障指示器动作的开关
#define FDIR_CB_BEFORE_OPEN				4096//半小时之内同一馈线发生的分闸或者事故跳闸的开关
#define FDIR_BUS_DETECT				    8192	//母线故障信息
#define FDIR_FAULT_HEAD					1024	//该设备为故障起始设备
#define FDIR_FAULT_TAIL					2048	//该设备为故障结束设备

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
    int ind;// 首端区段号
    int iznd;//末端区段号
    int q;
	int topflags;
	long cb_id;	//让FDIR库CB与DMS库CB建立关联

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
    bool IsRTable() const {return false;}	//判断是否关系表
	const long GetParentId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取父ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取子ID,line:%d\r\n", __LINE__);
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
    bool IsRTable() const {return false;}	//判断是否关系表
    const long GetParentId() const
    {
    	printf("!!!!!!!!警告：对非关系表调用获取父ID,line:%d\r\n", __LINE__);
    	return -1;
    }

    const long GetChildId() const
    {
    	printf("!!!!!!!!警告：对非关系表调用获取子ID,line:%d\r\n", __LINE__);
    	return -1;
    }

    ~CLS_FDIR_FZONE(){}
};

class CLS_FDIR_ZONE
{
public:
    long id;
    char descr[MAX_CLS_FDIR_ZONE_DESCR_LEN];
    int type;//区段所在类型
    int sub;// 区段所在设备类型，在设备表的物理号
	int icb;// 流入本区段电流的开关物理号
    int cbnum;//开关个数
    int cbptr;//本区段和第一个关联开关，在关联表的物理号；
    int q;
	int topflags;
	long zn_id;	//让FDIR库ZN与DMS库ZN建立关联

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
    bool IsRTable() const {return false;}	//判断是否关系表
	const long GetParentId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取父ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取子ID,line:%d\r\n", __LINE__);
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
    bool IsRTable() const {return false;}	//判断是否关系表
	const long GetParentId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取父ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取子ID,line:%d\r\n", __LINE__);
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
    bool IsRTable() const {return false;}	//判断是否关系表
	const long GetParentId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取父ID,line:%d\r\n", __LINE__);
		return -1;
	}

	const long GetChildId() const
	{
		printf("!!!!!!!!警告：对非关系表调用获取子ID,line:%d\r\n", __LINE__);
		return -1;
	}

    ~CLS_FDIR_ISOLATE(){}
};

#endif
