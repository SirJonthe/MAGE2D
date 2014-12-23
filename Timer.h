#ifndef TIMER_H
#define TIMER_H

#include "MTL/mtlList.h"

class Timer
{
private:
	float m_timeLast;
	float m_timeNow;
	float m_interval;
	float m_delta;

public:
	explicit		Timer(float intervalsPerSecond = 0.0f);
	void			SetInterval(float fractionOfSecond);
	void			SetIntervalsPerSecond(float intevalsPerSecond);
	float			GetInterval( void ) const;
	float			GetIntervalsPerSecond( void ) const;
	void			Start( void );
	void			Stop( void );
	void			Restart( void );
	bool			IsStopped( void ) const;
	bool			IsTicking( void ) const;
	float			GetTimeDeltaTick( void ) const;
	float			GetTimeDeltaNow( void ) const;
	float			GetTimeDeltaSecondsTick( void ) const;
	float			GetTimeDeltaSecondsNow( void ) const;
	void			Tick( void );
	static float	GetProgramTimeSeconds( void );
	float			GetProgramTimeInterval( void );

	/*void New_Mark( void ) { Tick(); }
	float New_GetLastMark( void ) { return GetTimeDelta(); }
	float New_GetLap( void ) { return GetTimeDeltaNow(); }
	void New_Lap( void ); // starts the timer over, saving the fractions
	void New_Reset( void ); // starts the timer over, not saving fractions
	void New_Pause( void ); // pauses a timer
	void New_Resume( void ); // starts where it was last stopped*/
};

class NewTimer
{
private:
	float	m_intervals_per_second;
	float	m_accumulated_time;
	float	m_time_last;
	bool	m_ticking;

private:
	void UpdateTimer( void );

public:
	explicit NewTimer(float intervals_per_second=1.0f);

	void			SetInterval(float frac_of_second);
	float			GetInterval( void ) const;

	void			SetIntervalsPerSecond(float intervals_per_second);
	float			GetIntervalsPerSecond( void ) const;

	void			Start( void ); // resumes
	void			Stop( void );
	void			Toggle( void ); // toggle between start and stop
	void			Reset( void ); // only resets, no resume unless already ticking
	void			Truncate( void ); // same as reset, only keeps the fractions

	bool			IsTicking( void ) const;
	bool			IsStopped( void ) const;

	float			GetTime( void ); // if running get time now, if stopped get delta

	static float	GetProgramTimeSeconds( void );
	float			GetProgramTime( void );
};

/*class Schedule
{
private:
	struct Item
	{
	private:
		float m_wait_seconds;
	public:
		virtual void operator()( void ) = 0;
	};
private:
	Timer m_timer;
	mtlList<Schedule::Item> m_items;
	mtlItem<Schedule::Item> *m_current;
	bool m_loop;
public:
	void Execute( void );
	// a list of things to do in order
	// set up a list of things to do at given times
	// call execute at every update - Schedule will call a function (or more) if the time is up, will do nothing if time is not up
};*/

#endif // TIMER_H
