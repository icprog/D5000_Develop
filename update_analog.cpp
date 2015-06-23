/*
 * update_analog.cpp
 *
 *  Created on: 2012-6-19
 *      Author: BetaKDE
 */

#include <iostream>
#include<vector>
#include "DBPSBOB.h"
#include "DBDMS.h"
namespace PSBOB
{
#include<psbob.h>
};

namespace DMS
{
#include<pwrflow.h>
};

using namespace std;

float average(vector<float> *panalog)
{
	assert(panalog != NULL);

	float value = 0;

	for(vector<float>::iterator it = panalog->begin(); it != panalog->end(); it++)
	{
		value += *it;
	}

	return value/(panalog->size() * 1.0000);
}

int main(int argc, char *argv[])
{
	DB_PSBOB *db_psbob;
	DB_DMS *db_dms;

	db_psbob = new DB_PSBOB;
	db_dms = new DB_DMS;

	if(argc < 2)
	{
		printf("usage:%s system_name", argv[0]);
		return -1;
	}

	if (db_psbob->Init("psbob", argv[1]) == OO_FAIL || db_dms->Init("pwrflow", argv[1]) == OO_FAIL)
	{
		delete db_psbob;
		delete db_dms;

		return -1;
	}

	db_dms->ResetTable("ANALOG");

	PSBOB::ANALOG psbob_analog = { 0 };
	DMS::ANALOG dms_analog = { 0 };
	int cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(db_psbob->GetTB("analog")));
	int size_psbob = sizeof(PSBOB::ANALOG);
	int size_dms = sizeof(DMS::ANALOG);
	vector<float> *panalog = new vector<float>[cnt_record];

	for (int i = 0; i < cnt_record; i++)
	{
		bzero(&psbob_analog, sizeof(PSBOB::ANALOG));
		bzero(&dms_analog, sizeof(DMS::ANALOG));

		if (oodbread_rp(&psbob_analog, i, const_cast<TB_DESCR*>(db_psbob->GetTB("analog")), 1, size_psbob) == OO_FAIL)
		{
			printf("��ȡPSBOB��ANALOG���%d����¼ʧ�ܣ�����ţ�%d\r\n", i, _oodb_errno);
			return -2;
		}

		strcpy(dms_analog.id, psbob_analog.id);
		strcpy(dms_analog.descr, psbob_analog.descr);
		dms_analog.value = psbob_analog.value;

		if (oodbinsert(&dms_analog, const_cast<TB_DESCR*>(db_dms->GetTB("ANALOG")), 1, size_dms) == OO_FAIL)
		{
			printf("��IDΪ%sд��DMS��ANALOG��ʧ�ܣ�����ţ�%d\r\n", dms_analog.id, _oodb_errno);
			return -3;
		}
	}

	while (true)
	{
		bzero(&psbob_analog, size_psbob); // = { 0 };
		bzero(&dms_analog, size_dms); // = { 0 };
		cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(db_psbob->GetTB("analog")));

		time_t now = time(0);

		cout << ctime(&now);
		cout << "�������ݡ���";
		for (int i = 0; i < cnt_record; i++)
		{
			bzero(&psbob_analog, sizeof(PSBOB::ANALOG));
			bzero(&dms_analog, sizeof(DMS::ANALOG));

			if (oodbread_rp(&psbob_analog, i, const_cast<TB_DESCR*>(db_psbob->GetTB("analog")), 1, size_psbob) == OO_FAIL)
			{
				printf("��ȡPSBOB��ANALOG���%d����¼ʧ�ܣ�����ţ�%d\r\n", i, _oodb_errno);
				return -2;
			}

			//ֵ�ӽ�0
			if (psbob_analog.value < 0.05)
			{
				if (panalog[i].size() <= 0)
					continue;
				dms_analog.value = average(&panalog[i]);
				panalog[i].clear();
			}
			else
			{
				//ֻͳ��60����
				if (panalog[i].size() > 60)
					panalog[i].erase(panalog[i].begin());

				panalog[i].push_back(psbob_analog.value);
				dms_analog.value = average(&panalog[i]);
			}

			strcpy(dms_analog.id, psbob_analog.id);
			strcpy(dms_analog.descr, psbob_analog.descr);
			dms_analog.value = max(dms_analog.value , psbob_analog.value);// ? dms_analog.value : psbob_analog.value;

			if (oodbupdate_rk(&dms_analog, dms_analog.id, const_cast<TB_DESCR*>(db_dms->GetTB("ANALOG")), size_dms) == OO_FAIL)
			{
				printf("��IDΪ%sд��DMS��ANALOG��ʧ�ܣ�����ţ�%d\r\n", dms_analog.id, _oodb_errno);
				return -3;
			}
		}
		cout << "����������ɣ�" << endl << endl;

		sleep(5);
	}

	delete []panalog;
	delete db_psbob;
	delete db_dms;

	cout << "All Done!" << endl;
	return 0;
}
