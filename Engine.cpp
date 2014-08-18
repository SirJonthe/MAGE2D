#include <iostream>
#include "Engine.h"
#include "Object.h"
#include "GUI.h"
#include "MML/mmlMath.h"

#define node_ref(x) x->GetItem().GetShared()
#define ptr_ref(x) x.GetShared()

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
	mtlNode<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->IsTicking()) {
			node_ref(object)->OnUpdate();
		}
		object = object->GetNext();
	}
}

void Engine::CollideObjects( void )
{
	mtlNode<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->IsTicking() && node_ref(object)->IsCollidable()) {
			mtlNode<ObjectRef> *nextObject = object->GetNext();
			while (nextObject != NULL) {
				if (node_ref(nextObject)->IsTicking() && node_ref(nextObject)->IsCollidable()) {
					unsigned int abCollision = node_ref(object)->GetCollisionMasks() & node_ref(nextObject)->GetObjectFlags();
					unsigned int baCollision = node_ref(nextObject)->GetObjectFlags() & node_ref(object)->GetCollisionMasks();
					if ((abCollision > 0 || baCollision > 0) && node_ref(object)->m_collider.GetShared()->Collides(*node_ref(nextObject)->m_collider.GetShared())) {
						if (abCollision > 0) {
							node_ref(object)->OnCollision(*node_ref(nextObject));
						}
						if (baCollision > 0) {
							node_ref(nextObject)->OnCollision(*node_ref(object));
						}
					}
				}
				nextObject = nextObject->GetNext();
			}
		}
		object = object->GetNext();
	}
}

struct GraphicsContainer
{
	ObjectRef	object;
	Transform	transform;
	bool operator<(const GraphicsContainer &c) const { return transform.GetPosition(Transform::Local)[1] < c.transform.GetPosition(Transform::Local)[1]; }
};

void Engine::DrawObjects( void )
{
	if (m_graphicsSort) {
		mtlArray<GraphicsContainer> graphics;
		graphics.SetCapacity(m_objects.GetSize());

		mtlNode<ObjectRef> *object = m_objects.GetFirst();
		while (object != NULL) {
			if (node_ref(object)->IsTicking() && node_ref(object)->IsVisible()) {
				GraphicsContainer c = { object->GetItem(), node_ref(object)->GetTransform().GetIndependentTransform(Transform::Global) };
				graphics.Add(c);
			}
			object = object->GetNext();
		}

		mtlArray<GraphicsContainer> sortedGraphics;
		graphics.MergeSort(sortedGraphics);

		for (int i = 0; i < sortedGraphics.GetSize(); ++i) {

			Engine::SetGameProjection();
			SetGameView(sortedGraphics[i].transform);

			glColor4f(
				sortedGraphics[i].object.GetShared()->GetGraphics().GetRed(),
				sortedGraphics[i].object.GetShared()->GetGraphics().GetGreen(),
				sortedGraphics[i].object.GetShared()->GetGraphics().GetBlue(),
				sortedGraphics[i].object.GetShared()->GetGraphics().GetAlpha()
			);
			sortedGraphics[i].object.GetShared()->OnDraw();
		}

	} else {
		mtlNode<ObjectRef> *object = m_objects.GetFirst();
		while (object != NULL) {
			if (node_ref(object)->IsTicking() && node_ref(object)->IsVisible()) {

				Engine::SetGameProjection();
				SetGameView(node_ref(object)->GetTransform());

				glColor4f(
					node_ref(object)->GetGraphics().GetRed(),
					node_ref(object)->GetGraphics().GetGreen(),
					node_ref(object)->GetGraphics().GetBlue(),
					node_ref(object)->GetGraphics().GetAlpha()
				);
				node_ref(object)->OnDraw();
			}
			object = object->GetNext();
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Engine::DrawGUI( void )
{
	GUI::SetCaretXY(0, 0);
	mtlNode<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->IsTicking()) {
			Engine::SetGUIProjection();
			Engine::SetGUIView();
			GUI::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			node_ref(object)->OnGUI();
		}
		object = object->GetNext();
	}
}

void Engine::FinalizeObjects( void )
{
	mtlNode<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->IsTicking()) {
			node_ref(object)->OnFinal();
		}
		object = object->GetNext();
	}
}

void Engine::DestroyObjects( void )
{
	mtlNode<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_destroy) {
			if (m_camera.GetShared() == node_ref(object)) { m_camera.Delete(); }
			node_ref(object)->OnDestroy();
			node_ref(object)->m_ref = NULL;
			node_ref(object)->m_engine = NULL;
			object = object->Remove();
		} else {
			object = object->GetNext();
		}
	}
	if (m_objects.GetSize() == 0 && m_pending.GetSize() == 0) {
		std::cout << "[EMPTY object list]" << std::endl;
		EndGame();
	}
}

void Engine::InitPendingObjects( void )
{
	mtlNode<ObjectRef> *object = m_pending.GetFirst();
	while (object != NULL) {
		m_objects.AddLast(object->GetItem());
		node_ref(object)->m_ref = &m_objects.GetLast()->GetItem();
		node_ref(object)->OnInit();
		object = object->Remove();
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

void Engine::AddObject(ObjectRef object)
{
	if (object.GetShared() == NULL) {
		std::cout << "Engine::AddObject: Object is NULL" << std::endl;
		return;
	}
	object.GetShared()->m_engine = this;
	m_pending.AddLast(object);
	object.GetShared()->m_ref = &m_pending.GetLast()->GetItem();
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
	m_objects(), m_camera(),
	m_timer(60.0f), m_deltaSeconds(0.0f),
	m_rseed_z(0), m_rseed_w(0),
	m_quit(false), m_inLoop(false),
	m_graphicsSort(true),
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

const mtlList<ObjectRef> &Engine::GetObjects( void ) const
{
	return m_objects;
}

void Engine::FilterByName(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const mtlChars &name)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (node_ref(n)->GetName().Compare(name)) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, TypeID id)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (node_ref(n)->GetInstanceType() == id) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByObjectFlags(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (node_ref(n)->GetObjectFlags(mask) == mask) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByCollisionMasks(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (node_ref(n)->GetCollisionMasks(mask) == mask) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByObjectFlagsInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (node_ref(n)->GetObjectFlags(mask) > 0) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByCollisionMasksInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (node_ref(n)->GetCollisionMasks(mask) > 0) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByRange(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Range &range, flags_t objectMask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		// we do not check if object can collide since this is not technically collisions
		if ((node_ref(object)->m_objectFlags & objectMask) > 0) {
			UnaryCollisionInfo info = RangeCollide(range, node_ref(object)->GetTransform().GetPosition(Transform::Global));
			if (info.collision) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByPlane(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t objectMask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		// we do not check if object can collide since this is not technically collisions
		if ((node_ref(object)->m_objectFlags & objectMask) > 0) {
			UnaryCollisionInfo info = PlaneCollide(plane, node_ref(object)->GetTransform().GetPosition(Transform::Global));
			if (info.collision) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByRayCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Ray &ray, flags_t collisionMask)
{
	// BONUS: sort by distance, from shortest to greatest
	out.RemoveAll();
	const mtlNode<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_collisions && (node_ref(object)->m_objectFlags & collisionMask) > 0) {
			if (node_ref(object)->GetCollider()->Collides(ray)) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByRangeCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Range &range, flags_t collisionMask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_collisions && (node_ref(object)->m_objectFlags & collisionMask) > 0) {
			if (node_ref(object)->GetCollider()->Collides(range)) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByPlaneCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t collisionMask)
{
	out.RemoveAll();
	const mtlNode<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_collisions && (node_ref(object)->m_objectFlags & collisionMask) > 0) {
			node_ref(object)->GetCollider()->Collides(plane);
			out.AddLast(object->GetItem());
		}
		object = object->GetNext();
	}
}

ObjectRef Engine::AddObject( void )
{
	ObjectRef o;
	o.New();
	AddObject(o);
	return o;
}

ObjectRef Engine::AddObject(const mtlChars &typeName)
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
				ObjectRef o = n->GetItem().creator_func();
				AddObject(o);
				return o;
			}
			n = n->GetNext();
		}
	}
	return ObjectRef();
}

void Engine::DestroyAllObjects( void )
{
	mtlNode<ObjectRef> *object = m_objects.GetFirst();
	if (m_inLoop) {
		while (object != NULL) {
			node_ref(object)->Destroy();
			object = object->GetNext();
		}
	} else {
		m_inLoop = true; // prevent some stupid object from recursively calling DestroyAllObjects
		/*while (object != NULL) {
			delete object->GetItem();
			object = object->GetNext();
		}*/
		m_objects.RemoveAll();
		m_inLoop = false;
	}
}

ObjectRef Engine::GetCamera( void )
{
	return m_camera;
}

const ObjectRef Engine::GetCamera( void ) const
{
	return m_camera;
}

void Engine::SetCamera(ObjectRef camera)
{
	if (camera.IsNull() || camera.GetShared()->m_engine != this) {
		m_camera.Delete();
	} else {
		m_camera = camera;
	}
}

void Engine::SetWindowCaption(const mtlChars &caption)
{
	SDL_WM_SetCaption(caption.GetChars(), NULL);
}

void Engine::SetGameProjection( void )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const GLdouble halfW = GLdouble(SDL_GetVideoSurface()->w) / 2.0;
	const GLdouble halfH = GLdouble(SDL_GetVideoSurface()->h) / 2.0;
	glOrtho(-halfW - 0.5, halfW + 0.5, halfH + 0.5, -halfH - 0.5, 0.0, 1.0);
}

void Engine::SetGameView(Transform transform)
{
	Object *camera = m_camera.GetShared();
	mmlMatrix<3,3> cameraView = camera != NULL ? mmlInv(camera->GetTransform().GetTransformMatrix(Transform::Global)) : mmlMatrix<3,3>::IdentityMatrix();
	const mmlMatrix<3,3> objectTransform = transform.GetTransformMatrix(Transform::Global);
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

void Engine::SetRelativeGUIView(Transform transform)
{
	Object *camera = m_camera.GetShared();
	mmlMatrix<3,3> cameraView = camera != NULL ? mmlInv(camera->GetTransform().GetTransformMatrix(Transform::Global)) : mmlMatrix<3,3>::IdentityMatrix();
	const mmlMatrix<3,3> objectTransform = transform.GetTransformMatrix(Transform::Global);
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

int Engine::RunGame( void )
{
	if (m_inLoop) { return 0; } // some stupid object is recursively calling RunGame

	std::cout << "[START main loop]" << std::endl;
	if (m_camera.IsNull()) {
		std::cout << "Engine::RunGame: No camera at game start" << std::endl;
	}

	m_quit = false;
	m_inLoop = true;

	m_timer.Restart();

	while (!m_quit) {
		UpdateInputBuffers();
		UpdateObjects();
		CollideObjects();
		DrawObjects();
		DrawGUI();
		UpdateVideo();
		FinalizeObjects();
		DestroyObjects();
		InitPendingObjects();
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

float Engine::GetElapsedTime( void ) const
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

void Engine::EnableGraphicsSorting( void )
{
	m_graphicsSort = true;
}

void Engine::DisableGraphicsSorting( void )
{
	m_graphicsSort = false;
}

bool Engine::RegisterType(const mtlChars &typeName, ObjectRef (*creator_func)())
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
