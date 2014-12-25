#ifndef TIMER_H
#define TIMER_H

#include "MTL/mtlList.h"
#include "MTL/mtlPointer.h"

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
};

class NewTimer
{
private:
	float			m_beats_per_second;
	mutable float	m_accumulated_time;
	mutable float	m_time_last;
	bool			m_ticking;

private:
	void UpdateTimer( void ) const;
	float GetStaticTime(float time_seconds) const;

public:
	explicit NewTimer(float beats_per_second=1.0f);

	void			SetBeatInterval(float frac_of_second);
	float			GetBeatInterval( void ) const;

	void			SetBeatsPerSecond(float beats_per_second);
	float			GetBeatsPerSecond( void ) const;

	void			Start( void ); // resumes
	void			Stop( void );
	void			Toggle( void ); // toggle between start and stop
	void			Reset( void ); // only resets, no resume unless already ticking
	void			Truncate( void ); // same as reset, only keeps the fractions
	void			Beat( void ); // same as Truncate, only removes one whole number, not all whole numbers

	bool			IsTicking( void ) const;
	bool			IsStopped( void ) const;
	bool			IsDue( void ) const;

	float			GetTime( void ) const; // if running get time now, if stopped get delta

	static float	GetProgramTimeSeconds( void );
	float			GetProgramTime( void ) const;
};

#endif // TIMER_H
