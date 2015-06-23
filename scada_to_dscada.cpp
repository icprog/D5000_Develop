#include <iostream>
#include <map>
#include <list>
#include <scada_api/CScaComm.h>
#include <sam_services_define.h>
#include "RTDB.h"
#include "db_struct_scada.h"
#include "db_struct_dscada.h"
#include "db_struct_public.h"

#define Transition_id        143833717394112511
#define BUG 				1

using namespace std;

RTDB *g_db_scada = NULL;
RTDB *g_db_dscada = NULL;

std::map<long, std::list<long> > map_breaker_measpoint;
std::map<long, std::list<long> > map_disconnector_measpoint;
std::map<long, std::list<long> > map_busbarsection_measpoint;

std::map<long, std::list<long> > map_disconnector_measanalog;
std::map<long, std::list<long> > map_breaker_measanalog;
std::map<long, std::list<long> > map_busbarsection_measanalog;

std::map<long, std::list<long> > map_disconnector_token;
std::map<long, std::list<long> > map_breaker_token;
std::map<long, std::list<long> > map_busbarsection_token;

breaker_scada breaker;
breaker_dscada d_breaker;
const int size_breaker = sizeof(breaker_scada);
const int size_d_breaker = sizeof(breaker_dscada);

disconnector_scada disconnector;
disconnector_dscada d_disconnector;
const int size_disconnector = sizeof(disconnector_scada);
const int size_d_disconnector = sizeof(disconnector_dscada);

busbarsection_scada busbarsection;
busbarsection_dscada d_busbarsection;
const int size_busbarsection = sizeof(busbarsection_scada);
const int size_d_busbarsection = sizeof(busbarsection_dscada);

measpoint_scada measpoint;
measpoint_dscada d_measpoint;
const int size_measpoint = sizeof(measpoint_scada);
const int size_d_measpoint = sizeof(measpoint_dscada);

measanalog_scada measanalog;
measanalog_dscada d_measanalog;
const int size_measanalog = sizeof(measanalog_scada);
const int size_d_measanalog = sizeof(measanalog_dscada);

token_info_scada token;
token_info_dscada d_token;
const int size_token = sizeof(token_info_scada);
const int size_d_token = sizeof(token_info_dscada);

///////////////////////////
CScaComm *g_sca_common;
time_t *model_chg_ntf;
time_t tmp_val0 = 0;
time_t last_model_chg_ntf;

pthread_mutex_t mutex_sync;

class AutoLock
{
private:
	pthread_mutex_t m_Lock;

public:
	AutoLock(pthread_mutex_t lk)
	{
		m_Lock = lk;

		pthread_mutex_lock(&m_Lock);
	}

	~AutoLock()
	{
		pthread_mutex_unlock(&m_Lock);
	}
};

const char* GetAppHost(const int app_id)
{
	static CServicesManage sermanager;

	for(int i = 0; i < MNG_MAX_APP_NUM; i++)
	{
		if(sermanager.m_AppStatusPtr[i].cur_status == APP_DUTY
		&& sermanager.m_AppStatusPtr[i].app_id == app_id
		&& sermanager.m_AppStatusPtr[i].context == AC_REALTIME_NO)
		{
			return sermanager.m_AppStatusPtr[i].node_name;
		}
	}

	return NULL;
}

const int GetDScadaMask()
{
	const char *p = GetAppHost(AP_PUBLIC);
	TB_DESCR tb_sys_menu = { 0 };
	int ret = 0, count;
	int size = sizeof(sys_menu_info_public);
	sys_menu_info_public menu_info;

	if (OpenTableByName(const_cast<char*>(p), "realtime", "public", "sys_menu_info", &tb_sys_menu) >= 0)
	{
		char *buf = NULL;

		if ((ret = GetTableRecs(&tb_sys_menu, 1, &buf)) < 0)
		{
			CloseTable(&tb_sys_menu);
			TRACE("读取菜单定义表失败!\r\n");

			return -1;
		}

		count = ret / size;

		for (int i = 0; i < count; i++)
		{
			memcpy(&menu_info, buf + i * size, size);

			if (strcmp(menu_info.display_value, "dscada") == 0
					&& strcmp("应用类型", menu_info.menu_name) == 0)
			{
				CloseTable(&tb_sys_menu);

				return 1 << (menu_info.actual_value - 1);
			}
		}

		BufFree(buf);
		CloseTable(&tb_sys_menu);
	}
	else
	{
		TRACE("打开菜单定义表失败!\r\n");
	}

	return -1;
}

const int record_app_dscada = GetDScadaMask();


//打开库表
bool InitDB(const char *scd1,const char *scd2)
{
	g_db_scada = new RTDB;
	g_db_dscada = new RTDB;

	if (g_db_scada->Init("", "realtime", "scada", scd1, 1) == OO_FAIL 
		|| g_db_dscada->Init("", "realtime", "dscada", scd2, 1) == OO_FAIL)
	{
		delete g_db_scada;
		delete g_db_dscada;
		return false;
	}

	return true;
}

//筛选需要同步的设备
bool SiftEquipment()
{
	cout<<"触发同步!!!"<<endl;

		
	char *buf = NULL;
	int cnt_record;
	int ret, i;
	std::map<long, std::list<long> > map_pntid_id;
	std::map<long, std::list<long> > map_algid_id;
	
	std::map<long, std::list<long> >::iterator it;

	AutoLock lock(mutex_sync);
	
	map_pntid_id.clear();
	map_algid_id.clear();
	
	
	map_breaker_measpoint.clear();
	map_disconnector_measpoint.clear();
	map_busbarsection_measpoint.clear();
	map_disconnector_measanalog.clear();
	map_breaker_measanalog.clear();
	map_busbarsection_measanalog.clear();

	//先将遥信表、遥测表的遥信/测ID与记录ID建立映射关系，方便后面查找
	if ((ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_scada->GetTB("measpoint")), 1, &buf)) < 0)
	{
		TRACE("读取measpoint表记录失败！\r\n");
		return false;
	}

	cnt_record = ret / size_measpoint;

	for (i = 0; i < cnt_record; i++)
	{
		memcpy(&measpoint, buf + i * size_measpoint, size_measpoint);

		map_pntid_id[(measpoint.pnt_id&Transition_id)].push_back(measpoint.id);
	}
	
	BufFree(buf);
	
	
	///////////////////////////////////////
	if ((ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_scada->GetTB("measanalog")), 1, &buf)) < 0)
	{
		TRACE("读取measanalog表记录失败！\r\n");
		return false;
	}

	cnt_record = ret / size_measanalog;

	for (i = 0; i < cnt_record; i++)
	{
		memcpy(&measanalog, buf + i * size_measanalog, size_measanalog);

		map_algid_id[(measanalog.alg_id&Transition_id)].push_back(measanalog.id);
	}
	
	BufFree(buf);
	
	
	//筛选断路器设备
	if ((ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_scada->GetTB("breaker")), 1, &buf)) < 0)
	{
		TRACE("读取breaker表记录失败！\r\n");
		return false;
	}

	cnt_record = ret / size_breaker;

	for (i = 0; i < cnt_record; i++)
	{
		memcpy(&breaker, buf + i * size_breaker, size_breaker);
		
		if (breaker.record_app & record_app_dscada)
		{
			map_breaker_measpoint[breaker.id] = (it = map_pntid_id.find(breaker.id)) == map_pntid_id.end() ? std::list<long>():it->second;
			map_breaker_measanalog[breaker.id] = (it = map_algid_id.find(breaker.id))== map_algid_id.end() ? std::list<long>():it->second;
		}
	}
	
	BufFree(buf);
	
	
	//筛选刀闸设备
	if ((ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_scada->GetTB("disconnector")), 1, &buf)) < 0)
	{
		TRACE("读取disconnector表记录失败！\r\n");
		return false;
	}

	cnt_record = ret / size_disconnector;

	for (i = 0; i < cnt_record; i++)
	{
		memcpy(&disconnector, buf + i * size_disconnector, size_disconnector);

		if (disconnector.record_app & record_app_dscada)
		{
			map_disconnector_measpoint[disconnector.id] = (it = map_pntid_id.find(disconnector.id)) == map_pntid_id.end() ? std::list<long>():it->second;
			map_disconnector_measanalog[disconnector.id] = (it = map_algid_id.find(disconnector.id))== map_algid_id.end() ? std::list<long>():it->second;
		}
	}
	
	BufFree(buf);
	
	
	//筛选母线设备
	if ((ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_scada->GetTB("busbarsection")), 1, &buf)) < 0)
	{
		TRACE("读取busbarsection表记录失败！\r\n");
		return false;
	}

	cnt_record = ret / size_busbarsection;

	for (i = 0; i < cnt_record; i++)
	{
		memcpy(&busbarsection, buf + i * size_busbarsection, size_busbarsection);

		if (busbarsection.record_app & record_app_dscada)
		{
			map_busbarsection_measpoint[busbarsection.id] = (it = map_pntid_id.find(busbarsection.id)) == map_pntid_id.end() ? std::list<long>():it->second;
			map_busbarsection_measanalog[busbarsection.id] = (it = map_algid_id.find(busbarsection.id))== map_algid_id.end() ? std::list<long>():it->second;
		}
	}
	BufFree(buf);
	
	return true;
}

//同步挂牌信息的数据
bool sync_token_data(token_info_dscada *d_token,token_info_scada *token){

		memcpy(d_token->graph_name,token->graph_name,sizeof(token->graph_name));
		d_token->device_id = token->device_id;
		d_token->token_id = token->token_id;
		d_token->device_pos_y= token->device_pos_y;
		d_token->device_pos_x= token->device_pos_x;
		d_token->token_no= token->token_no;
		memcpy(d_token->note,token->note,sizeof(token->note));
		d_token->if_display= token->if_display;
		d_token->father_token_id= token->father_token_id;
		d_token->record_app= token->record_app;
		memcpy(d_token->user_name,token->user_name,sizeof(token->user_name));
		d_token->reserv_int1= token->reserv_int1;
		d_token->reserv_int2= token->reserv_int2;
		d_token->reserv_int3= token->reserv_int3;
		d_token->reserv_int4= token->reserv_int4;
		d_token->reserv_int5= token->reserv_int5;
		d_token->reserv_int6= token->reserv_int6;
		d_token->reserv_int7= token->reserv_int7;
		d_token->reserv_int8= token->reserv_int8;
		memcpy(d_token->reserv_str1,token->reserv_str1,sizeof(token->reserv_str1));
		memcpy(d_token->reserv_str2,token->reserv_str2,sizeof(token->reserv_str2));
		memcpy(d_token->reserv_str3,token->reserv_str3,sizeof(token->reserv_str3));
		memcpy(d_token->reserv_str4,token->reserv_str4,sizeof(token->reserv_str4));
		d_token->flag1= token->flag1;
		d_token->flag2= token->flag2;
		d_token->flag3= token->flag3;
		d_token->flag4= token->flag4;
		d_token->st_id= token->st_id;
		d_token->resp_area= token->resp_area;
		
		return true;
}

bool sync_point(char *table_name,std::map<long, std::list<long> > map_test,char *p)
{
	std::map<long, std::list<long> >::iterator it_map;
	std::list<long>::iterator it_list;
	
	for (it_map = map_test.begin(); it_map != map_test.end(); it_map++)
	{
		for(it_list = it_map->second.begin();it_list != it_map->second.end();it_list++){
			if (GetRecByID(const_cast<TB_DESCR*>(g_db_scada->GetTB(table_name)),(char*) &(*it_list), size_measpoint, (char*) &measpoint) >= 0
					&& GetRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB(table_name)), (char*) &(*it_list), size_d_measpoint, (char*) &d_measpoint) >= 0)
			{
				d_measpoint.qual = measpoint.qual;
				d_measpoint.value = measpoint.value;
				
				if(UpdateRecByID (const_cast<TB_DESCR*>(g_db_dscada->GetTB(table_name)), (char*)&(*it_list), size_d_measpoint, (char*)&d_measpoint) < 0 ){
					TRACE("更新 %s表记录失败ID = %ld %s \r\n",table_name,it_map->first,p);
				}
			}else{
				TRACE("得到%s表记录失败ID = %ld %s \r\n",table_name,it_map->first,p);
			}
		}
	}
	return true;
}

bool sync_analog(char *table_name,std::map<long, std::list<long> > map_test,char *p)
{
	std::map<long, std::list<long> >::iterator it_map;
	std::list<long>::iterator it_list;
	
	for (it_map = map_test.begin(); it_map != map_test.end(); it_map++)
	{
		for(it_list = it_map->second.begin();it_list != it_map->second.end();it_list++){
			if (GetRecByID(const_cast<TB_DESCR*>(g_db_scada->GetTB(table_name)),(char*) &(*it_list), size_measanalog, (char*) &measanalog) >= 0
					&& GetRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB(table_name)), (char*) &(*it_list), size_d_measanalog, (char*) &d_measanalog) >= 0)
			{
				d_measanalog.qual = measanalog.qual;
				d_measanalog.value = measanalog.value;
				
				if(UpdateRecByID (const_cast<TB_DESCR*>(g_db_dscada->GetTB(table_name)), (char*)&(*it_list), size_d_measanalog, (char*)&d_measanalog) < 0 ){
					TRACE("更新 %s表记录失败ID = %ld %s \r\n",table_name,it_map->first,p);
				}
			}else{
				TRACE("得到%s表记录失败ID = %ld %s \r\n",table_name,it_map->first,p);
			}
		}
	}
	return true;
}

bool delete_token_info(){
	//因为上述的同步删除逻辑并没有考虑到当只有配网拥有挂牌ID时，主网不会对其进行删除操作，所以添加当前信息同步后，反向验证如果没有则进行删除
	int ret;
	char *buf = NULL;
	std::list<long> list_d_token;
	std::list<long>::iterator it_list;
	list_d_token.clear();
	if (( ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_dscada->GetTB("token_info")), 1, &buf)) < 0)
	{
		TRACE("读取d_token_info表记录失败！\r\n");
		return false;
	}

	int cnt_record = ret / size_d_token;

	for (int i = 0; i < cnt_record; i++)
	{
		memcpy(&d_token, buf + i * size_d_token, size_d_token);
		list_d_token.push_front(d_token.token_id);
	}
	BufFree(buf);
	//上述已经将配网同步后的挂牌信息加入到容器中，反向验证主网，如果没有就删除配网信息
	for(it_list = list_d_token.begin(); it_list != list_d_token.end();it_list++){
		if(GetRecByID(const_cast<TB_DESCR*>(g_db_scada->GetTB("token_info")), (char*) &(*it_list), size_d_token, (char*) &d_token) < 0){
			if( DeleteRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB("token_info")), (char*)&(*it_list)) < 0){
				TRACE("delete d_token_info 表记录失败  %ld\r\n",d_token.token_id);
			}
		}
	}
	return true;
}

bool sync_token_info(std::map<long, std::list<long> > map_test,char *p){
	
	std::map<long, std::list<long> >::iterator it_map;
	std::list<long>::iterator it_list;
	
	for (it_map = map_test.begin(); it_map != map_test.end(); it_map++)
	{
		for(it_list = it_map->second.begin();it_list != it_map->second.end();it_list++){
			
			if(GetRecByID(const_cast<TB_DESCR*>(g_db_scada->GetTB("token_info")), (char*) &(*it_list), size_token, (char*) &token) >= 0){
				
				sync_token_data(&d_token,&token);
				//同步挂牌信息，如果配网有就更新，没有就插入
				if(GetRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB("token_info")), (char*)&(*it_list), size_d_token, (char*) &d_token) >= 0){
					if(UpdateRecByID (const_cast<TB_DESCR*>(g_db_dscada->GetTB("token_info")), (char*)&(*it_list), size_d_token, (char*)&d_token) < 0 ){
						TRACE("更新token_info %s表记录失败！\r\n",p);
					}					
				}
				else{
					if(InsertRec(const_cast<TB_DESCR*>(g_db_dscada->GetTB("token_info")), 1,size_d_token,(char*)&d_token,1) >= 0 ){
						TRACE("插入token_info %s 表记录失败 %ld %ld\r\n",p,d_token.token_id,*it_list);
					}
				}
			}/*else{
				if(GetRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB("token_info")), (char*) &(*it_list), size_d_token, (char*) &d_token) >= 0){
					if( DeleteRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB("token_info")), (char*)&(*it_list)) >= 0){
						TRACE("delete token_info %s 表记录失败  %ld\r\n",p,d_token.token_id);
					}
				}
			}*/
		}
	}
	return true;
}


bool get_token_info()
{
	std::map<long, std::list<long> > map_tokenid_id;
	std::map<long, std::list<long> >::iterator it, it1;

	map_tokenid_id.clear();
	char *buf = NULL;
	int ret;

	map_disconnector_token.clear();
	map_breaker_token.clear();
	map_busbarsection_token.clear();

	//创建挂牌信息和设备ID的对应关系
	if ((ret = GetTableRecs(const_cast<TB_DESCR*>(g_db_scada->GetTB("token_info")), 1, &buf)) < 0)
	{
		TRACE("读取token_info表记录失败！\r\n");

		return false;
	}

	int cnt_record = ret / size_token;

	for (int i = 0; i < cnt_record; i++)
	{
		memcpy(&token, buf + i * size_token, size_token);

		map_tokenid_id[token.device_id].push_back(token.token_id);
	}

	BufFree(buf);

	for (it1 = map_breaker_measpoint.begin(); it1 != map_breaker_measpoint.end(); it1++)
	{
		map_breaker_token[it1->first] = (it = map_tokenid_id.find(it1->first)) == map_tokenid_id.end() ? std::list<long>() : it->second;
	}

#ifdef BUG
	TRACE("获得map_breaker_token 表记录 %d\r\n",map_breaker_token.size());
#endif

	for (it1 = map_busbarsection_measpoint.begin(); it1 != map_busbarsection_measpoint.end(); it1++)
	{
		map_busbarsection_token[it1->first] = (it = map_tokenid_id.find(it1->first)) == map_tokenid_id.end() ? std::list<long>() : it->second;
	}

#ifdef BUG
	TRACE("获得map_busbarsection_token 表记录 %d\r\n",map_busbarsection_token.size());
#endif

	for (it1 = map_disconnector_measpoint.begin(); it1 != map_disconnector_measpoint.end(); it1++)
	{
		map_disconnector_token[it1->first] = (it = map_tokenid_id.find(it1->first)) == map_tokenid_id.end() ? std::list<long>() : it->second;
	}

#ifdef BUG
	TRACE("获得map_disconnector_token 表记录 %d\r\n",map_disconnector_token.size());
#endif

	//同步
	sync_token_info(map_breaker_token, "breaker");
	sync_token_info(map_disconnector_token, "disconnector");
	sync_token_info(map_busbarsection_token, "busbarsection");
	
	//删除配网多余挂牌信息
	delete_token_info();

	return true;
}


bool SyncData()
{
	cout<<"开始同步数据"<<endl;

	AutoLock lock(mutex_sync);
	time_t	_tm;
	
	std::map<long, std::list<long> >::iterator it_map;
	std::list<long>::iterator it_list;
	
#ifdef BUG
	TRACE("%s 同步数据 cb.size = %d\r\n", ctime(&(_tm =time(NULL))), map_breaker_measpoint.size());
#endif
	
	for (it_map = map_breaker_measpoint.begin(); it_map != map_breaker_measpoint.end(); it_map++)
	{
		if (GetRecByID(const_cast<TB_DESCR*>(g_db_scada->GetTB("breaker")), (char*) &it_map->first, size_breaker, (char*) &breaker) >= 0
				&& GetRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB("breaker")), (char*) &it_map->first, size_d_breaker, (char*) &d_breaker) >= 0)
		{
			d_breaker.point = breaker.point;
			d_breaker.status = breaker.status;
			
			if(UpdateRecByID (const_cast<TB_DESCR*>(g_db_dscada->GetTB("breaker")), (char*)&it_map->first, size_d_breaker, (char*)&d_breaker) < 0 ){
				TRACE("更新d_breaker表记录失败！\r\n");
			}
		}else{

			TRACE("得到d_breaker表记录失败！\r\n");
		}
	}
	
#ifdef BUG
	TRACE("%s 同步数据 disconnector.size = %d\r\n", ctime(&(_tm =time(NULL))), map_disconnector_measpoint.size());
#endif
	
	for (it_map = map_disconnector_measpoint.begin(); it_map != map_disconnector_measpoint.end(); it_map++)
	{
		if (GetRecByID(const_cast<TB_DESCR*>(g_db_scada->GetTB("disconnector")), (char*) &it_map->first, size_disconnector, (char*) &disconnector) >= 0
				&& GetRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB("disconnector")), (char*) &it_map->first, size_d_disconnector, (char*) &d_disconnector) >= 0)
		{
			d_disconnector.point = disconnector.point;
			d_disconnector.status = disconnector.status;
			
			if(UpdateRecByID (const_cast<TB_DESCR*>(g_db_dscada->GetTB("disconnector")), (char*)&it_map->first, size_d_disconnector, (char*)&d_disconnector) < 0 ){
				TRACE("更新d_disconnector表记录失败！\r\n");
			}
		}

	}

#ifdef BUG
	TRACE("%s 同步数据 bus.size = %d\r\n", ctime(&(_tm =time(NULL))), map_busbarsection_measpoint.size());
#endif
	
	for (it_map = map_busbarsection_measpoint.begin(); it_map != map_busbarsection_measpoint.end(); it_map++)
	{
		if (GetRecByID(const_cast<TB_DESCR*>(g_db_scada->GetTB("busbarsection")), (char*) &it_map->first, size_busbarsection, (char*) &busbarsection) >= 0
				&& GetRecByID(const_cast<TB_DESCR*>(g_db_dscada->GetTB("busbarsection")), (char*) &it_map->first, size_d_busbarsection, (char*) &d_busbarsection) >= 0)
		{
			d_busbarsection.v = busbarsection.v;
			d_busbarsection.status = busbarsection.status;
			
			if(UpdateRecByID (const_cast<TB_DESCR*>(g_db_dscada->GetTB("busbarsection")), (char*)&it_map->first, size_d_busbarsection, (char*)&d_busbarsection) < 0 ){
				TRACE("更新d_busbarsection表记录失败！\r\n");
			}
		}

	}
#ifdef BUG
	TRACE("%s 同步数据 map_breaker_measpoint.size = %d   map_disconnector_measpoint  = %d map_busbarsection_measpoint = %d\r\n",
			ctime(&(_tm =time(NULL))),	map_breaker_measpoint.size(),map_disconnector_measpoint.size(),map_busbarsection_measpoint.size());
#endif
	
	sync_point("measpoint",map_breaker_measpoint,"breaker");
	sync_point("measpoint",map_disconnector_measpoint,"disconnector");
	sync_point("measpoint",map_busbarsection_measpoint,"busbarsection");

#ifdef BUG
	TRACE("%s 同步数据 map_disconnector_measanalog.size = %d   map_busbarsection_measanalog  = %d  map_breaker_measanalog = %d\r\n"
			, ctime(&(_tm =time(NULL))), map_disconnector_measanalog.size(),map_busbarsection_measanalog.size(),map_breaker_measanalog.size());
#endif
	
	sync_analog("measanalog",map_disconnector_measanalog,"disconnector");
	sync_analog("measanalog",map_busbarsection_measanalog,"busbarsection");
	sync_analog("measanalog",map_breaker_measanalog,"breaker");

	//挂牌信息同步，主网同步到配网
	//get_token_info();
	
	return true;
}

int main(int _argc, char *_argv[])
{
	const char *p1 = GetAppHost(AP_SCADA);
	const char *p2 = GetAppHost(AP_DSCADA);
	int count = 0;

	if (!InitDB(p1, p2))
	{
		return -1;
	}

	CScaCommonFactory *scacommFactory;
	GetScaCommonFactory(&scacommFactory);
	g_sca_common = scacommFactory->CreateScaCommon();
	scacommFactory->Release();

	model_chg_ntf = &tmp_val0;
	last_model_chg_ntf = -1;

	CContext::SetContextNo(_argc, _argv, AC_REALTIME_NO);
	g_sca_common->DbNotifyAt(SMNG_MODEL_NO, &model_chg_ntf, AP_SCADA);

	if (pthread_mutex_init(&mutex_sync, NULL))
	{
		TRACE("Init mutex_sync error");

		return -1;
	}

	while (true)
	{
		//如果模型改变触发同步程序，现同步 母线 断路器 刀闸
		if (*model_chg_ntf != last_model_chg_ntf)
		{
			TRACE("模型结构发生变化！%ld\r\n", *model_chg_ntf);

			last_model_chg_ntf = *model_chg_ntf;

			SiftEquipment();
		}

		//每三秒同步一次数据
		if ((count = (count++) % 3) == 0)
		{
			SyncData();
		}

		sleep(1);
	}

	return 0;
}
