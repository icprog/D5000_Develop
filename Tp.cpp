////////////////////////////////////////////////////////////////////////////////
//
//  TP.CPP
//  DMS project - Copyright (c) 1999, EPRI
//  Methods implementation of TP class
//
////////////////////////////////////////////////////////////////////////////////

#include "Tp.h"
#include<list>
#include<map>
#include<string>
////////////////////////////////////////////////////////////////////////////////
// constructor
TP::TP(DSBOB* m_pDb)
{
	dsbob		= m_pDb;
//	digital_tb	= dsbob->digital_tb;
//	analog_tb	= dsbob->analog_tb;
	co_tb		= dsbob->co_tb;
	st_tb		= dsbob->st_tb;
	xfmr_tb		= dsbob->xfmr_tb;
	xf_tb		= dsbob->xf_tb;
	cb_tb		= dsbob->cb_tb;
	so_tb		= dsbob->so_tb;
	ld_tb		= dsbob->ld_tb;
	cp_tb		= dsbob->cp_tb;
	bus_tb		= dsbob->bus_tb;
	zn_tb		= dsbob->zn_tb;
	sec_tb		= dsbob->sec_tb;
	nd_tb		= dsbob->nd_tb;
	bs_tb		= dsbob->bs_tb;
	island_tb	= dsbob->island_tb;
	global_tb	= dsbob->global_tb;
	tapty_tb	= dsbob->tapty_tb;
	r_co_st_tb	= dsbob->r_co_st_tb;
	r_st_xfmr_tb= dsbob->r_st_xfmr_tb;
	r_xfmr_xf_tb= dsbob->r_xfmr_xf_tb;
	r_st_cb_tb	= dsbob->r_st_cb_tb;
	r_st_so_tb	= dsbob->r_st_so_tb;
	r_st_ld_tb	= dsbob->r_st_ld_tb;
	r_st_cp_tb	= dsbob->r_st_cp_tb;
	r_st_bus_tb = dsbob->r_st_bus_tb;
	r_st_zn_tb	= dsbob->r_st_zn_tb;
	r_zn_ld_tb	= dsbob->r_zn_ld_tb;
	r_zn_sec_tb = dsbob->r_zn_sec_tb;
	r_nd_cb_tb	= dsbob->r_nd_cb_tb;
	r_nd_xf_tb	= dsbob->r_nd_xf_tb;
	r_zn_nd_tb	= dsbob->r_zn_nd_tb;
	r_nd_sec_tb = dsbob->r_nd_sec_tb;
	r_nd_ld_tb = dsbob->r_nd_ld_tb;
	r_nd_cp_tb = dsbob->r_nd_cp_tb;
	r_sec_fi_tb = dsbob->r_sec_fi_tb;
	fi_tb = dsbob->fi_tb;
}

////////////////////////////////////////////////////////////////////////////////
//  destructor
TP::~TP()
{
}

////////////////////////////////////////////////////////////////////////////////
//  tp_allnew()
int	TP::tp_allnew()
{
	//get record count in tables
	if(tp_table_count() == TP_FAIL)
	{
		return TP_FAIL;
	}

	//get records in tables
	if(tp_table_get() == TP_FAIL)
	{
		return TP_FAIL;
	}

	//sort nds into bs
	if(tp_nd_sort() == TP_FAIL)
	{
		return TP_FAIL;
	}

	//sort bs into island
    if(tp_bs_sort() == TP_FAIL)
    {
		return TP_FAIL;
	}

	//determine bs and status of each component
	if(tp_comp_status() == TP_FAIL)
	{
		return TP_FAIL;
	}

	return TP_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  tp_table_count()
int TP::tp_table_count()
{
	lv_st = st_tb->GetCount();
	lv_xfmr = xfmr_tb->GetCount();
	lv_xf = xf_tb->GetCount();
	lv_cb = cb_tb->GetCount();
	lv_so = so_tb->GetCount();
	lv_ld = ld_tb->GetCount();
	lv_cp = cp_tb->GetCount();
	lv_bus = bus_tb->GetCount();
	lv_zn = zn_tb->GetCount();
	lv_sec = sec_tb->GetCount();
	lv_nd = nd_tb->GetCount();
	lv_bs = bs_tb->GetCount();

	return TP_SUCCEED;
}

int TP::tp_table_get_cbchange()
{
	//get nd table
	nd = nd_tb->GetTable();

	//get bs table
	bs = bs_tb->GetTable();

	//get cb table
	cb = cb_tb->GetTable();

	//get global table
	global = global_tb->GetTable();

	//get so table
	so = so_tb->GetTable();

	//get ld table
	ld = ld_tb->GetTable();

	//get cp talbe
	cp = cp_tb->GetTable();

	//get nd table
	nd= nd_tb->GetTable();

	//get sec table
	sec = sec_tb->GetTable();

	//get xf table
	xf = xf_tb->GetTable();

	//get island table
	island = island_tb->GetTable();

	//get r_nd_sec table
	r_nd_sec = r_nd_sec_tb->GetTable();

	//get r_nd_cb table
	r_nd_cb = r_nd_cb_tb->GetTable();

	//get r_nd_xf table
	r_nd_xf = r_nd_xf_tb->GetTable();

	//get r_nd_cp table
	r_nd_cp = r_nd_cp_tb->GetTable();

	//get r_nd_ld table
	r_nd_ld = r_nd_ld_tb->GetTable();

	return TP_SUCCEED;

}

////////////////////////////////////////////////////////////////////////////////
//  tp_table_get()
int TP::tp_table_get()
{
	int		i;

	//get nd table
	nd = nd_tb->GetTable();

	//get bs table
	if(lv_bs != GetTableSize(g_db_dms->GetTB("DMS_BS")))
	{
		bs = new CLS_BS[GetTableSize(g_db_dms->GetTB("DMS_BS"))];
		for(i = 0; i < GetTableSize(g_db_dms->GetTB("DMS_BS")); i++)
		{
			(bs+i)->id = i;
		}
		bs_tb->Empty();
		lv_bs = bs_tb->Insert(bs, GetTableSize(g_db_dms->GetTB("DMS_BS")));
		delete[] bs;
		if(lv_bs != GetTableSize(g_db_dms->GetTB("DMS_BS")))
		{
			printf("fail to insert bs table\n");
			return TP_FAIL;
		}
	}
	bs = bs_tb->GetTable();

	//get cb table
	cb = cb_tb->GetTable();

	//get global table
	global = global_tb->GetTable();

	//get so table
	so = so_tb->GetTable();

	//get nd table
	nd= nd_tb->GetTable();

	return TP_SUCCEED;
}


////////////////////////////////////////////////////////////////////////////////
//  tp_nd_sort()
int TP::tp_nd_sort()
{
	int			i;
	int			knd;
	int			kknd;
	int			ibs;
	int			lastbs;
    int			anyshunt;
    int			loadgen;
    int			notopenbs;
	int			qndgrp;
	ChildPtr	*cb_p, *cb_p0;

	//initialize nd list and mark all nds as unprocessed
	for(i = 0; i < lv_nd; i++)
	{
		(nd+i)->ibs = 0;
		(nd+i)->primebs = 0;  //we don't form primebs in this version
		(nd+i)->q |= ND_UNPROC;
	}

	//initialize bs list
	for(i = 0; i < GetTableSize(g_db_dms->GetTB("DMS_BS")); i++)
	{
		(bs+i)->iisland = -2;
		(bs+i)->q = 0;
		(bs+i)->fbs = i + 1;
		(bs+i)->pnd = -1;
	}

	//initialize the nullbs chain
	i = 0;
	(bs+i)->fbs = i;  //point to itself
	(bs+i)->bbs = i;
	(bs+i)->iisland = -1;
	(bs+i)->q |= BS_DEAD;
	(bs+i)->q |= BS_OPEN;

	//last bs point to -1
	i = GetTableSize(g_db_dms->GetTB("DMS_BS")) - 1;
	(bs+i)->fbs = -1;

	//initialize global pointers to bs list
	global->nullbs = 0;
	global->nextbs = 1;

	//initialize last inserted bs
	lastbs = global->nullbs;

	//sort nds into bs
	for(i = 0; i < lv_nd; i++)
	{
		if((nd+i)->q & ND_UNPROC)  //this nd has not processed
		{
			//get bs number for this nd
			ibs = global->nextbs;
			if(ibs == -1 || ibs == GetTableSize(g_db_dms->GetTB("DMS_BS")) - 1)
			{
				printf("out of memory\n");
				return TP_FAIL;
			}

			//create circular chain for this nd
			knd = i;
			(nd+knd)->fch = knd;
			(nd+knd)->q &= (~ND_UNPROC);
			(nd+knd)->ibs = ibs;

			//initialize the nd group
			qndgrp = 0; //q of the nd group
			g_nbr = 0;	//branch number from the nd group

			while( 1 )
			{
				qndgrp |= (nd+knd)->q;

				//xf
				g_nbr += r_nd_xf_tb->FindChildNum((nd+knd)->r_xf);

				//sec
				g_nbr += r_nd_sec_tb->FindChildNum((nd+knd)->r_sec);

				//go through cbs connected to this nd
				cb_p = r_nd_cb_tb->FindChildPtr((nd+knd)->r_cb);
				cb_p0 = cb_p;
				while(cb_p)
				{
					if(!((cb+cb_p->Ptr)->q & CB_OPEN))  //cb is closed
					{
						//find the opposte nd through this cb
						if(knd == (cb+cb_p->Ptr)->ind)
						{
							kknd = (cb+cb_p->Ptr)->iznd;
						}
						else
						{
							kknd = (cb+cb_p->Ptr)->ind;
						}

						//process this opposite nd
						if((nd+kknd)->q & ND_UNPROC)
						{
							(nd+kknd)->fch = (nd+knd)->fch;
							(nd+knd)->fch = kknd;
							(nd+kknd)->q &= (~ND_UNPROC);
							(nd+kknd)->ibs = ibs;
						}
					}
					cb_p = cb_p->Next;
				}
				FreeChildPtr(cb_p0);

				//get next nd in the present nd group
				knd = (nd+knd)->fch;
				if(knd == i) break;
			}

			//check if the bs is open
			anyshunt = (qndgrp & ND_SO) || (qndgrp & ND_LD) || (qndgrp & ND_CP);
			loadgen = (qndgrp & ND_SO) || (qndgrp & ND_LD);
			notopenbs = (g_nbr > 1) || (g_nbr == 1 && anyshunt) || loadgen;

			//mark the bs if open
			if(notopenbs)
			{
				(bs+ibs)->q &= (~BS_OPEN);

				//loop through the nd group
				knd = i;
				do
				{
					(nd+knd)->q &= (~ND_OPEN);
					knd = (nd+knd)->fch;
				}	while(knd != i);
			}
			else
			{
				(bs+ibs)->q |= BS_OPEN;

				//loop through the nd group
				knd = i;
				do
				{
					(nd+knd)->q |= ND_OPEN;
					knd = (nd+knd)->fch;
				}	while(knd != i);
			}

			//insert bs into circular chain//????
			(bs+ibs)->fbs = (bs+lastbs)->fbs;
			(bs+ibs)->bbs = lastbs;
			(bs+(bs+lastbs)->fbs)->bbs = ibs;
			(bs+lastbs)->fbs = ibs;
			(bs+ibs)->pnd = i;
			(bs+ibs)->valid = 1;

			//move to next bs
			lastbs = global->nextbs++;
		}
	}

	return TP_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  tp_bs_sort()
//  sorts buses into islands by selecting a valid bus as a starting point,
//  finding all its neighbors, selecting each neighbor in turn and finding
//  all its neighbors until no bus in the island is unprocessed. flags are
//  maintained for each island to determine if an island has generation
//  (source) and a voltage reference. if an insland doesn't have, then it
//  is determined to be non-viable and all buses in the island will be
//  marked as dead and be assigned to the -1 island.
int TP::tp_bs_sort()
{///////////////////////////////////////////////////////////////////////////////

	int		i, j;
	int		ind;
	int		ibs;
	int		startbs;
	int		desiredisland;
	int		neighbor;
	int		kf;
	int		kb;
	int		knd0;
	int		knd;
	int		*fch_bs;
	int		*bch_bs;

	//initialize island table
	island_tb->Empty();

	//initialize the unprocessed bs chained
	fch_bs = new int[GetTableSize(g_db_dms->GetTB("DMS_BS"))];
	bch_bs = new int[GetTableSize(g_db_dms->GetTB("DMS_BS"))];
	for(i = 0; i < GetTableSize(g_db_dms->GetTB("DMS_BS")); i++)
	{
		*(fch_bs+i) = (bs+i)->fbs;
		*(bch_bs+i) = (bs+i)->bbs;
	}

	(bs+global->nullbs)->iisland = -1;

	//start an island from an unprocessed so bs
	for(i = 0; i < lv_so; i++)
	{
		//get this so bs
		ind = (so+i)->ind;
		ibs = (nd+ind)->ibs;

		//check if this so bs has been processed
		if(*(bch_bs+ibs) == -1)
		{
			////////////////////////////////////////
			//荣义平添加于2012.5.17，适用于一个岛上多个电源
			(so+i)->iisland = (bs+ibs)->iisland;
			////////////////////////////////////////
			continue;
		}

		//check if this so bs is open
		if((bs+ibs)->q & BS_OPEN)
		{
			continue;
		}

		//select this so bs as start bs
		startbs = ibs;

		//get island number to be created
		desiredisland = island_tb->GetCount();

		//delete start bs from unprocessed chain
		*(fch_bs+*(bch_bs+startbs)) = *(fch_bs+startbs);
		*(bch_bs+*(fch_bs+startbs)) = *(bch_bs+startbs);

        //initialize the current process chain, note that although
        //the unprocessed list is chained forward and backward,
        //the current process is chained forward only

		*(fch_bs+startbs) = startbs;
		*(bch_bs+startbs) = -1;
		g_bs = startbs;

		//find the neighbours of bs
		do
		{
			if(tp_bs_neighbor() == TP_FAIL)
			{
				printf("tp_bs_neighbor failed!\r\n");
				delete[] fch_bs;
				delete[] bch_bs;
				return TP_FAIL;
			}

			//loop on neighors of bs
			for(j = 0; j < g_nbr; j++)
			{
				neighbor = g_z_br[j];

				//if neighbor is unprocessed, remove it from the
                //unprocessed chain and add it to current process
				if(*(bch_bs+neighbor) >= 0)
				{
                    //the neighbor bs is not processed yet,
                    //remove from unprocessd chain
                    kf = *(fch_bs+neighbor);
                    kb = *(bch_bs+neighbor);
                    *(bch_bs+kf) = kb;
                    *(fch_bs+kb) = kf;

                    //add to the current process chain
                    kf = *(fch_bs+g_bs);
                    *(fch_bs+g_bs) = neighbor;
                    *(fch_bs+neighbor) = kf;
                    *(bch_bs+neighbor) = -1;  //mark as processed
                }
			}

			//get next bs in current process
			g_bs = *(fch_bs+g_bs);

		}   while(g_bs != startbs);

		//an viable island sort is now complete
		if(island_tb->GetCount() < island_tb->GetSize())
		{
			//create a valid island
			island = new CLS_ISLAND(desiredisland);
			island->valid = 1;
			island_tb->Insert(*island);
			//荣义平2012.05.17添加////////
			(so+i)->iisland = desiredisland;
			////////////////////////////

            //assign all buses in island to this island,
            //check whether any assignments are new.
            g_bs = startbs;
            do
            {
				//set status of this bs
				(bs+g_bs)->q &= (~BS_DEAD);
				(bs+g_bs)->iisland = desiredisland;

				//set status of nds of this bs
				knd0 = (bs+g_bs)->pnd;
				knd = knd0;
				do
				{
					(nd+knd)->q &= (~ND_DEAD);
					knd = (nd+knd)->fch;
				}   while(knd != knd0);

				g_bs = *(fch_bs+g_bs);

			}   while(g_bs != startbs);

			delete island;
		}
		else
		{
			//no more room for islands
			delete[] fch_bs;
			delete[] bch_bs;
			return TP_FAIL;
		}
	}

	//set all unprocessed bs as dead bs
	g_bs = *(fch_bs+global->nullbs);
	while(g_bs != global->nullbs)
	{
		//set status of this bs
		(bs+g_bs)->q |= BS_DEAD;
		(bs+g_bs)->iisland = -1;

		//set status of nds of this bs
		knd0 = (bs+g_bs)->pnd;
		knd = knd0;
		do
		{
			(nd+knd)->q |= ND_DEAD;
			knd = (nd+knd)->fch;
		}   while(knd != knd0);

		g_bs = *(fch_bs+g_bs);
	}

	delete[] fch_bs;
	delete[] bch_bs;

	return TP_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  tp_bs_neighbor()
int TP::tp_bs_neighbor()
{
	int			br;
	int			knd;
	int			knd0;
	int			znd;
	ChildPtr	*br_p, *br_p0;

	g_nbr = 0;  //number of branch in island

	knd0 = (bs+g_bs)->pnd;
	knd = knd0;
	do
	{
		//判断是否有线段与该结点相连
		//sec
		if((nd+knd)->r_sec != -1)
		{
			br_p = r_nd_sec_tb->FindChildPtr((nd+knd)->r_sec);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				sec = sec_tb->GetDataByPos(br);
				if(knd == sec->ind)
				{
					znd = sec->iznd;
				}
				else
				{
					znd = sec->ind;
				}
				g_z_br[g_nbr++] = (nd+znd)->ibs;
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);
		}

		//xf
		if((nd+knd)->r_xf != -1)
		{
			br_p = r_nd_xf_tb->FindChildPtr((nd+knd)->r_xf);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				xf = xf_tb->GetDataByPos(br);
				if(knd == xf->ind)
				{
					znd = xf->iznd;
				}
				else
				{
					znd = xf->ind;
				}
				g_z_br[g_nbr++] = (nd+znd)->ibs;
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);
		}

		//move to next nd of this bs
		knd = (nd+knd)->fch;

	}	while(knd != knd0);

	if(g_nbr > g_mxnbr)
	{
		printf("分支%d大于最大分支数%d\r\n", g_nbr, g_mxnbr);
		return TP_FAIL;
	}

	return TP_SUCCEED;
}

////////////////////////////////////////////////////////////////////////////////
//  tp_comp_status()
//  set status of each component by topology processing results
int TP::tp_comp_status()
{//////////////////////////////////////////////////////////////////////////////

	int		i;
	int		ind;
	int		znd;
	int     ind3;
	int     ind4;
	int     ind5;
	int		ibs;
	int		zbs;
	int		ibs3;
	int     ibs4;
	int     ibs5;
	int     len;
	int		kxfmr;

	//bus
	for(i = 0; i < lv_bus; i++)
	{
		bus = bus_tb->GetDataByPos(i);
		ind = bus->ind;
		ibs = (nd+ind)->ibs;
		if((bs+ibs)->iisland < 0)
		{
			bus->topflags |= BUS_DEAD;
		}
		else
		{
			bus->topflags &= (~BUS_DEAD);
		}
		bus->ibs = ibs;
	}

	//so
	for(i = 0; i < lv_so; i++)
	{
		so = so_tb->GetDataByPos(i);
		ind = so->ind;
		ibs = (nd+ind)->ibs;
		if(((bs+ibs)->q & BS_OPEN) || (so->q & SO_REMOVE))
		{
			so->q |= SO_OPEN;
		}
		else
		{
			so->q &= (~SO_OPEN);
		}
		if(((bs+ibs)->iisland < 0) || (so->q & SO_REMOVE))
		{
			so->topflags |= SO_DEAD;
		}
		else
		{
			so->topflags &= (~SO_DEAD);
		}
		so->ibs = ibs;
	}

	//ld
	for(i = 0; i < lv_ld; i++)
	{
		ld = ld_tb->GetDataByPos(i);
		ind = ld->ind;
		ibs = (nd+ind)->ibs;
		if(((bs+ibs)->q & BS_OPEN) || (ld->q & LD_REMOVE))
		{
			ld->q |= LD_OPEN;
		}
		else
		{
			ld->q &= (~LD_OPEN);
		}
		if(((bs+ibs)->iisland < 0) || (ld->q & LD_REMOVE))
		{
			ld->topflags |= LD_DEAD;
		}
		else
		{
			ld->topflags &= (~LD_DEAD);
		}
		ld->ibs = ibs;
	}

	//cp
	for(i = 0; i < lv_cp; i++)
	{
		cp = cp_tb->GetDataByPos(i);
		ind = cp->ind;
		ibs = (nd+ind)->ibs;
		if(((bs+ibs)->q & BS_OPEN) || (cp->q & LD_REMOVE))
		{
			cp->q |= CP_OPEN;
		}
		else
		{
			cp->q &= (~CP_OPEN);
		}
		if(((bs+ibs)->iisland < 0) || (cp->q & CP_REMOVE))
		{
			cp->topflags |= CP_DEAD;
		}
		else
		{
			cp->topflags &= (~CP_DEAD);
		}
		cp->ibs = ibs;
	}

	//zn
	for(i = 0; i < lv_zn; i++)
	{
		zn = zn_tb->GetDataByPos(i);

		///////////////////////////////////////////////////
		///////////////荣义平添加于20111130为适应超过5个结节的ZN
#if ZN_ND_USE_ARRAY
		int edge_num = 0;
		for(int j = 0; j < zn->type; j++)
		{
			if(zn->nds[j].is_edge)
			{
				edge_num++;
			}
		}
		if (edge_num == 1)
		{
			for(int j = 0; j < zn->type; j++)
			{
				if(zn->nds[j].is_edge)
				{
//					if (((bs + (nd + zn->inds[j])->ibs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
//					{
//						zn->topflags |= ZN_IOPEN;
//					}
//					else
//					{
//						zn->topflags &= (~ZN_IOPEN);
//					}

//					if ((bs + (nd + zn->inds[j])->ibs)->iisland < 0)
//					{
//						zn->topflags |= ZN_DEAD;
//					}
//					else
//					{
//						zn->topflags &= (~ZN_DEAD);
//					}
//
//					zn->ibss[j] = (nd + zn->inds[j])->ibs;

					break;
				}
			}

		}
		else if(edge_num > 1)
		{
//			for(int j = 0; j < zn->type; j++)
//			{
//				if(zn->nds[j].is_edge)
//				{
//					if (((bs + (nd + zn->inds[j])->ibs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
//					{
//						zn->topflags |= ZN_IOPEN;
//						break;
//					}
//					else
//					{
//						zn->topflags &= (~ZN_IOPEN);
//					}
//				}
//			}

//			int index = 0;
//			for (index = 0; index < zn->type; index++)
//			{
//				if ((bs + (nd + zn->inds[index])->ibs)->iisland >= 0)
//				{
//					zn->topflags &= (~ZN_DEAD);
//					break;
//				}
//			}
//
//			if (index >= zn->type)
//				zn->topflags |= ZN_DEAD;

//			for (index = 0; index < zn->type; index++)
//				zn->ibss[index] = (nd + zn->inds[index])->ibs;
		}
#else
		if(zn->type == 1)  //one terminal
		{
			ind = zn->ind;
			ibs = (nd+ind)->ibs;
			if(((bs+ibs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_IOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_IOPEN);
			}
			if((bs+ibs)->iisland < 0)
			{
				zn->topflags |= ZN_DEAD;
			}
			else
			{
				zn->topflags &= (~ZN_DEAD);
			}
			zn->ibs = ibs;
		}
		else if(zn->type == 2) //two terminals
		{
			ind = zn->ind;
			znd = zn->iznd;
			ibs = (nd+ind)->ibs;
			zbs = (nd+znd)->ibs;
			if(((bs+ibs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_IOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_IOPEN);
			}
			if(((bs+zbs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_ZOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_ZOPEN);
			}
			if((bs+ibs)->iisland < 0 && (bs+zbs)->iisland < 0)
			{
				zn->topflags |= ZN_DEAD;
			}
			else
			{
				zn->topflags &= (~ZN_DEAD);
			}
			zn->ibs = ibs;
			zn->zbs = zbs;
		}
		else if(zn->type == 3) //two terminals
		{
			ind = zn->ind;
			znd = zn->iznd;
			ind3 = zn->ind3;
			ibs = (nd+ind)->ibs;
			zbs = (nd+znd)->ibs;
			ibs3 = (nd+ind3)->ibs;
			if(((bs+ibs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_IOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_IOPEN);
			}
			if(((bs+zbs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_ZOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_ZOPEN);
			}
			if((bs+ibs)->iisland < 0 && (bs+zbs)->iisland < 0 && (bs+ibs3)->iisland < 0)
			{
				zn->topflags |= ZN_DEAD;
			}
			else
			{
				zn->topflags &= (~ZN_DEAD);
			}
			zn->ibs = ibs;
			zn->zbs = zbs;
			zn->ibs3 = ibs3;
		}
		else if(zn->type == 4) //two terminals
		{
			ind = zn->ind;
			znd = zn->iznd;
			ind3 = zn->ind3;
			ind4 = zn->ind4;
			ibs = (nd+ind)->ibs;
			zbs = (nd+znd)->ibs;
			ibs3 = (nd+ind3)->ibs;
			ibs4 = (nd+ind4)->ibs;
			if(((bs+ibs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_IOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_IOPEN);
			}
			if(((bs+zbs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_ZOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_ZOPEN);
			}
			if((bs+ibs)->iisland < 0 && (bs+zbs)->iisland < 0 && (bs+ibs3)->iisland < 0 &&
				(bs+ibs4)->iisland < 0)
			{
				zn->topflags |= ZN_DEAD;
			}
			else
			{
				zn->topflags &= (~ZN_DEAD);
			}
			zn->ibs = ibs;
			zn->zbs = zbs;
			zn->ibs3 = ibs3;
			zn->ibs4 = ibs4;
		}
		else if(zn->type == 5) //two terminals
		{
			ind = zn->ind;
			znd = zn->iznd;
			ind3 = zn->ind3;
			ind4 = zn->ind4;
			ind5 = zn->ind5;
			ibs = (nd+ind)->ibs;
			zbs = (nd+znd)->ibs;
			ibs3 = (nd+ind3)->ibs;
			ibs4 = (nd+ind4)->ibs;
			ibs5 = (nd+ind5)->ibs;
			ibs5 = (nd+ind5)->ibs;
			if(((bs+ibs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_IOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_IOPEN);
			}
			if(((bs+zbs)->q & BS_OPEN) || (zn->q & ZN_REMOVE))
			{
				zn->topflags |= ZN_ZOPEN;
			}
			else
			{
				zn->topflags &= (~ZN_ZOPEN);
			}
			if((bs+ibs)->iisland < 0 && (bs+zbs)->iisland < 0 && (bs+ibs3)->iisland < 0 &&
				(bs+ibs4)->iisland < 0 && (bs+ibs5)->iisland < 0)
			{
				zn->topflags |= ZN_DEAD;
			}
			else
			{
				zn->topflags &= (~ZN_DEAD);
			}
			zn->ibs = ibs;
			zn->zbs = zbs;
			zn->ibs3 = ibs3;
			zn->ibs4 = ibs4;
			zn->ibs5 = ibs5;
		}
#endif
	}

	//xf
	for(i = 0; i < lv_xf; i++)
	{
		xf = xf_tb->GetDataByPos(i);
		ind = xf->ind;
		znd = xf->iznd;
		ibs = (nd+ind)->ibs;
		zbs = (nd+znd)->ibs;
		if(((bs+ibs)->q & BS_OPEN) || (xf->q & XF_REMOVE))
		{
			xf->topflags |= XF_IOPEN;
		}
		else
		{
			xf->topflags &= (~XF_IOPEN);
		}
		if(((bs+zbs)->q & BS_OPEN) || (xf->q & XF_REMOVE))
		{
			xf->topflags |= XF_ZOPEN;
		}
		else
		{
			xf->topflags &= (~XF_ZOPEN);
		}
		if((bs+ibs)->iisland < 0 && (bs+zbs)->iisland < 0)
		{
			xf->topflags |= XF_DEAD;
		}
		else
		{
			xf->topflags &= (~XF_DEAD);
		}
		xf->ibs = ibs;
		xf->zbs = zbs;

		//set status of xfmr
		kxfmr = r_xfmr_xf_tb->FindParentPtr(xf->r_xfmr);
		if(kxfmr == -1)
		{
			return TP_FAIL;
		}
		xfmr = xfmr_tb->GetDataByPos(kxfmr);
		if(xfmr->type == 2)
		{
			if(xf->topflags & XF_IOPEN)
			{
				xfmr->topflags |= XFMR_HOPEN;
			}
			else
			{
				xfmr->topflags &= (~XFMR_HOPEN);
			}
			if(xf->topflags & XF_ZOPEN)
			{
				xfmr->topflags |= XFMR_LOPEN;
			}
			else
			{
				xfmr->topflags &= (~XFMR_LOPEN);
			}
			if(xf->topflags & XF_DEAD)
			{
				xfmr->topflags |= XFMR_DEAD;
			}
			else
			{
				xfmr->topflags &= (~XFMR_DEAD);
			}
		}
		else if(xfmr->type == 3)
		{
			//D5000 ID为自动生成的LONG型数据
//			len = strlen(xf->id);
//
//			if(xf->topflags & XF_IOPEN)
//			{
//				if(xf->id[len-1] == 'h')
//				{
//					xfmr->topflags |= XFMR_HOPEN;
//				}
//				if(xf->id[len-1] == 'm')
//				{
//					xfmr->topflags |= XFMR_MOPEN;
//				}
//				if(xf->id[len-1] == 'l')
//				{
//					xfmr->topflags |= XFMR_LOPEN;
//				}
//			}
//			else
//			{
//				if(xf->id[len-1] == 'h')
//				{
//					xfmr->topflags &= (~XFMR_HOPEN);
//				}
//				if(xf->id[len-1] == 'm')
//				{
//					xfmr->topflags &= (~XFMR_MOPEN);
//				}
//				if(xf->id[len-1] == 'l')
//				{
//					xfmr->topflags &= (~XFMR_LOPEN);
//				}
//			}
//			if(xf->topflags & XF_DEAD)
//			{
//				xfmr->topflags |= XFMR_DEAD;
//			}
//			else
//			{
//				xfmr->topflags &= (~XFMR_DEAD);
//			}
		}
	}

	//sec
	for(i = 0; i < lv_sec; i++)
	{
		sec = sec_tb->GetDataByPos(i);
		ind = sec->ind;
		znd = sec->iznd;
		ibs = (nd+ind)->ibs;
		zbs = (nd+znd)->ibs;
		if(((bs+ibs)->q & BS_OPEN) || (sec->q & SEC_REMOVE))
		{
			sec->topflags |= SEC_IOPEN;
		}
		else
		{
			sec->topflags &= (~SEC_IOPEN);
		}
		if(((bs+zbs)->q & BS_OPEN) || (sec->q & SEC_REMOVE))
		{
			sec->topflags |= SEC_ZOPEN;
		}
		else
		{
			sec->topflags &= (~SEC_ZOPEN);
		}
		if((bs+ibs)->iisland < 0 && (bs+zbs)->iisland < 0)
		{
			sec->topflags |= SEC_DEAD;
		}
		else
		{
			sec->topflags &= (~SEC_DEAD);
		}
		sec->ibs = ibs;
		sec->zbs = zbs;
	}

	return TP_SUCCEED;
}

/*
 * 某个开关状态改变，重新计算这个开关关联的计算母线和岛
 */
int TP::tp_cb_changed(const long id)
{
	CLS_CB* tmp_cb = dsbob->cb_tb->Find(CLS_CB(id));

	LIST_DATA* temp_node = new LIST_DATA;
	list<LIST_DATA>::iterator vi;//向量的游标
	map<int, int> somap;//保存电源和计算母线的对应关系
	map<int, int>::iterator ptr;
	map<int, int>::iterator ptr1;
	int island_nd, island_znd, ibs_nd, ibs_znd, is_island;
	int i, key, tmp_bs, change_bs;
	std::vector<bool> visit;
	visit.assign(GetTableSize(g_db_dms->GetTB("DMS_BS")), false);
//	bool visit[GetTableSize(g_db_dms->GetTB("DMS_BS"))] = {0};

	if (tmp_cb == 0)
	{
		cout << __FUNCTION__ << "未找到开关：" << id << endl;
		return TP_FAIL;
	}

	//get records in tables
	if(tp_table_get_cbchange() == TP_FAIL)
	{
		return TP_FAIL;
	}
	printf("cb_change !!!!!!!!!!!!!tmp_cb->id %ld\n",tmp_cb->id);
	ibs_nd = (nd + tmp_cb->ind)->ibs;
	ibs_znd = (nd + tmp_cb->iznd)->ibs;
	island_nd = (bs + ibs_nd)->iisland;
	island_znd = (bs + ibs_znd)->iisland;

	if(ibs_nd == -1 || ibs_znd == -1)
	{
		cout << __FUNCTION__ << "结点的bs错误" << endl;
		return TP_FAIL;
	}

	if (tmp_cb->q & CB_OPEN)
	{
		if (ibs_nd == ibs_znd) //开关是由闭合到断开
		{
			//在开关的结点增加一条计算母线
			change_bs = add_or_delete_BS(ibs_nd, tmp_cb->ind, tmp_cb->iznd, 1);
			if(change_bs == -1)
			{
				return TP_FAIL;
			}
			printf("开关是由闭合到断开!!!!!!!old bs is %d,add bs is %d\n",ibs_nd,change_bs);
			printf(" (nd + tmp_cb->ind)->ibs is %d,(nd + tmp_cb->iznd)->ibs %d\n",(nd + tmp_cb->ind)->ibs,(nd + tmp_cb->iznd)->ibs);
			printf("the island is %d\n",island_nd);
			lv_so = so_tb->GetCount();
			for(i = 0; i < lv_so; i++)
			{
				key = (so + i)->ibs;
				somap.insert(map<int, int>::value_type(key, i));
			}
			if (island_nd != -1)//开关在岛上
			{
				ptr = somap.find(ibs_nd);
				if(ptr != somap.end())//在开关所在的计算母线上存在电源
				{
					if((so + (*ptr).second)->ind == tmp_cb->ind)
					{
						pbss.clear();
						find_BS((nd + tmp_cb->iznd)->ibs, visit);
					}
					else
					{
						pbss.clear();
						find_BS((nd + tmp_cb->ind)->ibs, visit);
					}

					is_island = 0;
					for(vi = pbss.begin(); vi != pbss.end(); vi++)
					{
						ptr1 = somap.find((*vi).pos);
						if(ptr1 == somap.end())//没有找到电源
						{
							continue;
						}
						else//这些bs在岛上，有电源
						{
							if((*vi).pos == ibs_nd)
							{
								(so + (*ptr).second)->ibs = change_bs;
								continue;
							}
							if(is_island == 0)
							{
								if(invalid_island.empty())//如果无效岛为空，需要创建一个新岛
								{
									island_znd = island_tb->GetCount();
									(island + island_znd)->valid = 1;
									(island + island_znd)->id = island_znd;
									island_tb->Insert(*(island + island_znd));
								}
								else
								{
									temp_node->pos = (invalid_island.back().pos);
									island_znd = temp_node->pos;
									invalid_island.pop_back();
									(island + island_znd)->valid = 1;
								}
							}
							(so + (*ptr1).second)->iisland = island_znd;
							printf("so %d is change %d\n",(*ptr1).second, island_znd);
							is_island = 1;
						}
					}
					if(is_island == 0)
					{
						//printf("开关的另一侧不存在电源\n");
						for(vi = pbss.begin(); vi != pbss.end(); vi++)
						{
							(bs + (*vi).pos)->iisland = -1;
						}
					}
					else
					{
						for(vi = pbss.begin(); vi != pbss.end(); vi++)
						{
							(bs + (*vi).pos)->iisland = island_znd;
						}
					}
				}
				else
				{
					is_island = 0;
					pbss.clear();
					find_BS(ibs_nd, visit);
					for(vi = pbss.begin(); vi != pbss.end(); vi++)
					{
						ptr = somap.find((*vi).pos);
						if(ptr == somap.end())//没有找到电源
						{
							continue;
						}
						else//这些bs在岛上，有电源
						{
							is_island = 1;
							break;
						}
					}
					if(is_island == 0)//电源不在开关的首端，那开关的末端一定存在电源
					{
						for(vi = pbss.begin(); vi != pbss.end(); vi++)
						{
							(bs + (*vi).pos)->iisland = -1;
						}
					}
					else//在开关首端这侧存在电源
					{
						for(int num = 0; num < GetTableSize(g_db_dms->GetTB("DMS_BS")); num++)
						{
							visit[num] = false;
						}
						pbss.clear();
						find_BS(change_bs, visit);

						is_island = 0;
						for(vi = pbss.begin(); vi != pbss.end(); vi++)
						{
							ptr1 = somap.find((*vi).pos);
							if(ptr1 == somap.end())//没有找到电源
							{
								continue;
							}
							else//这些bs在岛上，有电源
							{
								if(is_island == 0)
								{
									if(invalid_island.empty())//如果无效岛为空，需要创建一个新岛
									{
										island_znd = island_tb->GetCount();
										(island + island_znd)->valid = 1;
										(island + island_znd)->id = island_znd;
										island_tb->Insert(*(island + island_znd));
									}
									else
									{
										temp_node->pos = (invalid_island.back().pos);
										island_znd = temp_node->pos;
										invalid_island.pop_back();
										(island + island_znd)->valid = 1;
									}
								}
								(so + (*ptr1).second)->iisland = island_znd;
								printf("so %d is change %d\n",(*ptr1).second, island_znd);
								is_island = 1;
							}
						}
						if(is_island == 0)//不需要添加新的岛
						{
							for(vi = pbss.begin(); vi != pbss.end(); vi++)
							{
								(bs + (*vi).pos)->iisland = -1;
							}
						}
						else//开关断开后两端都是在岛上，需要添加新的岛
						{
							for(vi = pbss.begin(); vi != pbss.end(); vi++)
							{
								(bs + (*vi).pos)->iisland = island_znd;
							}
						}
					}
				}
			}
			printf(" ind iisland is %d,iznd iisland is %d\n",(bs+(nd + tmp_cb->ind)->ibs)->iisland,(bs+(nd + tmp_cb->iznd)->ibs)->iisland);
			delete temp_node;
			somap.clear();
			return TP_SUCCEED;
		}
		else //开关本来是打开状态，无变化
		{
			delete temp_node;
			return TP_SUCCEED;
		}
	}
	else
	{
		if (ibs_nd == ibs_znd) ////开关本来是合闸状态，无变化
		{
			delete temp_node;
			return TP_SUCCEED;
		}
		else //开关由断开到闭合
		{
			printf("开关由断开到闭合!!!!!!!\n");
			printf(" (nd + tmp_cb->ind)->ibs is %d,(nd + tmp_cb->iznd)->ibs %d\n",ibs_nd,ibs_znd);
			printf("the island_nd is %d,the island_znd is %d\n",island_nd,island_znd);
			if(island_nd == -1 && island_znd != -1)//只有一端在岛上
			{
				pbss.clear();
				find_BS(ibs_nd, visit);
				for(vi = pbss.begin(); vi != pbss.end(); vi++)
				{
					tmp_bs = (*vi).pos;
					(bs +tmp_bs)->iisland = island_znd;
				}

				change_bs = add_or_delete_BS(ibs_nd, tmp_cb->ind, tmp_cb->iznd, 0);
				if(change_bs == -1)
				{
					return TP_FAIL;
				}
				lv_so = so_tb->GetCount();
				for(i = 0; i < lv_so; i++)
				{
					key = (so + i)->ibs;
					somap.insert(map<int, int>::value_type(key, i));
				}
				ptr = somap.find(change_bs);
				if(ptr != somap.end())
				{
					if(change_bs == ibs_znd)
					{
						(so + (*ptr).second)->ibs = ibs_nd;
					}
					else
					{
						(so + (*ptr).second)->ibs = ibs_znd;
					}
				}
				pbss.clear();
				somap.clear();
			}
			if(island_nd != -1 && island_znd == -1)//只有一端在岛上
			{
				//printf("2只有一端在岛上\n");
				pbss.clear();
				find_BS(ibs_znd, visit);
				for(vi = pbss.begin(); vi != pbss.end(); vi++)
				{
					tmp_bs = (*vi).pos;
					(bs +tmp_bs)->iisland = island_nd;
				}

				change_bs = add_or_delete_BS(ibs_nd, tmp_cb->ind, tmp_cb->iznd, 0);
				if(change_bs == -1)
				{
					return TP_FAIL;
				}
				//printf("the delete bs is %d\n",change_bs);
				lv_so = so_tb->GetCount();
				for(i = 0; i < lv_so; i++)
				{
					key = (so + i)->ibs;
					somap.insert(map<int, int>::value_type(key, i));
				}
				ptr = somap.find(change_bs);
				if(ptr != somap.end())
				{
					if(change_bs == ibs_nd)
					{
						(so + (*ptr).second)->ibs = ibs_znd;
					}
					else
					{
						(so + (*ptr).second)->ibs = ibs_nd;
					}
				}

				somap.clear();
				pbss.clear();
			}
			if(island_nd != -1 && island_znd != -1)//两端都在岛上合并为一个岛
			{
				//printf("两端都在岛上合并为一个岛\n");
				pbss.clear();
				find_BS(ibs_nd, visit);
				lv_so = so_tb->GetCount();
				for(i = 0; i < lv_so; i++)
				{
					key = (so + i)->ibs;
					somap.insert(map<int, int>::value_type(key, i));
				}

				for(vi = pbss.begin(); vi != pbss.end(); vi++)
				{
					tmp_bs = (*vi).pos;
					(bs +tmp_bs)->iisland = island_znd;
					ptr = somap.find((*vi).pos);
					if(ptr != somap.end())
					{
						(so + (*ptr).second)->iisland = island_znd;
					}
				}

				(island + island_nd)->valid = 0;
				temp_node->pos = island_nd;
				invalid_island.push_back(*temp_node);

				change_bs = add_or_delete_BS(ibs_nd, tmp_cb->ind, tmp_cb->iznd, 0);
				if(change_bs == -1)
				{
					return TP_FAIL;
				}

				ptr = somap.find(change_bs);
				if(ptr != somap.end())
				{
					if(change_bs == ibs_nd)
					{
						(so + (*ptr).second)->ibs = ibs_znd;
					}
					else
					{
						(so + (*ptr).second)->ibs = ibs_nd;
					}
				}

				pbss.clear();
				somap.clear();
			}
			if(island_nd == -1 && island_znd == -1)//开关两端都不在岛上
			{
				//printf("开关两端都不在岛上\n");
				change_bs = add_or_delete_BS(ibs_nd, tmp_cb->ind, tmp_cb->iznd, 0);
				if(change_bs == -1)
				{
					return TP_FAIL;
				}
			}
			//for test
			ibs_nd = (nd + tmp_cb->ind)->ibs;
			ibs_znd = (nd + (tmp_cb->iznd))->ibs;
			island_nd = (bs + (nd + (tmp_cb->ind))->ibs)->iisland;
			island_znd = (bs + (nd + (tmp_cb->iznd))->ibs)->iisland;
			printf("nd bs is %d,znd bs is %d\n",ibs_nd,ibs_znd);
			printf("the island_nd is %d,the island_znd is %d\n",island_nd,island_znd);
			delete temp_node;
			return TP_SUCCEED;
		}
	}
}
//计算母线变化时遍历同一条计算母线上的结点
void TP::find_DFS(int ind, std::vector<bool> &bvisited)
{

	assert(ind != -1);
	if(bvisited[ind])
	{
		//printf("the nd have be visited\n");
		return;
	}
	LIST_DATA* tempnd = new LIST_DATA;

	tempnd->pos = ind;
	//printf("pnds push_back is %d\n",ind);
	pnds.push_back(*tempnd);
	bvisited[ind] = true;
	if(pnds.empty() == 1)
	{
		return;
	}
	ind = dsbob->nd_tb->GetDataByPos(tempnd->pos)->fch;
	find_DFS(ind,bvisited);
	delete tempnd;

}

/*注意：调用此函数时会把sec和xf的全局值改变
 * 所以在调用这个函数后如果还需要用sec和xf时需重新赋值
 */
//遍历与一个结点想连通的所有计算母线
void TP::find_BS(int sbs, std::vector<bool> &bvisited)
{
	int 	i, j;
	int		neighbor;
	int		kf;
	int		kb;
	int		knd0;
	int		knd;
	int		znd;
	int		br;
	int		count;
	int 	g_br[g_mxnbr];
	ChildPtr	*br_p, *br_p0;
	CLS_SEC*	temp_sec;
	CLS_XF*		temp_xf;
	LIST_DATA* temp=NULL;
	temp = new LIST_DATA;
	assert(sbs != -1);
	//find the neighbours of bs
	knd0 = (bs + sbs)->pnd;
	knd = knd0;
	count = 0;
	do
	{
		//判断是否有线段与该结点相连
		//sec
		if((nd+knd)->r_sec != -1)
		{
			br_p = r_nd_sec_tb->FindChildPtr((nd+knd)->r_sec);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				temp_sec = sec_tb->GetDataByPos(br);
				if(knd == temp_sec->ind)
				{
					znd = temp_sec->iznd;
				}
				else
				{
					znd = temp_sec->ind;
				}
				g_br[count++] = (nd+znd)->ibs;
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);
		}

		//xf
		if((nd+knd)->r_xf != -1)
		{
			br_p = r_nd_xf_tb->FindChildPtr((nd+knd)->r_xf);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				temp_xf = xf_tb->GetDataByPos(br);
				if(knd == temp_xf->ind)
				{
					znd = temp_xf->iznd;
				}
				else
				{
					znd = temp_xf->ind;
				}
				g_br[count++] = (nd+znd)->ibs;
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);
		}

		//move to next nd of this bs
		knd = (nd+knd)->fch;

	}	while(knd != knd0);


	//loop on neighors of bs
	for(j = 0; j < count; j++)
	{
		neighbor = g_br[j];
		//if bs have not visited
		if(bvisited[neighbor] == false)
		{
			bvisited[neighbor] = true;
            memset(temp, 0, sizeof(LIST_DATA));
            temp->pos = neighbor;
            pbss.push_back(*temp);
            find_BS(neighbor, bvisited);

        }
	}

	delete temp;
	return;
}

//开关断开后增加一条计算母线,计算母线发生变化后此函数只处理了与变化结点相连的xf、sec、ld
int TP::add_or_delete_BS(int ibs, int ind, int iznd, int isadd)
{
	int i, br, key;
	int ibs_nd, ibs_znd, pnd, knd;
	LIST_DATA* tmp_node = new LIST_DATA;
	ChildPtr	*br_p, *br_p0;
	std::vector<bool> bvisited;
	bvisited.assign(GetTableSize(g_db_dms->GetTB("DMS_ND")), false);
//	bool bvisited[GetTableSize(g_db_dms->GetTB("DMS_ND"))] = {0};//遍历计算母线上结点是标示是否遍历
	list<LIST_DATA>::iterator vi;//向量的游标

	assert(ibs != -1);
	assert(ind != -1);
	assert(iznd != -1);

	if(isadd)//add bs
	{
		ibs_nd = ibs;

		i = (bs + ibs_nd)->pnd;
		if(i == ind || i == iznd)
		{
			if(i == ind)
			{
				bvisited[ind] = true;
				(nd + i)->fch = i;
				pnd = iznd;
			}
			else
			{
				(nd + i)->fch = i;
				bvisited[iznd] = true;
				pnd = ind;
			}
		}
		else
		{
			bvisited[ind] = true;
			bvisited[iznd] = true;
			pnds.clear();
			find_DFS(i, bvisited);
			if((nd + (pnds.back()).pos)->fch == ind)
			{
				(nd + (pnds.back()).pos)->fch = i;
				pnd = iznd;
			}
			else
			{
				(nd + (pnds.back()).pos)->fch = i;
				pnd = ind;
			}

		}
		pnds.clear();
		bvisited[pnd] = false;
		find_DFS(pnd, bvisited);
		(nd + pnds.back().pos)->fch = pnd;

		//find invalid bs,if none add a new bs
		if(invalid_bs.empty())
		{
			ibs_znd = global->nextbs;
			if(ibs_znd == -1 || ibs_znd == GetTableSize(g_db_dms->GetTB("DMS_BS")) - 1)
			{
				printf("out of memory\n");
				return TP_FAIL;
			}
			(bs + ibs_znd)->valid = 1;
			(nd + pnd)->ibs = ibs_znd;
			(bs + ibs_znd)->pnd = pnd;
			(bs + ibs_znd)->iisland = (bs + ibs_nd)->iisland;

			//move to next bs
			global->nextbs++;
		}
		else
		{
			ibs_znd = invalid_bs.back().pos;
			invalid_bs.pop_back();
			(bs + ibs_znd)->valid = 1;
			(nd + pnd)->ibs = ibs_znd;
			(bs + ibs_znd)->pnd = pnd;
			(bs + ibs_znd)->iisland = (bs + ibs_nd)->iisland;
		}

		for(vi = pnds.begin(); vi != pnds.end(); vi++)
		{
			tmp_node->pos = (*vi).pos;
			knd = tmp_node->pos;
			(nd + knd)->ibs = ibs_znd;

			printf("nd %d ibs change %d\n", knd, ibs_znd);

			//查找与nd相连的ld，并且ld的ibs发生相应的变化
			br_p = r_nd_ld_tb->FindChildPtr((nd + knd)->r_ld);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (ld + br)->ind)
				{
					(ld + br)->ibs = ibs_znd;
					printf("ld %d ibs change %d\n",br,ibs_znd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);

			//查找与nd相连的cp，并且cp的ibs发生相应的变化
			br_p = r_nd_cp_tb->FindChildPtr((nd + knd)->r_cp);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (cp + br)->ind)
				{
					(cp + br)->ibs = ibs_znd;
					printf("cp %d ibs change %d\n",br,ibs_znd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);

			//查找与nd相连的sec，并且sec的ibs或者zbs发生相应变化
			br_p = r_nd_sec_tb->FindChildPtr((nd + knd)->r_sec);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (sec + br)->ind)
				{
					(sec + br)->ibs = ibs_znd;
					printf("sec %d ibs change %d\n",br,ibs_znd);
				}
				else
				{
					(sec + br)->zbs = ibs_znd;
					printf("sec %d zbs change %d\n",br,ibs_znd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);

			//查找与nd相连的xf，并且xf的ibs或者zbs发生相应变化
			br_p = r_nd_xf_tb->FindChildPtr((nd + knd)->r_xf);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (xf + br)->ind)
				{
					(xf + br)->ibs = ibs_znd;
					printf("xf %d ibs change %d\n",br,ibs_znd);
				}
				else
				{
					(xf + br)->zbs = ibs_znd;
					printf("xf %d zbs change %d\n",br,ibs_znd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);
		}
		pnds.clear();
		delete tmp_node;
		return ibs_znd;
	}
	else//delete bs
	{
		ibs_nd = (nd + ind)->ibs;
		ibs_znd = (nd + iznd)->ibs;
		if(ibs_nd == -1 || ibs_znd == -1)
		{
			return -1;
		}
		//计算母线合并为一个,删除一个计算母线
		i = (bs + ibs_nd)->pnd;
		pnds.clear();
		find_DFS(i, bvisited);
		(nd + pnds.back().pos)->fch = (bs + ibs_znd)->pnd;
		pnds.clear();
		for(int j = 0; j < 100; j++)
		{
			bvisited[j] = false;
		}
		//printf("the pnd is %d\n",(bs + ibs_znd)->pnd);
		pnd = (bs + ibs_znd)->pnd;
		find_DFS(pnd, bvisited);

		for(vi = pnds.begin(); vi != pnds.end(); vi++)
		{
			tmp_node->pos = (*vi).pos;
			knd = tmp_node->pos;
			(nd + knd)->ibs = ibs_nd;
			printf("nd %d ibs change %d\n", knd, ibs_nd);
			//查找与nd相连的ld，并且ld的ibs发生相应的变化
			br_p = r_nd_ld_tb->FindChildPtr((nd + knd)->r_ld);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (ld + br)->ind)
				{
					(ld + br)->ibs = ibs_nd;
					printf("ld %d ibs change %d\n",br,ibs_nd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);

			//查找与nd相连的cp，并且cp的ibs发生相应的变化
			br_p = r_nd_cp_tb->FindChildPtr((nd + knd)->r_cp);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (cp + br)->ind)
				{
					(cp + br)->ibs = ibs_nd;
					printf("cp %d ibs change %d\n",br,ibs_nd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);

			//查找与nd相连的sec，并且sec的ibs或者zbs发生相应变化
			br_p = r_nd_sec_tb->FindChildPtr((nd + knd)->r_sec);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (sec + br)->ind)
				{
					(sec + br)->ibs = ibs_nd;
					printf("sec %d ibs change %d\n",br,ibs_nd);
				}
				else
				{
					(sec + br)->zbs = ibs_nd;
					printf("sec %d zbs change %d\n",br,ibs_nd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);

			//查找与nd相连的xf，并且xf的ibs或者zbs发生相应变化
			br_p = r_nd_xf_tb->FindChildPtr((nd + knd)->r_xf);
			br_p0 = br_p;
			while(br_p)
			{
				br = br_p->Ptr;
				if(knd == (xf + br)->ind)
				{
					(xf + br)->ibs = ibs_nd;
					printf("xf %d ibs change %d\n",br,ibs_nd);
				}
				else
				{
					(xf + br)->zbs = ibs_nd;
					printf("xf %d zbs change %d\n",br,ibs_nd);
				}
				br_p = br_p->Next;
			}
			FreeChildPtr(br_p0);
		}
		(nd + pnds.back().pos)->fch = i;
		(bs + ibs_znd)->valid = 0;
		tmp_node->pos = ibs_znd;
		invalid_bs.push_back(*tmp_node);

		pnds.clear();
		delete tmp_node;
		return ibs_znd;
	}
}
