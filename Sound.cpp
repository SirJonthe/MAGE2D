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

bool Sound::Load(const mtlPath &file)
{
	std::cout << "Sound::Load: " << file.GetPath().GetChars() << std::endl;
	Free();
	m_chunk = Mix_LoadWAV(file.GetPath().GetChars());
	if (!IsGood()) {
		std::cout << "\tfailed: " << Mix_GetError() << std::endl;
	}
	return IsGood();
}

int Sound::Play( void ) const
{
	return IsGood() ? Mix_PlayChannel(-1, m_chunk, 0) : -1;
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
