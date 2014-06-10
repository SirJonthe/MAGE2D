#include <cstdlib>
#include <iostream>
#include "MML/mmlMath.h"
#include "Image.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const Uint32 rmask = 0xff000000;
const Uint32 gmask = 0x00ff0000;
const Uint32 bmask = 0x0000ff00;
const Uint32 amask = 0x000000ff;
#else
const Uint32 rmask = 0x000000ff;
const Uint32 gmask = 0x0000ff00;
const Uint32 bmask = 0x00ff0000;
const Uint32 amask = 0xff000000;
#endif

Image::Image( void ) : m_image(NULL), m_width(0), m_height(0)
{
}

Image::Image(const mtlDirectory &file) : m_image(NULL), m_width(0), m_height(0)
{
	Load(file);
}

Image::~Image( void )
{
	if (m_image != NULL) {
		SDL_FreeSurface(m_image);
	}
}

void Image::Destroy( void )
{
	if (m_image != NULL) {
		SDL_FreeSurface(m_image);
		m_image = NULL;
	}
	m_width = 0;
	m_height = 0;
}

bool Image::Load(const mtlDirectory &file)
{
	std::cout << "Image::Load: " << file.GetDirectory().GetChars() << std::endl;

	Destroy();

	SDL_Surface *image = IMG_Load(file.GetDirectory().GetChars());
	if (image == NULL) {
		std::cout << "\tfailed: " << SDL_GetError() << std::endl;
		return false;
	}

	return SetSurface(image);
}

bool Image::IsGood( void ) const
{
	return m_image != NULL;
}

int Image::GetWidth( void ) const
{
	return m_width;
}

int Image::GetHeight( void ) const
{
	return m_height;
}

const SDL_Surface *Image::GetSurface( void ) const
{
	return m_image;
}

bool Image::SetSurface(SDL_Surface *image)
{
	std::cout << "Image::SetSurface: " << std::endl;

	Destroy();

	if (image == NULL) {
		std::cout << "\tfailed: image parameter is NULL" << std::endl;
		return false;
	}

	m_image = SDL_DisplayFormatAlpha(image);
	SDL_FreeSurface(image);
	if (m_image == NULL) {
		std::cout << "\tfailed: " << SDL_GetError() << std::endl;
		return false;
	}

	if (SDL_MUSTLOCK(m_image)) {
		SDL_LockSurface(m_image);
	}

	Uint32 *pixels = (Uint32*)m_image->pixels;
	Uint32 colorKey = SDL_MapRGBA(m_image->format, 255, 0, 255, 255);
	Uint32 alpha = colorKey & (~m_image->format->Amask);
	for (int i = 0; i < m_image->w*m_image->h; ++i, ++pixels) {
		if ((*pixels | m_image->format->Amask) == colorKey) {
			*pixels = alpha;
		}
	}

	if (SDL_MUSTLOCK(m_image)) {
		SDL_UnlockSurface(m_image);
	}

	m_width = m_image->w;
	m_height = m_image->h;

	LoadTexture(m_image->pixels, m_width, m_height, GL_RGBA);

	mtlArray< mmlVector<2> > vtx;
	vtx.Create(4);
	vtx[0][0] = 0.0f;
	vtx[0][1] = 0.0f;
	vtx[1][0] = float(m_width);
	vtx[1][1] = 0.0f;
	vtx[2][0] = 0.0f;
	vtx[2][1] = float(m_height);
	vtx[3][0] = float(m_width);
	vtx[3][1] = float(m_height);
	LoadVertexArray(vtx);

	mtlArray< mmlVector<2> > uv;
	uv.Create(4);
	uv[0][0] = 0.0f;
	uv[0][1] = 0.0f;
	uv[1][0] = 1.0f;
	uv[1][1] = 0.0f;
	uv[2][0] = 0.0f;
	uv[2][1] = 1.0f;
	uv[3][0] = 1.0f;
	uv[3][1] = 1.0f;
	LoadUVArray(uv);

	return true;
}

bool Image::IsColorKey(int x, int y) const
{
	if (!IsGood()) { return true; }
	return IsColorKey(*GetPixels(x, y));
}

bool Image::IsColorKey(Uint32 pixel) const
{
	Uint32 colorKey = SDL_MapRGBA(m_image->format, 255, 0, 255, 0);
	return pixel == colorKey;
}

const Uint32 *Image::GetPixels( void ) const
{
	return IsGood() ? (Uint32*)m_image->pixels : NULL;
}

const Uint32 *Image::GetPixels(int y) const
{
	return IsGood() ? ((Uint32*)(m_image->pixels) + y * m_image->w) : NULL;
}

const Uint32 *Image::GetPixels(int x, int y) const
{
	return IsGood() ? ((Uint32*)(m_image->pixels) + y * m_image->w + x) : NULL;
}

void Image::Draw(float) const
{
	DrawGraphics(0, 0, GL_TRIANGLE_STRIP, 2);
}
