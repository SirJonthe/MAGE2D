#ifndef FONT_H
#define FONT_H

#include <SDL/SDL.h>
#include "MTL/mtlString.h"

void DrawFont(const mtlChars &string, int x, int y, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255);
int GetFontWidth( void );
int GetFontHeight( void );

#endif // FONT_H
