/*
 * INTROCI.h
 *
 *  Created on: 2012-11-14
 *      Author: hp
 */

#include "dcisg.h"

#ifndef INTROCI_H_
#define INTROCI_H_

#ifndef OCI_ERROR
#define OCI_ERROR -1
#define OCI_SUCCEED 0
#endif

const int INTR_ID_LEN = 256;
typedef struct ORA_ATTR
{
	char id[INTR_ID_LEN];
	int dtype;
	int col_width;
} ORA_ATTRS;

class INTR_OCI
{
public:
	INTR_OCI();
	virtual ~INTR_OCI();

public:
	int connect(const char *username, const char *password, const char *server_name);
	int Exec_noBind(const char *sqlstr);
	void disconnect(void);
	int ReadWithBind(const char *query, char **buf, int *rec_num, int *attr_num, struct ORA_ATTR** attrs);
	void Readdata_Free();
public:
	ErrorInfo_t err;
private:
	CDci 		g_CDci;
	char 		*buf_ptr; 	// Ϊd5000�ӿ� free ������¼����ʱָ��
	ColAttr_t	*attrs_t; 	// Ϊd5000�ӿ� free ������¼����ʱָ��
	int			Cur_colnum;	// Ϊd5000�ӿ� free ������¼����ʱ����
	ORA_ATTR*	pattrs;   	// C2000ԭ�ӿ����õ������ͣ���Free��������Ҫ�ͷ����ڴ档

};

#endif /* INTROCI_H_ */
