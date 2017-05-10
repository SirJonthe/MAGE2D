#ifndef TIMER_H
#define TIMER_H

/*class Timer
{
private:
	float m_timeLast;
	float m_timeNow;
	float m_interval;
	float m_delta;

public:
	explicit     Timer(float intervalsPerSecond = 0.0f);
	void         SetInterval(float fractionOfSecond);
	void         SetIntervalsPerSecond(float intevalsPerSecond);
	float        GetInterval( void ) const;
	float        GetIntervalsPerSecond( void ) const;
	void         Start( void );
	void         Stop( void );
	void         Restart( void );
	bool         IsStopped( void ) const;
	bool         IsTicking( void ) const;
	float        GetTimeDeltaTick( void ) const;
	float        GetTimeDeltaNow( void ) const;
	float        GetTimeDeltaSecondsTick( void ) const;
	float        GetTimeDeltaSecondsNow( void ) const;
	void         Tick( void );
	static float GetProgramTimeSeconds( void );
	float        GetProgramTimeInterval( void );
};*/

class Timer
{
public:
	enum Units
	{
		BeatsPerSecond,
		BeatsPerMinute,
		FractionOfSecond
	};

	typedef long long Time;

private:
	float         m_beats_per_second;
	mutable float m_accumulated_time; // this MUST NOT be a float
	mutable float m_time_last;
	bool          m_ticking;

private:
	void         UpdateTimer( void ) const;
	float        GetStaticTime(float time_seconds) const;
	static float GetStaticTime(float time_seconds, float tempo, Units units);
	static float GetBeatsPerSecond(float tempo, Units units);

public:
	explicit     Timer(float beats_per_second = 1.0f, Units units = BeatsPerSecond);

	void         SetTempo(float tempo, Units units = BeatsPerSecond);
	float        GetTempo(Units units = BeatsPerSecond) const;

	void         Start( void ); // resumes
	void         Stop( void );
	void         Toggle( void ); // toggle between start and stop
	void         Reset( void ); // only resets, no resume unless already ticking
	void         Truncate( void ); // same as reset, only keeps the fractions
	void         Beat( void ); // same as Truncate, only removes one whole number, not all whole numbers

	bool         IsTicking( void ) const;
	bool         IsStopped( void ) const;
	bool         IsDue( void ) const;

	float        GetTime( void ) const; // if running, get time now; if stopped, get delta
	void         SetTime(float time);
	int          GetBeats( void ) const;

	static float GetTime(float time_sec, float tempo, Units units = BeatsPerSecond);
	static int   GetBeats(float time_sec, float tempo, Units units = BeatsPerSecond);

	static float GetProgramTimeSeconds( void );
	static float GetProgramTime(float tempo, Units units = BeatsPerSecond);
	float        GetProgramTime( void ) const;
};

#endif // TIMER_H
