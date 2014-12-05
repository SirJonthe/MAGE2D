#include "Collider.h"
#include "MTL/mtlParser.h"
#include <iostream>

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
		info.points->Create(1);
		(*info.points.GetShared())[0] = point;
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

	if (r.length > 0.0f && mmlDist(r.origin, a) > r.length) {
		return info;
	}

	mmlVector<2> normal = mmlNormalize(r.origin - a);
	float range = (mmlDot(normal, r.direction) + 1.0f) * mmlPI; // in relative radians (in full view = 1 => 2*pi)
	float halfApex = r.apexRadians / 2.0f;
	if (range >= mmlRAD_MAX - halfApex && range <= mmlRAD_MAX + halfApex) {
		info.points->Create(1);
		(*info.points.GetShared())[0] = a;
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

	mtlArray< mmlVector<2> > points;
	points.SetCapacity(2);

	float halfApex = r.apexRadians / 2.0f;
	mmlVector<2> normal;
	float range;

	if (r.length <= 0.0f || mmlDist(r.origin, a) <= r.length) {
		normal = mmlNormalize(r.origin - a);
		range = (mmlDot(normal, r.direction) + 1.0f) * mmlPI; // in relative radians (in full view = 1 => 2*pi)
		if (range >= mmlRAD_MAX - halfApex && range <= mmlRAD_MAX + halfApex) {
			info.collision = true;
			points.Add(a);
		}
	}

	if (r.length <= 0.0f || mmlDist(r.origin, b) <= r.length) {
		normal = mmlNormalize(r.origin - b);
		range = (mmlDot(normal, r.direction) + 1.0f) * mmlPI; // in relative radians (in full view = 1 => 2*pi)
		if (range >= mmlRAD_MAX - halfApex && range <= mmlRAD_MAX + halfApex) {
			info.collision = true;
			points.Add(b);
		}
	}

	mmlVector<2> dir1 = r.direction * mml2DRotationMatrix(halfApex);
	mmlVector<2> dir2 = r.direction - (dir1 - r.direction);
	Ray ray1 = { r.origin, dir1, r.length };
	Ray ray2 = { r.origin, dir2, r.length };

	UnaryCollisionInfo rayInfo = RayCollide(ray1, a, b);
	if (rayInfo.collision) {
		info.collision = true;
		points.Add((*info.points.GetShared())[0]);
	}

	rayInfo = RayCollide(ray2, a, b);
	if (rayInfo.collision) {
		info.collision = true;
		points.Add((*info.points.GetShared())[0]);
	}

	if (r.length >= 0.0f) {
		info.points->SetCapacity(points.GetSize() * 2);
		if (points.GetSize() > 1) {
			float j_dist = mmlDist(points[points.GetSize() - 1], r.origin);
			bool j_inside = j_dist <= r.length;
			for (int i = 0, j = points.GetSize() - 1; i < points.GetSize(); j=i, ++i) {
				float i_dist = mmlDist(points[i], r.origin);
				bool i_inside = i_dist <= r.length;

				if (j_inside != i_inside) {
					if (!i_inside) {
						info.points->Add((points[i] - r.origin) * (r.length / i_dist));
					} else {
						info.points->Add((points[j] - r.origin) * (r.length / j_dist));
					}
				}
				if (i_inside) {
					info.points->Add(points[i]);
				}

				j_dist = i_dist;
				j_inside = i_inside;
			}
		} else if (points.GetSize() == 1 && mmlDist(points[0], r.origin) <= r.length) {
			info.points->Add(points[0]);
		}
	} else {
		info.points->Copy(points);
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
		info.points->Create(1);
		(*info.points.GetShared())[0] = a;
	}

	return info;
}

UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a, mmlVector<2> b)
{
	if (!p.normal.IsNormalized()) { p.normal.NormalizeFast(); }

	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = NULL;
	info.points->SetCapacity(2);

	float a_dist = mmlDot(p.normal, p.point - a);
	float b_dist = mmlDot(p.normal, p.point - b);
	bool a_inside = (a_dist <= 0.0f);
	bool b_inside = (b_dist <= 0.0f);

	if (a_inside != b_inside) {
		info.points->Add(a + (b - a) * (a_dist / (a_dist - b_dist)));
	}

	if (a_inside) {
		info.collision = true;
		info.points->Add(a);
	}
	if (b_inside) {
		info.collision = true;
		info.points->Add(b);
	}

	return info;
}

bool LineIntersection(mmlVector<2> a1, mmlVector<2> a2, mmlVector<2> b1, mmlVector<2> b2, mmlVector<2> &out)
{
	mmlVector<2> sa = a2 - a1;
	mmlVector<2> sb = b2 - b1;

	float s = (-sa[1] * (a1[0] - b1[0]) + sa[0] * (a1[1] - b1[1])) / (-sb[0] * sa[1] + sa[0] * sb[1]);
	float t = ( sb[0] * (a1[1] - b1[1]) - sb[1] * (a1[0] - b1[0])) / (-sb[0] * sa[1] + sa[0] * sb[1]);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		out = a1 + (t * sa);
		return true;
	}
	return false;
}

bool PointInPolygon(mmlVector<2> a, const mtlArray< mmlVector<2> > &poly)
{
	bool c = false;
	for (int i = 0, j = poly.GetSize() - 1; i < poly.GetSize(); j = i++) {
		if ( ((poly[i][1] > a[1]) != (poly[j][1] > a[1])) && (a[0] < (poly[j][0] - poly[i][0]) * (a[1] - poly[i][1]) / (poly[j][1] - poly[i][1]) + poly[i][0]) ) {
			c = !c;
		}
	}
	return c;
}

Collider::Collider( void ) : mtlBase(this), m_transform(NULL)
{
}

Transform &Collider::GetTransform( void )
{
	return *m_transform;
}

const Transform &Collider::GetTransform( void ) const
{
	return *m_transform;
}

void Collider::SetTransform(Transform *transform)
{
	m_transform = transform;
	if (m_transform != NULL) {
		m_prevTransform = *m_transform;
	}
}

CollisionInfo PolygonCollider::CollidesWith(const PolygonCollider &c) const
{
	CollisionInfo info;
	info.c1 = NULL;
	info.c2 = NULL;
	info.collision = false;
	info.avg_intersection = mmlVector<2>(0.0f, 0.0f);
	info.unique_points = 0;

	if (m_vert.GetSize() == 0 || c.m_vert.GetSize() == 0) { return info; }

	mtlList< mmlVector<2> > points1;
	mtlList< mmlVector<2> > points2;

	mmlVector<2> a1 = m_globalVert[m_globalVert.GetSize() - 1];

	for (int i = 0/*, k = m_globalVert.GetSize() - 1*/; i < m_globalVert.GetSize(); /*k=i,*/ ++i) {

		mmlVector<2> a2 = m_globalVert[i];
		mmlVector<2> sa = a2 - a1;

		mmlVector<2> b1 = c.m_globalVert[c.m_globalVert.GetSize() - 1];

		for (int j = 0/*, l = c.m_globalVert.GetSize() - 1*/; j < c.m_globalVert.GetSize(); /*l=j,*/ ++j) {

			mmlVector<2> b2 = c.m_globalVert[j];
			mmlVector<2> sb = b2 - b1;

			float s = (-sa[1] * (a1[0] - b1[0]) + sa[0] * (a1[1] - b1[1])) / (-sb[0] * sa[1] + sa[0] * sb[1]);
			float t = ( sb[0] * (a1[1] - b1[1]) - sb[1] * (a1[0] - b1[0])) / (-sb[0] * sa[1] + sa[0] * sb[1]);

			if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {

				//m_collide[k] = true;
				//c.m_collide[l] = true;

				mmlVector<2> intersection = a1 + (t * sa);
				points1.AddLast(intersection);
				points2.AddLast(intersection);
				info.avg_intersection += intersection;
				++info.unique_points;

			} else {
				if (PointInPolygon(a2, c.m_globalVert) && PointInPolygon(a1, c.m_globalVert)) {
					points1.AddLast(a2);
					points1.AddLast(a1);
					//m_collide[k] = true;
					info.avg_intersection += (a1 + a2);
					info.unique_points += 2;
				}
				if (PointInPolygon(b2, m_globalVert) && PointInPolygon(b1, m_globalVert)) {
					points2.AddLast(b2);
					points2.AddLast(b1);
					//c.m_collide[l] = true;
					info.avg_intersection += (b1 + b2);
					info.unique_points += 2;
				}
			}

			b1 = b2;

		}

		a1 = a2;
	}

	if (points1.GetSize() > 0 || points2.GetSize() > 0) {

		info.c1 = this;
		info.c2 = &c;
		info.collision = true;

		info.c1_points.New();
		info.c1_points.GetShared()->Create(points1.GetSize());

		info.c2_points.New();
		info.c2_points.GetShared()->Create(points2.GetSize());

		mtlItem< mmlVector<2> > *node = points1.GetFirst();
		for (int i = 0; i < info.c1_points.GetShared()->GetSize(); ++i) {
			(*info.c1_points.GetShared())[i] = node->GetItem();
			node = node->GetNext();
		}

		node = points2.GetFirst();
		for (int i = 0; i < info.c2_points.GetShared()->GetSize(); ++i) {
			(*info.c2_points.GetShared())[i] = node->GetItem();
			node = node->GetNext();
		}

		info.avg_intersection /= (float)info.unique_points;

	}

	return info;
}

PolygonCollider::PolygonCollider( void ) : mtlInherit(this), m_vert(), m_globalVert()
{
}

PolygonCollider::PolygonCollider(PolygonCollider::Shape shape) : mtlInherit(this), m_vert(), m_globalVert()
{
	CreateShape(shape);
}

PolygonCollider::PolygonCollider(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center) : mtlInherit(this), m_vert(), m_globalVert()
{
	CopyVertexArray(vert, center);
}

void PolygonCollider::CreateShape(PolygonCollider::Shape shape)
{
	switch (shape) {
		case Triangle:
			m_vert.Create(3);
			//m_collide.Create(3);
			m_vert[0] = mmlVector<2>( 0.0f, -1.0f);
			m_vert[1] = mmlVector<2>( 1.0f,  1.0f);
			m_vert[2] = mmlVector<2>(-1.0f,  1.0f);
			break;
		case Rectangle:
			m_vert.Create(4);
			//m_collide.Create(4);
			m_vert[0] = mmlVector<2>(-1.0f, -1.0f);
			m_vert[1] = mmlVector<2>( 1.0f, -1.0f);
			m_vert[2] = mmlVector<2>( 1.0f,  1.0f);
			m_vert[3] = mmlVector<2>(-1.0f,  1.0f);
			break;
		case Hexagon:
			m_vert.Create(6);
			//m_collide.Create(6);
			m_vert[0] = mmlVector<2>( 0.866f,  0.5f);
			m_vert[1] = mmlVector<2>( 0.0f,    1.0f);
			m_vert[2] = mmlVector<2>(-0.866f,  0.5f);
			m_vert[3] = mmlVector<2>(-0.866f, -0.5f);
			m_vert[4] = mmlVector<2>( 0.0f,   -1.0f);
			m_vert[5] = mmlVector<2>( 0.866f, -0.5f);
			break;
		case Rhombus:
			m_vert.Create(4);
			//m_collide.Create(4);
			m_vert[0] = mmlVector<2>( 0.0f, -1.0f);
			m_vert[1] = mmlVector<2>( 1.0f,  0.0f);
			m_vert[2] = mmlVector<2>( 0.0f,  1.0f);
			m_vert[3] = mmlVector<2>(-1.0f,  0.0f);
			break;
	}
	/*for (int i = 0; i < m_collide.GetSize(); ++i) {
		m_collide[i] = false;
	}*/
}

bool PolygonCollider::Load(const mtlDirectory &file)
{
	std::cout << "PolygonCollider::Load: " << file.GetDirectory().GetChars() << std::endl;

	m_vert.Free();

	mtlList< mmlVector<2> > vert;

	mtlString file_contents;
	if (!mtlParser::BufferFile(file, file_contents)) {
		std::cout << "\tfailed to open/read file" << std::endl;
		return false;
	}
	mtlParser parser(file_contents);
	while (!parser.IsEnd()) {
		mtlChars word = parser.ReadWord();
		if (word.Compare("v")) {
			mmlVector<2> v;
			if (!parser.ReadWord().ToFloat(v[0]) || !parser.ReadWord().ToFloat(v[1])) {
				std::cout << "\tfailed to convert param to float" << std::endl;
				return false;
			}
			vert.AddLast(v);
		}
	}

	m_vert.Create(vert.GetSize());
	int i = 0;
	mtlItem< mmlVector<2> > *p = vert.GetFirst();
	while (p != NULL) {
		m_vert[i] = p->GetItem();
		p = p->GetNext();
		++i;
	}

	std::cout << "\tdone" << std::endl;
	return true;
}

int PolygonCollider::GetVertexCount( void ) const
{
	return m_vert.GetSize();
}

const mmlVector<2> &PolygonCollider::GetVertex(int i) const
{
	return m_vert[i];
}

mmlVector<2> &PolygonCollider::GetVertex(int i)
{
	return m_vert[i];
}

const mtlArray< mmlVector<2> > &PolygonCollider::GetVertexArray( void ) const
{
	return m_vert;
}

mtlArray< mmlVector<2> > &PolygonCollider::GetVertexArray( void )
{
	return m_vert;
}

void PolygonCollider::CopyVertexArray(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center)
{
	m_vert.Copy(vert);
	for (int i = 0; i < m_vert.GetSize(); ++i) {
		m_vert[i] -= center;
	}
}

mmlVector<2> PolygonCollider::GetHalfExtents( void ) const
{
	mmlVector<2> max = mmlVector<2>(0.0f, 0.0f);
	for (int i = 0; i < m_vert.GetSize(); ++i) {
		max = mmlMax(m_vert[i], max);
	}
	return max;
}

void PolygonCollider::SetHalfExtents(mmlVector<2> half)
{
	CreateShape(Rectangle);
	for (int i = 0; i < m_vert.GetSize(); ++i) {
		m_vert[i] *= half;
	}
}

void PolygonCollider::ResetState( void )
{
	/*for (int i = 0; i < m_collide.GetSize(); ++i) {
		m_collide[i] = false;
	}*/
	if (m_transform == NULL) {
		m_globalVert.Copy(m_vert);
	} else {
		if (m_vert.GetSize() != m_globalVert.GetSize()) { m_globalVert.Create(m_vert.GetSize()); }
		for (int i = 0; i < m_globalVert.GetSize(); ++i) {
			m_globalVert[i] = GetTransform().TransformPoint(Transform::Global, m_vert[i]);
		}
	}
	Collider::ResetState();
}

UnaryCollisionInfo PolygonCollider::Collides(Ray ray) const
{
	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = NULL;
	info.points.New();
	info.points->SetCapacity(m_vert.GetSize());

	for (int i = 0, j = m_vert.GetSize() - 1; i < m_vert.GetSize(); j = i++) {
		UnaryCollisionInfo temp = RayCollide(ray, m_vert[j], m_vert[i]);
		if (temp.collision) {
			info.collision = true;
			for (int n = 0; n < temp.points->GetSize(); ++n) { // there should only be 1 (otherwise I've done something wrong)
				info.points->Add((*temp.points.GetShared())[n]);
			}
		}
	}

	if (info.collision) {
		info.collider = this;
	}
	return info;
}

UnaryCollisionInfo PolygonCollider::Collides(Range range) const
{
	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = this;
	info.points.New();
	info.points->SetCapacity(m_vert.GetSize());

	for (int i = 0, j = m_vert.GetSize() - 1; i < m_vert.GetSize(); j = i++) {
		UnaryCollisionInfo temp = RangeCollide(range, m_vert[j], m_vert[i]);
		if (temp.collision) {
			info.collision = true;
			for (int n = 0; n < temp.points->GetSize(); ++n) {
				info.points->Add((*temp.points.GetShared())[n]);
			}
		}
	}

	if (info.collision) {
		info.collider = this;
	}
	return info;
}

UnaryCollisionInfo PolygonCollider::Collides(Plane plane) const
{
	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = this;
	info.points.New();
	info.points->SetCapacity(m_vert.GetSize());

	for (int i = 0, j = m_vert.GetSize() - 1; i < m_vert.GetSize(); j = i++) {
		UnaryCollisionInfo temp = PlaneCollide(plane, m_vert[j], m_vert[i]);
		if (temp.collision) {
			info.collision = true;
			for (int n = 0; n < temp.points->GetSize(); ++n) {
				info.points->Add((*temp.points.GetShared())[n]);
			}
		}
	}

	if (info.collision) {
		info.collider = this;
	}
	return info;
}

CollisionInfo PolygonCollider::Collides(const Collider &c) const
{
	return c.CollidesWith(*this);
}
