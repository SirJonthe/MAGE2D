#ifndef IMAGE_H
#define IMAGE_H

#include <GL/gl.h>
#include <SDL/SDL.h>
#include "MTL/mtlString.h"
#include "MTL/mtlType.h"
#include "Common.h"
#include "Timer.h"
#include "Graphics.h"

class Image : public mtlInherit<Graphics>
{
public:
	class Instance : public mtlInherit<Graphics::Instance>
	{
	private:
		Image *m_image;
	public:
		void SetAnimation(const mtlChars &name) {}
		void StartAnimation( void ) {}
		void StopAnimation( void ) {}
		void PauseAnimation( void ) {}
		void RestartAnimation( void ) {}

		void Update( void ) {}
		void Draw(Renderer *renderer);
	};
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
	void							Destroy( void );
	bool							Load(const mtlDirectory &file);
	bool							IsGood( void ) const;
	int								GetWidth( void ) const;
	int								GetHeight( void ) const;
	int								GetArea( void ) const;
	const SDL_Surface				*GetSurface( void ) const;
	bool							SetSurface(SDL_Surface *image);
	bool							IsColorKey(int x, int y) const;
	const Uint32					*GetPixels( void ) const;
	const Uint32					*GetPixels(int y) const;
	const Uint32					*GetPixels(int x, int y) const;
	void							Bind( void ) const;
	static void						Unbind( void );
	mtlShared<Graphics::Instance>	GetInstance( void ) const;
};

#endif // IMAGE_H
