#include <SDL/SDL.h>
#include <iostream>
#include "Engine.h"
#include "Object.h"
#include "MML/mmlMath.h"

void Engine::GenerateEventList( void )
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
					unsigned int abCollision = object->GetItem()->GetCollisionFlags() & nextObject->GetItem()->GetTypeFlags();
					unsigned int baCollision = nextObject->GetItem()->GetTypeFlags() & object->GetItem()->GetCollisionFlags();
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
	if (m_renderer == NULL) { return; }
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking() && object->GetItem()->IsVisible() && object->GetItem()->GetGraphics() != NULL) {
			m_renderer->AddGraphics(*object->GetItem()->GetGraphics(), object->GetItem()->GetTransform().GetWorldTransform());
		}
		object = object->GetNext();
	}
	m_renderer->RenderView();
}

void Engine::DrawGUI( void )
{
	mtlNode<Object*> *object = m_objects.GetFirst();
	while (object != NULL) {
		if (object->GetItem()->IsTicking()) {
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

Engine::Engine( void ) : m_objects(), m_timer(60.0f), m_quit(false), m_inLoop(false), m_music(NULL), m_renderer(NULL)
{
}

Engine::~Engine( void )
{
	if (SDL_GetVideoSurface() != NULL) {
		SDL_FreeSurface(SDL_GetVideoSurface());
	}
	delete m_renderer;
	StopMusic();
	Mix_CloseAudio();
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, (SDL_GetVideoSurface()->w - 1) + 0.5, (SDL_GetVideoSurface()->h - 1) + 0.5, -0.5, 0.0, 1.0);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	return true;
}

void Engine::AddObject(Object *object)
{
	if (object == NULL || object->m_engine != NULL) {
		return;
	}
	m_objects.AddLast(object);
	object->m_engine = this;
	object->OnInit();
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
	if (m_renderer == NULL) {
		std::cout << "WARNING: No renderer" << std::endl;
	}

	m_quit = false;
	m_inLoop = true;

	m_timer.Restart();

	while (!m_quit) {
		GenerateEventList();
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
	return m_timer.GetTimeDeltaSecondsTick();
}

const mtlList<SDL_Event> &Engine::GetEventList( void ) const
{
	return m_events;
}

int Engine::GetRandom( void ) const
{
	return rand();
}

int Engine::GetRandom(int max) const
{
	return rand() % (max+1);
}

int Engine::GetRandom(int min, int max) const
{
	return GetRandom(max-min) + min;
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

void Engine::FilterByTypeFlags(const mtlList<Object*> &in, mtlList<Object*> &out, unsigned int mask)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetTypeFlags(mask) == mask) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByCollisionFlags(const mtlList<Object*> &in, mtlList<Object*> &out, unsigned int mask)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem()->GetCollisionFlags(mask) == mask) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

void Engine::FilterByRay(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, mmlVector<2> direction)
{
	out.RemoveAll();
}

void Engine::FilterByCone(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, float range)
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

void Engine::SetRenderer(Renderer *renderer)
{
	if (m_renderer != NULL) {
		delete m_renderer;
	}
	m_renderer = renderer;
}
