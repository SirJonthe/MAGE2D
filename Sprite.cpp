#include <iostream>
#include "Sprite.h"
#include "MML/mmlMath.h"
#include "MTL/mtlParser.h"

int Sprite::GetFrameCount( void ) const
{
	return m_numFrames;
}

int Sprite::GetFramesPerRow( void ) const
{
	return m_sheet.GetAsset() != NULL ? m_sheet.GetAsset()->GetWidth() / m_frameWidth : 0;
}

int Sprite::GetFramesPerColumn( void ) const
{
	return m_sheet.GetAsset() != NULL ? m_sheet.GetAsset()->GetHeight() / m_frameHeight : 0;
}

int Sprite::GetWidth( void ) const
{
	return m_frameWidth;
}

int Sprite::GetHeight( void ) const
{
	return m_frameHeight;
}

float Sprite::GetFramesPerSecond( void ) const
{
	return m_framesPerSecond;
}

float Sprite::GetFrameDelay( void ) const
{
	return m_framesPerSecond != 0.0f ? 1.0f / m_framesPerSecond : 0.0f;
}

float Sprite::GetAnimationTime( void ) const
{
	return GetFrameDelay() * GetFrameCount();
}

int Sprite::GetLoopbackFrame( void ) const
{
	return m_loopBack;
}

int Sprite::GetFrameIndex(float time) const
{
	if (time <= 0.0f) { return 0; }

	const float delay = GetFrameDelay();
	const float totalAnimationTime = delay * GetFrameCount();
	float progress = time / totalAnimationTime;
	if (progress > 1.0f) {
		progress = mmlMax2((delay * GetLoopbackFrame()) / totalAnimationTime, progress - int(progress));
	}
	return int(progress * GetFrameCount());
}

bool Sprite::Load(const mtlDirectory &file)
{
	Destroy();

	if (!file.GetExtension().Compare("sprite")) { return false; }

	// parse

	return true;
}

void Sprite::Destroy( void )
{
	m_sheet.Release();
	m_frameWidth = 0;
	m_frameHeight = 0;
	m_numFrames = 0;
	m_framesPerSecond = 0.0f;
	m_loopBack = 0;
}

void Sprite::Draw(float time) const
{
	DrawGraphics(0, GetFrameIndex(time), GL_TRIANGLE_STRIP, 2);
}
