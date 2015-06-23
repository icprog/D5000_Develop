/*
 * Created on: 2013-1-16
 *      Author: BetaKDE
 */
#if !defined(__TMPTABLE_H)
#define	__TMPTABLE_H
#include"tmpodrtb.h"

////////////////////////////////////////////////////////////////////////////////
//  class TTable
template <class T> class TTable:public DTable<T>
{
public:
	TTable(int size);
	~TTable();
};

///////////////////////////////////////////////////////////////////////////////
//  constructor
template <class T> TTable<T>::TTable(int m_size)
		:DTable<T>(m_size)
{
}
	
///////////////////////////////////////////////////////////////////////////////
// destructor	
template <class T> TTable<T>::~TTable()
{
}

#endif
