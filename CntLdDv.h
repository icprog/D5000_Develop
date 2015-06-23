/*
 * cnt.h
 *
 *  Created on: 2013-9-23
 *      Author: tianyq
 */

#ifndef CNT_H_
#define CNT_H_

#include <iostream>
#include <map>
#include "FDIR_R.h"

using namespace std;
//保存统计负荷的结构
typedef struct
{
	long id;
	string descr;
	long dv_id;
	long st;
	long rel_st;
	string lwcb_id;
}COUNT_LD;

//保存馈线相关信息的结构
typedef struct
{
	string descr;
	long st_id;
	map<long, string> lwcb;
	map<long, string> st_lw;
	map<long, string> dv_lw;
}COUNT_DV;

class Cntld : public FDIR_R
{
public:
	Cntld(DSBOB *pds) :
			FDIR_R(pds)
	{

	}
	~Cntld()
	{

	}
	int Init();
////////////////////////////////////////统计馈线联络关系的时候使用，福州项目有此功能//////////////////////////
	bool get_info_dvs();
	bool count_dv();
	bool count_ld();
	void search_ld(map<long, COUNT_LD> &ld_info, int izone, char* lwcb_id, long st_id);
////////////////////////////////////////统计馈线联络关系的时候使用，福州项目有此功能//////////////////////////
};

extern int GetStbyDv(long dvid, PSBOB::ST &st);
extern int GetDv(long devid, const char *devtb, PSBOB::DV &dv);


#endif /* CNT_H_ */
