#include "Platform.h"
#include "GUI.h"
#include <climits>
#include <iostream>

static GLuint tId = 0, vId = 0, uvId = 0;
//static GLfloat vert[8];
//static GLfloat uv[8];

int &CaretX( void ) { static int caret_x = 0; return caret_x; }
int &CaretY( void ) { static int caret_y = 0; return caret_y; }
int &Newl( void ) { static int newl = 0; return newl; }

// the range of valid characters (inclusive)
/*#define first_char '!'
#define last_char '~'

// size of the bitmap in pixels (1 pixel = 1 bit)
#define font_width 128
#define font_height 128

// the size of the bitmap width in bytes (use this to get bitmap pitch)
#define font_bitmap_stride (font_width / 8)

// number of characters in width/height on bitmap
#define char_count_width 12
#define char_count_height 8

// size of a single character in pixels (1 pixel = 1 bit)
#define char_px_width 10
#define char_px_height 16

// fractional size of a single character if width = 1.0 and height = 1.0
#define char_uv_width (float(char_px_width) / float(font_width))
#define char_uv_height (float(char_px_height) / float(font_height))

// size of a tab character (change this later to align tabs to vertical boundries)
#define char_tab_count 4
#define char_tab_px_width (char_px_width * char_tab_count)

static const unsigned char font_bits[] = {
   0xf8, 0x23, 0x1e, 0xf1, 0xd7, 0xc7, 0x07, 0x3f, 0xfe, 0xc1, 0xe0, 0xbf,
   0xff, 0xff, 0xff, 0xff, 0xf8, 0x23, 0x1e, 0x71, 0x80, 0xc7, 0x03, 0x3e,
   0xfe, 0xc0, 0xc0, 0x0f, 0xfe, 0xff, 0xff, 0xff, 0xf8, 0xef, 0x0e, 0x20,
   0x80, 0xc7, 0x73, 0x7e, 0xff, 0xc0, 0xc0, 0x1f, 0x7f, 0xfc, 0xff, 0xff,
   0xf8, 0xff, 0x0f, 0x20, 0xea, 0xff, 0x23, 0xfe, 0xff, 0xf8, 0xc7, 0x1f,
   0x7f, 0xfc, 0xff, 0xff, 0xf8, 0xff, 0x0f, 0x20, 0xc0, 0x00, 0x02, 0xff,
   0xff, 0xf8, 0xc7, 0xef, 0x1e, 0xf0, 0xff, 0xff, 0xf8, 0xff, 0x1f, 0x31,
   0x80, 0x00, 0x86, 0xf8, 0xff, 0xf8, 0xc7, 0xff, 0x1f, 0xf0, 0xff, 0xff,
   0xf8, 0xff, 0x0f, 0x60, 0x80, 0x00, 0x02, 0xfc, 0xff, 0xf8, 0xc7, 0xff,
   0x1f, 0xf0, 0xff, 0xff, 0xf8, 0xff, 0x0f, 0xe0, 0x8a, 0xff, 0x33, 0xfe,
   0xff, 0xf8, 0xc7, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x20,
   0x80, 0xc7, 0x73, 0xfc, 0xff, 0xc0, 0xc0, 0xff, 0x7f, 0x3c, 0xfe, 0xff,
   0xf8, 0xff, 0x1f, 0x31, 0xc0, 0xc7, 0x03, 0xf8, 0xff, 0xc0, 0xc0, 0xff,
   0xff, 0x3f, 0xfe, 0xff, 0xf8, 0xff, 0x1f, 0x71, 0xfd, 0xc7, 0x07, 0xf9,
   0xff, 0xc1, 0xe0, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x78, 0xc0, 0xc3, 0x03, 0x1c,
   0x30, 0x8e, 0x00, 0x06, 0x0c, 0x60, 0xc0, 0xff, 0xff, 0xff, 0xff, 0x38,
   0x80, 0xc1, 0x03, 0x08, 0x20, 0x8e, 0x00, 0x02, 0x0c, 0x20, 0x80, 0xff,
   0xff, 0xff, 0x7f, 0x3c, 0x80, 0xc1, 0x03, 0x08, 0x20, 0x8e, 0x00, 0x02,
   0x0c, 0x20, 0x80, 0xff, 0xff, 0xff, 0x7f, 0x3c, 0x8e, 0xc7, 0xff, 0x88,
   0x23, 0x8e, 0xf8, 0xe3, 0xff, 0x23, 0x8e, 0xff, 0x01, 0xff, 0x3f, 0x3e,
   0x86, 0xc7, 0x07, 0xf8, 0x21, 0x80, 0x00, 0x03, 0xfc, 0x23, 0x80, 0xff,
   0x01, 0xff, 0x3f, 0x3e, 0x8a, 0xc7, 0x03, 0xf8, 0x30, 0x80, 0x00, 0x02,
   0xf8, 0x71, 0xc0, 0xff, 0x01, 0xff, 0x1f, 0x3f, 0x8c, 0xc7, 0x03, 0xfc,
   0x61, 0x80, 0x00, 0x02, 0xf8, 0x31, 0x80, 0xff, 0xff, 0xff, 0x1f, 0x3f,
   0x8e, 0xc7, 0xe3, 0x8f, 0xe3, 0x8f, 0x3f, 0xe2, 0xf8, 0x31, 0x8e, 0xff,
   0xff, 0xe3, 0x8f, 0x3f, 0x80, 0x01, 0x03, 0x08, 0xe0, 0x8f, 0x00, 0x02,
   0xf8, 0x38, 0x80, 0xff, 0xff, 0xe3, 0x8f, 0x3f, 0x80, 0x01, 0x03, 0x08,
   0xe0, 0x8f, 0x00, 0x02, 0xf8, 0x38, 0x80, 0xff, 0xff, 0xe3, 0x8f, 0x7f,
   0xc0, 0x01, 0x03, 0x18, 0xf0, 0x8f, 0x00, 0x07, 0xfc, 0x78, 0xc0, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x1f, 0x70, 0xc0, 0x01, 0x03, 0x1c, 0x30, 0xc0, 0xff,
   0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x20, 0x80, 0x00, 0x02,
   0x08, 0x20, 0x80, 0xff, 0x00, 0xfe, 0xff, 0xff, 0xe3, 0x01, 0xc7, 0x0f,
   0x20, 0x9f, 0x00, 0x02, 0x08, 0x20, 0x80, 0xff, 0x38, 0xe2, 0x8f, 0xff,
   0xf1, 0x01, 0x8f, 0x8f, 0x23, 0x83, 0x38, 0xe2, 0x88, 0x23, 0x8e, 0xff,
   0x00, 0xe2, 0x8f, 0xff, 0xf8, 0x01, 0x1f, 0xff, 0x20, 0x81, 0x38, 0x02,
   0x88, 0x3f, 0x8e, 0xff, 0x00, 0xe2, 0x8f, 0x7f, 0xfc, 0xff, 0x3f, 0x7e,
   0x30, 0x99, 0x00, 0x02, 0x8c, 0x3f, 0x8e, 0xff, 0x01, 0xfe, 0xff, 0xff,
   0xf8, 0x01, 0x1f, 0x7f, 0x3c, 0x81, 0x00, 0x02, 0x88, 0x3f, 0x8e, 0xff,
   0x3f, 0xfe, 0xff, 0xff, 0xf1, 0x01, 0x8f, 0x7f, 0x3c, 0x83, 0x00, 0xe2,
   0x88, 0x23, 0x8e, 0xff, 0x01, 0xe2, 0x8f, 0xff, 0xe3, 0x01, 0xc7, 0xff,
   0x3f, 0xff, 0x38, 0x02, 0x08, 0x20, 0x80, 0xff, 0x01, 0xe2, 0x8f, 0xff,
   0xff, 0xff, 0xff, 0x7f, 0x3c, 0x80, 0x38, 0x02, 0x08, 0x20, 0x80, 0xff,
   0x01, 0xe3, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7c, 0xc0, 0x38, 0x02,
   0x1c, 0x30, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x00, 0x02, 0x18, 0x30, 0x8e, 0x83, 0x03, 0x88, 0x23, 0xfe, 0x38, 0xe2,
   0x18, 0x30, 0xc0, 0xff, 0x00, 0x02, 0x08, 0x20, 0x8e, 0x83, 0x03, 0x88,
   0x23, 0xfe, 0x10, 0xe2, 0x08, 0x20, 0x80, 0xff, 0x00, 0x02, 0x08, 0x20,
   0x8e, 0x83, 0x03, 0x88, 0x23, 0xfe, 0x00, 0xe2, 0x08, 0x20, 0x80, 0xff,
   0xf8, 0xe3, 0x8f, 0x23, 0x8e, 0xc7, 0xff, 0x88, 0x23, 0xfe, 0x00, 0xc2,
   0x88, 0x23, 0x8e, 0xff, 0xc0, 0x03, 0x8f, 0x3f, 0x80, 0xc7, 0xff, 0x08,
   0x20, 0xfe, 0x00, 0x82, 0x88, 0x23, 0x8e, 0xff, 0xc0, 0x03, 0x8f, 0x20,
   0x80, 0xc7, 0xff, 0x08, 0x30, 0xfe, 0x28, 0x02, 0x88, 0x23, 0x80, 0xff,
   0xc0, 0x03, 0x8f, 0x20, 0x80, 0xc7, 0xe3, 0x08, 0x20, 0xfe, 0x38, 0x22,
   0x88, 0x23, 0x80, 0xff, 0xf8, 0xe3, 0x8f, 0x23, 0x8e, 0xc7, 0xe3, 0x88,
   0x23, 0xfe, 0x38, 0x62, 0x88, 0x23, 0xc0, 0xff, 0x00, 0xe2, 0x0f, 0x20,
   0x8e, 0x83, 0x03, 0x88, 0x23, 0x80, 0x38, 0xe2, 0x08, 0x20, 0xfe, 0xff,
   0x00, 0xe2, 0x0f, 0x20, 0x8e, 0x83, 0x03, 0x88, 0x23, 0x80, 0x38, 0xe2,
   0x08, 0x20, 0xfe, 0xff, 0x00, 0xe2, 0x1f, 0x30, 0x8e, 0x83, 0x07, 0x8c,
   0x23, 0x80, 0x38, 0xe2, 0x18, 0x30, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x01, 0x03, 0x1c, 0x30, 0x80, 0x38, 0xe2, 0x88,
   0x23, 0x8e, 0x38, 0x02, 0x38, 0x30, 0xfe, 0xff, 0x00, 0x02, 0x08, 0x20,
   0x80, 0x38, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0x02, 0x38, 0x30, 0xfe, 0xff,
   0x00, 0x02, 0x08, 0x20, 0x80, 0x38, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0x02,
   0x38, 0x70, 0xfc, 0xff, 0x38, 0xe2, 0x88, 0xff, 0xf1, 0x38, 0xe2, 0x88,
   0x23, 0x84, 0x38, 0xfe, 0x38, 0x7e, 0xfc, 0xff, 0x38, 0xe2, 0x08, 0xf0,
   0xf1, 0x38, 0xe2, 0x88, 0x23, 0x80, 0x00, 0x0e, 0x38, 0xfe, 0xf8, 0xff,
   0x38, 0x02, 0x08, 0xe0, 0xf1, 0x38, 0xe2, 0x88, 0x62, 0xc0, 0x00, 0x06,
   0x3c, 0xfe, 0xf8, 0xff, 0x08, 0x02, 0x1c, 0xe0, 0xf1, 0x38, 0xe2, 0x08,
   0x20, 0x80, 0x01, 0x03, 0x3e, 0xfe, 0xf1, 0xff, 0x08, 0x02, 0xf8, 0xe3,
   0xf1, 0x38, 0x42, 0x08, 0x20, 0x84, 0xc7, 0xe3, 0x3f, 0xfe, 0xf1, 0xff,
   0x00, 0xe2, 0x08, 0xe0, 0xf1, 0x00, 0x06, 0x0c, 0x20, 0x8e, 0xc7, 0x03,
   0x38, 0xf0, 0xe3, 0xff, 0x00, 0xe2, 0x08, 0xe0, 0xf1, 0x00, 0x0e, 0x0e,
   0x21, 0x8e, 0xc7, 0x03, 0x38, 0xf0, 0xe3, 0xff, 0x01, 0xe2, 0x18, 0xf0,
   0xf1, 0x01, 0x1f, 0x8f, 0x23, 0x8e, 0xc7, 0x03, 0x38, 0xf0, 0xe3, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xbf, 0xff, 0xff,
   0xf8, 0xff, 0xe3, 0xff, 0xff, 0x8f, 0xff, 0x0f, 0xfc, 0x3f, 0xfe, 0xff,
   0xc0, 0x1f, 0xff, 0xff, 0xf8, 0xff, 0xe3, 0xff, 0xff, 0x8f, 0xff, 0x07,
   0xfc, 0x3f, 0xfe, 0xff, 0xc0, 0x0f, 0xfe, 0xff, 0xfb, 0xff, 0xe3, 0xff,
   0xff, 0x8f, 0xff, 0x07, 0xfc, 0x3f, 0xfe, 0xff, 0xc7, 0x4f, 0xfe, 0xff,
   0xff, 0x01, 0x03, 0x1c, 0x60, 0x80, 0x01, 0xc7, 0x1f, 0x20, 0xc0, 0xff,
   0xc7, 0xef, 0xfe, 0xff, 0xff, 0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0x06,
   0x0f, 0x20, 0x80, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0x00, 0x02, 0x08,
   0x20, 0x80, 0x38, 0x06, 0x0f, 0x20, 0x80, 0xff, 0xc7, 0xff, 0xff, 0xff,
   0xff, 0x38, 0xe2, 0x88, 0x3f, 0x8e, 0x00, 0x06, 0x8f, 0x23, 0x8e, 0xff,
   0xc7, 0xff, 0xff, 0xff, 0xff, 0x38, 0xe2, 0x88, 0x3f, 0x8e, 0xf8, 0xc7,
   0x8f, 0x23, 0x8e, 0xff, 0xc0, 0xff, 0x0f, 0xe0, 0xff, 0x00, 0x02, 0x08,
   0x20, 0x80, 0x00, 0xc6, 0x0f, 0x20, 0x8e, 0xff, 0xc0, 0xff, 0x0f, 0xe0,
   0xff, 0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0xc6, 0x0f, 0x20, 0x8e, 0xff,
   0xc0, 0xff, 0x0f, 0xe0, 0xff, 0x01, 0x02, 0x1c, 0x60, 0x80, 0x01, 0xc6,
   0x1f, 0x20, 0x8e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xe0, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x0f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x8f, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0x8f, 0x7f, 0xfc, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0x8f, 0x63,
   0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xff,
   0xc7, 0x3f, 0x8e, 0x61, 0xfc, 0x00, 0x03, 0x1c, 0x30, 0xc0, 0x01, 0x46,
   0x3c, 0x78, 0xfc, 0xff, 0xc7, 0x3f, 0x8e, 0x70, 0xfc, 0x00, 0x02, 0x08,
   0x20, 0x80, 0x00, 0x06, 0x1c, 0x70, 0xf0, 0xff, 0xff, 0xff, 0x0f, 0x78,
   0xfc, 0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0x06, 0x9c, 0x7f, 0xf0, 0xff,
   0xc7, 0x3f, 0x0e, 0x7c, 0xfc, 0x28, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0x86,
   0x1f, 0x78, 0xf0, 0xff, 0xc7, 0x3f, 0x0e, 0x78, 0xfc, 0x38, 0xe2, 0x88,
   0x23, 0x8e, 0x38, 0xc6, 0x3f, 0x70, 0xfc, 0xff, 0xc7, 0x3f, 0x8e, 0x70,
   0xe0, 0x38, 0xe2, 0x08, 0x20, 0x80, 0x00, 0xc6, 0xff, 0x73, 0xe0, 0xff,
   0x83, 0x3f, 0x8e, 0x61, 0xe0, 0x38, 0xe2, 0x08, 0x20, 0x80, 0x00, 0xc6,
   0x1f, 0x70, 0xe0, 0xff, 0x83, 0x3f, 0x8e, 0xe3, 0xe0, 0x38, 0xe2, 0x18,
   0x30, 0xc0, 0x01, 0xc6, 0x3f, 0xf8, 0xe0, 0xff, 0xff, 0x23, 0xfe, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0x3f, 0xfe,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x3f, 0xfe, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
   0xf0, 0xf8, 0xe0, 0x5f, 0x0f, 0x20, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x3f, 0xf0, 0xf8, 0xc0, 0x0f, 0xef, 0xaf, 0xbf, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xf0, 0xf8, 0xc0, 0x0f,
   0xef, 0xaf, 0xbf, 0xff, 0x38, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0x02, 0x38,
   0xfe, 0xf8, 0xc7, 0xaf, 0xef, 0xaf, 0xbf, 0xff, 0x38, 0xe2, 0x88, 0x23,
   0x8e, 0x38, 0x02, 0x78, 0xfe, 0xf8, 0xe7, 0xff, 0xef, 0xaf, 0xbf, 0xff,
   0x38, 0xe2, 0x88, 0x63, 0xc4, 0x38, 0x7e, 0x38, 0xff, 0xf8, 0xcf, 0xff,
   0xef, 0xaf, 0xbf, 0xff, 0x38, 0xe2, 0x88, 0xe3, 0xe0, 0x38, 0x1e, 0x7c,
   0xfe, 0xf8, 0xe7, 0xff, 0xef, 0xaf, 0xbf, 0xff, 0x38, 0x42, 0x88, 0xe2,
   0xe0, 0x38, 0x06, 0x3f, 0xfe, 0xf8, 0xc7, 0xff, 0xef, 0xaf, 0xbf, 0xff,
   0x00, 0x06, 0x0c, 0x60, 0xc4, 0x00, 0xc2, 0x3f, 0xf0, 0xf8, 0xc0, 0xff,
   0xef, 0xaf, 0xbf, 0xff, 0x00, 0x0e, 0x0e, 0x20, 0x8e, 0x00, 0x02, 0x38,
   0xf0, 0xf8, 0xc0, 0xff, 0xef, 0xaf, 0xbf, 0xff, 0x01, 0x1e, 0x1f, 0x30,
   0x8e, 0x01, 0x02, 0x78, 0xf0, 0xf8, 0xe0, 0xff, 0x0f, 0x20, 0x80, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xfe, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};*/

// the range of valid characters (inclusive)
#define first_char '!'
#define last_char '~'

// size of the bitmap in pixels (1 pixel = 1 bit)
#define font_width 64
#define font_height 64

// the size of the bitmap width in bytes (use this to get bitmap pitch)
#define font_bitmap_stride (font_width / 8)

// number of characters in width/height on bitmap
#define char_count_width 10
#define char_count_height 10

// size of a single character in pixels (1 pixel = 1 bit)
#define char_px_width 6
#define char_px_height 6

// fractional size of a single character if width = 1.0 and height = 1.0
#define char_uv_width (float(char_px_width) / float(font_width))
#define char_uv_height (float(char_px_height) / float(font_height))

// size of a tab character (change this later to align tabs to vertical boundries)
#define char_tab_count 4
#define char_tab_px_width (char_px_width * char_tab_count)

static unsigned char font_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7d, 0xbd, 0x06, 0x77,
   0xdc, 0xef, 0xfd, 0xff, 0x7d, 0x1d, 0xa4, 0xff, 0xde, 0xf7, 0x7b, 0xfd,
   0xfd, 0xbf, 0x06, 0x41, 0xf5, 0xf7, 0xfb, 0xfe, 0xff, 0x1f, 0x2c, 0x7f,
   0xfb, 0xf7, 0x7b, 0xfd, 0xfd, 0xbf, 0x06, 0x77, 0xf4, 0xef, 0xfd, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5f,
   0x70, 0x07, 0x41, 0xf7, 0xfb, 0xff, 0xff, 0x6f, 0x37, 0x7f, 0x5f, 0xf7,
   0xf1, 0x1f, 0xff, 0x77, 0x75, 0x07, 0x43, 0xf0, 0xfb, 0xfe, 0xff, 0x7b,
   0x77, 0xf7, 0xdf, 0xf7, 0x7f, 0xff, 0xf7, 0x7d, 0x10, 0x04, 0xc1, 0xf7,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x41, 0x10, 0x04, 0xc1,
   0xff, 0xdf, 0x7f, 0xff, 0x7d, 0xff, 0x75, 0x5d, 0xbf, 0xef, 0xf1, 0xfe,
   0x41, 0xf0, 0x04, 0xc1, 0xff, 0xf7, 0xff, 0xfd, 0x5f, 0xf7, 0x75, 0x5f,
   0xbf, 0xef, 0xf1, 0xfe, 0x41, 0xf0, 0x05, 0xdf, 0xdf, 0xdf, 0x7f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x41, 0x10, 0x84, 0x41,
   0x18, 0x04, 0x41, 0xf7, 0x5f, 0xd7, 0xb5, 0x7d, 0xd7, 0xf7, 0x7d, 0xf7,
   0x41, 0x13, 0x04, 0x7d, 0x17, 0x86, 0x4d, 0xf0, 0x7f, 0xd5, 0x75, 0x7d,
   0xd7, 0xf7, 0x5d, 0xf7, 0x7d, 0xd1, 0x05, 0x41, 0x10, 0xf4, 0x41, 0xf7,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x41, 0xd0, 0xf5, 0x49,
   0x17, 0x04, 0x41, 0xf0, 0xf7, 0xd7, 0xf5, 0x55, 0xd6, 0x75, 0x5d, 0xf7,
   0xf7, 0x17, 0xf6, 0x5d, 0xd5, 0x05, 0x5d, 0xf0, 0x77, 0xd7, 0xf5, 0x5d,
   0xd3, 0xf5, 0x4d, 0xfb, 0x41, 0xd0, 0x05, 0x5d, 0x17, 0xf4, 0x41, 0xf7,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x43, 0xd0, 0x75, 0x5d,
   0xd7, 0x05, 0x79, 0xff, 0xfd, 0xdd, 0x75, 0x5d, 0xd7, 0x7d, 0xfd, 0xfe,
   0xc1, 0xdd, 0x75, 0xdd, 0xb8, 0x8e, 0xfd, 0xfd, 0xdf, 0xdd, 0xad, 0x55,
   0x77, 0xf7, 0xfd, 0xfb, 0xe1, 0x1d, 0xdc, 0x49, 0x77, 0x07, 0xf9, 0xf7,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xfe, 0xf7, 0x7f,
   0xff, 0x7f, 0xff, 0xf9, 0x7b, 0xfd, 0xef, 0x43, 0x3f, 0x7c, 0xe3, 0xfe,
   0xfb, 0xff, 0xff, 0x5d, 0xd8, 0x0f, 0xc1, 0xfc, 0xfb, 0xff, 0xff, 0x5d,
   0xd7, 0x77, 0xfd, 0xfe, 0xf9, 0x1f, 0xfc, 0x43, 0x10, 0x04, 0xc3, 0xfe,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x43, 0x7f, 0x7f, 0xfd,
   0xfe, 0xff, 0xff, 0xff, 0x5d, 0xff, 0xff, 0xdd, 0x1e, 0x86, 0x63, 0xf8,
   0x43, 0x38, 0x7f, 0xe1, 0x5e, 0x75, 0x5d, 0xf7, 0x5f, 0x77, 0x77, 0xdd,
   0xde, 0x75, 0x5d, 0xf0, 0x63, 0x37, 0x86, 0xdd, 0xd9, 0x75, 0x63, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x43, 0x31, 0xcc, 0x5d, 0xd7, 0x75, 0x5d, 0xf0,
   0x5d, 0x9e, 0xef, 0x5d, 0xd7, 0x8d, 0xc1, 0xfb, 0x41, 0x7f, 0xec, 0xdd,
   0x5a, 0x75, 0xdf, 0xfc, 0x5f, 0x1f, 0x9e, 0xe3, 0x9d, 0x76, 0x43, 0xf0,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x73, 0x9f, 0xaf, 0x41,
   0x10, 0x04, 0x41, 0xf0, 0x7b, 0xbf, 0xd7, 0x41, 0x10, 0x04, 0x41, 0xf0,
   0x7d, 0x7f, 0xff, 0x41, 0x10, 0x04, 0x41, 0xf0, 0x7b, 0xbf, 0xff, 0x41,
   0x10, 0x04, 0x41, 0xf0, 0x73, 0x9f, 0xff, 0x41, 0x10, 0x04, 0x41, 0xf0,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void GUI::Init( void )
{
	if (tId == 0) {
		glGenBuffers(1, &vId);
		glGenBuffers(1, &uvId);

		unsigned int *pixels = new unsigned int[font_width*font_height];

		// every row is aligned (16 bytes * 8 bits) * 128 bytes
		for (int y = 0, i = 0; y < font_height; ++y) {
			for (int x = 0; x < font_width; ++x) {
				unsigned char byte = font_bits[y * font_bitmap_stride + x / 8];
				unsigned char bit = byte & (1 << (x % 8));
				if (bit != 0) { // inverted
					pixels[i] = 0x00000000;
				} else {
					pixels[i] = 0xffffffff;
				}
				++i;
			}
		}

		glGenTextures(1, &tId);
		glBindTexture(GL_TEXTURE_2D, tId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)font_width, (GLsizei)font_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		delete [] pixels;
	}
}

void GUI::Destroy( void )
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_2D, 0);

	glDeleteBuffers(1, &vId);
	glDeleteBuffers(1, &uvId);
	glDeleteTextures(1, &tId);
}

/*void GUI::SetGUIView( void )
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, (SDL_GetVideoSurface()->w - 1) + 0.5, (SDL_GetVideoSurface()->h - 1) + 0.5, -0.5, 0.0, 1.0);
}*/

void GUI::SetColor(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void GUI::SetColor(mmlVector<3> color)
{
	glColor3f(color[0], color[1], color[2]);
}

void GUI::SetColor(mmlVector<4> color)
{
	glColor4f(color[0], color[1], color[2], color[3]);
}

int GUI::U_To_X(float u)
{
	return int(u * (SDL_GetVideoSurface()->w - 1));
}

int GUI::V_To_Y(float v)
{
	return int(v * (SDL_GetVideoSurface()->h - 1));
}

float GUI::X_To_U(int x)
{
	return float(x) / float(SDL_GetVideoSurface()->w - 1);
}

float GUI::Y_To_V(int y)
{
	return float(y) / float(SDL_GetVideoSurface()->h - 1);
}

void GUI::SetCaretXY(int x, int y)
{
	CaretX() = x;
	CaretY() = y;
}

void GUI::SetCaretX(int x)
{
	CaretX() = x;
}

void GUI::SetCaretY(int y)
{
	CaretY() = y;
}

void GUI::SetCaretUV(float u, float v)
{
	CaretX() = U_To_X(u);
	CaretY() = V_To_Y(v);
}

void GUI::SetCaretU(float u)
{
	CaretX() = U_To_X(u);
}

void GUI::SetCaretV(float v)
{
	CaretY() = V_To_Y(v);
}

Point GUI::GetCaretXY( void )
{
	Point c = { CaretX(), CaretY() };
	return c;
}

int GUI::GetCaretX( void )
{
	return CaretX();
}

int GUI::GetCaretY( void )
{
	return CaretY();
}

mmlVector<2> GUI::GetCaretUV( void )
{
	return mmlVector<2>(GetCaretU(), GetCaretV());
}

float GUI::GetCaretU( void )
{
	return X_To_U(GetCaretX());
}

float GUI::GetCaretV( void )
{
	return Y_To_V(GetCaretY());
}

void GUI::NewLine(int scale)
{
	CaretX() = 0;
	CaretY() += Newl();
	Newl() = char_px_height * mmlMax(scale, 1);
}

void GUI::Print(const mtlChars &text, int scale)
{
	if (text.GetSize() == 0) { return; }

	GLfloat vert[8];
	GLfloat uv[8];

	scale = mmlMax(1, scale);
	Newl() = mmlMax(Newl(), char_px_height * scale);

	glBindBuffer(GL_ARRAY_BUFFER, uvId);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, vId);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	glBindTexture(GL_TEXTURE_2D, tId);

#ifdef __linux__
	float ox = 0.5f / (float)SDL_GetVideoSurface()->w;
	float oy = 0.5f / (float)SDL_GetVideoSurface()->h; // take into account that OpenGL samples from the middle of the pixel, not the edge
#else
	float ox = 0.0f;
	float oy = 0.0f;
#endif
	for (int i = 0; i < text.GetSize(); ++i) {
		char ch = text.GetChars()[i];
		switch (ch) {
		case '\n':
		case '\r':
			SetCaretX(0);
			ox = 0.0f;
			oy += char_px_height * scale;
			continue;
		case ' ':
			ox += char_px_width * scale;
			continue;
		case '\t':
			ox += char_tab_px_width * scale;
			continue;
		default: break;
		}

		vert[0] = CaretX() + ox;
		vert[1] = CaretY() + oy + char_px_height * scale;

		vert[2] = CaretX() + ox + char_px_width * scale;
		vert[3] = CaretY() + oy + char_px_height * scale;

		vert[4] = CaretX() + ox + char_px_width * scale;
		vert[5] = CaretY() + oy;

		vert[6] = CaretX() + ox;
		vert[7] = CaretY() + oy;

		glBindBuffer(GL_ARRAY_BUFFER, vId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

		if (ch < first_char || ch > last_char) {
			ch = last_char + 1;
		}
		int index = ch - first_char;
#ifdef __APPLE__
		float ux = /*(0.5f / (float)font_width) +*/ char_uv_width * (index % char_count_width);
		float uy = (-0.5f / (float)font_height) + (char_uv_height * (index / char_count_width));
#else
		float ux = char_uv_width * (index % char_count_width);
		float uy = (char_uv_height * (index / char_count_width)); // should technically sample from the middle, but that breaks font
#endif
		uv[0] = ux;
		uv[1] = uy + char_uv_height;

		uv[2] = ux + char_uv_width;
		uv[3] = uy + char_uv_height;

		uv[4] = ux + char_uv_width;
		uv[5] = uy;

		uv[6] = ux;
		uv[7] = uy;

		glBindBuffer(GL_ARRAY_BUFFER, uvId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

		glDrawArrays(GL_QUADS, 0, 4);

		ox += char_px_width * scale;
	}
	CaretX() += int(ox);
	CaretY() += int(oy);
}

void GUI::Print(char ch, int scale)
{
	Print(mtlChars::FromDynamic(&ch, 0, 1), scale);
}

void GUI::Print(int number, int scale)
{
	mtlString text;
	text.FromInt(number);
	Print(text, scale);
}

void GUI::Print(float number, int scale)
{
	mtlString text;
	text.FromFloat(number);
	Print(text, scale);
}

void GUI::Print(Point p, int scale)
{
	GUI::Print(p.x, scale);
	GUI::Print(", ", scale);
	GUI::Print(p.y, scale);
}

void GUI::Print(Rect r, int scale)
{
	GUI::Print(r.x, scale);
	GUI::Print(", ", scale);
	GUI::Print(r.y, scale);
	GUI::Print("; ", scale);
	GUI::Print(r.w, scale);
	GUI::Print(", ", scale);
	GUI::Print(r.h, scale);
}

void GUI::Box(Rect rect)
{
	// Engine enables texture coord array, but box rendering does not use texture coordinate array.
	// Disable to avoid crash.
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	GLfloat vert[8];

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vId);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	vert[0] = rect.x;
	vert[1] = rect.y;

	vert[2] = rect.x + rect.w;
	vert[3] = rect.y;

	vert[4] = rect.x + rect.w;
	vert[5] = rect.y + rect.h;

	vert[6] = rect.x;
	vert[7] = rect.y + rect.h;

	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glDrawArrays(GL_QUADS, 0, 4);

	// Re-enable texture coord array for subsequent rendering.
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Newl() = mmlMax(Newl(), rect.h);
}

void GUI::Box(mmlVector<2> min, mmlVector<2> max)
{
	Rect r;
	r.x = (int)min[0];
	r.y = (int)min[1];
	r.w = (int)(max[0] - min[0]);
	r.h = (int)(max[1] - min[1]);
	GUI::Box(r);
}

Point GUI::GetTextSize(const mtlChars &text, int scale)
{
	Point p = { 0, 0 };
	if (text.GetSize() > 0) {
		int x = 0;
		p.y += char_px_height;
		for (int i = 0; i < text.GetSize(); ++i) {
			char ch = text.GetChars()[0];
			if (ch == '\n' || ch == '\r') {
				x = 0;
				p.y += char_px_height;
			} else if (ch == '\t') {
				x += char_tab_px_width;
			} else {
				x += char_px_width;
			}
			p.x = mmlMax(x, p.x);
		}
	}
	p.x *= scale;
	p.y *= scale;
	return p;
}

Point GUI::GetTextSize(int number, int scale)
{
	mtlString num;
	num.FromInt(number);
	return GetTextSize(num, scale);
}

Point GUI::GetTextSize(float number, int scale)
{
	mtlString num;
	num.FromFloat(number);
	return GetTextSize(num, scale);
}

int GUI::GetCharPixelWidth(int scale)
{
	return char_px_width * scale;
}

int GUI::GetCharPixelHeight(int scale)
{
	return char_px_height * scale;
}

GUI::GraphicsRect::GraphicsRect( void ) : m_color(1.0f, 1.0f, 1.0f, 1.0f), m_id(0)
{
	glGenBuffers(1, &m_id);
	if (m_id == 0) {
		std::cout << "GraphicsRect2: Failed to generate vertex array" << std::endl;
	}
	SetBounds(0, 0, 0, 0);
}

GUI::GraphicsRect::GraphicsRect(int x, int y, int w, int h) : m_color(1.0f, 1.0f, 1.0f, 1.0f), m_id(0)
{
	glGenBuffers(1, &m_id);
	if (m_id == 0) {
		std::cout << "GraphicsRect2: Failed to generate vertex array" << std::endl;
	}
	SetBounds(x, y, w, h);
}

GUI::GraphicsRect::~GraphicsRect( void )
{
	glDeleteBuffers(1, &m_id);
}

void GUI::GraphicsRect::SetBounds(int x, int y, int w, int h)
{
	m_rect.x = x;
	m_rect.y = y;
	m_rect.w = w;
	m_rect.h = h;

	mmlVector<2> xy[4] = {
		mmlVector<2>((float)x, (float)y),
		mmlVector<2>((float)(x+w), (float)y),
		mmlVector<2>((float)(x+w), (float)(y+h)),
		mmlVector<2>((float)x, (float)(y+h))
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(mmlVector<2>), (GLvoid*)&(xy[0][0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GUI::GraphicsRect::SetColor(float r, float g, float b, float a)
{
	m_color = mmlVector<4>(r, g, b, a);
	m_color.Clamp(mmlVector<4>(0.0f, 0.0f, 0.0f, 0.0f), mmlVector<4>(1.0f, 1.0f, 1.0f, 1.0f));
}

void GUI::GraphicsRect::DrawLines( void ) const
{
	glColor4f(m_color[0], m_color[1], m_color[2], m_color[3]);

	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GUI::GraphicsRect::DrawFilled( void ) const
{
	glColor4f(m_color[0], m_color[1], m_color[2], m_color[3]);

	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Rect GUI::GraphicsRect::GetRect( void ) const
{
	return m_rect;
}

/*GUI::ContentRect GUI::Control::ClipRect(GUI::ContentRect rect) const
{
	int x1 = m_rect.x, y1 = m_rect.y;
	int x2 = x1 + m_rect.w, y2 = y1 + m_rect.h;

	GUI::ContentRect r;
	r.x = m_rect.x;
	r.y = m_rect.y;
	r.w = m_rect.w;
	r.h = m_rect.h;

	r.clip_x = mmlMax(x1, rect.x);
	r.clip_y = mmlMax(y1, rect.y);
	r.clip_w = mmlMin(x1 + x2, rect.w);
	r.clip_h = mmlMin(y1 + y2, rect.h);

	return r;
}*/

void GUI::Control::DrawChildren(Rect clip) const
{
	clip = Clip(clip, m_rect.GetRect());
	const mtlItem< mtlShared<GUI::Control> > *c = m_children.GetFirst();
	while (c != NULL) {
		glScissor(clip.x, clip.y, clip.w, clip.h);
		c->GetItem()->Draw(clip);
		c = c->GetNext();
	}
}

Rect GUI::Control::Clip(Rect a, Rect b)
{
	Rect r;
	r.x = mmlMax(a.x, b.x);
	r.y = mmlMax(a.y, b.y);
	int ax2 = a.x + a.w;
	int ay2 = a.y + a.h;
	int bx2 = b.x + b.w;
	int by2 = b.y + b.h;
	r.w = mmlMax(mmlMin(ax2, bx2) - r.x, 0);
	r.h = mmlMax(mmlMin(ay2, by2) - r.y, 0);
	return r;
}

GUI::Control::Control( void ) :
	mtlBase(this),
	m_manager(NULL), m_parent(NULL),
	m_children(),
	m_rect(),
	m_name(),
	m_textScale(1),
	m_hasFocus(false), m_visible(true), m_locked(false)
{
}

GUI::Control::~Control( void )
{
}

void GUI::Control::SetPositionXY(int x, int y)
{
	m_rect.SetBounds(x, y, m_rect.GetRect().w, m_rect.GetRect().h);
}

void GUI::Control::SetPositionUV(float u, float v)
{
	m_rect.SetBounds(U_To_X(u), V_To_Y(v), m_rect.GetRect().w, m_rect.GetRect().h);
}

void GUI::Control::SetDimensionsXY(int w, int h)
{
	m_rect.SetBounds(m_rect.GetRect().x, m_rect.GetRect().y, w, h);
}

void GUI::Control::SetDimensionsUV(float u, float v)
{
	m_rect.SetBounds(m_rect.GetRect().x, m_rect.GetRect().y, U_To_X(u), V_To_Y(v));
}

void GUI::Control::Lock( void )
{
	m_locked = true;
}

void GUI::Control::Unlock( void )
{
	m_locked = false;
}

void GUI::Control::Focus( void )
{
	if (m_manager != NULL) {

		m_hasFocus = true;
	}
}

void GUI::Control::Unfocus( void )
{
	m_hasFocus = false;
}

void GUI::Control::Hide( void )
{
	m_visible = false;
	Unfocus();
}

void GUI::Control::Show( void )
{
	m_visible = true;
}

void GUI::Control::Draw(Rect clip) const
{
	OnDraw();
	DrawChildren(clip);
}

void GUI::Control::Update( void )
{
	OnUpdate();
	mtlItem< mtlShared<GUI::Control> > *control = m_children.GetFirst();
	while (control != NULL) {
		control->GetItem()->Update();
		control = control->GetNext();
	}
}

void GUI::Control::Init( void )
{
	OnInit();
	mtlItem< mtlShared<GUI::Control> > *control = m_children.GetFirst();
	while (control != NULL) {
		control->GetItem()->Init();
		control = control->GetNext();
	}
}

void GUI::Control::Destroy( void )
{
	mtlItem< mtlShared<GUI::Control> > *control = m_children.GetFirst();
	while (control != NULL) {
		control->GetItem()->Destroy();
		control = control->GetNext();
	}
	OnDestroy();
}

void GUI::Label::OnDraw( void ) const
{
	GUI::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	GUI::SetCaretXY(0, 0);
	GUI::Print(m_text);
}

const mtlString &GUI::Label::GetLabel( void ) const
{
	return m_text;
}

void GUI::Label::SetLabel(const mtlChars &text)
{
	m_text.Copy(text);
	Point content = GUI::GetTextSize(m_text, 1);
	SetDimensionsXY(content.x, content.y);
}

void GUI::Form::OnDraw( void ) const
{
	GUI::SetColor(0.0f, 0.0f, 0.0f, 0.5f);
	GUI::Box(m_rect.GetRect());
}

GUI::Form::Form( void ) : mtlInherit<GUI::Control, GUI::Form>(this)
{
	SetDimensionsXY(100, 70);
	m_theme.blur = 0.0f;
	m_theme.paddingX = 0;
	m_theme.paddingY = 0;
	m_theme.textScale = 1;
	m_theme.fontColor = mmlVector<4>(1.0f, 1.0f, 1.0f, 1.0f);
	m_theme.addColor = mmlVector<4>(0.0f, 0.0f, 0.0f, 0.0f);
	m_theme.mulColor = mmlVector<4>(1.0f, 1.0f, 1.0f, 1.0f);
}

void GUI::Manager::HandleInput( void )
{
	mtlItem< mtlShared<GUI::Form> > *n = m_forms.GetFirst();
	while (n != NULL) {
		n = n->GetNext();
	}
}

void GUI::Manager::DrawForms( void ) const
{
	glEnable(GL_SCISSOR_TEST);

	Rect clip;
	clip.x = clip.y = 0;
	clip.w = SDL_GetVideoSurface()->w;
	clip.h = SDL_GetVideoSurface()->h;

	const mtlItem< mtlShared<GUI::Form> > *form = m_forms.GetFirst();
	while (form != NULL) {
		form->GetItem()->DrawChildren(clip);
		form = form->GetNext();
	}

	glDisable(GL_SCISSOR_TEST);
}

GUI::Manager::Manager( void ) : m_forms(), m_focus(), m_color(1.0f, 1.0f, 1.0f, 1.0f), m_textCaretX(0), m_textCaretY(0), m_newlineHeight(char_px_height)
{}

GUI::Manager::~Manager( void )
{
}

void GUI::Manager::Init( void )
{
}

void GUI::Manager::Destroy( void )
{
	m_focus.Delete();
	m_forms.RemoveAll();
}

void GUI::Manager::SetTextColor(float r, float g, float b, float a)
{
	m_color[0] = r;
	m_color[1] = g;
	m_color[2] = b;
	m_color[3] = a;
}

void GUI::Manager::SetTextColor(mmlVector<4> color)
{
	m_color = color;
}

void GUI::Manager::SetTextColor(mmlVector<3> color)
{
	m_color[0] = color[0];
	m_color[1] = color[1];
	m_color[2] = color[2];
	m_color[3] = 1.0f;
}

void GUI::Manager::Print(const mtlChars &text, int scale)
{
	if (text.GetSize() == 0) { return; }

	GLfloat vert[8];
	GLfloat uv[8];

	scale = mmlMax(1, scale);
	m_newlineHeight = mmlMax(m_newlineHeight, char_px_height * scale);

	glBindBuffer(GL_ARRAY_BUFFER, uvId);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, vId);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	glBindTexture(GL_TEXTURE_2D, tId);

	float ox = 0.0f;
	float oy = 0.0f;
	for (int i = 0; i < text.GetSize(); ++i) {
		char ch = text.GetChars()[i];
		if ((int)(m_textCaretX + ox + char_px_width * scale) >= (int)SDL_GetVideoSurface()->w) {
			ch = '\n';
			--i;
		}
		switch (ch) {
		case '\n':
		case '\r':
			ox = 0.0f;
			oy += char_px_height * scale;
			continue;
		case ' ':
			ox += char_px_width * scale;
			continue;
		case '\t': // fix so that tabs are at fixed intervals
			ox += char_tab_px_width * scale;
			continue;
		default: break;
		}

		vert[0] = m_textCaretX + ox;
		vert[1] = m_textCaretY + oy + char_px_height * scale;

		vert[2] = m_textCaretX + ox + char_px_width * scale;
		vert[3] = m_textCaretY + oy + char_px_height * scale;

		vert[4] = m_textCaretX + ox + char_px_width * scale;
		vert[5] = m_textCaretY + oy;

		vert[6] = m_textCaretX + ox;
		vert[7] = m_textCaretY + oy;

		glBindBuffer(GL_ARRAY_BUFFER, vId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

		if (ch < first_char || ch > last_char) {
			ch = last_char + 1;
		}
		int index = ch - first_char;
		float ux = char_uv_width * (index % char_count_width);
		float uy = (char_uv_height * (index / char_count_width));

		uv[0] = ux;
		uv[1] = uy + char_uv_height;

		uv[2] = ux + char_uv_width;
		uv[3] = uy + char_uv_height;

		uv[4] = ux + char_uv_width;
		uv[5] = uy;

		uv[6] = ux;
		uv[7] = uy;

		glBindBuffer(GL_ARRAY_BUFFER, uvId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

		glDrawArrays(GL_QUADS, 0, 4);

		ox += char_px_width * scale;
	}
	m_textCaretX += int(ox);
	m_textCaretY += int(oy);
}

void GUI::Manager::Print(int number, int scale)
{
	mtlString text;
	text.FromInt(number);
	Print(text, scale);
}

void GUI::Manager::Print(float number, int scale)
{
	mtlString text;
	text.FromFloat(number);
	Print(text, scale);
}

void GUI::Manager::PrintNewLine(int scale)
{
	CaretX() = 0;
	CaretY() += Newl();
	Newl() = char_px_height * mmlMax(scale, 1);
}

void GUI::Manager::SetTextCaret(int x, int y)
{
	m_textCaretX = x;
	m_textCaretY = y;
}

void GUI::Manager::DrawFilledBox(int x1, int y1, int x2, int y2)
{
	glBegin(GL_TRIANGLES);

	glVertex2i(x1, y1);
	glVertex2i(x2, y1);
	glVertex2i(x1, y2);

	glVertex2i(x2, y1);
	glVertex2i(x2, y2);
	glVertex2i(x1, y2);

	glEnd();
}

void GUI::Manager::DrawBox(int x1, int y1, int x2, int y2)
{
	glBegin(GL_LINE);

	glVertex2i(x1, y1);
	glVertex2i(x2, y1);

	glVertex2i(x2, y1);
	glVertex2i(x2, y2);

	glVertex2i(x2, y2);
	glVertex2i(x1, y2);

	glVertex2i(x1, y2);
	glVertex2i(x1, y1);

	glEnd();
}

void GUI::Manager::DrawLine(int x1, int y1, int x2, int y2)
{
	glBegin(GL_LINE);

	glVertex2i(x1, y1);
	glVertex2i(x2, y2);

	glEnd();
}

void GUI::Manager::Update( void )
{
	HandleInput();
	DrawForms();
}
