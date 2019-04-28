#ifndef ENGINE_H
#define ENGINE_H

#include "Platform.h"

#include "MiniLib/MTL/mtlString.h"
#include "MiniLib/MTL/mtlList.h"
#include "MiniLib/MTL/mtlType.h"
#include "MiniLib/MTL/mtlRandom.h"
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
//	struct Box {
//		Point a, b;
//	};
//	struct BoxPoints {
//		Point a, b, c, d;
//	};
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
	mtlList<ObjectRef>  m_objects;
	mtlList<ObjectRef>  m_pending;
	ObjectRef           m_camera;
	Timer               m_frame_timer;
	Timer               m_delta_timer;
	float               m_deltaSeconds;
	float               m_timeScale;
	mtlRandom           m_rand;
	GUI::Manager        m_guiManager;
	bool                m_quit;
	bool                m_inLoop;
	bool                m_destroyingAll;
	OcclusionMethod     m_occlusionMethod;
	Mix_Music          *m_music;
	float               m_musicVolume;
	Point               m_mousePosition;
	Point               m_prevMousePosition;
	mmlVector<3>        m_clearColor;
	unsigned char       m_keyState[SDLK_LAST];
	unsigned char       m_mouseButtonState[MouseButton::Last];
	unsigned long long  m_loop_counter;

private:
	void                            UpdateInputBuffers( void );
	void                            UpdateObjects( void );
	void                            CollideObjects( void );
	void                            UpdatePhysics( void );
	void                            DrawObjects( void );
	void                            DrawGUI( void );
	void                            FinalizeObjects( void );
	void                            DestroyObjects( void );
	void                            InitPendingObjects( void );
	void                            UpdateTimer( void );

	void                            AddObject(ObjectRef object);
	void                            AddObjectNow(ObjectRef object);

	static mtlBinaryTree<TypeNode> &GetTypeTree( void );
	static void                     GetRegisteredTypes(const mtlNode<TypeNode> *branch, mtlList< mtlShared<mtlString> > &types);

private:
	Engine(const Engine&) {}
	Engine &operator=(const Engine&) { return *this; }

public:
								Engine( void );
							   ~Engine( void );

	void                        Execute(const mtlChars &cmd);

	bool                        Init(int width, int height, bool fullscreen, const mtlChars &windowCaption, int argc, char **argv);

	const mtlList<ObjectRef>   &GetObjects( void ) const;

	static void                 FilterByName(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const mtlChars &name);
	template < typename type_t >
	static void                 FilterByDynamicType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out);
	static void                 FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, mtlTypeID id);
	template < typename type_t >
	static void                 FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out);
	static void                 FilterByObjectFlags(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void                 FilterByCollisionMasks(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void                 FilterByObjectFlagsInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void                 FilterByCollisionMasksInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	static void                 FilterByRange(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Cone &range, flags_t objectMask = AllFlagsOn);
	static void                 FilterByPlane(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t objectMask = AllFlagsOn);
	static void                 FilterByRayCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Ray &ray, flags_t mask = AllFlagsOn);
	static void                 FilterByRangeCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Cone &range, flags_t mask = AllFlagsOn);
	static void                 FilterByPlaneCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t mask = AllFlagsOn);

	//UnaryCollisionInfo          TraceRay(const Ray &ray); // traverses some kind of spatial data structure, returns object with closest intersection

	//template < typename type_t >
	//ObjectRef                   AddObject( void );
	template < typename type_t >
	mtlShared<type_t>           AddObject( void );
	ObjectRef                   AddObject( void );
	ObjectRef                   AddObject(const mtlChars &typeName); // can fail (return NULL) if that type name is not registered
	template < typename type_t >
	ObjectRef                   AddObjectNow( void );
	template < typename type_t >
	mtlShared<type_t>           AddObjectNow( void );
	ObjectRef                   AddObjectNow( void );
	ObjectRef                   AddObjectNow(const mtlChars &typeName); // can fail (return NULL) if that type name is not registered

	void                        KillAllObjects( void );
	void                        DestroyAllObjects( void );

	ObjectRef                   GetCamera( void );
	const ObjectRef             GetCamera( void ) const;
	void                        SetCamera(ObjectRef camera);

	void                        SetWindowCaption(const mtlChars &caption);

	static void                 SetGameProjection( void ); // center at 0,0
	void                        SetGameView(const Transform &transform); // center at object position, rotations
	static void                 SetGUIProjection( void ); // top-left at 0,0
	static void                 SetGUIView( void ); // identity
	static void                 SetGUIView(mmlVector<2> offset);
	void                        SetRelativeGUIView(const Transform &transform); // center at object position, no rotations

	int                         RunGame( void );
	void                        EndGame( void );
	void                        KillProgram( void );

	void                        SetUpdateFrequency(float updatesPerSecond);
	float                       GetElapsedTime( void ) const;
	float                       GetTimeScale( void ) const;
	void                        SetTimeScale(float timeScale);

	void                        SetRandomizerState(unsigned long long state, unsigned long long inc = 1);
	unsigned int                GetRandomUint( void );
	unsigned int                GetRandomUint(unsigned int min, unsigned int max);
	int                         GetRandomInt( void );
	int                         GetRandomInt(int min, int max);
	float                       GetRandomUniform( void );
	float                       GetRandomFloat(float min, float max);
	float                       GetRandomRaisedCos( void );
	bool                        GetRandomBit( void );

	bool                        PlayMusic(const mtlChars &file);
	void                        StopMusic( void );
	float                       GetMusicVolume( void ) const;
	void                        SetMusicVolume(float volume);

	void                        UpdateVideo( void ) const;
	static int                  GetVideoWidth( void );
	static int                  GetVideoHeight( void );

	template < typename graphics_t >
	static mtlAsset<Graphics>   LoadGraphics(const mtlChars &file);
	static mtlAsset<Sound>      LoadSound(const mtlChars &file);

	static void                 PrintError(GLenum error);
	static void                 PrintError( void );

	Point                       GetMousePosition( void ) const;
	Point                       GetMouseMovement( void ) const;
	void                        SetMousePosition(int x, int y); // make sure to negate
	void                        SetMousePosition(Point p);

	mmlVector<2>                GetWorldMousePosition( void ) const;
	mmlVector<2>                GetWorldMouseMovement( void ) const;

	bool                        IsDown(SDLKey key) const;
	bool                        IsUp(SDLKey key) const;
	bool                        IsPressed(SDLKey key) const;
	bool                        IsHeld(SDLKey key) const;
	bool                        IsReleased(SDLKey key) const;

	bool                        IsDown(MouseButton::Button button) const;
	bool                        IsUp(MouseButton::Button button) const;
	bool                        IsPressed(MouseButton::Button button) const;
	bool                        IsHeld(MouseButton::Button button) const;
	bool                        IsReleased(MouseButton::Button button) const;

	bool                        IsAnyDown( void ) const;
	bool                        IsAnyUp( void ) const;
	bool                        IsAnyPressed( void ) const;
	bool                        IsAnyHeld( void ) const;
	bool                        IsAnyReleased( void ) const;

	void                        DisableOcclusion( void );
	void                        SetOcclusionMethod(OcclusionMethod method);

	void                        SetClearColor(float r, float g, float b);
	void                        SetClearColor(mmlVector<3> color);

	template < typename form_t >
	mtlShared<GUI::Form>        AddForm( void );

	static bool                 RegisterType(const mtlChars &typeName, ObjectRef (*creator_func)()); // don't call this manually
	static void                 GetRegisteredTypes(mtlList< mtlShared<mtlString> > &types);

	ObjectRef                   GetSelf(const Object *self) const;

	mmlVector<2>                GetScreenPoint(const mmlVector<2> &world_point) const;

	unsigned long long          GetLoopCounter( void ) const;
	int                         GetPendingObjectsCount( void ) const;
};

#define RegisterObject(ObjectTypeName) \
	ObjectRef Create_##ObjectTypeName( void ) { return ObjectRef::Create<ObjectTypeName>(); } \
	static const bool ObjectTypeName##_Registered = Engine::RegisterType(#ObjectTypeName, Create_##ObjectTypeName)

template < typename type_t >
void Engine::FilterByDynamicType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out)
{
	out.RemoveAll();
	const mtlItem<ObjectRef> *n = in.GetFirst();
	while (n != NULL) {
		if (n->GetItem().GetShared()->IsDynamicType<type_t>()) {
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
mtlShared<type_t> Engine::AddObject( void )
{
	mtlShared<type_t> o;
	o.New();
	AddObject(o);
	return o;
}

template < typename type_t >
mtlShared<type_t> Engine::AddObjectNow( void )
{
	mtlShared<type_t> o;
	o.New();
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
