#ifndef ENGINE_H
#define ENGINE_H

#ifdef _MSC_VER
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif

#include <SDL/SDL.h>
#include "MTL/mtlString.h"
#include "MTL/mtlList.h"
#include "Timer.h"
#include "Sound.h"
#include "Common.h"
#include "Renderer.h"

class Object;

class Engine
{
private:
	struct Box {
		Point a, b;
	};
	struct BoxPoints {
		Point a, b, c, d;
	};
private:
	mtlList<Object*>	m_objects;
	mtlList<SDL_Event>	m_events;
	Timer				m_timer;
	bool				m_quit;
	bool				m_inLoop;
	Mix_Music			*m_music;
	Renderer			*m_renderer;
private:
	void		GenerateEventList( void );
	void		UpdateObjects( void );
	void		CollideObjects( void );
	void		DrawObjects( void );
	void		DrawGUI( void );
	void		FinalizeObjects( void );
	void		DestroyObjects( void );
	void		UpdateTimer( void );
	bool		Collide(const Object *a, const Object *b) const; // custom rendering routine to determine pixel overlap
	bool		PointInBox(Point p, Box b) const;
	Box			ToBox(Rect r) const;
	bool		GetPixelOverlap(const Object *a, const Object *b, Box o) const;
private:
	Engine(const Engine&) {}
	Engine &operator=(const Engine&) { return *this; }
public:
								Engine( void );
								~Engine( void );
	bool						Init(int argc, char **argv);
	void						AddObject(Object *object);
	void						DestroyAllObjects( void );
	void						SetWindowCaption(const mtlChars &caption);
	int							RunGame( void );
	void						EndGame( void );
	void						KillProgram( void );
	void						SetUpdateFrequency(float updatesPerSecond);
	float						GetDeltaTime( void ) const;
	const mtlList<SDL_Event>	&GetEventList( void ) const;
	int							GetRandom( void ) const;
	int							GetRandom(int max) const;
	int							GetRandom(int min, int max) const;
	const mtlList<Object*>		&GetObjects( void ) const;
	static void					FilterByName(const mtlList<Object*> &in, mtlList<Object*> &out, const mtlChars &name);
	template < typename type_t >
	static void					FilterByRTTI(const mtlList<Object*> &in, mtlList<Object*> &out);
	static void					FilterByType(const mtlList<Object*> &in, mtlList<Object*> &out, TypeID id);
	static void					FilterByObjectFlags(const mtlList<Object*> &in, mtlList<Object*> &out, unsigned int mask);
	static void					FilterByCollisionMasks(const mtlList<Object*> &in, mtlList<Object*> &out, unsigned int mask);
	static void					FilterByRay(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, mmlVector<2> direction);
	static void					FilterByCone(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, float range);
	static void					FilterByBox(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> min, mmlVector<2> max);
	static void					FilterBySphere(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> origin, float radius);
	static void					FilterByPlane(const mtlList<Object*> &in, mtlList<Object*> &out, mmlVector<2> point, mmlVector<2> normal);
	bool						PlayMusic(const mtlChars &file);
	void						StopMusic( void );
	void						UpdateVideo( void ) const;
	void						SetRenderer(Renderer *renderer);
};

template < typename type_t >
void Engine::FilterByRTTI(const mtlList<Object*> &in, mtlList<Object*> &out)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (dynamic_cast<type_t*>(n->GetItem()) != NULL) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

#endif // ENGINE_H
