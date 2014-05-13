#include <iostream>
#include "Sprite.h"
#include "MML/mmlMath.h"
#include "MTL/mtlParser.h"

Sprite::Sprite( void ) : m_animations(), m_currentAnimation(0), m_currentFrame(0), m_frameTimer(), m_paused(false)
{
	m_frameTimer.Stop();
}

int Sprite::GetAnimationCount( void ) const
{
	return m_animations.GetSize();
}

const Animation *Sprite::GetAnimation( void ) const
{
	if (m_currentAnimation >= GetAnimationCount()) {
		return NULL;
	}
	return &m_animations[m_currentAnimation];
}

const Animation *Sprite::GetAnimation(const mtlChars &name) const
{
	int i = GetIndexOf(name);
	if (i < 0) { return NULL; }
	return &m_animations[m_currentAnimation];
}

const Animation *Sprite::GetAnimation(int index) const
{
	if (index < 0 || index >= GetAnimationCount()) {
		return NULL;
	}
	return &m_animations[m_currentAnimation];
}

void Sprite::SetAnimation(const mtlChars &name)
{
	int i =	GetIndexOf(name);
	if (i >= 0) {
		m_currentAnimation = i;
	}
}

void Sprite::SetAnimation(int index)
{
	if (index >= 0 && index < GetAnimationCount()) {
		m_currentAnimation = index;
	}
}

int Sprite::GetIndexOf(const mtlChars &name) const
{
	const mtlHash hash = mtlHash(name);
	for (int i = 0; i < GetAnimationCount(); ++i) {
		if (m_animations[i].hash.value == hash.value && m_animations[i].name.Compare(name)) {
			return i;
		}
	}
	return -1;
}

int Sprite::GetFrame( void ) const
{
	return m_currentFrame;
}

void Sprite::SetFrame(int frame)
{
	m_currentFrame = frame;
}

bool Sprite::Load(const mtlDirectory &file)
{
}

void Sprite::Destroy( void )
{
	m_animations.Free();
}

void Sprite::Start( void )
{
	m_paused = false;
	m_frameTimer.Start();
}

void Sprite::Pause( void )
{
	m_paused = true;
}

void Sprite::Stop( void )
{
	m_frameTimer.Stop();
	m_currentFrame = 0;
}

void Sprite::Restart( void )
{
	m_paused = false;
	m_currentFrame = 0;
	m_frameTimer.Restart();
}

void Sprite::TickFrame( void )
{

}
