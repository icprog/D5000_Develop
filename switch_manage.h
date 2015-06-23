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
	//���õ������Ծ���򷵻���Ծ��ĵ㣬���򷵻ؿ�
	long find_jump_point(const long& p);

	//����p1��p2֮������е�բ������ҵ����������ret_deq��
	bool find_switch_between(const long& p1, const long& p2,std::deque<long>& ret_deq);

	//����һ����բ
	void add_switch(long id, long ind, long jnd);

	//������բ��ӳ�䣬 ���������е�բ�����������
	void creat_map();

	//����Ψһʵ��
	static Switch_Manager* instance();

	//����ĳһ�豸�Ƿ�������բ
	bool is_ignore_switch(const long& dev_id);

	//���һ��������բ���豸
	void add_ignore_dev(const long& dev_id);

	//��������DEV֮������е�բ���������ret_deq��
	void find_switch_by_dev(DEV& first,DEV& second, std::deque<long>& ret_deq);

	//������ĳ���豸���������е�բ
	bool find_so_area_switch(long& dev_id,std::string& dev_tb, std::deque<long>& ret_deq);

	//�����е�բ�Ĺ������Ϣ�����ڵ�ǰĿ¼�µ�shared_point.txt��
	void print_shared_nd();

	//����ԭʼ�ڵ������ڵ���Ϣ
	void add_org_skip_node(const long& org, const long& skip);

	//ͨ����բ��id�ҵ�բ���ڵ�ǰ����Ϣ��cb��id��Ϣ
	Element_Info* get_cb_info_by_id(const long& cb_id);

	//ͨ��������ҵ�բ
	bool find_switch_by_shared_point(const long& shared_point,std::deque<Element_Info>& ret_deq);

private:

	//ͨ��������ҵ�բ
	bool find_switch_by_shared_point(const long& shared_point,std::deque<long>& ret_deq);

	//������һ����բ
	int find_next_switch(Element_Info e, std::deque<long>& ret_deq);
	Switch_Manager();
	~Switch_Manager();
	Switch_Manager(const Switch_Manager& other);
	Switch_Manager& operator=(const Switch_Manager& other);

	//  �ҵ��������ͨ�����е�
	void find_adjecent_point(const long& point, std::vector<long>& vec);

private:
	static Switch_Manager* 				m_instance;
	std::multimap<long, Element_Info*>	m_all_daoza_ind; //���浶բ��ind�͵�բ��Ϣ��Ӧ��ϵ
	std::multimap<long, Element_Info*>	m_all_daoza_jnd; //���浶բ��jnd�͵�բ��Ϣ��Ӧ��ϵ
	std::multimap<long, long>		    m_nd_map;        //���浶բ��ind��jnd�Ķ�Ӧ��ϵ
	std::map<long, Element_Info*>       m_cbid_map;      //��բ��id�͵�բ��Ϣ��Ӧ��ϵ

	std::map<long, long>    			m_shared_point;  //����ĳһ��͹����Ķ�Ӧ��ϵ
	std::set<long>				 		m_visit_daoza;   //�����ѷ��ʹ��ĵ�բid��Ϣ
	long  						        m_dst_point;     //����find_switch_between�����е�p2���������
	bool								m_is_find_path;  //���find_switch_between���
	std::deque<long>				    m_shortest_path; //���ж���·��ʱ��������̵�����·��
	bool								is_multi_path;   //����Ƿ��Ƕ�·������
	std::set<long>          			m_ignore_id;     //����������բ���豸id
	std::multimap<long, long>		    m_skip_org;		 //����ԭ ʼ�ڵ������ڵ���Ϣ
	std::set<long>                      m_visit_ind;     //�����ѷ��ʹ���ind
	std::set<long>						m_visit_jnd;	 //�����ѷ��ʹ���jnd
};
#endif  //end SWITCH_MANAGE_H_

