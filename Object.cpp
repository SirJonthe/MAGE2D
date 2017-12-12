#include "Object.h"
#include <iostream>

RegisterObject(Object);

void Object::DrawDebugInfo( void )
{
	if (m_draw_debug_info) {
		if (m_collisions && m_debug_collider_graphics.GetShared() != NULL) {
			m_debug_collider_graphics.GetShared()->Draw();
		}
	}
}

unsigned long long int GetObjectNumber( void )
{
	static unsigned long long int objectCounter = 0;
	return objectCounter++;
}

Object::Object( void ) :
	mtlBase(this),
	m_graphics(), m_name(),
	m_destroy(false), m_kill(false), m_collisions(true), m_visible(true), m_frozen(false),
	m_collider(),
	m_objectFlags(0x0000000000000001), m_collisionMask(AllFlagsOn), m_objectNumber(GetObjectNumber()),
	m_engine(NULL), /*m_objectRef(NULL),*/
	m_depth(1.0f),
	m_draw_debug_info(false),
	m_physics_object(),
	m_physics(false)
{
	m_transform.New();
	m_physics_object.New();
	m_physics_object->SetTransform(m_transform);
	m_name.Copy("object_generic");
	LoadCollider<NullCollider>();
}

const mtlString &Object::GetName( void ) const
{
	return m_name;
}

void Object::SetName(const mtlChars &name)
{
	m_name.Copy(name);
}

unsigned long long Object::GetObjectNumber( void ) const
{
	return m_objectNumber;
}

void Object::Destroy( void )
{
	m_destroy = true;
}

void Object::Kill( void )
{
	m_destroy = true;
	m_kill = true;
}

bool Object::IsTicking( void ) const
{
	return !m_frozen && !m_destroy;
}

bool Object::IsDestroyed( void ) const
{
	return m_destroy && m_engine == NULL;
}

bool Object::IsCollidable( void ) const
{
	return m_collisions && !m_collider.IsNull();
}

void Object::DisableCollisions( void )
{
	m_collisions = false;
}

void Object::EnableCollisions( void )
{
	m_collisions = true;
}

void Object::ToggleCollisions( void )
{
	m_collisions = !m_collisions;
}

bool Object::GetCollisionMask(unsigned int bit) const
{
	return (bool)((m_collisionMask>>bit) & 1);
}

flags_t Object::GetCollisionMasks(flags_t mask) const
{
	return m_collisionMask & mask;
}

void Object::SetCollisionMask(unsigned int bit_index, bool state)
{
	unsigned int bitState = state ? 1 : 0;
	unsigned int mask = bitState << bit_index;
	m_collisionMask |= mask;
}

void Object::SetCollisionMasks(flags_t mask)
{
	m_collisionMask |= mask;
}

void Object::ClearAllCollisionMasks( void )
{
	m_collisionMask = 0x0000000000000000;
}

void Object::DestroyCollider( void )
{
	LoadCollider<NullCollider>();
}

const Collider *Object::GetCollider( void ) const
{
	return m_collider.GetShared();
}

Collider *Object::GetCollider( void )
{
	return m_collider.GetShared();
}

Transform &Object::GetTransform( void )
{
	return *m_transform.GetShared();
}

const Transform &Object::GetTransform( void ) const
{
	return *m_transform.GetShared();
}

float Object::GetDepth( void ) const
{
	return m_depth;
}

void Object::SetDepth(float depth)
{
	m_depth = depth;
}

bool Object::IsStaticType(mtlTypeID id)
{
	return GetInstanceType() == id;
}

bool Object::GetObjectFlag(unsigned int bit) const
{
	return (bool)((m_objectFlags>>bit) & 1);
}

flags_t Object::GetObjectFlags(flags_t mask) const
{
	return m_objectFlags & mask;
}

void Object::SetObjectFlag(unsigned int bit, bool state)
{
	unsigned int bitState = state ? 1 : 0;
	unsigned int mask = bitState << bit;
	m_objectFlags |= mask;
}

void Object::SetObjectFlags(flags_t mask)
{
	m_objectFlags |= mask;
}

void Object::ClearAllObjectFlags( void )
{
	m_objectFlags = 0x0000000000000000;
}

bool Object::IsVisible( void ) const
{
	return m_visible;
}

void Object::EnableGraphics( void )
{
	m_visible = true;
}

void Object::DisableGraphics( void )
{
	m_visible = false;
}

void Object::ToggleGraphics( void )
{
	m_visible = !m_visible;
}

void Object::DestroyGraphics( void )
{
	m_graphics = mtlAsset<Graphics>();
}

const GraphicsInstance &Object::GetGraphics( void ) const
{
	return m_graphics;
}

GraphicsInstance &Object::GetGraphics( void )
{
	return m_graphics;
}

void Object::SetGraphics(const mtlAsset<Graphics> &graphics)
{
	m_graphics = graphics;
}

void Object::SetGraphics(const GraphicsInstance &graphics)
{
	m_graphics = graphics;
}

void Object::DrawGraphics( void )
{
	m_graphics.Draw();
}

void Object::EnableDebugGraphics( void )
{
	m_draw_debug_info = true;
}

void Object::DisableDebugGraphics( void )
{
	m_draw_debug_info = false;
}

const Engine *Object::GetEngine( void ) const
{
	if (m_engine == NULL) {
		std::cout << "ERROR: Engine null dereference via Object \"" << m_name.GetChars() << "\", 0x" << this << std::endl;
	}
	return m_engine;
}

Engine *Object::GetEngine( void )
{
	if (m_engine == NULL) {
		std::cout << "ERROR: Engine null dereference via Object \"" << m_name.GetChars() << "\", 0x" << this << std::endl;
	}
	return m_engine;
}

void Object::MakeRulesetObject( void )
{
	DestroyGraphics();
	DisableGraphics();
	LoadCollider<NullCollider>();
	DisableCollisions();
	ClearAllCollisionMasks();
	ClearAllObjectFlags();
	SetObjectFlags(Object::ruleset_object);
	DisablePhysics();
}

ObjectRef Object::GetSelfRef( void )
{
	return (GetEngine() != NULL) ? GetEngine()->GetSelf(this) : ObjectRef();
}

Physics &Object::GetPhysics( void )
{
	return *m_physics_object.GetShared();
}

const Physics &Object::GetPhysics( void ) const
{
	return *m_physics_object.GetShared();
}

void Object::EnablePhysics( void )
{
	m_physics = true;
}

void Object::DisablePhysics( void )
{
	m_physics = false;
}

void Object::TogglePhysics( void )
{
	m_physics = !m_physics;
}

bool Object::HasPhysics( void ) const
{
	return m_physics;
}
