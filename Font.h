#ifndef FONT_H
#define FONT_H

#include <GL/gl.h>
#include <SDL/SDL.h>
#include "Common.h"
#include "MTL/mtlString.h"

class Font
{
private:
	static GLuint m_tId, m_vId, m_uvId;
	static GLfloat m_vert[8];
	static GLfloat m_uv[8];
public:
	Font( void ); // if m_id == 0
	void DrawText(const mtlChars &text, float x, float y, float r = 1.0f, float g = 1.0f, float b =	1.0f, float a = 1.0f);
	int GetWidth( void ) const;
	int GetHeight( void ) const;
	Point GetSize(const mtlChars &text) const;
};

#endif // FONT_H
