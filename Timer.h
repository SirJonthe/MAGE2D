#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	enum Units
	{
		BeatsPerSecond,
		SecondsPerBeat
	};

private:
	typedef unsigned long long Time;

private:
	float        m_bps;
	Time         m_beat_interval;
	mutable Time m_acc_time;
	mutable Time m_time_last;
	bool         m_ticking;

private:
	void UpdateTimer( void ) const;
	static Time GetProgramTimeMS( void );

public:
	Timer( void );
	Timer(float tempo, Units units);

	void  SetTempo(float tempo, Units units);
	float GetTempo(Units units) const;

	void Start( void );
	void Stop( void );
	void Toggle( void );
	void Reset( void );
	void ResetPartBeat( void );
	void ResetBeats( void );
	void DecrementBeat( void );

	bool IsTicking( void ) const;
	bool IsStopped( void ) const;
	bool IsDue( void ) const;

	Time  GetFixedTime( void ) const;
	float GetTime( void ) const;
	int   GetBeats( void ) const;
	float GetPartBeat( void ) const;
};

#endif // TIMER_H
