#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Engine.h"
#include "Font.h"
#include "Object.h"
#include "Image.h"
#include "ImageRenderer.h"
#include <iostream>

class Controllable : public Inherit<Object>
{
private:
	mmlVector<2> m_movement;
protected:
	void OnUpdate( void );
public:
	Controllable( void );
};

void Unit_OpenGLTest( void );
void Unit_Font(Engine &engine);
void Unit_Controllable(Engine &engine);

int main(int argc, char **argv)
{
	//Unit_OpenGLTest();
	Engine engine;
	engine.Init(argc, argv);
	engine.SetWindowCaption("Lots-o-tests");
	Unit_Font(engine);
	Unit_Controllable(engine);
	return 0;
}

void Controllable::OnUpdate( void )
{
	const mtlNode<SDL_Event> *event = GetEventList().GetFirst();
	while (event != NULL) {
		switch (event->GetItem().type) {
		case SDL_KEYDOWN:
			switch (event->GetItem().key.keysym.sym) {
			case SDLK_UP:
				m_movement[1] -= 1.0f;
				break;
			case SDLK_DOWN:
				m_movement[1] += 1.0f;
				break;
			case SDLK_LEFT:
				m_movement[0] -= 1.0f;
				//GetTransform().SetLocalAxisX(mmlVector<2>(-1.0f, 0.0f));
				break;
			case SDLK_RIGHT:
				m_movement[0] += 1.0f;
				//GetTransform().SetLocalAxisX(mmlVector<2>(1.0f, 0.0f));
				break;
			case SDLK_ESCAPE:
				EndGame();
				break;
			default: break;
			}
			break;
		case SDL_KEYUP:
			switch (event->GetItem().key.keysym.sym) {
			case SDLK_UP:
				m_movement[1] += 1.0f;
				break;
			case SDLK_DOWN:
				m_movement[1] -= 1.0f;
				break;
			case SDLK_LEFT:
				m_movement[0] += 1.0f;
				break;
			case SDLK_RIGHT:
				m_movement[0] -= 1.0f;
				break;
			default: break;
			}
			break;
		default: break;
		}
		event = event->GetNext();
	}
	GetTransform().ApplyLocalTranslation(m_movement);
}

Controllable::Controllable( void ) : m_movement(0.0f, 0.0f)
{
	SetName("object_controllable");
}

void Unit_OpenGLTest( void )
{
	/*const SDL_VideoInfo* info = NULL;
	int width = 0;
	int height = 0;
	int bpp = 0;
	int flags = 0;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		fprintf( stderr, "Video initialization failed: %s\n",
			 SDL_GetError( ) );
		SDL_Quit( );
		exit( 1 );
	}

	// Let's get some video information.
	info = SDL_GetVideoInfo( );

	if( !info ) {
		fprintf( stderr, "Video query failed: %s\n",
			 SDL_GetError( ) );
		SDL_Quit( );
		exit( 1 );
	}

	// Set our width/height to 640/480 (you would
	// of course let the user decide this in a normal
	// app). We get the bpp we will request from
	// the display. On X11, VidMode can't change
	// resolution, so this is probably being overly
	// safe. Under Win32, ChangeDisplaySettings
	// can change the bpp.
	width = 640;
	height = 480;
	bpp = info->vfmt->BitsPerPixel;

	// Now, we want to setup our requested
	// window attributes for our OpenGL window.
	// We want *at least* 5 bits of red, green
	// and blue. We also want at least a 16-bit
	// depth buffer.

	// The last thing we do is request a double
	// buffered window. '1' turns on double
	// buffering, '0' turns it off.

	// Note that we do not use SDL_DOUBLEBUF in
	// the flags to SDL_SetVideoMode. That does
	// not affect the GL attribute state, only
	// the standard 2D blitting setup.

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	flags = SDL_OPENGL;

	// Set the video mode
	if( SDL_SetVideoMode( width, height, bpp, flags ) == 0 ) {
		fprintf( stderr, "Video mode set failed: %s\n",
			 SDL_GetError( ) );
		SDL_Quit( );
		exit( 1 );
	}

	// At this point, we should have a properly setup
	// double-buffered window for use with OpenGL.
	float ratio = (float) width / (float) height;

	// Our shading model--Gouraud (smooth).
	glShadeModel( GL_SMOOTH );

	// Culling.
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	glEnable( GL_CULL_FACE );

	// Set the clear color.
	glClearColor( 0, 0, 0, 0 );

	// Setup our viewport.
	glViewport( 0, 0, width, height );

	// Change to the projection matrix and set
	// our viewing volume.
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluPerspective( 60.0, ratio, 1.0, 1024.0 );

	static GLfloat v0[] = { -1.0f, -1.0f,  1.0f };
	static GLfloat v1[] = {  1.0f, -1.0f,  1.0f };
	static GLfloat v2[] = {  1.0f,  1.0f,  1.0f };
	static GLfloat v3[] = { -1.0f,  1.0f,  1.0f };
	static GLfloat v4[] = { -1.0f, -1.0f, -1.0f };
	static GLfloat v5[] = {  1.0f, -1.0f, -1.0f };
	static GLfloat v6[] = {  1.0f,  1.0f, -1.0f };
	static GLfloat v7[] = { -1.0f,  1.0f, -1.0f };
	static GLubyte red[]    = { 255,   0,   0, 255 };
	static GLubyte green[]  = {   0, 255,   0, 255 };
	static GLubyte blue[]   = {   0,   0, 255, 255 };
	static GLubyte white[]  = { 255, 255, 255, 255 };
	static GLubyte yellow[] = {   0, 255, 255, 255 };
	static GLubyte black[]  = {   0,   0,   0, 255 };
	static GLubyte orange[] = { 255, 255,   0, 255 };
	static GLubyte purple[] = { 255,   0, 255,   0 };

	GLboolean should_rotate = GL_TRUE;


	// Now we want to begin our normal app process--
	// an event loop with a lot of redrawing.
	SDL_Event event;
	while( 1 ) {
	   while( SDL_PollEvent( &event ) ) {

		   switch( event.type ) {
		   case SDL_KEYDOWN:
			   switch( event.key.keysym.sym ) {
			   case SDLK_ESCAPE:
				   SDL_Quit( );
				   exit( 0 );
				   break;
			   case SDLK_SPACE:
				   should_rotate = !should_rotate;
				   break;
			   default:
				   break;
			   }
			   break;
		   case SDL_QUIT:
			   SDL_Quit( );
			   exit( 0 );
			   break;
		   }

	   }
		static float angle = 0.0f;

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity( );

		glTranslatef( 0.0, 0.0, -5.0 );

		glRotatef( angle, 0.0, 1.0, 0.0 );

		if( should_rotate ) {

			if( ++angle > 360.0f ) {
				angle = 0.0f;
			}

		}

		glBegin( GL_TRIANGLES );

		glColor4ubv( red );
		glVertex3fv( v0 );
		glColor4ubv( green );
		glVertex3fv( v1 );
		glColor4ubv( blue );
		glVertex3fv( v2 );

		glColor4ubv( red );
		glVertex3fv( v0 );
		glColor4ubv( blue );
		glVertex3fv( v2 );
		glColor4ubv( white );
		glVertex3fv( v3 );

		glColor4ubv( green );
		glVertex3fv( v1 );
		glColor4ubv( black );
		glVertex3fv( v5 );
		glColor4ubv( orange );
		glVertex3fv( v6 );

		glColor4ubv( green );
		glVertex3fv( v1 );
		glColor4ubv( orange );
		glVertex3fv( v6 );
		glColor4ubv( blue );
		glVertex3fv( v2 );

		glColor4ubv( black );
		glVertex3fv( v5 );
		glColor4ubv( yellow );
		glVertex3fv( v4 );
		glColor4ubv( purple );
		glVertex3fv( v7 );

		glColor4ubv( black );
		glVertex3fv( v5 );
		glColor4ubv( purple );
		glVertex3fv( v7 );
		glColor4ubv( orange );
		glVertex3fv( v6 );

		glColor4ubv( yellow );
		glVertex3fv( v4 );
		glColor4ubv( red );
		glVertex3fv( v0 );
		glColor4ubv( white );
		glVertex3fv( v3 );

		glColor4ubv( yellow );
		glVertex3fv( v4 );
		glColor4ubv( white );
		glVertex3fv( v3 );
		glColor4ubv( purple );
		glVertex3fv( v7 );

		glColor4ubv( white );
		glVertex3fv( v3 );
		glColor4ubv( blue );
		glVertex3fv( v2 );
		glColor4ubv( orange );
		glVertex3fv( v6 );

		glColor4ubv( white );
		glVertex3fv( v3 );
		glColor4ubv( orange );
		glVertex3fv( v6 );
		glColor4ubv( purple );
		glVertex3fv( v7 );

		glColor4ubv( green );
		glVertex3fv( v1 );
		glColor4ubv( red );
		glVertex3fv( v0 );
		glColor4ubv( yellow );
		glVertex3fv( v4 );

		glColor4ubv( green );
		glVertex3fv( v1 );
		glColor4ubv( yellow );
		glVertex3fv( v4 );
		glColor4ubv( black );
		glVertex3fv( v5 );

		glEnd( );

		// Swap the buffers. This this tells the driver to
		// render the next frame from the contents of the
		// back-buffer, and to set all rendering operations
		// to occur on what was the front-buffer.

		// Double buffering prevents nasty visual tearing
		// from the application drawing on areas of the
		// screen that are being updated at the same time.
		SDL_GL_SwapBuffers( );
	}*/

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_SetVideoMode(640, 480, 32, SDL_OPENGL);
	glShadeModel( GL_SMOOTH );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	glEnable( GL_CULL_FACE );
	glClearColor( 0, 0, 0, 0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static GLfloat v0[] = { 0.0f, 0.0f,  1.0f };
	static GLfloat v1[] = {  SDL_GetVideoSurface()->w*0.5, 0.0f,  1.0f };
	static GLfloat v2[] = {  SDL_GetVideoSurface()->w*0.5,  SDL_GetVideoSurface()->h*0.5,  1.0f };
	static GLubyte red[]    = { 255,   0,   0, 255 };
	static GLubyte green[]  = {   0, 255,   0, 255 };
	static GLubyte blue[]   = {   0,   0, 255, 255 };

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho(0.0f, SDL_GetVideoSurface()->w, 0.0f, SDL_GetVideoSurface()->h, 1.0f, 1024.0f);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glTranslatef( 0.0, 0.0, -5.0 );

	glBegin( GL_TRIANGLES );

	glColor4ubv( red );
	glVertex3fv( v0 );
	glColor4ubv( green );
	glVertex3fv( v1 );
	glColor4ubv( blue );
	glVertex3fv( v2 );

	glEnd();

	SDL_GL_SwapBuffers();

	SDL_Event event;
	while (SDL_WaitEvent(&event) && event.type != SDL_KEYDOWN) {}
}

void Unit_Font(Engine &engine)
{

}

void Unit_Controllable(Engine &engine)
{
	std::cout << "Unit_Controllable: " << std::endl;
	Controllable *c = new Controllable();
	if (!c->LoadGraphics<Image>("test.bmp")) {
		return;
	}
	engine.AddObject(c);

	engine.SetRenderer(new ImageRenderer);

	engine.RunGame();
}
