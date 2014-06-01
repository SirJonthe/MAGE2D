#ifndef OBJECT_H
#define OBJECT_H

#include "Common.h"
#include "Transform.h"
#include "MTL/mtlString.h"
#include "MTL/mtlType.h"
#include "Collider.h"

class Engine;

class Object : public mtlBase
{
	friend class Engine;
private:
	GraphicsInstance			m_graphics; // only to get polymorphism
	mtlString					m_name;
	Transform					m_transform;
	bool						m_destroy;
	bool						m_collisions;
	bool						m_visible;
	bool						m_frozen;			// used by the debugger object to freeze game state
	mtlShared<Collider>			m_collider;
	unsigned long long			m_objectFlags;		// what is the object?
	unsigned long long			m_collisionMask;	// what can the object collide with?
	const unsigned long long	m_objectNumber;
	Engine						*m_engine;
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
	void				ToggleCollisions( void );
	bool				GetCollisionMask(unsigned int bit) const;
	unsigned int		GetCollisionMasks(unsigned int mask = 0xffffffff) const;
	void				SetCollisionMask(unsigned int bit, bool state);
	void				SetCollisionMasks(unsigned int mask);
	void				ClearAllCollisionMasks( void );
	void				DestroyCollider( void );
	const Collider		*GetCollider( void ) const;
	Collider			*GetCollider( void );
	void				SetCollider(mtlShared<Collider> &collider);
	template < typename collider_t >
	void				LoadCollider( void );

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
	void				DestroyGraphics( void );
	const Graphics		*GetGraphics( void ) const;
	void				SetGraphics(const mtlAsset<Graphics> &graphics);
	template < typename graphics_t >
	bool				LoadGraphics(const mtlChars &file);

	const Engine		*GetEngine( void ) const;
	Engine				*GetEngine( void );
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

template < typename collider_t >
void Object::LoadCollider( void )
{
	m_collider.New<collider_t>();
}

template < typename graphics_t >
bool Object::LoadGraphics(const mtlChars &file)
{
	m_graphics.Delete();
	mtlAsset<Graphics> asset = mtlAsset<Graphics>::Load<graphics_t>(file);
	if (asset.GetAsset() == NULL) { return false; }
	m_graphics = asset.GetAsset()->CreateInstance();
	GetGraphics()->GetTransform().SetParent(&m_transform);
	return m_graphics.GetGraphics() != NULL && m_graphics.GetGraphics()->GetError().GetSize() == 0;
}

#endif // OBJECT_H
