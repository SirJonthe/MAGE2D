#include "Graphics.h"

void Graphics::LoadVertexArray(const mtlArray< mmlVector<2> > &array) const
{
	//glFinish(); // do we need to wait for everything else to finish?
	glBindBuffer(GL_ARRAY_BUFFER, m_id.vtx);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBufferData(GL_ARRAY_BUFFER, array.GetSize() * sizeof(mmlVector<2>), &(array[0][0]), GL_STATIC_DRAW);

	Unbind();
}

void Graphics::LoadUVArray(const mtlArray< mmlVector<2> > &array) const
{
	//glFinish(); // do we need to wait for everything else to finish?
	glBindBuffer(GL_ARRAY_BUFFER, m_id.uv);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBufferData(GL_ARRAY_BUFFER, array.GetSize() * sizeof(mmlVector<2>), &(array[0][0]), GL_STATIC_DRAW);

	Unbind();
}

void Graphics::LoadTexture(const GLvoid *pixels, GLsizei width, GLsizei height, GLint format) const
{
	//glFinish(); // do we need to wait for everything else to finish?
	glBindTexture(GL_TEXTURE_2D, m_id.tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

	Unbind();
}

void Graphics::Draw(int vtxOffset, int uvOffset, int numTriangles) const
{
	if (!IsGood()) { return; }

	glBindBuffer(GL_BUFFER, m_id.vtx);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)(vtxOffset * sizeof(mmlVector<2>))); // are these needed?
	glBindBuffer(GL_BUFFER, m_id.uv);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(uvOffset * sizeof(mmlVector<2>))); // needed?
	glBindTexture(GL_TEXTURE_2D, m_id.tex);

	//glDrawArrays(GL_QUADS, 0, 4); // this needs to change to accomodate all arrays
	glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);

	Unbind();
}

Graphics::Graphics( void )
{
	glGenBuffers(1, &m_id.vtx);
	glGenBuffers(1, &m_id.uv);
	glGenTextures(1, &m_id.tex);
}

Graphics::~Graphics( void )
{
	Unbind();
	glDeleteBuffers(1, &m_id.vtx);
	glDeleteBuffers(1, &m_id.uv);
	glDeleteTextures(1, &m_id.tex);
}

int Graphics::GetArea( void ) const
{
	return GetWidth() * GetHeight();
}

void Graphics::Unbind( void )
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

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
	m_graphics.Release();
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

void GraphicsInstance::Draw( void )
{
	if (m_graphics.GetAsset() != NULL) {
		m_time += m_timer.GetTimeDeltaTick();
		m_graphics.GetAsset()->Draw(m_time);
		m_timer.Tick();
	}
}
