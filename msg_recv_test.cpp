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
#include "errno.h"
#include "dms_tcp.h"

typedef unsigned char tinyint;
extern int _oodb_errno; 

typedef struct
{
	int sock_id;
	int evt_num;
	int evt_id[100];
	int link_close;
} EVT_INF;

#define        false  0
#define		true  1
#define        MAX_LINK  100

#define	MID_RETURN(a,b) if(a==OO_FAIL) { printf("error in: %s,line:%d  No. :%d\n",b,__LINE__,_oodb_errno); return a; }
#define MID_ERROR(a,b)  if(a==NULL) { printf("get node:%s ip error\n",b);return OO_FAIL;}


#define	   	OODBTRAN   	"@OODBTRAN@"
// begin symbol for realtime database's data transfer.
#define    	COMDEXEC   	"@COMDEXEC@"
// begin symbol for execute system command and MRTP.
#define    	REDBTRAN   	"@REDBTRAN@"
// begin symbol for relation database's data transfer.

#define	   	COMMAND   	"COMMAND"
#define		RECONNECT	"RECONNECT"

#define	   	SENDEVENT   	"SENDEVENT"
#define	   	RECVEVENT   	"RECVEVENT"

#define    	SEPASIGN    	"$^#$"		// separator symbol define.

const int	MaxEventLength=350;

#define		MAX_TIME	20
#define		MAX_DIGIT	20

#define		DbDescrIdLen	50

#define    	RESULTSIGN	"@RESULT"
#define    	RESULTEND	"@RETEND"
#define    	RIGHT		"SUCCEED"
#define    	WRONG		"FAULT"

//error number define
#define    MID_FORMAT_ERROR	2001
#define    MID_TCP_READ_ERROR	2002
#define    MID_TCP_WRITE_ERROR	2003
#define    MID_DBOPENID_ERROR   2004

#define    MID_SQL_COMMAND_ERROR 3001
#define    MID_SQL_HANDLE_ERROR  3002

#define    MID_GETIP_ERROR       3003

// Add by SGH on 2009-03-27
#define MAX_NAME 128
#define MAX_USER_NUM 200
#define MAX_BUFF_LEN 256
#define MAX_CHAR 128
#define OO_FAIL  -1
#define OO_SUCCEED 0

using namespace std;
int		SepaSignLen=0;

message_invocation  *bob_inv = NULL;
proc_invocation *proc_inv = NULL;
typedef void*(*pthread_startroutine_t)(void *);
TCPClass* g_tcp_tool = NULL;
GDI_THREAD pthread_id[MAX_THREAD];
#define   pthread_addr_t			void *


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


int CalcOffset(int sumlen,int datatype)
{
	switch(datatype)
	{
	case    1:
	case	2:
		return sumlen;
	case    22:
		if(sumlen%sizeof(time_t))
			return  sumlen+(sizeof(time_t)-(sumlen%sizeof(time_t)));
		else    return  sumlen;
	case    15: // long type
		if(sumlen%sizeof(long ))
			return  sumlen+(sizeof(long)-(sumlen%sizeof(long)));
		else    return  sumlen;
	case    6:
		if(sumlen%sizeof(float))
			return  sumlen+(sizeof(float)-(sumlen%sizeof(float)));
		else    return  sumlen;
	case    7:
		if(sumlen%sizeof(double))
			return  sumlen+(sizeof(double)-(sumlen%sizeof(double)));
		else    return  sumlen;
	case    4:
		if(sumlen%sizeof(int))
			return  sumlen+(sizeof(int)-(sumlen%sizeof(int)));
		else    return  sumlen;
	case    3:
		if(sumlen%sizeof(short))
			return  sumlen+(sizeof(short)-(sumlen%sizeof(short)));
		else 
			return  sumlen;
	default:
		fprintf(stderr, "CalcOffset:type not support yet\n");
		break;
	}
}


int DropSepaSign(char *SrcString,char *DestString)
{
	char	*StrPtr;
	if( strncmp(SrcString,SEPASIGN,SepaSignLen) != 0 )
	{
		if( *SrcString == '\n' ) 	return OO_SUCCEED;

		_oodb_errno = MID_FORMAT_ERROR;
		return OO_FAIL;
	}
	SrcString = SrcString + SepaSignLen ;

	StrPtr = strstr(SrcString,SEPASIGN);

	if( StrPtr == NULL )
	{
		int length = strlen(SrcString)-1;
		if(1 > length)
		{
			_oodb_errno = MID_FORMAT_ERROR;
			return OO_FAIL;
		}
		strncpy(DestString,SrcString,length);
		DestString[length] = '\0';
	}
	else
	{
		strncpy(DestString,SrcString,StrPtr-SrcString);
		DestString[StrPtr-SrcString] = '\0';
	}

	return strlen(DestString);
}




int  DoSendEvent(int Sockfd,char *CmdPtr)
{
	int     ret=0;
	int	evtid;
	int	tempint;
	//Add by JFL 
	short   tempshort;
	//tinyint temptiny;
	unsigned char temptiny;// mod by Lijunliang 2009-08-22
	// Modify by SGH , bit     tempbit;
	char tempbit;

	//Add end
	int     i;
	float	tempfloat;
	double	tempdouble;
	time_t	temptime_t;
	long    templong;
	char	tempchar[512];
	char	*bufptr;
	char	buffer[4096];
	char    SrvName[MAX_NAME]="";
    char    UsrName[MAX_NAME]="";
	char	EventId[MAX_NAME]="";
	char	DataType[MAX_NAME]="";
	char	EventSetId[MAX_NAME] = "";


	TRACE("$$$$$$$in_buf:%s\r\n", CmdPtr);

	ret = DropSepaSign(CmdPtr,UsrName);
	MID_RETURN(ret,"DoEvent :DropSepaSign UsrName");
	CmdPtr = CmdPtr + strlen(UsrName) + SepaSignLen;
	ret = DropSepaSign(CmdPtr,SrvName);

	MID_RETURN(ret,"DoEvent :DropSepaSign ServerName");
	CmdPtr = CmdPtr + strlen(SrvName) + SepaSignLen;
	
	printf("CmdPtr is %s\n",CmdPtr);
	ret = DropSepaSign(CmdPtr,EventId);
	MID_RETURN(ret,"DoEvent :DropSepaSign EventId");
	CmdPtr = CmdPtr + strlen(EventId) + SepaSignLen;
	
	// Add by SGH on 2009-03-30, add event set id
	ret = DropSepaSign(CmdPtr,EventSetId);
	MID_RETURN(ret,"DoEvent :DropSepaSign EventSetId");
	CmdPtr = CmdPtr + strlen(EventSetId) + SepaSignLen;
	// End

	//ret = DropSepaSign(CmdPtr,NULL);
        //MID_RETURN(ret,"DoReadMak :DropSepaSign RecordNumber");

	/*analyst event*/
	bzero(buffer, sizeof(buffer));
	bufptr=buffer;
	
	// MUST get channel id from MMI
	Message msgcont;
	bzero(&msgcont, sizeof(Message));
	msgcont.header.event=atoi(EventId);
	msgcont.header.serv = atoi(EventSetId); 
	

	int sumlength=0,offset=0;
	for(;;){

		bzero(tempchar,512);
		bzero(DataType, sizeof(DataType));

		ret = DropSepaSign(CmdPtr,DataType);
		MID_RETURN(ret,"DoEvent :DropSepaSign variable");
		CmdPtr = CmdPtr + strlen(DataType) + SepaSignLen;

		ret = DropSepaSign(CmdPtr,tempchar);
		MID_RETURN(ret,"DoEvent :DropSepaSign temp");
		CmdPtr = CmdPtr + strlen(tempchar) + SepaSignLen;

		switch(atoi(DataType)){
			case 1:/*char   type, 1*/
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+strlen(tempchar);

				memcpy(bufptr+offset,tempchar,strlen(tempchar));
				printf("char type tempchar_char=%s  offset=%d\n",tempchar,offset);
				if(*(bufptr+offset)==' ')
				*(bufptr+offset)='\0';
				else
				{
					for(i=strlen(tempchar)-1;i>0;i--)
					{	
						if(*(bufptr+offset+i)!=' ')  
						{			 
							*(bufptr+offset+i+1)='\0';
							break;
						}
					}
					if(i==0)
					*(bufptr+offset+1)='\0';
				}
				//bufptr=bufptr+strlen(tempchar);
				break;
			case 15:   /* long type, 2*/
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(long);
				printf("long type tempchar=%s offset=%d\n",tempchar, offset);
				templong=(long)atol(tempchar);
				memcpy(bufptr+offset,(void*)&templong,sizeof(long));
				//bufptr=bufptr+sizeof(time_t);
				break;
            case 22:/*22, time_t type*/
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(time_t);
				temptime_t=(time_t)atoi(tempchar);
			//	printf("@@tempchar=%s @@",tempchar);
				printf("time_t type tempchar_time_t=%d offset=%d\n",temptime_t,offset);
				memcpy(bufptr+offset,(void*)&temptime_t,sizeof(time_t));
				break;
			case 6 :/*3, float type */
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(float);
				tempfloat=atof(tempchar);
				memcpy(bufptr+offset,(void*)&tempfloat,sizeof(float));
				//bufptr=bufptr+sizeof(float);
				break;
			case 7:/*4, double type */
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(double);
				tempdouble=atof(tempchar);
				memcpy(bufptr+offset,(void*)&tempdouble,sizeof(double));
				//bufptr=bufptr+sizeof(double);
				break;
			case 4:/*5, int    type*/
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(int);
				printf("int type tempchar=%s offset=%d\n",tempchar,offset);
				tempint=atoi(tempchar);
				memcpy(bufptr+offset,(void*)&tempint,sizeof(int));
			//	bufptr=bufptr+sizeof(int);
				break;
			case 3:/*6, short  type*/
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(short);
				//tempint=atoi(tempchar);
				tempshort=atoi(tempchar);
				printf("%s,%d\n",tempchar,tempshort);
				memcpy(bufptr+offset,(void*)&tempshort,sizeof(short));
			//	bufptr=bufptr+sizeof(short);
				break;
			case 2:/* 7, tinyint type, in d5000 changing to C_UCHAR_TYPE */
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(tinyint);
				//tempint=atoi(tempchar);
				printf("tinyint type tempchar=%s\n",tempchar);
				temptiny=atoi(tempchar);
				memcpy(bufptr+offset,(void*)&temptiny,sizeof(tinyint));
			//	bufptr=bufptr+sizeof(tinyint);
				break;
			case 8:/*bit    type*/
				// There is no this type, modify by SGH
				/*
				offset=CalcOffset(sumlength,atoi(DataType));
				sumlength=offset+sizeof(bit);
				tempbit=atoi(tempchar);
				memcpy(bufptr+offset,(void*)&tempbit,sizeof(bit));
			   */
				break;
			default:
				printf("----Java send datatype is wrong----\n");
				break;
		} if(strstr(CmdPtr,SEPASIGN)==NULL)break;
	}


	if( (evtid=atoi(EventId))==0 )
	{
		printf("----MiddleMain Get EventId Wrong----\n");
		return OO_FAIL;
	}

	/*throw off sepace sign len*/
	CmdPtr = CmdPtr + SepaSignLen ;


        
	printf("Midlle browser send eventid=%d,length=%d\n",evtid,sumlength);
	if(evtid == 108 ) {
		printf("event id  == 108 tempint = %d offset=%d\n", tempint,offset);
		memcpy((void *)&tempint,(void*)buffer,sizeof(int));
		printf("#####event id  == 108 tempint = %d offset=%d\n", tempint,offset);
		char yefei[512];
		printf("message is:  \n");
		for(int mm=0;mm<sizeof(buffer);mm++)
		{
			printf("%c",buffer[mm]);
		}
		printf("\n");

		cout<<"buffer="<<buffer<<endl;
	}
		cout<<"cout___buffer="<<buffer<<endl;

		Msg_destination *pp_udp=NULL;
		// copy event body
		memcpy(msgcont.Msg_buf,buffer,sumlength);
        //msg.messageSend((Message  *)&msgcont,sumlength,pp_udp);

/* Client never care about this result.
        ret = SendResult(Sockfd,OO_SUCCEED,0,0,NULL);
        if( ret == OO_FAIL )    return OO_FAIL;
*/

	return OO_SUCCEED;
}


void thread_recv_socket(void* param)
{
	int ret;
	int newsockfd;

	struct timeval tpstart, tpend;
	long iTimeInterval;
	newsockfd = *((int*)param);
	cout << " ******** New Thread (" << newsockfd << ")********" << endl;

	char recv_buf[MAX_BUFLEN];
	bzero(recv_buf, MAX_BUFLEN);

	while (GDI_TRUE)
	{
		if ((ret = g_tcp_tool->ReadLine(newsockfd, recv_buf, MAX_BUFLEN)) <= 0)
		{
			if (ret == 0)
			{
			}

			break;
		}

		cout<<__LINE__<<": buffer:"<<recv_buf<<endl;

		DoSendEvent(newsockfd, recv_buf);

	}

}


void thread_recv_rte(void *param)
{
	///开始循环接收RTE事件
	while (true)
	{
		//cout<<__LINE__<<"开始接收事件！"<<endl;
		Message msg;

		int len = bob_inv->messageReceive(&msg, NULL, 0);

		if (len < 0)
		{
			cout << "messageReceive fail!" << endl;
			pthread_exit(0);
		}
		else if (len == 0)
		{
		/*	usleep(10);*/
			continue;
		}
		else
		{
			//TRACE("接收到告警! id:%d,通道号:%d msg:%s\r\n", msg.header.event, msg.header.serv, msg.Msg_buf);

			switch (msg.header.event)
			{
			case MT_YX_CHANGE:
				{
					int cnt = msg.header.len / sizeof(ChangeYx);
					int pos = 1;					
					ChangeYx *yx = (ChangeYx *) msg.Msg_buf;
					if (yx != NULL)
					{
						TRACE("key_id:%ld, msecond:%d\r\n", yx->keyid, yx->msecond);
					}

				}
				break;
			}
			
		}

		sleep(1);
	}
}


int main(int argc, char **argv)
{
	int ret;
	bob_inv = new message_invocation;
	proc_inv = new proc_invocation;

	pthread_t pt_process, pt_socket;	
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if (rte_connect(argc, argv) > 0)
	{
		if (pthread_create(&pt_process, &attr, (pthread_startroutine_t) thread_recv_rte, 0) < 0)
		{
			cout << "创建RTE接收线程失败！" << endl;
		}

		pthread_detach(pt_process);
	} 
	else
	{
		cout<<__LINE__<<"连接消息总线失败！"<<endl;
	}


	g_tcp_tool = new TCPClass;
	ret = g_tcp_tool->Srv_Init(8920);

	if (ret < 0)
	{
		TRACE("初始化TCP连接失败！\r\n");
		return -1;
	}

	int listenfd = g_tcp_tool->GetListenfd();


	int numthread = 0;
	int newsockfd = 0;

	for (int i = 0; i < MAX_THREAD; i++)
	{
		pthread_id[i].fdinThread = -1;
	}

	while (GDI_TRUE)
	{
		if (numthread > MAX_THREAD)
		{
			numthread = 0;
		}

		if ((newsockfd = g_tcp_tool->Srv_Accept()) < 0)
		{
		}

		numthread = newsockfd;

		if (pthread_id[numthread].fdinThread == -1)
		{
			pthread_id[numthread].fdinThread = newsockfd;
		}

		if (newsockfd == -1 && numthread == -1)
		{
			break;
		}

		int ret;

		if ((ret = pthread_create(&pthread_id[numthread].thread_id, &attr, (pthread_startroutine_t) thread_recv_socket, (pthread_addr_t) &newsockfd)) < 0)
		{
#ifdef _DEBUG_
			printf("Create Failed!!!\n");
#endif
		}

		pthread_detach(pthread_id[numthread].thread_id);
	}



	return 0;
}