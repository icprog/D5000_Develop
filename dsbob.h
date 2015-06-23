//******************************************************************************
//
//  DSBOB.H
//  EMS Project - Copyright (c) 2000, EPRI
//  Class definitions of distribution network database object
//
//******************************************************************************
#if !defined(__DSBOB_H)
#define __DSBOB_H


//#include <oodb.h>
#include "netmom.h"		// Network model definition
//#include "dms.h"	// Network model record set definition
#include "tmptable.h"	// Table management template for database object
#include <stdio.h>
//#include "DBDMS.h"
#include <map>
#include <string>
#include <list>
#include "dsbob2.h"
using namespace std;

#define DSBOB_FAIL		0
#define	DSBOB_SUCCESS 	1

//设备挂牌
typedef struct DEV_STATUS
{
	DEV_STATUS(const long szid, const char *sztable, int istatus)
	{
		id = szid;
		table = sztable;
		status = istatus;
	}
	long id; //设备名
	string table; //设备类型
	int status; //挂牌类型
} DEV_STATUS, *PDEV_STATUS;
////////////////////////////////////////////////////////////////////////////////
//  class  DSBOB definition
class  DSBOB/* : public CObject*/
{
public:

	//constructor and destructor
	DSBOB();
	virtual ~DSBOB();
	//从dsbob2的内存中直接把数据拷到dsbob中20120918
	void ReadFromDsbob2(DSBOB2 *dsbob2);
	//read database object from relative database
	bool ReadReDB(const char* dsn,const char* user,const char* password);

	//write database object to relative database
	bool WriteReDB(const char* dsn,const char* user,const char* password);

	//获取指定厂站（st_id）下挂指定牌（status）的开关（lst_cb）和负荷（lst_ld）
	int GetDevStatusByST(const long st_id, int cb_status, int ld_status, list<DEV_STATUS> &lst_dev);

//	bool WritePSBOB();
#if 0
	//read database object from binary file
	int ReadObject();

	//write database object to binary file
	int WriteObject();

	//read or write database object
	void Serialize(CArchive& ar);

public:

	//source database name
	CString strDatabaseName;

	//binary file name
	CString strFileName;

	//text file name
	CString strTextName;
#endif
public:
	void Sequence();

	//table objects for network model database
//	TTable<CLS_ANALOG>*           analog_tb;
	TTable<CLS_BS>*               bs_tb;
	TTable<CLS_BUS>*              bus_tb;
	TTable<CLS_CB>*               cb_tb;
	TTable<CLS_SWITCH>*           switch_tb;
	TTable<CLS_CO>*               co_tb;
	TTable<CLS_CP>*               cp_tb;
//	TTable<CLS_DIGITAL>*          digital_tb;
	TTable<CLS_GLOBAL>*           global_tb;
	TTable<CLS_ISLAND>*           island_tb;
	TTable<CLS_LD>*               ld_tb;
	TTable<CLS_ND>*               nd_tb;
	TTable<CLS_R_CO_ST>*          r_co_st_tb;
	TTable<CLS_R_ND_CB>*          r_nd_cb_tb;
	TTable<CLS_R_ND_SWITCH>*      r_nd_switch_tb;
	TTable<CLS_R_ND_SEC>*         r_nd_sec_tb;
	TTable<CLS_R_ND_XF>*          r_nd_xf_tb;
	TTable<CLS_R_ZN_ND>*          r_zn_nd_tb;
	TTable<CLS_R_ST_BUS>*         r_st_bus_tb;
	TTable<CLS_R_ST_CB>*          r_st_cb_tb;
	TTable<CLS_R_ST_SWITCH>*      r_st_switch_tb;
	TTable<CLS_R_ST_CP>*          r_st_cp_tb;
	TTable<CLS_R_ST_LD>*          r_st_ld_tb;
	TTable<CLS_R_ST_SO>*          r_st_so_tb;
	TTable<CLS_R_ST_XFMR>*        r_st_xfmr_tb;
	TTable<CLS_R_ST_ZN>*          r_st_zn_tb;
	TTable<CLS_R_XFMR_XF>*        r_xfmr_xf_tb;
	TTable<CLS_R_ZN_LD>*          r_zn_ld_tb;
	TTable<CLS_R_ZN_SEC>*         r_zn_sec_tb;
	TTable<CLS_SEC>*              sec_tb;
	TTable<CLS_SO>*               so_tb;
	TTable<CLS_ST>*               st_tb;
	TTable<CLS_TAPTY>*            tapty_tb;
	TTable<CLS_XF>*               xf_tb;
	TTable<CLS_XFMR>*             xfmr_tb;
	TTable<CLS_ZN>*               zn_tb;
//	TTable<CLS_CTL>*			  ctl_tb;
	TTable<CLS_FI>*				  fi_tb;
	TTable<CLS_R_SEC_FI>*		  r_sec_fi_tb;
//	TTable<CLS_LNMODE>*			  lnmode_tb;
	TTable<CLS_R_ND_CP>*		  r_nd_cp_tb;
	TTable<CLS_R_ND_LD>*		  r_nd_ld_tb;

	//荣义平添加于2012.5.24
	map<long, list<CLS_LD> >			 m_map_nd_ld;	//string为负荷对应的nd名
};

extern DB_DMS *g_db_dms;
extern DB_PSBOB *g_db_psbob;
//extern DB_PSBOB *g_db_psbob;
#endif
