#ifndef SPRITE_H
#define SPRITE_H

#include <SDL/SDL.h>
#include "Image.h"
#include "Graphics.h"
#include "MTL/mtlArray.h"
#include "MTL/mtlDirectory.h"
#include "MTL/mtlAsset.h"
#include "MTL/mtlType.h"

class Sprite : public mtlInherit<Graphics>
{
public:
	struct Animation
	{
		mtlString		name;
		mtlHash			hash;
		mtlAsset<Image>	image;
		int				frameCount;
		float			framesPerSecond;
		int				loopBack; // if animation does not loop then set loopBack to frameCount - 1
		int				offsetX;
		int				offsetY;

		bool operator<(const Animation &r) { return hash < r.hash; }
		bool operator==(const Animation &r) { return hash == r.hash; }
	};
	class Instance : public Graphics::Instance
	{
	private:
		int		m_currentAnimation;
		int		m_currentFrame;
		Timer	m_frameTimer;
		bool	m_paused;
	public:
		const Animation	*GetAnimation( void ) const;
		void			SetAnimation(const mtlChars &name);
		void			SetAnimation(int index);

		int				GetFrame( void ) const;
		void			SetFrame(int frame);

		void			Start( void );
		void			Pause( void );
		void			Stop( void );
		void			Restart( void );

		void			TickFrame( void );
	};
private:
	mtlArray<Animation> m_animations;
private:
	Sprite(const Sprite&) {}
	Sprite &operator=(const Sprite&) { return *this; }
public:
					Sprite( void );

	int				GetAnimationCount( void ) const;
	const Animation	*GetAnimation(const mtlChars &name) const;
	const Animation	*GetAnimation(int index) const;

	int				GetAnimationIndex(const mtlChars &name) const;

	bool			Load(const mtlDirectory &file);
	void			Destroy( void );
};

#endif // SPRITE_H
