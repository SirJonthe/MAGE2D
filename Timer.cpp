#include "Platform.h"
#include "Timer.h"

Timer::Timer(float intervalsPerSecond) : m_timeLast(-1.0f), m_timeNow(-1.0f), m_interval(0.0f), m_delta(0.0f)
{
	SetIntervalsPerSecond(intervalsPerSecond);
}

void Timer::SetInterval(float fractionOfSecond)
{
	if (fractionOfSecond <= 0.0f) {
		m_interval = 0.0f;
	} else {
		m_interval = fractionOfSecond;
	}
	if (!IsStopped()) {
		Restart();
	} else {
		Stop();
	}
}

void Timer::SetIntervalsPerSecond(float intervalsPerSecond)
{
	if (intervalsPerSecond <= 0.0f) {
		m_interval = 0.0f;
	} else {
		m_interval = 1.0f / intervalsPerSecond;
	}
	if (!IsStopped()) {
		Restart();
	} else {
		Stop();
	}
}

float Timer::GetInterval( void ) const
{
	return m_interval;
}

float Timer::GetIntervalsPerSecond( void ) const
{
	if (m_interval <= 0.0f) {
		return 0.0f;
	}
	return 1.0f / m_interval;
}

void Timer::Start( void )
{
	if (IsStopped() && m_interval > 0.0f) {
		m_timeNow = GetProgramTimeSeconds();
		m_timeLast = m_timeNow;
	}
}

void Timer::Stop( void )
{
	m_timeLast = -1.0f;
	m_timeNow = -1.0f;
	m_delta = 0.0f;
}

void Timer::Restart( void )
{
	if (m_interval > 0.0f) {
		m_timeNow = GetProgramTimeSeconds();
		m_timeLast = m_timeNow;
		m_delta = 0.0f;
	}
}

bool Timer::IsStopped( void ) const
{
	return m_timeLast < 0.0f;
}

bool Timer::IsTicking( void ) const
{
	return !IsStopped();
}

float Timer::GetTimeDeltaTick( void ) const
{
	return m_delta;
}

float Timer::GetTimeDeltaNow( void ) const
{
	if (IsTicking() && m_interval > 0.0f) {
		float now = GetProgramTimeSeconds();
		return (now - m_timeNow) / m_interval;
	}
	return 0.0f;
}

float Timer::GetTimeDeltaSecondsTick( void ) const
{
	return m_delta * m_interval;
}

float Timer::GetTimeDeltaSecondsNow( void ) const
{
	return GetTimeDeltaNow() * m_interval;
}

void Timer::Tick( void )
{
	if (!IsStopped()) {
		m_timeLast = m_timeNow;
		m_timeNow = GetProgramTimeSeconds();
		m_delta = (m_timeNow - m_timeLast) / m_interval;
	}
}

float Timer::GetProgramTimeSeconds( void )
{
	return float(SDL_GetTicks()) / 1000.0f;
}

float Timer::GetProgramTimeInterval( void )
{
	return float(SDL_GetTicks()) / m_interval;
}

void NewTimer::UpdateTimer( void )
{
	if (m_ticking) {
		float time = GetProgramTimeSeconds();
		m_accumulated_time += time - m_time_last;
		m_time_last = time;
	}
}

float NewTimer::GetStaticTime(float time_seconds) const
{
	return time_seconds * m_intervals_per_second;
}

NewTimer::NewTimer(float intervals_per_second) : m_intervals_per_second(0.0f), m_accumulated_time(0.0f), m_time_last(0.0), m_ticking(false)
{
	SetIntervalsPerSecond(intervals_per_second);
}

void NewTimer::SetInterval(float frac_of_second)
{
	if (frac_of_second != 0.0f) {
		SetIntervalsPerSecond(1.0f / frac_of_second);
	} else {
		SetIntervalsPerSecond(0.0f);
	}
}

float NewTimer::GetInterval( void ) const
{
	return m_intervals_per_second != 0.0f ? 1.0f / m_intervals_per_second : 0.0f;
}

void NewTimer::SetIntervalsPerSecond(float intervals_per_second)
{
	m_intervals_per_second = intervals_per_second;
}

float NewTimer::GetIntervalsPerSecond( void ) const
{
	return m_intervals_per_second;
}

void NewTimer::Start( void )
{
	m_time_last = GetProgramTimeSeconds();
	m_ticking = true;
}

void NewTimer::Stop( void )
{
	UpdateTimer();
	m_ticking = false;
}

void NewTimer::Toggle( void )
{
	if (IsTicking()) {
		Stop();
	} else {
		Start();
	}
}

void NewTimer::Reset( void )
{
	m_accumulated_time = 0.0f;
}

void NewTimer::Truncate( void )
{
	if (m_intervals_per_second > 0.0f) {
		float rel_time = GetStaticTime(m_accumulated_time);
		m_accumulated_time -= (rel_time - int(rel_time)) / m_intervals_per_second;
	}
}

void NewTimer::TruncateOnce( void )
{
	if (m_intervals_per_second > 0.0f) {
		float rel_time = GetStaticTime(m_accumulated_time);
		if (rel_time >= 1.0f) {
			m_accumulated_time = (rel_time - 1.0f) / m_intervals_per_second;
		}
	}
}

bool NewTimer::IsTicking( void ) const
{
	return m_ticking;
}

bool NewTimer::IsStopped( void ) const
{
	return !m_ticking;
}

float NewTimer::GetTime( void )
{
	UpdateTimer();
	return GetStaticTime(m_accumulated_time);
}

float NewTimer::GetProgramTimeSeconds( void )
{
	return float(SDL_GetTicks()) / 1000.0f;
}

float NewTimer::GetProgramTime( void )
{
	return GetStaticTime(GetProgramTimeSeconds());
}
