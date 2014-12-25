#include "Schedule.h"

Schedule::Schedule( void ) : m_current_task(NULL), m_loop(true)
{}

bool Schedule::IsComplete( void ) const
{
	return m_current_task == NULL;
}

void Schedule::Execute(Object *object)
{
	while (!IsComplete() && m_timer.IsDue()) {
		m_current_task->GetItem()->operator()(object);
		m_current_task = m_current_task->GetNext();
		if (m_loop && IsComplete()) {
			m_current_task = m_tasks.GetFirst();
		}
		if (!IsComplete()) {
			m_timer.Beat();
			m_timer.SetBeatInterval(m_current_task->GetItem()->GetDelay());
		}
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
	m_current_task = m_tasks.GetFirst();
	if (!IsComplete()) {
		m_timer.SetBeatInterval(m_current_task->GetItem()->GetDelay());
	}
}

void Schedule::LoopSchedule(bool loop)
{
	m_loop = loop;
	if (IsComplete()) {
		RestartTimer();
	}
}

bool Schedule::IsLooping( void ) const
{
	return m_loop;
}

bool Schedule::IsTicking( void ) const
{
	return m_timer.IsTicking();
}

bool Schedule::IsStopped( void ) const
{
	return m_timer.IsStopped();
}
