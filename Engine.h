#ifndef ENGINE_H
#define ENGINE_H

#include "Platform.h"

#include "MTL/mtlString.h"
#include "MTL/mtlList.h"
#include "MTL/mtlType.h"
#include "MTL/mtlRandom.h"
#include "Timer.h"
#include "Sound.h"
#include "Common.h"
#include "Graphics.h"
#include "Collider.h"
#include "GUI.h"
#include "Object.h"

struct MouseButton
{
	enum Button
	{
		Left = 0,
		First = Left,
		Middle = 1,
		Right = 2,

		/*WheelUp = SDL_BUTTON_WHEELUP,
		WheelDown = SDL_BUTTON_WHEELDOWN,
		X1 = SDL_BUTTON_X1,
		X2 = SDL_BUTTON_X2,*/

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
		ObjectRef (*creator_func)();
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

public:
	enum OcclusionMethod
	{
		SortByX,
		SortByY,
		SortByZ,
		None
	};

private:
	mtlList<ObjectRef>	m_objects;
	mtlList<ObjectRef>	m_pending;
	ObjectRef			m_camera;
	Timer				m_timer;
	float				m_deltaSeconds;
	mtlRandom			m_rand;
	//CollisionSolver	m_collisionSolver;
	GUI::Manager		m_guiManager;
	bool				m_quit;
	bool				m_inLoop;
	bool				m_destroyingAll;
	OcclusionMethod		m_occlusionMethod;
	Mix_Music			*m_music;
	Point				m_mousePosition;
	Point				m_prevMousePosition;
	mmlVector<3>		m_clearColor;
	unsigned char		m_keyState[SDLK_LAST];
	unsigned char		m_mouseButtonState[MouseButton::Last];

private:
	void							UpdateInputBuffers( void );
	void							UpdateObjects( void );
	void							CollideObjects( void );
	void							DrawObjects( void );
	void							DrawGUI( void );
	void							FinalizeObjects( void );
	void							DestroyObjects( void );
	void							InitPendingObjects( void );
	void							UpdateTimer( void );

	void							AddObject(ObjectRef object);
	void							AddObjectNow(ObjectRef object);

	static mtlBinaryTree<TypeNode>	&GetTypeTree( void );
	static void						GetRegisteredTypes(const mtlNode<TypeNode> *branch, mtlList< mtlShared<mtlString> > &types);

private:
	Engine(const Engine&) {}
	Engine &operator=(const Engine&) { return *this; }

public:
								Engine( void );
								~Engine( void );

	bool						Init(int width, int height, const mtlChars &windowCaption, int argc, char **argv);

	const mtlList<ObjectRef>	&GetObjects( void ) const;

	static void					FilterByName(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const mtlChars &name);
	template < typename type_t >
	static void					FilterByDynamicType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out);
	static void					FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, TypeID id);
	template < typename type_t >
	static void					FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out);
	static void					FilterByObjectFlags(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void					FilterByCollisionMasks(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void					FilterByObjectFlagsInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void					FilterByCollisionMasksInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void					FilterByRange(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Range &range, flags_t objectMask = AllFlagsOn);
	static void					FilterByPlane(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t objectMask = AllFlagsOn);
	static void					FilterByRayCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Ray &ray, flags_t mask = AllFlagsOn);
	static void					FilterByRangeCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Range &range, flags_t mask = AllFlagsOn);
	static void					FilterByPlaneCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t mask = AllFlagsOn);

	//UnaryCollisionInfo		TraceRay(const Ray &ray); // traverses some kind of spatial data structure, returns object with closest intersection

	template < typename type_t >
	ObjectRef					AddObject( void );
	ObjectRef					AddObject( void );
	ObjectRef					AddObject(const mtlChars &typeName); // can fail (return NULL) if that type name is not registered
	template < typename type_t >
	ObjectRef					AddObjectNow( void );
	ObjectRef					AddObjectNow( void );
	ObjectRef					AddObjectNow(const mtlChars &typeName); // can fail (return NULL) if that type name is not registered

	void						DestroyAllObjects( void );

	ObjectRef					GetCamera( void );
	const ObjectRef				GetCamera( void ) const;
	void						SetCamera(ObjectRef camera);

	void						SetWindowCaption(const mtlChars &caption);

	static void					SetGameProjection( void ); // center at 0,0
	void						SetGameView(const Transform &transform); // center at object position, rotations
	static void					SetGUIProjection( void ); // top-left at 0,0
	static void					SetGUIView( void ); // identity
	static void					SetGUIView(mmlVector<2> offset);
	void						SetRelativeGUIView(const Transform &transform); // center at object position, no rotations

	int							RunGame( void );
	void						EndGame( void );
	void						KillProgram( void );

	void						SetUpdateFrequency(float updatesPerSecond);
	float						GetElapsedTime( void ) const;

	void						SetRandomizerState(unsigned long long state, unsigned long long inc = 1);
	unsigned int				GetRandomUint( void );
	unsigned int				GetRandomUint(unsigned int min, unsigned int max);
	int							GetRandomInt( void );
	int							GetRandomInt(int min, int max);
	float						GetRandomUniform( void );
	float						GetRandomFloat(float min, float max);
	float						GetRandomRaisedCos( void );

	bool						PlayMusic(const mtlChars &file);
	void						StopMusic( void );

	void						UpdateVideo( void ) const;
	static int					GetVideoWidth( void );
	static int					GetVideoHeight( void );

	template < typename graphics_t >
	static mtlAsset<Graphics>	LoadGraphics(const mtlChars &file);
	static mtlAsset<Sound>		LoadSound(const mtlChars &file);

	static void					PrintError(GLenum error);
	static void					PrintError( void );

	Point						GetMousePosition( void ) const;
	Point						GetMouseMovement( void ) const;
	void						SetMousePosition(int x, int y); // make sure to negate
	void						SetMousePosition(Point p);

	mmlVector<2>				GetWorldMousePosition( void ) const;
	mmlVector<2>				GetWorldMouseMovement( void ) const;

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

	bool						IsAnyDown( void ) const;
	bool						IsAnyUp( void ) const;
	bool						IsAnyPressed( void ) const;
	bool						IsAnyHeld( void ) const;
	bool						IsAnyReleased( void ) const;

	void						DisableOcclusion( void );
	void						SetOcclusionMethod(OcclusionMethod method);

	void						SetClearColor(float r, float g, float b);
	void						SetClearColor(mmlVector<3> color);

	template < typename form_t >
	mtlShared<GUI::Form>		AddForm( void );

	static bool					RegisterType(const mtlChars &typeName, ObjectRef (*creator_func)()); // don't call this manually
	static void					GetRegisteredTypes(mtlList< mtlShared<mtlString> > &types);

	ObjectRef					GetSelf(const Object *self) const;

	mmlVector<2>				GetScreenPoint(const mmlVector<2> &world_point) const;
};

/*class BaseProduct
{
public:
	virtual ObjectRef Create( void ) const = 0;
};

template < typename type_t >
class Product : public BaseProduct
{
public:
	ObjectRef Create( void ) const { return ObjectRef::Create<type_t>(); }
};

#define get_str(x) #x
template < typename type_t >
bool Engine::RegisterObject( void )
{
	mtlShared<BaseProduct> product = mtlShared<BaseProduct>::Create< Product<type_t> >();
	Engine::GetObjectFactory().Add(product, get_str(type_t));
	return true;
}
#undef get_str

ObjectRef Engine::AddObject(const mtlChars &name)
{
	mtlShared<BaseProduct> product = Engine::GetObjectFactory().Find(name);
	if (!product.IsNull()) {
		return product.GetShared()->Create();
	}
	return ObjectRef();
}

#define RegisterObject(ObjectName) static const bool ObjectName##_Registered = Engine::RegisterObject<ObjectName>();*/

#define RegisterObject(ObjectTypeName) \
	ObjectRef Create_##ObjectTypeName( void ) { return ObjectRef::Create<ObjectTypeName>(); } \
	static const bool ObjectTypeName##_Registered = Engine::RegisterType(#ObjectTypeName, Create_##ObjectTypeName)

template < typename type_t >
void Engine::FilterByDynamicType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out)
{
	out.RemoveAll();
	const mtlItem<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem().GetShared()->IsDynamicType<type_t>()) { // have to call dynamic_cast rather than GetAsDynamicType because Object is not defined yet
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

template < typename type_t >
void Engine::FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out)
{
	out.RemoveAll();
	const mtlItem<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem().GetShared()->IsStaticType<type_t>()) {
			out.AddLast(n->GetItem());
		}
		n = n->GetNext();
	}
}

template < typename type_t >
ObjectRef Engine::AddObject( void )
{
	ObjectRef o;
	o.New<type_t>();
	AddObject(o);
	return o;
}

template < typename type_t >
ObjectRef Engine::AddObjectNow( void )
{
	ObjectRef o;
	o.New<type_t>();
	AddObjectNow(o);
	return o;
}

template < typename graphics_t >
mtlAsset<Graphics> Engine::LoadGraphics(const mtlChars &file)
{
	return mtlAsset<Graphics>::Load<graphics_t>(file);
}

template < typename form_t >
mtlShared<GUI::Form> Engine::AddForm( void )
{
	return m_guiManager.AddForm<form_t>();
}

#endif // ENGINE_H
