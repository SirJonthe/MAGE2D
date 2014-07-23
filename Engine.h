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
#include "Collider.h"

class Object;

struct MouseButton
{
	enum Button
	{
		Left = SDL_BUTTON_LEFT,
		Middle = SDL_BUTTON_MIDDLE,
		Right = SDL_BUTTON_RIGHT,
		WheelUp = SDL_BUTTON_WHEELUP,
		WheelDown = SDL_BUTTON_WHEELDOWN,
		X1 = SDL_BUTTON_X1,
		X2 = SDL_BUTTON_X2,

		Last
	};
};

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
	struct InputState
	{
		enum
		{
			None,
			Press,
			Release,
			Hold
		};
	};
private:
	mtlList<Object*>				m_objects;
	Object							*m_camera;
	Timer							m_timer;
	float							m_deltaSeconds;
	//CollisionSolver					m_collisionSolver;
	bool							m_quit;
	bool							m_inLoop;
	Mix_Music						*m_music;
	Point							m_mousePosition;
	Point							m_prevMousePosition;
	unsigned char					m_keyState[SDLK_LAST];
	unsigned char					m_mouseButtonState[MouseButton::Last];
private:
	void							UpdateInputBuffers( void );
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
	static void						SetGameView( void );
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

	Point						GetMousePosition( void ) const;
	Point						GetMouseMovement( void ) const;
	void						SetMousePosition(int x, int y); // make sure to negate
	void						SetMousePosition(Point p);

	bool						IsDown(SDLKey key) const;
	bool						IsUp(SDLKey key) const;
	bool						IsPressed(SDLKey key) const;
	bool						IsHeld(SDLKey key) const;
	bool						IsReleased(SDLKey key) const;

	bool						IsDown(MouseButton::Button button) const;
	bool						IsUp(MouseButton::Button button) const;
	bool						IsPressed(MouseButton::Button button) const;
	bool						IsHeld(MouseButton::Button button) const;
	bool						IsReleased(MouseButton::Button button) const;
};

#define ENGINE_REGISTER_OBJECT_TYPE(ObjectTypeName) \
	Object *Create_##ObjectTypeName( void ) { return new ObjectTypeName; } \
	static bool ObjectTypeName##_Registered = Engine::RegisterType(#ObjectTypeName, Create_##ObjectTypeName)

#define OBJECT_DECL(ObjectName) \
	class ObjectName; \
	ENGINE_REGISTER_OBJECT_TYPE(ObjectName); \
	class ObjectName : public mtlInherit<Object>

#define OBJECT_DECL_INHERIT(ObjectName, ObjectInheritance) \
	class ObjectName; \
	ENGINE_REGISTER_OBJECT_TYPE(ObjectName); \
	class ObjectName : public mtlInherit<ObjectInheritance>

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
