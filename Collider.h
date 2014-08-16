#ifndef COLLIDER_H
#define COLLIDER_H

#include "Common.h"
#include "Sprite.h"
#include "MTL/mtlType.h"

//http://xbox.create.msdn.com/en-US/education/catalog/tutorial/collision_2d_perpixel_transformed

// NOTES
// All shapes are centered around 0,0
// Might want to do lazy evaluation since otherwise I will recalculate world space a lot

class Collider;
class BoxCollider; // a box that is always axis aligned even after world transform (means that non-square boxes change shape when transformed)
class CircleCollider;
//class PolygonCollider; // arbitrarily shaped polygon
class ColliderTreeQuad;

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

struct CollisionInfo
{
	Collider					*collider1;
	Collider					*collider2;
	DeepArray< mmlVector<2> >	contactPoints; // intrusion points and intersections
	bool						collision;
};

struct UnaryCollisionInfo
{
	Collider					*collider;
	DeepArray< mmlVector<2> >	contactPoints; // intersections
	//DeepArray<Ray>				reflection;
	bool						collision;
};

UnaryCollisionInfo RayCollide(Ray r, mmlVector<2> a, mmlVector<2> b);
UnaryCollisionInfo RangeCollide(Range r, mmlVector<2> a);
UnaryCollisionInfo RangeCollide(Range r, mmlVector<2> a, mmlVector<2> b);
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a);
UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a, mmlVector<2> b);

class ColliderTree
{
	friend class ColliderTreeQuad;

private:
	ColliderTree		*m_root; // is never ever NULL
	ColliderTree		*m_parent;
	ColliderTreeQuad	*m_children; // stop adding children when a quad only contains one object
	mtlList<Collider*>	m_colliders; // all of the colliders contained in this tree
	int					m_depth; // used as bit shift to partition space (starts at 0, max 31, but optimum is probably 3-5 depending on size)
	int					m_subquad;

	// all of these values are actually stored in m_root
	int					m_maxDepth; // calculated as: maxDepth - level
	int					m_spaceWidth;
	int					m_spaceHeight;

private:
	ColliderTree( void );
	ColliderTree(ColliderTree *parent, int subquad);
	ColliderTree(const ColliderTree&) {}
	ColliderTree &operator=(const ColliderTree&) { return *this; }

public:
	ColliderTree(int width, int height, int maxDepth);
	~ColliderTree( void ); // recursive destruction

	ColliderTree	*GetChild(int i);
	ColliderTree	*GetChild(int x, int y);
	bool			IsLastLevel( void ) const { return m_children == NULL; }

	mtlNode<Collider*>	*GetCollider( void ) { return m_colliders.GetFirst(); }
	int					GetColliderCount( void ) const { return m_colliders.GetSize(); }
	bool				IsEmpty( void ) const { return m_colliders.GetSize() == 0; }

	ColliderTree	*GetRoot( void ) const { return m_root; }
	ColliderTree	*GetParent( void ) const { return m_parent; }
	bool			IsRoot( void ) const { return this == m_root; }

	int		GetCurrentDepth( void ) const { return m_depth; }
	int		GetSubquad( void ) const { return m_subquad; }
	int		GetSubquadX( void ) const { return m_subquad & 1; }
	int		GetSubquadY( void ) const { return (m_subquad & 2) >> 1; }
	int		GetSpaceWidth( void ) const { return m_root->m_spaceWidth >> m_depth; }
	int		GetSpaceHeight( void ) const { return m_root->m_spaceHeight >> m_depth; }
	int		GetMaxDepth( void ) const { return m_root->m_maxDepth; }

	mtlNode<Collider*>	*AddCollider(Collider *collider);
	void				RemoveCollider(Collider *collider);
	void				RemoveCollider(mtlNode<Collider*> *collider);
	mtlNode<Collider*>	*FindCollider(Collider *collider);

	void	Destroy( void );
	void	ResizeTree(int width, int height, int maxDepth); // sets values, refreshes tree and all contents
	void	RefreshTree( void ); // recursively calls refresh tree, updates contents of m_colliders based on collider shape, width & height of tree, and max depth (also destroys all sub trees greater than max depth)

	// returns the deepest node with potential collisions
	// calculates the exit ray (returns that in ray)
	// returns NULL if there is no risk for collision at all along the ray
	// iteratively use TraceRay and check for collisions until collision is found, or until return value is NULL
	ColliderTree *TraceRay(Ray &ray);
};

struct ColliderTreeQuad
{
	ColliderTree q[4];
	// [0|1]
	// [2|3]

	explicit ColliderTreeQuad(ColliderTree *parent);
};

class CollisionSolver
{
private:
	ColliderTree m_tree;
	// stores spatial data in acceleration structure
	// checks for collisions and prevents colliders from intersecting
	// stores collision data in list
	// iterates over collision list and applies transforms to collision pairs to model physical behavior
	// feedback from physics solver is fed into collision solver during next update
public:
	CollisionSolver(int width, int height, int maxDepth);
};

class Collider : public mtlBase
{
	friend class CollisionSolver;
	friend class BoxCollider;
	friend class CircleCollider;
	//friend class PolygonCollider;

protected:
	Transform			m_transform; // position will be center of mass
	Transform			m_prevTransform;
	mmlVector<2>		m_velocity; // rate of change in position
	mmlVector<2>		m_acceleration; // rate of change in velocity
	float				m_angularMomentum; // rate of change in rotation???
	float				m_torque; // rate of change in angular momentum
	float				m_mass;
	float				m_friction; // 0 - 1
	float				m_bounce; // 0 - 1 (an object with 1 exits collision with same speed as entering)
	bool				m_isResting;
	bool				m_hasRigidBody; // when 'false' then solver only resolves collisions
	CollisionSolver		*m_collisionSolver;

protected:
	virtual bool CollidesWith(const BoxCollider&) const { return false; }
	virtual bool CollidesWith(const CircleCollider&) const { return false; }
	//virtual bool CollidesWith(const PolygonCollider&) const { return false; }

	float GetInverseMass( void ) const;

public:
	Collider( void );
	virtual ~Collider( void ) {} // remove item from collision solver, collision solver removes item from quad tree

	const Transform &GetTransform( void ) const;
	Transform &GetTransform( void );

	// do not set these directly (use Newtonian interface)
	const mmlVector<2> &GetVelocity( void ) const;
	const mmlVector<2> &GetAcceleration( void ) const;
	float GetAngularMomentum( void ) const;
	float GetTorque( void ) const;

	float GetMass( void ) const;
	void SetMass(float mass);
	void SetImmovable( void );
	bool IsImmovable( void );

	float GetFriction( void ) const;
	void SetFriction(float friction);

	bool IsResting( void ) const;

	bool HasRigidBody( void ) const;
	void EnableRigidBody( void );
	void DisableRigidBody( void );

	// various ApplyForce functions

	virtual bool Collides(const Collider&) const { return false; }
	virtual bool Collides(const Ray&) const { return false; }
	virtual bool Collides(const Range&) const { return false; }
	virtual bool Collides(const Plane&) const { return false; }

	virtual mmlVector<2> GetHalfExtents( void ) const { return mmlVector<2>(0.0f, 0.0f); }
	virtual void SetHalfExtents(const mmlVector<2> &halfExtents) {}
	void SetHalfExtents(float halfW, float halfH) { SetHalfExtents(mmlVector<2>(halfW, halfH)); }
};

typedef Collider NullCollider;

class BoxCollider : public mtlInherit<Collider> // Axis-aligned box
{
private:
	mmlVector<2> m_dimensions;

protected:
	bool CollidesWith(const BoxCollider &b) const;
	bool CollidesWith(const CircleCollider &c) const;
	//bool CollidesWith(const PolygonCollider &p) const;

public:
	BoxCollider( void );
	explicit BoxCollider(const mmlVector<2> &dimensions);
	BoxCollider(float width, float height);

	const mmlVector<2> &GetDimensions( void ) const;
	void SetDimensions(const mmlVector<2> &dimensions);
	void SetDimensions(float width, float height);

	float GetWidth( void ) const;
	float GetHeight( void ) const;
	void SetWidth(float width);
	void SetHeight(float height);

	bool Collides(const Collider &collider) const;
	bool Collides(const Ray &ray) const;
	bool Collides(const Range &range) const;
	bool Collides(const Plane &plane) const;

	mmlVector<2> GetHalfExtents( void ) const;
	void SetHalfExtents(const mmlVector<2> &halfExtents);
};

class CircleCollider : public mtlInherit<Collider>
{
private:
	float m_radius;

protected:
	bool CollidesWith(const BoxCollider &b) const;
	bool CollidesWith(const CircleCollider &c) const;
	//bool CollidesWith(const PolygonCollider &p) const;

public:
	CircleCollider( void );
	explicit CircleCollider(float radius);

	float GetRadius( void ) const;
	void SetRadiuds(float radius);

	bool Collides(const Collider &collider) const;
	//bool Collides(const Ray&) const;
	//bool Collides(const Range&) const;
	//bool Collides(const Plane&) const;

	mmlVector<2> GetHalfExtents( void ) const;
	void SetHalfExtents(const mmlVector<2> &halfExtents);
};

/*class PolygonCollider : public mtlInherit<Collider> // Free-form collider
{
private:
	mtlArray< mmlVector<2> > m_polygon; // circles around [n-1, 0]

protected:
	bool CollidesWith(const BoxCollider &b) const;
	bool CollidesWith(const CircleCollider &c) const;
	bool CollidesWith(const PolygonCollider &p) const;

public:
	bool Collides(const Collider&) const;
	bool Collides(const Ray&) const;
	bool Collides(const Range&) const;
	bool Collides(const Plane&) const;
};*/

#endif // COLLIDER_H
