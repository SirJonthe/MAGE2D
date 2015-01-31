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
		float	m_delay_sec;
		int		m_num_iter;

	public:
		float			GetDelay( void ) const { return m_delay_sec; }
		int				GetIterationsLeft( void ) const { return m_num_iter; }
		virtual void	operator()(Object *object) = 0;
	};

	static const int IterateForever = -1;

private:
	NewTimer					m_timer;
	mtlList< mtlShared<Task> >	m_tasks;
	mtlItem< mtlShared<Task> >	*m_current_task;

private:
	bool NullTask( void ) const;
	Task *GetTask( void );

public:
	Schedule( void );

	template < typename task_t >
	void AddTask(float wait_seconds, int iterations = Schedule::IterateForever);

	void Execute(Object *object);

	void StartTimer( void );
	void StopTimer( void );
	void RestartTimer( void );

	bool IsTicking( void ) const;
	bool IsStopped( void ) const;

	bool IsFinished( void ) const;

	void ClearSchedule( void );
};

template < typename task_t >
void Schedule::AddTask(float delay_sec, int iterations)
{
	if (iterations == 0) { return; }
	mtlShared<Task> task;
	task.New<task_t>();
	task->m_delay_sec = delay_sec;
	task->m_num_iter = iterations;
	m_tasks.AddLast(task);
	if (m_current_task == NULL) {
		m_current_task = m_tasks.GetFirst();
		m_timer.SetTempo(task->GetDelay());
	}
}

#endif // SCHEDULE_H
