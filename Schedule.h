#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "MTL/mtlList.h"
#include "MTL/mtlPointer.h"

#include "Object.h"

class Schedule
{
public:
	struct Item
	{
		float m_wait_seconds;
		virtual void operator()(Object *object) = 0;
	};
private:
	NewTimer					m_timer;
	mtlList< mtlShared<Item> >	m_items;
	mtlItem< mtlShared<Item> >	*m_current_function;
	bool						m_loop;
public:
	Schedule( void );
	template < typename type_t >
	void AddItem(float wait_seconds);
	void Execute(Object *object);
	void StartTimer( void );
	void StopTimer( void );
	void RestartTimer( void );
	// a list of things to do in order
	// set up a list of things to do at given times
	// call execute at every update - Schedule will call a function (or more) if the time is up, will do nothing if time is not up
};

template < typename type_t >
void Schedule::AddItem(float wait_seconds)
{
	mtlShared<Item> item = mtlShared<type_t>::Create();
	item->m_wait_seconds = wait_seconds;
	m_items.AddLast(item);
	if (m_current_function == NULL) {
		m_current_function = m_items.GetFirst();
	}
}

#endif // SCHEDULE_H
