//******************************************************************************
//
//  DSBOB2.CPP
//  EMS Project - Copyright (c) 2000, EPRI
//  Methods implemention of distribution network database object
//
//******************************************************************************

#include "dsbob2.h"
#define ONLY_GZC_DB 	false	//从PSBOB导库的时候只导故障测试馈线的记录，方便快速调试

char *err_descr[] = { "结点存在空值", "电压类型为空", "非10kV电压类型设备", "厂站类型为变电站", "两个结点值相同" };
#define ERR_INDEX_ND 		0
#define ERR_INDEX_BV 		1
#define ERR_INDEX_BV_VALUE 	2
#define ERR_INDEX_ST 		3
#define ERR_INDEX_SAME_ND	4

////////////////////////////////////////////////////////////////////////////////
//  constructor
DSBOB2::DSBOB2()
{
	bs_tb           = new DTable<CLS_BS>(GetTableSize(g_db_dms->GetTB("DMS_BS")));
	bus_tb          = new DTable<CLS_BUS>(GetTableSize(g_db_dms->GetTB("DMS_BUS")));
	cb_tb           = new DTable<CLS_CB>(GetTableSize(g_db_dms->GetTB("DMS_CB")));
	switch_tb       = new DTable<CLS_SWITCH>(GetTableSize(g_db_dms->GetTB("DMS_SWITCH")));
	co_tb           = new DTable<CLS_CO>(GetTableSize(g_db_dms->GetTB("DMS_CO")));
	cp_tb           = new DTable<CLS_CP>(GetTableSize(g_db_dms->GetTB("DMS_CP")));
	global_tb       = new DTable<CLS_GLOBAL>(GetTableSize(g_db_dms->GetTB("DMS_GLOBAL")));
	island_tb       = new DTable<CLS_ISLAND>(GetTableSize(g_db_dms->GetTB("DMS_ISLAND")));
	ld_tb           = new DTable<CLS_LD>(GetTableSize(g_db_dms->GetTB("DMS_LD")));
	nd_tb           = new DTable<CLS_ND>(GetTableSize(g_db_dms->GetTB("DMS_ND")));
	r_co_st_tb      = new DTable<CLS_R_CO_ST>(GetTableSize(g_db_dms->GetTB("DMS_r_CO_ST")));
	r_nd_cb_tb      = new DTable<CLS_R_ND_CB>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_CB")));
	r_nd_switch_tb  = new DTable<CLS_R_ND_SWITCH>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_SWITCH")));
	r_nd_sec_tb     = new DTable<CLS_R_ND_SEC>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_SEC")));
	r_nd_xf_tb      = new DTable<CLS_R_ND_XF>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_XF")));
	r_zn_nd_tb      = new DTable<CLS_R_ZN_ND>(GetTableSize(g_db_dms->GetTB("DMS_r_ZN_ND")));
	r_st_bus_tb     = new DTable<CLS_R_ST_BUS>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_BUS")));
	r_st_cb_tb      = new DTable<CLS_R_ST_CB>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_CB")));
	r_st_switch_tb  = new DTable<CLS_R_ST_SWITCH>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_SWITCH")));
	r_st_cp_tb      = new DTable<CLS_R_ST_CP>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_CP")));
	r_st_ld_tb      = new DTable<CLS_R_ST_LD>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_LD")));
	r_st_so_tb      = new DTable<CLS_R_ST_SO>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_SO")));
	r_st_xfmr_tb    = new DTable<CLS_R_ST_XFMR>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_XFMR")));
	r_st_zn_tb      = new DTable<CLS_R_ST_ZN>(GetTableSize(g_db_dms->GetTB("DMS_r_ST_ZN")));
	r_xfmr_xf_tb    = new DTable<CLS_R_XFMR_XF>(GetTableSize(g_db_dms->GetTB("DMS_r_XFMR_XF")));
	r_zn_ld_tb      = new DTable<CLS_R_ZN_LD>(GetTableSize(g_db_dms->GetTB("DMS_r_ZN_LD")));
	r_zn_sec_tb     = new DTable<CLS_R_ZN_SEC>(GetTableSize(g_db_dms->GetTB("DMS_r_ZN_SEC")));
	sec_tb          = new DTable<CLS_SEC>(GetTableSize(g_db_dms->GetTB("DMS_SEC")));
	so_tb           = new DTable<CLS_SO>(GetTableSize(g_db_dms->GetTB("DMS_SO")));
	st_tb           = new DTable<CLS_ST>(GetTableSize(g_db_dms->GetTB("DMS_ST")));
	tapty_tb        = new DTable<CLS_TAPTY>(GetTableSize(g_db_dms->GetTB("DMS_TAPTY")));
	xf_tb           = new DTable<CLS_XF>(GetTableSize(g_db_dms->GetTB("DMS_XF")));
	xfmr_tb         = new DTable<CLS_XFMR>(GetTableSize(g_db_dms->GetTB("DMS_XFMR")));
	zn_tb           = new DTable<CLS_ZN>(GetTableSize(g_db_dms->GetTB("DMS_ZN")));
	fi_tb			= new DTable<CLS_FI>(GetTableSize(g_db_dms->GetTB("DMS_FI")));
	r_sec_fi_tb		= new DTable<CLS_R_SEC_FI>(GetTableSize(g_db_dms->GetTB("DMS_R_SEC_FI")));
//	ctl_tb			= new DTable<CLS_CTL>(GetTableSize(g_db_dms->GetTB("DMS_CTL")));
//	lnmode_tb		= new DTable<CLS_LNMODE>(GetTableSize(g_db_dms->GetTB("DMS_LNMODE")));
	r_nd_cp_tb		= new DTable<CLS_R_ND_CP>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_CP")));
	r_nd_ld_tb		= new DTable<CLS_R_ND_LD>(GetTableSize(g_db_dms->GetTB("DMS_r_ND_LD")));
}

////////////////////////////////////////////////////////////////////////////////
//  destructor
DSBOB2::~DSBOB2()
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
	delete switch_tb;
	delete r_st_switch_tb;
	delete r_nd_switch_tb;
//	delete lnmode_tb;

	delete r_nd_cp_tb;
	delete r_nd_ld_tb;
}


//判断一个电压类型是不是10KV，是返回true,否则返回false
bool is_kv10(long kv_id)
{
	PSBOB::BV bv = { 0 };

	int ret = GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("basevoltage")), (char*) &kv_id, sizeof(bv), (char*) &bv);

	if (ret < 0)
	{
		return false;
	}
	else
	{
		if (bv.nomvol < MAX_KV && bv.nomvol > MIN_KV)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

//判断一个厂站是否为变电站
bool is_substn_dev(long st_id)
{
	if(st_id <= 0) return true;

	PSBOB::ST st = { 0 };

	int ret = GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), (char*) &st_id, sizeof(st), (char*) &st);

	if (ret < 0)
	{
		return true;
	}
	else
	{
		if (HasMask(g_db_psbob->GetTB("substation"), "st_type", st.st_type , MENU_FAC_TYPE_SUBSTN))
			return true;
	}

	return false;
}

bool is_st_in_dv(long stid, long dvid)
{
	if (dvid <= 0)
		return true;

	substation_dscada st = { 0 };

	if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), (char*) &stid, sizeof(st), (char*) &st) <= 0)
	{
		return false;
	}

	return st.subarea_id == dvid;
}

bool DSBOB2::ReadPSBOB(long dvid)
{
	int pos = -1, ph = -1;

	cout << "//0- read disconnector(switch) table" << endl;
	char *buf = NULL;
	PSBOB::SWITCH *switch_set = new PSBOB::SWITCH;
	CLS_SWITCH *swh = new CLS_SWITCH;
	int size = sizeof(PSBOB::SWITCH);
	TB_DESCR *tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("disconnector"));

	int ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int cnt = ret / size;
		TRACE("开始校验刀闸(disconnector)表……\r\n");

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			memcpy(switch_set, buf + pos * size, size);

			if (!is_st_in_dv(switch_set->st_id, dvid))
				continue;

			if (switch_set->ind <= 0 || switch_set->jnd <= 0)
			{
				//TRACE("刀闸%ld[%s]%s！\r\n", switch_set->id, switch_set->name, err_descr[ERR_INDEX_ND]);
				continue;
			}

			if (switch_set->ind == switch_set->jnd)
			{
				//TRACE("刀闸%ld[%s]%s！\r\n", switch_set->id, switch_set->name, err_descr[ERR_INDEX_SAME_ND]);
				continue;
			}

			if (switch_set->bv_id <= 0)
			{
				//TRACE("刀闸%ld[%s]%s！\r\n", switch_set->id, switch_set->name, err_descr[ERR_INDEX_BV]);
				continue;
			}

			if (!is_kv10(switch_set->bv_id))
			{
				//TRACE("刀闸%ld[%s]%s！\r\n", switch_set->id, switch_set->name, err_descr[ERR_INDEX_BV_VALUE]);
				continue; //不是10KV电压等级的
			}

			if (is_substn_dev(switch_set->st_id))
			{
				//TRACE("刀闸%ld[%s]%s！\r\n", switch_set->id, switch_set->name, err_descr[ERR_INDEX_ST]);
				continue;	//主站设备
			}

			bzero(swh, sizeof(CLS_SWITCH));

			swh->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SWITCH")), ph++);
			swh->pid = switch_set->id;
			swh->nd = switch_set->ind;
			swh->znd = switch_set->jnd;
			swh->stid = switch_set->st_id;
			strcpy(swh->descr, switch_set->name);

			//杨勇 20141017 添加nd与刀闸的对应关系，解决恢复区段刀闸不着色的问题
			g_map_nd_switch[switch_set->ind].push_back(switch_set->id);
			g_map_nd_switch[switch_set->jnd].push_back(switch_set->id);

			switch_tb->Insert(*swh);
		}

		BufFree(buf);
//		TRACE("before switch_tb sequence\r\n");
		switch_tb->Sequence();
//		TRACE("after switch_tb sequence\r\n");
	}
	else
	{
		TRACE("读取disconnector表出错\r\n");
	}

	delete swh;
	delete switch_set;

	cout << "//0- read grounddisconnector(switch) table" << endl;
	buf = NULL;
	grounddisconnector_dscada *gd_switch_set = new grounddisconnector_dscada;
	swh = new CLS_SWITCH;
	size = sizeof(grounddisconnector_dscada);
	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("grounddisconnector"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int cnt = ret / size;
		TRACE("开始校验接地刀闸(grounddisconnector)表……\r\n");

		for (pos = 0; pos < cnt; pos++)
		{
			memcpy(gd_switch_set, buf + pos * size, size);

			if (!is_st_in_dv(gd_switch_set->st_id, dvid))
				continue;

			if (gd_switch_set->nd <= 0)
			{
				//TRACE("接地刀闸%ld[%s]%s!\r\n", gd_switch_set->id, gd_switch_set->name, err_descr[ERR_INDEX_ND]);
				continue;
			}

			if (gd_switch_set->bv_id <= 0)
			{
				//TRACE("接地刀闸%ld[%s]%s!\r\n", gd_switch_set->id, gd_switch_set->name, err_descr[ERR_INDEX_BV]);
				continue;
			}

			if (!is_kv10(gd_switch_set->bv_id))
			{
				//TRACE("接地刀闸%ld[%s]%s！\r\n", switch_set->id, switch_set->name, err_descr[ERR_INDEX_BV_VALUE]);
				continue;
			}

			if (is_substn_dev(gd_switch_set->st_id))
			{
				//TRACE("接地刀闸%ld[%s]%s！\r\n", switch_set->id, switch_set->name, err_descr[ERR_INDEX_ST]);
				continue;	//主站设备
			}

			bzero(swh, sizeof(CLS_SWITCH));

			swh->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SWITCH")), ph++);
			swh->pid = gd_switch_set->id;
			swh->nd = gd_switch_set->nd;
			swh->znd = gd_switch_set->id;	//由于接地刀闸只有一个结点，要想将其当普通刀闸处理，就需要为其生成一个唯一的ZND
			swh->stid = gd_switch_set->st_id;
			strcpy(swh->descr, gd_switch_set->name);

			switch_tb->Insert(*swh);
		}

		BufFree(buf);
//		TRACE("before switch_tb sequence\r\n");
		switch_tb->Sequence();
//		TRACE("after switch_tb sequence\r\n");
	}
	else
	{
		TRACE("读取grounddisconnector表出错\r\n");
	}

	delete swh;
	delete gd_switch_set;

	//读取馈线表，根据馈线表中馈线首端结点生成虚拟进线开关
	cout << "// read subcontrolarea(DV) table" << endl;
	list<long> lst_virtual_bus;	//和虚拟进线开关连接的母线，由于该母线可能为变电站设备，防止后面读母线表时被过滤掉
	ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int size = sizeof(PSBOB::DV);
		int cnt = ret / size;
		TRACE("开始校验区域表(subcontrolarea)……\r\n");

		PSBOB::DV dv = { 0 };

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			memcpy(&dv, buf + pos * size, size);

			if(dvid > 0 && dv.id != dvid)
				continue;

			if (dv.lmtnorm <= 0)
			{
				TRACE("区域%ld[%s]馈线容量值过低，请检查！\r\n", dv.id, dv.name);
			}

			if ((dv.nd_id > 0) && (HasMask(g_db_psbob->GetTB("subcontrolarea"), "dtype", dv.dtype, MENU_DV_TYPE_FD)))
			{
				map<long, list<stack<NODE> > >::iterator it = g_map_nd_path.find(dv.nd_id);
				if (it != g_map_nd_path.end())
				{
					stack<NODE> path = it->second.front();

					while (path.size() > 0)
					{
						if (path.top().type == _BUS)
						{
							CLS_CB *cb = new CLS_CB;

							PSBOB::CB psbob_cb = { 0 };

							if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), (char*) &dv.cbid, sizeof(PSBOB::CB), (char*) &psbob_cb) < 0)
							{
								TRACE("读取断路器表关键字为%ld的开关记录失败,区域表馈线类型进线开关id有误！\r\n", dv.cbid);

								strcpy(cb->descr, dv.name);
							}
							else
								strcpy(cb->descr, psbob_cb.name);

							cb->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CB")), ph++);
							cb->nd = dv.nd_id;
							cb->znd = path.top().data.bus.nd;
							cb->q = CB_BREAKER | CB_VIRTUAL | CB_FDHCB;
							cb->pid = dv.cbid;
							cb->stid = dv.sourceid;

							cb_tb->Insert(*cb);

							lst_virtual_bus.push_back(path.top().data.bus.id);
							delete cb;

							break;
						}

						path.pop();
					}
				}
			}
			else
			{
				TRACE("区域%ld[%s]的类型为馈线，请为馈线类型的区域填充首端节点id\r\n", dv.id, dv.name);
			}
		}

		BufFree(buf);
	}
	else
	{
		TRACE("读取subcontrolarea表出错\r\n");
	}

	cout << "//1- read breaker(CB) table" << endl;
//	char *buf = NULL;
	PSBOB::CB *cb_set = new PSBOB::CB;
	CLS_CB *cb = new CLS_CB;
	size = sizeof(PSBOB::CB);
	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int cnt = ret / size;

		TRACE("开始校验断路器(breaker)表……\r\n");

		for (pos = 0; pos < cnt; pos++)
		{
			memcpy(cb_set, buf + pos * size, size);

			if (cb_set->id == 114560317232523410)
			{
				TRACE("pause!\r\n");
			}

			if (!is_st_in_dv(cb_set->st_id, dvid))
				continue;

			if (cb_set->ind <= 0 || cb_set->jnd <= 0)
			{
				//TRACE("断路器%ld[%s]%s！\r\n", cb_set->id, cb_set->name, err_descr[ERR_INDEX_ND]);
				continue;
			}

			if (cb_set->ind == cb_set->jnd)
			{
				//TRACE("断路器%ld[%s]%s！\r\n", cb_set->id, cb_set->name, err_descr[ERR_INDEX_SAME_ND]);
				continue;
			}

			if (cb_set->bv_id <= 0)
			{
				//TRACE("断路器%ld[%s]%s！\r\n", cb_set->id, cb_set->name, err_descr[ERR_INDEX_BV]);
				continue;
			}

			if (!is_kv10(cb_set->bv_id))
			{
				//TRACE("断路器%ld[%s]%s！\r\n", cb_set->id, cb_set->name, err_descr[ERR_INDEX_BV_VALUE]);
				continue;	//不是10KV电压等级的
			}

			if (is_substn_dev(cb_set->st_id))
			{
				//TRACE("断路器%ld[%s]%s！\r\n", cb_set->id, cb_set->name, err_descr[ERR_INDEX_ST]);
				continue;	//主站设备
			}

			cb->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CB")), ph++);
			cb->pid = cb_set->id;
			cb->q = CB_BREAKER;

			//设置进线开关标志位
			if (HasMask(tb_descr, "brk_type", cb_set->brk_type, MENU_BRK_TYPE_JX))
			{
				cb->q |= CB_FDHCB;
			}

			//设置联络开关标志位
			if (HasMask(tb_descr, "brk_type", cb_set->brk_type, MENU_BRK_TYPE_LW))
			{
				cb->q |= CB_LWCB;
			}

			//D5000隔离、保电牌在标志牌里读取
			/*
			 //读取隔离标志
			 if (HasMask(tb_descr, "status", cb_set->status , MENU_STATE_DEV_ISOLATE))
			 {
			 cb->q |= CB_ISOLATE;
			 }

			 //读取保电标志
			 if (HasMask(tb_descr, "status", cb_set->status , MENU_STATE_DEV_PRESERVE))
			 {
			 cb->q |= CB_PRESERVE;
			 }
			 */
			//读取遥控标致位
			if (HasMask(tb_descr, "brk_prop", cb_set->brk_prop, MENU_BRK_PROP_AUTO))
			{
				cb->q |= CB_AUTO;
			}
			else
			{
				//TRACE("断路器%ld[%s]brk_prop似乎未定义，请检查！\r\n", cb_set->id, cb_set->name);
			}

			strcpy(cb->descr, cb_set->name);

			cb->nd = cb_set->ind;
			cb->znd = cb_set->jnd;

//			cb->nd = cb_set->ind;
//			cb->znd = cb_set->jnd;
			cb->stid = cb_set->st_id;

			cb_tb->Insert(*cb);
		}

		BufFree(buf);
		cb_tb->Sequence();
	}
	else
	{
		TRACE("读取breaker表出错\r\n");
	}

	delete cb;
	delete cb_set;

	cout << "//2 - read faultinfo table" << endl;

	CLS_FI* fi = new CLS_FI;
	PSBOB::FI* faultinfo_set = new PSBOB::FI;
	size = sizeof(PSBOB::FI);

	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	fi_tb->Empty();

	if (ret >= 0)
	{
		int cnt = ret / size;

		TRACE("开始校验故障指标器(faultinfo)表……\r\n");

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			memcpy(faultinfo_set, buf + pos * size, size);

			if (!is_st_in_dv(faultinfo_set->st_id, dvid))
				continue;

			if (faultinfo_set->bv_id <= 0)
			{
				//TRACE("故障指示器%ld[%s]%s!\r\n", faultinfo_set->id, faultinfo_set->name, err_descr[ERR_INDEX_BV]);
				continue;
			}

			if (!is_kv10(faultinfo_set->bv_id))
			{
				//TRACE("故障指示器%ld[%s]%s!\r\n", faultinfo_set->id, faultinfo_set->name, err_descr[ERR_INDEX_BV_VALUE]);
				continue;	//不是10KV电压等级的
			}

			if (is_substn_dev(faultinfo_set->st_id))
			{
				//TRACE("故障指示器%ld[%s]%s!\r\n", faultinfo_set->id, faultinfo_set->name, err_descr[ERR_INDEX_ST]);
				continue;	//主站设备
			}

			if (faultinfo_set->aclnseg_id <= 0)
			{
				//TRACE("故障指示器%ld[%s]aclnseg_id未设置，请检查!\r\n", faultinfo_set->id, faultinfo_set->name);
				continue;
			}

			fi->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_FI")), ph++);
			fi->pid = faultinfo_set->id;
			fi->nd = faultinfo_set->nd;
			strcpy(fi->descr, faultinfo_set->name);
			fi->sec = IDTransfor(faultinfo_set->aclnseg_id);
			fi->pos = faultinfo_set->pos;
			fi->r_sec = -1;
			fi->idigital = -1;

			fi_tb->Insert(*fi);
		}

		BufFree(buf);
		fi_tb->Sequence();
	}
	else
	{
		TRACE("读取faultinfo表出错！\r\n");
	}

	delete fi;
	delete faultinfo_set;

	cout << "//3- read energyconsumer(LD) table" << endl;
	PSBOB::LD *ld_set = new PSBOB::LD;
	CLS_LD *ld = new CLS_LD;
	size = sizeof(PSBOB::LD);
	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int cnt = ret / size;

		TRACE("开始校验负荷(energyconsumer)表……\r\n");

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			memcpy(ld_set, buf + pos * size, size);

			if (!is_st_in_dv(ld_set->st_id, dvid))
				continue;

			if (ld_set->nd <= 0)
			{
				//TRACE("负荷%ld[%s]%s!\r\n", ld_set->id, ld_set->name, err_descr[ERR_INDEX_ND]);
				continue;
			}

			if (ld_set->bv_id <= 0)
			{
				//TRACE("负荷%ld[%s]%s!\r\n", ld_set->id, ld_set->name, err_descr[ERR_INDEX_BV]);
				continue;
			}

			if (!is_kv10(ld_set->bv_id))
			{
				//TRACE("负荷%ld[%s]%s!\r\n", ld_set->id, ld_set->name, err_descr[ERR_INDEX_BV_VALUE]);
				continue;	//不是10KV电压等级的
			}

			if (is_substn_dev(ld_set->st_id))
			{
				//TRACE("负荷%ld[%s]%s!\r\n", ld_set->id, ld_set->name, err_descr[ERR_INDEX_ST]);
				continue;	//主站设备
			}

			ld->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_LD")), ph++);
			ld->pid = ld_set->id;
			strcpy(ld->descr, ld_set->name);

			//跳刀闸
			ld->nd = ld_set->nd;
			ld->stid = ld_set->st_id;

			//2012.5.24荣义平
			//将负荷与节点形成映射关系，方便后面根据节点快速查找到对应的负荷
			m_map_nd_ld[ld->nd].push_back(*ld);

			ld_tb->Insert(*ld);
		}

		BufFree(buf);
		ld_tb->Sequence();
	}
	else
	{
		TRACE("读取energyconsumer表出错\r\n");
	}

	delete ld;
	delete ld_set;

	cout << "//4- read company(CO) table" << endl;
	PSBOB::CO *co_set = new PSBOB::CO;
	CLS_CO *co = new CLS_CO;
	size = sizeof(PSBOB::CO);
	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("company"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int cnt = ret / size;

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			memcpy(co_set, buf + pos * size, size);

			co->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CO")), ph++);
			co->pid = co_set->id;
			strcpy(co->descr, co_set->name);

			co_tb->Insert(*co);
		}

		BufFree(buf);
		co_tb->Sequence();
	}
	else
	{
		TRACE("读取company表出错\r\n");
	}

	delete co;
	delete co_set;

	cout << "//5- read substation(ST) table" << endl;
	PSBOB::ST *st_set = new PSBOB::ST;
	CLS_ST *st = new CLS_ST;
	size = sizeof(PSBOB::ST);
	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int cnt = ret / size;

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			memcpy(st_set, buf + pos * size, size);

			//if(HasMask(tb_descr, "st_type", st_set->st_type , MENU_FAC_TYPE_SUBSTN)) continue;		//主站

//			if(st_set->bv_id == 0 || !is_kv10(st_set->bv_id)) continue;	//不是10KV电压等级的

//			PSBOB::VL vl[20] = { 0 };
//
//			int size = oodbread_ra(&vl, "st_id", &st_set->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("voltagelevel")), sizeof(vl));
//
//			if (size > 0)
//			{
//				int num = size / sizeof(PSBOB::VL);
//
//				for (int i = 0; i < num; i++)
//				{
//					if (is_kv10(vl[i].bv_id))
//					{
			st->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_ST")), ph++);
			st->pid = st_set->id;
			strcpy(st->descr, st_set->name);

			st_tb->Insert(*st);

//						break;
//					}
//				}
//			}
		}

		BufFree(buf);
		st_tb->Sequence();
	}
	else
	{
		TRACE("读取substation表出错\r\n");
	}

	delete st;
	delete st_set;

	cout << "//6- read busbarsection(BUS) table" << endl;
	PSBOB::BUS *bus_set = new PSBOB::BUS;
	CLS_BUS *bus = new CLS_BUS;
	size = sizeof(PSBOB::BUS);
	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret >= 0)
	{
		int cnt = ret / size;
		TRACE("开始校验母线(busbarsection)表……\r\n");

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			memcpy(bus_set, buf + pos * size, size);

			if (!is_st_in_dv(bus_set->st_id, dvid))
				continue;

			if (bus_set->nd <= 0)
			{
				//TRACE("母线%ld[%s]%s!\r\n", bus_set->id, bus_set->name, err_descr[ERR_INDEX_ND]);
				continue;
			}

			if (bus_set->bv_id == 0)
			{
				//TRACE("母线%ld[%s]%s!\r\n", bus_set->id, bus_set->name, err_descr[ERR_INDEX_BV]);
				continue;
			}

			if (!is_kv10(bus_set->bv_id))
			{
				//TRACE("母线%ld[%s]%s!\r\n", bus_set->id, bus_set->name, err_descr[ERR_INDEX_BV_VALUE]);
				continue;	//不是10KV电压等级的
			}

			if (bus_set->v <= 0)
			{
				//TRACE("母线%ld[%s]线电压值过低，请检查!\r\n", bus_set->id, bus_set->name);
			}

			if (is_substn_dev(bus_set->st_id) &&			//主站设备
					std::find(lst_virtual_bus.begin(), lst_virtual_bus.end(), bus_set->id) == lst_virtual_bus.end()) 	//没有连接虚拟开关
			{
				//TRACE("母线%ld[%s]未发现与之对应的区域表进线开关id,可能无法将该母线当成电源!\r\n", bus_set->id, bus_set->name);
				continue;
			}

			//分支箱，配电室、、、
			PSBOB::ST psbob_st = { 0 };
			if (oodbread_rk(&psbob_st, &bus_set->st_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("substation")), sizeof(psbob_st)) > 0)
			{
				if (HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type, MENU_FAC_TYPE_CABLEBRANCHBOX)
						|| HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type, MENU_FAC_TYPE_DISTRIBUTEBOX)
						|| HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type, MENU_FAC_TYPE_RINGMAINUNIT)
						|| HasMask(g_db_psbob->GetTB("substation"), "st_type", psbob_st.st_type, MENU_FAC_TYPE_SWITCHING))
				{
					//杨勇 20140614 添加环网柜母线，相关的功能还没有测试，特此注意！
					g_map_nd_bus[bus_set->nd].push_back(bus_set->id);
					continue;
				}
			}
			else
			{
				TRACE("母线%ld[%s]在厂站ID在厂站表里找不到对应的记录!\r\n", bus_set->id, bus_set->name);
				continue;
			}

			g_map_nd_bus[bus_set->nd].push_back(bus_set->id);

			bus->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_BUS")), ph++);
			bus->pid = bus_set->id;
			strcpy(bus->descr, bus_set->name);

			bus->nd = bus_set->nd;
			bus->stid = bus_set->st_id;

			g_map_nd_bus[bus->nd].push_back(bus->pid);

			bus_tb->Insert(*bus);
		}

		BufFree(buf);
		bus_tb->Sequence();
	}
	else
	{
		TRACE("读取busbarsection表出错\r\n");
	}

	delete bus;
	delete bus_set;

	cout << "//7- read aclinesegment(SEC) table" << endl;
	PSBOB::SEC *sec_set = new PSBOB::SEC;
	CLS_SEC *sec = new CLS_SEC;
	size = sizeof(PSBOB::SEC);
	tb_descr = const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment"));

	ret = GetTableRecs(tb_descr, 1/*读取有效记录*/, &buf);

	if (ret > 0)
	{
		int cnt = ret / size;
		TRACE("开始校验交流线段(aclinesegment)表……\r\n");

		for (pos = 0, ph = 1; pos < cnt; pos++)
		{
			PSBOB::ACLINEEND ac_end[2] = { 0 };

			memcpy(sec_set, buf + pos * size, size);

			if (!is_st_in_dv(sec_set->ist_id, dvid) && !is_st_in_dv(sec_set->jst_id, dvid))
				continue;

			if (oodbread_ra(&ac_end, "aclnseg_id", &sec_set->id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclineend")), sizeof(ac_end)) <= 0)
			{
				//TRACE("读取线段%ld[%s]的端点信息失败，请确认交流线段端点表里存在与该线段对应的记录！\r\n", sec_set->id, sec_set->name);
				continue;
			}

			sec_set->ind = ac_end[0].nd;
			sec_set->jnd = ac_end[1].nd;

			if (sec_set->ind <= 0 || sec_set->jnd <= 0)
			{
				//TRACE("线段%ld[%s]%s！\r\n", sec_set->id, sec_set->name, err_descr[ERR_INDEX_ND]);
				continue;
			}

			if (sec_set->ind == sec_set->jnd)
			{
				//TRACE("线段%ld[%s]%s！\r\n", sec_set->id, sec_set->name, err_descr[ERR_INDEX_SAME_ND]);
				continue;
			}

			if (sec_set->bv_id == 0)
			{
				//TRACE("线段%ld[%s]%s！\r\n", sec_set->id, sec_set->name, err_descr[ERR_INDEX_BV]);
				continue;
			}

			if (!is_kv10(sec_set->bv_id))
			{
				//TRACE("线段%ld[%s]%s！\r\n", sec_set->id, sec_set->name, err_descr[ERR_INDEX_BV_VALUE]);
				continue;	//不是10KV电压等级的
			}

			if (is_substn_dev(sec_set->ist_id) || is_substn_dev(sec_set->jst_id))
			{
				//TRACE("线段%ld[%s]%s！\r\n", sec_set->id, sec_set->name, err_descr[ERR_INDEX_ST]);
				continue;	//主站设备
			}

			sec->pid = sec_set->id;
			sec->id = IDTransfor(sec_set->id);
			strcpy(sec->descr, sec_set->name);

			//跳刀闸
			sec->nd = sec_set->ind;
			sec->znd = sec_set->jnd;
			sec_tb->Insert(*sec);
		}

		BufFree(buf);
		sec_tb->Sequence();
	}
	else
	{
		TRACE("读取aclinesegment表出错\r\n");
	}

	delete sec;
	delete sec_set;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// write database object to relative database
bool DSBOB2::WriteReDB2(const char* dsn, const char* user, const char* password)
{

	cout << "写DMS RTDB库……" << endl;

   int count = 0, i = 0, size = 0;

	g_db_dms->ResetAllTables();

	cout << "//2 - write bs table" <<endl;

	CLS_BS* bs;
	DMS::BS* bs_set = new DMS::BS;
	size = sizeof(DMS::BS);

	bs_tb->Sequence();
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
		bs_set->valid = (bs + i)->valid;
//张瑞雪添加于20120213
		bs_set->kv_b =(bs + i)->kv_b ;
		bs_set->v_b =(bs + i)->v_b ;
		bs_set->adeg_b=(bs + i)->adeg_b;
		bs_set->w_b =(bs + i)->w_b ;
		bs_set->r_b =(bs + i)->r_b ;
		bs_set->kv_c =(bs + i)->kv_c ;
		bs_set->v_c =(bs + i)->v_c ;
		bs_set->adeg_c=(bs + i)->adeg_c;
		bs_set->w_c =(bs + i)->w_c ;
		bs_set->r_c =(bs + i)->r_c ;
//张瑞雪添加于20120503
		bs_set->kv_a =(bs + i)->kv_a ;
		bs_set->v_a =(bs + i)->v_a ;
		bs_set->adeg_a=(bs + i)->adeg_a;
		bs_set->w_a =(bs + i)->w_a ;
		bs_set->r_a =(bs + i)->r_a ;
		if (oodbinsert(bs_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_BS")), 1, size) == OO_FAIL)
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

	bus_tb->Sequence();
	bus = bus_tb->GetTable();
	count = bus_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(bus_set,sizeof(DMS::BUS));
		bus_set->id =  (bus + i)->id;
		bus_set->pid =  (bus + i)->pid;
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
//张瑞雪添加于20120426
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

		bus_set->stid =  (bus + i)->stid;
		if (oodbinsert(bus_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_BUS")), 1, size) == OO_FAIL)
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

	cb_tb->Sequence();

	cb = cb_tb->GetTable();
	count = cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(cb_set,sizeof(DMS::CB));
		cb_set->id =  (cb + i)->id;
		cb_set->pid =  (cb + i)->pid;
		strcpy(cb_set->descr, (cb + i)->descr);
		cb_set->nd =  (cb + i)->nd;
		cb_set->znd =  (cb + i)->znd;
		cb_set->ind = (cb + i)->ind;
		cb_set->iznd = (cb + i)->iznd;
//		cb_set->digital =  (cb + i)->digital;
//		cb_set->idigital = (cb + i)->idigital;
		cb_set->q = (cb + i)->q;
		cb_set->topflags = (cb + i)->topflags;
		cb_set->r_st = (cb + i)->r_st;
		cb_set->stid =  (cb + i)->stid;

		if (oodbinsert(cb_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CB")), 1, size) == OO_FAIL)
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

	co_tb->Sequence();
	co = co_tb->GetTable();
	count = co_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(co_set,sizeof(DMS::CO));
		co_set->id =  (co + i)->id;
		co_set->pid =  (co + i)->pid;
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


		if (oodbinsert(co_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CO")), 1, size) == OO_FAIL)
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

	cp_tb->Sequence();

	cp = cp_tb->GetTable();
	count = cp_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(cp_set,sizeof(DMS::CP));
		cp_set->id =  (cp + i)->id;
		cp_set->pid =  (cp + i)->pid;
		strcpy(cp_set->descr, (cp + i)->descr);
		cp_set->nd =  (cp + i)->nd;
		cp_set->stid =  (cp+i)->stid;
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

		if (oodbinsert(cp_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CP")), 1, size) == OO_FAIL)
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
////	digital_tb->Sequence();	//为了保证读入的数据和PSBOB中的逻辑号一致，不对数据进行排序
//
//	digital = digital_tb->GetTable();
//	count = digital_tb->GetCount();
//	for (i = 0; i < count; i++)
//	{
//        bzero(digital_set,sizeof(DMS::DIGITAL));
//		digital_set->id =  (digital + i)->id;
//		strcpy(digital_set->descr, (digital + i)->descr);
//		digital_set->value = (digital + i)->value;
//		digital_set->quality = (digital + i)->quality;
//		digital_set->icb = (digital + i)->icb;
//		digital_set->cb =  (digital + i)->cb;
//
//		if (oodbinsert(digital_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_DIGITAL")), 1, size) == OO_FAIL)
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

	global_tb->Sequence();

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
		if (oodbinsert(global_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_GLOBAL")), 1, size) == OO_FAIL)
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

	island_tb->Sequence();

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
		island_set->valid = (island + i)->valid;
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
		if (oodbinsert(island_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_ISLAND")), 1, size) == OO_FAIL)
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

	ld_tb->Sequence();

	ld = ld_tb->GetTable();
	count = ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(ld_set,sizeof(DMS::LD));
        ld_set->zn_id =  (ld + i)->zn_id;
		ld_set->id =  (ld + i)->id;
		ld_set->pid =  (ld + i)->pid;
		ld_set->stid =  (ld + i)->stid;
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

		if (oodbinsert(ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_LD")), 1, size) == OO_FAIL)
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

	nd_tb->Sequence();

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

		if (oodbinsert(nd_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_ND")), 1, size) == OO_FAIL)
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

	so_tb->Sequence();

	so = so_tb->GetTable();
	count = so_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(so_set,sizeof(DMS::SO));
		so_set->id =  (so + i)->id;
		so_set->pid =  (so + i)->pid;
		so_set->stid =  (so + i)->stid;
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
		//张瑞雪添加于20120426
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

		if (oodbinsert(so_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SO")), 1, size) == OO_FAIL)
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

	st_tb->Sequence();

	st = st_tb->GetTable();
	count = st_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(st_set,sizeof(DMS::ST));
		st_set->id =  (st + i)->id;
		st_set->pid =  (st + i)->pid;
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
		if (oodbinsert(st_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_ST")), 1, size) == OO_FAIL)
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

	tapty_tb->Sequence();

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
		if (oodbinsert(tapty_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_TAPTY")), 1, size) == OO_FAIL)
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

	xf_tb->Sequence();

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
		if (oodbinsert(xf_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_XF")), 1, size) == OO_FAIL)
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

	xfmr_tb->Sequence();

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
		xfmr_set->xftype= (xfmr + i)-> xftype;

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

		if (oodbinsert(xfmr_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_XFMR")), 1, size) == OO_FAIL)
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

	zn_tb->Sequence();

	zn = zn_tb->GetTable();
	count = zn_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(zn_set,sizeof(DMS::ZN));
		zn_set->id =  (zn + i)->id;
		zn_set->stid =  (zn + i)->stid;
		strcpy(zn_set->descr, (zn + i)->descr);
		zn_set->type = (zn + i)->type;
		///////////////////////////////////////////////////
		///////////////荣义平添加于20111130为适应超过5个结节的ZN
#if ZN_ND_USE_ARRAY
//		strcpy(zn_set->nd, (zn + i)->nds[0]);
//		strcpy(zn_set->znd, (zn + i)->nds[1]);
//		strcpy(zn_set->nd3, (zn + i)->nds[2]);
//		strcpy(zn_set->nd4, (zn + i)->nds[3]);
//		strcpy(zn_set->nd5, (zn + i)->nds[4]);
//		strcpy(zn_set->nd6, (zn + i)->nds[5]);
//		strcpy(zn_set->nd7, (zn + i)->nds[6]);
//		strcpy(zn_set->nd8, (zn + i)->nds[7]);
//		strcpy(zn_set->nd9, (zn + i)->nds[8]);
//		strcpy(zn_set->nd10, (zn + i)->nds[9]);
//		strcpy(zn_set->nd11, (zn + i)->nds[10]);
//		strcpy(zn_set->nd12, (zn + i)->nds[11]);
//		strcpy(zn_set->nd13, (zn + i)->nds[12]);
//		strcpy(zn_set->nd14, (zn + i)->nds[13]);
//		strcpy(zn_set->nd15, (zn + i)->nds[14]);
//		strcpy(zn_set->nd16, (zn + i)->nds[15]);
//		strcpy(zn_set->nd17, (zn + i)->nds[16]);
//		strcpy(zn_set->nd18, (zn + i)->nds[17]);
//		strcpy(zn_set->nd19, (zn + i)->nds[18]);
//		strcpy(zn_set->nd20, (zn + i)->nds[19]);

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
		strcpy(zn_set->nd, (zn + i)->nd);
		strcpy(zn_set->znd, (zn + i)->znd);
		strcpy(zn_set->nd3, (zn + i)->nd3);
		strcpy(zn_set->nd4, (zn + i)->nd4);
		strcpy(zn_set->nd5, (zn + i)->nd5);
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
		if (oodbinsert(zn_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_ZN")), 1, size) == OO_FAIL)
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

	sec_tb->Sequence();

	sec = sec_tb->GetTable();
	count = sec_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(sec_set,sizeof(DMS::SEC));
        sec_set->zn_id =  (sec + i)->zn_id;
		sec_set->id =  (sec + i)->id;
		sec_set->pid =  (sec + i)->pid;
		strcpy(sec_set->descr, (sec + i)->descr);
		sec_set->nd =  (sec + i)->nd;
		sec_set->r_fi =  (sec + i)->r_fi;
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
		if (oodbinsert(sec_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SEC")), 1, size) == OO_FAIL)
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

	r_co_st_tb->Sequence();

	r_co_st = r_co_st_tb->GetTable();
	count = r_co_st_tb->GetCount();
	for (i = 0; i < count; i++)
	{
             bzero(r_co_st_set, sizeof(DMS::R_CO_ST));
		r_co_st_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_CO_ST")), i + 1);
		r_co_st_set->CO_id =  (r_co_st + i)->co_id;
		r_co_st_set->ST_id =  (r_co_st + i)->st_id;
		r_co_st_set->CO_id_ptr = (r_co_st + i)->co_id_ptr;
		r_co_st_set->ST_id_ptr = (r_co_st + i)->st_id_ptr;
		r_co_st_set->fptr = (r_co_st + i)->fptr;
		r_co_st_set->bptr = (r_co_st + i)->bptr;
		if (oodbinsert((void *)r_co_st_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_CO_ST")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_co_st error!errno=" << _oodb_errno << endl;
			delete r_co_st_set;

			return false;
		}
	}

	delete r_co_st_set;

	cout << "//14 - write r_nd_cb table" <<endl;

	CLS_R_ND_CB* r_nd_cb;
	DMS::R_ND_CB* r_nd_cb_set = new DMS::R_ND_CB;
	size = sizeof(DMS::R_ND_CB);

	r_nd_cb_tb->Sequence();

	r_nd_cb = r_nd_cb_tb->GetTable();
	count = r_nd_cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_nd_cb_set, sizeof(DMS::R_ND_CB));
		r_nd_cb_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_CB")), i + 1);
		r_nd_cb_set->ND_id =  (r_nd_cb + i)->nd_id;
		r_nd_cb_set->CB_id =  (r_nd_cb + i)->cb_id;
		r_nd_cb_set->ND_id_ptr = (r_nd_cb + i)->nd_id_ptr;
		r_nd_cb_set->CB_id_ptr = (r_nd_cb + i)->cb_id_ptr;
		r_nd_cb_set->fptr = (r_nd_cb + i)->fptr;
		r_nd_cb_set->bptr = (r_nd_cb + i)->bptr;
		if (oodbinsert(r_nd_cb_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_CB")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_nd_cb error!errno=" << _oodb_errno << endl;

			delete r_nd_cb_set;

			return false;
		}
	}

	delete r_nd_cb_set;
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

	cout << "//14 - write r_nd_cp table" << endl;

	CLS_R_ND_CP* r_nd_cp;
	DMS::R_ND_CP* r_nd_cp_set = new DMS::R_ND_CP;
	size = sizeof(DMS::R_ND_CP);

	r_nd_cb_tb->Sequence();
	r_nd_cp = r_nd_cp_tb->GetTable();
	count = r_nd_cp_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_nd_cp_set, sizeof(DMS::R_ND_CP));
		r_nd_cp_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_CP")), i + 1);
		r_nd_cp_set->ND_id =  (r_nd_cp + i)->nd_id;
		r_nd_cp_set->CP_id =  (r_nd_cp + i)->cp_id;
		r_nd_cp_set->ND_id_ptr = (r_nd_cp + i)->nd_id_ptr;
		r_nd_cp_set->CP_id_ptr = (r_nd_cp + i)->cp_id_ptr;
		r_nd_cp_set->fptr = (r_nd_cp + i)->fptr;
		r_nd_cp_set->bptr = (r_nd_cp + i)->bptr;
		if (oodbinsert(r_nd_cp_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_CP")), 1, size) == OO_FAIL)
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

	r_nd_cb_tb->Sequence();
	r_nd_ld = r_nd_ld_tb->GetTable();
	count = r_nd_ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_nd_ld_set, sizeof(DMS::R_ND_LD));
		r_nd_ld_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_LD")), i + 1);
		r_nd_ld_set->ND_id =  (r_nd_ld + i)->nd_id;
		r_nd_ld_set->LD_id =  (r_nd_ld + i)->ld_id;
		r_nd_ld_set->ND_id_ptr = (r_nd_ld + i)->nd_id_ptr;
		r_nd_ld_set->LD_id_ptr = (r_nd_ld + i)->ld_id_ptr;
		r_nd_ld_set->fptr = (r_nd_ld + i)->fptr;
		r_nd_ld_set->bptr = (r_nd_ld + i)->bptr;
		if (oodbinsert(r_nd_ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_LD")), 1, size) == OO_FAIL)
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

	r_nd_sec_tb->Sequence();

	r_nd_sec = r_nd_sec_tb->GetTable();
	count = r_nd_sec_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_nd_sec_set, sizeof(DMS::R_ND_SEC));
		r_nd_sec_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_SEC")), i + 1);
		r_nd_sec_set->ND_id =  (r_nd_sec + i)->nd_id;
		r_nd_sec_set->SEC_id =  (r_nd_sec + i)->sec_id;
		r_nd_sec_set->ND_id_ptr = (r_nd_sec + i)->nd_id_ptr;
		r_nd_sec_set->SEC_id_ptr = (r_nd_sec + i)->sec_id_ptr;
		r_nd_sec_set->fptr = (r_nd_sec + i)->fptr;
		r_nd_sec_set->bptr = (r_nd_sec + i)->bptr;
		if (oodbinsert(r_nd_sec_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_SEC")), 1, size) == OO_FAIL)
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

	r_nd_xf_tb->Sequence();

	r_nd_xf = r_nd_xf_tb->GetTable();
	count = r_nd_xf_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_nd_xf_set, sizeof(DMS::R_ND_XF));
		r_nd_xf_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_XF")), i + 1);
		r_nd_xf_set->ND_id =  (r_nd_xf + i)->nd_id;
		r_nd_xf_set->XF_id =  (r_nd_xf + i)->xf_id;
		r_nd_xf_set->ND_id_ptr = (r_nd_xf + i)->nd_id_ptr;
		r_nd_xf_set->XF_id_ptr = (r_nd_xf + i)->xf_id_ptr;
		r_nd_xf_set->fptr = (r_nd_xf + i)->fptr;
		r_nd_xf_set->bptr = (r_nd_xf + i)->bptr;
		if (oodbinsert(r_nd_xf_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ND_XF")), 1, size) == OO_FAIL)
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

	r_zn_nd_tb->Sequence();

	r_zn_nd = r_zn_nd_tb->GetTable();
	count = r_zn_nd_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_zn_nd_set, sizeof(DMS::R_ZN_ND));
        r_zn_nd_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_ND")), i + 1);
        r_zn_nd_set->ND_id =  (r_zn_nd + i)->nd_id;
        r_zn_nd_set->ZN_id =  (r_zn_nd + i)->zn_id;
        r_zn_nd_set->ND_id_ptr = (r_zn_nd + i)->nd_id_ptr;
        r_zn_nd_set->ZN_id_ptr = (r_zn_nd + i)->zn_id_ptr;
        r_zn_nd_set->fptr = (r_zn_nd + i)->fptr;
        r_zn_nd_set->bptr = (r_zn_nd + i)->bptr;
		if (oodbinsert(r_zn_nd_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_ND")), 1, size) == OO_FAIL)
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

	r_st_bus_tb->Sequence();

	r_st_bus = r_st_bus_tb->GetTable();
	count = r_st_bus_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_bus_set, sizeof(DMS::R_ST_BUS));
		r_st_bus_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_BUS")), i + 1);
		r_st_bus_set->ST_id =  (r_st_bus + i)->st_id;
		r_st_bus_set->BUS_id =  (r_st_bus + i)->bus_id;
		r_st_bus_set->ST_id_ptr = (r_st_bus + i)->st_id_ptr;
		r_st_bus_set->BUS_id_ptr = (r_st_bus + i)->bus_id_ptr;
		r_st_bus_set->fptr = (r_st_bus + i)->fptr;
		r_st_bus_set->bptr = (r_st_bus + i)->bptr;
		if (oodbinsert(r_st_bus_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_BUS")), 1, size) == OO_FAIL)
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

	r_st_cb_tb->Sequence();

	r_st_cb = r_st_cb_tb->GetTable();
	count = r_st_cb_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_cb_set, sizeof(DMS::R_ST_CB));
		r_st_cb_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_CB")), i + 1);
		r_st_cb_set->ST_id =  (r_st_cb + i)->st_id;
		r_st_cb_set->CB_id =  (r_st_cb + i)->cb_id;
		r_st_cb_set->ST_id_ptr = (r_st_cb + i)->st_id_ptr;
		r_st_cb_set->CB_id_ptr = (r_st_cb + i)->cb_id_ptr;
		r_st_cb_set->fptr = (r_st_cb + i)->fptr;
		r_st_cb_set->bptr = (r_st_cb + i)->bptr;
		if (oodbinsert(r_st_cb_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_CB")), 1, size) == OO_FAIL)
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

	r_st_cp_tb->Sequence();

	r_st_cp = r_st_cp_tb->GetTable();
	count = r_st_cp_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_cp_set, sizeof(DMS::R_ST_CP));
		r_st_cp_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_CP")), i + 1);
		r_st_cp_set->ST_id =  (r_st_cp + i)->st_id;
		r_st_cp_set->CP_id =  (r_st_cp + i)->cp_id;
		r_st_cp_set->ST_id_ptr = (r_st_cp + i)->st_id_ptr;
		r_st_cp_set->CP_id_ptr = (r_st_cp + i)->cp_id_ptr;
		r_st_cp_set->fptr = (r_st_cp + i)->fptr;
		r_st_cp_set->bptr = (r_st_cp + i)->bptr;
		if (oodbinsert(r_st_cp_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_CP")), 1, size) == OO_FAIL)
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

	r_st_ld_tb->Sequence();

	r_st_ld = r_st_ld_tb->GetTable();
	count = r_st_ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_ld_set, sizeof(DMS::R_ST_LD));
		r_st_ld_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_LD")), i + 1);
		r_st_ld_set->ST_id =  (r_st_ld + i)->st_id;
		r_st_ld_set->LD_id =  (r_st_ld + i)->ld_id;
		r_st_ld_set->ST_id_ptr = (r_st_ld + i)->st_id_ptr;
		r_st_ld_set->LD_id_ptr = (r_st_ld + i)->ld_id_ptr;
		r_st_ld_set->fptr = (r_st_ld + i)->fptr;
		r_st_ld_set->bptr = (r_st_ld + i)->bptr;
		if (oodbinsert(r_st_ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_LD")), 1, size) == OO_FAIL)
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

	r_st_so_tb->Sequence();

	r_st_so = r_st_so_tb->GetTable();
	count = r_st_so_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_so_set, sizeof(DMS::R_ST_SO));
		r_st_so_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_SO")), i + 1);
		r_st_so_set->ST_id =  (r_st_so + i)->st_id;
		r_st_so_set->SO_id =  (r_st_so + i)->so_id;
		r_st_so_set->ST_id_ptr = (r_st_so + i)->st_id_ptr;
		r_st_so_set->SO_id_ptr = (r_st_so + i)->so_id_ptr;
		r_st_so_set->fptr = (r_st_so + i)->fptr;
		r_st_so_set->bptr = (r_st_so + i)->bptr;
		if (oodbinsert(r_st_so_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_SO")), 1, size) == OO_FAIL)
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

	r_st_xfmr_tb->Sequence();

	r_st_xfmr = r_st_xfmr_tb->GetTable();
	count = r_st_xfmr_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_st_xfmr_set, sizeof(DMS::R_ST_XFMR));
		r_st_xfmr_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_XFMR")), i + 1);
		r_st_xfmr_set->ST_id =  (r_st_xfmr + i)->st_id;
		r_st_xfmr_set->XFMR_id =  (r_st_xfmr + i)->xfmr_id;
		r_st_xfmr_set->ST_id_ptr = (r_st_xfmr + i)->st_id_ptr;
		r_st_xfmr_set->XFMR_id_ptr = (r_st_xfmr + i)->xfmr_id_ptr;
		r_st_xfmr_set->fptr = (r_st_xfmr + i)->fptr;
		r_st_xfmr_set->bptr = (r_st_xfmr + i)->bptr;
		if (oodbinsert(r_st_xfmr_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_XFMR")), 1, size) == OO_FAIL)
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

	r_st_zn_tb->Sequence();

	r_st_zn = r_st_zn_tb->GetTable();
	count = r_st_zn_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_st_zn_set, sizeof(DMS::R_ST_ZN));
		r_st_zn_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_ZN")), i + 1);
		r_st_zn_set->ST_id =  (r_st_zn + i)->st_id;
		r_st_zn_set->ZN_id =  (r_st_zn + i)->zn_id;
		r_st_zn_set->ST_id_ptr = (r_st_zn + i)->st_id_ptr;
		r_st_zn_set->ZN_id_ptr = (r_st_zn + i)->zn_id_ptr;
		r_st_zn_set->fptr = (r_st_zn + i)->fptr;
		r_st_zn_set->bptr = (r_st_zn + i)->bptr;
		if (oodbinsert(r_st_zn_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ST_ZN")), 1, size) == OO_FAIL)
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

	r_xfmr_xf_tb->Sequence();

	r_xfmr_xf = r_xfmr_xf_tb->GetTable();
	count = r_xfmr_xf_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_xfmr_xf_set, sizeof(DMS::R_XFMR_XF));
		r_xfmr_xf_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_XFMR_XF")), i + 1);
		r_xfmr_xf_set->XFMR_id =  (r_xfmr_xf + i)->xfmr_id;
		r_xfmr_xf_set->XF_id =  (r_xfmr_xf + i)->xf_id;
		r_xfmr_xf_set->XFMR_id_ptr = (r_xfmr_xf + i)->xfmr_id_ptr;
		r_xfmr_xf_set->XF_id_ptr = (r_xfmr_xf + i)->xf_id_ptr;
		r_xfmr_xf_set->fptr = (r_xfmr_xf + i)->fptr;
		r_xfmr_xf_set->bptr = (r_xfmr_xf + i)->bptr;
		if (oodbinsert(r_xfmr_xf_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_XFMR_XF")), 1, size) == OO_FAIL)
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

	r_zn_ld_tb->Sequence();

	r_zn_ld = r_zn_ld_tb->GetTable();
	count = r_zn_ld_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_zn_ld_set, sizeof(DMS::R_ZN_LD));
		r_zn_ld_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_LD")), i + 1);
		r_zn_ld_set->ZN_id =  (r_zn_ld + i)->zn_id;
		r_zn_ld_set->LD_id =  (r_zn_ld + i)->ld_id;
		r_zn_ld_set->ZN_id_ptr = (r_zn_ld + i)->zn_id_ptr;
		r_zn_ld_set->LD_id_ptr = (r_zn_ld + i)->ld_id_ptr;
		r_zn_ld_set->fptr = (r_zn_ld + i)->fptr;
		r_zn_ld_set->bptr = (r_zn_ld + i)->bptr;
		if (oodbinsert(r_zn_ld_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_LD")), 1, size) == OO_FAIL)
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

	r_zn_sec_tb->Sequence();

	r_zn_sec = r_zn_sec_tb->GetTable();
	count = r_zn_sec_tb->GetCount();
	for (i = 0; i < count; i++)
	{
        bzero(r_zn_sec_set, sizeof(DMS::R_ZN_SEC));
		r_zn_sec_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_SEC")), i + 1);
		r_zn_sec_set->ZN_id =  (r_zn_sec + i)->zn_id;
		r_zn_sec_set->SEC_id =  (r_zn_sec + i)->sec_id;
		r_zn_sec_set->ZN_id_ptr = (r_zn_sec + i)->zn_id_ptr;
		r_zn_sec_set->SEC_id_ptr = (r_zn_sec + i)->sec_id_ptr;
		r_zn_sec_set->fptr = (r_zn_sec + i)->fptr;
		r_zn_sec_set->bptr = (r_zn_sec + i)->bptr;
		if (oodbinsert(r_zn_sec_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_r_ZN_SEC")), 1, size) == OO_FAIL)
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

	fi_tb->Sequence();

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

		if (oodbinsert(fi_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_FI")), 1, size) == OO_FAIL)
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

	r_sec_fi_tb->Sequence();

	r_sec_fi = r_sec_fi_tb->GetTable();
	count = r_sec_fi_tb->GetCount();
	for (i = 0; i < count; i++)
	{
		bzero(r_sec_fi_set, sizeof(DMS::R_SEC_FI));
		r_sec_fi_set->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_R_SEC_FI")), i + 1);

		r_sec_fi_set->sec_id =  (r_sec_fi + i)->sec_id;
		r_sec_fi_set->fi_id =  (r_sec_fi + i)->fi_id;
		r_sec_fi_set->fi_id_ptr = (r_sec_fi + i)->fi_id_ptr;
		r_sec_fi_set->sec_id_ptr = (r_sec_fi + i)->sec_id_ptr;
		r_sec_fi_set->fptr = (r_sec_fi + i)->fptr;
		r_sec_fi_set->bptr = (r_sec_fi + i)->bptr;

		if (oodbinsert(r_sec_fi_set, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_R_SEC_FI")), 1, size) == OO_FAIL)
		{
			cout << "oodbinsert r_sec_fi error!errno=" << _oodb_errno << endl;

			delete r_sec_fi_set;

			return false;
		}
	}

	delete r_sec_fi_set;

	return true;
}

