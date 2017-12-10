#ifndef DEBUGRENDER_H
#define DEBUGRENDER_H

#include "Graphics.h"
#include "Collider.h"

template < typename collider_t >
class ColliderGraphics : public mtlInherit< Graphics, ColliderGraphics<collider_t> >
{
private:
	mtlShared<collider_t> m_collider;

public:
	ColliderGraphics( void ) : mtlInherit< Graphics, ColliderGraphics<collider_t> >(this) {}

	bool Load(const mtlPath&) { return false; }

	int  GetWidth( void )  const { return 0; }
	int  GetHeight( void ) const { return 0; }
	bool IsGood( void )    const { return m_collider.GetShared() != NULL; }

	void Destroy( void )                                   { m_collider.Delete(); }
	void Draw(int, float)                            const {}
	void SetCollider(mtlShared<collider_t> collider)       { m_collider = collider; }

};

template <>
class ColliderGraphics<PolygonCollider> : public mtlInherit< Graphics, ColliderGraphics<PolygonCollider> >
{
private:
	mtlShared<PolygonCollider> m_collider;

public:
	ColliderGraphics( void ) : mtlInherit< Graphics, ColliderGraphics<PolygonCollider> >(this) {}

	bool Load(const mtlPath&)       { return false; }
	int  GetWidth( void )     const { return IsGood() ? (int)(m_collider.GetShared()->GetHalfExtents()[0] * 2.0f) : 0; }
	int  GetHeight( void )    const { return IsGood() ? (int)(m_collider.GetShared()->GetHalfExtents()[1] * 2.0f) : 0; }
	bool IsGood( void )       const { return m_collider.GetShared() != NULL; }
	void Destroy( void )            { m_collider.Delete(); }
	void Draw(int, float)     const
	{
		if (IsGood()) {
			LoadVertexArray(m_collider.GetShared()->GetVertexArray());
			DrawGraphics(0, 0, GL_LINE_LOOP, m_collider.GetShared()->GetVertexCount());
		}
	}

	void SetCollider(mtlShared<PolygonCollider> collider) { m_collider = collider; }
};

#endif // DEBUGRENDER_H
