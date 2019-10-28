#ifndef IMAGE_H
#define IMAGE_H

#include "MiniLib/MTL/mtlString.h"
#include "MiniLib/MTL/mtlType.h"
#include "Common.h"
#include "Timer.h"
#include "Graphics.h"

// @data Image
// @info Contains pixel information. Stored in main RAM and GPU memory.
class Image : public mtlInherit<Graphics, Image>
{
private:
	SDL_Surface *m_image;
	int          m_width;
	int          m_height;

private:
	Image(const Image &) : mtlInherit<Graphics, Image>(this) {}
	Image &operator=(const Image &) { return *this; }

public:
					   Image( void );
					   Image(const mtlPath &file);
					  ~Image( void );

	// @algo Destroy
	// @info Frees graphics resources
	void               Destroy( void );
	
	// @algo Load
	// @info Loads image data from file.
	// @in file -> The directory of the image file.
	// @out The result of the file read operation (TRUE on success).
	bool               Load(const mtlPath &file);

	// @algo IsGood
	// @out TRUE if everything is OK.
	bool               IsGood( void ) const;

	// @algo GetWidth
	// @out The width in pixels.
	int                GetWidth( void ) const;
	
	// @algo GetHeight
	// @out The height in pixels
	int                GetHeight( void ) const;

	// @algo GetSurface
	// @out The SDL_Surface that contains the raw image data.
	const SDL_Surface *GetSurface( void ) const;
	
	// @algo SetSurface
	// @in image -> The raw image data.
	// @inout image -> The SDL_Surface to use as raw image data. Is set to NULL after function call.
	// @out The error code (TRUE on success).
	bool               SetSurface(SDL_Surface *&image);
	
	// @algo SetSurface
	// @info Creates new image with the given 32-bit pixel data and dimensions.
	// @in
	//   pixels -> The pixels (32-bit, 8 bits per channel).
	//   width -> image width in pixels.
	//   height -> image height in pixels.
	// @out Result of operation (TRUE on success).
	bool               SetSurface(unsigned int *pixels, int width, int height);

	// @algo IsColorKey
	// @in x, y -> coordinates of pixel to test.
	// @out TRUE if color at specified coordinates is color key.
	bool               IsColorKey(int x, int y) const;
	
	// @algo IsColorKey
	// @in pixel -> A 32-bit color (8 bits per channel).
	// @out TRUE if 32-bit input color is same as current color key.
	bool               IsColorKey(Uint32 pixel) const;

	// @algo GetPixels
	// @out Pixel data in 32-bit format (address).
	const Uint32      *GetPixels( void ) const;
	
	// @algo GetPixels
	// @info Gets the address of the first pixel in a given row of pixels.
	// @in y -> The requested row index.
	// @out The address of first pixel in a given row of pixels.
	const Uint32      *GetPixels(int y) const;
	
	// @algo GetPixels
	// @info Gets the ad
	// @in x, y -> The coordinates of the requested pixel (32-bit format, 8 bits per channel).
	// @out The address of the pixel at the given pixel coordinate.
	const Uint32      *GetPixels(int x, int y) const;

	// @algo Draw
	// @info Draws the image.
	// @in int, float -> unused.
	void               Draw(int, float) const;
};

#endif // IMAGE_H
