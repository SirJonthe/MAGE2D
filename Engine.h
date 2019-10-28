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

// @data MouseButton
// @info Describes mouse button constant identifiers.
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

// @data Engine
// @info The main data structure for the game used to update all objects and send signals between objects.
// @note Only have one active Engine at a time.
class Engine
{
private:
//	struct Box {
//		Point a, b;
//	};
//	struct BoxPoints {
//		Point a, b, c, d;
//	};
	
	// @data Type
	// @info Used internally by engine to register objects by C++ class name to enable object creation and initialization by string.
	struct Type
	{
		mtlString name;
		ObjectRef (*creator_func)();
		Type( void ) : name(), creator_func(NULL) {}
		Type(const Type &t) : name(), creator_func(t.creator_func) { name.Copy(t.name); }
		Type &operator=(const Type &t) { if (this != &t) { name.Copy(t.name); creator_func = t.creator_func; } return *this; }
	};
	
	// @data TypeNode
	// @info Stores object types in a node with a precomputed hash as identifier.
	struct TypeNode
	{
		mtlHash								hash;
		mutable mtlShared< mtlList<Type> >	types;
		bool operator>(mtlHash h) const { return hash > h; }
		bool operator==(mtlHash h) const { return hash == h; }
		bool operator>(const TypeNode &n) { return hash > n.hash; }
		bool operator==(const TypeNode &n) { return hash == n.hash; }
	};
	
	// @data InputState
	// @info The generic state of any binary (non-analogue) input.
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
	// @data OcclusionMethod
	// @info The constants used to set sorting graphics by axis.
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

	// @algo Execute
	// @info Executes a registered command defined by input string.
	// @in cmd -> The command string.
	void                        Execute(const mtlChars &cmd);

	// @algo Init
	// @info Initializes the engine.
	// @note Must call this function before running main engine loop.
	// @in
	//   width -> The requested window resolution in width.
	//   height -> The requested window resolution in height.
	//   fullscreen -> TRUE = fullscreen / FALSE = windowed.
	//   windowCaption -> The window caption.
	//   argc -> Number of user input commands.
	//   argv -> The user input command strings.
	// @out The success state of the initialization (TRUE on success).
	bool                        Init(int width, int height, bool fullscreen, const mtlChars &windowCaption, int argc, char **argv);

	// @algo GetObjects
	// @out The full list of game objects.
	const mtlList<ObjectRef>   &GetObjects( void ) const;

	// @algo FilterByName
	// @info Filters a list of objects by their name.
	// @in
	//   in -> The input list.
	//   name -> The name to filter the list by.
	// @inout out -> The filtered output list.
	static void                 FilterByName(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const mtlChars &name);
	
	// @algo FilterByDynamicType
	// @info Filters a list of objects based on their RTTI.
	// @in in -> The input list.
	// @inout The filtered output list.
	template < typename type_t >
	static void                 FilterByDynamicType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out);
	
	// @algo FilterByStaticType
	// @info Filters a list of objects based on their MiniLib RTTI.
	// @in
	//   in -> The input list.
	//   id -> The ID to filter the list by.
	// @inout out -> The filtered output list.
	static void                 FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, mtlTypeID id);
	
	// @algo FilterByStaticType
	// @info Filters a list of objects based on their MiniLib RTTI.
	// @in in -> The input list.
	// @inout out -> The filtered output list.
	template < typename type_t >
	static void                 FilterByStaticType(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out);
	
	// @algo FilterByObjectFlags
	// @info Filters a list of objects based on their object flags. Looks for exact match.
	// @in
	//   in -> The input list.
	//   mask -> The masks to filter the list by.
	// @inout out -> The filtered output list.
	static void                 FilterByObjectFlags(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	
	// @algo FilterByCollisionMasks
	// @info Filters a list of objects based on their collision flags. Looks for exact match.
	// @in
	//   in -> The input list.
	//   mask -> The masks to filter the list by.
	// @inout out -> The filtered output list.
	static void                 FilterByCollisionMasks(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	
	// @algo FilterByObjectFlagsInclusive
	// @info Filters a list of objects based on their object flags. Looks for overlapping match.
	// @in
	//   in -> The input list.
	//   mask -> The masks to filter the list by.
	// @inout out -> The filtered output list.
	static void                 FilterByObjectFlagsInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	
	// @algo FilterByCollisionMasksInclusive
	// @info Filters a list of objects based on their collision flags. Looks for overlapping match.
	// @in
	//   in -> The input list.
	//   mask -> The masks to filter the list by.
	// @inout out -> The filtered output list.
	static void                 FilterByCollisionMasksInclusive(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, flags_t mask);
	
	// @algo FilterByRange
	// @info Filters a list of objects based on their spatial point location inside a cone.
	// @in
	//   in -> The input list.
	//   cone -> The cone to filter the list by.
	//   objectMask -> Object masks the cone uses to omit results from list.
	// @inout out -> The filtered output list.
	static void                 FilterByRange(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Cone &range, flags_t objectMask = AllFlagsOn);
	
	// @algo FilterByPlane
	// @info Filters a list of objects based on their spatial point location in relation to plane's positive half space.
	// @in
	//   in -> The input list.
	//   plane -> The plane to filter the objects by.
	//   objectMask -> Object masks the plane uses to omit results from list.
	// @inout out -> The filtered output list.
	static void                 FilterByPlane(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t objectMask = AllFlagsOn);
	
	// @algo FilterByRayCollision
	// @info Filters a list of objects based on collision between their colliders and the input ray.
	// @in
	//   in -> The input list.
	//   ray -> The ray to filter the objects by.
	//   mask -> The ray mask used to omit results from list.
	// @inout out -> The filtered output list.
	static void                 FilterByRayCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Ray &ray, flags_t mask = AllFlagsOn);
	
	// @algo FilterByRangeCollision
	// @info Filters a list of objects based on collision between their colliders and the input cone.
	// @in
	//   in -> The input list.
	//   range -> The cone to filter the objects by.
	//   mask -> The cone mask used to omit results from list.
	// @inout out -> The filtered output list.
	static void                 FilterByRangeCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Cone &range, flags_t mask = AllFlagsOn);
	
	// @algo FileterByPlaneCollision
	// @info Filters a list of objects based on collision between their colliders and the input plane.
	// @in
	//   in -> The input list.
	//   plane -> The plane to filter the objects by.
	//   mask -> The plane mask used to omit results from list.
	// @inout out -> The filtered output list.
	static void                 FilterByPlaneCollision(const mtlList<ObjectRef> &in, mtlList<ObjectRef> &out, const Plane &plane, flags_t mask = AllFlagsOn);

	//UnaryCollisionInfo          TraceRay(const Ray &ray); // traverses some kind of spatial data structure, returns object with closest intersection

	//template < typename type_t >
	//ObjectRef                   AddObject( void );
	
	// @algo AddObject
	// @info Creates a new object of the requested type.
	// @out A new object of the requested type.
	template < typename type_t >
	mtlShared<type_t>           AddObject( void );
	
	// @algo AddObject
	// @info Creates a new object of generic type.
	// @out A new object of generic type.
	ObjectRef                   AddObject( void );
	
	// @algo AddObject
	// @info Creates a new object of the requested type (by string).
	// @in typeName -> The string C++ class name of the object the user wants to instanciate.
	// @out A new object of generic type. NULL on fail.
	ObjectRef                   AddObject(const mtlChars &typeName); // can fail (return NULL) if that type name is not registered
	
	// @algo AddObjectNow
	// @info Creates a new object of the requested type and initialize immediately.
	// @out A new object of generic type.
	template < typename type_t >
	ObjectRef                   AddObjectNow( void );
	
	// @algo AddObjectNow
	// @info Creates a new object of the requested type and initialize immediately.
	// @out A new object of the requested type.
	template < typename type_t >
	mtlShared<type_t>           AddObjectNow( void );
	
	// @algo AddObjectNow
	// @info Creates a new object of generic type and initialize immediately.
	// @out A new object of generic type.
	ObjectRef                   AddObjectNow( void );
	
	// @algo AddObjectNow
	// @info Creates a new object of the requested type (by string) and initialize immediately.
	// @in typeName -> The string C++ class name of the object the user wants to instanciate.
	// @out A new object of generic type. NULL on fail.
	ObjectRef                   AddObjectNow(const mtlChars &typeName); // can fail (return NULL) if that type name is not registered

	// @algo KillAllObjects
	// @info Kill all objects without calling their Destroy functions.
	void                        KillAllObjects( void );
	
	// @algo DestroyAllObjects
	// @info Destroys all objects by calling they Destroy functions.
	void                        DestroyAllObjects( void );

	// @algo GetCamera
	// @out The game object set as the game camera.
	ObjectRef                   GetCamera( void );
	const ObjectRef             GetCamera( void ) const;
	
	// @algo SetCamera
	// @info Set a new camera by following a game object's position, scale, and orientation.
	// @in camera -> The game object to be used as camera.
	void                        SetCamera(ObjectRef camera);

	// @algo SetWindowCaption
	// @info Changes the window caption.
	// @in caption -> The new window caption.
	void                        SetWindowCaption(const mtlChars &caption);

	// @algo SetGameProjection
	// @info Set origin an (0,0)
	static void                 SetGameProjection( void ); // center at 0,0
	
	// @algo SetGameView
	// @info Set origin at the provided transform.
	// @in transform -> The transform to "follow".
	void                        SetGameView(const Transform &transform); // center at object position, rotations
	
	// @algo SetGUIProjection
	// @info Set origin at top-left corner of screen. Good for printing debug data or GUI elements.
	static void                 SetGUIProjection( void ); // top-left at 0,0
	
	// @algo SetGUIView
	// @info Sets the identity transform.
	static void                 SetGUIView( void ); // identity
	
	// @algo SetGUIView
	// @info Set the identity transform with an added offset.
	// @in offset -> Offset vector.
	static void                 SetGUIView(mmlVector<2> offset);
	
	// @algo SetRelativeGUIView
	// @info Center view at provided transform.
	// @in transform -> The transform to be centered around.
	void                        SetRelativeGUIView(const Transform &transform); // center at object position, no rotations

	// @algo Run
	// @info The main engine game loop that updates objects and .
	// @note Add at least one object to game engine before calling this (otherwise nothing will happen).
	// @out The C program main function return codes.
	int                         RunGame( void );
	
	// @algo EndGame
	// @info Ends game gracefully.
	void                        EndGame( void );
	
	// @algo KillProgram
	// @info Forcefully kills the program.
	void                        KillProgram( void );

	// @algo SetUpdateFrequency
	// @info Used to limit the number of updates per second.
	// @in updatesPerSecond -> The maximum Hz requested.
	void                        SetUpdateFrequency(float updatesPerSecond);
	
	// @algo GetElapsedTime
	// @out Elapsed time since program start.
	float                       GetElapsedTime( void ) const;
	
	// @algo GetTimeScale
	// @out The time scaling factor used to modify game time.
	float                       GetTimeScale( void ) const;
	
	// @algo SetTimeScale
	// @info Modifies game time.
	// @in timeScale -> The new time scaling factor used to modify game time.
	void                        SetTimeScale(float timeScale);

	// @algo SetRandomizerState
	// @info Sets a new random seed.
	// @in
	//   state -> Randomizer seed.
	//   inc -> Number of steps randomizer uses at every fetch.
	void                        SetRandomizerState(unsigned long long state, unsigned long long inc = 1);
	
	// @algo GetRandomUint
	// @out Random unsigned integer.
	unsigned int                GetRandomUint( void );
	
	// @algo GetRandomUint
	// @info Gets a random unsigned integer within specified bounds.
	// @in
	//   min -> The minimum value (inclusive).
	//   max -> The maximum value (not inclusive).
	// @out Random unsigned integer within specified bounds.
	unsigned int                GetRandomUint(unsigned int min, unsigned int max);
	
	// @algo GetRandomInt
	// @out Random signed integer.
	int                         GetRandomInt( void );
	
	// @algo GetRandomInt
	// @info Returns a random signed integer within specified bounds.
	// @in
	//   min -> The minimum value (inclusive).
	//   max -> The Maximum value (not inclusive).
	// @out A random signed integer within specified bounds.
	int                         GetRandomInt(int min, int max);
	
	// @algo GetRandomUniform
	// @out A random float in range 0-1 (1 not inclusive).
	float                       GetRandomUniform( void );
	
	// @algo GetRandomFloat
	// @info Returns a random floating point value within specified bounds.
	// @in
	//   min -> The minimum value (inclusive)
	//   max -> The maximum value (not inclusive).
	// @out A random floating point value withing specified bounds.
	float                       GetRandomFloat(float min, float max);
	
	// @algo GetRandomRaisedCos
	// @out A random float (0-1, 1 non-inclusive) based on a cosine distribution.
	float                       GetRandomRaisedCos( void );
	
	// @algo GetRandomBit
	// @out A random TRUE or FALSE
	bool                        GetRandomBit( void );

	// @algo PlayMusic
	// @info Plays music from a file.
	// @in file -> The music file path.
	// @out The success state (TRUE on everything OK).
	bool                        PlayMusic(const mtlChars &file);
	
	// @algo StopMusic
	// @info Stops the music.
	void                        StopMusic( void );
	
	// @algo GetMusicVolume
	// @out Volume in range 0-1
	float                       GetMusicVolume( void ) const;
	
	// @algo SetMusicVolume
	// @in volume -> The new volume in range 0-1.
	void                        SetMusicVolume(float volume);

	// @algo UpdateVideo
	// @info Updates the window to reflect graphics buffer contents.
	// @note Automatically called by the engine during normal game loop. Not generally called by the user.
	void                        UpdateVideo( void ) const;
	
	// @algo GetVideoWidth
	// @out The resolution width of the window.
	static int                  GetVideoWidth( void );
	
	// @algo GetVideoHeight
	// @out The resolution height of the window.
	static int                  GetVideoHeight( void );

	// @algo LoadGraphics
	// @info Returns a reference to a graphics asset. Either loads a new instance of the asset or, if already in memory, a reference to an existing asset.
	// @in file -> The file containing the requested graphics asset.
	// @out The graphics asset.
	template < typename graphics_t >
	static mtlAsset<Graphics>   LoadGraphics(const mtlChars &file);
	
	// @algo LoadSound
	// @info Returns a reference to a sound asset. Either loads a new instance of the asset or, if already in memory, a reference to an existing asset.
	// @in file -> The file containing the requested graphics asset.
	// @out The sound asset.
	static mtlAsset<Sound>      LoadSound(const mtlChars &file);

	// @algo PrintError
	// @info Prints the error string corresponding to the input OpenGL error constant.
	// @in error -> The OpenGL error constant.
	static void                 PrintError(GLenum error);
	
	// @algo PrintError
	// @info Prints any current error in screen coordinates.
	static void                 PrintError( void );

	// @algo GetMousePosition
	// @out The current mouse position in screen coordinates.
	Point                       GetMousePosition( void ) const;
	
	// @algo GetMouseMovement
	// @out The current mouse movement.
	Point                       GetMouseMovement( void ) const;
	
	// @algo SetMousePosition
	// @info Forces the mouse cursor to a new position (without counting that as a mouse movement).
	// @in x, y -> The new mouse cursor position in screen coordinates.
	void                        SetMousePosition(int x, int y); // make sure to negate
	
	// @algo SetMousePosition
	// @info Forces the mouse cursor to a new position (without counting that as a mouse movement).
	// @in p -> The new mouse cursor position in screen coordinates.
	void                        SetMousePosition(Point p);

	// @algo GetWorldPosition
	// @out The mouse position in world coordinates.
	mmlVector<2>                GetWorldMousePosition( void ) const;
	
	// @algo GetWorldMouseMovement
	// @out The mouse movement in world coordinates.
	mmlVector<2>                GetWorldMouseMovement( void ) const;

	// @algo IsDown
	// @in key -> The key button constant corresponding to the key.
	// @out TRUE if the key is held down.
	bool                        IsDown(SDLKey key) const;
	
	// @algo IsUp
	// @in key -> The key button constant corresponding to the key.
	// @out TRUE if the key is untouched.
	bool                        IsUp(SDLKey key) const;
	
	// @algo IsPressed
	// @in key -> The key button constant corresponding to the key.
	// @out TRUE on the frame the key is pressed. FALSE on all other frames.
	bool                        IsPressed(SDLKey key) const;
	
	// @algo IsHeld
	// @in key -> The key button constant corresponding to the key.
	// @out TRUE on frame AFTER press and BEFORE release. FALSE on all other frames.
	bool                        IsHeld(SDLKey key) const;
	
	// @algo IsReleased
	// @in key -> The key button constant corresponding to the key.
	// @out TRUE on frame of release. FALSE on all other frames.
	bool                        IsReleased(SDLKey key) const;

	// @algo IsDown
	// @in button -> The mouse button constant corresponding to the key.
	// @out TRUE if the key is held down.
	bool                        IsDown(MouseButton::Button button) const;
	
	// @algo IsUp
	// @in button -> The mouse button constant corresponding to the key.
	// @out TRUE if the key is untouched.
	bool                        IsUp(MouseButton::Button button) const;
	
	// @algo IsPressed
	// @in button -> The mouse button constant corresponding to the key.
	// @out TRUE on the frame the mouse is pressed. FALSE on all other frames.
	bool                        IsPressed(MouseButton::Button button) const;
	
	// @algo IsHeld
	// @in button -> The mouse button constant corresponding to the key.
	// @out TRUE on frame AFTER press and BEFORE release. FALSE on all other frames.
	bool                        IsHeld(MouseButton::Button button) const;
	
	// @algo IsReleased
	// @info
	// @in button -> The mouse button constant corresponding to the key.
	// @inout
	// @out TRUE on frame of release. FALSE on all other frames.
	bool                        IsReleased(MouseButton::Button button) const;

	// @algo IsAnyDown
	// @out TRUE if the any key or mouse button is held down.
	bool                        IsAnyDown( void ) const;
	
	// @algo IsAnyUp
	// @out TRUE if the key or mouse button is untouched.
	bool                        IsAnyUp( void ) const;
	
	// @algo IsAnyPressed
	// @out TRUE on the frame any key or mouse button is pressed. FALSE on all other frames.
	bool                        IsAnyPressed( void ) const;
	
	// @algo IsAnyHeld
	// @info
	// @in
	// @inout
	// @out TRUE on frame AFTER press and BEFORE release. FALSE on all other frames
	bool                        IsAnyHeld( void ) const;
	
	// @algo IsAnyReleased
	// @out TRUE on frame of release. FALSE on all other frames.
	bool                        IsAnyReleased( void ) const;

	// @algo DisableOcclusion
	// @info Disables all occlusion and renders objects in initialization order.
	void                        DisableOcclusion( void );
	
	// @algo SetOcclusionMethod
	// @info Changes the occlusion method to render objects by.
	// @in method -> The specified occlusion method.
	void                        SetOcclusionMethod(OcclusionMethod method);

	// @algo SetClearColor
	// @info Sets screen clear color.
	// @in
	//   r -> red (0-1)
	//   g -> green (0-1)
	//   b -> blue (0-1)
	void                        SetClearColor(float r, float g, float b);
	
	// @algo SetClearColor
	// @info Sets screen clear color.
	// @in color -> RGB (0-1)
	void                        SetClearColor(mmlVector<3> color);

	// @algo AddForm
	// @info Creates a GUI form.
	// @out The created GUI form.
	template < typename form_t >
	mtlShared<GUI::Form>        AddForm( void );

	// @algo RegisterType
	// @info Registers a object type name and a initialization function pointer so that a game object can be created via string.
	// @note Use RegisterObject macro instead.
	// @in
	//   typeName -> The C++ class name of the object (although can be any string).
	//   creator_func -> The function wrapping the object constructor.
	// @out Always returns TRUE.
	static bool                 RegisterType(const mtlChars &typeName, ObjectRef (*creator_func)()); // don't call this manually
	
	// @algo GetRegisteredTypes
	// @inout The list of strings containing names of registered types.
	static void                 GetRegisteredTypes(mtlList< mtlShared<mtlString> > &types);

	// @algo GetSelf
	// @info Can be used to return the ObjectRef of the input game object pointer.
	// @in self -> A game object pointer.
	// @out The ObjectRef of the object. NULL if not found.
	ObjectRef                   GetSelf(const Object *self) const;

	// @algo GetScreenPoint
	// @in world_point -> A world point.
	// @out The screen space point corresponding to the input world point.
	mmlVector<2>                GetScreenPoint(const mmlVector<2> &world_point) const;

	// @algo GetLoopCounter
	// @out The number of iterations performed by the main game loop.
	unsigned long long          GetLoopCounter( void ) const;
	
	// @algo GetPendingObjectsCount
	// @out The number of game objects that are created but are still pending to be initialized.
	int                         GetPendingObjectsCount( void ) const;
};

// @algo RegisterObject
// @info Registers a game object C++ class name in order to enable creation and initialization by string.
// @in ObjectTypeName -> The C++ class type name of the game object (without quotation marks).
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
