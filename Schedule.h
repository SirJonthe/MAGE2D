#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "MiniLib/MTL/mtlList.h"
#include "MiniLib/MTL/mtlPointer.h"

#include "Object.h"

// @data ScheduleTask
// @info A task to be repeated in accordance with a given schedule.
class ScheduleTask
{
	friend class Schedule;
	friend class SerialSchedule;
	friend class ParallelSchedule;

private:
	float m_delay_sec;
	int   m_num_iter;
	Timer m_timer;

public:
	// @algo GetDelay
	// @out The delay in seconds between executions.
	float        GetDelay( void ) const { return m_delay_sec; }
	
	// @algo GetIterationsLeft
	// @out The number of times left to execute task before removing it.
	int          GetIterationsLeft( void ) const { return m_num_iter; }
	
	// @algo IsDue
	// @info Determines if the task is ready to be executed (virtual).
	// @out TRUE if task is ready to be executed.
	virtual bool IsDue( void ) const; // overload this to have an arbitrary condition for execution
	
	// @algo operator()
	// @info The main function to call when it is time (virtual).
	// @in The game object to operate on.
	virtual void operator()(Object *object) = 0;
};

// @algo CreateTask
// @info Creates a new task.
template < typename task_t >
inline mtlShared<ScheduleTask> CreateTask( void )
{
	return mtlShared<ScheduleTask>::Create<task_t>();
}

// @data Schedule
// @info A collection of tasks that are executed in accordance with their respective timers (abstract).
class Schedule
{
public:
	static const int IterateForever = -1;

protected:
	mtlList< mtlShared<ScheduleTask> > m_tasks;

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

// @data SerialSchedule
// @info A collection of tasks that execute in accordance with their internal timers. One one task can be executed at a time.
class SerialSchedule : public Schedule
{
private:
	Timer                               m_timer;
	mtlItem< mtlShared<ScheduleTask> > *m_current_task;

private:
	bool          NullTask( void ) const;
	ScheduleTask *GetTask( void );

public:
	SerialSchedule( void );

	// @algo AddTask
	// @info Adds a new task to the schedule.
	// @in
	//   task -> The task.
	//   wait_seconds -> The time between executions.
	//   iterations -> The number of times the task is executed before being removed from schedule.
	void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, int iterations);
	//void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, float exist_seconds);

	// @algo Execute
	// @info Executes the schedule on the object. Only one task is performed per iteration.
	void Execute(Object *object);

	// @algo StartTimer
	// @info Starts task timer.
	void StartTimer( void );
	
	// @algo StopTimer
	// @info Stops task timer.
	void StopTimer( void );
	
	// @algo RestartTimer
	// @info Restarts task timer.
	void RestartTimer( void );

	// @algo IsTicking
	// @out TRUE if task timer is running.
	bool IsTicking( void ) const;
	
	// @algo IsStopped
	// @out TRUE if task timer is not running.
	bool IsStopped( void ) const;

	// @algo ClearSchedule
	// @info Removes all tasks from the schedule.
	void ClearSchedule( void );
};

// @data ParallelSchedule
// @info A collection of tasks that execute in accordance with their internal timers. Several tasks can be executed in sequence.
class ParallelSchedule : public Schedule
{
private:
	mtlList<Timer> m_timers;
	bool           m_ticking;

public:
	ParallelSchedule( void );

	// @algo AddTask
	// @info Adds a new task to the schedule.
	// @in
	//   task -> The task.
	//   wait_seconds -> The time between executions.
	//   iterations -> The number of times the task is executed before being removed from schedule.
	void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, int iterations);
	//void AddTask(mtlShared<ScheduleTask> task, float wait_seconds, float exist_seconds);

	// @algo Execute
	// @info Executes the schedule on the object. Several tasks may be executed at every call to this function.
	void Execute(Object *object);

	// @algo StartTimer
	// @info Starts task timer.
	void StartTimer( void );
	
	// @algo StopTimer
	// @info Stops task timer.
	void StopTimer( void );
	
	// @algo RestartTimer
	// @info Restarts task timer.
	void RestartTimer( void );

	// @algo IsTicking
	// @out TRUE if task timer is running.
	bool IsTicking( void ) const;
	
	// @algo IsStopped
	// @out TRUE if task timer is not running.
	bool IsStopped( void ) const;

	// @algo ClearSchedule
	// @info Removes all tasks from the schedule.
	void ClearSchedule( void );
};

#endif // SCHEDULE_H
