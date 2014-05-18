#include <iostream>
#include "Sprite.h"
#include "MML/mmlMath.h"
#include "MTL/mtlParser.h"

const Sprite::Animation *Sprite::Instance::GetAnimation( void ) const
{
	if (GetGraphics() == NULL) {
		return NULL;
	}
	return GetGraphics()->GetAnimation(m_currentAnimation);
}

void Sprite::Instance::SetAnimation(const mtlChars &name)
{
	if (GetGraphics() == NULL) { return; }
	int i =	GetGraphics()->GetAnimationIndex(name);
	SetAnimation(i);
}

void Sprite::Instance::SetAnimation(int index)
{
	if (GetGraphics() != NULL && index >= 0 && index < GetGraphics()->GetAnimationCount()) {
		m_currentAnimation = index;
		m_currentFrame = 0;
		m_frameTimer.SetIntervalsPerSecond(GetAnimation()->framesPerSecond);
	}
}

int Sprite::Instance::GetFrame( void ) const
{
	return m_currentFrame;
}

void Sprite::Instance::SetFrame(int frame)
{
	if (frame < 0 || GetAnimation() == NULL) { return; }
	m_currentFrame = frame;
	const Sprite::Animation *animation = GetAnimation();
	if (animation != NULL && m_currentFrame >= animation->frameCount) {
		m_currentFrame = (m_currentFrame % animation->frameCount) + animation->loopBack;
	}
}

void Sprite::Instance::Start( void )
{
	m_paused = false;
	m_frameTimer.Start();
}

void Sprite::Instance::Pause( void )
{
	m_paused = true;
}

void Sprite::Instance::Stop( void )
{
	m_frameTimer.Stop();
	m_currentFrame = 0;
}

void Sprite::Instance::Restart( void )
{
	m_paused = false;
	m_currentFrame = 0;
	m_frameTimer.Restart();
}

void Sprite::Instance::TickFrame( void )
{
	if (GetAnimation() == NULL) { return; }
	m_frameTimer.Tick();
	SetFrame(m_currentFrame + int(m_frameTimer.GetTimeDeltaTick()));
}

Sprite::Sprite( void ) : m_animations()
{}

int Sprite::GetAnimationCount( void ) const
{
	return m_animations.GetSize();
}

const Sprite::Animation *Sprite::GetAnimation(const mtlChars &name) const
{
	int i = GetAnimationIndex(name);
	if (i < 0) { return NULL; }
	return &m_animations[i];
}

const Sprite::Animation *Sprite::GetAnimation(int index) const
{
	if (index < 0 || index >= GetAnimationCount()) {
		return NULL;
	}
	return &m_animations[index];
}

int Sprite::GetAnimationIndex(const mtlChars &name) const
{
	const mtlHash hash = mtlHash(name);
	for (int i = 0; i < GetAnimationCount(); ++i) {
		if (m_animations[i].hash.value == hash.value && m_animations[i].name.Compare(name)) {
			return i;
		}
	}
	return -1;
}

bool Sprite::Load(const mtlDirectory &file)
{
	Destroy();
	mtlArray<Animation> animations;

	if (!file.GetExtension().Compare("sprite")) { return false; }

	// parse

	animations.MergeSort(m_animations);
	return true;
}

void Sprite::Destroy( void )
{
	m_animations.Free();
}
