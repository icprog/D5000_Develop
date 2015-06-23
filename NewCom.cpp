/*
 * NewCom.cpp
 *
 *  Created on: 2014年6月7日
 *      Author: Jason.Yang
 *      功能描述：改写DA前后台交互方式，以服务总线实现
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
#include <warn_service_message_m.h>
#include "dsbob.h"
#include "dsbob2.h"
#include "Nmd.h"
#include "Tp.h"
#include "dms_tcp.h"
#include "tinyxml.h"
#include "FDIRR2.h"
#include "comm.h"
//#include <commdata.h>
#include "INTROCI.h"
#include "DBPSBOB.h"
#include "DBDMS.h"
#include "DBFDIR.h"
#include "FHZG.h"
#include "typical.h"
#include "serviceglobal.h"
#include "services.h"


using namespace std;

#define SRV_BUS_ID 9294
ServiceInfo srv;
#define SRVBUS_PORT 9812
using namespace std;
extern thread_mutex_t	g_rq_mutex;


typedef struct
{
	int port;
	int msglen;
	int time_out;
	int publish_times;
}Serv_para;


static int		buflen;
static int		time_out_in = 0;

const char headcolorxml[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						<Message>\r\n\
						<Header type = \"Equipment_Color\"/>\r\n\
						<Body>\r\n";
//设备着色响应报文尾
const char tailcolorxml[] = "</Order>\r\n\
						</Body>\r\n\
						</Message>\r\n\
						";
//响应故障执行步骤方案
const char headfdirstepxml[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						 <Message>\r\n\
						 <Header type = \"FDIR_STEP\"/>\r\n\
						 <Body>\r\n";
//设备着色响应报文尾
const char tailfdirstepxml[] = "</Order>\r\n\
							</Body>\r\n\
							</Message>\r\n\
							";


const char head_xml4[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						 <Message>\r\n\
						 <Header type = \"study_fdir\"/>\r\n\
						 <Body>\r\n";

const char tail_xml[] = "</Order>\r\n\
						</Body>\r\n\
						</Message>\r\n\
						";
//防误闭锁判断响应报文头
const char head_xml2[] = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						 <Message>\r\n\
						 <Header type=\"misoperation\"/>\r\n\
						 <Body>\r\n";
//防误闭锁判断响应报文尾
const char tail_xml2[] = "</Body>\r\n\
						 </Message>\r\n\
						 ";
const char head_xml3[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						 <Message>\r\n\
						 <Header type = \"FHZG\"/>\r\n\
						 <Body>\r\n";
//响应典型运行方式报文
const char head_xml6[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						 <Message>\r\n\
						 <Header type = \"typical\"/>\r\n\
						 <Body>\r\n";


//响应数据校验报文
const char head_xml7[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						 <Message>\r\n\
						 <Header type = \"check_data\"/>\r\n\
						 <Body>\r\n";


//响应故障重新计算报文
const char head_xml8[] = "<?xml version = \"1.0\" encoding=\"ISO-8859-1\" ?>\r\n\
						 <Message>\r\n\
						 <Header type = \"recalc\"/>\r\n\
						 <Body>\r\n";

void* DispatchCallBack(char* requestBuffer, int requestlen, char** responseBuffer, int* responselen)
{
	cout<<"********Eenter Dispatch Thread!********"<<endl;
	pthread_t threadID = pthread_self();
	cout<<"前后台交互线程ID："<<threadID<<endl;

	if (responselen <= 0)
	{
		cout<<"接收的缓存数据长度为0！"<<endl;
		return 0;
	} 
	else if(requestBuffer == NULL)
	{
		cout<<"缓存区地址为空！"<<endl;
		return 0;
	}
	else
	{
		struct timeval tpstart, tpend;
		long iTimeInterval;
		int msgType = 0;
		timeval stimer = {0};
		time_t now = 0;

		FILE *pfileRecv = NULL;
		char filename[256];

		//接收到新的XML格式请求信息，创建新的临时文件储存接下来收到的XML文件数据
		if(strncasecmp(requestBuffer, "<?xml", 5) == 0)
		{
			cout << ">>>>>>>>"<<ctime(&(now = time(NULL)))<<"new request!" << endl;

			gettimeofday(&stimer, NULL);

			pfileRecv = fopen(filename, "w+");

			if (pfileRecv == NULL)
			{
				cout << "创建临时XML文件失败，" << strerror(errno) << endl;
				return (void*)-1;
			}
		}

		//将收到的数据写入临时文件
		if (pfileRecv > 0)
			fputs(requestBuffer, pfileRecv);
		else
			cout << "错误的请求报文数据：" << filename << endl;

		TiXmlDocument *pXmlDoc = new TiXmlDocument();

		sprintf(filename, "dms_servicebus_%d.xml", threadID);
		//XML传送结束
		if (strstr(requestBuffer, "</Message>") != NULL)
		{
			try
			{
				fclose(pfileRecv);
			} catch (...)
			{
			}
			pfileRecv = NULL;

			MAP_SO_PATH paths;
			ORDER order = {0};
			//char ret_buf[1024] = { 0 };

			paths.clear();

			cout << "下行报文结束！" << endl;

			int cmd = 0;


			//XML解析器载入临时XML文档并解析
			if (pXmlDoc->LoadFile(filename))
			{
				try
				{
					unlink(filename); //删除临时文件
				} catch (...)
				{
				}

				TiXmlNode *pNode = pXmlDoc->FirstChild(); //xml版本
				FILE* pFile = NULL;

				//将上一次的数据清空
				pFile = fopen("remessage.xml","w");
				if (pFile == NULL)
				{
					cout<<"报文响应文件打开或创建失败！"<<endl;
					cout<<"Error No.:"<<errno<<endl;
					char* pErrMsg = strerror(errno);
					cout<<"Error Message:"<<pErrMsg<<endl;
					fclose(pFile);
					return (void*)-1;
				} 

				while ((pNode = pNode->NextSibling()) != NULL)
				{
					TiXmlElement* pElement = pNode->ToElement();


					//查找到message节点
					if (pElement != NULL && strcasecmp(pElement->Value(),"Message") == 0)
					{
						TiXmlNode* pMsgNode = pNode->FirstChild();
						char strHead[100] = {0};//请求类型

						while (pMsgNode != NULL)
						{
							TiXmlElement* pMsgElement = pMsgNode->ToElement();
							if (strcasecmp(pMsgElement->Value(), "Header") == 0)		//Header节点
							{
								strncpy(strHead, pMsgElement->Attribute("type"), 100);
							}
							else if (strcasecmp(pMsgElement->Value(), "Body") == 0)//Body节点
							{
								TiXmlNode* pBodyNode = pMsgNode->FirstChild();
								while (pBodyNode != NULL)
								{
									TiXmlElement* pBodyElement = pBodyNode->ToElement();

									//设备着色
									if (strcasecmp(strHead, "Equipment_Color") == 0)
									{
										if (strcasecmp(pBodyElement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.equipment_type, pBodyElement->Attribute("equipment_type"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));
											strcpy(order.st, pBodyElement->Attribute("st"));

											cmd = atoi(pBodyElement->Attribute("equipment_type"));

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											//更新开关状态,并根据母线的遥测信息添加SO
											if (nmd->update_cb_state() && nmd->add_so()/* && nmd->DoValidate() && g_bob2->WriteReDB(NULL, NULL, NULL)*/)
											{
												TiXmlNode *pordernode = pBodyNode->FirstChild();

												//循环查找Record项,并处理
												while (pordernode != NULL)
												{
													TiXmlElement *porderelement = pordernode->ToElement();

													if (strcasecmp(porderelement->Value(), "Record") == 0)
													{
														switch (cmd)
														{
														case 1: //电源跟踪
															{
																nmd->find_so(atol(porderelement->Attribute("id")), porderelement->Attribute("tablename"), paths, FIND_TYPE_SO);

																break;
															}
														case 2: //供电范围
															{
																nmd->find_so(atol(porderelement->Attribute("id")), porderelement->Attribute("tablename"), paths, FIND_TYPE_SO_AREA);

																break;
															}
														case 100: //检查设备连接状态
															{
																nmd->find_so(atol(porderelement->Attribute("id")), porderelement->Attribute("tablename"), paths, FIND_TYPE_CONNECT);

																break;
															}

														default:
															break;
														}

													}
													else
													{
													}

													pordernode = pordernode->NextSibling();
												}
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{

										}

										//发送请求响应数据
										cout<<headcolorxml<< endl;

										//创建响应报文文件
										pFile = fopen("remessage.xml","a");
										if (pFile == NULL)
										{
											cout<<"报文响应文件打开或创建失败！"<<endl;
											cout<<"Error No.:"<<errno<<endl;
											char* pErrMsg = strerror(errno);
											cout<<"Error Message:"<<pErrMsg<<endl;
											fclose(pFile);
											return (void*)-1;
										} 
										else
										{
											long lWtNum = fwrite(headcolorxml, sizeof(char), sizeof(headcolorxml), pFile);
											//移动到文件尾
											fseek(pFile,0, SEEK_END);
											char strContext[128];
											sprintf(strContext,"<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" equipment_type = \"%s\" >\r\n", order.picturename,
												order.contextname, order.bobname, order.st, order.equipment_type);
											//插入报文
											lWtNum = fwrite(strContext, sizeof(char), sizeof(strContext), pFile);
										}


										//将设备列表里的设备信息写入回执响应文件中
										for (MAP_SO_PATH::iterator it = paths.begin(); it != paths.end(); it++)
										{
											while (it->second.path.size() > 0)
											{
												char strBuff[64];
												sprintf(strBuff, "<Record id = \"%ld\" tablename = \"%s\" />\r\n", it->second.path.top().id, it->second.path.top().table.c_str());
												//移动到文件尾
												fseek(pFile,0, SEEK_END);
												//插入报文
												long lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);


												if(it->second.path.top().table == "busbarsection" || it->second.path.top().table == "energyconsumer")
												{
													int field_no[2] = { 0, 0 };
													int ret = 0;
													int field_num = 2;

													ret = GetFieldNoByName(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), "nd,name", field_no, &field_num);
													if (ret >= 0)
													{

														struct
														{
															long nd;
															char name[64];
														} value;

														ret = GetFieldsByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), (char*) &it->second.path.top().id, field_no,
															field_num, sizeof(value), (char*) &value);
														if (ret == 0)
														{
															TRACE("nd:%ld,name:%s\r\n", value.nd, value.name);
														}
													}

												}
												else
												{
													int field_no[3] = { 0, 0, 0 };
													int ret = 0;
													int field_num = 3;

													ret = GetFieldNoByName(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), "ind,jnd,name", field_no, &field_num);
													if (ret >= 0)
													{

														struct
														{
															long ind;
															long jnd;
															char name[64];
														} value;

														ret = GetFieldsByID(const_cast<TB_DESCR*>(g_db_psbob->GetTB(it->second.path.top().table.c_str())), (char*) &it->second.path.top().id, field_no,
															field_num, sizeof(value), (char*) &value);
														if (ret == 0)
														{
															TRACE("ind:%ld,jnd:%ld,name:%s\r\n", value.ind, value.jnd, value.name);
														}
													}
												}
												it->second.path.pop();

											}
										}

										cout<<tailcolorxml<<endl;
										//移动到文件尾
										fseek(pFile,0, SEEK_END);
										//插入报文尾
										long lWtNum = fwrite(tailcolorxml, sizeof(char), sizeof(tailcolorxml), pFile);

									}
									else if (strcasecmp(strHead,"fdir_step") == 0)
									{
										char fault_id[100] = {0};
										char fault_machine[100] = { 0 };
										if (strcasecmp(pBodyElement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));
											strcpy(order.st, pBodyElement->Attribute("st"));
											strcpy(fault_machine, pBodyElement->Attribute("machine"));
											strcpy(fault_id, pBodyElement->Attribute("fault_id"));

											int ret = FDIR_R_FAIL;

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											sprintf(sql, "DELETE FROM FDIR_STEP WHERE ID =\'%s\' and status=0", fault_id);
											ExecSQL(sql);

											TiXmlNode *pordernode = pBodyNode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													sprintf(sql, "Insert into FDIR_STEP (ID, CB_ID, OPERATION, STEP, STATUS, CONTROL, STAGE) Values(\'%s\', \'%s\', %s, %s, 0, %s, %s)"
														,fault_id, porderelement->Attribute("cbId"), porderelement->Attribute("operation")
														,porderelement->Attribute("step"), porderelement->Attribute("control"), porderelement->Attribute("stage"));
													ExecSQL(sql);
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										UpdateExtStatus(fault_id);

										//发送XML响应头
										cout << headfdirstepxml << endl;
										//创建响应报文文件
										pFile = fopen("remessage.xml","a");
										if (pFile == NULL)
										{
											cout<<"报文响应文件打开或创建失败！"<<endl;
											cout<<"Error No.:"<<errno<<endl;
											char* pErrMsg = strerror(errno);
											cout<<"Error Message:"<<pErrMsg<<endl;
											fclose(pFile);
											return (void*)-1;
										} 
										else
										{
											long lWtNum = fwrite(headcolorxml, sizeof(char), sizeof(headcolorxml), pFile);
											//移动到文件尾
											fseek(pFile,0, SEEK_END);
											char strContext[128];

											//请求上下文信息
											sprintf(strContext, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" machine=\"%s\">\r\n", order.picturename,
												order.contextname, order.bobname, order.st, fault_machine);
											//插入报文
											lWtNum = fwrite(strContext, sizeof(char), sizeof(strContext), pFile);
										}

										char strBuff[64];
										//逐项发送设备列表里的设备信息
										sprintf(strBuff, "<Record id = \"%s\" result = \"0\" error=\"\"/>\r\n", fault_id);
										//移动到文件尾
										fseek(pFile,0, SEEK_END);
										//插入报文
										long lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);
										cout << strBuff << endl;

										cout << tailfdirstepxml << endl;
										//插入报文尾
										long lWtNum = fwrite(tailfdirstepxml, sizeof(char), sizeof(tailfdirstepxml), pFile);
										
									}
									else if (strcasecmp(strHead, "study_fdir") == 0)
									{
										map<string, string> map_fault_group;
										list<string>  lst_group;
										list<string>  lst_remove_group;//故障模拟完成后需要从内存中删除的故障id

										char fault_id[100] = {0};
										long fault_sec;
										char fault_table[50] = { 0 };
										char fault_machine[100] = { 0 };
										if (strcasecmp(pBodyElement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));
											strcpy(order.st, pBodyElement->Attribute("st"));
											strcpy(fault_machine, pBodyElement->Attribute("machine"));

											int ret = FDIR_R_FAIL;

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											pthread_mutex_lock(&task_list_mutex);
											TiXmlNode *pordernode = pBodyNode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													fault_sec = atol(porderelement->Attribute("id"));
													strcpy(fault_table, porderelement->Attribute("tablename"));

													FDIR_TASK data_task;
													PSBOB::DV dv = { 0 };
													PSBOB::CB cb = { 0 };

													//获取故障指示器从属的设备
													if (strcasecmp(fault_table, "faultinfo") == 0)
													{
														PSBOB::FI faultinfo = { 0 };
														if (oodbread_rk(&faultinfo, &fault_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("faultinfo")), sizeof(PSBOB::FI)) < 0)
														{
															TRACE("oodbread_rk(%s) failed!\r\n", fault_sec);
														}
														else
														{
															GetDv(faultinfo.aclnseg_id, "sec", dv);
															data_task.task_data.fault.st = faultinfo.st_id;
														}
													}
													else
													{
														//模拟故障的设备是cb
														if (/*strcasecmp(fault_table, "fi") == 0 || */strcasecmp(fault_table, "cb") == 0)
														{
															if (oodbread_rk(&cb, &fault_sec, const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(PSBOB::CB)) < 0)
															{
																TRACE("oodbread_rk(%s) failed\r\n", fault_sec);
															}
															else
															{
																data_task.task_data.fault.st = cb.st_id;

																if (cb.brk_type == MENU_BRK_TYPE_JX)
																{
																	GetJxcbDv(cb.id, dv);
																}
																else
																{
																	GetDv(cb.id, "cb", dv);
																}
															}
														}
														else
														{
															GetDv(fault_sec, fault_table, dv);
														}
													}

													if (dv.id == 0)
													{
														int fdir_damode = GetDvDamode(dv.id);
														if (fdir_damode == -1)
														{
															cout << "获取馈线的故障处理模式失败" << dv.id << endl;
															pordernode = pordernode->NextSibling();
															continue;
														}
														//禁止处理故障
														if (fdir_damode == FDIR_MODE_FORBID)
														{
															pordernode = pordernode->NextSibling();
															continue;
														}
														GetFdirConfig(g_FdirConfig);
														data_task.rq_type = TASK_TYPE_FAULT;
														gettimeofday(&data_task.tv_creat, NULL);

														data_task.task_data.fault.next_step = TASK_FAULT_STATUS_CALC_ISO;
														if (g_FdirConfig.AUTO_DEAL_HIS)
														{
															data_task.task_data.fault.bAuto = 2;
														}
														else
														{
															data_task.task_data.fault.bAuto = fdir_damode == FDIR_MODE_AUTO ? 1 : 0;
														}

														data_task.task_data.fault.dv = dv.id;
														data_task.task_data.fault.type = TASK_FAULT_STUDY;
														data_task.task_data.fault.head.type = 0;
														data_task.task_data.fault.head.size = sizeof(FDIR_TASK_DATA_FAULT);
														data_task.task_data.fault.point_name = atol(porderelement->Attribute("id"));

														tm *ptm = localtime(&data_task.tv_creat.tv_sec);
														sprintf(data_task.task_data.fault.head.fault_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday,
															ptm->tm_hour, ptm->tm_min, ptm->tm_sec, data_task.tv_creat.tv_usec);

														strcpy(fault_id, data_task.task_data.fault.head.fault_id);

														cout << "新故障,故障ID:" << data_task.task_data.fault.head.fault_id << endl;

														gettimeofday(&data_task.tv_start, NULL);

														if (!AddTask(&data_task))
														{
															cout << "添加新故障任务失败" << endl;
														}
														else
														{
															if (g_lstTask.back().task_data.fault.fdir_r->ExecFdir(fault_sec, fault_table) == FDIR_R_SUCCEED)
															{
																map_fault_group[fault_id] = "";
																cout << "模拟故障成功" << endl;
															}
															else
															{
																sprintf(sql,
																	"Insert into FDIR_FDIRECT (ID, GROUPID, TYPE, FD, FD_DESCR, STATUS,STUDY_EQ,STUDY_MACHINE) Values (\'%s\', \'%s\', 1, \'%ld\', \'%s\', %d, \'%ld\',\'%s\')",
																	fault_id, fault_id, dv.id, dv.name, STATUS_FDIRECT_DIRECT_FAIL, fault_sec, fault_machine);
																ExecSQL(sql);

																RemoveTask(fault_id);
															}
														}
													}
													else
													{
														cout << "获取馈线信息失败" << porderelement->Attribute("id") << endl;
													}
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&task_list_mutex);
											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										map<string, string>::iterator it_map_fault_group;

										while (true)
										{
											pthread_mutex_lock(&task_list_mutex);

											for (list<FDIR_TASK>::iterator it = g_lstTask.begin(); it != g_lstTask.end(); it++)
											{
												if (it->rq_type != TASK_TYPE_FAULT)
													continue;
												if (it->task_data.fault.next_step != TASK_FAULT_STATUS_ISO)
													continue;

												it_map_fault_group = map_fault_group.find(it->task_data.fault.head.fault_id);

												if (it_map_fault_group != map_fault_group.end())
												{
													it_map_fault_group->second = it->task_data.fault.head.group_id;

													if (it->task_data.fault.bAuto == 0)
													{
														//计算完成后把故障从任务列表的内存中删除，在执行的时候再把内存数据恢复
														lst_remove_group.push_back(it->task_data.fault.head.fault_id);
													}
												}
											}

											pthread_mutex_unlock(&task_list_mutex);

											for (it_map_fault_group = map_fault_group.begin(); it_map_fault_group != map_fault_group.end(); it_map_fault_group++)
											{
												if(it_map_fault_group->second.size() < 10) break;
												else
												{
													lst_group.push_back(it_map_fault_group->second);
												}
											}

											if (it_map_fault_group == map_fault_group.end())
											{
												break;
											}

											sleep(1);
										}

										lst_group.sort();
										lst_group.unique();

										string str_group;

										for(list<string>::iterator it_lst_group = lst_group.begin(); it_lst_group != lst_group.end(); it_lst_group++)
										{
											str_group += *it_lst_group;
											str_group += ";";

											sprintf(sql, "UPDATE FDIR_FDIRECT SET TYPE=1,STUDY_MACHINE=\'%s\' WHERE GROUPID=\'%s\'", fault_machine, it_lst_group->c_str());
											ExecSQL(sql);
										}

										pthread_mutex_lock(&task_list_mutex);
										for (list<string>::iterator it_lst = lst_remove_group.begin(); it_lst != lst_remove_group.end(); it_lst++)
										{
											RemoveTask(it_lst->c_str());
										}
										pthread_mutex_unlock(&task_list_mutex);

										//发送请求响应数据
										cout << head_xml4 << endl;

										//创建响应报文文件
										pFile = fopen("remessage.xml","a");
										if (pFile == NULL)
										{
											cout<<"报文响应文件打开或创建失败！"<<endl;
											cout<<"Error No.:"<<errno<<endl;
											char* pErrMsg = strerror(errno);
											cout<<"Error Message:"<<pErrMsg<<endl;
											fclose(pFile);
											return (void*)-1;
										} 
										else
										{
											long lWtNum = fwrite(head_xml4, sizeof(char), sizeof(head_xml4), pFile);
											//移动到文件尾
											fseek(pFile,0, SEEK_END);
											char strContext[128];
											sprintf(strContext, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" machine=\"%s\">\r\n", order.picturename,
												order.contextname, order.bobname, order.st, fault_machine);
											//插入报文
											lWtNum = fwrite(strContext, sizeof(char), sizeof(strContext), pFile);
											cout << strContext << endl;
										}

										char strBuff[64];
										sprintf(strBuff, "<Record fault_id=\"%s\" restult = \"0\" error=\"0\"/>\r\n", str_group.c_str());

										cout << strBuff << endl;
										//移动到文件尾
										fseek(pFile, 0, SEEK_END);
										long lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);

										//发送XML响应尾
										cout << tail_xml << endl;
										fseek(pFile, 0, SEEK_END);
										lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);

									}
									else if (strcasecmp(strHead, "pntcontrol") == 0)  //故障隔离恢复遥控
									{
										if (strcasecmp(pBodyElement->Value(), "operation") == 0)
										{
											TiXmlNode *pordernode = pBodyNode->FirstChild();

											//循环查找order项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "order") == 0)
												{
													if (strcmp(porderelement->Attribute("op_type"), "finish") == 0)	//故障处理完成
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));
													}
													else if (strcmp(porderelement->Attribute("op_type"), "auto") == 0)//手工转自动
													{
														//														if (!g_FdirConfig.USER2AUTO)
													}

														cout << "**********手动转自动执行" << endl;

														//
														pthread_mutex_lock(&task_list_mutex);

														list<FDIR_TASK>::iterator it;
														for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
														{
															if (it->rq_type == TASK_TYPE_FAULT)
															{
																if (strcmp(it->task_data.fault.head.group_id, porderelement->Attribute("fault_id")) == 0)
																{
																	it->task_data.fault.bAuto = 1;
																	break;
																}
															}
														}

														//如果不在任务列表，把故障重新加载到内存中，然后再把故障详细加载进去
														if (it == g_lstTask.end())
														{
															RecoMemTask(porderelement->Attribute("fault_id"));
															RecoTaskStep(porderelement->Attribute("fault_id"));
															for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
															{
																if (it->rq_type == TASK_TYPE_FAULT)
																{
																	if (strcmp(it->task_data.fault.head.group_id, porderelement->Attribute("fault_id")) == 0)
																	{
																		it->task_data.fault.bAuto = 1;
																	}
																}
															}
														}

														list<CB_CONTROL> lstcb;

														GetLeftStep(porderelement->Attribute("fault_id"), lstcb);

														//还有步骤执行
														if (lstcb.size() > 0)
														{
															AddControlItem(porderelement->Attribute("fault_id"), lstcb.begin()->point_id, lstcb.begin()->bOpen, lstcb.begin()->stage);
														}
														//
														else
														{
														}

														pthread_mutex_unlock(&task_list_mutex);
													}
													else if(strcmp(porderelement->Attribute("op_type"), "stop") == 0)	//人工停止故障处理
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));
														//更新故障状态为人工停止故障处理,首先判断故障信息是否已经转入his表里
														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_STOP, porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_STOP, porderelement->Attribute("fault_id"));
															ExecSQL(sql);
														}

														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);

														RemoveTasks(porderelement->Attribute("fault_id"));

														pthread_mutex_unlock(&task_list_mutex);
													}
													else if(strcmp(porderelement->Attribute("op_type"), "manposer") == 0) //人工处理故障完成
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));

														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d, USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", STATUS_FDIRECT_MANPOSER,
															porderelement->Attribute("userName"), porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d, USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", STATUS_FDIRECT_MANPOSER,
																porderelement->Attribute("userName"), porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
															ExecSQL(sql);

															//更新故障步骤状态为未知-1
															sprintf(sql, "UPDATE FDIR_STEP SET STATUS=%d WHERE ID=\'%s\'", STATUS_FDIRECT_UNKOWN, porderelement->Attribute("fault_id"));
															ExecSQL(sql);

															//更新故障步骤状态为未知-1
															sprintf(sql, "UPDATE FDIR_STEP_HIS SET STATUS=%d WHERE ID=\'%s\'", STATUS_FDIRECT_UNKOWN, porderelement->Attribute("fault_id"));
															ExecSQL(sql);
														}

														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);

														RemoveTasks(porderelement->Attribute("fault_id"));

														pthread_mutex_unlock(&task_list_mutex);

													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "yes_Fault") == 0) //误报、可能故障确认为真实故障
													{
														sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d, USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", STATUS_FDIRECT_DIRECT_OK,
															porderelement->Attribute("userName"), porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
														ExecSQL(sql);
													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "no_Fault") == 0) //误报、可能故障确认为非故障
													{
														ClearFdirDB(porderelement->Attribute("fault_id"));

														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET USERID=\'%s\', MACHINE=\'%s\' WHERE GROUPID=\'%s\'", porderelement->Attribute("userName"),
															porderelement->Attribute("machine"), porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															SetFdirectStatus(porderelement->Attribute("fault_id"));
														}
														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);

														RemoveTasks(porderelement->Attribute("fault_id"));

														pthread_mutex_unlock(&task_list_mutex);
													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "invalid") == 0)	//故障执行步骤失效
													{
														sprintf(sql, "UPDATE FDIR_STEP SET STATUS=%d, STARTTIME=SYSDATE, ENDTIME=SYSDATE WHERE ID=\'%s\' AND STAGE=\'%s\' AND CB_ID=\'%s\'",
															STATUS_FDIR_STEP_INVALID, porderelement->Attribute("fault_id"), porderelement->Attribute("stage"), porderelement->Attribute("cb"));
														ExecSQL(sql);
														ClearFdirDB(porderelement->Attribute("fault_id"));
														sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_INVALID, porderelement->Attribute("fault_id"));
														if (ExecSQL(sql) != OCI_ERROR)
														{
															sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_INVALID, porderelement->Attribute("fault_id"));
															ExecSQL(sql);
														}

														//从任务列表中删除所有与该故障关联的项目
														pthread_mutex_lock(&task_list_mutex);

														RemoveTasks(porderelement->Attribute("fault_id"));

														pthread_mutex_unlock(&task_list_mutex);
													}
													else if (strcasecmp(porderelement->Attribute("op_type"), "pass") == 0)		//跳过指定遥控步骤
													{
														SetPassControl(porderelement->Attribute("fault_id"), atol(porderelement->Attribute("cb")), STATUS_FDIR_STEP_PASS,
															atoi(porderelement->Attribute("stage")));
														UpdateExtStatus(porderelement->Attribute("fault_id"));

														//置处理完成标志
														list<CB_CONTROL> lstCB;

														if (GetLeftStep(porderelement->Attribute("fault_id"), lstCB) == 0)
														{
															ClearFdirDB(porderelement->Attribute("fault_id"));
															sprintf(sql, "UPDATE FDIR_FDIRECT_HIS SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, porderelement->Attribute("fault_id"));
															if (ExecSQL(sql) != OCI_ERROR)
															{
																sprintf(sql, "UPDATE FDIR_FDIRECT SET STATUS=%d WHERE groupid=\'%s\'", STATUS_FDIRECT_FINISH, porderelement->Attribute("fault_id"));
																ExecSQL(sql);
															}

															RemoveTasks(porderelement->Attribute("fault_id"));

															printf("故障组%s处理完成！！！！\r\n", porderelement->Attribute("fault_id"));
														}
													}
													else
													{
														list<FDIR_TASK>::iterator it;
														for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
														{
															if (it->rq_type == TASK_TYPE_FAULT)
															{
																if (strcmp(it->task_data.fault.head.group_id, porderelement->Attribute("fault_id")) == 0)
																{
																	break;
																}
															}
														}

														//如果不在任务列表，把故障重新加载到内存中，然后再把故障详细加载进去
														if (it == g_lstTask.end())
														{
															RecoMemTask(porderelement->Attribute("fault_id"));
															RecoTaskStep(porderelement->Attribute("fault_id"));

														}

														if (strcasecmp("realtime", pBodyElement->Attribute("contextname")) == 0)
														{
															pthread_mutex_lock(&task_list_mutex);
															AddReturnControl(porderelement->Attribute("fault_id"), atol(porderelement->Attribute("cb")), atoi(porderelement->Attribute("op_type")) - 1,
																atoi(porderelement->Attribute("stage")));
															pthread_mutex_unlock(&task_list_mutex);
														}
														else
														{
															pthread_mutex_lock(&task_list_mutex);
															AddControlItem(porderelement->Attribute("fault_id"), atol(porderelement->Attribute("cb")), atoi(porderelement->Attribute("op_type")) - 1,
																atoi(porderelement->Attribute("stage")));
															pthread_mutex_unlock(&task_list_mutex);
														}
													}
												}

												pordernode = pordernode->NextSibling();

										}
									}
									//防误闭锁判断
									else if (strcasecmp(strHead, "misoperation") == 0)
									{
										if (strcasecmp(pBodyElement->Value(), "operation") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));

											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											//更新开关状态,并根据母线的遥测信息添加SO
											if (nmd->update_cb_state() && nmd->add_so()/* && nmd->DoValidate() && g_bob2->WriteReDB(NULL, NULL, NULL)*/)
											{
												TiXmlNode *pordernode = pBodyNode->FirstChild();

												//循环查找Record项,并处理
												while (pordernode != NULL)
												{
													TiXmlElement *porderelement = pordernode->ToElement();

													if (strcasecmp(porderelement->Value(), "order") == 0)
													{
														DATA_MISOPERATION data_mis;
														int op_type = atoi(porderelement->Attribute("op_type"));

														nmd->find_so(atol(porderelement->Attribute("cb")), "breaker", paths, FIND_TYPE_MISOPERAT, &data_mis);

														cout << "*******DV1******" << endl;
														{
															for (list<PSBOB::DV>::iterator it = data_mis.dv1.begin(); it != data_mis.dv1.end(); it++)
															{
																cout << it->id << "," << it->name << endl;
															}
														}
														cout << "*******LD1******" << endl;
														{
															for (list<CLS_LD>::iterator it = data_mis.lst_ld1.begin(); it != data_mis.lst_ld1.end(); it++)
															{
																cout << it->id << "," << it->descr << endl;
															}
														}
														cout << "*******DV2******" << endl;
														{
															for (list<PSBOB::DV>::iterator it = data_mis.dv2.begin(); it != data_mis.dv2.end(); it++)
															{
																cout << it->id << "," << it->name << endl;
															}
														}
														cout << "*******LD2******" << endl;
														{
															for (list<CLS_LD>::iterator it = data_mis.lst_ld2.begin(); it != data_mis.lst_ld2.end(); it++)
															{
																cout << it->id << "," << it->descr << endl;
															}
														}

														int Result = -1;
														bool bhasdv[2] = {false, false};
														list<CLS_LD> lst_ld;	//充电或失电的LD

														bhasdv[0] = data_mis.dv1.size() > 0 ? true : false;
														bhasdv[1] = data_mis.dv2.size() > 0 ? true : false;

														if (op_type == 1) //合闸操作
														{
															if (bhasdv[0]) //一端有电源
															{
																if (bhasdv[1]) Result = 1; //另一端也有电源，合环了
																else
																{
																	Result = 3; //另一端无电源，充电

																	lst_ld = data_mis.lst_ld2;
																}
															}
															else //一端无电源
															{
																if (bhasdv[1])
																{
																	Result = 3; //另一端有电源， 充电了

																	lst_ld = data_mis.lst_ld1;
																}
																else ; //另一端也无电源，啥都不是
															}
														}
														else if (op_type == 2) //分闸操作
														{
															if (bhasdv[0]) //一端有电源
															{
																if (bhasdv[1]) Result = 2; //另一端也有电源，解环了
																else
																{
																	Result = 4; //另一端无电源，失电

																	lst_ld = data_mis.lst_ld2;
																}
															}
															else //一端无电源
															{
																if (bhasdv[1])
																{
																	Result = 4; //另一端有电源， 失电了

																	lst_ld = data_mis.lst_ld1;
																}
																else
																	; //另一端也无电源，啥都不是
															}
														}
														else	//其它操作无效
														{
															cout << "无效操作！" << endl;
															nmd->m_Errors.push_back("无效操作！");
														}

														//发送请求响应数据
														//写XML响应头
														cout << head_xml2 << endl;
														pFile = fopen("remessage.xml","a");
														if (pFile == NULL)
														{
															cout<<"报文响应文件打开或创建失败！"<<endl;
															cout<<"Error No.:"<<errno<<endl;
															char* pErrMsg = strerror(errno);
															cout<<"Error Message:"<<pErrMsg<<endl;
															fclose(pFile);
															return (void*)-1;
														} 
														else
														{
															long lWtNum = fwrite(head_xml2, sizeof(char), sizeof(head_xml2), pFile);
															//移动到文件尾
															fseek(pFile,0, SEEK_END);
															char strContext[128];

															//请求上下文信息
															sprintf(strContext, "<operation pictureName=\"%s\" contextname=\"%s\" bobname=\"%s\">\r\n<order cb=\"%s\" op_type=\"%d\" />\r\n</operation>\r\n",
																order.picturename, order.contextname, order.bobname, porderelement->Attribute("cb"), op_type);
															//插入报文
															lWtNum = fwrite(strContext, sizeof(char), sizeof(strContext), pFile);
														}

							
														//发送错误信息
														string strError = "<ErrorMsg>\r\n";

														//逐条处理错误信息
														{
															for (list<string>::iterator it = nmd->m_Errors.begin(); it != nmd->m_Errors.end(); it++)
															{
																strError += "<error msg=\"";//" />
																strError += *it;
																strError += "\" />";
																strError += "\r\n";
															}
														}
														strError += "</ErrorMsg>\r\n";

														cout << strError<< endl;
														//移动到文件尾
														fseek(pFile,0, SEEK_END);
														//插入报文
														long lWtNum = fwrite(strError.c_str(), sizeof(char), sizeof(strError), pFile);

														//有合解环数据
														if (Result != -1)
														{
															//发送合解环结果
															char strLoop[64];
															sprintf(strLoop, "<Result type=\"%d\" level=\"1\">\r\n", Result);
															cout << strLoop << endl;
															fseek(pFile, 0, SEEK_END);
															//插入报文
															lWtNum = fwrite(strLoop, sizeof(char), sizeof(strLoop), pFile);

															//发送设备信息
															switch (Result)
															{
																//合、解环
															case 1:
															case 2:
																{
																	list<PSBOB::DV>::iterator it;

																	for (it = data_mis.dv1.begin(); it != data_mis.dv1.end(); it++)
																	{
																		char strBuff[64];
																		sprintf(strBuff, "<Record id=\"%ld\" descr=\"%s\" tablename=\"subcontrolarea\" />\r\n", it->id, it->name);
																		cout << strBuff << endl;
																		fseek(pFile, 0, SEEK_END);
																		//插入报文
																		lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);
																	}

																	for (it = data_mis.dv2.begin(); it != data_mis.dv2.end(); it++)
																	{
																		char strBuff[64];
																		sprintf(strBuff, "<Record id=\"%ld\" descr=\"%s\" tablename=\"subcontrolarea\" />\r\n", it->id, it->name);
																		cout << strBuff << endl;
																		fseek(pFile, 0, SEEK_END);
																		//插入报文
																		lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);
																	}
																	break;
																}
																//充、失电
															case 3:
															case 4:
																{
																	for (list<CLS_LD>::iterator it = lst_ld.begin(); it != lst_ld.end(); it++)
																	{
																		char strBuff[64];
																		sprintf(strBuff, "<Record id=\"%ld\" descr=\"%s\" tablename=\"subcontrolarea\" />\r\n", it->id, it->name);
																		cout << strBuff << endl;
																		fseek(pFile, 0, SEEK_END);
																		//插入报文
																		lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);
																	}
																	break;
																}
															default:
																break;
															}

															char strResult[16];
															strcpy(strResult, "</Result>\r\n");
															cout << strResult << endl;
															fseek(pFile, 0, SEEK_END);
															//插入报文
															lWtNum = fwrite(strResult, sizeof(char), sizeof(strResult), pFile);
															
														}
														//发送XML响应尾
														cout << tail_xml2 << endl;
														fseek(pFile, 0, SEEK_END);
														//插入报文
														lWtNum = fwrite(tail_xml2, sizeof(char), sizeof(tail_xml2), pFile);
													}

													pordernode = pordernode->NextSibling();
												}
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
									}
									else if (strcasecmp(strHead, "fhzg") == 0)
									{
										char cb_id[MAX_CB_ID_LEN] = { 0 };
										char fhzg_id[50] = { 0 };
										char error[500] = { 0 };
										int ret_err = -1;

										if (strcasecmp(pBodyElement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));
											strcpy(order.st, pBodyElement->Attribute("st"));

											TiXmlNode *pordernode = pBodyNode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													timeval tval = { 0 };
													strcpy(cb_id, porderelement->Attribute("id"));

													gettimeofday(&tval, NULL);

													tm *ptm = localtime(&tval.tv_sec);
													sprintf(fhzg_id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
														tval.tv_usec);

													FHZG *fhzg = new FHZG(g_bob);

													int fhzg_ret = fhzg->Do(atol(porderelement->Attribute("id")));

													switch (fhzg_ret)
													{
													case 0: //转供成功
														{
															//写负荷转供关系库表
															fill_fhzg_db(atol(cb_id), fhzg_id, fhzg, true);
															sprintf(error, "%f", fhzg->fhzg_capacity);
															ret_err = 0;
															break;
														}

													case FHZG_FAIL_NO_SO:
														{
															strcpy(error, "没有可供转供的电源，转供失败！");
															ret_err = -1 ;
															break;
														}

													case FHZG_FAIL_SO_FULL:
														{
															strcpy(error, "所有可转供的电源剩余容量不足，转供失败！");
															ret_err = -1;
															break;
														}
													case FHZG_FAIL_NO_IVALUE:
														{
															strcpy(error, "没有获取到要转供开关的电流值，转供失败！");
															ret_err = -1;
															break;
														}
													}

													delete fhzg;
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}
										}
										else
										{
										}

										//发送请求响应数据
										//发送XML响应头
										cout << head_xml3 << endl;
										pFile = fopen("remessage.xml","a");
										if (pFile == NULL)
										{
											cout<<"报文响应文件打开或创建失败！"<<endl;
											cout<<"Error No.:"<<errno<<endl;
											char* pErrMsg = strerror(errno);
											cout<<"Error Message:"<<pErrMsg<<endl;
											fclose(pFile);
											return (void*)-1;
										} 
										else
										{
											long lWtNum = fwrite(head_xml3, sizeof(char), sizeof(head_xml3), pFile);
											//移动到文件尾
											fseek(pFile,0, SEEK_END);
											char strContext[128];

											//请求上下文信息
											sprintf(strContext, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  st = \"%s\" >\r\n", order.picturename, order.contextname, order.bobname,
												order.st);
											//插入报文
											lWtNum = fwrite(strContext, sizeof(char), sizeof(strContext), pFile);
										}

										char strBuff[64];
										sprintf(strBuff, "<Record id = \"%s\" tablename = \"cb\" fhzg_id=\"%s\" result=\"%d\" error=\"%s\"/>\r\n", cb_id, fhzg_id, ret_err, error);

										//写入故障转供标识符
										cout << strBuff << endl;
										fseek(pFile, 0, SEEK_END);
										long lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);

										//发送XML响应尾
										cout << tail_xml << endl;
										fseek(pFile, 0, SEEK_END);
										long lWtNum = fwrite(tail_xml, sizeof(char), sizeof(tail_xml), pFile);

									}
									else if(strcasecmp(strHead, "typical") == 0)
									{
										//D5000 典型运行方式暂未移植
										char userid[100] = { 0 };
										char machine[100] = { 0 };
										char id[50] = { 0 };
										char str_time[50] = { 0 };
										char ch_time[40] = { 0 };
										time_t res_time;
										string dv_id;
										int result = -1;

										if (strcasecmp(pBodyElement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));
											strcpy(machine, pBodyElement->Attribute("machine"));
											strcpy(userid, pBodyElement->Attribute("userid"));

											int ret = FDIR_R_FAIL;
											//加多请求线程互斥锁
											pthread_mutex_lock(&g_rq_mutex);

											TiXmlNode *pordernode = pBodyNode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													cmd = atoi(porderelement->Attribute("operate_type"));
													switch (cmd)
													{
													case 0: //保存典型方式
														{
															timeval tval = { 0 };
															gettimeofday(&tval, NULL);
															tm *ptm = localtime(&tval.tv_sec);
															sprintf(id, "%04d/%02d/%02d %02d:%02d:%02d.%06ld", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
																tval.tv_usec);
															typical *typi = new typical(g_fdir_r, id);

															if(typi->typical_save(atol(porderelement->Attribute("cb_id")), machine, userid))
															{
																result = 0;
															}

															break;
														}
													case 1: //恢复典型方式
														{
															typical *typi = new typical(g_fdir_r, id);

															if(typi->typical_restore(atol(porderelement->Attribute("cb_id")), machine, userid, dv_id, res_time))
															{
																result = 0;
															}
															struct tm *tmtime;
															tmtime = localtime(&res_time);
															sprintf(ch_time,"%d/%d/%d %d:%d:%d",(tmtime->tm_mon + 1), tmtime->tm_mday, (tmtime->tm_year + 1900), tmtime->tm_hour, tmtime->tm_min, tmtime->tm_sec);
															strcpy(id, dv_id.c_str());
															break;
														}

													default:
														break;
													}
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										//发送请求响应数据
										//写入XML响应头
										cout << head_xml6 << endl;
										pFile = fopen("remessage.xml","a");
										if (pFile == NULL)
										{
											cout<<"报文响应文件打开或创建失败！"<<endl;
											cout<<"Error No.:"<<errno<<endl;
											char* pErrMsg = strerror(errno);
											cout<<"Error Message:"<<pErrMsg<<endl;
											fclose(pFile);
											return (void*)-1;
										} 
										else
										{
											long lWNum = fwrite(head_xml6, sizeof(char), sizeof(head_xml6), pFile);

										}

										//写入响应请求上下文信息

										//移动到文件尾
										fseek(pFile,0, SEEK_END);
										char strContext[128];
										sprintf(strContext, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  userid = \"%s\" machine=\"%s\">\r\n", order.picturename,
											order.contextname, order.bobname, userid, machine);

										cout << strContext << endl;
										long lWtNum = fwrite(strContext, sizeof(char), sizeof(strContext), pFile);
										//移动到文件尾
										fseek(pFile,0, SEEK_END);
										//逐项发送设备列表里的设备信息
										char strBuff[128];
										sprintf(strBuff, "<Record id = \"%s\" time = \"%s\" result = \"%d\" error=\"\"/>\r\n", id, ch_time, result);

										cout << strBuff << endl;
										
										//插入报文
										lWtNum = fwrite(strBuff, sizeof(char), sizeof(strBuff), pFile);

										//发送XML响应尾
										cout << tail_xml << endl;
										//移动到文件尾
										fseek(pFile,0, SEEK_END);

										//插入报文
										lWtNum = fwrite(tail_xml, sizeof(char), sizeof(tail_xml), pFile);
									}
									else if (strcasecmp(strHead, "check_data") == 0)
									{
										char userid[100] = { 0 };
										char machine[100] = { 0 };
										char groupid[100] = { 0 };
										const char *errmsg[] = { "校验通过，数据一致", "实时库版本号变化", "故障相关开关状态发生变化", "读取保存开关状态的数据库失败", "解析报文中groupid失败" };
										if (strcasecmp(pBodyElement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));
											strcpy(machine, pBodyElement->Attribute("machine"));
											strcpy(userid, pBodyElement->Attribute("userid"));

											int ret = FDIR_R_FAIL;
											//加多请求线程互斥锁
											pthread_mutex_lock (&g_rq_mutex);

											TiXmlNode *pordernode = pBodyNode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													strcpy(groupid, porderelement->Attribute("fault_id"));

												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										int result = CHECK_DATA_ID_ERR;
										if (strlen(groupid) != 0)
										{
											//校验数据是否跟故障时一致，如果一致返回0，实时库版本号变化返回1，如果相关数据发生变化返回2，读库失败返回3
											result = data_verify(groupid);
											if (result == CHECK_DATA_CB_ERR)
											{
												//方案已经失效，设置故障标识为失效状态
												sprintf(sql, "UPDATE FDIR_FDIRECT SET EXT_STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_INVALID, groupid);
												ExecSQL(sql);
											}
											else if (result == CHECK_DATA_PSBOB_ERR)
											{
												//实时库版本号变化设置故障标识为版本号变化
												sprintf(sql, "UPDATE FDIR_FDIRECT SET EXT_STATUS=%d WHERE GROUPID=\'%s\'", STATUS_FDIRECT_VERSION_ERR, groupid);
												ExecSQL(sql);
											}
										}
										//发送请求响应数据
										//发送XML响应头
										cout << head_xml7 << endl;
										FILE* pFile = fopen("remessage.xml", "w");
										if (pFile == NULL)
										{
											cout<<"报文响应文件打开或创建失败！"<<endl;
											cout<<"Error No.:"<<errno<<endl;
											char* pErrMsg = strerror(errno);
											cout<<"Error Message:"<<pErrMsg<<endl;
											fclose(pFile);
											return (void*)-1;
										} 
										else
										{
											long lWNum = fwrite(head_xml7, sizeof(char), sizeof(head_xml7), pFile);
										}

										//发送响应请求上下文信息
										char strContext[128];
										sprintf(strContext, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  userid = \"%s\" machine=\"%s\">\r\n", order.picturename,
											order.contextname, order.bobname, userid, machine);

										cout << strContext << endl;

										long lWNum = fwrite(strContext, sizeof(char), sizeof(strContext), pFile);

										//逐项发送设备列表里的设备信息
										char strBuff[128];
										sprintf(strBuff, "<Record id = \"%s\" result = \"%d\" error=\"%s\"/>\r\n", groupid, result, errmsg[result]);

										cout << strBuff << endl;
										fseek(pFile, 0, SEEK_END);
										lWNum = fwrite(strBuff, sizeof(char), sizeof(strBuff),pFile);


										//发送XML响应尾
										cout << tail_xml << endl;

										fseek(pFile, 0, SEEK_END);
										lWNum = fwrite(tail_xml, sizeof(char), sizeof(tail_xml),pFile);
									}
									else if (strcasecmp(strHead, "recalc") == 0)
									{
										//方案失效，重新计算隔离和恢复方案

										char userid[100] = { 0 };
										char machine[100] = { 0 };
										char groupid[100] = { 0 };
										if (strcasecmp(pBodyElement->Value(), "Order") == 0)
										{
											//将请求上下文信息储存起来以便返回响应包里使用
											strcpy(order.contextname, pBodyElement->Attribute("contextname"));
											strcpy(order.bobname, pBodyElement->Attribute("bobname"));
											strcpy(order.picturename, pBodyElement->Attribute("pictureName"));
											strcpy(machine, pBodyElement->Attribute("machine"));
											strcpy(userid, pBodyElement->Attribute("userid"));

											int ret = FDIR_R_FAIL;
											//加多请求线程互斥锁
											pthread_mutex_lock (&g_rq_mutex);

											TiXmlNode *pordernode = pBodyNode->FirstChild();

											//循环查找Record项,并处理
											while (pordernode != NULL)
											{
												TiXmlElement *porderelement = pordernode->ToElement();

												if (strcasecmp(porderelement->Value(), "Record") == 0)
												{
													strcpy(groupid, porderelement->Attribute("fault_id"));
												}
												else
												{
												}

												pordernode = pordernode->NextSibling();
											}

											pthread_mutex_unlock(&g_rq_mutex);
										}
										else
										{
										}

										int result = -1;
										char errmsg[50] = { 0 };

										if (strlen(groupid) != 0)
										{

											pthread_mutex_lock(&task_list_mutex);

											list<FDIR_TASK>::iterator it;
											for (it = g_lstTask.begin(); it != g_lstTask.end(); it++)
											{
												if (it->rq_type == TASK_TYPE_FAULT)
												{
													if (strcmp(it->task_data.fault.head.group_id, groupid) == 0)
													{
														break;
													}
												}
											}

											//如果不在任务列表，把故障重新加载到内存中，然后再把故障详细加载进去
											if (it == g_lstTask.end())
											{
												RecoMemTask(groupid);
												RecoTaskStep(groupid);
											}
											pthread_mutex_unlock(&task_list_mutex);

											if (invalid_recalc(groupid))
											{
												result = 0;
											}
										}
										else
										{
											sprintf(errmsg, "报文中解析groupid有问题！");
										}
										//发送请求响应数据
										//发送XML响应头
										cout << head_xml8 << endl;

										pFile = fopen("remessage.xml","a");
										if (pFile == NULL)
										{
											cout<<"报文响应文件打开或创建失败！"<<endl;
											cout<<"Error No.:"<<errno<<endl;
											char* pErrMsg = strerror(errno);
											cout<<"Error Message:"<<pErrMsg<<endl;
											fclose(pFile);
											return (void*)-1;
										} 
										else
										{
											long lWNum = fwrite(head_xml8, sizeof(char), sizeof(head_xml8), pFile);

										}
										//发送响应请求上下文信息
										char strContext[128];
										sprintf(strContext, "<Order  pictureName = \"%s\"  contextname = \"%s\"  bobname = \"%s\"  userid = \"%s\" machine=\"%s\">\r\n", order.picturename,
											order.contextname, order.bobname, userid, machine);

										cout << strContext << endl;

										fseek(pFile, 0 ,SEEK_END);
										long lWNum = fwrite(strContext, sizeof(char),sizeof(strContext),pFile);

										//逐项发送设备列表里的设备信息
										char strRetBuff[128];
										sprintf(strRetBuff, "<Record id = \"%s\" result = \"%d\" error=\"%s\"/>\r\n", groupid, result, errmsg);

										cout << strRetBuff << endl;

										fseek(pFile, 0, SEEK_END);
										lWNum = fwrite(strRetBuff, sizeof(char), sizeof(strRetBuff), pFile);

										//发送XML响应尾
										cout << tail_xml << endl;
										fseek(pFile, 0, SEEK_END);
										lWNum = fwrite(tail_xml, sizeof(char), sizeof(tail_xml), pFile);
									}
									else
									{
										TRACE("不识别的类型[%s]\r\n", strHead);
									}

									pBodyNode = pBodyNode->NextSibling();
								}
							} 
							else
							{
							}

							pMsgNode = pMsgNode->NextSibling();
						}

						fclose(pFile);
						return (void*)1;
					} 
					else
					{
					}
				}
			}
			else
			{
				cout << "请求报文XML语法错误！"<< endl;
			}

			timeval etimer = {0};

			gettimeofday(&etimer, NULL);

			float cost = etimer.tv_sec * 1000 * 1000 + etimer.tv_usec - (stimer.tv_sec * 1000 * 1000 + stimer.tv_usec);

			printf(">>>>>>>>%sprocessed, total cost:%.2fs\r\n", ctime(&(now = time(NULL))), cost / 1000 / 1000);
		}


		printf("********** Exit Thread (%d) **********\n", threadID);

		delete pXmlDoc;
		pXmlDoc = NULL;

	}

	return (void*)1;
}



int dispatch_main(const int port, const int msglen, const int time_out)
{
	ServiceInfo serviceinfo;

	buflen = msglen * 1024;

	serviceinfo.port = port;
	time_out_in= time_out;
	ServiceServerInit(serviceinfo, DISPATCH);

	ServiceRegisterInit("dmshan", "realtime", "public", port, port, 0);

	int iCycle = 100;
	int i = 1;

	while(true)
	{
		pthread_t threadID = pthread_self();
		cout<<"线程ID："<<threadID<<endl;
		ServiceDispatch (serviceinfo, 2, DispatchCallBack);
		cout<<"----------------ServiceDispatch在这里返回！"<<endl;
		sleep(10);
		cout<<"循环次数："<<(i++)<<endl;
	}
	return 0;
}




void *disaptch_thread(void *arg)
{
	pthread_t tid = pthread_self();
	Pthread_detach(tid);
	Serv_para *pServ_para = (Serv_para *)arg;
	int port = pServ_para->port;
	int msglen = pServ_para->msglen;
	int time_out = pServ_para->time_out;
	delete pServ_para;
	dispatch_main(port,msglen,time_out);
	return NULL;
}



int main(int argc, char** argv)
{
	int dispatch_port = SRVBUS_PORT, publish_port = 0, time_out = 90, success_times = 90;

	if( 0 != dispatch_port )
	{
		Serv_para * pServ_para = new Serv_para;
		pServ_para->msglen = 0;
		if ( 0 < time_out )
		{
			pServ_para->time_out = time_out;
		}
		pServ_para->port = dispatch_port;

		pthread_t tid;
		Pthread_create(&tid, NULL, disaptch_thread, (void*)pServ_para);
	}


	while( 1 )
	{
		sleep(1000);
	}

	return 0;

}
