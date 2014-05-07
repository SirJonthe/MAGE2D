#include <iostream>
#include "Sprite.h"
#include "MML/mmlMath.h"
#include "MTL/mtlParser.h"

/*Sprite::Sprite( void ) : m_animations(), m_currentAnimation(0), m_frameTimer()
{
}

bool Sprite::Draw(int x, int y, int xOrientation, SDL_Surface *dst)
{
	if (m_currentAnimation < 0 || m_currentAnimation >= m_animations.GetSize()) { return false; }
	bool result = m_animations[m_currentAnimation].image.Draw(x, y, xOrientation, dst);
	m_animations[m_currentAnimation].image.TickAnimation();
	return result;
}

bool Sprite::Draw(Point p, int xOrientation, SDL_Surface *dst)
{
	return Draw(p.x, p.y, xOrientation, dst);
}

void Sprite::SetAnimation(int index)
{
	m_currentAnimation = mmlClamp(0, index, m_animations.GetSize()-1);
}

int Sprite::GetAnimationCount( void ) const
{
	return m_animations.GetSize();
}

void Sprite::SetFrame(int frame)
{
	if (m_currentAnimation < 0 || m_currentAnimation >= m_animations.GetSize()) { return; }
	m_animations[m_currentAnimation].image.SetCurrentFrame(frame);
}

int Sprite::GetFrame( void ) const
{
	if (m_currentAnimation < 0 || m_currentAnimation >= m_animations.GetSize()) { return 0; }
	m_animations[m_currentAnimation].image.GetCurrentFrame();
}

void Sprite::Destroy( void )
{
	m_animations.Free();
	m_currentAnimation = 0;
}

const Animation *Sprite::GetAnimation( void ) const
{
	if (m_currentAnimation < 0 || m_currentAnimation >= m_animations.GetSize()) { return NULL; }
	return &m_animations[m_currentAnimation];
}*/
