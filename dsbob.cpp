//******************************************************************************
//
//  DSBOB.CPP
//  EMS Project - Copyright (c) 2000, EPRI
//  Methods implemention of distribution network database object
//
//******************************************************************************

#include "dsbob.h"
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  constructor
DSBOB::DSBOB()
{
	bs_tb           = new TTable<CLS_BS>(GetTableSize(g_db_dms->GetTB("DMS_BS")));
	bus_tb          = new TTable<CLS_BUS>(GetTableSize(g_db_dms->GetTB("DMS_BUS")));
	cb_tb           = new TTable<CLS_CB>(GetTableSize(g_db_dms->GetTB("DMS_CB")));
	switch_tb       = new TTable<CLS_SWITCH>(GetTableSize(g_db_dms->GetTB("DMS_SWITCH")));
	co_tb           = new TTable<CLS_CO>(GetTableSize(g_db_dms->GetTB("DMS_CO")));
	cp_tb           = new TTable<CLS_CP>(GetTableSize(g_db_dms->GetTB("DMS_CP")));
	global_tb       = new TTable<CLS_GLOBAL>(GetTableSize(g_db_dms->GetTB("DMS_GLOBAL")));
	island_tb       = new TTable<CLS_ISLAND>(GetTableSize(g_db_dms->GetTB("DMS_ISLAND")));
	ld_tb           = new TTable<CLS_LD>(GetTableSize(g_db_dms->GetTB("DMS_LD")));
	nd_tb           = new TTable<CLS_ND>(GetTableSize(g_db_dms->GetTB("DMS_ND")));
	r_co_st_tb      = new TTable<CLS_R_CO_ST>(GetTableSize(g_db_dms->GetTB("DMS_r_CO_ST")));
	r_nd_cb_tb      = new TTable<CLS_R_ND_CB>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_CB")));
	r_nd_switch_tb  = new TTable<CLS_R_ND_SWITCH>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_SWITCH")));
	r_nd_sec_tb     = new TTable<CLS_R_ND_SEC>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_SEC")));
	r_nd_xf_tb      = new TTable<CLS_R_ND_XF>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_XF")));
	r_zn_nd_tb      = new TTable<CLS_R_ZN_ND>(GetTableSize(g_db_dms->GetTB("DMS_r_ZN_ND")));
	r_st_bus_tb     = new TTable<CLS_R_ST_BUS>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_BUS")));
	r_st_cb_tb      = new TTable<CLS_R_ST_CB>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_CB")));
	r_st_switch_tb  = new TTable<CLS_R_ST_SWITCH>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_SWITCH")));
	r_st_cp_tb      = new TTable<CLS_R_ST_CP>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_CP")));
	r_st_ld_tb      = new TTable<CLS_R_ST_LD>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_LD")));
	r_st_so_tb      = new TTable<CLS_R_ST_SO>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_SO")));
	r_st_xfmr_tb    = new TTable<CLS_R_ST_XFMR>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_XFMR")));
	r_st_zn_tb      = new TTable<CLS_R_ST_ZN>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_ZN")));
	r_xfmr_xf_tb    = new TTable<CLS_R_XFMR_XF>(GetTableSize(g_db_dms->GetTB("DMS_r_XFMR_XF")));
	r_zn_ld_tb      = new TTable<CLS_R_ZN_LD>(GetTableSize(g_db_dms->GetTB("DMS_r_ZN_LD")));
	r_zn_sec_tb     = new TTable<CLS_R_ZN_SEC>(GetTableSize(g_db_dms->GetTB("DMS_r_ZN_SEC")));
	sec_tb          = new TTable<CLS_SEC>(GetTableSize(g_db_dms->GetTB("DMS_SEC")));
	so_tb           = new TTable<CLS_SO>(GetTableSize(g_db_dms->GetTB("DMS_SO")));
	st_tb           = new TTable<CLS_ST>(GetTableSize(g_db_dms->GetTB("DMS_ST")));
	tapty_tb        = new TTable<CLS_TAPTY>(GetTableSize(g_db_dms->GetTB("DMS_TAPTY")));
	xf_tb           = new TTable<CLS_XF>(GetTableSize(g_db_dms->GetTB("DMS_XF")));
	xfmr_tb         = new TTable<CLS_XFMR>(GetTableSize(g_db_dms->GetTB("DMS_XFMR")));
	zn_tb           = new TTable<CLS_ZN>(GetTableSize(g_db_dms->GetTB("DMS_ZN")));
	fi_tb			= new TTable<CLS_FI>(GetTableSize(g_db_dms->GetTB("DMS_FI")));
	r_sec_fi_tb		= new TTable<CLS_R_SEC_FI>(GetTableSize(g_db_dms->GetTB("DMS_R_SEC_FI")));
//	ctl_tb			= new TTable<CLS_CTL>(GetTableSize(g_db_dms->GetTB("DMS_CTL")));
//	lnmode_tb		= new TTable<CLS_LNMODE>(GetTableSize(g_db_dms->GetTB("DMS_LNMODE")));
	r_nd_cp_tb		= new TTable<CLS_R_ND_CP>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_CP")));
	r_nd_ld_tb		= new TTable<CLS_R_ND_LD>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_LD")));
}

////////////////////////////////////////////////////////////////////////////////
//  destructor
DSBOB::~DSBOB()
{
//	delete analog_tb;
	delete bs_tb;
	delete bus_tb;
	delete cb_tb;
	delete co_tb;
	delete cp_tb;
//	delete digital_tb;
	delete global_tb;
	delete island_tb;
	delete ld_tb;
	delete nd_tb;
	delete r_co_st_tb;
	delete r_nd_cb_tb;
	delete r_nd_sec_tb;
	delete r_nd_xf_tb;
	delete r_zn_nd_tb;
	delete r_st_bus_tb;
	delete r_st_cb_tb;
	delete r_st_cp_tb;
	delete r_st_ld_tb;
	delete r_st_so_tb;
	delete r_st_xfmr_tb;
	delete r_st_zn_tb;
	delete r_xfmr_xf_tb;
	delete r_zn_ld_tb;
	delete r_zn_sec_tb;
	delete sec_tb;
	delete so_tb;
	delete st_tb;
	delete tapty_tb;
	delete xf_tb;
	delete xfmr_tb;
	delete zn_tb;
//	delete ctl_tb;
	delete fi_tb;
	delete r_sec_fi_tb;
	delete r_nd_cp_tb;
	delete r_nd_ld_tb;
	delete switch_tb;
	delete r_st_switch_tb;
	delete r_nd_switch_tb;

//	delete lnmode_tb;
}

void DSBOB::ReadFromDsbob2(DSBOB2 *dsbob2)
{
	assert(dsbob2 != NULL);

	//analog_tb->Insert(dsbob2->analog_tb->GetTable(), dsbob2->analog_tb->GetCount());
	switch_tb->Insert(dsbob2->switch_tb->GetTable(), dsbob2->switch_tb->GetCount());
	r_nd_switch_tb->Insert(dsbob2->r_nd_switch_tb->GetTable(), dsbob2->r_nd_switch_tb->GetCount());
	r_st_switch_tb->Insert(dsbob2->r_st_switch_tb->GetTable(), dsbob2->r_st_switch_tb->GetCount());

	bs_tb->Insert(dsbob2->bs_tb->GetTable(), dsbob2->bs_tb->GetCount());

	bus_tb->Insert(dsbob2->bus_tb->GetTable(), dsbob2->bus_tb->GetCount());

	cb_tb->Insert(dsbob2->cb_tb->GetTable(), dsbob2->cb_tb->GetCount());

	co_tb->Insert(dsbob2->co_tb->GetTable(), dsbob2->co_tb->GetCount());

	cp_tb->Insert(dsbob2->cp_tb->GetTable(), dsbob2->cp_tb->GetCount());

	//digital_tb->Insert(dsbob2->digital_tb->GetTable(), dsbob2->digital_tb->GetCount());

	global_tb->Insert(dsbob2->global_tb->GetTable(), dsbob2->global_tb->GetCount());

	island_tb->Insert(dsbob2->island_tb->GetTable(), dsbob2->island_tb->GetCount());

	ld_tb->Insert(dsbob2->ld_tb->GetTable(), dsbob2->ld_tb->GetCount());

	nd_tb->Insert(dsbob2->nd_tb->GetTable(), dsbob2->nd_tb->GetCount());

	r_co_st_tb->Insert(dsbob2->r_co_st_tb->GetTable(), dsbob2->r_co_st_tb->GetCount());

	r_nd_cb_tb->Insert(dsbob2->r_nd_cb_tb->GetTable(), dsbob2->r_nd_cb_tb->GetCount());

	r_nd_sec_tb->Insert(dsbob2->r_nd_sec_tb->GetTable(), dsbob2->r_nd_sec_tb->GetCount());

	r_nd_xf_tb->Insert(dsbob2->r_nd_xf_tb->GetTable(), dsbob2->r_nd_xf_tb->GetCount());

	r_zn_nd_tb->Insert(dsbob2->r_zn_nd_tb->GetTable(), dsbob2->r_zn_nd_tb->GetCount());

	r_st_bus_tb->Insert(dsbob2->r_st_bus_tb->GetTable(), dsbob2->r_st_bus_tb->GetCount());

	r_st_cb_tb->Insert(dsbob2->r_st_cb_tb->GetTable(), dsbob2->r_st_cb_tb->GetCount());

	r_st_cp_tb->Insert(dsbob2->r_st_cp_tb->GetTable(), dsbob2->r_st_cp_tb->GetCount());

	r_st_ld_tb->Insert(dsbob2->r_st_ld_tb->GetTable(), dsbob2->r_st_ld_tb->GetCount());

	r_st_so_tb->Insert(dsbob2->r_st_so_tb->GetTable(), dsbob2->r_st_so_tb->GetCount());

	r_st_xfmr_tb->Insert(dsbob2->r_st_xfmr_tb->GetTable(), dsbob2->r_st_xfmr_tb->GetCount());

	r_st_zn_tb->Insert(dsbob2->r_st_zn_tb->GetTable(), dsbob2->r_st_zn_tb->GetCount());

	r_xfmr_xf_tb->Insert(dsbob2->r_xfmr_xf_tb->GetTable(), dsbob2->r_xfmr_xf_tb->GetCount());

	r_zn_ld_tb->Insert(dsbob2->r_zn_ld_tb->GetTable(), dsbob2->r_zn_ld_tb->GetCount());

	r_zn_sec_tb->Insert(dsbob2->r_zn_sec_tb->GetTable(), dsbob2->r_zn_sec_tb->GetCount());

	sec_tb->Insert(dsbob2->sec_tb->GetTable(), dsbob2->sec_tb->GetCount());

	so_tb->Insert(dsbob2->so_tb->GetTable(), dsbob2->so_tb->GetCount());

	st_tb->Insert(dsbob2->st_tb->GetTable(), dsbob2->st_tb->GetCount());

	tapty_tb->Insert(dsbob2->tapty_tb->GetTable(), dsbob2->tapty_tb->GetCount());

	xf_tb->Insert(dsbob2->xf_tb->GetTable(), dsbob2->xf_tb->GetCount());

	xfmr_tb->Insert(dsbob2->xfmr_tb->GetTable(), dsbob2->xfmr_tb->GetCount());

	zn_tb->Insert(dsbob2->zn_tb->GetTable(), dsbob2->zn_tb->GetCount());

//	ctl_tb->Insert(dsbob2->ctl_tb->GetTable(), dsbob2->ctl_tb->GetCount());

	fi_tb->Insert(dsbob2->fi_tb->GetTable(), dsbob2->fi_tb->GetCount());

	r_sec_fi_tb->Insert(dsbob2->r_sec_fi_tb->GetTable(), dsbob2->r_sec_fi_tb->GetCount());

//	lnmode_tb->Insert(dsbob2->lnmode_tb->GetTable(), dsbob2->lnmode_tb->GetCount());

	r_nd_cp_tb->Insert(dsbob2->r_nd_cp_tb->GetTable(), dsbob2->r_nd_cp_tb->GetCount());

	r_nd_ld_tb->Insert(dsbob2->r_nd_ld_tb->GetTable(), dsbob2->r_nd_ld_tb->GetCount());
	m_map_nd_ld = dsbob2->m_map_nd_ld;
}

#if 0
////////////////////////////////////////////////////////////////////////////////
//  Serialize()
void DSBOB::Serialize(CArchive& ar)
{
	analog_tb->Serialize(ar);
	bs_tb->Serialize(ar);
	bus_tb->Serialize(ar);
	cb_tb->Serialize(ar);
	co_tb->Serialize(ar);
	cp_tb->Serialize(ar);
	digital_tb->Serialize(ar);
	global_tb->Serialize(ar);
	island_tb->Serialize(ar);
	ld_tb->Serialize(ar);
	nd_tb->Serialize(ar);
	r_co_st_tb->Serialize(ar);
	r_nd_cb_tb->Serialize(ar);
	r_nd_sec_tb->Serialize(ar);
	r_nd_xf_tb->Serialize(ar);
	r_zn_nd_tb->Serialize(ar);
	r_st_bus_tb->Serialize(ar);
	r_st_cb_tb->Serialize(ar);
	r_st_cp_tb->Serialize(ar);
	r_st_ld_tb->Serialize(ar);
	r_st_so_tb->Serialize(ar);
	r_st_xfmr_tb->Serialize(ar);
	r_st_zn_tb->Serialize(ar);
	r_xfmr_xf_tb->Serialize(ar);
	r_zn_ld_tb->Serialize(ar);
	r_zn_sec_tb->Serialize(ar);
	sec_tb->Serialize(ar);
	so_tb->Serialize(ar);
	st_tb->Serialize(ar);
	tapty_tb->Serialize(ar);
	xf_tb->Serialize(ar);
	xfmr_tb->Serialize(ar);
	zn_tb->Serialize(ar);
}

////////////////////////////////////////////////////////////////////////////////
//	read object from binary file
int DSBOB::ReadObject()
{
	CFile		f;

	if( !f.Open(strFileName, CFile::modeRead) )
	{
		return FALSE;
	}

	CArchive ar(&f, CArchive::load);

	Serialize(ar);

	ar.Close();
	f.Close();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	write object to binary file
int DSBOB::WriteObject()
{
	CFile		f;

	if( !f.Open(strFileName, CFile::modeCreate | CFile::modeWrite) )
	{
		return FALSE;
	}

	CArchive ar(&f, CArchive::store);

	Serialize(ar);

	ar.Close();
	f.Close();

	return TRUE;
}
#endif
////////////////////////////////////////////////////////////////////////////////
// read database from sql server
bool DSBOB::ReadReDB(const char* dsn,const char* user,const char* password)
{

	printf("%s ……\r\n", __FUNCTION__);

	int pos = -1;
	int cnt_record, size;
	// d5000 不需要这个表？
/*	cout << "//1 - read analog table" << endl;
	CLS_ANALOG* analog = new CLS_ANALOG;
	DMS::ANALOG* analog_set = new DMS::ANALOG;
	int cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")));
	int size = sizeof(DMS::ANALOG);

	analog_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(analog_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), 1, size) <= 0)
		{
			delete analog;
			delete analog_set;

			return false;
		}

		analog->id = analog_set->id;
		strcpy(analog->descr, analog_set->descr);
		analog->value = analog_set->value;
		analog->quality = analog_set->quality;
		analog_tb->Insert(*analog);
	}

	delete analog;
	delete analog_set;*/

	cout << "//2 - read bs table" << endl;
	CLS_BS* bs = new CLS_BS;
	DMS::BS* bs_set = new DMS::BS;
	size = sizeof(DMS::BS);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("BS")));
	bs_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(bs_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("BS")), 1, size) <= 0)
		{
			delete bs;
			delete bs_set;

			return false;
		}

		bs->id = bs_set->id;
		bs->iisland = bs_set->iisland;
		bs->q = bs_set->q;
		bs->qq = bs_set->qq;
		bs->fbs = bs_set->fbs;
		bs->bbs = bs_set->bbs;
		bs->pnd = bs_set->pnd;
		bs->kv = bs_set->kv;
		bs->v = bs_set->v;
		bs->adeg = bs_set->adeg;
		bs->w = bs_set->w;
		bs->r = bs_set->r;
		bs->valid = bs_set->valid;
//张瑞雪添加于20120210
		bs->kv_b = bs_set->kv_b;
		bs->v_b = bs_set->v_b;
		bs->adeg_b = bs_set->adeg_b;
		bs->w_b = bs_set->w_b;
		bs->r_b = bs_set->r_b;
		bs->kv_c = bs_set->kv_c;
		bs->v_c = bs_set->v_c;
		bs->adeg_c = bs_set->adeg_c;
		bs->w_c = bs_set->w_c;
		bs->r_c = bs_set->r_c;
		//张瑞雪添加于20120503
		bs->kv_a = bs_set->kv_a;
		bs->v_a = bs_set->v_a;
		bs->adeg_a = bs_set->adeg_a;
		bs->w_a = bs_set->w_a;
		bs->r_a = bs_set->r_a;
		bs_tb->Insert(*bs);

	}

	delete bs;
	delete bs_set;

	cout << "//3 - read bus table" << endl;
	CLS_BUS* bus = new CLS_BUS;
	DMS::BUS* bus_set = new DMS::BUS;
	size = sizeof(DMS::BUS);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("BUS")));
	bus_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(bus_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("BUS")), 1, size) <= 0)
		{
			delete bus;
			delete bus_set;

			return false;
		}

		bus->id = bus_set->id;
		strcpy(bus->descr, bus_set->descr);
		bus->nd = bus_set->nd;
		bus->stid = bus_set->stid;
		bus->ind = bus_set->ind;
		bus->ibs = bus_set->ibs;
		bus->algv = bus_set->algv;
		bus->ialgv = bus_set->ialgv;
		bus->topflags = bus_set->topflags;
		bus->vtarget = bus_set->vtarget;
		bus->vl = bus_set->vl;
		bus->vhlim = bus_set->vhlim;
		bus->vllim = bus_set->vllim;
		bus->v = bus_set->v;
		bus->adeg = bus_set->adeg;
		bus->r_st = bus_set->r_st;
//张瑞雪添加于20120210
		bus->algv_b = bus_set->algv_b;
		bus->ialgv_b = bus_set->ialgv_b;
		bus->vtarget_b = bus_set->vtarget_b;
		bus->vhlim_b = bus_set->vhlim_b;
		bus->vllim_b = bus_set->vllim_b;
		bus->v_b = bus_set->v_b;
		bus->adeg_b = bus_set->adeg_b;
		bus->algv_c = bus_set->algv_c;
		bus->ialgv_c = bus_set->ialgv_c;
		bus->vtarget_c = bus_set->vtarget_c;
		bus->vhlim_c = bus_set->vhlim_c;
		bus->vllim_c = bus_set->vllim_c;
		bus->v_c = bus_set->v_c;
		bus->adeg_c = bus_set->adeg_c;
		//郝泽赟添加于20120426
		bus->algp = bus_set->algp;
		bus->algp_b = bus_set->algp_b;
		bus->algp_c = bus_set->algp_c;
		bus->ialgp = bus_set->ialgp;
		bus->ialgp_b = bus_set->ialgp_b;
		bus->ialgp_c = bus_set->ialgp_c;
		bus->alga = bus_set->alga;
		bus->alga_b = bus_set->alga_b;
		bus->alga_c = bus_set->alga_c;
		bus->ialga = bus_set->ialga;
		bus->ialga_b = bus_set->ialga_b;
		bus->ialga_c = bus_set->ialga_c;
		//张瑞雪添加于20120503
		bus->algp_a = bus_set->algp_a;
		bus->ialgp_a = bus_set->ialgp_a;
		bus->alga_a = bus_set->alga_a;
		bus->ialga_a = bus_set->ialga_a;
		bus->v_a = bus_set->v_a;
		bus->adeg_a = bus_set->adeg_a;
		bus->algv_a = bus_set->algv_a;
		bus->ialgv_a = bus_set->ialgv_a;
		bus->wmx = bus_set->wmx;
		bus_tb->Insert(*bus);

	}

	delete bus;
	delete bus_set;

	cout << "//4 - read cb table" << endl;
	CLS_CB* cb = new CLS_CB;
	DMS::CB* cb_set = new DMS::CB;
	size = sizeof(DMS::CB);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("CB")));
	cb_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(cb_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("CB")), 1, size) <= 0)
		{
			delete cb;
			delete cb_set;

			return false;
		}

		cb->id = cb_set->id;
		strcpy(cb->descr, cb_set->descr);
		cb->nd = cb_set->nd;
		cb->znd = cb_set->znd;
		cb->ind = cb_set->ind;
		cb->iznd = cb_set->iznd;
//		cb->digital = cb_set->digital;
//		cb->idigital = cb_set->idigital;
		cb->q = cb_set->q;
		cb->topflags = cb_set->topflags;
		cb->r_st = cb_set->r_st;
		cb->stid = cb_set->stid;
		cb_tb->Insert(*cb);

	}

	delete cb;
	delete cb_set;

	cout << "//5 - read co table" << endl;
	CLS_CO* co = new CLS_CO;
	DMS::CO* co_set = new DMS::CO;
	size = sizeof(DMS::CO);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("CO")));
	co_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(co_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("CO")), 1, size) <= 0)
		{
			delete co;
			delete co_set;

			return false;
		}

		co->id = co_set->id;
		strcpy(co->descr, co_set->descr);
		co->wloss = co_set->wloss;
		co->rloss = co_set->rloss;
		co->wso = co_set->wso;
		co->rso = co_set->rso;
		co->wload = co_set->wload;
		co->rload = co_set->rload;
		co->cap = co_set->cap;
		co->reac = co_set->reac;
		co->r_st = co_set->r_st;
//张瑞雪添加于20120210
		co->wloss_b = co_set->wloss_b;
		co->rloss_b = co_set->rloss_b;
		co->wso_b = co_set->wso_b;
		co->rso_b = co_set->rso_b;
		co->wload_b = co_set->wload_b;
		co->rload_b= co_set->rload_b;
		co->cap_b = co_set->cap_b;
		co->reac_b = co_set->reac_b;
		co->wloss_c = co_set->wloss_c;
		co->rloss_c = co_set->rloss_c;
		co->wso_c = co_set->wso_c;
		co->rso_c = co_set->rso_c;
		co->wload_c = co_set->wload_c;
		co->rload_c = co_set->rload_c;
		co->cap_c = co_set->cap_c;
		co->reac_c = co_set->reac_c;
		//张瑞雪添加于20120503
		co->wloss_a = co_set->wloss_a;
		co->rloss_a = co_set->rloss_a;
		co->wso_a = co_set->wso_a;
		co->rso_a = co_set->rso_a;
		co->wload_a = co_set->wload_a;
		co->rload_a= co_set->rload_a;
		co->cap_a = co_set->cap_a;
		co->reac_a = co_set->reac_a;
		co_tb->Insert(*co);

	}

	delete co;
	delete co_set;

	cout << "//6 - read cp table" << endl;
	CLS_CP* cp = new CLS_CP;
	DMS::CP* cp_set = new DMS::CP;
	size = sizeof(DMS::CP);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("CP")));
	cp_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(cp_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("CP")), 1, size) <= 0)
		{
			delete cp;
			delete cp_set;

			return false;
		}

		cp->id = cp_set->id;
		strcpy(cp->descr, cp_set->descr);
		cp->nd = cp_set->nd;
		cp->stid = cp_set->stid;
		cp->ind = cp_set->ind;
		cp->ibs = cp_set->ibs;
		cp->algq = cp_set->algq;
		cp->ialgq = cp_set->ialgq;
		cp->q = cp_set->q;
		cp->topflags = cp_set->topflags;
		cp->mrnom = cp_set->mrnom;
		cp->r = cp_set->r;
		cp->r_st = cp_set->r_st;
//张瑞雪添加于20120210
		cp->algq_b = cp_set->algq_b;
		cp->ialgq_b = cp_set->ialgq_b;
		cp->mrnom_b = cp_set->mrnom_b;
		cp->r_b = cp_set->r_b;
		cp->algq_c = cp_set->algq_c;
		cp->ialgq_c = cp_set->ialgq_c;
		cp->mrnom_c = cp_set->mrnom_c;
		cp->r_c = cp_set->r_c;
		//张瑞雪添加于20120503
		cp->algq_a = cp_set->algq_a;
		cp->ialgq_a = cp_set->ialgq_a;
		cp->r_a = cp_set->r_a;
		cp_tb->Insert(*cp);

	}

	delete cp;
	delete cp_set;

	/*
	cout << "//7 - read digital table" << endl;
	CLS_DIGITAL* digital = new CLS_DIGITAL;
	DMS::DIGITAL* digital_set = new DMS::DIGITAL;
	size = sizeof(DMS::DIGITAL);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("DIGITAL")));
	digital_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(digital_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("DIGITAL")), 1, size) <= 0)
		{
			delete digital;
			delete digital_set;

			return false;
		}

		digital->id = digital_set->id;
		strcpy(digital->descr, digital_set->descr);
		digital->value = digital_set->value;
		digital->quality = digital_set->quality;
		digital->icb = digital_set->icb;
		digital->cb = digital_set->cb;

		digital_tb->Insert(*digital);

	}

	delete digital;
	delete digital_set;*/

	cout << "//8 - read global table" << endl;
	CLS_GLOBAL* global = new CLS_GLOBAL;
	DMS::GLOBAL* global_set = new DMS::GLOBAL;
	size = sizeof(DMS::GLOBAL);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("GLOBAL")));
	global_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(global_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("GLOBAL")), 1, size) <= 0)
		{
			delete global;
			delete global_set;

			return false;
		}

		global->id = global_set->id;
		global->nullbs = global_set->nullbs;
		global->nextbs = global_set->nextbs;
		global->tptime = global_set->tptime;
		global->timedb = global_set->timedb;
		global->statusdb = global_set->statusdb;
		global->error = global_set->error;
		global->dberrnum = global_set->dberrnum;
		global_tb->Insert(*global);

	}

	delete global;
	delete global_set;

	cout << "//9 - read island table" << endl;
	CLS_ISLAND* island = new CLS_ISLAND;
	DMS::ISLAND* island_set = new DMS::ISLAND;
	size = sizeof(DMS::ISLAND);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("ISLAND")));
	island_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(island_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("ISLAND")), 1, size) <= 0)
		{
			delete island;
			delete island_set;

			return false;
		}

		island->id = island_set->id;
		island->refbs = island_set->refbs;
		island->refso = island_set->refso;
		island->vref = island_set->vref;
		island->wload = island_set->wload;
		island->rload = island_set->rload;
		island->wso = island_set->wso;
		island->rso = island_set->rso;
		island->valid = island_set->valid;
//张瑞雪添加于20120212
		island->vref_b = island_set->vref_b;
		island->wload_b = island_set->wload_b;
		island->rload_b = island_set->rload_b;
		island->wso_b = island_set->wso_b;
		island->rso_b = island_set->rso_b;
		island->vref_c = island_set->vref_c;
		island->wload_c = island_set->wload_c;
		island->rload_c = island_set->rload_c;
		island->wso_c = island_set->wso_c;
		island->rso_c = island_set->rso_c;
		//张瑞雪添加于20120503
		island->wload_a = island_set->wload_a;
		island->rload_a = island_set->rload_a;
		island->wso_a = island_set->wso_a;
		island->rso_a = island_set->rso_a;
		island_tb->Insert(*island);

	}

	delete island;
	delete island_set;

	cout << "//10 - read ld table" << endl;
	CLS_LD* ld = new CLS_LD;
	DMS::LD* ld_set = new DMS::LD;
	size = sizeof(DMS::LD);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("LD")));
	ld_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(ld_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("LD")), 1, size) <= 0)
		{
			delete ld;
			delete ld_set;

			return false;
		}

		ld->zn_id = ld_set->zn_id;
		ld->id = ld_set->id;
		strcpy(ld->descr, ld_set->descr);
		ld->type = ld_set->type;
		ld->nd = ld_set->nd;
		ld->ind = ld_set->ind;
		ld->ibs = ld_set->ibs;
		ld->algp = ld_set->algp;
		ld->ialgp = ld_set->ialgp;
		ld->algq = ld_set->algq;
		ld->ialgq = ld_set->ialgq;
		ld->algi = ld_set->algi;
		ld->ialgi = ld_set->ialgi;
		ld->q = ld_set->q;
		ld->topflags = ld_set->topflags;
		ld->w = ld_set->w;
		ld->r = ld_set->r;
		ld->wm = ld_set->wm;
		ld->rm = ld_set->rm;
//张瑞雪添加于20120212
		ld->ldtype = ld_set->ldtype;
		ld->i = ld_set->i;
		ld->a = ld_set->a;
		ld->zr = ld_set->zr;
		ld->zx = ld_set->zx;
		ld->i_b = ld_set->i_b;
		ld->a_b = ld_set->a_b;
		ld->zr_b = ld_set->zr_b;
		ld->zx_b = ld_set->zx_b;
		ld->algia = ld_set->algia;
		ld->ialgia = ld_set->ialgia;
		ld->algp_b = ld_set->algp_b;
		ld->ialgp_b = ld_set->ialgp_b;
		ld->algq_b = ld_set->algq_b;
		ld->ialgq_b = ld_set->ialgq_b;
		ld->algi_b = ld_set->algi_b;
		ld->ialgi_b = ld_set->ialgi_b;
		ld->algia_b = ld_set->algia_b;
		ld->ialgia_b = ld_set->ialgia_b;
		ld->w_b = ld_set->w_b;
		ld->r_b = ld_set->r_b;
		ld->wm_b = ld_set->wm_b;
		ld->rm_b = ld_set->rm_b;
		ld->i_c = ld_set->i_c;
		ld->a_c = ld_set->a_c;
		ld->zr_c = ld_set->zr_c;
		ld->zx_c = ld_set->zx_c;
		ld->algp_c = ld_set->algp_c;
		ld->ialgp_c = ld_set->ialgp_c;
		ld->algq_c = ld_set->algq_c;
		ld->ialgq_c = ld_set->ialgq_c;
		ld->algi_c = ld_set->algi_c;
		ld->ialgi_c = ld_set->ialgi_c;
		ld->algia_c = ld_set->algia_c;
		ld->ialgia_c = ld_set->ialgia_c;
		ld->w_c = ld_set->w_c;
		ld->r_c = ld_set->r_c;
		ld->wm_c = ld_set->wm_c;
		ld->rm_c = ld_set->rm_c;
		ld->r_st = ld_set->r_st;
		ld->r_zn = ld_set->r_zn;
		ld->stid = ld_set->stid;
		//张瑞雪添加于20120503
		ld->algp_a = ld_set->algp_a;
		ld->ialgp_a = ld_set->ialgp_a;
		ld->algq_a = ld_set->algq_a;
		ld->ialgq_a = ld_set->ialgq_a;
		ld->algi_a = ld_set->algi_a;
		ld->ialgi_a = ld_set->ialgi_a;
		ld->algia_a = ld_set->algia_a;
		ld->ialgia_a = ld_set->ialgia_a;
		ld->w_a = ld_set->w_a;
		ld->r_a = ld_set->r_a;
		ld->zr_a = ld_set->zr_a;
		ld->zx_a = ld_set->zx_a;
		ld->i_a = ld_set->i_a;
		ld->a_ = ld_set->a_;
		ld_tb->Insert(*ld);

		//2012.5.24荣义平
		//将负荷与节点形成映射关系，方便后面根据节点快速查找到对应的负荷
		m_map_nd_ld[ld->nd].push_back(*ld);
	}

	delete ld;
	delete ld_set;

	cout<<"ld_tb count:"<<ld_tb->GetCount()<<endl;
	cout << "//12 - read nd table" << endl;
	CLS_ND* nd = new CLS_ND;
	DMS::ND* nd_set = new DMS::ND;
	size = sizeof(DMS::ND);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("ND")));
	nd_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(nd_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("ND")), 1, size) <= 0)
		{
			delete nd;
			delete nd_set;

			return false;
		}

		nd->id = nd_set->id;
		nd->vl = nd_set->vl;
		nd->q = nd_set->q;
		nd->ibs = nd_set->ibs;
		nd->primebs = nd_set->primebs;
		nd->fch = nd_set->fch;
		nd->bch = nd_set->bch;
		nd->r_cb = nd_set->r_cb;
		nd->r_switch = nd_set->r_switch;
		nd->zn_id = nd_set->zn_id;
		nd->r_xf = nd_set->r_xf;
		nd->r_sec = nd_set->r_sec;
		nd->r_cp = nd_set->r_cp;
		nd->r_ld = nd_set->r_ld;
		nd_tb->Insert(*nd);

	}

	delete nd;
	delete nd_set;

	cout << "//13 - read r_co_st table" << endl;
	CLS_R_CO_ST* r_co_st = new CLS_R_CO_ST;
	DMS::R_CO_ST* r_co_st_set = new DMS::R_CO_ST;
	size = sizeof(DMS::R_CO_ST);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_CO_ST")));
	r_co_st_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_co_st_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_CO_ST")), 1, size) <= 0)
		{
			delete r_co_st;
			delete r_co_st_set;

			return false;
		}

		r_co_st->co_id = r_co_st_set->CO_id;
		r_co_st->st_id = r_co_st_set->ST_id;
		r_co_st->co_id_ptr = r_co_st_set->CO_id_ptr;
		r_co_st->st_id_ptr = r_co_st_set->ST_id_ptr;
		r_co_st->fptr = r_co_st_set->fptr;
		r_co_st->bptr = r_co_st_set->bptr;
		r_co_st_tb->Insert(*r_co_st);

	}

	delete r_co_st;
	delete r_co_st_set;

	cout << "//14 - read r_nd_cb table" << endl;
	CLS_R_ND_CB* r_nd_cb = new CLS_R_ND_CB;
	DMS::R_ND_CB* r_nd_cb_set = new DMS::R_ND_CB;
	size = sizeof(DMS::R_ND_CB);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_CB")));
	r_nd_cb_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_nd_cb_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_CB")), 1, size) <= 0)
		{
			delete r_nd_cb;
			delete r_nd_cb_set;

			return false;
		}

		r_nd_cb->nd_id = r_nd_cb_set->ND_id;
		r_nd_cb->cb_id = r_nd_cb_set->CB_id;
		r_nd_cb->nd_id_ptr = r_nd_cb_set->ND_id_ptr;
		r_nd_cb->cb_id_ptr = r_nd_cb_set->CB_id_ptr;
		r_nd_cb->fptr = r_nd_cb_set->fptr;
		r_nd_cb->bptr = r_nd_cb_set->bptr;
		r_nd_cb_tb->Insert(*r_nd_cb);

	}

	delete r_nd_cb;
	delete r_nd_cb_set;

	cout << "//15 - read r_nd_sec table" << endl;
	CLS_R_ND_SEC* r_nd_sec = new CLS_R_ND_SEC;
	DMS::R_ND_SEC* r_nd_sec_set = new DMS::R_ND_SEC;
	size = sizeof(DMS::R_ND_SEC);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_SEC")));
	r_nd_sec_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_nd_sec_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_SEC")), 1, size) <= 0)
		{
			delete r_nd_sec;
			delete r_nd_sec_set;

			return false;
		}

		r_nd_sec->nd_id = r_nd_sec_set->ND_id;
		r_nd_sec->sec_id = r_nd_sec_set->SEC_id;
		r_nd_sec->nd_id_ptr = r_nd_sec_set->ND_id_ptr;
		r_nd_sec->sec_id_ptr = r_nd_sec_set->SEC_id_ptr;
		r_nd_sec->fptr = r_nd_sec_set->fptr;
		r_nd_sec->bptr = r_nd_sec_set->bptr;
		r_nd_sec_tb->Insert(*r_nd_sec);

	}

	delete r_nd_sec;
	delete r_nd_sec_set;

	cout << "//16 - read r_nd_xf table" << endl;
	CLS_R_ND_XF* r_nd_xf = new CLS_R_ND_XF;
	DMS::R_ND_XF* r_nd_xf_set = new DMS::R_ND_XF;
	size = sizeof(DMS::R_ND_XF);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_XF")));
	r_nd_xf_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_nd_xf_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_XF")), 1, size) <= 0)
		{
			delete r_nd_xf;
			delete r_nd_xf_set;

			return false;
		}

		r_nd_xf->nd_id = r_nd_xf_set->ND_id;
		r_nd_xf->xf_id = r_nd_xf_set->XF_id;
		r_nd_xf->nd_id_ptr = r_nd_xf_set->ND_id_ptr;
		r_nd_xf->xf_id_ptr = r_nd_xf_set->XF_id_ptr;
		r_nd_xf->fptr = r_nd_xf_set->fptr;
		r_nd_xf->bptr = r_nd_xf_set->bptr;
		r_nd_xf_tb->Insert(*r_nd_xf);

	}

	delete r_nd_xf;
	delete r_nd_xf_set;

	cout << "//17 - read r_zn_nd table" << endl;
	CLS_R_ZN_ND* r_zn_nd = new CLS_R_ZN_ND;
	DMS::R_ZN_ND* r_zn_nd_set = new DMS::R_ZN_ND;
	size = sizeof(DMS::R_ZN_ND);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_ND")));
	r_zn_nd_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_zn_nd_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_ND")), 1, size) <= 0)
		{
			delete r_zn_nd;
			delete r_zn_nd_set;

			return false;
		}

		r_zn_nd->nd_id = r_zn_nd_set->ND_id;
		r_zn_nd->zn_id = r_zn_nd_set->ZN_id;
		r_zn_nd->nd_id_ptr = r_zn_nd_set->ND_id_ptr;
		r_zn_nd->zn_id_ptr = r_zn_nd_set->ZN_id_ptr;
		r_zn_nd->fptr = r_zn_nd_set->fptr;
		r_zn_nd->bptr = r_zn_nd_set->bptr;
		r_zn_nd_tb->Insert(*r_zn_nd);

	}

	delete r_zn_nd;
	delete r_zn_nd_set;

	cout << "//15 - read r_nd_cp table" << endl;
	CLS_R_ND_CP* r_nd_cp = new CLS_R_ND_CP;
	DMS::R_ND_CP* r_nd_cp_set = new DMS::R_ND_CP;
	size = sizeof(DMS::R_ND_CP);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_CP")));
	r_nd_cp_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_nd_cp_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_CP")), 1, size) <= 0)
		{
			delete r_nd_cp;
			delete r_nd_cp_set;

			return false;
		}

		r_nd_cp->nd_id = r_nd_cp_set->ND_id;
		r_nd_cp->cp_id = r_nd_cp_set->CP_id;
		r_nd_cp->nd_id_ptr = r_nd_cp_set->ND_id_ptr;
		r_nd_cp->cp_id_ptr = r_nd_cp_set->CP_id_ptr;
		r_nd_cp->fptr = r_nd_cp_set->fptr;
		r_nd_cp->bptr = r_nd_cp_set->bptr;
		r_nd_cp_tb->Insert(*r_nd_cp);

	}

	delete r_nd_cp;
	delete r_nd_cp_set;

	cout << "//15 - read r_nd_ld table" << endl;
	CLS_R_ND_LD* r_nd_ld = new CLS_R_ND_LD;
	DMS::R_ND_LD* r_nd_ld_set = new DMS::R_ND_LD;
	size = sizeof(DMS::R_ND_LD);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_LD")));
	r_nd_ld_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_nd_ld_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_LD")), 1, size) <= 0)
		{
			delete r_nd_ld;
			delete r_nd_ld_set;

			return false;
		}

		r_nd_ld->nd_id = r_nd_ld_set->ND_id;
		r_nd_ld->ld_id = r_nd_ld_set->LD_id;
		r_nd_ld->nd_id_ptr = r_nd_ld_set->ND_id_ptr;
		r_nd_ld->ld_id_ptr = r_nd_ld_set->LD_id_ptr;
		r_nd_ld->fptr = r_nd_ld_set->fptr;
		r_nd_ld->bptr = r_nd_ld_set->bptr;
		r_nd_ld_tb->Insert(*r_nd_ld);

	}

	delete r_nd_ld;
	delete r_nd_ld_set;

	cout << "//18 - read r_st_bus table" << endl;
	CLS_R_ST_BUS* r_st_bus = new CLS_R_ST_BUS;
	DMS::R_ST_BUS* r_st_bus_set = new DMS::R_ST_BUS;
	size = sizeof(DMS::R_ST_BUS);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_BUS")));
	r_st_bus_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_st_bus_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_BUS")), 1, size) <= 0)
		{
			delete r_st_bus;
			delete r_st_bus_set;

			return false;
		}

		r_st_bus->st_id = r_st_bus_set->ST_id;
		r_st_bus->bus_id = r_st_bus_set->BUS_id;
		r_st_bus->st_id_ptr = r_st_bus_set->ST_id_ptr;
		r_st_bus->bus_id_ptr = r_st_bus_set->BUS_id_ptr;
		r_st_bus->fptr = r_st_bus_set->fptr;
		r_st_bus->bptr = r_st_bus_set->bptr;
		r_st_bus_tb->Insert(*r_st_bus);

	}

	delete r_st_bus;
	delete r_st_bus_set;

	cout << "//19 - read r_st_cb table" << endl;
	CLS_R_ST_CB* r_st_cb = new CLS_R_ST_CB;
	DMS::R_ST_CB* r_st_cb_set = new DMS::R_ST_CB;
	size = sizeof(DMS::R_ST_CB);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_CB")));
	r_st_cb_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_st_cb_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_CB")), 1, size) <= 0)
		{
			delete r_st_cb;
			delete r_st_cb_set;

			return false;
		}

		r_st_cb->st_id = r_st_cb_set->ST_id;
		r_st_cb->cb_id = r_st_cb_set->CB_id;
		r_st_cb->st_id_ptr = r_st_cb_set->ST_id_ptr;
		r_st_cb->cb_id_ptr = r_st_cb_set->CB_id_ptr;
		r_st_cb->fptr = r_st_cb_set->fptr;
		r_st_cb->bptr = r_st_cb_set->bptr;
		r_st_cb_tb->Insert(*r_st_cb);

	}

	delete r_st_cb;
	delete r_st_cb_set;

	cout << "//20 - read r_st_cp table" << endl;
	CLS_R_ST_CP* r_st_cp = new CLS_R_ST_CP;
	DMS::R_ST_CP* r_st_cp_set = new DMS::R_ST_CP;
	size = sizeof(DMS::R_ST_CP);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_CP")));
	r_st_cp_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_st_cp_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_CP")), 1, size) <= 0)
		{
			delete r_st_cp;
			delete r_st_cp_set;

			return false;
		}

		r_st_cp->st_id = r_st_cp_set->ST_id;
		r_st_cp->cp_id = r_st_cp_set->CP_id;
		r_st_cp->st_id_ptr = r_st_cp_set->ST_id_ptr;
		r_st_cp->cp_id_ptr = r_st_cp_set->CP_id_ptr;
		r_st_cp->fptr = r_st_cp_set->fptr;
		r_st_cp->bptr = r_st_cp_set->bptr;
		r_st_cp_tb->Insert(*r_st_cp);

	}

	delete r_st_cp;
	delete r_st_cp_set;

	cout << "//21 - read r_st_ld table" << endl;
	CLS_R_ST_LD* r_st_ld = new CLS_R_ST_LD;
	DMS::R_ST_LD* r_st_ld_set = new DMS::R_ST_LD;
	size = sizeof(DMS::R_ST_LD);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_LD")));
	r_st_ld_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_st_ld_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_LD")), 1, size) <= 0)
		{
			delete r_st_ld;
			delete r_st_ld_set;

			return false;
		}

		r_st_ld->st_id = r_st_ld_set->ST_id;
		r_st_ld->ld_id = r_st_ld_set->LD_id;
		r_st_ld->st_id_ptr = r_st_ld_set->ST_id_ptr;
		r_st_ld->ld_id_ptr = r_st_ld_set->LD_id_ptr;
		r_st_ld->fptr = r_st_ld_set->fptr;
		r_st_ld->bptr = r_st_ld_set->bptr;
		r_st_ld_tb->Insert(*r_st_ld);

	}

	delete r_st_ld;
	delete r_st_ld_set;

	cout << "//22 - read r_st_so table" << endl;
	CLS_R_ST_SO* r_st_so = new CLS_R_ST_SO;
	DMS::R_ST_SO* r_st_so_set = new DMS::R_ST_SO;
	size = sizeof(DMS::R_ST_SO);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_SO")));
	r_st_so_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_st_so_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_SO")), 1, size) <= 0)
		{
			delete r_st_so;
			delete r_st_so_set;

			return false;
		}

		r_st_so->st_id = r_st_so_set->ST_id;
		r_st_so->so_id = r_st_so_set->SO_id;
		r_st_so->st_id_ptr = r_st_so_set->ST_id_ptr;
		r_st_so->so_id_ptr = r_st_so_set->SO_id_ptr;
		r_st_so->fptr = r_st_so_set->fptr;
		r_st_so->bptr = r_st_so_set->bptr;
		r_st_so_tb->Insert(*r_st_so);

	}

	delete r_st_so;
	delete r_st_so_set;

	cout << "//23 - read r_st_xfmr table" << endl;
	CLS_R_ST_XFMR* r_st_xfmr = new CLS_R_ST_XFMR;
	DMS::R_ST_XFMR* r_st_xfmr_set = new DMS::R_ST_XFMR;
	size = sizeof(DMS::R_ST_XFMR);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_XFMR")));
	r_st_xfmr_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_st_xfmr_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_XFMR")), 1, size) <= 0)
		{
			delete r_st_xfmr;
			delete r_st_xfmr_set;

			return false;
		}

		r_st_xfmr->st_id = r_st_xfmr_set->ST_id;
		r_st_xfmr->xfmr_id = r_st_xfmr_set->XFMR_id;
		r_st_xfmr->st_id_ptr = r_st_xfmr_set->ST_id_ptr;
		r_st_xfmr->xfmr_id_ptr = r_st_xfmr_set->XFMR_id_ptr;
		r_st_xfmr->fptr = r_st_xfmr_set->fptr;
		r_st_xfmr->bptr = r_st_xfmr_set->bptr;
		r_st_xfmr_tb->Insert(*r_st_xfmr);

	}

	delete r_st_xfmr;
	delete r_st_xfmr_set;

	cout << "//24 - read r_st_zn table" << endl;
	CLS_R_ST_ZN* r_st_zn = new CLS_R_ST_ZN;
	DMS::R_ST_ZN* r_st_zn_set = new DMS::R_ST_ZN;
	size = sizeof(DMS::R_ST_ZN);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_ZN")));
	r_st_zn_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_st_zn_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_ZN")), 1, size) <= 0)
		{
			delete r_st_zn;
			delete r_st_zn_set;

			return false;
		}

		r_st_zn->st_id = r_st_zn_set->ST_id;
		r_st_zn->zn_id = r_st_zn_set->ZN_id;
		r_st_zn->st_id_ptr = r_st_zn_set->ST_id_ptr;
		r_st_zn->zn_id_ptr = r_st_zn_set->ZN_id_ptr;
		r_st_zn->fptr = r_st_zn_set->fptr;
		r_st_zn->bptr = r_st_zn_set->bptr;
		r_st_zn_tb->Insert(*r_st_zn);

	}

	delete r_st_zn;
	delete r_st_zn_set;

	cout << "//25 - read r_xfmr_xf table" << endl;
	CLS_R_XFMR_XF* r_xfmr_xf = new CLS_R_XFMR_XF;
	DMS::R_XFMR_XF* r_xfmr_xf_set = new DMS::R_XFMR_XF;
	size = sizeof(DMS::R_XFMR_XF);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_XFMR_XF")));
	r_xfmr_xf_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_xfmr_xf_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_XFMR_XF")), 1, size) <= 0)
		{
			delete r_xfmr_xf;
			delete r_xfmr_xf_set;

			return false;
		}

		r_xfmr_xf->xfmr_id = r_xfmr_xf_set->XFMR_id;
		r_xfmr_xf->xf_id = r_xfmr_xf_set->XF_id;
		r_xfmr_xf->xfmr_id_ptr = r_xfmr_xf_set->XFMR_id_ptr;
		r_xfmr_xf->xf_id_ptr = r_xfmr_xf_set->XF_id_ptr;
		r_xfmr_xf->fptr = r_xfmr_xf_set->fptr;
		r_xfmr_xf->bptr = r_xfmr_xf_set->bptr;
		r_xfmr_xf_tb->Insert(*r_xfmr_xf);

	}

	delete r_xfmr_xf;
	delete r_xfmr_xf_set;

	cout << "//26 - read r_zn_ld table" << endl;
	CLS_R_ZN_LD* r_zn_ld = new CLS_R_ZN_LD;
	DMS::R_ZN_LD* r_zn_ld_set = new DMS::R_ZN_LD;
	size = sizeof(DMS::R_ZN_LD);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_LD")));
	r_zn_ld_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_zn_ld_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_LD")), 1, size) <= 0)
		{
			delete r_zn_ld;
			delete r_zn_ld_set;

			return false;
		}

		r_zn_ld->zn_id = r_zn_ld_set->ZN_id;
		r_zn_ld->ld_id = r_zn_ld_set->LD_id;
		r_zn_ld->zn_id_ptr = r_zn_ld_set->ZN_id_ptr;
		r_zn_ld->ld_id_ptr = r_zn_ld_set->LD_id_ptr;
		r_zn_ld->fptr = r_zn_ld_set->fptr;
		r_zn_ld->bptr = r_zn_ld_set->bptr;
		r_zn_ld_tb->Insert(*r_zn_ld);

	}

	delete r_zn_ld;
	delete r_zn_ld_set;

	cout << "//27 - read r_zn_sec table" << endl;
	CLS_R_ZN_SEC* r_zn_sec = new CLS_R_ZN_SEC;
	DMS::R_ZN_SEC* r_zn_sec_set = new DMS::R_ZN_SEC;
	size = sizeof(DMS::R_ZN_SEC);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_SEC")));
	r_zn_sec_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_zn_sec_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_SEC")), 1, size) <= 0)
		{
			delete r_zn_sec;
			delete r_zn_sec_set;

			return false;
		}

		r_zn_sec->zn_id = r_zn_sec_set->ZN_id;
		r_zn_sec->sec_id = r_zn_sec_set->SEC_id;
		r_zn_sec->zn_id_ptr = r_zn_sec_set->ZN_id_ptr;
		r_zn_sec->sec_id_ptr = r_zn_sec_set->SEC_id_ptr;
		r_zn_sec->fptr = r_zn_sec_set->fptr;
		r_zn_sec->bptr = r_zn_sec_set->bptr;
		r_zn_sec_tb->Insert(*r_zn_sec);

	}

	delete r_zn_sec;
	delete r_zn_sec_set;

	cout << "//28 - read sec table" << endl;
	CLS_SEC* sec = new CLS_SEC;
	DMS::SEC* sec_set = new DMS::SEC;
	size = sizeof(DMS::SEC);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("SEC")));
	sec_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(sec_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("SEC")), 1, size) <= 0)
		{
			delete sec;
			delete sec_set;

			return false;
		}

		sec->zn_id = sec_set->zn_id;
		sec->id = sec_set->id;
		strcpy(sec->descr, sec_set->descr);
		sec->nd = sec_set->nd;
		sec->znd = sec_set->znd;
		sec->ind = sec_set->ind;
		sec->iznd = sec_set->iznd;
		sec->ibs = sec_set->ibs;
		sec->zbs = sec_set->zbs;
		sec->r0 = sec_set->r0;
		sec->x0 = sec_set->x0;
		sec->b0 = sec_set->b0;
		sec->q = sec_set->q;
		sec->topflags = sec_set->topflags;
		sec->algip = sec_set->algip;
		sec->ialgip = sec_set->ialgip;
		sec->algiq = sec_set->algiq;
		sec->ialgiq = sec_set->ialgiq;
		sec->algii = sec_set->algii;
		sec->ialgii = sec_set->ialgii;
		sec->algzp = sec_set->algzp;
		sec->ialgzp = sec_set->ialgzp;
		sec->algzq = sec_set->algzq;
		sec->ialgzq = sec_set->ialgzq;
		sec->algzi = sec_set->algzi;
		sec->ialgzi = sec_set->ialgzi;
		sec->w = sec_set->w;
		sec->r = sec_set->r;
		sec->i = sec_set->i;
		sec->wz = sec_set->wz;
		sec->rz = sec_set->rz;
		sec->iz = sec_set->iz;
		sec->r_zn = sec_set->r_zn;
//张瑞雪添加于20120213
		sec->r0_ab = sec_set->r0_ab;
		sec->r0_ac = sec_set->r0_ac;
		sec->r0_ba = sec_set->r0_ba;
		sec->r0_bb = sec_set->r0_bb;
		sec->r0_bc = sec_set->r0_bc;
		sec->r0_ca = sec_set->r0_ca;
		sec->r0_cb = sec_set->r0_cb;
		sec->r0_cc = sec_set->r0_cc;
		sec->x0_ab = sec_set->x0_ab;
		sec->x0_ac = sec_set->x0_ac;
		sec->x0_ba = sec_set->x0_ba;
		sec->x0_bb = sec_set->x0_bb;
		sec->x0_bc = sec_set->x0_bc;
		sec->x0_ca = sec_set->x0_ca;
		sec->x0_cb = sec_set->x0_cb;
		sec->x0_cc = sec_set->x0_cc;
		sec->b0_ab = sec_set->b0_ab;
		sec->b0_ac = sec_set->b0_ac;
		sec->b0_ba = sec_set->b0_ba;
		sec->b0_bb = sec_set->b0_bb;
		sec->b0_bc = sec_set->b0_bc;
		sec->b0_ca = sec_set->b0_ca;
		sec->b0_cb = sec_set->b0_cb;
		sec->b0_cc = sec_set->b0_cc;
		sec->algip_b = sec_set->algip_b;
		sec->ialgip_b = sec_set->ialgip_b;
		sec->algiq_b = sec_set->algiq_b;
		sec->ialgiq_b = sec_set->ialgiq_b;
		sec->algii_b = sec_set->algii_b;
		sec->ialgii_b = sec_set->ialgii_b;
		sec->algzp_b = sec_set->algzp_b;
		sec->ialgzp_b = sec_set->ialgzp_b;
		sec->algzq_b = sec_set->algzq_b;
		sec->ialgzq_b = sec_set->ialgzq_b;
		sec->algzi_b = sec_set->algzi_b;
		sec->ialgzi_b = sec_set->ialgzi_b;
		sec->w_b = sec_set->w_b;
		sec->r_b = sec_set->r_b;
		sec->i_b = sec_set->i_b;
		sec->wz_b = sec_set->wz_b;
		sec->rz_b = sec_set->rz_b;
		sec->iz_b = sec_set->iz_b;
		sec->algip_c = sec_set->algip_c;
		sec->ialgip_c = sec_set->ialgip_c;
		sec->algiq_c = sec_set->algiq_c;
		sec->ialgiq_c = sec_set->ialgiq_c;
		sec->algii_c = sec_set->algii_c;
		sec->ialgii_c = sec_set->ialgii_c;
		sec->algzp_c = sec_set->algzp_c;
		sec->ialgzp_c = sec_set->ialgzp_c;
		sec->algzq_c = sec_set->algzq_c;
		sec->ialgzq_c = sec_set->ialgzq_c;
		sec->algzi_c = sec_set->algzi_c;
		sec->ialgzi_c = sec_set->ialgzi_c;
		sec->w_c = sec_set->w_c;
		sec->r_c = sec_set->r_c;
		sec->i_c = sec_set->i_c;
		sec->wz_c = sec_set->wz_c;
		sec->rz_c = sec_set->rz_c;
		sec->iz_c = sec_set->iz_c;
		sec->phases = sec_set->phases;
		sec->l = sec_set->l;
		sec->mode = sec_set->mode;
		//张瑞雪添加于20120503
		sec->algip_c = sec_set->algip_c;
		sec->ialgip_c = sec_set->ialgip_c;
		sec->algiq_c = sec_set->algiq_c;
		sec->ialgiq_c = sec_set->ialgiq_c;
		sec->algii_c = sec_set->algii_c;
		sec->ialgii_c = sec_set->ialgii_c;
		sec->algzp_c = sec_set->algzp_c;
		sec->ialgzp_c = sec_set->ialgzp_c;
		sec->algzq_c = sec_set->algzq_c;
		sec->ialgzq_c = sec_set->ialgzq_c;
		sec->algzi_c = sec_set->algzi_c;
		sec->ialgzi_c = sec_set->ialgzi_c;
		sec->w_c = sec_set->w_c;
		sec->r_c = sec_set->r_c;
		sec->i_c = sec_set->i_c;
		sec->wz_c = sec_set->wz_c;
		sec->rz_c = sec_set->rz_c;
		sec->iz_c = sec_set->iz_c;
		sec->r0_aa = sec_set->r0_aa;
		sec->x0_aa = sec_set->x0_aa;
		sec->b0_aa = sec_set->b0_aa;
                sec->r_fi = sec_set->r_fi;
		sec_tb->Insert(*sec);

	}

	delete sec;
	delete sec_set;

	cout << "//29 - read so table" << endl;
	CLS_SO* so = new CLS_SO;
	DMS::SO* so_set = new DMS::SO;
	size = sizeof(DMS::SO);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("SO")));
	so_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(so_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("SO")), 1, size) <= 0)
		{
			delete so;
			delete so_set;

			return false;
		}

		so->id = so_set->id;
		strcpy(so->descr, so_set->descr);
		so->nd = so_set->nd;
		so->ind = so_set->ind;
		so->ibs = so_set->ibs;
		so->iisland = so_set->iisland;
		so->algp = so_set->algp;
		so->ialgp = so_set->ialgp;
		so->algq = so_set->algq;
		so->ialgq = so_set->ialgq;
		so->algi = so_set->algi;
		so->ialgi = so_set->ialgi;
		so->q = so_set->q;
		so->topflags = so_set->topflags;
		so->w = so_set->w;
		so->r = so_set->r;
		so->i = so_set->i;
		so->r_st = so_set->r_st;
		so->ss = so_set->ss;
		so->sw = so_set->sw;
		so->sr = so_set->sr;
//张瑞雪添加于20120213
		so->v = so_set->v;
		so->slackavr = so_set->slackavr;
		so->wmx = so_set->wmx;
		so->rmx = so_set->rmx;
		so->wmn = so_set->wmn;
		so->rmn = so_set->rmn;
		so->w_b = so_set->w_b;
		so->r_b = so_set->r_b;
		so->i_b = so_set->i_b;
		so->v_b = so_set->v_b;
		so->algp_b = so_set->algp_b;
		so->ialgp_b = so_set->ialgp_b;
		so->algq_b = so_set->algq_b;
		so->ialgq_b = so_set->ialgq_b;
		so->algi_b = so_set->algi_b;
		so->ialgi_b = so_set->ialgi_b;
		so->w_c = so_set->w_c;
		so->r_c = so_set->r_c;
		so->i_c = so_set->i_c;
		so->v_c = so_set->v_c;
		so->algp_c = so_set->algp_c;
		so->ialgp_c = so_set->ialgp_c;
		so->algq_c = so_set->algq_c;
		so->ialgq_c = so_set->ialgq_c;
		so->algi_c = so_set->algi_c;
		so->ialgi_c = so_set->ialgi_c;

		//郝泽赟添加于2012.04.26
		so->adeg = so_set->adeg;
		so->adeg_b = so_set->adeg_b;
		so->adeg_c = so_set->adeg_c;
		//张瑞雪添加于20120503
		so->adeg_a = so_set->adeg_a;
		so->w_a = so_set->w_a;
		so->r_a = so_set->r_a;
		so->i_a = so_set->i_a;
		so->v_a = so_set->v_a;
		so->algp_a = so_set->algp_a;
		so->ialgp_a = so_set->ialgp_a;
		so->algq_a = so_set->algq_a;
		so->ialgq_a = so_set->ialgq_a;
		so->algi_a = so_set->algi_a;
		so->ialgi_a = so_set->ialgi_a;
		so_tb->Insert(*so);

	}

	delete so;
	delete so_set;

	cout << "//30 - read st table" << endl;
	CLS_ST* st = new CLS_ST;
	DMS::ST* st_set = new DMS::ST;
	size = sizeof(DMS::ST);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("ST")));
	st_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(st_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("ST")), 1, size) <= 0)
		{
			delete st;
			delete st_set;
			return false;
		}

		st->id = st_set->id;
		strcpy(st->descr, st_set->descr);
		//st->abb = st_set->abb;
		st->type = st_set->type;
		st->qcomp = st_set->qcomp;
		st->wso = st_set->wso;
		st->rso = st_set->rso;
		st->wload = st_set->wload;
		st->rload = st_set->rload;
		st->r_co = st_set->r_co;
		st->r_xfmr = st_set->r_xfmr;
		st->r_so = st_set->r_so;
		st->r_cb = st_set->r_cb;
		st->r_switch = st_set->r_switch;
		st->r_ld = st_set->r_ld;
		st->r_cp = st_set->r_cp;
		st->r_bus = st_set->r_bus;
		st->r_zn = st_set->r_zn;
//张瑞雪添加于20120213
		st->wso_b = st_set->wso_b;
		st->rso_b = st_set->rso_b;
		st->wload_b = st_set->wload_b;
		st->rload_b = st_set->rload_b;
		st->wso_c = st_set->wso_c;
		st->rso_c = st_set->rso_c;
		st->wload_c = st_set->wload_c;
		st->rload_c = st_set->rload_c;
		//张瑞雪添加于20120503
		st->wso_a = st_set->wso_a;
		st->rso_a = st_set->rso_a;
		st->wload_a = st_set->wload_a;
		st->rload_a = st_set->rload_a;
		st_tb->Insert(*st);

	}

	delete st;
	delete st_set;

	cout << "//31 - read tapty table" << endl;
	CLS_TAPTY* tapty = new CLS_TAPTY;
	DMS::TAPTY* tapty_set = new DMS::TAPTY;
	size = sizeof(DMS::TAPTY);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("TAPTY")));
	tapty_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(tapty_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("TAPTY")), 1, size) <= 0)
		{
			delete tapty;
			delete tapty_set;
			return false;
		}

		tapty->id = tapty_set->id;
		tapty->mn = tapty_set->mn;
		tapty->mx = tapty_set->mx;
		tapty->nom = tapty_set->nom;
		tapty->step = tapty_set->step;
		tapty_tb->Insert(*tapty);

	}

	delete tapty;
	delete tapty_set;

	cout << "//32 - read xf table" << endl;
	CLS_XF* xf = new CLS_XF;
	DMS::XF* xf_set = new DMS::XF;
	size = sizeof(DMS::XF);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("XF")));
	xf_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(xf_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("XF")), 1, size) <= 0)
		{
			delete xf;
			delete xf_set;

			return false;
		}

		xf->id = xf_set->id;
		strcpy(xf->descr, xf_set->descr);
		xf->itapty = xf_set->itapty;
		xf->iztapty = xf_set->iztapty;
		xf->tap = xf_set->tap;
		xf->ztap = xf_set->ztap;
		xf->kvnom = xf_set->kvnom;
		xf->zkvnom = xf_set->zkvnom;
		xf->nd = xf_set->nd;
		xf->znd = xf_set->znd;
		xf->ind = xf_set->ind;
		xf->iznd = xf_set->iznd;
		xf->ibs = xf_set->ibs;
		xf->zbs = xf_set->zbs;
		xf->mvanom = xf_set->mvanom;
		xf->q = xf_set->q;
		xf->topflags = xf_set->topflags;
		xf->r = xf_set->r;
		xf->x = xf_set->x;
		xf->ialgip = xf_set->ialgip;
		xf->ialgiq = xf_set->ialgiq;
		xf->ialgii = xf_set->ialgii;
		xf->ialgit = xf_set->ialgit;
		xf->ialgzp = xf_set->ialgzp;
		xf->ialgzq = xf_set->ialgzq;
		xf->ialgzi = xf_set->ialgzi;
		xf->ialgzt = xf_set->ialgzt;
		xf->w = xf_set->w;
		xf->r_ = xf_set->r_;
		xf->i = xf_set->i;
		xf->wz = xf_set->wz;
		xf->rz = xf_set->rz;
		xf->iz = xf_set->iz;
		xf->r_xfmr = xf_set->r_xfmr;
//张瑞雪添加于20120215
		xf->xftype = xf_set->xftype;
		xf->itapty_b = xf_set->itapty_b;
		xf->iztapty_b = xf_set->iztapty_b;
		xf->tap_b = xf_set->tap_b;
		xf->ztap_b = xf_set->ztap_b;
		xf->r_b = xf_set->r_b;
		xf->x_b = xf_set->x_b;
		xf->ialgip_b = xf_set->ialgip_b;
		xf->ialgiq_b = xf_set->ialgiq_b;
		xf->ialgii_b = xf_set->ialgii_b;
		xf->ialgit_b = xf_set->ialgit_b;
		xf->ialgzp_b = xf_set->ialgzp_b;
		xf->ialgzq_b = xf_set->ialgzq_b;
		xf->ialgzi_b = xf_set->ialgzi_b;
		xf->ialgzt_b = xf_set->ialgzt_b;
		xf->w_b = xf_set->w_b;
		xf->rq_b = xf_set->rq_b;
		xf->i_b = xf_set->i_b;
		xf->wz_b = xf_set->wz_b;
		xf->rz_b = xf_set->rz_b;
		xf->iz_b = xf_set->iz_b;
		xf->itapty_c = xf_set->itapty_c;
		xf->iztapty_c = xf_set->iztapty_c;
		xf->tap_c = xf_set->tap_c;
		xf->ztap_c = xf_set->ztap_c;
		xf->r_c = xf_set->r_c;
		xf->x_c = xf_set->x_c;
		xf->ialgip_c = xf_set->ialgip_c;
		xf->ialgiq_c = xf_set->ialgiq_c;
		xf->ialgii_c = xf_set->ialgii_c;
		xf->ialgit_c = xf_set->ialgit_c;
		xf->ialgzp_c = xf_set->ialgzp_c;
		xf->ialgzq_c = xf_set->ialgzq_c;
		xf->ialgzi_c = xf_set->ialgzi_c;
		xf->ialgzt_c = xf_set->ialgzt_c;
		xf->w_c = xf_set->w_c;
		xf->rq_c = xf_set->rq_c;
		xf->i_c = xf_set->i_c;
		xf->wz_c = xf_set->wz_c;
		xf->rz_c = xf_set->rz_c;
		xf->iz_c = xf_set->iz_c;
		//张瑞雪添加于20120503
		xf->r_a = xf_set->r_a;
		xf->x_a = xf_set->x_a;
		xf->ialgip_a = xf_set->ialgip_a;
		xf->ialgiq_a = xf_set->ialgiq_a;
		xf->ialgii_a = xf_set->ialgii_a;
		xf->ialgzp_a = xf_set->ialgzp_a;
		xf->ialgzq_a = xf_set->ialgzq_a;
		xf->ialgzi_a = xf_set->ialgzi_a;
		xf->w_a = xf_set->w_a;
		xf->rq_a = xf_set->rq_a;
		xf->i_a = xf_set->i_a;
		xf->wz_a = xf_set->wz_a;
		xf->rz_a = xf_set->rz_a;
		xf->iz_a = xf_set->iz_a;
		xf_tb->Insert(*xf);

	}

	delete xf;
	delete xf_set;

	cout << "//33 - read xfmr table" << endl;
	CLS_XFMR* xfmr = new CLS_XFMR;
	DMS::XFMR* xfmr_set = new DMS::XFMR;
	size = sizeof(DMS::XFMR);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("XFMR")));
	xfmr_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(xfmr_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("XFMR")), 1, size) <= 0)
		{
			delete xfmr;
			delete xfmr_set;

			return false;
		}

		xfmr->id = xfmr_set->id;
		strcpy(xfmr->descr, xfmr_set->descr);
		xfmr->type = xfmr_set->type;
		xfmr->st = xfmr_set->st;
		xfmr->rh = xfmr_set->rh;
		xfmr->xh = xfmr_set->xh;
		xfmr->rm = xfmr_set->rm;
		xfmr->xm = xfmr_set->xm;
		xfmr->rl = xfmr_set->rl;
		xfmr->xl = xfmr_set->xl;
		xfmr->kvnomh = xfmr_set->kvnomh;
		xfmr->kvnomm = xfmr_set->kvnomm;
		xfmr->kvnoml = xfmr_set->kvnoml;
		xfmr->mvah = xfmr_set->mvah;
		xfmr->mvam = xfmr_set->mvam;
		xfmr->mval = xfmr_set->mval;
		xfmr->algph = xfmr_set->algph;
		xfmr->ialgph = xfmr_set->ialgph;
		xfmr->algqh = xfmr_set->algqh;
		xfmr->ialgqh = xfmr_set->ialgqh;
		xfmr->algih = xfmr_set->algih;
		xfmr->ialgih = xfmr_set->ialgih;
		xfmr->algth = xfmr_set->algth;
		xfmr->ialgth = xfmr_set->ialgth;
		xfmr->algpm = xfmr_set->algpm;
		xfmr->ialgpm = xfmr_set->ialgpm;
		xfmr->algqm = xfmr_set->algqm;
		xfmr->ialgqm = xfmr_set->ialgqm;
		xfmr->algim = xfmr_set->algim;
		xfmr->ialgim = xfmr_set->ialgim;
		xfmr->algtm = xfmr_set->algtm;
		xfmr->ialgtm = xfmr_set->ialgtm;
		xfmr->algpl = xfmr_set->algpl;
		xfmr->ialgpl = xfmr_set->ialgpl;
		xfmr->algql = xfmr_set->algql;
		xfmr->ialgql = xfmr_set->ialgql;
		xfmr->algil = xfmr_set->algil;
		xfmr->ialgil = xfmr_set->ialgil;
		xfmr->algtl = xfmr_set->algtl;
		xfmr->ialgtl = xfmr_set->ialgtl;
		xfmr->hnd = xfmr_set->hnd;
		xfmr->mnd = xfmr_set->mnd;
		xfmr->lnd = xfmr_set->lnd;
		xfmr->htapty = xfmr_set->htapty;
		xfmr->mtapty = xfmr_set->mtapty;
		xfmr->ltapty = xfmr_set->ltapty;
		xfmr->htap = xfmr_set->htap;
		xfmr->mtap = xfmr_set->mtap;
		xfmr->ltap = xfmr_set->ltap;
		xfmr->q = xfmr_set->q;
		xfmr->topflags = xfmr_set->topflags;
		xfmr->r_st = xfmr_set->r_st;
		xfmr->r_xf = xfmr_set->r_xf;
//张瑞雪添加于20120215
		xfmr->rh_b = xfmr_set->rh_b;
		xfmr->xh_b = xfmr_set->xh_b;
		xfmr->rl_b = xfmr_set->rl_b;
		xfmr->xl_b = xfmr_set->xl_b;
		xfmr->algph_b = xfmr_set->algph_b;
		xfmr->ialgph_b = xfmr_set->ialgph_b;
		xfmr->algqh_b = xfmr_set->algqh_b;
		xfmr->ialgqh_b = xfmr_set->ialgqh_b;
		xfmr->algih_b = xfmr_set->algih_b;
		xfmr->ialgih_b = xfmr_set->ialgih_b;
		xfmr->algth_b = xfmr_set->algth_b;
		xfmr->algpl_b = xfmr_set->algpl_b;
		xfmr->ialgpl_b = xfmr_set->ialgpl_b;
		xfmr->algql_b = xfmr_set->algql_b;
		xfmr->ialgql_b = xfmr_set->ialgql_b;
		xfmr->algil_b = xfmr_set->algil_b;
		xfmr->ialgil_b = xfmr_set->ialgil_b;
		xfmr->algtl_b = xfmr_set->algtl_b;
		xfmr->ialgtl_b = xfmr_set->ialgtl_b;
		xfmr->ialgth_b = xfmr_set->ialgth_b;
		xfmr->htapty_b = xfmr_set->htapty_b;
		xfmr->ltapty_b = xfmr_set->ltapty_b;
		xfmr->htap_b = xfmr_set->htap_b;
		xfmr->ltap_b = xfmr_set->ltap_b;
		xfmr->rh_c = xfmr_set->rh_c;
		xfmr->xh_c = xfmr_set->xh_c;
		xfmr->rl_c = xfmr_set->rl_c;
		xfmr->xl_c = xfmr_set->xl_c;
		xfmr->algph_c = xfmr_set->algph_c;
		xfmr->ialgph_c = xfmr_set->ialgph_c;
		xfmr->algqh_c = xfmr_set->algqh_c;
		xfmr->ialgqh_c = xfmr_set->ialgqh_c;
		xfmr->algih_c = xfmr_set->algih_c;
		xfmr->ialgih_c = xfmr_set->ialgih_c;
		xfmr->algth_c = xfmr_set->algth_c;
		xfmr->algpl_c = xfmr_set->algpl_c;
		xfmr->ialgpl_c = xfmr_set->ialgpl_c;
		xfmr->algql_c = xfmr_set->algql_c;
		xfmr->ialgql_c = xfmr_set->ialgql_c;
		xfmr->algil_c = xfmr_set->algil_c;
		xfmr->ialgil_c = xfmr_set->ialgil_c;
		xfmr->algtl_c = xfmr_set->algtl_c;
		xfmr->ialgtl_c = xfmr_set->ialgtl_c;
		xfmr->ialgth_c = xfmr_set->ialgth_c;
		xfmr->htapty_c = xfmr_set->htapty_c;
		xfmr->ltapty_c = xfmr_set->ltapty_c;
		xfmr->htap_c = xfmr_set->htap_c;
		xfmr->ltap_c = xfmr_set->ltap_c;
		xfmr-> xftype = xfmr_set-> xftype;
		//张瑞雪添加于20120503
		xfmr->rh_a = xfmr_set->rh_a;
		xfmr->xh_a = xfmr_set->xh_a;
		xfmr->rl_a = xfmr_set->rl_a;
		xfmr->xl_a = xfmr_set->xl_a;
		xfmr->algph_a = xfmr_set->algph_a;
		xfmr->ialgph_a = xfmr_set->ialgph_a;
		xfmr->algqh_a = xfmr_set->algqh_a;
		xfmr->ialgqh_a = xfmr_set->ialgqh_a;
		xfmr->algih_a = xfmr_set->algih_a;
		xfmr->ialgih_a = xfmr_set->ialgih_a;
		xfmr->algpl_a = xfmr_set->algpl_a;
		xfmr->ialgpl_a = xfmr_set->ialgpl_a;
		xfmr->algql_a = xfmr_set->algql_a;
		xfmr->ialgql_a = xfmr_set->ialgql_a;
		xfmr->algil_a = xfmr_set->algil_a;
		xfmr->ialgil_a = xfmr_set->ialgil_a;

		xfmr_tb->Insert(*xfmr);
	}

	delete xfmr;
	delete xfmr_set;

	cout << "//34 - read zn table" << endl;
	CLS_ZN* zn = new CLS_ZN;
	DMS::ZN* zn_set = new DMS::ZN;
	size = sizeof(DMS::ZN);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("ZN")));
	zn_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(zn_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("ZN")), 1, size) <= 0)
		{
			delete zn;
			delete zn_set;
			return false;
		}

		zn->id = zn_set->id;
		strcpy(zn->descr, zn_set->descr);
		zn->type = zn_set->type;
		///////////////////////////////////////////////////
		///////////////荣义平添加于20111130为适应超过5个结节的ZN
#if ZN_ND_USE_ARRAY
//		zn->nds[0].nd_id = zn_set->nd;
//		zn->nds[1].nd_id = zn_set->znd;
//		zn->nds[2].nd_id = zn_set->nd3;
//		zn->nds[3].nd_id = zn_set->nd4;
//		zn->nds[4].nd_id = zn_set->nd5;
//		zn->nds[5].nd_id = zn_set->nd6;
//		zn->nds[6].nd_id = zn_set->nd7;
//		zn->nds[7].nd_id = zn_set->nd8;
//		zn->nds[8].nd_id = zn_set->nd9;
//		zn->nds[9].nd_id = zn_set->nd10;
//		zn->nds[10].nd_id = zn_set->nd11;
//		zn->nds[11].nd_id = zn_set->nd12;
//		zn->nds[12].nd_id = zn_set->nd13;
//		zn->nds[13].nd_id = zn_set->nd14;
//		zn->nds[14].nd_id = zn_set->nd15;
//		zn->nds[15] = zn_set->nd16;
//		zn->nds[16] = zn_set->nd17;
//		zn->nds[17] = zn_set->nd18;
//		zn->nds[18] = zn_set->nd19;
//		zn->nds[19] = zn_set->nd20;

//		zn->inds[0] = zn_set->ind;
//		zn->inds[1] = zn_set->iznd;
//		zn->inds[2] = zn_set->ind3;
//		zn->inds[3] = zn_set->ind4;
//		zn->inds[4] = zn_set->ind5;
//		zn->inds[5] = zn_set->ind6;
//		zn->inds[6] = zn_set->ind7;
//		zn->inds[7] = zn_set->ind8;
//		zn->inds[8] = zn_set->ind9;
//		zn->inds[9] = zn_set->ind10;
//		zn->inds[10] = zn_set->ind11;
//		zn->inds[11] = zn_set->ind12;
//		zn->inds[12] = zn_set->ind13;
//		zn->inds[13] = zn_set->ind14;
//		zn->inds[14] = zn_set->ind15;
//		zn->inds[15] = zn_set->ind16;
//		zn->inds[16] = zn_set->ind17;
//		zn->inds[17] = zn_set->ind18;
//		zn->inds[18] = zn_set->ind19;
//		zn->inds[19] = zn_set->ind20;

//		zn->ibss[0] = zn_set->ibs;
//		zn->ibss[1] = zn_set->zbs;
//		zn->ibss[2] = zn_set->ibs3;
//		zn->ibss[3] = zn_set->ibs4;
//		zn->ibss[4] = zn_set->ibs5;
//		zn->ibss[5] = zn_set->ibs6;
//		zn->ibss[6] = zn_set->ibs7;
//		zn->ibss[7] = zn_set->ibs8;
//		zn->ibss[8] = zn_set->ibs9;
//		zn->ibss[9] = zn_set->ibs10;
//		zn->ibss[10] = zn_set->ibs11;
//		zn->ibss[11] = zn_set->ibs12;
//		zn->ibss[12] = zn_set->ibs13;
//		zn->ibss[13] = zn_set->ibs14;
//		zn->ibss[14] = zn_set->ibs15;
//		zn->ibss[15] = zn_set->ibs16;
//		zn->ibss[16] = zn_set->ibs17;
//		zn->ibss[17] = zn_set->ibs18;
//		zn->ibss[18] = zn_set->ibs19;
//		zn->ibss[19] = zn_set->ibs20;
#else
		zn->nd = zn_set->nd;
		zn->znd = zn_set->znd;
		zn->nd3 = zn_set->nd3;
		zn->nd4 = zn_set->nd4;
		zn->nd5 = zn_set->nd5;
		zn->ind = zn_set->ind;
		zn->iznd = zn_set->iznd;
		zn->ind3 = zn_set->ind3;
		zn->ind4 = zn_set->ind4;
		zn->ind5 = zn_set->ind5;
		zn->ibs = zn_set->ibs;
		zn->zbs = zn_set->zbs;
		zn->ibs3 = zn_set->ibs3;
		zn->ibs4 = zn_set->ibs4;
		zn->ibs5 = zn_set->ibs5;
#endif
//		zn->algip = zn_set->algip;
//		zn->ialgip = zn_set->ialgip;
//		zn->algiq = zn_set->algiq;
//		zn->ialgiq = zn_set->ialgiq;
//		zn->algii = zn_set->algii;
//		zn->ialgii = zn_set->ialgii;
//		zn->algzp = zn_set->algzp;
//		zn->ialgzp = zn_set->ialgzp;
//		zn->algzq = zn_set->algzq;
//		zn->ialgzq = zn_set->ialgzq;
//		zn->algzi = zn_set->algzi;
//		zn->ialgzi = zn_set->ialgzi;
//		zn->alg3p = zn_set->alg3p;
//		zn->ialg3p = zn_set->ialg3p;
//		zn->alg3q = zn_set->alg3q;
//		zn->ialg3q = zn_set->ialg3q;
//		zn->alg3i = zn_set->alg3i;
//		zn->ialg3i = zn_set->ialg3i;
//		zn->alg4p = zn_set->alg4p;
//		zn->ialg4p = zn_set->ialg4p;
//		zn->alg4q = zn_set->alg4q;
//		zn->ialg4q = zn_set->ialg4q;
//		zn->alg4i = zn_set->alg4i;
//		zn->ialg4i = zn_set->ialg4i;
//		zn->alg5p = zn_set->alg5p;
//		zn->ialg5p = zn_set->ialg5p;
//		zn->alg5q = zn_set->alg5q;
//		zn->ialg5q = zn_set->ialg5q;
//		zn->alg5i = zn_set->alg5i;
//		zn->ialg5i = zn_set->ialg5i;
		zn->q = zn_set->q;
		zn->topflags = zn_set->topflags;
//		zn->w = zn_set->w;
//		zn->r = zn_set->r;
//		zn->i = zn_set->i;
//		zn->wz = zn_set->wz;
//		zn->rz = zn_set->rz;
//		zn->iz = zn_set->iz;
//		zn->w3 = zn_set->w3;
//		zn->r3 = zn_set->r3;
//		zn->i3 = zn_set->i3;
//		zn->w4 = zn_set->w4;
//		zn->r4 = zn_set->r4;
//		zn->i4 = zn_set->i4;
//		zn->w5 = zn_set->w5;
//		zn->r5 = zn_set->r5;
//		zn->i5 = zn_set->i5;
		zn->r_st = zn_set->r_st;
		zn->r_ld = zn_set->r_ld;
		zn->r_sec = zn_set->r_sec;
		zn->r_nd = zn_set->r_nd;
		zn_tb->Insert(*zn);

	}

	delete zn;
	delete zn_set;

	cout << "//35 - read fi table" << endl;
	CLS_FI* fi = new CLS_FI;
	DMS::FI* fi_set = new DMS::FI;
	size = sizeof(DMS::FI);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("FI")));
	fi_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(fi_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("FI")), 1, size) <= 0)
		{
			delete fi;
			delete fi_set;

			return false;
		}

		fi->pid = fi_set->pid;
		fi->id = fi_set->id;
		strcpy(fi->descr, fi_set->descr);
		fi->sec = fi_set->sec;
		fi->digital = fi_set->digital;
		fi->idigital = fi_set->idigital;
		fi->pos = fi_set->pos;
		fi->r_sec = fi_set->r_sec;

		fi_tb->Insert(*fi);

	}

	delete fi;
	delete fi_set;

	cout << "//36 - read r_sec_fi table" << endl;
	CLS_R_SEC_FI* r_sec_fi = new CLS_R_SEC_FI;
	DMS::R_SEC_FI* r_sec_fi_set = new DMS::R_SEC_FI;
	size = sizeof(DMS::R_SEC_FI);

	cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(g_db_dms->GetTB("R_SEC_FI")));
	r_sec_fi_tb->Empty();
	for (pos = 0; pos < cnt_record; pos++)
	{
		if (oodbread_rp(r_sec_fi_set, pos, const_cast<TB_DESCR*>(g_db_dms->GetTB("R_SEC_FI")), 1, size) <= 0)
		{
			delete r_sec_fi;
			delete r_sec_fi_set;

			return false;
		}

		r_sec_fi->sec_id = r_sec_fi_set->sec_id;
		r_sec_fi->fi_id = r_sec_fi_set->fi_id;

		r_sec_fi->sec_id_ptr = r_sec_fi_set->sec_id_ptr;
		r_sec_fi->fi_id_ptr = r_sec_fi_set->fi_id_ptr;
		r_sec_fi->bptr = r_sec_fi_set->bptr;
		r_sec_fi->fptr = r_sec_fi_set->fptr;

		r_sec_fi_tb->Insert(*r_sec_fi);

	}

	delete r_sec_fi;
	delete r_sec_fi_set;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// write database object to relative database
bool DSBOB::WriteReDB(const char* dsn, const char* user, const char* password)
{

	printf("%s ……\r\n", __FUNCTION__);

	int count = 0, i = 0, size = 0;

	//g_db_dms->ResetAllTables();
	map<string, TB_DESCR*> *pTables = g_db_dms->GetTables();

	for(map<string, TB_DESCR*>::iterator it = pTables->begin(); it != pTables->end(); it++)
	{
		if(it->first != "ANALOG") g_db_dms->ResetTable(it->first.c_str());
	}

	cout << "//2 - write bs table" <<endl;

	CLS_BS* bs;
	DMS::BS* bs_set = new DMS::BS;
	size = sizeof(DMS::BS);

	bs = bs_tb->GetTable();
	count = bs_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		//if((bs + i)->iisland == -2) continue;

        bzero(bs_set,sizeof(DMS::BS));
		bs_set->id = (bs + i)->id;
		bs_set->iisland = (bs + i)->iisland;
		bs_set->q = (bs + i)->q;
		bs_set->qq = (bs + i)->qq;
		bs_set->fbs = (bs + i)->fbs;
		bs_set->bbs = (bs + i)->bbs;
		bs_set->pnd = (bs + i)->pnd;
		bs_set->kv = (bs + i)->kv;
		bs_set->v = (bs + i)->v;
		bs_set->adeg = (bs + i)->adeg;
		bs_set->w = (bs + i)->w;
		bs_set->r = (bs + i)->r;
//张瑞雪添加于20120213
		bs_set->kv_b =(bs + i)->kv_b ;
		bs_set->v_b =(bs + i)->v_b ;
		bs_set->adeg_b=(bs + i)->adeg_b;
		bs_set->w_b =(bs + i)->w_b ;
		bs_set->r_b = (bs + i)->r_b;
		bs_set->kv_c = (bs + i)->kv_c;
		bs_set->v_c = (bs + i)->v_c;
		bs_set->adeg_c = (bs + i)->adeg_c;
		bs_set->w_c = (bs + i)->w_c;
		bs_set->r_c = (bs + i)->r_c;
		//张瑞雪添加于20120503
		bs_set->kv_a = (bs + i)->kv_a;
		bs_set->v_a = (bs + i)->v_a;
		bs_set->adeg_a = (bs + i)->adeg_a;
		bs_set->w_a = (bs + i)->w_a;
		bs_set->r_a = (bs + i)->r_a;
        bs_set->valid = (bs+i)->valid;
		if (oodbinsert(bs_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("BS")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert bs error!errno=" << _oodb_errno << endl;

			delete bs_set;

			return false;
		}
	}

	delete bs_set;

	cout << "//3 - write bus table" <<endl;

	CLS_BUS* bus;
	DMS::BUS* bus_set = new DMS::BUS;
	size = sizeof(DMS::BUS);

	bus = bus_tb->GetTable();
	count = bus_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(bus_set,sizeof(DMS::BUS));
		bus_set->id =  (bus + i)->id;
		strcpy(bus_set->descr, (bus + i)->descr);
		bus_set->nd =  (bus + i)->nd;
		bus_set->ind = (bus + i)->ind;
		bus_set->ibs = (bus + i)->ibs;
		bus_set->algv =  (bus + i)->algv;
		bus_set->ialgv = (bus + i)->ialgv;
		bus_set->topflags = (bus + i)->topflags;
		bus_set->vtarget = (bus + i)->vtarget;
		bus_set->vl = (bus + i)->vl;
		bus_set->vhlim = (bus + i)->vhlim;
		bus_set->vllim = (bus + i)->vllim;
		bus_set->v = (bus + i)->v;
		bus_set->adeg = (bus + i)->adeg;
		bus_set->r_st = (bus + i)->r_st;
//张瑞雪添加于20120213
		bus_set->algv_b =  (bus + i)->algv_b;
		bus_set->ialgv_b = (bus + i)->ialgv_b;
		bus_set->vtarget_b = (bus + i)->vtarget_b;
		bus_set->vhlim_b = (bus + i)->vhlim_b;
		bus_set->vllim_b = (bus + i)->vllim_b;
		bus_set->v_b = (bus + i)->v_b;
		bus_set->adeg_b = (bus + i)->adeg_b;
		bus_set->algv_c =  (bus + i)->algv_c;
		bus_set->ialgv_c = (bus + i)->ialgv_c;
		bus_set->vtarget_c = (bus + i)->vtarget_c;
		bus_set->vhlim_c = (bus + i)->vhlim_c;
		bus_set->vllim_c = (bus + i)->vllim_c;
		bus_set->v_c = (bus + i)->v_c;
		bus_set->adeg_c = (bus + i)->adeg_c;
		//郝泽赟添加于20120426
		bus_set->algp =  (bus + i)->algp;
		bus_set->algp_b =  (bus + i)->algp_b;
		bus_set->algp_c =  (bus + i)->algp_c;
		bus_set->alga =  (bus + i)->alga;
		bus_set->alga_b =  (bus + i)->alga_b;
		bus_set->alga_c =  (bus + i)->alga_c;
		bus_set->ialgp = (bus + i)->ialgp;
		bus_set->ialgp_b = (bus + i)->ialgp_b;
		bus_set->ialgp_c = (bus + i)->ialgp_c;
		bus_set->ialga = (bus + i)->ialga;
		bus_set->ialga_b = (bus + i)->ialga_b;
		bus_set->ialga_c = (bus + i)->ialga_c;
		bus_set->stid =  (bus + i)->stid;
		//郝泽赟添加于20120503
		bus_set->alga_a =  (bus + i)->alga_a;
		bus_set->ialga_a = (bus + i)->ialga_a;
		bus_set->algp_a =  (bus + i)->algp_a;
		bus_set->ialgp_a = (bus + i)->ialgp_a;
		bus_set->algv_a =  (bus + i)->algv_a;
		bus_set->ialgv_a = (bus + i)->ialgv_a;
		bus_set->v_a = (bus + i)->v_a;
		bus_set->adeg_a = (bus + i)->adeg_a;
		bus_set->wmx = (bus + i)->wmx;
		if (oodbinsert(bus_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("BUS")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert bus error!errno=" << _oodb_errno << endl;

			delete bus_set;

			return false;
		}
	}

	delete bus_set;

	cout << "//4 - write cb table" <<endl;

	CLS_CB* cb;
	DMS::CB* cb_set = new DMS::CB;
	size = sizeof(DMS::CB);


	cb = cb_tb->GetTable();
	count = cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(cb_set,sizeof(DMS::CB));
		cb_set->id =  (cb + i)->id;
		strcpy(cb_set->descr, (cb + i)->descr);
		cb_set->nd =  (cb + i)->nd;
		cb_set->znd =  (cb + i)->znd;
		cb_set->ind = (cb + i)->ind;
		cb_set->iznd = (cb + i)->iznd;
//		cb_set->digital =  (cb + i)->digital;
//		cb_set->idigital = (cb + i)->idigital;
		cb_set->q = (cb + i)->q;
		cb_set->topflags = (cb + i)->topflags;
		cb_set->r_st = (cb  + i)->r_st;
		cb_set->stid =  (cb + i)->stid;
		if (oodbinsert(cb_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("CB")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert cb error!errno=" << _oodb_errno << endl;

			delete cb_set;

			return false;
		}
	}

	delete cb_set;

	cout << "//1 - write switch table" <<endl;

	CLS_SWITCH* swh;
	DMS::SWITCH* swh_set = new DMS::SWITCH;
	size = sizeof(DMS::SWITCH);
	switch_tb->Sequence();

	swh = switch_tb->GetTable();
	count = switch_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(swh_set,sizeof(DMS::SWITCH));
		swh_set->id =  (swh + i)->id;
		swh_set->pid =  (swh + i)->pid;
		strcpy(swh_set->descr, (swh + i)->descr);
		swh_set->nd =  (swh + i)->nd;
		swh_set->znd =  (swh + i)->znd;
		swh_set->ind = (swh + i)->ind;
		swh_set->iznd = (swh + i)->iznd;
		swh_set->q = (swh + i)->q;
		swh_set->topflags = (swh + i)->topflags;
		swh_set->r_st = (swh + i)->r_st;
		swh_set->stid =  (swh + i)->stid;
		if (oodbinsert(swh_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SWITCH")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert swh error!errno=" << _oodb_errno << endl;

			delete swh_set;

			return false;
		}
	}

	delete swh_set;

	cout << "//5 - write co table" <<endl;

	CLS_CO* co;
	DMS::CO* co_set = new DMS::CO;
	size = sizeof(DMS::CO);

	co = co_tb->GetTable();
	count = co_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(co_set,sizeof(DMS::CO));
		co_set->id =  (co + i)->id;
		strcpy(co_set->descr, (co + i)->descr);
		co_set->wloss = (co + i)->wloss;
		co_set->rloss = (co + i)->rloss;
		co_set->wso = (co + i)->wso;
		co_set->rso = (co + i)->rso;
		co_set->wload = (co + i)->wload;
		co_set->rload = (co + i)->rload;
		co_set->cap = (co + i)->cap;
		co_set->reac = (co + i)->reac;
//张瑞雪添加于20120213
		co_set->wloss_b = (co + i)->wloss_b;
		co_set->rloss_b = (co + i)->rloss_b;
		co_set->wso_b = (co + i)->wso_b;
		co_set->rso_b = (co + i)->rso_b;
		co_set->wload_b = (co + i)->wload_b;
		co_set->rload_b = (co + i)->rload_b;
		co_set->cap_b = (co + i)->cap_b;
		co_set->reac_b = (co + i)->reac_b;
		co_set->wloss_c = (co + i)->wloss_c;
		co_set->rloss_c = (co + i)->rloss_c;
		co_set->wso_c = (co + i)->wso_c;
		co_set->rso_c = (co + i)->rso_c;
		co_set->wload_c = (co + i)->wload_c;
		co_set->rload_c = (co + i)->rload_c;
		co_set->cap_c = (co + i)->cap_c;
		co_set->reac_c = (co + i)->reac_c;
		co_set->r_st = (co + i)->r_st;
		//郝泽赟添加于2012.5.03
		co_set->wload_a = (co + i)->wload_a;
		co_set->rload_a = (co + i)->rload_a;
		co_set->wso_a = (co + i)->wso_a;
		co_set->rso_a = (co + i)->rso_a;
		co_set->wloss_a = (co + i)->wloss_a;
		co_set->rloss_a = (co + i)->rloss_a;
		co_set->cap_a = (co + i)->cap_a;
		co_set->reac_a = (co + i)->reac_a;
		if (oodbinsert(co_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("CO")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert co error!errno=" << _oodb_errno << endl;

			delete co_set;

			return false;
		}
	}

	delete co_set;

	cout << "//6 - write cp table" <<endl;

	CLS_CP* cp;
	DMS::CP* cp_set = new DMS::CP;
	size = sizeof(DMS::CP);


	cp = cp_tb->GetTable();
	count = cp_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(cp_set,sizeof(DMS::CP));
		cp_set->id =  (cp + i)->id;
		strcpy(cp_set->descr, (cp + i)->descr);
		cp_set->nd =  (cp + i)->nd;
		cp_set->stid =  (cp + i)->stid;
		cp_set->ind = (cp + i)->ind;
		cp_set->ibs = (cp + i)->ibs;
		cp_set->algq =  (cp + i)->algq;
		cp_set->ialgq = (cp + i)->ialgq;
		cp_set->q = (cp + i)->q;
		cp_set->topflags = (cp + i)->topflags;
		cp_set->mrnom = (cp + i)->mrnom;
		cp_set->r = (cp + i)->r;
		cp_set->r_st = (cp + i)->r_st;
//张瑞雪添加于20120213
		cp_set->algq_b =  (cp + i)->algq_b;
		cp_set->ialgq_b = (cp + i)->ialgq_b;
		cp_set->mrnom_b = (cp + i)->mrnom_b;
		cp_set->r_b = (cp + i)->r_b;
		cp_set->algq_c =  (cp + i)->algq_c;
		cp_set->ialgq_c = (cp + i)->ialgq_c;
		cp_set->mrnom_c = (cp + i)->mrnom_c;
		cp_set->r_c = (cp + i)->r_c;
		//郝泽赟添加于2012.05.03
		cp_set->algq_a =  (cp + i)->algq_a;
		cp_set->ialgq_a = (cp + i)->ialgq_a;
		cp_set->r_a = (cp + i)->r_a;
		if (oodbinsert(cp_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("CP")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert cp error!errno=" << _oodb_errno << endl;

			delete cp_set;

			return false;
		}
	}

	delete cp_set;
//
//	cout << "//7 - write digital table" <<endl;
//
//	CLS_DIGITAL* digital;
//	DMS::DIGITAL* digital_set = new DMS::DIGITAL;
//	size = sizeof(DMS::DIGITAL);
//
//
//	digital = digital_tb->GetTable();
//	count = digital_tb->GetCount();
//	for (i = 0; i < count; i++)
//	{
//        bzero(digital_set,sizeof(DMS::DIGITAL));
//		digital_set->id =  (digital + i)->id;
//		digital_set->descr =  (digital + i)->descr;
//		digital_set->value = (digital + i)->value;
//		digital_set->quality = (digital + i)->quality;
//		digital_set->icb = (digital + i)->icb;
//		digital_set->cb =  (digital + i)->cb;
//
//		if (oodbinsert(digital_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DIGITAL")), 1, size) == OO_FAIL)
//		{
//			cout << "oodbinsert digital error!errno=" << _oodb_errno << endl;
//
//			delete digital_set;
//
//			return false;
//		}
//	}
//
//	delete digital_set;

	cout << "//8 - write global table" <<endl;

	CLS_GLOBAL* global;
	DMS::GLOBAL* global_set = new DMS::GLOBAL;
	size = sizeof(DMS::GLOBAL);


	global = global_tb->GetTable();
	count = global_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(global_set,sizeof(DMS::GLOBAL));
		global_set->id = (global + i)->id;
		global_set->nullbs = (global + i)->nullbs;
		global_set->nextbs = (global + i)->nextbs;
		global_set->tptime = (global + i)->tptime;
		global_set->timedb = (global + i)->timedb;
		global_set->statusdb = (global + i)->statusdb;
		global_set->error = (global + i)->error;
		global_set->dberrnum = (global + i)->dberrnum;
		if (oodbinsert(global_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("GLOBAL")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert global error!errno=" << _oodb_errno << endl;

			delete global_set;

			return false;
		}
	}

	delete global_set;

	cout << "//9 - write island table" <<endl;

	CLS_ISLAND* island;
	DMS::ISLAND* island_set = new DMS::ISLAND;
	size = sizeof(DMS::ISLAND);


	island = island_tb->GetTable();
	count = island_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(island_set,sizeof(DMS::ISLAND));
		island_set->id = (island + i)->id;
		island_set->refbs = (island + i)->refbs;
		island_set->refso = (island + i)->refso;
		island_set->vref = (island + i)->vref;
		island_set->wload = (island + i)->wload;
		island_set->rload = (island + i)->rload;
		island_set->wso = (island + i)->wso;
		island_set->rso = (island + i)->rso;
//张瑞雪添加于20120213
		island_set->vref_b = (island + i)->vref_b;
		island_set->wload_b = (island + i)->wload_b;
		island_set->rload_b = (island + i)->rload_b;
		island_set->wso_b = (island + i)->wso_b;
		island_set->rso_b = (island + i)->rso_b;
		island_set->vref_c = (island + i)->vref_c;
		island_set->wload_c = (island + i)->wload_c;
		island_set->rload_c = (island + i)->rload_c;
		island_set->wso_c = (island + i)->wso_c;
		island_set->rso_c = (island + i)->rso_c;
		//郝泽赟添加于2012.05.03
		island_set->wload_a = (island + i)->wload_a;
		island_set->rload_a = (island + i)->rload_a;
		island_set->wso_a = (island + i)->wso_a;
		island_set->rso_a = (island + i)->rso_a;
        island_set->valid = (island + i)->valid;
		if (oodbinsert(island_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("ISLAND")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert island error!errno=" << _oodb_errno << endl;

			delete island_set;

			return false;
		}
	}

	delete island_set;

	cout << "//10 - write ld table" <<endl;

	CLS_LD* ld;
	DMS::LD* ld_set = new DMS::LD;
	size = sizeof(DMS::LD);


	ld = ld_tb->GetTable();
	count = ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(ld_set,sizeof(DMS::LD));
        ld_set->zn_id =  (ld + i)->zn_id;
		ld_set->id =  (ld + i)->id;
		strcpy(ld_set->descr, (ld + i)->descr);
		ld_set->type = (ld + i)->type;
		ld_set->nd =  (ld + i)->nd;
		ld_set->ind = (ld + i)->ind;
		ld_set->ibs = (ld + i)->ibs;
		ld_set->algp =  (ld + i)->algp;
		ld_set->ialgp = (ld + i)->ialgp;
		ld_set->algq =  (ld + i)->algq;
		ld_set->ialgq = (ld + i)->ialgq;
		ld_set->algi =  (ld + i)->algi;
		ld_set->ialgi = (ld + i)->ialgi;
		ld_set->q = (ld + i)->q;
		ld_set->topflags = (ld + i)->topflags;
		ld_set->w = (ld + i)->w;
		ld_set->r = (ld + i)->r;
		ld_set->wm = (ld + i)->wm;
		ld_set->rm = (ld + i)->rm;
		ld_set->r_st = (ld + i)->r_st;
		ld_set->r_zn = (ld + i)->r_zn;
//张瑞雪添加于20120213
		ld_set->ldtype = (ld + i)->ldtype;
		ld_set->i = (ld + i)->i;
		ld_set->a = (ld + i)->a;
		ld_set->zr = (ld + i)->zr;
		ld_set->zx = (ld + i)->zx;
		ld_set->i_b = (ld + i)->i_b;
		ld_set->a_b = (ld + i)->a_b;
		ld_set->zr_b = (ld + i)->zr_b;
		ld_set->zx_b = (ld + i)->zx_b;
		ld_set->algia =  (ld + i)->algia;
		ld_set->ialgia = (ld + i)->ialgia;
		ld_set->algp_b =  (ld + i)->algp_b;
		ld_set->ialgp_b = (ld + i)->ialgp_b;
		ld_set->algq_b =  (ld + i)->algq_b;
		ld_set->ialgq_b = (ld + i)->ialgq_b;
		ld_set->algi_b =  (ld + i)->algi_b;
		ld_set->stid =  (ld + i)->stid;
		ld_set->ialgi_b = (ld + i)->ialgi_b;
		ld_set->algia_b =  (ld + i)->algia_b;
		ld_set->ialgia_b = (ld + i)->ialgia_b;
		ld_set->w_b = (ld + i)->w_b;
		ld_set->r_b = (ld + i)->r_b;
		ld_set->wm_b = (ld + i)->wm_b;
		ld_set->rm_b = (ld + i)->rm_b;
		ld_set->i_c = (ld + i)->i_c;
		ld_set->a_c = (ld + i)->a_c;
		ld_set->zr_c = (ld + i)->zr_c;
		ld_set->zx_c = (ld + i)->zx_c;
		ld_set->algp_c =  (ld + i)->algp_c;
		ld_set->ialgp_c = (ld + i)->ialgp_c;
		ld_set->algq_c =  (ld + i)->algq_c;
		ld_set->ialgq_c = (ld + i)->ialgq_c;
		ld_set->algi_c =  (ld + i)->algi_c;
		ld_set->ialgi_c = (ld + i)->ialgi_c;
		ld_set->algia_c =  (ld + i)->algia_c;
		ld_set->ialgia_c = (ld + i)->ialgia_c;
		ld_set->w_c = (ld + i)->w_c;
		ld_set->r_c = (ld + i)->r_c;
		ld_set->wm_c = (ld + i)->wm_c;
		ld_set->rm_c = (ld + i)->rm_c;
		//郝泽赟添加于2012.05.03
		ld_set->algp_a =  (ld + i)->algp_a;
		ld_set->ialgp_a = (ld + i)->ialgp_a;
		ld_set->algq_a =  (ld + i)->algq_a;
		ld_set->ialgq_a = (ld + i)->ialgq_a;
		ld_set->algi_a =  (ld + i)->algi_a;
		ld_set->ialgi_a = (ld + i)->ialgi_a;

		ld_set->w_a = (ld + i)->w_a;
		ld_set->r_a = (ld + i)->r_a;

		ld_set->algia_a =  (ld + i)->algia_a;
		ld_set->ialgia_a = (ld + i)->ialgia_a;

		ld_set->zr_a = (ld + i)->zr_a;
		ld_set->zx_a = (ld + i)->zx_a;
		ld_set->i_a = (ld + i)->i_a;
		ld_set->a_ = (ld + i)->a_;
		if (oodbinsert(ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("LD")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert ld error!errno=" << _oodb_errno << endl;

			delete ld_set;

			return false;
		}
	}

	delete ld_set;

	cout << "//12 - write nd table" <<endl;

	CLS_ND* nd;
	DMS::ND* nd_set = new DMS::ND;
	size = sizeof(DMS::ND);


	nd = nd_tb->GetTable();
	count = nd_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(nd_set,sizeof(DMS::ND));
		nd_set->id =  (nd + i)->id;
		nd_set->vl = (nd + i)->vl;
		nd_set->q = (nd + i)->q;
		nd_set->ibs = (nd + i)->ibs;
		nd_set->primebs = (nd + i)->primebs;
		nd_set->fch = (nd + i)->fch;
		nd_set->bch = (nd + i)->bch;
		nd_set->r_cb = (nd + i)->r_cb;
		nd_set->r_switch = (nd + i)->r_switch;
		nd_set->zn_id = (nd + i)->zn_id;
		nd_set->r_xf = (nd + i)->r_xf;
		nd_set->r_sec = (nd + i)->r_sec;
		nd_set->r_cp = (nd+i)->r_cp;
		nd_set->r_ld = (nd+i)->r_ld;
		if (oodbinsert(nd_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("ND")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert nd error!errno=" << _oodb_errno << endl;

			delete nd_set;

			return false;
		}
	}

	delete nd_set;


	cout << "//29 - write so table" <<endl;

	CLS_SO* so;
	DMS::SO* so_set = new DMS::SO;
	size = sizeof(DMS::SO);


	so = so_tb->GetTable();
	count = so_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(so_set,sizeof(DMS::SO));
		so_set->id =  (so + i)->id;
		strcpy(so_set->descr, (so + i)->descr);
		so_set->nd =  (so + i)->nd;
		so_set->ind = (so + i)->ind;
		so_set->ibs = (so + i)->ibs;
		so_set->iisland = (so + i)->iisland;
		so_set->algp =  (so + i)->algp;
		so_set->ialgp = (so + i)->ialgp;
		so_set->algq =  (so + i)->algq;
		so_set->ialgq = (so + i)->ialgq;
		so_set->algi =  (so + i)->algi;
		so_set->ialgi = (so + i)->ialgi;
		so_set->q = (so + i)->q;
		so_set->topflags = (so + i)->topflags;
		so_set->w = (so + i)->w;
		so_set->r = (so + i)->r;
		so_set->i = (so + i)->i;
		so_set->r_st = (so + i)->r_st;
		so_set->ss = (so + i)->ss;
		so_set->sw = (so + i)->sw;
		so_set->sr = (so + i)->sr;
//张瑞雪添加于20120213
		so_set->slackavr = (so + i)->slackavr;
		so_set->v = (so + i)->v;
		so_set->wmx = (so + i)->wmx;
		so_set->rmx = (so + i)->rmx;
		so_set->wmn = (so + i)->wmn;
		so_set->rmn = (so + i)->rmn;
		so_set->algp_b =  (so + i)->algp_b;
		so_set->ialgp_b = (so + i)->ialgp_b;
		so_set->algq_b =  (so + i)->algq_b;
		so_set->ialgq_b = (so + i)->ialgq_b;
		so_set->algi_b =  (so + i)->algi_b;
		so_set->ialgi_b = (so + i)->ialgi_b;
		so_set->w_b = (so + i)->w_b;
		so_set->r_b = (so + i)->r_b;
		so_set->i_b = (so + i)->i_b;
		so_set->v_b = (so + i)->v_b;
		so_set->algp_c =  (so + i)->algp_c;
		so_set->ialgp_c = (so + i)->ialgp_c;
		so_set->algq_c =  (so + i)->algq_c;
		so_set->ialgq_c = (so + i)->ialgq_c;
		so_set->algi_c =  (so + i)->algi_c;
		so_set->ialgi_c = (so + i)->ialgi_c;
		so_set->w_c = (so + i)->w_c;
		so_set->r_c = (so + i)->r_c;
		so_set->i_c = (so + i)->i_c;
		so_set->v_c = (so + i)->v_c;
		//郝泽赟添加于20120426
		so_set->adeg = (so + i)->adeg;
		so_set->adeg_b = (so + i)->adeg_b;
		so_set->adeg_c = (so + i)->adeg_c;
		//郝泽赟添加于2012.05.03
		so_set->algp_a =  (so + i)->algp_a;
		so_set->ialgp_a = (so + i)->ialgp_a;
		so_set->algq_a =  (so + i)->algq_a;
		so_set->ialgq_a = (so + i)->ialgq_a;
		so_set->algi_a =  (so + i)->algi_a;
		so_set->ialgi_a = (so + i)->ialgi_a;

		so_set->w_a = (so + i)->w_a;
		so_set->r_a = (so + i)->r_a;
		so_set->i_a = (so + i)->i_a;
		so_set->v_a = (so + i)->v_a;
		so_set->adeg_a = (so + i)->adeg_a;
		if (oodbinsert(so_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("SO")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert so error!errno=" << _oodb_errno << endl;

			delete so_set;

			return false;
		}
	}

	delete so_set;

	cout << "//30 - write st table" <<endl;

	CLS_ST* st;
	DMS::ST* st_set = new DMS::ST;
	size = sizeof(DMS::ST);


	st = st_tb->GetTable();
	count = st_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(st_set,sizeof(DMS::ST));
		st_set->id =  (st + i)->id;
		strcpy(st_set->descr, (st + i)->descr);
		//st_set->abb =  (st + i)->abb;
		st_set->type = (st + i)->type;
		st_set->qcomp = (st + i)->qcomp;
		st_set->wso = (st + i)->wso;
		st_set->rso = (st + i)->rso;
		st_set->wload = (st + i)->wload;
		st_set->rload = (st + i)->rload;
		st_set->r_co = (st + i)->r_co;
		st_set->r_xfmr = (st + i)->r_xfmr;
		st_set->r_so = (st + i)->r_so;
		st_set->r_cb = (st + i)->r_cb;
		st_set->r_switch = (st + i)->r_switch;
		st_set->r_ld = (st + i)->r_ld;
		st_set->r_cp = (st + i)->r_cp;
		st_set->r_bus = (st + i)->r_bus;
		st_set->r_zn = (st + i)->r_zn;
//张瑞雪添加于20120213
		st_set->wso_b = (st + i)->wso_b;
		st_set->rso_b = (st + i)->rso_b;
		st_set->wload_b = (st + i)->wload_b;
		st_set->rload_b = (st + i)->rload_b;
		st_set->wso_c = (st + i)->wso_c;
		st_set->rso_c = (st + i)->rso_c;
		st_set->wload_c = (st + i)->wload_c;
		st_set->rload_c = (st + i)->rload_c;
		//张瑞雪添加于20120503
		st_set->wso_a = (st + i)->wso_a;
		st_set->rso_a = (st + i)->rso_a;
		st_set->wload_a = (st + i)->wload_a;
		st_set->rload_a = (st + i)->rload_a;
		if (oodbinsert(st_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("ST")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert st error!errno=" << _oodb_errno << endl;

			delete st_set;

			return false;
		}
	}

	delete st_set;

	cout << "//31 - write tapty table" <<endl;

	CLS_TAPTY* tapty;
	DMS::TAPTY* tapty_set = new DMS::TAPTY;
	size = sizeof(DMS::TAPTY);


	tapty = tapty_tb->GetTable();
	count = tapty_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(tapty_set,sizeof(DMS::TAPTY));
		tapty_set->id =  (tapty + i)->id;
		tapty_set->mn = (tapty + i)->mn;
		tapty_set->mx = (tapty + i)->mx;
		tapty_set->nom = (tapty + i)->nom;
		tapty_set->step = (tapty + i)->step;
		if (oodbinsert(tapty_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("TAPTY")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert tapty error!errno=" << _oodb_errno << endl;

			delete tapty_set;

			return false;
		}
	}

	delete tapty_set;

	cout << "//32 - write xf table" <<endl;

	CLS_XF* xf;
	DMS::XF* xf_set = new DMS::XF;
	size = sizeof(DMS::XF);


	xf = xf_tb->GetTable();
	count = xf_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(xf_set,sizeof(DMS::XF));
		xf_set->id =  (xf + i)->id;
		strcpy(xf_set->descr, (xf + i)->descr);
		xf_set->itapty = (xf + i)->itapty;
		xf_set->iztapty = (xf + i)->iztapty;
		xf_set->tap = (xf + i)->tap;
		xf_set->ztap = (xf + i)->ztap;
		xf_set->kvnom = (xf + i)->kvnom;
		xf_set->zkvnom = (xf + i)->zkvnom;
		xf_set->nd =  (xf + i)->nd;
		xf_set->znd =  (xf + i)->znd;
		xf_set->ind = (xf + i)->ind;
		xf_set->iznd = (xf + i)->iznd;
		xf_set->ibs = (xf + i)->ibs;
		xf_set->zbs = (xf + i)->zbs;
		xf_set->mvanom = (xf + i)->mvanom;
		xf_set->q = (xf + i)->q;
		xf_set->topflags = (xf + i)->topflags;
		xf_set->r = (xf + i)->r;
		xf_set->x = (xf + i)->x;
		xf_set->ialgip = (xf + i)->ialgip;
		xf_set->ialgiq = (xf + i)->ialgiq;
		xf_set->ialgii = (xf + i)->ialgii;
		xf_set->ialgit = (xf + i)->ialgit;
		xf_set->ialgzp = (xf + i)->ialgzp;
		xf_set->ialgzq = (xf + i)->ialgzq;
		xf_set->ialgzi = (xf + i)->ialgzi;
		xf_set->ialgzt = (xf + i)->ialgzt;
		xf_set->w = (xf + i)->w;
		xf_set->r_ = (xf + i)->r_;
		xf_set->i = (xf + i)->i;
		xf_set->wz = (xf + i)->wz;
		xf_set->rz = (xf + i)->rz;
		xf_set->iz = (xf + i)->iz;
		xf_set->r_xfmr = (xf + i)->r_xfmr;
//张瑞雪添加于20120215
		xf_set->xftype = (xf + i)->xftype;
		xf_set->itapty_b = (xf + i)->itapty_b;
		xf_set->iztapty_b = (xf + i)->iztapty_b;
		xf_set->tap_b = (xf + i)->tap_b;
		xf_set->ztap_b = (xf + i)->ztap_b;
		xf_set->r_b = (xf + i)->r_b;
		xf_set->x_b = (xf + i)->x_b;
		xf_set->ialgip_b = (xf + i)->ialgip_b;
		xf_set->ialgiq_b = (xf + i)->ialgiq_b;
		xf_set->ialgii_b = (xf + i)->ialgii_b;
		xf_set->ialgit_b = (xf + i)->ialgit_b;
		xf_set->ialgzp_b = (xf + i)->ialgzp_b;
		xf_set->ialgzq_b = (xf + i)->ialgzq_b;
		xf_set->ialgzi_b = (xf + i)->ialgzi_b;
		xf_set->ialgzt_b = (xf + i)->ialgzt_b;
		xf_set->w_b = (xf + i)->w_b;
		xf_set->rq_b = (xf + i)->rq_b;
		xf_set->i_b = (xf + i)->i_b;
		xf_set->wz_b = (xf + i)->wz_b;
		xf_set->rz_b = (xf + i)->rz_b;
		xf_set->iz_b = (xf + i)->iz_b;
		xf_set->itapty_c = (xf + i)->itapty_c;
		xf_set->iztapty_c = (xf + i)->iztapty_c;
		xf_set->tap_c = (xf + i)->tap_c;
		xf_set->ztap_c = (xf + i)->ztap_c;
		xf_set->r_c = (xf + i)->r_c;
		xf_set->x_c = (xf + i)->x_c;
		xf_set->ialgip_c = (xf + i)->ialgip_c;
		xf_set->ialgiq_c = (xf + i)->ialgiq_c;
		xf_set->ialgii_c = (xf + i)->ialgii_c;
		xf_set->ialgit_c = (xf + i)->ialgit_c;
		xf_set->ialgzp_c = (xf + i)->ialgzp_c;
		xf_set->ialgzq_c = (xf + i)->ialgzq_c;
		xf_set->ialgzi_c = (xf + i)->ialgzi_c;
		xf_set->ialgzt_c = (xf + i)->ialgzt_c;
		xf_set->w_c = (xf + i)->w_c;
		xf_set->rq_c = (xf + i)->rq_c;
		xf_set->i_c = (xf + i)->i_c;
		xf_set->wz_c = (xf + i)->wz_c;
		xf_set->rz_c = (xf + i)->rz_c;
		xf_set->iz_c = (xf + i)->iz_c;
		//郝泽赟添加于2012.05.03
		xf_set->r_a = (xf + i)->r_a;
		xf_set->x_a = (xf + i)->x_a;
		xf_set->ialgip_a = (xf + i)->ialgip_a;
		xf_set->ialgiq_a = (xf + i)->ialgiq_a;
		xf_set->ialgii_a = (xf + i)->ialgii_a;
		xf_set->ialgzp_a = (xf + i)->ialgzp_a;
		xf_set->ialgzq_a = (xf + i)->ialgzq_a;
		xf_set->ialgzi_a = (xf + i)->ialgzi_a;
		xf_set->w_a = (xf + i)->w_a;
		xf_set->rq_a = (xf + i)->rq_a;
		xf_set->i_a = (xf + i)->i_a;
		xf_set->wz_a = (xf + i)->wz_a;
		xf_set->rz_a = (xf + i)->rz_a;
		xf_set->iz_a = (xf + i)->iz_a;
		if (oodbinsert(xf_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("XF")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert xf error!errno=" << _oodb_errno << endl;

			delete xf_set;

			return false;
		}
	}

	delete xf_set;

	cout << "//33 - write xfmr table" <<endl;

	CLS_XFMR* xfmr;
	DMS::XFMR* xfmr_set = new DMS::XFMR;
	size = sizeof(DMS::XFMR);


	xfmr = xfmr_tb->GetTable();
	count = xfmr_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(xfmr_set,sizeof(DMS::XFMR));
		xfmr_set->id =  (xfmr + i)->id;
		strcpy(xfmr_set->descr, (xfmr + i)->descr);
		xfmr_set->type = (xfmr + i)->type;
		xfmr_set->st =  (xfmr + i)->st;
		xfmr_set->rh = (xfmr + i)->rh;
		xfmr_set->xh = (xfmr + i)->xh;
		xfmr_set->rm = (xfmr + i)->rm;
		xfmr_set->xm = (xfmr + i)->xm;
		xfmr_set->rl = (xfmr + i)->rl;
		xfmr_set->xl = (xfmr + i)->xl;
		xfmr_set->kvnomh = (xfmr + i)->kvnomh;
		xfmr_set->kvnomm = (xfmr + i)->kvnomm;
		xfmr_set->kvnoml = (xfmr + i)->kvnoml;
		xfmr_set->mvah = (xfmr + i)->mvah;
		xfmr_set->mvam = (xfmr + i)->mvam;
		xfmr_set->mval = (xfmr + i)->mval;
		xfmr_set->algph =  (xfmr + i)->algph;
		xfmr_set->ialgph = (xfmr + i)->ialgph;
		xfmr_set->algqh =  (xfmr + i)->algqh;
		xfmr_set->ialgqh = (xfmr + i)->ialgqh;
		xfmr_set->algih =  (xfmr + i)->algih;
		xfmr_set->ialgih = (xfmr + i)->ialgih;
		xfmr_set->algth =  (xfmr + i)->algth;
		xfmr_set->ialgth = (xfmr + i)->ialgth;
		xfmr_set->algpm =  (xfmr + i)->algpm;
		xfmr_set->ialgpm = (xfmr + i)->ialgpm;
		xfmr_set->algqm =  (xfmr + i)->algqm;
		xfmr_set->ialgqm = (xfmr + i)->ialgqm;
		xfmr_set->algim =  (xfmr + i)->algim;
		xfmr_set->ialgim = (xfmr + i)->ialgim;
		xfmr_set->algtm =  (xfmr + i)->algtm;
		xfmr_set->ialgtm = (xfmr + i)->ialgtm;
		xfmr_set->algpl =  (xfmr + i)->algpl;
		xfmr_set->ialgpl = (xfmr + i)->ialgpl;
		xfmr_set->algql =  (xfmr + i)->algql;
		xfmr_set->ialgql = (xfmr + i)->ialgql;
		xfmr_set->algil =  (xfmr + i)->algil;
		xfmr_set->ialgil = (xfmr + i)->ialgil;
		xfmr_set->algtl =  (xfmr + i)->algtl;
		xfmr_set->ialgtl = (xfmr + i)->ialgtl;
		xfmr_set->hnd =  (xfmr + i)->hnd;
		xfmr_set->mnd =  (xfmr + i)->mnd;
		xfmr_set->lnd =  (xfmr + i)->lnd;
		xfmr_set->htapty =  (xfmr + i)->htapty;
		xfmr_set->mtapty =  (xfmr + i)->mtapty;
		xfmr_set->ltapty =  (xfmr + i)->ltapty;
		xfmr_set->htap = (xfmr + i)->htap;
		xfmr_set->mtap = (xfmr + i)->mtap;
		xfmr_set->ltap = (xfmr + i)->ltap;
		xfmr_set->q = (xfmr + i)->q;
		xfmr_set->topflags = (xfmr + i)->topflags;
		xfmr_set->r_st = (xfmr + i)->r_st;
		xfmr_set->r_xf = (xfmr + i)->r_xf;
//张瑞雪添加于20120215
		xfmr_set->rh_b = (xfmr + i)->rh_b;
		xfmr_set->xh_b = (xfmr + i)->xh_b;
		xfmr_set->rl_b = (xfmr + i)->rl_b;
		xfmr_set->xl_b = (xfmr + i)->xl_b;
		xfmr_set->algph_b =  (xfmr + i)->algph_b;
		xfmr_set->ialgph_b = (xfmr + i)->ialgph_b;
		xfmr_set->algqh_b =  (xfmr + i)->algqh_b;
		xfmr_set->ialgqh_b = (xfmr + i)->ialgqh_b;
		xfmr_set->algih_b =  (xfmr + i)->algih_b;
		xfmr_set->ialgih_b = (xfmr + i)->ialgih_b;
		xfmr_set->algth_b =  (xfmr + i)->algth_b;
		xfmr_set->ialgth_b = (xfmr + i)->ialgth_b;
		xfmr_set->algpl_b =  (xfmr + i)->algpl_b;
		xfmr_set->ialgpl_b = (xfmr + i)->ialgpl_b;
		xfmr_set->algql_b =  (xfmr + i)->algql_b;
		xfmr_set->ialgql_b = (xfmr + i)->ialgql_b;
		xfmr_set->algil_b =  (xfmr + i)->algil_b;
		xfmr_set->ialgil_b = (xfmr + i)->ialgil_b;
		xfmr_set->algtl_b =  (xfmr + i)->algtl_b;
		xfmr_set->ialgtl_b = (xfmr + i)->ialgtl_b;
		xfmr_set->htapty_b =  (xfmr + i)->htapty_b;
		xfmr_set->ltapty_b =  (xfmr + i)->ltapty_b;
		xfmr_set->htap_b = (xfmr + i)->htap_b;
		xfmr_set->ltap_b = (xfmr + i)->ltap_b;
		xfmr_set->rh_c = (xfmr + i)->rh_c;
		xfmr_set->xh_c = (xfmr + i)->xh_c;
		xfmr_set->rl_c = (xfmr + i)->rl_c;
		xfmr_set->xl_c = (xfmr + i)->xl_c;
		xfmr_set->algph_c =  (xfmr + i)->algph_c;
		xfmr_set->ialgph_c = (xfmr + i)->ialgph_c;
		xfmr_set->algqh_c =  (xfmr + i)->algqh_c;
		xfmr_set->ialgqh_c = (xfmr + i)->ialgqh_c;
		xfmr_set->algih_c =  (xfmr + i)->algih_c;
		xfmr_set->ialgih_c = (xfmr + i)->ialgih_c;
		xfmr_set->algth_c =  (xfmr + i)->algth_c;
		xfmr_set->ialgth_c = (xfmr + i)->ialgth_c;
		xfmr_set->algpl_c =  (xfmr + i)->algpl_c;
		xfmr_set->ialgpl_c = (xfmr + i)->ialgpl_c;
		xfmr_set->algql_c =  (xfmr + i)->algql_c;
		xfmr_set->ialgql_c = (xfmr + i)->ialgql_c;
		xfmr_set->algil_c =  (xfmr + i)->algil_c;
		xfmr_set->ialgil_c = (xfmr + i)->ialgil_c;
		xfmr_set->algtl_c =  (xfmr + i)->algtl_c;
		xfmr_set->ialgtl_c = (xfmr + i)->ialgtl_c;
		xfmr_set->htapty_c =  (xfmr + i)->htapty_c;
		xfmr_set->ltapty_c =  (xfmr + i)->ltapty_c;
		xfmr_set->htap_c = (xfmr + i)->htap_c;
		xfmr_set->ltap_c = (xfmr + i)->ltap_c;
		xfmr_set->xftype = (xfmr + i)->xftype;
		//郝泽赟添加于2012.05.03
		xfmr_set->rh_a = (xfmr + i)->rh_a;
		xfmr_set->xh_a = (xfmr + i)->xh_a;
		xfmr_set->rl_a = (xfmr + i)->rl_a;
		xfmr_set->xl_a = (xfmr + i)->xl_a;

		xfmr_set->algph_a =  (xfmr + i)->algph_a;
		xfmr_set->ialgph_a = (xfmr + i)->ialgph_a;

		xfmr_set->algqh_a =  (xfmr + i)->algqh_a;
		xfmr_set->ialgqh_a = (xfmr + i)->ialgqh_a;

		xfmr_set->algih_a =  (xfmr + i)->algih_a;
		xfmr_set->ialgih_a = (xfmr + i)->ialgih_a;

		xfmr_set->algpl_a =  (xfmr + i)->algpl_a;
		xfmr_set->ialgpl_a = (xfmr + i)->ialgpl_a;

		xfmr_set->algql_a =  (xfmr + i)->algql_a;
		xfmr_set->ialgql_a = (xfmr + i)->ialgql_a;

		xfmr_set->algil_a =  (xfmr + i)->algil_a;
		xfmr_set->ialgil_a = (xfmr + i)->ialgil_a;
		if (oodbinsert(xfmr_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("XFMR")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert xfmr error!errno=" << _oodb_errno << endl;

			delete xfmr_set;

			return false;
		}
	}

	delete xfmr_set;

	cout << "//34 - write zn table" <<endl;

	CLS_ZN* zn;
	DMS::ZN* zn_set = new DMS::ZN;
	size = sizeof(DMS::ZN);


	zn = zn_tb->GetTable();
	count = zn_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(zn_set,sizeof(DMS::ZN));
		zn_set->id =  (zn + i)->id;
		strcpy(zn_set->descr, (zn + i)->descr);
		zn_set->type = (zn + i)->type;
		///////////////////////////////////////////////////
		///////////////荣义平添加于20111130为适应超过5个结节的ZN
#if ZN_ND_USE_ARRAY
//		zn_set->nd =  (zn + i)->nds[0];
//		zn_set->znd =  (zn + i)->nds[1];
//		zn_set->nd3 =  (zn + i)->nds[2];
//		zn_set->nd4 =  (zn + i)->nds[3];
//		zn_set->nd5 =  (zn + i)->nds[4];
//		zn_set->nd6 =  (zn + i)->nds[5];
//		zn_set->nd7 =  (zn + i)->nds[6];
//		zn_set->nd8 =  (zn + i)->nds[7];
//		zn_set->nd9 =  (zn + i)->nds[8];
//		zn_set->nd10 =  (zn + i)->nds[9];
//		zn_set->nd11 =  (zn + i)->nds[10];
//		zn_set->nd12 =  (zn + i)->nds[11];
//		zn_set->nd13 =  (zn + i)->nds[12];
//		zn_set->nd14 =  (zn + i)->nds[13];
//		zn_set->nd15 =  (zn + i)->nds[14];
//		zn_set->nd16 =  (zn + i)->nds[15];
//		zn_set->nd17 =  (zn + i)->nds[16];
//		zn_set->nd18 =  (zn + i)->nds[17];
//		zn_set->nd19 =  (zn + i)->nds[18];
//		zn_set->nd20 =  (zn + i)->nds[19];

//		zn_set->ind = (zn + i)->inds[0];
//		zn_set->iznd = (zn + i)->inds[1];
//		zn_set->ind3 = (zn + i)->inds[2];
//		zn_set->ind4 = (zn + i)->inds[3];
//		zn_set->ind5 = (zn + i)->inds[4];
//		zn_set->ind6 = (zn + i)->inds[5];
//		zn_set->ind7 = (zn + i)->inds[6];
//		zn_set->ind8 = (zn + i)->inds[7];
//		zn_set->ind9 = (zn + i)->inds[8];
//		zn_set->ind10 = (zn + i)->inds[9];
//		zn_set->ind11 = (zn + i)->inds[10];
//		zn_set->ind12 = (zn + i)->inds[11];
//		zn_set->ind13 = (zn + i)->inds[12];
//		zn_set->ind14 = (zn + i)->inds[13];
//		zn_set->ind15 = (zn + i)->inds[14];
//		zn_set->ind16 = (zn + i)->inds[15];
//		zn_set->ind17 = (zn + i)->inds[16];
//		zn_set->ind18 = (zn + i)->inds[17];
//		zn_set->ind19 = (zn + i)->inds[18];
//		zn_set->ind20 = (zn + i)->inds[19];


//		zn_set->ibs = (zn + i)->ibss[0];
//		zn_set->zbs = (zn + i)->ibss[1];
//		zn_set->ibs3 = (zn + i)->ibss[2];
//		zn_set->ibs4 = (zn + i)->ibss[3];
//		zn_set->ibs5 = (zn + i)->ibss[4];
//		zn_set->ibs6 = (zn + i)->ibss[5];
//		zn_set->ibs7 = (zn + i)->ibss[6];
//		zn_set->ibs8 = (zn + i)->ibss[7];
//		zn_set->ibs9 = (zn + i)->ibss[8];
//		zn_set->ibs10 = (zn + i)->ibss[9];
//		zn_set->ibs11 = (zn + i)->ibss[10];
//		zn_set->ibs12 = (zn + i)->ibss[11];
//		zn_set->ibs13 = (zn + i)->ibss[12];
//		zn_set->ibs14 = (zn + i)->ibss[13];
//		zn_set->ibs15 = (zn + i)->ibss[14];
//		zn_set->ibs16 = (zn + i)->ibss[15];
//		zn_set->ibs17 = (zn + i)->ibss[16];
//		zn_set->ibs18 = (zn + i)->ibss[17];
//		zn_set->ibs19 = (zn + i)->ibss[18];
//		zn_set->ibs20 = (zn + i)->ibss[19];
#else
		zn_set->nd =  (zn + i)->nd;
		zn_set->znd =  (zn + i)->znd;
		zn_set->nd3 =  (zn + i)->nd3;
		zn_set->nd4 =  (zn + i)->nd4;
		zn_set->nd5 =  (zn + i)->nd5;
		zn_set->ind = (zn + i)->ind;
		zn_set->iznd = (zn + i)->iznd;
		zn_set->ind3 = (zn + i)->ind3;
		zn_set->ind4 = (zn + i)->ind4;
		zn_set->ind5 = (zn + i)->ind5;
		zn_set->ibs = (zn + i)->ibs;
		zn_set->zbs = (zn + i)->zbs;
		zn_set->ibs3 = (zn + i)->ibs3;
		zn_set->ibs4 = (zn + i)->ibs4;
		zn_set->ibs5 = (zn + i)->ibs5;
#endif
//		zn_set->algip =  (zn + i)->algip;
//		zn_set->ialgip = (zn + i)->ialgip;
//		zn_set->algiq =  (zn + i)->algiq;
//		zn_set->ialgiq = (zn + i)->ialgiq;
//		zn_set->algii =  (zn + i)->algii;
//		zn_set->ialgii = (zn + i)->ialgii;
//		zn_set->algzp =  (zn + i)->algzp;
//		zn_set->ialgzp = (zn + i)->ialgzp;
//		zn_set->algzq =  (zn + i)->algzq;
//		zn_set->ialgzq = (zn + i)->ialgzq;
//		zn_set->algzi =  (zn + i)->algzi;
//		zn_set->ialgzi = (zn + i)->ialgzi;
//		zn_set->alg3p =  (zn + i)->alg3p;
//		zn_set->ialg3p = (zn + i)->ialg3p;
//		zn_set->alg3q =  (zn + i)->alg3q;
//		zn_set->ialg3q = (zn + i)->ialg3q;
//		zn_set->alg3i =  (zn + i)->alg3i;
//		zn_set->ialg3i = (zn + i)->ialg3i;
//		zn_set->alg4p =  (zn + i)->alg4p;
//		zn_set->ialg4p = (zn + i)->ialg4p;
//		zn_set->alg4q =  (zn + i)->alg4q;
//		zn_set->ialg4q = (zn + i)->ialg4q;
//		zn_set->alg4i =  (zn + i)->alg4i;
//		zn_set->ialg4i = (zn + i)->ialg4i;
//		zn_set->alg5p =  (zn + i)->alg5p;
//		zn_set->ialg5p = (zn + i)->ialg5p;
//		zn_set->alg5q =  (zn + i)->alg5q;
//		zn_set->ialg5q = (zn + i)->ialg5q;
//		zn_set->alg5i =  (zn + i)->alg5i;
//		zn_set->ialg5i = (zn + i)->ialg5i;
		zn_set->q = (zn + i)->q;
		zn_set->topflags = (zn + i)->topflags;
//		zn_set->w = (zn + i)->w;
//		zn_set->r = (zn + i)->r;
//		zn_set->i = (zn + i)->i;
//		zn_set->wz = (zn + i)->wz;
//		zn_set->rz = (zn + i)->rz;
//		zn_set->iz = (zn + i)->iz;
//		zn_set->w3 = (zn + i)->w3;
//		zn_set->r3 = (zn + i)->r3;
//		zn_set->i3 = (zn + i)->i3;
//		zn_set->w4 = (zn + i)->w4;
//		zn_set->r4 = (zn + i)->r4;
//		zn_set->i4 = (zn + i)->i4;
//		zn_set->w5 = (zn + i)->w5;
//		zn_set->r5 = (zn + i)->r5;
//		zn_set->i5 = (zn + i)->i5;
		zn_set->r_st = (zn + i)->r_st;
		zn_set->r_ld = (zn + i)->r_ld;
		zn_set->r_sec = (zn + i)->r_sec;
		zn_set->r_nd = (zn + i)->r_nd;
		if (oodbinsert(zn_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("ZN")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert zn error!errno=" << _oodb_errno << endl;

			delete zn_set;

			return false;
		}
	}

	delete zn_set;

		cout << "//28 - write sec table" <<endl;

	CLS_SEC* sec;
	DMS::SEC* sec_set = new DMS::SEC;
	size = sizeof(DMS::SEC);


	sec = sec_tb->GetTable();
	count = sec_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(sec_set,sizeof(DMS::SEC));
        sec_set->zn_id =  (sec + i)->zn_id;
		sec_set->id =  (sec + i)->id;
		strcpy(sec_set->descr, (sec + i)->descr);
		sec_set->nd =  (sec + i)->nd;
		sec_set->znd =  (sec + i)->znd;
		sec_set->ind = (sec + i)->ind;
		sec_set->iznd = (sec + i)->iznd;
		sec_set->ibs = (sec + i)->ibs;
		sec_set->zbs = (sec + i)->zbs;
		sec_set->r0 = (sec + i)->r0;
		sec_set->x0 = (sec + i)->x0;
		sec_set->b0 = (sec + i)->b0;
		sec_set->q = (sec + i)->q;
		sec_set->topflags = (sec + i)->topflags;
		sec_set->algip =  (sec + i)->algip;
		sec_set->ialgip = (sec + i)->ialgip;
		sec_set->algiq =  (sec + i)->algiq;
		sec_set->ialgiq = (sec + i)->ialgiq;
		sec_set->algii =  (sec + i)->algii;
		sec_set->ialgii = (sec + i)->ialgii;
		sec_set->algzp =  (sec + i)->algzp;
		sec_set->ialgzp = (sec + i)->ialgzp;
		sec_set->algzq =  (sec + i)->algzq;
		sec_set->ialgzq = (sec + i)->ialgzq;
		sec_set->algzi =  (sec + i)->algzi;
		sec_set->ialgzi = (sec + i)->ialgzi;
		sec_set->w = (sec + i)->w;
		sec_set->r = (sec + i)->r;
		sec_set->i = (sec + i)->i;
		sec_set->wz = (sec + i)->wz;
		sec_set->rz = (sec + i)->rz;
		sec_set->iz = (sec + i)->iz;
		sec_set->r_zn = (sec + i)->r_zn;
//张瑞雪添加于20120213
		sec_set->r0_ab = (sec + i)->r0_ab;
		sec_set->r0_ac = (sec + i)->r0_ac;
		sec_set->r0_ba = (sec + i)->r0_ba;
		sec_set->r0_bb = (sec + i)->r0_bb;
		sec_set->r0_bc = (sec + i)->r0_bc;
		sec_set->r0_ca = (sec + i)->r0_ca;
		sec_set->r0_cb = (sec + i)->r0_cb;
		sec_set->r0_cc = (sec + i)->r0_cc;
		sec_set->x0_ab = (sec + i)->x0_ab;
		sec_set->x0_ac = (sec + i)->x0_ac;
		sec_set->x0_ba = (sec + i)->x0_ba;
		sec_set->x0_bb = (sec + i)->x0_bb;
		sec_set->x0_bc = (sec + i)->x0_bc;
		sec_set->x0_ca = (sec + i)->x0_ca;
		sec_set->x0_cb = (sec + i)->x0_cb;
		sec_set->x0_cc = (sec + i)->x0_cc;
		sec_set->b0_ab = (sec + i)->b0_ab;
		sec_set->b0_ac = (sec + i)->b0_ac;
		sec_set->b0_ba = (sec + i)->b0_ba;
		sec_set->b0_bb = (sec + i)->b0_bb;
		sec_set->b0_bc = (sec + i)->b0_bc;
		sec_set->b0_ca = (sec + i)->b0_ca;
		sec_set->b0_cb = (sec + i)->b0_cb;
		sec_set->b0_cc = (sec + i)->b0_cc;
		sec_set->algip_b =  (sec + i)->algip_b;
		sec_set->ialgip_b = (sec + i)->ialgip_b;
		sec_set->algiq_b =  (sec + i)->algiq_b;
		sec_set->ialgiq_b = (sec + i)->ialgiq_b;
		sec_set->algii_b =  (sec + i)->algii_b;
		sec_set->ialgii_b = (sec + i)->ialgii_b;
		sec_set->algzp_b =  (sec + i)->algzp_b;
		sec_set->ialgzp_b = (sec + i)->ialgzp_b;
		sec_set->algzq_b =  (sec + i)->algzq_b;
		sec_set->ialgzq_b = (sec + i)->ialgzq_b;
		sec_set->algzi_b =  (sec + i)->algzi_b;
		sec_set->ialgzi_b = (sec + i)->ialgzi_b;
		sec_set->w_b = (sec + i)->w_b;
		sec_set->r_b = (sec + i)->r_b;
		sec_set->i_b = (sec + i)->i_b;
		sec_set->wz_b = (sec + i)->wz_b;
		sec_set->rz_b = (sec + i)->rz_b;
		sec_set->iz_b = (sec + i)->iz_b;
		sec_set->algip_c =  (sec + i)->algip_c;
		sec_set->ialgip_c = (sec + i)->ialgip_c;
		sec_set->algiq_c =  (sec + i)->algiq_c;
		sec_set->ialgiq_c = (sec + i)->ialgiq_c;
		sec_set->algii_c =  (sec + i)->algii_c;
		sec_set->ialgii_c = (sec + i)->ialgii_c;
		sec_set->algzp_c =  (sec + i)->algzp_c;
		sec_set->ialgzp_c = (sec + i)->ialgzp_c;
		sec_set->algzq_c =  (sec + i)->algzq_c;
		sec_set->ialgzq_c = (sec + i)->ialgzq_c;
		sec_set->algzi_c =  (sec + i)->algzi_c;
		sec_set->ialgzi_c = (sec + i)->ialgzi_c;
		sec_set->w_c = (sec + i)->w_c;
		sec_set->r_c = (sec + i)->r_c;
		sec_set->i_c = (sec + i)->i_c;
		sec_set->wz_c = (sec + i)->wz_c;
		sec_set->rz_c = (sec + i)->rz_c;
		sec_set->iz_c = (sec + i)->iz_c;
		sec_set->r_fi = (sec+i)->r_fi;
		//张瑞雪添加于2012.4.12
		sec_set->phases = (sec + i)->phases;
		sec_set->l = (sec + i)->l;
		sec_set->mode = (sec + i)->mode;
		//郝泽赟添加于2012.05.03
		sec_set->r0_aa = (sec + i)->r0_aa;
		sec_set->x0_aa = (sec + i)->x0_aa;
		sec_set->b0_aa = (sec + i)->b0_aa;

		sec_set->algip_a =  (sec + i)->algip_a;
		sec_set->ialgip_a = (sec + i)->ialgip_a;
		sec_set->algiq_a =  (sec + i)->algiq_a;
		sec_set->ialgiq_a = (sec + i)->ialgiq_a;
		sec_set->algii_a =  (sec + i)->algii_a;
		sec_set->ialgii_a = (sec + i)->ialgii_a;
		sec_set->algzp_a =  (sec + i)->algzp_a;
		sec_set->ialgzp_a = (sec + i)->ialgzp_a;
		sec_set->algzq_a =  (sec + i)->algzq_a;
		sec_set->ialgzq_a = (sec + i)->ialgzq_a;
		sec_set->algzi_a =  (sec + i)->algzi_a;
		sec_set->ialgzi_a = (sec + i)->ialgzi_a;

		sec_set->w_a = (sec + i)->w_a;
		sec_set->r_a  = (sec + i)->r_a ;
		sec_set->i_a = (sec + i)->i_a;
		sec_set->wz_a = (sec + i)->wz_a;
		sec_set->rz_a = (sec + i)->rz_a;
		sec_set->iz_a = (sec + i)->iz_a;
		if (oodbinsert(sec_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("SEC")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert sec error!errno=" << _oodb_errno << endl;

			delete sec_set;

			return false;
		}
	}

	delete sec_set;

	cout << "//13 - write r_co_st table" <<endl;

	CLS_R_CO_ST* r_co_st;
	DMS::R_CO_ST* r_co_st_set = new DMS::R_CO_ST;
	size = sizeof(DMS::R_CO_ST);


	r_co_st = r_co_st_tb->GetTable();
	count = r_co_st_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_co_st_set,sizeof(DMS::R_CO_ST));
		r_co_st_set->CO_id =  (r_co_st + i)->co_id;
		r_co_st_set->ST_id =  (r_co_st + i)->st_id;
		r_co_st_set->CO_id_ptr = (r_co_st + i)->co_id_ptr;
		r_co_st_set->ST_id_ptr = (r_co_st + i)->st_id_ptr;
		r_co_st_set->fptr = (r_co_st + i)->fptr;
		r_co_st_set->bptr = (r_co_st + i)->bptr;
		if (oodbinsert((void *)r_co_st_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_CO_ST")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_co_st error!errno=" << _oodb_errno << endl;
			delete r_co_st_set;

			return false;
		}
	}

	delete r_co_st_set;

	///////////////////////////////////////////////////////////////////////////////////////////
	cout << "//1 - write r_nd_switch table" <<endl;

	CLS_R_ND_SWITCH* r_nd_switch;
	DMS::R_ND_SWITCH* r_nd_switch_set = new DMS::R_ND_SWITCH;
	size = sizeof(DMS::R_ND_SWITCH);

	r_nd_switch_tb->Sequence();

	r_nd_switch = r_nd_switch_tb->GetTable();
	count = r_nd_switch_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_nd_switch_set, size);
		r_nd_switch_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_SWITCH")), i + 1);
		r_nd_switch_set->ND_id =  (r_nd_switch + i)->nd_id;
		r_nd_switch_set->SWITCH_id =  (r_nd_switch + i)->switch_id;
		r_nd_switch_set->ND_id_ptr = (r_nd_switch + i)->nd_id_ptr;
		r_nd_switch_set->SWITCH_id_ptr = (r_nd_switch + i)->switch_id_ptr;
		r_nd_switch_set->fptr = (r_nd_switch + i)->fptr;
		r_nd_switch_set->bptr = (r_nd_switch + i)->bptr;
		if (oodbinsert(r_nd_switch_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_SWITCH")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_nd_switch error!errno=" << _oodb_errno << endl;

			delete r_nd_switch_set;

			return false;
		}
	}

	delete r_nd_switch_set;
	//////=================================================================
	cout << "//2 - write r_st_switch table" <<endl;

	CLS_R_ST_SWITCH* r_st_switch;
	DMS::R_ST_SWITCH* r_st_switch_set = new DMS::R_ST_SWITCH;
	size = sizeof(DMS::R_ST_SWITCH);

	r_st_switch_tb->Sequence();

	r_st_switch = r_st_switch_tb->GetTable();
	count = r_st_switch_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_st_switch_set, sizeof(DMS::R_ST_SWITCH));
		r_st_switch_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_SWITCH")), i + 1);
		r_st_switch_set->ST_id =  (r_st_switch + i)->st_id;
		r_st_switch_set->SWITCH_id =  (r_st_switch + i)->switch_id;
		r_st_switch_set->ST_id_ptr = (r_st_switch + i)->st_id_ptr;
		r_st_switch_set->SWITCH_id_ptr = (r_st_switch + i)->switch_id_ptr;
		r_st_switch_set->fptr = (r_st_switch + i)->fptr;
		r_st_switch_set->bptr = (r_st_switch + i)->bptr;
		if (oodbinsert(r_st_switch_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_SWITCH")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_switch error!errno=" << _oodb_errno << endl;

			delete r_st_switch_set;

			return false;
		}
	}

	delete r_st_switch_set;
	////////////////////////////////////////////////////////////////////////////////////////////

	cout << "//14 - write r_nd_cb table" <<endl;

	CLS_R_ND_CB* r_nd_cb;
	DMS::R_ND_CB* r_nd_cb_set = new DMS::R_ND_CB;
	size = sizeof(DMS::R_ND_CB);


	r_nd_cb = r_nd_cb_tb->GetTable();
	count = r_nd_cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_nd_cb_set,sizeof(DMS::R_ND_CB));
		r_nd_cb_set->ND_id =  (r_nd_cb + i)->nd_id;
		r_nd_cb_set->CB_id =  (r_nd_cb + i)->cb_id;
		r_nd_cb_set->ND_id_ptr = (r_nd_cb + i)->nd_id_ptr;
		r_nd_cb_set->CB_id_ptr = (r_nd_cb + i)->cb_id_ptr;
		r_nd_cb_set->fptr = (r_nd_cb + i)->fptr;
		r_nd_cb_set->bptr = (r_nd_cb + i)->bptr;
		if (oodbinsert(r_nd_cb_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_CB")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_nd_cb error!errno=" << _oodb_errno << endl;

			delete r_nd_cb_set;

			return false;
		}
	}

	delete r_nd_cb_set;

	cout << "//14 - write r_nd_cp table" << endl;

	CLS_R_ND_CP* r_nd_cp;
	DMS::R_ND_CP* r_nd_cp_set = new DMS::R_ND_CP;
	size = sizeof(DMS::R_ND_CP);

	r_nd_cp = r_nd_cp_tb->GetTable();
	count = r_nd_cp_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_nd_cp_set, sizeof(DMS::R_ND_CP));
		r_nd_cp_set->ND_id =  (r_nd_cp + i)->nd_id;
		r_nd_cp_set->CP_id =  (r_nd_cp + i)->cp_id;
		r_nd_cp_set->ND_id_ptr = (r_nd_cp + i)->nd_id_ptr;
		r_nd_cp_set->CP_id_ptr = (r_nd_cp + i)->cp_id_ptr;
		r_nd_cp_set->fptr = (r_nd_cp + i)->fptr;
		r_nd_cp_set->bptr = (r_nd_cp + i)->bptr;
		if (oodbinsert(r_nd_cp_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_CP")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_nd_cp error!errno=" << _oodb_errno << endl;

			delete r_nd_cb_set;

			return false;
		}
	}

	delete r_nd_cp_set;

	cout << "//14 - write r_nd_ld table" << endl;

	CLS_R_ND_LD* r_nd_ld;
	DMS::R_ND_LD* r_nd_ld_set = new DMS::R_ND_LD;
	size = sizeof(DMS::R_ND_LD);

	r_nd_ld = r_nd_ld_tb->GetTable();
	count = r_nd_ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_nd_ld_set, sizeof(DMS::R_ND_LD));
		r_nd_ld_set->ND_id =  (r_nd_ld + i)->nd_id;
		r_nd_ld_set->LD_id =  (r_nd_ld + i)->ld_id;
		r_nd_ld_set->ND_id_ptr = (r_nd_ld + i)->nd_id_ptr;
		r_nd_ld_set->LD_id_ptr = (r_nd_ld + i)->ld_id_ptr;
		r_nd_ld_set->fptr = (r_nd_ld + i)->fptr;
		r_nd_ld_set->bptr = (r_nd_ld + i)->bptr;
		if (oodbinsert(r_nd_ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_LD")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_nd_ld error!errno=" << _oodb_errno << endl;

			delete r_nd_ld_set;

			return false;
		}
	}

	delete r_nd_ld_set;

	cout << "//15 - write r_nd_sec table" <<endl;

	CLS_R_ND_SEC* r_nd_sec;
	DMS::R_ND_SEC* r_nd_sec_set = new DMS::R_ND_SEC;
	size = sizeof(DMS::R_ND_SEC);


	r_nd_sec = r_nd_sec_tb->GetTable();
	count = r_nd_sec_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_nd_sec_set,sizeof(DMS::R_ND_SEC));
		r_nd_sec_set->ND_id =  (r_nd_sec + i)->nd_id;
		r_nd_sec_set->SEC_id =  (r_nd_sec + i)->sec_id;
		r_nd_sec_set->ND_id_ptr = (r_nd_sec + i)->nd_id_ptr;
		r_nd_sec_set->SEC_id_ptr = (r_nd_sec + i)->sec_id_ptr;
		r_nd_sec_set->fptr = (r_nd_sec + i)->fptr;
		r_nd_sec_set->bptr = (r_nd_sec + i)->bptr;
		if (oodbinsert(r_nd_sec_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_SEC")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_nd_sec error!errno=" << _oodb_errno << endl;

			delete r_nd_sec_set;

			return false;
		}
	}

	delete r_nd_sec_set;

	cout << "//16 - write r_nd_xf table" <<endl;

	CLS_R_ND_XF* r_nd_xf;
	DMS::R_ND_XF* r_nd_xf_set = new DMS::R_ND_XF;
	size = sizeof(DMS::R_ND_XF);


	r_nd_xf = r_nd_xf_tb->GetTable();
	count = r_nd_xf_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_nd_xf_set,sizeof(DMS::R_ND_XF));
		r_nd_xf_set->ND_id =  (r_nd_xf + i)->nd_id;
		r_nd_xf_set->XF_id =  (r_nd_xf + i)->xf_id;
		r_nd_xf_set->ND_id_ptr = (r_nd_xf + i)->nd_id_ptr;
		r_nd_xf_set->XF_id_ptr = (r_nd_xf + i)->xf_id_ptr;
		r_nd_xf_set->fptr = (r_nd_xf + i)->fptr;
		r_nd_xf_set->bptr = (r_nd_xf + i)->bptr;
		if (oodbinsert(r_nd_xf_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ND_XF")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_nd_xf error!errno=" << _oodb_errno << endl;

			delete r_nd_xf_set;

			return false;
		}
	}

	delete r_nd_xf_set;

	cout << "//17 - write r_zn_nd table" <<endl;

	CLS_R_ZN_ND* r_zn_nd;
	DMS::R_ZN_ND* r_zn_nd_set = new DMS::R_ZN_ND;
	size = sizeof(DMS::R_ZN_ND);


	r_zn_nd = r_zn_nd_tb->GetTable();
	count = r_zn_nd_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_zn_nd_set,sizeof(DMS::R_ZN_ND));
		r_zn_nd_set->ND_id =  (r_zn_nd + i)->nd_id;
		r_zn_nd_set->ZN_id =  (r_zn_nd + i)->zn_id;
		r_zn_nd_set->ND_id_ptr = (r_zn_nd + i)->nd_id_ptr;
		r_zn_nd_set->ZN_id_ptr = (r_zn_nd + i)->zn_id_ptr;
		r_zn_nd_set->fptr = (r_zn_nd + i)->fptr;
		r_zn_nd_set->bptr = (r_zn_nd + i)->bptr;
		if (oodbinsert(r_zn_nd_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_ND")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_zn_nd error!errno=" << _oodb_errno << endl;

			delete r_zn_nd_set;

			return false;
		}
	}

	delete r_zn_nd_set;

	cout << "//18 - write r_st_bus table" <<endl;

	CLS_R_ST_BUS* r_st_bus;
	DMS::R_ST_BUS* r_st_bus_set = new DMS::R_ST_BUS;
	size = sizeof(DMS::R_ST_BUS);


	r_st_bus = r_st_bus_tb->GetTable();
	count = r_st_bus_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_bus_set,sizeof(DMS::R_ST_BUS));
		r_st_bus_set->ST_id =  (r_st_bus + i)->st_id;
		r_st_bus_set->BUS_id =  (r_st_bus + i)->bus_id;
		r_st_bus_set->ST_id_ptr = (r_st_bus + i)->st_id_ptr;
		r_st_bus_set->BUS_id_ptr = (r_st_bus + i)->bus_id_ptr;
		r_st_bus_set->fptr = (r_st_bus + i)->fptr;
		r_st_bus_set->bptr = (r_st_bus + i)->bptr;
		if (oodbinsert(r_st_bus_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_BUS")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_bus error!errno=" << _oodb_errno << endl;

			delete r_st_bus_set;

			return false;
		}
	}

	delete r_st_bus_set;

	cout << "//19 - write r_st_cb table" <<endl;

	CLS_R_ST_CB* r_st_cb;
	DMS::R_ST_CB* r_st_cb_set = new DMS::R_ST_CB;
	size = sizeof(DMS::R_ST_CB);


	r_st_cb = r_st_cb_tb->GetTable();
	count = r_st_cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_cb_set,sizeof(DMS::R_ST_CB));
		r_st_cb_set->ST_id =  (r_st_cb + i)->st_id;
		r_st_cb_set->CB_id =  (r_st_cb + i)->cb_id;
		r_st_cb_set->ST_id_ptr = (r_st_cb + i)->st_id_ptr;
		r_st_cb_set->CB_id_ptr = (r_st_cb + i)->cb_id_ptr;
		r_st_cb_set->fptr = (r_st_cb + i)->fptr;
		r_st_cb_set->bptr = (r_st_cb + i)->bptr;
		if (oodbinsert(r_st_cb_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_CB")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_cb error!errno=" << _oodb_errno << endl;

			delete r_st_cb_set;

			return false;
		}
	}

	delete r_st_cb_set;

	cout << "//20 - write r_st_cp table" <<endl;

	CLS_R_ST_CP* r_st_cp;
	DMS::R_ST_CP* r_st_cp_set = new DMS::R_ST_CP;
	size = sizeof(DMS::R_ST_CP);


	r_st_cp = r_st_cp_tb->GetTable();
	count = r_st_cp_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_cp_set,sizeof(DMS::R_ST_CP));
		r_st_cp_set->ST_id =  (r_st_cp + i)->st_id;
		r_st_cp_set->CP_id =  (r_st_cp + i)->cp_id;
		r_st_cp_set->ST_id_ptr = (r_st_cp + i)->st_id_ptr;
		r_st_cp_set->CP_id_ptr = (r_st_cp + i)->cp_id_ptr;
		r_st_cp_set->fptr = (r_st_cp + i)->fptr;
		r_st_cp_set->bptr = (r_st_cp + i)->bptr;
		if (oodbinsert(r_st_cp_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_CP")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_cp error!errno=" << _oodb_errno << endl;

			delete r_st_cp_set;

			return false;
		}
	}

	delete r_st_cp_set;

	cout << "//21 - write r_st_ld table" <<endl;

	CLS_R_ST_LD* r_st_ld;
	DMS::R_ST_LD* r_st_ld_set = new DMS::R_ST_LD;
	size = sizeof(DMS::R_ST_LD);


	r_st_ld = r_st_ld_tb->GetTable();
	count = r_st_ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_ld_set,sizeof(DMS::R_ST_LD));
		r_st_ld_set->ST_id =  (r_st_ld + i)->st_id;
		r_st_ld_set->LD_id =  (r_st_ld + i)->ld_id;
		r_st_ld_set->ST_id_ptr = (r_st_ld + i)->st_id_ptr;
		r_st_ld_set->LD_id_ptr = (r_st_ld + i)->ld_id_ptr;
		r_st_ld_set->fptr = (r_st_ld + i)->fptr;
		r_st_ld_set->bptr = (r_st_ld + i)->bptr;
		if (oodbinsert(r_st_ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_LD")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_ld error!errno=" << _oodb_errno << endl;

			delete r_st_ld_set;

			return false;
		}
	}

	delete r_st_ld_set;

	cout << "//22 - write r_st_so table" <<endl;

	CLS_R_ST_SO* r_st_so;
	DMS::R_ST_SO* r_st_so_set = new DMS::R_ST_SO;
	size = sizeof(DMS::R_ST_SO);


	r_st_so = r_st_so_tb->GetTable();
	count = r_st_so_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_so_set,sizeof(DMS::R_ST_SO));
		r_st_so_set->ST_id =  (r_st_so + i)->st_id;
		r_st_so_set->SO_id =  (r_st_so + i)->so_id;
		r_st_so_set->ST_id_ptr = (r_st_so + i)->st_id_ptr;
		r_st_so_set->SO_id_ptr = (r_st_so + i)->so_id_ptr;
		r_st_so_set->fptr = (r_st_so + i)->fptr;
		r_st_so_set->bptr = (r_st_so + i)->bptr;
		if (oodbinsert(r_st_so_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_SO")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_so error!errno=" << _oodb_errno << endl;

			delete r_st_so_set;

			return false;
		}
	}

	delete r_st_so_set;

	cout << "//23 - write r_st_xfmr table" <<endl;

	CLS_R_ST_XFMR* r_st_xfmr;
	DMS::R_ST_XFMR* r_st_xfmr_set = new DMS::R_ST_XFMR;
	size = sizeof(DMS::R_ST_XFMR);


	r_st_xfmr = r_st_xfmr_tb->GetTable();
	count = r_st_xfmr_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_st_xfmr_set,sizeof(DMS::R_ST_XFMR));
		r_st_xfmr_set->ST_id =  (r_st_xfmr + i)->st_id;
		r_st_xfmr_set->XFMR_id =  (r_st_xfmr + i)->xfmr_id;
		r_st_xfmr_set->ST_id_ptr = (r_st_xfmr + i)->st_id_ptr;
		r_st_xfmr_set->XFMR_id_ptr = (r_st_xfmr + i)->xfmr_id_ptr;
		r_st_xfmr_set->fptr = (r_st_xfmr + i)->fptr;
		r_st_xfmr_set->bptr = (r_st_xfmr + i)->bptr;
		if (oodbinsert(r_st_xfmr_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_XFMR")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_xfmr error!errno=" << _oodb_errno << endl;

			delete r_st_xfmr_set;

			return false;
		}
	}

	delete r_st_xfmr_set;

	cout << "//24 - write r_st_zn table" <<endl;

	CLS_R_ST_ZN* r_st_zn;
	DMS::R_ST_ZN* r_st_zn_set = new DMS::R_ST_ZN;
	size = sizeof(DMS::R_ST_ZN);


	r_st_zn = r_st_zn_tb->GetTable();
	count = r_st_zn_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_zn_set,sizeof(DMS::R_ST_ZN));
		r_st_zn_set->ST_id =  (r_st_zn + i)->st_id;
		r_st_zn_set->ZN_id =  (r_st_zn + i)->zn_id;
		r_st_zn_set->ST_id_ptr = (r_st_zn + i)->st_id_ptr;
		r_st_zn_set->ZN_id_ptr = (r_st_zn + i)->zn_id_ptr;
		r_st_zn_set->fptr = (r_st_zn + i)->fptr;
		r_st_zn_set->bptr = (r_st_zn + i)->bptr;
		if (oodbinsert(r_st_zn_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ST_ZN")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_st_zn error!errno=" << _oodb_errno << endl;

			delete r_st_zn_set;

			return false;
		}
	}

	delete r_st_zn_set;

	cout << "//25 - write r_xfmr_xf table" <<endl;

	CLS_R_XFMR_XF* r_xfmr_xf;
	DMS::R_XFMR_XF* r_xfmr_xf_set = new DMS::R_XFMR_XF;
	size = sizeof(DMS::R_XFMR_XF);


	r_xfmr_xf = r_xfmr_xf_tb->GetTable();
	count = r_xfmr_xf_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_xfmr_xf_set,sizeof(DMS::R_XFMR_XF));
		r_xfmr_xf_set->XFMR_id =  (r_xfmr_xf + i)->xfmr_id;
		r_xfmr_xf_set->XF_id =  (r_xfmr_xf + i)->xf_id;
		r_xfmr_xf_set->XFMR_id_ptr = (r_xfmr_xf + i)->xfmr_id_ptr;
		r_xfmr_xf_set->XF_id_ptr = (r_xfmr_xf + i)->xf_id_ptr;
		r_xfmr_xf_set->fptr = (r_xfmr_xf + i)->fptr;
		r_xfmr_xf_set->bptr = (r_xfmr_xf + i)->bptr;
		if (oodbinsert(r_xfmr_xf_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_XFMR_XF")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_xfmr_xf error!errno=" << _oodb_errno << endl;

			delete r_xfmr_xf_set;

			return false;
		}
	}

	delete r_xfmr_xf_set;

	cout << "//26 - write r_zn_ld table" <<endl;

	CLS_R_ZN_LD* r_zn_ld;
	DMS::R_ZN_LD* r_zn_ld_set = new DMS::R_ZN_LD;
	size = sizeof(DMS::R_ZN_LD);


	r_zn_ld = r_zn_ld_tb->GetTable();
	count = r_zn_ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_zn_ld_set,sizeof(DMS::R_ZN_LD));
		r_zn_ld_set->ZN_id =  (r_zn_ld + i)->zn_id;
		r_zn_ld_set->LD_id =  (r_zn_ld + i)->ld_id;
		r_zn_ld_set->ZN_id_ptr = (r_zn_ld + i)->zn_id_ptr;
		r_zn_ld_set->LD_id_ptr = (r_zn_ld + i)->ld_id_ptr;
		r_zn_ld_set->fptr = (r_zn_ld + i)->fptr;
		r_zn_ld_set->bptr = (r_zn_ld + i)->bptr;
		if (oodbinsert(r_zn_ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_LD")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_zn_ld error!errno=" << _oodb_errno << endl;

			delete r_zn_ld_set;

			return false;
		}
	}

	delete r_zn_ld_set;

	cout << "//27 - write r_zn_sec table" <<endl;

	CLS_R_ZN_SEC* r_zn_sec;
	DMS::R_ZN_SEC* r_zn_sec_set = new DMS::R_ZN_SEC;
	size = sizeof(DMS::R_ZN_SEC);


	r_zn_sec = r_zn_sec_tb->GetTable();
	count = r_zn_sec_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_zn_sec_set,sizeof(DMS::R_ZN_SEC));
		r_zn_sec_set->ZN_id =  (r_zn_sec + i)->zn_id;
		r_zn_sec_set->SEC_id =  (r_zn_sec + i)->sec_id;
		r_zn_sec_set->ZN_id_ptr = (r_zn_sec + i)->zn_id_ptr;
		r_zn_sec_set->SEC_id_ptr = (r_zn_sec + i)->sec_id_ptr;
		r_zn_sec_set->fptr = (r_zn_sec + i)->fptr;
		r_zn_sec_set->bptr = (r_zn_sec + i)->bptr;
		if (oodbinsert(r_zn_sec_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("r_ZN_SEC")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_zn_sec error!errno=" << _oodb_errno << endl;

			delete r_zn_sec_set;

			return false;
		}
	}

	delete r_zn_sec_set;

	cout << "//35 - write fi table" << endl;

	CLS_FI* fi;
	DMS::FI* fi_set = new DMS::FI;
	size = sizeof(DMS::FI);

	fi = fi_tb->GetTable();
	count = fi_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(fi_set, sizeof(DMS::FI));

		fi_set->id =  (fi + i)->id;
		strcpy(fi_set->descr, (fi + i)->descr);
		fi_set->pid = (fi + i)->pid;
		fi_set->digital =  (fi + i)->digital;
		fi_set->sec =  (fi + i)->sec;
		fi_set->idigital = (fi + i)->idigital;
		fi_set->pos = (fi + i)->pos;
		fi_set->r_sec = (fi + i)->r_sec;

		if (oodbinsert(fi_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("FI")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert fi error!errno=" << _oodb_errno << endl;

			delete fi_set;

			return false;
		}
	}

	delete fi_set;

	cout << "//36 - write r_sec_fi table" << endl;

	CLS_R_SEC_FI* r_sec_fi;
	DMS::R_SEC_FI* r_sec_fi_set = new DMS::R_SEC_FI;
	size = sizeof(DMS::R_SEC_FI);

	r_sec_fi = r_sec_fi_tb->GetTable();
	count = r_sec_fi_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_sec_fi_set, sizeof(DMS::R_SEC_FI));

		r_sec_fi_set->sec_id =  (r_sec_fi + i)->sec_id;
		r_sec_fi_set->fi_id =  (r_sec_fi + i)->fi_id;
		r_sec_fi_set->fi_id_ptr = (r_sec_fi + i)->fi_id_ptr;
		r_sec_fi_set->sec_id_ptr = (r_sec_fi + i)->sec_id_ptr;
		r_sec_fi_set->fptr = (r_sec_fi + i)->fptr;
		r_sec_fi_set->bptr = (r_sec_fi + i)->bptr;

		if (oodbinsert(r_sec_fi_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("R_SEC_FI")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_sec_fi error!errno=" << _oodb_errno << endl;

			delete r_sec_fi_set;

			return false;
		}
	}

	delete r_sec_fi_set;

	return true;
}
#if 0
bool DSBOB::WritePSBOB()
{
	cout<<"写DSBOB库……"<<endl;
	int i = 0,count = 0,size = 0;
	CLS_ANALOG* analog;
	PSBOB::ANALOG* analog_set = new PSBOB::ANALOG;
	size = sizeof(PSBOB::ANALOG);

	analog = analog_tb->GetTable();
	count = analog_tb->GetCount();
	for(i = 0;i < count;i ++)
	{
		analog_set->id = (analog + i)->id;
		analog_set->descr = (analog + i)->descr;
		analog_set->value = (analog + i)->value;
		analog_set->quality = (analog + i)->quality;

		if (oodbinsert(analog_set, const_cast<TB_DESCR*>(g_db_psbob->GetTB("analog")),1,size) == OO_FAIL)
		{
			cout<<"写analog表失败，errno:"<<_oodb_errno<<",line:"<<__LINE__<<endl;
			delete analog;
			delete analog_set;
			return false;
		}
	}
	delete analog_set;
	return true;
}
#endif
void DSBOB::Sequence()
{
	//seqence data in each table
//	digital_tb->Sequence();
//	analog_tb->Sequence();
	co_tb->Sequence();
	st_tb->Sequence();
	xfmr_tb->Sequence();
	xf_tb->Sequence();
	zn_tb->Sequence();
	cb_tb->Sequence();
	so_tb->Sequence();
	ld_tb->Sequence();
	cp_tb->Sequence();
	bus_tb->Sequence();
	sec_tb->Sequence();
	nd_tb->Sequence();
	bs_tb->Sequence();
	island_tb->Sequence();
	global_tb->Sequence();
	tapty_tb->Sequence();
	r_co_st_tb->Sequence();
	r_st_xfmr_tb->Sequence();
	r_xfmr_xf_tb->Sequence();
	r_st_cb_tb->Sequence();
	r_st_so_tb->Sequence();
	r_st_ld_tb->Sequence();
	r_st_cp_tb->Sequence();
	r_st_bus_tb->Sequence();
	r_st_zn_tb->Sequence();
	r_zn_ld_tb->Sequence();
	r_zn_sec_tb->Sequence();
	r_nd_cb_tb->Sequence();
	r_nd_xf_tb->Sequence();
	r_zn_nd_tb->Sequence();
	r_nd_sec_tb->Sequence();

//	lnmode_tb->Sequence();
}

int DSBOB::GetDevStatusByST(const long st_id, int cb_status, int ld_status, list<DEV_STATUS> &lst_dev)
{
	lst_dev.clear();

	//在关系表r_st_cb中获取si_id对应的cb
	ChildPtr *ptr = r_st_cb_tb->FindChildPtr(st_id);
	ChildPtr *ptr1 = ptr;

	while (ptr != NULL)
	{
		PSBOB::CB psbob_cb = { 0 };
		CLS_CB *cb = cb_tb->GetDataByPos(ptr->Ptr);

		//获取CB的挂牌信息
		if (oodbread_rk(&psbob_cb, &cb->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(psbob_cb)) <= 0)
		{
			printf("%s(%d):读取开关%ld记录信息失败，错误号:%d\r\n", __FUNCTION__, __LINE__, cb->id, _oodb_errno);
		}
		else
		{
			if (psbob_cb.status & cb_status)
			{
				lst_dev.push_back(DEV_STATUS(cb->id, "cb", psbob_cb.status));
			}
		}

		ptr = ptr->Next;
	}

	FreeChildPtr(ptr1);

	//在关系表r_st_ld中获取si_id对应的ld
	ptr = r_st_ld_tb->FindChildPtr(st_id);
	ptr1 = ptr;

	while (ptr != NULL)
	{
		PSBOB::LD psbob_ld = { 0 };
		CLS_LD *ld = ld_tb->GetDataByPos(ptr->Ptr);

		//获取LD的挂牌信息
		if (oodbread_rk(&psbob_ld, &ld->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), sizeof(psbob_ld)) <= 0)
		{
			printf("%s(%d):读取负荷%ld记录信息失败，错误号:%d\r\n", __FUNCTION__, __LINE__, ld->id, _oodb_errno);
		}
		else
		{
			if (psbob_ld.status & ld_status)
			{
				lst_dev.push_back(DEV_STATUS(ld->id, "ld", psbob_ld.status));
			}
		}

		ptr = ptr->Next;
	}

	FreeChildPtr(ptr1);

	ptr = r_st_zn_tb->FindChildPtr(st_id);
	ptr1 = ptr;

	while (ptr != NULL)
	{
		CLS_ZN *zn = zn_tb->GetDataByPos(ptr->Ptr);

		ChildPtr *tmp_ptr = r_zn_ld_tb->FindChildPtr(zn->id);
		ChildPtr *tmp_ptr1 = tmp_ptr;

		while (tmp_ptr != NULL)
		{
			PSBOB::LD psbob_ld = { 0 };
			CLS_LD *ld = ld_tb->GetDataByPos(tmp_ptr->Ptr);

			//获取LD的挂牌信息
			if (oodbread_rk(&psbob_ld, &ld->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), sizeof(psbob_ld)) <= 0)
			{
				printf("%s(%d):读取负荷%ld记录信息失败，错误号:%d\r\n", __FUNCTION__, __LINE__, ld->id, _oodb_errno);
			}
			else
			{
				if (psbob_ld.status & ld_status)
				{
					lst_dev.push_back(DEV_STATUS(ld->id, "ld", psbob_ld.status));
				}
			}

			tmp_ptr = tmp_ptr->Next;
		}

		FreeChildPtr(tmp_ptr1);

		ptr = ptr->Next;
	}

	FreeChildPtr(ptr);

	return DSBOB_SUCCESS;
}
