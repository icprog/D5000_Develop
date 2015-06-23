#ifndef SWITCH_MANAGE_H_
#define SWITCH_MANAGE_H_

#include <iostream>
#include <map>
#include <deque>
#include <string>
#include <set>
#include <vector>
#include "Nmd.h"


struct Element_Info
{
	long id;
	long ind;
	long jnd;
	int direct;  // 0 unspecify  1-->ind,  2-->jnd
	Element_Info();
	Element_Info(long id_, long ind_, long jnd_);
	Element_Info(const Element_Info& other);
	Element_Info& operator=(const Element_Info& other);
	friend std::ostream& operator << (std::ostream& os, const Element_Info& e);
};

class Switch_Manager
{
public:
	//若该点可以跳跃，则返回跳跃后的点，否则返回空
	long find_jump_point(const long& p);

	//查找p1和p2之间的所有刀闸，如果找到将结果存在ret_deq中
	bool find_switch_between(const long& p1, const long& p2,std::deque<long>& ret_deq);

	//增加一个刀闸
	void add_switch(long id, long ind, long jnd);

	//建立刀闸的映射， 必须在所有刀闸都添加完后调用
	void creat_map();

	//返回唯一实例
	static Switch_Manager* instance();

	//查找某一设备是否跳过刀闸
	bool is_ignore_switch(const long& dev_id);

	//添加一个跳过刀闸的设备
	void add_ignore_dev(const long& dev_id);

	//查找两个DEV之间的所有刀闸，结果存在ret_deq中
	void find_switch_by_dev(DEV& first,DEV& second, std::deque<long>& ret_deq);

	//查找与某个设备相连的所有刀闸
	bool find_so_area_switch(long& dev_id,std::string& dev_tb, std::deque<long>& ret_deq);

	//将所有刀闸的共享点信息保存在当前目录下的shared_point.txt中
	void print_shared_nd();

	//增加原始节点和跳后节点信息
	void add_org_skip_node(const long& org, const long& skip);

	//通过刀闸的id找刀闸跳节点前的信息找cb的id信息
	Element_Info* get_cb_info_by_id(const long& cb_id);

	//通过共享点找刀闸
	bool find_switch_by_shared_point(const long& shared_point,std::deque<Element_Info>& ret_deq);

private:

	//通过共享点找刀闸
	bool find_switch_by_shared_point(const long& shared_point,std::deque<long>& ret_deq);

	//查找下一个刀闸
	int find_next_switch(Element_Info e, std::deque<long>& ret_deq);
	Switch_Manager();
	~Switch_Manager();
	Switch_Manager(const Switch_Manager& other);
	Switch_Manager& operator=(const Switch_Manager& other);

	//  找到起点相连通的所有点
	void find_adjecent_point(const long& point, std::vector<long>& vec);

private:
	static Switch_Manager* 				m_instance;
	std::multimap<long, Element_Info*>	m_all_daoza_ind; //保存刀闸的ind和刀闸信息对应关系
	std::multimap<long, Element_Info*>	m_all_daoza_jnd; //保存刀闸的jnd和刀闸信息对应关系
	std::multimap<long, long>		    m_nd_map;        //保存刀闸的ind和jnd的对应关系
	std::map<long, Element_Info*>       m_cbid_map;      //刀闸的id和刀闸信息对应关系

	std::map<long, long>    			m_shared_point;  //保存某一点和共享点的对应关系
	std::set<long>				 		m_visit_daoza;   //保存已访问过的刀闸id信息
	long  						        m_dst_point;     //保存find_switch_between函数中的p2，方便查找
	bool								m_is_find_path;  //标记find_switch_between结果
	std::deque<long>				    m_shortest_path; //当有多条路径时，保存最短的那条路径
	bool								is_multi_path;   //标记是否是多路径查找
	std::set<long>          			m_ignore_id;     //保存有跳刀闸的设备id
	std::multimap<long, long>		    m_skip_org;		 //保存原 始节点和跳后节点信息
	std::set<long>                      m_visit_ind;     //保存已访问过的ind
	std::set<long>						m_visit_jnd;	 //保存已访问过的jnd
};
#endif  //end SWITCH_MANAGE_H_

