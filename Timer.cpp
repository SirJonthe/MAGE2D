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
	if (m_interval > 0.0f) {
		float now = GetProgramTimeSeconds();
		return (now - m_timeLast) / m_interval;
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
