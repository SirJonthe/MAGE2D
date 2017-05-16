#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include "MiniLib/MTL/mtlArray.h"
#include "MiniLib/MTL/mtlPath.h"
#include "MiniLib/MTL/mtlAsset.h"
#include "MiniLib/MTL/mtlType.h"

// REMEMBER:
// Sprites may animate in height as well

class Sprite : public mtlInherit<Graphics, Sprite>
{
private:
	struct Metadata
	{
		mtlString file;
		int frameWidth; // init to -1 (calculate m_frameCount based on this if not -1)
		int frameCount; // init to -1 (calculate m_frameWidth based on this if not -1)
		int frameHeight; // calculate if -1
		float framesPerSecond;
		int loopBack;
		bool isLooping;
		Metadata( void ) : file(), frameWidth(-1), frameCount(-1), frameHeight(-1), framesPerSecond(0.0f), loopBack(0), isLooping(true) {}
	};

private:
	mtlAsset<Graphics> m_sheet;
	int                m_frameWidth;
	int                m_frameHeight;
	int                m_numFrames;
	float              m_framesPerSecond;
	int                m_loopBack; // if animation does not loop then set loopBack to frameCount - 1

private:
	Sprite(const Sprite&) : mtlInherit<Graphics, Sprite>(this) {}
	Sprite &operator=(const Sprite&) { return *this; }
	bool LoadMetadata(Metadata &out, const mtlPath &file, mtlList<mtlPath> &filesOpened);

public:
	Sprite( void );

	int   GetFrameCount( void ) const;
	int   GetWidth( void ) const;
	int   GetHeight( void ) const;
	float GetFramesPerSecond( void ) const;
	float GetFrameDelay( void ) const;
	float GetAnimationTime( void ) const;
	int   GetLoopbackFrame( void ) const;
	//int GetStartFrame( void ) const;

	void SetFrameWidth(int width);
	void SetFrameHeight(int height);
	void SetFrameCount(int count);
	void SetFramesPerSecond(float frames);
	void SetLoopbackFrame(int frame);

	bool LoadSpriteSheet(const mtlChars &file);

	int  GetFrameIndex(int frame) const;

	bool Load(const mtlPath &file);
	void Destroy( void );
	void Draw(int frame, float) const;
	bool IsGood( void ) const;
};

#endif // SPRITE_H
