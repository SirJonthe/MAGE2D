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

// @data Ray
// @info Represents a ray with a fixed origin extending indefinitely in the direction of the direction vector.
// @note Directions need to be normalized to work dependably.
struct Ray
{
	mmlVector<2> origin;
	mmlVector<2> direction;
//	float        length;
};

// @data Cone
// @info Represents a cone with a fixed origin extending indefinitely in the direction of the direction vector. Uses 'apexRadians' to describe the base of the cone.
// @note Directions need to be normalized to work dependably.
struct Cone
{
	mmlVector<2> origin;
	mmlVector<2> direction;
//	float        length;
	float        apexRadians;
};

// @data Plane
// @info Represents a plane in 2D space (i.e. a line without end segments).
// @note Directions need to be normalized to work dependably.
struct Plane // a plane in 2d is actually just a line
{
	mmlVector<2> point;
	mmlVector<2> normal;
};

// @data UnaryCollisionInfo
// @info Describes collisions between points and basic collision shapes (Ray, Cone, Plane).
struct UnaryCollisionInfo
{
	const Collider             *collider;
	ShallowArray(mmlVector<2>)  points; // intersections
//	ShallowArray(mmlVector<2>)  surf_normals;
	mmlVector<2>                avg_collision;
	bool                        collision;
};

// @algo LineInstersectionProjection
// @info Returns the intersection point between two unbounded lines (i.e. intersection point may occur outside of the defined lines points).
// @in
//   a1, a2 -> define line segment A
//   b1, b2 -> define line segment B.
// @out Intersection point.
mmlVector<2>       LineIntersectionProjection(const mmlVector<2> &a1, const mmlVector<2> &a2, const mmlVector<2> &b1, const mmlVector<2> &b2);

// @algo LineIntersection
// @info Returns the intersection point (if any) between two bounded line segments.
// @in
//   a1, a2 -> Define line segment A
//   b1, b2 -> Define line segment B.
// @inout out -> Stores intersection point if return value is true.
// @out TRUE if intersection detected, FALSE if not.
bool               LineIntersection(const mmlVector<2> &a1, const mmlVector<2> &a2, const mmlVector<2> &b1, const mmlVector<2> &b2, mmlVector<2> &out);

// @algo PointInPolygon
// @info Checks if a given point is inside a given (convex) polygon.
// @in
//   a -> The point to test against the polygon bounds.
//   poly -> The points that make up the polygon. Must be convex polygon.
// @out TRUE if 'a' inside 'poly', otherwise FALSE.
bool               PointInPolygon(const mmlVector<2> &a, const mtlArray< mmlVector<2> > &poly);

// @algo RayCollide
// @info Checks for collision between Ray and line segment.
// @in
//   r -> Ray.
//   a, b -> Line segment.
// @out The collision information
UnaryCollisionInfo RayCollide(Ray r, mmlVector<2> a, mmlVector<2> b);

// @algo ConeCollide
// @info Checks for collision between Cone and point.
// @in
//   r -> Cone.
//   a -> Point.
// @out The collision information.
UnaryCollisionInfo ConeCollide(Cone r, mmlVector<2> a);

// @algo ConeCollide
// @info Checks for collision between Cone and line segment.
// @in
//   r -> Cone.
//   a, b -> Line segment.
// @out The collision info.
UnaryCollisionInfo ConeCollide(Cone r, mmlVector<2> a, mmlVector<2> b);

// @algo PlaneCollide
// @info Returns a positive collision if point is behind plane.
// @in
//   p -> Plane.
//   a -> Point.
// @out The collision info.
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a);

// @algo PlaneCollide
// @info Checks for collision between plane and line segment.
// @in
//   p -> Plane.
//   a, b -> Line segment.
// @out The collision info.
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a, mmlVector<2> b);

// @data GridWalker
// @info A data structure used to performe DDA (digital differential analysis) to step through a grid in variable amounts in the direction of a given ray. Can be used to implement a Wolfenstein 3D-like raycasting engine.
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

	// @algo SetInitialState
	// @info Sets the initial state of the data structure by providing ray starting point.
	// @in p_ray -> The initial ray starting point.
	void                SetInitialState(const Ray &p_ray);

	// @algo Step
	// @info Moves one step (one cell in X or Y axis).
	void                Step( void );
	
	// @algo GetX
	// @out The current X position.
	int                 GetX( void ) const { return m_xy[0]; }
	
	// @algo GetY
	// @out The current Y position.
	int                 GetY( void ) const { return m_xy[1]; }

	// @algo GetImpactAxis
	// @out The number corresponding to the axis that was changes last when stepping through the grid.
	int                 GetImpactAxis( void ) const { return m_side; }
	// @algo GetImpactDistance
	// @out The distance from the original position to the current impact point.
	float               GetImpactDistance( void ) const { return fabs((m_xy[m_side] - m_origin[m_side] + (1 - m_step[m_side]) / 2) / m_direction[m_side]); }
	
	// @algo GetImpactPosition
	// @out The current impact point.
	mmlVector<2>        GetImpactPosition( void ) const { return m_origin + m_direction * GetImpactDistance(); }
	
	// @algo GetImpactUV
	// @out The impact point in normalized units (0-1), i.e. the fractional remainder of the impact point.
	mmlVector<2>        GetImpactUV( void ) const;

	// @algo GetOrigin
	// @out The origin.
	const mmlVector<2> &GetOrigin( void ) const { return m_origin; }
};

class Collider;
class PolygonCollider;

// @data CollisionInfo
// @info Contains collision information between two complex collision shapes.
struct CollisionInfo
{
	const Collider             *A;
	const Collider             *B;
	ShallowArray(mmlVector<2>)  points;
	mmlVector<2>                avg_collision;
	mmlVector<2>                A_normal;
	mmlVector<2>                B_normal;
	bool                        collision;
};

// @data ContactPoint
// @info Contains information relating to a single contact point.
struct ContactPoint
{
	mmlVector<2> point;
	float        depth;
};

// @data Contact
// @info Contains information relating to a single stable contact (2 points in 2D space).
struct Contact
{
	ContactPoint points[2];
	mmlVector<2> normal;
	int          point_count;
};

// @data BoundRect
// @info Represents an axis-aligned bounding box.
struct BoundRect
{
	// @data a, b
	// @info Represents the minimum and maximum point defining an axis-aligned bounding box.
	mmlVector<2> a, b;

	// @algo GetArea
	// @out The area of the box.
	float GetArea( void ) const
	{
		return mmlMax(b[0] - a[0], 0.0f) * mmlMax(b[1] - a[1], 0.0f);
	}
	
	// @algo GetOverlap
	// @info Checks for an overlapping rectangle between two rectangles.
	// @in
	//   r1, r2 -> The input rectangles to perform test on.
	// @out The overlapping rectangle. Area = 0 if there is no overlap.
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

// @data Collider
// @info A base class representing a complex collision shape.
// @note Needs to be extended to work for a given collision shape.
class Collider : public mtlBase
{
	friend class PolygonCollider;

protected:
	mtlShared<Transform> m_transform;
	//Transform    *m_transform;
	//Transform     m_prevTransform;

protected:
	//virtual CollisionInfo CollidesWith(const Collider&) const        { CollisionInfo c; c.A = c.B = NULL; c.collision = false; return c; }
	//virtual CollisionInfo CollidesWith(const PolygonCollider&) const { CollisionInfo c; c.A = c.B = NULL; c.collision = false; return c; }
	virtual Contact CollidesWith(const Collider&)        const { Contact c; c.point_count = 0; return c; }
	virtual Contact CollidesWith(const PolygonCollider&) const { Contact c; c.point_count = 0; return c; }

public:
			 Collider( void );
	virtual ~Collider( void ) {}

	// @algo GetTransform
	// @out The transform of the collider.
	Transform                  &GetTransform( void );
	const Transform            &GetTransform( void ) const;
	
	// @algo SetTransform
	// @info Used to alter the current transform. Can be used to store a separate copy of a transform or reference an existing one (in a safe manner).
	// @in transform -> A shared pointer to a transform.
//	void                        SetTransform(Transform *transform);
	void                        SetTransform(mtlShared<Transform> &transform);

	// @algo Load
	// @info Used to load a collider from a file (virtual).
	// @in file -> The path of the file storing the collider data.
	// @out The result of the file operation (TRUE on successful read).
	virtual bool                Load(const mtlPath &file)               { return false; }

	// @algo SetHalfExtents
	// @info Sets the half extenst of the collision shape (virtual).
	// @in w, h -> The dimensions of the half extents.
	virtual void                SetHalfExtents(float w, float h)        { SetHalfExtents(mmlVector<2>(w, h)); }
	virtual void                SetHalfExtents(const mmlVector<2>&)     {} // exactly what this does is left undefined, hopefully something reasonable
	
	// @algo GetHalfExtents
	// @out The collision shape's half extents.
	virtual mmlVector<2>        GetHalfExtents( void ) const            { return mmlVector<2>(0.0f, 0.0f); }
	
	// @algo GetBoundRect
	// @out The bounding rectangle that contains the collision shape.
	virtual BoundRect           GetBoundRect( void ) const              { BoundRect r; r.a = mmlVector<2>(0.0f, 0.0f); r.b = mmlVector<2>(0.0f, 0.0f); return r; }
	//virtual void                ResetState( void )                      {}
	
	// @algo UpdateWorldState
	// @info Updates the collision shape to correspond to related global changes (virtual).
	virtual void                UpdateWorldState( void )                {}
	//void                        TrackPreviousTransform( void );

	// @algo Collides
	// @info Checks for collision between collision shape and a ray (virtual).
	// @in ray -> The ray.
	// @out The collision info.
	virtual UnaryCollisionInfo  Collides(Ray) const                     { UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	
	// @algo Collides
	// @info Checks for collision between collision shape and cone (virtual).
	// @in cone -> The cone.
	// @out The collision info.
	virtual UnaryCollisionInfo  Collides(Cone) const                    { UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	
	// @algo Collides
	// @info Checks for collision between collision shape and plane (virtual).
	// @in plane -> The plane.
	// @out The collision info.
	virtual UnaryCollisionInfo  Collides(Plane)	const                   { UnaryCollisionInfo c; c.collider = NULL; c.collision = false; return c; }
	//virtual CollisionInfo       Collides(const Collider&) const         { CollisionInfo c; c.A = c.B = NULL; c.collision = false; return c; }
	
	// @algo Collision
	// @info Checks for collision between two complex collision shapes (virtual).
	// @in The complex collision shape.
	// @out The contact info.
	virtual Contact             Collides(const Collider&) const         { Contact c; c.point_count = 0; return c; }

	// @algo GetCircumference
	// @out The circumference of the collision shape at the greatest radius (virtual).
	virtual float               GetCircumference( void ) const          { return 0.0f; }
	
	// @algo GetArea
	// @out The area of the collision shape (virtual).
	virtual float               GetArea( void ) const                   { return 0.0f; }

	//void                        RevertTransform( void ) const           { if (m_transform != NULL) { *m_transform = m_prevTransform; } }
};

typedef Collider NullCollider;

// @data PolygonCollider
// @info Describes a polygonal complex collision shape.
class PolygonCollider : public mtlInherit<Collider, PolygonCollider>
{
private:
	mtlArray< mmlVector<2> > m_vert;       // original data
	mtlArray< mmlVector<2> > m_globalVert; // rotated data
	//mtlArray< bool >         m_collide;

public:
	// @data Shape
	// @info Constants representing pre-defined collision shapes.
	enum Shape
	{
		Triangle,
		Rectangle,
		Hexagon,
		Rhombus
	};

protected:
	//CollisionInfo CollidesWith(const Collider&) const { CollisionInfo c; c.A = c.B = NULL; c.collision = false; return c; }
	//CollisionInfo CollidesWith(const PolygonCollider &c) const;
	Contact CollidesWith(const Collider&)          const { Contact c; c.point_count = 0; return c; }
	Contact CollidesWith(const PolygonCollider &c) const;

public:
									PolygonCollider( void );
	explicit                        PolygonCollider(Shape shape);
	explicit                        PolygonCollider(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center);

	// @algo CreateShape
	// @info Creates a predefined polygonal shape.
	// @in The constant corresponding to the shape.
	void                            CreateShape(Shape shape);

	// @algo Load
	// @info Used to load a collider from a simplified 2D version of an OBJ file.
	bool                            Load(const mtlPath &file);

	// @algo GetVertexCount
	// @out The number of vertices in the shape.
	int                             GetVertexCount( void ) const;
	
	// @algo GetVertex
	// @info Returns the requested vertex in object space.
	// @in i -> The index of the requested vertex.
	// @out The requested vertex in object space.
	const mmlVector<2>             &GetVertex(int i) const;
	mmlVector<2>                   &GetVertex(int i);
	
	// @algo GetVertexArray
	// @out The full vertex array in object space.
	const mtlArray< mmlVector<2> > &GetVertexArray( void ) const;
	mtlArray< mmlVector<2> >       &GetVertexArray( void );

	// @algo GetWorldVertex
	// @info Returns the requested vertex in world space.
	// @in i -> The index of the requested vertex.
	// @out The requested vertex in world space.
	const mmlVector<2>             &GetWorldVertex(int i) const;
	mmlVector<2>                   &GetWorldVertex(int i);
	
	// @algo GetWorldVertexArray
	// @out The full vertex array in world space.
	const mtlArray< mmlVector<2> > &GetWorldVertexArray( void ) const;
	mtlArray< mmlVector<2> >       &GetWorldVertexArray( void );

	// @algo CopyVertexArray
	// @info Sets a new vertex array, thereby creating a new collision shape.
	// @in
	//   vert -> The new vertex array.
	//   center -> The center of the collision shape.
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
	//CollisionInfo                   Collides(const Collider &c) const;
	Contact                         Collides(const Collider &c) const;

	// @algo IsConvex
	// @out The convexity of the polygonal collision shape (TRUE if convex).
	bool                            IsConvex( void ) const;
};

#endif // COLLIDER_H
