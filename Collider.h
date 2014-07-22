#ifndef COLLIDER_H
#define COLLIDER_H

#include "Common.h"
#include "Sprite.h"
#include "MTL/mtlType.h"

//http://xbox.create.msdn.com/en-US/education/catalog/tutorial/collision_2d_perpixel_transformed

// NOTES
// All shapes are centered around 0,0
// Transformed by m_transform
// Might want to do lazy evaluation since otherwise I will recalculate world space a lot
// Collider transform will probably need to be object transform (i.e. remove object transform, replace by collider transform)

class CollisionInfo
{
private:
	Collider				*m_collider1;
	Collider				*m_collider2;
	mtlList< mmlVector<2> > m_contact;
	bool					m_collision;
};

class Collider;

class ColliderQuadTree
{
private:
	struct ChildNode
	{
		ColliderQuadTree q[4];
		// [0|1]
		// [2|3]
	};

private:
	ColliderQuadTree	*m_root; // is never ever NULL
	ColliderQuadTree	*m_parent;
	ChildNode			*m_children; // stop adding children when a quad only contains one object
	mtlList<Collider*>	m_colliders; // all of the colliders contained in this tree
	int					m_depth; // used as bit shift to partition space (starts at 0, max 31)
	int					m_subquad;
	// all of these values are actually stored in m_root
	int					m_maxDepth; // calculated as: maxDepth - level
	int					m_spaceWidth;
	int					m_spaceHeight;

private:
	ColliderQuadTree(ColliderQuadTree *parent, int subquad);

public:
	ColliderQuadTree(int width, int height, int maxDepth);
	~ColliderQuadTree( void ); // recursive destruction

	ColliderQuadTree	*GetChild(int i) { return !IsLastLevel() ? ((i>-1&&i<4) ? m_children->q[i] : NULL) : NULL; }
	bool				IsLastLevel( void ) const { return m_children == NULL; }

	mtlNode<Collider*>	*GetCollider( void ) { return m_colliders.GetFirst(); }
	int					GetColliderCount( void ) const { return m_colliders.GetSize(); }

	ColliderQuadTree	*GetRoot( void ) const { return m_root; }
	ColliderQuadTree	*GetParent( void ) const { return m_parent; }
	bool				IsRoot( void ) const { return this == m_root; }

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
	ColliderQuadTree *TraceRay(Ray &ray);
};

class ColliderCollisionSolver
{
private:
	ColliderQuadTree m_tree;
	// stores spatial data in acceleration structure
	// checks for collisions and prevents colliders from intersecting
	// stores collision data in list
	// iterates over collision list and applies transforms to collision pairs to model physical behavior
	// feedback from physics solver is fed into collision solver during next update
};

class BoxCollider; // a box that is always axis aligned even after world transform
class CircleCollider;
/*class PlaneCollider; // detects collisions for shapes past the plane (not just intersecting)
class PolygonCollider; // arbitrarily shaped polygon
class RayCollider; // a ray with origin and distance (neg distance is infinite)
class RangeCollider; // 2d cone, with origin and distance (neg distance is infinite)
*/

class Collider : public mtlBase
{
	friend class ColliderCollisionSolver;

protected:
	Transform				m_transform; // position will be center of mass
	mmlVector<2>			m_momentum;
	float					m_angularMomentum;
	float					m_mass;
	float					m_friction;
	bool					m_isResting;
	bool					m_hasRigidBody; // when 'false' then solver only resolves collisions
	ColliderQuadTree		*m_quadTree;
	ColliderCollisionSolver	*m_collisionSolver;

protected:
	virtual bool CollidesWith(const BoxCollider&) const = 0;
	virtual bool CollidesWith(const CircleCollider&) const = 0;
	/*virtual bool CollidesWith(const RayCollider&) const = 0;
	virtual bool CollidesWith(const RangeCollider&) const = 0;
	virtual bool CollidesWith(const PlaneCollider&) const = 0;
	virtual bool CollidesWith(const PolygonCollider&) const = 0;*/

public:
	Collider( void );
	virtual ~Collider( void ) {} // remove item from collision solver, collision solver removes item from quad tree

	virtual bool Collides(const Collider&) const = 0;
	virtual Box GetBoundingBox( void ) const = 0;
	virtual Circle GetBoundingCircle( void ) const = 0;

	const Transform &GetTransform( void ) const;
	Transform &GetTransform( void );

	// do not set these directly (use Newtonian interface)
	const mmlVector<2> &GetMomentum( void ) const;
	float GetAngularMomentum( void ) const;

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
};

class BoxCollider : public mtlInherit<Collider>
{
private:
	mmlVector<2> m_dimensions;

private:
	struct BoxPoints { mmlVector<2> p[4]; };
	BoxPoints GetBoxPoints( void ) const;

protected:
	virtual bool CollidesWith(const BoxCollider &b) const;
	virtual bool CollidesWith(const CircleCollider &c) const;
	/*virtual bool CollidesWith(const RayCollider &r) const;
	virtual bool CollidesWith(const RangeCollider &r) const;
	virtual bool CollidesWith(const PlaneCollider &p) const;
	virtual bool CollidesWith(const PolygonCollider &p) const;*/

public:
	BoxCollider( void );
	explicit BoxCollider(const mmlVector<2> &dimensions);
	BoxCollider(float width, float height);
	BoxCollider(const BoxCollider &collider);
	BoxCollider &operator=(const BoxCollider &collider);

	const mmlVector<2> &GetDimensions( void ) const;
	void SetDimensions(const mmlVector<2> &dimensions);
	void SetDimensions(float width, float height);

	float GetWidth( void ) const;
	float GetHeight( void ) const;
	void SetWidth(float width);
	void SetHeight(float height);

	mmlVector<2> GetMinimumExtents( void ) const;
	mmlVector<2> GetMaximumExtents( void ) const;

	bool Collides(const Collider &collider);
	Box GetBoundingBox( void ) const;
	Circle GetBoundingCircle( void ) const;
};

class CircleCollider : public mtlInherit<Collider>
{
private:
	float m_radius;

protected:
	virtual bool CollidesWith(const BoxCollider &b) const;
	virtual bool CollidesWith(const CircleCollider &c) const;
	/*virtual bool CollidesWith(const RayCollider &r) const;
	virtual bool CollidesWith(const RangeCollider &r) const;
	virtual bool CollidesWith(const PlaneCollider &p) const;
	virtual bool CollidesWith(const PolygonCollider &p) const;*/
public:
	CircleCollider( void );
	explicit CircleCollider(float radius);
	CircleCollider(const Circle &circle);
	CircleCollider(const CircleCollider &collider);
	CircleCollider &operator=(const CircleCollider &collider);

	float GetRadius( void ) const;
	void SetRadiuds(float radius);

	bool Collides(const Collider &collider) const;
	Box GetBoundingBox( void ) const;
	Circle GetBoundingCircle( void ) const;
};

/*class RayCollider : public mtlInherit<Collider>
{
private:
	mmlVector<2>	m_direction;
	float			m_range;

public:
	float GetRange( void ) const { return m_range; }
	void SetRange(float range) { m_range = mmlMax2(0.0f, range); }
	void SetInfiniteRange( void ) { m_range = -1.0f; }
	bool IsInfiniteRange( void ) const { return m_range < 0.0f; }
};*/

/*class RangeCollider : public mtlInherit<RayCollider>
{
private:
	float m_apex;
};*/

/*class PlaneCollider : public mtlInherit<Collider>
{
private:
	// use transform's position
	mmlVector<2> m_normal;
};*/

/*class PolygonCollider : public mtlInherit<Collider>
{
private:
	mtlArray< mmlVector<2> > m_polygon; // circles around [n-1, 0]
};*/

#endif // COLLIDER_H
