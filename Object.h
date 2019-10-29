#ifndef OBJECT_H
#define OBJECT_H

#include <climits>
#include "Common.h"
#include "Transform.h"
#include "MiniLib/MTL/mtlString.h"
#include "MiniLib/MTL/mtlType.h"
#include "Collider.h"
#include "Graphics.h"
#include "DebugRender.h"
#include "Physics.h"

class Engine;

class Object;

// @data ObjectRef
// @info A shared reference to a game object.
typedef mtlShared<Object> ObjectRef;

// @data Object
// @info The main building block of the engine representing an object in the game. Requires the user to extend functionality using the On* functions.
// @note Extending the Object class should be done using conveinience functions ObjectDeclaration, InheritedObjectDeclaration, ConstructObject, and ConstructInheritedObject as well as RegisterObject.
class Object : public mtlBase
{
	friend class Engine;

public:
	// @data default_object
	// @info The flags used for the most generic object (i.e. at least one bit flagged 1).
	static const flags_t default_object = 1;
	
	// @data ruleset_object
	// @info The flags that represent an object that has no physical presence, but still acts on other objects (like establishes rules and relationships between other objects).
	static const flags_t ruleset_object = (1ull << (sizeof(flags_t)*CHAR_BIT - 1));

private:
	GraphicsInstance          m_graphics;
	mtlString                 m_name;
	mtlShared<Transform>      m_transform;
	bool                      m_destroy;
	bool                      m_kill;
	bool                      m_collisions;
	bool                      m_visible;
	bool                      m_frozen; // used by the debugger object to freeze game state
	mtlShared<Collider>       m_collider;
	flags_t                   m_objectFlags; // what is the object?
	flags_t                   m_collisionMask; // what can the object collide with?
	const unsigned long long  m_objectNumber;
	Engine                   *m_engine;
	//const ObjectRef          *m_objectRef;
	float                     m_depth;
	bool                      m_draw_debug_info;
	mtlShared<Graphics>       m_debug_collider_graphics;
	mtlShared<Physics>        m_physics_object;
	bool                      m_physics;

protected:
	// @algo OnInit
	// @info Called once when the object is initialized.
	// @note This function is not necessarily called when the object is constructed.
	virtual void OnInit( void )                               {}
	
	// @algo OnDestroy
	// @info Code that should be executed when the object is removed from the game.
	// @note There are some instances where this will not be called, such as killing objects or the program itself. Therefore, do not use this function to free memory unless the memory is also freed by the destructor.
	virtual void OnDestroy( void )                            {}
	
	// @algo OnUpdate
	// @info The main workhorse for any object. Is called once every frame. Suitable to contain main game object logic.
	virtual void OnUpdate( void )                             {}
	
	// @algo OnCollision
	// @info Called for both objects once their colliders overlap. This code should not resolve collisions (this is done by the engine). Instead this function should establish what happens to the objects' states.
	// @in
	//   ObjectRef -> The other colliding object.
	//   CollisionInfo -> The information regarding the collision as such.
	virtual void OnCollision(ObjectRef, const CollisionInfo&) {}
	
	// @algo OnDraw
	// @info Renders the graphics. The user most not extend this unless the user wants custom rendering. If not, the graphics are displayed in default fashion.
	virtual void OnDraw( void )                               { m_graphics.Draw(); }
	
	// @algo OnGUI
	// @info For use when the user wants to draw GUI. The render state is automatically reset and set to a state appropriate for rendering GUI.
	virtual void OnGUI( void )                                {}
	
	// @algo
	// @info Like OnUpdate, but happens as a finishing step after all other On* functions have been called by the engine.
	virtual void OnFinal( void )                              {}

private:
	Object(const Object&) : mtlBase(this), m_objectNumber(0) {}
	Object &operator=(const Object&) { return *this; }

	void DrawDebugInfo( void );
	bool ApplyForce(const Physics::Force &force, bool falloff, float length);

public:
	explicit                Object( void );
	virtual                ~Object( void ) {}
	
	// @algo Execute
	// @info Executes a command (virtual). Does nothing by default.
	// @in mtlChars -> The command in string format.
	virtual void            Execute(const mtlChars&) {}

	// @algo GetName
	// @out The internal, possibly unique, name of the object.
	const mtlString        &GetName( void ) const;
	
	// @algo SetName
	// @info Changes internal name of the object.
	// @in The new name.
	void                    SetName(const mtlChars &name);

	// @algo GetObjectNumber
	// @out The sequential UUID of the object.
	unsigned long long      GetObjectNumber( void ) const;

	// @algo Destroy
	// @info The public interface for destroying the object. Calls OnDestroy.
	void                    Destroy( void );
	
	// @algo Kill
	// @info Kills the object by destroying it, but not calling OnDestroy.
	void                    Kill( void );
	
	// @algo IsTicking
	// @info Checks if the object is disabled or destroyed.
	// @out TRUE if object is not disabled or destroyed/killed.
	bool                    IsTicking( void ) const;
	
	// @algo IsDestroyed
	// @out TRUE if the object is destroyed/killed.
	bool                    IsDestroyed( void ) const;

	// @algo IsCollideable
	// @out TRUE if object has collider and is set to be able to interact with other colliders.
	bool                    IsCollidable( void ) const;
	
	// @algo DisableCollisions
	// @info Disables collisions.
	void                    DisableCollisions( void );
	
	// @algo EnableCollisions
	// @info Enables collisions.
	void                    EnableCollisions( void );
	
	// @algo ToggleCollisions
	// @info Toggles collisions on/off.
	void                    ToggleCollisions( void );
	
	// @algo GetCollisionMask
	// @info Returns the individual bit flag from the collision mask.
	// @in bit -> The bit index.
	// @out The flag state.
	bool                    GetCollisionMask(unsigned int bit) const;
	
	// @algo GetCollisionMasks
	// @info Returns the requested flags from the collision mask.
	// @in mask -> The mask used to omit some results (bitwise and).
	// @out The requested flags from the collision mask.
	flags_t                 GetCollisionMasks(flags_t mask = AllFlagsOn) const;
	
	// @algo SetCollisionMask
	// @info Sets the state of the specified flag in the collision mask.
	// @in
	//   bit_index -> The index of the bit.
	//   state -> The state of the bit.
	void                    SetCollisionMask(unsigned int bit_index, bool state);
	
	// @algo SetCollisionMasks
	// @info Enables the provided flags (bitwise or).
	// @in mask -> The collision flags the user wants to enable.
	void                    SetCollisionMasks(flags_t mask);
	
	// @algo ClearAllCollisionMasks
	// @info Sets all collision flags to 0.
	void                    ClearAllCollisionMasks( void );
	
	// @algo DestroyCollider
	// @info Removes the collider from the object.
	void                    DestroyCollider( void );
	
	// @algo GetCollider
	// @out The object collider.
	const Collider         *GetCollider( void ) const;
	Collider               *GetCollider( void );
	
	// @algo LoadCollider
	// @info Loads a collider of the specified type.
	template < typename collider_t >
	void                    LoadCollider( void );

	// @algo GetTransform
	// @info Returns the transform of the object.
	Transform              &GetTransform( void );
	const Transform        &GetTransform( void ) const;

	// @algo GetDepth
	// @info The depth value of the object (i.e. an imaginary Z-axis value) used for depth sorting.
	float                   GetDepth( void ) const;
	
	// @algo SetDepth
	// @info Alters the depth value.
	// @in depth -> The new depth value used for depth sorting.
	void                    SetDepth(float depth);

	// @algo GetAsDynamicType
	// @info Returns a pointer to the requested type provided the object can be casted to that type using C++ RTTI.
	// @out The type pointer. NULL on fail.
	template < typename object_t >
	const object_t         *GetAsDynamicType( void ) const;
	template < typename object_t >
	object_t               *GetAsDynamicType( void );
	
	// @algo IsDynamicType
	// @info Checks if the object can be casted to the requested type using C++ RTTI.
	// @out TRUE if casting is possible.
	template < typename object_t >
	bool                    IsDynamicType( void ) const;
	
	// @algo IsStaticType
	// @info Checks if the object can be casted to the requested type using MiniLib RTTI.
	// @out TRUE if casting is possible.
	template < typename object_t >
	bool                    IsStaticType( void ) const;
	
	// @algo IsStaticType
	// @info Checks if the object can be casted to the requested type using MiniLib RTTI.
	// @in id -> The MiniLib type ID.
	// @out TRUE if casting is possible.
	bool                    IsStaticType(mtlTypeID id);
	
	// @algo
	// @info Returns the individual bit flag from the object mask.
	// @in bit -> The bit index.
	// @out The flag state.
	bool                    GetObjectFlag(unsigned int bit) const;
	
	// @algo
	// @info Enables the provided flags (bitwise or).
	// @in mask -> The collision flags the user wants to enable.
	// @out The requested flag states.
	flags_t                 GetObjectFlags(flags_t mask = AllFlagsOn) const;
	
	// @algo SetObjectFlag
	// @info Sets the state of the specified flag in the object mask.
	// @in
	//   bit_index -> The index of the bit.
	//   state -> The state of the bit.
	void                    SetObjectFlag(unsigned int bit, bool state);
	
	// @algo SetObjectFlags
	// @info Enables the provided flags (bitwise or).
	// @in mask -> The collision flags the user wants to enable.
	void                    SetObjectFlags(flags_t mask);
	
	// @algo ClearAllObjectFlags
	// @info Sets all object flags to 0.
	void                    ClearAllObjectFlags( void );

	// @algo IsVisible
	// @out TRUE if the visibility flag is set to TRUE.
	bool                    IsVisible( void ) const;
	
	// @algo EnableGraphics
	// @info Enables graphics making object visible if there is a valid attached graphics object.
	void                    EnableGraphics( void );
	
	// @algo DisableGraphics
	// @info Disables graphics, making the object invisible.
	void                    DisableGraphics( void );
	
	// @algo ToggleGraphics
	// @info Toggles graphics on/off.
	void                    ToggleGraphics( void );
	
	// @algo DestroyGraphics
	// @info Frees graphics object resources.
	void                    DestroyGraphics( void );
	
	// @algo GetGraphics
	// @out Returns the graphics object.
	const GraphicsInstance &GetGraphics( void ) const;
	GraphicsInstance       &GetGraphics( void );
	
	// @algo SetGraphics
	// @info Attaches a new graphics object.
	// @in graphics -> A shared graphics asset used to create a new graphics instance.
	void                    SetGraphics(const mtlAsset<Graphics> &graphics);
	
	// @algo SetGraphics
	// @info Attaches new graphics object.
	// @in graphics -> A new graphics instance.
	void                    SetGraphics(const GraphicsInstance &graphics);
	
	// @algo LoadGraphics
	// @info Loads the specified graphics tyoe from a file.
	// @in file -> The path to the serialized graphics object.
	// @out TRUE if file operation was success.
	template < typename graphics_t >
	bool                    LoadGraphics(const mtlChars &file);
	
	// @algo DrawGraphics
	// @info Draws the graphics. Usually not a function the user will call manually unless custom rendering code is used.
	void                    DrawGraphics( void );
	
	// @algo Enable debug graphics.
	// @info Enables collider outline.
	void                    EnableDebugGraphics( void );
	
	// @algo DisableDebugGraphics
	// @info Disables collider outline.
	void                    DisableDebugGraphics( void );

	// @algo GetEngine
	// @out The engine object that the game object is attached to.
	const Engine           *GetEngine( void ) const;
	Engine                 *GetEngine( void );

	// @algo MakeRulesetObject
	// @info Sets up the current object as a ruleset object (i.e. invisible, uncollideable, that only establishes rules and relationships with other objects).
	void                    MakeRulesetObject( void );

	// @algo GetSelfRef
	// @out The ObjectRef of the game object.
	ObjectRef               GetSelfRef( void );

	// @algo GetPhysics
	// @out The physics data.
	Physics                &GetPhysics( void );
	const Physics          &GetPhysics( void ) const;
	
	// @algo EnablePhysics
	// @info Enables physics.
	void                    EnablePhysics( void );
	
	// @algo DisablePhysics
	// @info Disables physics.
	void                    DisablePhysics( void );
	
	// @algo TogglePhysics
	// @info Toggles physics on/off.
	void                    TogglePhysics( void );
	
	// @algo HasPhysics
	// @out TRUE if physics is enabled.
	bool                    HasPhysics( void ) const;
	
	// @algo ApplyForce
	// @info Adds a force to the physics object.
	// @in force -> The force to be applied to the object.
	// @out TRUE if the object is collideable, has physics, and the force hits the collider.
	bool                    ApplyForce(const Physics::Force &force);
	
	// @algo ApplyForce
	// @info Adds a force to the physics object.
	// @in
	//   force -> The force to be applied to the object.
	//   length -> The maximum reach of the force.
	// @out TRUE if the object is collideable, has physics, and the force hits the collider.
	bool                    ApplyForce(const Physics::Force &force, float length);
};

#include "Engine.h"

template < typename object_t >
const object_t *Object::GetAsDynamicType( void ) const
{
	return dynamic_cast<const object_t*>(this);
}

template < typename object_t >
object_t *Object::GetAsDynamicType( void )
{
	return dynamic_cast<object_t*>(this);
}

template < typename object_t >
bool Object::IsDynamicType( void ) const
{
	return GetAsDynamicType<object_t>() != NULL;
}

template < typename object_t >
bool Object::IsStaticType( void ) const
{
	return object_t::GetClassType() == GetInstanceType();
}

template < typename collider_t >
void Object::LoadCollider( void )
{
	mtlShared<collider_t> collider;
	collider.New();
	m_collider = collider;
	m_collider->SetTransform(m_transform);
	if (m_graphics.IsGood()) {
		m_collider.GetShared()->SetHalfExtents((float)m_graphics.GetGraphicsWidth() * 0.5f, (float)m_graphics.GetGraphicsHeight() * 0.5f);
	}

	mtlShared< ColliderGraphics<collider_t> > collider_graphics;
	collider_graphics.New();
	collider_graphics.GetShared()->SetCollider(collider);
	m_debug_collider_graphics = collider_graphics;
}

template < typename graphics_t >
bool Object::LoadGraphics(const mtlChars &file)
{
	m_graphics = mtlAsset<Graphics>::Load<graphics_t>(file);
	return m_graphics.GetGraphics().GetAsset() != NULL;
}

// @algo
// @info
#define ObjectDeclaration(ObjectName) class ObjectName : public mtlInherit<Object, ObjectName>

// @algo
// @info
#define InheritedObjectDeclaration(ObjectName, Inherited) class ObjectName : public mtlInherit<Inherited, ObjectName>

// @algo
// @info
#define ConstructObject(ObjectName) mtlInherit<Object, ObjectName>(this)

// @algo
// @info
#define ConstructInheritedObject(ObjectName, Inherited) mtlInherit<Inherited, ObjectName>(this)

#endif // OBJECT_H
