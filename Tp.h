////////////////////////////////////////////////////////////////////////////////
//
//  TP.H
//  DMS project - Copyright (c) 1999, EPRI
//  Class definition for distribution topology processing
//  Developed by Chen Jingcheng, April - 2000
//
////////////////////////////////////////////////////////////////////////////////
#if !defined(__TP_H)
#define __TP_H

#include "netmom.h"
#include <assert.h>
#include<vector>
//#include "dms.h"
#include "dsbob.h"
#include "tmptable.h"
#include <list>
//#include "tmpodrtb.h"

using namespace std;

#define TP_FAIL		0
#define	TP_SUCCEED	1

#define g_mxnbr 200

//链表信息
typedef struct
{
	char    id[20];
	int    pos;
}LIST_DATA;

////////////////////////////////////////////////////////////////////////////////
//  TP
class  TP
{
public:

	//constructor and destructor
	TP(DSBOB* m_pDb);
	~TP();

	//complete topology processing
	int tp_allnew();
	int tp_cb_changed(const long id);

private:

	//internal processing  methods
	int tp_table_count();
	int tp_table_get();
	int tp_nd_sort();
	int tp_bs_sort();
	int tp_bs_neighbor();
	int tp_comp_status();
	int tp_table_get_cbchange();
	void find_DFS(int ind, std::vector<bool> &bvisited);
	void find_BS(int sbs, std::vector<bool> &bvisited);
	int add_or_delete_BS(int ibs, int ind, int iznd, int isadd);

private:

	//variables
	int g_bs;
	int g_unit;
	int g_shunt;
	int g_nbr;
	int g_z_br[g_mxnbr];

private:

	//record count variables
	int	lv_digital;
	int	lv_analog;
	int	lv_co;
	int	lv_st;
	int	lv_xfmr;
	int	lv_xf;
	int	lv_cb;
	int	lv_so;
	int	lv_ld;
	int	lv_cp;
	int	lv_bus;
	int	lv_zn;
	int	lv_sec;
	int	lv_nd;
	int	lv_bs;
	int	lv_island;
	int	lv_global;
	int	lv_tapty;
	int	lv_lnty;
	int lv_r_co_st;
	int	lv_r_st_xfmr;
	int	lv_r_xfmr_xf;
	int	lv_r_st_cb;
	int	lv_r_st_so;
	int	lv_r_st_ld;
	int	lv_r_st_cp;
	int	lv_r_st_bus;
	int	lv_r_st_zn;
	int	lv_r_zn_ld;
	int	lv_r_zn_sec;
	int	lv_r_nd_cb;
	int	lv_r_nd_xf;
	int	lv_r_zn_nd;
	int	lv_r_nd_sec;
	int	lv_r_nd_cp;
	int	lv_r_nd_ld;

private:

	//pointer variables
//	CLS_DIGITAL*	digital;
//	CLS_ANALOG*		analog;
	CLS_CO*			co;
	CLS_ST*			st;
	CLS_XFMR*		xfmr;
	CLS_XF*			xf;
	CLS_CB*			cb;
	CLS_SO*			so;
	CLS_LD*			ld;
	CLS_CP*			cp;
	CLS_BUS*		bus;
	CLS_ZN*			zn;
	CLS_SEC*		sec;
	CLS_ND*			nd;
	CLS_BS*			bs;
	CLS_ISLAND*		island;
	CLS_GLOBAL*		global;
	CLS_TAPTY*		tapty;
	CLS_R_CO_ST*	r_co_st;
	CLS_R_ST_XFMR*	r_st_xfmr;
	CLS_R_XFMR_XF*	r_xfmr_xf;
	CLS_R_ST_CB*	r_st_cb;
	CLS_R_ST_SO*	r_st_so;
	CLS_R_ST_LD*	r_st_ld;
	CLS_R_ST_CP*	r_st_cp;
	CLS_R_ST_BUS*	r_st_bus;
	CLS_R_ST_ZN*	r_st_zn;
	CLS_R_ZN_LD*	r_zn_ld;
	CLS_R_ZN_SEC*	r_zn_sec;
	CLS_R_ND_CB*	r_nd_cb;
	CLS_R_ND_XF*	r_nd_xf;
	CLS_R_ZN_ND*	r_zn_nd;
	CLS_R_ND_SEC*	r_nd_sec;
	CLS_R_ND_CP*	r_nd_cp;
	CLS_R_ND_LD*	r_nd_ld;
	CLS_R_SEC_FI*	r_sec_fi;
	CLS_FI*			fi;

private:
	int ClearTopflags();

	//database object
	DSBOB* dsbob;
	//bs表中无效的记录
	list<LIST_DATA> invalid_bs;
	//island表中无效的记录
	list<LIST_DATA> invalid_island;
	list<LIST_DATA> pbss;
	list<LIST_DATA> pnds;


//	TTable<CLS_DIGITAL>*	digital_tb;
//	TTable<CLS_ANALOG>*		analog_tb;
	TTable<CLS_CO>*			co_tb;
	TTable<CLS_ST>*			st_tb;
	TTable<CLS_XFMR>*		xfmr_tb;
	TTable<CLS_XF>*			xf_tb;
	TTable<CLS_CB>*			cb_tb;
	TTable<CLS_SO>*			so_tb;
	TTable<CLS_LD>*			ld_tb;
	TTable<CLS_CP>*			cp_tb;
	TTable<CLS_BUS>*		bus_tb;
	TTable<CLS_ZN>*			zn_tb;
	TTable<CLS_SEC>*		sec_tb;
	TTable<CLS_ND>*			nd_tb;
	TTable<CLS_BS>*			bs_tb;
	TTable<CLS_ISLAND>*		island_tb;
	TTable<CLS_GLOBAL>*		global_tb;
	TTable<CLS_TAPTY>*		tapty_tb;
	TTable<CLS_R_CO_ST>*	r_co_st_tb;
	TTable<CLS_R_ST_XFMR>*	r_st_xfmr_tb;
	TTable<CLS_R_XFMR_XF>*	r_xfmr_xf_tb;
	TTable<CLS_R_ST_CB>*	r_st_cb_tb;
	TTable<CLS_R_ST_SO>*	r_st_so_tb;
	TTable<CLS_R_ST_LD>*	r_st_ld_tb;
	TTable<CLS_R_ST_CP>*	r_st_cp_tb;
	TTable<CLS_R_ST_BUS>*	r_st_bus_tb;
	TTable<CLS_R_ST_ZN>*	r_st_zn_tb;
	TTable<CLS_R_ZN_LD>*	r_zn_ld_tb;
	TTable<CLS_R_ZN_SEC>*	r_zn_sec_tb;
	TTable<CLS_R_ND_CB>*	r_nd_cb_tb;
	TTable<CLS_R_ND_XF>*	r_nd_xf_tb;
	TTable<CLS_R_ZN_ND>*	r_zn_nd_tb;
	TTable<CLS_R_ND_SEC>*	r_nd_sec_tb;
	TTable<CLS_R_ND_CP>*	r_nd_cp_tb;
	TTable<CLS_R_ND_LD>*	r_nd_ld_tb;
	TTable<CLS_R_SEC_FI>*	r_sec_fi_tb;
	TTable<CLS_FI>*			fi_tb;
};

#endif
