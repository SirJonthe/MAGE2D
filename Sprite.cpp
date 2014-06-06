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

int Sprite::GetLoopbackFrame( void ) const
{
	return m_loopBack;
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
	// get frame based on time
	// call Graphics::Draw() with appropriate values
}
