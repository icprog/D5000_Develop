/*
 * update_value.cpp
 *
 *  Created on: 2012-12-4
 *      Author: BetaKDE
 */

#include<iostream>
//#include<oodb.h>
#include "RTDB.h"

using namespace std;

#define MAX_PATH	260

int main(int argc, char *argv[])
{
	if (argc < 6)
	{
		cout << "usage:update_value systemname bobname tablename attrname attrvalue [servername]" << endl;
		return -1;
	}

	char servername[MAX_PATH];
	char systemname[MAX_PATH];
	char bobname[MAX_PATH];
	char tablename[MAX_PATH];
	char attrname[MAX_PATH];
	char attrvalue[MAX_PATH];

	strncpy(systemname, argv[1], MAX_PATH);
	strncpy(bobname, argv[2], MAX_PATH);
	strncpy(tablename, argv[3], MAX_PATH);
	strncpy(attrname, argv[4], MAX_PATH);
	strncpy(attrvalue, argv[5], MAX_PATH);

	if (argc > 6)
		strncpy(servername, argv[6], MAX_PATH);
	else
		servername[0] = 0;

	cout << "servername:" << servername << endl;
	cout << "systemname:" << systemname << endl;
	cout << "bobname:" << bobname << endl;
	cout << "tablename:" << tablename << endl;
	cout << "attrname:" << attrname << endl;
	cout << "attrvalue:" << attrvalue << endl;

	RTDB rtdb;

	if(rtdb.Init("", systemname, bobname, servername, 1) == OO_FAIL)
	{
		printf("打开数据库失败！\r\n");
		return -1;
	}

	TB_DESCR *tb_descr =  const_cast<TB_DESCR*>(rtdb.GetTB(tablename));

	if(tb_descr == NULL)
	{
		return -1;
	}

	int len = oodbget_attrlen(tb_descr, attrname);
	int type = oodbget_attrtype(tb_descr, attrname);

	if (len <= 0 || type < 0)
	{
		printf("获取表性格信息失败！\r\n");
		return -1;
	}

	float fvalue = 0;
	double dvalue = 0;
	int ivalue = 0;
	short svalue = 0;
	long lvalue;
	unsigned char cvalue;


	switch (type)
	{
	case C_STRING_TYPE:
	{
		break;
	}
	case C_UCHAR_TYPE:
	{
		cvalue = atoi(attrvalue);
		break;
	}
	case C_SHORT_TYPE:
	{
		svalue = atol(attrvalue);
		break;
	}
	case C_INT_TYPE:
	{
		ivalue = atoi(attrvalue);
		break;
	}
	case C_DATETIME_TYPE:
	{
		lvalue = atol(attrvalue);
		break;
	}
	case C_FLOAT_TYPE:
	{
		fvalue = atof(attrvalue);
		break;
	}
	case C_DOUBLE_TYPE:
	{
		dvalue = atof(attrvalue);
		break;
	}
	case C_LONG_TYPE:
	{
		lvalue = atol(attrvalue);
		break;
	}
	case C_KEYID_TYPE:
	case C_BINARY_TYPE:
	case C_TEXT_TYPE:
	case C_IMAGE_TYPE:
	case C_APPKEY_TYPE:
	case C_APPID_TYPE:
	case C_UINT_TYPE:
	default:
	{
		printf("不支持的数据类型！\r\n");
		break;
	}
	}

	printf("开始更新记录信息……\r\n");

	int cnt = oodbget_recnum(tb_descr);
	if (cnt == OO_FAIL)
	{
		printf("获取表记录条数信息失败！\r\n");
		return -4;
	}

	TABLE_PARA_STRU tps = { 0 };
	if (GetTablePara(tb_descr, &tps) < 0)
	{
		printf("获取表结构信息失败！\r\n");
		return -1;
	}

	char *buf = NULL;
	if(GetTableRecs(tb_descr, 1, &buf) < 0)
	{
		printf("获取表记录信息失败！\r\n");
		return -1;
	}

	int offset = 0;
	int field_no[2] = { 0 };
	int fieldnum = 0;
	int ret_code = GetFieldNoByName(tb_descr, attrname, field_no, &fieldnum);
	if (ret_code < 0)
	{
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	FIELD_PARA_STRU fps = { 0 };

	if (GetFieldParaByID(tb_descr, field_no[0], &fps) == 0)
	{
		offset = fps.offset;
	}
	else
	{
		return OO_FAIL;
	}

	int len_str = len >= strlen(attrvalue) ? strlen(attrvalue) : len;

	for(int i = 0; i < cnt; i++)
	{
		char *pAttr = buf + i * tps.record_size + offset;

		memset(pAttr, 0, len);

		switch (type)
		{
		case C_STRING_TYPE:
		{
			memcpy(pAttr, attrvalue, len_str);
			break;
		}
		case C_UCHAR_TYPE:
		{
			memcpy(pAttr, &cvalue, sizeof(cvalue));
			//cvalue = atoi(attrvalue);
			break;
		}
		case C_SHORT_TYPE:
		{
			memcpy(pAttr, &svalue, sizeof(svalue));
			//svalue = atol(attrvalue);
			break;
		}
		case C_INT_TYPE:
		{
			memcpy(pAttr, &ivalue, sizeof(ivalue));
			//ivalue = atoi(attrvalue);
			break;
		}
		case C_DATETIME_TYPE:
		{
			memcpy(pAttr, &lvalue, sizeof(lvalue));
			//lvalue = atol(attrvalue);
			break;
		}
		case C_FLOAT_TYPE:
		{
			memcpy(pAttr, &fvalue, sizeof(fvalue));
			//fvalue = atof(attrvalue);
			break;
		}
		case C_DOUBLE_TYPE:
		{
			memcpy(pAttr, &dvalue, sizeof(dvalue));
			//dvalue = atof(attrvalue);
			break;
		}
		case C_LONG_TYPE:
		{
			memcpy(pAttr, &lvalue, sizeof(lvalue));
			//lvalue = atol(attrvalue);
			break;
		}
		case C_KEYID_TYPE:
		case C_BINARY_TYPE:
		case C_TEXT_TYPE:
		case C_IMAGE_TYPE:
		case C_APPKEY_TYPE:
		case C_APPID_TYPE:
		case C_UINT_TYPE:
		default:
		{
			printf("不支持的数据类型！\r\n");
			break;
		}
		}
	}

	if(tb_descr->is_local)
		tb_descr->ptab->TableUpdate(buf, cnt, tps.record_size);
	else
		tb_descr->ptabnet->TableUpdate(buf, cnt * tps.record_size);

	cout << "更新所有记录完成!" << endl;
}
