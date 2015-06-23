/*
 * oodb.cpp
 *
 *  Created on: 2012-10-26
 *      Author: BetaKDE
 */

//#include <db_struct_scada.h>
#include"oodb.h"
#include"rtdb_api.h"

int _oodb_errno = 0;

void dberr(int err)
{
	if (err >= 0)
		return;

	char str[2048] = { 0 };

	sprintf(str, "cat %s/src/include/db_com/db_error_define.h | grep \"\\%d[^0-9]\" -E", getenv("HOME"), err);
	system(str);
	sprintf(str, "cat %s/src/include/d5000_err.h | grep \"\\%d[^0-9]\" -E", getenv("HOME"), err);
	system(str);
}

int oodbinit()
{
	_oodb_errno = 0;
	return OO_SUCCEED;
}

int oodbopen(char *ds_name, char *sys_name, char *bob_name, char *server, int mode, DB_DESCR *db_descr)
{
	assert(db_descr != NULL);

	if (ds_name != NULL)
		strcpy(db_descr->ds_name, ds_name);

	if (sys_name != NULL)
		strcpy(db_descr->sys_name, sys_name);

	if (bob_name != NULL)
		strcpy(db_descr->bob_name, bob_name);

	if (server != NULL)
		strcpy(db_descr->server, server);

	db_descr->mode = mode;

	_oodb_errno = 0;

	return OO_SUCCEED;
}

int oodbclose(char *ds_name, char *sys_name, char *bob_name, char *server, DB_DESCR *db_descr)
{
	return OO_SUCCEED;
}

int oodbuse(char *tb_name, DB_DESCR *db_descr, TB_DESCR *tb_descr)
{
	assert(tb_name != NULL && db_descr != NULL && tb_descr != NULL);

	_oodb_errno = OpenTableByName(db_descr->server, db_descr->sys_name, db_descr->bob_name, tb_name, tb_descr);

	return _oodb_errno >= 0 ? OO_SUCCEED : OO_FAIL;
}
//功能测试OK。
int oodbread_rph(void *buffer, int s_pos, TB_DESCR *tb_descr, int length)
{
	assert(NULL!= buffer && NULL != tb_descr);
	int ret_code = 0;
	int key_fieldno = 0;
	key_fieldno = GetKeyfieldNo(tb_descr);


	// 需要修改，不争取，返回的是个数组。
	// 构造关键字ID
	KEY_STRU key_stru;
	key_stru.key = s_pos;
	key_stru.field_id = key_fieldno;
	key_stru.table_no = tb_descr->table_id;
	unsigned long kid = 0;
	CCommon::keyid_to_long(&key_stru, &kid);

	// 正常情况下返回值大于等于0
	ret_code = GetRecByID(tb_descr, (char*)&kid,  length, (char*)buffer);

	if (ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	} else {
		return length ;
	}

}

int oodbread_rk(void *buffer, void *key_value, TB_DESCR *tb_descr, int length)
{
	int ret = GetRecByID(tb_descr, (char*) key_value, length, (char*) buffer);

	if (ret < 0)
	{
		_oodb_errno = ret;
		return OO_FAIL;
	}
	else
	{
		_oodb_errno = 0;
	}

	return ret;
}

int oodbget_attrlen(TB_DESCR *tb_descr, char *attr_name)
{
	int size = 0;
	int ret_code = 0;
	int field_no[2] = { 0 };
	int fieldnum = 0;
	ret_code = GetFieldNoByName(tb_descr, attr_name, field_no, &fieldnum);
	if (ret_code < 0)
	{
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	FIELD_PARA_STRU fps = { 0 };

	if (GetFieldParaByID(tb_descr, field_no[0], &fps) == 0)
	{
		return fps.field_length;
	}
	else
	{
		return OO_FAIL;
	}
}

int oodbget_attrtype(TB_DESCR *tb_descr, char *attr_name)
{
	int size = 0;
	int ret_code = 0;
	int field_no[2] = { 0 };
	int fieldnum = 0;
	ret_code = GetFieldNoByName(tb_descr, attr_name, field_no, &fieldnum);
	if (ret_code < 0)
	{
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	FIELD_PARA_STRU fps = { 0 };

	if (GetFieldParaByID(tb_descr, field_no[0], &fps) == 0)
	{
		return fps.data_type;
	}
	else
	{
		return OO_FAIL;
	}
}

// 类型不全
int oodbread_ra(void *buffer, char *attr_name, void *attr_value, TB_DESCR *tb_descr, int length)
{
	assert(buffer != NULL && attr_name != NULL && attr_value != NULL && tb_descr != NULL);

	int size = 0;
	int ret_code = 0;
	int field_no[2] = {0};
	int fieldnum = 0;
	ret_code = GetFieldNoByName(tb_descr, attr_name, field_no, &fieldnum);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	char* buf_ptr = NULL;
	ret_code = GetRecsByAttr(tb_descr,  field_no[0], EQU, (char*)attr_value, 0, &buf_ptr);
	if(0 >= ret_code) {
		if(NULL!=buf_ptr) {
			BufFree(buf_ptr);
			buf_ptr = NULL;
		}
		_oodb_errno = ret_code;
		return OO_FAIL;
	}
	if (ret_code > length) {
		memcpy(buffer, buf_ptr, length);
		ret_code = length;
	} else {
		memcpy(buffer, buf_ptr, ret_code);
	}

	BufFree(buf_ptr);

	return ret_code;
}
// 没调
int oodbget_tabnum(DB_DESCR *db_descr)
{
	vector<int> vec;
	TB_DESCR tb_des;
	int appno, ret, num;
	num = 0;
#ifdef _SCADA
	ret = tb_des.ptab->GetAllOdbNoByAppName(appno, vec, "SCADA");
#else
	ret = tb_des.ptab->GetAllOdbNoByAppName(appno, vec, "DSCADA");
#endif

	if(ret == 0)
	{
		for(vector<int>::iterator it = vec.begin(); it != vec.end(); it++)
		{
			num ++;
			cout << *it << ":";
			char tname[CHN_NAME_LEN] = { 0 };
			tb_des.ptab->GetTableNameByNo(tname, *it, false);
			cout << tname << ",";
			tb_des.ptab->GetTableNameByNo(tname, *it, true);
			cout << tname << endl;
		}
		return num;
	}
	return OO_FAIL;

}
// 没调
int oodbget_tbname(DB_DESCR *db_descr, char **tb_name)
{
	return OO_SUCCEED;
}
// 能读取。 已测试功能
int oodbread_rp(void *buffer, int s_pos, TB_DESCR *tb_descr, int number,int length)
{
	// d5000内部已经存在接口。
	assert(NULL!= buffer && NULL != tb_descr);
	int ret_code = 0;
	//判断有效记录数是否满足
	TABLE_PARA_STRU tps = { 0 };
	ret_code = GetTablePara(tb_descr, &tps);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	if(tps.record_number < s_pos+number) {
		_oodb_errno = tps.record_number;
		return OO_FAIL;
	}

	char* buf = NULL;

	ret_code = GetSerialRecs(tb_descr, s_pos, number, (char**)&buf);
	if(0 > ret_code) {

		if(NULL!=buf) {
			BufFree(buf);
			buf = NULL;
		}
		_oodb_errno = ret_code;
		return OO_FAIL;
	}
	memcpy(buffer, buf, length);

	if(NULL!=buf) {
		BufFree(buf);
		buf = NULL;
	}

	return ret_code;

#if 0
	// 判断表中记录数要大于 s_pos + number
	assert(NULL!= buffer && NULL != tb_descr && 0 > number);


	FIELD_PARA_STRU *pfps = NULL;
	ret_code = GetFieldsPara(tb_descr, &pfps);
	// 添加错误处理。

	// 构造关键字ID
	KEY_STRU key_stru;
	key_stru.key = s_pos;
	key_stru.field_id = pfps->field_no;
	key_stru.table_no = tb_descr->table_id;
	unsigned long kid = 0;
	CCommon::keyid_to_long(&key_stru, &kid);

	long id[number];
	for(int i = 0; i < number; ++i)
	{
		id[i] = kid;
		++kid;
	}

	// 正常情况下返回值大于等于0
	ret_code = GetRecsByID(tb_descr, (char*)id,  nunber, 20*sizeof(SUBSTATION_SCADA), (char*)st);


	return OO_SUCCEED;

#endif
}
// 能正常读取数据。
int oodbread_aa(void *buffer, char *attr_name1,  void *attr_value,char *attr_name2, TB_DESCR *tb_descr, int length)
{
	int ret_code;
	char fieldname[1000] = {0};
	int field_no[10] = {0};
	int fieldnum = 2;
	sprintf(fieldname, "%s%s%s",attr_name1,",",attr_name2);

	ret_code = GetFieldNoByName(tb_descr, fieldname, field_no, &fieldnum);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	//FIELD_PARA_STRU *pfps = NULL;
	//ret_code = GetFieldsPara(tb_descr, &pfps);

	char* buf_ptr = NULL;
	ret_code = GetFieldsByAttr (tb_descr, &field_no[1], 1, field_no[0], EQU, (char*)attr_value, 0, &buf_ptr);
	if(0 > ret_code) {
		if(NULL!=buf_ptr) {
			BufFree(buf_ptr);
			buf_ptr = NULL;
		}
		_oodb_errno = ret_code;
		return OO_FAIL;
	}
	if (ret_code > length) {
		memcpy(buffer, buf_ptr, length);
		ret_code = length;
	} else {
		memcpy(buffer, buf_ptr, ret_code);
	}

	BufFree(buf_ptr);

	return ret_code;
}

// 能读取。 测试通过
int oodbread_ap(void *buffer, int s_pos, char *attr_name, TB_DESCR *tb_descr, int number, int length)
{
	assert(NULL!= buffer && NULL != tb_descr);
	if(number <= 1) {
		number = 1;
	}
	int ret_code = 0;
	int fieldnum;
	int field_no[10] = {0};
	char* buf_ptr;
	int size;
	//判断有效记录数是否满足
	TABLE_PARA_STRU tps = { 0 };
	ret_code = GetTablePara(tb_descr, &tps);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	if(tps.record_number < s_pos+number) {
		_oodb_errno = tps.record_number;
		return OO_FAIL;
	}
	//接口只提供读取一个属性的值
	ret_code = GetFieldNoByName(tb_descr, attr_name, field_no, &fieldnum);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}
	// 读全表的一个域
	ret_code = GetTableFields(tb_descr,  0, field_no, fieldnum, &buf_ptr);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	// 截取要读的内存。
	FIELD_PARA_STRU field_para;

	ret_code = GetFieldParaByID(tb_descr, field_no[0], &field_para);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	if(field_para.field_length <= (length/number)) {

	} else {
		//此种情况咱不容错。
		_oodb_errno = -1;
		return OO_FAIL;
	}
	buf_ptr = buf_ptr+(s_pos*field_para.field_length);

	size = number*field_para.field_length;

	memcpy(buffer, buf_ptr, size);

	return size;

#if 0

	ret_code = GetSerialRecs(tb_descr, s_pos, number, &buf_ptr);
	if(0 > ret_code) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}
	//接口只提供读取一个属性的值
	ret_code = GetFieldNoByName(tb_descr, attr_name, &field_no, 1);
	for (int i=0; i<number; i++) {
		GetFieldsByName(tb_descr, code, field_no, 1, sizeof(MY_TMP_STRU), (char*)&tmp_stru);
	}
	return OO_SUCCEED;
#endif
}
// 测试能正常读取数据。
int oodbread_ak(void *buffer, void *key_value, char *attr_name, TB_DESCR *tb_descr,int length)
{
	assert(NULL!= buffer && NULL != tb_descr && NULL!= attr_name && NULL!=key_value);
	int field_no[1] = {0};
	int ret = 0;
	int field_num = 1;

	ret = GetFieldNoByName(tb_descr, attr_name, field_no, &field_num);
	if(ret < 0) {
		_oodb_errno = ret;
		return ret;
	}

	ret = GetFieldsByID(tb_descr, (char*)key_value, field_no, field_num, length, (char*)buffer);
	if(ret != 0) {
		_oodb_errno = ret;
		return ret;
	}
	return OO_SUCCEED;
#if 0
	TABLE_PARA_STRU tps = { 0 };
	int ret = GetTablePara(tb_descr, &tps);

	FIELD_PARA_STRU *pfps = NULL;
	ret = GetFieldsPara(tb_descr, &pfps);
	int field_no;
	field_no = GetKeyfieldNoBy(tb_descr);


	return OO_SUCCEED;
#endif
}

// 测试通过。
int oodbget_ph(TB_DESCR *tb_descr, void *key_value)
{
	TABLE_PARA_STRU tps = { 0 };
	int ret = GetTablePara(tb_descr, &tps);
	if(true != tps.is_auto_generated)
	{
		//
		return OO_FAIL;
	}
	//自动生成的主键的数据时long型。
	KEY_STRU key_stru;
	CCommon::long_to_keyid(*(long*)key_value, &key_stru);

	return key_stru.key;

}
// 通过关键字的值更新指定区域的值。
//接口只提供读取一个属性的值
// 测试通过
int oodbupdate_ak(void *buffer, void *key_value, char *attr_name,TB_DESCR *tb_descr, int length)
{
	assert(NULL != buffer && NULL != tb_descr && NULL != key_value);
	int ret = 0;
	int field_no[1] = {0};
	int fieldnum = 1;

	ret = GetFieldNoByName(tb_descr, attr_name, field_no, &fieldnum);
	if(ret < 0) {
		_oodb_errno = ret;
		return OO_FAIL;
	}
	ret = UpdateFieldsByID(tb_descr, (char*)key_value,
							field_no, fieldnum,
							length, (char*)buffer);
	if(ret < 0) {
		_oodb_errno = ret;
		return OO_FAIL;
	}

	return ret;
}
// 测试通过。
int oodbupdate_rk(void *buffer, void *key_value, TB_DESCR *tb_descr, int   length)
{
	// 参数判断
	assert(NULL != buffer && NULL != tb_descr && NULL != key_value);
	int ret_code = 0;
	ret_code = UpdateRecByID (tb_descr, (char*)key_value, length, (char*)buffer);
	// 接口文档中写=0 成功， 有的时候大于零也成功。
	if(0 > ret_code) {
		_oodb_errno = ret_code;
		return ret_code;
	}
	return length;
}

int oodbget_recnum(TB_DESCR *tb_descr)
{
	int ret_code = 0;
	//判断有效记录数是否满足
	TABLE_PARA_STRU tps = { 0 };
	ret_code = GetTablePara(tb_descr, &tps);
	if(ret_code < 0) {
		_oodb_errno = ret_code;
		return OO_FAIL;
	}
	return tps.record_number;
}

int GetTableSize(const TB_DESCR *tb_descr)
{
	assert(tb_descr != NULL);

	TABLE_PARA_STRU tps = { 0 };

	int ret_code = GetTablePara(const_cast<TB_DESCR*>(tb_descr), &tps);
	if (ret_code < 0)
	{
		_oodb_errno = ret_code;
		return OO_FAIL;
	}

	return tps.max_valid_rec;
}

int oodbinsert(void *buffer, TB_DESCR *tb_descr, int number, int length)
{
	assert(tb_descr != NULL);

	int ret = InsertRec(tb_descr, number, length, (char*) buffer, 1);

	return ret <= 0 ? OO_FAIL : length;
}

unsigned long get_id(TB_DESCR *tb_descr, int pos)
{
	assert(tb_descr != NULL);

	TABLE_PARA_STRU tps = { 0 };
	int ret = GetTablePara(tb_descr, &tps);

	if (ret >= 0)
	{
		FIELD_PARA_STRU *pfps = NULL;
		ret = GetFieldsPara(tb_descr, &pfps);
		if (ret >= 0)
		{
			int cnt = ret / sizeof(FIELD_PARA_STRU);

			for (int i = 0; i < cnt; i++)
			{
				if (pfps[i].is_keyword)
				{
					if (pfps[i].data_type != C_LONG_TYPE)
						return 0;

					if (pos < 0)
					{
						if (tps.record_number >= tps.max_valid_rec)
						{
							cout << "table is full!" << endl;
							return 0;
						}

						while (true)
						{
							KEY_STRU key_stru;
							key_stru.field_id = pfps[i].column_id;
							key_stru.table_no = tb_descr->table_id;
							unsigned long kid = 0;
							int key = 0;

							char *buf = NULL;

							ret = GetTableRecs(tb_descr, 0, (char**) &buf);

							if (ret < 0)
							{
								return 0;
							}
							else if (ret == 0) //表里无记录
							{
								key = 1;
							}
							else
							{
								int num = ret / tps.record_size;

								char *ptr_record = buf;
								long *ptr_attr = NULL;
								int pre_key = 0;

								for (int j = 0; j < num; j++)
								{
									ptr_attr = (long*) (ptr_record + pfps[i].offset);
									ptr_record += tps.record_size;

									CCommon::long_to_keyid(*ptr_attr, &key_stru);

									if ((key_stru.key - pre_key) > 1)
									{
										key = pre_key + 1;
										break;
									}
									else
									{
										pre_key = key_stru.key;
									}
								}

								if (key == 0) //表里的记录连续
								{
									key = pre_key + 1;
								}

								BufFree(buf);
							}

							key_stru.key = key;

							CCommon::keyid_to_long(&key_stru, &kid);

							return kid;
						}
					}
					else
					{
						KEY_STRU key_stru;
						key_stru.key = pos;
						key_stru.field_id = pfps[i].field_no;
						key_stru.table_no = tb_descr->table_id;
						unsigned long kid = 0;
						CCommon::keyid_to_long(&key_stru, &kid);

						return kid;
					}
				}
			}

			cout << "no keyword field!" << endl;
			BufFree(pfps);

			return 0;
		}
		else
			return 0;

	}
	else
		return 0;
}
// 目前没有使用，没有调试。
int GetKeyfieldNo(TB_DESCR *tb_descr)
{
	int ret;
	FIELD_PARA_STRU *pfps = NULL;
	ret = GetFieldsPara(tb_descr, &pfps);
	if (ret >= 0)
	{
		int cnt = ret / sizeof(FIELD_PARA_STRU);

		for (int i = 0; i < cnt; i++)
		{
			if (pfps[i].is_keyword)
			{
				return pfps[i].field_no;
			}
		}

		BufFree(pfps);
	}

	return -1;
}

bool HasMask(const TB_DESCR *tb_descr, const char *fieldname, const int fieldvalue, int mask)
{
	assert(tb_descr != NULL);
	assert(fieldname!=NULL);

	int field_no[1];
	int fieldnum = 1;

	int ret_code = GetFieldNoByName(const_cast<TB_DESCR*>(tb_descr), const_cast<char*>(fieldname), field_no, &fieldnum);
	if (ret_code < 0)
	{
		TRACE("表%s可能不存在域%s\r\n", tb_descr->table_name, fieldname);
		return false;
	}

	FIELD_PARA_STRU fps = { 0 };

	ret_code = GetFieldParaByID(const_cast<TB_DESCR*>(tb_descr), field_no[0], &fps);
	if (ret_code < 0)
	{
		TRACE("读取表%s的%s域参数失败，域号%d\r\n", tb_descr->table_name, fieldname, field_no[0]);
		return false;
	}

	if (fps.display_type == D_SINGLEMENU_TYPE)
	{
		return fieldvalue == mask;
	}
	else if (fps.display_type == D_MUTIMENU_TYPE)
	{
		return (fieldvalue >> (mask - 1)) & 1 == 1;
	}

	TRACE("表%s的%s域非菜单类型!\r\n", tb_descr->table_name, fieldname);

	return false;
}
