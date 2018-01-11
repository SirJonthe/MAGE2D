#include "Engine.h"
#include "GUI.h"
#include "Object.h"
#include "Image.h"
#include "Tools.h"
#include "Schedule.h"
#include "MiniLib/MTL/mtlString.h"
#include "MiniLib/MTL/mtlStringMap.h"
#include <iostream>

// BUG: mtlShared<a> = mtlShared<b> (can't access the other class' private members)
// Investigate if the font rendering is fixed by using unsigned ints as UVs rather than floats

// BIG BUGS
	// mtlShared< ColliderGraphics<PolygonCollider> > -> SetHaflExtents(float, float) not defined -> this SHOULD be virtual, and is defined in the base class
	// Polygon collisions are OFF, try disabling collision response/resolution and move the boxes around in PhysicsTest

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
		GUI::Print(GetEngine()->GetMousePosition(), 2);
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
			GUI::Print(walker.GetX(), 2);
			GUI::Print(", ", 2);
			GUI::Print(walker.GetY(), 2);
			GUI::Print(" - ", 2);
			GUI::Print(rect.x, 2);
			GUI::Print(", ", 2);
			GUI::Print(rect.y, 2);
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
	Quitter( void ) : ConstructObject(Quitter) { SetName("Quitter"); }
};

ObjectDeclaration(TimerObject)
{
private:
	Timer m_timer;
protected:
	void OnUpdate( void )
	{
		if (GetEngine()->IsPressed(SDLK_SPACE)) {
			m_timer.Toggle();
		}
		if (GetEngine()->IsPressed(SDLK_RETURN)) {
			m_timer.DecrementBeat();
		}
		if (GetEngine()->IsPressed(SDLK_UP)) {
			m_timer.SetTempo(m_timer.GetTempo(Timer::BeatsPerSecond) + 1.0f, Timer::BeatsPerSecond);
			std::cout << m_timer.GetTempo(Timer::SecondsPerBeat) << std::endl;
		} else if (GetEngine()->IsPressed(SDLK_DOWN)) {
			m_timer.SetTempo(m_timer.GetTempo(Timer::BeatsPerSecond) / 2.0f, Timer::BeatsPerSecond);
		}
	}
	void OnGUI( void )
	{
		GUI::Print(m_timer.GetTime(), 2);
	}
	void OnInit( void )
	{
		MakeRulesetObject();
		SetName("object_timer");
		m_timer.SetTempo(1.0f, Timer::BeatsPerSecond);
	}
public:
	TimerObject( void ) : ConstructObject(TimerObject) {}
};

ObjectDeclaration(PhysicsObject)
{
private:
	bool m_colliding;

protected:
	void OnInit( void )
	{
		LoadCollider<PolygonCollider>();
		GetCollider()->SetHalfExtents(50.0f, 30.0f);
		EnablePhysics();
		EnableCollisions();
	}

	void OnCollision(ObjectRef obj, const CollisionInfo &info)
	{
		GetEngine()->SetGUIProjection();

		GUI::SetColor(GetGraphics().GetTint());
		GUI::Print(GetName(), 2);
		GUI::SetColor(1.0f, 1.0f, 1.0f);
		GUI::Print(" collided with ", 2);
		GUI::SetColor(obj->GetGraphics().GetTint());
		GUI::Print(obj->GetName(), 2);
		GUI::NewLine();

		GetEngine()->SetGameProjection();

		for (int i = 0; i < info.points->GetSize(); ++i) {
			GUI::SetColor(1.0f, 0.0f, 1.0f);
			Rect r;
			r.x = int((*info.points.GetShared())[i][0] - 2.0f);
			r.y = int((*info.points.GetShared())[i][1] - 2.0f);
			r.w = 4;
			r.h = 4;
			GUI::Box(r);
		}

		GUI::SetColor(1.0f, 0.0f, 0.0f);
		Rect r;
		r.x = int(info.avg_collision[0] - 2.0f);
		r.y = int(info.avg_collision[1] - 2.0f);
		r.w = 4;
		r.h = 4;
		GUI::Box(r);
	}

public:
	PhysicsObject( void ) : ConstructObject(PhysicsObject)
	{
		SetName("PhysicsObject");
		EnableDebugGraphics();
	}
};

ObjectDeclaration(PhysicsTest)
{
private:
	static const int NUM_BOX = 15;
	ObjectRef      m_box[NUM_BOX];
	Physics::Force m_ray;
	Point          m_mouse_pt;
	bool           m_establishing_force;

protected:
	void OnInit( void )
	{	
		for (int i = 0; i < NUM_BOX; ++i) {
			m_box[i] = GetEngine()->AddObject<PhysicsObject>();
			m_box[i]->GetTransform().SetPosition(Transform::Local, -GetEngine()->GetVideoWidth() / 2 + 75 + 150 * (i % 5), -GetEngine()->GetVideoHeight() / 2 + 50 + 100 * (i / 5));
			mtlString name;
			name.Append("BOX").AppendInt(i+1);
			m_box[i]->SetName(name);
			m_box[i]->GetGraphics().SetTint(GetEngine()->GetRandomUniform(), GetEngine()->GetRandomUniform(), GetEngine()->GetRandomUniform());
		}
		m_establishing_force = false;
	}

	void OnUpdate( void )
	{
		mmlVector<2> world_mouse = GetEngine()->GetWorldMousePosition();

		if (GetEngine()->IsPressed(SDLK_LSHIFT)) {
			m_ray.origin = world_mouse;
			m_mouse_pt.x = GetEngine()->GetMousePosition().x;
			m_mouse_pt.y = GetEngine()->GetMousePosition().y;
			m_establishing_force = true;
		}

		if (m_establishing_force) {
			GUI::SetColor(1.0f, 0.0f, 0.0f);
			GUI::Arrow(m_mouse_pt, GetEngine()->GetMousePosition(), 10);
		}

		m_ray.force = (world_mouse - m_ray.origin).Len();
		m_ray.direction = (world_mouse - m_ray.origin) / m_ray.force;
		m_ray.force *= 0.2f;

		if (GetEngine()->IsReleased(SDLK_LSHIFT)) {
			m_establishing_force = false;
			for (int i = 0; i < NUM_BOX; ++i) {
				m_box[i]->GetPhysics().UnlockPosition();
				m_box[i]->GetPhysics().UnlockRotation();
				m_box[i]->ApplyForce(m_ray, 100.0f);
			}
		}

		if (GetEngine()->IsPressed(SDLK_r)) {
			for (int i = 0; i < NUM_BOX; ++i) {
				m_box[i]->GetPhysics().ResetForce();
				m_box[i]->GetTransform().SetRotation(Transform::Local, 0.0f);
				m_box[i]->GetTransform().SetPosition(Transform::Local, -GetEngine()->GetVideoWidth() / 2 + 75 + 150 * (i % 5), -GetEngine()->GetVideoHeight() / 2 + 50 + 100 * (i / 5));
			}
		}
		if (GetEngine()->IsPressed(SDLK_SPACE)) {
			for (int i = 0; i < NUM_BOX; ++i) {
				m_box[i]->GetPhysics().ResetForce();
				m_box[i]->GetPhysics().LockPosition();
				m_box[i]->GetPhysics().LockRotation();
			}
		}

		if (GetEngine()->IsDown(SDLK_w)) {
			m_box[0]->GetTransform().Translate(Transform::Local, 0.0f, -5.0f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_a)) {
			m_box[0]->GetTransform().Translate(Transform::Local, -5.0f * GetEngine()->GetElapsedTime(), 0.0f);
		}
		if (GetEngine()->IsDown(SDLK_s)) {
			m_box[0]->GetTransform().Translate(Transform::Local, 0.0f, 5.0f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_d)) {
			m_box[0]->GetTransform().Translate(Transform::Local, 5.0f * GetEngine()->GetElapsedTime(), 0.0f);
		}
		if (GetEngine()->IsDown(SDLK_q)) {
			m_box[0]->GetTransform().Rotate(0.01f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_e)) {
			m_box[0]->GetTransform().Rotate(-0.01f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_z)) {
			m_box[0]->GetPhysics().UnlockRotation();
			m_box[0]->GetPhysics().AddTorque(-0.001f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_x)) {
			m_box[0]->GetPhysics().UnlockRotation();
			m_box[0]->GetPhysics().AddTorque(0.001f * GetEngine()->GetElapsedTime());
		}

		if (GetEngine()->IsDown(SDLK_i)) {
			m_box[1]->GetTransform().Translate(Transform::Local, 0.0f, -5.0f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_j)) {
			m_box[1]->GetTransform().Translate(Transform::Local, -5.0f * GetEngine()->GetElapsedTime(), 0.0f);
		}
		if (GetEngine()->IsDown(SDLK_k)) {
			m_box[1]->GetTransform().Translate(Transform::Local, 0.0f, 5.0f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_l)) {
			m_box[1]->GetTransform().Translate(Transform::Local, 5.0f * GetEngine()->GetElapsedTime(), 0.0f);
		}
		if (GetEngine()->IsDown(SDLK_u)) {
			m_box[1]->GetTransform().Rotate(0.01f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_o)) {
			m_box[1]->GetTransform().Rotate(-0.01f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_m)) {
			m_box[0]->GetPhysics().UnlockRotation();
			m_box[1]->GetPhysics().AddTorque(-0.001f * GetEngine()->GetElapsedTime());
		}
		if (GetEngine()->IsDown(SDLK_n)) {
			m_box[0]->GetPhysics().UnlockRotation();
			m_box[1]->GetPhysics().AddTorque(0.001f * GetEngine()->GetElapsedTime());
		}
	}

	void OnGUI( void )
	{
		GUI::SetColor(0.0f, 1.0f, 1.0f);
		if (m_establishing_force) {
			GUI::Print("o = [ ", 2);
			GUI::Print(m_ray.origin[0], 2);
			GUI::Print(", ", 2);
			GUI::Print(m_ray.origin[1], 2);
			GUI::Print(" ]", 2);
			GUI::NewLine();

			GUI::Print("d = [ ", 2);
			GUI::Print(m_ray.direction[0], 2);
			GUI::Print(", ", 2);
			GUI::Print(m_ray.direction[1], 2);
			GUI::Print(" ]", 2);
			GUI::NewLine();

			GUI::Print("f = ", 2);
			GUI::Print(m_ray.force, 2);
		} else {
			mmlVector<2> world_mouse = GetEngine()->GetWorldMousePosition();
			GUI::Print("o = [ ", 2);
			GUI::Print(world_mouse[0], 2);
			GUI::Print(", ", 2);
			GUI::Print(world_mouse[1], 2);
			GUI::Print(" ]", 2);
		}
		GUI::NewLine();
	}

public:
	PhysicsTest( void ) : ConstructObject(PhysicsTest)
	{
		SetName("PhysicsTest");
		m_establishing_force = false;
		MakeRulesetObject();
	}
};

RegisterObject(Controllable);
RegisterObject(Anchor);
RegisterObject(FollowCamera);
RegisterObject(NPC);
RegisterObject(FontRenderer);
RegisterObject(Quitter);
RegisterObject(GridRender);
RegisterObject(TimerObject);
RegisterObject(PhysicsObject);
RegisterObject(PhysicsTest);

void PrintString(const mtlChars &ch);

void RunUnitTests(Engine &engine);
void Unit_OpenGLTest( void );
void Unit_RegisteredObjects( void );
void Unit_Controllable(Engine &engine);
void Unit_StringMap( void );
void Unit_ArrayResize( void );
void Unit_GUI(Engine &engine);
void Unit_RandomFloat(Engine &engine);
void Unit_Font(Engine &engine);
void Unit_Collisions(Engine &engine);
void Unit_Math( void );

int main(int argc, char **argv)
{
	Unit_Math();
	Engine engine;
	engine.Init(800, 600, false, "Lots-o-tests", argc, argv);
	//engine.AddObject<SpriteEditor>();
	//engine.RunGame();
	RunUnitTests(engine);
	return 0;
}

void RunUnitTests(Engine &engine)
{
//	Unit_OpenGLTest();
//	Unit_RegisteredObjects();
//	Unit_Controllable(engine);
//	Unit_StringMap();
//	Unit_ArrayResize();
//	Unit_GUI(engine);
//	Unit_RandomFloat(engine);
//	Unit_Font(engine);
	Unit_Collisions(engine);
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
	GUI::Print("Object={", 2);
	GUI::NewLine();
	for (int i = 0; i < 2; ++i) {
		GUI::Print("\t", 2);
		for (int j = 0; j < 2; ++j) {
			GUI::Print(r[i][j], 2);
			GUI::Print(";", 2);
		}
		GUI::NewLine();
	}
	GUI::Print("}", 2);
	GUI::NewLine();
	if (GetTransform().GetParent() == NULL) {
		GUI::SetColor(1.0f, 0.0f, 0.0f);
		GUI::Print("DETACHED", 2);
	} else {
		GUI::SetColor(0.0f, 1.0f, 0.0);
		GUI::Print("Attached", 2);
	}
	GUI::NewLine();
}

void Controllable::OnCollision(ObjectRef, const CollisionInfo&)
{
	GUI::SetColor(1.0f, 0.0f, 0.0f);
	GUI::Print("OUCH!", 2);
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
	GUI::Print("Camera=", 2);
	GUI::Print(p[0]); GUI::Print(";", 2); GUI::Print(p[1], 2);
	GUI::NewLine();
}

FollowCamera::FollowCamera( void ) : ConstructObject(FollowCamera), m_follow()
{
	SetName("object_camera");
}

void PrintString(const mtlChars &ch)
{
	if (ch.GetChars() != NULL) {
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

class Task1 : public ScheduleTask
{
public:
	virtual void operator()(Object*)
	{
		std::cout << "\t1" << std::endl;
	}
};

class Task2 : public ScheduleTask
{
public:
	virtual void operator()(Object*)
	{
		std::cout << "\t2" << std::endl;
	}
};

class Task3 : public ScheduleTask
{
public:
	virtual void operator()(Object*)
	{
		std::cout << "\t3" << std::endl;
	}
};

void Unit_Collisions(Engine &engine)
{
	std::cout << "Unit_Collisions: " << std::endl;
	engine.AddObject<PhysicsTest>();
	engine.AddObject<Quitter>();
	engine.SetCamera(engine.AddObject<DebugCamera>());
	engine.RunGame();
	std::cout << "[done]" << std::endl;
}

void Unit_Math( void )
{
	std::cout << "Unit_Math: " << std::endl;
	std::cout << "  pi=" << mmlPi<20,float>() << std::endl;
	std::cout << "     " << mmlPI << std::endl;
	std::cout << "[done]" << std::endl;
}
