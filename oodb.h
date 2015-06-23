/*
 * oodb.h
 *
 *  Created on: 2012-10-26
 *      Author: BetaKDE
 */

#ifndef OODB_H_
#define OODB_H_

#include <stdlib.h>
#include <rtdb_api.h>

extern int _oodb_errno;

#ifndef NULL
#define NULL 0
#endif

#ifndef OO_FAIL
#define OO_FAIL -1
#endif

#ifndef OO_SUCCEED
#define OO_SUCCEED	0
#endif

typedef struct _DB_DESCR
{
	_DB_DESCR()
	{
		bzero(ds_name, sizeof(ds_name));
		bzero(sys_name, sizeof(sys_name));
		bzero(bob_name, sizeof(bob_name));
		bzero(server, sizeof(server));
	}

	char ds_name[256];
	char sys_name[256];
	char bob_name[256];
	char server[256];
	int mode;
} DB_DESCR;

void dberr(int err);
int oodbinit();
int oodbopen(char *ds_name, char *sys_name, char *bob_name, char *server, int mode, DB_DESCR *db_descr);
int oodbclose(char *ds_name, char *sys_name, char *bob_name, char *server, DB_DESCR *db_descr);
int oodbuse(char *tb_name, DB_DESCR *db_descr, TB_DESCR *tb_descr);
int oodbread_rph(void *buffer, int s_pos, TB_DESCR *tb_descr, int length);
int oodbread_rk(void *buffer, void *key_value, TB_DESCR *tb_descr, int length);
int oodbread_ra(void *buffer, char *attr_name, void *attr_value, TB_DESCR *tb_descr, int length);
int oodbget_tabnum(DB_DESCR *db_descr);
int oodbget_tbname(DB_DESCR *db_descr, char **tb_name);
int oodbread_rp(void *buffer, int s_pos, TB_DESCR *tb_descr, int number,int length);
int oodbread_aa(void *buffer, char *attr_name1,  void *attr_value,char *attr_name2, TB_DESCR *tb_descr, int length);
int oodbread_ap(void *buffer, int s_pos, char *attr_name, TB_DESCR *tb_descr, int  number,int length);
int oodbread_ak(void *buffer, void *key_value, char *attr_name, TB_DESCR *tb_descr,int length);
int oodbget_ph(TB_DESCR *tb_descr, void *key_value);
int oodbupdate_ak(void *buffer, void *key_value, char *attr_name,TB_DESCR *tb_descr, int length);
int oodbupdate_rk(void *buffer, void *key_value, TB_DESCR *tb_descr, int   length);
int oodbinsert(void *buffer, TB_DESCR *tb_descr, int number, int length);
int oodbget_recnum(TB_DESCR *tb_descr);
unsigned long get_id(TB_DESCR *tb_descr, int pos = -1);	//自动生成表关键字
int GetKeyfieldNo(TB_DESCR *tb_descr);
int GetTableSize(const TB_DESCR *tb_descr);
int oodbget_attrlen(TB_DESCR *tb_descr, char *attr);
int oodbget_attrtype(TB_DESCR *tb_descr, char *attr);
bool HasMask(const TB_DESCR *tb_descr, const char *fieldname, const int fieldvalue, int mask);//对单选菜单和多选菜单类型的值进行判断
#endif /* OODB_H_ */
