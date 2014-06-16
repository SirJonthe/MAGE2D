#include "Object.h"
#include "Engine.h"

ENGINE_REGISTER_OBJECT_TYPE(Object);

unsigned long long int GetObjectNumber( void )
{
	static unsigned long long int objectCounter = 0;
	return objectCounter++;
}

Object::Object( void ) :
	m_graphics(), m_name(), m_transform(),
	m_destroy(false), m_collisions(true), m_visible(true), m_frozen(false),
	m_collider(),
	m_objectFlags(0x0000000000000001), m_collisionMask(0xffffffffffffffff), m_objectNumber(GetObjectNumber()),
	m_engine(NULL)
{
	m_name.Copy("object_generic");
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

bool Object::IsTicking( void ) const
{
	return !m_frozen && !m_destroy;
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

unsigned long long Object::GetCollisionMasks(unsigned int mask) const
{
	return m_collisionMask & mask;
}

void Object::SetCollisionMask(unsigned int bit, bool state)
{
	unsigned int bitState = state ? 1 : 0;
	unsigned int mask = bitState << bit;
	m_collisionMask |= mask;
}

void Object::SetCollisionMasks(unsigned int mask)
{
	m_collisionMask |= mask;
}

void Object::ClearAllCollisionMasks( void )
{
	m_collisionMask = 0x0000000000000000;
}

void Object::DestroyCollider( void )
{
	m_collider.Delete();
}

const Collider *Object::GetCollider( void ) const
{
	return m_collider.GetShared();
}

Collider *Object::GetCollider( void )
{
	return m_collider.GetShared();
}

void Object::SetCollider(mtlShared<Collider> &collider)
{
	m_collider = collider;
}

Transform &Object::GetTransform( void )
{
	return m_transform;
}

const Transform &Object::GetTransform( void ) const
{
	return m_transform;
}

bool Object::GetObjectFlag(unsigned int bit) const
{
	return (bool)((m_objectFlags>>bit) & 1);
}

unsigned long long Object::GetObjectFlags(unsigned int mask) const
{
	return m_objectFlags & mask;
}

void Object::SetObjectFlag(unsigned int bit, bool state)
{
	unsigned int bitState = state ? 1 : 0;
	unsigned int mask = bitState << bit;
	m_objectFlags |= mask;
}

void Object::SetObjectFlags(unsigned int mask)
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
	m_graphics.GetTransform().SetParentTransform(&m_transform);
}

void Object::SetGraphics(const GraphicsInstance &graphics)
{
	m_graphics = graphics;
	m_graphics.GetTransform().SetParentTransform(&m_transform);
}

const Engine *Object::GetEngine( void ) const
{
	return m_engine;
}

Engine *Object::GetEngine( void )
{
	return m_engine;
}
