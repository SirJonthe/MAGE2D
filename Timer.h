#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	// @data Units
	// @info Constants representing the units that a number represents.
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

	// @algo SetTempo
	// @info Sets the tempo of the timer.
	// @in
	//   tempo -> The absolute tempo.
	//   units -> The units the tempo represents.
	void  SetTempo(float tempo, Units units);
	
	// @algo GetTempo
	// @info Returns the tempo in the requested unit format.
	// @in The requested units to be returned.
	// @out The tempo in the specified units.
	float GetTempo(Units units) const;

	// @algo Start
	// @info Starts the timer.
	void Start( void );
	
	// @algo Stop
	// @info Stops the timer.
	void Stop( void );
	
	// @algo Toggle
	// @info Toggle starts/stops the timer.
	void Toggle( void );
	
	// @algo Reset
	// @info Resets the timer.
	void Reset( void );
	
	// @algo ResetPartBeat
	// @info Resets the fractional part of the beat.
	void ResetPartBeat( void );
	
	// @algo ResetBears
	// @info Resets the integer part of the beat.
	void ResetBeats( void );
	
	// @algo DecremetBeat
	// @info Decrements one integer from the current beat count.
	void DecrementBeat( void );

	// @algo IsTicking
	// @info Checks if timer is currently ticking.
	// @out TRUE if ticking.
	bool IsTicking( void ) const;
	
	// @algo IsStopped
	// @info Checks if timer is currently not ticking.
	// @out TRUE if not ticking.
	bool IsStopped( void ) const;
	
	// @algo IsDue
	// @out TRUE when current beat count > 1.
	bool IsDue( void ) const;

	// @algo GetFixedTime
	// @out Current time in a fixed-point format.
	Time  GetFixedTime( void ) const;
	
	// @algo GetTime
	// @out Current time in floating-point format.
	float GetTime( void ) const;
	
	// @algo GetBeats
	// @out Current number of beats as whole integers.
	int   GetBeats( void ) const;
	
	// @algo GetPartBeat
	// @out Current fractional part of beat.
	float GetPartBeat( void ) const;
};

#endif // TIMER_H
