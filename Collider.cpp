#include "Collider.h"

#define RoundErr 0.001f

UnaryCollisionInfo RayCollide(Ray r, mmlVector<2> a, mmlVector<2> b)
{
	if (!r.direction.IsNormalized()) { r.direction.NormalizeFast(); }

	UnaryCollisionInfo info;
	info.collider = NULL;
	info.collision = false;

	mmlVector<2> r1 = r.origin;
	mmlVector<2> r2 = (r.length >= 0.0f) ? r.origin + r.direction * r.length : r.origin + r.direction;

	float rCross = Cross2D(r1, r2);
	float lCross = Cross2D(a, b);
	float div = 1.0f / Cross2D(r1 - r2, a - b);

	mmlVector<2> point = ((a - b) * rCross - (r1 - r2) * lCross) * div;

	mmlVector<2> ln1 = mmlNormalizeFast(b - a);
	mmlVector<2> ln2 = -ln1;
	mmlVector<2> pn1 = mmlNormalizeFast(a - point);
	mmlVector<2> pn2 = mmlNormalizeFast(b - point);

	float dl1 = mmlDot(ln1, pn1);
	float dl2 = mmlDot(ln2, pn2);

	if (dl1 >= 1.0f - RoundErr && dl1 <= 1.0f + RoundErr && dl2 >= 1.0f - RoundErr && dl2 <= 1.0f + RoundErr) {
		if (r.length >= 0.0f) {
			mmlVector<2> rn1 = mmlNormalizeFast(r2 - r1);
			mmlVector<2> rn2 = -rn2;
			mmlVector<2> prn1 = mmlNormalizeFast(r1 - point);
			mmlVector<2> prn2 = mmlNormalizeFast(r2 - point);

			float dr1 = mmlDot(rn1, prn1);
			float dr2 = mmlDot(rn2, prn2);

			info.collision = (dr1 >= 1.0f - RoundErr && dr1 <= 1.0f + RoundErr && dr2 >= 1.0f - RoundErr && dr2 <= 1.0f - RoundErr);
		} else {
			info.collision = true;
		}
	}

	if (info.collision) {
		info.contactPoints.Create(1);
		info.contactPoints[0] = point;
		//info.reflection.Create(1);
		//info.reflection[0] = mmlReflect(r.direction, LineNormal(a, b));
	}

	return info;
}

UnaryCollisionInfo RangeCollide(Range r, mmlVector<2> a)
{
	if (!r.direction.IsNormalized()) { r.direction.NormalizeFast(); }
	if (r.apexRadians < 0.0f) { r.apexRadians = 0.0f; }

	UnaryCollisionInfo info;
	info.collider = NULL;
	info.collision = false;

	mmlVector<2> normal = mmlNormalize(r.origin - a);
	float range = (mmlDot(normal, r.direction) + 1.0f) * mmlPI; // in relative radians (in full view = 1 => 2*pi)
	float halfApex = r.apexRadians / 2.0f;
	if (range >= mmlRAD_MAX - halfApex && range <= mmlRAD_MAX + halfApex) {
		info.contactPoints.Create(1);
		info.contactPoints[0] = a;
		info.collision = true;
	}

	return info;
}

UnaryCollisionInfo RangeCollide(Range r, mmlVector<2> a, mmlVector<2> b)
{
	if (!r.direction.IsNormalized()) { r.direction.NormalizeFast(); }
	if (r.apexRadians < 0.0f) { r.apexRadians = 0.0f; }

	UnaryCollisionInfo info;
	info.collider = NULL;
	info.collision = false;
	info.contactPoints.SetCapacity(2);

	float halfApex = r.apexRadians / 2.0f;
	mmlVector<2> normal;
	float range;

	if (r.length <= 0.0f || mmlDist(r.origin, a) <= r.length) {
		normal = mmlNormalize(r.origin - a);
		range = (mmlDot(normal, r.direction) + 1.0f) * mmlPI; // in relative radians (in full view = 1 => 2*pi)
		if (range >= mmlRAD_MAX - halfApex && range <= mmlRAD_MAX + halfApex) {
			info.collision = true;
			info.contactPoints.Add(a);
		}
	}

	if (r.length <= 0.0f || mmlDist(r.origin, b) <= r.length) {
		normal = mmlNormalize(r.origin - b);
		range = (mmlDot(normal, r.direction) + 1.0f) * mmlPI; // in relative radians (in full view = 1 => 2*pi)
		if (range >= mmlRAD_MAX - halfApex && range <= mmlRAD_MAX + halfApex) {
			info.collision = true;
			info.contactPoints.Add(b);
		}
	}

	mmlVector<2> dir1 = r.direction * mml2DRotationMatrix(halfApex);
	mmlVector<2> dir2 = r.direction - (dir1 - r.direction);
	Ray ray1 = { r.origin, dir1, r.length };
	Ray ray2 = { r.origin, dir2, r.length };

	UnaryCollisionInfo rayInfo = RayCollide(ray1, a, b);
	if (rayInfo.collision) {
		info.collision = true;
		info.contactPoints.Add(info.contactPoints[0]);
	}

	rayInfo = RayCollide(ray2, a, b);
	if (rayInfo.collision) {
		info.collision = true;
		info.contactPoints.Add(info.contactPoints[0]);
	}

	return info;
}

UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a)
{
	if (!p.normal.IsNormalized()) { p.normal.NormalizeFast(); }

	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = NULL;

	if (mmlDot(p.normal, p.point - a) <= 0.0f) {
		info.collision = true;
		info.contactPoints.Create(1);
		info.contactPoints[0] = a;
	}

	return info;
}

UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a, mmlVector<2> b)
{
	if (!p.normal.IsNormalized()) { p.normal.NormalizeFast(); }

	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = NULL;
	info.contactPoints.SetCapacity(2);

	float a_dist = mmlDot(p.normal, p.point - a);
	float b_dist = mmlDot(p.normal, p.point - b);
	bool a_inside = (a_dist <= 0.0f);
	bool b_inside = (b_dist <= 0.0f);

	if (a_inside != b_inside) {
		info.contactPoints.Add(a + (b - a) * (a_dist / (a_dist - b_dist)));
	}

	if (a_inside) {
		info.collision = true;
		info.contactPoints.Add(a);
	}
	if (b_inside) {
		info.collision = true;
		info.contactPoints.Add(b);
	}

	return info;
}

void GetBoxPoints(mmlVector<2> min, mmlVector<2> max, mmlVector<2> (&out)[4])
{
	out[0] = min;
	out[1] = mmlVector<2>(max[0], min[1]);
	out[2] = max;
	out[3] = mmlVector<2>(min[0], max[1]);
}

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

bool BoxPlane(mmlVector<2> amin, mmlVector<2> amax, mmlVector<2> bpoint, mmlVector<2> bnormal)
{
	mmlVector<2> box[4];
	GetBoxPoints(amin, amax, box);
	return PointPlane(box[0], bpoint, bnormal) || PointPlane(box[1], bpoint, bnormal) || PointPlane(box[2], bpoint, bnormal) || PointPlane(box[3], bpoint, bnormal);
}

bool BoxBox(mmlVector<2> amin, mmlVector<2> amax, mmlVector<2> bmin, mmlVector<2> bmax)
{
	mmlVector<2> min = mmlMax(amin, bmin);
	mmlVector<2> max = mmlMin(amax, bmax);
	return min[0] <= max[0] && min[1] <= max[1];
}

bool LineCircle(const mmlVector<2> &l1, const mmlVector<2> &l2, const mmlVector<2> &cpos, float crad)
{
	const mmlVector<2> lineVector = l2 - l1;
	const mmlVector<2> circleLineVector = cpos - l1;
	const float projectedDistance = mmlClamp(0.0f, mmlDot(circleLineVector, mmlNormalize(lineVector)), lineVector.Len());
	const mmlVector<2> closest = lineVector * projectedDistance;
	const float circleLineDistance = mmlDist(cpos, closest);
	return circleLineDistance < crad;
}

bool BoxCircle(mmlVector<2> amin, mmlVector<2> amax, mmlVector<2> bcenter, float bradius)
{
	mmlVector<2> box[4];
	GetBoxPoints(amin, amax, box);
	return
		PointBox(bcenter, amin, amax) ||
		LineCircle(box[0], box[1], bcenter, bradius) ||
		LineCircle(box[1], box[2], bcenter, bradius) ||
		LineCircle(box[2], box[3], bcenter, bradius) ||
		LineCircle(box[3], box[0], bcenter, bradius);
}

bool CircleCircle(mmlVector<2> acenter, float aradius, mmlVector<2> bcenter, float bradius)
{
	const float radius = aradius + bradius;
	const float dist = mmlDist(acenter, bcenter);
	return dist < radius;
}

// a line is a segment
// a ray has a starting point, but no end point
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

bool CirclePlane(const mmlVector<2> &apos, float arad, const mmlVector<2> &bpos, const mmlVector<2> &bnor)
{
	if (PointPlane(apos, bpos, bnor)) { return true; } // 50/50 culling

	const mmlVector<2> planeVector = mmlTangent(bnor); // already normalized
	const mmlVector<2> circlePlaneVector = apos - bpos;
	const float projectedDistance = mmlDot(circlePlaneVector, planeVector); // planeVector needs to be normalized, however we rely on the input being normalized already
	const mmlVector<2> closest = planeVector * projectedDistance;
	const float circlePlaneDistance = mmlDist(apos, closest);

	return circlePlaneDistance < arad;
}

ColliderTree::ColliderTree( void ) : m_root(NULL), m_parent(NULL), m_children(NULL), m_colliders(), m_depth(0), m_subquad(0), m_maxDepth(0), m_spaceWidth(0), m_spaceHeight(0)
{}

ColliderTree::ColliderTree(ColliderTree *parent, int subquad) : m_root(parent->m_root), m_parent(parent), m_children(NULL), m_colliders(), m_depth(parent->m_depth+1), m_subquad(subquad), m_maxDepth(m_root->m_maxDepth), m_spaceWidth(m_root->m_spaceWidth), m_spaceHeight(m_root->m_spaceHeight)
{
	RefreshTree();
}

ColliderTree::ColliderTree(int width, int height, int maxDepth) : m_root(this), m_parent(NULL), m_children(NULL), m_colliders(), m_depth(0), m_maxDepth(mmlClamp(0, maxDepth, 31)), m_spaceWidth(width), m_spaceHeight(height)
{}

ColliderTree::~ColliderTree( void )
{
	delete m_children;
}

ColliderTree *ColliderTree::GetChild(int i)
{
	return !IsLastLevel() ? ((i>-1&&i<4) ? &m_children->q[i] : NULL) : NULL;
}

ColliderTree *ColliderTree::GetChild(int x, int y)
{
	return !IsLastLevel() ? ((x>-1&&x<2&&y>-1&&y<2) ? &m_children->q[x+(y<<1)] : NULL) : NULL;
}

mtlNode<Collider*> *ColliderTree::AddCollider(Collider *collider)
{
	if (!IsRoot()) {
		return m_root->AddCollider(collider);
	}
	m_colliders.AddLast(collider);
	return m_colliders.GetLast();
}

void ColliderTree::RemoveCollider(Collider *collider)
{
	if (!IsRoot()) {
		m_root->RemoveCollider(collider);
	} else {
		mtlNode<Collider*> *c = m_colliders.GetFirst();
		while (c != NULL) {
			if (c->GetItem() == collider) {
				c = m_colliders.Remove(c);
			} else {
				c = c->GetNext();
			}
		}
	}
}

void ColliderTree::RemoveCollider(mtlNode<Collider*> *collider)
{
	if (!IsRoot()) {
		m_root->RemoveCollider(collider);
	} else {
		m_colliders.Remove(collider);
	}
}

mtlNode<Collider*> *ColliderTree::FindCollider(Collider *collider)
{
	mtlNode<Collider*> *c = m_colliders.GetFirst();
	while (c != NULL) {
		if (c->GetItem() == collider) {
			return c;
		}
		c = c->GetNext();
	}
	return NULL;
}

void ColliderTree::Destroy( void )
{
	delete m_children;
	m_children = NULL;
	m_colliders.RemoveAll();
}

void ColliderTree::ResizeTree(int width, int height, int maxDepth)
{
	if (!IsRoot()) {
		m_root->ResizeTree(width, height, maxDepth);
	} else {
		m_spaceWidth = width;
		m_spaceHeight = height;
		m_maxDepth = mmlClamp(0, maxDepth, 31);
		RefreshTree();
	}
}

void ColliderTree::RefreshTree( void )
{
	delete m_children;
	m_children = NULL;

	if (!IsRoot()) {
		m_colliders.RemoveAll();

		const int w = GetSpaceWidth();
		const int h = GetSpaceHeight();
		const int x = GetSubquadX() * w;
		const int y = GetSubquadY() * h;

		BoxCollider box = BoxCollider(float(w), float(h));
		box.GetTransform().SetPosition(Transform::Local, float(x), float(y));

		mtlNode<Collider*> *collider = m_parent->GetCollider();
		while (collider != NULL) {
			if (box.Collides(*collider->GetItem())) {
				m_colliders.AddFirst(collider->GetItem());
			}
			collider = collider->GetNext();
		}
	}

	if (GetColliderCount() > 1 && GetCurrentDepth() < GetMaxDepth() && GetSpaceWidth() > 1 && GetSpaceHeight() > 1) {
		m_children = new ColliderTreeQuad(this);
		for (int i = 0; i < 4; ++i) {
			m_children->q[i].RefreshTree();
		}
	}
}

ColliderTree *ColliderTree::TraceRay(Ray &ray)
{
	return NULL;
}

ColliderTreeQuad::ColliderTreeQuad(ColliderTree *parent)
{
	for (int i = 0; i < 4; ++i) {
		q[i] = ColliderTree(parent, i);
	}
}

float Collider::GetInverseMass( void ) const
{
	if (m_mass <= 0.0f) {
		return 0.0f;
	}
	return 1.0f / m_mass;
}

Collider::Collider( void ) : m_transform(), m_prevTransform(), m_velocity(0.0f, 0.0f), m_acceleration(0.0f, 0.0f), m_angularMomentum(0.0f), m_torque(0.0f), m_mass(-1.0f), m_friction(0.5f), m_isResting(false), m_hasRigidBody(false)
{}

const Transform &Collider::GetTransform( void ) const
{
	return m_transform;
}

Transform &Collider::GetTransform( void )
{
	return m_transform;
}

const mmlVector<2> &Collider::GetVelocity( void ) const
{
	return m_velocity;
}

const mmlVector<2> &Collider::GetAcceleration( void ) const
{
	return m_acceleration;
}

float Collider::GetAngularMomentum( void ) const
{
	return m_angularMomentum;
}

float Collider::GetTorque( void ) const
{
	return m_torque;
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

bool BoxCollider::CollidesWith(const BoxCollider &b) const
{
	const mmlVector<2> apos = GetTransform().GetPosition(Transform::Global);
	const mmlVector<2> amin = apos - GetHalfExtents();
	const mmlVector<2> amax = apos + GetHalfExtents();
	const mmlVector<2> bpos = b.GetTransform().GetPosition(Transform::Global);
	const mmlVector<2> bmin = bpos - b.GetHalfExtents();
	const mmlVector<2> bmax = bpos + b.GetHalfExtents();
	return BoxBox(amin, amax, bmin, bmax);
}

bool BoxCollider::CollidesWith(const CircleCollider &c) const
{
	const mmlVector<2> apos = GetTransform().GetPosition(Transform::Global);
	const mmlVector<2> amin = apos - GetHalfExtents();
	const mmlVector<2> amax = apos + GetHalfExtents();
	const mmlVector<2> bcenter = c.GetTransform().GetPosition(Transform::Global);
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

bool BoxCollider::Collides(const Collider &collider) const
{
	return collider.CollidesWith(*this);
}

bool BoxCollider::Collides(const Ray &ray) const
{
	mmlVector<2> rdirection = ray.direction;
	if (!rdirection.IsNormalized()) { rdirection.NormalizeFast(); }

	mmlVector<2> boxPosition = GetTransform().GetPosition(Transform::Global);
	mmlVector<2> b[4] = { // WRONG
		boxPosition - GetHalfExtents(),
		boxPosition + mmlVector<2>(-GetHalfExtents()[0], GetHalfExtents()[1]),
		boxPosition + GetHalfExtents(),
		boxPosition + mmlVector<2>(GetHalfExtents()[0], -GetHalfExtents()[1])
	};
	mmlVector<2> r1 = ray.origin;
	mmlVector<2> r2 = r1 + rdirection;
	for (int i = 0, j = 3; i < 4; j = i, ++i) {
		if (RayLine(r1, r2, b[j], b[i])) { return true; }
	}
	return false;
}

bool BoxCollider::Collides(const Range &range) const
{
	mmlVector<2> rdirection = range.direction;
	if (!rdirection.IsNormalized()) { rdirection.NormalizeFast(); }

	mmlVector<2> boxPosition = GetTransform().GetPosition(Transform::Global);
	mmlVector<2> b[4] = { // WRONG
		boxPosition - GetHalfExtents(),
		boxPosition + mmlVector<2>(-GetHalfExtents()[0], GetHalfExtents()[1]),
		boxPosition + GetHalfExtents(),
		boxPosition + mmlVector<2>(GetHalfExtents()[0], -GetHalfExtents()[1])
	};

	for (int i = 0; i < 4; ++i) {
		mmlVector<2> pointToOriginNormal = mmlNormalize(range.origin - b[i]);
		float relativeRange = (mmlDot(pointToOriginNormal, rdirection) + 1.0f) * mmlPI;
		float halfApex = range.apexRadians;
		if (relativeRange >= mmlRAD_MAX - halfApex && relativeRange <= mmlRAD_MAX + halfApex) {
			return true;
		}
	}

	mmlVector<2> r1 = range.origin;
	mmlVector<2> r2 = r1 + rdirection;
	for (int i = 0, j = 3; i < 4; j = i, ++i) {
		if (RayLine(r1, r2, b[i], b[i])) {
			return true;
		}
	}

	return false;
}

bool BoxCollider::Collides(const Plane &plane) const
{
	// point intersection tests
	mmlVector<2> pnormal = plane.normal;
	if (!pnormal.IsNormalized()) { pnormal.NormalizeFast(); }
	mmlVector<2> boxPosition = GetTransform().GetPosition(Transform::Global);
	mmlVector<2> b[4] = { // MAKE NON-AXIS-ALIGNED?
		boxPosition - GetHalfExtents(),
		boxPosition + mmlVector<2>(-GetHalfExtents()[0], GetHalfExtents()[1]),
		boxPosition + GetHalfExtents(),
		boxPosition + mmlVector<2>(GetHalfExtents()[0], -GetHalfExtents()[1])
	};
	return
		PointPlane(b[0], plane.point, pnormal) || PointPlane(b[1], plane.point, pnormal) ||
		PointPlane(b[2], plane.point, pnormal) || PointPlane(b[3], plane.point, pnormal);
}

mmlVector<2> BoxCollider::GetHalfExtents( void ) const
{
	return m_dimensions / 2.0f;
}

void BoxCollider::SetHalfExtents(const mmlVector<2> &halfExtents)
{
	m_dimensions = mmlAbs(halfExtents) * 2.0f;
}

bool CircleCollider::CollidesWith(const BoxCollider &b) const
{
	mmlVector<2> acenter = m_transform.GetPosition(Transform::Global);
	mmlVector<2> bpos = b.GetTransform().GetPosition(Transform::Global);
	mmlVector<2> bmin = bpos - b.GetHalfExtents();
	mmlVector<2> bmax = bpos + b.GetHalfExtents();
	return BoxCircle(bmin, bmax, acenter, m_radius);
}

bool CircleCollider::CollidesWith(const CircleCollider &c) const
{
	mmlVector<2> acenter = m_transform.GetPosition(Transform::Global);
	mmlVector<2> bcenter = c.GetTransform().GetPosition(Transform::Global);
	return CircleCircle(acenter, m_radius, bcenter, c.GetRadius());
}

CircleCollider::CircleCollider( void ) : m_radius(0.0f)
{}

CircleCollider::CircleCollider(float radius) : m_radius(radius)
{}


float CircleCollider::GetRadius( void ) const
{
	return m_radius;
}

void CircleCollider::SetRadiuds(float radius)
{
	m_radius = fabs(radius);
}

bool CircleCollider::Collides(const Collider &collider) const
{
	return collider.CollidesWith(*this);
}

mmlVector<2> CircleCollider::GetHalfExtents( void ) const
{
	return mmlVector<2>(m_radius, m_radius);
}

void CircleCollider::SetHalfExtents(const mmlVector<2> &halfExtents)
{
	m_radius = halfExtents.Len();
}
