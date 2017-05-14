#include "Platform.h"
#include "Timer.h"
#include "MiniLib/MML/mmlMath.h"

#define TICKS_PER_SEC   1000
#define TICKS_PER_SEC_F 1000.0f

void Timer::UpdateTimer( void ) const
{
	if (m_ticking) {
		Time time = GetProgramTimeMS();
		m_acc_time += time - m_time_last;
		m_time_last = time;
	}
}

Timer::Time Timer::GetProgramTimeMS( void )
{
	return (Time)SDL_GetTicks();
}

Timer::Timer( void ) : m_bps(0.0f), m_beat_interval(0), m_acc_time(0), m_time_last(0), m_ticking(false)
{
	SetTempo(1.0f, BeatsPerSecond);
}

Timer::Timer(float tempo, Timer::Units units): m_bps(0.0), m_beat_interval(0), m_acc_time(0), m_time_last(0), m_ticking(false)
{
	SetTempo(tempo, units);
}

void Timer::SetTempo(float tempo, Timer::Units units)
{
	tempo = mmlMax(tempo, 0.0f);
	if (units == BeatsPerSecond) {
		m_bps = tempo;
	} else if (units == SecondsPerBeat) {
		m_bps = tempo > 0.0f ? 1.0f / tempo : 0.0f;
	} else {
		m_bps = 0.0f;
	}
	m_beat_interval = (Time)(TICKS_PER_SEC_F / m_bps);
}

float Timer::GetTempo(Timer::Units units) const
{
	if (units == BeatsPerSecond) {
		return m_bps;
	} else if (units == SecondsPerBeat) {
		return m_bps > 0.0f ? 1.0f / m_bps : 0.0f;
	}
	return 0.0f;
}

void Timer::Start( void )
{
	if (!IsTicking()) {
		m_time_last = GetProgramTimeMS();
		m_ticking = true;
	}
}

void Timer::Stop( void )
{
	if (IsTicking()) {
		UpdateTimer();
		m_ticking = false;
	}
}

void Timer::Toggle( void )
{
	if (IsTicking()) {
		Stop();
	} else {
		Start();
	}
}

void Timer::Reset( void )
{
	m_acc_time = 0;
}


void Timer::ResetPartBeat( void )
{
	m_acc_time = m_acc_time - m_acc_time % m_beat_interval;
}

void Timer::ResetBeats( void )
{
	m_acc_time = m_acc_time % m_beat_interval;
}

void Timer::DecrementBeat( void )
{
	if (m_acc_time >= m_beat_interval) {
		m_acc_time -= m_beat_interval;
	}
}

bool Timer::IsTicking( void ) const
{
	return m_ticking;
}

bool Timer::IsStopped( void ) const
{
	return !m_ticking;
}

bool Timer::IsDue( void ) const
{
	return GetTime() >= 1.0f;
}

float Timer::GetTime( void ) const
{
	UpdateTimer();
	return (float)m_acc_time / (float)m_beat_interval;
}

int Timer::GetBeats( void ) const
{
	return (Time)GetTime();
}

float Timer::GetPartBeat( void ) const
{
	float beats = GetTime();
	return beats - (Time)beats;
}
