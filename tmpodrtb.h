/*
 * Created on: 2013-1-9
 *      Author: BetaKDE
 */
#if !defined(__TMPODRTB_H)
#define	__TMPODRTB_H

#include<assert.h>
#include<iostream>
#include<map>
#include<vector>
#include<string.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  class ChildPtr
class ChildPtr
{
public:

	int			Ptr;
	ChildPtr*	Next;

public:

	//constructors
	ChildPtr()
	{
		Ptr = -1;
		Next = 0;
	}

	ChildPtr(int m_ptr)
	{
		Ptr = m_ptr;
		Next = 0;
	}

	//assign value operator
	const ChildPtr& operator = (const ChildPtr& m_ChildPtr)
	{
		Ptr = m_ChildPtr.Ptr;
		Next = m_ChildPtr.Next;

		return *this;
	}

	//free child pointer list
	friend void FreeChildPtr(ChildPtr* m_ChildPtr)
	{
		ChildPtr *head, *temp;

		head = m_ChildPtr;
		while(head)
		{
			temp = head->Next;
			delete head;
			head = temp;
		}
	}

	//free child pointer list
	friend void FreeChildPtr1(ChildPtr* m_ChildPtr)
	{
		FreeChildPtr(m_ChildPtr);
	}
};

//仅为了兼容旧版本的代码
typedef ChildPtr ChildPtr1;

template<typename T>
class KEY
{
	protected:
	T*     data;

	public:
	KEY(T* t)
	{
		data = t;
	}

	T* GetData() const{return data;}

	int operator<(const KEY key) const
	{
		return *data < *key.data;
	}

	int operator==(const KEY key) const
	{
		return *data == *key.data;
	}

	const KEY& operator = (const KEY& key)
	{
		data = key.data;
		return *this;
	}
};

////////////////////////////////////////////////////////////////////////////////
//  class DTable
template <class T> class DTable
{
public:

	T*		Data;		//record data
	int*	Ph;			//physical position corresponding to logic position
	int*	Log;		//logic position corresponding to physical position
	int		Size;		//table size
	int		Count;		//record count
	int		Sequent;	//indicate if in sequent storage

	typedef map<unsigned int, T*> _MAP_INDEX;
	typedef map<long, vector<T*> > _MAP_R_INDEX;
	typedef typename _MAP_R_INDEX::iterator _MAP_R_INDEX_ITER;
	typedef typename _MAP_R_INDEX::const_iterator _MAP_R_INDEX_CONST_ITER;
	typedef map<long, long> _MAP_R_CHILD;
	typedef typename _MAP_R_CHILD::const_iterator _MAP_R_CHILD_CONST_ITER;
	typedef typename _MAP_R_CHILD::iterator _MAP_R_CHILD_ITER;
	typedef typename map<KEY<T> , int>::iterator _MAP_CH_ITER;

	_MAP_INDEX map_index;
	_MAP_R_INDEX map_r_index;
	_MAP_R_CHILD map_r_child;
	map<KEY<T> , int>	map_ch;

public:

	//constructor and destructor
	DTable(int m_size);
	~DTable();

	//get tale size
	int GetSize() const
	{
		return Size;
	}

	//get record count
	int GetCount() const
	{
		return Count;
	}

	//get next physical position
	int GetNext() const
	{
		return Count;
	}

	//check if sequent
	int IsSequent() const
	{
		return 1;
	}

	//get table pointer
	T* GetTable()
	{
		return Data;
	}

	//get physical position by logic one
	int GetPhByLog(int log) const
	{
		return Ph[log];
	}

	//get logic position by physical one
	int	GetLogByPh(int ph) const
	{
		return Log[ph];
	}

	//get data by logic position
	T* GetDataByLog(int log)
	{
		return &Data[Ph[log]];
	}

	T* GetDataByPos(int pos)
	{
		return &Data[pos];
	}

	//get data by physical position
	T* GetDataByPh(int ph)
	{
		return &Data[ph];
	}

	//copy data in table
	void CopyTable(T* buffer) const;

	//copy data by logic position
	void CopyDataByLog(T* buffer, int log) const
	{
		*buffer = Data[Ph[log]];
	}

	//copy data by physical position
	void CopyDataByPh(T* buffer, int ph) const
	{
		*buffer = Data[ph];
	}

	//update data in table
	void UpdateTable(T* buffer);

	//update data by logic position
	void UpdateDataByLog(T* buffer, int log)
	{
		Data[Ph[log]] = *buffer;
	}

	//update data by physical position
	void UpdateDataByPh(T* buffer, int ph)
	{
		Data[ph] = *buffer;
	}

	//find data
	T* Find( const T& t );

	//find physical position
	int	FindPh( const T& t );

	//find logical position
	int FindLog( const T& t );

	int FindPos( const T& t );

	//insert data
	int	Insert( const T& t );

	//insert a number of data
	int Insert(T* buffer, int num);

	//remove data
//	int	Remove( const T& t );

	//empty table
	int	Empty();

	//sequence table
	int	Sequence(bool b_move_data = true);

	//build chain for sequent data
	int BuildChain();

public:

	//methods designed for relation table

	//！！！！注意FindParentPtr函数只适用于一对多，不适用于多对多(比如r_nd_cb表)20130109荣义平
	//find parent pointer by child id
	int	FindParentPtr(const long c_id) const;

	//find parent pointer by relation pointer value
	int	FindParentPtr(int r_ptr) const;

	//find child pointer list by parent id
	ChildPtr* FindChildPtr(const long p_id) const;

	//仅为了兼容旧版本的代码
	ChildPtr1* FindChildPtr1(const long p_id) const
	{
		return FindChildPtr(p_id);
	}

	//find child pointer list by relation pointer value
	ChildPtr* FindChildPtr(int r_ptr) const;

	//仅为了兼容旧版本的代码
	ChildPtr1* FindChildPtr1(int r_ptr) const
	{
		return FindChildPtr(r_ptr);
	}

	//find child number by parent id
	int	FindChildNum(const long p_id) const;

	//find child number by relation pointer value
	int FindChildNum(int r_ptr) const;

	static unsigned int calc_hashnr(const unsigned char *key, unsigned int length)
	{
		register unsigned int nr = 1, nr2 = 4;

		while (length--)
		{
			nr ^= (((nr & 63) + nr2) * ((unsigned int) (unsigned char) *key++)) + (nr << 8);
			nr2 += 3;
		}

		return ((unsigned int) nr);
	}

	void UpdateLogPh(T* t, int Log_value)
	{
		assert(t != NULL);

		char key[100];
		t->GetKey(key);

		unsigned int index = calc_hashnr((unsigned char *) key, strlen(key));

		typename _MAP_INDEX::const_iterator it = map_index.find(index);

		if (it != map_index.end())
		{
			int pos = it->second - Data;
			Log[pos] = Log_value;
			Ph[Log_value] = pos;
		}
		else
		{
			printf("%s:!!!!!!!!!!!!!!!!!!!!!!!!找不到%s\r\n", __FUNCTION__, key);
		}
	}

	void Quicksort(T* r, int s, int t)
	{
		int i = s, j = t;
		T tmp;
		if (s < t)
		{
			tmp = r[s];
			int tmpLog = Log[s];
			
			while (i != j)
			{
				while (j > i && tmp < r[j])
					j--;

				if(j > i)
				{
					r[i] = r[j];					
//					UpdateLogPh(&r[j], i);
				}					

				while (i < j && r[i] < tmp)
					i++;

				if(j > i)
				{
					r[j] = r[i];					
//					UpdateLogPh(&r[i], j);
				}					
			}

			r[i] = tmp;			
//			UpdateLogPh(&tmp, i);

			Quicksort(r, s, i - 1);
			Quicksort(r, i + 1, t);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
//  constructor
template <class T> DTable<T>::DTable(int m_size)
{
	Data = new T[m_size];
	Ph = new int[m_size];
	Log = new int[m_size];
	Size = m_size;
	Count = 0;
	Sequent = 0;

	memset(Ph, -1, m_size * sizeof(int));
	memset(Log, -1, m_size * sizeof(int));
}

///////////////////////////////////////////////////////////////////////////////
// destructor
template <class T> DTable<T>::~DTable()
{
	delete[] Data;
	delete[] Ph;
	delete[] Log;
}

///////////////////////////////////////////////////////////////////////////////
//  CopyTable()
//  Attention:	This function will not check the size of buffer.
template <class T> void DTable<T>::CopyTable( T* buffer ) const
{
	for (int i = 0; i < Count; i++)
	{
		buffer[i] = Data[i];
	}
//	memcpy(buffer, Data, Count * sizeof(T));
}

///////////////////////////////////////////////////////////////////////////////
//  UpdateTable()
//  Attention:	This function will not check the size of buffer.
template <class T> void DTable<T>::UpdateTable( T* buffer )
{
	for (int i = 0; i < Count; i++)
	{
		Data[i] = buffer[i];
	}
//	memcpy(Data, buffer, Count * sizeof(T));
}

///////////////////////////////////////////////////////////////////////////////
//  Find()
template <class T> T* DTable<T>::Find( const T& t )
{
	char key[100];
	t.GetKey(key);

	unsigned int index = calc_hashnr((unsigned char *) key, strlen(key));

	typename map<unsigned int, T*>::iterator it = map_index.find(index);

	if (it == map_index.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  FindPh()
template <class T> int DTable<T>::FindPh( const T& t )
{
	int index = FindLog(t);

	if(index == -1) return -1;

	return Ph[index];
}

///////////////////////////////////////////////////////////////////////////////
//  FindLog()
template <class T> int DTable<T>::FindLog( const T& t )
{
	char key[100];
	t.GetKey(key);

	unsigned int index = calc_hashnr((unsigned char *) key, strlen(key));

	typename map<unsigned int, T*>::iterator it = map_index.find(index);

	if (it == map_index.end())
	{
		return -1;
	}
	else
	{
		return Log[it->second - Data];
	}
}

template<class T> int DTable<T>::FindPos(const T& t)
{
	char key[100];
	t.GetKey(key);

	unsigned int index = calc_hashnr((unsigned char *) key,	strlen(key));

	typename map<unsigned int, T*>::iterator it = map_index.find(index);

	if (it == map_index.end())
	{
		return -1;
	}
	else
	{
		return it->second - Data;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  Insert()
//  Insert a record into the table by ascending order.
template <class T> int DTable<T>::Insert( const T& t )
{
	if (Count == Size)
	{
		return -1;
	}

	Sequent = 0;

	Data[Count] = t;

	char key[100];

	t.GetKey(key);

	//建立索引
	map_index[calc_hashnr((unsigned char *) key, strlen(key))] = &Data[Count];
	//对数据进行排序（不移动数据在数组中的位置，仅仅对指针进行排序）
	KEY<T> tmp = KEY<T>(&Data[Count]);
	if (map_ch.find(tmp) != map_ch.end())
	{
		cout << "\033[31m出现致命错误，map_ch数据重复,key:" << key << "\033[0m" << endl;
		exit(0);
	}
	map_ch[tmp] = Count;
	Log[Count] = Count;
	Ph[Count] = Count;

	//建立关系表父子索引
	if (t.IsRTable())
	{
		map_r_child[t.GetChildId()] = t.GetParentId();
		map_r_index[t.GetParentId()].push_back(&Data[Count]);
	}

	return Count++;
}

///////////////////////////////////////////////////////////////////////////////
//  Insert()
template <class T> int DTable<T>::Insert( T* buffer, int num )
{
	int i;

	for (i = 0; i < num; i++)
	{
		if (Insert(buffer[i]) == -1)
		{
			break;
		}
	}

	return i;
}

///////////////////////////////////////////////////////////////////////////////
//  Empty()
//  Remove all records in table.
template <class T> int DTable<T>::Empty()
{
	bzero(Data, sizeof(T) * Size);
	Count = 0;
	Sequent = 0;
	map_ch.clear();
	map_index.clear();
	map_r_child.clear();
	map_r_index.clear();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//  Sequence()
//  Sort the records in table by sequence order. After executing
//  this method, the logic positions and physical positions of
//  all records in the table are the same.
template <class T> int DTable<T>::Sequence(bool b_move_data)
{
	if(Sequent == 1) return 1;

	if(b_move_data)
	{
		//快速排序
//		Quicksort(Data, 0, Count - 1);			//快速排序内临时变量对应的数据结构太大时，递归可能超出堆栈大小限制

		T* buf = new T[Count];
		typename map<KEY<T>, int>::iterator it = map_ch.begin();

		for (int pos = 0; it != map_ch.end(); it++, pos++)
		{
			buf[pos] = *it->first.GetData();
		}

		for(int i = 0; i < Count; i++)
		{
			Data[i] = buf[i];
		}

		delete []buf;

		Sequent = 1;
		//重建索引
		map_index.clear();
		map_r_index.clear();
		map_r_child.clear();

		for (int i = 0; i < Count; i++)
		{
			Ph[i] = i;
			Log[i] = i;

			char key[100];

			Data[i].GetKey(key);

			map_index[calc_hashnr((unsigned char *) key, strlen(key))] = &Data[i];

			if (Data[i].IsRTable())
			{
				map_r_child[Data[i].GetChildId()] = Data[i].GetParentId();
				map_r_index[Data[i].GetParentId()].push_back(&Data[i]);
			}
		}
	}
	//仅仅只是建立Ph,Log，不移动数据
	else
	{
		typename map<KEY<T>, int>::iterator it = map_ch.begin();

		for (int pos = 0; it != map_ch.end(); it++, pos++)
		{
//			char key[100];
//			it->first.GetData()->GetKey(key);
//
//			cout << key << endl;
			Ph[pos] = it->second;
			Log[it->second] = pos;
		}
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
//  FindParentPtr()
template <class T> 	int DTable<T>::FindParentPtr(const long c_id) const
{
	assert(c_id > 0);

	_MAP_R_CHILD_CONST_ITER it = map_r_child.find(c_id);

	if(map_r_child.end() == it) return -1;

	_MAP_R_INDEX_CONST_ITER it_r_index = map_r_index.find(it->second);

	if(it_r_index == map_r_index.end()) return -1;

	const T* p = it_r_index->second.front();

	return p->GetParentPtr();
}


////////////////////////////////////////////////////////////////////////////////
//  FindParentPtr()
template <class T> 	int DTable<T>::FindParentPtr(int r_ptr) const
{
	if(r_ptr < 0 || r_ptr >= Count) return -1;

	if(r_ptr != -1)
	{
		return Data[r_ptr].GetParentPtr();
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
//  FindChildPtr()
template <class T> 	ChildPtr* DTable<T>::FindChildPtr(const long p_id) const
{
	assert(p_id > 0);

	ChildPtr	*head, *tail, *temp;

	head = 0;
	tail = 0;
	temp = 0;

	_MAP_R_INDEX_CONST_ITER it = map_r_index.find(p_id);

	if(it == map_r_index.end()) return NULL;

	typename vector<T*>::const_iterator tmp_it;

	for (tmp_it = it->second.begin(); tmp_it != it->second.end(); tmp_it++)
	{
		temp = new ChildPtr((*tmp_it)->GetChildPtr());
		if (!head)
		{
			head = temp;
			tail = temp;
		}
		else
		{
			tail->Next = temp;
			tail = temp;
		}
	}

	return head;
}

///////////////////////////////////////////////////////////////////////////////
//  FindChildPtr()
template <class T> 	ChildPtr* DTable<T>::FindChildPtr(int r_ptr) const
{
	if(r_ptr < 0 || r_ptr >= Count) return NULL;

	return FindChildPtr(Data[r_ptr].GetParentId());
}

///////////////////////////////////////////////////////////////////////////////
//  FindChildNum()
template <class T> 	int DTable<T>::FindChildNum(const long p_id) const
{
	assert(p_id > 0);

	_MAP_R_INDEX_CONST_ITER it = map_r_index.find(p_id);

	if (it == map_r_index.end())
		return 0;

	typename vector<T*>::const_iterator tmp_it;

	return it->second.size();
}

///////////////////////////////////////////////////////////////////////////////
//  FindChildNum()
template <class T> 	int DTable<T>::FindChildNum(int r_ptr) const
{
	if(r_ptr < 0 || r_ptr >= Count) return -1;

	return FindChildNum(Data[r_ptr].GetParentId());
}

#endif




