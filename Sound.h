#ifndef SOUND_H
#define SOUND_H

#include "Platform.h"
#include "MTL/mtlAsset.h"
#include "MTL/mtlString.h"

class Sound : public mtlAssetInterface
{
private:
	Mix_Chunk *m_chunk;

public:
	Sound( void );
	Sound(const mtlChars &file);
	~Sound( void );
	bool Load(const mtlDirectory &file);
	int Play( void ) const;
	void Free( void );
	bool IsGood( void ) const;
	static void StopAll( void );
};

/*class SoundInstance
{
private:
	mtlAsset<Sound>	m_sound;
	float			m_volume;
	int				m_channel;

public:
	SoundInstance( void );
	SoundInstance(const mtlAsset<Sound> &sound);
	SoundInstance(const SoundInstance &instance);
	SoundInstance &operator=(const mtlAsset<Sound> &sound);
	SoundInstance &operator=(const SoundInstance &instance);

	float GetVolume( void ) const;
	void SetVolume(float volume);

	int GetChannel( void ) const;

	const mtlAsset<Sound> &GetSound( void ) const;
	void SetSound(const mtlAsset<Sound> &sound);

	bool LoadSound(const mtlChars &file);
	void DeleteSound( void );

	void Play( void ) const;
};*/

#endif // SOUND_H
