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

void Graphics::LoadTexture(const GLvoid *pixels, GLsizei width, GLsizei height, GLint bytesPerPixel, GLint format) const
{
	//glFinish(); // do we need to wait for everything else to finish?
	glBindTexture(GL_TEXTURE_2D, m_id.tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, bytesPerPixel, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

	Unbind();
}

void Graphics::DrawGraphics(int vtxOffset, int uvOffset, GLenum mode, GLsizei count, long long textureID) const
{
	if (!IsGood()) { return; }

	glBindBuffer(GL_ARRAY_BUFFER, m_id.vtx);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)(vtxOffset * sizeof(mmlVector<2>)));

	glBindBuffer(GL_ARRAY_BUFFER, m_id.uv);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(uvOffset * sizeof(mmlVector<2>)));

	if (textureID < 0) {
		glBindTexture(GL_TEXTURE_2D, m_id.tex);
	} else {
		glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
	}

	switch (mode) {
	case GL_TRIANGLES:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
		glDrawArrays(mode, 0, count*3);
		break;
	case GL_LINES:
	case GL_LINE_LOOP:
	case GL_LINE_STRIP:
		glDrawArrays(mode, 0, count*2);
		break;
	case GL_POINTS:
		glDrawArrays(mode, 0, count);
		break;
	default: break;
	}

	Unbind();
}

Graphics::Graphics( void ) : mtlBase(this)
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

GLuint Graphics::GetTextureID( void ) const
{
	return m_id.tex;
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

int GraphicsInstance::GetGraphicsWidth( void ) const
{
	return !m_graphics.IsNull() ? m_graphics.GetAsset()->GetWidth() : 0;
}

int GraphicsInstance::GetGraphicsHeight( void ) const
{
	return !m_graphics.IsNull() ? m_graphics.GetAsset()->GetHeight() : 0;
}

int GraphicsInstance::GetGraphicsArea( void ) const
{
	return !m_graphics.IsNull() ? m_graphics.GetAsset()->GetArea() : 0;
}

bool GraphicsInstance::IsGood( void ) const
{
	return !m_graphics.IsNull() ? m_graphics.GetAsset()->IsGood() : 0;
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

void GraphicsInstance::SetTint(float r, float g, float b, float a)
{
	SetRed(r);
	SetGreen(g);
	SetBlue(b);
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
	return m_timer.GetTempo(Timer::BeatsPerSecond);
}

void GraphicsInstance::SetIntervalsPerSecond(float intervals)
{
	m_timer.SetTempo(intervals, Timer::BeatsPerSecond);
}

float GraphicsInstance::GetInterval( void ) const
{
	return m_timer.GetTempo(Timer::SecondsPerBeat);
}

void GraphicsInstance::SetInterval(float interval)
{
	m_timer.SetTempo(interval, Timer::SecondsPerBeat);
}

void GraphicsInstance::Start( void )
{
	m_timer.Start();
}

void GraphicsInstance::Stop( void )
{
	m_timer.Stop();
}

void GraphicsInstance::Restart( void )
{
	m_timer.Reset();
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
		m_graphics.GetAsset()->Draw(m_timer.GetBeats(), m_timer.GetPartBeat());
	}
}
