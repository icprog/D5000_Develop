#include<iostream>
#include<pwrflow.h>
#include"RTDB.h"
#include"DBDMS.h"
using namespace std;

int main()
{
	FILE * fp = fopen("/data/users/osa/analog.out.txt", "r");
	char buf[1024] = { 0 };
	char Temp[512] = { 0 };

	DB_DMS *g_db_dms = new DB_DMS;

	if (g_db_dms->Init() == OO_FAIL)
	{
		delete g_db_dms;

		return -1;
	}

	if (fp != NULL)
	{
		system("tb_reset realtime dms pwrflow ANALOG");

		while (fgets(buf, 1024, fp) > 0)
		{
			ANALOG analog = { 0 };
			char *p = strchr(buf, '\t');
			if (p != NULL)
			{
				strncpy(analog.id, buf, p - buf);

				char *p1 = ++p;

				p = strchr(p1, '\t');

				if (p != NULL)
				{
					strncpy(analog.descr, p1, p - p1);

					char *p2 = ++p;

					p = strchr(p2, '\t');

					if (p != NULL)
					{
						bzero(Temp, sizeof(Temp));
						strncpy(Temp, p2, p - p2);

						analog.value = atof(Temp);

						char *p3 = ++p;

						p = strchr(p3, '\t');

						if (p != NULL)
						{
							bzero(Temp, sizeof(Temp));
							strncpy(Temp, p3, p - p3);
							analog.quality = atoi(Temp);
						}
					}
				}
			}

			if (oodbinsert(&analog, const_cast<TB_DESCR*>(g_db_dms->GetTB("ANALOG")), 1, sizeof(analog)) == OO_FAIL)
			{
				cout << buf << endl;
				cout << _oodb_errno << endl;
			}
		}
	}
}
