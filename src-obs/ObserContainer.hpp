#pragma once
#include <list>

/*
π€≤Ï’ﬂ
*/
template<class T>
class IObserver_Container
{
public:
	virtual ~IObserver_Container(){}

	virtual void InsertObserver(T * pObserver)
	{
		if (NULL == pObserver)
			return;
		m_lstObservers.remove(pObserver);
		m_lstObservers.push_back(pObserver);
	}
	virtual void RemoveObserver(T * pObserver)
	{
		if (NULL == pObserver)
			return;

		m_lstObservers.remove(pObserver);
	}
protected:
	std::list<T*>	m_lstObservers;
};
