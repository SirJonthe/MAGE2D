#ifndef SOUND_H
#define SOUND_H

#include "Platform.h"
#include "MiniLib/MTL/mtlAsset.h"
#include "MiniLib/MTL/mtlString.h"

// @data Sound
// @info Container for sound data.
class Sound : public mtlAssetInterface
{
private:
	Mix_Chunk *m_chunk;

public:
	Sound( void );
	Sound(const mtlChars &file);
	~Sound( void );
	
	// @algo Load
	// @info Loads a sound file.
	// @in file -> The path to the sound file.
	// @out The result of the file operation (TRUE on success).
	bool Load(const mtlPath &file);
	
	// @algo Play
	// @info Plays the sound.
	// @out The channel of output (-1 on fail).
	int Play( void ) const;
	
	// @algo Free
	// @info Frees the sound data resources.
	void Free( void );
	
	// @algo IsGood
	// @out TRUE if everything is OK.
	bool IsGood( void ) const;
	
	// @algo StopAll
	// @info Stops playing all sounds in SDL_mixer sub system.
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
