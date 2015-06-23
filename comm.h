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

#define VERSION_STUDY		1		//�о�̬
#define VERSION_REALTIME	0		//ʵʱ̬

#define FAULT_ID_LENGTH		50
#define CB_ID_LENGTH		21
#define DV_ID_LENGTH		21
#define ST_ID_LENGTH		21

#define FDIR_MSG_ISO		1	//���ϸ����¼�
#define FDIR_MSG_REC		2	//���ϻָ��¼�
#define FDIR_MSG_CTRLCB_ISO	3	//���ϸ���CB��������
#define FDIR_MSG_CTRLCB_REC	4	//���ϻָ�CB��������
#define FDIR_MSG_SET_AUTO	5	//�ֶ�ת�Զ�ָ��

#define CHECK_DATA_OK			0	//У������ͨ��
#define CHECK_DATA_PSBOB_ERR	1	//�汾�Ų�һ��
#define CHECK_DATA_CB_ERR		2	//������ؿ���״̬�仯
#define CHECK_DATA_READ_ERR		3	//�����������ʧ��
#define CHECK_DATA_ID_ERR		4	//ҪУ��Ĺ�����id����

extern int version;	//0:��ʵ̬   1��ģ��̬
/*
 *�˻�����̨���������õ������ݽṹ
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

	int type;			//��������
	int size;			//���ݰ���С
	char fault_id[FAULT_ID_LENGTH];	//����ID
	char group_id[FAULT_ID_LENGTH]; //��ID
}HEAD, *PHEAD;

typedef HEAD	FDIR_DATA_AUTO, *PFDIR_DATA_AUTO;

typedef struct
{
	HEAD head;
	char cb_id[CB_ID_LENGTH];	//CB��ID
	int  icb;					//CB���߼���
	int  op_type;				//Ҫִ�е�CB��������0����1��
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
    time_t gtime;            //���������뼶ʱ��
    time_t gmtime;           //��������ʱ�̺�����
};

//�����ź�
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
	int		trig_type;			//��ӹ����ź����ԣ���Ϊģ������źź���ʵ�����ź�����
	std::string simu_plan_id;	//ģ������źŵķ���id
} FDIR_POINT_TRIG;

typedef std::list<FDIR_POINT_TRIG> FDIR_POINT_TRIG_LIST, *PFDIR_POINT_TRIG_LIST;
typedef std::list<FDIR_POINT_TRIG*> FDIR_POINT_TRIG_SECTION, *PFDIR_POINT_TRIG_SECTION;


#if 0	//Ǩ�Ƶ�psbobevt.hͷ�ļ��ж���
//���ϸ��롢�ָ�RTE�¼���
typedef struct
{
//	HEAD head;
	char fault_id[FAULT_ID_LENGTH];	//����ID
	char dv_id[DV_ID_LENGTH];
	char st_id[ST_ID_LENGTH];
}FDIR_MSG_ISO_REC, *PFDIR_MSG_ISO_REC;
#endif
/*
 * ������������ж���
 * ����������������
 * >����ң������
 * >������������λ�����롢�ָ�
 * >�ֶ�ת�Զ�ָ��
 * >
 */
#define TASK_TYPE_CB_CTRL	1
#define TASK_TYPE_FAULT		2
#define TASK_TYPE_SET_AUTO	3

//������
#define TASK_FAULT_STATUS_WAIT				1 	//���ϴ�����
#define TASK_FAULT_STATUS_CALC				2	//����λ���������ָ�
#define TASK_FAULT_STATUS_ISO				3	//������
#define TASK_FAULT_STATUS_PROCESS			4	//���ڸ���ָ�
#define TASK_FAULT_STATUS_SLEEP				5	//���ߵȴ������ź�
#define TASK_FAULT_STATUS_CALC_ISO			6   //�������ڵȴ�����
#define TASK_FAULT_STATUS_GROUPED			7 	//������ɣ���ʼ���㷽��
#define TASK_FAULT_STATUS_CALC_STEP 		8	//���ڼ�����ϵĸ��롢�ָ�����
#define TASK_FAULT_STATUS_RE_CALC			9 	//ң����ʧ�ܣ����¼������ָ�����
#define TASK_FAULT_STATUS_INVALID_RECALC	10	//У������ʧЧ�����¼������ָ�����

//���ϴ���ģʽ
#define FDIR_MODE_FORBID	0 //��ֹ�������
#define FDIR_MODE_AUTO		1 //�Զ��������
#define FDIR_MODE_HAND		2 //�ֶ��������

#define TASK_FAULT_BUS			1	//ĸ�ߵ�ѹԽ����
#define TASK_FAULT_LD			2	//���ɿ�����բ
#define TASK_FAULT_JXCB			3	//���߿����غ�բ
#define TASK_FAULT_FI			4	//����ָʾ���й����ź��������ϴ���
#define TASK_FAULT_STUDY		5	//ѧϰ̬

#define TASK_CTL_STATUS_WAIT	1	//��ִ��
#define TASK_CTL_STATUS_RUN		2	//����ִ��
#define TASK_CTL_STATUS_FINISH	3	//���

//////////////////////////////////////
#define STATUS_FDIRECT_DIRECT_FAIL	0	//��λʧ��
#define STATUS_FDIRECT_DIRECT_OK	1	//��λ�ɹ�
#define STATUS_FDIRECT_ISO			2	//���ڸ���
#define STATUS_FDIRECT_RES			3	//���ڻָ�
#define STATUS_FDIRECT_FINISH		4 	//�������
#define STATUS_FDIRECT_STOP			5	//�˹�ֹͣ
#define STATUS_FDIRECT_RE_ISO		6	//���¸���
#define STATUS_FDIRECT_RE_TRANSFER	7	//����ת��
#define STATUS_FDIRECT_MANPOSER     8   //�˹��������
#define STATUS_FDIRECT_WINK			9	//˲ʱ����
#define STATUS_FDIRECT_POSSIBLE		10	//���ܴ��ڹ���
#define STATUS_FDIRECT_NONE			11	//��
#define STATUS_FDIRECT_DOK_RFAIL    12  //���϶�λ������ɹ�������ָ�ʧ��
#define STATUS_FDIRECT_WINK_NO		13  //˲ʱ����ת��ʷ
#define STATUS_FDIRECT_POSS_NO      14  //���ܹ��Ϸ���ʵ����
#define STATUS_FDIRECT_NONE_NO		15  //�󱨹��Ϸ���ʵ����
#define STATUS_FDIRECT_INVALID		16	//�ù��Ϸ����Ѿ�ʧЧ
#define STATUS_FDIRECT_DOK_ISFAIL	17	//���϶�λ�ɹ���������뷽��ʧ��
#define STATUS_FDIRECT_VERSION_ERR	18	//psbob�汾�ű仯����ͻָ�����ʧЧ
#define STATUS_FDIRECT_RE_CALC		19	//���Ϸ���ʧЧ�����¼������ͻָ�����
#define STATUS_FDIRECT_UNKOWN       -1  //δ֪��ִ��״̬


#define	STATUS_DZONE_WAIT			0	//δִ��
#define STATUS_DZONE_EXEC			1	//����ִ����
#define STATUS_DZONE_OK				2	//ִ�гɹ�
#define STATUS_DZONE_FAIL			3	//ִ��ʧ��

#define	STATUS_ISOLATE_WAIT			0	//δִ��
#define STATUS_ISOLATE_EXEC			1	//����ִ����
#define STATUS_ISOLATE_OK			2	//ִ�гɹ�
#define STATUS_ISOLATE_FAIL			3	//ִ��ʧ��

#define STATUS_RESTORE_NONE			0	//������
#define STATUS_RESTORE_EXEC			1	//��ѡ������ִ�еĻָ�·��
#define STATUS_RESTORE_GIVE_UP		2	//��������·��
#define STATUS_RESTORE_OK			3	//ִ�гɹ���·��
#define STATUS_RESTORE_FAIL			4	//ִ�й���ʧ�ܵ�

#define	STATUS_RESTORE_STEP_WAIT	0	//δִ��
#define STATUS_RESTORE_STEP_EXEC	1	//����ִ����
#define STATUS_RESTORE_STEP_OK		2	//ִ�гɹ�
#define STATUS_RESTORE_STEP_FAIL	3	//ִ��ʧ��

#define	STATUS_FDIR_STEP_WAIT	0	//δִ��
#define STATUS_FDIR_STEP_EXEC	1	//����ִ����
#define STATUS_FDIR_STEP_FAIL	2	//ִ��ʧ��
#define STATUS_FDIR_STEP_OK		3	//ִ�гɹ�
#define STATUS_FDIR_STEP_INVALID	4	//�Ѿ�ʧЧ
#define STATUS_FDIR_STEP_PASS	5	//������

#define STAGE_FDIR_STEP_ISOLATE		1	//���ڸ���
#define STAGE_FDIR_STEP_RE_ISOLATE	2	//���¸���
#define STAGE_FDIR_STEP_CUT_LD		3	//������
#define STAGE_FDIR_STEP_TRANSFER	4	//ת��
#define STAGE_FDIR_STEP_RESTORE		5	//��������բ����

#define EXT_STATUS_FDIRECT_WAIT		0	//δִ��
#define EXT_STATUS_FDIRECT_EXEC		1	//ִ����
#define EXT_STATUS_FDIRECT_SUCCEED	2	//ִ�гɹ�
#define EXT_STATUS_FDIRECT_FAILED	3	//ִ��ʧ��
#define EXT_STATUS_FDIRECT_RECALC	4	//���¸��롢����ת��

#define TRIG_TYPE_SIMU				2	//ģ����Ĺ����ź�
#define SIMU_PLAN_PERMANENT			0	//����ģ�����÷���
#define SIMU_PLAN_TEMP				1	//��ʱ����ģ�ⷽ��

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
	int bAuto;			//�Ƿ����Զ����롢�ָ�ģʽ
	int next_step;		//��һ��Ҫִ�еĲ�����������������λ�����롢�ָ�)
	long point_name;	//��բ���غ�բ���ٴ���բ�Ľ��߿���ң��ID
	long dv;			//���������ź����ڵ�����
	long st;			//���������ź����ڵĳ�վ
	long fail_point; 	//ִ��ʱִ��ʧ�ܵĿ���id�����������ִ��ʧ�ܵĿ���
	int type; //RTE������ʽ�����ɿ�����բ�����߿����غ�բ��ĸ�ߵ�ѹԽ����
	int fault_zone;
	FDIR_R2 *fdir_r;
	FDIR_POINT_TRIG_LIST	trig_list;	//�������õ��Ĺ����ź��б�	
	std::string simu_plan_id;	//ģ������źŶ�Ӧ�ķ���ID
	int signal_type;					//��������
};

//ң��
typedef struct FDIR_TASK_DATA_CONTROL
{
	FDIR_TASK_DATA_CONTROL()
	{
		bzero(this, sizeof(*this));
	}
	HEAD head;
	long point_name;
	bool bOpen;
	bool bLast; //���и���ָ�STEP�����һ��
	int  stage;//ң�������Ľ׶Σ����롢�ָ��ȣ�
	int status;	//״̬���ȴ�ִ�У�����ִ�У����
}FDIR_TASK_DATA_CONTROL;

//�����Զ�
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
	NO_POINT = 1, 		//ң�Ų�����
	NO_AUTO, 		//�����Զ���բ
	NO_CONTROL, 	//����ң��
	NO_VALUE, 		//״̬���ɼ�
	NO_UPDATE		//������
} CB_INFO_TYPE;

typedef struct
{
	char id[FAULT_ID_LENGTH];				//���ϱ�ʶ��
	long cb_id;								//����ID
	long pt_id;								//���ض�Ӧ��ң�ŵ�ID
	char cb_descr[MAX_CB_DESCR_LEN];
	int cb_q;								//���ص�q����ֵ
	int cb_status;							//���ص�status����ֵ
	int pt_control;							//���ض�Ӧң��control����ֵ
	time_t pt_timexc;						//���ض�Ӧң��timexc����ֵ
	int pt_quality;							//���ض�Ӧң��quality����ֵ
	CB_INFO_TYPE reason;					//״̬
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
			cout << "������ϴ�������" << endl;
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
	int rq_type; //��������
	struct FDIR_TASK_DATA
	{
		FDIR_TASK_DATA_FAULT fault;
		FDIR_TASK_DATA_CONTROL control;
		FDIR_TASK_DATA_SET_AUTO set_auto;
	}task_data;
	timeval tv_creat; //��ӵ��������б��ʱ��
	timeval tv_start; //��ʼ����ʱ��
	timeval tv_end; //�������ʱ��
	int status; //����״̬�������������С���ɣ�
protected:
private:
};

typedef std::list<FDIR_TASK>  FDIR_TASK_LIST, *PFDIR_TASK_LIST;

typedef struct
{
	int FDIR_MODE;			//���ϴ���ģʽ	0-��ֹ��1-�Զ���2-�ֶ�
	int USER2AUTO;			//�����ֶ���ʱת�Զ�  0������1����
	int TIMEOUT_CONTROL;	//�ȴ�ң�ؽ����ʱʱ��
	int TIMEOUT_FAULT;		//����������ʱ
	int TIMEOUT_PROTPNT;	//�ȴ�������Ϣ��ʱ
	int TIMEOUT_JD_FAULT;   //�ӵ��źŵȴ����ԳƵ���Դʱ��
	int TIMEOUT_AUTO;		//�ֶ���ʱת�Զ�
	int SIMULATE;			//ģ��ң��
	int AUTO_DEAL_HIS;		//�Զ��鵵��0-��ֹ��1-�Զ��鵵�����϶�λ����ָ�������ɺ�д����ʷ��
	char OP_USER[50];		//��̨�Զ�ִ��ң����ʹ�õ��û�
	char OP_NODE[50];		//��̨�Զ�ִ��ң����ʹ�õĽڵ�
}FDIR_CONFIG, *PFDIR_CONFIG;

//�ƶ�ͼ���¼����Ľṹ
struct  TScaBaseOp
{
	long	op_time;			//����ʱ��
	char	op_user_name[64];	//������Ա����
	char	op_node_name[64];	//���������ڵ�����
	char	gua_user_name[64];	//�໤��Ա����
	char	gua_node_name[64];	//�໤�����ڵ�����
	long	id;					//��������ID
	int		data_type;			//��������
	int		int_value;			//int\short\char\UCHAR
	float	float_value; 		//float
	long	long_value;			//long\time_t
};

struct TScaOpAnswer
{
	long key_id;
	long time;
	char ctrl_result;    //���ƽ����0==�ɹ�������ֵ��ʾ����
};

//�����¼���
struct TScaTokenOp
{
	long op_time;                       //����ʱ��
	char op_user_name[64];   			//������Ա����
	char op_node_name[64];   			//���������ڵ�����
	long dev_id;             			//�����豸ID
	long token_id;           			//��־��ID
	long token_type_id;      			//��־������ID
	long father_token_id;    			//����־��ID
	int pos_x;              			//x����
	int pos_y;              			//y����
	char graph_name[128];    			//ͼ������
	char note[128];              		//����˵��
};

#define  EVPS_FDIR_MSG_ISO_REC   1111

typedef struct FDIR_MSG_ISO_REC
{
 	char fault_id[FAULT_ID_LENGTH]; //����ID
	long dv_id;
	long st_id;
	char operate;	//���� 0�����ϴ������
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

//�����ڴ��Զ�������
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
