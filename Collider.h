#ifndef COLLIDER_H
#define COLLIDER_H

#include "Common.h"
#include "Sprite.h"
#include "MiniLib/MTL/mtlType.h"

//http://xbox.create.msdn.com/en-US/education/catalog/tutorial/collision_2d_perpixel_transformed

// NOTES
// All shapes are centered around 0,0

class Collider;
class PolygonCollider; // arbitrarily shaped polygon

struct Ray
{
	mmlVector<2> origin;
	mmlVector<2> direction;
	float        length;
};

struct Cone
{
	mmlVector<2> origin;
	mmlVector<2> direction;
	float        length;
	float        apexRadians;
};

struct Plane // a plane in 2d is actually just a line
{
	mmlVector<2> point;
	mmlVector<2> normal;
};

struct UnaryCollisionInfo
{
	const Collider             *collider;
	ShallowArray(mmlVector<2>)  points; // intersections
	//ShallowArray(Ray)           reflection;
	bool                        collision;
};

mmlVector<2>       LineIntersectionProjection(const mmlVector<2> &a1, const mmlVector<2> &a2, const mmlVector<2> &b1, const mmlVector<2> &b2);
bool               LineIntersection(const mmlVector<2> &a1, const mmlVector<2> &a2, const mmlVector<2> &b1, const mmlVector<2> &b2, mmlVector<2> &out);
bool               PointInPolygon(const mmlVector<2> &a, const mtlArray< mmlVector<2> > &poly);
UnaryCollisionInfo RayCollide(Ray r, mmlVector<2> a, mmlVector<2> b);
UnaryCollisionInfo ConeCollide(Cone r, mmlVector<2> a);
UnaryCollisionInfo ConeCollide(Cone r, mmlVector<2> a, mmlVector<2> b);
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a);
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a, mmlVector<2> b);

class GridWalker
{
private:
	mmlVector<2> m_origin;
	mmlVector<2> m_lengths;
	mmlVector<2> m_direction;
	mmlVector<2> m_delta;
	int          m_xy[2];
	int          m_step[2];
	int          m_side;

public:
						GridWalker( void ) : m_side(0) {}
	explicit            GridWalker(const Ray &p_ray) { SetInitialState(p_ray); }

	void                SetInitialState(const Ray &p_ray);

	void                Step( void );
	int                 GetX( void ) const { return m_xy[0]; }
	int                 GetY( void ) const { return m_xy[1]; }

	int                 GetImpactAxis( void ) const { return m_side; }
	float               GetImpactDistance( void ) const { return fabs((m_xy[m_side] - m_origin[m_side] + (1 - m_step[m_side]) / 2) / m_direction[m_side]); }
	mmlVector<2>        GetImpactPosition( void ) const { return m_origin + m_direction * GetImpactDistance(); }
	mmlVector<2>        GetImpactUV( void ) const;

	const mmlVector<2> &GetOrigin( void ) const { return m_origin; }
};

class Collider;
class PolygonCollider;

struct CollisionInfo
{
	const Collider             *c1;
	const Collider             *c2;
	ShallowArray(mmlVector<2>)  c1_points;
	ShallowArray(mmlVector<2>)  c2_points;
	mmlVector<2>                c1_avg_collision;
	mmlVector<2>                c2_avg_collision;
	bool                        collision;
};

struct BoundRect
{
	mmlVector<2> a, b;

	float GetArea(const BoundRect &r) const
	{
		return mmlMax(b[0] - a[0], 0.0f) * mmlMax(b[1] - a[1], 0.0f);
	}
	static BoundRect GetOverlap(const BoundRect &r1, const BoundRect &r2)
	{
		BoundRect r;
		r.a[0] = mmlMax(r1.a[0], r2.a[0]);
		r.a[1] = mmlMax(r1.a[1], r2.a[1]);
		r.b[0] = mmlMin(r1.b[0], r2.b[0]);
		r.b[1] = mmlMin(r1.b[1], r2.b[1]);
		return r;
	}
};



class Collider : public mtlBase
{
	friend class PolygonCollider;

protected:
	mtlShared<Transform> m_transform;
	//Transform    *m_transform;
	//Transform     m_prevTransform;

protected:
	virtual CollisionInfo CollidesWith(const Collider&) const        { CollisionInfo c; c.c1 = NULL; c.c2 = NULL; c.collision = false; return c; }
	virtual CollisionInfo CollidesWith(const PolygonCollider&) const { CollisionInfo c; c.c1 = NULL; c.c2 = NULL; c.collision = false; return c; }

public:
			 Collider( void );
	virtual ~Collider( void ) {}

	Transform                  &GetTransform( void );
	const Transform            &GetTransform( void ) const;
//	void                        SetTransform(Transform *transform);
	void                        SetTransform(mtlShared<Transform> &transform);

	virtual bool                Load(const mtlPath &file)               { return false; }

	virtual void                SetHalfExtents(float w, float h)        { SetHalfExtents(mmlVector<2>(w, h)); }
	virtual void                SetHalfExtents(const mmlVector<2>&)     {} // exactly what this does is left undefined, hopefully something reasonable
	virtual mmlVector<2>        GetHalfExtents( void ) const            { return mmlVector<2>(0.0f, 0.0f); }
	virtual BoundRect           GetBoundRect( void ) const              { BoundRect r; r.a = mmlVector<2>(0.0f, 0.0f); r.b = mmlVector<2>(0.0f, 0.0f); return r; }
	//virtual void                ResetState( void )                      {}
	virtual void                UpdateWorldState( void )                {}
	//void                        TrackPreviousTransform( void );

	virtual UnaryCollisionInfo  Collides(Ray) const                     { UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	virtual UnaryCollisionInfo  Collides(Cone) const                    { UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	virtual UnaryCollisionInfo  Collides(Plane)	const                   { UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	virtual CollisionInfo       Collides(const Collider&) const         { CollisionInfo c; c.c1 = NULL; c.c2 = NULL; c.collision = false; return c; }

	virtual float               GetCircumference( void ) const          { return 0.0f; }
	virtual float               GetArea( void ) const                   { return 0.0f; }


	//void                        RevertTransform( void ) const           { if (m_transform != NULL) { *m_transform = m_prevTransform; } }
};

typedef Collider NullCollider;

class PolygonCollider : public mtlInherit<Collider, PolygonCollider>
{
private:
	mtlArray< mmlVector<2> > m_vert;       // original data
	mtlArray< mmlVector<2> > m_globalVert; // rotated data
	//mtlArray< bool >         m_collide;

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
	explicit                        PolygonCollider(Shape shape);
	explicit                        PolygonCollider(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center);

	void                            CreateShape(Shape shape);

	bool                            Load(const mtlPath &file);

	int                             GetVertexCount( void ) const;
	const mmlVector<2>             &GetVertex(int i) const;
	mmlVector<2>                   &GetVertex(int i);
	const mtlArray< mmlVector<2> > &GetVertexArray( void ) const;
	mtlArray< mmlVector<2> >       &GetVertexArray( void );

	void                            CopyVertexArray(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center);

	mmlVector<2>                    GetHalfExtents( void ) const;
	void                            SetHalfExtents(const mmlVector<2> &half);
	BoundRect                       GetBoundRect( void ) const;
	//void                            ResetState( void );
	void                            UpdateWorldState( void );

	float                           GetCircumference( void ) const;
	float                           GetArea( void ) const;

	UnaryCollisionInfo              Collides(Ray ray) const;
	UnaryCollisionInfo              Collides(Cone cone) const;
	UnaryCollisionInfo              Collides(Plane plane) const;
	CollisionInfo                   Collides(const Collider &c) const;

	bool                            IsConvex( void ) const;
};

#endif // COLLIDER_H
