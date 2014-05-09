#include "Polygon.h"

GLuint Polygon::m_bound = 0;

Polygon::Polygon( void ) : m_vert(), m_color(1.0f, 1.0f, 1.0f, 1.0f), m_id(0)
{}

Polygon::~Polygon( void )
{
	Destroy();
}

const mmlVector<2> &Polygon::GetVertex(int i) const
{
	return m_vert[i];
}

mmlVector<2> &Polygon::GetVertex(int i)
{
	return m_vert[i];
}

void Polygon::SetVertex(int i, mmlVector<2> v)
{
	m_vert[i] = v;
}

void Polygon::AllocateVertex(int count)
{
	m_vert.Create(count);
}

int Polygon::GetVertexCount( void ) const
{
	return m_vert.GetSize();
}

void Polygon::Destroy( void )
{
	if (m_id != 0) {
		if (m_id == m_bound) {
			Unbind();
		}
		glDeleteBuffers(1, &m_id);
		m_id = 0;
	}
}

void Polygon::ApplyChanges( void )
{
	// create is there is no buffer
	if (m_id == 0) {
		glGenBuffers(1, &m_id);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_VERTEX_ARRAY, sizeof(mmlVector<2>)*GetVertexCount(), m_vert, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_bound); // bind back to old so we don't disrupt state
}

void Polygon::Bind( void ) const
{
	if (m_id != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);
		m_bound = m_id;
	}
}

void Polygon::Unbind( void )
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_bound = 0;
}

mmlVector<4> Polygon::GetColor( void ) const
{
	return m_color;
}

void Polygon::SetColor(mmlVector<3> color)
{
	SetRed(color[0]);
	SetGreen(color[1]);
	SetBlue(color[2]);
}

void Polygon::SetColor(mmlVector<4> color)
{
	SetRed(color[0]);
	SetGreen(color[1]);
	SetBlue(color[2]);
	SetAlpha(color[3]);
}

void Polygon::SetColor(float r, float g, float b, float a)
{
	SetRed(r);
	SetGreen(g);
	SetBlue(b);
	SetAlpha(a);
}

float Polygon::GetRed( void ) const
{
	return m_color[0];
}

float Polygon::GetGreen( void ) const
{
	return m_color[1];
}

float Polygon::GetBlue( void ) const
{
	return m_color[2];
}

float Polygon::GetAlpha( void ) const
{
	return m_color[3];
}

void Polygon::SetRed(float r)
{
	m_color[0] = mmlClamp(0.0f, r, 1.0f);
}

void Polygon::SetGreen(float g)
{
	m_color[1] = mmlClamp(0.0f, g, 1.0f);
}

void Polygon::SetBlue(float b)
{
	m_color[2] = mmlClamp(0.0f, b, 1.0f);
}

void Polygon::SetAlpha(float a)
{
	m_color[3] = mmlClamp(0.0f, a, 1.0f);
}

bool Polygon::Load(const mtlDirectory &file)
{
	return false;
}
