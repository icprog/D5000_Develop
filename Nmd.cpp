////////////////////////////////////////////////////////////////////////////////
//
//  NMD.CPP
//  DMS project - Copyright (c) 1999, EPRI
//  Methods implemention of NMD class
//  Developed by Chen Jingcheng, July-1999
//
////////////////////////////////////////////////////////////////////////////////
#include "Nmd.h"
#include "assert.h"
////////////////////////////////////////////////////////////////////////////////
// constructor
NMD::NMD(DSBOB2* m_pDb)
{
    assert(m_pDb != NULL);
    dsbob = m_pDb;

//    digital_tb = dsbob->digital_tb;
//    analog_tb = dsbob->analog_tb;
    co_tb = dsbob->co_tb;
    st_tb = dsbob->st_tb;
    xfmr_tb = dsbob->xfmr_tb;
    xf_tb = dsbob->xf_tb;
    cb_tb = dsbob->cb_tb;
    switch_tb = dsbob->switch_tb;
    so_tb = dsbob->so_tb;
    ld_tb = dsbob->ld_tb;
    cp_tb = dsbob->cp_tb;
    bus_tb = dsbob->bus_tb;
    zn_tb = dsbob->zn_tb;
    sec_tb = dsbob->sec_tb;
    nd_tb = dsbob->nd_tb;
    bs_tb = dsbob->bs_tb;
    island_tb = dsbob->island_tb;
    global_tb = dsbob->global_tb;
    tapty_tb = dsbob->tapty_tb;
    r_co_st_tb = dsbob->r_co_st_tb;
    r_st_xfmr_tb = dsbob->r_st_xfmr_tb;
    r_xfmr_xf_tb = dsbob->r_xfmr_xf_tb;
    r_st_cb_tb = dsbob->r_st_cb_tb;
    r_st_switch_tb = dsbob->r_st_switch_tb;
    r_st_so_tb = dsbob->r_st_so_tb;
    r_st_ld_tb = dsbob->r_st_ld_tb;
    r_st_cp_tb = dsbob->r_st_cp_tb;
    r_st_bus_tb = dsbob->r_st_bus_tb;
    r_st_zn_tb = dsbob->r_st_zn_tb;
    r_zn_ld_tb = dsbob->r_zn_ld_tb;
    r_zn_sec_tb = dsbob->r_zn_sec_tb;
    r_nd_cb_tb = dsbob->r_nd_cb_tb;
    r_nd_switch_tb = dsbob->r_nd_switch_tb;
    r_nd_xf_tb = dsbob->r_nd_xf_tb;
    r_zn_nd_tb = dsbob->r_zn_nd_tb;
    r_nd_sec_tb = dsbob->r_nd_sec_tb;
    r_sec_fi_tb= dsbob->r_sec_fi_tb;
    r_nd_cp_tb = dsbob->r_nd_cp_tb;
    r_nd_ld_tb = dsbob->r_nd_ld_tb;
    fi_tb = dsbob->fi_tb;

    lv_digital = 0;
    lv_analog = 0;
    lv_co = 0;
    lv_st = 0;
    lv_xfmr = 0;
    lv_xf = 0;
    lv_cb = 0;
    lv_switch = 0;
    lv_so = 0;
    lv_ld = 0;
    lv_cp = 0;
    lv_bus = 0;
    lv_zn = 0;
    lv_sec = 0;
    lv_nd = 0;
    lv_bs = 0;
    lv_island = 0;
    lv_global = 0;
    lv_tapty = 0;
    lv_lnty = 0;
    lv_r_co_st = 0;
    lv_r_st_xfmr = 0;
    lv_r_xfmr_xf = 0;
    lv_r_st_cb = 0;
    lv_r_st_switch = 0;
    lv_r_st_so = 0;
    lv_r_st_ld = 0;
    lv_r_st_cp = 0;
    lv_r_st_bus = 0;
    lv_r_st_zn = 0;
    lv_r_zn_ld = 0;
    lv_r_zn_sec = 0;
    lv_r_nd_cb = 0;
    lv_r_nd_switch = 0;
    lv_r_nd_xf = 0;
    lv_r_zn_nd = 0;
    lv_r_nd_sec = 0;
    lv_fi = 0;
    lv_r_sec_fi = 0;


//    digital = 0;
//    analog = 0;
    co = 0;
    st = 0;
    xfmr = 0;
    xf = 0;
    cb = 0;
    so = 0;
    ld = 0;
    cp = 0;
    bus = 0;
    zn = 0;
    sec = 0;
    nd = 0;
    bs = 0;
    island = 0;
    global = 0;
    tapty = 0;
    r_co_st = 0;
    r_st_xfmr = 0;
    r_xfmr_xf = 0;
    r_st_cb = 0;
    r_st_so = 0;
    r_st_ld = 0;
    r_st_cp = 0;
    r_st_bus = 0;
    r_st_zn = 0;
    r_zn_ld = 0;
    r_zn_sec = 0;
    r_nd_cb = 0;
    r_nd_xf = 0;
    r_zn_nd = 0;
    r_nd_sec = 0;
    fi = 0;
    r_sec_fi=0;
}


////////////////////////////////////////////////////////////////////////////////
//  destructor
NMD::~NMD()
{
//    delete[] digital;
//    delete[] analog;
    delete[] co;
    delete[] st;
    delete[] xfmr;
    delete[] xf;
    delete[] cb;
    delete[] so;
    delete[] ld;
    delete[] cp;
    delete[] bus;
    delete[] zn;
    delete[] sec;
    delete[] nd;
    delete[] bs;
    delete[] island;
    delete[] global;
    delete[] tapty;
    delete[] r_co_st;
    delete[] r_st_xfmr;
    delete[] r_xfmr_xf;
    delete[] r_st_cb;
    delete[] r_st_so;
    delete[] r_st_ld;
    delete[] r_st_cp;
    delete[] r_st_bus;
    delete[] r_st_zn;
    delete[] r_zn_ld;
    delete[] r_zn_sec;
    delete[] r_nd_cb;
    delete[] r_nd_xf;
    delete[] r_zn_nd;
    delete[] r_nd_sec;
    delete[] fi;
    delete[] r_sec_fi;
}

////////////////////////////////////////////////////////////////////////////////
//  DoValidate()
int NMD::DoValidate()
{
    if (ValidateStart() == NMD_FAIL)
    {
        cout << "校验初始化有误!" << endl;
        return NMD_FAIL;
    }

    if (ST_Validate() == NMD_FAIL)
    {
        cout << "电站表校验有误!" << endl;
        return NMD_FAIL;
    }

    if (ZN_Validate() == NMD_FAIL)
    {
        cout << "区段表校验有误!" << endl;
        return NMD_FAIL;
    }

    if (CB_Validate() == NMD_FAIL)
    {
        cout << "开关表校验有误!" << endl;
        return NMD_FAIL;
    }

    if (SWITCH_Validate() == NMD_FAIL)
    {
    	cout << "刀闸表校验有误!" << endl;
    	return NMD_FAIL;
    }

    if (BUS_Validate() == NMD_FAIL)
    {

        cout << "母线表校验有误!" << endl;

        return NMD_FAIL;
    }

    if (CP_Validate() == NMD_FAIL)
    {

        cout << "电容表校验有误!" <<endl;

        return NMD_FAIL;
    }

    if (LD_Validate() == NMD_FAIL)
    {

        cout << "负荷表校验有误!" <<endl;

        return NMD_FAIL;
    }

    if (SO_Validate() == NMD_FAIL)
    {

        cout << "电源表校验有误!" <<endl;

        return NMD_FAIL;
    }

    if (SEC_Validate() == NMD_FAIL)
    {

        cout << "线段表校验有误!" <<endl;

        return NMD_FAIL;
    }

    if (XFMR_Validate() == NMD_FAIL)
    {

        cout << "变压器表校验有误!" <<endl;

        return NMD_FAIL;
    }

    if (XF_Validate() == NMD_FAIL)
    {

        cout << "绕组表校验有误!" <<endl;

        return NMD_FAIL;
    }

    if(R_SEC_FI_Build() == NMD_FAIL)
    {
    	cout<<"R_SEC_FI_Build failed"<<endl;
    	return NMD_FAIL;
    }

    if (ValidateEnd() == NMD_FAIL)
    {

        cout << "校验结束有误!" <<endl;

        return NMD_FAIL;
    }

    cout << "校验通过!" <<endl;


    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  Validate_Start()
int	NMD::ValidateStart()
{
    int	i;

    //initialize global data
    global = new CLS_GLOBAL[1];
    global_tb->Empty();

    //set the qcomp mask of all st records to zero
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        cout << "电站表中无记录!" <<endl;

        return NMD_SUCCEED;
    }
    for (i = 0; i < lv_st; i++)
    {
        st_tb->GetDataByLog(i)->qcomp = 0;
    }

    //remove all xf records
    xf_tb->Empty();

    //remove all nd records
    nd_tb->Empty();

    //remove all r_co_st records
    r_co_st_tb->Empty();

    //remove all r_st_xfmr records
    r_st_xfmr_tb->Empty();

    //remove all r_xfmr_xf records
    r_xfmr_xf_tb->Empty();

    //remove all r_st_cb records
    r_st_cb_tb->Empty();

    //remove all r_st_switch records
    r_st_switch_tb->Empty();

    //remove all r_nd_switch records
    r_nd_switch_tb->Empty();

    //remove all r_st_so records
    r_st_so_tb->Empty();

    //remove all r_st_ld records
    r_st_ld_tb->Empty();

    //remove all r_st_cp records
    r_st_cp_tb->Empty();

    //remove all r_st_bus records
    r_st_bus_tb->Empty();

    //remove all r_st_zn records
    r_st_zn_tb->Empty();

    //remove all r_zn_ld records
    r_zn_ld_tb->Empty();

    //remove all r_zn_sec records
    r_zn_sec_tb->Empty();

    //remove all r_nd_cb records
    r_nd_cb_tb->Empty();

    //remove all r_nd_xf records
    r_nd_xf_tb->Empty();

    //remove all r_zn_nd records
    r_zn_nd_tb->Empty();

    //remove all r_nd_sec records
    r_nd_sec_tb->Empty();

    //remove all r_sec_fi records
    r_sec_fi_tb->Empty();

    r_nd_cp_tb->Empty();
    r_nd_ld_tb->Empty();

    //reset pointers in tables
    ResetPtr();

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ValidateEnd()
int	NMD::ValidateEnd()
{
//    int		i, comp;

    //the nds in nd table will not be removed even if
    //there is no component connected to them
    lv_nd = nd_tb->GetCount();
    if (lv_nd == 0)
    {
       cout << "节点表无记录!" <<endl;

        return NMD_SUCCEED;
    }

//    nd = new CLS_ND;
//
//    for (i = 0; i < lv_nd; i++)
//    {
//        nd_tb->CopyDataByLog(nd, i);
//
//        comp = 0;
//        comp += r_nd_cb_tb->FindChildNum(nd->id);
//        comp += r_nd_xf_tb->FindChildNum(nd->id);
////		comp += r_zn_nd_tb->FindChildNum(nd->id);
//        comp += r_nd_sec_tb->FindChildNum(nd->id);
//        comp += r_nd_cp_tb->FindChildNum(nd->id);
//        comp += r_nd_ld_tb->FindChildNum(nd->id);
//
//        if ((nd->q & ND_SO) || (nd->q & ND_LD) || (nd->q & ND_CP))
//        {
//            comp++;
//        }
//
//        if (comp <= 1)
//        {
//        	TRACE("节点(%ld)仅有一个元件相连!\r\n",nd->id);
//
//        }
//    }
//
//    delete nd;
//    nd = 0;

    //update the global data
    if (global->dberrnum != 0)
    {
        global->error = 1;
        global->statusdb = 0;
    }
    else
    {
        global->error = 0;
        global->statusdb = 1;
        global->timedb = time(0);	 //time stamp of validation
    }

    global->id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_GLOBAL")), 1);
    global_tb->Insert(*global);

    delete[] global;
    global = 0;

    //rebuild pointers in tables
    RebuildPtr();

    //build pointers to digital and analog tables
//    DIGITAL_ANALOG_PtrBuild();

    //set vl of nds
    SetVL();
    add_so();
    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_CO_ST_Build()
int	NMD::R_CO_ST_Build()
{
    int		i, j;
    int		first, prevph;
    long	co_id;

    //get record count in co table
    lv_co = co_tb->GetCount();
    if (lv_co == 0)
    {
        return NMD_SUCCEED;
    }

    if (lv_co != 1)
    {
        cout << "公司表中有多条记录!" <<endl;

        return NMD_FAIL;
    }

    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //read co table into buffer
    co = new CLS_CO[lv_co];
    co_tb->CopyTable(co);

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //build relation between co and st
    r_co_st = new CLS_R_CO_ST;
    first = 1;
    i = 0;
    co_id = (co + i)->id;
    for (j = 0; j < lv_st; j++)
    {
        *r_co_st = CLS_R_CO_ST(co_id, (st+j)->id);
        r_co_st->co_id_ptr = co_tb->GetPhByLog(i);
        r_co_st->st_id_ptr = st_tb->GetPhByLog(j);

        (st+j)->r_co = r_co_st_tb->GetNext();

        if (first)
        {
            (co+i)->r_st = r_co_st_tb->GetNext();
            first = 0;
        }
        else
        {
            r_co_st->bptr = prevph;
            r_co_st_tb->GetDataByPh(prevph)->fptr = r_co_st_tb->GetNext();
        }

        prevph = r_co_st_tb->Insert(*r_co_st);
        if (prevph == -1)
        {
            return NMD_FAIL;
        }
    }
    delete r_co_st;
    r_co_st = 0;

    //update co and st tables
    co_tb->UpdateTable(co);
    st_tb->UpdateTable(st);

    delete[] co;
    delete[] st;
    co = 0;
    st = 0;
    lv_co = 0;
    lv_st = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ST_Validate()
int	NMD::ST_Validate()
{
    int		i;

    //first build relation between co and st
    if (R_CO_ST_Build() == NMD_FAIL)
    {
        cout << "公司和电站关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read st table into buffer
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        cout << "电站表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //check st one by one
    for (i = 0; i < lv_st; i++)
    {
        if ((st+i)->r_co == -1)
        {
        	TRACE("公司和电站关系表中无电站(%ld)记录!\r\n",(st + i)->pid);

            global->dberrnum++;
        }
    }

    delete[] st;
    st = 0;
    lv_st = 0;

   cout << "电站表校验通过!" <<endl;


    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_ZN_Build()
int NMD::R_ST_ZN_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;

    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in zn table
    lv_zn = zn_tb->GetCount();
    if (lv_zn == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read zn table into buffer
    zn = new CLS_ZN[lv_zn];
    zn_tb->CopyTable(zn);

    //build relation between st and zn by Ids
    r_st_zn = new CLS_R_ST_ZN;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_zn; j++)
        {
            if (st_id == (zn + j)->stid)
            {
                *r_st_zn = CLS_R_ST_ZN((st + i)->id, (zn+j)->id);
                r_st_zn->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_zn->zn_id_ptr = zn_tb->GetPhByLog(j);

                (zn+j)->r_st = r_st_zn_tb->GetNext();

                if (first)
                {
                    (st+i)->r_zn = r_st_zn_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_zn->bptr = prevph;
                    r_st_zn_tb->GetDataByPh(prevph)->fptr = r_st_zn_tb->GetNext();
                }

                prevph = r_st_zn_tb->Insert(*r_st_zn);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_zn;
    r_st_zn = 0;

    //update st and zn table
    st_tb->UpdateTable(st);
    zn_tb->UpdateTable(zn);

    delete[] st;
    delete[] zn;
    st = 0;
    zn = 0;
    lv_st = 0;
    lv_zn = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ZN_Validate()
int	NMD::ZN_Validate()
{
    int		i, ist, index;

    //first build relation between st and zn
    if (R_ST_ZN_Build() == NMD_FAIL)
    {
        cout << "电站和区段关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read zn table into buffer
    lv_zn = zn_tb->GetCount();
    if (lv_zn == 0)
    {
        cout << "区段表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    zn = new CLS_ZN[lv_zn];
    zn_tb->CopyTable(zn);

    st = new CLS_ST;

    //check zn one by one
    for (i = 0; i < lv_zn; i++)
    {
    	 //地调导入的厂站数据无ABB
    	 //20111201荣义平注释掉
#if 0
        if ((zn+i)->r_st == -1)
        {
//            TRACE("there is no record in r_st_zn table about zn(%d)\n", i);

            TRACE("电站和区段关系表中无区段(%ld)记录!\r\n",(zn+i)->id);

            global->dberrnum++;
        }
        else
#endif
        {
        	for (index = 0; index < (zn + i)->type; index++)
			{
				if (ND_Insert((zn + i)->nds[index].nd_id, &((zn + i)->nds[index].ind), ND_ZN) != NMD_SUCCEED)
				{
					TRACE("fail to insert nd for zn (%d)\n", i);
					break;
				}
			}
        }
    }

    //update sec table
    zn_tb->UpdateTable(zn);

    delete st;
    delete[] zn;
    st = 0;
    zn = 0;
    lv_zn = 0;

	//first build relation between st and zn
	if (R_ZN_ND_Build() == NMD_FAIL)
	{
		cout << "区段与结点关系表建立失败!" << endl;

		return NMD_FAIL;
	}

   cout << "区段表校验通过!" <<endl;


    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_CB_Build()
int	NMD::R_ST_CB_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;


    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in cb table
    lv_cb = cb_tb->GetCount();
    if (lv_cb == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read cb table into buffer
    cb = new CLS_CB[lv_cb];
    cb_tb->CopyTable(cb);

    //build relation between kv and cb by Ids
    r_st_cb = new CLS_R_ST_CB;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_cb; j++)
        {
//            if (strncmp(st_abb, (cb+j)->id, ST_ABB_LEN) == 0)	//荣义平于20110928注释掉
        	if (st_id == (cb + j)->stid) //荣义平于20110928添加
            {
                *r_st_cb = CLS_R_ST_CB((st + i)->id, (cb+j)->id);
                r_st_cb->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_cb->cb_id_ptr = cb_tb->GetPhByLog(j);

                (cb+j)->r_st = r_st_cb_tb->GetNext();

                if (first)
                {
                    (st+i)->r_cb = r_st_cb_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_cb->bptr = prevph;
                    r_st_cb_tb->GetDataByPh(prevph)->fptr = r_st_cb_tb->GetNext();
                }

                prevph = r_st_cb_tb->Insert(*r_st_cb);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_cb;
    r_st_cb = 0;

    //update st and cb tables
    st_tb->UpdateTable(st);
    cb_tb->UpdateTable(cb);

    delete[] st;
    delete[] cb;
    st = 0;
    cb = 0;
    lv_st = 0;
    lv_cb = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_SWITCH_Build()
int	NMD::R_ST_SWITCH_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;


    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in switch table
    lv_switch = switch_tb->GetCount();
    if (lv_switch == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read switch table into buffer
    swh = new CLS_SWITCH[lv_switch];
    switch_tb->CopyTable(swh);

    //build relation between kv and switch by Ids
    r_st_switch = new CLS_R_ST_SWITCH;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_switch; j++)
        {
//            if (strncmp(st_abb, (switch+j)->id, ST_ABB_LEN) == 0)	//荣义平于20110928注释掉
        	if (st_id == (swh + j)->stid) //荣义平于20110928添加
            {
                *r_st_switch = CLS_R_ST_SWITCH((st + i)->id, (swh+j)->id);
                r_st_switch->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_switch->switch_id_ptr = switch_tb->GetPhByLog(j);

                (swh+j)->r_st = r_st_switch_tb->GetNext();

                if (first)
                {
                    (st+i)->r_switch = r_st_switch_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_switch->bptr = prevph;
                    r_st_switch_tb->GetDataByPh(prevph)->fptr = r_st_switch_tb->GetNext();
                }

                prevph = r_st_switch_tb->Insert(*r_st_switch);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_switch;
    r_st_switch = 0;

    //update st and switch tables
    st_tb->UpdateTable(st);
    switch_tb->UpdateTable(swh);

    delete[] st;
    delete[] swh;
    st = 0;
    swh = 0;
    lv_st = 0;
    lv_switch = 0;

    return NMD_SUCCEED;
}


////////////////////////////////////////////////////////////////////////////////
//  CB_Validate()
int NMD::CB_Validate()
{
    int		i, j, ist;

    //build relation between st and cb
    if (R_ST_CB_Build() == NMD_FAIL)
    {
        cout << "电站和开关关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read cb table into buffer
    lv_cb = cb_tb->GetCount();
    if (lv_cb == 0)
    {
        cout << "开关表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    cb = new CLS_CB[lv_cb];
    cb_tb->CopyTable(cb);

    st = new CLS_ST;

    //check cb one by one
    for (i = 0; i < lv_cb; i++)
    {
        if ((cb+i)->r_st == -1)
        {
//            TRACE("there is no record in r_st_cb table about cb (%d)\n", i);

            TRACE("电站和开关关系表中无开关(%ld)记录!\r\n",(cb+i)->id);

            global_tb->Data->dberrnum++;
        }
        else
        {
            //check cb's type
            j = 0;
            if ( (cb+i)->q & CB_BREAKER ) j++;
            if ( (cb+i)->q & CB_MOTODIS ) j++;
            if ( (cb+i)->q & CB_SWITCH  ) j++;
            if ( j == 0 || j > 1 )
            {
//                TRACE("the type of cb (%d) is illegal\n", i);

                TRACE("开关(%ld)类型错误!\r\n",(cb + i)->id);

                global->dberrnum++;
            }

            j = 0;
            if ( (cb+i)->q & CB_FDHCB ) j++;
            if ( (cb+i)->q & CB_FDSCB ) j++;
            if ( (cb+i)->q & CB_FDCCB ) j++;
            if ( (cb+i)->q & CB_FDLCB ) j++;
            if ( j > 1 )
            {
//                TRACE("the type of feeder cb (%d) is illegal\n", i);

                TRACE("馈线开关(%ld)类型错误!\r\n",(cb + i)->id);

                global->dberrnum++;
            }

            //insert nd table
            if ( ND_Insert((cb+i)->nd,&((cb+i)->ind),-1) == NMD_SUCCEED &&
                    ND_Insert((cb+i)->znd,&((cb+i)->iznd),-1) == NMD_SUCCEED )
            {
                //insert r_nd_cb table
                if ( ND_BR_Insert(_CB,(cb+i)->ind,(cb+i)->nd,i,(cb+i)->id) == NMD_FAIL ||
                        ND_BR_Insert(_CB,(cb+i)->iznd,(cb+i)->znd,i,(cb+i)->id) == NMD_FAIL )
                {
                    TRACE("开关(%ld)的节点和开关关系建立失败!\r\n",(cb + i)->id);

                    return NMD_FAIL;
                }

            }
            else
            {
                TRACE("开关(%ld)插入节点失败!\r\n",(cb + i)->id);

            }

            //mark the station having a cb
            ist = (cb+i)->r_st;
            ist = r_st_cb_tb->GetDataByPh(ist)->st_id_ptr;
            st_tb->CopyDataByPh(st, ist);
            st->qcomp = st->qcomp | ST_CB;
            st_tb->UpdateDataByPh(st, ist);
        }
    }

    //update cb table
    cb_tb->UpdateTable(cb);

    delete st;
    delete[] cb;
    st = 0;
    cb = 0;
    lv_cb = 0;

    cout << "开关表校验通过!" <<endl;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CB_Validate()
int NMD::SWITCH_Validate()
{
    int		i, j;

    //build relation between st and swh
    if (R_ST_SWITCH_Build() == NMD_FAIL)
    {
        cout << "电站和刀闸关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read swh table into buffer
    lv_switch = switch_tb->GetCount();
    if (lv_switch == 0)
    {
        cout << "刀闸表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    swh = new CLS_SWITCH[lv_switch];
    switch_tb->CopyTable(swh);

    st = new CLS_ST;

    //check swh one by one
    for (i = 0; i < lv_switch; i++)
    {
        if ((swh+i)->r_st == -1)
        {
//            TRACE("there is no record in r_st_cb table about swh (%d)\n", i);

            TRACE("电站和刀闸关系表中无刀闸(%ld)记录!\r\n",(swh+i)->id);

            global_tb->Data->dberrnum++;
        }
        else
        {
        	//insert nd table
            if ( ND_Insert((swh+i)->nd,&((swh+i)->ind),-1) == NMD_SUCCEED &&
                    ND_Insert((swh+i)->znd,&((swh+i)->iznd),-1) == NMD_SUCCEED )
            {
                //insert r_nd_cb table
                if ( ND_BR_Insert(_SWITCH,(swh+i)->ind,(swh+i)->nd,i,(swh+i)->id) == NMD_FAIL ||
                        ND_BR_Insert(_SWITCH,(swh+i)->iznd,(swh+i)->znd,i,(swh+i)->id) == NMD_FAIL )
                {
                    TRACE("刀闸(%ld)的节点和刀闸关系建立失败!\r\n",(swh + i)->id);

                    return NMD_FAIL;
                }

            }
            else
            {
                TRACE("刀闸(%ld)插入节点失败!\r\n",(swh + i)->id);

            }
        }
    }

    //update swh table
    switch_tb->UpdateTable(swh);

    delete st;
    delete[] swh;
    st = 0;
    swh = 0;
    lv_switch = 0;

    cout << "刀闸表校验通过!" <<endl;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_BUS_Build()
int	NMD::R_ST_BUS_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;


    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in bus table
    lv_bus = bus_tb->GetCount();
    if (lv_bus == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read bus table into buffer
    bus = new CLS_BUS[lv_bus];
    bus_tb->CopyTable(bus);

    //build relation between st and bus by Ids
    r_st_bus = new CLS_R_ST_BUS;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_bus; j++)
        {
//            if (strncmp(st_abb, (bus+j)->id, ST_ABB_LEN) == 0)	//荣义平于20110927注释掉
        	if (st_id == (bus + j)->stid)					//荣义平于20110927添加
            {
                *r_st_bus = CLS_R_ST_BUS((st + i)->id, (bus+j)->id);
                r_st_bus->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_bus->bus_id_ptr = bus_tb->GetPhByLog(j);

                (bus+j)->r_st = r_st_bus_tb->GetNext();

                if (first)
                {
                    (st+i)->r_bus = r_st_bus_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_bus->bptr = prevph;
                    r_st_bus_tb->GetDataByPh(prevph)->fptr = r_st_bus_tb->GetNext();
                }

                prevph = r_st_bus_tb->Insert(*r_st_bus);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_bus;
    r_st_bus = 0;

    //update st and bus tables
    st_tb->UpdateTable(st);
    bus_tb->UpdateTable(bus);

    delete[] st;
    delete[] bus;
    st = 0;
    bus = 0;
    lv_st = 0;
    lv_bus = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  BUS_Validate()
int	NMD::BUS_Validate()
{
    int		i;
    float   vl;

    //first build relation between st and bus
    if (R_ST_BUS_Build() == NMD_FAIL)
    {
        cout << "电站和母线关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read bus table into buffer
    lv_bus = bus_tb->GetCount();
    if (lv_bus == 0)
    {
        cout << "该母线表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    bus = new CLS_BUS[lv_bus];
    bus_tb->CopyTable(bus);

    //check bus one by one
    for (i = 0; i < lv_bus; i++)
    {
        if ((bus+i)->r_st == -1)
        {
//            TRACE("there is no record in r_st_bus table about bus (%d)\n", i);

            TRACE("电站和母线关系表中无母线(%ld)记录!\r\n",(bus+i)->id);

            global->dberrnum++;
        }
        else
        {
            //insert new nd for this bus
            if (ND_Insert((bus+i)->nd,&(bus+i)->ind,-1) != NMD_SUCCEED)
            {
                TRACE("母线(%ld)插入节点失败!\r\n",(bus + i)->id);

                return NMD_FAIL;
            }

            //check vl of this bus
            vl = (bus+i)->vl;
            if (vl != 10 && vl != 35 && vl != 110 && vl != 220)
            {
                TRACE("母线(%ld)电压取值错误!\r\n",(bus + i)->id);

            }
        }
    }

    //update bus table
    bus_tb->UpdateTable(bus);

    delete[] bus;
    bus = 0;
    lv_bus = 0;

    cout << "母线表校验通过!" <<endl;


    return NMD_SUCCEED;
}

int NMD::R_SEC_FI_Build()
{
	int i, j, first, prevph;

	lv_sec = sec_tb->GetCount();
	if (lv_sec == 0)
	{
		return NMD_SUCCEED;
	}

	lv_fi = fi_tb->GetCount();
	if (lv_fi == 0)
	{
		return NMD_SUCCEED;
	}

	sec = new CLS_SEC[lv_sec];
	sec_tb->CopyTable(sec);

	//read cp table into buffer
	fi = new CLS_FI[lv_fi];
	fi_tb->CopyTable(fi);

	//build relation between st and cp by Ids
	r_sec_fi = new CLS_R_SEC_FI;
	for (i = 0; i < lv_sec; i++)
	{
		first = 1;

		for (j = 0; j < lv_fi; j++)
		{
			if ((fi + j)->sec == (sec + i)->id)
			{
				*r_sec_fi = CLS_R_SEC_FI((sec + i)->id, (fi + j)->id);
				r_sec_fi->sec_id_ptr = sec_tb->GetPhByLog(i);
				r_sec_fi->fi_id_ptr = fi_tb->GetPhByLog(j);

				(fi + j)->r_sec = r_sec_fi_tb->GetNext();

				if (first)
				{
					(sec + i)->r_fi = r_sec_fi_tb->GetNext();
					first = 0;
				}
				else
				{
					r_sec_fi->bptr = prevph;
					r_sec_fi_tb->GetDataByPh(prevph)->fptr = r_sec_fi_tb->GetNext();
				}

				prevph = r_sec_fi_tb->Insert(*r_sec_fi);
				if (prevph == -1)
				{
					return NMD_FAIL;
				}
			}
		}
	}

	delete r_sec_fi;
	r_sec_fi = 0;

	//update sec and fi tables
	sec_tb->UpdateTable(sec);
	fi_tb->UpdateTable(fi);

	delete[] sec;
	delete[] fi;
	sec = 0;
	fi = 0;
	lv_sec = 0;
	lv_fi = 0;

	return NMD_SUCCEED;
}
////////////////////////////////////////////////////////////////////////////////
//  R_ST_CP_Build()
int	NMD::R_ST_CP_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;

    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in cp table
    lv_cp = cp_tb->GetCount();
    if (lv_cp == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read cp table into buffer
    cp = new CLS_CP[lv_cp];
    cp_tb->CopyTable(cp);

    //build relation between st and cp by Ids
    r_st_cp = new CLS_R_ST_CP;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_cp; j++)
        {
            if (st_id == (cp + j)->stid)
            {
                *r_st_cp = CLS_R_ST_CP((st + i)->id, (cp+j)->id);
                r_st_cp->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_cp->cp_id_ptr = cp_tb->GetPhByLog(j);

                (cp+j)->r_st = r_st_cp_tb->GetNext();

                if (first)
                {
                    (st+i)->r_cp = r_st_cp_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_cp->bptr = prevph;
                    r_st_cp_tb->GetDataByPh(prevph)->fptr = r_st_cp_tb->GetNext();
                }

                prevph = r_st_cp_tb->Insert(*r_st_cp);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_cp;
    r_st_cp = 0;

    //update st and cp tables
    st_tb->UpdateTable(st);
    cp_tb->UpdateTable(cp);

    delete[] st;
    delete[] cp;
    st = 0;
    cp = 0;
    lv_st = 0;
    lv_cp = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CP_Validate()
int NMD::CP_Validate()
{
    int		i, ist;

    //build relation between st and cp
    if (R_ST_CP_Build() == NMD_FAIL)
    {
        cout << "电站和电容/电抗关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read cp table into buffer
    lv_cp = cp_tb->GetCount();
    if (lv_cp == 0)
    {
        cout << "电容/电抗表无记录!" <<endl;

        return NMD_SUCCEED;
    }

    cp = new CLS_CP[lv_cp];
    cp_tb->CopyTable(cp);

    st = new CLS_ST;

    //check cp one by one
    for (i = 0; i < lv_cp; i++)
    {
        if ((cp+i)->r_st == -1)
        {
        	TRACE("电站和电容关系表中无电容(%ld)记录!\r\n",(cp+i)->id);

            global_tb->Data->dberrnum++;
        }
        else
        {
            //insert new nd for this cp
            if ( ND_Insert((cp+i)->nd,&((cp+i)->ind),ND_CP) == NMD_SUCCEED)
            {
            	if(ND_BR_Insert(_CP,(cp+i)->ind,(cp+i)->nd,i,(cp+i)->id) == NMD_FAIL)
            	{
            		TRACE("fail to insert r_nd_cp for cp (%d)\n", i);

            		return NMD_FAIL;
            	}
            }
            else
            {
               TRACE("电容(%ld)插入节点失败!\r\n",(cp+i)->id);

                return NMD_FAIL;
            }

            //mark the station having a cp
            ist = (cp+i)->r_st;
            ist = r_st_cp_tb->GetDataByPh(ist)->st_id_ptr;
            st_tb->CopyDataByPh(st, ist);
            st->qcomp = st->qcomp | ST_CP;
            st_tb->UpdateDataByPh(st, ist);
        }
    }

    //update cp table
    cp_tb->UpdateTable(cp);

    delete st;
    delete[] cp;
    st = 0;
    cp = 0;
    lv_cp = 0;

    cout << "电容表校验通过!" <<endl;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_LD_Build()
int	NMD::R_ST_LD_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;

    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in ld table
    lv_ld = ld_tb->GetCount();
    if (lv_ld == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read ld table into buffer
    ld = new CLS_LD[lv_ld];
    ld_tb->CopyTable(ld);

    //build relation between st and ld by Ids
    r_st_ld = new CLS_R_ST_LD;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_ld; j++)
        {
            if ((ld+j)->type != 0) //not ld within st
            {
                continue;
            }

//            if (strncmp(st_abb, (ld+j)->id, ST_ABB_LEN) == 0)	//荣义平于20110928注释掉
            if (st_id == (ld + j)->stid) //荣义平于20110928添加
            {
                *r_st_ld = CLS_R_ST_LD((st + i)->id, (ld+j)->id);
                r_st_ld->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_ld->ld_id_ptr = ld_tb->GetPhByLog(j);

                (ld+j)->r_st = r_st_ld_tb->GetNext();

                if (first)
                {
                    (st+i)->r_ld = r_st_ld_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_ld->bptr = prevph;
                    r_st_ld_tb->GetDataByPh(prevph)->fptr = r_st_ld_tb->GetNext();
                }

                prevph = r_st_ld_tb->Insert(*r_st_ld);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_ld;
    r_st_ld = 0;

    //update st and ld tables
    st_tb->UpdateTable(st);
    ld_tb->UpdateTable(ld);

    delete[] st;
    delete[] ld;
    st = 0;
    ld = 0;
    lv_st = 0;
    lv_ld = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ZN_LD_Build()
int	NMD::R_ZN_LD_Build()
{
    int		i, j;
    int		first, prevph;
    long	zn_id;


    //get record count in zn table
    lv_zn = zn_tb->GetCount();
    if (lv_zn == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in ld table
    lv_ld = ld_tb->GetCount();
    if (lv_ld == 0)
    {
        return NMD_SUCCEED;
    }

    //read zn table into buffer
    zn = new CLS_ZN[lv_zn];
    zn_tb->CopyTable(zn);

    //read ld table into buffer
    ld = new CLS_LD[lv_ld];
    ld_tb->CopyTable(ld);

    //build relation between zn and ld by Ids
    r_zn_ld = new CLS_R_ZN_LD;
    for (i = 0; i < lv_zn; i++)
    {
        first = 1;
        zn_id=(zn+i)->id;
        for (j = 0; j < lv_ld; j++)
        {
            if ((ld+j)->type != 1) //not ld within zn
            {
                continue;
            }

#if 0 //荣义平修改于20121214 建立ZN与LD间的关系
            if (strncmp(zn_id, (ld+j)->id, ZN_ID_LEN) == 0)
#else
            if (zn_id == (ld + j)->zn_id)
#endif
            {
                *r_zn_ld = CLS_R_ZN_LD(zn_id, (ld+j)->id);
                r_zn_ld->zn_id_ptr = zn_tb->GetPhByLog(i);
                r_zn_ld->ld_id_ptr = ld_tb->GetPhByLog(j);

                (ld+j)->r_zn = r_zn_ld_tb->GetNext();

                if (first)
                {
                    (zn+i)->r_ld = r_zn_ld_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_zn_ld->bptr = prevph;
                    r_zn_ld_tb->GetDataByPh(prevph)->fptr = r_zn_ld_tb->GetNext();
                }

                prevph = r_zn_ld_tb->Insert(*r_zn_ld);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_zn_ld;
    r_zn_ld = 0;

    //update zn and ld tables
    zn_tb->UpdateTable(zn);
    ld_tb->UpdateTable(ld);

    delete[] zn;
    delete[] ld;
    zn = 0;
    ld = 0;
    lv_zn = 0;
    lv_ld = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  LD_Validate()
int NMD::LD_Validate()
{
    int		i, ist;

    //build relation between st and ld
    if (R_ST_LD_Build() == NMD_FAIL)
    {
        cout << "电站和负荷关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //build relation between zn and ld
    if (R_ZN_LD_Build() == NMD_FAIL)
    {
        cout << "区段和负荷关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read ld table into buffer
    lv_ld = ld_tb->GetCount();
    if (lv_ld == 0)
    {
        cout << "负荷表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    ld = new CLS_LD[lv_ld];
    ld_tb->CopyTable(ld);

    st = new CLS_ST;

    //check ld one by one
    for (i = 0; i < lv_ld; i++)
    {
        if ((ld+i)->type == 0) //ld within st
        {
            if ((ld+i)->r_st == -1)
            {
            	TRACE("电站和负荷关系表中无负荷%ld记录!\r\n",(ld+i)->id);

                global_tb->Data->dberrnum++;
            }
            else
            {
                //insert nd for this ld
                if (ND_Insert((ld + i)->nd, &(ld + i)->ind, ND_LD) == NMD_SUCCEED)
				{
					if (ND_BR_Insert(_LD, (ld + i)->ind, (ld + i)->nd, i, (ld + i)->id) == NMD_FAIL)
					{
						TRACE("fail to insert r_nd_ld for ld (%d)\n", i);

						return NMD_FAIL;
					}
				}
                else
                {
                    return NMD_FAIL;
                }

                //mark the station having a ld
                ist = (ld+i)->r_st;
                ist = r_st_ld_tb->GetDataByPh(ist)->st_id_ptr;
                st_tb->CopyDataByPh(st, ist);
                st->qcomp = st->qcomp | ST_LD;
                st_tb->UpdateDataByPh(st, ist);
            }
        }
        else if ((ld+i)->type == 1) //ld within zn
        {
            if ((ld+i)->r_zn == -1)
            {
            	TRACE("区段和负荷关系表中无负荷(%ld)记录!\r\n",(ld+i)->id);

                global_tb->Data->dberrnum++;
            }
            else
            {
                //insert nd for this ld
                if (ND_Insert((ld + i)->nd, &(ld + i)->ind, ND_LD) == NMD_SUCCEED)
				{
					if (ND_BR_Insert(_LD, (ld + i)->ind, (ld + i)->nd, i, (ld + i)->id) == NMD_FAIL)
					{
						TRACE("fail to insert r_nd_ld for ld (%d)\n", i);

						return NMD_FAIL;
					}
				}
                else
                {
                    return NMD_FAIL;
                }
            }
        }
    }

    //update ld table
    ld_tb->UpdateTable(ld);

    delete st;
    delete[] ld;
    st = 0;
    ld = 0;
    lv_ld = 0;

    cout << "负荷表校验通过!" <<endl;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_SO_Build()
int	NMD::R_ST_SO_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;

    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in so table
    lv_so = so_tb->GetCount();
    if (lv_so == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read so table into buffer
    so = new CLS_SO[lv_so];
    so_tb->CopyTable(so);

    //build relation between st and so by Ids
    r_st_so = new CLS_R_ST_SO;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_so; j++)
        {
            if (st_id == (so + j)->stid)
            {
                *r_st_so = CLS_R_ST_SO((st + i)->id, (so+j)->id);
                r_st_so->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_so->so_id_ptr = so_tb->GetPhByLog(j);

                (so+j)->r_st = r_st_so_tb->GetNext();

                if (first)
                {
                    (st+i)->r_so = r_st_so_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_so->bptr = prevph;
                    r_st_so_tb->GetDataByPh(prevph)->fptr = r_st_so_tb->GetNext();
                }

                prevph = r_st_so_tb->Insert(*r_st_so);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_so;
    r_st_so = 0;

    //update st and so tables
    st_tb->UpdateTable(st);
    so_tb->UpdateTable(so);

    delete[] st;
    delete[] so;
    st = 0;
    so = 0;
    lv_st = 0;
    lv_so = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  SO_Validate()
int NMD::SO_Validate()
{
    int		i, ist;

    //first build relation between st and so
    if (R_ST_SO_Build() == NMD_FAIL)
    {
        cout << "电站和电源关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read so table into buffer
    lv_so = so_tb->GetCount();
    if (lv_so == 0)
    {
        cout << "电源表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    so = new CLS_SO[lv_so];
    so_tb->CopyTable(so);

    st = new CLS_ST;

    //check so one by one
    for (i = 0; i < lv_so; i++)
    {
        if ((so+i)->r_st == -1)
        {
//            TRACE("there is no record in r_st_so table about so (%d)\n", i);

            TRACE("电站和电源关系表中无电源(%ld)记录!\r\n",(so+i)->id);

            global_tb->Data->dberrnum++;
        }
        else
        {
            //insert nd for this so
            if (ND_Insert((so+i)->nd,&(so+i)->ind,ND_SO) != NMD_SUCCEED)
            {
                return NMD_FAIL;
            }

            //mark the station having a so
            ist = (so+i)->r_st;
            ist = r_st_so_tb->GetDataByPh(ist)->st_id_ptr;
            st_tb->CopyDataByPh(st, ist);
            st->qcomp = st->qcomp | ST_SO;
            st_tb->UpdateDataByPh(st, ist);
        }
    }

    //update so table
    so_tb->UpdateTable(so);

    delete st;
    delete[] so;
    st = 0;
    so = 0;
    lv_so = 0;

    cout << "电源表校验通过!" <<endl;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ZN_SEC_Build()
int	NMD::R_ZN_SEC_Build()
{
    int		i, j;
    int		first, prevph;
    long	zn_id;


    //get record count in zn table
    lv_zn = zn_tb->GetCount();
    if (lv_zn == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in sec table
    lv_sec = sec_tb->GetCount();
    if (lv_sec == 0)
    {
        return NMD_SUCCEED;
    }

    //read zn table into buffer
    zn = new CLS_ZN[lv_zn];
    zn_tb->CopyTable(zn);

    //read sec table into buffer
    sec = new CLS_SEC[lv_sec];
    sec_tb->CopyTable(sec);

    //build relation between zn and sec by Ids
    r_zn_sec = new CLS_R_ZN_SEC;
    for (i = 0; i < lv_zn; i++)
    {
        first = 1;
        zn_id = (zn + i)->id;
        for (j = 0; j < lv_sec; j++)
        {
#if 0
            if (strncmp(zn_id, (sec+j)->id, ZN_ID_LEN) == 0)
#else
            if (zn_id == (sec + j)->zn_id)
#endif
            {
                *r_zn_sec = CLS_R_ZN_SEC(zn_id, (sec+j)->id);
                r_zn_sec->zn_id_ptr = zn_tb->GetPhByLog(i);
                r_zn_sec->sec_id_ptr = sec_tb->GetPhByLog(j);

                (sec+j)->r_zn = r_zn_sec_tb->GetNext();

                if (first)
                {
                    (zn+i)->r_sec = r_zn_sec_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_zn_sec->bptr = prevph;
                    r_zn_sec_tb->GetDataByPh(prevph)->fptr = r_zn_sec_tb->GetNext();
                }

                prevph = r_zn_sec_tb->Insert(*r_zn_sec);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_zn_sec;
    r_zn_sec = 0;

    //update zn and sec tables
    zn_tb->UpdateTable(zn);
    sec_tb->UpdateTable(sec);

    delete[] zn;
    delete[] sec;
    zn = 0;
    sec = 0;
    lv_zn = 0;
    lv_sec = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ZN_ND_Build()
int	NMD::R_ZN_ND_Build()
{
    int		i, j;
    int		first, prevph;
    long	zn_id;


    //get record count in zn table
    lv_zn = zn_tb->GetCount();
    if (lv_zn == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in nd table
    lv_nd = nd_tb->GetCount();
    if (lv_nd == 0)
    {
        return NMD_SUCCEED;
    }

    //read zn table into buffer
    zn = new CLS_ZN[lv_zn];
    zn_tb->CopyTable(zn);

    //read nd table into buffer
    nd = new CLS_ND[lv_nd];
    nd_tb->CopyTable(nd);

    //build relation between zn and nd by Ids
    r_zn_nd = new CLS_R_ZN_ND;
    for (i = 0; i < lv_zn; i++)
	{
		first = 1;
		zn_id = (zn + i)->id;
		for (j = 0; j < (zn + i)->type; j++)
		{
			*r_zn_nd = CLS_R_ZN_ND(zn_id, (zn + i)->nds[j].nd_id);
			r_zn_nd->zn_id_ptr = zn_tb->GetPhByLog(i);
			r_zn_nd->nd_id_ptr = nd_tb->GetPhByLog((zn + i)->nds[j].ind);

			(nd + (zn + i)->nds[j].ind)->zn_id = zn_id;

			if (first)
			{
				(zn + i)->r_nd = r_zn_nd_tb->GetNext();
				first = 0;
			}
			else
			{
				r_zn_nd->bptr = prevph;
				r_zn_nd_tb->GetDataByPh(prevph)->fptr = r_zn_nd_tb->GetNext();
			}

			prevph = r_zn_nd_tb->Insert(*r_zn_nd);
			if (prevph == -1)
			{
				return NMD_FAIL;
			}
		}
	}
    delete r_zn_nd;
    r_zn_nd = 0;

    //update zn and nd tables
    zn_tb->UpdateTable(zn);
    nd_tb->UpdateTable(nd);

    delete[] zn;
    delete[] nd;
    zn = 0;
    nd = 0;
    lv_zn = 0;
    lv_nd = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  SEC_Validate()
int NMD::SEC_Validate()
{
    int		i;

    //build relation between zn and sec
    if (R_ZN_SEC_Build() == NMD_FAIL)
    {
        cout << "区段和线段关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read sec table into buffer
    lv_sec = sec_tb->GetCount();
    if (lv_sec == 0)
    {
        cout << "线段表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    sec = new CLS_SEC[lv_sec];
    sec_tb->CopyTable(sec);

    //check sec one by one
    for (i = 0; i < lv_sec; i++)
    {
    	//暂时屏蔽掉区段_ryp
//        if ((sec+i)->r_zn == -1)
//        {
//            TRACE("there is no record in r_zn_sec table about sec (%d)\n", i);
//
//            TRACE("区段和线段关系表中无线段(%ld)记录!",(sec+i)->id);
//
//            global_tb->Data->dberrnum++;
//        }
//        else
        {
            //insert nd table
        	if((sec+i)->nd == 0 || (sec+i)->znd == 0)
        	{
//        		TRACE("线段%ld无结点信息\r\n", (sec+ i)->id);
        		continue;
        	}

            if ( ND_Insert((sec+i)->nd,&((sec+i)->ind),-1) == NMD_SUCCEED &&
                    ND_Insert((sec+i)->znd,&((sec+i)->iznd),-1) == NMD_SUCCEED )
            {
                if ( ND_BR_Insert(_SEC,(sec+i)->ind,(sec+i)->nd,i,(sec+i)->id) == NMD_FAIL ||
                        ND_BR_Insert(_SEC,(sec+i)->iznd,(sec+i)->znd,i,(sec+i)->id) == NMD_FAIL )
                {
                    TRACE("线段(%ld)的节点和线段关系建立失败!\r\n",(sec+ i)->id);

                    return NMD_FAIL;
                }
            }
            else
            {
                TRACE("线段(%ld)插入节点失败!\r\n",(sec+ i)->id);

                return NMD_FAIL;
            }
        }
    }

    //update sec table
    sec_tb->UpdateTable(sec);

    delete[] sec;
    sec = 0;
    lv_sec = 0;

    cout << "线段表校验通过!" <<endl;


    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_XFMR_Build()
int NMD::R_ST_XFMR_Build()
{
    int		i, j;
    int		first, prevph;
    long	st_id;

    //get record count in st table
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    //get record count in xfmr table
    lv_xfmr = xfmr_tb->GetCount();
    if (lv_xfmr == 0)
    {
        return NMD_SUCCEED;
    }

    //read st table into buffer
    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //read xfmr table into buffer
    xfmr = new CLS_XFMR[lv_xfmr];
    xfmr_tb->CopyTable(xfmr);

    //build relation between st and xfmr by Ids
    r_st_xfmr = new CLS_R_ST_XFMR;
    for (i = 0; i < lv_st; i++)
    {
        first = 1;
        st_id = (st + i)->pid;

        for (j = 0; j < lv_xfmr; j++)
        {
            if (st_id == (xfmr + j)->stid) //Attention: using abb
            {
                *r_st_xfmr = CLS_R_ST_XFMR((st + i)->id, (xfmr+j)->id);
                r_st_xfmr->st_id_ptr = st_tb->GetPhByLog(i);
                r_st_xfmr->xfmr_id_ptr = xfmr_tb->GetPhByLog(j);

                (xfmr+j)->r_st = r_st_xfmr_tb->GetNext();

                if (first)
                {
                    (st+i)->r_xfmr = r_st_xfmr_tb->GetNext();
                    first = 0;
                }
                else
                {
                    r_st_xfmr->bptr = prevph;
                    r_st_xfmr_tb->GetDataByPh(prevph)->fptr = r_st_xfmr_tb->GetNext();
                }

                prevph = r_st_xfmr_tb->Insert(*r_st_xfmr);
                if (prevph == -1)
                {
                    return NMD_FAIL;
                }
            }
        }
    }
    delete r_st_xfmr;
    r_st_xfmr = 0;

    //update st and xfmr table
    st_tb->UpdateTable(st);
    xfmr_tb->UpdateTable(xfmr);

    delete[] st;
    delete[] xfmr;
    st = 0;
    xfmr = 0;
    lv_st = 0;
    lv_xfmr = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  XFMR_Validate()
int	NMD::XFMR_Validate()
{
#if 0	//D5000 ID为自动生成的LONG型数据，没办法用字母区别高、中、低压侧
    int		i, prevph, ist;
    int		hnd, mnd, lnd, cnd;
    char	xf_id[MAX_XF_ID_LEN], nd_id[MAX_ND_ID_LEN];

    //first build relation between st and xfmr
    if (R_ST_XFMR_Build() == NMD_FAIL)
    {
        TRACE("the r_st_xfmr table has not built up\n");

        cout << "电站和变压器关系表建立失败!" <<endl;

        return NMD_FAIL;
    }

    //read xfmr table into buffer
    lv_xfmr = xfmr_tb->GetCount();
    if (lv_xfmr == 0)
    {
        TRACE("there is no record in xfmr table\n");

        cout << "变压器表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    xfmr = new CLS_XFMR[lv_xfmr];
    xfmr_tb->CopyTable(xfmr);

    st = new CLS_ST;
    xf = new CLS_XF;
    r_xfmr_xf = new CLS_R_XFMR_XF;

    //check xfmr one by one
    for (i = 0; i < lv_xfmr; i++)
    {
        if ((xfmr+i)->r_st == -1)
        {
//            TRACE("there is no record in r_st_xfmr table about xfmr(%d)\n", i);

            TRACE("电站和变压器关系表中无变压器(%ld)记录!",(xfmr+i)->id);

            global->dberrnum++;
            continue;
        }
        else
        {
            //check st of this xfmr
            ist = (xfmr+i)->r_st;
            ist = r_st_xfmr_tb->GetDataByPh(ist)->st_id_ptr;
            st_tb->CopyDataByPh(st, ist);
            /*			if(strcmp(st->id, (xfmr+i)->st) != 0)
            			{
            				TRACE("the st of xfmr (%d) is not correct\n", i);

            				TRACE("变压器(%ld)所属电站错误!",(xfmr + i)->id);

            				global->dberrnum++;
            				continue;
            			}
            */
            //check two-winding transformer
            if ((xfmr+i)->type == 2)
            {
                //check nds of xfmr
                hnd = nd_tb->FindPh(CLS_ND((xfmr+i)->hnd));
                if (hnd == -1)
                {

                    TRACE("变压器(%ld)高压侧节点(%s)无连接元件!",(xfmr + i)->id,(xfmr+i)->hnd);

                    return NMD_FAIL;
                }
                lnd = nd_tb->FindPh(CLS_ND((xfmr+i)->lnd));
                if (lnd == -1)
                {

                    TRACE("变压器(%s)低压侧节点(%s)无连接元件!",(xfmr + i)->id,(xfmr+i)->lnd);

                    return NMD_FAIL;
                }

                //check tap type
                if ((xfmr+i)->htapty[0] != '\0')
                {
                    (xfmr+i)->htap = tapty_tb->FindPh(CLS_TAPTY((xfmr+i)->htapty));
                    if ((xfmr+i)->htap == -1)
                    {

                        TRACE("变压器(%s)高压侧抽头类型(%s)有误!",(xfmr + i)->id,(xfmr+i)->htapty);

                        return NMD_FAIL;
                    }
                }
                if ((xfmr+i)->ltapty[0] != '\0')
                {
                    (xfmr+i)->ltap = tapty_tb->FindPh(CLS_TAPTY((xfmr+i)->ltapty));
                    if ((xfmr+i)->ltap == -1)
                    {

                        TRACE("变压器(%s)低压侧抽头类型(%s)有误!",(xfmr + i)->id,(xfmr+i)->ltapty);

                        return NMD_FAIL;
                    }
                }

                //insert xf and r_xfmr_xf for two-winding transformer
                (xfmr+i)->r_xf = r_xfmr_xf_tb->GetNext();
                strcpy(xf_id, (xfmr+i)->id);
                strcat(xf_id, "t");
                *xf = CLS_XF(xf_id);
                strcpy(xf->descr, (xfmr+i)->descr);
                strcpy(xf->nd, (xfmr+i)->hnd);
                strcpy(xf->znd, (xfmr+i)->lnd);
                xf->ind = hnd;
                xf->iznd = lnd;
                xf->r = (xfmr+i)->rh;
                xf->x = (xfmr+i)->xh;
                xf->kvnom = (xfmr+i)->kvnomh;
                xf->zkvnom = (xfmr+i)->kvnoml;
                xf->itapty = (xfmr+i)->htap;
                xf->iztapty = (xfmr+i)->ltap;
//张瑞雪添加于20120217
                xf->r_b = (xfmr+i)->rh_b;
                xf->x_b = (xfmr+i)->xh_b;
                xf->r_c = (xfmr+i)->rh_c;
                xf->x_c = (xfmr+i)->xh_c;
                xf->itapty_b = (xfmr+i)->htap_b;
                xf->iztapty_b = (xfmr+i)->ltap_b;
                xf->itapty_c = (xfmr+i)->htap_c;
                xf->iztapty_c = (xfmr+i)->ltap_c;
                xf->xftype = (xfmr+i)->xftype;
                xf->r_xfmr = r_xfmr_xf_tb->GetNext();

                *r_xfmr_xf = CLS_R_XFMR_XF((xfmr+i)->id, xf_id);
                r_xfmr_xf->xfmr_id_ptr = xfmr_tb->GetPhByLog(i);
                r_xfmr_xf->xf_id_ptr = xf_tb->GetNext();

                if (xf_tb->Insert(*xf) == -1)
                {
                    TRACE("fail to insert xf table\n");

                    TRACE("绕组(%s)中插入失败!",(xf + i)->id);

                    return NMD_FAIL;
                }
                if (r_xfmr_xf_tb->Insert(*r_xfmr_xf) == -1)
                {
                    TRACE("fail to insert r_xfmr_xf table\n");

                    cout << "变压器和绕组关系插入失败!" <<endl;

                    return NMD_FAIL;
                }
            }
            else if ((xfmr+i)->type == 3) //check three-winding transformer
            {
                //check nds of xfmr
                hnd = nd_tb->FindPh(CLS_ND((xfmr+i)->hnd));
                if (hnd == -1)
                {

                    TRACE("变压器(%s)高压侧节点(%s)无连接元件!",(xfmr + i)->id,(xfmr+i)->hnd);

                    return NMD_FAIL;
                }
                mnd = nd_tb->FindPh(CLS_ND((xfmr+i)->mnd));
                if (mnd == -1)
                {

                    TRACE("变压器(%s)中压侧节点(%s)无连接元件!",(xfmr + i)->id,(xfmr+i)->mnd);

                    return NMD_FAIL;
                }
                lnd = nd_tb->FindPh(CLS_ND((xfmr+i)->lnd));
                if (lnd == -1)
                {

                    TRACE("变压器(%s)低压侧节点(%s)无连接元件!",(xfmr + i)->id,(xfmr+i)->lnd);

                    return NMD_FAIL;
                }

                //insert the center nd of this three-winding xfmr
                strcpy(nd_id, (xfmr+i)->id);
                strcat(nd_id, "c");
                nd = new CLS_ND(nd_id);
                cnd = nd_tb->Insert(*nd);
                if (cnd == -1)
                {
                    return NMD_FAIL;
                }
                delete nd;
                nd = 0;

                //check tap type
                if ((xfmr+i)->htapty[0] != '\0')
                {
                    (xfmr+i)->htap = tapty_tb->FindPh(CLS_TAPTY((xfmr+i)->htapty));
                    if ((xfmr+i)->htap == -1)
                    {

                        TRACE("变压器(%s)高压侧抽头类型(%s)有误!",(xfmr + i)->id,(xfmr+i)->htapty);

                        return NMD_FAIL;
                    }
                }
                if ((xfmr+i)->mtapty[0] != '\0')
                {
                    (xfmr+i)->mtap = tapty_tb->FindPh(CLS_TAPTY((xfmr+i)->mtapty));
                    if ((xfmr+i)->mtap == -1)
                    {

                        TRACE("变压器(%s)中压侧抽头类型(%s)有误!",(xfmr + i)->id,(xfmr+i)->mtapty);

                        return NMD_FAIL;
                    }
                }
                if ((xfmr+i)->ltapty[0] != '\0')
                {
                    (xfmr+i)->ltap = tapty_tb->FindPh(CLS_TAPTY((xfmr+i)->ltapty));
                    if ((xfmr+i)->ltap == -1)
                    {

                        TRACE("变压器(%s)低压侧抽头类型(%s)有误!",(xfmr + i)->id,(xfmr+i)->ltapty);

                        return NMD_FAIL;
                    }
                }

                //insert primary side xf for three-winding xfmr
                (xfmr+i)->r_xf = r_xfmr_xf_tb->GetNext();
                strcpy(xf_id, (xfmr+i)->id);
                strcat(xf_id, "h");
                *xf = CLS_XF(xf_id);
                strcpy(xf->descr, (xfmr+i)->descr);
                strcat(xf->descr, "高压侧");
                strcpy(xf->nd, (xfmr+i)->hnd);
                strcpy(xf->znd, nd_id);
                xf->ind = hnd;
                xf->iznd = cnd;
                xf->r = (xfmr+i)->rh;
                xf->x = (xfmr+i)->xh;
                xf->kvnom = (xfmr+i)->kvnomh;
                xf->zkvnom = 1.0;
                xf->itapty = (xfmr+i)->htap;
                xf->r_xfmr = r_xfmr_xf_tb->GetNext();

                *r_xfmr_xf = CLS_R_XFMR_XF((xfmr+i)->id, xf_id);
                r_xfmr_xf->xfmr_id_ptr = xfmr_tb->GetPhByLog(i);
                r_xfmr_xf->xf_id_ptr = xf_tb->GetNext();

                if (xf_tb->Insert(*xf) == -1)
                {
                    TRACE("fail to insert xf table\n");

                    TRACE("绕组(%s)插入失败!",(xf + i)->id);

                    return NMD_FAIL;
                }
                prevph = r_xfmr_xf_tb->Insert(*r_xfmr_xf);
                if (prevph == -1)
                {
                    TRACE("fail to insert r_xfmr_xf table\n");

                    cout << "变压器和绕组关系插入失败!" <<endl;

                    return NMD_FAIL;
                }

                //insert secondary side xf for three-winding xfmr
                strcpy(xf_id, (xfmr+i)->id);
                strcat(xf_id, "m");
                *xf = CLS_XF(xf_id);
                strcpy(xf->descr, (xfmr+i)->descr);
                strcat(xf->descr, "中压侧");
                strcpy(xf->nd, (xfmr+i)->mnd);
                strcpy(xf->znd, nd_id);
                xf->ind = mnd;
                xf->iznd = cnd;
                xf->r = (xfmr+i)->rm;
                xf->x = (xfmr+i)->xm;
                xf->kvnom = (xfmr+i)->kvnomm;
                xf->zkvnom = 1.0;
                xf->itapty = (xfmr+i)->mtap;
                xf->r_xfmr = r_xfmr_xf_tb->GetNext();

                *r_xfmr_xf = CLS_R_XFMR_XF((xfmr+i)->id, xf_id);
                r_xfmr_xf->xfmr_id_ptr = xfmr_tb->GetPhByLog(i);
                r_xfmr_xf->xf_id_ptr = xf_tb->GetNext();
                r_xfmr_xf_tb->GetDataByPh(prevph)->fptr = r_xfmr_xf_tb->GetNext();
                r_xfmr_xf->bptr = prevph;

                if (xf_tb->Insert(*xf) == -1)
                {
                    TRACE("fail to insert xf table\n");

                    TRACE("绕组(%s)插入失败!",(xf + i)->id);

                    return NMD_FAIL;
                }
                prevph = r_xfmr_xf_tb->Insert(*r_xfmr_xf);
                if (prevph == -1)
                {
                    TRACE("fail to insert r_xfmr_xf table\n");

                    cout << "变压器和绕组关系插入失败!" <<endl;

                    return NMD_FAIL;
                }

                //insert third side xf for three-winding xfmr
                strcpy(xf_id, (xfmr+i)->id);
                strcat(xf_id, "l");
                *xf = CLS_XF(xf_id);
                strcpy(xf->descr, (xfmr+i)->descr);
                strcat(xf->descr, "低压侧");
                strcpy(xf->nd, (xfmr+i)->lnd);
                strcpy(xf->znd, nd_id);
                xf->ind = lnd;
                xf->iznd = cnd;
                xf->r = (xfmr+i)->rl;
                xf->x = (xfmr+i)->xl;
                xf->kvnom = (xfmr+i)->kvnoml;
                xf->zkvnom = 1.0;
                xf->itapty = (xfmr+i)->ltap;
                xf->r_xfmr = r_xfmr_xf_tb->GetNext();

                *r_xfmr_xf = CLS_R_XFMR_XF((xfmr+i)->id, xf_id);
                r_xfmr_xf->xfmr_id_ptr = xfmr_tb->GetPhByLog(i);
                r_xfmr_xf->xf_id_ptr = xf_tb->GetNext();
                r_xfmr_xf_tb->GetDataByPh(prevph)->fptr = r_xfmr_xf_tb->GetNext();
                r_xfmr_xf->bptr = prevph;

                if (xf_tb->Insert(*xf) == -1)
                {
                    TRACE("fail to insert xf table\n");

                    TRACE("绕组(%s)插入记录失败!",(xf + i)->id);

                    return NMD_FAIL;
                }
                prevph = r_xfmr_xf_tb->Insert(*r_xfmr_xf);
                if (prevph == -1)
                {
                    TRACE("fail to insert r_xfmr_xf table\n");

                    cout << "变压器和绕组关系插入记录失败!" <<endl;

                    return NMD_FAIL;
                }
            }
            else
            {
                TRACE("the type of xfmr (%d) is illegal\n", i);

                TRACE("变压器(%s)类型非法!",(xfmr + i)->id);

                global->dberrnum++;
                continue;
            }

            //mark the station having a xfmr
            st->qcomp = st->qcomp | ST_XFMR;
            st_tb->UpdateDataByPh(st, ist);
        }
    }

    //update xfmr table
    xfmr_tb->UpdateTable(xfmr);

    delete[] xfmr;
    delete st;
    delete xf;
    delete r_xfmr_xf;
    xfmr = 0;
    st = 0;
    xf = 0;
    r_xfmr_xf = 0;
    lv_xfmr = 0;

    TRACE("xfmr validation passed\n");

    cout << "变压器表校验通过!" <<endl;
#endif
    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  XF_Validate()
int NMD::XF_Validate()
{
    int		i;

    //read xf table into buffer
    lv_xf = xf_tb->GetCount();
    if (lv_xf == 0)
    {
        cout << "绕组表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    xf = new CLS_XF[lv_xf];
    xf_tb->CopyTable(xf);

    //check xf one by one
    for (i = 0; i < lv_xf; i++)
    {
        if (r_xfmr_xf_tb->FindParentPtr((xf+i)->id) == -1)
        {
        	TRACE("变压器和绕组关系表中无绕组(%ld)记录!\r\n",(xf+i)->id);

            global->dberrnum++;
            continue;
        }
        else
        {
            //insert r_nd_xf
            if ( ND_BR_Insert(_XF,(xf+i)->ind,(xf+i)->nd,i,(xf+i)->id) == NMD_FAIL ||
                    ND_BR_Insert(_XF,(xf+i)->iznd,(xf+i)->znd,i,(xf+i)->id) == NMD_FAIL )
            {
                TRACE("绕组(%ld)的变压器和绕组关系建立失败!\r\n",(xf + i)->id);

                return NMD_FAIL;
            }
        }
    }

    //update xf table
    xf_tb->UpdateTable(xf);

    delete[] xf;
    xf = 0;
    lv_xf = 0;

    cout << "绕组表校验通过!" <<endl;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ND_BR_Insert()
//  This function inserts r_nd_br record.
int NMD::ND_BR_Insert(int type, int nd_p, const long nd_id, int ibr, const long br_id)
{
    ///////////////////////////////////////////////////////////////////////////////
    int		phpos, fptr;

    switch ( type )
    {
    case _CB:  //cb
        r_nd_cb = new CLS_R_ND_CB;
        *r_nd_cb = CLS_R_ND_CB(nd_id, br_id);
        r_nd_cb->nd_id_ptr = nd_p;
        r_nd_cb->cb_id_ptr = cb_tb->GetPhByLog(ibr);
        phpos = nd_tb->GetDataByPh(nd_p)->r_cb;
        if (phpos == -1) //first connected cb of this nd
        {
            nd_tb->GetDataByPh(nd_p)->r_cb = r_nd_cb_tb->GetNext();
        }
        else  //build cb chain for this nd
        {
            fptr = r_nd_cb_tb->GetDataByPh(phpos)->fptr;
            while ( fptr != -1 )
            {
                phpos = fptr;
                fptr = r_nd_cb_tb->GetDataByPh(phpos)->fptr;
            }
            r_nd_cb_tb->GetDataByPh(phpos)->fptr = r_nd_cb_tb->GetNext();
            r_nd_cb->bptr = phpos;
        }
        phpos = r_nd_cb_tb->Insert(*r_nd_cb);
        if (phpos == -1)
        {
            return NMD_FAIL;
        }
        delete r_nd_cb;
        r_nd_cb = 0;
        break;
    case _SWITCH:  //switch
		r_nd_switch = new CLS_R_ND_SWITCH;
		*r_nd_switch = CLS_R_ND_SWITCH(nd_id, br_id);
		r_nd_switch->nd_id_ptr = nd_p;
		r_nd_switch->switch_id_ptr = switch_tb->GetPhByLog(ibr);
		phpos = nd_tb->GetDataByPh(nd_p)->r_switch;
		if (phpos == -1) //first connected switch of this nd
		{
			nd_tb->GetDataByPh(nd_p)->r_switch = r_nd_switch_tb->GetNext();
		}
		else  //build switch chain for this nd
		{
			fptr = r_nd_switch_tb->GetDataByPh(phpos)->fptr;
			while ( fptr != -1 )
			{
				phpos = fptr;
				fptr = r_nd_switch_tb->GetDataByPh(phpos)->fptr;
			}
			r_nd_switch_tb->GetDataByPh(phpos)->fptr = r_nd_switch_tb->GetNext();
			r_nd_switch->bptr = phpos;
		}
		phpos = r_nd_switch_tb->Insert(*r_nd_switch);
		if (phpos == -1)
		{
			return NMD_FAIL;
		}
		delete r_nd_switch;
		r_nd_switch = 0;
		break;
    case _XF:  //xf
        r_nd_xf = new CLS_R_ND_XF;
        *r_nd_xf = CLS_R_ND_XF(nd_id, br_id);
        r_nd_xf->nd_id_ptr = nd_p;
        r_nd_xf->xf_id_ptr = xf_tb->GetPhByLog(ibr);
        phpos = nd_tb->GetDataByPh(nd_p)->r_xf;
        if (phpos == -1) //first connected xf of this nd
        {
            nd_tb->GetDataByPh(nd_p)->r_xf = r_nd_xf_tb->GetNext();
        }
        else  //build xf chain for this nd
        {
            fptr = r_nd_xf_tb->GetDataByPh(phpos)->fptr;
            while ( fptr != -1 )
            {
                phpos = fptr;
                fptr = r_nd_xf_tb->GetDataByPh(phpos)->fptr;
            }
            r_nd_xf_tb->GetDataByPh(phpos)->fptr = r_nd_xf_tb->GetNext();
            r_nd_xf->bptr = phpos;
        }
        phpos = r_nd_xf_tb->Insert(*r_nd_xf);
        if (phpos == -1)
        {
            return NMD_FAIL;
        }
        delete r_nd_xf;
        r_nd_xf = 0;
        break;

    case _SEC:  //sec
        r_nd_sec = new CLS_R_ND_SEC;
        *r_nd_sec = CLS_R_ND_SEC(nd_id, br_id);
        r_nd_sec->nd_id_ptr = nd_p;
        r_nd_sec->sec_id_ptr = sec_tb->GetPhByLog(ibr);
        phpos = nd_tb->GetDataByPh(nd_p)->r_sec;
        if (phpos == -1) //first connected sec of this nd
        {
            nd_tb->GetDataByPh(nd_p)->r_sec = r_nd_sec_tb->GetNext();
        }
        else  //build sec chain for this nd
        {
            fptr = r_nd_sec_tb->GetDataByPh(phpos)->fptr;
            while ( fptr != -1 )
            {
                phpos = fptr;
                fptr = r_nd_sec_tb->GetDataByPh(phpos)->fptr;
            }
            r_nd_sec_tb->GetDataByPh(phpos)->fptr = r_nd_sec_tb->GetNext();
            r_nd_sec->bptr = phpos;
        }
        phpos = r_nd_sec_tb->Insert(*r_nd_sec);
        if (phpos == -1)
        {
            return NMD_FAIL;
        }
        delete r_nd_sec;
        r_nd_sec = 0;
        break;
    //////////////////////////////////////////////////////////////////
    ////////////////////荣义平20120529添加，建立ND与CP、LD关系表
    case _CP: //cp
		r_nd_cp = new CLS_R_ND_CP;
		*r_nd_cp = CLS_R_ND_CP(nd_id, br_id);
		r_nd_cp->nd_id_ptr = nd_p;
		r_nd_cp->cp_id_ptr = cp_tb->GetPhByLog(ibr);
		phpos = nd_tb->GetDataByPh(nd_p)->r_cp;
		if (phpos == -1) //first connected cp of this nd
		{
			nd_tb->GetDataByPh(nd_p)->r_cp = r_nd_cp_tb->GetNext();
		}
		else //build cp chain for this nd
		{
			fptr = r_nd_cp_tb->GetDataByPh(phpos)->fptr;
			while (fptr != -1)
			{
				phpos = fptr;
				fptr = r_nd_cp_tb->GetDataByPh(phpos)->fptr;
			}
			r_nd_cp_tb->GetDataByPh(phpos)->fptr = r_nd_cp_tb->GetNext();
			r_nd_cp->bptr = phpos;
		}
		phpos = r_nd_cp_tb->Insert(*r_nd_cp);
		if (phpos == -1)
		{
			return NMD_FAIL;
		}
		delete r_nd_cp;
		r_nd_cp = 0;
		break;

	case _LD: //ld
		r_nd_ld = new CLS_R_ND_LD;
		*r_nd_ld = CLS_R_ND_LD(nd_id, br_id);
		r_nd_ld->nd_id_ptr = nd_p;
		r_nd_ld->ld_id_ptr = ld_tb->GetPhByLog(ibr);
		phpos = nd_tb->GetDataByPh(nd_p)->r_ld;
		if (phpos == -1) //first connected ld of this nd
		{
			nd_tb->GetDataByPh(nd_p)->r_ld = r_nd_ld_tb->GetNext();
		}
		else //build ld chain for this nd
		{
			fptr = r_nd_ld_tb->GetDataByPh(phpos)->fptr;
			while (fptr != -1)
			{
				phpos = fptr;
				fptr = r_nd_ld_tb->GetDataByPh(phpos)->fptr;
			}
			r_nd_ld_tb->GetDataByPh(phpos)->fptr = r_nd_ld_tb->GetNext();
			r_nd_ld->bptr = phpos;
		}
		phpos = r_nd_ld_tb->Insert(*r_nd_ld);
		if (phpos == -1)
		{
			return NMD_FAIL;
		}
		delete r_nd_ld;
		r_nd_ld = 0;
		break;
    }

    return NMD_SUCCEED;
}


////////////////////////////////////////////////////////////////////////////////
//  ND_Insert()
int NMD::ND_Insert(const long nd_id, int* nd_p, int mask)
{
    int		phpos, len;

    phpos = nd_tb->FindPh(CLS_ND(nd_id));
    if ( phpos != -1 ) //an old nd
    {
        if ( mask != -1 )
        {
            nd = new CLS_ND;
            nd_tb->CopyDataByPh(nd, phpos);
            nd->q = nd->q | mask;
            nd_tb->UpdateDataByPh(nd, phpos);
            delete nd;
            nd = 0;
        }
        *nd_p = phpos;
    }
    else  // a new nd
    {
        //insert the new nd
        nd = new CLS_ND(nd_id);
        if ( mask != -1 )
        {
            nd->q =  nd->q | mask;
        }
        //D5000采用数字ID
//        len = strlen(nd_id);
//        if (nd_id[len-2] == 'G' && nd_id[len-1] == 'n')
//        {
//            nd->q = nd->q | ND_GRND;
//        }
        phpos = nd_tb->Insert(*nd);
        if ( phpos == -1 )
        {
            cout << "节点表中插入记录失败!" <<endl;

            return NMD_FAIL;
        }
        *nd_p = phpos;
        delete nd;
        nd = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ResetPtr()
//  This function resets all pointer variables in physical component tables.
//  As we build the relation tables by Ids of physical component, so before
//  doing validation, we should remove all records in relation tables and
//  set the pointer variables in physical component tables to -1.
int NMD::ResetPtr()
{
    ///////////////////////////////////////////////////////////////////////////////

    CO_ResetPtr();
    ST_ResetPtr();
    XFMR_ResetPtr();
    CB_ResetPtr();
    SWITCH_ResetPtr();
    SO_ResetPtr();
    LD_ResetPtr();
    CP_ResetPtr();
    BUS_ResetPtr();
    ZN_ResetPtr();
    SEC_ResetPtr();
    FI_ResetPtr();

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CO_ResetPtr()
int NMD::CO_ResetPtr()
{
    int		i;

    lv_co = co_tb->GetCount();
    if (lv_co != 0)
    {
        //read table into buffer
        co = new CLS_CO[lv_co];
        co_tb->CopyTable(co);

        //reset pointers
        for (i = 0; i < lv_co; i++)
        {
            (co+i)->r_st = -1;
        }

        //update table
        co_tb->UpdateTable(co);

        delete[] co;
        co = 0;
        lv_co = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ST_ResetPtr()
int NMD::ST_ResetPtr()
{
    int		i;

    lv_st = st_tb->GetCount();
    if (lv_st != 0)
    {
        //read table into buffer
        st = new CLS_ST[lv_st];
        st_tb->CopyTable(st);

        //reset pointers
        for (i = 0; i < lv_st; i++)
        {
            (st+i)->r_co = -1;
            (st+i)->r_xfmr = -1;
            (st+i)->r_so = -1;
            (st+i)->r_cb = -1;
            (st+i)->r_ld = -1;
            (st+i)->r_cp = -1;
            (st+i)->r_bus = -1;
            (st+i)->r_zn = -1;
        }

        //update table
        st_tb->UpdateTable(st);

        delete[] st;
        st = 0;
        lv_st = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  XFMR_ResetPtr()
int NMD::XFMR_ResetPtr()
{
    int		i;

    lv_xfmr = xfmr_tb->GetCount();
    if (lv_xfmr != 0)
    {
        //read table into buffer
        xfmr = new CLS_XFMR[lv_xfmr];
        xfmr_tb->CopyTable(xfmr);

        //reset pointers
        for (i = 0; i < lv_xfmr; i++)
        {
            (xfmr+i)->ialgph = -1;
            (xfmr+i)->ialgqh = -1;
            (xfmr+i)->ialgih = -1;
            (xfmr+i)->ialgth = -1;
            (xfmr+i)->ialgpm = -1;
            (xfmr+i)->ialgqm = -1;
            (xfmr+i)->ialgim = -1;
            (xfmr+i)->ialgtm = -1;
            (xfmr+i)->ialgpl = -1;
            (xfmr+i)->ialgql = -1;
            (xfmr+i)->ialgil = -1;
            (xfmr+i)->ialgtl = -1;
            (xfmr+i)->htap = -1;
            (xfmr+i)->mtap = -1;
            (xfmr+i)->ltap = -1;
            (xfmr+i)->r_st = -1;
            (xfmr+i)->r_xf = -1;
//张瑞雪添加于20120217
            (xfmr+i)->ialgph_b = -1;
            (xfmr+i)->ialgqh_b = -1;
            (xfmr+i)->ialgih_b = -1;
            (xfmr+i)->ialgth_b = -1;
            (xfmr+i)->ialgpl_b = -1;
            (xfmr+i)->ialgql_b = -1;
            (xfmr+i)->ialgil_b = -1;
            (xfmr+i)->ialgtl_b = -1;
            (xfmr+i)->htap_b = -1;
            (xfmr+i)->ltap_b = -1;
            (xfmr+i)->ialgph_c = -1;
            (xfmr+i)->ialgqh_c = -1;
            (xfmr+i)->ialgih_c = -1;
            (xfmr+i)->ialgth_c = -1;
            (xfmr+i)->ialgpl_c = -1;
            (xfmr+i)->ialgql_c = -1;
            (xfmr+i)->ialgil_c = -1;
            (xfmr+i)->ialgtl_c = -1;
            (xfmr+i)->htap_c = -1;
            (xfmr+i)->ltap_c = -1;
        }

        //update table
        xfmr_tb->UpdateTable(xfmr);

        delete[] xfmr;
        xfmr = 0;
        lv_xfmr = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CB_ResetPtr()
int NMD::CB_ResetPtr()
{
    int		i;

    lv_cb = cb_tb->GetCount();
    if (lv_cb != 0)
    {
        //read table into buffer
        cb = new CLS_CB[lv_cb];
        cb_tb->CopyTable(cb);

        //reset pointers
        for (i = 0; i < lv_cb; i++)
        {
            (cb+i)->ind = -1;
            (cb+i)->iznd = -1;
//            (cb+i)->idigital = -1;
            (cb+i)->r_st = -1;
        }

        //update table
        cb_tb->UpdateTable(cb);

        delete[] cb;
        cb = 0;
        lv_cb = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CB_ResetPtr()
int NMD::SWITCH_ResetPtr()
{
    int		i;

    lv_switch = switch_tb->GetCount();
    if (lv_switch != 0)
    {
        //read table into buffer
        swh = new CLS_SWITCH[lv_switch];
        switch_tb->CopyTable(swh);

        //reset pointers
        for (i = 0; i < lv_switch; i++)
        {
            (swh+i)->ind = -1;
            (swh+i)->iznd = -1;
//            (switch+i)->idigital = -1;
            (swh+i)->r_st = -1;
        }

        //update table
        switch_tb->UpdateTable(swh);

        delete[] swh;
        swh = 0;
        lv_switch = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  SO_ResetPtr()
int NMD::SO_ResetPtr()
{
    int		i;

    lv_so = so_tb->GetCount();
    if (lv_so != 0)
    {
        //read table into buffer
        so = new CLS_SO[lv_so];
        so_tb->CopyTable(so);

        //reset pointers
        for (i = 0; i < lv_so; i++)
        {
            (so+i)->ind = -1;
            (so+i)->ialgp = -1;
            (so+i)->ialgq = -1;
            (so+i)->r_st = -1;
//张瑞雪添加于20120217
            (so+i)->ialgp_b = -1;
            (so+i)->ialgq_b = -1;
            (so+i)->ialgp_c = -1;
            (so+i)->ialgq_c = -1;
        }

        //update table
        so_tb->UpdateTable(so);

        delete[] so;
        so = 0;
        lv_so = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  LD_ResetPtr()
int NMD::LD_ResetPtr()
{
    int		i;

    lv_ld = ld_tb->GetCount();
    if (lv_ld != 0)
    {
        //read table into buffer
        ld = new CLS_LD[lv_ld];
        ld_tb->CopyTable(ld);

        //reset pointers
        for (i = 0; i < lv_ld; i++)
        {
            (ld+i)->ind = -1;
            (ld+i)->ialgp = -1;
            (ld+i)->ialgq = -1;
            (ld+i)->r_st = -1;
            (ld+i)->r_zn = -1;
//张瑞雪添加于20120217
            (ld+i)->ialgp_b = -1;
            (ld+i)->ialgq_b = -1;
            (ld+i)->ialgp_c = -1;
            (ld+i)->ialgq_c = -1;
            (ld+i)->ialgi = -1;
            (ld+i)->ialgi_b = -1;
            (ld+i)->ialgi_c = -1;
            (ld+i)->ialgia = -1;
            (ld+i)->ialgia_b = -1;
            (ld+i)->ialgia_c = -1;
        }

        //update table
        ld_tb->UpdateTable(ld);

        delete[] ld;
        ld = 0;
        lv_ld = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CP_ResetPtr()
int NMD::CP_ResetPtr()
{
    int		i;

    lv_cp = cp_tb->GetCount();
    if (lv_cp != 0)
    {
        //read table into buffer
        cp = new CLS_CP[lv_cp];
        cp_tb->CopyTable(cp);

        //reset pointers
        for (i = 0; i < lv_cp; i++)
        {
            (cp+i)->ind = -1;
            (cp+i)->ialgq = -1;
            (cp+i)->r_st = -1;
//张瑞雪添加于20120217
            (cp+i)->ialgq_b = -1;
            (cp+i)->ialgq_c = -1;
        }

        //update table
        cp_tb->UpdateTable(cp);

        delete[] cp;
        cp = 0;
        lv_cp = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  BUS_ResetPtr()
int NMD::BUS_ResetPtr()
{
    int		i;

    lv_bus = bus_tb->GetCount();
    if (lv_bus != 0)
    {
        //read table into buffer
        bus = new CLS_BUS[lv_bus];
        bus_tb->CopyTable(bus);

        //reset pointers
        for (i = 0; i < lv_bus; i++)
        {
            (bus+i)->ind = -1;
            (bus+i)->ialgv = -1;
            (bus+i)->r_st = -1;
//张瑞雪添加于20120217
            (bus+i)->ialgv_b = -1;
            (bus+i)->ialgv_c = -1;
        }

        //update table
        bus_tb->UpdateTable(bus);

        delete[] bus;
        bus = 0;
        lv_bus = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ZN_ResetPtr()
int NMD::ZN_ResetPtr()
{
    int		i;

    lv_zn = zn_tb->GetCount();
    if (lv_zn != 0)
    {
        //read table into buffer
        zn = new CLS_ZN[lv_zn];
        zn_tb->CopyTable(zn);

        //reset pointers
        for (i = 0; i < lv_zn; i++)
        {
#if ZN_ND_USE_ARRAY
        	for(int cnt = 0; cnt < (zn+i)->type; cnt++)
        		(zn+i)->nds[cnt].ind = -1;
#else
            (zn+i)->ind = -1;
            (zn+i)->iznd = -1;
#endif
//            (zn+i)->ialgip = -1;
//            (zn+i)->ialgiq = -1;
//            (zn+i)->ialgii = -1;
//            (zn+i)->ialgzp = -1;
//            (zn+i)->ialgzq = -1;
//            (zn+i)->ialgzi = -1;
            (zn+i)->r_st = -1;
            (zn+i)->r_ld = -1;
            (zn+i)->r_sec = -1;
        }

        //update table
        zn_tb->UpdateTable(zn);

        delete[] zn;
        zn = 0;
        lv_zn = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  SEC_ResetPtr()
int NMD::SEC_ResetPtr()
{
    int		i;

    lv_sec = sec_tb->GetCount();
    if (lv_sec != 0)
    {
        //read table into buffer
        sec = new CLS_SEC[lv_sec];
        sec_tb->CopyTable(sec);

        //reset pointers
        for (i = 0; i < lv_sec; i++)
        {
            (sec+i)->ind = -1;
            (sec+i)->iznd = -1;
            (sec+i)->ialgip = -1;
            (sec+i)->ialgiq = -1;
            (sec+i)->ialgii = -1;
            (sec+i)->ialgzp = -1;
            (sec+i)->ialgzq = -1;
            (sec+i)->ialgzi = -1;
            (sec+i)->r_zn = -1;
//张瑞雪添加于20120217
            (sec+i)->ialgip_b = -1;
            (sec+i)->ialgiq_b = -1;
            (sec+i)->ialgii_b = -1;
            (sec+i)->ialgzp_b = -1;
            (sec+i)->ialgzq_b = -1;
            (sec+i)->ialgzi_b = -1;
            (sec+i)->ialgip_c = -1;
            (sec+i)->ialgiq_c = -1;
            (sec+i)->ialgii_c = -1;
            (sec+i)->ialgzp_c = -1;
            (sec+i)->ialgzq_c = -1;
            (sec+i)->ialgzi_c = -1;
             (sec+i)->r_fi = -1;
        }

        //update table
        sec_tb->UpdateTable(sec);

        delete[] sec;
        sec = 0;
        lv_sec = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  SEC_ResetPtr()
int NMD::FI_ResetPtr()
{
    int		i;

    lv_fi = fi_tb->GetCount();
    if (lv_fi != 0)
    {
        //read table into buffer
        fi = new CLS_FI[lv_fi];
        fi_tb->CopyTable(fi);

        //reset pointers
        for (i = 0; i < lv_fi; i++)
        {
            (fi+i)->idigital = -1;
            (fi+i)->r_sec = -1;
        }

        //update table
        fi_tb->UpdateTable(fi);

        delete[] fi;
        fi = 0;
        lv_fi = 0;
    }

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  RebuildPtr()
//  This function rebuilds all position pointers between tables. It first
//  converts the physical position pointers into logic ones, and then
//  stores the data in each table in sequence order.
int NMD::RebuildPtr()
{
	//////////////////////////////////////////////////////////////////////////////
	co_tb->Sequence(false);
	st_tb->Sequence(false);
	xfmr_tb->Sequence(false);
	xf_tb->Sequence(false);
	zn_tb->Sequence(false);
	cb_tb->Sequence(false);
	switch_tb->Sequence(false);
	so_tb->Sequence(false);
	ld_tb->Sequence(false);
	cp_tb->Sequence(false);
	bus_tb->Sequence(false);
	sec_tb->Sequence(false);
	nd_tb->Sequence(false);
	bs_tb->Sequence(false);
	island_tb->Sequence(false);
	global_tb->Sequence(false);
	tapty_tb->Sequence(false);
	fi_tb->Sequence(false);
	r_co_st_tb->Sequence(false);
	r_st_xfmr_tb->Sequence(false);
	r_xfmr_xf_tb->Sequence(false);
	r_st_cb_tb->Sequence(false);
	r_st_switch_tb->Sequence(false);
	r_st_so_tb->Sequence(false);
	r_st_ld_tb->Sequence(false);
	r_st_cp_tb->Sequence(false);
	r_st_bus_tb->Sequence(false);
	r_st_zn_tb->Sequence(false);
	r_zn_ld_tb->Sequence(false);
	r_zn_sec_tb->Sequence(false);
	r_nd_cb_tb->Sequence(false);
	r_nd_switch_tb->Sequence(false);
	r_nd_xf_tb->Sequence(false);
	r_zn_nd_tb->Sequence(false);
	r_nd_sec_tb->Sequence(false);
	r_nd_cp_tb->Sequence(false);
	r_nd_ld_tb->Sequence(false);
	r_sec_fi_tb->Sequence(false);

    //rebuild position pointers in each table
    CO_RebuildPtr();
    ST_RebuildPtr();
    XFMR_RebuildPtr();
    XF_RebuildPtr();
    ZN_RebuildPtr();
    CB_RebuildPtr();
    SWITCH_RebuildPtr();
    SO_RebuildPtr();
    LD_RebuildPtr();
    CP_RebuildPtr();
    BUS_RebuildPtr();
    SEC_RebuildPtr();
    ND_RebuildPtr();
    FI_RebuildPtr();
    R_SEC_FI_RebuildPtr();
    R_CO_ST_RebuildPtr();
    R_ST_XFMR_RebuildPtr();
    R_XFMR_XF_RebuildPtr();
    R_ST_CB_RebuildPtr();
    R_ST_SWITCH_RebuildPtr();
    R_ST_SO_RebuildPtr();
    R_ST_LD_RebuildPtr();
    R_ST_CP_RebuildPtr();
    R_ST_BUS_RebuildPtr();
    R_ST_ZN_RebuildPtr();
    R_ZN_LD_RebuildPtr();
    R_ZN_ND_RebuildPtr();
    R_ZN_SEC_RebuildPtr();
    R_ND_CB_RebuildPtr();
    R_ND_SWITCH_RebuildPtr();
    R_ND_XF_RebuildPtr();
    R_ZN_ND_RebuildPtr();
    R_ND_SEC_RebuildPtr();
    R_ND_CP_RebuildPtr();
    R_ND_LD_RebuildPtr();

    //seqence data in each table
//    digital_tb->Sequence();	//为了保证读入的数据和PSBOB中的逻辑号一致，不对数据进行排序,荣义平修改于20111012
//    analog_tb->Sequence();
//    ctl_tb->Sequence();
    co_tb->Sequence();
    st_tb->Sequence();
    xfmr_tb->Sequence();
    xf_tb->Sequence();
    zn_tb->Sequence();
    cb_tb->Sequence();
    switch_tb->Sequence();
    so_tb->Sequence();
    ld_tb->Sequence();
    cp_tb->Sequence();
    bus_tb->Sequence();
    sec_tb->Sequence();
    nd_tb->Sequence();
    bs_tb->Sequence();
    fi_tb->Sequence();
    island_tb->Sequence();
    global_tb->Sequence();
    tapty_tb->Sequence();
    r_co_st_tb->Sequence();
    r_st_xfmr_tb->Sequence();
    r_xfmr_xf_tb->Sequence();
    r_st_cb_tb->Sequence();
    r_st_switch_tb->Sequence();
    r_st_so_tb->Sequence();
    r_st_ld_tb->Sequence();
    r_st_cp_tb->Sequence();
    r_st_bus_tb->Sequence();
    r_st_zn_tb->Sequence();
    r_zn_ld_tb->Sequence();
    r_zn_sec_tb->Sequence();
    r_nd_cb_tb->Sequence();
    r_nd_switch_tb->Sequence();
    r_nd_xf_tb->Sequence();
    r_zn_nd_tb->Sequence();
    r_nd_sec_tb->Sequence();
    r_nd_cp_tb->Sequence();
    r_nd_ld_tb->Sequence();
    r_sec_fi_tb->Sequence();

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CO_RebuildPtr()
int NMD::CO_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_co = co_tb->GetCount();
    if (lv_co == 0)
    {
        return NMD_SUCCEED;
    }

    co = new CLS_CO[lv_co];
    co_tb->CopyTable(co);

    //rebuild pointer
    for (i = 0; i < lv_co; i++)
    {
        if ((co+i)->r_st != -1)
        {
            (co+i)->r_st = r_co_st_tb->GetLogByPh((co+i)->r_st);
        }
    }

    //update table
    co_tb->UpdateTable(co);

    delete[] co;
    co = 0;
    lv_co = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ST_RebuildPtr()
int NMD::ST_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_st = st_tb->GetCount();
    if (lv_st == 0)
    {
        return NMD_SUCCEED;
    }

    st = new CLS_ST[lv_st];
    st_tb->CopyTable(st);

    //rebuild pointer
    for (i = 0; i < lv_st; i++)
    {
        if ((st+i)->r_co != -1)
        {
            (st+i)->r_co = r_co_st_tb->GetLogByPh((st+i)->r_co);
        }
        if ((st+i)->r_xfmr != -1)
        {
            (st+i)->r_xfmr = r_st_xfmr_tb->GetLogByPh((st+i)->r_xfmr);
        }
        if ((st+i)->r_so != -1)
        {
            (st+i)->r_so = r_st_so_tb->GetLogByPh((st+i)->r_so);
        }
        if ((st+i)->r_cb != -1)
        {
            (st+i)->r_cb = r_st_cb_tb->GetLogByPh((st+i)->r_cb);
        }
        if ((st+i)->r_switch != -1)
		{
			(st+i)->r_switch = r_st_switch_tb->GetLogByPh((st+i)->r_switch);
		}
        if ((st+i)->r_ld != -1)
        {
            (st+i)->r_ld = r_st_ld_tb->GetLogByPh((st+i)->r_ld);
        }
        if ((st+i)->r_cp != -1)
        {
            (st+i)->r_cp = r_st_cp_tb->GetLogByPh((st+i)->r_cp);
        }
        if ((st+i)->r_bus != -1)
        {
            (st+i)->r_bus = r_st_bus_tb->GetLogByPh((st+i)->r_bus);
        }
        if ((st+i)->r_zn != -1)
        {
            (st+i)->r_zn = r_st_zn_tb->GetLogByPh((st+i)->r_zn);
        }
    }

    //update table
    st_tb->UpdateTable(st);

    delete[] st;
    st = 0;
    lv_st = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  XFMR_RebuildPtr()
int NMD::XFMR_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_xfmr = xfmr_tb->GetCount();
    if (lv_xfmr == 0)
    {
        return NMD_SUCCEED;
    }

    xfmr = new CLS_XFMR[lv_xfmr];
    xfmr_tb->CopyTable(xfmr);

    //rebuild pointer
    for (i = 0; i < lv_xfmr; i++)
    {
        if ((xfmr+i)->r_st != -1)
        {
            (xfmr+i)->r_st = r_st_xfmr_tb->GetLogByPh((xfmr+i)->r_st);
        }
        if ((xfmr+i)->r_xf != -1)
        {
            (xfmr+i)->r_xf = r_xfmr_xf_tb->GetLogByPh((xfmr+i)->r_xf);
        }
    }

    //update table
    xfmr_tb->UpdateTable(xfmr);

    delete[] xfmr;
    xfmr = 0;
    lv_xfmr = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  XF_RebuildPtr()
int NMD::XF_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_xf = xf_tb->GetCount();
    if (lv_xf == 0)
    {
        return NMD_SUCCEED;
    }

    xf = new CLS_XF[lv_xf];
    xf_tb->CopyTable(xf);

    //rebuild pointer
    for (i = 0; i < lv_xf; i++)
    {
        if ((xf+i)->r_xfmr != -1)
        {
            (xf+i)->r_xfmr = r_xfmr_xf_tb->GetLogByPh((xf+i)->r_xfmr);
        }
        if ((xf+i)->ind != -1)
        {
            (xf+i)->ind = nd_tb->GetLogByPh((xf+i)->ind);
        }
        if ((xf+i)->iznd != -1)
        {
            (xf+i)->iznd = nd_tb->GetLogByPh((xf+i)->iznd);
        }
        if ((xf+i)->itapty != -1)
        {
            (xf+i)->itapty = tapty_tb->GetLogByPh((xf+i)->itapty);
        }
        if ((xf+i)->iztapty != -1)
        {
            (xf+i)->iztapty = tapty_tb->GetLogByPh((xf+i)->iztapty);
        }
    }

    //update table
    xf_tb->UpdateTable(xf);

    delete[] xf;
    xf = 0;
    lv_xf = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CB_RebuildPtr()
int NMD::CB_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_cb = cb_tb->GetCount();
    if (lv_cb == 0)
    {
        return NMD_SUCCEED;
    }

    cb = new CLS_CB[lv_cb];
    cb_tb->CopyTable(cb);

    //rebuild pointer
    for (i = 0; i < lv_cb; i++)
    {
        if ((cb+i)->r_st != -1)
        {
            (cb+i)->r_st = r_st_cb_tb->GetLogByPh((cb+i)->r_st);
        }
        if ((cb+i)->ind != -1)
        {
            (cb+i)->ind = nd_tb->GetLogByPh((cb+i)->ind);
        }
        if ((cb+i)->iznd != -1)
        {
            (cb+i)->iznd = nd_tb->GetLogByPh((cb+i)->iznd);
        }
    }

    //update table
    cb_tb->UpdateTable(cb);

    delete[] cb;
    cb = 0;
    lv_cb = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CB_RebuildPtr()
int NMD::SWITCH_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_switch = switch_tb->GetCount();
    if (lv_switch == 0)
    {
        return NMD_SUCCEED;
    }

    swh = new CLS_SWITCH[lv_switch];
    switch_tb->CopyTable(swh);

    //rebuild pointer
    for (i = 0; i < lv_switch; i++)
    {
        if ((swh+i)->r_st != -1)
        {
            (swh+i)->r_st = r_st_switch_tb->GetLogByPh((swh+i)->r_st);
        }
        if ((swh+i)->ind != -1)
        {
            (swh+i)->ind = nd_tb->GetLogByPh((swh+i)->ind);
        }
        if ((swh+i)->iznd != -1)
        {
            (swh+i)->iznd = nd_tb->GetLogByPh((swh+i)->iznd);
        }
    }

    //update table
    switch_tb->UpdateTable(swh);

    delete[] swh;
    swh = 0;
    lv_switch = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  SO_RebuildPtr()
int NMD::SO_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_so = so_tb->GetCount();
    if (lv_so == 0)
    {
        return NMD_SUCCEED;
    }

    so = new CLS_SO[lv_so];
    so_tb->CopyTable(so);

    //rebuild pointer
    for (i = 0; i < lv_so; i++)
    {
        if ((so+i)->r_st != -1)
        {
            (so+i)->r_st = r_st_so_tb->GetLogByPh((so+i)->r_st);
        }
        if ((so+i)->ind != -1)
        {
            (so+i)->ind = nd_tb->GetLogByPh((so+i)->ind);
        }
    }

    //update table
    so_tb->UpdateTable(so);

    delete[] so;
    so = 0;
    lv_so = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  FI_RebuildPtr()
int NMD::FI_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_fi = fi_tb->GetCount();
    if (lv_fi == 0)
    {
        return NMD_SUCCEED;
    }

    fi = new CLS_FI[lv_fi];
    fi_tb->CopyTable(fi);

    //rebuild pointer
    for (i = 0; i < lv_fi; i++)
    {
        if ((fi+i)->r_sec != -1)
        {
            (fi+i)->r_sec = r_sec_fi_tb->GetLogByPh((fi+i)->r_sec);
        }
        if ((fi+i)->idigital != -1)
        {
        	//D5000 没有遥信表
//            (fi+i)->idigital = digital_tb->GetLogByPh((fi+i)->idigital);
        }
    }

    //update table
    fi_tb->UpdateTable(fi);

    delete[] fi;
    fi = 0;
    lv_fi = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  LD_RebuildPtr()
int NMD::LD_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_ld = ld_tb->GetCount();
    if (lv_ld == 0)
    {
        return NMD_SUCCEED;
    }

    ld = new CLS_LD[lv_ld];
    ld_tb->CopyTable(ld);

    //rebuild pointer
    for (i = 0; i < lv_ld; i++)
    {
        if ((ld+i)->r_st != -1)
        {
            (ld+i)->r_st = r_st_ld_tb->GetLogByPh((ld+i)->r_st);
        }
        if ((ld+i)->r_zn != -1)
        {
            (ld+i)->r_zn = r_zn_ld_tb->GetLogByPh((ld+i)->r_zn);
        }
        if ((ld+i)->ind != -1)
        {
            (ld+i)->ind = nd_tb->GetLogByPh((ld+i)->ind);
        }
    }

    //update table
    ld_tb->UpdateTable(ld);

    delete[] ld;
    ld = 0;
    lv_ld = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  CP_RebuildPtr()
int NMD::CP_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_cp = cp_tb->GetCount();
    if (lv_cp == 0)
    {
        return NMD_SUCCEED;
    }

    cp = new CLS_CP[lv_cp];
    cp_tb->CopyTable(cp);

    //rebuild pointer
    for (i = 0; i < lv_cp; i++)
    {
        if ((cp+i)->r_st != -1)
        {
            (cp+i)->r_st = r_st_cp_tb->GetLogByPh((cp+i)->r_st);
        }
        if ((cp+i)->ind != -1)
        {
            (cp+i)->ind = nd_tb->GetLogByPh((cp+i)->ind);
        }
    }

    //update table
    cp_tb->UpdateTable(cp);

    delete[] cp;
    cp = 0;
    lv_cp = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  BUS_RebuildPtr()
int NMD::BUS_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_bus = bus_tb->GetCount();
    if (lv_bus == 0)
    {
        return NMD_SUCCEED;
    }

    bus = new CLS_BUS[lv_bus];
    bus_tb->CopyTable(bus);

    //rebuild pointer
    for (i = 0; i < lv_bus; i++)
    {
        if ((bus+i)->r_st != -1)
        {
            (bus+i)->r_st = r_st_bus_tb->GetLogByPh((bus+i)->r_st);
        }
        if ((bus+i)->ind != -1)
        {
            (bus+i)->ind = nd_tb->GetLogByPh((bus+i)->ind);
        }
    }

    //update table
    bus_tb->UpdateTable(bus);

    delete[] bus;
    bus = 0;
    lv_bus = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ZN_RebuildPtr()
int NMD::ZN_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_zn = zn_tb->GetCount();
    if (lv_zn == 0)
    {
        return NMD_SUCCEED;
    }

    zn = new CLS_ZN[lv_zn];
    zn_tb->CopyTable(zn);

    //rebuild pointer
    for (i = 0; i < lv_zn; i++)
    {
        if ((zn+i)->r_st != -1)
        {
            (zn+i)->r_st = r_st_zn_tb->GetLogByPh((zn+i)->r_st);
        }
        if ((zn+i)->r_ld != -1)
        {
            (zn+i)->r_ld = r_zn_ld_tb->GetLogByPh((zn+i)->r_ld);
        }
        if ((zn+i)->r_sec != -1)
        {
            (zn+i)->r_sec = r_zn_sec_tb->GetLogByPh((zn+i)->r_sec);
        }
#if ZN_ND_USE_ARRAY
        for (int cnt = 0; cnt < (zn + i)->type; cnt++)
		{
			if ((zn + i)->nds[cnt].ind != -1)
				(zn + i)->nds[cnt].ind = nd_tb->GetLogByPh((zn + i)->nds[cnt].ind);
		}
#else
        if ((zn+i)->ind != -1)
        {
            (zn+i)->ind = nd_tb->GetLogByPh((zn+i)->ind);
        }
        if ((zn+i)->iznd != -1)
        {
            (zn+i)->iznd = nd_tb->GetLogByPh((zn+i)->iznd);
        }
        if ((zn+i)->ind3 != -1)
        {
            (zn+i)->ind3 = nd_tb->GetLogByPh((zn+i)->ind3);
        }
        if ((zn+i)->ind4 != -1)
        {
            (zn+i)->ind4 = nd_tb->GetLogByPh((zn+i)->ind4);
        }
        if ((zn+i)->ind5 != -1)
        {
            (zn+i)->ind5 = nd_tb->GetLogByPh((zn+i)->ind5);
        }
#endif
    }

    //update table
    zn_tb->UpdateTable(zn);

    delete[] zn;
    zn = 0;
    lv_zn = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  SEC_RebuildPtr()
int NMD::SEC_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_sec = sec_tb->GetCount();
    if (lv_sec == 0)
    {
        return NMD_SUCCEED;
    }

    sec = new CLS_SEC[lv_sec];
    sec_tb->CopyTable(sec);

    //rebuild pointer
    for (i = 0; i < lv_sec; i++)
    {
        if ((sec+i)->r_zn != -1)
        {
            (sec+i)->r_zn = r_zn_sec_tb->GetLogByPh((sec+i)->r_zn);
        }
        if ((sec+i)->ind != -1)
        {
            (sec+i)->ind = nd_tb->GetLogByPh((sec+i)->ind);
        }
        if ((sec+i)->iznd != -1)
        {
            (sec+i)->iznd = nd_tb->GetLogByPh((sec+i)->iznd);
        }
    }

    //update table
    sec_tb->UpdateTable(sec);

    delete[] sec;
    sec = 0;
    lv_sec = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  ND_RebuildPtr()
int NMD::ND_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_nd = nd_tb->GetCount();
    if (lv_nd == 0)
    {
        return NMD_SUCCEED;
    }

    nd = new CLS_ND[lv_nd];
    nd_tb->CopyTable(nd);

    //rebuild pointer
    for (i = 0; i < lv_nd; i++)
    {
        if ((nd+i)->r_cb != -1)
        {
            (nd+i)->r_cb = r_nd_cb_tb->GetLogByPh((nd+i)->r_cb);
        }
        if ((nd+i)->r_switch != -1)
		{
			(nd+i)->r_switch = r_nd_switch_tb->GetLogByPh((nd+i)->r_switch);
		}
        if ((nd+i)->r_xf != -1)
        {
            (nd+i)->r_xf = r_nd_xf_tb->GetLogByPh((nd+i)->r_xf);
        }
        if ((nd+i)->r_sec != -1)
        {
            (nd+i)->r_sec = r_nd_sec_tb->GetLogByPh((nd+i)->r_sec);
        }
        if ((nd + i)->r_cp != -1)
		{
			(nd + i)->r_cp = r_nd_cp_tb->GetLogByPh((nd + i)->r_cp);
		}
		if ((nd + i)->r_ld != -1)
		{
			(nd + i)->r_ld = r_nd_ld_tb->GetLogByPh((nd + i)->r_ld);
		}
    }

    //update table
    nd_tb->UpdateTable(nd);

    delete[] nd;
    nd = 0;
    lv_nd = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_CO_ST_RebuildPtr()
int NMD::R_CO_ST_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_co_st = r_co_st_tb->GetCount();
    if (lv_r_co_st == 0)
    {
        return NMD_SUCCEED;
    }

    r_co_st = new CLS_R_CO_ST[lv_r_co_st];
    r_co_st_tb->CopyTable(r_co_st);

    //rebuild pointer
    for (i = 0; i < lv_r_co_st; i++)
    {
        if ((r_co_st+i)->co_id_ptr != -1)
        {
            (r_co_st+i)->co_id_ptr = co_tb->GetLogByPh((r_co_st+i)->co_id_ptr);
        }
        if ((r_co_st+i)->st_id_ptr != -1)
        {
            (r_co_st+i)->st_id_ptr = st_tb->GetLogByPh((r_co_st+i)->st_id_ptr);
        }
        if ((r_co_st+i)->fptr != -1)
        {
            (r_co_st+i)->fptr = r_co_st_tb->GetLogByPh((r_co_st+i)->fptr);
        }
        if ((r_co_st+i)->bptr != -1)
        {
            (r_co_st+i)->bptr = r_co_st_tb->GetLogByPh((r_co_st+i)->bptr);
        }
    }

    //update table
    r_co_st_tb->UpdateTable(r_co_st);

    delete[] r_co_st;
    r_co_st = 0;
    lv_r_co_st = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_XFMR_RebuildPtr()
int NMD::R_ST_XFMR_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_xfmr = r_st_xfmr_tb->GetCount();
    if (lv_r_st_xfmr == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_xfmr = new CLS_R_ST_XFMR[lv_r_st_xfmr];
    r_st_xfmr_tb->CopyTable(r_st_xfmr);

    //rebuild pointer
    for (i = 0; i < lv_r_st_xfmr; i++)
    {
        if ((r_st_xfmr+i)->st_id_ptr != -1)
        {
            (r_st_xfmr+i)->st_id_ptr = st_tb->GetLogByPh((r_st_xfmr+i)->st_id_ptr);
        }
        if ((r_st_xfmr+i)->xfmr_id_ptr != -1)
        {
            (r_st_xfmr+i)->xfmr_id_ptr = xfmr_tb->GetLogByPh((r_st_xfmr+i)->xfmr_id_ptr);
        }
        if ((r_st_xfmr+i)->fptr != -1)
        {
            (r_st_xfmr+i)->fptr = r_st_xfmr_tb->GetLogByPh((r_st_xfmr+i)->fptr);
        }
        if ((r_st_xfmr+i)->bptr != -1)
        {
            (r_st_xfmr+i)->bptr = r_st_xfmr_tb->GetLogByPh((r_st_xfmr+i)->bptr);
        }
    }

    //update table
    r_st_xfmr_tb->UpdateTable(r_st_xfmr);

    delete[] r_st_xfmr;
    r_st_xfmr = 0;
    lv_r_st_xfmr = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_XFMR_XF_RebuildPtr()
int NMD::R_XFMR_XF_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_xfmr_xf = r_xfmr_xf_tb->GetCount();
    if (lv_r_xfmr_xf == 0)
    {
        return NMD_SUCCEED;
    }

    r_xfmr_xf = new CLS_R_XFMR_XF[lv_r_xfmr_xf];
    r_xfmr_xf_tb->CopyTable(r_xfmr_xf);

    //rebuild pointer
    for (i = 0; i < lv_r_xfmr_xf; i++)
    {
        if ((r_xfmr_xf+i)->xfmr_id_ptr != -1)
        {
            (r_xfmr_xf+i)->xfmr_id_ptr = xfmr_tb->GetLogByPh((r_xfmr_xf+i)->xfmr_id_ptr);
        }
        if ((r_xfmr_xf+i)->xf_id_ptr != -1)
        {
            (r_xfmr_xf+i)->xf_id_ptr = xf_tb->GetLogByPh((r_xfmr_xf+i)->xf_id_ptr);
        }
        if ((r_xfmr_xf+i)->fptr != -1)
        {
            (r_xfmr_xf+i)->fptr = r_xfmr_xf_tb->GetLogByPh((r_xfmr_xf+i)->fptr);
        }
        if ((r_xfmr_xf+i)->bptr != -1)
        {
            (r_xfmr_xf+i)->bptr = r_xfmr_xf_tb->GetLogByPh((r_xfmr_xf+i)->bptr);
        }
    }

    //update table
    r_xfmr_xf_tb->UpdateTable(r_xfmr_xf);

    delete[] r_xfmr_xf;
    r_xfmr_xf = 0;
    lv_r_xfmr_xf = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_CB_RebuildPtr()
int NMD::R_ST_CB_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_cb = r_st_cb_tb->GetCount();
    if (lv_r_st_cb == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_cb = new CLS_R_ST_CB[lv_r_st_cb];
    r_st_cb_tb->CopyTable(r_st_cb);

    //rebuild pointer
    for (i = 0; i < lv_r_st_cb; i++)
    {
        if ((r_st_cb+i)->st_id_ptr != -1)
        {
            (r_st_cb+i)->st_id_ptr = st_tb->GetLogByPh((r_st_cb+i)->st_id_ptr);
        }
        if ((r_st_cb+i)->cb_id_ptr != -1)
        {
            (r_st_cb+i)->cb_id_ptr = cb_tb->GetLogByPh((r_st_cb+i)->cb_id_ptr);
        }
        if ((r_st_cb+i)->fptr != -1)
        {
            (r_st_cb+i)->fptr = r_st_cb_tb->GetLogByPh((r_st_cb+i)->fptr);
        }
        if ((r_st_cb+i)->bptr != -1)
        {
            (r_st_cb+i)->bptr = r_st_cb_tb->GetLogByPh((r_st_cb+i)->bptr);
        }
    }

    //update table
    r_st_cb_tb->UpdateTable(r_st_cb);

    delete[] r_st_cb;
    r_st_cb = 0;
    lv_r_st_cb = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_CB_RebuildPtr()
int NMD::R_ST_SWITCH_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_switch = r_st_switch_tb->GetCount();
    if (lv_r_st_switch == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_switch = new CLS_R_ST_SWITCH[lv_r_st_switch];
    r_st_switch_tb->CopyTable(r_st_switch);

    //rebuild pointer
    for (i = 0; i < lv_r_st_switch; i++)
    {
        if ((r_st_switch+i)->st_id_ptr != -1)
        {
            (r_st_switch+i)->st_id_ptr = st_tb->GetLogByPh((r_st_switch+i)->st_id_ptr);
        }
        if ((r_st_switch+i)->switch_id_ptr != -1)
        {
            (r_st_switch+i)->switch_id_ptr = switch_tb->GetLogByPh((r_st_switch+i)->switch_id_ptr);
        }
        if ((r_st_switch+i)->fptr != -1)
        {
            (r_st_switch+i)->fptr = r_st_switch_tb->GetLogByPh((r_st_switch+i)->fptr);
        }
        if ((r_st_switch+i)->bptr != -1)
        {
            (r_st_switch+i)->bptr = r_st_switch_tb->GetLogByPh((r_st_switch+i)->bptr);
        }
    }

    //update table
    r_st_switch_tb->UpdateTable(r_st_switch);

    delete[] r_st_switch;
    r_st_switch = 0;
    lv_r_st_switch = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_SO_RebuildPtr()
int NMD::R_ST_SO_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_so = r_st_so_tb->GetCount();
    if (lv_r_st_so == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_so = new CLS_R_ST_SO[lv_r_st_so];
    r_st_so_tb->CopyTable(r_st_so);

    //rebuild pointer
    for (i = 0; i < lv_r_st_so; i++)
    {
        if ((r_st_so+i)->st_id_ptr != -1)
        {
            (r_st_so+i)->st_id_ptr = st_tb->GetLogByPh((r_st_so+i)->st_id_ptr);
        }
        if ((r_st_so+i)->so_id_ptr != -1)
        {
            (r_st_so+i)->so_id_ptr = so_tb->GetLogByPh((r_st_so+i)->so_id_ptr);
        }
        if ((r_st_so+i)->fptr != -1)
        {
            (r_st_so+i)->fptr = r_st_so_tb->GetLogByPh((r_st_so+i)->fptr);
        }
        if ((r_st_so+i)->bptr != -1)
        {
            (r_st_so+i)->bptr = r_st_so_tb->GetLogByPh((r_st_so+i)->bptr);
        }
    }

    //update table
    r_st_so_tb->UpdateTable(r_st_so);

    delete[] r_st_so;
    r_st_so = 0;
    lv_r_st_so = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_LD_RebuildPtr()
int NMD::R_ST_LD_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_ld = r_st_ld_tb->GetCount();
    if (lv_r_st_ld == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_ld = new CLS_R_ST_LD[lv_r_st_ld];
    r_st_ld_tb->CopyTable(r_st_ld);

    //rebuild pointer
    for (i = 0; i < lv_r_st_ld; i++)
    {
        if ((r_st_ld+i)->st_id_ptr != -1)
        {
            (r_st_ld+i)->st_id_ptr = st_tb->GetLogByPh((r_st_ld+i)->st_id_ptr);
        }
        if ((r_st_ld+i)->ld_id_ptr != -1)
        {
            (r_st_ld+i)->ld_id_ptr = ld_tb->GetLogByPh((r_st_ld+i)->ld_id_ptr);
        }
        if ((r_st_ld+i)->fptr != -1)
        {
            (r_st_ld+i)->fptr = r_st_ld_tb->GetLogByPh((r_st_ld+i)->fptr);
        }
        if ((r_st_ld+i)->bptr != -1)
        {
            (r_st_ld+i)->bptr = r_st_ld_tb->GetLogByPh((r_st_ld+i)->bptr);
        }
    }

    //update table
    r_st_ld_tb->UpdateTable(r_st_ld);

    delete[] r_st_ld;
    r_st_ld = 0;
    lv_r_st_ld = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_CP_RebuildPtr()
int NMD::R_ST_CP_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_cp = r_st_cp_tb->GetCount();
    if (lv_r_st_cp == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_cp = new CLS_R_ST_CP[lv_r_st_cp];
    r_st_cp_tb->CopyTable(r_st_cp);

    //rebuild pointer
    for (i = 0; i < lv_r_st_cp; i++)
    {
        if ((r_st_cp+i)->st_id_ptr != -1)
        {
            (r_st_cp+i)->st_id_ptr = st_tb->GetLogByPh((r_st_cp+i)->st_id_ptr);
        }
        if ((r_st_cp+i)->cp_id_ptr != -1)
        {
            (r_st_cp+i)->cp_id_ptr = cp_tb->GetLogByPh((r_st_cp+i)->cp_id_ptr);
        }
        if ((r_st_cp+i)->fptr != -1)
        {
            (r_st_cp+i)->fptr = r_st_cp_tb->GetLogByPh((r_st_cp+i)->fptr);
        }
        if ((r_st_cp+i)->bptr != -1)
        {
            (r_st_cp+i)->bptr = r_st_cp_tb->GetLogByPh((r_st_cp+i)->bptr);
        }
    }

    //update table
    r_st_cp_tb->UpdateTable(r_st_cp);

    delete[] r_st_cp;
    r_st_cp = 0;
    lv_r_st_cp = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_BUS_RebuildPtr()
int NMD::R_ST_BUS_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_bus = r_st_bus_tb->GetCount();
    if (lv_r_st_bus == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_bus = new CLS_R_ST_BUS[lv_r_st_bus];
    r_st_bus_tb->CopyTable(r_st_bus);

    //rebuild pointer
    for (i = 0; i < lv_r_st_bus; i++)
    {
        if ((r_st_bus+i)->st_id_ptr != -1)
        {
            (r_st_bus+i)->st_id_ptr = st_tb->GetLogByPh((r_st_bus+i)->st_id_ptr);
        }
        if ((r_st_bus+i)->bus_id_ptr != -1)
        {
            (r_st_bus+i)->bus_id_ptr = bus_tb->GetLogByPh((r_st_bus+i)->bus_id_ptr);
        }
        if ((r_st_bus+i)->fptr != -1)
        {
            (r_st_bus+i)->fptr = r_st_bus_tb->GetLogByPh((r_st_bus+i)->fptr);
        }
        if ((r_st_bus+i)->bptr != -1)
        {
            (r_st_bus+i)->bptr = r_st_bus_tb->GetLogByPh((r_st_bus+i)->bptr);
        }
    }

    //update table
    r_st_bus_tb->UpdateTable(r_st_bus);

    delete[] r_st_bus;
    r_st_bus = 0;
    lv_r_st_bus = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ST_ZN_RebuildPtr()
int NMD::R_ST_ZN_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_st_zn = r_st_zn_tb->GetCount();
    if (lv_r_st_zn == 0)
    {
        return NMD_SUCCEED;
    }

    r_st_zn = new CLS_R_ST_ZN[lv_r_st_zn];
    r_st_zn_tb->CopyTable(r_st_zn);

    //rebuild pointer
    for (i = 0; i < lv_r_st_zn; i++)
    {
        if ((r_st_zn+i)->st_id_ptr != -1)
        {
            (r_st_zn+i)->st_id_ptr = st_tb->GetLogByPh((r_st_zn+i)->st_id_ptr);
        }
        if ((r_st_zn+i)->zn_id_ptr != -1)
        {
            (r_st_zn+i)->zn_id_ptr = zn_tb->GetLogByPh((r_st_zn+i)->zn_id_ptr);
        }
        if ((r_st_zn+i)->fptr != -1)
        {
            (r_st_zn+i)->fptr = r_st_zn_tb->GetLogByPh((r_st_zn+i)->fptr);
        }
        if ((r_st_zn+i)->bptr != -1)
        {
            (r_st_zn+i)->bptr = r_st_zn_tb->GetLogByPh((r_st_zn+i)->bptr);
        }
    }

    //update table
    r_st_zn_tb->UpdateTable(r_st_zn);

    delete[] r_st_zn;
    r_st_zn = 0;
    lv_r_st_zn = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ZN_LD_RebuildPtr()
int NMD::R_ZN_LD_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_zn_ld = r_zn_ld_tb->GetCount();
    if (lv_r_zn_ld == 0)
    {
        return NMD_SUCCEED;
    }

    r_zn_ld = new CLS_R_ZN_LD[lv_r_zn_ld];
    r_zn_ld_tb->CopyTable(r_zn_ld);

    //rebuild pointer
    for (i = 0; i < lv_r_zn_ld; i++)
    {
        if ((r_zn_ld+i)->zn_id_ptr != -1)
        {
            (r_zn_ld+i)->zn_id_ptr = zn_tb->GetLogByPh((r_zn_ld+i)->zn_id_ptr);
        }
        if ((r_zn_ld+i)->ld_id_ptr != -1)
        {
            (r_zn_ld+i)->ld_id_ptr = ld_tb->GetLogByPh((r_zn_ld+i)->ld_id_ptr);
        }
        if ((r_zn_ld+i)->fptr != -1)
        {
            (r_zn_ld+i)->fptr = r_zn_ld_tb->GetLogByPh((r_zn_ld+i)->fptr);
        }
        if ((r_zn_ld+i)->bptr != -1)
        {
            (r_zn_ld+i)->bptr = r_zn_ld_tb->GetLogByPh((r_zn_ld+i)->bptr);
        }
    }

    //update table
    r_zn_ld_tb->UpdateTable(r_zn_ld);

    delete[] r_zn_ld;
    r_zn_ld = 0;
    lv_r_zn_ld = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ZN_SEC_RebuildPtr()
int NMD::R_ZN_SEC_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_zn_sec = r_zn_sec_tb->GetCount();
    if (lv_r_zn_sec == 0)
    {
        return NMD_SUCCEED;
    }

    r_zn_sec = new CLS_R_ZN_SEC[lv_r_zn_sec];
    r_zn_sec_tb->CopyTable(r_zn_sec);

    //rebuild pointer
    for (i = 0; i < lv_r_zn_sec; i++)
    {
        if ((r_zn_sec+i)->zn_id_ptr != -1)
        {
            (r_zn_sec+i)->zn_id_ptr = zn_tb->GetLogByPh((r_zn_sec+i)->zn_id_ptr);
        }
        if ((r_zn_sec+i)->sec_id_ptr != -1)
        {
            (r_zn_sec+i)->sec_id_ptr = sec_tb->GetLogByPh((r_zn_sec+i)->sec_id_ptr);
        }
        if ((r_zn_sec+i)->fptr != -1)
        {
            (r_zn_sec+i)->fptr = r_zn_sec_tb->GetLogByPh((r_zn_sec+i)->fptr);
        }
        if ((r_zn_sec+i)->bptr != -1)
        {
            (r_zn_sec+i)->bptr = r_zn_sec_tb->GetLogByPh((r_zn_sec+i)->bptr);
        }
    }

    //update table
    r_zn_sec_tb->UpdateTable(r_zn_sec);

    delete[] r_zn_sec;
    r_zn_sec = 0;
    lv_r_zn_sec = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ND_CB_RebuildPtr()
int NMD::R_ND_CB_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_nd_cb = r_nd_cb_tb->GetCount();
    if (lv_r_nd_cb == 0)
    {
        return NMD_SUCCEED;
    }

    r_nd_cb = new CLS_R_ND_CB[lv_r_nd_cb];
    r_nd_cb_tb->CopyTable(r_nd_cb);

    //rebuild pointer
    for (i = 0; i < lv_r_nd_cb; i++)
    {
        if ((r_nd_cb+i)->nd_id_ptr != -1)
        {
            (r_nd_cb+i)->nd_id_ptr = nd_tb->GetLogByPh((r_nd_cb+i)->nd_id_ptr);
        }
        if ((r_nd_cb+i)->cb_id_ptr != -1)
        {
            (r_nd_cb+i)->cb_id_ptr = cb_tb->GetLogByPh((r_nd_cb+i)->cb_id_ptr);
        }
        if ((r_nd_cb+i)->fptr != -1)
        {
            (r_nd_cb+i)->fptr = r_nd_cb_tb->GetLogByPh((r_nd_cb+i)->fptr);
        }
        if ((r_nd_cb+i)->bptr != -1)
        {
            (r_nd_cb+i)->bptr = r_nd_cb_tb->GetLogByPh((r_nd_cb+i)->bptr);
        }
    }

    //update table
    r_nd_cb_tb->UpdateTable(r_nd_cb);

    delete[] r_nd_cb;
    r_nd_cb = 0;
    lv_r_nd_cb = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ND_SWITCH_RebuildPtr()
int NMD::R_ND_SWITCH_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_nd_switch = r_nd_switch_tb->GetCount();
    if (lv_r_nd_switch == 0)
    {
        return NMD_SUCCEED;
    }

    r_nd_switch = new CLS_R_ND_SWITCH[lv_r_nd_switch];
    r_nd_switch_tb->CopyTable(r_nd_switch);

    //rebuild pointer
    for (i = 0; i < lv_r_nd_switch; i++)
    {
        if ((r_nd_switch+i)->nd_id_ptr != -1)
        {
            (r_nd_switch+i)->nd_id_ptr = nd_tb->GetLogByPh((r_nd_switch+i)->nd_id_ptr);
        }
        if ((r_nd_switch+i)->switch_id_ptr != -1)
        {
            (r_nd_switch+i)->switch_id_ptr = switch_tb->GetLogByPh((r_nd_switch+i)->switch_id_ptr);
        }
        if ((r_nd_switch+i)->fptr != -1)
        {
            (r_nd_switch+i)->fptr = r_nd_switch_tb->GetLogByPh((r_nd_switch+i)->fptr);
        }
        if ((r_nd_switch+i)->bptr != -1)
        {
            (r_nd_switch+i)->bptr = r_nd_switch_tb->GetLogByPh((r_nd_switch+i)->bptr);
        }
    }

    //update table
    r_nd_switch_tb->UpdateTable(r_nd_switch);

    delete[] r_nd_switch;
    r_nd_switch = 0;
    lv_r_nd_switch = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ND_XF_RebuildPtr()
int NMD::R_ND_XF_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_nd_xf = r_nd_xf_tb->GetCount();
    if (lv_r_nd_xf == 0)
    {
        return NMD_SUCCEED;
    }

    r_nd_xf = new CLS_R_ND_XF[lv_r_nd_xf];
    r_nd_xf_tb->CopyTable(r_nd_xf);

    //rebuild pointer
    for (i = 0; i < lv_r_nd_xf; i++)
    {
        if ((r_nd_xf+i)->nd_id_ptr != -1)
        {
            (r_nd_xf+i)->nd_id_ptr = nd_tb->GetLogByPh((r_nd_xf+i)->nd_id_ptr);
        }
        if ((r_nd_xf+i)->xf_id_ptr != -1)
        {
            (r_nd_xf+i)->xf_id_ptr = xf_tb->GetLogByPh((r_nd_xf+i)->xf_id_ptr);
        }
        if ((r_nd_xf+i)->fptr != -1)
        {
            (r_nd_xf+i)->fptr = r_nd_xf_tb->GetLogByPh((r_nd_xf+i)->fptr);
        }
        if ((r_nd_xf+i)->bptr != -1)
        {
            (r_nd_xf+i)->bptr = r_nd_xf_tb->GetLogByPh((r_nd_xf+i)->bptr);
        }
    }

    //update table
    r_nd_xf_tb->UpdateTable(r_nd_xf);

    delete[] r_nd_xf;
    r_nd_xf = 0;
    lv_r_nd_xf = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ND_ZN_RebuildPtr()
int NMD::R_ZN_ND_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_zn_nd = r_zn_nd_tb->GetCount();
    if (lv_r_zn_nd == 0)
    {
        return NMD_SUCCEED;
    }

    r_zn_nd = new CLS_R_ZN_ND[lv_r_zn_nd];
    r_zn_nd_tb->CopyTable(r_zn_nd);

    //rebuild pointer
    for (i = 0; i < lv_r_zn_nd; i++)
    {
        if ((r_zn_nd+i)->nd_id_ptr != -1)
        {
            (r_zn_nd+i)->nd_id_ptr = nd_tb->GetLogByPh((r_zn_nd+i)->nd_id_ptr);
        }
        if ((r_zn_nd+i)->zn_id_ptr != -1)
        {
            (r_zn_nd+i)->zn_id_ptr = zn_tb->GetLogByPh((r_zn_nd+i)->zn_id_ptr);
        }
        if ((r_zn_nd+i)->fptr != -1)
        {
            (r_zn_nd+i)->fptr = r_zn_nd_tb->GetLogByPh((r_zn_nd+i)->fptr);
        }
        if ((r_zn_nd+i)->bptr != -1)
        {
            (r_zn_nd+i)->bptr = r_zn_nd_tb->GetLogByPh((r_zn_nd+i)->bptr);
        }
    }

    //update table
    r_zn_nd_tb->UpdateTable(r_zn_nd);

    delete[] r_zn_nd;
    r_zn_nd = 0;
    lv_r_zn_nd = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ND_CB_RebuildPtr()
int NMD::R_ND_CP_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_nd_cp = r_nd_cp_tb->GetCount();
    if (lv_r_nd_cp == 0)
    {
        return NMD_SUCCEED;
    }

    r_nd_cp = new CLS_R_ND_CP[lv_r_nd_cp];
    r_nd_cp_tb->CopyTable(r_nd_cp);

    //rebuild pointer
    for (i = 0; i < lv_r_nd_cp; i++)
    {
        if ((r_nd_cp+i)->nd_id_ptr != -1)
        {
            (r_nd_cp+i)->nd_id_ptr = nd_tb->GetLogByPh((r_nd_cp+i)->nd_id_ptr);
        }
        if ((r_nd_cp+i)->cp_id_ptr != -1)
        {
            (r_nd_cp+i)->cp_id_ptr = cp_tb->GetLogByPh((r_nd_cp+i)->cp_id_ptr);
        }
        if ((r_nd_cp+i)->fptr != -1)
        {
            (r_nd_cp+i)->fptr = r_nd_cp_tb->GetLogByPh((r_nd_cp+i)->fptr);
        }
        if ((r_nd_cp+i)->bptr != -1)
        {
            (r_nd_cp+i)->bptr = r_nd_cp_tb->GetLogByPh((r_nd_cp+i)->bptr);
        }
    }

    //update table
    r_nd_cp_tb->UpdateTable(r_nd_cp);

    delete[] r_nd_cp;
    r_nd_cp = 0;
    lv_r_nd_cp = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ND_CB_RebuildPtr()
int NMD::R_ND_LD_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_nd_ld = r_nd_ld_tb->GetCount();
    if (lv_r_nd_ld == 0)
    {
        return NMD_SUCCEED;
    }

    r_nd_ld = new CLS_R_ND_LD[lv_r_nd_ld];
    r_nd_ld_tb->CopyTable(r_nd_ld);

    //rebuild pointer
    for (i = 0; i < lv_r_nd_ld; i++)
    {
        if ((r_nd_ld+i)->nd_id_ptr != -1)
        {
            (r_nd_ld+i)->nd_id_ptr = nd_tb->GetLogByPh((r_nd_ld+i)->nd_id_ptr);
        }
        if ((r_nd_ld+i)->ld_id_ptr != -1)
        {
            (r_nd_ld+i)->ld_id_ptr = ld_tb->GetLogByPh((r_nd_ld+i)->ld_id_ptr);
        }
        if ((r_nd_ld+i)->fptr != -1)
        {
            (r_nd_ld+i)->fptr = r_nd_ld_tb->GetLogByPh((r_nd_ld+i)->fptr);
        }
        if ((r_nd_ld+i)->bptr != -1)
        {
            (r_nd_ld+i)->bptr = r_nd_ld_tb->GetLogByPh((r_nd_ld+i)->bptr);
        }
    }

    //update table
    r_nd_ld_tb->UpdateTable(r_nd_ld);

    delete[] r_nd_ld;
    r_nd_ld = 0;
    lv_r_nd_ld = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_SEC_FI_RebuildPtr()
int NMD::R_SEC_FI_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_sec_fi = r_sec_fi_tb->GetCount();
    if (lv_r_sec_fi == 0)
    {
        return NMD_SUCCEED;
    }

    r_sec_fi = new CLS_R_SEC_FI[lv_r_sec_fi];
    r_sec_fi_tb->CopyTable(r_sec_fi);

    //rebuild pointer
    for (i = 0; i < lv_r_sec_fi; i++)
    {
        if ((r_sec_fi+i)->fi_id_ptr != -1)
        {
            (r_sec_fi+i)->fi_id_ptr = fi_tb->GetLogByPh((r_sec_fi+i)->fi_id_ptr);
        }
        if ((r_sec_fi+i)->sec_id_ptr != -1)
        {
            (r_sec_fi+i)->sec_id_ptr = sec_tb->GetLogByPh((r_sec_fi+i)->sec_id_ptr);
        }
        if ((r_sec_fi+i)->fptr != -1)
        {
            (r_sec_fi+i)->fptr = r_sec_fi_tb->GetLogByPh((r_sec_fi+i)->fptr);
        }
        if ((r_sec_fi+i)->bptr != -1)
        {
            (r_sec_fi+i)->bptr = r_sec_fi_tb->GetLogByPh((r_sec_fi+i)->bptr);
        }
    }

    //update table
    r_sec_fi_tb->UpdateTable(r_sec_fi);

    delete[] r_sec_fi;
    r_sec_fi = 0;
    lv_r_sec_fi = 0;

    return NMD_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  R_ND_SEC_RebuildPtr()
int NMD::R_ND_SEC_RebuildPtr()
{
    int		i;

    //read table into buffer
    lv_r_nd_sec = r_nd_sec_tb->GetCount();
    if (lv_r_nd_sec == 0)
    {
        return NMD_SUCCEED;
    }

    r_nd_sec = new CLS_R_ND_SEC[lv_r_nd_sec];
    r_nd_sec_tb->CopyTable(r_nd_sec);

    //rebuild pointer
    for (i = 0; i < lv_r_nd_sec; i++)
    {
        if ((r_nd_sec+i)->nd_id_ptr != -1)
        {
            (r_nd_sec+i)->nd_id_ptr = nd_tb->GetLogByPh((r_nd_sec+i)->nd_id_ptr);
        }
        if ((r_nd_sec+i)->sec_id_ptr != -1)
        {
            (r_nd_sec+i)->sec_id_ptr = sec_tb->GetLogByPh((r_nd_sec+i)->sec_id_ptr);
        }
        if ((r_nd_sec+i)->fptr != -1)
        {
            (r_nd_sec+i)->fptr = r_nd_sec_tb->GetLogByPh((r_nd_sec+i)->fptr);
        }
        if ((r_nd_sec+i)->bptr != -1)
        {
            (r_nd_sec+i)->bptr = r_nd_sec_tb->GetLogByPh((r_nd_sec+i)->bptr);
        }
    }

    //update table
    r_nd_sec_tb->UpdateTable(r_nd_sec);

    delete[] r_nd_sec;
    r_nd_sec = 0;
    lv_r_nd_sec = 0;

    return NMD_SUCCEED;
}
#if 0	//D5000没有ANALOG、POINT表
////////////////////////////////////////////////////////////////////////////////
//  DIGITAL_ANALOG_PtrBuild()
int NMD::DIGITAL_ANALOG_PtrBuild()
{
    int			i, j;
    ChildPtr1	*ptr, *temp;

    //build pinters to digital table for each cb
    lv_cb = cb_tb->GetCount();
    if (lv_cb != 0)
    {
        for (i = 0; i < lv_cb; i++)
        {
            cb = cb_tb->GetDataByLog(i);
            cb->idigital = digital_tb->FindPh(CLS_DIGITAL(cb->digital));	//荣义平修改于20111012查找逻辑号FindLog改为查找物理号FindPh

            //荣义平添加于20110929
            digital_tb->Data[cb->idigital].icb = i;
            strcpy(digital_tb->Data[cb->idigital].cb, cb->id);
            //////////////////
        }
        cb = 0;
        lv_cb = 0;
    }

    lv_fi = fi_tb->GetCount();
    if(lv_fi != 0)
    {
    	for(i = 0; i < lv_fi; i++)
    	{
    		fi = fi_tb->GetDataByLog(i);
    		fi->idigital = digital_tb->FindPh(CLS_DIGITAL(fi->digital));
    	}

    	fi = 0;
    	lv_fi = 0;
    }
    //build pointers to analog table for each zn
    lv_zn = zn_tb->GetCount();
    if (lv_zn != 0)
    {
        for (i = 0; i < lv_zn; i++)
        {
            zn = zn_tb->GetDataByLog(i);
            zn->ialgip = analog_tb->FindLog(CLS_ANALOG(zn->algip));
            zn->ialgiq = analog_tb->FindLog(CLS_ANALOG(zn->algiq));
            zn->ialgii = analog_tb->FindLog(CLS_ANALOG(zn->algii));
            zn->ialgzp = analog_tb->FindLog(CLS_ANALOG(zn->algzp));
            zn->ialgzq = analog_tb->FindLog(CLS_ANALOG(zn->algzq));
            zn->ialgzi = analog_tb->FindLog(CLS_ANALOG(zn->algzi));
            zn->ialg3p = analog_tb->FindLog(CLS_ANALOG(zn->alg3p));
            zn->ialg3q = analog_tb->FindLog(CLS_ANALOG(zn->alg3q));
            zn->ialg3i = analog_tb->FindLog(CLS_ANALOG(zn->alg3i));
            zn->ialg4p = analog_tb->FindLog(CLS_ANALOG(zn->alg4p));
            zn->ialg4q = analog_tb->FindLog(CLS_ANALOG(zn->alg4q));
            zn->ialg4i = analog_tb->FindLog(CLS_ANALOG(zn->alg4i));
            zn->ialg5p = analog_tb->FindLog(CLS_ANALOG(zn->alg5p));
            zn->ialg5q = analog_tb->FindLog(CLS_ANALOG(zn->alg5q));
            zn->ialg5i = analog_tb->FindLog(CLS_ANALOG(zn->alg5i));
        }
        zn = 0;
        lv_zn = 0;
    }

    //build pointers to analog tabl for each sec
    lv_sec = sec_tb->GetCount();
    if (lv_sec != 0)
    {
        for (i = 0; i < lv_sec;i++)
        {
            sec = sec_tb->GetDataByLog(i);
            sec->ialgip = analog_tb->FindLog(CLS_ANALOG(sec->algip));
            sec->ialgiq = analog_tb->FindLog(CLS_ANALOG(sec->algiq));
            sec->ialgii = analog_tb->FindLog(CLS_ANALOG(sec->algii));
            sec->ialgzp = analog_tb->FindLog(CLS_ANALOG(sec->algzp));
            sec->ialgzq = analog_tb->FindLog(CLS_ANALOG(sec->algzq));
            sec->ialgzi = analog_tb->FindLog(CLS_ANALOG(sec->algzi));
//张瑞雪添加于20120216
            sec->ialgip_b = analog_tb->FindLog(CLS_ANALOG(sec->algip_b));
            sec->ialgiq_b = analog_tb->FindLog(CLS_ANALOG(sec->algiq_b));
            sec->ialgii_b = analog_tb->FindLog(CLS_ANALOG(sec->algii_b));
            sec->ialgzp_b = analog_tb->FindLog(CLS_ANALOG(sec->algzp_b));
            sec->ialgzq_b = analog_tb->FindLog(CLS_ANALOG(sec->algzq_b));
            sec->ialgzi_b = analog_tb->FindLog(CLS_ANALOG(sec->algzi_b));
            sec->ialgip_c = analog_tb->FindLog(CLS_ANALOG(sec->algip_c));
            sec->ialgiq_c = analog_tb->FindLog(CLS_ANALOG(sec->algiq_c));
            sec->ialgii_c = analog_tb->FindLog(CLS_ANALOG(sec->algii_c));
            sec->ialgzp_c = analog_tb->FindLog(CLS_ANALOG(sec->algzp_c));
            sec->ialgzq_c = analog_tb->FindLog(CLS_ANALOG(sec->algzq_c));
            sec->ialgzi_c = analog_tb->FindLog(CLS_ANALOG(sec->algzi_c));
            //张瑞雪添加于20120503
            sec->ialgip_a = analog_tb->FindLog(CLS_ANALOG(sec->algip_a));
            sec->ialgiq_a = analog_tb->FindLog(CLS_ANALOG(sec->algiq_a));
            sec->ialgii_a = analog_tb->FindLog(CLS_ANALOG(sec->algii_a));
            sec->ialgzp_a = analog_tb->FindLog(CLS_ANALOG(sec->algzp_a));
            sec->ialgzq_a = analog_tb->FindLog(CLS_ANALOG(sec->algzq_a));
            sec->ialgzi_a = analog_tb->FindLog(CLS_ANALOG(sec->algzi_a));
        }
        sec = 0;
        lv_sec = 0;
    }

    //build pointers to analog table for each bus
    lv_bus = bus_tb->GetCount();
    if (lv_bus != 0)
    {
        for (i = 0; i < lv_bus; i++)
        {
            bus = bus_tb->GetDataByLog(i);
          // bus->ialgv = analog_tb->FindLog(CLS_ANALOG(bus->algv));

            ////////////荣义平修改于20111031  获取BUS对应的analog逻辑号
            ///////////
            lv_analog = analog_tb->GetCount();

			for (int index = 0; index < lv_analog; index++)
			{
				if (strcmp(bus->algv, analog_tb->Data[index].id) == 0)
				{
					bus->ialgv = index;
					break;
				}
				if (strcmp(bus->algv_b, analog_tb->Data[index].id) == 0)
				{
					bus->ialgv_b = index;
					break;
				}
				if (strcmp(bus->algv_c, analog_tb->Data[index].id) == 0)
				{
					bus->ialgv_c = index;
					break;
				}
				if (strcmp(bus->algp, analog_tb->Data[index].id) == 0)
				{
					bus->ialgp = index;
					break;
				}
				if (strcmp(bus->algp_b, analog_tb->Data[index].id) == 0)
				{
					bus->ialgp_b = index;
					break;
				}
				if (strcmp(bus->algp_c, analog_tb->Data[index].id) == 0)
				{
					bus->ialgp_c = index;
					break;
				}
				if (strcmp(bus->alga, analog_tb->Data[index].id) == 0)
				{
					bus->ialga = index;
					break;
				}
				if (strcmp(bus->alga_b, analog_tb->Data[index].id) == 0)
				{
					bus->ialga_b = index;
					break;
				}
				if (strcmp(bus->alga_c, analog_tb->Data[index].id) == 0)
				{
					bus->ialga_c = index;
					break;
				}

			}
            //////////////
            /////////////////
        }
        bus = 0;
        lv_bus = 0;
    }

    //build pointers to analog table for each ld
    lv_ld = ld_tb->GetCount();
    if (lv_ld != 0)
    {
        for (i = 0; i < lv_ld; i++)
        {
            ld = ld_tb->GetDataByLog(i);
            ld->ialgp = analog_tb->FindLog(CLS_ANALOG(ld->algp));
            ld->ialgq = analog_tb->FindLog(CLS_ANALOG(ld->algq));
            ld->ialgi = analog_tb->FindLog(CLS_ANALOG(ld->algi));
//张瑞雪添加于20120216
            ld->ialgp_b = analog_tb->FindLog(CLS_ANALOG(ld->algp_b));
            ld->ialgq_b = analog_tb->FindLog(CLS_ANALOG(ld->algq_b));
            ld->ialgi_b = analog_tb->FindLog(CLS_ANALOG(ld->algi_b));
            ld->ialgp_c = analog_tb->FindLog(CLS_ANALOG(ld->algp_c));
            ld->ialgq_c = analog_tb->FindLog(CLS_ANALOG(ld->algq_c));
            ld->ialgi_c = analog_tb->FindLog(CLS_ANALOG(ld->algi_c));
            ld->ialgia = analog_tb->FindLog(CLS_ANALOG(ld->algia));
            ld->ialgia_c = analog_tb->FindLog(CLS_ANALOG(ld->algia_c));
            ld->ialgia_b = analog_tb->FindLog(CLS_ANALOG(ld->algia_b));
            //张瑞雪添加于20120503
            ld->ialgp_a = analog_tb->FindLog(CLS_ANALOG(ld->algp_a));
            ld->ialgq_a = analog_tb->FindLog(CLS_ANALOG(ld->algq_a));
            ld->ialgi_a = analog_tb->FindLog(CLS_ANALOG(ld->algi_a));
            ld->ialgia_a = analog_tb->FindLog(CLS_ANALOG(ld->algia_a));
        }
        ld = 0;
        lv_ld = 0;
    }

    //build pointers to analog table for each so
    lv_so = so_tb->GetCount();
    if (lv_so != 0)
    {
        for (i = 0; i < lv_so; i++)
        {
            so = so_tb->GetDataByLog(i);
            so->ialgp = analog_tb->FindLog(CLS_ANALOG(so->algp));
            so->ialgq = analog_tb->FindLog(CLS_ANALOG(so->algq));
            so->ialgi = analog_tb->FindLog(CLS_ANALOG(so->algi));
//张瑞雪添加于20120216
            so->ialgp_b = analog_tb->FindLog(CLS_ANALOG(so->algp_b));
            so->ialgq_b = analog_tb->FindLog(CLS_ANALOG(so->algq_b));
            so->ialgi_b = analog_tb->FindLog(CLS_ANALOG(so->algi_b));
            so->ialgp_c = analog_tb->FindLog(CLS_ANALOG(so->algp_c));
            so->ialgq_c = analog_tb->FindLog(CLS_ANALOG(so->algq_c));
            so->ialgi_c = analog_tb->FindLog(CLS_ANALOG(so->algi_c));
            //张瑞雪添加于20120503

        }
        so = 0;
        lv_so = 0;
    }

    //build pointers to analog table for each cp
    lv_cp = cp_tb->GetCount();
    if (lv_cp != 0)
    {
        for (i = 0; i < lv_cp; i++)
        {
            cp = cp_tb->GetDataByLog(i);
            cp->ialgq = analog_tb->FindLog(CLS_ANALOG(cp->algq));
//张瑞雪添加于20120216
            cp->ialgq_b = analog_tb->FindLog(CLS_ANALOG(cp->algq_b));
            cp->ialgq_c = analog_tb->FindLog(CLS_ANALOG(cp->algq_c));
            //张瑞雪添加于20120503
            cp->ialgq_a = analog_tb->FindLog(CLS_ANALOG(cp->algq_a));
        }
        cp = 0;
        lv_cp = 0;
    }

    //build pointers to analog table for each xfmr and xf
    lv_xfmr = xfmr_tb->GetCount();
    if (lv_xfmr != 0)
    {
        for (i = 0; i < lv_xfmr; i++)
        {
            xfmr = xfmr_tb->GetDataByLog(i);
            if (xfmr->type == 2) //two-winding xfmr
            {
                xfmr->ialgph = analog_tb->FindLog(CLS_ANALOG(xfmr->algph));
                xfmr->ialgqh = analog_tb->FindLog(CLS_ANALOG(xfmr->algqh));
                xfmr->ialgih = analog_tb->FindLog(CLS_ANALOG(xfmr->algih));
                xfmr->ialgth = analog_tb->FindLog(CLS_ANALOG(xfmr->algth));
                xfmr->ialgpl = analog_tb->FindLog(CLS_ANALOG(xfmr->algpl));
                xfmr->ialgql = analog_tb->FindLog(CLS_ANALOG(xfmr->algql));
                xfmr->ialgil = analog_tb->FindLog(CLS_ANALOG(xfmr->algil));
                xfmr->ialgtl = analog_tb->FindLog(CLS_ANALOG(xfmr->algtl));
//张瑞雪添加于20120216
                xfmr->ialgph_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algph_b));
                xfmr->ialgqh_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algqh_b));
                xfmr->ialgih_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algih_b));
                xfmr->ialgth_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algth_b));
                xfmr->ialgpl_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algpl_b));
                xfmr->ialgql_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algql_b));
                xfmr->ialgil_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algil_b));
                xfmr->ialgtl_b = analog_tb->FindLog(CLS_ANALOG(xfmr->algtl_b));
                xfmr->ialgph_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algph_c));
                xfmr->ialgqh_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algqh_c));
                xfmr->ialgih_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algih_c));
                xfmr->ialgth_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algth_c));
                xfmr->ialgpl_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algpl_c));
                xfmr->ialgql_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algql_c));
                xfmr->ialgil_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algil_c));
                xfmr->ialgtl_c = analog_tb->FindLog(CLS_ANALOG(xfmr->algtl_c));
                //张瑞雪添加于20120503
                xfmr->ialgph_a = analog_tb->FindLog(CLS_ANALOG(xfmr->algph_a));
                xfmr->ialgqh_a = analog_tb->FindLog(CLS_ANALOG(xfmr->algqh_a));
                xfmr->ialgih_a = analog_tb->FindLog(CLS_ANALOG(xfmr->algih_a));
                xfmr->ialgpl_a = analog_tb->FindLog(CLS_ANALOG(xfmr->algpl_a));
                xfmr->ialgql_a = analog_tb->FindLog(CLS_ANALOG(xfmr->algql_a));
                xfmr->ialgil_a = analog_tb->FindLog(CLS_ANALOG(xfmr->algil_a));
            }
            else if (xfmr->type == 3) //three-winding xfmr
            {
                xfmr->ialgph = analog_tb->FindLog(CLS_ANALOG(xfmr->algph));
                xfmr->ialgqh = analog_tb->FindLog(CLS_ANALOG(xfmr->algqh));
                xfmr->ialgih = analog_tb->FindLog(CLS_ANALOG(xfmr->algih));
                xfmr->ialgth = analog_tb->FindLog(CLS_ANALOG(xfmr->algth));
                xfmr->ialgpm = analog_tb->FindLog(CLS_ANALOG(xfmr->algpm));
                xfmr->ialgqm = analog_tb->FindLog(CLS_ANALOG(xfmr->algqm));
                xfmr->ialgim = analog_tb->FindLog(CLS_ANALOG(xfmr->algim));
                xfmr->ialgtm = analog_tb->FindLog(CLS_ANALOG(xfmr->algtm));
                xfmr->ialgpl = analog_tb->FindLog(CLS_ANALOG(xfmr->algpl));
                xfmr->ialgql = analog_tb->FindLog(CLS_ANALOG(xfmr->algql));
                xfmr->ialgil = analog_tb->FindLog(CLS_ANALOG(xfmr->algil));
                xfmr->ialgtl = analog_tb->FindLog(CLS_ANALOG(xfmr->algtl));
            }
            else
            {
                continue;
            }
        }

        for (i = 0; i < lv_xfmr; i++)
        {
            j = 0;  //child number
            xfmr = xfmr_tb->GetDataByLog(i);
            ptr = r_xfmr_xf_tb->FindChildPtr1(xfmr->r_xf);
            temp = ptr;
            while ( temp )
            {
                j++;
                temp = temp->Next;
            }
            if (j == 1)
            {
                temp = ptr;
                xf = xf_tb->GetDataByPh(temp->Ptr);
                xf->ialgip = xfmr->ialgph;
                xf->ialgiq = xfmr->ialgqh;
                xf->ialgii = xfmr->ialgih;
                xf->ialgit = xfmr->ialgth;
                xf->ialgzp = xfmr->ialgpl;
                xf->ialgzq = xfmr->ialgql;
                xf->ialgzi = xfmr->ialgil;
                xf->ialgzt = xfmr->ialgtl;
//张瑞雪添加于20120216
                xf->ialgip_b = xfmr->ialgph_b;
                xf->ialgiq_b = xfmr->ialgqh_b;
                xf->ialgii_b = xfmr->ialgih_b;
                xf->ialgit_b = xfmr->ialgth_b;
                xf->ialgzp_b = xfmr->ialgpl_b;
                xf->ialgzq_b = xfmr->ialgql_b;
                xf->ialgzi_b = xfmr->ialgil_b;
                xf->ialgzt_b = xfmr->ialgtl_b;
                xf->ialgip_c = xfmr->ialgph_c;
                xf->ialgiq_c = xfmr->ialgqh_c;
                xf->ialgii_c = xfmr->ialgih_c;
                xf->ialgit_c = xfmr->ialgth_c;
                xf->ialgzp_c = xfmr->ialgpl_c;
                xf->ialgzq_c = xfmr->ialgql_c;
                xf->ialgzi_c = xfmr->ialgil_c;
                xf->ialgzt_c = xfmr->ialgtl_c;
                //张瑞雪添加于20120503
                xf->ialgip_a = xfmr->ialgph_a;
                xf->ialgiq_a = xfmr->ialgqh_a;
                xf->ialgii_a = xfmr->ialgih_a;
                xf->ialgzp_a = xfmr->ialgpl_a;
                xf->ialgzq_a = xfmr->ialgql_a;
                xf->ialgzi_a = xfmr->ialgil_a;
            }
            else if (j == 3)
            {
                temp = ptr;
                xf = xf_tb->GetDataByPh(temp->Ptr);
                xf->ialgip = xfmr->ialgph;
                xf->ialgiq = xfmr->ialgqh;
                xf->ialgii = xfmr->ialgih;
                xf->ialgit = xfmr->ialgth;
                xf->ialgzp = -1;
                xf->ialgzq = -1;
                xf->ialgzi = -1;
                xf->ialgzt = -1;

                temp = temp->Next;
                xf = xf_tb->GetDataByPh(temp->Ptr);
                xf->ialgip = xfmr->ialgpm;
                xf->ialgiq = xfmr->ialgqm;
                xf->ialgii = xfmr->ialgim;
                xf->ialgit = xfmr->ialgtm;
                xf->ialgzp = -1;
                xf->ialgzq = -1;
                xf->ialgzi = -1;
                xf->ialgzt = -1;

                temp = temp->Next;
                xf = xf_tb->GetDataByPh(temp->Ptr);
                xf->ialgip = xfmr->ialgpl;
                xf->ialgiq = xfmr->ialgql;
                xf->ialgii = xfmr->ialgil;
                xf->ialgit = xfmr->ialgtl;
                xf->ialgzp = -1;
                xf->ialgzq = -1;
                xf->ialgzi = -1;
                xf->ialgzt = -1;
            }
            FreeChildPtr1(ptr);
        }

        xfmr = 0;
        xf = 0;
        lv_xfmr = 0;
    }

    return NMD_SUCCEED;
}
#endif
////////////////////////////////////////////////////////////////////////////////
//  SetVL()
//  This function sets vl filed of nd table. It searches through all nds
//  from bus nd by non-xf branches (i.e. cb, zn, sec), and sets vl of all
//  these nds equal to that of bus nd.
int NMD::SetVL()
{
    ///////////////////////////////////////////////////////////////////////////////

    int			i;
    float       vl;
    int			ind, knd, kknd;
    int			*busnd;
    ChildPtr1	*cb_p, *cb_p0;
    ChildPtr1	*sec_p, *sec_p0;

    //get record count in nd table
    lv_nd = nd_tb->GetCount();
    if (lv_nd == 0)
    {
       cout << "节点表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    //read all nds into buffer
    nd = new CLS_ND[lv_nd];
    nd_tb->CopyTable(nd);

    //mark all nds as unprocessed
    for (i = 0; i < lv_nd; i++)
    {
        (nd+i)->q |= ND_UNPROC;
    }

    //get record count in bus table
    lv_bus = bus_tb->GetCount();
    if (lv_bus == 0)
    {
        cout << "母线表中无记录!" <<endl;

        return NMD_SUCCEED;
    }

    //read all bus nds into buffer
    //and set vl of all bus nds
    busnd = new int[lv_bus];
    for (i = 0; i < lv_bus; i++)
    {
        *(busnd+i) = bus_tb->GetDataByLog(i)->ind;
        (nd+*(busnd+i))->vl = bus_tb->GetDataByLog(i)->vl;
    }

    //search through nds from bus nd
    for (i = 0; i < lv_bus; i++)
    {
        ind = *(busnd+i);

#if 1			//荣义平于20110927调试段错误添加
        if(ind < 0) continue;
#endif

        if ((nd+ind)->q & ND_UNPROC) //this bus nd has not processed
        {
            vl = (nd+ind)->vl;

            //create circular chain for this bus nd
            knd = ind;
            (nd+knd)->fch = knd;
            (nd+knd)->q &= (~ND_UNPROC);

            while ( 1 )
            {
                //read cbs connected to this nd
//            	if(strcmp((nd+knd)->id, "LSLSEC5@Z*") == 0)
//            	{
//            		sleep(0);
//            	}
                cb_p = r_nd_cb_tb->FindChildPtr1((nd+knd)->r_cb);
                cb_p0 = cb_p;
                while (cb_p)
                {
                    //find the opposite nd through this cb
                    if (knd == cb_tb->GetDataByPh(cb_p->Ptr)->ind)
                    {
                        kknd = cb_tb->GetDataByPh(cb_p->Ptr)->iznd;
                    }
                    else
                    {
                        kknd = cb_tb->GetDataByPh(cb_p->Ptr)->ind;
                    }

                    //process this opposite nd
                    if ((nd+kknd)->q & ND_UNPROC)
                    {
                        //set vl of this nd
                        if ((nd+kknd)->vl == 0) //non-bus nd
                        {
                            (nd+kknd)->vl = vl;
                        }
                        else  //bus nd
                        {
                            if ((nd+kknd)->vl != vl)
                            {
                                TRACE("节点(%ld)的母线电压等级可能错误!\r\n",(nd+kknd)->id);

                            }
                        }

                        //put this nd into circular chain
                        (nd+kknd)->fch = (nd+knd)->fch;
                        (nd+knd)->fch = kknd;
                        (nd+kknd)->q &= (~ND_UNPROC);
                    }
                    cb_p = cb_p->Next;
                }
                FreeChildPtr1(cb_p0);

                //read secs connected to this nd
                sec_p = r_nd_sec_tb->FindChildPtr1((nd+knd)->r_sec);
                sec_p0 = sec_p;
                while (sec_p)
                {
                    //find the opposite nd through this sec
                    if (knd == sec_tb->GetDataByPh(sec_p->Ptr)->ind)
                    {
                        kknd = sec_tb->GetDataByPh(sec_p->Ptr)->iznd;
                    }
                    else
                    {
                        kknd = sec_tb->GetDataByPh(sec_p->Ptr)->ind;
                    }

                    //process this opposite nd
                    if ((nd+kknd)->q & ND_UNPROC)
                    {
                        //set vl of this nd
                        if ((nd+kknd)->vl == 0) //non-bus nd
                        {
                            (nd+kknd)->vl = vl;
                        }
                        else  //bus nd
                        {
                            if ((nd+kknd)->vl != vl)
                            {
                                TRACE("the bus vl of nd (%d) may be wrong\n", kknd);
                            }
                        }

                        //put this nd into circular chain
                        (nd+kknd)->fch = (nd+knd)->fch;
                        (nd+knd)->fch = kknd;
                        (nd+kknd)->q &= (~ND_UNPROC);
                    }
                    sec_p = sec_p->Next;
                }
                FreeChildPtr1(sec_p0);

                //move to next nd
                knd = (nd+knd)->fch;
                if (knd == ind)
                {
                    break;
                }
            }
        }
    }

    //update table
    nd_tb->UpdateTable(nd);

    delete[] busnd;
    delete[] nd;
    nd = 0;
    lv_nd = 0;

    return NMD_SUCCEED;
}

//根据母线的遥测信息在母线上增加SO
int NMD::add_so()
{
	cout << "增加电源！" << endl;

	so_tb->Empty();

	lv_bus = bus_tb->GetCount();

	for (int i = 0; i < lv_bus; i++)
	{
		int ind = bus_tb->Data[i].ind;

		if (ind < 0)
			continue; //无节点

		PSBOB::BUS psbob_bus = { 0 };

		//读取母线对应的遥测信息
//		if (oodbread_ak(&value, bus_tb->Data[i].algv, "value", tb_descr, 4) == OO_FAIL)
		if (oodbread_rk(&psbob_bus, &bus_tb->Data[i].pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(psbob_bus)) <= 0)
		{
			cout << "读取母线" << bus_tb->Data[i].id << "电压信息失败,errno:" << _oodb_errno << endl;

			continue; //物理母线无电压信息
		}

		if(psbob_bus.v < 1) continue;

		CLS_SO tmp_so;

		KEY_STRU ks = { 0 };

		CCommon::long_to_keyid(dsbob->bus_tb->GetDataByPos(i)->id, &ks);

		tmp_so.id= get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SO")), ks.key);//bus_tb->Data[i].id;
		tmp_so.stid = bus_tb->Data[i].stid;
		tmp_so.pid = bus_tb->Data[i].pid;

		tmp_so.ind = ind;
		tmp_so.nd= bus_tb->Data[i].nd;
		strcpy(tmp_so.descr, bus_tb->Data[i].descr);
//		analog_tb->Data[bus_tb->Data[i].ialgv].value = value;	//更新ANALOG表信息
		//张瑞雪添加于20120426
		//D5000没有ANALOG、POINT表
//		tmp_so.v = analog_tb->Data[bus_tb->Data[i].ialgv].value;
//		tmp_so.v_a = analog_tb->Data[bus_tb->Data[i].ialgv_a].value;
//		tmp_so.v_b = analog_tb->Data[bus_tb->Data[i].ialgv_b].value;
//		tmp_so.v_c = analog_tb->Data[bus_tb->Data[i].ialgv_c].value;
//		tmp_so.adeg = analog_tb->Data[bus_tb->Data[i].ialga].value;
//		tmp_so.adeg_a = analog_tb->Data[bus_tb->Data[i].ialga_a].value;
//		tmp_so.adeg_b = analog_tb->Data[bus_tb->Data[i].ialga_b].value;
//		tmp_so.adeg_c = analog_tb->Data[bus_tb->Data[i].ialga_c].value;
//		strcpy(tmp_so.algp , bus_tb->Data[i].algp);
//		strcpy(tmp_so.algp_a,bus_tb->Data[i].algp_a);
//		strcpy(tmp_so.algp_b,bus_tb->Data[i].algp_b);
//		strcpy(tmp_so.algp_c , bus_tb->Data[i].algp_c);
//		tmp_so.ialgp = bus_tb->Data[i].ialgp;
//		tmp_so.ialgp_a = bus_tb->Data[i].ialgp_a;
//		tmp_so.ialgp_b = bus_tb->Data[i].ialgp_b;
//		tmp_so.ialgp_c = bus_tb->Data[i].ialgp_c;
//		tmp_so.wmx = bus_tb->Data[i].wmx;
//		tmp_so.slackavr = bus_tb->Data[i].slackavr;

		so_tb->Insert(tmp_so);

		nd_tb->Find(CLS_ND(bus_tb->Data[i].nd))->q |= ND_SO;;
	}

	lv_so = so_tb->GetCount();

	return true;
}

//根据馈线首端节点获取该节点到电源的有效路径
stack<NODE> get_virtual_path(long nd)
{
	map<long, list<stack<NODE> > >::const_iterator it = g_map_nd_path.find(nd);

	if (it != g_map_nd_path.end())
	{
		for (list<stack<NODE> >::const_iterator it_lst = it->second.begin(); it_lst != it->second.end(); it_lst++)
		{
			stack<NODE> path = *it_lst;

			while (path.size() > 0)
			{
				NODE *p = &path.top();

				if (p->type == _CB)
				{
					if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), (char*) &(p->data.cb.id), sizeof(PSBOB::CB), (char*) &(p->data.cb)) < 0)
					{
						TRACE("读取ID为%ld的开关记录失败！\r\n", p->data.cb.id);
					}

					if (p->data.cb.point <= 0)
					{
						break;
					}
				}
				else if (p->type == _SWITCH)
				{
					if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("disconnector")), (char*) &(p->data.disconnector.id), sizeof(PSBOB::SWITCH), (char*) &(p->data.disconnector)) < 0)
					{
						TRACE("读取ID为%ld的刀闸记录失败！\r\n", p->data.disconnector.id);
					}

					if (p->data.disconnector.point <= 0)
					{
						break;
					}
				}
				else if (p->type == _LD)
				{
				}
				else if (p->type == _BUS)
				{
					if (GetRecByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), (char*) &(p->data.bus.id), sizeof(PSBOB::BUS), (char*) &(p->data.bus)) < 0)
					{
						TRACE("读取ID为%ld的母线记录失败！\r\n", p->data.bus.id);
					}

					if (p->data.bus.v <= 0)
					{
						break;
					}
				}
				else
				{
				}

				path.pop();
			}

			if (path.size() == 0)
			{
				//将列表元素顺序进行调整，把进线开关放在最顶，母线放在最底
				stack<NODE> path = *it_lst;
				stack<NODE> path1;

				while(path.size() > 0)
				{
					path1.push(path.top());

					path.pop();
				}

				return path1;
			}
		}
	}

	return stack<NODE>();
}
//更新开关的开合状态
bool NMD::update_cb_state()
{
	cout << "更新开关状态！" << endl;

	lv_cb = cb_tb->GetCount();

	//更新开关状态
	for (int i = 0; i < lv_cb; i++)
	{
		CLS_CB *cb = cb_tb->GetDataByPos(i);

		//虚拟开关开合状态
		if (cb->q & CB_VIRTUAL)
		{
			if(get_virtual_path(cb->nd).size() > 0)
			{
				cb->q &= ~CB_OPEN;
			}
			else
			{
				cb->q |= CB_OPEN;
			}
		}
		//普通开关开合状态
		else
		{
			PSBOB::CB psbob_cb = { 0 };

			if (oodbread_rk(&psbob_cb, &cb->pid, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) == OO_FAIL)
			{
				cout << "更新开关" << cb->id << "状态失败,errno:" << _oodb_errno << endl;

				return false;
			}

			cb->q &= ~CB_OPEN;

			if (psbob_cb.point <= 0)
				cb->q |= CB_OPEN;
		}
	}

	return true;
}

/*
 * 功能：递归查找与指定节点相连的设备
 *
 * so:存储所有电源信息、路径信息
 * ind：节点物理号
 * bfindso：true只返回设备到电源的路径,false返回所有搜索到的设备
 * bcare_cb_close：是否忽略开关的开合状态 true表示开关断开时停止本路径的继续搜索，false开关分闸时继续搜索
 * bbuild_zn:是否自动建立ZN
 * bsave_ld：是否需要搜索关联的LD
 */

void NMD::find(MAP_SO_PATH &so, int ind, bool bfindso, bool bcare_cb_close, DEV *data, bool bbuild_zn, bool bsave_ld)
{
	assert(bvisited_nd != NULL && bvisited_zn != NULL);
//	cout << "visit " << nd_tb->Data[ind].id << endl;


	ChildPtr1 *br_p, *br_p0;
	int br, znd;
	CLS_ND *pnd = nd_tb->GetDataByLog(ind);

	if(!bbuild_zn)
	{
		TRACE("->PUSH %ld, %s, %ld\r\n", data->id, data->table.c_str(), pnd->id);
		m_path.push(*data);
	}

	m_zn_ind[ind] = nd_tb->Data[ind]; //将该ND存入ZN的ind列表

	bvisited_nd[ind] = true;	//设置该节点已经访问过

	DEV dev;

	if (bbuild_zn)
	{
		if(nd_tb->Data[ind].r_ld >= 0)
		{
			br_p = dsbob->r_nd_ld_tb->FindChildPtr1(pnd->r_ld);
			br_p0 = br_p;

			while(br_p != NULL)
			{
				m_zn_ld.push_back(*dsbob->ld_tb->GetDataByLog(br_p->Ptr));
				br_p = br_p->Next;
			}

			FreeChildPtr1(br_p0);

#if ZN_ND_USE_ARRAY
			//由于受表结构的限制，一个区段只能存储6个未端结点，但实际某些区段的未端结点数不止6个，故将连接负荷的
			//未端结点忽略
			m_zn_end_ind[ind] = nd_tb->Data[ind];
#endif
			//return;
		}

		map<long, CLS_BUS>::iterator it_bus = m_map_bus.find(nd_tb->Data[ind].id);
		//建立ZN搜索遇到BUS即中止本条路径的继续搜索,并将该ND存入ZN的未端ND列表
		if(it_bus != m_map_bus.end())
		{
			m_zn_end_ind[ind] = nd_tb->Data[ind];

			return;
		}
	}
	else
	{
		//需要查找关联的LD时，逐个查找与该ND关联的ZN
		if (bsave_ld && pnd->r_ld != -1)
		{
			br_p = r_nd_ld_tb->FindChildPtr1(pnd->r_ld);

			br_p0 = br_p;
			while (br_p)
			{
				br = br_p->Ptr;

				ld = ld_tb->GetDataByLog(br);
				m_lst_ld.push_back(*ld);

				br_p = br_p->Next;
			}
			FreeChildPtr1(br_p0);
		}

		MAP_SO_PATH::iterator it = so.find(nd_tb->Data[ind].id);

		if (it != so.end())
		{
			cout << "找到电源" << so_tb->Data[it->second.so_id].descr << endl;

			dev.id = so_tb->Data[it->second.so_id].pid;
			dev.table = "busbarsection";
			dev.nd = pnd->id;
//			m_path.push(dev);

			it->second.path = m_path;

			//获取虚拟开关到母线间的路径
			DEV tmp_dev = it->second.path.top();

			DMS::CB dms_cb = { 0 };

			if (oodbread_ra(&dms_cb, "pid", &tmp_dev.id, const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CB")), sizeof(DMS::CB)) > 0)
			{
				stack<NODE> path = get_virtual_path(dms_cb.nd);

				//该进线开关是虚拟开关
				if (path.size() > 0)
				{
					it->second.path.pop();

					while(path.size() >0)
					{
						switch(path.top().type)
						{
						case _CB:
							tmp_dev.id = path.top().data.cb.id;
							tmp_dev.table = "breaker";
							break;

						case _SWITCH:
							tmp_dev.id = path.top().data.disconnector.id;
							tmp_dev.table = "disconnector";
							break;

						case _BUS:
							tmp_dev.id = path.top().data.bus.id;
							tmp_dev.table = "busbarsection";
							break;

						case _SEC:
							tmp_dev.id = path.top().data.sec.id;
							tmp_dev.table = "aclinesegment";
							break;

						default:
							tmp_dev.id = 0;
							tmp_dev.table = "null";
							break;
						}

						if(tmp_dev.id > 0)
							it->second.path.push(tmp_dev);

						path.pop();
					}
				}
				//不是虚拟开关
				else
				{
					it->second.path.push(dev);
				}
			}
			else
				it->second.path.push(dev);
			TRACE("<-POP  %ld,%s\r\n", m_path.top().id, m_path.top().table.c_str());

			if (bfindso)
			{
				m_path.pop();
			}
			else
			{
				m_path.push(dev);
			}

			if(bsave_ld)
			{
				//先储存SO的ID值，在其它代码中进行统一转换到DV的ID
				m_dv_sec.b_has_so = true;

				m_dv_sec.last_sec = *m_dv_sec.secs.begin();
			}

			return;
		}
	}

	//查找与该节点相连的所有线段
//	cout << "======sec======" << endl;
	if (pnd->r_sec != -1)
	{
		br_p = r_nd_sec_tb->FindChildPtr1(pnd->r_sec);

		br_p0 = br_p;
		while (br_p)
		{
			br = br_p->Ptr;
			sec = sec_tb->GetDataByLog(br);
			if (ind == sec->ind)
			{
				znd = sec->iznd;
			}
			else
			{
				znd = sec->ind;
			}

			if (!bvisited_nd[znd])
			{
				//防误闭锁时储存一条SEC用来查找DV
				if (bsave_ld)
				{
					//先储存SEC的ID值，在其它代码中进行统一转换到DV的ID
					m_dv_sec.secs.push_front(sec->id);
				}

				if (!bbuild_zn)
				{
					dev.id = sec->pid;
					dev.table = "aclinesegment";
					dev.nd = pnd->id;
//					m_path.push(dev);
				}
				else
				{
					//将该SEC存入SEC链表，以便建立SEC与ZN的关系
					m_zn_sec.push_back(*sec);
				}

				if(!bbuild_zn)
				{
					TRACE("[   SEC   ]%ld[%s], ind:%ld, jnd:%ld\r\n", sec->pid, sec->descr, sec->nd, sec->znd);
				}

				find(so, znd, bfindso, bcare_cb_close, &dev, bbuild_zn, bsave_ld);
			}
			else
			{
//				cout << nd_tb->Data[ind].id << " visited![sec]" << endl;
			}
			br_p = br_p->Next;
		}
		FreeChildPtr1(br_p0);
	}

	if (pnd->r_switch != -1)
	{
		br_p = r_nd_switch_tb->FindChildPtr1(pnd->r_switch);

		br_p0 = br_p;
		while (br_p)
		{
			br = br_p->Ptr;
			swh = switch_tb->GetDataByLog(br);
			if (ind == swh->ind)
			{
				znd = swh->iznd;
			}
			else
			{
				znd = swh->ind;
			}

			if(bbuild_zn)
			{
				if (!bvisited_nd[znd])
				{
					find(so, znd, bfindso, bcare_cb_close, NULL, bbuild_zn, bsave_ld);
				}
				else
				{
					//该结点如果为母线结点，就可能在建立其它区段时已经被访问过
					CLS_ND* tmp = nd_tb->GetDataByPos(znd);

					if(m_map_bus.find(tmp->id) != m_map_bus.end())
					{
						m_zn_end_ind[znd] = *tmp;
					}
				}
			}
			else
			{
				if (!bvisited_nd[znd])
				{

					dev.id = swh->pid;
					dev.table = "disconnector";
					dev.nd = pnd->id;

					TRACE("[DISCONNECTOR]%ld[%s], ind:%ld, jnd:%ld\r\n", swh->pid, swh->descr, swh->nd, swh->znd);

					find(so, znd, bfindso, bcare_cb_close, &dev, bbuild_zn, bsave_ld);
				}
			}
			br_p = br_p->Next;
		}
		FreeChildPtr1(br_p0);
	}

	//查找与该节点相连的所有绕组
	//xf
//	cout << "======xf======" << endl;
	if (pnd->r_xf != -1)
	{
		br_p = r_nd_xf_tb->FindChildPtr1(pnd->r_xf);

		br_p0 = br_p;
		while (br_p)
		{
			br = br_p->Ptr;
			xf = xf_tb->GetDataByLog(br);
			if (ind == xf->ind)
			{
				znd = xf->iznd;
			}
			else
			{
				znd = xf->ind;
			}

			if (!bvisited_nd[znd])
			{
				if (!bbuild_zn)
				{
					dev.id = xf->pid;
					dev.table = "transformerwinding";
					dev.nd = pnd->id;
//					m_path.push(dev);
				}

				cout << "[   XF   ]" << xf->descr << endl;
				find(so, znd, bfindso, bcare_cb_close, &dev, bbuild_zn, bsave_ld);
			}
			else
			{
//				cout << nd_tb->Data[ind].id << " visited[xf]" << endl;
			}
			br_p = br_p->Next;
		}
		FreeChildPtr1(br_p0);
	}

	//查找与该节点相连的所有开关
	//cb
//	cout << "======cb======" << endl;
	if (pnd->r_cb != -1)
	{
		if (bbuild_zn)
		{
			br_p = r_nd_cb_tb->FindChildPtr1(pnd->r_cb);

			br_p0 = br_p;

			while (br_p)
			{
				br = br_p->Ptr;
				cb = cb_tb->GetDataByLog(br);

				m_zn_end_ind[ind] = nd_tb->Data[ind];

				br_p = br_p->Next;
			}

			FreeChildPtr1(br_p0);
		}
		else
		{
			br_p = r_nd_cb_tb->FindChildPtr1(pnd->r_cb);

			br_p0 = br_p;
			while (br_p)
			{
				br = br_p->Ptr;
				cb = cb_tb->GetDataByLog(br);

				if (ind == cb->ind)
				{
					znd = cb->iznd;
				}
				else
				{
					znd = cb->ind;
				}

				if ((cb->q & CB_OPEN) && bcare_cb_close)
				{
					cout << cb->descr << "开关断开！" << endl;
				}
				else if (!bvisited_nd[znd])
				{
					//建立ZN搜索遇到CB即中止本条路径的继续搜索,并将该ND存入ZN的未端ND列表

					dev.id = cb->pid; //cb->id;	//返回CB对应的遥信ID
					dev.table = "breaker";
					dev.nd = pnd->id;
//					m_path.push(dev);

//					cout << "[===CB===]" << cb->descr << endl;
					TRACE("[===CB===]%ld[%s], ind:%ld, jnd:%ld\r\n", cb->pid, cb->descr, cb->nd, cb->znd);
					find(so, znd, bfindso, bcare_cb_close, &dev, bbuild_zn, bsave_ld);

				}
				else
				{
//					cout << nd_tb->Data[ind].id << " visited[cb]" << endl;
				}
				br_p = br_p->Next;
			}
			FreeChildPtr1(br_p0);
		}
	}

	if (bfindso && !bbuild_zn)
	{
		if (m_path.size())
		{
			TRACE("<-POP  %ld,%s\r\n", m_path.top().id, m_path.top().table.c_str());
			m_path.pop();
		}
		else
		{
			cout << "stack is empty!" << endl;
		}
	}
}

/*
 * id 设备ID
 * table 设备类型
 * paths 返回路径信息
 * type 查找类型  FIND_TYPE_SO电源跟踪 ，FIND_TYPE_SO_AREA供电范围，FIND_TYPE_CONNECT设备连通检查
 * pdata_mis 防误闭锁判断时返回结果，其它搜索该参数被忽略
 *
 * 成功返回true,失败返回false
 */

bool NMD::find_so(const long id, const char *table, MAP_SO_PATH &paths, int type, PDATA_MISOPERATION pdata_mis/* = NULL*/)
{
	//参数有效性检查
	switch (type)
	{
	case FIND_TYPE_SO:
	case FIND_TYPE_SO_AREA:
	case FIND_TYPE_CONNECT:
		assert(table != NULL);

		if (table == NULL) return false;
		break;

	case FIND_TYPE_MISOPERAT:
		assert(table != NULL && pdata_mis != NULL);

		if (table == NULL || pdata_mis == NULL) return false;

		break;

	case FIND_TYPE_BUILD_ZN:
		break;

	default:
		break;
	}

	paths.clear();
	m_Errors.clear();

	int cnt_nd = 0;
	char sError[1024] = {0};
	int pnds[5] = { 0 };

	long newid = id;

	//将传进来的SCADA设备ID转换为对应 的DMS表的设备ID
	pid_to_id(newid);

	switch (type)
	{
	case FIND_TYPE_SO:
	case FIND_TYPE_SO_AREA:
	case FIND_TYPE_CONNECT:
	case FIND_TYPE_MISOPERAT:
		//获取请求设备的节点信息
		GetDevNDs(newid, table, pnds, cnt_nd);

		if (cnt_nd <= 0)
		{
			sprintf(sError, "获取%ld[%s]对应的结点信息失败！\r\n", newid, table);
			cout << sError << endl;
			m_Errors.push_back(sError);

			return false;
		}

		break;

	case FIND_TYPE_BUILD_ZN:
		break;

	default:
		break;
	}

	//清空上一次的路径信息
	while(m_path.size() > 0) m_path.pop();

	MAP_SO_PATH map_so;

	int i = 0;
	lv_so = so_tb->GetCount();	//电源个数
	lv_nd = nd_tb->GetCount();	//节点个数
	lv_zn = zn_tb->GetCount();

	//将所有电源存入MAP
	for (i = 0; i < lv_so; i++)
	{
		SO_PATH tPath = { i, STACK_DEV() };
		map_so[so_tb->Data[i].nd] = tPath;	//每个电源对应一条路径信息
	}

	bvisited_nd = new bool[lv_nd];
	bvisited_zn = new bool[lv_zn];

	memset(bvisited_zn, false, sizeof(bool) * lv_zn);
	memset(bvisited_nd, false, sizeof(bool) * lv_nd);

	DEV dev = {0};
	//开始深度优先搜索
	if (type != FIND_TYPE_BUILD_ZN)
	{
		dev.id = id;
		dev.table = table;
		dev.nd = 0;
	}

	switch (type)
	{
	case FIND_TYPE_SO:		//电源跟踪
		cout << "开始电源跟踪" << endl;

		for (i = 0; i < cnt_nd; i++)
		{
			find(map_so, pnds[i], true, true, &dev);
		}

		paths = map_so;

		break;

	case FIND_TYPE_SO_AREA: //供电范围
	{
		cout << "开始查找供电范围" << endl;

		m_lst_ld.clear();

		for (i = 0; i < cnt_nd; i++)
		{
			find(map_so, pnds[i], false, true, &dev, false, true);
		}

		bool bhasso = false;

		//是否找到电源
		for(MAP_SO_PATH::iterator it = map_so.begin(); it != map_so.end(); it++)
		{
			if(it->second.path.size() > 0)
			{
				bhasso = true;
				break;
			}
		}

		//无电源无供电范围
		if(!bhasso)
		{
			while(m_path.size()) m_path.pop();
		}

		for (LST_LD::iterator it = m_lst_ld.begin(); it != m_lst_ld.end(); it++)
		{
			long pid = it->id;

			id_to_pid(pid);

			DEV dev = { pid, "energyconsumer", 0 };
			m_path.push(dev);
		}

		SO_PATH path = { 0, m_path };
		paths[SO_AREA] = path;

		break;
	}

	case FIND_TYPE_CONNECT:	//设备连通性检查
	{
		cout << "开始设备连通性检查" << endl;

		m_lst_ld.clear();

		for (i = 0; i < cnt_nd; i++)
		{
			find(map_so, pnds[i], false, false, &dev, false, true);
		}

		for (LST_LD::iterator it = m_lst_ld.begin(); it != m_lst_ld.end(); it++)
		{
			long pid = it->id;

			id_to_pid(pid);

			DEV dev = { pid, "energyconsumer", 0 };
			m_path.push(dev);
		}

		SO_PATH path = { 0, m_path };
		paths[CONNECTION] = path;

		break;
	}

	case FIND_TYPE_MISOPERAT:	//防误闭锁判断
	{
		cout << "开始防误闭锁判断" << map_zn_ld.size() << endl;

		CLS_CB cb_data(newid);

		m_dv_sec.b_has_so = false;
		m_dv_sec.last_sec = 0;
		m_dv_sec.secs.clear();

		m_lst_ld.clear();

		cb_tb->Find(cb_data)->q |= CB_OPEN;	//先将本CB断开，防止搜索CB一端电源时搜索到CB的另一端

		bool bhasso[2] = { false, false };
		//在CB的一端查找电源
		find(map_so, pnds[0], true, true, &dev, false, true);
/*
		MAP_SO_PATH::iterator it;

		//是否找到电源
		for (it = map_so.begin(); it != map_so.end(); it++)
		{
			if (it->second.path.size() > 0)
			{
				bhasso[0] = true;
				break;
			}
		}
*/
		pdata_mis->lst_ld1 = m_lst_ld;
		//pdata_mis->dv1 = m_dv;

		bhasso[0] = m_dv_sec.b_has_so;

		//将SO的ID转换为DV的ID
		if (bhasso[0])
		{
			PSBOB::DV psbob_dv = { 0 };
			PSBOB::SEC psbob_sec = { 0 };
			PSBOB::OO_R_DV_ST oo_r_dv_st = { 0 };

			id_to_pid(m_dv_sec.last_sec);

			if(oodbread_rk(&psbob_sec, &m_dv_sec.last_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), sizeof(PSBOB::SEC)) < 0)
			{
				TRACE("%s(%d):读取aclinesegment表关键字为%ld的记录失败，错误号%d\r\n",__FUNCTION__, __LINE__, m_dv_sec.last_sec, _oodb_errno);
				break;
			}

			if(oodbread_ra(&oo_r_dv_st, "second_id", &psbob_sec.ist_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subctrlarea_substation")), sizeof(PSBOB::OO_R_DV_ST)) < 0)
			{
				TRACE("%s(%d):读取subctrlarea_substation表second为%ld的记录失败，错误号%d\r\n",__FUNCTION__, __LINE__, psbob_sec.jst_id, _oodb_errno);
				break;
			}

			//读取DV记录
			if (oodbread_rk(&psbob_dv, &oo_r_dv_st.first_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(PSBOB::DV)) < 0)
			{
				TRACE("%s(%d):读取subcontrolarea关键字为%ld的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, oo_r_dv_st.first_id, _oodb_errno);
				break;
			}

			pdata_mis->dv1.push_back(psbob_dv);
		}

		TRACE("CB[%ld]一端dv:%d个, ld:%d个\r\n", id, pdata_mis->dv1.size(), m_lst_ld.size());
/*
		//清空电源路径
		for (it = map_so.begin(); it != map_so.end(); it++)
		{
			while(it->second.path.size() > 0)
				it->second.path.pop();
		}
*/

		m_dv_sec.b_has_so = false;
		m_dv_sec.last_sec = 0;
		m_dv_sec.secs.clear();

		m_lst_ld.clear();

		//在CB的另一端查找电源
		find(map_so, pnds[1], true, true, &dev, false, true);
/*
		//是否找到电源
		for (it = map_so.begin(); it != map_so.end(); it++)
		{
			if (it->second.path.size() > 0)
			{
				bhasso[1] = true;
				break;
			}
		}
*/
		pdata_mis->lst_ld2 = m_lst_ld;
//		pdata_mis->dv2 = m_dv;

		bhasso[1] = m_dv_sec.b_has_so;

		//将SO的ID转换为DV的ID
		if (bhasso[1])
		{
			PSBOB::DV psbob_dv = { 0 };
			PSBOB::SEC psbob_sec = { 0 };
			PSBOB::OO_R_DV_ST oo_r_dv_st = { 0 };

			id_to_pid(m_dv_sec.last_sec);

			if(oodbread_rk(&psbob_sec, &m_dv_sec.last_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), sizeof(PSBOB::SEC)) < 0)
			{
				TRACE("%s(%d):读取aclinesegment表关键字为%ld的记录失败，错误号%d\r\n",__FUNCTION__, __LINE__, m_dv_sec.last_sec, _oodb_errno);
				break;
			}

			if(oodbread_ra(&oo_r_dv_st, "second_id", &psbob_sec.ist_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subctrlarea_substation")), sizeof(PSBOB::OO_R_DV_ST)) < 0)
			{
				TRACE("%s(%d):读取subctrlarea_substation表second为%ld的记录失败，错误号%d\r\n",__FUNCTION__, __LINE__, psbob_sec.jst_id, _oodb_errno);
				break;
			}

			//读取DV记录
			if (oodbread_rk(&psbob_dv, &oo_r_dv_st.first_id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("subcontrolarea")), sizeof(PSBOB::DV)) < 0)
			{
				TRACE("%s(%d):读取subcontrolarea关键字为%ld的记录失败，错误号：%d\r\n", __FUNCTION__, __LINE__, oo_r_dv_st.first_id, _oodb_errno);
				break;
			}

			pdata_mis->dv2.push_back(psbob_dv);
		}

		TRACE("CB[%ld]另一端dv:%d个, ld:%d个\r\n", id, pdata_mis->dv2.size(), m_lst_ld.size());

//		delete bvisited_zn;
		break;
	}

	case FIND_TYPE_BUILD_ZN:
	{
		cout << "开始建立ZN" << endl;

//		m_map_ld.clear();
		m_map_bus.clear();

		r_zn_ld_tb->Empty();

		//int	first, prevph_ld = 0, prevph_sec = 0;

		lv_ld = ld_tb->GetCount();

//		for(i = 0; i < lv_ld; i++)
//		{
//			m_map_ld[ld_tb->Data[i].nd].push_back(ld_tb->Data[i]);
//		}

		lv_bus = bus_tb->GetCount();

		for(i = 0; i < lv_bus; i++)
		{
			m_map_bus[bus_tb->Data[i].nd] = bus_tb->Data[i];
		}

		lv_nd = nd_tb->GetCount();

		for(int index = 0, i = 0; i < lv_nd; i++)
		{
//			cout << "**********" << nd_tb->Data[i].id << "**********" << endl;
			////母线结点不单独生成ZN
			if (m_map_bus.find(nd_tb->Data[i].id) != m_map_bus.end())
				continue;

			////负荷结点不单独生成ZN
			//if (m_map_ld.find(nd_tb->Data[i].id) != m_map_ld.end())
//			if(nd_tb->Data[i].r_ld >= 0)
//				continue;

			if(!bvisited_nd[i])
			{
				index++;
				m_zn_ind.clear();
				m_zn_ld.clear();
				m_zn_sec.clear();
				m_zn_end_ind.clear();

				find(map_so, i, false, false, NULL, true);

				if(m_zn_end_ind.size() <= 0)
				{
					index--;
					continue;
				}

				///////////////////////////////////////////////
				//不建立包含母线在内的区段，后面FDIR_R类BuildZone时会重新建立母线区段
				//rongyiping 2013/12/17
				map<int, CLS_ND>::iterator it;
				for (it = m_zn_end_ind.begin(); it != m_zn_end_ind.end(); it++)
				{
					if (m_map_bus.find(nd_tb->GetDataByPos(it->first)->id) != m_map_bus.end())
					{
						break;
					}
				}

				if(it != m_zn_end_ind.end())
				{
					index--;
					continue;
				}
				///////////////////////////////////////////////////////

				CLS_ZN zn_data;
#if 0
				bzero(&zn_data, sizeof(DMS::ZN));
#if ZN_ND_USE_ARRAY
				for (int cnt = 0; cnt < zn_data.type; cnt++)
				{
					zn_data.inds[cnt] = -1;
				}
#else
				zn_data.iznd = -1;
				zn_data.ind3 = -1;
				zn_data.ind4 = -1;
				zn_data.ind5 = -1;
#endif
				zn_data.r_sec = -1;
				zn_data.r_ld = -1;
				zn_data.r_st = -1;
#endif
				///////////////////////////////////////////////////
				///////////////荣义平添加于20111130为适应超过5个结节的ZN
#if ZN_ND_USE_ARRAY
				//zn_data.type = m_zn_end_ind.size();
				//tianyq 修改于20120910为适应查找刀闸跳过的结点
				zn_data.type = m_zn_ind.size();
				zn_data.id = get_id(const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_ZN")), zn_tb->GetCount() + 1);//zn_data.nds[0].nd_id + index;
				for (int cnt = 0; cnt < zn_data.type; cnt++)
				{
					NODE_DATA data = { m_zn_ind.begin()->second.id, -1, m_zn_end_ind.find(m_zn_ind.begin()->first) != m_zn_end_ind.end() };
					zn_data.nds.push_back(data);
					nd_tb->Find(m_zn_ind.begin()->second.id)->zn_id = zn_data.id;
					m_zn_ind.erase(m_zn_ind.begin());
				}


				sprintf(zn_data.descr, "%ld区段%d", zn_data.nds[0].nd_id, index);
#else
				strcpy(zn_data.nd, m_zn_end_ind.begin()->second.id);
				//zn_data.ind = *m_zn_end_ind.begin();
				m_zn_end_ind.erase(m_zn_end_ind.begin());
				sprintf(zn_data.id, "%.3s_%d", zn_data.nd, index);
				sprintf(zn_data.descr, "%.3s区段%d", zn_data.nd, index);
				zn_data.type++;

				//是否有ZND
				if (m_zn_end_ind.size() > 0)
				{
					strcpy(zn_data.znd, m_zn_end_ind.begin()->second.id);
					//zn_data.iznd = *m_zn_end_ind.begin();
					m_zn_end_ind.erase(m_zn_end_ind.begin());

					zn_data.type++;

					//是否有ND3
					if (m_zn_end_ind.size() > 0)
					{
						strcpy(zn_data.nd3, m_zn_end_ind.begin()->second.id);
						//zn_data.ind3 = *m_zn_end_ind.begin();
						m_zn_end_ind.erase(m_zn_end_ind.begin());

						zn_data.type++;

						//是否有ND4
						if (m_zn_end_ind.size() > 0)
						{
							strcpy(zn_data.nd4, m_zn_end_ind.begin()->second.id);
							//zn_data.ind4 = *m_zn_end_ind.begin();
							m_zn_end_ind.erase(m_zn_end_ind.begin());

							zn_data.type++;

							//是否有ND5
							if (m_zn_end_ind.size() > 0)
							{
								strcpy(zn_data.nd5, m_zn_end_ind.begin()->second.id);
								//zn_data.ind5 = *m_zn_end_ind.begin();
								m_zn_end_ind.erase(m_zn_end_ind.begin());

								zn_data.type++;

								if (m_zn_end_ind.size() > 0)
								{
									TRACE("zn %s has more than 6 nds!!!!!\r\n", zn_data.id);
								}
							}
						}
					}
				}
#endif
				int izn = zn_tb->Insert(zn_data);

				map_zn_ld[zn_data.id] = m_zn_ld;

				for (LST_LD::iterator it = m_zn_ld.begin(); it != m_zn_ld.end(); it++)
				{
					ld_tb->Find(*it)->type = 1;	//LD in ZN

					ld_tb->Find(*it)->zn_id = zn_data.id;
				}

				for (list<CLS_SEC>::iterator it = m_zn_sec.begin(); it != m_zn_sec.end(); it++)
				{
					sec_tb->Find(*it)->zn_id = zn_data.id;
				}
			}
		}

		if(DoValidate() == NMD_FAIL)
		{
			return false;
		}

		break;
	}

	default:
		break;
	}

	delete []bvisited_nd;
	delete []bvisited_zn;

	return true;
}
/*
 * 获取设备的节点
 *
 * id 设备id
 * table 设备类型
 * pnds 返回设备的节点信息
 * cnt_nd 返回的节点个数
 */
int GetDevNDs(const long pid, const char *table, int *pnds, int &cnt_nd)
{
	assert(table != NULL && pnds != NULL);

	long id = pid;

	pid_to_id(id);

	cout << "获取设备" << id << "[" << table << "]对应的节点信息…" << endl;

	DMS::CB cb_data = { 0 };
	DMS::XF xf_data = { 0 };
	DMS::SEC sec_data = { 0 };
	DMS::LD ld_data = { 0 };

	if (strcasecmp(table, "breaker") == 0)
	{
		//获取开关信息
		if (oodbread_rk(&cb_data, const_cast<long*>(&id), const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_CB")), sizeof(DMS::CB)) == OO_FAIL)
		{
			cout << __FUNCTION__ << ":读取表CB记录失败,错误号：" << _oodb_errno << endl;

			return OO_FAIL;
		}

		cnt_nd = 2;

		pnds[0] = cb_data.ind;
		pnds[1] = cb_data.iznd;
	}
	else if (strcasecmp(table, "transformerwinding") == 0)
	{
		if (oodbread_rk(&xf_data, const_cast<long*>(&id), const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_XF")), sizeof(DMS::XF)) == OO_FAIL)
		{
			cout << __FUNCTION__ << ":读取表XF记录失败,错误号：" << _oodb_errno << endl;

			return OO_FAIL;
		}

		cnt_nd = 2;

		pnds[0] = xf_data.ind;
		pnds[1] = xf_data.iznd;
	}
	else if (strcasecmp(table, "aclinesegment") == 0)
	{
		if (oodbread_rk(&sec_data, const_cast<long*>(&id), const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_SEC")), sizeof(DMS::SEC)) == OO_FAIL)
		{
			TRACE("读取表SEC记录[%ld]失败,line:%d, 错误号：%d\r\n", id, __LINE__, _oodb_errno);

			return OO_FAIL;
		}

		cnt_nd = 2;

		pnds[0] = sec_data.ind;
		pnds[1] = sec_data.iznd;
	}
	else if (strcasecmp(table, "energyconsumer") == 0)
	{
		if (oodbread_rk(&ld_data, const_cast<long*>(&id), const_cast<TB_DESCR*>(g_db_dms->GetTB("DMS_LD")), sizeof(DMS::LD)) == OO_FAIL)
		{
			cout << __FUNCTION__ << ":读取表LD记录失败,错误号：" << _oodb_errno << endl;

			return OO_FAIL;
		}

		cnt_nd = 1;

		pnds[0] = ld_data.ind;
	}
	else
	{
		cout << __FUNCTION__ << ":未识别的设备类型：" << table << endl;
		cnt_nd = 0;
	}

	return OO_SUCCEED;
}
