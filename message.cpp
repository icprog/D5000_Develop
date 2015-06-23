/*
 * message.cpp
 *
 *  Created on: 2015-5-11
 *      Author: admin
 */
#include <iostream>
#include <dcisg.h>
#include <rtdb_api.h>
#include <msg_api/message_inv.h>
#include <msg_api/messageheader.h>
#include <message_channel.h>
#include <proc_inv.h>
#include <message_type.h>
#include <priv_access_new.h>
#include <scada_api/scada_event_m.h>
#include <scada_api/scada_front_struct.h>
#include <scada_api/CScaComm.h>
#include <warn_service_message_m.h>

message_invocation  *bob_inv = NULL;
proc_invocation *proc_inv = NULL;
int rte_connect(int argc, char **argv)
{
	cout << "开始连接RTE..." << endl;
	//D5000连接消息总线
	int iProcess = proc_inv->proc_init(argv[1], argv[2], argv[3]);

	if (iProcess >= 0)
	{
		TRACE("进程管理初始化成功，本进程号为:%d\r\n", iProcess);

		int iMessage = bob_inv->messageInit(argv[1], argv[2], argv[3]);

		if (iMessage >= 0)
		{
			//for (int i = 0; i <= 255; i++)
			//	bob_inv->messageSubscribe(i);
			bob_inv->messageSubscribe(APP_TO_WARN_CHANNEL);
			bob_inv->messageSubscribe(WARN_INFORM_CHANNEL);
		}
		else
		{
			TRACE("连接消息总线失败！\r\n");
			return 0;
		}
	}
	else
	{
		TRACE("进程管理初始化失败！\r\n");
		return 0;
	}

	return 1;
}
int main(int argc, char *argv[])
{
	int ret;
	bob_inv = new message_invocation;
	proc_inv = new proc_invocation;

	rte_connect(argc, argv);
	Message message = { 0 };
	RealDataPkg pkg={0};
	ChangeYx yx = { 114560487319931434, true, true, 222, -1 };

//	memcpy(pkg.mes, &yx, sizeof(ChangeYx));
//	struct timeval 		g_cur_time;
//	gettimeofday(&g_cur_time,NULL);
//
//	pkg.package_head.data_num=1;
//	pkg.package_head.package_type = MT_YC_CHANGE;
//	pkg.package_head.second = time(NULL);
//	pkg.package_head.msecond = 0;

	message.header.event = MT_YX_CHANGE;
	message.header.serv = D_CH_UP_REAL_DATA;

//	memcpy(message.Msg_buf, &pkg, sizeof(pkg));
	memcpy(message.Msg_buf, &yx, sizeof(ChangeYx));

	for(int i = 0; i < 1; i++)
	{
		cout << "bob_inv->messageSend,return:" << bob_inv->messageSend(&message, sizeof(message.header) /*+ sizeof(pkg.package_head)*/+sizeof(yx), NULL) << endl;
		sleep(5);
	}

	return 0;
}



