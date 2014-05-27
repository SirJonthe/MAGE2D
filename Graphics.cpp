#include "Graphics.h"

const mtlAsset<Graphics> &GraphicsInstance::GetGraphics( void ) const
{
	return m_graphics;
}

void GraphicsInstance::SetGraphics(const mtlAsset<Graphics> &graphics)
{
	m_graphics = graphics;
}

Transform &GraphicsInstance::GetTransform( void )
{
	return m_transform;
}

const Transform &GraphicsInstance::GetTransform( void ) const
{
	return m_transform;
}
