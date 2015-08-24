#include <iostream>
#include "Engine.h"
#include "Object.h"
#include "GUI.h"
#include "MiniLib/MML/mmlMath.h"
#include "Tools.h"

#define node_ref(x) x->GetItem().GetShared()
#define ptr_ref(x) x.GetShared()

void Engine::UpdateInputBuffers( void )
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) { // some annoying valgrind error here (already tried initializing 'event')
		if (event.type == SDL_QUIT) {
			EndGame();
		}
	}
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
	mtlItem<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->IsTicking()) {
			node_ref(object)->OnUpdate();
		}
		object = object->GetNext();
	}
}

void Engine::CollideObjects( void )
{
	// construct a list of possible collisions and reset the colliders' state
	mtlItem<ObjectRef> *object = m_objects.GetFirst();
	mtlList<ObjectRef&> colliders;
	while (object != NULL) {
		if (node_ref(object)->IsTicking() && node_ref(object)->IsCollidable()) {
			node_ref(object)->GetCollider()->ResetState();
			colliders.AddLast(object->GetItem());
		}
		object = object->GetNext();
	}

	// test for collisions (O(n^2))
	mtlItem<ObjectRef&> *collider = colliders.GetFirst();
	while (collider != NULL) {
		mtlItem<ObjectRef&> *nextCollider = collider->GetNext();
		while (nextCollider != NULL) {
			flags_t abCollision = node_ref(collider)->GetCollisionMasks() & node_ref(nextCollider)->GetObjectFlags();
			flags_t baCollision = node_ref(nextCollider)->GetObjectFlags() & node_ref(collider)->GetCollisionMasks();
			if (abCollision > 0 || baCollision > 0) {
				CollisionInfo info = node_ref(collider)->m_collider->Collides(*node_ref(nextCollider)->m_collider.GetShared());
				if (info.collision) {

					if (baCollision > 0) {
						node_ref(nextCollider)->OnCollision(collider->GetItem(), info);
					}

					mmlSwap(info.c1, info.c2);
					mmlSwap(info.c1_points, info.c2_points);
					if (abCollision > 0) {
						node_ref(collider)->OnCollision(nextCollider->GetItem(), info);
					}
				}
			}
			nextCollider = nextCollider->GetNext();
		}
		collider->GetItem()->GetCollider()->TrackPreviousTransform();
		collider = collider->GetNext();
	}
}

struct GraphicsContainer
{
	Object *object;
	float   z;
	bool    operator<(const GraphicsContainer &c) const { return z < c.z; }
};

void Engine::DrawObjects( void )
{
	if (m_occlusionMethod >= SortByX && m_occlusionMethod <= SortByZ) {
		mtlArray<GraphicsContainer> graphics;
		graphics.SetCapacity(m_objects.GetSize());

		mtlItem<ObjectRef> *object = m_objects.GetFirst();
		while (object != NULL) {
			if (!node_ref(object)->IsDestroyed() && node_ref(object)->IsVisible()) {
				//GraphicsContainer c = { object->GetItem(), node_ref(object)->GetTransform().GetIndependentTransform(Transform::Global), m_occlusionMethod };
				GraphicsContainer c;
				c.object = object->GetItem().GetShared();
				if (m_occlusionMethod == SortByX || m_occlusionMethod == SortByY) {
					c.z = c.object->GetTransform().GetPosition(Transform::Global)[(int)m_occlusionMethod];
				} else {
					c.z = c.object->GetDepth();
				}
				graphics.Add(c);
			}
			object = object->GetNext();
		}

		mtlArray<GraphicsContainer> sortedGraphics;
		graphics.MergeSort(sortedGraphics);

		for (int i = 0; i < sortedGraphics.GetSize(); ++i) {

			Engine::SetGameProjection();
			SetGameView(sortedGraphics[i].object->GetTransform());

			glColor4f(
				sortedGraphics[i].object->GetGraphics().GetRed(),
				sortedGraphics[i].object->GetGraphics().GetGreen(),
				sortedGraphics[i].object->GetGraphics().GetBlue(),
				sortedGraphics[i].object->GetGraphics().GetAlpha()
			);
			sortedGraphics[i].object->OnDraw();
		}

	} else {
		mtlItem<ObjectRef> *object = m_objects.GetFirst();
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
	mtlItem<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->IsTicking()) {
			Engine::SetGUIProjection();
			Engine::SetGUIView();
			GUI::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			node_ref(object)->OnGUI();
		}
		object = object->GetNext();
	}

	m_guiManager.Update();
}

void Engine::FinalizeObjects( void )
{
	mtlItem<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->IsTicking()) {
			node_ref(object)->OnFinal();
		}
		object = object->GetNext();
	}
}

void Engine::DestroyObjects( void )
{
	// NOTE
	// There is still a potential that an some object
	// is keeping a reference (ObjectRef) to a destroyed
	// object and using that as a parent transform for
	// new objects. Those transforms will not be set
	// to NULL since the parent object was already
	// removed from the engine.

	// The only sane way to do this is to store
	// parent transforms inside Transform as a
	// safe reference that will automatically
	// turn to NULL when the parent is destroyed

	mtlItem<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_destroy) {
			if (m_camera.GetShared() == node_ref(object)) { m_camera.Delete(); }
			if (!node_ref(object)->m_kill) { // do not call OnDestroy if we kill the object
				node_ref(object)->OnDestroy();
			}
			node_ref(object)->m_engine = NULL;
			//node_ref(object)->m_objectRef = NULL;
			const Transform *transform_addr = &node_ref(object)->GetTransform();
			object = object->Remove();

			// Now we need to remove any references to the transform
			// that will dissapear as a result of calling Destroy
			mtlItem<ObjectRef> *remaining = m_objects.GetFirst();
			while (remaining != NULL) {
				if (node_ref(remaining)->GetTransform().GetParent() == transform_addr) {
					node_ref(remaining)->GetTransform().SetParent(Transform::Global, NULL);
				}
				remaining = remaining->GetNext();
			}
			remaining = m_pending.GetFirst();
			while (remaining != NULL) {
				if (node_ref(remaining)->GetTransform().GetParent() == transform_addr) {
					node_ref(remaining)->GetTransform().SetParent(Transform::Global, NULL);
				}
				remaining = remaining->GetNext();
			}

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
	mtlItem<ObjectRef> *object = m_pending.GetFirst();
	while (object != NULL) {
		m_objects.AddLast(object->GetItem());
		//node_ref(object)->m_objectRef = &m_objects.GetLast()->GetItem();
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
	if (object.IsNull()) {
		std::cout << "Engine::AddObject: Object is NULL" << std::endl;
		return;
	}
	object.GetShared()->m_engine = this;
	m_pending.AddLast(object);
}

void Engine::AddObjectNow(ObjectRef object)
{
	if (object.IsNull()) {
		std::cout << "Engine::AddObjectNow: Object is NULL" << std::endl;
		return;
	}
	object.GetShared()->m_engine = this;
	m_objects.AddLast(object);
	//object.GetShared()->m_objectRef = &m_objects.GetLast()->GetItem();
	object.GetShared()->OnInit();
}

mtlBinaryTree<Engine::TypeNode> &Engine::GetTypeTree( void )
{
	static mtlBinaryTree<TypeNode> typeTree; // should be a mtlStringMap
	return typeTree;
}

void Engine::GetRegisteredTypes(const mtlNode<TypeNode> *branch, mtlList< mtlShared<mtlString> > &types)
{
	if (branch == NULL) { return; }
	if (branch->GetLeft() != NULL) {
		GetRegisteredTypes(branch->GetLeft(), types);
	}
	mtlItem<Type> *node = branch->GetItem().types.GetShared()->GetFirst();
	while (node != NULL) {
		mtlShared<mtlString> name;
		name.New();
		name.GetShared()->Copy(node->GetItem().name);
		types.AddLast(name);
		node = node->GetNext();
	}
	if (branch->GetRight() != NULL) {
		GetRegisteredTypes(branch->GetRight(), types);
	}
}

Engine::Engine( void ) :
	m_objects(), m_camera(),
	m_timer(60.0f), m_deltaSeconds(0.0f), m_timeScale(1.0f),
	m_rand(),
	m_quit(false), m_inLoop(false),
	m_destroyingAll(false),
	m_occlusionMethod(None),
	m_music(NULL), m_musicVolume(1.0f),
	m_clearColor(0.0f, 0.0f, 0.0f),
	m_loop_counter(0)
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

void Engine::Execute(const mtlChars &cmd)
{
}

bool Engine::Init(int width, int height, bool fullscreen, const mtlChars &windowCaption, int argc, char **argv)
{
	std::cout << "Engine::Init: " << std::endl;
	struct Args
	{
		int width, height;
		int fullscreen;
		bool autofit;
	};

	Args args;
	args.width = width;
	args.height = height;
	args.fullscreen = fullscreen ? 1 : 0;
	args.autofit = false;

	for (int i = 1; i < argc; ++i) {
		std::cout << "\t" << argv[i] << ": ";
		if (strcmp(argv[i], "-width") == 0 && i < argc-1) {
			args.width = mmlMax2(atoi(argv[++i]), 0);
			std::cout << "Window width set (" << args.width << ")";
		}
		else if (strcmp(argv[i], "-height") == 0 && i < argc-1) {
			args.height = mmlMax2(atoi(argv[++i]), 0);
			std::cout << "Window height set (" << args.height << ")";
		}
		else if (strcmp(argv[i], "-fullscreen") == 0 && i < argc-1) {
			args.fullscreen = mmlClamp(0, atoi(argv[++i]), 1);
			std::cout << "Window mode set (" << args.fullscreen << ")";
		}
		else if (strcmp(argv[i], "-freq") == 0 && i < argc-1) {
			float freq = mmlMax2(1.0f, (float)atof(argv[++i]));
			SetUpdateFrequency(freq);
			std::cout << "Update frequency set (" << freq << ")";
		}
		else if (strcmp(argv[i], "-console") == 0) {
			AddObject<Console>();
			std::cout << "Console enabled";
		}
		else if (strcmp(argv[i], "-autofit") == 0) {
			std::cout << "Fitting resolution to screen";
			args.autofit = true;
		} else if (strcmp(argv[i], "-rendermsg") == 0 && i < argc-2) {
			std::cout << "[PENDING IMPLEMENTATION] Rendering \"" << argv[i+1] << "\" to file \"" << argv[i+2] << "\"";
			i+=2;
		}
		else {
			std::cout << "UNKNOWN ARGUMENT";
		}
		std::cout << std::endl;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cout << "\tSDL fail: " << SDL_GetError() << std::endl;
		return false;
	}

	if (args.autofit) {
		const SDL_VideoInfo *info = SDL_GetVideoInfo();
		if (info != NULL) {
			float w_scale = (float)args.width / (float)info->current_w;
			float h_scale = (float)args.height / (float)info->current_h;
			float scale = 1.0f / mmlMax2(w_scale, h_scale);
			args.width *= scale;
			args.height *= scale;
			std::cout << "Resolution resized to " << args.width << "x" << args.height << " to fit " << info->current_w << "x" << info->current_h << " (scale factor " << scale << ")" << std::endl;
		} else {
			std::cout << "Failed to fit resolution" << std::endl;
		}
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (SDL_SetVideoMode(args.width, args.height, 32, SDL_OPENGL) == NULL) {
		std::cout << "\tvideo fail: " << SDL_GetError() << std::endl;
		return false;
	}

#ifdef __GLEW_H__
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "\tGLEW fail: " << glewGetErrorString(err);
		return false;
	}
#endif

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
		std::cout << "\tSDL_mixer fail: " << Mix_GetError() << std::endl;
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

	SetWindowCaption(windowCaption);

	return true;
}

const mtlList<ObjectRef> &Engine::GetObjects( void ) const
{
	return m_objects;
}

void Engine::FilterByName(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const mtlChars &name)
{
	out.RemoveAll();
	const mtlItem<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (node_ref(n)->GetName().Compare(name)) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, mtlTypeID id)
{
	out.RemoveAll();
	const mtlItem<ObjectRef> *n = in.GetFirst();
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
	const mtlItem<ObjectRef> *n = in.GetFirst();
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
	const mtlItem<ObjectRef> *n = in.GetFirst();
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
	const mtlItem<ObjectRef> *n = in.GetFirst();
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
	const mtlItem<ObjectRef> *n = in.GetFirst();
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
	const mtlItem<ObjectRef> *object = in.GetFirst();
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
	const mtlItem<ObjectRef> *object = in.GetFirst();
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
	const mtlItem<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_collisions && (node_ref(object)->m_objectFlags & collisionMask) > 0) {
			UnaryCollisionInfo info = node_ref(object)->GetCollider()->Collides(ray);
			if (info.collision) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByRangeCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Range &range, flags_t collisionMask)
{
	out.RemoveAll();
	const mtlItem<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_collisions && (node_ref(object)->m_objectFlags & collisionMask) > 0) {
			UnaryCollisionInfo info = node_ref(object)->GetCollider()->Collides(range);
			if (info.collision) {
				out.AddLast(object->GetItem());
			}
		}
		object = object->GetNext();
	}
}

void Engine::FilterByPlaneCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t collisionMask)
{
	out.RemoveAll();
	const mtlItem<ObjectRef> *object = in.GetFirst();
	while (object != NULL) {
		if (node_ref(object)->m_collisions && (node_ref(object)->m_objectFlags & collisionMask) > 0) {
			UnaryCollisionInfo info = node_ref(object)->GetCollider()->Collides(plane);
			if (info.collision) {
				out.AddLast(object->GetItem());
			}
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
	mtlNode<TypeNode> *b = GetTypeTree().GetRoot();
	if (b != NULL) {
		b = b->Find(h);
	}
	if (b != NULL) {
		mtlItem<Type> *n = b->GetItem().types.GetShared()->GetFirst();
		while (n != NULL) {
			if (n->GetItem().name.Compare(typeName)) {
				ObjectRef o = n->GetItem().creator_func();
				AddObject(o);
				return o;
			}
			n = n->GetNext();
		}
	}
	mtlString name(typeName);
	std::cout << "Engine::AddObject: Object \"" << name.GetChars() << "\" not registered" << std::endl;
	return ObjectRef();
}

ObjectRef Engine::AddObjectNow( void )
{
	ObjectRef o;
	o.New();
	AddObjectNow(o);
	return o;
}

ObjectRef Engine::AddObjectNow(const mtlChars &typeName)
{
	mtlHash h(typeName);
	mtlNode<TypeNode> *b = GetTypeTree().GetRoot();
	if (b != NULL) {
		b = b->Find(h);
	}
	if (b != NULL) {
		mtlItem<Type> *n = b->GetItem().types.GetShared()->GetFirst();
		while (n != NULL) {
			if (n->GetItem().name.Compare(typeName)) {
				ObjectRef o = n->GetItem().creator_func();
				AddObjectNow(o);
				return o;
			}
			n = n->GetNext();
		}
	}
	return ObjectRef();
}

void Engine::DestroyAllObjects( void )
{
	if (m_destroyingAll) { return; }
	m_destroyingAll = true;

	mtlItem<ObjectRef> *object = m_objects.GetFirst();
	while (object != NULL) {
		node_ref(object)->Destroy();
		object = object->GetNext();
	}

	m_camera.Delete();

	m_destroyingAll = false;
}

void Engine::KillAllObjects( void )
{
	if (m_destroyingAll) { return; }
	m_destroyingAll = true;

	mtlItem<ObjectRef> *object = m_objects.GetFirst();
	if (m_inLoop) {
		while (object != NULL) {
			node_ref(object)->Kill();
			object = object->GetNext();
		}
	} else {
		m_objects.RemoveAll();
	}

	m_camera.Delete();

	m_destroyingAll = false;
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
	std::cout << "Engine::SetCamera: ";
	if (camera.IsNull() || camera.GetShared()->m_engine != this) {
		m_camera.Delete();
		std::cout << "Invalid camera; Camera set to identity" << std::endl;
	} else {
		m_camera = camera;
		std::cout << "New camera set to follow object " << camera->GetName().GetChars() << " @ " << camera.GetShared() << std::endl;
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

void Engine::SetGameView(const Transform &transform)
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

void Engine::SetGUIView(mmlVector<2> offset)
{
	GLfloat m[16] = {
		1.0f,     0.0f,       0.0f, 0.0f,
		0.0f,     1.0f,       0.0f, 0.0f,
		0.0f,     0.0f,       1.0f, 0.0f,
		offset[0], offset[1], 0.0f, 1.0f
	};
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m);
}

void Engine::SetRelativeGUIView(const Transform &transform)
{
	Object *camera = m_camera.GetShared();
	mmlMatrix<3,3> c = camera != NULL ? mmlInv(camera->GetTransform().GetTransformMatrix(Transform::Global)) : mmlMatrix<3,3>::IdentityMatrix();
	const mmlMatrix<3,3> objectTransform = transform.GetTransformMatrix(Transform::Global);
	const mmlMatrix<3,3> f = c * objectTransform;
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
	m_loop_counter = 0;

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
		++m_loop_counter;
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
	std::cout << "[KILL PROGRAM]" << std::endl;
	if (SDL_GetVideoSurface() != NULL) {
		SDL_FreeSurface(SDL_GetVideoSurface());
	}
	StopMusic();
	Mix_CloseAudio();
	SDL_Quit();
	GUI::Destroy();
	m_inLoop = false;
	m_quit = true;
	KillAllObjects();
	exit(0);
}

void Engine::SetUpdateFrequency(float updatesPerSecond)
{
	m_timer.SetIntervalsPerSecond(updatesPerSecond);
}

float Engine::GetElapsedTime( void ) const
{
	return m_deltaSeconds * m_timeScale;
}

float Engine::GetTimeScale( void ) const
{
	return m_timeScale;
}

void Engine::SetTimeScale(float timeScale)
{
	m_timeScale = timeScale;
}

void Engine::SetRandomizerState(unsigned long long state, unsigned long long inc)
{
	m_rand.SetSeed(state, inc);
}

unsigned int Engine::GetRandomUint( void )
{
	return m_rand.GetUint();
}

unsigned int Engine::GetRandomUint(unsigned int min, unsigned int max)
{
	return m_rand.GetUint(min, max);
}

int Engine::GetRandomInt( void )
{
	return m_rand.GetInt();
}

int Engine::GetRandomInt(int min, int max)
{
	return m_rand.GetInt(min, max);
}

float Engine::GetRandomUniform( void )
{
	return m_rand.GetFloat();
}

float Engine::GetRandomFloat(float min, float max)
{
	return m_rand.GetFloat(min, max);
}

float Engine::GetRandomRaisedCos( void )
{
	float x = (GetRandomUniform() - 0.5f) * 2.0f * mmlPI;
	return (cos(x) + 1.0f) / 2.0f;
}

bool Engine::GetRandomBit( void )
{
	return bool(GetRandomUint() & 1);
}

bool Engine::PlayMusic(const mtlChars &file)
{
	std::cout << "Engine::PlayMusic: " << file.GetChars() << std::endl;
	StopMusic();
	m_music = Mix_LoadMUS(file.GetChars());
	m_musicVolume = 1.0f;
	if (m_music == NULL || Mix_PlayMusic(m_music, -1) == -1) {
		std::cout << "\tfailed: " << Mix_GetError() << std::endl;
		return false;
	} else {
		Mix_VolumeMusic(MIX_MAX_VOLUME);
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

float Engine::GetMusicVolume( void ) const
{
	return m_musicVolume;
}

void Engine::SetMusicVolume(float volume)
{
	volume = mmlClamp(0.0f, volume, 1.0f);
	if (m_music != NULL) {
		Mix_VolumeMusic((int)(volume*MIX_MAX_VOLUME));
	}
}

void Engine::UpdateVideo( void ) const
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

mmlVector<2> Engine::GetWorldMousePosition( void ) const
{
	Point iMouse = GetMousePosition();
	mmlVector<2> fMouse((float)iMouse.x - (float)GetVideoWidth() / 2.0f, (float)iMouse.y - (float)GetVideoHeight() / 2.0f);
	if (!m_camera.IsNull()) {
		fMouse = m_camera->GetTransform().TransformPoint(Transform::Global, fMouse);
	}
	return fMouse;
}

mmlVector<2> Engine::GetWorldMouseMovement( void ) const
{
	Point iMov = GetMouseMovement();
	mmlVector<2> fMov((float)iMov.x, (float)iMov.y);
	if (!m_camera.IsNull()) {
		fMov *= m_camera->GetTransform().GetRotation(Transform::Global);
	}
	return fMov;
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

bool Engine::IsAnyDown( void ) const
{
	for (int i = (int)SDLK_FIRST; i < (int)SDLK_LAST; ++i) {
		if (IsDown((SDLKey)i)) { return true; }
	}
	for (int i = (int)MouseButton::First; i < (int)MouseButton::Last; ++i) {
		if (IsDown((MouseButton::Button)i)) { return true; }
	}
	return false;
}

bool Engine::IsAnyUp( void ) const
{
	for (int i = (int)SDLK_FIRST; i < (int)SDLK_LAST; ++i) {
		if (IsUp((SDLKey)i)) { return true; }
	}
	for (int i = (int)MouseButton::First; i < (int)MouseButton::Last; ++i) {
		if (IsUp((MouseButton::Button)i)) { return true; }
	}
	return false;
}

bool Engine::IsAnyPressed( void ) const
{
	for (int i = (int)SDLK_FIRST; i < (int)SDLK_LAST; ++i) {
		if (IsPressed((SDLKey)i)) { return true; }
	}
	for (int i = (int)MouseButton::First; i < (int)MouseButton::Last; ++i) {
		if (IsPressed((MouseButton::Button)i)) { return true; }
	}
	return false;
}

bool Engine::IsAnyHeld( void ) const
{
	for (int i = (int)SDLK_FIRST; i < (int)SDLK_LAST; ++i) {
		if (IsHeld((SDLKey)i)) { return true; }
	}
	for (int i = (int)MouseButton::First; i < (int)MouseButton::Last; ++i) {
		if (IsHeld((MouseButton::Button)i)) { return true; }
	}
	return false;
}

bool Engine::IsAnyReleased( void ) const
{
	for (int i = (int)SDLK_FIRST; i < (int)SDLK_LAST; ++i) {
		if (IsReleased((SDLKey)i)) { return true; }
	}
	for (int i = (int)MouseButton::First; i < (int)MouseButton::Last; ++i) {
		if (IsReleased((MouseButton::Button)i)) { return true; }
	}
	return false;
}

void Engine::DisableOcclusion( void )
{
	m_occlusionMethod = None;
}

void Engine::SetOcclusionMethod(OcclusionMethod method)
{
	switch (method) {
	case SortByX:
	case SortByY:
	case SortByZ:
		m_occlusionMethod = method;
		break;
	default:
		m_occlusionMethod = None;
		break;
	}
}

void Engine::SetClearColor(float r, float g, float b)
{
	glClearColor(r, g, b, 0.0f);
}

void Engine::SetClearColor(mmlVector<3> color)
{
	SetClearColor(color[0], color[1], color[2]);
}

bool Engine::RegisterType(const mtlChars &typeName, ObjectRef (*creator_func)())
{
	mtlHash h(typeName);
	mtlNode<TypeNode> *b = GetTypeTree().GetRoot();
	if (b != NULL) {
		b = b->Find(h);
	}
	mtlItem<Type> *n = NULL;
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

ObjectRef Engine::GetSelf(const Object *self) const
{
	if (self == NULL || self->m_engine != this) {
		return ObjectRef();
	}
	const mtlItem<ObjectRef> *i = m_objects.GetFirst();
	while (i != NULL) {
		if (i->GetItem().GetShared() == self) {
			return i->GetItem();
		}
		i = i->GetNext();
	}
	return ObjectRef();
}

mmlVector<2> Engine::GetScreenPoint(const mmlVector<2> &world_point) const
{
	return world_point + mmlVector<2>((float)GetVideoWidth() / 2.0f, (float)GetVideoHeight() / 2.0f);
}

unsigned long long Engine::GetLoopCounter( void ) const
{
	return m_loop_counter;
}
