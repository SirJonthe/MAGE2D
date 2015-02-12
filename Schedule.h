#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "MTL/mtlList.h"
#include "MTL/mtlPointer.h"

#include "Object.h"

class ScheduleTask
{
	friend class Schedule;
	friend class SerialSchedule;
	friend class ParallelSchedule;

private:
<<<<<<< HEAD
	float		m_delay_sec;
	int			m_num_iter;
	NewTimer	m_timer;
=======
	float	m_delay_sec;
	int		m_num_iter;
	//NewTimer m_timer;
>>>>>>> e3f6a39163065f7eb8dc0ff7f22aeda405acfefe

public:
	float			GetDelay( void ) const { return m_delay_sec; }
	int				GetIterationsLeft( void ) const { return m_num_iter; }
<<<<<<< HEAD
	virtual bool	IsDue( void ) const; // overload this to have an arbitrary condition for execution
=======
	//virtual bool	IsDue( void ); // put any kind of execution condition here
>>>>>>> e3f6a39163065f7eb8dc0ff7f22aeda405acfefe
	virtual void	operator()(Object *object) = 0;
};

template < typename task_t >
inline mtlShared<ScheduleTask> CreateTask( void )
{
	return mtlShared<ScheduleTask>::Create<task_t>();
}

class Schedule
{
public:
	static const int IterateForever = -1;

protected:
	mtlList< mtlShared<ScheduleTask> >	m_tasks;

protected:
	void ExecuteTask(mtlItem< mtlShared< ScheduleTask > > *task, Object *object);
	void NextTask(mtlItem< mtlShared<ScheduleTask> > *&task);

public:
	virtual ~Schedule( void ) {}

	virtual void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, int iterations = Schedule::IterateForever) = 0;
	//virtual void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, float exist_seconds) = 0;

	virtual void Execute(Object *object) = 0;

	virtual void StartTimer( void ) = 0;
	virtual void StopTimer( void ) = 0;
	virtual void RestartTimer( void ) = 0;

	virtual bool IsTicking( void ) const = 0;
	virtual bool IsStopped( void ) const = 0;

	bool IsFinished( void ) const;

	virtual void ClearSchedule( void ) = 0;
};

class SerialSchedule : public Schedule
{
private:
	NewTimer							m_timer;
	mtlItem< mtlShared<ScheduleTask> >	*m_current_task;

private:
	bool			NullTask( void ) const;
	ScheduleTask	*GetTask( void );

public:
	SerialSchedule( void );

	void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, int iterations);
	//void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, float exist_seconds);

	void Execute(Object *object);

	void StartTimer( void );
	void StopTimer( void );
	void RestartTimer( void );

	bool IsTicking( void ) const;
	bool IsStopped( void ) const;

	void ClearSchedule( void );
};

class ParallelSchedule : public Schedule
{
private:
	mtlList<NewTimer>	m_timers;
	bool				m_ticking;

public:
	ParallelSchedule( void );

	void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, int iterations);
	//void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, float exist_seconds);

	void Execute(Object *object);

	void StartTimer( void );
	void StopTimer( void );
	void RestartTimer( void );

	bool IsTicking( void ) const;
	bool IsStopped( void ) const;

	void ClearSchedule( void );
};

#endif // SCHEDULE_H
