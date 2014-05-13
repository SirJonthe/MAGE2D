#include "Collider.h"

#define RoundErr 0.001f

bool PointPlane(mmlVector<2> a, mmlVector<2> bpoint, mmlVector<2> bnormal)
{
	mmlVector<2> p = bpoint - a;
	float dot = mmlDot(bnormal, p);
	return dot <= 0.0f;
}

bool PointPoint(mmlVector<2> a, mmlVector<2> b)
{
	a.Floor();
	b.Floor();
	return a[0] == b[0] && a[1] == b[1];
}

bool PointBox(mmlVector<2> a, mmlVector<2> bmin, mmlVector<2> bmax)
{
	return a[0] >= bmin[0] && a[0] < bmax[0] && a[1] >= bmin[1] && a[1] < bmax[1];
}

bool PointCircle(mmlVector<2> a, mmlVector<2> b, float bradius)
{
	return mmlDist(a, b) > bradius;
}

bool BoxBox(mmlVector<2> amin, mmlVector<2> amax, mmlVector<2> bmin, mmlVector<2> bmax)
{
	mmlVector<2> min = mmlMax2(amin, bmin);
	mmlVector<2> max = mmlMin2(amax, bmax);
	return min[0] <= max[0] && min[1] <= max[1];
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

Transform &Collider::GetTransform( void )
{
	return m_transform;
}

const Transform &Collider::GetTransform( void ) const
{
	return m_transform;
}

PointCollider::PointCollider( void ) : m_point(0.0f, 0.0f) {}

PointCollider::PointCollider(float x, float y) : m_point(x, y) {}

PointCollider::PointCollider(mmlVector<2> point) : m_point(point) {}

PointCollider::PointCollider(const PointCollider &collider) : m_point(collider.m_point) {}

PointCollider &PointCollider::operator=(const PointCollider &collider)
{
	m_point = collider.m_point;
	return *this;
}

mmlVector<2> PointCollider::GetPoint( void ) const
{
	return m_point;
}

void PointCollider::SetPoint(float x, float y)
{
	m_point[0] = x;
	m_point[1] = y;
}

void PointCollider::SetPoint(mmlVector<2> point)
{
	m_point = point;
}

float PointCollider::GetX( void ) const
{
	return m_point[0];
}

float PointCollider::GetY( void ) const
{
	return m_point[1];
}

void PointCollider::SetX(float x)
{
	m_point[0] = x;
}

void PointCollider::SetY(float y)
{
	m_point[1] = y;
}

bool PointCollider::Collides(const Collider &collider)
{
	if (collider.GetInstanceType() == PointCollider::GetClassType()) {
		return PointPoint(m_point, dynamic_cast<const PointCollider*>(&collider)->GetPoint());
	} else if (collider.GetInstanceType() == AABBCollider::GetClassType()) {
		const AABBCollider *box = dynamic_cast<const AABBCollider*>(&collider);
		return PointBox(m_point, box->GetMinimumBounds(), box->GetMaximumBounds());
	}
	return false;
}

bool PointCollider::CollidesRay(mmlVector<2> origin, mmlVector<2> direction)
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
}

AABBCollider::Box AABBCollider::GetBoxPoints( void ) const
{
	Box b = { { m_min, mmlVector<2>(m_max[0], m_min[1]), m_max, mmlVector<2>(m_min[0], m_max[1]) } };
	return b;
}

AABBCollider::AABBCollider( void ) : m_min(0.0f, 0.0f), m_max(0.0f, 0.0f) {}

AABBCollider::AABBCollider(float minx, float miny, float maxx, float maxy) : m_min(minx, miny), m_max(maxx, maxy) {}

AABBCollider::AABBCollider(float minx, float miny, mmlVector<2> max) : m_min(minx, miny), m_max(max) {}

AABBCollider::AABBCollider(mmlVector<2> min, mmlVector<2> max) : m_min(min), m_max(max) {}

AABBCollider::AABBCollider(mmlVector<2> min, float maxx, float maxy) : m_min(min), m_max(maxx, maxy) {}

AABBCollider::AABBCollider(const AABBCollider &collider) : m_min(collider.m_min), m_max(collider.m_max) {}

AABBCollider &AABBCollider::operator =(const AABBCollider &collider)
{
	m_min = collider.m_min;
	m_max = collider.m_max;
	return *this;
}

mmlVector<2> AABBCollider::GetMinimumBounds( void ) const
{
	return m_min;
}

mmlVector<2> AABBCollider::GetMaximumBounds( void ) const
{
	return m_max;
}

void AABBCollider::SetMinimumBounds(float x, float y)
{
	m_min[0] = x;
	m_max[1] = y;
}

void AABBCollider::SetMinimumBounds(mmlVector<2> min)
{
	m_min = min;
}

void AABBCollider::SetMaximumBounds(float x, float y)
{
	m_max[0] = x;
	m_max[1] = y;
}

void AABBCollider::SetMaximumBounds(mmlVector<2> max)
{
	m_max = max;
}

float AABBCollider::GetMinimumX( void ) const
{
	return m_min[0];
}

float AABBCollider::GetMinimumY( void ) const
{
	return m_min[1];
}

float AABBCollider::GetMaximumX( void ) const
{
	return m_max[0];
}

float AABBCollider::GetMaximumY( void ) const
{
	return m_max[1];
}

void AABBCollider::SetMinimumX(float x)
{
	m_min[0] = x;
}

void AABBCollider::SetMinimumY(float y)
{
	m_min[1] = y;
}

void AABBCollider::SetMaximumX(float x)
{
	m_max[0] = x;
}

void AABBCollider::SetMaximumY(float y)
{
	m_max[1] = y;
}

bool AABBCollider::Collides(const Collider &collider)
{
	if (collider.GetInstanceType() == PointCollider::GetClassType()) {
		return PointBox(dynamic_cast<const PointCollider*>(&collider)->GetPoint(), m_min, m_max);
	} else if (collider.GetInstanceType() == AABBCollider::GetClassType()) {
		const AABBCollider *box = dynamic_cast<const AABBCollider*>(&collider);
		return BoxBox(m_min, m_max, box->GetMinimumBounds(), box->GetMaximumBounds());
	}
	return false;
}

bool AABBCollider::CollidesRay(mmlVector<2> origin, mmlVector<2> direction)
{
	if (!direction.IsNormalized()) { direction.NormalizeFast(); }

	Box b = GetBoxPoints();
	mmlVector<2> r1 = origin;
	mmlVector<2> r2 = r1 + direction;
	for (int i = 0, j = 3; i < 4; j = i, ++i) {
		if (RayLine(r1, r2, b.p[j], b.p[i])) { return true; }
	}
	return false;
}

bool AABBCollider::CollidesCone(mmlVector<2> origin, mmlVector<2> direction, float apex)
{
	// I don't know

	// check all points to see if they are within cone
	// if one or more points are inside
		// return true
	// if no point is inside cone
		// check all box lines if they are intersecting with the cone lines
		// if intersection
			// return true
	// return false

	return false;
}

bool AABBCollider::CollidesPlane(mmlVector<2> point, mmlVector<2> normal)
{
	// point intersection tests
	if (!normal.IsNormalized()) { normal.NormalizeFast(); }
	Box b = GetBoxPoints();
	return
		PointPlane(b.p[0], point, normal) || PointPlane(b.p[1], point, normal) ||
		PointPlane(b.p[2], point, normal) || PointPlane(b.p[3], point, normal);
}
