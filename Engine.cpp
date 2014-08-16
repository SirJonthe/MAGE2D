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
	if (SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, SDL_QUITMASK)) {
		EndGame();
	} else {
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
					if ((abCollision > 0 || baCollision > 0) && object->GetItem()->m_collider.GetShared()->Collides(*nextObject->GetItem()->m_collider.GetShared())) {
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
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking() && object->GetItem()->IsVisible()) {

			Engine::SetGameProjection();
			SetGameView(object->GetItem());

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
			Engine::SetGUIProjection();
			Engine::SetGUIView();
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
			object->GetItem()->OnDestroy();
			object->GetItem()->m_engine = NULL;
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

void Engine::SetGameProjection( void )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const GLdouble halfW = GLdouble(SDL_GetVideoSurface()->w) / 2.0;
	const GLdouble halfH = GLdouble(SDL_GetVideoSurface()->h) / 2.0;
	glOrtho(-halfW - 0.5, halfW + 0.5, halfH + 0.5, -halfH - 0.5, 0.0, 1.0);
}

void Engine::SetGameView(const Object *object)
{
	mmlMatrix<3,3> cameraView = m_camera != NULL ? mmlInv(m_camera->GetTransform().GetTransformMatrix(Transform::Global)) : mmlMatrix<3,3>::IdentityMatrix();
	const mmlMatrix<3,3> objectTransform = object != NULL ? object->GetTransform().GetTransformMatrix(Transform::Global) : mmlMatrix<3,3>::IdentityMatrix();
	const mmlMatrix<3,3> f = cameraView * objectTransform;
	GLfloat m[16] = {
		f[0][0], f[0][1], 0.0f, 0.0f,
		f[1][0], f[1][1], 0.0f, 0.0f,
		0.0f,    0.0f,    1.0f, 0.0f,
		f[0][2], f[1][2], 0.0f, 1.0f
	};

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m);
}

void Engine::SetGUIProjection( void )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, (SDL_GetVideoSurface()->w - 1) + 0.5, (SDL_GetVideoSurface()->h - 1) + 0.5, -0.5, 0.0, 1.0);
}

void Engine::SetGUIView( void )
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Engine::SetGameGUIView(const Object *object)
{
	mmlMatrix<3,3> cameraView = m_camera != NULL ? mmlInv(m_camera->GetTransform().GetTransformMatrix(Transform::Global)) : mmlMatrix<3,3>::IdentityMatrix();
	const mmlMatrix<3,3> objectTransform = object != NULL ? object->GetTransform().GetTransformMatrix(Transform::Global) : mmlMatrix<3,3>::IdentityMatrix();
	const mmlMatrix<3,3> f = cameraView * objectTransform;
	GLfloat m[16] = {
		1.0f,    0.0f,    0.0f, 0.0f,
		0.0f,    1.0f,    0.0f, 0.0f,
		0.0f,    0.0f,    1.0f, 0.0f,
		f[0][2], f[1][2], 0.0f, 1.0f
	};

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m);
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

Engine::Engine( void ) :
	m_objects(), m_camera(NULL),
	m_timer(60.0f), m_deltaSeconds(0.0f),
	m_rseed_z(0), m_rseed_w(0),
	m_quit(false), m_inLoop(false),
	m_music(NULL)
{
	SetRandomizerSeeds(0, 0); // grabs default values
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
	Engine::SetGameProjection();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	GUI::Init();

	return true;
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

void Engine::FilterByObjectFlags(const mtlList<Object*> &in, mtlList<Object*> &out, flags_t mask)
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

void Engine::FilterByCollisionMasks(const mtlList<Object*> &in, mtlList<Object*> &out, flags_t mask)
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

void Engine::FilterByObjectFlagsInclusive(const mtlList<Object*> &in, mtlList<Object*> &out, flags_t mask)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetObjectFlags(mask) > 0) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByCollisionMasksInclusive(const mtlList<Object*> &in, mtlList<Object*> &out, flags_t mask)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetCollisionMasks(mask) > 0) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByRange(const mtlList<Object*> &in, mtlList<Object*> &out, const Range &range, flags_t collisionMask)
{
	out.RemoveAll();
	mtlNode<Object*> *object = in.GetFirst();
	while (object != NULL) {
		if ((object->GetItem()->m_objectFlags & collisionMask) > 0) {
			RangeCollisionInfo info = RangeCollide(range, object->GetItem()->GetTransform().GetPosition(Transform::Global));
			if (info.collision) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByPlane(const mtlList<Object*> &in, mtlList<Object*> &out, const Plane &plane, flags_t collisionMask)
{
	out.RemoveAll();
	mtlNode<Object*> *object = in.GetFirst();
	while (object != NULL) {
		if ((object->GetItem()->m_objectFlags & collisionMask) > 0) {
			PlaneCollisionInfo info = PlaneCollide(plane, object->GetItem()->GetTransform().GetPosition(Transform::Global));
			if (info.collision) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByRayCollision(const mtlList<Object*> &in, mtlList<Object*> &out, const Ray &ray, flags_t collisionMask)
{
	/*struct DistanceStruct
	{
		Object *object;
		float distance;
		bool operator<(DistanceStruct d) const { return distance < d.distance; }
	};
	out.RemoveAll();
	mtlArray<DistanceStruct> collisionInfo;
	collisionInfo.SetCapacity(in.GetSize());
	mtlNode<Object*> *object = in.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->m_collisions) {
			RayCollisionInfo info = object->GetItem()->GetCollider()->Collides(ray);
			if (info.collision) {
				DistanceStruct d;
				d.object = object->GetItem();
				d.distance = mmlDist(ray.origin - d.contact);
				collisionInfo.Add(d);
			}
		}
		object = object->GetNext();
	}
	if (collisionInfo.GetSize() > 0) {
		mtlArray<DistanceStruct> sorted;
		collisionInfo.MergeSort(sorted);
		for (int i = 0; i < sorted.GetSize(); ++i) {
			out.AddLast(sorted[i].object);
		}
	}*/

	out.RemoveAll();
	mtlNode<Object*> *object = in.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->m_collisions) {
			if (object->GetItem()->GetCollider()->Collides(ray)) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByRangeCollision(const mtlList<Object*> &in, mtlList<Object*> &out, const Range &range, flags_t collisionMask)
{
	out.RemoveAll();
	mtlNode<Object*> *object = in.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->m_collisions) {
			if (object->GetItem()->GetCollider()->Collides(range)) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByPlaneCollision(const mtlList<Object*> &in, mtlList<Object*> &out, const Plane &plane, flags_t collisionMask)
{
	out.RemoveAll();
	mtlNode<Object*> *object = in.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->m_collisions) {
			object->GetItem()->GetCollider()->Collides(plane);
			out.AddLast(object->GetItem());
		}
		object = object->GetNext();
	}
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

void Engine::SetRandomizerSeeds(unsigned int z, unsigned int w)
{
	// must be non-zero
	m_rseed_z = z == 0 ? 0xfeedface : z;
	m_rseed_w = w == 0 ? 0xdeadbeef : w;
}

unsigned int Engine::GetRandomUint( void )
{
	m_rseed_z = 36969 * (m_rseed_z & 65535) + (m_rseed_z >> 16);
	m_rseed_w = 18000 * (m_rseed_w & 65535) + (m_rseed_w >> 16);
	return (m_rseed_z << 16) + m_rseed_w;
}

unsigned int Engine::GetRandomUint(unsigned int min, unsigned int max)
{
	unsigned int r = GetRandomUint();
	return (r % (max-min+1)) + min;
}

int Engine::GetRandomInt( void )
{
	return (int)GetRandomUint();
}

int Engine::GetRandomInt(int min, int max)
{
	int r = GetRandomUint();
	return (r % (max-min+1)) + min;
}

float Engine::GetRandomUniform( void )
{
	const float invRand = 1.0f / float(0xffffffff);
	return float(GetRandomUint()) * invRand;
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

void Engine::UpdateVideo( void )
{
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
}

int Engine::GetVideoWidth( void )
{
	return SDL_GetVideoSurface()->w;
}

int Engine::GetVideoHeight( void )
{
	return SDL_GetVideoSurface()->h;
}

mtlAsset<Sound> Engine::LoadSound(const mtlChars &file)
{
	return mtlAsset<Sound>::Load(file);
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
	SDL_WarpMouse((Uint16)x, (Uint16)y); // have to do additional computations, I don't want SetMousePosition to affect relative mouse movement
}

void Engine::SetMousePosition(Point p)
{
	SetMousePosition(p.x, p.y);
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
