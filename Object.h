#ifndef OBJECT_H
#define OBJECT_H

#include "Common.h"
#include "Transform.h"
#include "MTL/mtlString.h"
#include "MTL/mtlType.h"
#include "Collider.h"
#include "Graphics.h"

class Engine;

class Object : public mtlBase
{
	friend class Engine;
private:
	GraphicsInstance			m_graphics;
	mtlString					m_name;
	//Transform					m_transform; // transform is reverted to an interpolated value between this and next should next be rejected by solver
	//Transform					m_nextTransform; // Get* returns this, Set* modifies this
	bool						m_destroy;
	bool						m_collisions;
	bool						m_visible;
	bool						m_frozen;			// used by the debugger object to freeze game state
	mtlShared<Collider>			m_collider;
	flags_t						m_objectFlags;		// what is the object?
	flags_t						m_collisionMask;	// what can the object collide with?
	const unsigned long long	m_objectNumber;
	Engine						*m_engine;
protected:
	virtual void	OnInit( void ) {}
	virtual void	OnUpdate( void ) {}
	virtual void	OnCollision(Object&) {}
	virtual void	OnDraw( void ) { m_graphics.Draw(); }
	virtual void	OnGUI( void ) {}
	virtual void	OnFinal( void ) {}
private:
	Object(const Object&) : m_objectNumber(0) {}
	Object &operator=(const Object&) { return *this; }
public:
	explicit				Object( void );
	virtual					~Object( void ) {}
	virtual void			SetDefaultValues(const mtlChars &variation) {}

	const mtlString			&GetName( void ) const;
	void					SetName(const mtlChars &name);

	unsigned long long		GetObjectNumber( void ) const;

	void					Destroy( void );
	bool					IsTicking( void ) const;

	bool					IsCollidable( void ) const;
	void					DisableCollisions( void );
	void					EnableCollisions( void );
	void					ToggleCollisions( void );
	bool					GetCollisionMask(unsigned int bit) const;
	flags_t					GetCollisionMasks(flags_t mask = 0xffffffffffffffff) const;
	void					SetCollisionMask(unsigned int bit, bool state);
	void					SetCollisionMasks(flags_t mask);
	void					ClearAllCollisionMasks( void );
	void					DestroyCollider( void );
	const Collider			*GetCollider( void ) const;
	Collider				*GetCollider( void );
	//void					SetCollider(mtlShared<Collider> &collider);
	template < typename collider_t >
	void					LoadCollider( void );

	Transform				&GetTransform( void );
	const Transform			&GetTransform( void ) const;

	template < typename object_t >
	const object_t			*GetAsType( void ) const;
	template < typename object_t >
	object_t				*GetAsType( void );
	bool					GetObjectFlag(unsigned int bit) const;
	flags_t					GetObjectFlags(flags_t mask = 0xffffffffffffffff) const;
	void					SetObjectFlag(unsigned int bit, bool state);
	void					SetObjectFlags(flags_t mask);
	void					ClearAllObjectFlags( void );

	bool					IsVisible( void ) const;
	void					EnableGraphics( void );
	void					DisableGraphics( void );
	void					ToggleGraphics( void );
	void					DestroyGraphics( void );
	const GraphicsInstance	&GetGraphics( void ) const;
	GraphicsInstance		&GetGraphics( void );
	void					SetGraphics(const mtlAsset<Graphics> &graphics);
	void					SetGraphics(const GraphicsInstance &graphics);
	template < typename graphics_t >
	bool					LoadGraphics(const mtlChars &file);

	const Engine			*GetEngine( void ) const;
	Engine					*GetEngine( void );
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
	Transform transform = m_collider.GetShared()->GetTransform();
	m_collider.New<collider_t>();
	m_collider.GetShared()->GetTransform() = transform;
	if (m_graphics.IsGood()) {
		m_collider.GetShared()->SetHalfExtents((float)m_graphics.GetGraphicsWidth() * 0.5f, (float)m_graphics.GetGraphicsHeight() * 0.5f);
	}
}

template < typename graphics_t >
bool Object::LoadGraphics(const mtlChars &file)
{
	m_graphics = mtlAsset<Graphics>::Load<graphics_t>(file);
	return m_graphics.GetGraphics().GetAsset() != NULL;
}

#endif // OBJECT_H
