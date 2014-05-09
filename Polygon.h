#ifndef POLYGON_H
#define POLYGON_H

#include <GL/gl.h>
#include "Graphics.h"
#include "MTL/mtlArray.h"

class Polygon : public Inherit<Graphics>
{
private:
	mtlArray< mmlVector<2> >	m_vert;
	mmlVector<4>				m_color;
	GLuint						m_id;
	static GLuint				m_bound;
private:
	Polygon(const Polygon&) {}
	Polygon &operator=(const Polygon&) { return *this; }
public:

	Polygon( void );
	~Polygon( void );

	const mmlVector<2> &GetVertex(int i) const;
	mmlVector<2> &GetVertex(int i);
	void SetVertex(int i, mmlVector<2> v);
	void AllocateVertex(int count);
	int GetVertexCount( void ) const;

	void Destroy( void );
	void ApplyChanges( void );
	void Bind( void ) const;
	static void Unbind( void );

	mmlVector<4> GetColor( void ) const;
	void SetColor(mmlVector<3> color);
	void SetColor(mmlVector<4> color);
	void SetColor(float r, float g, float b, float a = 1.0f);

	float GetRed( void ) const;
	float GetGreen( void ) const;
	float GetBlue( void ) const;
	float GetAlpha( void ) const;
	void SetRed(float r);
	void SetGreen(float g);
	void SetBlue(float b);
	void SetAlpha(float a);

	bool Load(const mtlDirectory &file);
};

#endif // POLYGON_H
