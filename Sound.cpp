#include "Sound.h"
#include <iostream>

Sound::Sound( void ) : m_chunk(NULL)
{
}

Sound::Sound(const mtlChars &file) : m_chunk(NULL)
{
	Load(file);
}

Sound::~Sound( void )
{
	Free();
}

bool Sound::Load(const mtlChars &file)
{
	std::cout << "Sound::Load: " << file.GetChars() << std::endl;
	Free();
	m_chunk = Mix_LoadWAV(file.GetChars());
	if (!IsGood()) {
		std::cout << "\tfailed: " << Mix_GetError() << std::endl;
	}
	return IsGood();
}

void Sound::Play( void )
{
	if (IsGood()) {
		Mix_PlayChannel(-1, m_chunk, 0);
	}
}

void Sound::Free( void )
{
	if (IsGood()) {
		Mix_FreeChunk(m_chunk);
	}
}

bool Sound::IsGood( void ) const
{
	return m_chunk != NULL;
}

void Sound::StopAll( void )
{
	Mix_HaltChannel(-1);
}
