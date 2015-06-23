#include<regex.h>
#include <iostream>
#include<vector>
#include "DBPSBOB.h"

namespace PSBOB
{
#include<psbob.h>
};

using namespace std;

int main(int _argc, char *argv[])
{
	const char reg_str[] = "[^1]+10kV.*母.*";
	regex_t reg;
	int ret = regcomp(&reg, reg_str, REG_EXTENDED | REG_NOSUB | REG_ICASE);
	DB_PSBOB *db_psbob;

	db_psbob = new DB_PSBOB;

	if (db_psbob->Init() == OO_FAIL)
	{
		return -1;
	}

	int cnt_record = oodbget_recnum(const_cast<TB_DESCR*>(db_psbob->GetTB("bus")));
	PSBOB::BUS psbob_bus;
	int len = sizeof(psbob_bus);

	for (int pos = 0; pos < cnt_record; pos++)
	{
		bzero(&psbob_bus, len);

		if (oodbread_rp(&psbob_bus, pos, const_cast<TB_DESCR*>(db_psbob->GetTB("bus")), 1, len) != OO_FAIL)
		{
			regmatch_t pm[5] = { 0 };
			const size_t nmatch = 5;
			if (regexec(&reg, psbob_bus.descr, nmatch, pm, 0) == REG_NOMATCH)
			{
				continue;
			}
			else
			{
				PSBOB::ANALOG psbob_analog;

				psbob_analog.value = 0;

				if (oodbupdate_ak(&psbob_analog.value, psbob_bus.analogv, "value", const_cast<TB_DESCR*>(db_psbob->GetTB("analog")), sizeof(psbob_analog.value)) != OO_FAIL)
				{
					printf("成功设置母线%s[%s]电压值为%f！\r\n", psbob_bus.id, psbob_bus.descr, psbob_analog.value);
				}
				else
				{
					printf("更新PSBOB库ANALOG表关键字为%s记录失败，错误号：%d\r\n", psbob_bus.analogv, _oodb_errno);
				}
			}
		}
		else
		{
			printf("读取PSBOB库BUS表记录失败，错误号：%d\r\n", _oodb_errno);
		}
	}

	regfree(&reg);
	delete db_psbob;
	return 0;
}
