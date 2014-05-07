#ifndef RENDERER_H
#define RENDERER_H

#include "Graphics.h"
#include "Camera.h"

class Renderer : public Base
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
	virtual			~Renderer( void ) {}
	virtual void	SetView(const Camera &camera) = 0;
	virtual void	AddGraphics(const Graphics&, const mmlMatrix<3,3>&) = 0;
	virtual void	RenderView( void ) = 0;
};

#endif // RENDERER_H
