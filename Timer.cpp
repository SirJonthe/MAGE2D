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

void NewTimer::UpdateTimer( void ) const
{
	if (m_ticking) {
		float time = GetProgramTimeSeconds();
		m_accumulated_time += time - m_time_last;
		m_time_last = time;
	}
}

float NewTimer::GetStaticTime(float time_seconds) const
{
	return time_seconds * m_beats_per_second;
}

float NewTimer::GetBeatsPerSecond(float tempo, NewTimer::Units units)
{
	if (units == BeatsPerMinute) {
		return tempo / 60.0f;
	} else if (units == FractionOfSecond) {
		return tempo != 0.0f ? 1.0f / tempo : 0.0f;
	}
	return tempo;
}

NewTimer::NewTimer(float tempo, Units units) : m_beats_per_second(0.0f), m_accumulated_time(0.0f), m_time_last(0.0), m_ticking(false)
{
	SetTempo(tempo, units);
}

void NewTimer::SetTempo(float tempo, NewTimer::Units units)
{
	m_beats_per_second = GetBeatsPerSecond(tempo, units);
}

float NewTimer::GetTempo(NewTimer::Units units) const
{
	float tempo = 0.0f;
	if (units == BeatsPerMinute) {
		tempo = m_beats_per_second * 60.0f;
	} else if (units == FractionOfSecond) {
		tempo = m_beats_per_second != 0.0f ? 1.0f / m_beats_per_second : 0.0f;
	} else {
		tempo = m_beats_per_second;
	}
	return tempo;
}

void NewTimer::Start( void )
{
	if (!IsTicking()) {
		m_time_last = GetProgramTimeSeconds();
		m_ticking = true;
	}
}

void NewTimer::Stop( void )
{
	if (IsTicking()) {
		UpdateTimer();
		m_ticking = false;
	}
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
	if (m_beats_per_second > 0.0f) {
		float rel_time = GetStaticTime(m_accumulated_time);
		m_accumulated_time = (rel_time - int(rel_time)) / m_beats_per_second;
	}
}

void NewTimer::Beat( void )
{
	if (m_beats_per_second > 0.0f) {
		float rel_time = GetStaticTime(m_accumulated_time);
		if (rel_time >= 1.0f) {
			m_accumulated_time -= (1.0f / m_beats_per_second);
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

bool NewTimer::IsDue( void ) const
{
	return GetTime() >= 1.0f;
}

float NewTimer::GetTime( void ) const
{
	UpdateTimer();
	return GetStaticTime(m_accumulated_time);
}

void NewTimer::SetTime(float time)
{
	m_accumulated_time = time / m_beats_per_second;
}

int NewTimer::GetBeats( void ) const
{
	return (int)GetTime();
}

float NewTimer::GetProgramTimeSeconds( void )
{
	return float(SDL_GetTicks()) / 1000.0f;
}

float NewTimer::GetTime(float time_sec, float tempo, NewTimer::Units units)
{
	return time_sec * GetBeatsPerSecond(tempo, units);
}

int NewTimer::GetBeats(float time_sec, float tempo, Units units)
{
	return (int)GetTime(time_sec, tempo, units);
}

float NewTimer::GetProgramTime(float tempo, NewTimer::Units units)
{
	float time = 0.0f;
	if (units == BeatsPerMinute) {
		time = GetProgramTimeSeconds() * (tempo / 60.0f);
	} else if (units == FractionOfSecond) {
		time = tempo != 0.0f ? 1.0f / tempo : 0.0f;
	} else {
		time = GetProgramTimeSeconds() * tempo;
	}
	return time;
}

float NewTimer::GetProgramTime( void ) const
{
	return GetStaticTime(GetProgramTimeSeconds());
}
