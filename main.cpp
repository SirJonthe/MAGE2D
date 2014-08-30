#include "Engine.h"
#include "GUI.h"
#include "Object.h"
#include "Image.h"
#include "MTL/mtlString.h"
#include "MTL/mtlStringMap.h"
#include <iostream>

class Controllable : public mtlInherit<Object>
{
protected:
	void OnUpdate( void );
	void OnGUI( void );
	void OnCollision(ObjectRef, const CollisionInfo&);
public:
	Controllable( void );
};

class Anchor : public mtlInherit<Object>
{
protected:
	void OnUpdate( void );
	void OnDraw( void );
};

class FollowCamera : public mtlInherit<Object>
{
private:
	ObjectRef m_follow;
protected:
	void OnInit( void );
	void OnUpdate( void );
	void OnGUI( void );
public:
	FollowCamera( void );
};

class NPC : public mtlInherit<Object>
{
};

RegisterObject(Controllable);
RegisterObject(Anchor);
RegisterObject(FollowCamera);
RegisterObject(NPC);

void PrintString(const mtlChars &ch);

void Unit_OpenGLTest( void );
void Unit_RegisteredObjects( void );
void Unit_Controllable(Engine &engine);
void Unit_StringMap( void );

int main(int argc, char **argv)
{
	//Unit_OpenGLTest();
	Engine engine;
	engine.Init(800, 600, "Lots-o-tests", argc, argv);
	Unit_RegisteredObjects();
	Unit_Controllable(engine);
	Unit_StringMap();
	return 0;
}

void Controllable::OnUpdate( void )
{
	if (GetEngine()->IsDown(SDLK_UP)) {
		GetTransform().AxisTranslate(0.0f, -80.0f * GetEngine()->GetElapsedTime());
	}
	if (GetEngine()->IsDown(SDLK_DOWN)) {
		GetTransform().AxisTranslate(0.0f, 80.0f * GetEngine()->GetElapsedTime());
	}
	if (GetEngine()->IsDown(SDLK_LEFT)) {
		GetTransform().AxisTranslate(-80.0f * GetEngine()->GetElapsedTime(), 0.0f);
	}
	if (GetEngine()->IsDown(SDLK_RIGHT)) {
		GetTransform().AxisTranslate(80.0f * GetEngine()->GetElapsedTime(), 0.0f);
	}
	if (GetTransform().GetParent() == NULL) {
		if (GetEngine()->IsDown(SDLK_a)) {
			GetTransform().Rotate(-0.8f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_s)) {
			GetTransform().Rotate(0.8f * GetEngine()->GetElapsedTime());
		}
	}
	if (GetEngine()->IsDown(SDLK_ESCAPE)) {
		GetEngine()->EndGame();
	}
}

void Controllable::OnGUI( void )
{
	mmlMatrix<2,2> r = GetTransform().GetRotation(Transform::Local);
	GUI::Print("Object={");
	GUI::NewLine();
	for (int i = 0; i < 2; ++i) {
		GUI::Print("\t");
		for (int j = 0; j < 2; ++j) {
			GUI::Print(r[i][j]);
			GUI::Print(";");
		}
		GUI::NewLine();
	}
	GUI::Print("}");
	GUI::NewLine();
	if (GetTransform().GetParent() == NULL) {
		GUI::SetColor(1.0f, 0.0f, 0.0f);
		GUI::Print("DETACHED");
	} else {
		GUI::SetColor(0.0f, 1.0f, 0.0);
		GUI::Print("Attached");
	}
	GUI::NewLine();
}

void Controllable::OnCollision(ObjectRef, const CollisionInfo&)
{
	GUI::SetColor(1.0f, 0.0f, 0.0f);
	GUI::Print("OUCH!");
}

void Anchor::OnUpdate( void )
{
	if (GetEngine()->IsDown(SDLK_a)) {
		GetTransform().Rotate(-GetEngine()->GetElapsedTime());
	}
	if (GetEngine()->IsDown(SDLK_s)) {
		GetTransform().Rotate(GetEngine()->GetElapsedTime());
	}
	if (GetEngine()->IsPressed(SDLK_SPACE)) {
		mtlList<ObjectRef> list;
		GetEngine()->FilterByName(GetEngine()->GetObjects(), list, "object_controllable");
		mtlNode<ObjectRef> *node = list.GetFirst();
		if (node != NULL) {
			if (node->GetItem().GetShared()->GetTransform().GetParent() == NULL) {
				node->GetItem().GetShared()->GetTransform().SetParent(Transform::Global, &GetTransform());
			} else {
				node->GetItem().GetShared()->GetTransform().SetParent(Transform::Global, NULL);
			}
		}
	}
}

void Anchor::OnDraw( void )
{
	Rect r = { 0,0,10,10 };
	GUI::SetColor(1.0f, 0.0f, 1.0f);
	GUI::Box(r);
}

Controllable::Controllable( void )
{
	SetName("object_controllable");
}

void FollowCamera::OnInit( void )
{
	mtlList<ObjectRef> objects;
	GetEngine()->FilterByDynamicType<Controllable>(GetEngine()->GetObjects(), objects);
	if (objects.GetSize() > 0) {
		m_follow = objects.GetFirst()->GetItem();
	}
}

void FollowCamera::OnUpdate( void )
{
	if (!m_follow.IsNull() && !m_follow.GetShared()->IsDestroyed()) {
		GetTransform().SetPosition(Transform::Global, m_follow.GetShared()->GetTransform().GetPosition(Transform::Global));
	}
}

void FollowCamera::OnGUI( void )
{
	GUI::SetColor(1.0f, 1.0f, 0.0f);
	mmlVector<2> p = GetTransform().GetPosition(Transform::Local);
	GUI::Print("Camera=");
	GUI::Print(p[0]); GUI::Print(";"); GUI::Print(p[1]);
	GUI::NewLine();
}

FollowCamera::FollowCamera( void ) : m_follow()
{
	SetName("object_camera");
}

void PrintString(const mtlChars &ch)
{
	if (!ch.IsNull()) {
		if (ch.IsNullTerminated()) {
			std::cout << ch.GetChars();
		} else {
			for (int i = 0; i < ch.GetSize(); ++i) {
				std::cout << ch[i];
			}
		}
	}
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

	engine.SetOcclusionMethod(Engine::SortByY);

	ObjectRef a = engine.AddObject("Controllable");
	ObjectRef c = engine.AddObject("FollowCamera");
	engine.SetCamera(c);
	if (a.IsNull() || !a.GetShared()->LoadGraphics<Sprite>("test.sprite")) {
		std::cout << "\tfailed to load" << std::endl;
		return;
	}
	a.GetShared()->LoadCollider<PolygonCollider>();
	a->GetTransform().Scale(2.0f);

	ObjectRef b = engine.AddObject<NPC>();
	b.GetShared()->SetName("NPC");
	if (b.IsNull() || !b.GetShared()->LoadGraphics<Image>("test.bmp")) {
		std::cout << "\tfailed to load" << std::endl;
		return;
	}
	b.GetShared()->GetGraphics().SetRed(0.2f);
	b.GetShared()->GetGraphics().SetGreen(0.2f);
	b.GetShared()->GetGraphics().SetBlue(0.2f);
	b.GetShared()->LoadCollider<PolygonCollider>();

	ObjectRef d = engine.AddObject("Anchor");
	a.GetShared()->GetTransform().SetParent(Transform::Local, &d.GetShared()->GetTransform());

	engine.RunGame();
}

void Unit_StringMap( void )
{
	std::cout << "Unit_StringMap: ";
	mtlStringMap<Graphics> map;
	Graphics *a = map.CreateEntry<Image>("~/.local/share/game/image.bmp");
	Graphics *b = map.CreateEntry<Image>("~/.local/share/game/another_image.bmp");
	Graphics *c = map.CreateEntry<Image>("~/.local/share/game/a_third_image.bmp");

	if (a != map.GetEntry("~/.local/share/game/image.bmp")) { std::cout << "failed (a)" << std::endl; return; }
	if (b != map.GetEntry("~/.local/share/game/another_image.bmp")) { std::cout << "failed (b)" << std::endl; return; }
	if (c != map.GetEntry("~/.local/share/game/a_third_image.bmp")) { std::cout << "failed (c)" << std::endl; return; }
	std::cout << "success" << std::endl;
}
