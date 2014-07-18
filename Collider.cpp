#include "Collider.h"

#define RoundErr 0.001f

bool PointPlane(mmlVector<2> a, mmlVector<2> bpoint, mmlVector<2> bnormal)
{
	mmlVector<2> p = bpoint - a;
	float dot = mmlDot(bnormal, p);
	return dot <= 0.0f;
}

bool PointBox(mmlVector<2> a, mmlVector<2> bmin, mmlVector<2> bmax)
{
	return a[0] >= bmin[0] && a[0] < bmax[0] && a[1] >= bmin[1] && a[1] < bmax[1];
}

bool PointCircle(mmlVector<2> a, mmlVector<2> b, float bradius)
{
	return mmlDist(a, b) < bradius;
}

bool BoxBox(mmlVector<2> amin, mmlVector<2> amax, mmlVector<2> bmin, mmlVector<2> bmax)
{
	mmlVector<2> min;
	mmlVector<2> max;
	min[0] = mmlMax2(amin[0], bmin[0]);
	min[1] = mmlMax2(amin[1], bmin[1]);
	max[0] = mmlMin2(amax[0], bmax[0]);
	max[1] = mmlMin2(amax[1], bmax[1]);
	return min[0] <= max[0] && min[1] <= max[1];
}

bool BoxCircle(mmlVector<2> amin, mmlVector<2> amax, mmlVector<2> bcenter, float bradius)
{
	return
		PointBox(bcenter, amin, amax);
		// check all line segments of box for intersection with circle
}

bool CircleCircle(mmlVector<2> acenter, float aradius, mmlVector<2> bcenter, float bradius)
{
	const float radius = aradius + bradius;
	const float dist = mmlDist(acenter, bcenter);
	return dist < radius;
}

bool RayLine(mmlVector<2> r1, mmlVector<2> r2, mmlVector<2> b1, mmlVector<2> b2)
{
	float rCross = Cross2D(r1, r2);
	float lCross = Cross2D(b1, b2);
	float div = 1.0f / Cross2D(r1 - r2, b1 - b2);

	mmlVector<2> point = ((b1 - b2) * rCross - (r1 - r2) * lCross) * div;

	mmlVector<2> ln1 = mmlNormalizeFast(b2 - b1);
	mmlVector<2> ln2 = -ln1;
	mmlVector<2> pn1 = mmlNormalizeFast(b1 - point);
	mmlVector<2> pn2 = mmlNormalizeFast(b2 - point);

	float d1 = mmlDot(ln1, pn1);
	float d2 = mmlDot(ln2, pn2);

	return (d1 >= 1.0f - RoundErr && d1 <= 1.0f + RoundErr && d2 >= 1.0f - RoundErr && d2 <= 1.0f + RoundErr);
}

bool LineLine(mmlVector<2> a1, mmlVector<2> a2, mmlVector<2> b1, mmlVector<2> b2)
{
	float rCross = Cross2D(a1, a2);
	float lCross = Cross2D(b1, b2);
	float div = 1.0f / Cross2D(a1 - a2, b1 - b2);

	mmlVector<2> point = ((b1 - b2) * rCross - (a1 - a2) * lCross) * div;

	mmlVector<2> ln1 = mmlNormalizeFast(b2 - b1);
	mmlVector<2> ln2 = -ln1;
	mmlVector<2> pln1 = mmlNormalizeFast(b1 - point);
	mmlVector<2> pln2 = mmlNormalizeFast(b2 - point);

	mmlVector<2> rn1 = mmlNormalizeFast(a2 - a1);
	mmlVector<2> rn2 = -rn2;
	mmlVector<2> prn1 = mmlNormalizeFast(a1 - point);
	mmlVector<2> prn2 = mmlNormalizeFast(a2 - point);

	float dl1 = mmlDot(ln1, pln1);
	float dl2 = mmlDot(ln2, pln2);

	float dr1 = mmlDot(rn1, prn1);
	float dr2 = mmlDot(rn2, prn2);

	return
		(dl1 >= 1.0f - RoundErr && dl1 <= 1.0f + RoundErr && dl2 >= 1.0f - RoundErr && dl2 <= 1.0f + RoundErr) &&
		(dr1 >= 1.0f - RoundErr && dr1 <= 1.0f + RoundErr && dr2 >= 1.0f - RoundErr && dr2 <= 1.0f - RoundErr);
}

Collider::Collider( void ) : m_transform(), m_momentum(0.0f, 0.0f, 0.0f), m_angularMomentum(0.0f), m_mass(-1.0f), m_friction(0.5f), m_isResting(false), m_hasRigidBody(false)
{}

const Transform &Collider::GetTransform( void ) const
{
	return m_transform;
}

const mmlVector<2> Collider::GetMomentum( void ) const
{
	return m_momentum;
}

float Collider::GetAngularMomentum( void ) const
{
	return m_angularMomentum;
}

float Collider::GetMass( void ) const
{
	return m_mass;
}

void Collider::SetMass(float mass)
{
	m_mass = mmlMax2(0.0f, mass);
}

void Collider::SetImmovable( void )
{
	m_mass = -1.0f;
}

bool Collider::IsImmovable( void )
{
	return m_mass < 0.0f;
}

float Collider::GetFriction( void ) const
{
	return m_friction;
}

void Collider::SetFriction(float friction)
{
	m_friction = mmlClamp(0.0f, friction, 1.0f);
}

bool Collider::IsResting( void ) const
{
	return m_isResting;
}

bool Collider::HasRigidBody( void ) const
{
	return m_hasRigidBody;
}

void Collider::EnableRigidBody( void )
{
	m_hasRigidBody = true;
}

void Collider::DisableRigidBody( void )
{
	m_hasRigidBody = false;
}

/*bool PointCollider::CollidesRay(mmlVector<2> origin, mmlVector<2> direction)
{
	if (!direction.IsNormalized()) { direction.NormalizeFast(); }
	mmlVector<2> p = m_point - origin;
	int px = (int)floor(p[0]);
	int py = (int)floor(p[1]);
	float len = p.Len();
	mmlVector<2> l = direction * len;
	int lx = (int)floor(l[0]);
	int ly = (int)floor(l[1]);
	return px == lx && py == ly;
}

bool PointCollider::CollidesCone(mmlVector<2> origin, mmlVector<2> direction, float apex)
{
	if (!direction.IsNormalized()) { direction.NormalizeFast(); }
	mmlVector<2> p = mmlNormalize(m_point - origin);
	float dot = mmlDot(direction, p);
	return dot >= 1.0f - apex && dot <= 1.0f + apex;
}

bool PointCollider::CollidesPlane(mmlVector<2> point, mmlVector<2> normal)
{
	if (!normal.IsNormalized()) { normal.NormalizeFast(); }
	return PointPlane(m_point, point, normal);
}*/

BoxCollider::BoxPoints BoxCollider::GetBoxPoints( void ) const
{
	BoxPoints b = { { m_box.min, mmlVector<2>(m_box.max[0], m_box.min[1]), m_box.max, mmlVector<2>(m_box.min[0], m_box.max[1]) } };
	return b;
}

bool BoxCollider::CollidesWith(const BoxCollider &b) const
{
	const mmlVector<2> amin = m_transform.TransformPoint(GetMinimumExtents());
	const mmlVector<2> amax = m_transform.TransformPoint(GetMaximumExtents());
	const mmlVector<2> bmin = b.GetTransform().TransformPoint(b.GetMinimumExtents());
	const mmlVector<2> bmax = b.GetTransform().TransformPoint(b.GetMaximumExtents());
	return BoxBox(amin, amax, bmin, bmax);
}

bool BoxCollider::CollidesWith(const CircleCollider &c) const
{
	const mmlVector<2> amin = m_transform.TransformPoint(GetMinimumExtents());
	const mmlVector<2> amax = m_transform.TransformPoint(GetMaximumExtents());
	const mmlVector<2> bcenter = c.GetTransform.TransformPoint(mmlVector<2>(0.0f, 0.0f));
	return BoxCircle(amin, amax, bcenter, c.GetRadius());
}

BoxCollider::BoxCollider( void ) : m_dimensions(0.0f, 0.0f)
{
}

BoxCollider::BoxCollider(const mmlVector<2> &dimensions) : m_dimensions()
{
	SetDimensions(dimensions);
}

BoxCollider::BoxCollider(float width, float height) : m_dimensions()
{
	SetDimensions(width, height);
}

BoxCollider::BoxCollider(const BoxCollider &collider) : m_dimensions(collider.m_dimensions)
{
}

BoxCollider &BoxCollider::operator =(const BoxCollider &collider)
{
	m_dimensions = collider.m_dimensions;
	return *this;
}

const mmlVector<2> &BoxCollider::GetDimensions( void ) const
{
	return m_dimensions;
}

void BoxCollider::SetDimensions(const mmlVector<2> &dimensions)
{
	SetWidth(dimensions[0]);
	SetHeight(dimensions[1]);
}

void BoxCollider::SetDimensions(float width, float height)
{
	SetWidth(width);
	SetHeight(height);
}

float BoxCollider::GetWidth( void ) const
{
	return m_dimensions[0];
}

float BoxCollider::GetHeight( void ) const
{
	return m_dimensions[1];
}

void BoxCollider::SetWidth(float width)
{
	m_dimensions[0] = mmlMax2(-width, width);
}

void BoxCollider::SetHeight(float height)
{
	m_dimensions[1] = mmlMax2(-height, height);
}

mmlVector<2> BoxCollider::GetMinimumExtents( void ) const
{
	return m_dimensions * -0.5f;
}

mmlVector<2> BoxCollider::GetMaximumExtents( void ) const
{
	return m_dimensions * 0.5f;
}

bool BoxCollider::Collides(const Collider &collider)
{
	return collider.CollidesWith(*this);
}

/*bool BoxCollider::CollidesRay(mmlVector<2> origin, mmlVector<2> direction)
{
	if (!direction.IsNormalized()) { direction.NormalizeFast(); }

	BoxPoints b = GetBoxPoints();
	mmlVector<2> r1 = origin;
	mmlVector<2> r2 = r1 + direction;
	for (int i = 0, j = 3; i < 4; j = i, ++i) {
		if (RayLine(r1, r2, b.p[j], b.p[i])) { return true; }
	}
	return false;
}

bool BoxCollider::CollidesCone(mmlVector<2> origin, mmlVector<2> direction, float apex)
{
	// I don't know

	// check all points to see if they are within cone
	// if one or more points are inside
		// return true
	// if no point is inside cone
		// check all box lines if they are intersecting with the cone lines (only need to check one of the lines, since if only one line is intersected then a point is inside the code, however, when we get here we have already determined that there are no points inside cone)
		// if intersection
			// return true
	// return false

	return false;
}

bool BoxCollider::CollidesPlane(mmlVector<2> point, mmlVector<2> normal)
{
	// point intersection tests
	if (!normal.IsNormalized()) { normal.NormalizeFast(); }
	BoxPoints b = GetBoxPoints();
	return
		PointPlane(b.p[0], point, normal) || PointPlane(b.p[1], point, normal) ||
		PointPlane(b.p[2], point, normal) || PointPlane(b.p[3], point, normal);
}*/

Box BoxCollider::GetBoundingBox( void ) const
{
	Box b;
	b.min = GetMinimumExtents();
	b.max = GetMaximumExtents();
	return b;
}

Circle BoxCollider::GetBoundingCircle( void ) const
{
	Circle c;
	c.center = (GetMaximumExtents() - GetMinimumExtents()) * 0.5f;
	c.radius = (c.center - GetMaximumExtents()).Len();
	return c;
}

bool CircleCollider::CollidesWith(const BoxCollider &b) const
{
	mmlVector<2> acenter = m_transform.TransformPoint(0.0f, 0.0f);
	mmlVector<2> bmin = b.GetTransform().TransformPoint(b.GetMinimumExtents());
	mmlVector<2> bmax = b.GetTransform().TransformPoint(b.GetMaximumExtents());
	return BoxCircle(bmin, bmax, acenter, m_radius);
}

bool CircleCollider::CollidesWith(const CircleCollider &c) const
{
	mmlVector<2> acenter = m_transform.TransformPoint(0.0f, 0.0f);
	mmlVector<2> bcenter = c.GetTransform().TransformPoint(0.0f, 0.0f);
	return CircleCircle(acenter, m_radius, bcenter, c.GetRadius());
}

bool CircleCollider::Collides(const Collider &collider) const
{
	return collider.CollidesWith(*this);
}
