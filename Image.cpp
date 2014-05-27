#include <cstdlib>
#include <iostream>
#include <GL/glext.h>
#include "MML/mmlMath.h"
#include "Image.h"
#include "Renderer.h"

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

GLuint Image::m_bound = 0;

void Image::Instance::Draw(Renderer *renderer)
{
	if (GetGraphics() == NULL) { return; }

	renderer->Render(image, GetTransform());

	// Renderer::Render(image, GetTransform(), vertex, uv);
}

Image::Image( void ) : m_image(NULL), m_width(0), m_height(0), m_id(0)
{
}

Image::Image(const mtlDirectory &file) : m_image(NULL), m_width(0), m_height(0), m_id(0)
{
	Load(file);
}

Image::~Image( void )
{
	Destroy();
}

void Image::Destroy( void )
{
	if (m_image != NULL) {
		SDL_FreeSurface(m_image);
		m_image = NULL;
	}
	m_width = 0;
	m_height = 0;
	if (m_bound == m_id) {
		Unbind();
	}
	if (m_id != 0) {
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
}

bool Image::Load(const mtlDirectory &file)
{
	std::cout << "Image::Load: " << file.GetDirectory().GetChars() << std::endl;

	Destroy();

	SDL_Surface *image = SDL_LoadBMP(file.GetDirectory().GetChars());
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

int Image::GetArea( void ) const
{
	return m_width*m_height;
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

	Uint32 *pixels = (Uint32*)m_image->pixels;
	Uint32 colorKey = SDL_MapRGBA(m_image->format, 255, 0, 255, 255);
	Uint32 alpha = colorKey & (~m_image->format->Amask);
	for (int i; i < m_image->w*m_image->h; ++i, ++pixels) {
		if ((*pixels | m_image->format->Amask) == colorKey) {
			*pixels = alpha;
		}
	}

	m_width = m_image->w;
	m_height = m_image->h;

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_image->w, (GLsizei)m_image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image->pixels);

	if (glGetError() != GL_NO_ERROR) {
		std::cout << "\tfailed: OpenGL error" << std::endl;
		SDL_FreeSurface(m_image);
		m_image = NULL;
		m_width	= 0;
		m_height = 0;
		m_id = 0;
		return false;
	}

	if (m_bound != 0) {
		glBindTexture(GL_TEXTURE_2D, m_bound);
	} else {
		Unbind();
	}

	return true;
}

bool Image::IsColorKey(int x, int y) const
{
	if (!IsGood()) { return true; }
	Uint32 color, colorKey;
	color = *GetPixels(x, y);
	colorKey = SDL_MapRGBA(m_image->format, 255, 0, 255, 0);
	return color == colorKey;
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

void Image::Bind( void ) const
{
	if (m_id != 0 && m_bound != m_id) {
		glBindTexture(GL_TEXTURE_2D, m_id);
		m_bound = m_id;
	}
}

void Image::Unbind( void )
{
	glBindTexture(GL_TEXTURE_2D, 0);
	m_bound = 0;
}

mtlShared<Graphics::Instance> Image::GetInstance( void ) const
{
	mtlShared<Graphics::Instance> instance = mtlShared<Graphics::Instance>::Create<Image>();
	instance.GetShared()->SetGraphics;
	return instance;
}
