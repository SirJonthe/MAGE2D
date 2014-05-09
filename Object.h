#ifndef OBJECT_H
#define OBJECT_H

#include "Sprite.h"
#include "Common.h"
#include "Transform.h"
#include "MTL/mtlString.h"
#include "Collider.h"

class Engine;
class Object;

class EngineInterface
{
	friend class Engine;
private:
	Engine *m_engine;
public:
	EngineInterface( void );
	void						AddObject(Object *object);
	void						DestroyAllObjects( void );
	void						EndGame( void );
	void						KillProgram( void );
	float						GetDeltaTime( void ) const;
	const mtlList<SDL_Event>	&GetEventList( void ) const;
	int							GetRandom( void ) const;
	int							GetRandom(int max) const;
	int							GetRandom(int min, int max) const;
	const mtlList<Object*>		&GetObjects( void ) const;
	bool						PlayMusic(const mtlChars &file);
	void						StopMusic( void );
};

class Object : protected EngineInterface, public Base
{
	friend class Engine;
private:
	mtlAsset<Graphics>			m_graphics;
	mtlString					m_name;
	Transform					m_transform;
	bool						m_destroy;
	bool						m_collisions;
	bool						m_visible;
	bool						m_frozen;			// used by the debugger object to freeze game state
	Collider					*m_collider;
	unsigned int				m_objectFlags;		// what is the object?
	unsigned int				m_collisionMask;	// what can the object collide with?
	const unsigned long long	m_objectNumber;
protected:
	virtual void	OnInit( void ) {}
	virtual void	OnUpdate( void ) {}
	virtual void	OnCollision(Object&) {}
	virtual void	OnGUI( void ) {}
	virtual void	OnFinal( void ) {}
private:
	Object(const Object&) : m_objectNumber(0) {}
	Object &operator=(const Object&) { return *this; }
public:
	explicit			Object( void );
	virtual				~Object( void ) {}

	const mtlString		&GetName( void ) const;
	void				SetName(const mtlChars &name);

	unsigned long long	GetObjectNumber( void ) const;

	void				Destroy( void );
	bool				IsTicking( void ) const;

	bool				IsCollidable( void ) const;
	void				DisableCollisions( void );
	void				EnableCollisions( void );
	bool				GetCollisionMask(unsigned int bit) const;
	unsigned int		GetCollisionMasks(unsigned int mask = 0xffffffff) const;
	void				SetCollisionMask(unsigned int bit, bool state);
	void				SetCollisionMasks(unsigned int mask);
	void				ClearAllCollisionMasks( void );

	Transform			&GetTransform( void );
	const Transform		&GetTransform( void ) const;

	template < typename object_t >
	const object_t		*GetAsType( void ) const;
	template < typename object_t >
	object_t			*GetAsType( void );
	bool				GetObjectFlag(unsigned int bit) const;
	unsigned int		GetObjectFlags(unsigned int mask = 0xffffffff) const;
	void				SetObjectFlag(unsigned int bit, bool state);
	void				SetObjectFlags(unsigned int mask);
	void				ClearAllObjectFlags( void );

	bool				IsVisible( void ) const;
	void				EnableGraphics( void );
	void				DisableGraphics( void );
	void				ToggleGraphics( void );

	// const Collider *GetCollider( void ) const;
	// Collider *GetCollider( void );
	// template < typename collision_t >
	// void LoadCollider( void );

	const Graphics *GetGraphics( void ) const;
	template < typename graphics_t >
	bool LoadGraphics(const mtlChars &file);
};

template < typename object_t >
const object_t *Object::GetAsType( void ) const
{
	return dynamic_cast<const object_t*>(this);
}

template < typename object_t >
object_t *Object::GetAsType( void )
{
	return dynamic_cast<object_t*>(this);
}

template < typename graphics_t >
bool Object::LoadGraphics(const mtlChars &file)
{
	return m_graphics.Load<graphics_t>(file);
}

#endif // OBJECT_H
