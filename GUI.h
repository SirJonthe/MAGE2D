#ifndef FONT_H
#define FONT_H

#include <GL/gl.h>
#include <SDL/SDL.h>
#include "Common.h"
#include "MTL/mtlString.h"

namespace GUI
{

void Init( void );
void Destroy( void );

void SetIdentityView( void );

void SetColor(float r, float g, float b, float a = 1.0f);
void SetColor(mmlVector<3> color);
void SetColor(mmlVector<4> color);

void Text(const mtlChars &text, int x, int y, int scale = 1);
void Text(const mtlChars &text, float u, float v, int scale = 1);
void Box(Rect rect);
void Box(mmlVector<2> min, mmlVector<2> max);
//void Image(const Image &image, Rect rect);
//void Image(const Image &image, mmlVector<2> min, mmlVector<2> max);

Point GetTextSize(const mtlChars &text, int scale = 1);

}

/*class Font
{
private:
	static GLuint m_tId, m_vId, m_uvId;
	static GLfloat m_vert[8];
	static GLfloat m_uv[8];
public:
	Font( void ); // if m_id == 0
	void DrawText(const mtlChars &text, float x, float y, int textScale = 1, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	int GetWidth( void ) const;
	int GetHeight( void ) const;
	Point GetSize(const mtlChars &text, int textScale = 1) const;
};*/

#endif // FONT_H
