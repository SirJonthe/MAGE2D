#include <GL/gl.h>
#include "Renderer.h"

bool Renderer::GraphicsInstance::operator <(const Renderer::GraphicsInstance &inst) const
{
	return graphics < inst.graphics;
}

int Renderer::GetGraphicsQueueSize( void ) const
{
	return m_graphics.GetSize();
}

const Graphics *Renderer::GetGraphics(int i) const
{
	return m_graphics[i].graphics;
}

mmlMatrix<3,3> Renderer::GetGraphicsTransform(int i) const
{
	return m_graphics[i].transform * m_view;
}

void Renderer::AddToGraphicsQueue(const Graphics &graphics, const mmlMatrix<3,3> &worldTransform)
{
	if (m_graphics.GetSize() + 1 > m_graphics.GetCapacity()) {
		m_graphics.SetCapacity(m_graphics.GetCapacity() * 2);
	}
	m_graphics.Resize(m_graphics.GetSize() + 1);
	int index = m_graphics.GetSize() - 1;
	m_graphics[index].graphics = &graphics;
	m_graphics[index].transform = worldTransform;
}

void Renderer::ClearGraphicsQueue( void )
{
	m_graphics.Resize(0);
}

void Renderer::SortGraphicsQueue( void )
{
	mtlArray<GraphicsInstance> sorted;
	m_graphics.MergeSort(sorted);
	m_graphics.Copy(sorted);
}

void Renderer::SetViewTransform(mmlMatrix<3,3> viewTransform)
{
	m_view = viewTransform;
}

Renderer::Renderer( void ) : m_graphics(), m_view(mmlMatrix<3,3>::IdentityMatrix())
{
	m_graphics.poolMemory = true;
	m_graphics.SetCapacity(512);
}
