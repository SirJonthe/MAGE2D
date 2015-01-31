#include "Schedule.h"

bool Schedule::NullTask( void ) const
{
	return m_current_task == NULL;
}

Schedule::Task *Schedule::GetTask( void )
{
	return m_current_task->GetItem().GetShared();
}

Schedule::Schedule( void ) : m_current_task(NULL)
{}

void Schedule::Execute(Object *object)
{
	if (IsFinished()) { return; }
	while (!NullTask() && m_timer.IsDue()) {
		GetTask()->operator()(object);
		if (GetTask()->m_num_iter > Schedule::IterateForever) { // decrement counter if not infinite
			GetTask()->m_num_iter -= 1;
		}
		if (GetTask()->m_num_iter != 0) { // next item
			m_current_task = m_current_task->GetNext();
		} else { // no more iterations, remove item
			m_current_task = m_current_task->Remove();
		}
		if (NullTask()) { // if we reach end, we start at beginning
			m_current_task = m_tasks.GetFirst();
		}
		if (!NullTask()) {
			m_timer.Beat();
			m_timer.SetTempo(m_current_task->GetItem()->GetDelay(), NewTimer::FractionOfSecond);
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
	if (!NullTask()) {
		m_timer.SetTempo(m_current_task->GetItem()->GetDelay(), NewTimer::FractionOfSecond);
	}
}

bool Schedule::IsTicking( void ) const
{
	return m_timer.IsTicking();
}

bool Schedule::IsStopped( void ) const
{
	return m_timer.IsStopped();
}

bool Schedule::IsFinished( void ) const
{
	return m_tasks.GetSize() == 0;
}

void Schedule::ClearSchedule( void )
{
	m_tasks.RemoveAll();
	m_current_task = NULL;
	m_timer.SetTempo(1.0f, NewTimer::BeatsPerSecond);
	m_timer.Stop();
	m_timer.Reset();
}
