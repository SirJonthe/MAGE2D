#ifndef TIMER_H
#define TIMER_H

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

#endif // TIMER_H
