#ifndef SPRITE_H
#define SPRITE_H

#include <SDL/SDL.h>
#include "Image.h"
#include "Graphics.h"
#include "MTL/mtlArray.h"
#include "MTL/mtlDirectory.h"
#include "MTL/mtlAsset.h"

struct Animation
{
	mtlString		name;
	mtlAsset<Image>	image;
	int				frameCount;
	float			framesPerSecond;
	int				loopBack; // if animation does not loop then set loopBack to frameCount - 1
	int				offsetX;
	int				offsetY;
	//CollisionShape	*collisionShape;
};

/*class Sprite : public mtlAssetInterface, public Inherit<Graphics>
{
public:

private:
	mtlArray<Animation> m_animations;
	int					m_currentAnimation;
	Timer				m_frameTimer;
private:
	Sprite(const Sprite&) {}
	Sprite &operator=(const Sprite&) { return *this; }
public:
					Sprite( void );
	bool			Draw(int x, int y, int xOrientation = 1, SDL_Surface *dst = NULL);
	bool			Draw(Point p, int xOrientation = 1, SDL_Surface *dst = NULL);
	//int				GetIndexOf(const mtlChars &name) const;
	//void			SetAnimation(const mtlChars &name) const;
	void			SetAnimation(int index);
	int				GetAnimationCount( void ) const;
	void			SetFrame(int frame);
	int				GetFrame( void ) const;
	void			Destroy( void );
	const Animation	*GetAnimation( void ) const;
};*/

#endif // SPRITE_H
