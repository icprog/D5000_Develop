// netmom.cpp: //IMPLEMENTation of the netmom class.
//
//////////////////////////////////////////////////////////////////////


#include "netmom.h"
#include <iostream>

using namespace std;
///////////////////////////////////////////////////////////////////
// ---DMS project---
// netmom.cpp //IMPLEMENTion
// developed by mawenxiao.
// On September 21,2000

#define GET_R_TABLE_ID(id1, id2) \
{\
	KEY_STRU ks1, ks2;\
	CCommon::long_to_keyid(id1, &ks1);\
	CCommon::long_to_keyid(id2, &ks2);\
\
	id = (long(ks1.key) << 32) | ks2.key;\
}\

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "netmom.h"
#define PI 3.14159

#ifdef _CC2000_
/////////////////////////////////////////////////////////////////////////
// class ANALOG //IMPLEMENTation

//IMPLEMENT_SERIAL(ANALOG,CObject,0)

CLS_ANALOG::CLS_ANALOG()
{
    id[0] = 0;
    descr[0] = 0;
    value = 0;
    quality = 0;
}

const CLS_ANALOG& CLS_ANALOG::operator = (const CLS_ANALOG& m_ANALOG)
{
	strcpy(id,m_ANALOG.id);
	strcpy(descr,m_ANALOG.descr);
	value = m_ANALOG.value;
	quality = m_ANALOG.quality;
	return* this;
}
CLS_ANALOG::CLS_ANALOG(const char* m_id)
{
    strcpy(id, m_id);
    descr[0] = 0;
    value = 0.0;
    quality = 0;
}
//
//void ANALOG::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar << value;
//		ar << quality;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar >> value;
//		ar >> quality;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class BS //IMPLEMENTation

//IMPLEMENT_SERIAL(BS,CObject,0)
#endif
CLS_BS::CLS_BS()
{
    id = 0;
    iisland = -2;
    q = 0;
    valid = 0;
    qq = 0;
    fbs = -1;
    bbs = -1;
	pnd = 0;
	kv = 0;
	v = 1.0;
	adeg = 00.0;
	w = 0;
	r = 0;
//张瑞雪添加于20120209
	kv_b = 0;
	kv_c = 0;
	v_b = 1.0;
	v_c = 1.0;
	adeg_b = -2 * PI / 3;
	adeg_c = 2 * PI / 3;
	w_b = 0;
	w_c = 0;
	r_b = 0;
	r_c = 0;
//张瑞雪添加于20120503
	w_a = 0;
	r_a = 0;
	kv_a = 0;
	v_a = 0;
	adeg_a = 0;
}

const CLS_BS& CLS_BS::operator = (const CLS_BS& m_BS)
{
	id = m_BS.id;
	iisland = m_BS.iisland;
	q = m_BS.q;
	valid = m_BS.valid;
	qq = m_BS.qq;
	fbs = m_BS.fbs;
	bbs = m_BS.bbs;
	pnd = m_BS.pnd;
	kv = m_BS.kv;
	v = m_BS.v;
	adeg = m_BS.adeg;
	w = m_BS.w;
	r = m_BS.r;
//张瑞雪添加于20120209
	kv_b = m_BS.kv_b;
	v_b = m_BS.v_b;
	adeg_b = m_BS.adeg_b;
	w_b = m_BS.w_b;
	r_b = m_BS.r_b;
	kv_c = m_BS.kv_c;
	v_c = m_BS.v_c;
	adeg_c = m_BS.adeg_c;
	w_c= m_BS.w_c;
	r_c = m_BS.r_c;
//张瑞雪添加于20120503
	w_a = m_BS.w_a;
	r_a = m_BS.r_a;
	kv_a = m_BS.kv_a;
	v_a= m_BS.v_a;
	adeg_a = m_BS.adeg_a;
	return* this;
}
CLS_BS::CLS_BS(const int m_id)
{
    id = m_id;
    iisland = -2;
    q = 0;
    valid = 0;
    qq = 0;
    fbs = -1;
    bbs = -1;
    pnd = 0;
    kv = 0.0;
    v = 1.0;
	adeg = 0.0;
	w = 0.0;
	r = 0.0;
//张瑞雪添加于20120209
	kv_b = 0;
	kv_c = 0;
	v_b = 1.0;
	v_c = 1.0;
	adeg_b = -2 * PI / 3;
	adeg_c = 2 * PI / 3;
	w_b = 0;
	w_c = 0;
	r_b = 0;
	r_c = 0;
	//张瑞雪添加于20120503
	w_a = 0;
	r_a = 0;
	kv_a = 0;
	v_a = 0;
	adeg_a = 0;
}

//void BS::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar << id;
//		ar << iisland;
//		ar << q;
//		ar << qq;
//		ar << fbs;
//		ar << bbs;
//		ar << pnd;
//		ar << kv;
//		ar << v;
//		ar << adeg;
//		ar << w;
//		ar << r;
//	}
//	else
//	{
//		ar >> id;
//		ar >> iisland;
//		ar >> q;
//		ar >> qq;
//		ar >> fbs;
//		ar >> bbs;
//		ar >> pnd;
//		ar >> kv;
//		ar >> v;
//		ar >> adeg;
//		ar >> w;
//		ar >> r;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class BUS //IMPLEMENTation

//IMPLEMENT_SERIAL(BUS,CObject,0)

CLS_BUS::CLS_BUS()
{
	id = 0;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	ind = -1;
	ibs = -1;
	algv = 0;
	ialgv = -1;
	topflags = 0;
	vtarget = 0;
	vl = 10.00;
	vhlim = 999;
	vllim = 0;
	v = 1.0;
	adeg = 0.0;
	r_st = -1;
	stid = 0;
//张瑞雪添加于20120209
	algv_b = 0;
	algv_c = 0;
	ialgv_b = -1;
	ialgv_c = -1;
	vtarget_b = 0;
	vtarget_c = 0;
	vhlim_b = 999;
	vhlim_c = 999;
	vllim_b = 0;
	vllim_c = 0;
	v_b = 1.0;
	v_c = 1.0;
	adeg_b = -2 * PI / 3;
	adeg_c = 2 * PI / 3;

//郝泽赟添加于2012.04.26
	algp = 0;
	algp_b = 0;
	algp_c = 0;
	alga = 0;
	alga_b = 0;
	alga_c = 0;
	ialgp = -1;
	ialgp_b = -1;
	ialgp_c = -1;
	ialga = -1;
	ialga_b = -1;
	ialga_c = -1;
//张瑞雪添加于20120503
	algv_a = 0;
	ialgv_a = -1;
	v_a = 0;
	adeg_a = 0;
	algp_a = 0;
	alga_a = 0;
	ialgp_a = -1;
	ialga_a = -1;
	wmx = 0.0;
	slackavr = -1;
}

const CLS_BUS& CLS_BUS::operator = (const CLS_BUS& m_BUS)
{
	id = m_BUS.id;
	pid = m_BUS.pid;
	strcpy(descr, m_BUS.descr);
	nd = m_BUS.nd;
	ind = m_BUS.ind;
	ibs = m_BUS.ibs;
	algv = m_BUS.algv;
	ialgv = m_BUS.ialgv;
	topflags = m_BUS.topflags;
	vtarget = m_BUS.vtarget;
	vl = m_BUS.vl;
	vhlim = m_BUS.vhlim;
	vllim = m_BUS.vllim;
	v = m_BUS.v;
	adeg = m_BUS.adeg;
	r_st = m_BUS.r_st;
//张瑞雪添加于20120209
	algv_b = m_BUS.algv_b;
	ialgv_b = m_BUS.ialgv_b;
	vtarget_b = m_BUS.vtarget_b;
	vhlim_b = m_BUS.vhlim_b;
	vllim_b = m_BUS.vllim_b;
	v_b = m_BUS.v_b;
	adeg_b = m_BUS.adeg_b;

	algv_c = m_BUS.algv_c;
	ialgv_c = m_BUS.ialgv_c;
	vtarget_c = m_BUS.vtarget_c;
	vhlim_c = m_BUS.vhlim_c;
	vllim_c = m_BUS.vllim_c;
	v_c = m_BUS.v_c;
	adeg_c = m_BUS.adeg_c;

	stid = m_BUS.stid;

	//郝泽赟添加于2012.04.26
	algp = m_BUS.algp;
	algp_b = m_BUS.algp_b;
	algp_c = m_BUS.algp_c;
	alga = m_BUS.alga;
	alga_b = m_BUS.alga_b;
	alga_c = m_BUS.alga_c;
	ialgp = m_BUS.ialgp;
	ialgp_b = m_BUS.ialgp_b;
	ialgp_c = m_BUS.ialgp_c;
	ialga = m_BUS.ialga;
	ialga_b = m_BUS.ialga_b;
	ialga_c = m_BUS.ialga_c;
	//张瑞雪添加于20120503
	algv_a = m_BUS.algv_a;
	ialgv_a = m_BUS.ialgv_a;
	v_a = m_BUS.v_a;
	adeg_a = m_BUS.adeg_a;
	algp_a = m_BUS.algp_a;
	alga_a = m_BUS.alga_a;
	ialgp_a = m_BUS.ialgp_a;
	ialga_a = m_BUS.ialga_a;
	wmx = m_BUS.wmx;
	slackavr = m_BUS.slackavr;
	return *this;
}
CLS_BUS::CLS_BUS(const long m_id)
{
	id = m_id;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	ind = -1;
	ibs = -1;
	algv = 0;
	ialgv = -1;
	topflags = 0;
	vtarget = 0.0;
	vl = 10.0;
	vhlim = 999;
	vllim = 0.0;
	v = 1.0;
	adeg = 0.0;
	r_st = -1;
	stid = 0;
//张瑞雪添加于20120209
	algv_b = 0;
	algv_c = 0;
	ialgv_b = -1;
	ialgv_c = -1;
	vtarget_b = 0;
	vtarget_c = 0;
	vhlim_b = 999;
	vhlim_c = 999;
	vllim_b = 0;
	vllim_c = 0;
	v_b = 1.0;
	v_c = 1.0;
	adeg_b = -2 * PI / 3;
	adeg_c = 2 * PI / 3;

	//郝泽赟添加于2012.04.26
	algp = 0;
	algp_b = 0;
	algp_c = 0;
	alga = 0;
	alga_b = 0;
	alga_c = 0;
	ialgp = -1;
	ialgp_b = -1;
	ialgp_c = -1;
	ialga = -1;
	ialga_b = -1;
	ialga_c = -1;
//张瑞雪添加于20120503
	algv_a = 0;
	ialgv_a = -1;
	v_a = 0;
	adeg_a = 0;
	algp_a = 0;
	alga_a = 0;
	ialgp_a = -1;
	ialga_a = -1;
	wmx = 0;
	slackavr = -1;
}

//void BUS::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar.Write(nd,20);
//		ar << ind;
//		ar << ibs;
//		ar.Write(algv,20);
//		ar << ialgv;
//		ar << topflags;
//		ar << vtarget;
//		ar << vl;
//		ar << vhlim;
//		ar << vllim;
//		ar << v;
//		ar << adeg;
//		ar << r_st;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar.Read(nd,20);
//		ar >> ind;
//		ar >> ibs;
//		ar.Read(algv,20);
//		ar >> ialgv;
//		ar >> topflags;
//		ar >> vtarget;
//		ar >> vl;
//		ar >> vhlim;
//		ar >> vllim;
//		ar >> v;
//		ar >> adeg;
//		ar >> r_st;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class CB //IMPLEMENTation

//IMPLEMENT_SERIAL(CB,CObject,0)

CLS_CB::CLS_CB()
{
	id = 0;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	znd = 0;
	ind = -1;
	iznd = -1;
//	digital = 0;
//	idigital = -1;
	q = 0;
	topflags = 0;
	r_st = -1;
	stid = 0;
}

const CLS_CB& CLS_CB::operator = (const CLS_CB& m_CB)
{
	id = m_CB.id;
	pid = m_CB.pid;
	strcpy(descr, m_CB.descr);
	nd = m_CB.nd;
	znd = m_CB.znd;
	ind = m_CB.ind;
	iznd = m_CB.iznd;
//	digital = m_CB.digital;
//	idigital = m_CB.idigital;
	q = m_CB.q;
	topflags = m_CB.topflags;
	r_st = m_CB.r_st;
	stid = m_CB.stid;
	return *this;
}
CLS_CB::CLS_CB(const long m_id)
{
    id= m_id;
    pid=0;
    descr[0] = 0;
    nd = 0;
    znd = 0;
    ind = -1;
    iznd = -1;
//    digital = 0;
//    idigital = -1;
    q = 0;
    topflags = 0;
    r_st = -1;
    stid=0;
}

////////////SWITCH
CLS_SWITCH::CLS_SWITCH()
{
	id = 0;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	znd = 0;
	ind = -1;
	iznd = -1;
//	digital = 0;
//	idigital = -1;
	q = 0;
	topflags = 0;
	r_st = -1;
	stid = 0;
}

const CLS_SWITCH& CLS_SWITCH::operator = (const CLS_SWITCH& m_CB)
{
	id = m_CB.id;
	pid = m_CB.pid;
	strcpy(descr, m_CB.descr);
	nd = m_CB.nd;
	znd = m_CB.znd;
	ind = m_CB.ind;
	iznd = m_CB.iznd;
//	digital = m_CB.digital;
//	idigital = m_CB.idigital;
	q = m_CB.q;
	topflags = m_CB.topflags;
	r_st = m_CB.r_st;
	stid = m_CB.stid;
	return *this;
}
CLS_SWITCH::CLS_SWITCH(const long m_id)
{
    id= m_id;
    pid=0;
    descr[0] = 0;
    nd = 0;
    znd = 0;
    ind = -1;
    iznd = -1;
//    digital = 0;
//    idigital = -1;
    q = 0;
    topflags = 0;
    r_st = -1;
    stid=0;
}

//void CB::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar.Write(nd,20);
//		ar.Write(znd,20);
//		ar << ind;
//		ar << iznd;
//		ar.Write(digital,20);
//		ar << idigital;
//		ar << q;
//		ar << topflags;
//		ar << r_st;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar.Read(nd,20);
//		ar.Read(znd,20);
//		ar >> ind;
//		ar >> iznd;
//		ar.Read(digital,20);
//		ar >> idigital;
//		ar >> q;
//		ar >> topflags;
//		ar >> r_st;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class CO //IMPLEMENTation

//IMPLEMENT_SERIAL(CO,CObject,0)

CLS_CO::CLS_CO()
{
    id = 0;
    pid = 0;
    descr[0] = 0;
    wloss = 0;
    rloss = 0;
    wso = 0;
	rso = 0;
	wload = 0;
	rload = 0;
	cap = 0;
	reac = 0;
	r_st = -1;
//张瑞雪添加于20120209
	wloss_b = 0;
	wloss_c = 0;
	rloss_b = 0;
	rloss_c = 0;
	wso_b = 0;
	wso_c = 0;
	rso_b = 0;
	rso_c = 0;
	wload_b = 0;
	wload_c = 0;
	rload_b = 0;
	rload_c = 0;
	cap_b = 0;
	cap_c = 0;
	reac_b = 0;
	reac_c = 0;
//张瑞雪添加于20120503
	wloss_a = 0;
	rloss_a = 0;
	wso_a = 0;
	rso_a = 0;
	wload_a = 0;
	rload_a = 0;
	cap_a = 0;
	reac_a = 0;
}

const CLS_CO& CLS_CO::operator = (const CLS_CO& m_CO)
{
	id=m_CO.id;
	pid=m_CO.pid;
	strcpy(descr,m_CO.descr);
	wloss = m_CO.wloss;
	rloss = m_CO.rloss;
	wso = m_CO.wso;
	rso = m_CO.rso;
	wload = m_CO.wload;
	rload = m_CO.rload;
	cap = m_CO.cap;
	reac = m_CO.reac;
//张瑞雪添加于20120209
	wloss_b = m_CO.wloss_b;
	rloss_b = m_CO.rloss_b;
	wso_b = m_CO.wso_b;
	rso_b = m_CO.rso_b;
	wload_b = m_CO.wload_b;
	rload_b = m_CO.rload_b;
	cap_b = m_CO.cap_b;
	reac_b = m_CO.reac_b;
	wloss_c = m_CO.wloss_c;
	rloss_c = m_CO.rloss_c;
	wso_c = m_CO.wso_c;
	rso_c  = m_CO.rso_c ;
	wload_c  = m_CO.wload_c ;
	rload_c  = m_CO.rload_c ;
	cap_c  = m_CO.cap_c ;
	reac_c  = m_CO.reac_c ;
//张瑞雪添加于20120503
	wloss_a = m_CO.wloss_a;
	rloss_a = m_CO.rloss_a;
	wso_a = m_CO.wso_a;
	rso_a = m_CO.rso_a;
	wload_a = m_CO.wload_a;
	rload_a = m_CO.rload_a;
	cap_a = m_CO.cap_a;
	reac_a = m_CO.reac_a;
	r_st = m_CO.r_st;
	return* this;
}
CLS_CO::CLS_CO(const long m_id)
{
    id = m_id;
    pid = 0;
    descr[0] = 0;
    wloss = 0.0;
    rloss = 0.0;
    wso = 0.0;
    rso = 0.0;
    wload = 0.0;
    rload = 0.0;
    cap = 0.0;
    reac = 0.0;
    r_st = -1;
//张瑞雪添加于20120209
    wloss_b = 0;
	wloss_c = 0;
	rloss_b = 0;
	rloss_c = 0;
	wso_b = 0;
	wso_c = 0;
	rso_b = 0;
	rso_c = 0;
	wload_b = 0;
	wload_c = 0;
	rload_b = 0;
	rload_c = 0;
	cap_b = 0;
	cap_c = 0;
	reac_b = 0;
	reac_c = 0;
//张瑞雪添加于20120503
	wloss_a = 0;
	rloss_a = 0;
	wso_a = 0;
	rso_a = 0;
	wload_a = 0;
	rload_a = 0;
	cap_a = 0;
	reac_a = 0;
}

//void CO::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar << wloss;
//		ar << rloss;
//		ar << wso;
//		ar << rso;
//		ar << wload;
//		ar << rload;
//		ar << cap;
//		ar << reac;
//		ar << r_st;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar >> wloss;
//		ar >> rloss;
//		ar >> wso;
//		ar >> rso;
//		ar >> wload;
//		ar >> rload;
//		ar >> cap;
//		ar >> reac;
//		ar >> r_st;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class CP //IMPLEMENTation

//IMPLEMENT_SERIAL(CP,CObject,0)

CLS_CP::CLS_CP()
{
	id = 0;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	ind = -1;
	ibs = -1;
	algq = 0;
	ialgq = -1;
	q = 0;
	topflags = 0;
	mrnom = 0;
	r = 0;
	r_st = -1;
	stid = 0;
//张瑞雪添加于20120209
	algq_b = 0;
	algq_c = 0;
	ialgq_b = -1;
	ialgq_c = -1;
	mrnom_b = 0;
	mrnom_c = 0;
	r_b = 0;
	r_c = 0;
//张瑞雪添加于20120503
	r_a = 0;
	algq_a = 0;
	ialgq_a = -1;
}

const CLS_CP& CLS_CP::operator = (const CLS_CP& m_CP)
{
	id = m_CP.id;
	pid = m_CP.pid;
	strcpy(descr, m_CP.descr);
	nd = m_CP.nd;
	ind = m_CP.ind;
	ibs = m_CP.ibs;
	algq = m_CP.algq;
	ialgq = m_CP.ialgq;
	q = m_CP.q;
	topflags = m_CP.topflags;
	mrnom = m_CP.mrnom;
	r = m_CP.r;
	r_st = m_CP.r_st;
	stid = m_CP.stid;
//张瑞雪添加于20120209
	algq_b = m_CP.algq_b;
	ialgq_b = m_CP.ialgq_b;
	mrnom_b = m_CP.mrnom_b;
	r_b = m_CP.r_b;
	algq_c = m_CP.algq_c;
	ialgq_c = m_CP.ialgq_c;
	mrnom_c = m_CP.mrnom_c;
	r_c = m_CP.r_c;
//张瑞雪添加于20120503
	algq_a = m_CP.algq_a;
	ialgq_a = m_CP.ialgq_a;
	r_a = m_CP.r_a;
	return *this;
}
CLS_CP::CLS_CP(const long m_id)
{
	id = m_id;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	ind = -1;
	ibs = -1;
	algq = 0;
	ialgq = -1;
	q = 0;
	topflags = 0;
	mrnom = 0.0;
	r = 0.0;
	r_st = -1;
	stid = 0;
//张瑞雪添加于20120209
	algq_b = 0;
	algq_c = 0;
	ialgq_b = -1;
	ialgq_c = -1;
	mrnom_b = 0;
	mrnom_c = 0;
	r_b = 0;
	r_c = 0;
	//张瑞雪添加于20120503
	r_a = 0;
	algq_a = 0;
	ialgq_a = -1;
}

//void CP::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar.Write(nd,20);
//		ar << ind;
//		ar << ibs;
//		ar.Write(algq,20);
//		ar << ialgq;
//		ar << q;
//		ar << topflags;
//		ar << mrnom;
//		ar << r;
//		ar << r_st;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar.Read(nd,20);
//		ar >> ind;
//		ar >> ibs;
//		ar.Read(algq,20);
//		ar >> ialgq;
//		ar >> q;
//		ar >> topflags;
//		ar >> mrnom;
//		ar >> r;
//		ar >> r_st;
//	}
//}
// class GLOBAL //IMPLEMENTation

//IMPLEMENT_SERIAL(GLOBAL,CObject,0)

CLS_GLOBAL::CLS_GLOBAL()
{
    id = 0;
    nullbs = 0;
    nextbs = 0;
    tptime = time(0);
    timedb = time(0);
    statusdb = 0;
    error = 0;
    dberrnum = 0;
}

const CLS_GLOBAL& CLS_GLOBAL::operator = (const CLS_GLOBAL& m_GLOBAL)
{
	id = m_GLOBAL.id;
	nullbs = m_GLOBAL.nullbs;
	nextbs = m_GLOBAL.nextbs;
	tptime = m_GLOBAL.tptime;
	timedb = m_GLOBAL.timedb;
	statusdb = m_GLOBAL.statusdb;
	error = m_GLOBAL.error;
	dberrnum = m_GLOBAL.dberrnum;
	return* this;
}
CLS_GLOBAL::CLS_GLOBAL(const long m_id)
{
    id = m_id;
    nullbs = 0;
    nextbs = 0;
    tptime = time(0);
    timedb = time(0);
    statusdb = 0;
    error = 0;
    dberrnum = 0;
}

//void GLOBAL::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar << id;
//		ar << nullbs;
//		ar << nextbs;
//		ar << tptime;
//		ar << timedb;
//		ar << statusdb;
//		ar << error;
//		ar << dberrnum;
//	}
//	else
//	{
//		ar >> id;
//		ar >> nullbs;
//		ar >> nextbs;
//		ar >> tptime;
//		ar >> timedb;
//		ar >> statusdb;
//		ar >> error;
//		ar >> dberrnum;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class ISLAND //IMPLEMENTation

//IMPLEMENT_SERIAL(ISLAND,CObject,0)

CLS_ISLAND::CLS_ISLAND()
{
    id = 0;
    refbs = 0;
    refso = 0;
	vref = 0;
	wload = 0;
	rload = 0;
	wso = 0;
	rso = 0;
	valid = 0;
//张瑞雪添加于20120209
	vref_b = 0;
	vref_c = 0;
	wload_b = 0;
	wload_c = 0;
	rload_b = 0;
	rload_c = 0;
	wso_b = 0;
	wso_c = 0;
	rso_b = 0;
	rso_c = 0;
//张瑞雪添加于20120503
	wso_a = 0;
	rso_a = 0;
	wload_a = 0;
	rload_a = 0;
}

const CLS_ISLAND& CLS_ISLAND::operator = (const CLS_ISLAND& m_ISLAND)
{
	id = m_ISLAND.id;
	refbs = m_ISLAND.refbs;
	refso = m_ISLAND.refso;
	vref = m_ISLAND.vref;
	wload = m_ISLAND.wload;
	rload = m_ISLAND.rload;
	wso = m_ISLAND.wso;
	rso = m_ISLAND.rso;
	valid = m_ISLAND.valid;
//张瑞雪添加于20120209
	vref_b = m_ISLAND.vref_b ;
	wload_b  = m_ISLAND.wload_b ;
	rload_b  = m_ISLAND.rload_b ;
	wso_b  = m_ISLAND.wso_b ;
	rso_b  = m_ISLAND.rso_b ;
	vref_c = m_ISLAND.vref_c ;
	wload_c  = m_ISLAND.wload_c ;
	rload_c  = m_ISLAND.rload_c ;
	wso_c  = m_ISLAND.wso_c ;
	rso_c  = m_ISLAND.rso_c ;
//张瑞雪添加于20120503
	wload_a  = m_ISLAND.wload_a ;
	rload_a  = m_ISLAND.rload_a ;
	wso_a  = m_ISLAND.wso_a ;
	rso_a  = m_ISLAND.rso_a ;
	return* this;
}
CLS_ISLAND::CLS_ISLAND(const int m_id)
{
    id = m_id;
    refbs = 0;
    refso = 0;
    valid = 0;
    vref = 0.0;
    wload = 0.0;
    rload = 0.0;
    wso = 0.0;
    rso = 0.0;
//张瑞雪添加于20120209
	vref_b = 0;
	vref_c = 0;
	wload_b = 0;
	wload_c = 0;
	rload_b = 0;
	rload_c = 0;
	wso_b = 0;
	wso_c = 0;
	rso_b = 0;
	rso_c = 0;
//张瑞雪添加于20120503
	wso_a = 0;
	rso_a = 0;
	wload_a = 0;
	rload_a = 0;
}

//void ISLAND::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar << id;
//		ar << refbs;
//		ar << refso;
//		ar << vref;
//		ar << wload;
//		ar << rload;
//		ar << wso;
//		ar << rso;
//	}
//	else
//	{
//		ar >> id;
//		ar >> refbs;
//		ar >> refso;
//		ar >> vref;
//		ar >> wload;
//		ar >> rload;
//		ar >> wso;
//		ar >> rso;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class LD //IMPLEMENTation

//IMPLEMENT_SERIAL(LD,CObject,0)

CLS_LD::CLS_LD()
{
	id = 0;
	pid = 0;
	descr[0] = 0;
	type = 0;
	nd = 0;
	ind = -1;
	ibs = -1;
	algp = 0;
	ialgp = -1;
	algq = 0;
	ialgq = -1;
	algi = 0;
	zn_id = 0;
	ialgi = -1;
	q = 0;
	topflags = 0;
	w = 0;
	r = 0;
	wm = 0;
	rm = 0;
	r_st = -1;
	r_zn = -1;
	stid = 0;
//张瑞雪添加于20120209
	ldtype = 0;
	i = 0;
	i_b = 0;
	i_c = 0;
	a = 0;
	a_b = 0;
	a_c = 0;
	zr = 0;
	zr_b = 0;
	zr_c = 0;
	zx = 0;
	zx_b = 0;
	zx_c = 0;
	algp_b = 0;
	algp_c = 0;
	ialgp_b = -1;
	ialgp_c = -1;
	algq_b = 0;
	algq_c = 0;
	ialgq_b = -1;
	ialgq_c = -1;
	algi_b = 0;
	algi_c = 0;
	ialgi_b = -1;
	ialgi_c = -1;
	algia = 0;
	algia_b = 0;
	algia_c = 0;
	ialgia = -1;
	ialgia_b = -1;
	ialgia_c = -1;
	w_b = 0;
	w_c = 0;
	r_b = 0;
	r_c = 0;
	wm_b = 0;
	wm_c = 0;
	rm_b = 0;
	rm_c = 0;
//张瑞雪添加于20120503
	algp_a = 0;
	ialgp_a = -1;
	algq_a = 0;
	ialgq_a = -1;
	algi_a = 0;
	ialgi_a = -1;
	algia_a = 0;
	ialgia_a = -1;
	w_a = 0;
	r_a = 0;
	i_a = 0;
	a_ = 0;
	zr_a = 0;
	zx_a = 0;
}

const CLS_LD& CLS_LD::operator = (const CLS_LD& m_LD)
{
	id = m_LD.id;
	pid = m_LD.pid;
	strcpy(descr, m_LD.descr);
	type = m_LD.type;
	nd = m_LD.nd;
	ind = m_LD.ind;
	ibs = m_LD.ibs;
	algp = m_LD.algp;
	ialgp = m_LD.ialgp;
	algq = m_LD.algq;
	ialgq = m_LD.ialgq;
	algi = m_LD.algi;
	ialgi = m_LD.ialgi;
	q = m_LD.q;
	topflags = m_LD.topflags;
	w = m_LD.w;
	r = m_LD.r;
	wm = m_LD.wm;
	rm = m_LD.rm;
	r_st = m_LD.r_st;
	r_zn = m_LD.r_zn;
	zn_id = m_LD.zn_id;
//张瑞雪添加于20120209
	ldtype = m_LD.ldtype;
	i = m_LD.i;
	a = m_LD.a;
	zr = m_LD.zr;
	zx = m_LD.zx;
	i_b = m_LD.i_b;
	a_b = m_LD.a_b;
	zr_b = m_LD.zr_b;
	zx_b = m_LD.zx_b;
	i_c = m_LD.i_c;
	a_c = m_LD.a_c;
	zr_c = m_LD.zr_c;
	zx_c = m_LD.zx_c;
	algia = m_LD.algia;
	ialgia = m_LD.ialgia;
	algp_b = m_LD.algp_b;
	ialgp_b = m_LD.ialgp_b;
	algq_b = m_LD.algq_b;
	ialgq_b = m_LD.ialgq_b;
	algi_b = m_LD.algi_b;
	ialgi_b = m_LD.ialgi_b;
	algia_b = m_LD.algia_b;
	ialgia_b = m_LD.ialgia_b;
	w_b = m_LD.w_b;
	r_b = m_LD.r_b;
	wm_b = m_LD.wm_b;
	rm_b = m_LD.rm_b;
	algp_c = m_LD.algp_c;
	ialgp_c = m_LD.ialgp_c;
	algq_c = m_LD.algq_c;
	ialgq_c = m_LD.ialgq_c;
	algi_c = m_LD.algi_c;
	ialgi_c = m_LD.ialgi_c;
	algia_c = m_LD.algia_c;
	ialgia_c = m_LD.ialgia_c;
	w_c = m_LD.w_c;
	r_c = m_LD.r_c;
	wm_c = m_LD.wm_c;
	rm_c = m_LD.rm_c;
	stid = m_LD.stid;
//张瑞雪添加于20120503
	algp_a = m_LD.algp_a;
	ialgp_a = m_LD.ialgp_a;
	algq_a = m_LD.algq_a;
	ialgq_a = m_LD.ialgq_a;
	algi_a = m_LD.algi_a;
	ialgi_a = m_LD.ialgi_a;
	algia_a = m_LD.algia_a;
	ialgia_a = m_LD.ialgia_a;
	w_a = m_LD.w_a;
	r_a = m_LD.r_a;
	i_a = m_LD.i_a;
	a_ = m_LD.a_;
	zr_a = m_LD.zr_a;
	zx_a = m_LD.zx_a;
	return *this;
}
CLS_LD::CLS_LD(const long m_id)
{
	id = m_id;
	pid = 0;
	descr[0] = 0;
	type = 0;
	nd = -1;
	ind = -1;
	ibs = 0;
	algp = 0;
	ialgp = -1;
	algq = 0;
	ialgq = -1;
	algi = 0;
	ialgi = -1;
	q = 0;
	topflags = 0;
	w = 0.0;
	r = 0.0;
	wm = 0.0;
	rm = 0.0;
	r_st = -1;
	r_zn = -1;
	zn_id = 0;
	stid = 0;
	ldtype = -1;
	i = 0;
	i_b = 0;
	i_c = 0;
	a = 0;
	a_b = 0;
	a_c = 0;
	zr = 0;
	zr_b = 0;
	zr_c = 0;
	zx = 0;
	zx_b = 0;
	zx_c = 0;
	algp_b = 0;
	algp_c = 0;
	ialgp_b = -1;
	ialgp_c = -1;
	algq_b = 0;
	algq_c = 0;
	ialgq_b = -1;
	ialgq_c = -1;
	algi_b = 0;
	algi_c = 0;
	ialgi_b = -1;
	ialgi_c = -1;
	algia = 0;
	algia_b = 0;
	algia_c = 0;
	ialgia = -1;
	ialgia_b = -1;
	ialgia_c = -1;
	w_b = 0;
	w_c = 0;
	r_b = 0;
	r_c = 0;
	wm_b = 0;
	wm_c = 0;
	rm_b = 0;
	rm_c = 0;
//张瑞雪添加于20120503
	algp_a = 0;
	ialgp_a = -1;
	algq_a = 0;
	ialgq_a = -1;
	algi_a = 0;
	ialgi_a = -1;
	algia_a = 0;
	ialgia_a = -1;
	w_a = 0;
	r_a = 0;
	i_a = 0;
	a_ = 0;
	zr_a = 0;
	zx_a = 0;
}

//void LD::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar << type;
//		ar.Write(nd,20);
//		ar << ind;
//		ar << ibs;
//		ar.Write(algp,20);
//		ar << ialgp;
//		ar.Write(algq,20);
//		ar << ialgq;
//		ar.Write(algi,20);
//		ar << ialgi;
//		ar << q;
//		ar << topflags;
//		ar << w;
//		ar << r;
//		ar << wm;
//		ar << rm;
//		ar << r_st;
//		ar << r_zn;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar >> type;
//		ar.Read(nd,20);
//		ar >> ind;
//		ar >> ibs;
//		ar.Read(algp,20);
//		ar >> ialgp;
//		ar.Read(algq,20);
//		ar >> ialgq;
//		ar.Read(algi,20);
//		ar >> ialgi;
//		ar >> q;
//		ar >> topflags;
//		ar >> w;
//		ar >> r;
//		ar >> wm;
//		ar >> rm;
//		ar >> r_st;
//		ar >> r_zn;
//	}
//}


/////////////////////////////////////////////////////////////////////////
// class ND //IMPLEMENTation

//IMPLEMENT_SERIAL(ND,CObject,0)

CLS_ND::CLS_ND()
{
    id = 0;
    vl = 0.0;
    q = 0;
    ibs = -1;
    primebs = 0;
    fch = -1;
    bch = -1;
    r_cb = -1;
    r_switch = -1;
    zn_id = -1;
    r_xf = -1;
    r_sec = -1;
    r_cp = -1;
    r_ld = -1;
}

const CLS_ND& CLS_ND::operator = (const CLS_ND& m_ND)
{
	id=m_ND.id;
	vl = m_ND.vl;
	q = m_ND.q;
	ibs = m_ND.ibs;
	primebs = m_ND.primebs;
	fch = m_ND.fch;
	bch = m_ND.bch;
	r_cb = m_ND.r_cb;
	r_switch = m_ND.r_switch;
	zn_id = m_ND.zn_id;
	r_xf = m_ND.r_xf;
	r_sec = m_ND.r_sec;
	r_cp = m_ND.r_cp;
	r_ld = m_ND.r_ld;
	return* this;
}
CLS_ND::CLS_ND(const long m_id)
{
    id= m_id;
    vl = 0.0;
    q = 0;
    ibs = -1;
    primebs = 0;
    fch = -1;
    bch = -1;
    r_cb = -1;
    r_switch = -1;
    zn_id = -1;
    r_xf = -1;
    r_sec = -1;
    r_ld = -1;
    r_cp = -1;
}

//void ND::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar << vl;
//		ar << q;
//		ar << ibs;
//		ar << primebs;
//		ar << fch;
//		ar << bch;
//		ar << r_cb;
//		ar << r_zn;
//		ar << r_xf;
//		ar << r_sec;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar >> vl;
//		ar >> q;
//		ar >> ibs;
//		ar >> primebs;
//		ar >> fch;
//		ar >> bch;
//		ar >> r_cb;
//		ar >> r_zn;
//		ar >> r_xf;
//		ar >> r_sec;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_CO_ST //IMPLEMENTation

//IMPLEMENT_SERIAL(R_CO_ST,CObject,0)

CLS_R_CO_ST::CLS_R_CO_ST()
{
	id=0;
    co_id = 0;
    st_id = 0;
    co_id_ptr = -1;
    st_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_CO_ST& CLS_R_CO_ST::operator = (const CLS_R_CO_ST& m_R_CO_ST)
{
	id=m_R_CO_ST.id;
	co_id=m_R_CO_ST.co_id;
	st_id=m_R_CO_ST.st_id;
	co_id_ptr = m_R_CO_ST.co_id_ptr;
	st_id_ptr = m_R_CO_ST.st_id_ptr;
	fptr = m_R_CO_ST.fptr;
	bptr = m_R_CO_ST.bptr;
	return* this;
}
CLS_R_CO_ST::CLS_R_CO_ST(const long m_co_id,const long m_st_id)
{
	id = 0;
    co_id= m_co_id;
    st_id=m_st_id;
    co_id_ptr = -1;
    st_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_CO_ST::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(co_id,20);
//		ar.Write(st_id,20);
//		ar << co_id_ptr;
//		ar << st_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(co_id,20);
//		ar.Read(st_id,20);
//		ar >> co_id_ptr;
//		ar >> st_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ND_CB //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ND_CB,CObject,0)

CLS_R_ND_CB::CLS_R_ND_CB()
{
	id = 0;
    nd_id = 0;
    cb_id = 0;
    nd_id_ptr = -1;
    cb_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ND_CB& CLS_R_ND_CB::operator = (const CLS_R_ND_CB& m_R_ND_CB)
{
	nd_id=m_R_ND_CB.nd_id;
	cb_id=m_R_ND_CB.cb_id;
	nd_id_ptr = m_R_ND_CB.nd_id_ptr;
	cb_id_ptr = m_R_ND_CB.cb_id_ptr;
	fptr = m_R_ND_CB.fptr;
	bptr = m_R_ND_CB.bptr;
	return* this;
}
CLS_R_ND_CB::CLS_R_ND_CB(const long m_nd_id, const long m_cb_id)
{
	id = 0;
    nd_id=m_nd_id;
    cb_id=m_cb_id;
    nd_id_ptr = -1;
    cb_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

///////////////////////////////////////////////
CLS_R_ND_SWITCH::CLS_R_ND_SWITCH()
{
	id = 0;
    nd_id = 0;
    switch_id = 0;
    nd_id_ptr = -1;
    switch_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ND_SWITCH& CLS_R_ND_SWITCH::operator = (const CLS_R_ND_SWITCH& m_R_ND_CB)
{
	nd_id=m_R_ND_CB.nd_id;
	switch_id=m_R_ND_CB.switch_id;
	nd_id_ptr = m_R_ND_CB.nd_id_ptr;
	switch_id_ptr = m_R_ND_CB.switch_id_ptr;
	fptr = m_R_ND_CB.fptr;
	bptr = m_R_ND_CB.bptr;
	return* this;
}
CLS_R_ND_SWITCH::CLS_R_ND_SWITCH(const long m_nd_id, const long m_cb_id)
{
	id = 0;
    nd_id=m_nd_id;
    switch_id=m_cb_id;
    nd_id_ptr = -1;
    switch_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

CLS_R_ND_CP::CLS_R_ND_CP()
{
	id = 0;
    nd_id = 0;
    cp_id = 0;
    nd_id_ptr = -1;
    cp_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ND_CP& CLS_R_ND_CP::operator = (const CLS_R_ND_CP& m_R_ND_CP)
{
	nd_id=m_R_ND_CP.nd_id;
	cp_id=m_R_ND_CP.cp_id;
	nd_id_ptr = m_R_ND_CP.nd_id_ptr;
	cp_id_ptr = m_R_ND_CP.cp_id_ptr;
	fptr = m_R_ND_CP.fptr;
	bptr = m_R_ND_CP.bptr;
	return* this;
}
CLS_R_ND_CP::CLS_R_ND_CP(const long m_nd_id, const long m_cp_id)
{
	id = 0;
    nd_id=m_nd_id;
    cp_id=m_cp_id;
    nd_id_ptr = -1;
    cp_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

CLS_R_ND_LD::CLS_R_ND_LD()
{
	id = 0;
    nd_id = 0;
    ld_id = 0;
    nd_id_ptr = -1;
    ld_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ND_LD& CLS_R_ND_LD::operator = (const CLS_R_ND_LD& m_R_ND_LD)
{
	nd_id=m_R_ND_LD.nd_id;
	ld_id=m_R_ND_LD.ld_id;
	nd_id_ptr = m_R_ND_LD.nd_id_ptr;
	ld_id_ptr = m_R_ND_LD.ld_id_ptr;
	fptr = m_R_ND_LD.fptr;
	bptr = m_R_ND_LD.bptr;
	return* this;
}
CLS_R_ND_LD::CLS_R_ND_LD(const long m_nd_id, const long m_cp_id)
{
	id = 0;
    nd_id=m_nd_id;
    ld_id=m_cp_id;
    nd_id_ptr = -1;
    ld_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}
//void R_ND_CB::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(nd_id,35);
//		ar.Write(cb_id,30);
//		ar << nd_id_ptr;
//		ar << cb_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(nd_id,35);
//		ar.Read(cb_id,30);
//		ar >> nd_id_ptr;
//		ar >> cb_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ND_SEC //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ND_SEC,CObject,0)

CLS_R_ND_SEC::CLS_R_ND_SEC()
{
	id = 0;
    nd_id = 0;
    sec_id = 0;
    nd_id_ptr = -1;
    sec_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ND_SEC& CLS_R_ND_SEC::operator = (const CLS_R_ND_SEC& m_R_ND_SEC)
{
	nd_id=m_R_ND_SEC.nd_id;
	sec_id=m_R_ND_SEC.sec_id;
	nd_id_ptr = m_R_ND_SEC.nd_id_ptr;
	sec_id_ptr = m_R_ND_SEC.sec_id_ptr;
	fptr = m_R_ND_SEC.fptr;
	bptr = m_R_ND_SEC.bptr;
	return* this;
}
CLS_R_ND_SEC::CLS_R_ND_SEC(const long m_nd_id,const long m_sec_id)
{
	id = 0;
    nd_id=m_nd_id;
    sec_id=m_sec_id;
    nd_id_ptr = -1;
    sec_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ND_SEC::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(nd_id,20);
//		ar.Write(sec_id,20);
//		ar << nd_id_ptr;
//		ar << sec_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(nd_id,20);
//		ar.Read(sec_id,20);
//		ar >> nd_id_ptr;
//		ar >> sec_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ND_XF //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ND_XF,CObject,0)

CLS_R_ND_XF::CLS_R_ND_XF()
{
	id = 0;
    nd_id = 0;
    xf_id = 0;
    nd_id_ptr = -1;
    xf_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ND_XF& CLS_R_ND_XF::operator = (const CLS_R_ND_XF& m_R_ND_XF)
{
	nd_id=m_R_ND_XF.nd_id;
	xf_id=m_R_ND_XF.xf_id;
	nd_id_ptr = m_R_ND_XF.nd_id_ptr;
	xf_id_ptr = m_R_ND_XF.xf_id_ptr;
	fptr = m_R_ND_XF.fptr;
	bptr = m_R_ND_XF.bptr;
	return* this;
}
CLS_R_ND_XF::CLS_R_ND_XF(const long m_nd_id,const long m_xf_id)
{
	id = 0;
    nd_id=m_nd_id;
    xf_id=m_xf_id;
    nd_id_ptr = -1;
    xf_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ND_XF::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(nd_id,20);
//		ar.Write(xf_id,20);
//		ar << nd_id_ptr;
//		ar << xf_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(nd_id,20);
//		ar.Read(xf_id,20);
//		ar >> nd_id_ptr;
//		ar >> xf_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ND_ZN //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ND_ZN,CObject,0)

CLS_R_ZN_ND::CLS_R_ZN_ND()
{
	id = 0;
    nd_id = 0;
    zn_id = 0;
    nd_id_ptr = -1;
    zn_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ZN_ND& CLS_R_ZN_ND::operator = (const CLS_R_ZN_ND& m_R_ND_ZN)
{
	nd_id=m_R_ND_ZN.nd_id;
	zn_id=m_R_ND_ZN.zn_id;
	nd_id_ptr = m_R_ND_ZN.nd_id_ptr;
	zn_id_ptr = m_R_ND_ZN.zn_id_ptr;
	fptr = m_R_ND_ZN.fptr;
	bptr = m_R_ND_ZN.bptr;
	return* this;
}
CLS_R_ZN_ND::CLS_R_ZN_ND(const long m_zn_id, const long m_nd_id)
{
	id = 0;
    nd_id=m_nd_id;
    zn_id=m_zn_id;
    nd_id_ptr = -1;
    zn_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ND_ZN::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(nd_id,20);
//		ar.Write(zn_id,20);
//		ar << nd_id_ptr;
//		ar << zn_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(nd_id,20);
//		ar.Read(zn_id,20);
//		ar >> nd_id_ptr;
//		ar >> zn_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ST_BUS //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ST_BUS,CObject,0)

CLS_R_ST_BUS::CLS_R_ST_BUS()
{
	id = 0;
    st_id = 0;
    bus_id = 0;
    st_id_ptr = -1;
    bus_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_BUS& CLS_R_ST_BUS::operator = (const CLS_R_ST_BUS& m_R_ST_BUS)
{
	st_id=m_R_ST_BUS.st_id;
	bus_id=m_R_ST_BUS.bus_id;
	st_id_ptr = m_R_ST_BUS.st_id_ptr;
	bus_id_ptr = m_R_ST_BUS.bus_id_ptr;
	fptr = m_R_ST_BUS.fptr;
	bptr = m_R_ST_BUS.bptr;
	return* this;
}
CLS_R_ST_BUS::CLS_R_ST_BUS(const long m_st_id,const long m_bus_id)
{
	id = 0;
    st_id=m_st_id;
    bus_id=m_bus_id;
    st_id_ptr = -1;
    bus_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ST_BUS::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(st_id,20);
//		ar.Write(bus_id,20);
//		ar << st_id_ptr;
//		ar << bus_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(st_id,20);
//		ar.Read(bus_id,20);
//		ar >> st_id_ptr;
//		ar >> bus_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ST_CB //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ST_CB,CObject,0)

CLS_R_ST_CB::CLS_R_ST_CB()
{
	id = 0;
    st_id = 0;
    cb_id = 0;
    st_id_ptr = -1;
    cb_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_CB& CLS_R_ST_CB::operator = (const CLS_R_ST_CB& m_R_ST_CB)
{
	st_id = m_R_ST_CB.st_id;
	cb_id = m_R_ST_CB.cb_id;
	st_id_ptr = m_R_ST_CB.st_id_ptr;
	cb_id_ptr = m_R_ST_CB.cb_id_ptr;
	fptr = m_R_ST_CB.fptr;
	bptr = m_R_ST_CB.bptr;
	return *this;
}
CLS_R_ST_CB::CLS_R_ST_CB(const long m_st_id,const long m_cb_id)
{
	id = 0;
	st_id = m_st_id;
	cb_id = m_cb_id;
	st_id_ptr = -1;
	cb_id_ptr = -1;
	fptr = -1;
	bptr = -1;
}

////////////////////////////////////////////
CLS_R_ST_SWITCH::CLS_R_ST_SWITCH()
{
	id = 0;
    st_id = 0;
    switch_id = 0;
    st_id_ptr = -1;
    switch_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_SWITCH& CLS_R_ST_SWITCH::operator = (const CLS_R_ST_SWITCH& m_R_ST_CB)
{
	st_id = m_R_ST_CB.st_id;
	switch_id = m_R_ST_CB.switch_id;
	st_id_ptr = m_R_ST_CB.st_id_ptr;
	switch_id_ptr = m_R_ST_CB.switch_id_ptr;
	fptr = m_R_ST_CB.fptr;
	bptr = m_R_ST_CB.bptr;
	return *this;
}
CLS_R_ST_SWITCH::CLS_R_ST_SWITCH(const long m_st_id,const long m_cb_id)
{
	id = 0;
	st_id = m_st_id;
	switch_id = m_cb_id;
	st_id_ptr = -1;
	switch_id_ptr = -1;
	fptr = -1;
	bptr = -1;
}

//void R_ST_CB::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(st_id,20);
//		ar.Write(cb_id,20);
//		ar << st_id_ptr;
//		ar << cb_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(st_id,20);
//		ar.Read(cb_id,20);
//		ar >> st_id_ptr;
//		ar >> cb_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ST_CP //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ST_CP,CObject,0)

CLS_R_ST_CP::CLS_R_ST_CP()
{
	id = 0;
    st_id = 0;
    cp_id = 0;
    st_id_ptr = -1;
    cp_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_CP& CLS_R_ST_CP::operator = (const CLS_R_ST_CP& m_R_ST_CP)
{
	st_id=m_R_ST_CP.st_id;
	cp_id=m_R_ST_CP.cp_id;
	st_id_ptr = m_R_ST_CP.st_id_ptr;
	cp_id_ptr = m_R_ST_CP.cp_id_ptr;
	fptr = m_R_ST_CP.fptr;
	bptr = m_R_ST_CP.bptr;
	return* this;
}
CLS_R_ST_CP::CLS_R_ST_CP(const long m_st_id,const long m_cp_id)
{
	id = 0;
    st_id=m_st_id;
    cp_id=m_cp_id;
    st_id_ptr = -1;
    cp_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ST_CP::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(st_id,20);
//		ar.Write(cp_id,20);
//		ar << st_id_ptr;
//		ar << cp_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(st_id,20);
//		ar.Read(cp_id,20);
//		ar >> st_id_ptr;
//		ar >> cp_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ST_LD //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ST_LD,CObject,0)

CLS_R_ST_LD::CLS_R_ST_LD()
{
	id = 0;
    st_id = 0;
    ld_id = 0;
    st_id_ptr = -1;
    ld_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_LD& CLS_R_ST_LD::operator = (const CLS_R_ST_LD& m_R_ST_LD)
{
	st_id=m_R_ST_LD.st_id;
	ld_id=m_R_ST_LD.ld_id;
	st_id_ptr = m_R_ST_LD.st_id_ptr;
	ld_id_ptr = m_R_ST_LD.ld_id_ptr;
	fptr = m_R_ST_LD.fptr;
	bptr = m_R_ST_LD.bptr;
	return* this;
}
CLS_R_ST_LD::CLS_R_ST_LD(const long m_st_id,const long m_ld_id)
{
	id = 0;
    st_id=m_st_id;
    ld_id=m_ld_id;
    st_id_ptr = -1;
    ld_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ST_LD::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(st_id,20);
//		ar.Write(ld_id,20);
//		ar << st_id_ptr;
//		ar << ld_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(st_id,20);
//		ar.Read(ld_id,20);
//		ar >> st_id_ptr;
//		ar >> ld_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ST_SO //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ST_SO,CObject,0)

CLS_R_ST_SO::CLS_R_ST_SO()
{
	id = 0;
    st_id = 0;
    so_id = 0;
    st_id_ptr = -1;
    so_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_SO& CLS_R_ST_SO::operator = (const CLS_R_ST_SO& m_R_ST_SO)
{
	st_id=m_R_ST_SO.st_id;
	so_id=m_R_ST_SO.so_id;
	st_id_ptr = m_R_ST_SO.st_id_ptr;
	so_id_ptr = m_R_ST_SO.so_id_ptr;
	fptr = m_R_ST_SO.fptr;
	bptr = m_R_ST_SO.bptr;
	return* this;
}
CLS_R_ST_SO::CLS_R_ST_SO(const long m_st_id,const long m_so_id)
{
	id = 0;
    st_id=m_st_id;
    so_id=m_so_id;
    st_id_ptr = -1;
    so_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ST_SO::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(st_id,20);
//		ar.Write(so_id,20);
//		ar << st_id_ptr;
//		ar << so_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(st_id,20);
//		ar.Read(so_id,20);
//		ar >> st_id_ptr;
//		ar >> so_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ST_XFMR //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ST_XFMR,CObject,0)

CLS_R_ST_XFMR::CLS_R_ST_XFMR()
{
	id = 0;
    st_id = 0;
    xfmr_id = 0;
    st_id_ptr = -1;
    xfmr_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_XFMR& CLS_R_ST_XFMR::operator = (const CLS_R_ST_XFMR& m_R_ST_XFMR)
{
	id=m_R_ST_XFMR.id;
	st_id=m_R_ST_XFMR.st_id;
	xfmr_id=m_R_ST_XFMR.xfmr_id;
	st_id_ptr = m_R_ST_XFMR.st_id_ptr;
	xfmr_id_ptr = m_R_ST_XFMR.xfmr_id_ptr;
	fptr = m_R_ST_XFMR.fptr;
	bptr = m_R_ST_XFMR.bptr;
	return* this;
}
CLS_R_ST_XFMR::CLS_R_ST_XFMR(const long m_st_id,const long m_xfmr_id)
{
	id = 0;
    st_id=m_st_id;
    xfmr_id=m_xfmr_id;
    st_id_ptr = -1;
    xfmr_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ST_XFMR::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(st_id,20);
//		ar.Write(xfmr_id,20);
//		ar << st_id_ptr;
//		ar << xfmr_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(st_id,20);
//		ar.Read(xfmr_id,20);
//		ar >> st_id_ptr;
//		ar >> xfmr_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ST_ZN //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ST_ZN,CObject,0)

CLS_R_ST_ZN::CLS_R_ST_ZN()
{
	id = 0;
    st_id = 0;
    zn_id = 0;
    st_id_ptr = -1;
    zn_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ST_ZN& CLS_R_ST_ZN::operator = (const CLS_R_ST_ZN& m_R_ST_ZN)
{
	st_id=m_R_ST_ZN.st_id;
	zn_id=m_R_ST_ZN.zn_id;
	st_id_ptr = m_R_ST_ZN.st_id_ptr;
	zn_id_ptr = m_R_ST_ZN.zn_id_ptr;
	fptr = m_R_ST_ZN.fptr;
	bptr = m_R_ST_ZN.bptr;
	return* this;
}
CLS_R_ST_ZN::CLS_R_ST_ZN(const long m_st_id,const long m_zn_id)
{
	id = 0;
    st_id=m_st_id;
    zn_id=m_zn_id;
    st_id_ptr = -1;
    zn_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ST_ZN::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(st_id,20);
//		ar.Write(zn_id,20);
//		ar << st_id_ptr;
//		ar << zn_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(st_id,20);
//		ar.Read(zn_id,20);
//		ar >> st_id_ptr;
//		ar >> zn_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_XFMR_XF //IMPLEMENTation

//IMPLEMENT_SERIAL(R_XFMR_XF,CObject,0)

CLS_R_XFMR_XF::CLS_R_XFMR_XF()
{
	id=0;
    xfmr_id = 0;
    xf_id = 0;
    xfmr_id_ptr = -1;
    xf_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_XFMR_XF& CLS_R_XFMR_XF::operator = (const CLS_R_XFMR_XF& m_R_XFMR_XF)
{
	id=m_R_XFMR_XF.id;
	xfmr_id=m_R_XFMR_XF.xfmr_id;
	xf_id=m_R_XFMR_XF.xf_id;
	xfmr_id_ptr = m_R_XFMR_XF.xfmr_id_ptr;
	xf_id_ptr = m_R_XFMR_XF.xf_id_ptr;
	fptr = m_R_XFMR_XF.fptr;
	bptr = m_R_XFMR_XF.bptr;
	return* this;
}
CLS_R_XFMR_XF::CLS_R_XFMR_XF(const long m_xfmr_id,const long m_xf_id)
{
	id = 0;
    xfmr_id=m_xfmr_id;
    xf_id=m_xf_id;
    xfmr_id_ptr = -1;
    xf_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_XFMR_XF::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(xfmr_id,20);
//		ar.Write(xf_id,20);
//		ar << xfmr_id_ptr;
//		ar << xf_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(xfmr_id,20);
//		ar.Read(xf_id,20);
//		ar >> xfmr_id_ptr;
//		ar >> xf_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ZN_LD //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ZN_LD,CObject,0)

CLS_R_ZN_LD::CLS_R_ZN_LD()
{
	id = 0;
    zn_id = 0;
    ld_id = 0;
    zn_id_ptr = -1;
    ld_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ZN_LD& CLS_R_ZN_LD::operator = (const CLS_R_ZN_LD& m_R_ZN_LD)
{
	zn_id=m_R_ZN_LD.zn_id;
	ld_id=m_R_ZN_LD.ld_id;
	zn_id_ptr = m_R_ZN_LD.zn_id_ptr;
	ld_id_ptr = m_R_ZN_LD.ld_id_ptr;
	fptr = m_R_ZN_LD.fptr;
	bptr = m_R_ZN_LD.bptr;
	return* this;
}
CLS_R_ZN_LD::CLS_R_ZN_LD(const long m_zn_id,const long m_ld_id)
{
	id = 0;
    zn_id = m_zn_id;
    ld_id = m_ld_id;
    zn_id_ptr = -1;
    ld_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ZN_LD::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(zn_id,20);
//		ar.Write(ld_id,20);
//		ar << zn_id_ptr;
//		ar << ld_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(zn_id,20);
//		ar.Read(ld_id,20);
//		ar >> zn_id_ptr;
//		ar >> ld_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class R_ZN_SEC //IMPLEMENTation

//IMPLEMENT_SERIAL(R_ZN_SEC,CObject,0)

CLS_R_ZN_SEC::CLS_R_ZN_SEC()
{
	id = 0;
    zn_id = 0;
    sec_id = 0;
    zn_id_ptr = -1;
    sec_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

const CLS_R_ZN_SEC& CLS_R_ZN_SEC::operator = (const CLS_R_ZN_SEC& m_R_ZN_SEC)
{
	zn_id=m_R_ZN_SEC.zn_id;
	sec_id=m_R_ZN_SEC.sec_id;
	zn_id_ptr = m_R_ZN_SEC.zn_id_ptr;
	sec_id_ptr = m_R_ZN_SEC.sec_id_ptr;
	fptr = m_R_ZN_SEC.fptr;
	bptr = m_R_ZN_SEC.bptr;
	return* this;
}
CLS_R_ZN_SEC::CLS_R_ZN_SEC(const long m_zn_id,const long m_sec_id)
{
	id = 0;
    zn_id=m_zn_id;
    sec_id=m_sec_id;
    zn_id_ptr = -1;
    sec_id_ptr = -1;
    fptr = -1;
    bptr = -1;
}

//void R_ZN_SEC::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(zn_id,20);
//		ar.Write(sec_id,20);
//		ar << zn_id_ptr;
//		ar << sec_id_ptr;
//		ar << fptr;
//		ar << bptr;
//	}
//	else
//	{
//		ar.Read(zn_id,20);
//		ar.Read(sec_id,20);
//		ar >> zn_id_ptr;
//		ar >> sec_id_ptr;
//		ar >> fptr;
//		ar >> bptr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class SEC //IMPLEMENTation

//IMPLEMENT_SERIAL(SEC,CObject,0)

CLS_SEC::CLS_SEC()
{
	id = 0;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	znd = 0;
	ind = -1;
	iznd = -1;
	ibs = -1;
	zbs = -1;
	r0 = 0.0;
	x0 = 0.0;
	b0 = 0.0;
	q = 0;
	topflags = 0;
	algip = 0;
	ialgip = -1;
	algiq = 0;
	ialgiq = -1;
	algii = 0;
	ialgii = -1;
	algzp = 0;
	ialgzp = -1;
	algzq = 0;
	ialgzq = -1;
	algzi = 0;
	ialgzi = -1;
	w = 0;
	r = 0;
	i = 0;
	wz = 0;
	rz = 0;
	iz = 0;
	r_zn = -1;
//张瑞雪添加于20120209
	r0_ab = 0.0;
	r0_ac = 0.0;
	r0_ba = 0.0;
	r0_bb = 0.0;
	r0_bc = 0.0;
	r0_ca = 0.0;
	r0_cb = 0.0;
	r0_cc = 0.0;
	x0_ab = 0.0;
	x0_ac = 0.0;
	x0_ba = 0.0;
	x0_bb = 0.0;
	x0_bc = 0.0;
	x0_ca = 0.0;
	x0_cb = 0.0;
	x0_cc = 0.0;
	b0_ab = 0.0;
	b0_ac = 0.0;
	b0_ba = 0.0;
	b0_bb = 0.0;
	b0_bc = 0.0;
	b0_ca = 0.0;
	b0_cb = 0.0;
	b0_cc = 0.0;
	algip_b = 0;
	ialgip_b = -1;
	algiq_b = 0;
	ialgiq_b = -1;
	algii_b = 0;
	ialgii_b = -1;
	algzp_b = 0;
	ialgzp_b = -1;
	algzq_b = 0;
	ialgzq_b = -1;
	algzi_b = 0;
	ialgzi_b = -1;
	w_b = 0;
	r_b = 0;
	i_b = 0;
	wz_b = 0;
	rz_b = 0;
	iz_b = 0;
	algip_c = 0;
	ialgip_c = -1;
	algiq_c = 0;
	ialgiq_c = -1;
	algii_c = 0;
	ialgii_c = -1;
	algzp_c = 0;
	ialgzp_c = -1;
	algzq_c = 0;
	ialgzq_c = -1;
	algzi_c = 0;
	ialgzi_c = -1;
	w_c = 0;
	r_c = 0;
	i_c = 0;
	wz_c = 0;
	rz_c = 0;
	iz_c = 0;
	zn_id = 0;

	//张瑞雪添加于2012.4.12
	phases = -1;
	l = 0;
	mode = -1;
	//张瑞雪添加于20120503
	r0_aa = 0;
	x0_aa = 0;
	b0_aa = 0;
	w_a = 0;
	r_a = 0;
	i_a = 0;
	wz_a = 0;
	rz_a = 0;
	iz_a = 0;
	algip_a = 0;
	ialgip_a = -1;
	algiq_a = 0;
	ialgiq_a = -1;
	algii_a = 0;
	ialgii_a = -1;
	algzp_a = 0;
	ialgzp_a = -1;
	algzq_a = 0;
	ialgzq_a = -1;
	algzi_a = 0;
	ialgzi_a = -1;
	r_fi = -1;
}

const CLS_SEC& CLS_SEC::operator = (const CLS_SEC& m_SEC)
{
	id = m_SEC.id;
	pid = m_SEC.pid;
	strcpy(descr, m_SEC.descr);
	nd = m_SEC.nd;
	znd = m_SEC.znd;
	zn_id = m_SEC.zn_id;
	ind = m_SEC.ind;
	iznd = m_SEC.iznd;
	ibs = m_SEC.ibs;
	zbs = m_SEC.zbs;
	r0 = m_SEC.r0;
	x0 = m_SEC.x0;
	b0 = m_SEC.b0;
	q = m_SEC.q;
	topflags = m_SEC.topflags;
	algip = m_SEC.algip;
	ialgip = m_SEC.ialgip;
	algiq = m_SEC.algiq;
	ialgiq = m_SEC.ialgiq;
	algii = m_SEC.algii;
	ialgii = m_SEC.ialgii;
	algzp = m_SEC.algzp;
	ialgzp = m_SEC.ialgzp;
	algzq = m_SEC.algzq;
	ialgzq = m_SEC.ialgzq;
	algzi = m_SEC.algzi;
	ialgzi = m_SEC.ialgzi;
	w = m_SEC.w;
	r = m_SEC.r;
	i = m_SEC.i;
	wz = m_SEC.wz;
	rz = m_SEC.rz;
	iz = m_SEC.iz;
//张瑞雪添加于20120209
	r0_ab = m_SEC.r0_ab;
	r0_ac = m_SEC.r0_ac;
	r0_ba = m_SEC.r0_ba;
	r0_bb = m_SEC.r0_bb;
	r0_bc = m_SEC.r0_bc;
	r0_ca = m_SEC.r0_ca;
	r0_cb = m_SEC.r0_cb;
	r0_cc = m_SEC.r0_cc;
	x0_ab = m_SEC.x0_ab;
	x0_ac = m_SEC.x0_ac;
	x0_ba = m_SEC.x0_ba;
	x0_bb = m_SEC.x0_bb;
	x0_bc = m_SEC.x0_bc;
	x0_ca = m_SEC.x0_ca;
	x0_cb = m_SEC.x0_cb;
	x0_cc = m_SEC.x0_cc;
	b0_ab = m_SEC.b0_ab;
	b0_ac = m_SEC.b0_ac;
	b0_ba = m_SEC.b0_ba;
	b0_bb = m_SEC.b0_bb;
	b0_bc = m_SEC.b0_bc;
	b0_ca = m_SEC.b0_ca;
	b0_cb = m_SEC.b0_cb;
	b0_cc = m_SEC.b0_cc;
	algip_b = m_SEC.algip_b;
	ialgip_b = m_SEC.ialgip_b;
	algiq_b = m_SEC.algiq_b;
	ialgiq_b = m_SEC.ialgiq_b;
	algii_b = m_SEC.algii_b;
	ialgii_b = m_SEC.ialgii_b;
	algzp_b = m_SEC.algzp_b;
	ialgzp_b = m_SEC.ialgzp_b;
	algzq_b = m_SEC.algzq_b;
	ialgzq_b = m_SEC.ialgzq_b;
	algzi_b = m_SEC.algzi_b;
	ialgzi_b = m_SEC.ialgzi_b;
	w_b = m_SEC.w_b;
	r_b = m_SEC.r_b;
	i_b = m_SEC.i_b;
	wz_b = m_SEC.wz_b;
	rz_b = m_SEC.rz_b;
	iz_b = m_SEC.iz_b;
	algip_c = m_SEC.algip_c;
	ialgip_c = m_SEC.ialgip_c;
	algiq_c = m_SEC.algiq_c;
	ialgiq_c = m_SEC.ialgiq_c;
	algii_c = m_SEC.algii_c;
	ialgii_c = m_SEC.ialgii_c;
	algzp_c = m_SEC.algzp_c;
	ialgzp_c = m_SEC.ialgzp_c;
	algzq_c = m_SEC.algzq_c;
	ialgzq_c = m_SEC.ialgzq_c;
	algzi_c = m_SEC.algzi_c;
	ialgzi_c = m_SEC.ialgzi_c;
	w_c = m_SEC.w_c;
	r_c = m_SEC.r_c;
	i_c = m_SEC.i_c;
	wz_c = m_SEC.wz_c;
	rz_c = m_SEC.rz_c;
	iz_c = m_SEC.iz_c;
	r_zn = m_SEC.r_zn;
	phases = m_SEC.phases;
	l = m_SEC.l;
	mode = m_SEC.mode;
	//张瑞雪添加于20120503
	w_a = m_SEC.w_a;
	r_a = m_SEC.r_a;
	i_a = m_SEC.i_a;
	wz_a = m_SEC.wz_a;
	rz_a = m_SEC.rz_a;
	iz_a = m_SEC.iz_a;
	r0_aa = m_SEC.r0_aa;
	x0_aa = m_SEC.x0_aa;
	b0_aa = m_SEC.b0_aa;
	algip_a = m_SEC.algip_a;
	ialgip_a = m_SEC.ialgip_a;
	algiq_a = m_SEC.algiq_a;
	ialgiq_a = m_SEC.ialgiq_a;
	algii_a = m_SEC.algii_a;
	ialgii_a = m_SEC.ialgii_a;
	algzp_a = m_SEC.algzp_a;
	ialgzp_a = m_SEC.ialgzp_a;
	algzq_a = m_SEC.algzq_a;
	ialgzq_a = m_SEC.ialgzq_a;
	algzi_a = m_SEC.algzi_a;
	ialgzi_a = m_SEC.ialgzi_a;
	r_fi = m_SEC.r_fi;
	return *this;
}
CLS_SEC::CLS_SEC(const long m_id)
{
	id = m_id;
	pid = 0;
	descr[0] = 0;
	nd = 0;
	znd = 0;
	ind = -1;
	iznd = -1;
	ibs = -1;
	zbs = -1;
	r0 = 0.0;
	x0 = 0.0;
	b0 = 0.0;
	q = 0;
	topflags = 0;
	algip = 0;
	ialgip = -1;
	algiq = 0;
	ialgiq = -1;
	algii = 0;
	ialgii = -1;
	algzp = 0;
	ialgzp = -1;
	algzq = 0;
	ialgzq = -1;
	algzi = 0;
	ialgzi = -1;
	w = 0.0;
	r = 0.0;
	i = 0.0;
	wz = 0.0;
	rz = 0.0;
	iz = 0.0;
//张瑞雪添加于20120209
	r0_ab = 0.0;
	r0_ac = 0.0;
	r0_ba = 0.0;
	r0_bb = 0.0;
	r0_bc = 0.0;
	r0_ca = 0.0;
	r0_cb = 0.0;
	r0_cc = 0.0;
	x0_ab = 0.0;
	x0_ac = 0.0;
	x0_ba = 0.0;
	x0_bb = 0.0;
	x0_bc = 0.0;
	x0_ca = 0.0;
	x0_cb = 0.0;
	x0_cc = 0.0;
	b0_ab = 0.0;
	b0_ac = 0.0;
	b0_ba = 0.0;
	b0_bb = 0.0;
	b0_bc = 0.0;
	b0_ca = 0.0;
	b0_cb = 0.0;
	b0_cc = 0.0;
	algip_b = 0;
	ialgip_b = -1;
	algiq_b = 0;
	ialgiq_b = -1;
	algii_b = 0;
	ialgii_b = -1;
	algzp_b = 0;
	ialgzp_b = -1;
	algzq_b = 0;
	ialgzq_b = -1;
	algzi_b = 0;
	ialgzi_b = -1;
	w_b = 0;
	r_b = 0;
	i_b = 0;
	wz_b = 0;
	rz_b = 0;
	iz_b = 0;
	algip_c = 0;
	ialgip_c = -1;
	algiq_c = 0;
	ialgiq_c = -1;
	algii_c = 0;
	ialgii_c = -1;
	algzp_c = 0;
	ialgzp_c = -1;
	algzq_c = 0;
	ialgzq_c = -1;
	algzi_c = 0;
	ialgzi_c = -1;
	w_b = 0;
	r_b = 0;
	i_b = 0;
	wz_b = 0;
	rz_b = 0;
	iz_b = 0;
	r_zn = -1;
	zn_id = 0;
	phases = -1;
	l = 0;
	mode = -1;
	//张瑞雪添加于20120503
	r0_aa = 0;
	x0_aa = 0;
	b0_aa = 0;
	w_a = 0;
	r_a = 0;
	i_a = 0;
	wz_a = 0;
	rz_a = 0;
	iz_a = 0;
	algip_a = 0;
	ialgip_a = -1;
	algiq_a = 0;
	ialgiq_a = -1;
	algii_a = 0;
	ialgii_a = -1;
	algzp_a = 0;
	ialgzp_a = -1;
	algzq_a = 0;
	ialgzq_a = -1;
	algzi_a = 0;
	ialgzi_a = -1;
	r_fi = -1;
}

//void SEC::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar.Write(nd,20);
//		ar.Write(znd,20);
//		ar << ind;
//		ar << iznd;
//		ar << ibs;
//		ar << zbs;
//		ar << r0;
//		ar << x0;
//		ar << b0;
//		ar << q;
//		ar << topflags;
//		ar.Write(algip,20);
//		ar << ialgip;
//		ar.Write(algiq,20);
//		ar << ialgiq;
//		ar.Write(algii,20);
//		ar << ialgii;
//		ar.Write(algzp,20);
//		ar << ialgzp;
//		ar.Write(algzq,20);
//		ar << ialgzq;
//		ar.Write(algzi,20);
//		ar << ialgzi;
//		ar << w;
//		ar << r;
//		ar << i;
//		ar << wz;
//		ar << rz;
//		ar << iz;
//		ar << r_zn;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar.Read(nd,20);
//		ar.Read(znd,20);
//		ar >> ind;
//		ar >> iznd;
//		ar >> ibs;
//		ar >> zbs;
//		ar >> r0;
//		ar >> x0;
//		ar >> b0;
//		ar >> q;
//		ar >> topflags;
//		ar.Read(algip,20);
//		ar >> ialgip;
//		ar.Read(algiq,20);
//		ar >> ialgiq;
//		ar.Read(algii,20);
//		ar >> ialgii;
//		ar.Read(algzp,20);
//		ar >> ialgzp;
//		ar.Read(algzq,20);
//		ar >> ialgzq;
//		ar.Read(algzi,20);
//		ar >> ialgzi;
//		ar >> w;
//		ar >> r;
//		ar >> i;
//		ar >> wz;
//		ar >> rz;
//		ar >> iz;
//		ar >> r_zn;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class SO //IMPLEMENTation

//IMPLEMENT_SERIAL(SO,CObject,0)

CLS_SO::CLS_SO()
{
    id = 0;
    pid = 0;
    descr[0] = 0;
    nd = 0;
    ind = -1;
    ibs = -1;
    iisland = -1;
    algp = 0;
    ialgp = -1;
    algq = 0;
    ialgq = -1;
    algi = 0;
    ialgi = -1;
    q = 0;
    topflags = 0;
    w = 0;
    r = 0;
    i = 0;
    r_st = -1;
	ss = 0;
	sw = 0;
	sr = 0;
//张瑞雪添加于20120209
	slackavr = -2;
	w_b = 0;
	w_c = 0;
	r_b = 0;
	r_c = 0;
	i_b = 0;
	i_c = 0;
	v = 1;
	v_b = 0;
	v_c = 0;
	wmx = 1.0e6;
	rmx = 1.0e6;
	wmn = -1.0e6;
	rmn = -1.0e6;
	algp_b = 0;
	algp_c = 0;
	ialgp_b = 0;
	ialgp_c = 0;
	algq_b = 0;
	algq_c = 0;
	ialgq_b = 0;
	algi_b = 0;
	ialgq_c = 0;
	algi_c = 0;
	ialgi_b = 0;
	ialgi_c = 0;
	adeg = 0;
	adeg_b = 0;
	adeg_c = 0;
//张瑞雪添加于20120503
	algp_a = 0;
	ialgp_a = -1;
	algq_a = 0;
	ialgq_a = -1;
	algi_a = 0;
	ialgi_a = -1;
	w_a = 0;
	r_a = 0;
	i_a = 0;
	v_a = 0;
	adeg_a = 0;
	stid=0;
}

const CLS_SO& CLS_SO::operator = (const CLS_SO& m_SO)
{
	id = m_SO.id;
	pid = m_SO.pid;
	strcpy(descr, m_SO.descr);
	nd = m_SO.nd;
	ind = m_SO.ind;
	ibs = m_SO.ibs;
	iisland = m_SO.iisland;
	algp = m_SO.algp;
	ialgp = m_SO.ialgp;
	algq = m_SO.algq;
	ialgq = m_SO.ialgq;
	algi = m_SO.algi;
	ialgi = m_SO.ialgi;
	q = m_SO.q;
	topflags = m_SO.topflags;
	w = m_SO.w;
	r = m_SO.r;
	i = m_SO.i;
	r_st = m_SO.r_st;
	ss = m_SO.ss;
	sw = m_SO.sw;
	sr = m_SO.sr;
//张瑞雪添加于20120209
	slackavr = m_SO.slackavr;
	v = m_SO.v;
	wmx = m_SO.wmx;
	rmx = m_SO.rmx;
	wmn = m_SO.wmn;
	rmn = m_SO.rmn;
	algp_b = m_SO.algp_b;
	ialgp_b = m_SO.ialgp_b;
	algq_b = m_SO.algq_b;
	ialgq_b = m_SO.ialgq_b;
	algi_b = m_SO.algi_b;
	ialgi_b = m_SO.ialgi_b;
	w_b = m_SO.w_b;
	r_b = m_SO.r_b;
	i_b = m_SO.i_b;
	v_b = m_SO.v_b;
	algp_c = m_SO.algp_c;
	ialgp_c = m_SO.ialgp_c;
	algq_c = m_SO.algq_c;
	ialgq_c = m_SO.ialgq_c;
	algi_c = m_SO.algi_c;
	ialgi_c = m_SO.ialgi_c;
	w_c = m_SO.w_c;
	r_c = m_SO.r_c;
	i_c = m_SO.i_c;
	v_c = m_SO.v_c;
	adeg = m_SO.adeg;
	adeg_b = m_SO.adeg_b;
	adeg_c = m_SO.adeg_c;
//张瑞雪添加于20120503
	algp_a = m_SO.algp_a;
	ialgp_a = m_SO.ialgp_a;
	algq_a = m_SO.algq_a;
	ialgq_a = m_SO.ialgq_a;
	algi_a = m_SO.algi_a;
	ialgi_a = m_SO.ialgi_a;
	w_a = m_SO.w_a;
	r_a = m_SO.r_a;
	i_a = m_SO.i_a;
	v_a = m_SO.v_a;
	adeg_a = m_SO.adeg_a;
	stid = m_SO.stid;
	return *this;
}
CLS_SO::CLS_SO(const long m_id)
{
	pid = 0;
	id = m_id;
	descr[0] = 0;
	nd = 0;
	ind = -1;
	ibs = -1;
	iisland = -1;
	algp = 0;
	ialgp = -1;
	algq = 0;
	ialgq = -1;
	algi = 0;
	ialgi = -1;
	q = 0;
	topflags = 0;
	w = 0.0;
	r = 0.0;
	i = 0.0;
	r_st = -1;
	ss = 0.0;
	sw = 0.0;
	sr = 0.0;
//张瑞雪添加于20120209
	slackavr = -2;
	w_b = 0;
	w_c = 0;
	r_b = 0;
	r_c = 0;
	i_b = 0;
	i_c = 0;
	v = 1;
	v_b = 0;
	v_c = 0;
	wmx = 1.0e6;
	rmx = 1.0e6;
	wmn = -1.0e6;
	rmn = -1.0e6;
	algp_b = 0;
	algp_c = 0;
	ialgp_b = 0;
	ialgp_c = 0;
	algq_b = 0;
	algq_c = 0;
	ialgq_b = 0;
	algi_b = 0;
	ialgq_c = 0;
	algi_c = 0;
	ialgi_b = 0;
	ialgi_c = 0;
	adeg = 0;
	adeg_b = 0;
	adeg_c = 0;
//张瑞雪添加于20120503
	algp_a = 0;
	ialgp_a = -1;
	algq_a = 0;
	ialgq_a = -1;
	algi_a = 0;
	ialgi_a = -1;
	w_a = 0;
	r_a = 0;
	i_a = 0;
	v_a = 0;
	adeg_a = 0;
	stid=0;
}

//void SO::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar.Write(nd,35);
//		ar << ind;
//		ar << ibs;
//		ar << iisland;
//		ar.Write(algp,20);
//		ar << ialgp;
//		ar.Write(algq,20);
//		ar << ialgq;
//		ar.Write(algi,20);
//		ar << ialgi;
//		ar << q;
//		ar << topflags;
//		ar << w;
//		ar << r;
//		ar << i;
//		ar << r_st;
//		ar << ss;
//		ar << sw;
//		ar << sr;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar.Read(nd,35);
//		ar >> ind;
//		ar >> ibs;
//		ar >> iisland;
//		ar.Read(algp,20);
//		ar >> ialgp;
//		ar.Read(algq,20);
//		ar >> ialgq;
//		ar.Read(algi,20);
//		ar >> ialgi;
//		ar >> q;
//		ar >> topflags;
//		ar >> w;
//		ar >> r;
//		ar >> i;
//		ar >> r_st;
//		ar >> ss;
//		ar >> sw;
//		ar >> sr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class ST //IMPLEMENTation

//IMPLEMENT_SERIAL(ST,CObject,0)

CLS_ST::CLS_ST()
{
    id = 0;
    pid=0;
    descr[0] = 0;
    type = 0;
    qcomp = 0;
    wso = 0;
    rso = 0;
    wload = 0;
    rload = 0;
    r_co = -1;
	r_xfmr = -1;
	r_so = -1;
	r_cb = -1;
	r_switch = -1;
	r_ld = -1;
	r_cp = -1;
	r_bus = -1;
	r_zn = -1;
//张瑞雪添加于20120209
	wso_b = 0;
	rso_b = 0;
	wload_b = 0;
	rload_b = 0;
	wso_c = 0;
	rso_c = 0;
	wload_c = 0;
	rload_c = 0;
//张瑞雪添加于20120503
	wso_a = 0;
	rso_a = 0;
	wload_a = 0;
	rload_a = 0;
}

const CLS_ST& CLS_ST::operator = (const CLS_ST& m_ST)
{
	id=m_ST.id;
	pid=m_ST.pid;
	strcpy(descr,m_ST.descr);
	type = m_ST.type;
	qcomp = m_ST.qcomp;
	wso = m_ST.wso;
	rso = m_ST.rso;
	wload = m_ST.wload;
	rload = m_ST.rload;
	r_co = m_ST.r_co;
	r_xfmr = m_ST.r_xfmr;
	r_so = m_ST.r_so;
	r_cb = m_ST.r_cb;
	r_switch = m_ST.r_switch;
	r_ld = m_ST.r_ld;
	r_cp = m_ST.r_cp;
	r_bus = m_ST.r_bus;
	r_zn = m_ST.r_zn;
//张瑞雪添加于20120209
	wso_b = m_ST.wso_b;
	rso_b = m_ST.rso_b;
	wload_b = m_ST.wload_b;
	rload_b = m_ST.rload_b;
	wso_c = m_ST.wso_c;
	rso_c = m_ST.rso_c;
	wload_c = m_ST.wload_c;
	rload_c = m_ST.rload_c;
//张瑞雪添加于20120503
	wso_a = m_ST.wso_a;
	rso_a = m_ST.rso_a;
	wload_a = m_ST.wload_a;
	rload_a = m_ST.rload_a;
	return* this;
}
CLS_ST::CLS_ST(const long m_id)
{
	id=m_id;
    pid = 0;
    descr[0] = 0;
    type = 0;
    qcomp = 0;
    wso = 0.0;
    rso = 0.0;
    wload = 0.0;
    rload = 0.0;
    r_co = -1;
    r_xfmr = -1;
    r_so = -1;
    r_cb = -1;
    r_switch = -1;
    r_ld = -1;
	r_cp = -1;
	r_bus = -1;
	r_zn = -1;
//张瑞雪添加于20120209
	wso_b = 0;
	rso_b = 0;
	wload_b = 0;
	rload_b = 0;
	wso_c = 0;
	rso_c = 0;
	wload_c = 0;
	rload_c = 0;
//张瑞雪添加于20120503
	wso_a = 0;
	rso_a = 0;
	wload_a = 0;
	rload_a = 0;
}

//void ST::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar.Write(abb,4);
//		ar << type;
//		ar << qcomp;
//		ar << wso;
//		ar << rso;
//		ar << wload;
//		ar << rload;
//		ar << r_co;
//		ar << r_xfmr;
//		ar << r_so;
//		ar << r_cb;
//		ar << r_ld;
//		ar << r_cp;
//		ar << r_bus;
//		ar << r_zn;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar.Read(abb,4);
//		ar >> type;
//		ar >> qcomp;
//		ar >> wso;
//		ar >> rso;
//		ar >> wload;
//		ar >> rload;
//		ar >> r_co;
//		ar >> r_xfmr;
//		ar >> r_so;
//		ar >> r_cb;
//		ar >> r_ld;
//		ar >> r_cp;
//		ar >> r_bus;
//		ar >> r_zn;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class TAPTY //IMPLEMENTation

//IMPLEMENT_SERIAL(TAPTY,CObject,0)

CLS_TAPTY::CLS_TAPTY()
{
    id = 0;
    pid=0;
    mn = 0;
    mx = 0;
    nom = 0;
    step = 0;
}

const CLS_TAPTY& CLS_TAPTY::operator = (const CLS_TAPTY& m_TAPTY)
{
	id = m_TAPTY.id;
	pid = m_TAPTY.pid;
	mn = m_TAPTY.mn;
	mx = m_TAPTY.mx;
	nom = m_TAPTY.nom;
	step = m_TAPTY.step;
	return *this;
}
CLS_TAPTY::CLS_TAPTY(const long m_id)
{
	id = m_id;
	pid = 0;
	mn = 0;
	mx = 0;
	nom = 0;
	step = 0.0;
}

//void TAPTY::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar << mn;
//		ar << mx;
//		ar << nom;
//		ar << step;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar >> mn;
//		ar >> mx;
//		ar >> nom;
//		ar >> step;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class XF //IMPLEMENTation

//IMPLEMENT_SERIAL(XF,CObject,0)

CLS_XF::CLS_XF()
{
    id = 0;
    descr[0] = 0;
    itapty = -1;
    iztapty = -1;
    tap = 0;
    ztap = 0;
    kvnom = 0;
    zkvnom = 0;
    nd = 0;
    znd = 0;
    ind = -1;
    iznd = -1;
    ibs = -1;
    zbs = -1;
    mvanom = 0;
    q = 0;
    topflags = 0;
    r = 0;
    x = 0;
    ialgip = -1;
    ialgiq = -1;
    ialgii = -1;
    ialgit = -1;
    ialgzp = -1;
    ialgzq = -1;
    ialgzi = -1;
    ialgzt = -1;
    w = 0;
    r_ = 0;
    i = 0;
    wz = 0;
    rz = 0;
    iz = 0;
    r_xfmr = -1;
//张瑞雪添加于20120215
    xftype = -1;
	itapty_b = -1;
	itapty_c = -1;
	iztapty_b = -1;
	iztapty_c = -1;
	tap_b = 0;
	tap_c = 0;
	ztap_b = 0;
	ztap_c = 0;
	r_b = 0;
	r_c = 0;
	x_b = 0;
	x_c = 0;
	ialgip_b = -1;
	ialgip_c = -1;
	ialgiq_b = -1;
	ialgiq_c = -1;
	ialgii_b = -1;
	ialgii_c = -1;
	ialgit_b = -1;
	ialgit_c = -1;
	ialgzp_b = -1;
	ialgzp_c = -1;
	ialgzq_b = -1;
	ialgzq_c = -1;
	ialgzi_b = -1;
	ialgzi_c = -1;
	ialgzt_b = -1;
	ialgzt_c = -1;
	w_b = 0;
	w_c = 0;
	rq_b = 0;
	rq_c = 0;
	i_b = 0;
	i_c = 0;
	wz_b = 0;
	wz_c = 0;
	rz_b = 0;
	rz_c = 0;
	iz_b = 0;
	iz_c = 0;
//张瑞雪添加于20120503
	r_a = 0;
	x_a = 0;
	w_a = 0;
	rq_a = 0;
	i_a = 0;
	wz_a = 0;
	rz_a = 0;
	iz_a = 0;
	ialgip_a = -1;
	ialgiq_a = -1;
	ialgii_a = -1;
	ialgzp_a = -1;
	ialgzq_a = -1;
	ialgzi_a = -1;
}

const CLS_XF& CLS_XF::operator = (const CLS_XF& m_XF)
{
	id = m_XF.id;
	pid = m_XF.pid;
	strcpy(descr, m_XF.descr);
	itapty = m_XF.itapty;
	iztapty = m_XF.iztapty;
	tap = m_XF.tap;
	ztap = m_XF.ztap;
	kvnom = m_XF.kvnom;
	zkvnom = m_XF.zkvnom;
	nd = m_XF.nd;
	znd = m_XF.znd;
	ind = m_XF.ind;
	iznd = m_XF.iznd;
	ibs = m_XF.ibs;
	zbs = m_XF.zbs;
	mvanom = m_XF.mvanom;
	q = m_XF.q;
	topflags = m_XF.topflags;
	r = m_XF.r;
	x = m_XF.x;
	ialgip = m_XF.ialgip;
	ialgiq = m_XF.ialgiq;
	ialgii = m_XF.ialgii;
	ialgit = m_XF.ialgit;
	ialgzp = m_XF.ialgzp;
	ialgzq = m_XF.ialgzq;
	ialgzi = m_XF.ialgzi;
	ialgzt = m_XF.ialgzt;
	w = m_XF.w;
	r_ = m_XF.r_;
	i = m_XF.i;
	wz = m_XF.wz;
	rz = m_XF.rz;
	iz = m_XF.iz;
	r_xfmr = m_XF.r_xfmr;
//张瑞雪添加于20120215
	xftype = m_XF.xftype;
	itapty_b = m_XF.itapty_b;
	iztapty_b = m_XF.iztapty_b;
	tap_b = m_XF.tap_b;
	ztap_b = m_XF.ztap_b;
	r_b = m_XF.r_b;
	x_b = m_XF.x_b;
	ialgip_b = m_XF.ialgip_b;
	ialgiq_b = m_XF.ialgiq_b;
	ialgii_b = m_XF.ialgii_b;
	ialgit_b = m_XF.ialgit_b;
	ialgzp_b = m_XF.ialgzp_b;
	ialgzq_b = m_XF.ialgzq_b;
	ialgzi_b = m_XF.ialgzi_b;
	ialgzt_b = m_XF.ialgzt_b;
	w_b = m_XF.w_b;
	rq_b = m_XF.rq_b;
	i_b = m_XF.i_b;
	wz_b = m_XF.wz_b;
	rz_b = m_XF.rz_b;
	iz_b = m_XF.iz_b;
	itapty_c = m_XF.itapty_c;
	iztapty_c = m_XF.iztapty_c;
	tap_c = m_XF.tap_c;
	ztap_c = m_XF.ztap_c;
	r_c = m_XF.r_c;
	x_c = m_XF.x_c;
	ialgip_c = m_XF.ialgip_c;
	ialgiq_c = m_XF.ialgiq_c;
	ialgii_c = m_XF.ialgii_c;
	ialgit_c = m_XF.ialgit_c;
	ialgzp_c = m_XF.ialgzp_c;
	ialgzq_c = m_XF.ialgzq_c;
	ialgzi_c = m_XF.ialgzi_c;
	ialgzt_c = m_XF.ialgzt_c;
	w_c = m_XF.w_c;
	rq_c = m_XF.rq_c;
	i_c = m_XF.i_c;
	wz_c = m_XF.wz_c;
	rz_c = m_XF.rz_c;
	iz_c = m_XF.iz_c;
//张瑞雪添加于20120503
	r_a = m_XF.r_a;
	x_a = m_XF.x_a;
	ialgip_a = m_XF.ialgip_a;
	ialgiq_a = m_XF.ialgiq_a;
	ialgii_a = m_XF.ialgii_a;
	ialgzp_a = m_XF.ialgzp_a;
	ialgzq_a = m_XF.ialgzq_a;
	ialgzi_a = m_XF.ialgzi_a;
	w_a = m_XF.w_a;
	rq_a = m_XF.rq_a;
	i_a = m_XF.i_a;
	wz_a = m_XF.wz_a;
	rz_a = m_XF.rz_a;
	iz_a = m_XF.iz_a;
	return *this;
}
CLS_XF::CLS_XF(const long m_id)
{
	id = m_id;
	pid = 0;
	descr[0] = 0;
	itapty = -1;
	iztapty = -1;
	tap = 0;
	ztap = 0;
	kvnom = 0.0;
	zkvnom = 0.0;
	nd = 0;
	znd = 0;
	ind = -1;
	iznd = -1;
	ibs = -1;
	zbs = -1;
	mvanom = 0.0;
	q = 0;
	topflags = 0;
	r = 0.0;
	x = 0.0;
	ialgip = -1;
	ialgiq = -1;
	ialgii = -1;
	ialgit = -1;
	ialgzp = -1;
	ialgzq = -1;
	ialgzi = -1;
	ialgzt = -1;
	w = 0.0;
	r_ = 0.0;
	i = 0.0;
	wz = 0.0;
	rz = 0.0;
	iz = 0.0;
	r_xfmr = -1;
//张瑞雪添加于20120215
	xftype = -1;
	itapty_b = -1;
	itapty_c = -1;
	iztapty_b = -1;
	iztapty_c = -1;
	tap_b = 0;
	tap_c = 0;
	ztap_b = 0;
	ztap_c = 0;
	r_b = 0;
	r_c = 0;
	x_b = 0;
	x_c = 0;
	ialgip_b = -1;
	ialgip_c = -1;
	ialgiq_b = -1;
	ialgiq_c = -1;
	ialgii_b = -1;
	ialgii_c = -1;
	ialgit_b = -1;
	ialgit_c = -1;
	ialgzp_b = -1;
	ialgzp_c = -1;
	ialgzq_b = -1;
	ialgzq_c = -1;
	ialgzi_b = -1;
	ialgzi_c = -1;
	ialgzt_b = -1;
	ialgzt_c = -1;
	w_b = 0;
	w_c = 0;
	rq_b = 0;
	rq_c = 0;
	i_b = 0;
	i_c = 0;
	wz_b = 0;
	wz_c = 0;
	rz_b = 0;
	rz_c = 0;
	iz_b = 0;
	iz_c = 0;
//张瑞雪添加于20120503
	r_a = 0;
	x_a = 0;
	w_a = 0;
	rq_a = 0;
	i_a = 0;
	wz_a = 0;
	rz_a = 0;
	iz_a = 0;
	ialgip_a = -1;
	ialgiq_a = -1;
	ialgii_a = -1;
	ialgzp_a = -1;
	ialgzq_a = -1;
	ialgzi_a = -1;
}

//void XF::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar << itapty;
//		ar << iztapty;
//		ar << tap;
//		ar << ztap;
//		ar << kvnom;
//		ar << zkvnom;
//		ar.Write(nd,20);
//		ar.Write(znd,20);
//		ar << ind;
//		ar << iznd;
//		ar << ibs;
//		ar << zbs;
//		ar << mvanom;
//		ar << q;
//		ar << topflags;
//		ar << r;
//		ar << x;
//		ar << ialgip;
//		ar << ialgiq;
//		ar << ialgii;
//		ar << ialgit;
//		ar << ialgzp;
//		ar << ialgzq;
//		ar << ialgzi;
//		ar << ialgzt;
//		ar << w;
//		ar << r_;
//		ar << i;
//		ar << wz;
//		ar << rz;
//		ar << iz;
//		ar << r_xfmr;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar >> itapty;
//		ar >> iztapty;
//		ar >> tap;
//		ar >> ztap;
//		ar >> kvnom;
//		ar >> zkvnom;
//		ar.Read(nd,20);
//		ar.Read(znd,20);
//		ar >> ind;
//		ar >> iznd;
//		ar >> ibs;
//		ar >> zbs;
//		ar >> mvanom;
//		ar >> q;
//		ar >> topflags;
//		ar >> r;
//		ar >> x;
//		ar >> ialgip;
//		ar >> ialgiq;
//		ar >> ialgii;
//		ar >> ialgit;
//		ar >> ialgzp;
//		ar >> ialgzq;
//		ar >> ialgzi;
//		ar >> ialgzt;
//		ar >> w;
//		ar >> r_;
//		ar >> i;
//		ar >> wz;
//		ar >> rz;
//		ar >> iz;
//		ar >> r_xfmr;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class XFMR //IMPLEMENTation

//IMPLEMENT_SERIAL(XFMR,CObject,0)

CLS_XFMR::CLS_XFMR()
{
    id = 0;
    pid=0;
    descr[0] = 0;
    type = 0;
    st = 0;
    rh = 0;
    xh = 0;
    rm = 0;
    xm = 0;
    rl = 0;
    xl = 0;
    kvnomh = 10;
    kvnomm = 10;
    kvnoml = 10;
    mvah = 100;
    mvam = 100;
    mval = 100;
    algph = 0;
    ialgph = -1;
    algqh = 0;
    ialgqh = -1;
    algih = 0;
    ialgih = -1;
    algth = 0;
    ialgth = -1;
    algpm = 0;
    ialgpm = -1;
    algqm = 0;
    ialgqm = -1;
    algim = 0;
    ialgim = -1;
    algtm = 0;
    ialgtm = -1;
    algpl = 0;
    ialgpl = -1;
    algql = 0;
    ialgql = -1;
    algil = 0;
    ialgil = -1;
    algtl = 0;
    ialgtl = -1;
    hnd = 0;
    mnd = 0;
    lnd = 0;
    htapty = 0;
    mtapty = 0;
    ltapty = 0;
    htap = -1;
    mtap = -1;
    ltap = -1;
    q = 0;
	topflags = 0;
	r_st = -1;
	r_xf = -1;
//张瑞雪添加于20120215
	rh_b = 0;
	rh_c = 0;
	xh_b = 0;
	xh_c = 0;
	rl_b = 0;
	rl_c = 0;
	xl_b = 0;
	xl_c = 0;
	algph_b = 0;
	algph_c = 0;
	ialgph_b = -1;
	ialgph_c = -1;
	algqh_b = 0;
	algqh_c = 0;
	ialgqh_b = -1;
	ialgqh_c = -1;
	algih_b = 0;
	algih_c = 0;
	ialgih_b = -1;
	ialgih_c = -1;
	algth_b = 0;
	algth_c = 0;
	ialgth_b = -1;
	ialgth_c = -1;
	algpl_b = 0;
	algpl_c = 0;
	ialgpl_b = -1;
	ialgpl_c = -1;
	algql_b = 0;
	algql_c = 0;
	ialgql_b = -1;
	ialgql_c = -1;
	algil_b = 0;
	algil_c = 0;
	ialgil_b = -1;
	ialgil_c = -1;
	algtl_b = 0;
	algtl_c = 0;
	ialgtl_b = -1;
	ialgtl_c = -1;
	htapty_b = 0;
	htapty_c = 0;
	ltapty_b = 0;
	ltapty_c = 0;
	htap_b = -1;
	htap_c = -1;
	ltap_b = -1;
	ltap_c = -1;
	xftype = -1;
//张瑞雪添加于20120503
	rh_a = 0;
	xh_a = 0;
	rl_a = 0;
	xl_a = 0;
	algph_a = 0;
	ialgph_a = -1;
	algqh_a = 0;
	ialgqh_a = -1;
	algih_a = 0;
	ialgih_a = -1;
	algpl_a = 0;
	ialgpl_a = -1;
	algql_a = 0;
	ialgql_a = -1;
	algil_a = 0;
	ialgil_a = -1;
	stid=0;
}

const CLS_XFMR& CLS_XFMR::operator = (const CLS_XFMR& m_XFMR)
{
	id = m_XFMR.id;
	strcpy(descr, m_XFMR.descr);
	type = m_XFMR.type;
	st = m_XFMR.st;
	rh = m_XFMR.rh;
	xh = m_XFMR.xh;
	rm = m_XFMR.rm;
	xm = m_XFMR.xm;
	rl = m_XFMR.rl;
	xl = m_XFMR.xl;
	kvnomh = m_XFMR.kvnomh;
	kvnomm = m_XFMR.kvnomm;
	kvnoml = m_XFMR.kvnoml;
	mvah = m_XFMR.mvah;
	mvam = m_XFMR.mvam;
	mval = m_XFMR.mval;
	algph = m_XFMR.algph;
	ialgph = m_XFMR.ialgph;
	algqh = m_XFMR.algqh;
	ialgqh = m_XFMR.ialgqh;
	algih = m_XFMR.algih;
	ialgih = m_XFMR.ialgih;
	algth = m_XFMR.algth;
	ialgth = m_XFMR.ialgth;
	algpm = m_XFMR.algpm;
	ialgpm = m_XFMR.ialgpm;
	algqm = m_XFMR.algqm;
	ialgqm = m_XFMR.ialgqm;
	algim = m_XFMR.algim;
	ialgim = m_XFMR.ialgim;
	algtm = m_XFMR.algtm;
	ialgtm = m_XFMR.ialgtm;
	algpl = m_XFMR.algpl;
	ialgpl = m_XFMR.ialgpl;
	algql = m_XFMR.algql;
	ialgql = m_XFMR.ialgql;
	algil = m_XFMR.algil;
	ialgil = m_XFMR.ialgil;
	algtl = m_XFMR.algtl;
	ialgtl = m_XFMR.ialgtl;
	hnd = m_XFMR.hnd;
	mnd = m_XFMR.mnd;
	lnd = m_XFMR.lnd;
	htapty = m_XFMR.htapty;
	mtapty = m_XFMR.mtapty;
	ltapty = m_XFMR.ltapty;
	htap = m_XFMR.htap;
	mtap = m_XFMR.mtap;
	ltap = m_XFMR.ltap;
	q = m_XFMR.q;
	topflags = m_XFMR.topflags;
	r_st = m_XFMR.r_st;
	r_xf = m_XFMR.r_xf;
//张瑞雪添加于20120215
	rh_b = m_XFMR.rh_b;
	rh_c = m_XFMR.rh_c;
	xh_b = m_XFMR.xh_b;
	xh_c = m_XFMR.xh_c;
	rl_b = m_XFMR.rl_b;
	rl_c = m_XFMR.rl_c;
	xl_b = m_XFMR.xl_b;
	xl_c = m_XFMR.xl_c;
	algph_b = m_XFMR.algph_b;
	algph_c = m_XFMR.algph_c;
	ialgph_b = m_XFMR.ialgph_b;
	ialgph_c = m_XFMR.ialgph_c;
	algqh_b = m_XFMR.algqh_b;
	algqh_c = m_XFMR.algqh_c;
	ialgqh_b = m_XFMR.ialgqh_b;
	ialgqh_c = m_XFMR.ialgqh_c;
	algih_b = m_XFMR.algih_b;
	algih_c = m_XFMR.algih_c;
	ialgih_b = m_XFMR.ialgih_b;
	ialgih_c = m_XFMR.ialgih_c;
	algth_b = m_XFMR.algth_b;
	algth_c = m_XFMR.algth_c;
	ialgth_b = m_XFMR.ialgth_b;
	ialgth_c = m_XFMR.ialgth_c;
	algpl_b = m_XFMR.algpl_b;
	algpl_c = m_XFMR.algpl_c;
	ialgpl_b = m_XFMR.ialgpl_b;
	ialgpl_c = m_XFMR.ialgpl_c;
	algql_b = m_XFMR.algql_b;
	algql_c = m_XFMR.algql_c;
	ialgql_b = m_XFMR.ialgql_b;
	ialgql_c = m_XFMR.ialgql_c;
	algil_b = m_XFMR.algil_b;
	algil_c = m_XFMR.algil_c;
	ialgil_b = m_XFMR.ialgil_b;
	ialgil_c = m_XFMR.ialgil_c;
	algtl_b = m_XFMR.algtl_b;
	algtl_c = m_XFMR.algtl_c;
	ialgtl_b = m_XFMR.ialgtl_b;
	ialgtl_c = m_XFMR.ialgtl_c;
	htapty_b = m_XFMR.htapty_b;
	htapty_c = m_XFMR.htapty_c;
	ltapty_b = m_XFMR.ltapty_b;
	ltapty_c = m_XFMR.ltapty_c;
	htap_b = m_XFMR.htap_b;
	htap_c = m_XFMR.htap_c;
	ltap_b = m_XFMR.ltap_b;
	ltap_c = m_XFMR.ltap_c;
	xftype = m_XFMR.xftype;
//张瑞雪添加于20120503
	rh_a = m_XFMR.rh_a;
	xh_a = m_XFMR.xh_a;
	rl_a = m_XFMR.rl_a;
	xl_a = m_XFMR.xl_a;
	algph_a = m_XFMR.algph_a;
	ialgph_a = m_XFMR.ialgph_a;
	algqh_a = m_XFMR.algqh_a;
	ialgqh_a = m_XFMR.ialgqh_a;
	algih_a = m_XFMR.algih_a;
	ialgih_a = m_XFMR.ialgih_a;
	algpl_a = m_XFMR.algpl_a;
	ialgpl_a = m_XFMR.ialgpl_a;
	algql_a = m_XFMR.algql_a;
	ialgql_a = m_XFMR.ialgql_a;
	algil_a = m_XFMR.algil_a;
	ialgil_a = m_XFMR.ialgil_a;
	stid = m_XFMR.stid;

	return *this;
}
CLS_XFMR::CLS_XFMR(const long m_id)
{
    id=m_id;
    pid=0;
    descr[0] = 0;
    type = 0;
    st = 0;
    rh = 0.0;
    xh = 0.0;
    rm = 0.0;
    xm = 0.0;
    rl = 0.0;
    xl = 0.0;
    kvnomh = 10;
    kvnomm = 10;
    kvnoml = 10;
    mvah = 100.0;
    mvam = 100.0;
    mval = 100.0;
    algph = 0;
    ialgph = -1;
    algqh = 0;
    ialgqh = -1;
    algih = 0;
    ialgih = -1;
    algth = 0;
    ialgth = -1;
    algpm = 0;
    ialgpm = -1;
    algqm = 0;
    ialgqm = -1;
    algim = 0;
    ialgim = -1;
    algtm = 0;
    ialgtm = -1;
    algpl = 0;
    ialgpl = -1;
    algql = 0;
    ialgql = -1;
    algil = 0;
    ialgil = -1;
    algtl = 0;
    ialgtl = -1;
    hnd = 0;
    mnd = 0;
    lnd = 0;
    htapty = 0;
    mtapty = 0;
    ltapty = 0;
    htap = -1;
    mtap = -1;
    ltap = -1;
    q = 0;
    topflags = 0;
    r_st = -1;
	r_xf = -1;
//张瑞雪添加于20120209
	rh_b = 0;
	rh_c = 0;
	xh_b = 0;
	xh_c = 0;
	rl_b = 0;
	rl_c = 0;
	xl_b = 0;
	xl_c = 0;
	algph_b = 0;
	algph_c = 0;
	ialgph_b = -1;
	ialgph_c = -1;
	algqh_b = 0;
	algqh_c = 0;
	ialgqh_b = -1;
	ialgqh_c = -1;
	algih_b = 0;
	algih_c = 0;
	ialgih_b = -1;
	ialgih_c = -1;
	algth_b = 0;
	algth_c = 0;
	ialgth_b = -1;
	ialgth_c = -1;
	algpl_b = 0;
	algpl_c = 0;
	ialgpl_b = -1;
	ialgpl_c = -1;
	algql_b = 0;
	algql_c = 0;
	ialgql_b = -1;
	ialgql_c = -1;
	algil_b = 0;
	algil_c = 0;
	ialgil_b = -1;
	ialgil_c = -1;
	algtl_b = 0;
	algtl_c = 0;
	ialgtl_b = -1;
	ialgtl_c = -1;
	htapty_b = 0;
	htapty_c = 0;
	ltapty_b = 0;
	ltapty_c = 0;
	htap_b = -1;
	htap_c = -1;
	ltap_b = -1;
	ltap_c = -1;
	xftype = -1;
//张瑞雪添加于20120503
	rh_a = 0;
	xh_a = 0;
	rl_a = 0;
	xl_a = 0;
	algph_a = 0;
	ialgph_a = -1;
	algqh_a = 0;
	ialgqh_a = -1;
	algih_a = 0;
	ialgih_a = -1;
	algpl_a = 0;
	ialgpl_a = -1;
	algql_a = 0;
	ialgql_a = -1;
	algil_a = 0;
	ialgil_a = -1;
	stid=0;
}

//void XFMR::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar << type;
//		ar.Write(st,20);
//		ar << rh;
//		ar << xh;
//		ar << rm;
//		ar << xm;
//		ar << rl;
//		ar << xl;
//		ar << kvnomh;
//		ar << kvnomm;
//		ar << kvnoml;
//		ar << mvah;
//		ar << mvam;
//		ar << mval;
//		ar.Write(algph,20);
//		ar << ialgph;
//		ar.Write(algqh,20);
//		ar << ialgqh;
//		ar.Write(algih,20);
//		ar << ialgih;
//		ar.Write(algth,20);
//		ar << ialgth;
//		ar.Write(algpm,20);
//		ar << ialgpm;
//		ar.Write(algqm,20);
//		ar << ialgqm;
//		ar.Write(algim,20);
//		ar << ialgim;
//		ar.Write(algtm,20);
//		ar << ialgtm;
//		ar.Write(algpl,20);
//		ar << ialgpl;
//		ar.Write(algql,20);
//		ar << ialgql;
//		ar.Write(algil,20);
//		ar << ialgil;
//		ar.Write(algtl,20);
//		ar << ialgtl;
//		ar.Write(hnd,20);
//		ar.Write(mnd,20);
//		ar.Write(lnd,20);
//		ar.Write(htapty,20);
//		ar.Write(mtapty,20);
//		ar.Write(ltapty,20);
//		ar << htap;
//		ar << mtap;
//		ar << ltap;
//		ar << q;
//		ar << topflags;
//		ar << r_st;
//		ar << r_xf;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar >> type;
//		ar.Read(st,20);
//		ar >> rh;
//		ar >> xh;
//		ar >> rm;
//		ar >> xm;
//		ar >> rl;
//		ar >> xl;
//		ar >> kvnomh;
//		ar >> kvnomm;
//		ar >> kvnoml;
//		ar >> mvah;
//		ar >> mvam;
//		ar >> mval;
//		ar.Read(algph,20);
//		ar >> ialgph;
//		ar.Read(algqh,20);
//		ar >> ialgqh;
//		ar.Read(algih,20);
//		ar >> ialgih;
//		ar.Read(algth,20);
//		ar >> ialgth;
//		ar.Read(algpm,20);
//		ar >> ialgpm;
//		ar.Read(algqm,20);
//		ar >> ialgqm;
//		ar.Read(algim,20);
//		ar >> ialgim;
//		ar.Read(algtm,20);
//		ar >> ialgtm;
//		ar.Read(algpl,20);
//		ar >> ialgpl;
//		ar.Read(algql,20);
//		ar >> ialgql;
//		ar.Read(algil,20);
//		ar >> ialgil;
//		ar.Read(algtl,20);
//		ar >> ialgtl;
//		ar.Read(hnd,20);
//		ar.Read(mnd,20);
//		ar.Read(lnd,20);
//		ar.Read(htapty,20);
//		ar.Read(mtapty,20);
//		ar.Read(ltapty,20);
//		ar >> htap;
//		ar >> mtap;
//		ar >> ltap;
//		ar >> q;
//		ar >> topflags;
//		ar >> r_st;
//		ar >> r_xf;
//	}
//}
/////////////////////////////////////////////////////////////////////////
// class ZN //IMPLEMENTation

//IMPLEMENT_SERIAL(ZN,CObject,0)

CLS_ZN::CLS_ZN()
{
    id = 0;
    descr[0] = 0;
    type = 0;
    nds.clear();
    q = 0;
    topflags = 0;
    r_st = -1;
    r_ld = -1;
    r_sec = -1;
    r_nd = -1;
	stid=0;
}

const CLS_ZN& CLS_ZN::operator = (const CLS_ZN& m_ZN)
{
	id=m_ZN.id;
	strcpy(descr,m_ZN.descr);
	type = m_ZN.type;
	nds = m_ZN.nds;
	q = m_ZN.q;
	topflags = m_ZN.topflags;
	r_st = m_ZN.r_st;
	r_ld = m_ZN.r_ld;
	r_sec = m_ZN.r_sec;
	r_nd = m_ZN.r_nd;
	stid = m_ZN.stid;
	return* this;
}
CLS_ZN::CLS_ZN(const long m_id)
{
	id=m_id;
    descr[0] = 0;
    type = 0;
    nds.clear();
    q = 0;
    topflags = 0;
    r_st = -1;
    r_ld = -1;
    r_sec = -1;
    r_nd = -1;
	stid=0;
}

//void ZN::Serialize( CArchive& ar )
//{
//	if( ar.IsStoring() )
//	{
//		ar.Write(id,20);
//		ar.Write(descr,64);
//		ar << type;
//		ar.Write(nd,20);
//		ar.Write(znd,20);
//		ar.Write(nd3,20);
//		ar.Write(nd4,20);
//		ar.Write(nd5,20);
//		ar << ind;
//		ar << iznd;
//		ar << ind3;
//		ar << ind4;
//		ar << ind5;
//		ar << ibs;
//		ar << zbs;
//		ar << ibs3;
//		ar << ibs4;
//		ar << ibs5;
//		ar.Write(algip,20);
//		ar << ialgip;
//		ar.Write(algiq,20);
//		ar << ialgiq;
//		ar.Write(algii,20);
//		ar << ialgii;
//		ar.Write(algzp,20);
//		ar << ialgzp;
//		ar.Write(algzq,20);
//		ar << ialgzq;
//		ar.Write(algzi,20);
//		ar << ialgzi;
//		ar.Write(alg3p,20);
//		ar << ialg3p;
//		ar.Write(alg3q,20);
//		ar << ialg3q;
//		ar.Write(alg3i,20);
//		ar << ialg3i;
//		ar.Write(alg4p,20);
//		ar << ialg4p;
//		ar.Write(alg4q,20);
//		ar << ialg4q;
//		ar.Write(alg4i,20);
//		ar << ialg4i;
//		ar.Write(alg5p,20);
//		ar << ialg5p;
//		ar.Write(alg5q,20);
//		ar << ialg5q;
//		ar.Write(alg5i,20);
//		ar << ialg5i;
//		ar << q;
//		ar << topflags;
//		ar << w;
//		ar << r;
//		ar << i;
//		ar << wz;
//		ar << rz;
//		ar << iz;
//		ar << w3;
//		ar << r3;
//		ar << i3;
//		ar << w4;
//		ar << r4;
//		ar << i4;
//		ar << w5;
//		ar << r5;
//		ar << i5;
//		ar << r_st;
//		ar << r_ld;
//		ar << r_sec;
//	}
//	else
//	{
//		ar.Read(id,20);
//		ar.Read(descr,64);
//		ar >> type;
//		ar.Read(nd,20);
//		ar.Read(znd,20);
//		ar.Read(nd3,20);
//		ar.Read(nd4,20);
//		ar.Read(nd5,20);
//		ar >> ind;
//		ar >> iznd;
//		ar >> ind3;
//		ar >> ind4;
//		ar >> ind5;
//		ar >> ibs;
//		ar >> zbs;
//		ar >> ibs3;
//		ar >> ibs4;
//		ar >> ibs5;
//		ar.Read(algip,20);
//		ar >> ialgip;
//		ar.Read(algiq,20);
//		ar >> ialgiq;
//		ar.Read(algii,20);
//		ar >> ialgii;
//		ar.Read(algzp,20);
//		ar >> ialgzp;
//		ar.Read(algzq,20);
//		ar >> ialgzq;
//		ar.Read(algzi,20);
//		ar >> ialgzi;
//		ar.Read(alg3p,20);
//		ar >> ialg3p;
//		ar.Read(alg3q,20);
//		ar >> ialg3q;
//		ar.Read(alg3i,20);
//		ar >> ialg3i;
//		ar.Read(alg4p,20);
//		ar >> ialg4p;
//		ar.Read(alg4q,20);
//		ar >> ialg4q;
//		ar.Read(alg4i,20);
//		ar >> ialg4i;
//		ar.Read(alg5p,20);
//		ar >> ialg5p;
//		ar.Read(alg5q,20);
//		ar >> ialg5q;
//		ar.Read(alg5i,20);
//		ar >> ialg5i;
//		ar >> q;
//		ar >> topflags;
//		ar >> w;
//		ar >> r;
//		ar >> i;
//		ar >> wz;
//		ar >> rz;
//		ar >> iz;
//		ar >> w3;
//		ar >> r3;
//		ar >> i3;
//		ar >> w4;
//		ar >> r4;
//		ar >> i4;
//		ar >> w5;
//		ar >> r5;
//		ar >> i5;
//		ar >> r_st;
//		ar >> r_ld;
//		ar >> r_sec;
//	}
//}

//haozeyun 2011.11.23 tianjia
CLS_CTL::CLS_CTL()
{
	id=0;
	method=0;
	itermx=0;
	vtol=0.0;
	start=time(0);
	second=0.0;
	iter=0;
	convg=0;
}
CLS_CTL::CLS_CTL(int m_id)
{
	id=m_id;
	method=0;
	itermx=0;
	vtol=0.0;
	start=time(0);
	second=0.0;
	iter=0;
	convg=0;
}

const CLS_CTL&CLS_CTL:: operator = (const CLS_CTL& m_ctl)
{
	id = m_ctl.id;
	method=m_ctl.method;
	itermx=m_ctl.itermx;
	vtol=m_ctl.vtol;
	start=m_ctl.start;
	second=m_ctl.second;
	iter=m_ctl.iter;
	convg = m_ctl.convg;
	return* this;
}


//haozeyun 2012.4.24tianjia
CLS_LNMODE::CLS_LNMODE()
{
	 id = 0;
	 descr[0] = 0;
	 type = 0;
	 r_aa = 0.0;
	 r_ab = 0.0;
	 r_ac = 0.0;
	 r_ba = 0.0;
	 r_bb = 0.0;
	 r_bc = 0.0;
	 r_ca = 0.0;
	 r_cb = 0.0;
	 r_cc = 0.0;
	 x_aa = 0.0;
	 x_ab = 0.0;
	 x_ac = 0.0;
	 x_ba = 0.0;
	 x_bb = 0.0;
	 x_bc = 0.0;
	 x_ca = 0.0;
	 x_cb = 0.0;
	 x_cc = 0.0;
	 b_aa = 0.0;
	 b_ab = 0.0;
	 b_ac = 0.0;
	 b_ba = 0.0;
	 b_bb = 0.0;
	 b_bc = 0.0;
	 b_ca = 0.0;
	 b_cb = 0.0;
	 b_cc = 0.0;
//张瑞雪添加于20120503
	 r = 0.0;
	 x = 0.0;
	 b = 0.0;

}

CLS_LNMODE::CLS_LNMODE(const char* m_id)
{
	 id = atoi(m_id);
	 descr[0] = 0;
	 type = 0;
	 r_aa = 0.0;
	 r_ab = 0.0;
	 r_ac = 0.0;
	 r_ba = 0.0;
	 r_bb = 0.0;
	 r_bc = 0.0;
	 r_ca = 0.0;
	 r_cb = 0.0;
	 r_cc = 0.0;
	 x_aa = 0.0;
	 x_ab = 0.0;
	 x_ac = 0.0;
	 x_ba = 0.0;
	 x_bb = 0.0;
	 x_bc = 0.0;
	 x_ca = 0.0;
	 x_cb = 0.0;
	 x_cc = 0.0;
	 b_aa = 0.0;
	 b_ab = 0.0;
	 b_ac = 0.0;
	 b_ba = 0.0;
	 b_bb = 0.0;
	 b_bc = 0.0;
	 b_ca = 0.0;
	 b_cb = 0.0;
	 b_cc = 0.0;
//张瑞雪添加于20120503
	 r = 0.0;
	 x = 0.0;
	 b = 0.0;
}

const CLS_LNMODE & CLS_LNMODE::operator = (const CLS_LNMODE& m_lnmode)
{
	id = m_lnmode.id;
	strcpy(descr,m_lnmode.descr);
	type = m_lnmode.type;
	r_aa = m_lnmode.r_aa;
	r_ab = m_lnmode.r_ab;
	r_ac = m_lnmode.r_ac;
	r_ba = m_lnmode.r_ba;
	r_bb = m_lnmode.r_bb;
	r_bc = m_lnmode.r_bc;
	r_ca = m_lnmode.r_ca;
	r_cb = m_lnmode.r_cb;
	r_cc = m_lnmode.r_cc;
	x_aa = m_lnmode.x_aa;
	x_ab = m_lnmode.x_ab;
	x_ac = m_lnmode.x_ac;
	x_ba = m_lnmode.x_ba;
	x_bb = m_lnmode.x_bb;
	x_bc = m_lnmode.x_bc;
	x_ca = m_lnmode.x_ca;
	x_cb = m_lnmode.x_cb;
	x_cc = m_lnmode.x_cc;
	b_aa = m_lnmode.b_aa;
	b_ab = m_lnmode.b_ab;
	b_ac = m_lnmode.b_ac;
	b_ba = m_lnmode.b_ba;
	b_bb = m_lnmode.b_bb;
	b_bc = m_lnmode.b_bc;
	b_ca = m_lnmode.b_ca;
	b_cb = m_lnmode.b_cb;
	b_cc = m_lnmode.b_cc;
	r = m_lnmode.r;
	x = m_lnmode.x;
	b = m_lnmode.b;
	return* this;
}
