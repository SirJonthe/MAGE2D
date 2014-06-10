#ifndef ENGINE_H
#define ENGINE_H

#include "Platform.h"

#include "MTL/mtlString.h"
#include "MTL/mtlList.h"
#include "MTL/mtlType.h"
#include "Timer.h"
#include "Sound.h"
#include "Common.h"
#include "Graphics.h"

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
	struct Type
	{
		mtlString name;
		Object *(*creator_func)();
		Type( void ) : name(), creator_func(NULL) {}
		Type(const Type &t) : name(), creator_func(t.creator_func) { name.Copy(t.name); }
		Type &operator=(const Type &t) { if (this != &t) { name.Copy(t.name); creator_func = t.creator_func; } return *this; }
	};
	struct TypeNode
	{
		mtlHash								hash;
		mutable mtlShared< mtlList<Type> >	types;
		bool operator>(mtlHash h) const { return hash > h; }
		bool operator==(mtlHash h) const { return hash == h; }
		bool operator>(const TypeNode &n) { return hash > n.hash; }
		bool operator==(const TypeNode &n) { return hash == n.hash; }
	};
private:
	mtlList<Object*>				m_objects;
	Object							*m_camera;
	mtlList<SDL_Event>				m_events;
	Timer							m_timer;
	float							m_deltaSeconds;
	bool							m_quit;
	bool							m_inLoop;
	Mix_Music						*m_music;
private:
	void							GenerateEventList( void );
	void							UpdateObjects( void );
	void							CollideObjects( void );
	void							DrawObjects( void );
	void							DrawGUI( void );
	void							FinalizeObjects( void );
	void							DestroyObjects( void );
	void							UpdateTimer( void );
	bool							Collide(const Object *a, const Object *b) const; // custom rendering routine to determine pixel overlap
	bool							PointInBox(Point p, Box b) const;
	Box								ToBox(Rect r) const;
	bool							GetPixelOverlap(const Object *a, const Object *b, Box o) const;
	static mtlBinaryTree<TypeNode>	&GetTypeTree( void );
	static void						GetRegisteredTypes(const mtlBranch<TypeNode> *branch, mtlList< mtlShared<mtlString> > &types);
private:
	Engine(const Engine&) {}
	Engine &operator=(const Engine&) { return *this; }
public:
								Engine( void );
								~Engine( void );
	bool						Init(int argc, char **argv);
	void						AddObject(Object *object);
	Object						*GetCamera( void );
	const Object				*GetCamera( void ) const;
	void						SetCamera(Object *camera);
	void						DestroyAllObjects( void );
	void						SetWindowCaption(const mtlChars &caption);
	int							RunGame( void );
	void						EndGame( void );
	void						KillProgram( void );
	void						SetUpdateFrequency(float updatesPerSecond);
	float						GetDeltaTime( void ) const;
	const mtlList<SDL_Event>	&GetEventList( void ) const;
	int							GetRandomInt( void ) const;
	int							GetRandomInt(int max) const;
	int							GetRandomInt(int min, int max) const;
	float						GetRandomFloat( void ) const;
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
	template < typename type_t >
	Object						*AddObject( void );
	Object						*AddObject( void );
	Object						*AddObject(const mtlChars &typeName); // can fail (return NULL) if that type name is not registered
	static bool					RegisterType(const mtlChars &typeName, Object *(*creator_func)()); // don't call this manually
	static void					GetRegisteredTypes(mtlList< mtlShared<mtlString> > &types); // requires recursive in-order tree traversal to build tree
	template < typename graphics_t >
	static mtlAsset<Graphics>	LoadGraphics(const mtlChars &file);
	static void					PrintError(GLenum error);
	static void					PrintError( void );
};

#define ENGINE_REGISTER_OBJECT_TYPE(ObjectTypeName) \
	Object *Create_##ObjectTypeName( void ) { return new ObjectTypeName; } \
	static bool ObjectTypeName##_Registered = Engine::RegisterType(#ObjectTypeName, Create_##ObjectTypeName)

template < typename type_t >
void Engine::FilterByRTTI(const mtlList<Object*> &in, mtlList<Object*> &out)
{
	out.RemoveAll();
	const mtlNode<Object*> *n = in.GetFirst();
	while (n != NULL) {
		if (dynamic_cast<type_t*>(n->GetItem()) != NULL) { // have to call dynamic_cast rather than GetAsType because Object is not defined yet
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

template < typename type_t >
Object *Engine::AddObject( void )
{
	type_t *o = new type_t;
	AddObject(o);
	return o;
}

template < typename graphics_t >
mtlAsset<Graphics> Engine::LoadGraphics(const mtlChars &file)
{
	return mtlAsset<Graphics>::Load<graphics_t>(file);
}

#endif // ENGINE_H
