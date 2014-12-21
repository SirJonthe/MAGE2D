#include "Engine.h"
#include "GUI.h"
#include "Object.h"
#include "Image.h"
#include "Tools.h"
#include "MTL/mtlString.h"
#include "MTL/mtlStringMap.h"
#include <iostream>

// Investigate if the font rendering is fixed by using unsigned ints as UVs rather than floats

ObjectDeclaration(Controllable)
{
protected:
	void OnUpdate( void );
	void OnGUI( void );
	void OnCollision(ObjectRef, const CollisionInfo&);
public:
	Controllable( void );
};

ObjectDeclaration(Anchor)
{
protected:
	void OnUpdate( void );
	void OnDraw( void );

public:
	Anchor( void ) : ConstructObject(Anchor) {}
};

ObjectDeclaration(FollowCamera)
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

ObjectDeclaration(NPC)
{
public:
	NPC( void ) : ConstructObject(NPC) {}
};

ObjectDeclaration(FontRenderer)
{
protected:
	void OnGUI( void )
	{
		for (int i = 1; i < 4; ++i) {
			GUI::Print("the quick brown fox jumps over the lazy dog", i);
			GUI::NewLine();
			GUI::Print("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", i);
			GUI::NewLine();
			GUI::Print("1234567890", i);
			GUI::NewLine();
			GUI::Print(".,:;!?()<>[]{}\"\'#%&/", i);
			GUI::NewLine();
		}
	}

public:
	FontRenderer( void ) : ConstructObject(FontRenderer) {}
};

ObjectDeclaration(GridRender)
{
public:
	GridRender( void ) : ConstructObject(GridRender) {}
	void OnGUI( void )
	{
		int num = 10;
		Rect rect;
		rect.w = GetEngine()->GetVideoWidth() / num;
		rect.h = GetEngine()->GetVideoHeight() / num;

		Ray r;
		r.origin[0] = GetEngine()->GetVideoWidth() / 2.0f;
		r.origin[1] = GetEngine()->GetVideoHeight() / 2.0f;
		GUI::SetCaretXY(0, 0);
		GUI::SetColor(1.0f, 0.0f, 0.0f);
		GUI::Print(GetEngine()->GetMousePosition());
		GUI::NewLine();
		Point mouse = GetEngine()->GetMousePosition();
		mmlVector<2> vMouse = mmlVector<2>(float(mouse.x), float(mouse.y)) - r.origin;
		r.direction = mmlNormalize(vMouse);
		r.origin[0] /= rect.w;
		r.origin[1] /= rect.h;
		GridWalker walker(r);
		for (int i = 0; i < num; ++i) {

			rect.x = walker.GetX()*rect.w;
			rect.y = walker.GetY()*rect.h;
			GUI::SetColor(0.0f, 0.0f, 0.5f);
			GUI::Box(rect);

			GUI::SetColor(0.0f, 1.0f, 0.0f);
			GUI::Print(walker.GetX());
			GUI::Print(", ");
			GUI::Print(walker.GetY());
			GUI::Print(" - ");
			GUI::Print(rect.x);
			GUI::Print(", ");
			GUI::Print(rect.y);
			GUI::NewLine();

			walker.Step();
		}
	}
};

ObjectDeclaration(Quitter)
{
protected:
	void OnUpdate( void )
	{
		if (GetEngine()->IsPressed(SDLK_ESCAPE)) {
			GetEngine()->EndGame();
		}
	}
public:
	Quitter( void ) : ConstructObject(Quitter) {}
};

RegisterObject(Controllable);
RegisterObject(Anchor);
RegisterObject(FollowCamera);
RegisterObject(NPC);
RegisterObject(FontRenderer);
RegisterObject(Quitter);
RegisterObject(GridRender);

void PrintString(const mtlChars &ch);

void Unit_OpenGLTest( void );
void Unit_RegisteredObjects( void );
void Unit_Controllable(Engine &engine);
void Unit_StringMap( void );
void Unit_ArrayResize( void );
void Unit_GUI(Engine &engine);
void Unit_RandomFloat(Engine &engine);
void Unit_Font(Engine &engine);

int main(int argc, char **argv)
{
	//Unit_OpenGLTest();
	Engine engine;
	engine.Init(800, 600, "Lots-o-tests", argc, argv);
	//Unit_RegisteredObjects();
	//Unit_Controllable(engine);
	//Unit_StringMap();
	//Unit_ArrayResize();
	//Unit_GUI(engine);
	//Unit_RandomFloat(engine);
	//Unit_Font(engine);
	engine.AddObject<SpriteEditor>();
	//engine.AddObject<GridRender>();
	engine.RunGame();
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
		mtlItem<ObjectRef> *node = list.GetFirst();
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

Controllable::Controllable( void ) : ConstructObject(Controllable)
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

FollowCamera::FollowCamera( void ) : ConstructObject(FollowCamera), m_follow()
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
	mtlItem< mtlShared<mtlString> > *node = list.GetFirst();
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
	if (a.IsNull() || !a.GetShared()->LoadGraphics<Sprite>("../tmp/test.sprite")) {
		std::cout << "\tfailed to load" << std::endl;
		return;
	}
	a.GetShared()->LoadCollider<PolygonCollider>();
	a->GetTransform().Scale(2.0f);

	ObjectRef b = engine.AddObject<NPC>();
	b.GetShared()->SetName("NPC");
	if (b.IsNull() || !b.GetShared()->LoadGraphics<Image>("../tmp/test.png")) {
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

void Unit_ArrayResize( void )
{
	std::cout << "Unit_ArrayResize: ";

	mtlArray<int> arr;
	arr.poolMemory = true;
	arr.SetCapacity(1024);
	arr.Resize(512);

	if (arr.GetCapacity() != 1024 || arr.GetSize() != 512) {
		std::cout << "failed" << std::endl;
		return;
	}

	std::cout << "success" << std::endl;
}

void Unit_GUI(Engine &engine)
{
	std::cout << "Unit_GUI: ";
	engine.SetClearColor(1.0f, 0.0f, 1.0f);
	engine.DestroyAllObjects();
	engine.AddObject<Object>();
	mtlShared<GUI::Form> form = engine.AddForm<GUI::Form>();
	mtlShared<GUI::Control> label = form->AddControl<GUI::Label>();
	dynamic_cast<GUI::Label*>(label.GetShared())->SetLabel("A label");
	engine.RunGame();
	std::cout << "success" << std::endl;
}

void Unit_RandomFloat(Engine &engine)
{
	std::cout << "Unit_RandomFloat: " << std::endl;
	std::cout << "\tuniform: " ;
	for (int i = 0; i < 10; ++i) {
		std::cout << engine.GetRandomUniform() << " ";
	}
	std::cout << std::endl << "\traised cos: " ;
	for (int i = 0; i < 10; ++i) {
		std::cout << engine.GetRandomRaisedCos() << " ";
	}
	std::cout << std::endl;
}

void Unit_Font(Engine &engine)
{
	engine.AddObject("Quitter");
	engine.AddObject("FontRenderer");
	engine.RunGame();
}
