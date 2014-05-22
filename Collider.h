#ifndef COLLIDER_H
#define COLLIDER_H

#include "Common.h"
#include "Sprite.h"
#include "MTL/mtlType.h"

class Collider : public mtlBase
{
protected:
	Transform m_transform;
public:
	virtual ~Collider( void ) {}
	virtual bool Collides(const Collider &collider) = 0;
	virtual bool CollidesRay(mmlVector<2> origin, mmlVector<2> direction) = 0;
	//virtual bool CollidesRay(mmlVector<2> origin, mmlVector<2> direction, float range) = 0;
	virtual bool CollidesCone(mmlVector<2> origin, mmlVector<2> direction, float apex) = 0;
	//virtual bool CollidesCone(mmlVector<2> origin, mmlVector<2> direction, float apex, float range) = 0;
	virtual bool CollidesPlane(mmlVector<2> point, mmlVector<2> normal) = 0;
	Transform &GetTransform( void );
	const Transform &GetTransform( void ) const;
};

class PointCollider : public mtlInherit<Collider>
{
private:
	mmlVector<2> m_point;
public:
	PointCollider( void );
	PointCollider(float x, float y);
	explicit PointCollider(mmlVector<2> point);
	PointCollider(const PointCollider &collider);
	PointCollider &operator=(const PointCollider &collider);

	mmlVector<2> GetPoint( void ) const;
	void SetPoint(float x, float y);
	void SetPoint(mmlVector<2> point);

	float GetX( void ) const;
	float GetY( void ) const;
	void SetX(float x);
	void SetY(float y);

	bool Collides(const Collider &collider);
	bool CollidesRay(mmlVector<2> origin, mmlVector<2> direction);
	bool CollidesCone(mmlVector<2> origin, mmlVector<2> direction, float apex);
	bool CollidesPlane(mmlVector<2> point, mmlVector<2> normal);
};

class AABBCollider : public mtlInherit<Collider>
{
private:
	struct Box {
		mmlVector<2> p[4];
	};
private:
	mmlVector<2> m_min;
	mmlVector<2> m_max;
private:
	Box GetBoxPoints( void ) const;
public:
	AABBCollider( void );
	AABBCollider(float minx, float miny, float maxx, float maxy);
	AABBCollider(float minx, float miny, mmlVector<2> max);
	AABBCollider(mmlVector<2> min, mmlVector<2> max);
	AABBCollider(mmlVector<2> min, float maxx, float maxy);
	AABBCollider(const AABBCollider &collider);
	AABBCollider &operator=(const AABBCollider &collider);

	mmlVector<2> GetMinimumBounds( void ) const;
	mmlVector<2> GetMaximumBounds( void ) const;
	void SetMinimumBounds(float x, float y);
	void SetMinimumBounds(mmlVector<2> min);
	void SetMaximumBounds(float x, float y);
	void SetMaximumBounds(mmlVector<2> max);

	float GetMinimumX( void ) const;
	float GetMinimumY( void ) const;
	float GetMaximumX( void ) const;
	float GetMaximumY( void ) const;
	void SetMinimumX(float x);
	void SetMinimumY(float y);
	void SetMaximumX(float x);
	void SetMaximumY(float y);

	bool Collides(const Collider &collider);
	bool CollidesRay(mmlVector<2> origin, mmlVector<2> direction);
	bool CollidesCone(mmlVector<2> origin, mmlVector<2> direction, float apex);
	bool CollidesPlane(mmlVector<2> point, mmlVector<2> normal);
};

/*class CircleCollider : public mtlInherit<Collider>
{
private:
	mmlVector<2> m_center;
	float m_radius;
};

class PolygonCollider : public mtlInherit<Collider>
{
private:
	mtlArray< mmlVector<2> > m_polygon; // circles around [n-1, 0]
};

class PlaneCollider : public mtlInherit<Collider>
{
private:
	mmlVector<2> m_point;
	mmlVector<2> m_normal;
};

http://xbox.create.msdn.com/en-US/education/catalog/tutorial/collision_2d_perpixel_transformed
class SpriteCollider : public mtlInherit<Collider>
{
private:
	mtlAsset<Sprite>	m_pixels;
	Collider			*m_culler; // used to quickly cull collisions before checking pixel overlap
public:
	~SpriteCollider( void ) { delete m_culler; }
};
*/

#endif // COLLIDER_H
