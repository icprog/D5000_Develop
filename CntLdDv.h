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
//����ͳ�Ƹ��ɵĽṹ
typedef struct
{
	long id;
	string descr;
	long dv_id;
	long st;
	long rel_st;
	string lwcb_id;
}COUNT_LD;

//�������������Ϣ�Ľṹ
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
////////////////////////////////////////ͳ�����������ϵ��ʱ��ʹ�ã�������Ŀ�д˹���//////////////////////////
	bool get_info_dvs();
	bool count_dv();
	bool count_ld();
	void search_ld(map<long, COUNT_LD> &ld_info, int izone, char* lwcb_id, long st_id);
////////////////////////////////////////ͳ�����������ϵ��ʱ��ʹ�ã�������Ŀ�д˹���//////////////////////////
};

extern int GetStbyDv(long dvid, PSBOB::ST &st);
extern int GetDv(long devid, const char *devtb, PSBOB::DV &dv);


#endif /* CNT_H_ */
