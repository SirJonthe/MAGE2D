#include "Graphics.h"
#include "Renderer.h"

GraphicsInstance &GraphicsInstance::operator =(const GraphicsInstance &instance)
{
	if (this != &instance) {
		m_graphics = instance.m_graphics;
		m_tint = instance.m_tint;
		m_timer = instance.m_timer;
		m_time = instance.m_time;
	}
	return *this;
}

GraphicsInstance &GraphicsInstance::operator =(const mtlAsset<Graphics> &graphics)
{
	m_graphics = graphics;
	return *this;
}

const mtlAsset<Graphics> &GraphicsInstance::GetGraphics( void ) const
{
	return m_graphics;
}

void GraphicsInstance::SetGraphics(const mtlAsset<Graphics> &graphics)
{
	m_graphics = graphics;
}

void GraphicsInstance::DeleteGraphics( void )
{
	m_graphics.Delete();
}

Transform &GraphicsInstance::GetTransform( void )
{
	return m_transform;
}

const Transform &GraphicsInstance::GetTransform( void ) const
{
	return m_transform;
}

const mmlVector<4> &GraphicsInstance::GetTint( void ) const
{
	return m_tint;
}

void GraphicsInstance::SetTint(const mmlVector<4> &tint)
{
	SetRed(tint[0]);
	SetGreen(tint[1]);
	SetBlue(tint[2]);
	SetAlpha(tint[3]);
}

void GraphicsInstance::SetTint(const mmlVector<3> &tint, float a)
{
	SetRed(tint[0]);
	SetGreen(tint[1]);
	SetBlue(tint[2]);
	SetAlpha(a);
}

float GraphicsInstance::GetRed( void ) const
{
	return m_tint[0];
}

void GraphicsInstance::SetRed(float r)
{
	m_tint[0] = mmlClamp(0.0f, r, 1.0f);
}

float GraphicsInstance::GetGreen( void ) const
{
	return m_tint[1];
}

void GraphicsInstance::SetGreen(float g)
{
	m_tint[1] = mmlClamp(0.0f, g, 1.0f);
}

float GraphicsInstance::GetBlue( void ) const
{
	return m_tint[2];
}

void GraphicsInstance::SetBlue(float b)
{
	m_tint[2] = mmlClamp(0.0f, b, 1.0f);
}

float GraphicsInstance::GetAlpha( void ) const
{
	return m_tint[3];
}

void GraphicsInstance::SetAlpha(float a)
{
	m_tint[3] = mmlClamp(0.0f, a, 1.0f);
}

float GraphicsInstance::GetIntervalsPerSecond( void ) const
{
	return m_timer.GetIntervalsPerSecond();
}

void GraphicsInstance::SetIntervalsPerSecond(float intervals)
{
	m_timer.SetIntervalsPerSecond(intervals);
}

float GraphicsInstance::GetInterval( void ) const
{
	return m_timer.GetInterval();
}

void GraphicsInstance::SetInterval(float interval)
{
	m_timer.SetInterval(interval);
}

void GraphicsInstance::Start( void )
{
	m_timer.Start();
}

void GraphicsInstance::Stop( void )
{
	m_timer.Stop();
	m_time = 0.0f;
}

void GraphicsInstance::Restart( void )
{
	m_timer.Restart();
	m_time = 0.0f;
}

bool GraphicsInstance::IsStopped( void ) const
{
	return m_timer.IsStopped();
}

bool GraphicsInstance::IsTicking( void ) const
{
	return m_timer.IsTicking();
}

void GraphicsInstance::Draw(Renderer &renderer)
{
	if (m_graphics.GetAsset() != NULL) {
		m_time += m_timer.GetTimeDeltaTick();
		m_graphics.GetAsset()->Draw(renderer, m_transform, m_tint, m_time);
		m_timer.Tick();
	}
}
