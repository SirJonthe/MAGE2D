#ifndef RENDERER_H
#define RENDERER_H

#include <GL/gl.h>
#include "Graphics.h"
#include "Camera.h"

class Renderer : public mtlBase
{
private:
	struct GraphicsInstance
	{
		const Graphics *graphics;
		mmlMatrix<3,3> transform;
		bool operator<(const GraphicsInstance &inst) const;
	};
	mtlArray<GraphicsInstance>	m_graphics;
	mmlMatrix<3,3>				m_view;
	GLfloat						m_vert[8];
	GLfloat						m_tex[8];
	GLuint						m_vertId;
	GLuint						m_texId;
protected:
	int				GetGraphicsQueueSize( void ) const;
	const Graphics	*GetGraphics(int i) const;
	mmlMatrix<3,3>	GetGraphicsTransform(int i) const;
	void			AddToGraphicsQueue(const Graphics &graphics, const mmlMatrix<3,3> &worldTransform);
	void			ClearGraphicsQueue( void );
	void			SortGraphicsQueue( void );
	void			SetViewTransform(mmlMatrix<3,3> cameraTransform);
public:
					Renderer( void );
	virtual			~Renderer( void );
	virtual void	SetView(const Camera &camera);
	virtual void	AddGraphics(const Graphics &graphics, const mmlMatrix<3,3> &worldTransform);
	virtual void	RenderView( void );
};

#endif // RENDERER_H
