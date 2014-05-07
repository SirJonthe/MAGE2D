#ifndef IMAGE_H
#define IMAGE_H

#include <GL/gl.h>
#include <SDL/SDL.h>
#include "MTL/mtlString.h"
#include "MTL/mtlAsset.h"
#include "Common.h"
#include "Timer.h"
#include "Graphics.h"

class Image : public mtlAssetInterface, public Inherit<Graphics>
{
private:
	SDL_Surface		*m_image;
	int				m_width;
	int				m_height;
	GLuint			m_id;
	static GLuint	m_bound;
private:
	Image(const Image &) {}
	Image &operator=(const Image &) { return *this; }
public:
						Image( void );
						Image(const mtlDirectory &file);
						~Image( void );
	void				Destroy( void );
	bool				Load(const mtlDirectory &file);
	bool				IsGood( void ) const;
	int					GetWidth( void ) const;
	int					GetHeight( void ) const;
	int					GetArea( void ) const;
	const SDL_Surface	*GetSurface( void ) const;
	bool				SetSurface(SDL_Surface *image);
	bool				IsColorKey(int x, int y) const;
	const Uint32		*GetPixels( void ) const;
	const Uint32		*GetPixels(int y) const;
	const Uint32		*GetPixels(int x, int y) const;
	void				BindTexture( void ) const;
	static void			UnbindTexture( void );
};

#endif // IMAGE_H
