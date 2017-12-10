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
#include "phonenumbers/phonenumbermatch.h"

class Engine;

class Object;
typedef mtlShared<Object> ObjectRef;

class Object : public mtlBase
{
	friend class Engine;

public:
	static const flags_t default_object = 1;
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
	virtual void OnInit( void )                               {}
	virtual void OnDestroy( void )                            {}
	virtual void OnUpdate( void )                             {}
	virtual void OnCollision(ObjectRef, const CollisionInfo&) {}
	virtual void OnDraw( void )                               { m_graphics.Draw(); }
	virtual void OnGUI( void )                                {}
	virtual void OnFinal( void )                              {}

private:
	Object(const Object&) : mtlBase(this), m_objectNumber(0) {}
	Object &operator=(const Object&) { return *this; }

	void DrawDebugInfo( void );

public:
	explicit                Object( void );
	virtual                ~Object( void ) {}
	virtual void            Execute(const mtlChars&) {}

	const mtlString        &GetName( void ) const;
	void                    SetName(const mtlChars &name);

	unsigned long long      GetObjectNumber( void ) const;

	void                    Destroy( void );
	void                    Kill( void );
	bool                    IsTicking( void ) const;
	bool                    IsDestroyed( void ) const;

	bool                    IsCollidable( void ) const;
	void                    DisableCollisions( void );
	void                    EnableCollisions( void );
	void                    ToggleCollisions( void );
	bool                    GetCollisionMask(unsigned int bit) const;
	flags_t                 GetCollisionMasks(flags_t mask = AllFlagsOn) const;
	void                    SetCollisionMask(unsigned int bit_index, bool state);
	void                    SetCollisionMasks(flags_t mask);
	void                    ClearAllCollisionMasks( void );
	void                    DestroyCollider( void );
	const Collider         *GetCollider( void ) const;
	Collider               *GetCollider( void );
	template < typename collider_t >
	void                    LoadCollider( void );

	Transform              &GetTransform( void );
	const Transform        &GetTransform( void ) const;

	float                   GetDepth( void ) const;
	void                    SetDepth(float depth);

	template < typename object_t >
	const object_t         *GetAsDynamicType( void ) const;
	template < typename object_t >
	object_t               *GetAsDynamicType( void );
	template < typename object_t >
	bool                    IsDynamicType( void ) const;
	template < typename object_t >
	bool                    IsStaticType( void ) const;
	bool                    IsStaticType(mtlTypeID id);
	bool                    GetObjectFlag(unsigned int bit) const;
	flags_t                 GetObjectFlags(flags_t mask = AllFlagsOn) const;
	void                    SetObjectFlag(unsigned int bit, bool state);
	void                    SetObjectFlags(flags_t mask);
	void                    ClearAllObjectFlags( void );

	bool                    IsVisible( void ) const;
	void                    EnableGraphics( void );
	void                    DisableGraphics( void );
	void                    ToggleGraphics( void );
	void                    DestroyGraphics( void );
	const GraphicsInstance &GetGraphics( void ) const;
	GraphicsInstance       &GetGraphics( void );
	void                    SetGraphics(const mtlAsset<Graphics> &graphics);
	void                    SetGraphics(const GraphicsInstance &graphics);
	template < typename graphics_t >
	bool                    LoadGraphics(const mtlChars &file);
	void                    DrawGraphics( void );
	void                    EnableDebugGraphics( void );
	void                    DisableDebugGraphics( void );

	const Engine           *GetEngine( void ) const;
	Engine                 *GetEngine( void );

	void                    MakeRulesetObject( void );

	ObjectRef               GetSelfRef( void );

	Physics                &GetPhysics( void );
	const Physics          &GetPhysics( void ) const;
	void                    EnablePhysics( void );
	void                    DisablePhysics( void );
	void                    TogglePhysics( void );
	bool                    HasPhysics( void ) const;
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

#define ObjectDeclaration(ObjectName) class ObjectName : public mtlInherit<Object, ObjectName>
#define InheritedObjectDeclaration(ObjectName, Inherited) class ObjectName : public mtlInherit<Inherited, ObjectName>
#define ConstructObject(ObjectName) mtlInherit<Object, ObjectName>(this)
#define ConstructInheritedObject(ObjectName, Inherited) mtlInherit<Inherited, ObjectName>(this)

#endif // OBJECT_H
