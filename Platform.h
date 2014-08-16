#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __APPLE__

	// DON'T FORGET TO INCLUDE THE FRAMEWORKS IN THE LINKER:
	// Cocoa.Framework
	// OpenGL.Framework
	// SDL.framework
	// SDL_image.framework
	// SDL_mixer.framework

	#include <OpenGL/gl.h> // latest supported version of OpenGL in OSX/iOS

	#include <SDL/SDL.h>
	#include <SDL_image/SDL_image.h>
	#include <SDL_mixer/SDL_mixer.h>

#elif defined(_MSC_VER)

	#pragma comment(lib, "opengl32.lib")
	// set library search path for the following libs...
	#pragma comment(lib, "glew32.lib")
	#pragma comment(lib, "SDL.lib")
	#pragma comment(lib, "SDLmain.lib")
	#pragma comment(lib, "SDL_image.lib")
	#pragma comment(lib, "SDL_mixer.lib")

	#include <GL/gl.h> // only core profile (1.1), need glew/glext/glee (I chose glew)
	#include <GL/glew.h> // download install in GL folder (remember to put .dll and .lib files in appropriate places as well)

	#include "SDL.h"
	#include "SDL_image.h"
	#include "SDL_mixer.h"

#elif defined(__linux__)

	// LINK TO THESE
	// -lGL -lSDL -lSDLmain -lSDL_image -lSDL_mixer

	#include <GL/gl.h>
	#include <GL/glext.h>

	#include <SDL/SDL.h>
	#include <SDL/SDL_image.h>
	#include <SDL/SDL_mixer.h>

#elif defined(_WIN32) && defined(__MINGW32__)

	// LINK TO THESE
	// -lopengl32 -lglew32 -lSDL -lSDLmain -lSDL_image -lSDL_mixer

	#include <GL/gl.h> // only core profile (1.1), need glew/glext/glee (I chose glew)
	#include <GL/glew.h> // download install in GL folder (remember to put .dll and .lib files in appropriate places as well)

	#include "SDL/SDL.h"
	#include "SDL/SDL_image.h"
	#include "SDL/SDL_mixer.h"

#else

	#error Unsupported operating system/compiler combination.

#endif

#endif // PLATFORM_H
