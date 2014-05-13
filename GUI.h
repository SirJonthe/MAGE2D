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

void SetCaretXY(int x, int y);
void SetCaretX(int x);
void SetCaretY(int y);
void SetCaretUV(float u, float v);
void SetCaretU(float u);
void SetCaretV(float v);
Point GetCaretXY( void );
int GetCaretX( void );
int GetCaretY( void );
mmlVector<2> GetCaretUV( void );
float GetCaretU( void );
float GetCaretV( void );

void Text(const mtlChars &text, int scale = 1);
void Text(const mtlChars &text, int x, int y, int scale = 1);
void Text(const mtlChars &text, float u, float v, int scale = 1);
void Box(Rect rect);
void Box(mmlVector<2> min, mmlVector<2> max);
//void Image(const Image &image, Rect rect);
//void Image(const Image &image, mmlVector<2> min, mmlVector<2> max);

Point GetTextSize(const mtlChars &text, int scale = 1);

void GetCharsWidth(int scale = 1); // number of characters in width
void GetCharsHeight(int scale = 1); // number of characters in height

}

#endif // FONT_H
