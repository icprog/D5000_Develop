/*
 * comm.h
 *
 *  Created on: 2011-10-24
 *      Author: BetaKDE
 */

#ifndef COMM_H_
#define COMM_H_
#include <scada_api/scada_event_m.h>
#include <list>

typedef struct PDR_POINT_TRIG_EVT
{
        long            point_name;
        unsigned        char type;
}PDR_POINT_TRIG_EVT;

class FDIR_R2;
//extern FDIR_R2 *g_fdir_r;
//extern DSBOB *g_bob;

#define VERSION_STUDY		1		//研究态
#define VERSION_REALTIME	0		//实时态

#define FAULT_ID_LENGTH		50
#define CB_ID_LENGTH		21
#define DV_ID_LENGTH		21
#define ST_ID_LENGTH		21

#define FDIR_MSG_ISO		1	//故障隔离事件
#define FDIR_MSG_REC		2	//故障恢复事件
#define FDIR_MSG_CTRLCB_ISO	3	//故障隔离CB控制命令
#define FDIR_MSG_CTRLCB_REC	4	//故障恢复CB控制命令
#define FDIR_MSG_SET_AUTO	5	//手动转自动指令

#define CHECK_DATA_OK			0	//校验数据通过
#define CHECK_DATA_PSBOB_ERR	1	//版本号不一致
#define CHECK_DATA_CB_ERR		2	//故障相关开关状态变化
#define CHECK_DATA_READ_ERR		3	//读库相关数据失败
#define CHECK_DATA_ID_ERR		4	//要校验的故障组id错误

extern int version;	//0:真实态   1：模拟态
/*
 *人机给后台发送数据用到的数据结构
 */
typedef struct _HEAD
{
	_HEAD()
		{
		type = 0;
		size = 0;
			bzero(fault_id, sizeof(fault_id));
			bzero(group_id, sizeof(group_id));
		}

	int type;			//命令类型
	int size;			//数据包大小
	char fault_id[FAULT_ID_LENGTH];	//故障ID
	char group_id[FAULT_ID_LENGTH]; //组ID
}HEAD, *PHEAD;

typedef HEAD	FDIR_DATA_AUTO, *PFDIR_DATA_AUTO;

typedef struct
{
	HEAD head;
	char cb_id[CB_ID_LENGTH];	//CB的ID
	int  icb;					//CB的逻辑号
	int  op_type;				//要执行的CB操作（开0、合1）
}FDIR_DATA_CTRLCB, *PFDIR_DATA_CTRLCB;

enum ALARM_DEV {ALARM_BREAKER, ALARM_BUS, ALARM_SGZ, ALARM_FAULTINFO_DL, ALARM_FAULTINFO_JD};

struct ALARM
{
	int type;
	time_t occur_time;
	short msec;
	unsigned long key_id;
	int respid;
	long st_id;
	long bayid;
	int status;
	string str;
	string name;
	long bvid;
	char tabname[30];
	char fault_info[30];
	ALARM_DEV dev;
    time_t gtime;            //报警发生秒级时刻
    time_t gmtime;           //报警发生时刻毫秒数
};

//过流信号
typedef struct
{
	ALARM alarm;

	union
	{
		PSBOB::CB point;
		PSBOB::BUS analog;
		PSBOB::FI	faultinfo;
	} psbob_data;

	list<string> lst_fault;
	timeval	tm_add;
	long 	dv_id;
	int 	damode;
	int		trig_type;			//添加过流信号属性，分为模拟过流信号和真实故障信号两种
	std::string simu_plan_id;	//模拟过流信号的方案id
} FDIR_POINT_TRIG;

typedef std::list<FDIR_POINT_TRIG> FDIR_POINT_TRIG_LIST, *PFDIR_POINT_TRIG_LIST;
typedef std::list<FDIR_POINT_TRIG*> FDIR_POINT_TRIG_SECTION, *PFDIR_POINT_TRIG_SECTION;


#if 0	//迁移到psbobevt.h头文件中定义
//故障隔离、恢复RTE事件体
typedef struct
{
//	HEAD head;
	char fault_id[FAULT_ID_LENGTH];	//故障ID
	char dv_id[DV_ID_LENGTH];
	char st_id[ST_ID_LENGTH];
}FDIR_MSG_ISO_REC, *PFDIR_MSG_ISO_REC;
#endif
/*
 * 待处理任务队列定义
 * 队列里的任务包括：
 * >开关遥控请求
 * >故障启动、定位、隔离、恢复
 * >手动转自动指令
 * >
 */
#define TASK_TYPE_CB_CTRL	1
#define TASK_TYPE_FAULT		2
#define TASK_TYPE_SET_AUTO	3

//故障项
#define TASK_FAULT_STATUS_WAIT				1 	//故障待启动
#define TASK_FAULT_STATUS_CALC				2	//待定位、计算隔离恢复
#define TASK_FAULT_STATUS_ISO				3	//待隔离
#define TASK_FAULT_STATUS_PROCESS			4	//正在隔离恢复
#define TASK_FAULT_STATUS_SLEEP				5	//休眠等待过流信号
#define TASK_FAULT_STATUS_CALC_ISO			6   //故障正在等待分组
#define TASK_FAULT_STATUS_GROUPED			7 	//分组完成，开始计算方案
#define TASK_FAULT_STATUS_CALC_STEP 		8	//正在计算故障的隔离、恢复方案
#define TASK_FAULT_STATUS_RE_CALC			9 	//遥控制失败，重新计算隔离恢复方案
#define TASK_FAULT_STATUS_INVALID_RECALC	10	//校验数据失效后，重新计算隔离恢复方案

//故障处理模式
#define FDIR_MODE_FORBID	0 //禁止处理故障
#define FDIR_MODE_AUTO		1 //自动处理故障
#define FDIR_MODE_HAND		2 //手动处理故障

#define TASK_FAULT_BUS			1	//母线电压越下限
#define TASK_FAULT_LD			2	//负荷开关跳闸
#define TASK_FAULT_JXCB			3	//进线开关重合闸
#define TASK_FAULT_FI			4	//故障指示器有故障信号启动故障处理
#define TASK_FAULT_STUDY		5	//学习态

#define TASK_CTL_STATUS_WAIT	1	//待执行
#define TASK_CTL_STATUS_RUN		2	//正在执行
#define TASK_CTL_STATUS_FINISH	3	//完成

//////////////////////////////////////
#define STATUS_FDIRECT_DIRECT_FAIL	0	//定位失败
#define STATUS_FDIRECT_DIRECT_OK	1	//定位成功
#define STATUS_FDIRECT_ISO			2	//正在隔离
#define STATUS_FDIRECT_RES			3	//正在恢复
#define STATUS_FDIRECT_FINISH		4 	//处理完成
#define STATUS_FDIRECT_STOP			5	//人工停止
#define STATUS_FDIRECT_RE_ISO		6	//重新隔离
#define STATUS_FDIRECT_RE_TRANSFER	7	//重新转供
#define STATUS_FDIRECT_MANPOSER     8   //人工处理完成
#define STATUS_FDIRECT_WINK			9	//瞬时故障
#define STATUS_FDIRECT_POSSIBLE		10	//可能存在故障
#define STATUS_FDIRECT_NONE			11	//误报
#define STATUS_FDIRECT_DOK_RFAIL    12  //故障定位、隔离成功，计算恢复失败
#define STATUS_FDIRECT_WINK_NO		13  //瞬时故障转历史
#define STATUS_FDIRECT_POSS_NO      14  //可能故障非真实故障
#define STATUS_FDIRECT_NONE_NO		15  //误报故障非真实故障
#define STATUS_FDIRECT_INVALID		16	//该故障方案已经失效
#define STATUS_FDIRECT_DOK_ISFAIL	17	//故障定位成功，计算隔离方案失败
#define STATUS_FDIRECT_VERSION_ERR	18	//psbob版本号变化隔离和恢复策略失效
#define STATUS_FDIRECT_RE_CALC		19	//故障方案失效，重新计算隔离和恢复方案
#define STATUS_FDIRECT_UNKOWN       -1  //未知的执行状态


#define	STATUS_DZONE_WAIT			0	//未执行
#define STATUS_DZONE_EXEC			1	//正在执行中
#define STATUS_DZONE_OK				2	//执行成功
#define STATUS_DZONE_FAIL			3	//执行失败

#define	STATUS_ISOLATE_WAIT			0	//未执行
#define STATUS_ISOLATE_EXEC			1	//正在执行中
#define STATUS_ISOLATE_OK			2	//执行成功
#define STATUS_ISOLATE_FAIL			3	//执行失败

#define STATUS_RESTORE_NONE			0	//无意义
#define STATUS_RESTORE_EXEC			1	//被选中正在执行的恢复路径
#define STATUS_RESTORE_GIVE_UP		2	//被放弃的路径
#define STATUS_RESTORE_OK			3	//执行成功的路径
#define STATUS_RESTORE_FAIL			4	//执行过程失败的

#define	STATUS_RESTORE_STEP_WAIT	0	//未执行
#define STATUS_RESTORE_STEP_EXEC	1	//正在执行中
#define STATUS_RESTORE_STEP_OK		2	//执行成功
#define STATUS_RESTORE_STEP_FAIL	3	//执行失败

#define	STATUS_FDIR_STEP_WAIT	0	//未执行
#define STATUS_FDIR_STEP_EXEC	1	//正在执行中
#define STATUS_FDIR_STEP_FAIL	2	//执行失败
#define STATUS_FDIR_STEP_OK		3	//执行成功
#define STATUS_FDIR_STEP_INVALID	4	//已经失效
#define STATUS_FDIR_STEP_PASS	5	//被跳过

#define STAGE_FDIR_STEP_ISOLATE		1	//正在隔离
#define STAGE_FDIR_STEP_RE_ISOLATE	2	//重新隔离
#define STAGE_FDIR_STEP_CUT_LD		3	//减负荷
#define STAGE_FDIR_STEP_TRANSFER	4	//转供
#define STAGE_FDIR_STEP_RESTORE		5	//合上游跳闸开关

#define EXT_STATUS_FDIRECT_WAIT		0	//未执行
#define EXT_STATUS_FDIRECT_EXEC		1	//执行中
#define EXT_STATUS_FDIRECT_SUCCEED	2	//执行成功
#define EXT_STATUS_FDIRECT_FAILED	3	//执行失败
#define EXT_STATUS_FDIRECT_RECALC	4	//重新隔离、重新转供

#define TRIG_TYPE_SIMU				2	//模拟出的故障信号
#define SIMU_PLAN_PERMANENT			0	//故障模拟永久方案
#define SIMU_PLAN_TEMP				1	//临时故障模拟方案

class FDIR_TASK_DATA_FAULT
{
public:
	FDIR_TASK_DATA_FAULT()
	{
		point_name = -1;
		dv = 0;
		st = 0;
		fail_point = 0;
		bAuto = false;
		next_step = 0;
		type = 0;
		fault_zone = -1;
		fdir_r = NULL;
		signal_type = 0;
		simu_plan_id = "";
	}
	HEAD head;
	int bAuto;			//是否是自动隔离、恢复模式
	int next_step;		//下一步要执行的操作（故障启动、定位、隔离、恢复)
	long point_name;	//跳闸、重合闸及再次跳闸的进线开关遥信ID
	long dv;			//故障启动信号所在的馈线
	long st;			//故障启动信号所在的厂站
	long fail_point; 	//执行时执行失败的开关id，仅保存最近执行失败的开关
	int type; //RTE启动方式：负荷开关跳闸，进线开关重合闸，母线电压越下限
	int fault_zone;
	FDIR_R2 *fdir_r;
	FDIR_POINT_TRIG_LIST	trig_list;	//本故障用到的故障信号列表	
	std::string simu_plan_id;	//模拟故障信号对应的方案ID
	int signal_type;					//故障类型
};

//遥控
typedef struct FDIR_TASK_DATA_CONTROL
{
	FDIR_TASK_DATA_CONTROL()
	{
		bzero(this, sizeof(*this));
	}
	HEAD head;
	long point_name;
	bool bOpen;
	bool bLast; //所有隔离恢复STEP的最后一步
	int  stage;//遥控所属的阶段（隔离、恢复等）
	int status;	//状态：等待执行，正在执行，完成
}FDIR_TASK_DATA_CONTROL;

//设置自动
typedef struct FDIR_TASK_DATA_SET_AUTO
{
	FDIR_TASK_DATA_SET_AUTO()
	{
		bzero(this, sizeof(*this));
	}

	HEAD head;
	long point_name;
	bool bOpen;
}FDIR_TASK_DATA_SET_AUTO;

typedef enum{
	NO_POINT = 1, 		//遥信不存在
	NO_AUTO, 		//不能自动跳闸
	NO_CONTROL, 	//不可遥控
	NO_VALUE, 		//状态不采集
	NO_UPDATE		//老数据
} CB_INFO_TYPE;

typedef struct
{
	char id[FAULT_ID_LENGTH];				//故障标识符
	long cb_id;								//开关ID
	long pt_id;								//开关对应的遥信的ID
	char cb_descr[MAX_CB_DESCR_LEN];
	int cb_q;								//开关的q属性值
	int cb_status;							//开关的status属性值
	int pt_control;							//开关对应遥信control属性值
	time_t pt_timexc;						//开关对应遥信timexc属性值
	int pt_quality;							//开关对应遥信quality属性值
	CB_INFO_TYPE reason;					//状态
}FDIR_CB_INFO, *PFDIR_CB_INFO;

typedef std::list<long>	FDIR_SEC_TRIG_LIST, *PFDIR_SEC_TRIG_LIST;

class FDIR_PROCESSOR
{
public:
	FDIR_PROCESSOR()
	{

	}

	~FDIR_PROCESSOR()
	{

	}

	FDIR_R2 *GetProcessor()
	{
		return NULL;//g_fdir_r;
	}
};

class FDIR_TASK
{
public:
	FDIR_TASK()
	{
		bzero(&tv_creat, sizeof(tv_creat));
		bzero(&tv_start, sizeof(tv_start));
		bzero(&tv_end, sizeof(tv_end));
		status= 0;
		rq_type = 0;
	}
#if 0
	FDIR_TASK()
	{
		fdir_r =  NULL;//new FDIR_R(pdsbob);
	}

	~FDIR_TASK()
	{
		if(fdir_r != NULL)
		{
			cout << "清除故障处理器。" << endl;
			delete fdir_r;
		}
	}

	void build_fdir(DSBOB * pdsbob)
	{
		fdir_r =  new FDIR_R(pdsbob);
	}
	FDIR_R *fdir_r;
#endif
//	FDIR_PROCESSOR processor;
	int rq_type; //任务类型
	struct FDIR_TASK_DATA
	{
		FDIR_TASK_DATA_FAULT fault;
		FDIR_TASK_DATA_CONTROL control;
		FDIR_TASK_DATA_SET_AUTO set_auto;
	}task_data;
	timeval tv_creat; //添加到待处理列表的时间
	timeval tv_start; //开始处理时间
	timeval tv_end; //处理完成时间
	int status; //处理状态（待处理、处理中、完成）
protected:
private:
};

typedef std::list<FDIR_TASK>  FDIR_TASK_LIST, *PFDIR_TASK_LIST;

typedef struct
{
	int FDIR_MODE;			//故障处理模式	0-禁止，1-自动，2-手动
	int USER2AUTO;			//允许手动超时转自动  0不允许，1允许
	int TIMEOUT_CONTROL;	//等待遥控结果超时时间
	int TIMEOUT_FAULT;		//故障启动超时
	int TIMEOUT_PROTPNT;	//等待过流信息超时
	int TIMEOUT_JD_FAULT;   //接地信号等待不对称电流源时间
	int TIMEOUT_AUTO;		//手动超时转自动
	int SIMULATE;			//模拟遥控
	int AUTO_DEAL_HIS;		//自动归档，0-禁止，1-自动归档，故障定位隔离恢复计算完成后写入历史库
	char OP_USER[50];		//后台自动执行遥控所使用的用户
	char OP_NODE[50];		//后台自动执行遥控所使用的节点
}FDIR_CONFIG, *PFDIR_CONFIG;

//科东图形事件报文结构
struct  TScaBaseOp
{
	long	op_time;			//操作时间
	char	op_user_name[64];	//操作人员名称
	char	op_node_name[64];	//操作机器节点名称
	char	gua_user_name[64];	//监护人员名称
	char	gua_node_name[64];	//监护机器节点名称
	long	id;					//操作对象ID
	int		data_type;			//数据类型
	int		int_value;			//int\short\char\UCHAR
	float	float_value; 		//float
	long	long_value;			//long\time_t
};

struct TScaOpAnswer
{
	long key_id;
	long time;
	char ctrl_result;    //控制结果，0==成功；其他值表示错误
};

//挂牌事件体
struct TScaTokenOp
{
	long op_time;                       //操作时间
	char op_user_name[64];   			//操作人员名称
	char op_node_name[64];   			//操作机器节点名称
	long dev_id;             			//挂牌设备ID
	long token_id;           			//标志牌ID
	long token_type_id;      			//标志牌类型ID
	long father_token_id;    			//父标志牌ID
	int pos_x;              			//x坐标
	int pos_y;              			//y坐标
	char graph_name[128];    			//图形名称
	char note[128];              		//操作说明
};

#define  EVPS_FDIR_MSG_ISO_REC   1111

typedef struct FDIR_MSG_ISO_REC
{
 	char fault_id[FAULT_ID_LENGTH]; //故障ID
	long dv_id;
	long st_id;
	char operate;	//操作 0：故障处理完成
}FDIR_MSG_ISO_REC;


typedef struct
{
	union
	{
		PSBOB::CB cb;
		PSBOB::LD ld;
		PSBOB::BUS bus;
		PSBOB::SEC sec;
		PSBOB::SWITCH disconnector;
	} data;

	int type;
} NODE, *PNODE;

//简易内存自动管理类
class MyBufManager
{
	typedef void* 			PVOID;
	typedef PSBOB::CB*   	PCB;
	typedef PSBOB::LD*		PLD;
	typedef PSBOB::SEC*		PSEC;
	typedef PSBOB::BUS*		PBUS;
	typedef PSBOB::SWITCH*	PSWITCH;
public:
	MyBufManager(int size)
	{
		p = malloc(size);
	}
	~MyBufManager()
	{
		free(p);
	}
	operator PVOID() const
	{
		return (PVOID)p;
	}
	operator PCB() const
	{
		return (PCB)p;
	}
	operator PSEC() const
	{
		return (PSEC)p;
	}
	operator PBUS() const
	{
		return (PBUS)p;
	}
	operator PLD() const
	{
		return (PLD)p;
	}
	operator PSWITCH() const
	{
		return (PSWITCH)p;
	}

protected:
	void *p;
};
#endif /* COMM_H_ */
