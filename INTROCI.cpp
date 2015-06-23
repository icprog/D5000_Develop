/*
 * INTROCI.cpp
 *
 *  Created on: 2012-11-14
 *      Author: hp
 */

#include "INTROCI.h"

INTR_OCI::INTR_OCI()
{
	buf_ptr = NULL;
	attrs_t = NULL;
	pattrs	= NULL;
	// TODO Auto-generated constructor stub

}

INTR_OCI::~INTR_OCI()
{
	// TODO Auto-generated destructor stub
}

int INTR_OCI::ReadWithBind(const char *query, char **buf, int *rec_num, int *attr_num, struct ORA_ATTR** attrs)
{
	int ret;

	ColAttr_t *attrs_1;
	int rec_num_tmp, attr_num_tmp;
	ORA_ATTR* att = NULL;
	char *buf_tmp = NULL;

	if(NULL==query || NULL==buf || NULL==rec_num || NULL==attr_num || NULL==attrs) {

		return OCI_ERROR;
	}
	ret = g_CDci.ReadData(query, &rec_num_tmp, &attr_num_tmp, &attrs_1, &buf_tmp, &err);
	if(true != ret) {
		printf("%s(%d):ReadData failed, errno:%d, error:%s\r\n", __FILE__, __LINE__, err.error_no, err.error_info);
		return OCI_ERROR;
	}
	att = (ORA_ATTR*)malloc(sizeof(ORA_ATTR)*attr_num_tmp);
	if(NULL == att) {
		g_CDci.FreeReadData(attrs_1, attr_num_tmp, buf_tmp);
	}
	// 给出参赋值
	for(int i=0; i<attr_num_tmp; i++) {
		att[i].col_width	= attrs_1[i].data_size;
		att[i].dtype	 	= attrs_1[i].data_type;
		strcpy(att[i].id, attrs_1[i].col_name);
	}

	*attrs 	= att;
	*buf	= buf_tmp;
	*rec_num = rec_num_tmp;
	*attr_num = attr_num_tmp;

	//保存变量为了后面释放内存操作
	attrs_t = attrs_1;
	Cur_colnum = attr_num_tmp;
	buf_ptr = buf_tmp;
	pattrs	= att;

	return OCI_SUCCEED;
}

void INTR_OCI::Readdata_Free()
{
	if(NULL != buf_ptr || NULL != attrs_t) {
		g_CDci.FreeReadData(attrs_t, Cur_colnum, buf_ptr);
		buf_ptr = NULL;
		attrs_t = NULL;
	}
	if(NULL != pattrs) {
		free(pattrs);
		pattrs = NULL;
	}
}

// 成功返回true ，错误返回false
int INTR_OCI::connect(const char *username, const char *password, const char *server_name)
{
	int ret_code;
	//ErrorInfo_t err;
	ret_code = g_CDci.Connect(server_name, username, password, &err);
	if(0 == ret_code) {
		printf("%s(%d):Connect failed, errno:%d, error:%s\r\n", __FILE__, __LINE__, err.error_no, err.error_info);
		return OCI_ERROR;
	}
	return OCI_SUCCEED;

}
int INTR_OCI::Exec_noBind(const char *sqlstr)
{
	//ErrorInfo_t err;
	int ret;
	if(NULL == sqlstr) {
		return OCI_ERROR;
	}

	ret = g_CDci.ExecSingle(sqlstr, &err);
	if(0 == ret) {
		printf("%s(%d):ExecSingle failed, errno:%d, error:%s\r\n", __FILE__, __LINE__, err.error_no, err.error_info);
		return OCI_ERROR;
	}
	return OCI_SUCCEED;
}
void INTR_OCI::disconnect(void)
{
	//ErrorInfo_t  err;
	int ret_code = 0;

	ret_code = g_CDci.DisConnect(&err);
	// 异常处理怎么处理？ C2000的接口没有处理。
}
