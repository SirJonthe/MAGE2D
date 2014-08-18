#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include "MTL/mtlArray.h"
#include "MTL/mtlDirectory.h"
#include "MTL/mtlAsset.h"
#include "MTL/mtlType.h"

// REMEMBER:
// Sprites may animate in height as well

class Sprite : public mtlInherit<Graphics>
{
private:
	struct Metadata
	{
		mtlString file;
		int frameWidth; // init to -1 (calculate m_frameCount based on this if not -1)
		int frameCount; // init to -1 (calculate m_frameWidth based on this if not -1)
		float framesPerSecond;
		int loopBack;
		bool isLooping;
		Metadata( void ) : file(), frameWidth(-1), frameCount(-1), framesPerSecond(0.0f), loopBack(0), isLooping(true) {}
	};

private:
	mtlAsset<Graphics>	m_sheet;
	int					m_frameWidth;
	int					m_frameHeight;
	int					m_numFrames;
	float				m_framesPerSecond;
	int					m_loopBack; // if animation does not loop then set loopBack to frameCount - 1

private:
	Sprite(const Sprite&) {}
	Sprite &operator=(const Sprite&) { return *this; }
	bool LoadMetadata(Metadata &out, const mtlDirectory &file, mtlList<mtlDirectory> &filesOpened);

public:
			Sprite( void );

	int		GetFrameCount( void ) const;
	int		GetWidth( void ) const;
	int		GetHeight( void ) const;
	float	GetFramesPerSecond( void ) const;
	float	GetFrameDelay( void ) const;
	float	GetAnimationTime( void ) const;
	int		GetLoopbackFrame( void ) const;
	//int		GetStartFrame( void ) const;

	int		GetFrameIndex(float time) const;

	bool	Load(const mtlDirectory &file);
	void	Destroy( void );
	void	Draw(float time) const;
	bool	IsGood( void ) const;
};

#endif // SPRITE_H
