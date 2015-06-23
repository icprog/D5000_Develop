////////////////////////////////////////////////////////////////////////////////
//
//  NETMOM.H
//  DMS project - Copyright (c) 1999, EPRI
//  Class definitions of distribution network model
//
////////////////////////////////////////////////////////////////////////////////
#if !defined(__NETMOM_H)
#define __NETMOM_H

//#include "arr.h"
#include <string.h>
#include<time.h>
#include "oodb.h"
#include "RTDB.h"
#include "DBDMS.h"
#include "DBPSBOB.h"
#ifdef _SCADA
#include <db_struct_scada.h>
#else
#include <db_struct_dscada.h>
#endif
#include <sys_menu_info.h>
#ifdef _SCADA
namespace PSBOB
 {
typedef measpoint_scada		POINT;
typedef measanalog_scada		ANALOG;
typedef basevoltage_scada		BV;
typedef aclineend_scada			ACLINEEND;
typedef controldigital_scada	CD;
typedef relaysig_scada			PROTECT;
typedef subcontrolarea_scada 	DV;
typedef energyconsumer_scada 	LD;
typedef aclinesegment_scada		SEC;
typedef voltagelevel_scada		VL;
typedef subctrlarea_substation_scada OO_R_DV_ST;
typedef breaker_scada			CB;
typedef faultinfo_scada			FI;
typedef company_scada			CO;
typedef substation_scada		ST;
typedef busbarsection_scada		BUS;
typedef disconnector_scada		SWITCH;
typedef secinfo_scada			GLOBAL;
};
namespace DMS
 {
typedef DMS_CO_scada CO;
typedef DMS_ST_scada ST;
typedef DMS_XFMR_scada XFMR;
typedef DMS_XF_scada XF;
typedef DMS_CB_scada CB;
typedef DMS_SWITCH_scada SWITCH;
typedef DMS_SO_scada SO;
typedef DMS_LD_scada LD;
typedef DMS_CP_scada CP;
typedef DMS_BUS_scada BUS;
typedef DMS_ZN_scada ZN;
typedef DMS_SEC_scada SEC;
typedef DMS_ND_scada ND;
typedef DMS_BS_scada BS;
typedef DMS_ISLAND_scada ISLAND;
typedef DMS_GLOBAL_scada GLOBAL;
typedef DMS_TAPTY_scada TAPTY;
typedef DMS_FI_scada FI;
typedef DMS_r_XFMR_XF_scada R_XFMR_XF;
typedef DMS_r_ST_CB_scada R_ST_CB;
typedef DMS_r_ST_SO_scada R_ST_SO;
typedef DMS_r_ST_LD_scada R_ST_LD;
typedef DMS_r_ST_CP_scada R_ST_CP;
typedef DMS_r_ST_BUS_scada R_ST_BUS;
typedef DMS_r_ST_ZN_scada R_ST_ZN;
typedef DMS_r_ZN_LD_scada R_ZN_LD;
typedef DMS_r_ZN_SEC_scada R_ZN_SEC;
typedef DMS_r_ND_CB_scada R_ND_CB;
typedef DMS_r_ND_SWITCH_scada R_ND_SWITCH;
typedef DMS_r_ST_SWITCH_scada R_ST_SWITCH;
typedef DMS_r_ND_XF_scada R_ND_XF;
typedef DMS_r_ZN_ND_scada R_ZN_ND;
typedef DMS_r_ND_SEC_scada R_ND_SEC;
typedef DMS_r_ND_LD_scada R_ND_LD;
typedef DMS_r_ND_CP_scada R_ND_CP;
typedef DMS_r_ST_XFMR_scada R_ST_XFMR;
typedef DMS_r_CO_ST_scada R_CO_ST;
typedef DMS_R_SEC_FI_scada R_SEC_FI;
};

typedef FDIR_CB_scada	FDIR_CB;
typedef FDIR_ZONE_scada FDIR_ZONE;
typedef FDIR_ZONE_CB_scada FDIR_ZONE_CB;
#else
namespace PSBOB
 {
typedef measpoint_dscada		POINT;
typedef measanalog_dscada		ANALOG;
typedef basevoltage_dscada		BV;
typedef voltagelevel_dscada		VL;
typedef aclineend_dscada		ACLINEEND;
typedef controldigital_dscada	CD;
typedef relaysig_dscada			PROTECT;
typedef subcontrolarea_dscada 	DV;
typedef energyconsumer_dscada 	LD;
typedef aclinesegment_dscada		SEC;
typedef subctrlarea_substation_dscada OO_R_DV_ST;
typedef breaker_dscada			CB;
typedef faultinfo_dscada			FI;
typedef company_dscada			CO;
typedef substation_dscada		ST;
typedef busbarsection_dscada		BUS;
typedef disconnector_dscada		SWITCH;
typedef secinfo_dscada			GLOBAL;
};
namespace DMS
 {
typedef DMS_CO_dscada CO;
typedef DMS_ST_dscada ST;
typedef DMS_XFMR_dscada XFMR;
typedef DMS_XF_dscada XF;
typedef DMS_CB_dscada CB;
typedef DMS_SWITCH_dscada SWITCH;
typedef DMS_SO_dscada SO;
typedef DMS_LD_dscada LD;
typedef DMS_CP_dscada CP;
typedef DMS_BUS_dscada BUS;
typedef DMS_ZN_dscada ZN;
typedef DMS_SEC_dscada SEC;
typedef DMS_ND_dscada ND;
typedef DMS_BS_dscada BS;
typedef DMS_ISLAND_dscada ISLAND;
typedef DMS_GLOBAL_dscada GLOBAL;
typedef DMS_TAPTY_dscada TAPTY;
typedef DMS_FI_dscada FI;
typedef DMS_r_XFMR_XF_dscada R_XFMR_XF;
typedef DMS_r_ST_CB_dscada R_ST_CB;
typedef DMS_r_ST_SO_dscada R_ST_SO;
typedef DMS_r_ST_LD_dscada R_ST_LD;
typedef DMS_r_ST_CP_dscada R_ST_CP;
typedef DMS_r_ST_BUS_dscada R_ST_BUS;
typedef DMS_r_ST_ZN_dscada R_ST_ZN;
typedef DMS_r_ZN_LD_dscada R_ZN_LD;
typedef DMS_r_ZN_SEC_dscada R_ZN_SEC;
typedef DMS_r_ND_CB_dscada R_ND_CB;
typedef DMS_r_ND_SWITCH_dscada R_ND_SWITCH;
typedef DMS_r_ST_SWITCH_dscada R_ST_SWITCH;
typedef DMS_r_ND_XF_dscada R_ND_XF;
typedef DMS_r_ZN_ND_dscada R_ZN_ND;
typedef DMS_r_ND_SEC_dscada R_ND_SEC;
typedef DMS_r_ND_LD_dscada R_ND_LD;
typedef DMS_r_ND_CP_dscada R_ND_CP;
typedef DMS_r_ST_XFMR_dscada R_ST_XFMR;
typedef DMS_r_CO_ST_dscada R_CO_ST;
typedef DMS_R_SEC_FI_dscada R_SEC_FI;
};

typedef FDIR_CB_dscada	FDIR_CB;
typedef FDIR_ZONE_dscada FDIR_ZONE;
typedef FDIR_ZONE_CB_dscada FDIR_ZONE_CB;
#endif
#include<strings.h>
#include<iostream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//	length definition of object ID and DESCR
#define     MAX_DIGITAL_ID_LEN		20
#define     MAX_DIGITAL_DESCR_LEN	64
#define     MAX_ANALOG_ID_LEN		20
#define     MAX_ANALOG_DESCR_LEN	64
#define     MAX_CO_ID_LEN			20
#define     MAX_CO_DESCR_LEN		64
#define     MAX_ST_ID_LEN			20
#define     MAX_ST_DESCR_LEN		64
#define     MAX_ST_ABB_LEN			4
#define     MAX_XFMR_ID_LEN			20
#define     MAX_XFMR_DESCR_LEN		64
#define     MAX_XF_ID_LEN			20
#define     MAX_XF_DESCR_LEN		64
#define	    MAX_CB_ID_LEN			20
#define	    MAX_CB_DESCR_LEN		64
#define     MAX_SO_ID_LEN			20
#define	    MAX_SO_DESCR_LEN		64
#define	    MAX_LD_ID_LEN			20
#define     MAX_LD_DESCR_LEN		64
#define     MAX_CP_ID_LEN			20
#define     MAX_CP_DESCR_LEN		64
#define     MAX_BUS_ID_LEN			20
#define     MAX_BUS_DESCR_LEN		64
#define     MAX_ZN_ID_LEN			20
#define     MAX_ZN_DESCR_LEN		64
#define     MAX_SEC_ID_LEN			20
#define     MAX_SEC_DESCR_LEN		64
#define     MAX_ND_ID_LEN			20
#define     MAX_TAPTY_ID_LEN		20
#define     MAX_LNTY_ID_LEN			20


//haozeyun 2011.11.23tianjia
#define		MAX_CTL_CONVG_LEN		64


//haozeyun 2012.4.24tianjia
#define MAX_LNMODE_ID_LEN			20
#define MAX_LNMODE_CONVG_LEN		64
////////////////////////////////////////////////////////////////////////////////
//  value definition of mask operation variable
#define     ST_SUBST				1
#define     ST_SWIST				2
#define     ST_SO					1
#define     ST_CP					2
#define     ST_XFMR					4
#define     ST_LD					8
#define     ST_CB					16
#define     ST_ZN					32
#define     XFMR_REMOVE 			1
#define     XFMR_RMVENABL       	2
#define     XFMR_NRMDEAD       		1
#define     XFMR_INRMOPEN			2
#define     XFMR_INRMGRND			4
#define     XFMR_ZNRMOPEN			8
#define     XFMR_ZNRMGRND			16
#define     XFMR_LIVE				32
#define     XFMR_DEAD				64
#define     XFMR_AMBIG				128
#define     XFMR_UKNOWN				256
#define     XFMR_HOPEN				512
#define     XFMR_HCONN				1024
#define     XFMR_HGRND				2048
#define     XFMR_HCMDMAN			4096
#define     XFMR_MOPEN				8192
#define     XFMR_MCONN				16384
#define     XFMR_MGRND				32768
#define     XFMR_MCMDMAN			65536
#define     XFMR_LOPEN				131072
#define     XFMR_LCONN				262144
#define     XFMR_LGRND				524288
#define     XFMR_LCMDMAN			1048576
#define     XFMR_ABNRML				2097152
#define     XF_REMOVE				1
#define     XF_RMVENABL				2
#define     XF_NRMDEAD				1
#define     XF_INRMOPEN				2
#define     XF_INRMGRND				4
#define     XF_ZNRMOPEN				8
#define     XF_ZNRMGRND				16
#define     XF_LIVE					32
#define     XF_DEAD					64
#define     XF_AMBIG				128
#define     XF_UNKNOWN				256
#define     XF_IOPEN				512
#define     XF_ICONN				1024
#define     XF_IGRND				2048
#define     XF_ICMDMAN				4096
#define     XF_ZOPEN				8192
#define     XF_ZCONN				16384
#define     XF_ZGRND				32768
#define     XF_ZCMDMAN				65536
#define     XF_ABNRML				131072
#define     CB_NMLOPEN				1
#define     CB_FORCST				2
#define     CB_BREAKER				4
#define     CB_MOTODIS				8
#define     CB_SWITCH				16
#define     CB_DEADBUS				32
#define     CB_AUTO					64
#define		CB_N_RESTORE			128
#define     CB_OPANABL				256
#define     CB_FDHCB				512
#define     CB_FDSCB				1024
#define     CB_FDCCB				2048
#define		CB_FDLCB				4096
#define		CB_N_ISOLATE			8192
#define     CB_OPEN					16384
#define 	CB_LWCB					32768
#define		CB_ISOLATE				65536
#define		CB_PRESERVE				131072
#define		CB_VIRTUAL				262144		//虚拟的进线开关
#define 	CB_FAULT_ISOLATE		1073741824	//开关已经故障隔离


#define     CB_NRMDEAD				1
#define     CB_INRMOPEN				2
#define     CB_INRMGRND				4
#define     CB_ZNRMOPEN				8
#define     CB_ZNRMGRND				16
#define     CB_LIVE					32
#define     CB_DEAD					64
#define     CB_AMBIG				128
#define     CB_UNKNOWN				256
#define     CB_IOPEN				512
#define     CB_ICONN				1024
#define     CB_IGRND				2048
#define     CB_ICMDMAN				4096
#define     CB_ZOPEN				8192
#define     CB_ZGRND				16384
#define     CB_ZCMDMAN				32768
#define     CB_ABNRMAL				65536
#define		CB_FAULT				131072		//for fdir

#define     SO_REMOVE				1
#define     SO_RMVENABL				2
#define     SO_OPEN					4
#define     SO_NRMDEAD				1
#define     SO_NRMOPEN				2
#define     SO_NRMGRND				4
#define     SO_LIVE					8
#define     SO_DEAD					16
#define     SO_AMBIG				32
#define     SO_UNKNOWN				64
#define     SO_IOPEN				128
#define     SO_CONN					256
#define     SO_GRND					512
#define     SO_CMDMAN				1024
#define     SO_ABNRML				2048
#define     LD_REMOVE				1
#define     LD_RMVENABL				2
#define     LD_OPEN					4
#define     LD_ZNLD					8
#define     LD_NRMDEAD				1
#define     LD_NRMOPEN				2
#define     LD_NRMGRND				4
#define     LD_LIVE					8
#define     LD_DEAD					16
#define     LD_AMBIG				32
#define     LD_UNKNOWN				64
#define     LD_IOPEN				128
#define     LD_CONN					256
#define     LD_GRND					512
#define     LD_CMDMAN				1024
#define     LD_ABNRML				2048
#define     LD_FAULT				4096		//for fdir
#define		LD_FLASH				8192		//for fdir

#define     CP_REMOVE				1
#define     CP_RMVENABL				2
#define     CP_OPEN					4
#define     CP_NRMDEAD				1
#define     CP_NRMOPEN				2
#define     CP_NRMGRND				4
#define     CP_LIVE					8
#define     CP_DEAD					16
#define     CP_AMBIG				32
#define     CP_UNKNOWN				64
#define     CP_IOPEN				128
#define     CP_CONN					256
#define     CP_GRND					512
#define     CP_CMDMAN				1024
#define     CP_ABNRML				2048
#define     BUS_NRMDEAD				1
#define     BUS_NRMOPEN				2
#define     BUS_NRMGRND				4
#define     BUS_LIVE				8
#define     BUS_DEAD				16
#define     BUS_AMBIG				32
//#define     BUS_UNKNOWN				64
#define     BUS_OPEN				128
#define     BUS_CONN				256
#define     BUS_GRND				512
#define     BUS_CMDMAN				1024
#define     BUS_ABNRML				2048
#define		BUS_FAULT				4096		//for fdir
#define		BUS_FLASH				8192		//for fdir

#define		ZN_REMOVE				1
#define		ZN_RMVENABL				2
#define     ZN_LOWCAP				4

#define		ZN_NRMDEAD				1
#define     ZN_INRMOPEN				2
#define     ZN_INRMGRND				4
#define     ZN_ZNRMOPEN				8
#define     ZN_ZNRMGRND				16
#define     ZN_LIVE					32
#define     ZN_DEAD					64
#define     ZN_AMBIG				128
#define     ZN_UNKNOWN				256
#define     ZN_IOPEN				512
#define     ZN_ICONN				1024
#define     ZN_IGRND				2048
#define     ZN_ICMDMAN				4096
#define     ZN_ZOPEN				8192
#define     ZN_ZCONN				16384
#define     ZN_ZGRND				32768
#define     ZN_ZCMDMAN				65536
#define     ZN_ABNRML				131072
#define		ZN_FAULT				262144		//for fdir
#define		ZN_FLASH				524288		//for fdir

#define     SEC_REMOVE				1
#define     SEC_RMVENABL			2
#define     SEC_ZPRIME				4
#define     SEC_NRMDEAD				1
#define     SEC_INRMOPEN			2
#define     SEC_INRMGRND			4
#define     SEC_ZNRMOPEN			8
#define     SEC_ZNRMGRND			16
#define     SEC_LIVE				32
#define     SEC_DEAD				64
#define     SEC_AMBIG				128
#define     SEC_UNKNOWN				256
#define     SEC_IOPEN				512
#define     SEC_ICONN				1024
#define     SEC_IGRND				2048
#define     SEC_ICMDMAN				4096
#define     SEC_ZOPEN				8192
#define     SEC_ZCONN				16384
#define     SEC_ZGRND				32768
#define     SEC_ZCMDMAN				65536
#define     SEC_ABNRML				131072
#define		SEC_FAULT				262144		//for fdir
#define		SEC_FLASH				524288		//for fdir

#define		ND_DEAD					1
#define     ND_OPEN					2
#define     ND_SO					4
#define     ND_LD					8
#define     ND_CP					16
#define     ND_ZN					32
#define		ND_REG					64
#define		ND_GRND					128
#define     ND_UNPROC				256
#define     BS_DEAD					1
#define     BS_OPEN					2
#define     BS_UNREG				4
#define     BS_REGERR				8
#define     BS_VHI					16
#define     BS_VLO					32
#define     BS_REF					64
#define     BS_PRI					1
#define     BS_AVAIL				2

////////////////////////////////////////////////////////////////////////////////
//  flag definition of table name
#define		_DIGITAL				1
#define		_ANALOG					2
#define		_CO						3
#define		_ST						4
#define		_XFMR					5
#define		_XF						6
#define		_CB						7
#define		_SO						8
#define		_LD						9
#define		_CP						10
#define		_BUS					11
#define		_ZN						12
#define		_SEC					13
#define		_ND						14
#define		_BS						15
#define		_ISLAND					16
#define		_GLOBAL					17
#define		_TAPTY					18
#define		_LNTY					19
#define		_R_CO_ST				20
#define		_R_ST_XFMR				21
#define		_R_XFMR_XF				22
#define		_R_ST_CB				23
#define		_R_ST_SO				24
#define		_R_ST_LD				25
#define		_R_ST_CP				26
#define		_R_ST_BUS				27
#define		_R_ST_ZN				28
#define		_R_ZN_LD				29
#define		_R_ZN_SEC				30
#define		_R_ND_CB				31
#define		_R_ND_XF				32
#define		_R_ND_ZN				33
#define		_R_ND_SEC				34
#define		_FI						36
#define		_SWITCH					37
#define		_R_ST_SWITCH			38
#define		_R_ND_SWITCH			39

//haozeyun 2011.11.23tian jia
#define		_CTL					35
////////////////////////////////////////////////////////////////////////////////
//  size definition of each table
#define		mx_digital			50000
#define		mx_analog			50000
#define		mx_co				1
#define		mx_st				800
#define		mx_xfmr				2000
#define		mx_xf				6000
#define		mx_cb				25000
#define		mx_so				1600
#define		mx_ld				20000
#define		mx_cp				2000
#define		mx_bus				1600
#define		mx_zn				10000
#define		mx_sec				30000
#define		mx_nd				30000
#define		mx_bs				30000
#define		mx_island			1000
#define		mx_global			1
#define		mx_tapty			4000
#define		mx_lnty				20
#define		mx_r_co_st			1000
#define		mx_r_st_xfmr		2000
#define		mx_r_xfmr_xf		6000
#define		mx_r_st_cb			25000
#define		mx_r_st_so			1600
#define		mx_r_st_ld			20000
#define		mx_r_st_cp			2000
#define		mx_r_st_bus			1600
#define		mx_r_st_zn			10000
#define		mx_r_zn_ld			20000
#define		mx_r_zn_sec			30000
#define		mx_r_nd_cb			30000
#define		mx_r_nd_xf			18000
#define		mx_r_zn_nd			30000
#define		mx_r_nd_sec			30000
#define 	mx_fi				30000
#define 	mx_r_sec_fi			30000
#define 	mx_r_nd_cp			2000
#define		mx_r_nd_ld			20000

#define 	ZN_ND_USE_ARRAY	1	//ZN的节点采用数组的方式存储

//haozeyun 2011.11.23 tianjia
#define		mx_ctl				1
//haozeyun 2012.4.24 tianjia
#define 	mx_lnmode 			30000

//tianyq修改于20120910为适应查找区段内非边界的结点
typedef struct
{
	long 	nd_id;
	int		ind;
	bool	is_edge;//结点的类型1：边界结点，0：非边界结点
}NODE_DATA;
//

#ifdef _CC2000_
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_DIGITAL //: public CObject
{
public:

	char	id[MAX_DIGITAL_ID_LEN];
	char	descr[MAX_DIGITAL_DESCR_LEN];
	int		value;
	int		quality;

	//荣义平添加于20110929
	char	cb[MAX_CB_ID_LEN];
	int		icb;

public:

	//constructors
	CLS_DIGITAL();
	CLS_DIGITAL(const char* m_id);

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

	//assign value operator
	const CLS_DIGITAL& operator = (const CLS_DIGITAL& m_digital);

	//compare operator ==
	int operator == (const CLS_DIGITAL& m_digital) const
	{
		return strcmp(id, m_digital.id) == 0;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_DIGITAL& m_digital) const
	{
		return strcmp(id, m_digital.id) < 0;
	}

	//read or write object
    //void Serialize( CArchive& ar );


protected:

	 //DECLARE_SERIAL( DIGITAL )
};

//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_ANALOG //: public CObject
{
public:

	char	id[MAX_ANALOG_ID_LEN];
	char	descr[MAX_ANALOG_DESCR_LEN];
	float	value;
	int		quality;

public:

	//constructors
	CLS_ANALOG();
	CLS_ANALOG(const char* m_id);

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

	//assign value operator
	const CLS_ANALOG& operator = (const CLS_ANALOG& m_analog);

	//compare operator ==
	int operator == (const CLS_ANALOG& m_analog) const
	{
		return strcmp(id, m_analog.id) == 0;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_ANALOG& m_analog) const
	{
		return strcmp(id, m_analog.id) < 0;
	}

	//read or write object
    //void Serialize( CArchive& ar );


protected:

	 //DECLARE_SERIAL( ANALOG )
};
#endif

//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_CO //: public CObject
{
public:

	long	id;
	long	pid;
	char 	descr[MAX_CO_DESCR_LEN];
	float 	wloss;
	float	rloss;
	float	wso;
	float 	rso;
	float	wload;
	float	rload;
	float	cap;
	float	reac;
	int		r_st;
//张瑞雪添加于2012年2月7日
	float 	wloss_b;
	float 	wloss_c;
	float   rloss_b;
	float   rloss_c;
	float	wso_b;
	float	wso_c;
	float   rso_b;
	float   rso_c;
	float	wload_b;
	float	wload_c;
	float	rload_b;
	float	rload_c;
	float	cap_b;
	float	cap_c;
	float	reac_b;
	float	reac_c;
//张瑞雪添加于2012年5月3日
	float 	wloss_a;
	float   rloss_a;
	float	wso_a;
	float   rso_a;
	float	wload_a;
	float	rload_a;
	float	cap_a;
	float	reac_a;


public:

	//constructors
	CLS_CO();
	CLS_CO(const long m_id);

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


	//assign value operator
	const CLS_CO& operator = (const CLS_CO& m_co);

	//compare operator ==
	int	operator == (const CLS_CO& m_co) const
	{
		return id == m_co.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_CO& m_co) const
	{
		return id < m_co.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( CO )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_ST //: public CObject
{
public:

	long   id;
	long 	pid;
	char   descr[MAX_ST_DESCR_LEN];
	int    type;
	int    qcomp;
	float  wso;
	float  rso;
	float  wload;
	float  rload;
	int    r_co;
	int    r_xfmr;
	int    r_so;
	int    r_cb;
	int    r_switch;
	int    r_ld;
	int    r_cp;
	int    r_bus;
	int    r_zn;
//张瑞雪添加于2012年2月7日
	float  wso_b;
	float  rso_b;
	float  wload_b;
	float  rload_b;
	float  wso_c;
	float  rso_c;
	float  wload_c;
	float  rload_c;
//张瑞雪添加于2012年5月3日
	float  wso_a;
	float  rso_a;
	float  wload_a;
	float  rload_a;
public:

	//constructors
	CLS_ST();
	CLS_ST(const long m_id);

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

	//assign value operator
	const CLS_ST& operator = (const CLS_ST& m_st);

	//compare operator ==
	int operator == (const CLS_ST& m_st) const
	{
		return id==m_st.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_ST& m_st) const
	{
		return id < m_st.id;
	}

	//read or write object
     //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( ST )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_XFMR //: public CObject
{
public:

	long	id;
	long	pid;
	char	descr[MAX_XFMR_DESCR_LEN];
	int		type;
	long	st;
	float	rh;
	float	xh;
	float	rm;
	float	xm;
	float	rl;
	float	xl;
	float	kvnomh;
	float	kvnomm;
	float	kvnoml;
	float	mvah;
	float	mvam;
	float	mval;
	long	algph;
	int		ialgph;
	long	algqh;
	int		ialgqh;
	long	algih;
	int		ialgih;
	long	algth;
	int		ialgth;
	long	algpm;
	int		ialgpm;
	long	algqm;
	int		ialgqm;
	long	algim;
	int		ialgim;
	long	algtm;
	int		ialgtm;
	long	algpl;
	int		ialgpl;
	long	algql;
	int		ialgql;
	long	algil;
	int		ialgil;
	long	algtl;
	int		ialgtl;
	long	hnd;
	long	mnd;
	long	lnd;
	long	htapty;
	long	mtapty;
	long	ltapty;
	int		htap;
	int		mtap;
	int		ltap;
	int		q;
	int		topflags;
	int		r_st;
	int		r_xf;
//张瑞雪添加于2012年2月15日
	float    rh_b;
	float    rh_c;
	float    xh_b;
	float    xh_c;
	float    rl_b;
	float    rl_c;
	float    xl_b;
	float    xl_c;
	long    algph_b;
	long    algph_c;
	int    ialgph_b;
	int    ialgph_c;
	long    algqh_b;
	long    algqh_c;
	int    ialgqh_b;
	int    ialgqh_c;
	long    algih_b;
	long    algih_c;
	int    ialgih_b;
	int    ialgih_c;
	long    algth_b;
	long    algth_c;
	int    ialgth_b;
	int    ialgth_c;
	long    algpl_b;
	long    algpl_c;
	int    ialgpl_b;
	int    ialgpl_c;
	long    algql_b;
	long    algql_c;
	int    ialgql_b;
	int    ialgql_c;
	long    algil_b;
	long    algil_c;
	int    ialgil_b;
	int    ialgil_c;
	long    algtl_b;
	long    algtl_c;
	int    ialgtl_b;
	int    ialgtl_c;
	long    htapty_b;
	long    htapty_c;
	long    ltapty_b;
	long    ltapty_c;
	int    htap_b;
	int    htap_c;
	int    ltap_b;
	int    ltap_c;
	int    xftype;
//张瑞雪添加于20120503
	float    rh_a;
	float    xh_a;
	float    rl_a;
	float    xl_a;
	long    algph_a;
	int    ialgph_a;
	long    algqh_a;
	int    ialgqh_a;
	long    algih_a;
	int    ialgih_a;
	long    algpl_a;
	int    ialgpl_a;
	long    algql_a;
	int    ialgql_a;
	long    algil_a;
	int    ialgil_a;
	long	stid;
public:

	//constructors
	CLS_XFMR();
	CLS_XFMR(const long m_id);

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

	//assign value operator
	const CLS_XFMR& operator = (const CLS_XFMR& m_xfmr);

	//compare operator
	int operator == (const CLS_XFMR& m_xfmr) const
	{
		return id == m_xfmr.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_XFMR& m_xfmr) const
	{
		return id < m_xfmr.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( XFMR )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_XF //: public CObject
{
public:

	long	id;
	long	pid;
	char	descr[MAX_XF_DESCR_LEN];
	int		itapty;
	int		iztapty;
	int		tap;
	int		ztap;
	float  	kvnom;
	float	zkvnom;
	long	nd;
	long	znd;
	int		ind;
	int		iznd;
	int		ibs;
	int		zbs;
	float	mvanom;
	int		q;
	int		topflags;
	float	r;
	float	x;
	int		ialgip;
	int		ialgiq;
	int		ialgii;
	int		ialgit;
	int		ialgzp;
	int		ialgzq;
	int		ialgzi;
	int		ialgzt;
	float	w;
	float	r_;
	float	i;
	float	wz;
	float	rz;
	float	iz;
	int		r_xfmr;
//张瑞雪添加于20120215
	int     xftype;
	int     itapty_b;
	int 	itapty_c;
	int 	iztapty_b;
	int 	iztapty_c;
	int 	tap_b;
	int 	tap_c;
	int 	ztap_b;
	int    	ztap_c;
	float    r_b;
	float    r_c;
	float    x_b;
	float    x_c;
	int    	ialgip_b;
	int    	ialgip_c;
	int    	ialgiq_b;
	int    	ialgiq_c;
	int    	ialgii_b;
	int    	ialgii_c;
	int    	ialgit_b;
	int   	ialgit_c;
	int   	ialgzp_b;
	int    	ialgzp_c;
	int    	ialgzq_b;
	int    	ialgzq_c;
	int    	ialgzi_b;
	int    	ialgzi_c;
	int    	ialgzt_b;
	int    	ialgzt_c;
	float    w_b;
	float    w_c;
	float    rq_b;
	float    rq_c;
	float    i_b;
	float    i_c;
	float    wz_b;
	float    wz_c;
	float    rz_b;
	float	 rz_c;
	float	 iz_b;
	float 	iz_c;
//张瑞雪添加于20120503
	float 	r_a;
	float 	x_a;
	float 	w_a;
	float 	rq_a;
	float 	i_a;
	float 	wz_a;
	float 	rz_a;
	float 	iz_a;
	int    	ialgip_a;
	int    	ialgiq_a;
	int    	ialgii_a;
	int   	ialgzp_a;
	int    	ialgzq_a;
	int    	ialgzi_a;
public:

	//constructors
	CLS_XF();
	CLS_XF(const long m_id);

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

	//assign value operator
	const CLS_XF& operator = (const CLS_XF& m_xf);

	//compare operator ==
	int operator == (const CLS_XF& m_xf) const
	{
		return id == m_xf.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_XF& m_xf) const
	{
		return id < m_xf.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( XF )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_CB //: public CObject
{
public:

	long	id;
	long	pid;
	char	descr[MAX_CB_DESCR_LEN];
	long	nd;
	long	znd;
	int		ind;
	int		iznd;
	long	digital;
//	int		idigital;
	int		q;
	int		topflags;
	int		r_st;
	long 	stid;	//荣义平添加于20110927，从PSBOB导入用于建立CB与ST间的关系

public:

	//constructors
	CLS_CB();
	CLS_CB(const long m_id);

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

	//assign value operator
	const CLS_CB& operator = (const CLS_CB& m_cb);

	//compare operator ==
	int operator == (const CLS_CB& m_cb) const
	{
		return id == m_cb.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_CB& m_cb) const
	{
		return id < m_cb.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( CB )
};


class CLS_SWITCH //: public CObject
{
public:

	long	id;
	long	pid;
	char	descr[MAX_CB_DESCR_LEN];
	long	nd;
	long	znd;
	int		ind;
	int		iznd;
//	long	digital;
//	int		idigital;
	int		q;
	int		topflags;
	int		r_st;
	long 	stid;	//荣义平添加于20110927，从PSBOB导入用于建立CB与ST间的关系

public:

	//constructors
	CLS_SWITCH();
	CLS_SWITCH(const long m_id);

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

	//assign value operator
	const CLS_SWITCH& operator = (const CLS_SWITCH& m_cb);

	//compare operator ==
	int operator == (const CLS_SWITCH& m_cb) const
	{
		return id == m_cb.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_SWITCH& m_cb) const
	{
		return id < m_cb.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( CB )
};

//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_SO //: public CObject
{
public:
	long	id;
	long	pid;
	char	descr[MAX_SO_DESCR_LEN];
	long    nd;
	int		ind;
	int    	ibs;
	int    	iisland;
	long    algp;
	int    	ialgp;
	long    algq;
	int    	ialgq;
	long    algi;
	int    	ialgi;
	int  	q;
	int    	topflags;
	float   w;
	float   r;
	float	i;
	int    	r_st;
	float	ss;
	float	sw;
	float	sr;
//张瑞雪添加于2012年2月9日
	int    slackavr;
	float	w_b;
	float	w_c;
	float	r_b;
	float	r_c;
	float	i_b;
	float	i_c;
	float	v;
	float	v_b;
	float	v_c;
	float	wmx;
	float	rmx;
	float	wmn;
	float	rmn;
	long    algp_b;
	long    algp_c;
	int    ialgp_b;
	int    ialgp_c;
	long    algq_b;
	long    algq_c;
	int    ialgq_b;
	long    algi_b;
	int    ialgq_c;
	long    algi_c;
	int    ialgi_b;
	int    ialgi_c;

//郝泽赟添加于2012.04.26
	float	adeg;
	float 	adeg_b;
	float	adeg_c;
//张瑞雪添加于20120503
	long    algp_a;
	int    ialgp_a;
	long    algq_a;
	int    ialgq_a;
	long    algi_a;
	int    ialgi_a;
	float	w_a;
	float	r_a;
	float	i_a;
	float	v_a;
	float 	adeg_a;
	long	stid;
public:

	//constructors
	CLS_SO();
	CLS_SO(const long m_id);

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

	//assign value operator
	const CLS_SO& operator = (const CLS_SO& m_so);

	//compare operator ==
	int operator == (const CLS_SO& m_so) const
	{
		return id == m_so.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_SO& m_so) const
	{
		return id < m_so.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );


protected:

	 //DECLARE_SERIAL( SO )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_LD //: public CObject
{
public:

	long	id;
	long	pid;
	char	descr[MAX_LD_DESCR_LEN];
	int		type;
	long	nd;
	int		ind;
	int		ibs;
	long    algp;
	int		ialgp;
	long    algq;
	int		ialgq;
	long	algi;
	int		ialgi;
	int		q;
	int		topflags;
	float   w;
	float   r;
	float   wm;
	float   rm;
	int		r_st;
	int		r_zn;
	long 	stid;	//荣义平添加于20110927，从PSBOB导入用于建立LD与ST间的关系
	long 	zn_id;

//张瑞雪添加于20120209
	int    ldtype;
	float    i;
	float    i_b;
	float    i_c;
	float    a;
	float    a_b;
	float    a_c;
	float    zr;
	float    zr_b;
	float    zr_c;
	float    zx;
	float    zx_b;
	float    zx_c;
	long    algp_b;
	long    algp_c;
	int    ialgp_b;
	int    ialgp_c;
	long    algq_b;
	long    algq_c;
	int    ialgq_b;
	int    ialgq_c;
	long    algi_b;
	long    algi_c;
	int    ialgi_b;
	int    ialgi_c;
	float    w_b;
	float    w_c;
	float    r_b;
	float    r_c;
	float    wm_b;
	float    wm_c;
	float    rm_b;
	float    rm_c;
	long    algia;
	long    algia_b;
	long    algia_c;
	int    ialgia;
	int    ialgia_b;
	int    ialgia_c;
//张瑞雪添加于20120503
	long    algp_a;
	int    ialgp_a;
	long    algq_a;
	int    ialgq_a;
	long    algi_a;
	int    ialgi_a;
	long    algia_a;
	int    ialgia_a;
	float    w_a;
	float    r_a;
	float    i_a;
	float    a_;
	float    zr_a;
	float    zx_a;
public:

	//constructors
	CLS_LD();
	CLS_LD(const long m_id);

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

	//assign value operator
	const CLS_LD& operator = (const CLS_LD& m_ld);

	//compare operator ==
	int operator == (const CLS_LD& m_ld) const
	{
		return id == m_ld.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_LD& m_ld) const
	{
		return id < m_ld.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( LD )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_CP //: public CObject
{
public:

	long	id;
	long	pid;
	char	descr[MAX_CP_DESCR_LEN];
	long	nd;
	int		ind;
	int		ibs;
	long    algq;
	int		ialgq;
	int		q;
	int		topflags;
	float	mrnom;
	float   r;
	int		r_st;
	long	stid;
	long    algq_b;
	long    algq_c;
	int    ialgq_b;
	int    ialgq_c;
	float    mrnom_b;
	float    mrnom_c;
	float    r_b;
	float    r_c;
//张瑞雪添加于20120503
	float    r_a;
	long    algq_a;
	int    ialgq_a;

public:

	//constructors
	CLS_CP();
	CLS_CP(const long m_id);

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

	//assign value operator
	const CLS_CP& operator = (const CLS_CP& m_cp);

	//compare operator ==
	int operator == (const CLS_CP& m_cp) const
	{
		return id == m_cp.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_CP& m_cp) const
	{
		return id < m_cp.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( CP )
};

//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_BUS //: public CObject
{
public:

	long	id;
	long	pid;
	char    descr[MAX_BUS_DESCR_LEN];
	long    nd;
	int		ind;
	int		ibs;
	long    algv;
	int		ialgv;
	int		topflags;
	float	vtarget;
	float	vl;
	float	vhlim;
	float	vllim;
	float   v;
	float   adeg;
	int		r_st;
	long	stid;		//荣义平添加于20110927，从PSBOB导入用于建立BUS与ST间的关系
//张瑞雪添加于20120209
	long    algv_b;
	long    algv_c;
	int    ialgv_b;
	int    ialgv_c;
	float    vtarget_b;
	float    vtarget_c;
	float    vhlim_b;
	float    vhlim_c;
	float    vllim_b;
	float    vllim_c;
	float    v_b;
	float    v_c;
	float    adeg_b;
	float    adeg_c;

//郝泽赟添加于2012.04.26
	long	 algp;
	long	 algp_b;
	long	 algp_c;
	long	 alga;
	long	 alga_b;
	long	 alga_c;
	int		 ialgp;
	int		 ialgp_b;
	int		 ialgp_c;
	int		 ialga;
	int		 ialga_b;
	int		 ialga_c;
//张瑞雪添加于20120503
	long    algv_a;
	int    ialgv_a;
	float    v_a;
	float    adeg_a;
	long	 algp_a;
	long	 alga_a;
	int		 ialgp_a;
	int		 ialga_a;
	float    wmx;
	int     slackavr;
public:

	//constructors
	CLS_BUS();
	CLS_BUS(const long m_id);

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

	//assign value operator
	const CLS_BUS& operator = (const CLS_BUS& m_bus);

	//compare operator ==
	int operator == (const CLS_BUS& m_bus) const
	{
		return id == m_bus.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_BUS& m_bus) const
	{
		return id < m_bus.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( BUS )
};

//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_FI //: public CObject
{
public:

	long    id;
	long	pid;
	long	nd;
	char 	descr[64];
	int		jdpoint;
	int		dlpoint;
	int    pos;
	long    sec;
	int    r_sec;
	long    digital;
	int    idigital;
	int 	topflags;

public:

	//constructors
	CLS_FI()
	{
		id = 0;
		pid = 0;
		pos = -1;
		sec = 0;
		r_sec = -1;
		digital = 0;
		idigital = -1;
		nd = 0;
		jdpoint = 0;
		dlpoint = 0;
		topflags = 0;
	}

	CLS_FI(const long m_id)
	{
		id = m_id;
		pid = 0;
		pos = -1;
		sec = 0;
		r_sec = -1;
		digital = 0;
		idigital = -1;
		nd = 0;
		jdpoint = 0;
		dlpoint = 0;
		topflags = 0;
	}

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

	//assign value operator
	const CLS_FI& operator = (const CLS_FI& m_fi)
	{
		id = m_fi.id;
		strcpy(descr, m_fi.descr);
		pid = m_fi.pid;
		nd = m_fi.nd;
		pos = m_fi.pos;
		sec = m_fi.sec;
		r_sec = m_fi.r_sec;
		digital = m_fi.digital;
		idigital = m_fi.idigital;

		return *this;
	}

	//compare operator ==
	int operator == (const CLS_FI& m_fi) const
	{
		return id == m_fi.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_FI& CLS_FI) const
	{
		return id < CLS_FI.id;
	}
};

//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_SEC_FI //: public CObject
{
public:
	long	id;
	long	sec_id;
	long	fi_id;
	int		sec_id_ptr;
	int		fi_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_SEC_FI()
	{
		sec_id = 0;
		fi_id = 0;
		sec_id_ptr = -1;
		fi_id_ptr = -1;
		fptr = -1;
		bptr = -1;
	}

	CLS_R_SEC_FI(const long m_sec_id, const long m_fi_id)
	{
		sec_id = m_sec_id;
		fi_id = m_fi_id;

		sec_id_ptr = -1;
		fi_id_ptr = -1;
		fptr = -1;
		bptr = -1;
	}

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_SEC_FI& operator =(const CLS_R_SEC_FI& m_r_sec_fi)
	{
		sec_id = m_r_sec_fi.sec_id;
		fi_id = m_r_sec_fi.fi_id;

		sec_id_ptr = m_r_sec_fi.sec_id_ptr;
		fi_id_ptr = m_r_sec_fi.fi_id_ptr;
		fptr = m_r_sec_fi.fptr;
		bptr = m_r_sec_fi.bptr;

		return *this;
	}

	//compare operator ==
	int operator == (const CLS_R_SEC_FI& m_r_sec_fi) const
	{
		return (sec_id == m_r_sec_fi.sec_id)
				&& (fi_id == m_r_sec_fi.fi_id);
	}


	void GetKey(char *key) const {sprintf(key, "%ld_%ld", sec_id, fi_id);}

	//compare operator <
	int operator < (const CLS_R_SEC_FI& m_r_sec_fi) const
	{
		if (sec_id < m_r_sec_fi.sec_id)
		{
			return true;
		}
		else if (sec_id == m_r_sec_fi.sec_id)
		{
			return fi_id < m_r_sec_fi.fi_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return sec_id;
	}

	//get child id
	const long GetChildId() const
	{
		return fi_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return sec_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return fi_id_ptr;
	}
};


///////荣义平添加于2012.5.29用于生成节点与CP关系表
class CLS_R_ND_CP //: public CObject
{
public:
	long	id;
	long	nd_id;
	long	cp_id;
	int		nd_id_ptr;
	int		cp_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ND_CP();
	CLS_R_ND_CP(const long m_nd_id, const long m_cp_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ND_CP& operator = (const CLS_R_ND_CP& m_r_nd_cp);

	//compare operator ==
	int operator == (const CLS_R_ND_CP& m_r_nd_cp) const
	{
		return cp_id == m_r_nd_cp.cp_id
				&& nd_id == m_r_nd_cp.nd_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", nd_id, cp_id);}

	//compare operator <
	int operator < (const CLS_R_ND_CP& m_r_nd_cb) const
	{
		if (nd_id < m_r_nd_cb.nd_id)
		{
			return true;
		}
		else if (nd_id == m_r_nd_cb.nd_id)
		{
			return cp_id < m_r_nd_cb.cp_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return nd_id;
	}

	//get child id
	const long GetChildId() const
	{
		return cp_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return nd_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return cp_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ND_CB )
};

///////荣义平添加于2012.5.29用于生成节点与LD关系表
class CLS_R_ND_LD //: public CObject
{
public:
	long	id;
	long	nd_id;
	long	ld_id;
	int		nd_id_ptr;
	int		ld_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ND_LD();
	CLS_R_ND_LD(const long m_nd_id, const long m_ld_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ND_LD& operator = (const CLS_R_ND_LD& m_r_nd_ld);

	//compare operator ==
	int operator == (const CLS_R_ND_LD& m_r_nd_cb) const
	{
		return ld_id == m_r_nd_cb.ld_id
				&& nd_id == m_r_nd_cb.nd_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", nd_id, ld_id);}

	//compare operator <
	int operator < (const CLS_R_ND_LD& m_r_nd_ld) const
	{
		if (nd_id < m_r_nd_ld.nd_id)
		{
			return true;
		}
		else if (nd_id == m_r_nd_ld.nd_id)
		{
			return ld_id < m_r_nd_ld.ld_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return nd_id;
	}

	//get child id
	const long GetChildId() const
	{
		return ld_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return nd_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return ld_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ND_CB )
};

//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_ZN //: public CObject
{
public:

	long    id;
	long	stid;
	char    descr[MAX_ZN_DESCR_LEN];
	int		type;
	vector<NODE_DATA> nds;
	int		q;
	int		topflags;
	int		r_st;
	int		r_ld;
	int		r_sec;
	int 	r_nd;

public:

	//constructors
	CLS_ZN();
	CLS_ZN(const long m_id);

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

	//assign value operator
	const CLS_ZN& operator = (const CLS_ZN& m_zn);

	//compare operator ==
	int operator == (const CLS_ZN& m_zn) const
	{
		return id == m_zn.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_ZN& m_zn) const
	{
		return id < m_zn.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( ZN )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_SEC //: public CObject
{
public:

	long	id;
	long	pid;
	char    descr[MAX_SEC_DESCR_LEN];
	long    nd;
	long    znd;
	int		ind;
	int		iznd;
	int		ibs;
	int		zbs;
	float	r0;
	float	x0;
	float   b0;
	int		q;
	int		topflags;
	long	algip;
	int		ialgip;
	long	algiq;
	int		ialgiq;
	long	algii;
	int		ialgii;
	long	algzp;
	int		ialgzp;
	long	algzq;
	int		ialgzq;
	long	algzi;
	int		ialgzi;
	float	w;
	float	r;
	float	i;
	float	wz;
	float	rz;
	float	iz;
	int		r_zn;
	long	zn_id;
//张瑞雪添加于20120209
	float    x0_ab;
	float    x0_ac;
	float    x0_ba;
	float    x0_bb;
	float    x0_bc;
	float    x0_ca;
	float    x0_cb;
	float    x0_cc;
	float    r0_ab;
	float    r0_ac;
	float    r0_ba;
	float    r0_bb;
	float    r0_bc;
	float    r0_ca;
	float    r0_cb;
	float    r0_cc;
	float    b0_ab;
	float    b0_ac;
	float    b0_ba;
	float    b0_bb;
	float    b0_bc;
	float    b0_ca;
	float    b0_cb;
	float    b0_cc;
	long    algip_b;
	long    algip_c;
	int    ialgip_b;
	int    ialgip_c;
	long    algiq_b;
	long    algiq_c;
	int    ialgiq_b;
	int    ialgiq_c;
	long    algii_b;
	long    algii_c;
	int    ialgii_b;
	int    ialgii_c;
	long    algzp_b;
	long    algzp_c;
	int    ialgzp_b;
	int    ialgzp_c;
	long    algzq_b;
	long    algzq_c;
	int    ialgzq_b;
	int    ialgzq_c;
	long    algzi_b;
	long    algzi_c;
	int    ialgzi_b;
	int    ialgzi_c;
	float    w_b;
	float    w_c;
	float    r_b;
	float    r_c;
	float    i_b;
	float    i_c;
	float    wz_b;
	float    wz_c;
	float    rz_b;
	float    rz_c;
	float    iz_b;
	float    iz_c;
	int 	r_fi;
//张瑞雪添加于2012.4.12
	int		phases;
	float	l;
	int 	mode;
//张瑞雪添加于20120503
	float  r0_aa;
	float  x0_aa;
	float  b0_aa;
	float  w_a;
	float  r_a;
	float  i_a;
	float  wz_a;
	float  rz_a;
	float  iz_a;
	long    algip_a;
	int    ialgip_a;
	long    algiq_a;
	int    ialgiq_a;
	long    algii_a;
	int    ialgii_a;
	long    algzp_a;
	int    ialgzp_a;
	long    algzq_a;
	int    ialgzq_a;
	char    algzi_a;
	int    ialgzi_a;

public:

	//constructors
	CLS_SEC();
	CLS_SEC(const long m_id);

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

	//assign value operator
	const CLS_SEC& operator = (const CLS_SEC& m_sec);

	//compare operator ==
	int operator == (const CLS_SEC& m_sec) const
	{
		return id == m_sec.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_SEC& m_sec) const
	{
		return id < m_sec.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( SEC )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_ND //: public CObject
{
public:

	long	id;
	float	vl;
	int		q;
	int		ibs;
	int     primebs;
	int     fch;
	int		bch;
	int		r_cb;
	int		r_switch;
	int		r_xf;
	long	zn_id;
	int		r_sec;
	int		r_cp;
	int		r_ld;

public:

	//constructors
	CLS_ND();
	CLS_ND(const long m_id);

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

	//assign value operator
	const CLS_ND& operator = (const CLS_ND& m_nd);

	//compare operator ==
	int operator == (const CLS_ND& m_nd) const
	{
		return id == m_nd.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_ND& m_nd) const
	{
		return id < m_nd.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( ND )
};


//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_BS //: public CObject
{
public:

	int		id;
	int		iisland;
	int		q;
	int		qq;
	int		fbs;
	int		bbs;
	int		pnd;
	int 	valid;
	float	kv;
	float	v;
	float	adeg;
	float	w;
	float	r;
	float    kv_b;
	float    kv_c;
	float    v_b;
	float    v_c;
	float    adeg_b;
	float    adeg_c;
	float    w_b;
	float    w_c;
	float    r_b;
	float    r_c;
//张瑞雪添加于20120503
	float    w_a;
	float    r_a;
	float    kv_a;
	float    v_a;
	float    adeg_a;

public:

	//constructors
	CLS_BS();
	CLS_BS(int m_id);

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

	//assign value operator
	const CLS_BS& operator = (const CLS_BS& m_bs);

	//compare operator ==
	int operator == (const CLS_BS& m_bs) const
	{
		return id == m_bs.id;
	}

	void GetKey(char *key) const {sprintf(key, "%d", id);}

	//compare operator <
	int operator < (const CLS_BS& m_bs) const
	{
		return id < m_bs.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( BS )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_ISLAND //: public CObject
{
public:

	int		id;
	int		refbs;
	int		refso;
	int 	valid;
	float	vref;
	float   wload;
	float   rload;
	float   wso;
	float   rso;
	float    vref_b;
	float    vref_c;
	float    wload_b;
	float    wload_c;
	float    rload_b;
	float    rload_c;
	float    wso_b;
	float    wso_c;
	float    rso_b;
	float    rso_c;
//张瑞雪添加于20120503
	float    wso_a;
	float    rso_a;
	float	 wload_a;
	float    rload_a;
public:

	//constructors
	CLS_ISLAND();
	CLS_ISLAND(int m_id);

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

	//assign value operator
	const CLS_ISLAND& operator = (const CLS_ISLAND& m_island);

	//compare operator ==
	int operator == (const CLS_ISLAND& m_island) const
	{
		return id == m_island.id;
	}

	void GetKey(char *key) const {sprintf(key, "%d", id);}

	//compare operator <
	int operator < (const CLS_ISLAND& m_island) const
	{
		return id < m_island.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( ISLAND )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_GLOBAL //: public CObject
{
public:

	long		id;
	int		nullbs;
	int		nextbs;
	time_t  tptime;
	time_t	timedb;
	int		statusdb;
	int		error;
	int		dberrnum;

public:

	//constructors
	CLS_GLOBAL();
	CLS_GLOBAL(long m_id);

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

	//assign value operator
	const CLS_GLOBAL& operator = (const CLS_GLOBAL& m_global);

	//compare operator ==
	int operator == (const CLS_GLOBAL& m_global) const
	{
		return id == m_global.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_GLOBAL& m_global) const
	{
		return id < m_global.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( GLOBAL )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_TAPTY //: public CObject
{
public:

	long	id;
	long	pid;
	int		mn;
	int		mx;
	int		nom;
	float	step;

public:

	//constructors
	CLS_TAPTY();
	CLS_TAPTY(const long m_id);

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

	//assign value operator
	const CLS_TAPTY& operator = (const CLS_TAPTY& m_tapty);

	//compare operator ==
	int operator == (const CLS_TAPTY& m_tapty) const
	{
		return id == m_tapty.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_TAPTY& m_tapty) const
	{
		return id < m_tapty.id;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( TAPTY )
};




//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_CO_ST //: public CObject
{
public:
	long 	id;
	long	co_id;
	long	st_id;
	int		co_id_ptr;
	int		st_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_CO_ST();
	CLS_R_CO_ST(const long m_co_id, const long m_st_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_CO_ST& operator = (const CLS_R_CO_ST& m_r_co_st);

	//compare operator ==
	int operator == (const CLS_R_CO_ST& m_r_co_st) const
	{
		return co_id == m_r_co_st.co_id && st_id == m_r_co_st.st_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", co_id, st_id);}

	//compare operator <
	int operator < (const CLS_R_CO_ST& m_r_co_st) const
	{
		if (co_id < m_r_co_st.co_id)
		{
			return true;
		}
		else if (co_id == m_r_co_st.co_id)
		{
			return st_id < m_r_co_st.st_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return co_id;
	}

	//get child id
	const long GetChildId() const
	{
		return st_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return co_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return st_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_CO_ST )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ST_XFMR //: public CObject
{
public:
	long 	id;
	long	st_id;
	long	xfmr_id;
	int		st_id_ptr;
	int		xfmr_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_XFMR();
	CLS_R_ST_XFMR(const long m_st_id, const long m_xfmr_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_XFMR& operator = (const CLS_R_ST_XFMR& m_r_st_xfmr);

	//compare operator ==
	int operator == (const CLS_R_ST_XFMR& m_r_st_xfmr) const
	{
		return st_id == m_r_st_xfmr.st_id && xfmr_id == m_r_st_xfmr.xfmr_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, xfmr_id);}

	//compare operator <
	int operator < (const CLS_R_ST_XFMR& m_r_st_xfmr) const
	{
		if (st_id < m_r_st_xfmr.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_xfmr.st_id)
		{
			return xfmr_id < m_r_st_xfmr.xfmr_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return xfmr_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return xfmr_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_XFMR )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_XFMR_XF //: public CObject
{
public:
	long	id;
	long	xfmr_id;
	long	xf_id;
	int		xfmr_id_ptr;
	int		xf_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_XFMR_XF();
	CLS_R_XFMR_XF(const long m_xfmr_id, const long m_xf_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_XFMR_XF& operator = (const CLS_R_XFMR_XF& m_r_xfmr_xf);

	//compare operator ==
	int operator == (const CLS_R_XFMR_XF& m_r_xfmr_xf) const
	{
		return xfmr_id == m_r_xfmr_xf.xfmr_id && xf_id == m_r_xfmr_xf.xf_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", xfmr_id, xf_id);}

	//compare operator <
	int operator < (const CLS_R_XFMR_XF& m_r_xfmr_xf) const
	{
		if (xfmr_id < m_r_xfmr_xf.xfmr_id)
		{
			return true;
		}
		else if (xfmr_id == m_r_xfmr_xf.xfmr_id)
		{
			return xf_id < m_r_xfmr_xf.xf_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return xfmr_id;
	}

	//get child id
	const long GetChildId() const
	{
		return xf_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return xfmr_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return xf_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_XFMR_XF )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ST_CB //: public CObject
{
public:
	long	id;
	long	st_id;
	long	cb_id;
	int		st_id_ptr;
	int		cb_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_CB();
	CLS_R_ST_CB(const long m_st_id, const long m_cb_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_CB& operator = (const CLS_R_ST_CB& m_r_st_cb);

	//compare operator ==
	int operator == (const CLS_R_ST_CB& m_r_st_cb) const
	{
		return st_id == m_r_st_cb.st_id && cb_id == m_r_st_cb.cb_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, cb_id);}

	//compare operator <
	int operator < (const CLS_R_ST_CB& m_r_st_cb) const
	{
		if (st_id < m_r_st_cb.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_cb.st_id)
		{
			return cb_id < m_r_st_cb.cb_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return cb_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return cb_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_CB )
};


class CLS_R_ST_SWITCH //: public CObject
{
public:
	long	id;
	long	st_id;
	long	switch_id;
	int		st_id_ptr;
	int		switch_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_SWITCH();
	CLS_R_ST_SWITCH(const long m_st_id, const long m_cb_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_SWITCH& operator = (const CLS_R_ST_SWITCH& m_r_st_cb);

	//compare operator ==
	int operator == (const CLS_R_ST_SWITCH& m_r_st_cb) const
	{
		return st_id == m_r_st_cb.st_id && switch_id == m_r_st_cb.switch_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, switch_id);}

	//compare operator <
	int operator < (const CLS_R_ST_SWITCH& m_r_st_cb) const
	{
		if (st_id < m_r_st_cb.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_cb.st_id)
		{
			return switch_id < m_r_st_cb.switch_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return switch_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return switch_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_CB )
};

//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ST_SO //: public CObject
{
public:
	long	id;
	long	st_id;
	long	so_id;
	int		st_id_ptr;
	int		so_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_SO();
	CLS_R_ST_SO(const long m_st_id, const long m_so_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_SO& operator = (const CLS_R_ST_SO& m_r_st_so);

	//compare operator ==
	int operator == (const CLS_R_ST_SO& m_r_st_so) const
	{
		return st_id == m_r_st_so.st_id && so_id == m_r_st_so.so_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, so_id);}

	//compare operator <
	int operator < (const CLS_R_ST_SO& m_r_st_so) const
	{
		if (st_id < m_r_st_so.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_so.st_id)
		{
			return so_id < m_r_st_so.so_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return so_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return so_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_SO )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ST_LD //: public CObject
{
public:
	long 	id;
	long	st_id;
	long	ld_id;
	int		st_id_ptr;
	int		ld_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_LD();
	CLS_R_ST_LD(const long m_st_id, const long m_ld_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_LD& operator = (const CLS_R_ST_LD& m_r_st_ld);

	//compare operator ==
	int operator == (const CLS_R_ST_LD& m_r_st_ld) const
	{
		return st_id == m_r_st_ld.st_id && ld_id == m_r_st_ld.ld_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, ld_id);}

	//compare operator <
	int operator < (const CLS_R_ST_LD& m_r_st_ld) const
	{
		if (st_id < m_r_st_ld.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_ld.st_id)
		{
			return ld_id < m_r_st_ld.ld_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return ld_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return ld_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_LD )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ST_CP //: public CObject
{
public:
	long	id;
	long	st_id;
	long	cp_id;
	int		st_id_ptr;
	int		cp_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_CP();
	CLS_R_ST_CP(const long m_st_id, const long m_cp_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_CP& operator = (const CLS_R_ST_CP& m_r_st_cp);

	//compare operator ==
	int operator == (const CLS_R_ST_CP& m_r_st_cp) const
	{
		return st_id == m_r_st_cp.st_id && cp_id == m_r_st_cp.cp_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, cp_id);}

	//compare operator <
	int operator < (const CLS_R_ST_CP& m_r_st_cp) const
	{
		if (st_id < m_r_st_cp.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_cp.st_id)
		{
			return cp_id < m_r_st_cp.cp_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return cp_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return cp_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_CP )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ST_BUS //: public CObject
{
public:
	long	id;
	long	st_id;
	long	bus_id;
	int		st_id_ptr;
	int		bus_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_BUS();
	CLS_R_ST_BUS(const long m_st_id, const long m_bus_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_BUS& operator = (const CLS_R_ST_BUS& m_r_st_bus);

	//compare operator ==
	int operator == (const CLS_R_ST_BUS& m_r_st_bus) const
	{
		return st_id == m_r_st_bus.st_id && bus_id == m_r_st_bus.bus_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, bus_id);}

	//compare operator <
	int operator < (const CLS_R_ST_BUS& m_r_st_bus) const
	{
		if (st_id < m_r_st_bus.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_bus.st_id)
		{
			return bus_id < m_r_st_bus.bus_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return bus_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return bus_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_BUS )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ST_ZN //: public CObject
{
public:
	long	id;
	long	st_id;
	long	zn_id;
	int		st_id_ptr;
	int		zn_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ST_ZN();
	CLS_R_ST_ZN(const long m_st_id, const long m_zn_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ST_ZN& operator = (const CLS_R_ST_ZN& m_r_st_zn);

	//compare operator ==
	int operator == (const CLS_R_ST_ZN& m_r_st_zn) const
	{
		return st_id == m_r_st_zn.st_id && zn_id == m_r_st_zn.zn_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", st_id, zn_id);}

	//compare operator <
	int operator < (const CLS_R_ST_ZN& m_r_st_zn) const
	{
		if (st_id < m_r_st_zn.st_id)
		{
			return true;
		}
		else if (st_id == m_r_st_zn.st_id)
		{
			return zn_id < m_r_st_zn.zn_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return st_id;
	}

	//get child id
	const long GetChildId() const
	{
		return zn_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return st_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return zn_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ST_ZN )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ZN_LD //: public CObject
{
public:
	long	id;
	long	zn_id;
	long	ld_id;
	int		zn_id_ptr;
	int		ld_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ZN_LD();
	CLS_R_ZN_LD(const long m_zn_id, const long m_ld_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ZN_LD& operator = (const CLS_R_ZN_LD& m_r_zn_ld);

	//compare operator ==
	int operator == (const CLS_R_ZN_LD& m_r_zn_ld) const
	{
		return zn_id == m_r_zn_ld.zn_id && ld_id == m_r_zn_ld.ld_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", zn_id, ld_id);}

	//compare operator <
	int operator < (const CLS_R_ZN_LD& m_r_zn_ld) const
	{
		if (zn_id < m_r_zn_ld.zn_id)
		{
			return true;
		}
		else if (zn_id == m_r_zn_ld.zn_id)
		{
			return ld_id < m_r_zn_ld.ld_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return zn_id;
	}

	//get child id
	const long GetChildId() const
	{
		return ld_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return zn_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return ld_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ZN_LD )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ZN_SEC //: public CObject
{
public:
	long	id;
	long	zn_id;
	long	sec_id;
	int		zn_id_ptr;
	int		sec_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ZN_SEC();
	CLS_R_ZN_SEC(const long m_zn_id, const long m_sec_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ZN_SEC& operator = (const CLS_R_ZN_SEC& m_r_zn_sec);

	//compare operator ==
	int operator == (const CLS_R_ZN_SEC& m_r_zn_sec) const
	{
		return zn_id == m_r_zn_sec.zn_id && sec_id == m_r_zn_sec.sec_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", zn_id, sec_id);}

	//compare operator <
	int operator < (const CLS_R_ZN_SEC& m_r_zn_sec) const
	{
		if (zn_id < m_r_zn_sec.zn_id)
		{
			return true;
		}
		else if (zn_id == m_r_zn_sec.zn_id)
		{
			return sec_id < m_r_zn_sec.sec_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return zn_id;
	}

	//get child id
	const long GetChildId() const
	{
		return sec_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return zn_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return sec_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ZN_SEC )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ND_CB //: public CObject
{
public:
	long	id;
	long	nd_id;
	long	cb_id;
	int		nd_id_ptr;
	int		cb_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ND_CB();
	CLS_R_ND_CB(const long m_nd_id, const long m_cb_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ND_CB& operator = (const CLS_R_ND_CB& m_r_nd_cb);

	//compare operator ==
	int operator == (const CLS_R_ND_CB& m_r_nd_cb) const
	{
		return nd_id == m_r_nd_cb.nd_id && cb_id == m_r_nd_cb.cb_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", nd_id, cb_id);}

	//compare operator <
	int operator < (const CLS_R_ND_CB& m_r_nd_cb) const
	{
		if (nd_id < m_r_nd_cb.nd_id)
		{
			return true;
		}
		else if (nd_id == m_r_nd_cb.nd_id)
		{
			return cb_id < m_r_nd_cb.cb_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return nd_id;
	}

	//get child id
	const long GetChildId() const
	{
		return cb_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return nd_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return cb_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ND_CB )
};

class CLS_R_ND_SWITCH //: public CObject
{
public:
	long	id;
	long	nd_id;
	long	switch_id;
	int		nd_id_ptr;
	int		switch_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ND_SWITCH();
	CLS_R_ND_SWITCH(const long m_nd_id, const long m_cb_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ND_SWITCH& operator = (const CLS_R_ND_SWITCH& m_r_nd_cb);

	//compare operator ==
	int operator == (const CLS_R_ND_SWITCH& m_r_nd_cb) const
	{
		return nd_id == m_r_nd_cb.nd_id && switch_id == m_r_nd_cb.switch_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", nd_id, switch_id);}

	//compare operator <
	int operator < (const CLS_R_ND_SWITCH& m_r_nd_cb) const
	{
		if (nd_id < m_r_nd_cb.nd_id)
		{
			return true;
		}
		else if (nd_id == m_r_nd_cb.nd_id)
		{
			return switch_id < m_r_nd_cb.switch_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return nd_id;
	}

	//get child id
	const long GetChildId() const
	{
		return switch_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return nd_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return switch_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ND_CB )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ND_XF //: public CObject
{
public:
	long	id;
	long	nd_id;
	long	xf_id;
	int		nd_id_ptr;
	int		xf_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ND_XF();
	CLS_R_ND_XF(const long m_nd_id, const long m_xf_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ND_XF& operator = (const CLS_R_ND_XF& m_r_nd_xf);

	//compare operator ==
	int operator == (const CLS_R_ND_XF& m_r_nd_xf) const
	{
		return nd_id == m_r_nd_xf.nd_id && xf_id == m_r_nd_xf.xf_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", nd_id, xf_id);}

	//compare operator <
	int operator < (const CLS_R_ND_XF& m_r_nd_xf) const
	{
		if (nd_id < m_r_nd_xf.nd_id)
		{
			return true;
		}
		else if (nd_id == m_r_nd_xf.nd_id)
		{
			return xf_id < m_r_nd_xf.xf_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return nd_id;
	}

	//get child id
	const long GetChildId() const
	{
		return xf_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return nd_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return xf_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ND_XF )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ZN_ND //: public CObject
{
public:
	long	id;
	long	nd_id;
	long	zn_id;
	int		nd_id_ptr;
	int		zn_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ZN_ND();
	CLS_R_ZN_ND(const long m_zn_id, const long m_nd_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ZN_ND& operator = (const CLS_R_ZN_ND& m_r_nd_zn);

	//compare operator ==
	int operator == (const CLS_R_ZN_ND& m_r_nd_zn) const
	{
		return nd_id == m_r_nd_zn.nd_id && zn_id == m_r_nd_zn.zn_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", nd_id, zn_id);}

	//compare operator <
	int operator < (const CLS_R_ZN_ND& m_r_nd_zn) const
	{
		if (zn_id < m_r_nd_zn.zn_id)
		{
			return true;
		}
		else if (zn_id == m_r_nd_zn.zn_id)
		{
			return nd_id < m_r_nd_zn.nd_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return zn_id;
	}

	//get child id
	const long GetChildId() const
	{
		return nd_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return zn_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return nd_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ND_ZN )
};


//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CLS_R_ND_SEC //: public CObject
{
public:
	long	id;
	long	nd_id;
	long	sec_id;
	int		nd_id_ptr;
	int		sec_id_ptr;
	int		fptr;
	int		bptr;

public:

	//constructors
	CLS_R_ND_SEC();
	CLS_R_ND_SEC(const long m_nd_id, const long m_sec_id);

	bool IsRTable() const {return true;}	//判断是否关系表

	//assign value operator
	const CLS_R_ND_SEC& operator = (const CLS_R_ND_SEC& m_r_nd_sec);

	//compare operator ==
	int operator == (const CLS_R_ND_SEC& m_r_nd_sec) const
	{
		return nd_id == m_r_nd_sec.nd_id && sec_id == m_r_nd_sec.sec_id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld_%ld", nd_id, sec_id);}

	//compare operator <
	int operator < (const CLS_R_ND_SEC& m_r_nd_sec) const
	{
		if (nd_id < m_r_nd_sec.nd_id)
		{
			return true;
		}
		else if (nd_id == m_r_nd_sec.nd_id)
		{
			return sec_id < m_r_nd_sec.sec_id;
		}
		else
		{
			return false;
		}
	}

	//get parent id
	const long GetParentId() const
	{
		return nd_id;
	}

	//get child id
	const long GetChildId() const
	{
		return sec_id;
	}

	//get parent ptr
	int GetParentPtr() const
	{
		return nd_id_ptr;
	}

	//get child ptr
	int GetChildPtr() const
	{
		return sec_id_ptr;
	}

	//read or write object
    //void Serialize( CArchive& ar );

protected:

	 //DECLARE_SERIAL( R_ND_SEC )
};


//haozeyun 2011.11.23tianjia
class CLS_CTL
{
public:
	long   id;
	int	   method;
	int	   itermx;
	float  vtol;
	time_t start;
	float  second;
	int	   iter;
	long   convg;
public:
	//constructors
	CLS_CTL();
	CLS_CTL(int m_id);

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

	//assign value operator
	const CLS_CTL& operator = (const CLS_CTL& m_ctl);

	//compare operator ==
	int operator == (const CLS_CTL& m_ctl)const
	{
		return id == m_ctl.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	//compare operator <
	int operator < (const CLS_CTL& m_ctl)const
	{
		return convg < m_ctl.convg;
	}
};

//郝泽赟2012.4.24添加
class CLS_LNMODE
{
public:
	long     id;
	char     descr[MAX_LNMODE_CONVG_LEN];
	int    	 type;
	float    r_aa;
	float    r_ab;
	float    r_ac;
	float    r_ba;
	float    r_bb;
	float    r_bc;
	float    r_ca;
	float    r_cb;
	float    r_cc;
	float    x_aa;
	float    x_ab;
	float    x_ac;
	float    x_ba;
	float    x_bb;
	float    x_bc;
	float    x_ca;
	float    x_cb;
	float    x_cc;
	float    b_aa;
	float    b_ab;
	float    b_ac;
	float    b_ba;
	float    b_bb;
	float    b_bc;
	float    b_ca;
	float    b_cb;
	float    b_cc;
//张瑞雪添加于20120503
	float    r;
	float    x;
	float    b;
public:
	CLS_LNMODE();
	CLS_LNMODE(const char* m_id);

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

	const CLS_LNMODE& operator = (const CLS_LNMODE& m_lnmode);
	int operator == (const CLS_LNMODE& m_lnmode)const
	{
		return id == m_lnmode.id;
	}

	void GetKey(char *key) const {sprintf(key, "%ld", id);}

	int operator < (const CLS_LNMODE& m_lnmode)const
	{
		return id < m_lnmode.id;
	}
};
#endif
