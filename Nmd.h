////////////////////////////////////////////////////////////////////////////////
//
//  NMD.H
//	DMS project - Copyright (c) 1999, EPRI
//  Class definition for distribution network database validation
//  Developed by Chen Jingcheng, April - 2000
//
////////////////////////////////////////////////////////////////////////////////
#if !defined(__NMD_H)
#define __NMD_H

#include "dsbob2.h"
#include <string>
#include <list>
#include <vector>
#include <assert.h>
#include <strings.h>
#include <stdarg.h>
#include <stdio.h>

#include <stack>
#include <map>

#include "tmptable.h"

using namespace std;

#define SO_AREA	1l
#define CONNECTION	2l

#define ST_ABB_LEN		3
#define ZN_ID_LEN		9

#define NMD_FAIL		0
#define NMD_SUCCEED		1


#define TYPE_CB		0
#define TYPE_LD		1
#define TYPE_SEC	2
#define TYPE_XF		3
#define TYPE_CP		4

#define FIND_TYPE_SO		1
#define FIND_TYPE_SO_AREA	2
#define FIND_TYPE_CONNECT	3
#define FIND_TYPE_BUILD_ZN	4
#define FIND_TYPE_MISOPERAT 5

typedef struct
{
	long id;		//设备ID
	string table;	//设备类型
	long	nd;
}DEV, *PDEV;

typedef stack<DEV> STACK_DEV, *PSTACK_DEV;

typedef struct
{
	int so_id;			//电源所在实时库表中的逻辑号
	STACK_DEV path;		//到该电源的路径
}SO_PATH, *PSO_PATH;

typedef map<long, SO_PATH> MAP_SO_PATH, *PMAP_SO_PATH;	//string为SO的ND

//防误闭锁判断返回结果
typedef struct
{
	list<PSBOB::DV> dv1;	//CB一端的DV列表
	list<PSBOB::DV> dv2;	//CB另一端的DV列表
	list<CLS_LD>	lst_ld1;//CB一端的LD列表
	list<CLS_LD>	lst_ld2;//CB另一端的LD列表
}DATA_MISOPERATION, *PDATA_MISOPERATION;

typedef list<CLS_LD>	LST_LD,*PLST_LD;

////////////////////////////////////////////////////////////////////////////////
//  NMD
class  NMD
{
public:

	//constructor and destructor
	NMD(DSBOB2* m_pDb);
	~NMD();

	//execute validation
	int DoValidate();

	int add_so();
	bool update_cb_state();
	bool find_so(const long id, const char *table, MAP_SO_PATH &paths, int type, PDATA_MISOPERATION pdata_mis = NULL);
public:

	//start validation
	int	ValidateStart();

	//end validation
	int	ValidateEnd();

	//validate st table
	int	ST_Validate();

	//validate zn table
	int	ZN_Validate();

	//validate cb table
	int	CB_Validate();

	//validata switch table
	int SWITCH_Validate();

	//validate bus table
	int BUS_Validate();

	//validate cp table
	int	CP_Validate();

	//validate ld table
	int	LD_Validate();

	//validate so table
	int	SO_Validate();

	//validate sec table
	int SEC_Validate();

	//validate xfmr table
	int	XFMR_Validate();

	//validate xf table
	int	XF_Validate();

	//insert nd table
	int ND_Insert(const long nd_id, int* nd_p, int mask);

	//insert r_nd_br table
	int ND_BR_Insert(int type, int nd_p, const long nd_id, int ibr, const long br_id);

	//reset pointers in tables
	int ResetPtr();

	//reset pointers in co table
	int CO_ResetPtr();

	//reset pointers in st table
	int ST_ResetPtr();

	//reset pointers in xfmr table
	int XFMR_ResetPtr();

	//reset pointers in fd table
	int ZN_ResetPtr();

	//reset pointers in fi table
	int FI_ResetPtr();

	//reset pointers in cb table
	int CB_ResetPtr();

	//reset pointers in cb table
	int SWITCH_ResetPtr();

	//reset pointers in so table
	int SO_ResetPtr();

	//reset pointers in ld table
	int LD_ResetPtr();

	//reset pointers in cp table
	int CP_ResetPtr();

	//reset pointers in bus table
	int BUS_ResetPtr();

	//reset pointers in set table
	int SEC_ResetPtr();

	//build r_co_st table
	int R_CO_ST_Build();

	//build r_st_xfmr table
	int	R_ST_XFMR_Build();

	//build r_st_cb table
	int R_ST_CB_Build();

	//build r_st_switch table
	int R_ST_SWITCH_Build();

	//build r_st_so table
	int R_ST_SO_Build();

	//build r_st_ld table
	int R_ST_LD_Build();

	//build r_st_cp table
	int R_ST_CP_Build();

	//build r_st_bus table
	int R_ST_BUS_Build();

	//build r_st_zn table
	int R_ST_ZN_Build();

	//build r_zn_ld table
	int R_ZN_LD_Build();

	//build r_zn_sec table
	int R_ZN_SEC_Build();

	//build r_zn_nd table
	int R_ZN_ND_Build();

	//build r_sec_fi table
	int R_SEC_FI_Build();

	//rebuild pointers in tables
	int RebuildPtr();

	//rebuild pointers in co table
	int CO_RebuildPtr();

	//rebuild pointers in st table
	int ST_RebuildPtr();

	//rebuild pointers in xfmr table
	int XFMR_RebuildPtr();

	//rebuild pointers in xf table
	int XF_RebuildPtr();

	//rebuild pointers in cb table
	int CB_RebuildPtr();

	//rebuild pointers in cb table
	int SWITCH_RebuildPtr();

	//rebuild pointers in un table
	int SO_RebuildPtr();

	//rebuild pointers in ld table
	int LD_RebuildPtr();

	//rebuild pointers in cp table
	int CP_RebuildPtr();

	//rebuild pointers in bus table
	int BUS_RebuildPtr();

	//rebuild pointers in zn table
	int ZN_RebuildPtr();

	//rebuild pointers in fi table
	int FI_RebuildPtr();

	//rebuild pointers in sec table
	int SEC_RebuildPtr();

	//rebuild pointers in nd table
	int ND_RebuildPtr();

	//rebuild pointers in r_co_st table
	int R_CO_ST_RebuildPtr();

	//rebuild pointers in r_st_xfmr table
	int R_ST_XFMR_RebuildPtr();

	//rebuild pointers in r_xfmr_xf table
	int R_XFMR_XF_RebuildPtr();

	//rebuild pointers in r_st_cb table
	int R_ST_CB_RebuildPtr();

	//rebuild pointers in r_st_switch table
	int R_ST_SWITCH_RebuildPtr();

	//rebuild pointers in r_st_so table
	int R_ST_SO_RebuildPtr();

	//rebuild pointers in r_st_ld table
	int R_ST_LD_RebuildPtr();

	//rebuild pointers in r_st_cp table
	int R_ST_CP_RebuildPtr();

	//rebuild pointers in r_st_bus table
	int R_ST_BUS_RebuildPtr();

	//rebuild pointers in r_st_zn table
	int R_ST_ZN_RebuildPtr();

	//rebuild pointers in r_zn_ld table
	int R_ZN_LD_RebuildPtr();

	//rebuild pointers in r_zn_sec table
	int R_ZN_SEC_RebuildPtr();

	//rebuild pointers in r_nd_cb table
	int R_ND_CB_RebuildPtr();

	//rebuild pointers in r_nd_switch table
	int R_ND_SWITCH_RebuildPtr();

	//rebuild pointers in r_nd_xf table
	int R_ND_XF_RebuildPtr();

	//rebuild pointers in r_nd_zn table
	int R_ZN_ND_RebuildPtr();

	//rebuild pointers in r_nd_sec table
	int R_ND_SEC_RebuildPtr();

	//rebuild pointers in r_nd_sec table
	int R_ND_CP_RebuildPtr();

	//rebuild pointers in r_nd_sec table
	int R_ND_LD_RebuildPtr();

	//rebuild pointers in r_sec_fi table
	int R_SEC_FI_RebuildPtr();

	//build pointers to digital and ananlog table
//	int	DIGITAL_ANALOG_PtrBuild();

	//set vl of nds
	int SetVL();

private:

	//database object to be validated
	DSBOB2*				dsbob;
//	DTable<CLS_DIGITAL>*	digital_tb;
//	DTable<CLS_ANALOG>*		analog_tb;
	DTable<CLS_CO>*			co_tb;
	DTable<CLS_ST>*			st_tb;
	DTable<CLS_XFMR>*		xfmr_tb;
	DTable<CLS_XF>*			xf_tb;
	DTable<CLS_CB>*			cb_tb;
	DTable<CLS_SWITCH>*		switch_tb;
	DTable<CLS_SO>*			so_tb;
	DTable<CLS_LD>*			ld_tb;
	DTable<CLS_CP>*			cp_tb;
	DTable<CLS_BUS>*		bus_tb;
	DTable<CLS_ZN>*			zn_tb;
	DTable<CLS_SEC>*		sec_tb;
	DTable<CLS_ND>*			nd_tb;
	DTable<CLS_BS>*			bs_tb;
	DTable<CLS_ISLAND>*		island_tb;
	DTable<CLS_GLOBAL>*		global_tb;
	DTable<CLS_TAPTY>*		tapty_tb;
	DTable<CLS_R_CO_ST>*	r_co_st_tb;
	DTable<CLS_R_ST_XFMR>*	r_st_xfmr_tb;
	DTable<CLS_R_XFMR_XF>*	r_xfmr_xf_tb;
	DTable<CLS_R_ST_CB>*	r_st_cb_tb;
	DTable<CLS_R_ST_SWITCH>*	r_st_switch_tb;
	DTable<CLS_R_ST_SO>*	r_st_so_tb;
	DTable<CLS_R_ST_LD>*	r_st_ld_tb;
	DTable<CLS_R_ST_CP>*	r_st_cp_tb;
	DTable<CLS_R_ST_BUS>*	r_st_bus_tb;
	DTable<CLS_R_ST_ZN>*	r_st_zn_tb;
	DTable<CLS_R_ZN_LD>*	r_zn_ld_tb;
	DTable<CLS_R_ZN_SEC>*	r_zn_sec_tb;
	DTable<CLS_R_ND_CB>*	r_nd_cb_tb;
	DTable<CLS_R_ND_SWITCH>*	r_nd_switch_tb;
	DTable<CLS_R_ND_XF>*	r_nd_xf_tb;
	DTable<CLS_R_ZN_ND>*	r_zn_nd_tb;
	DTable<CLS_R_ND_SEC>*	r_nd_sec_tb;
	DTable<CLS_R_SEC_FI>*	r_sec_fi_tb;
	DTable<CLS_R_ND_CP>*	r_nd_cp_tb;
	DTable<CLS_R_ND_LD>*	r_nd_ld_tb;
	//张瑞雪添加于20120220
	DTable<CLS_FI>*			fi_tb;
	DTable<CLS_CTL>*		ctl_tb;

private:

	//integer variables for dynamic array size
	int				lv_digital;
	int				lv_analog;
	int				lv_co;
	int				lv_st;
	int				lv_xfmr;
	int				lv_xf;
	int				lv_cb;
	int				lv_switch;
	int				lv_so;
	int				lv_ld;
	int				lv_cp;
	int				lv_bus;
	int				lv_zn;
	int				lv_sec;
	int				lv_nd;
	int				lv_bs;
	int				lv_island;
	int				lv_global;
	int				lv_tapty;
	int				lv_lnty;
	int				lv_r_co_st;
	int				lv_r_st_xfmr;
	int				lv_r_xfmr_xf;
	int				lv_r_st_cb;
	int				lv_r_st_switch;
	int				lv_r_st_so;
	int				lv_r_st_ld;
	int				lv_r_st_cp;
	int				lv_r_st_bus;
	int				lv_r_st_zn;
	int				lv_r_zn_ld;
	int				lv_r_zn_sec;
	int				lv_r_nd_cb;
	int				lv_r_nd_switch;
	int				lv_r_nd_cp;
	int				lv_r_nd_ld;
	int				lv_r_nd_xf;
	int				lv_r_zn_nd;
	int				lv_r_nd_sec;
	int 			lv_fi;
	int				lv_r_sec_fi;

	//pointer variables for dynamic arrays
//	CLS_DIGITAL*		digital;
//	CLS_ANALOG*			analog;
	CLS_CO*				co;
	CLS_ST*				st;
	CLS_XFMR*			xfmr;
	CLS_XF*				xf;
	CLS_CB*				cb;
	CLS_SWITCH*			swh;
	CLS_SO*				so;
	CLS_LD*				ld;
	CLS_CP*				cp;
	CLS_BUS*			bus;
	CLS_ZN*				zn;
	CLS_SEC*			sec;
	CLS_ND*				nd;
	CLS_BS*				bs;
	CLS_ISLAND*			island;
	CLS_GLOBAL*			global;
	CLS_TAPTY*			tapty;
	CLS_R_CO_ST*		r_co_st;
	CLS_R_ST_XFMR*		r_st_xfmr;
	CLS_R_XFMR_XF*		r_xfmr_xf;
	CLS_R_ST_CB*		r_st_cb;
	CLS_R_ST_SWITCH*	r_st_switch;
	CLS_R_ST_SO*		r_st_so;
	CLS_R_ST_LD*		r_st_ld;
	CLS_R_ST_CP*		r_st_cp;
	CLS_R_ST_BUS*		r_st_bus;
	CLS_R_ST_ZN*		r_st_zn;
	CLS_R_ZN_LD*		r_zn_ld;
	CLS_R_ZN_SEC*		r_zn_sec;
	CLS_R_ND_CB*		r_nd_cb;
	CLS_R_ND_SWITCH*	r_nd_switch;
	CLS_R_ND_XF*		r_nd_xf;
	CLS_R_ZN_ND*		r_zn_nd;
	CLS_R_ND_SEC*		r_nd_sec;
	CLS_R_ND_CP*		r_nd_cp;
	CLS_R_ND_LD*		r_nd_ld;
	CLS_FI*				fi;
	CLS_R_SEC_FI*		r_sec_fi;
private:
	std::list<std::string> m_list;
	bool	*bvisited_nd;
	bool 	*bvisited_zn;

	void find(MAP_SO_PATH &so, int ind, bool bfindso, bool bcare_cb_close, DEV *data, bool bbuild_zn = false, bool bsave_ld = false);
	STACK_DEV m_path;

	//用于防误闭锁判断
	struct
	{
		bool b_has_so;
		long last_sec;	//离SO最近的一条SEC
		list<long> secs;
	} m_dv_sec; //CB一端的DV列表

	LST_LD m_lst_ld; //CB一端的LD列表

	///////////////////////////
	//用于建立ZN
	/////////////////////////////
	map<int, CLS_ND> m_zn_ind; 				//ZN内的所有ND,int为结点物理号
	LST_LD m_zn_ld; 					//某个ZN内的所有LD
	list<CLS_SEC> m_zn_sec; 				//某个ZN内的所有SEC
	map<int, CLS_ND> m_zn_end_ind; 			//ZN边界结点MAP,int为结点物理号
	map<long, CLS_BUS> m_map_bus; 		//ND与BUS的MAP数据,string为ND的ID
//	map<string, LST_LD> m_map_ld; //ND与LD的MAP数据,string为ND的ID
	map<long, LST_LD> map_zn_ld; //将ZN内的所有LD形成内存数据结构，以便快速查找，string为ZN的ID
	map<long, CLS_CB> map_zn_cb; //将ZN边界CB形成数据结构，string为ZN的ID

public:
	list<string> m_Errors;
};


int GetDevNDs(const long id, const char *table, int *pnds, int &cnt_nd);
extern bool pid_to_id(long &id);
extern bool id_to_pid(long &id);
#endif
