#include "Object.h"
#include "Engine.h"

ENGINE_REGISTER_OBJECT_TYPE(Object);

EngineInterface::EngineInterface( void ) : m_engine(NULL) {}

static unsigned long long int g_objectCounter = 0;

void EngineInterface::AddObject(Object *object)
{
	m_engine->AddObject(object);
}

void EngineInterface::DestroyAllObjects( void )
{
	m_engine->DestroyAllObjects();
}

void EngineInterface::EndGame( void )
{
	m_engine->EndGame();
}

void EngineInterface::KillProgram( void )
{
	m_engine->KillProgram();
}

float EngineInterface::GetDeltaTime( void ) const
{
	return m_engine->GetDeltaTime();
}

const mtlList<SDL_Event> &EngineInterface::GetEventList( void ) const
{
	return m_engine->GetEventList();
}

int EngineInterface::GetRandom( void ) const
{
	return m_engine->GetRandom();
}

int EngineInterface::GetRandom(int max) const
{
	return m_engine->GetRandom(max);
}

int EngineInterface::GetRandom(int min, int max) const
{
	return m_engine->GetRandom(min, max);
}

const mtlList<Object*> &EngineInterface::GetObjects( void ) const
{
	return m_engine->GetObjects();
}

bool EngineInterface::PlayMusic(const mtlChars &file)
{
	return m_engine->PlayMusic(file);
}

void EngineInterface::StopMusic( void )
{
	return m_engine->StopMusic();
}

//ENGINE_REGISTER_OBJECT_TYPE(Object);

Object::Object( void ) : EngineInterface(), m_transform(), m_destroy(false), m_collisions(true), m_visible(true), m_frozen(false), m_collider(NULL), m_objectFlags(0x00000001), m_collisionMask(0xffffffff), m_objectNumber(g_objectCounter++)
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
	return m_collisions && m_collider != NULL;
}

void Object::DisableCollisions( void )
{
	m_collisions = false;
}

void Object::EnableCollisions( void )
{
	m_collisions = true;
}

bool Object::GetCollisionMask(unsigned int bit) const
{
	return (bool)((m_collisionMask>>bit) & 1);
}

unsigned int Object::GetCollisionMasks(unsigned int mask) const
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
	m_collisionMask = 0x00000000;
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

unsigned int Object::GetObjectFlags(unsigned int mask) const
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
	m_objectFlags = 0x00000000;
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

const Graphics *Object::GetGraphics( void ) const
{
	return m_graphics.GetAsset();
}
