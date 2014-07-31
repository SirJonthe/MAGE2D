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
	void Play( void );
	void Free( void );
	bool IsGood( void ) const;
	static void StopAll( void );
};

#endif // SOUND_H
