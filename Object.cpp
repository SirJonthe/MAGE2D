#include "Object.h"
#include "Engine.h"

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

Object::Object( void ) : EngineInterface(), m_transform(), m_destroy(false), m_collisions(true), m_visible(true), m_frozen(false), m_collider(NULL), m_typeFlags(0x00000001), m_collisionFlags(0xffffffff), m_objectNumber(g_objectCounter++)
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

bool Object::GetCollisionFlag(unsigned int bit) const
{
	return (bool)((m_collisionFlags>>bit) & 1);
}

unsigned int Object::GetCollisionFlags(unsigned int mask) const
{
	return m_collisionFlags & mask;
}

void Object::SetCollisionFlag(unsigned int bit, bool state)
{
	unsigned int bitState = state ? 1 : 0;
	unsigned int mask = bitState << bit;
	m_collisionFlags |= mask;
}

void Object::SetCollisionFlags(unsigned int mask)
{
	m_collisionFlags |= mask;
}

void Object::ClearAllCollisionFlags( void )
{
	m_collisionFlags = 0x00000000;
}

Transform &Object::GetTransform( void )
{
	return m_transform;
}

const Transform &Object::GetTransform( void ) const
{
	return m_transform;
}

bool Object::GetTypeFlag(unsigned int bit) const
{
	return (bool)((m_typeFlags>>bit) & 1);
}

unsigned int Object::GetTypeFlags(unsigned int mask) const
{
	return m_typeFlags & mask;
}

void Object::SetTypeFlag(unsigned int bit, bool state)
{
	unsigned int bitState = state ? 1 : 0;
	unsigned int mask = bitState << bit;
	m_typeFlags |= mask;
}

void Object::SetTypeFlags(unsigned int mask)
{
	m_typeFlags |= mask;
}

void Object::ClearAllTypeFlags( void )
{
	m_typeFlags = 0x00000000;
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
