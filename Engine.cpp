#include <iostream>
#include "Engine.h"
#include "Object.h"
#include "GUI.h"
#include "MML/mmlMath.h"

/*void Engine::GenerateEventList( void )
{
	m_events.RemoveAll();
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEMOTION:
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			m_events.AddLast(event);
			break;
		case SDL_QUIT: m_quit = true; break;
		}
	}
}*/

void Engine::UpdateInputBuffers( void )
{
	SDL_PumpEvents();
	Uint8 *keys = SDL_GetKeyState(NULL);
	for (int i = 0; i < SDLK_LAST; ++i) {
		m_keyState[i] = ((m_keyState[i] << 1) | keys[i]) & 3;
	}
	m_prevMousePosition = m_mousePosition;
	Uint8 mouse = SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
	for (int i = 0; i < MouseButton::Last; ++i) {
		m_mouseButtonState[i] = ((m_mouseButtonState[i] << 1) | ((mouse >> i) & 1)) & 3;
	}
}

void Engine::UpdateObjects( void )
{
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking()) {
			object->GetItem()->OnUpdate();
		}
		object = object->GetNext();
	}
}

void Engine::CollideObjects( void )
{
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking() && object->GetItem()->IsCollidable()) {
			mtlNode<Object*> *nextObject = object->GetNext();
			while (nextObject != NULL) {
				if (nextObject->GetItem()->IsTicking() && nextObject->GetItem()->IsCollidable()) {
					unsigned int abCollision = object->GetItem()->GetCollisionMasks() & nextObject->GetItem()->GetObjectFlags();
					unsigned int baCollision = nextObject->GetItem()->GetObjectFlags() & object->GetItem()->GetCollisionMasks();
					if ((abCollision > 0 || baCollision > 0) && Collide(object->GetItem(), nextObject->GetItem())) {
						if (abCollision > 0) {
							object->GetItem()->OnCollision(*nextObject->GetItem());
						}
						if (baCollision > 0) {
							nextObject->GetItem()->OnCollision(*object->GetItem());
						}
					}
				}
				nextObject = nextObject->GetNext();
			}
		}
		object = object->GetNext();
	}
}

void Engine::DrawObjects( void )
{
	Engine::SetGameView();

	//const mmlMatrix<3,3> viewTransform = (m_camera != NULL) ? (mmlInv(m_camera->GetTransform().GetWorldTransform())) : (mmlMatrix<3,3>::IdentityMatrix());
	const mmlMatrix<2,2> rotation = m_camera != NULL ? mmlInv(m_camera->GetTransform().GetWorldRotation()) : mmlMatrix<2,2>::IdentityMatrix();
	const mmlVector<2> position = m_camera != NULL ? -m_camera->GetTransform().GetWorldPosition() : mmlVector<2>(0.0f, 0.0f);

	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking() && object->GetItem()->IsVisible()) {
			//const mmlMatrix<3,3> t = object->GetItem()->GetTransform().GetWorldTransform() * viewTransform;
			const mmlMatrix<2,2> r = object->GetItem()->GetTransform().GetWorldRotation() * rotation;
			const mmlVector<2> p = (object->GetItem()->GetTransform().GetWorldPosition() + position) * r;
			/*GLfloat m[16] = {
				t[0][0], t[0][1], 0.0f, 0.0f,
				t[1][0], t[1][1], 0.0f, 0.0f,
				0.0f,    0.0f,    1.0f, 0.0f,
				t[0][2], t[1][2], 0.0f, 1.0f
			};*/
			/*GLfloat m[16] = {
				t[0][0], t[1][0], 0.0f, 0.0f,
				t[0][1], t[1][1], 0.0f, 0.0f,
				0.0f,    0.0f,    1.0f, 0.0f,
				t[0][2], t[1][2], 0.0f, 1.0f
			};*/
			GLfloat m[16] = {
				r[0][0], r[1][0], 0.0f, 0.0f,
				r[0][1], r[1][1], 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				p[0], p[1], 0.0f, 1.0f
			};
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(m);
			glColor4f(
				object->GetItem()->GetGraphics().GetRed(),
				object->GetItem()->GetGraphics().GetGreen(),
				object->GetItem()->GetGraphics().GetBlue(),
				object->GetItem()->GetGraphics().GetAlpha()
			);
			object->GetItem()->OnDraw();
		}
		object = object->GetNext();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Engine::DrawGUI( void )
{
	GUI::SetCaretXY(0, 0);
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking()) {
			GUI::SetGUIView();
			GUI::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			object->GetItem()->OnGUI();
		}
		object = object->GetNext();
	}
}

void Engine::FinalizeObjects( void )
{
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking()) {
			object->GetItem()->OnFinal();
		}
		object = object->GetNext();
	}
}

void Engine::DestroyObjects( void )
{
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->m_destroy) {
			if (m_camera == object->GetItem()) { m_camera = NULL; }
			delete object->GetItem();
			object = object->Remove();
		} else {
			object = object->GetNext();
		}
	}
	if (m_objects.GetSize() == 0) {
		std::cout << "[EMPTY object list]" << std::endl;
		EndGame();
	}
}

void Engine::UpdateTimer( void )
{
	if (m_timer.GetTimeDeltaTick() < 1.0f) {
		SDL_Delay(Uint32((1000.0f / 60.0f) * (1.0f - m_timer.GetTimeDeltaTick())));
	}
	m_timer.Tick();
	m_deltaSeconds = m_timer.GetTimeDeltaSecondsTick();
}

bool Engine::Collide(const Object *a, const Object *b) const
{
	/*Box ab = ToBox(a->GetCollisionBox());
	Box bb = ToBox(b->GetCollisionBox());

	Box overlap;
	overlap.a.x = mmlMax2(ab.a.x, bb.a.x);
	overlap.a.y = mmlMax2(ab.a.y, bb.a.y);
	overlap.b.x = mmlMin2(ab.b.x, bb.b.x);
	overlap.b.y = mmlMin2(ab.b.y, bb.b.y);

	return (overlap.a.x < overlap.b.x && overlap.a.y < overlap.b.y && GetPixelOverlap(a, b, overlap));*/
	return false;
}

bool Engine::PointInBox(Point p, Engine::Box b) const
{
	return (p.x >= b.a.x && p.x < b.b.x && p.y >= b.a.y && p.y < b.b.y);
}

Engine::Box Engine::ToBox(Rect r) const
{
	Box b = { { r.x, r.y }, { r.x+r.w, r.y+r.h } };
	return b;
}

bool Engine::GetPixelOverlap(const Object *a, const Object *b, Box o) const
{
	//a->
	/*int ax = o.a.x - int(a->GetPosition()[0]);
	int ay = o.a.y - int(a->GetPosition()[1]);
	int bx = o.a.x - int(b->GetPosition()[0]);
	int by = o.a.y - int(b->GetPosition()[1]);
	int w = o.b.x - o.a.x;
	int h = o.b.y - o.a.y;

	const Image *ai = &a->GetCurrentImage();
	const Image *bi = &b->GetCurrentImage();

	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (ai->IsColorKey(x+ax,y+ay, a->GetOrientationX()) == false && bi->IsColorKey(x+bx,y+by, b->GetOrientationX()) == false) {
				return true;
			}
		}
	}*/

	return false;
}

void Engine::SetGameView( void )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-0.5, (SDL_GetVideoSurface()->w - 1) + 0.5, (SDL_GetVideoSurface()->h - 1) + 0.5, -0.5, 0.0, 1.0);
	const GLdouble halfW = float(SDL_GetVideoSurface()->w) / 2.0;
	const GLdouble halfH = float(SDL_GetVideoSurface()->h) / 2.0;
	glOrtho(-halfW - 0.5, halfW + 0.5, halfH + 0.5, -halfH - 0.5, 0.0, 1.0);
}

mtlBinaryTree<Engine::TypeNode> &Engine::GetTypeTree( void )
{
	static mtlBinaryTree<TypeNode> typeTree; // should be a mtlStringMap
	return typeTree;
}

void Engine::GetRegisteredTypes(const mtlBranch<TypeNode> *branch, mtlList< mtlShared<mtlString> > &types)
{
	if (branch == NULL) { return; }
	if (branch->GetNegative() != NULL) {
		GetRegisteredTypes(branch->GetNegative(), types);
	}
	mtlNode<Type> *node = branch->GetItem().types.GetShared()->GetFirst();
	while (node != NULL) {
		mtlShared<mtlString> name;
		name.New();
		name.GetShared()->Copy(node->GetItem().name);
		types.AddLast(name);
		node = node->GetNext();
	}
	if (branch->GetPositive() != NULL) {
		GetRegisteredTypes(branch->GetPositive(), types);
	}
}

Engine::Engine( void ) : m_objects(), m_camera(NULL), /*m_events(),*/ m_timer(60.0f), m_deltaSeconds(0.0f), m_quit(false), m_inLoop(false), m_music(NULL)
{
	m_mousePosition.x = 0;
	m_mousePosition.y = 0;
	m_prevMousePosition.x = 0;
	m_prevMousePosition.y = 0;
	for (int i = 0; i < SDLK_LAST; ++i) {
		m_keyState[i] = InputState::None;
	}
	for (int i = 0; i < MouseButton::Last; ++i) {
		m_mouseButtonState[i] = InputState::None;
	}
}

Engine::~Engine( void )
{
	if (SDL_GetVideoSurface() != NULL) {
		SDL_FreeSurface(SDL_GetVideoSurface());
	}
	StopMusic();
	Mix_CloseAudio();
	GUI::Destroy();
	SDL_Quit();
}

bool Engine::Init(int argc, char **argv)
{
	std::cout << "Engine::Init: " << std::endl;
	struct Args
	{
		int width, height;
		int fullscreen;
	};

	srand(1); // make things predictably random

	Args args;
	args.width = 800;
	args.height = 600;
	args.fullscreen = false;

	for (int i = 1; i < argc; ++i) {
		std::cout << "Argument " << argc << ": " << argv[i] << std::endl;
		if (strcmp(argv[i], "-width") == 0 && i < argc-1) {
			args.width = mmlMax2(atoi(argv[++i]), 0);
		}
		else if (strcmp(argv[i], "-height") == 0 && i < argc-1) {
			args.height = mmlMax2(atoi(argv[++i]), 0);
		}
		else if (strcmp(argv[i], "-fullscreen") == 0 && i < argc-1) {
			args.fullscreen = mmlClamp(0, atoi(argv[++i]), 1);
		}
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cout << "\tfailed: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (SDL_SetVideoMode(args.width, args.height, 32, SDL_OPENGL) == NULL) {
		std::cout << "\tfailed: " << SDL_GetError() << std::endl;
		return false;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
		std::cout << "\tfailed: " << Mix_GetError() << std::endl;
		return false;
	}

	glViewport(0, 0, SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // means we can apply color to the textures we are rendering using glColor3

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Engine::SetGameView();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	GUI::Init();

	return true;
}

void Engine::AddObject(Object *object)
{
	if (object == NULL) {
		std::cout << "Engine::AddObject: Object is NULL" << std::endl;
		return;
	} else if (object->m_engine != NULL) {
		std::cout << "Engine::AddObject: Object is already attached to an engine" << std::endl;
		return;
	}
	m_objects.AddLast(object);
	object->m_engine = this;
	object->OnInit();
}

Object *Engine::GetCamera( void )
{
	return m_camera;
}

const Object *Engine::GetCamera( void ) const
{
	return m_camera;
}

void Engine::SetCamera(Object *camera)
{
	if (camera == NULL || camera->m_engine != this) {
		m_camera = NULL;
	} else {
		m_camera = camera;
	}
}

void Engine::DestroyAllObjects( void )
{
	mtlNode<Object*> *object = m_objects.GetFirst();
	if (m_inLoop) {
		while (object != NULL) {
			object->GetItem()->Destroy();
			object = object->GetNext();
		}
	} else {
		m_inLoop = true; // prevent some stupid object from recursively calling DestroyAllObjects
		while (object != NULL) {
			delete object->GetItem();
			object = object->GetNext();
		}
		m_objects.RemoveAll();
		m_inLoop = false;
	}
}

void Engine::SetWindowCaption(const mtlChars &caption)
{
	SDL_WM_SetCaption(caption.GetChars(), NULL);
}

int Engine::RunGame( void )
{
	if (m_inLoop) { return 0; } // some stupid object is recursively calling RunGame

	std::cout << "[START main loop]" << std::endl;
	if (m_camera == NULL) {
		std::cout << "Engine::RunGame: No camera at game start" << std::endl;
	}

	m_quit = false;
	m_inLoop = true;

	m_timer.Restart();

	while (!m_quit) {
		//GenerateEventList();
		UpdateInputBuffers();
		UpdateObjects();
		CollideObjects();
		DrawObjects();
		DrawGUI();
		UpdateVideo();
		FinalizeObjects();
		DestroyObjects();
		UpdateTimer();
	}

	m_inLoop = false;

	std::cout << "[END main loop]" << std::endl;

	return 0;
}

void Engine::EndGame( void )
{
	m_quit = true;
	// don't set m_inLoop to false, since we are letting the engine finish one cycle
}

void Engine::KillProgram( void )
{
	std::cout << "[KILL program]" << std::endl;
	if (SDL_GetVideoSurface() != NULL) {
		SDL_FreeSurface(SDL_GetVideoSurface());
	}
	StopMusic();
	Mix_CloseAudio();
	SDL_Quit();
	GUI::Destroy();
	m_inLoop = false;
	m_quit = true;
	DestroyAllObjects();
	exit(0);
}

void Engine::SetUpdateFrequency(float updatesPerSecond)
{
	m_timer.SetIntervalsPerSecond(updatesPerSecond);
}

float Engine::GetDeltaTime( void ) const
{
	return m_deltaSeconds;
}

/*const mtlList<SDL_Event> &Engine::GetEventList( void ) const
{
	return m_events;
}*/

int Engine::GetRandomInt( void ) const
{
	return rand();
}

int Engine::GetRandomInt(int max) const
{
	return rand() % (max+1);
}

int Engine::GetRandomInt(int min, int max) const
{
	return GetRandomInt(max-min) + min;
}

float Engine::GetRandomFloat( void ) const
{
	const float invRand = 1.0f / float(RAND_MAX);
	return float(rand()) * invRand;
}

const mtlList<Object*> &Engine::GetObjects( void ) const
{
	return m_objects;
}

void Engine::FilterByName(const mtlList<Object *> &in, mtlList<Object *> &out, const mtlChars &name)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetName().Compare(name)) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByType(const mtlList<Object*> &in, mtlList<Object*> &out, TypeID id)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetInstanceType() == id) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByObjectFlags(const mtlList<Object*> &in, mtlList<Object*> &out, unsigned int mask)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetObjectFlags(mask) == mask) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByCollisionMasks(const mtlList<Object*> &in, mtlList<Object*> &out, unsigned int mask)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetCollisionMasks(mask) == mask) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByRay(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, mmlVector<2> direction)
{
	out.RemoveAll();
}

void Engine::FilterByCone(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, float apex)
{
	out.RemoveAll();
}

void Engine::FilterByBox(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> min, mmlVector<2> max)
{
	out.RemoveAll();
}

void Engine::FilterBySphere(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, float radius)
{
	out.RemoveAll();
}

void Engine::FilterByPlane(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> point, mmlVector<2> normal)
{
	out.RemoveAll();
}

bool Engine::PlayMusic(const mtlChars &file)
{
	std::cout << "Engine::PlayMusic: " << file.GetChars() << std::endl;
	StopMusic();
	m_music = Mix_LoadMUS(file.GetChars());
	if (m_music == NULL || Mix_PlayMusic(m_music, -1) == -1) {
		std::cout << "\tfailed: " << Mix_GetError() << std::endl;
		return false;
	}
	return true;
}

void Engine::StopMusic( void )
{
	if (m_music != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(m_music);
		m_music = NULL;
	}
}

void Engine::UpdateVideo( void ) const
{
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
}

Object *Engine::AddObject( void )
{
	return AddObject<Object>();
}

Object *Engine::AddObject(const mtlChars &typeName)
{
	mtlHash h(typeName);
	mtlBranch<TypeNode> *b = GetTypeTree().GetRoot();
	if (b != NULL) {
		b = b->Find(h);
	}
	if (b != NULL) {
		mtlNode<Type> *n = b->GetItem().types.GetShared()->GetFirst();
		while (n != NULL) {
			if (n->GetItem().name.Compare(typeName)) {
				Object *o = n->GetItem().creator_func();
				AddObject(o);
				return o;
			}
			n = n->GetNext();
		}
	}
	return NULL;
}

bool Engine::RegisterType(const mtlChars &typeName, Object *(*creator_func)())
{
	mtlHash h(typeName);
	mtlBranch<TypeNode> *b = GetTypeTree().GetRoot();
	if (b != NULL) {
		b = b->Find(h);
	}
	mtlNode<Type> *n = NULL;
	if (b != NULL) {
		n = b->GetItem().types.GetShared()->GetFirst();
		while (n != NULL) {
			if (n->GetItem().name.Compare(typeName)) { // this type is already registered
				return false;
			}
			n = n->GetNext();
		}
	}
	if (n == NULL) {
		if (b == NULL) {
			TypeNode tn;
			tn.hash = h;
			tn.types.New();
			b = GetTypeTree().Insert(tn);
		}
		Type t;
		t.name.Copy(typeName);
		t.creator_func = creator_func;
		b->GetItem().types.GetShared()->AddLast(t);
	}
	return true;
}

void Engine::GetRegisteredTypes(mtlList< mtlShared<mtlString> > &types)
{
	types.RemoveAll();
	GetRegisteredTypes(GetTypeTree().GetRoot(), types);
}

void Engine::PrintError(GLenum error)
{
	switch (error) {
	case GL_NO_ERROR:
		std::cout << "GL_NO_ERROR" << std::endl;
		break;
	case GL_INVALID_ENUM:
		std::cout << "GL_INVALID_ENUM" << std::endl;
		break;
	case  GL_INVALID_VALUE:
		std::cout << "GL_INVALID_VALUE" << std::endl;
		break;
	case  GL_INVALID_OPERATION:
		std::cout << "GL_INVALID_OPERATION" << std::endl;
		break;
	case  GL_STACK_OVERFLOW:
		std::cout << "GL_STACK_OVERFLOW" << std::endl;
		break;
	case  GL_STACK_UNDERFLOW:
		std::cout << "GL_STACK_UNDERFLOW" << std::endl;
		break;
	case  GL_OUT_OF_MEMORY:
		std::cout << "GL_OUT_OF_MEMORY" << std::endl;
		break;
	}
}

void Engine::PrintError( void )
{
	PrintError(glGetError());
}

Point Engine::GetMousePosition( void ) const
{
	return m_mousePosition;
}

Point Engine::GetMouseMovement( void ) const
{
	Point p;
	p.x = m_mousePosition.x	- m_prevMousePosition.x;
	p.y = m_mousePosition.y - m_prevMousePosition.y;
	return p;
}

void Engine::SetMousePosition(int x, int y)
{
	SDL_WarpMouse((Uint16)x, (Uint16)y);
}

void Engine::SetMousePosition(Point p)
{
	SDL_WarpMouse((Uint16)p.x, (Uint16)p.y);
}

bool Engine::IsDown(SDLKey key) const
{
	return m_keyState[key] != InputState::None;
}

bool Engine::IsUp(SDLKey key) const
{
	return m_keyState[key] == InputState::None;
}

bool Engine::IsPressed(SDLKey key) const
{
	return m_keyState[key] == InputState::Press;
}

bool Engine::IsHeld(SDLKey key) const
{
	return m_keyState[key] == InputState::Hold;
}

bool Engine::IsReleased(SDLKey key) const
{
	return m_keyState[key] == InputState::Release;
}

bool Engine::IsDown(MouseButton::Button button) const
{
	return m_mouseButtonState[button] != InputState::None;
}

bool Engine::IsUp(MouseButton::Button button) const
{
	return m_mouseButtonState[button] == InputState::None;
}

bool Engine::IsPressed(MouseButton::Button button) const
{
	return m_mouseButtonState[button] == InputState::Press;
}

bool Engine::IsHeld(MouseButton::Button button) const
{
	return m_mouseButtonState[button] == InputState::Hold;
}

bool Engine::IsReleased(MouseButton::Button button) const
{
	return m_mouseButtonState[button] == InputState::Release;
}
