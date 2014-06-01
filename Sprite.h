#ifndef SPRITE_H
#define SPRITE_H

#include <SDL/SDL.h>
#include "Image.h"
#include "Graphics.h"
#include "MTL/mtlArray.h"
#include "MTL/mtlDirectory.h"
#include "MTL/mtlAsset.h"
#include "MTL/mtlType.h"

// REMEMBER:
// Sprites must animate in height as well, since all textures must be n^2

class Sprite : public mtlInherit<Graphics>
{
private:
	mtlAsset<Image>	m_sheet;
	int				m_frameWidth;
	int				m_frameHeight;
	float			m_framesPerSecond;
	int				m_loopBack; // if animation does not loop then set loopBack to frameCount - 1
private:
	Sprite(const Sprite&) {}
	Sprite &operator=(const Sprite&) { return *this; }
public:
			Sprite( void );

	int		GetFrameCount( void ) const;

	bool	Load(const mtlDirectory &file);

	int		GetWidth( void ) const;
	int		GetHeight( void ) const;
	void	Destroy( void );
	void	Bind( void );
	void	Unbind( void );
	void	Draw(Renderer &renderer, Transform transform, mmlVector<4> tint, float time);
};

#endif // SPRITE_H
