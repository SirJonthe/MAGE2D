#ifndef IMAGERENDERER_H
#define IMAGERENDERER_H

#include <GL/gl.h>
#include <GL/glext.h>
#include "Renderer.h"

class ImageRenderer : public Inherit<Renderer>
{
private:
	GLfloat m_vert[8];
	GLfloat m_tex[8];
	GLuint m_vertId;
	GLuint m_texId;
public:
	ImageRenderer( void );
	~ImageRenderer( void );
	void SetView(const Camera &camera);
	void AddGraphics(const Graphics &graphics, const mmlMatrix<3,3> &worldTransform);
	void RenderView( void );
};

#endif // IMAGERENDERER_H
