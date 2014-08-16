#include "Platform.h"
#include "GUI.h"
#include <climits>

static GLuint tId = 0, vId = 0, uvId = 0;
static GLfloat vert[8];
static GLfloat uv[8];

int &CaretX( void ) { static int caret_x = 0; return caret_x; }
int &CaretY( void ) { static int caret_y = 0; return caret_y; }
int &Newl( void ) { static int newl = 0; return newl; }

#define first_char '!'
#define last_char '~'

#define font_width 128
#define font_height 128

#define font_table_width 16
#define font_table_height 128

#define char_count_width 11
#define char_count_height 9

#define char_px_width 11
#define char_px_height 13

#define char_tab_count 4
#define char_tab_px_width (char_px_width * char_tab_count)

#define char_uv_width (float(char_px_width) / float(font_width))
#define char_uv_height (float(char_px_height) / float(font_height))

static const unsigned char font_bits[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x8f, 0x1f, 0xf1, 0x88, 0x7f, 0xfd, 0xf8, 0xc3,
	0x3f, 0xfe, 0xc1, 0x83, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x1f, 0xf1, 0x88,
	0x07, 0xf8, 0xfa, 0x81, 0x3f, 0xfe, 0xc0, 0x03, 0xff, 0xfe, 0xff, 0xff,
	0x8f, 0x3f, 0x73, 0x00, 0x03, 0xf8, 0xf8, 0x89, 0x7f, 0xff, 0xc0, 0x03,
	0x3f, 0xf8, 0xe3, 0xff, 0x8f, 0xff, 0x7f, 0x00, 0xa3, 0xfe, 0xff, 0xc3,
	0xff, 0xff, 0xf8, 0x1f, 0x7f, 0xfc, 0xe3, 0xff, 0x8f, 0xff, 0x7f, 0x00,
	0x03, 0x1c, 0xc0, 0xe3, 0xff, 0xff, 0xf8, 0x1f, 0x7f, 0xfc, 0x80, 0xff,
	0x8f, 0xff, 0xff, 0x88, 0x03, 0x18, 0xc0, 0xc1, 0xfe, 0xff, 0xf8, 0x1f,
	0xbf, 0xfb, 0x80, 0xff, 0x8f, 0xff, 0x7f, 0x00, 0x07, 0x18, 0xc0, 0x18,
	0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0x7f, 0x00,
	0xaf, 0xf8, 0xff, 0x38, 0xfe, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xe3, 0xff,
	0x8f, 0xff, 0x7f, 0x00, 0x03, 0xf8, 0xf8, 0x00, 0xff, 0xff, 0xc0, 0x03,
	0xff, 0xff, 0xe3, 0xff, 0x8f, 0xff, 0xff, 0x88, 0x03, 0xfc, 0xfa, 0x00,
	0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0x88,
	0xd7, 0xff, 0xf8, 0x81, 0xff, 0xff, 0xc1, 0x83, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x3f, 0x3e, 0xe0, 0xc3, 0x07, 0x78, 0xc0, 0x71, 0x0c, 0xe0, 0x80, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x3f, 0x1e, 0xc0, 0xc1, 0x07, 0x30, 0x80, 0x71,
	0x0c, 0x60, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x1f, 0xc0, 0xc1,
	0x07, 0x30, 0x80, 0x71, 0x0c, 0x60, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x1f, 0x1f, 0xc7, 0xc7, 0xff, 0x31, 0x8e, 0x71, 0x8c, 0x7f, 0xfc, 0xff,
	0xff, 0x1f, 0xf0, 0xff, 0x8f, 0x1f, 0xc3, 0xc7, 0x0f, 0xf0, 0x87, 0x01,
	0x0c, 0x70, 0x80, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0x8f, 0x1f, 0xc5, 0xc7,
	0x07, 0xf0, 0xc3, 0x01, 0x0c, 0x60, 0x00, 0xff, 0xff, 0x1f, 0xf0, 0xff,
	0xc7, 0x1f, 0xc6, 0xc7, 0x07, 0xf8, 0x87, 0x03, 0x0c, 0x60, 0x00, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xc7, 0x1f, 0xc7, 0xc7, 0xc7, 0x3f, 0x8e, 0x7f,
	0xfc, 0x63, 0x1c, 0xff, 0xc7, 0xff, 0xff, 0xf1, 0xe3, 0x1f, 0xc0, 0x01,
	0x07, 0x30, 0x80, 0x7f, 0x0c, 0x60, 0x00, 0xff, 0xc7, 0xff, 0xff, 0xf1,
	0xe3, 0x1f, 0xc0, 0x01, 0x07, 0x30, 0x80, 0x7f, 0x0c, 0x60, 0x00, 0xff,
	0xcf, 0xff, 0xff, 0xf1, 0xe3, 0x3f, 0xe0, 0x01, 0x07, 0x70, 0xc0, 0x7f,
	0x0c, 0xf0, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x01, 0x1c, 0xf0, 0x80, 0x8f, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x03,
	0x1e, 0xf0, 0x80, 0xff, 0x01, 0x0c, 0x60, 0x00, 0x8f, 0x7f, 0xfc, 0xff,
	0xff, 0xff, 0xff, 0x01, 0x0c, 0x60, 0x00, 0xff, 0x01, 0x0c, 0x60, 0x00,
	0x8f, 0x7f, 0xfc, 0x8f, 0x0f, 0xf8, 0xf8, 0x01, 0xcc, 0x67, 0x00, 0xff,
	0x7f, 0x8c, 0x63, 0x1c, 0xff, 0xff, 0xff, 0xc7, 0x0f, 0xf8, 0xf1, 0x71,
	0xcc, 0x60, 0x1c, 0xff, 0x7f, 0x0c, 0x60, 0x00, 0xff, 0xff, 0xff, 0xe3,
	0x0f, 0xf8, 0xe3, 0x1f, 0x4c, 0x60, 0x1c, 0xff, 0x3f, 0x1e, 0x70, 0x00,
	0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xc7, 0x0f, 0x4e, 0x66, 0x00, 0xff,
	0x3f, 0x0e, 0xe0, 0x00, 0xff, 0xff, 0xff, 0xe3, 0x0f, 0xf8, 0xe3, 0x8f,
	0x4f, 0x60, 0x00, 0xff, 0x3f, 0x8e, 0xe3, 0x1f, 0xff, 0xff, 0xff, 0xc7,
	0x0f, 0xf8, 0xf1, 0xff, 0xcf, 0x60, 0x00, 0xff, 0x1f, 0x0f, 0xe0, 0x00,
	0x8f, 0x7f, 0xfc, 0x8f, 0x0f, 0xf8, 0xf8, 0x8f, 0xcf, 0x7f, 0x1c, 0xff,
	0x1f, 0x0f, 0xe0, 0x00, 0x8f, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x8f,
	0x0f, 0x60, 0x1c, 0xff, 0x1f, 0x1f, 0xf0, 0x80, 0x8f, 0xff, 0xfc, 0xff,
	0xff, 0xff, 0xff, 0x8f, 0x1f, 0x70, 0x1c, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x01, 0x1e, 0x70, 0x80, 0x03, 0x18, 0xc0, 0x01,
	0xc7, 0x71, 0xc0, 0x01, 0x8c, 0x63, 0xfc, 0xff, 0x01, 0x0c, 0x60, 0x00,
	0x03, 0x18, 0xc0, 0x00, 0xc6, 0x71, 0xc0, 0x01, 0x8c, 0x63, 0xfc, 0xff,
	0x01, 0x0c, 0x60, 0x00, 0x03, 0x18, 0xc0, 0x00, 0xc6, 0x71, 0xc0, 0x01,
	0x8c, 0x63, 0xfc, 0xff, 0x71, 0x8c, 0x63, 0x1c, 0xe3, 0x1f, 0xff, 0x38,
	0xc6, 0xf1, 0xf1, 0x7f, 0x8c, 0x63, 0xfc, 0xff, 0x01, 0x8c, 0x7f, 0x1c,
	0x03, 0x1f, 0xf8, 0xf8, 0x07, 0xf0, 0xf1, 0x7f, 0x0c, 0x60, 0xfc, 0xff,
	0x01, 0x8e, 0x7f, 0x1c, 0x03, 0x1f, 0xf8, 0x08, 0x06, 0xf0, 0xf1, 0x7f,
	0x0c, 0x70, 0xfc, 0xff, 0x01, 0x8c, 0x7f, 0x1c, 0x03, 0x1f, 0xf8, 0x08,
	0x06, 0xf0, 0xf1, 0x71, 0x0c, 0x60, 0xfc, 0xff, 0x71, 0x8c, 0x63, 0x1c,
	0xe3, 0x1f, 0xff, 0x38, 0xc6, 0xf1, 0xf1, 0x71, 0x8c, 0x63, 0xfc, 0xff,
	0x01, 0x0c, 0x60, 0x00, 0x03, 0x18, 0xff, 0x00, 0xc6, 0x71, 0xc0, 0x01,
	0x8c, 0x63, 0x00, 0xff, 0x01, 0x0c, 0x60, 0x00, 0x03, 0x18, 0xff, 0x00,
	0xc6, 0x71, 0xc0, 0x01, 0x8c, 0x63, 0x00, 0xff, 0x01, 0x1e, 0x70, 0x80,
	0x03, 0x18, 0xff, 0x01, 0xc7, 0x71, 0xc0, 0x03, 0x8e, 0x63, 0x00, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x8c, 0xe3, 0x80,
	0x03, 0x3c, 0xe0, 0x00, 0x0f, 0x38, 0x80, 0x71, 0x8c, 0x63, 0x1c, 0xff,
	0x71, 0x8c, 0x63, 0x00, 0x03, 0x18, 0xc0, 0x00, 0x06, 0x30, 0x80, 0x71,
	0x8c, 0x63, 0x1c, 0xff, 0x21, 0x8c, 0x63, 0x00, 0x03, 0x18, 0xc0, 0x00,
	0x06, 0x30, 0x80, 0x71, 0x8c, 0x63, 0x1c, 0xff, 0x01, 0x0c, 0x63, 0x1c,
	0xe3, 0x18, 0xc7, 0x38, 0xc6, 0xff, 0xf1, 0x71, 0x8c, 0x63, 0x1c, 0xff,
	0x01, 0x0c, 0x62, 0x1c, 0xe3, 0x18, 0xc7, 0x38, 0x06, 0xf8, 0xf1, 0x71,
	0x8c, 0x63, 0x1c, 0xff, 0x51, 0x0c, 0x60, 0x1c, 0x03, 0x18, 0xc7, 0x00,
	0x06, 0xf0, 0xf1, 0x71, 0x8c, 0x63, 0x14, 0xff, 0x71, 0x8c, 0x60, 0x1c,
	0x03, 0x18, 0xc1, 0x00, 0x0f, 0xf0, 0xf1, 0x71, 0x8c, 0x63, 0x00, 0xff,
	0x71, 0x8c, 0x61, 0x1c, 0x03, 0x1c, 0xc1, 0x00, 0xfe, 0xf1, 0xf1, 0x71,
	0x1c, 0x71, 0x00, 0xff, 0x71, 0x8c, 0x63, 0x00, 0xe3, 0x1f, 0xc0, 0x38,
	0x06, 0xf0, 0xf1, 0x01, 0x3c, 0x78, 0x08, 0xff, 0x71, 0x8c, 0x63, 0x00,
	0xe3, 0x1f, 0xc0, 0x38, 0x06, 0xf0, 0xf1, 0x01, 0x7c, 0x7c, 0x1c, 0xff,
	0x71, 0x8c, 0xe3, 0x80, 0xe3, 0x3f, 0xc0, 0x38, 0x0e, 0xf8, 0xf1, 0x03,
	0xfe, 0xfe, 0xbe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x71, 0x8c, 0x63, 0x00, 0x0f, 0x3c, 0xfe, 0x81, 0x7f, 0xff, 0xff, 0x8f,
	0x1f, 0x70, 0x80, 0xff, 0x71, 0x8c, 0x63, 0x00, 0x0f, 0x3c, 0xfe, 0x81,
	0x3f, 0xfe, 0xff, 0x8f, 0x0f, 0x60, 0x00, 0xff, 0x71, 0x8c, 0x63, 0x00,
	0x0f, 0x7c, 0xfc, 0x81, 0x1f, 0xfc, 0xff, 0xbf, 0x0f, 0x60, 0x00, 0xff,
	0x21, 0x8c, 0xe3, 0x1f, 0x8f, 0x7f, 0xfc, 0x8f, 0x9f, 0xfc, 0xff, 0xff,
	0x8f, 0x63, 0x1c, 0xff, 0x01, 0x0c, 0xe0, 0x01, 0x8f, 0xff, 0xf8, 0x8f,
	0xdf, 0xfd, 0xff, 0xff, 0x8f, 0x63, 0x00, 0xff, 0x03, 0x0e, 0xe0, 0x80,
	0x8f, 0xff, 0xf8, 0x8f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x60, 0x80, 0xff,
	0x01, 0x1c, 0x70, 0xc0, 0x8f, 0xff, 0xf1, 0x8f, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x60, 0x00, 0xff, 0x21, 0x7c, 0x7c, 0xfc, 0x8f, 0xff, 0xf1, 0x8f,
	0xff, 0xff, 0xff, 0xff, 0x0f, 0x60, 0x1c, 0xff, 0x71, 0x7c, 0x7c, 0x00,
	0x0f, 0xfc, 0xe3, 0x81, 0xff, 0x3f, 0x80, 0xff, 0x8f, 0x63, 0x00, 0xff,
	0x71, 0x7c, 0x7c, 0x00, 0x0f, 0xfc, 0xe3, 0x81, 0xff, 0x3f, 0x80, 0xff,
	0x8f, 0x63, 0x00, 0xff, 0x71, 0x7c, 0x7c, 0x00, 0x0f, 0xfc, 0xe3, 0x81,
	0xff, 0x3f, 0x80, 0xff, 0x8f, 0x63, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x03, 0x0e, 0x70, 0x00, 0x03, 0x38, 0xe0, 0x38,
	0x0e, 0x38, 0x80, 0x71, 0x8c, 0x7f, 0x3e, 0xff, 0x01, 0x0c, 0x60, 0x00,
	0x03, 0x18, 0xc0, 0x38, 0x0e, 0x38, 0x80, 0x71, 0x8c, 0x7f, 0x1c, 0xff,
	0x01, 0x0c, 0x60, 0x00, 0x03, 0x18, 0xc0, 0x38, 0x0e, 0x38, 0x80, 0x71,
	0x8c, 0x7f, 0x08, 0xff, 0x71, 0x8c, 0x63, 0xfc, 0xe3, 0x1f, 0xc7, 0x38,
	0x3e, 0xfe, 0x8f, 0x71, 0x8c, 0x7f, 0x00, 0xff, 0xf1, 0x8f, 0x63, 0xe0,
	0x03, 0x1f, 0xff, 0x00, 0x3e, 0xfe, 0x8f, 0x01, 0x8c, 0x7f, 0x00, 0xff,
	0xf1, 0x8f, 0x63, 0xe0, 0x03, 0x1f, 0xc1, 0x00, 0x3e, 0xfe, 0x8f, 0x01,
	0x8e, 0x7f, 0x14, 0xff, 0xf1, 0x8f, 0x63, 0xe0, 0x03, 0x1f, 0xc1, 0x00,
	0x3e, 0x3e, 0x8e, 0x01, 0x8c, 0x7f, 0x1c, 0xff, 0x71, 0x8c, 0x63, 0xfc,
	0xe3, 0x1f, 0xc7, 0x38, 0x3e, 0x3e, 0x8e, 0x71, 0x8c, 0x7f, 0x1c, 0xff,
	0x01, 0x0c, 0x60, 0x00, 0xe3, 0x1f, 0xc0, 0x38, 0x0e, 0x38, 0x80, 0x71,
	0x0c, 0x60, 0x1c, 0xff, 0x01, 0x0c, 0x60, 0x00, 0xe3, 0x1f, 0xc0, 0x38,
	0x0e, 0x38, 0x80, 0x71, 0x0c, 0x60, 0x1c, 0xff, 0x03, 0x0e, 0x70, 0x00,
	0xe3, 0x3f, 0xe0, 0x38, 0x0e, 0x78, 0xc0, 0x71, 0x0c, 0x60, 0x1c, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x71, 0x1c, 0x70, 0x80,
	0x07, 0x1c, 0xe0, 0x01, 0x07, 0x30, 0x8e, 0x71, 0x8c, 0x63, 0x1c, 0xff,
	0x71, 0x0c, 0x60, 0x00, 0x03, 0x18, 0xc0, 0x00, 0x06, 0x30, 0x8e, 0x71,
	0x8c, 0x63, 0x1c, 0xff, 0x71, 0x0c, 0x60, 0x00, 0x03, 0x18, 0xc0, 0x00,
	0x06, 0x30, 0x8e, 0x71, 0x8c, 0x63, 0x1c, 0xff, 0x61, 0x8c, 0x63, 0x1c,
	0xe3, 0x18, 0xc7, 0xf8, 0x3f, 0x3e, 0x8e, 0x71, 0x8c, 0x63, 0x08, 0xff,
	0x41, 0x8c, 0x63, 0x1c, 0xe3, 0x18, 0xc7, 0x00, 0x3f, 0x3e, 0x8e, 0x71,
	0x8c, 0x63, 0x00, 0xff, 0x01, 0x8c, 0x63, 0x00, 0xe3, 0x18, 0xc0, 0x00,
	0x3e, 0x3e, 0x8e, 0x71, 0x8c, 0xe2, 0x80, 0xff, 0x11, 0x8c, 0x63, 0x00,
	0x23, 0x18, 0xe0, 0x01, 0x3e, 0x3e, 0x8e, 0x71, 0x0c, 0x60, 0x00, 0xff,
	0x31, 0x8c, 0x63, 0x80, 0x23, 0x18, 0xc0, 0x3f, 0x3e, 0x3e, 0x8e, 0x23,
	0x0e, 0x60, 0x08, 0xff, 0x71, 0x0c, 0x60, 0xfc, 0x03, 0x18, 0xc7, 0x00,
	0x3e, 0x3e, 0x80, 0x07, 0x0f, 0x61, 0x1c, 0xff, 0x71, 0x0c, 0x60, 0xfc,
	0x03, 0x18, 0xc7, 0x00, 0x3e, 0x3e, 0x80, 0x8f, 0x8f, 0x63, 0x1c, 0xff,
	0x71, 0x1c, 0x70, 0xfc, 0x07, 0x18, 0xc7, 0x01, 0x3f, 0x7e, 0xc0, 0xdf,
	0xdf, 0x77, 0x1c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x71, 0x0c, 0xe0, 0x83, 0x1f, 0x3f, 0xf8, 0xd7, 0x07, 0xf0, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x71, 0x0c, 0xe0, 0x81, 0x1f, 0x3f, 0xf0, 0xc3,
	0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x71, 0x0c, 0xe0, 0x81,
	0x1f, 0x3f, 0xf0, 0xc3, 0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x71, 0xfc, 0xe3, 0xf1, 0x1f, 0xff, 0xf1, 0xeb, 0xf7, 0xf7, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x01, 0x3c, 0xe0, 0xf3, 0x1f, 0xff, 0xf9, 0xff,
	0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x1c, 0xf0, 0xf9,
	0x1f, 0xff, 0xf3, 0xff, 0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x03, 0x0e, 0xf8, 0xf3, 0x1f, 0xff, 0xf9, 0xff, 0xf7, 0xf7, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x8f, 0x8f, 0xff, 0xf1, 0x1f, 0xff, 0xf1, 0xff,
	0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x0f, 0xe0, 0x81,
	0x1f, 0x3f, 0xf0, 0xff, 0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x8f, 0x0f, 0xe0, 0x81, 0x1f, 0x3f, 0xf0, 0xff, 0xf7, 0xf7, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x8f, 0x0f, 0xe0, 0x83, 0x1f, 0x3f, 0xf8, 0xff,
	0x07, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

void GUI::Init( void )
{
	if (tId == 0) {
		glGenBuffers(1, &vId);
		glGenBuffers(1, &uvId);

		unsigned int *pixels = new unsigned int[font_width*font_height];

		// every row is aligned (16 bytes * 8 bits) * 128 bytes
		for (int y = 0, i = 0; y < font_height; ++y) {
			for (int x = 0; x < font_width; ++x) {
				unsigned char byte = font_bits[y * font_table_width + x / 8];
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
	glColor3f(color[0], color[1], color[1]);
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
	Newl() = char_px_height * mmlMax2(scale, 1);
}

void GUI::Print(const mtlChars &text, int scale)
{
	if (text.GetSize() == 0) { return; }
	scale = mmlMax2(1, scale);
	Newl() = mmlMax2(Newl(), char_px_height * scale);

	glBindBuffer(GL_ARRAY_BUFFER, uvId);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, vId);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	glBindTexture(GL_TEXTURE_2D, tId);

	float ox = 0.0f;
	float oy = 0.0f;
	for (int i = 0; i < text.GetSize(); ++i) {
		char ch = text.GetChars()[i];
		switch (ch) {
		case '\n':
		case '\r':
			ox = 0.0f;
			oy += char_px_height * scale;
			continue;
		case ' ':
			ox += char_px_width * scale;
			continue;
		case '\t':
			ox += char_tab_px_width * scale;
			continue;
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
	CaretX() += int(ox);
	CaretY() += int(oy);
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

void GUI::Box(Rect rect)
{
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

	Newl() = mmlMax2(Newl(), rect.h);
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
			p.x = mmlMax2(x, p.x);
		}
	}
	p.x *= scale;
	p.y *= scale;
	return p;
}

int GUI::GetCharPixelWidth(int scale)
{
	return char_px_width * scale;
}

int GUI::GetCharPixelHeight(int scale)
{
	return char_px_height * scale;
}

GUI::Manager::Manager( void ) : m_windows(), m_focus(), m_color(1.0f, 1.0f, 1.0f, 1.0f), m_textCaretX(0), m_textCaretY(0), m_newlineHeight(char_px_height)
{}

GUI::Manager::~Manager( void )
{
}

void GUI::Manager::Destroy( void )
{
	m_focus.Delete();
	m_windows.RemoveAll();
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
	scale = mmlMax2(1, scale);
	m_newlineHeight = mmlMax2(m_newlineHeight, char_px_height * scale);

	glBindBuffer(GL_ARRAY_BUFFER, uvId);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, vId);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	glBindTexture(GL_TEXTURE_2D, tId);

	float ox = 0.0f;
	float oy = 0.0f;
	for (int i = 0; i < text.GetSize(); ++i) {
		char ch = text.GetChars()[i];
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
	Newl() = char_px_height * mmlMax2(scale, 1);
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
