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
		mtlAsset<Graphics>	m_graphics;
		Transform			m_transform;
	public:
		const Graphics	*GetGraphics( void ) const { return m_graphics.GetAsset(); }
		void			SetGraphics(mtlAsset<Graphics> &graphics) { m_graphics = graphics; }
		const Transform	&GetTransform( void ) const { return m_transform; }
		Transform		&GetTransform( void ) { return m_transform; }

		virtual void	SetTime(float time) = 0;
	};
public:
	virtual ~Graphics( void ) {}
};

#endif // GRAPHICS_H
