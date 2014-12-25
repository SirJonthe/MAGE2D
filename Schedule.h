#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "MTL/mtlList.h"
#include "MTL/mtlPointer.h"

#include "Object.h"

class Schedule
{
public:
	struct Task
	{
		friend class Schedule;

	private:
		float m_delay_sec;

	public:
		float GetDelay( void ) const { return m_delay_sec; }
		virtual void operator()(Object *object) = 0;
	};
private:
	NewTimer					m_timer;
	mtlList< mtlShared<Task> >	m_tasks;
	mtlItem< mtlShared<Task> >	*m_current_task;
	bool						m_loop;
public:
	Schedule( void );

	template < typename task_t >
	void AddTask(float wait_seconds);

	bool IsComplete( void ) const;
	void Execute(Object *object);

	void StartTimer( void );
	void StopTimer( void );
	void RestartTimer( void );

	void LoopSchedule(bool loop);
	bool IsLooping( void ) const;

	bool IsTicking( void ) const;
	bool IsStopped( void ) const;
};

template < typename task_t >
void Schedule::AddTask(float delay_sec)
{
	mtlShared<Task> task;
	task.New<task_t>();
	task->m_delay_sec = delay_sec;
	m_tasks.AddLast(task);
	if (m_current_task == NULL) {
		m_current_task = m_tasks.GetFirst();
		m_timer.SetBeatInterval(task->GetDelay());
	}
}

#endif // SCHEDULE_H
