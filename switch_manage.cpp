
/**********************
switch_manage.cpp

功能:实现跳刀闸,由CC2000环境下移植到D5000环境下

Created on: 2013-9-9
Author: shengpengfei

**********************/

#include "switch_manage.h"
#include "dsbob2.h"
#include "DBPSBOB.h"
#include <db_struct_scada.h>
#include <sys_menu_info.h>
#include <rtdb_api.h>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>


extern DB_PSBOB *g_db_psbob;

typedef energyconsumer_scada 	LD;
typedef aclinesegment_scada		SEC;
typedef breaker_scada			CB;
typedef busbarsection_scada		BUS;




Element_Info::Element_Info() 
{
	id  = 0;
	ind = 0;
	jnd = 0;
}

Element_Info::Element_Info(long id_, long ind_, long jnd_):direct(0)
{
	id  = id_;
	ind = ind_;
	jnd = jnd_;
}

Element_Info::Element_Info(const Element_Info& other)
{
	id     = other.id;
	ind    = other.ind;
	jnd    = other.jnd;
	direct = other.direct;
}

Element_Info& Element_Info::operator=(const Element_Info& other)
{
	if(this != &other)
	{
		id     = other.id;
		ind    = other.ind;
		jnd    = other.jnd;
		direct = other.direct;

	}

	return *this;
}

std::ostream& operator << (std::ostream& os, const Element_Info& e)
{
	os << "id[" << e.id << "]  ind[" << e.ind << "]  jnd[" << e.jnd << "]";
	return os;
}

//////////////////////////////////////////////////////////////////////////////
Switch_Manager* Switch_Manager::m_instance = NULL;
Switch_Manager* Switch_Manager::instance()
{
	if(!m_instance)
		m_instance = new Switch_Manager();
	return m_instance;
}

void Switch_Manager::print_shared_nd()
{
	std::ofstream fout("shared_point.txt");
	std::map<long, long>::iterator it = m_shared_point.begin();
	for(; it != m_shared_point.end(); ++it)
		fout << "key=" << (*it).first << "-------------value=" << (*it).second << std::endl;
	fout.close();
}

void Switch_Manager::creat_map()
{
	std::cout << "---------------------------begin create_map---------\n";
	time_t start_creat = time(0);
	std::multimap<long, long>::iterator iter_nd_znd = m_nd_map.begin();
	std::vector<long> ret_vec; 
	std::vector<long>::iterator iter_vec;
	long shared_point = 0; 
	for(; iter_nd_znd != m_nd_map.end(); ++iter_nd_znd)
	{
		ret_vec.clear();
		shared_point = (*iter_nd_znd).first;
		find_adjecent_point((*iter_nd_znd).first, ret_vec);
		ret_vec.push_back((*iter_nd_znd).first);
		ret_vec.push_back((*iter_nd_znd).second);
		if(ret_vec.empty())
			continue;

		for(iter_vec = ret_vec.begin(); iter_vec != ret_vec.end(); ++iter_vec)
			m_shared_point.insert(std::make_pair(*iter_vec, shared_point));

	}
	m_visit_ind.clear();
	m_visit_jnd.clear();
	std::cout << "---------------------------create map spend " << time(0) - start_creat << "s" << std::endl;
//	print_shared_nd();
}

long Switch_Manager::find_jump_point(const long& p)
{
	long ret=0;   
	std::map<long, long>::iterator it = m_shared_point.find(p);
	if(it != m_shared_point.end())
		ret = (*it).second;     

	return ret;
}


bool Switch_Manager::find_switch_between(const long& p1, const long& p2,std::deque<long>& ret_deq)
{
	//初始化一些相关的变量 
	ret_deq.clear();
	m_visit_daoza.clear();
	m_shortest_path.clear();
	is_multi_path = false;
	m_is_find_path = false;
	m_dst_point = p2;
	int src_numind, src_numjnd, dst_numind, dst_numjnd;
	long dzid;

	if(p1 <= 0 || p2 <= 0)
		return false;

	//先确定给定的这两点是否和刀闸直接相连，若不是，则不用继续查找
	Element_Info e;
	typedef std::multimap<long, Element_Info*>::iterator Iter;
	src_numind = m_all_daoza_ind.count(p1);
	src_numjnd = m_all_daoza_jnd.count(p1);
	dst_numind = m_all_daoza_ind.count(p2);
	dst_numjnd = m_all_daoza_jnd.count(p2);
	if(src_numind == 0 && src_numjnd == 0)
		return false;
	if(dst_numind == 0 && dst_numjnd == 0)
		return false;

	if(src_numind != 0 && src_numjnd != 0)
		is_multi_path = true;

	if(src_numind > 0) //若p1直接与刀闸的ind相连的情况
	{
		if(src_numind > 1)
			is_multi_path = true;

		Iter it_nd = m_all_daoza_ind.find(p1);
		for(int i = 0; i < src_numind; ++i)
		{
			if(m_is_find_path)
				return true;

			ret_deq.clear();
			e = *((*it_nd).second);
			dzid = e.id;
			m_visit_daoza.insert(dzid);
			e.direct = 1;
			find_next_switch(e, ret_deq);
			if(m_is_find_path)
				return true;
			ret_deq.clear();
			e.direct = 2;
			find_next_switch(e, ret_deq);
			++it_nd;
		}
	}

	m_visit_daoza.clear();

	if(m_is_find_path)
		return true;

	ret_deq.clear();

	if(src_numjnd > 0) //若p1直接与刀闸的jnd相连的情况
	{
		if(src_numjnd > 1)
			is_multi_path = true;

		Iter it_jnd = m_all_daoza_jnd.find(p1);
		for(int j = 0; j < src_numjnd; ++j)
		{
			if(m_is_find_path)
				return true;
			ret_deq.clear();
			e = *((*it_jnd).second); 
			dzid = e.id;
			m_visit_daoza.insert(dzid);
			e.direct = 2;
			find_next_switch(e, ret_deq);
			if(m_is_find_path)
				return true;
			ret_deq.clear();
			e.direct = 1;
			find_next_switch(e, ret_deq);
			++it_jnd;
		}
	}
	if(is_multi_path && !m_shortest_path.empty())
	{
		ret_deq = m_shortest_path;
		m_is_find_path = true;

	}

	return m_is_find_path;

}

void Switch_Manager::add_switch(long id, long ind, long jnd)
{
	Element_Info* e = new Element_Info(id, ind, jnd);

	m_all_daoza_ind.insert(std::make_pair(ind, e));
	m_all_daoza_jnd.insert(std::make_pair(jnd, e));
	m_nd_map.insert(std::make_pair(ind, jnd));
	m_cbid_map.insert(std::make_pair(id, e));
}

Switch_Manager::Switch_Manager()
{

}

Switch_Manager::~Switch_Manager()
{

}

int Switch_Manager::find_next_switch(Element_Info e, std::deque<long>& ret_deq)
{
	if(m_is_find_path)
		return 0;

	ret_deq.push_back(e.id);
	std::list<Element_Info> e_childs;
	typedef std::multimap<long, Element_Info*>::iterator Iter;
	int numind, numjnd;
	int i = 0;

	if( 1 == e.direct)//从ind穿过去 查找下个刀闸
	{
		if(e.jnd == m_dst_point || e.ind == m_dst_point)
		{
			if(is_multi_path)
			{
				if(m_shortest_path.empty())//不是最短路径 ，则将该路径保存为最短路径
				{
					m_shortest_path = ret_deq;
				}

				if(ret_deq.size() <  m_shortest_path.size())
					m_shortest_path = ret_deq;
			}else 
			{
				m_is_find_path = true;
				return 0;
			}
		}

		///find in m_all_daoza_jnd;

		numind = m_all_daoza_jnd.count(e.jnd);
		Iter it_jnd = m_all_daoza_jnd.find(e.jnd);
		for(i = 0; i < numind; ++i)
		{

			long dz = (*it_jnd).second->id;
			if(m_visit_daoza.find(dz) == m_visit_daoza.end())
			{
				m_visit_daoza.insert(dz);
				Element_Info tmpe = *((*it_jnd).second);
				tmpe.direct = 2;
				e_childs.push_back(tmpe);
			}

			++it_jnd;
		}

		///find in m_all_daoza_ind

		numind = m_all_daoza_ind.count(e.jnd);
		Iter it_ind = m_all_daoza_ind.find(e.jnd);
		for(i = 0; i < numind; ++i)
		{
			long dz = (*it_ind).second->id;
			if(m_visit_daoza.find(dz) == m_visit_daoza.end()) //该刀闸没有被访问过
			{
				m_visit_daoza.insert(dz);
				Element_Info tmpe = *((*it_ind).second);
				tmpe.direct = 1;
				e_childs.push_back(tmpe);
			}

			++it_ind;
		}


	}
	else if (2 == e.direct)
	{
		if(e.ind == m_dst_point || e.jnd == m_dst_point)
		{
			if(is_multi_path)
			{
				if(m_shortest_path.empty())
				{
					m_shortest_path = ret_deq;
				}

				if(ret_deq.size() <  m_shortest_path.size())
					m_shortest_path = ret_deq;

			}else 
			{
				m_is_find_path = true;
				return 0;
			}
		}

		///find in m_all_daoza_jnd;
		numind = m_all_daoza_jnd.count(e.ind);
		Iter it_jnd = m_all_daoza_jnd.find(e.ind);
		for(i = 0; i < numind; ++i)
		{
			long dz = (*it_jnd).second->id;
			if(m_visit_daoza.find(dz) == m_visit_daoza.end())//该刀闸没有被访问过
			{
				m_visit_daoza.insert(dz);
				Element_Info tmpe = *((*it_jnd).second);
				tmpe.direct = 2;
				e_childs.push_back(tmpe);
			}
			++it_jnd;
		}

		////////////find in m_all_daoza_ind

		numjnd =  m_all_daoza_ind.count(e.ind);
		Iter it_ind = m_all_daoza_ind.find(e.ind);
		for(int j = 0; j < numjnd; ++j)
		{
			long dz = (*it_ind).second->id;
			if(m_visit_daoza.find(dz) == m_visit_daoza.end())
			{
				m_visit_daoza.insert(dz);
				Element_Info tmpe = *((*it_ind).second);
				tmpe.direct = 1;
				e_childs.push_back(tmpe);
			}

			++it_ind;
		}
	}


	//递归对刀闸的孩子调用find_next_switch
	std::list<Element_Info>::iterator it_list = e_childs.begin();
	for(; it_list != e_childs.end(); ++it_list)
	{
		if(m_is_find_path)
			return 0;
		find_next_switch(*it_list, ret_deq);
	}

	if(!m_is_find_path)
		ret_deq.pop_back();

	return 0;
}

bool Switch_Manager::is_ignore_switch(const long& dev_id)
{
	if( m_ignore_id.find(dev_id) != m_ignore_id.end())//找到要跳过的设备ID
		return true;

	return false;
}

void Switch_Manager::add_ignore_dev(const long& dev_id)
{
	m_ignore_id.insert(dev_id);
}

void Switch_Manager::find_switch_by_dev(DEV& first,DEV& second, std::deque<long>& ret_deq)
{
	//电源追踪查询 查询相连的刀闸设备

	long st_ind  = 0;
	long st_jnd  = 0;
	long end_ind = 0;
	long end_jnd = 0;

	CB* cb;
	SEC* sec;
	LD* ld;
	BUS* bus;

	int ret;

	if(first.table == "breaker")
	{ 
		cb = new CB;
		ret =  oodbread_rk(cb,&first.id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(CB));
		if(ret > 0)
		{
			st_ind = cb->ind;
			st_jnd = cb->jnd;
		}
		delete cb;

	}else if(first.table == "busbarsection")
	{
		bus = new BUS;
		ret =  oodbread_rk(bus, &first.id, const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(BUS));
		if(ret > 0)
		{
			st_ind = bus->nd;
		}
		delete bus;

	}else if(first.table == "energyconsumer")
	{
		ld = new LD;
		ret =  oodbread_rk(ld, &first.id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), sizeof(LD));
		if(ret > 0)
		{
			st_ind = ld->nd;
		}
		delete ld;

	}else if(first.table == "aclinesegment")
	{
		sec = new SEC;
		ret =  oodbread_rk(sec, &first.id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), sizeof(SEC));
		if(ret > 0)
		{
			st_ind = sec->ind;
			st_jnd = sec->jnd;
		}
		delete sec;
	}

	if(second.table == "breaker")
	{
		cb = new CB;
		ret =  oodbread_rk(cb,&second.id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(CB));
		if(ret > 0)
		{
			end_ind = cb->ind;
			end_jnd = cb->jnd;
		}
		delete cb;

	}else if(second.table == "busbarsection")
	{
		bus = new BUS;
		ret =  oodbread_rk(bus,&second.id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("busbarsection")), sizeof(BUS));
		if(ret > 0)
		{
			end_ind = bus->nd;
		}
		delete bus;
	}else if(second.table == "energyconsumer")
	{
		ld = new LD;
		ret =  oodbread_rk(ld, &second.id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("energyconsumer")), sizeof(LD));
		if(ret > 0)
		{
			end_ind = ld->nd;
		}
		delete ld;

	}else if(second.table == "aclinesegment")
	{
		sec = new SEC;
		ret =  oodbread_rk(sec, &second.id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), sizeof(SEC));
		if(ret > 0)
		{
			end_ind = sec->ind;
			end_jnd = sec->jnd;
		}
		delete sec;
	}

	long p1 = st_ind;
	long p2 = st_jnd;
	long p3 = end_ind;
	long p4 = end_jnd;
	long start_point=0;
	long end_point=0;

	///////如果这两个设备是直接相连,就不用补刀闸
	if(p1 == p3 && p1 != 0)
	{
		return ;
	}else if(p1 == p4 && p1 != 0)
	{
		return ;
	}else if(p2 == p3 && p2 != 0)
	{
		return ;
	}else if(p2 == p4 && p2 !=0)
	{
		return ;
	}

	long tmp1=0;
	long tmp2=0;
	long tmp3=0;
	long tmp4=0;

	std::map<long, long>::iterator it_map_shared;
	it_map_shared = m_shared_point.find(p1);
	if(it_map_shared != m_shared_point.end())
		tmp1 = it_map_shared->second;

	it_map_shared = m_shared_point.find(p2);
	if(it_map_shared != m_shared_point.end())
		tmp2 = it_map_shared->second;

	it_map_shared = m_shared_point.find(p3);
	if(it_map_shared != m_shared_point.end())
		tmp3 = it_map_shared->second;

	it_map_shared = m_shared_point.find(p4);
	if(it_map_shared != m_shared_point.end())
		tmp4 = it_map_shared->second;

	if(tmp1 == tmp3 && tmp1 != 0)
	{
		start_point = p1;
		end_point = p3;
	}else if(tmp1 == tmp4 && tmp1 != 0)
	{
		start_point = p1;
		end_point = p4;
	}else if(tmp2 == tmp3 && tmp2 != 0)
	{
		start_point = p2;
		end_point = p3;
	}else if(tmp2 == tmp4 && tmp2 != 0)
	{
		start_point = p2;
		end_point = p4;
	}

	if(start_point == 0 || end_point == 0)
	{
		return ;
	}

	find_switch_between(start_point, end_point, ret_deq);
}

bool Switch_Manager::find_switch_by_shared_point(const long& shared_point,std::deque<Element_Info>& ret_deq)
{
	std::multimap<long, Element_Info*>::iterator it_ele;
	Element_Info one_ele;
	int num, i;
	bool ret_val = false;
	std::vector<long> vec;
	std::map<long, long>::iterator iter_shared;
	std::set<long> set_visit_dz;
	std::set<long>::iterator iter_set;
	for(iter_shared = m_shared_point.begin(); iter_shared != m_shared_point.end(); ++iter_shared)
	{
		//把共享点等于shared_point的点都存到vec中 vec中存放的是共享点对应的节点信息
		if(shared_point == iter_shared->second) 
			vec.push_back(iter_shared->first);
	}

	std::vector<long>::iterator iter_vec;
	for(iter_vec = vec.begin(); iter_vec != vec.end(); ++iter_vec)
	{
		num = m_all_daoza_ind.count(*iter_vec);
		it_ele = m_all_daoza_ind.find(*iter_vec);
		for(i = 0; i < num; ++i)
		{
			iter_set = set_visit_dz.find((*it_ele).second->id);
			if(iter_set == set_visit_dz.end())
			{
				one_ele = *(it_ele->second);
				ret_deq.push_back(one_ele);
				if(!ret_val)
					ret_val = true;
				set_visit_dz.insert((*it_ele).second->id);
			}

			++it_ele;
		}

		num = m_all_daoza_jnd.count(*iter_vec);
		it_ele = m_all_daoza_jnd.find(*iter_vec);
		for(i = 0; i < num; ++i)
		{
			iter_set = set_visit_dz.find((*it_ele).second->id);
			if(iter_set == set_visit_dz.end())
			{
				one_ele = *(it_ele->second);
				ret_deq.push_back(one_ele);
				if(!ret_val)
					ret_val = true;
				set_visit_dz.insert((*it_ele).second->id);
			}
			++it_ele;
		}
	}

	return ret_val;

}

bool Switch_Manager::find_switch_by_shared_point(const long& shared_point, std::deque<long>& ret_deq )
{
	std::multimap<long, Element_Info*>::iterator it_ele;
	int num, i;
	bool ret_val = false;
	std::vector<long> vec;
	std::map<long, long>::iterator iter_shared;
	std::set<long> set_visit_dz;
	std::set<long>::iterator iter_set;
	for(iter_shared = m_shared_point.begin(); iter_shared != m_shared_point.end(); ++iter_shared)
	{
		//把共享点等于shared_point的点都存到vec中
		if(shared_point == iter_shared->second)
			vec.push_back(iter_shared->first);
	}

	std::vector<long>::iterator iter_vec;
	for(iter_vec = vec.begin(); iter_vec != vec.end(); ++iter_vec)
	{
		num = m_all_daoza_ind.count(*iter_vec);
		it_ele = m_all_daoza_ind.find(*iter_vec);
		for(i = 0; i < num; ++i)
		{
			iter_set = set_visit_dz.find((*it_ele).second->id);
			if(iter_set == set_visit_dz.end())
			{
				ret_deq.push_back((*it_ele).second->id);
				if(!ret_val)
					ret_val = true;
				set_visit_dz.insert((*it_ele).second->id);
			}
			++it_ele;
		}

		num = m_all_daoza_jnd.count(*iter_vec);
		it_ele = m_all_daoza_jnd.find(*iter_vec);

		for(i = 0; i < num; ++i)
		{
			iter_set = set_visit_dz.find((*it_ele).second->id);
			if(iter_set == set_visit_dz.end())
			{
				ret_deq.push_back((*it_ele).second->id);
				if(!ret_val)
					ret_val = true;
				set_visit_dz.insert((*it_ele).second->id);
			}

			++it_ele;
		}
	}

	return ret_val;
}

bool Switch_Manager::find_so_area_switch(long& dev_id,std::string& dev_tb, std::deque<long>& ret_deq)
{
	CB* cb;
	SEC* sec;
	long st_ind = 0;
	long st_jnd = 0;
	long shared_point =0;
	int ret;

	if(dev_tb == "breaker") //断路器设备时
	{
		cb = new CB;
		ret =  oodbread_rk(cb,&dev_id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("breaker")), sizeof(CB));
		if(ret > 0)
		{
			st_ind = cb->ind;
			st_jnd = cb->jnd;
		}
		delete cb;
	}else if(dev_tb == "aclinesegment")
	{
		sec = new SEC;
		ret =  oodbread_rk(sec,&dev_id,const_cast<TB_DESCR*>(g_db_psbob->GetTB("aclinesegment")), sizeof(SEC));
		if(ret > 0)
		{
			st_ind = sec->ind;
			st_jnd = sec->jnd;
		}
		delete sec;
	}

	if(st_ind > 0)
	{
		shared_point = find_jump_point(st_ind);
		if(shared_point)
		{    
			find_switch_by_shared_point(shared_point, ret_deq);
		}
	}

	if(st_jnd  > 0)
	{
		shared_point = find_jump_point(st_jnd);
		if(shared_point)
		{
			find_switch_by_shared_point(shared_point, ret_deq);
		}
	}

	if(ret_deq.empty())
		return false;

	return true;
}

void Switch_Manager::add_org_skip_node(const long& org, const long& skip)
{
	m_skip_org.insert(std::make_pair(skip, org));
}

void Switch_Manager::find_adjecent_point(const long& point, std::vector<long>& vec)
{

	//查找相连点 先查询所有的ind点 再查询所有的jnd点，都存放于tmp中 ，再在tmp中查询相连接的点 
	std::set<long>::iterator it_ind;
	std::set<long>::iterator it_jnd;
	vec.push_back(point);

	std::multimap<long, Element_Info*>::iterator  it_ele;
	std::vector<long> tmp_vec;
	int num = m_all_daoza_ind.count(point);
	int i = 0;
	it_ele = m_all_daoza_ind.find(point);
	for(i = 0; i < num; ++i)
	{
		it_ind = m_visit_ind.find(point);
		it_jnd = m_visit_jnd.find((*it_ele).second->jnd);
		if(it_ind != m_visit_ind.end() && it_jnd != m_visit_jnd.end())//找到了ind和jnd,表示已经访问过的节点信息
		{
			//do nothing
		}else
		{	//如果这个cb没有被访问过,则把它的端点存起来
			tmp_vec.push_back((*it_ele).second->jnd);
			if(it_ind == m_visit_ind.end())
				m_visit_ind.insert(point);
			if(it_jnd == m_visit_jnd.end())
				m_visit_jnd.insert((*it_ele).second->jnd);
		}

		++it_ele;
	}
	//查询所有的Jnd点
	num = m_all_daoza_jnd.count(point);
	it_ele = m_all_daoza_jnd.find(point);

	for(i = 0; i < num; ++i)
	{
		it_ind = m_visit_ind.find((*it_ele).second->ind);
		it_jnd = m_visit_jnd.find(point);
		if(it_ind != m_visit_ind.end() && it_jnd != m_visit_jnd.end())//已被访问过了
		{
			//do nothing
		}else
		{	//如果这个cb没有被访问过,则把它的端点存起来
			tmp_vec.push_back((*it_ele).second->ind);
			if(it_ind == m_visit_ind.end())
				m_visit_ind.insert((*it_ele).second->ind);
			if(it_jnd == m_visit_jnd.end())
				m_visit_jnd.insert(point);
		}

		++it_ele;
	}

	std::vector<long>::iterator iter_vec = tmp_vec.begin();
	for(; iter_vec != tmp_vec.end(); ++iter_vec)
	{
		find_adjecent_point(*iter_vec, vec);
	}
}

Element_Info* Switch_Manager::get_cb_info_by_id(const long& cb_id)
{
	Element_Info* e = NULL;
	std::map<long, Element_Info*>::iterator it = m_cbid_map.find(cb_id);
	if(it != m_cbid_map.end())
		e = it->second;
	return e;
}


