#ifndef IMAGE_H
#define IMAGE_H

#include "MTL/mtlString.h"
#include "MTL/mtlType.h"
#include "Common.h"
#include "Timer.h"
#include "Graphics.h"

class Image : public mtlInherit<Graphics>
{
private:
	SDL_Surface	*m_image;
	int			m_width;
	int			m_height;
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

	const SDL_Surface	*GetSurface( void ) const;
	bool				SetSurface(SDL_Surface *image);

	bool				IsColorKey(int x, int y) const;
	bool				IsColorKey(Uint32 pixel) const;

	const Uint32		*GetPixels( void ) const;
	const Uint32		*GetPixels(int y) const;
	const Uint32		*GetPixels(int x, int y) const;

	void				Draw(float time) const;
};

#endif // IMAGE_H
