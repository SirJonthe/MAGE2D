#ifndef IMAGE_H
#define IMAGE_H

#include "MiniLib/MTL/mtlString.h"
#include "MiniLib/MTL/mtlType.h"
#include "Common.h"
#include "Timer.h"
#include "Graphics.h"

class Image : public mtlInherit<Graphics, Image>
{
private:
	SDL_Surface	*m_image;
	int			m_width;
	int			m_height;

private:
	Image(const Image &) : mtlInherit<Graphics, Image>(this) {}
	Image &operator=(const Image &) { return *this; }

public:
						Image( void );
						Image(const mtlPath &file);
						~Image( void );

	void				Destroy( void );
	bool				Load(const mtlPath &file);

	bool				IsGood( void ) const;

	int					GetWidth( void ) const;
	int					GetHeight( void ) const;

	const SDL_Surface	*GetSurface( void ) const;
	bool				SetSurface(SDL_Surface *&image);
	bool				SetSurface(unsigned int *pixels, int width, int height);

	bool				IsColorKey(int x, int y) const;
	bool				IsColorKey(Uint32 pixel) const;

	const Uint32		*GetPixels( void ) const;
	const Uint32		*GetPixels(int y) const;
	const Uint32		*GetPixels(int x, int y) const;

	void				Draw(int, float) const;
};

#endif // IMAGE_H
