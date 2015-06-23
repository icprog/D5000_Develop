//******************************************************************************
//
//  DSBOB2.H
//  EMS Project - Copyright (c) 2000, EPRI
//  Class definitions of distribution network database object
//
//******************************************************************************
#if !defined(__DSBOB2_H)
#define __DSBOB2_H


//#include "arr.h"
#include "netmom.h"		// Network model definition
//#include "dms.h"	// Network model record set definition
#include "tmpodrtb.h"	// Table management template for database object
#include <map>
#include <string>
#include <list>
#include <stack>
#include "comm.h"
using namespace std;

#define MAX_KV	10.6
#define MIN_KV	9.4

////////////////////////////////////////////////////////////////////////////////
//  class  DSBOB2 definition
class  DSBOB2 /*: public CObject*/
{
public:

	//constructor and destructor
	DSBOB2();
	virtual ~DSBOB2();

	//read database object from relative database
	bool ReadPSBOB(long dvid = 0);

	//write database object to relative database
	bool WriteReDB2(const char* dsn,const char* user,const char* password);

public:

	//table objects for network model database
//	DTable<CLS_ANALOG>*           analog_tb;
	DTable<CLS_BS>*               bs_tb;
	DTable<CLS_BUS>*              bus_tb;
	DTable<CLS_CB>*               cb_tb;
	DTable<CLS_SWITCH>*           switch_tb;
	DTable<CLS_CO>*               co_tb;
	DTable<CLS_CP>*               cp_tb;
//	DTable<CLS_DIGITAL>*          digital_tb;
	DTable<CLS_GLOBAL>*           global_tb;
	DTable<CLS_ISLAND>*           island_tb;
	DTable<CLS_LD>*               ld_tb;
	DTable<CLS_ND>*               nd_tb;
	DTable<CLS_R_CO_ST>*          r_co_st_tb;
	DTable<CLS_R_ND_CB>*          r_nd_cb_tb;
	DTable<CLS_R_ND_SWITCH>*      r_nd_switch_tb;
	DTable<CLS_R_ND_SEC>*         r_nd_sec_tb;
	DTable<CLS_R_ND_XF>*          r_nd_xf_tb;
	DTable<CLS_R_ZN_ND>*          r_zn_nd_tb;
	DTable<CLS_R_ST_BUS>*         r_st_bus_tb;
	DTable<CLS_R_ST_CB>*          r_st_cb_tb;
	DTable<CLS_R_ST_SWITCH>*      r_st_switch_tb;
	DTable<CLS_R_ST_CP>*          r_st_cp_tb;
	DTable<CLS_R_ST_LD>*          r_st_ld_tb;
	DTable<CLS_R_ST_SO>*          r_st_so_tb;
	DTable<CLS_R_ST_XFMR>*        r_st_xfmr_tb;
	DTable<CLS_R_ST_ZN>*          r_st_zn_tb;
	DTable<CLS_R_XFMR_XF>*        r_xfmr_xf_tb;
	DTable<CLS_R_ZN_LD>*          r_zn_ld_tb;
	DTable<CLS_R_ZN_SEC>*         r_zn_sec_tb;
	DTable<CLS_SEC>*              sec_tb;
	DTable<CLS_SO>*               so_tb;
	DTable<CLS_ST>*               st_tb;
	DTable<CLS_TAPTY>*            tapty_tb;
	DTable<CLS_XF>*               xf_tb;
	DTable<CLS_XFMR>*             xfmr_tb;
	DTable<CLS_ZN>*               zn_tb;
//	DTable<CLS_CTL>*			  ctl_tb;
	DTable<CLS_FI>*				  fi_tb;
	DTable<CLS_R_SEC_FI>*		  r_sec_fi_tb;
	DTable<CLS_R_ND_CP>*		  r_nd_cp_tb;
	DTable<CLS_R_ND_LD>*		  r_nd_ld_tb;

	//郝泽赟添加于2012.4.24
//	DTable<CLS_LNMODE>*			  lnmode_tb;

	//荣义平添加于2012.5.24
	map<long, list<CLS_LD> >			 m_map_nd_ld;	//string为负荷对应的nd名
};

extern DB_PSBOB *g_db_psbob;
extern DB_DMS *g_db_dms;
extern map<long, list<long> > g_map_nd_bus, g_map_nd_switch;
extern map<long, list<stack<NODE> > >	g_map_nd_path;
extern bool is_substn_dev(long st_id);
extern bool is_kv10(long kv_id);
extern long IDTransfor(long &id);
#endif
