#include "Schedule.h"

void Schedule::ExecuteTask(mtlItem< mtlShared<ScheduleTask> > *task, Object *object)
{
	if (task != NULL) {
		ScheduleTask *t = task->GetItem().GetShared();
		t->operator()(object);
		if (t->m_num_iter > Schedule::IterateForever) {
			--t->m_num_iter;
		}
	}
}

void Schedule::NextTask(mtlItem< mtlShared<ScheduleTask> > *&task)
{
	if (task != NULL) {
		if (task->GetItem()->m_num_iter == 0) {
			task = task->Remove();
		} else {
			task = task->GetNext();
		}
	}
	if (task == NULL) {
		task = m_tasks.GetFirst();
	}
}

bool Schedule::IsFinished( void ) const
{
	return m_tasks.GetSize() == 0;
}

bool SerialSchedule::NullTask( void ) const
{
	return m_current_task == NULL;
}

ScheduleTask *SerialSchedule::GetTask( void )
{
	return m_current_task->GetItem().GetShared();
}

SerialSchedule::SerialSchedule( void ) : m_current_task(NULL)
{}

void SerialSchedule::AddTask(mtlShared<ScheduleTask> task, float delay_sec, int iterations)
{
	if (iterations == 0) { return; }
	task->m_delay_sec = delay_sec;
	task->m_num_iter = iterations;
	m_tasks.AddLast(task);
	if (m_current_task == NULL) {
		m_current_task = m_tasks.GetFirst();
		m_timer.SetTempo(task->GetDelay(), NewTimer::FractionOfSecond);
	}
}

/*void SerialSchedule::AddTask(mtlShared<ScheduleTask> task, float wait_seconds, float exist_seconds)
{
	int iterations = (int)(exist_seconds / wait_seconds);
	AddTask(task, wait_seconds, iterations);
}*/

void SerialSchedule::Execute(Object *object)
{
	if (IsFinished() || IsStopped()) { return; }
	while (!NullTask() && m_timer.IsDue()) {
		ExecuteTask(m_current_task, object);
		NextTask(m_current_task);
		if (!NullTask()) {
			m_timer.Beat();
			m_timer.SetTempo(m_current_task->GetItem()->GetDelay(), NewTimer::FractionOfSecond);
		}
	}
}

void SerialSchedule::StartTimer( void )
{
	m_timer.Start();
}

void SerialSchedule::StopTimer( void )
{
	m_timer.Stop();
}

void SerialSchedule::RestartTimer( void )
{
	m_timer.Reset();
	m_current_task = m_tasks.GetFirst();
	if (!NullTask()) {
		m_timer.SetTempo(m_current_task->GetItem()->GetDelay(), NewTimer::FractionOfSecond);
	}
}

bool SerialSchedule::IsTicking( void ) const
{
	return m_timer.IsTicking();
}

bool SerialSchedule::IsStopped( void ) const
{
	return m_timer.IsStopped();
}

void SerialSchedule::ClearSchedule( void )
{
	m_tasks.RemoveAll();
	m_current_task = NULL;
	m_timer.SetTempo(1.0f, NewTimer::BeatsPerSecond);
	m_timer.Stop();
	m_timer.Reset();
}
//---------------------------
ParallelSchedule::ParallelSchedule( void ) : m_ticking(false)
{}

void ParallelSchedule::AddTask(mtlShared<ScheduleTask> task, float delay_sec, int iterations)
{
	if (iterations == 0) { return; }
	task->m_delay_sec = delay_sec;
	task->m_num_iter = iterations;
	m_tasks.AddLast(task);
	NewTimer timer;
	timer.SetTempo(delay_sec, NewTimer::FractionOfSecond);
	if (m_ticking) {
		timer.Start();
	}
	m_timers.AddLast(timer);
}

/*void ParallelSchedule::AddTask(mtlShared<ScheduleTask> task, float wait_seconds, float exist_seconds)
{
	int iterations = (int)(exist_seconds / wait_seconds);
	AddTask(task, wait_seconds, iterations);
}*/
#include <iostream>
void ParallelSchedule::Execute(Object *object)
{
	if (IsFinished() || IsStopped()) { return; }
	mtlItem< mtlShared<ScheduleTask> > *task = m_tasks.GetFirst();
	mtlItem<NewTimer> *timer = m_timers.GetFirst();
	int a = 0;
	while (task != NULL && timer != NULL) {
		int beats = timer->GetItem().GetBeats();
		timer->GetItem().Truncate();
		bool is_inf = task->GetItem()->GetIterationsLeft() < 0;
		int i;
		for (i = 0; i < beats; ++i) {
			ExecuteTask(task, object);
			if (!is_inf && task->GetItem()->GetIterationsLeft() == 0) {
				break;
			}
		}
		if (i < beats) {
			task = task->Remove();
			timer = timer->Remove();
		} else {
			task = task->GetNext();
			timer = timer->GetNext();
		}
		++a;
	}
}

void ParallelSchedule::StartTimer( void )
{
	m_ticking = true;
	mtlItem<NewTimer> *timer = m_timers.GetFirst();
	while (timer != NULL) {
		timer->GetItem().Start();
		timer = timer->GetNext();
	}
}

void ParallelSchedule::StopTimer( void )
{
	m_ticking = false;
	mtlItem<NewTimer> *timer = m_timers.GetFirst();
	while (timer != NULL) {
		timer->GetItem().Stop();
		timer = timer->GetNext();
	}
}

void ParallelSchedule::RestartTimer( void )
{
	mtlItem<NewTimer> *timer = m_timers.GetFirst();
	while (timer != NULL) {
		timer->GetItem().Reset();
		timer = timer->GetNext();
	}
}

bool ParallelSchedule::IsTicking( void ) const
{
	return m_ticking;
}

bool ParallelSchedule::IsStopped( void ) const
{
	return !m_ticking;
}

void ParallelSchedule::ClearSchedule( void )
{
	m_tasks.RemoveAll();
	m_timers.RemoveAll();
	m_ticking = false;
}
