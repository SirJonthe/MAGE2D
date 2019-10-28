#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include "MiniLib/MTL/mtlArray.h"
#include "MiniLib/MTL/mtlPath.h"
#include "MiniLib/MTL/mtlAsset.h"
#include "MiniLib/MTL/mtlType.h"

// REMEMBER:
// Sprites may animate in height as well

// @data Sprite
// @info An animated image.
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

	// @algo GetFrameCount
	// @out The number of frames in the animation.
	int   GetFrameCount( void ) const;
	
	// @algo GetWidth
	// @out The width in pixels of a frame.
	int   GetWidth( void ) const;
	
	// @algo GetHeight
	// @out The height in pixels of a frame.
	int   GetHeight( void ) const;
	
	// @algo GetFramesPerSecond
	// @out The animation speed, in frames per second.
	float GetFramesPerSecond( void ) const;
	
	// @algo GetFrameDelay
	// @out The delay in seconds between each frame.
	float GetFrameDelay( void ) const;
	
	// @algo GetAbinationTime
	// @out The time in seconds for the playback to loop.
	float GetAnimationTime( void ) const;
	
	// @algo GetLoopbackFrame
	// @out The frame on which the animation loops back to.
	int   GetLoopbackFrame( void ) const;
	//int GetStartFrame( void ) const;

	// @algo SetFrameWidth
	// @info Sets the width of a frame.
	// @in width -> Width in pixels of frame.
	void SetFrameWidth(int width);
	
	// @algo SetFrameHeight
	// @info Sets height of a frame.
	// @in height -> Height in pixels of frame.
	void SetFrameHeight(int height);
	
	// @algo SetFrameCount
	// @info Sets the number of frames in the animation.
	// @in count -> Number of frames in animation.
	void SetFrameCount(int count);
	
	// @algo SetFramesPerSecond
	// @info Sets the number of frames to display per second.
	// @in frames -> The number of frames to display per second.
	void SetFramesPerSecond(float frames);
	
	// @algo SetLoopbackFrame
	// @info Sets the frame to loop back to at the end of the animation.
	// @in frame -> The frame to loop back to at the end of the animation.
	void SetLoopbackFrame(int frame);

	// @algo LoadSpriteSheet
	// @info Loads an image to be used as sprite sheet.
	// @in file -> The directory to the image file.
	// @out Error code of file operation (TRUE on success).
	bool LoadSpriteSheet(const mtlChars &file);

	// @algo GetFrameIndex
	// @info Returns a wrapped frame index based on the input absolute index.
	// @in frame -> Absolute index.
	// @out The wrapped frame index.
	int  GetFrameIndex(int frame) const;

	// @algo Load
	// @info Loads a sprite file from disk.
	// @in file -> The path to the sprite file.
	// @out The error code of the file operation (TRUE on success).
	bool Load(const mtlPath &file);
	
	// @algo Destroy
	// @info Frees sprite resources.
	void Destroy( void );
	
	// @algo Draw
	// @info Draws the given frame.
	// @in frame -> Frame index.
	void Draw(int frame, float) const;
	
	// @algo IsGood
	// @out TRUE if everything is OK.
	bool IsGood( void ) const;
};

#endif // SPRITE_H
