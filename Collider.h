#ifndef COLLIDER_H
#define COLLIDER_H

#include "Common.h"
#include "Sprite.h"
#include "MTL/mtlType.h"

//http://xbox.create.msdn.com/en-US/education/catalog/tutorial/collision_2d_perpixel_transformed

// NOTES
// All shapes are centered around 0,0

class Collider;
class PolygonCollider; // arbitrarily shaped polygon

struct Ray
{
	mmlVector<2>	origin;
	mmlVector<2>	direction;
	float			length;
};

struct Range
{
	mmlVector<2>	origin;
	mmlVector<2>	direction;
	float			length;
	float			apexRadians;
};

struct Plane // a plane in 2d is actually just a line
{
	mmlVector<2>	point;
	mmlVector<2>	normal;
};

struct UnaryCollisionInfo
{
	const Collider				*collider;
	ShallowArray(mmlVector<2>)	points; // intersections
	//ShallowArray(Ray)			reflection;
	bool						collision;
};

UnaryCollisionInfo RayCollide(Ray r, mmlVector<2> a, mmlVector<2> b);
UnaryCollisionInfo RangeCollide(Range r, mmlVector<2> a);
UnaryCollisionInfo RangeCollide(Range r, mmlVector<2> a, mmlVector<2> b);
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a);
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a, mmlVector<2> b);

class Collider;
class PolygonCollider;

struct CollisionInfo
{
	const Collider				*c1;
	const Collider				*c2;
	ShallowArray(mmlVector<2>)	c1_points;
	ShallowArray(mmlVector<2>)	c2_points;
	mmlVector<2>				avg_intersection;
	int							unique_points;
	bool						collision;
};

bool LineIntersection(mmlVector<2> a1, mmlVector<2> a2, mmlVector<2> b1, mmlVector<2> b2, mmlVector<2> &out);
bool PointInPolygon(mmlVector<2> a, const mtlArray< mmlVector<2> > &poly);

// NOTE:
// If objects interpenetrate apply a penalty force
// so we can later apply calculations that will
// accurately alter trajectory and rotational angle.
// This penalty can be seen as a spring that compresses
// more the more two objects interpenetrate meaning
// there are forces that are trying to separate them.

class Collider : public mtlBase
{
	friend class PolygonCollider;

protected:
	Transform *m_transform;
	Transform m_prevTransform;
protected:
	virtual CollisionInfo CollidesWith(const Collider&) const			{ CollisionInfo c; c.c1 = NULL; c.c2 = NULL; c.collision = false; return c; }
	virtual CollisionInfo CollidesWith(const PolygonCollider&) const	{ CollisionInfo c; c.c1 = NULL; c.c2 = NULL; c.collision = false; return c; }

public:
	Collider( void );
	virtual ~Collider( void ) {}

	Transform					&GetTransform( void );
	const Transform				&GetTransform( void ) const;
	void						SetTransform(Transform *transform);

	virtual bool				Load(const mtlDirectory &file)		{ return false; }

	virtual void				SetHalfExtents(float w, float h)	{ SetHalfExtents(mmlVector<2>(w, h)); }
	virtual void				SetHalfExtents(mmlVector<2>)		{} // exactly what this does is left undefined, hopefully something reasonable
	virtual mmlVector<2>		GetHalfExtents( void ) const		{ return mmlVector<2>(0.0f, 0.0f); }
	virtual void				ResetState( void )					{}
	void						TrackPreviousTransform( void );

	virtual UnaryCollisionInfo	Collides(Ray) const					{ UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	virtual UnaryCollisionInfo	Collides(Range) const				{ UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	virtual UnaryCollisionInfo	Collides(Plane)	const				{ UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	virtual CollisionInfo		Collides(const Collider&) const		{ CollisionInfo c; c.c1 = NULL; c.c2 = NULL; c.collision = false; return c; }

	void						RevertTransform( void ) const		{ if (m_transform != NULL) { *m_transform = m_prevTransform; } }
};

typedef Collider NullCollider;

class PolygonCollider : public mtlInherit<Collider, PolygonCollider>
{
private:
	mtlArray< mmlVector<2> >	m_vert;			// original data
	mtlArray< mmlVector<2> >	m_globalVert;	// rotated data
	//mtlArray< bool >			m_collide;

public:
	enum Shape
	{
		Triangle,
		Rectangle,
		Hexagon,
		Rhombus
	};

protected:
	CollisionInfo CollidesWith(const Collider&) const { CollisionInfo c; c.c1 = NULL; c.c2 = NULL; c.collision = false; return c; }
	CollisionInfo CollidesWith(const PolygonCollider &c) const;

public:
	PolygonCollider( void );
	explicit PolygonCollider(Shape shape);
	explicit PolygonCollider(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center);

	void CreateShape(Shape shape);

	bool							Load(const mtlDirectory &file);

	int								GetVertexCount( void ) const;
	const mmlVector<2>				&GetVertex(int i) const;
	mmlVector<2>					&GetVertex(int i);
	const mtlArray< mmlVector<2> >	&GetVertexArray( void ) const;
	mtlArray< mmlVector<2> >		&GetVertexArray( void );

	void							CopyVertexArray(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center);

	mmlVector<2>					GetHalfExtents( void ) const;
	void							SetHalfExtents(mmlVector<2> half);
	void							ResetState( void );

	UnaryCollisionInfo				Collides(Ray ray) const;
	UnaryCollisionInfo				Collides(Range range) const;
	UnaryCollisionInfo				Collides(Plane plane) const;
	CollisionInfo					Collides(const Collider &c) const;
};

#endif // COLLIDER_H
