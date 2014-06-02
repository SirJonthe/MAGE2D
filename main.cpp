#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Engine.h"
#include "GUI.h"
#include "Object.h"
#include "Image.h"
#include "Renderer.h"
#include "MTL/mtlString.h"
#include "MTL/mtlStringMap.h"
#include <iostream>

class Controllable : public mtlInherit<Object>
{
private:
	mmlVector<2> m_movement;
protected:
	void OnUpdate( void );
	void OnGUI( void );
public:
	Controllable( void );
};

ENGINE_REGISTER_OBJECT_TYPE(Controllable);

void Unit_OpenGLTest( void );
void Unit_RegisteredObjects( void );
void Unit_Controllable(Engine &engine);
void Unit_StringMap( void );

int main(int argc, char **argv)
{
	//Unit_OpenGLTest();
	Engine engine;
	engine.Init(argc, argv);
	engine.SetWindowCaption("Lots-o-tests");
	Unit_RegisteredObjects();
	Unit_Controllable(engine);
	Unit_StringMap();
	return 0;
}

void Controllable::OnUpdate( void )
{
	const mtlNode<SDL_Event> *event = GetEngine()->GetEventList().GetFirst();
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
				GetTransform().SetAxisXDirection(1);
				break;
			case SDLK_RIGHT:
				m_movement[0] += 1.0f;
				GetTransform().SetAxisXDirection(-1);
				break;
			case SDLK_ESCAPE:
				GetEngine()->EndGame();
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

void Controllable::OnGUI( void )
{
	GUI::SetColor(1.0f, 0.0f, 0.0f);
	GUI::Text("Hello", 0, 0);
	GUI::SetColor(0.0f, 1.0f, 0.0f);
	GUI::Text(", ");
	GUI::SetColor(0.0f, 0.0f, 1.0f);
	GUI::Text("world");
	GUI::SetColor(1.0f, 1.0f, 1.0f);
	GUI::Text("!");
}

Controllable::Controllable( void ) : m_movement(0.0f, 0.0f)
{
	SetName("object_controllable");
}

void Unit_OpenGLTest( void )
{
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
	static GLfloat v1[] = {  SDL_GetVideoSurface()->w*0.5f, 0.0f,  1.0f };
	static GLfloat v2[] = {  SDL_GetVideoSurface()->w*0.5f,  SDL_GetVideoSurface()->h*0.5f,  1.0f };
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

void Unit_RegisteredObjects( void )
{
	std::cout << "Unit_RegisteredObjects: " << std::endl;
	mtlList< mtlShared<mtlString> > list;
	Engine::GetRegisteredTypes(list);
	mtlNode< mtlShared<mtlString> > *node = list.GetFirst();
	while (node != NULL) {
		std::cout << "\t" << node->GetItem().GetShared()->GetChars() << std::endl;
		node = node->GetNext();
	}
}

void Unit_Controllable(Engine &engine)
{
	std::cout << "Unit_Controllable: " << std::endl;
	Object *c = engine.AddObject("Controllable");
	if (c != NULL && !c->LoadGraphics<Image>("test.bmp")) {
		return;
	}

	engine.SetRenderer(new Renderer);

	engine.RunGame();
}

void Unit_StringMap( void )
{
	std::cout << "Unit_StringMap: ";
	mtlStringMap<Graphics> map;
	Graphics *a = map.CreateEntry<Image>("~/.local/share/game/image.bmp");
	Graphics *b = map.CreateEntry<Image>("~/.local/share/game/another_image.bmp");
	Graphics *c = map.CreateEntry<Image>("~/.local/share/game/a_third_image.bmp");

	if (a == map.GetEntry("~/.local/share/game/image.bmp") && b == map.GetEntry("~/.local/share/game/another_image.bmp") && c == map.GetEntry("~/.local/share/game/a_third_image.bmp")) {
		std::cout << "success" << std::endl;
	} else {
		std::cout << "failed" << std::endl;
	}
}
