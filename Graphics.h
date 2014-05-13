#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "MTL/mtlAsset.h"
#include "MTL/mtlType.h"
#include "Common.h"
#include "Transform.h"

class Graphics : public mtlAssetInterface, public mtlBase
{
public:
	class Instance
	{
	private:
		const Graphics	*m_graphics;
		Transform		m_transform;
	public:
		const Graphics	*GetGraphics( void ) const { return m_graphics; }
		const Transform	&GetTransform( void ) const { return m_transform; }
		Transform		&GetTransform( void ) { return m_transform; }
	};
public:
	virtual ~Graphics( void ) {}
};

#endif // GRAPHICS_H
