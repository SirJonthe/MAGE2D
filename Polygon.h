#ifndef POLYGON_H
#define POLYGON_H

#include <GL/gl.h>
#include "Graphics.h"
#include "MTL/mtlArray.h"

class Polygon : public Inherit<Graphics>
{
private:
	mtlArray< mmlVector<2> >	m_vert;
	mmlVector<3>				m_color;
	GLuint						m_id;
public:
};

#endif // POLYGON_H
