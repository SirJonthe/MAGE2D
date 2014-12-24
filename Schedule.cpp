#include "Schedule.h"

Schedule::Schedule( void ) : m_current_function(NULL), m_loop(true)
{}

void Schedule::Execute(Object *object)
{
	while (m_timer.GetTime() >= 1.0f) {

	}
}

void Schedule::StartTimer( void )
{
	m_timer.Start();
}

void Schedule::StopTimer( void )
{
	m_timer.Stop();
}

void Schedule::RestartTimer( void )
{
	m_timer.Reset();
	m_current_function = m_items.GetFirst();
}
