// fdirdb.cpp: implementation of the fdirdb class.
//
//////////////////////////////////////////////////////////////////////

#include "fdirdb.h"

CLS_FDIR_FZONE::CLS_FDIR_FZONE()
{
    id = 0;
    descr[0] = 0;
    izone = 0;
}

const CLS_FDIR_FZONE& CLS_FDIR_FZONE::operator = (const CLS_FDIR_FZONE& aFDIR_FZONE)
{
    id = aFDIR_FZONE.id;
    strcpy(descr,aFDIR_FZONE.descr);
    izone = aFDIR_FZONE.izone;
    return *this;
}

CLS_FDIR_FZONE::CLS_FDIR_FZONE(const long m_id)
{
    id = m_id;
    descr[0] = 0;
    izone = 0;
}

CLS_FDIR_CB::CLS_FDIR_CB()
{
    id = 0;
	descr[0] = 0;
	cb_id=0;
    ind = 0;
    iznd = 0;
    q = 0;
	topflags = 0;
}

const CLS_FDIR_CB& CLS_FDIR_CB::operator = (const CLS_FDIR_CB& aCLS_FDIR_CB)
{
    id = aCLS_FDIR_CB.id;
	strcpy(descr,aCLS_FDIR_CB.descr);
    ind = aCLS_FDIR_CB.ind;
    iznd = aCLS_FDIR_CB.iznd;
    q = aCLS_FDIR_CB.q;
	topflags = aCLS_FDIR_CB.topflags;
	cb_id = aCLS_FDIR_CB.cb_id;
    return *this;
}

CLS_FDIR_CB::CLS_FDIR_CB(const long m_id)
{
    id = m_id;
	descr[0] = 0;
	cb_id=0;
    ind = 0;
    iznd = 0;
    q = 0;
	topflags = 0;
}

CLS_FDIR_ZONE::CLS_FDIR_ZONE()
{
    id = 0;
    descr[0] = 0;
    zn_id=0;
    type = 0;
    sub = 0;
	icb = -1;
    cbnum = 0;
    cbptr = 0;
    q = 0;
	topflags = 0;
}

const CLS_FDIR_ZONE& CLS_FDIR_ZONE::operator = (const CLS_FDIR_ZONE& aCLS_FDIR_ZONE)
{
    id = aCLS_FDIR_ZONE.id;
    strcpy(descr,aCLS_FDIR_ZONE.descr);
    type = aCLS_FDIR_ZONE.type;
    sub = aCLS_FDIR_ZONE.sub;
	icb = aCLS_FDIR_ZONE.icb;
    cbnum = aCLS_FDIR_ZONE.cbnum;
    cbptr = aCLS_FDIR_ZONE.cbptr;
    q = aCLS_FDIR_ZONE.q;
	topflags = aCLS_FDIR_ZONE.topflags;
	zn_id= aCLS_FDIR_ZONE.zn_id;
    return *this;
}

CLS_FDIR_ZONE::CLS_FDIR_ZONE(const long m_id)
{
    id = m_id;
    descr[0] = 0;
    zn_id=0;
    type = 0;
    sub = 0;
	icb = 0;
    cbnum = 0;
    cbptr = 0;
    q = 0;
	topflags = 0;
}

CLS_FDIR_ZONE_CB::CLS_FDIR_ZONE_CB()
{
    id = -1;
    icb = -1;
}

const CLS_FDIR_ZONE_CB& CLS_FDIR_ZONE_CB::operator = (const CLS_FDIR_ZONE_CB& aCLS_FDIR_ZONE_CB)
{
    id = aCLS_FDIR_ZONE_CB.id;
    icb = aCLS_FDIR_ZONE_CB.icb;
    return *this;
}

CLS_FDIR_ZONE_CB::CLS_FDIR_ZONE_CB(const long m_id)
{
    id = m_id;
    icb = 0;
}

/////////////////////////////////////////////////////////////////////////
// class FDIR_RESTORE implementation

CLS_FDIR_RESTORE::CLS_FDIR_RESTORE()
{
    id = 0;
    descr[0] = 0;
    so_id = 0;
    cb = 0;
    cb_desc[0] = 0;
    icb = 0;
    q = 0;
}

const CLS_FDIR_RESTORE& CLS_FDIR_RESTORE::operator = (const CLS_FDIR_RESTORE& aFDIR_RESTORE)
{
    id = aFDIR_RESTORE.id;
    strcpy(descr,aFDIR_RESTORE.descr);
    cb= aFDIR_RESTORE.cb;
    strcpy(cb_desc, aFDIR_RESTORE.cb_desc);
    icb = aFDIR_RESTORE.icb;
    q = aFDIR_RESTORE.q;
    return *this;
}

CLS_FDIR_RESTORE::CLS_FDIR_RESTORE(const int m_id)
{
    id = m_id;
    descr[0] = 0;
    icb = 0;
    q = 0;
    so_id = 0;
    cb = 0;
    cb_desc[0] = 0;
}

/////////////////////////////////////////////////////////////////////////
// class FDIR_ISOLATE implementation

CLS_FDIR_ISOLATE::CLS_FDIR_ISOLATE()
{
    id = 0;
    descr[0] = 0;
    icb = 0;
    cb = 0;
    cb_desc[0] = 0;
    q = 0;
}

const CLS_FDIR_ISOLATE& CLS_FDIR_ISOLATE::operator = (const CLS_FDIR_ISOLATE& aFDIR_ISOLATE)
{
    id = aFDIR_ISOLATE.id;
    strcpy(descr,aFDIR_ISOLATE.descr);
    cb = aFDIR_ISOLATE.cb;
    strcpy(cb_desc, aFDIR_ISOLATE.cb_desc);
    icb = aFDIR_ISOLATE.icb;
    q = aFDIR_ISOLATE.q;
    return *this;
}

CLS_FDIR_ISOLATE::CLS_FDIR_ISOLATE(const int m_id)
{
    id = m_id;
    descr[0] = 0;
    icb = 0;
    cb = 0;
    cb_desc[0] = 0;
    q = 0;
}
