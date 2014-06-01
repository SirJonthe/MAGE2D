#ifndef RENDERER_H
#define RENDERER_H

#include <GL/gl.h>
#include "Graphics.h"
#include "Camera.h"

class Renderer : public mtlBase
{
private:
	struct Instance
	{
		mtlAsset<Image>				texture;
		mtlArray< mmlVector<2> >	vertex;
		mtlArray< mmlVector<2> >	uv;
		mmlVector<3>				tint;
		mmlMatrix<3,3>				worldTransform;
	};
private:
	mtlArray<Instance>	m_graphics;
	mmlMatrix<3,3>		m_view;
	GLfloat				m_vert[8];
	GLfloat				m_tex[8];
	GLuint				m_vertId;
	GLuint				m_texId;
protected:
	int				GetGraphicsQueueSize( void ) const;
	const Graphics	*GetGraphics(int i) const;
	mmlMatrix<3,3>	GetGraphicsTransform(int i) const;
	void			AddToGraphicsQueue(const Renderer::Instance &graphics);
	void			SetViewTransform(mmlMatrix<3,3> cameraTransform);
public:
					Renderer( void );
	virtual			~Renderer( void );
	virtual void	SetView(const Camera &camera);
	virtual void	AddGraphics(transform, texture, tint); // image
	virtual void	AddGraphics(transform, texture, uv, tint); // sprite
	virtual void	AddGraphics(transform, vert, tint); // polygon
	virtual void	AddGraphics(transform, image, vert, uv, tint); // arbitrary
	virtual void	RenderView( void );
};

#endif // RENDERER_H
