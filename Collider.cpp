#include "Collider.h"
#include "MiniLib/MTL/mtlParser.h"
#include <iostream>

#define RoundErr 0.001f

mmlVector<2> LineIntersectionProjection(const mmlVector<2> &a1, const mmlVector<2> &a2, const mmlVector<2> &b1, const mmlVector<2> &b2)
{
	mmlVector<2> sa = a2 - a1;
	mmlVector<2> sb = b2 - b1;
	mmlVector<2> proj;
	proj[0] = ( sb[0] * (a1[1] - b1[1]) - sb[1] * (a1[0] - b1[0])) / (-sb[0] * sa[1] + sa[0] * sb[1]);
	proj[1] = (-sa[1] * (a1[0] - b1[0]) + sa[0] * (a1[1] - b1[1])) / (-sb[0] * sa[1] + sa[0] * sb[1]);
	return proj;
}

//bool LineIntersection(const mmlVector<2> &a1, const mmlVector<2> &a2, const mmlVector<2> &b1, const mmlVector<2> &b2, mmlVector<2> &out)
//{
//	mmlVector<2> sa = a2 - a1;
//	mmlVector<2> sb = b2 - b1;
//
//	float s = (-sa[1] * (a1[0] - b1[0]) + sa[0] * (a1[1] - b1[1])) / (-sb[0] * sa[1] + sa[0] * sb[1]);
//	float t = ( sb[0] * (a1[1] - b1[1]) - sb[1] * (a1[0] - b1[0])) / (-sb[0] * sa[1] + sa[0] * sb[1]);
//
//	if ((s >= 0.0f && s <= 1.0f) && (t >= 0.0f && t <= 1.0f)) {
//		out = a1 + (t * sa);
//		return true;
//	}
//	return false;
//}

bool LineIntersection(const mmlVector<2> &a1, const mmlVector<2> &a2, const mmlVector<2> &b1, const mmlVector<2> &b2, mmlVector<2> &out)
{
	mmlVector<2> p = LineIntersectionProjection(a1, a2, b1, b2);
	if ((p[0] >= 0.0f && p[0] <= 1.0f) && (p[1] >= 0.0f && p[1] <= 1.0f)) {
		out = mmlLerp(a1, a2, p[0]);
		return true;
	}
	return false;
}

bool PointInPolygon(const mmlVector<2> &a, const mtlArray< mmlVector<2> > &poly)
{
	bool c = false;
	for (int i = 0, j = poly.GetSize() - 1; i < poly.GetSize(); j = i++) {
		if ( ((poly[i][1] > a[1]) != (poly[j][1] > a[1])) && (a[0] < (poly[j][0] - poly[i][0]) * (a[1] - poly[i][1]) / (poly[j][1] - poly[i][1]) + poly[i][0]) ) {
			c = !c;
		}
	}
	return c;
}

//UnaryCollisionInfo RayCollide(Ray r, mmlVector<2> a, mmlVector<2> b)
//{
//	if (!r.direction.IsNormalized()) { r.direction.Normalize(); }

//	UnaryCollisionInfo info;
//	info.collider = NULL;
//	info.collision = false;

//	mmlVector<2> r1 = r.origin;
//	mmlVector<2> r2 = (r.length >= 0.0f) ? r.origin + r.direction * r.length : r.origin + r.direction;

//	float rCross = Cross2D(r1, r2);
//	float lCross = Cross2D(a, b);
//	float div = 1.0f / Cross2D(r1 - r2, a - b);

//	mmlVector<2> point = ((a - b) * rCross - (r1 - r2) * lCross) * div;

//	mmlVector<2> ln1 = mmlNormalize(b - a);
//	mmlVector<2> ln2 = -ln1;
//	mmlVector<2> pn1 = mmlNormalize(a - point);
//	mmlVector<2> pn2 = mmlNormalize(b - point);

//	float dl1 = mmlDot(ln1, pn1);
//	float dl2 = mmlDot(ln2, pn2);

//	if (dl1 >= 1.0f - RoundErr && dl1 <= 1.0f + RoundErr && dl2 >= 1.0f - RoundErr && dl2 <= 1.0f + RoundErr) {
//		if (r.length >= 0.0f) {
//			mmlVector<2> rn1 = mmlNormalize(r2 - r1);
//			mmlVector<2> rn2 = -rn2;
//			mmlVector<2> prn1 = mmlNormalize(r1 - point);
//			mmlVector<2> prn2 = mmlNormalize(r2 - point);

//			float dr1 = mmlDot(rn1, prn1);
//			float dr2 = mmlDot(rn2, prn2);

//			info.collision = (dr1 >= 1.0f - RoundErr && dr1 <= 1.0f + RoundErr && dr2 >= 1.0f - RoundErr && dr2 <= 1.0f - RoundErr);
//		} else {
//			info.collision = true;
//		}
//	}

//	if (info.collision) {
//		info.points->Create(1);
//		(*info.points.GetShared())[0] = point;
//		//info.reflection.Create(1);
//		//info.reflection[0] = mmlReflect(r.direction, LineNormal(a, b));
//	}

//	return info;
//}

UnaryCollisionInfo RayCollide(Ray r, mmlVector<2> a, mmlVector<2> b)
{
	if (!r.direction.IsNormalized()) { r.direction.Normalize(); }

	UnaryCollisionInfo info;
	info.collider = NULL;
	info.points.New();

	mmlVector<2> r1 = r.origin;
	mmlVector<2> r2 = r.origin + r.direction;
	mmlVector<2> p = LineIntersectionProjection(r1, r2, a, b);
	info.collision = p[0] >= 0.0f && (p[1] >= 0.0f && p[1] <= 1.0f);
	if (info.collision) {
		info.points->Create(1);
		(*info.points.GetShared())[0] = mmlLerp(r1, r2, p[0]);
	}

	return info;
}

UnaryCollisionInfo ConeCollide(Cone r, mmlVector<2> a)
{
	if (!r.direction.IsNormalized()) { r.direction.Normalize(); }
	if (r.apexRadians < 0.0f) { r.apexRadians = 0.0f; }

	UnaryCollisionInfo info;
	info.collider = NULL;
	info.collision = false;
	info.points.New();

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

UnaryCollisionInfo ConeCollide(Cone r, mmlVector<2> a, mmlVector<2> b)
{
	if (!r.direction.IsNormalized()) { r.direction.Normalize(); }
	if (r.apexRadians < 0.0f) { r.apexRadians = 0.0f; }

	float halfApex = r.apexRadians / 2.0f;
	mmlVector<2> dir1 = r.direction * mml2DRotationMatrix(halfApex);
	mmlVector<2> dir2 = r.direction - (dir1 - r.direction);
	Ray ray1 = { r.origin, dir1 };
	Ray ray2 = { r.origin, dir2 };

	Plane plane1 = { ray1.origin,  mmlTangent(ray1.direction) };
	Plane plane2 = { ray1.origin, -mmlTangent(ray2.direction) }; // if this does not work, flip signs between the planes
	UnaryCollisionInfo info = PlaneCollide(plane1, a, b);
	if (info.collision && info.points.GetShared() != NULL && info.points->GetSize() == 2) {
		info = PlaneCollide(plane2, (*info.points.GetShared())[0], (*info.points.GetShared())[1]);
	}
	return info;
}

UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a)
{
	if (!p.normal.IsNormalized()) { p.normal.Normalize(); }

	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = NULL;
	info.points.New();

	if (mmlDot(p.normal, p.point - a) <= 0.0f) {
		info.collision = true;
		info.points->Create(1);
		(*info.points.GetShared())[0] = a;
	}

	return info;
}

UnaryCollisionInfo PlaneCollide(Plane p, mmlVector<2> a, mmlVector<2> b)
{
	if (!p.normal.IsNormalized()) { p.normal.Normalize(); }

	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = NULL;
	info.points.New();
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


void GridWalker::SetInitialState(const Ray &p_ray)
{
	m_origin = p_ray.origin;
	m_direction = p_ray.direction;
	m_xy[0] = int( p_ray.origin[0] );
	m_xy[1] = int( p_ray.origin[1] );
	for (int i = 0; i < 2; ++i) {
		m_delta[i] = (m_direction * (1.0f / m_direction[i])).Len();
		if (m_direction[i] < 0.0f) {
			m_step[i] = -1;
			m_lengths[i] = (p_ray.origin[i] - m_xy[i]) * m_delta[i];
		} else {
			m_step[i] = 1;
			m_lengths[i] = (m_xy[i] + 1.0f - p_ray.origin[i]) * m_delta[i];
		}
	}
	Step();
}

void GridWalker::Step( void )
{
	m_side = 0;
	for (int i = 1; i < 2; ++i) {
		if (m_lengths[m_side] > m_lengths[i]) {
			m_side = i;
		}
	}
	m_lengths[m_side] += m_delta[m_side];
	m_xy[m_side] += m_step[m_side];
}

mmlVector<2> GridWalker::GetImpactUV( void ) const
{
	mmlVector<2> impact = GetImpactPosition();
	float u = impact[(m_side + 1) % 2];
	float v = impact[(m_side + 2) % 2];
	return mmlVector<2>(u - floor(u), v - floor(v));
}

Collider::Collider( void ) : mtlBase(this), m_transform(/*NULL*/)
{
}

Transform &Collider::GetTransform( void )
{
//	return *m_transform;
	return *m_transform.GetShared();
}

const Transform &Collider::GetTransform( void ) const
{
//	return *m_transform;
	return *m_transform.GetShared();
}

void Collider::SetTransform(mtlShared<Transform> &transform)
{
	m_transform = transform;
}

//void Collider::SetTransform(Transform *transform)
//{
//	m_transform = transform;
//	if (m_transform != NULL) {
//		m_prevTransform = *m_transform;
//	}
//}

//void Collider::TrackPreviousTransform( void )
//{
//	if (m_transform != NULL) {
//		m_prevTransform = *m_transform;
//	} else {
//		m_prevTransform.SetParent(Transform::Local, NULL);
//		m_prevTransform.SetIdentity(Transform::Local);
//	}
//}

//CollisionInfo PolygonCollider::CollidesWith(const PolygonCollider &c) const
//{
//	CollisionInfo info;
//	info.A = info.B = NULL;
//	info.avg_collision = mmlVector<2>(0.0f, 0.0f);
//	info.A_normal = mmlVector<2>(0.0f, 0.0f);
//	info.B_normal = mmlVector<2>(0.0f, 0.0f);
//	info.points.New();
//	info.collision = false;

//	if (m_globalVert.GetSize() == 0 || c.m_globalVert.GetSize() == 0) { return info; }

//	mtlList< mmlVector<2> > points;

//	int a_intersections = PointInPolygon(m_globalVert[m_globalVert.GetSize() - 1],     c.m_globalVert) ? 1 : 0;
//	int b_intersections = PointInPolygon(c.m_globalVert[c.m_globalVert.GetSize() - 1], m_globalVert)   ? 1 : 0;

//	for (int i = 0, j = m_globalVert.GetSize() - 1; i < m_globalVert.GetSize(); j = i++) {
//		mmlVector<2> a1 = m_globalVert[j];
//		mmlVector<2> a2 = m_globalVert[i];
//		mmlVector<2> a_normal = mmlLineNormal(a1, a2);

//		if ((a_intersections & 1) == 1) {
//			points.AddLast(a1);
//			info.avg_collision += a1;
//		}

//		for (int k = 0, l = c.m_globalVert.GetSize() - 1; k < c.m_globalVert.GetSize(); l = k++) {
//			mmlVector<2> b1 = c.m_globalVert[l];
//			mmlVector<2> b2 = c.m_globalVert[k];

//			mmlVector<2> out;
//			if (LineIntersection(a1, a2, b1, b2, out)) {
//				points.AddLast(out);
//				info.avg_collision += out;
//				++a_intersections;
//				info.A_normal += a_normal;
//			}
//		}
//	}
//	info.A_normal = a_intersections > 0 ? info.A_normal / a_intersections : mmlVector<2>(0.0f, 0.0f);

//	for (int i = 0, j = c.m_globalVert.GetSize() - 1; i < c.m_globalVert.GetSize(); j = i++) {
//		mmlVector<2> b1 = c.m_globalVert[j];
//		mmlVector<2> b2 = c.m_globalVert[i];
//		mmlVector<2> b_normal = mmlLineNormal(b1, b2);

//		if ((b_intersections & 1) == 1) {
//			points.AddLast(b1);
//			info.avg_collision += b1;
//		}

//		for (int k = 0, l = m_globalVert.GetSize() - 1; k < m_globalVert.GetSize(); l = k++) {
//			mmlVector<2> a1 = m_globalVert[l];
//			mmlVector<2> a2 = m_globalVert[k];

//			mmlVector<2> out;
//			if (LineIntersection(a1, a2, b1, b2, out)) {
//				++b_intersections;
//				info.B_normal += b_normal;
//			}
//		}
//	}
//	info.B_normal = b_intersections > 0 ? info.B_normal / b_intersections : mmlVector<2>(0.0f, 0.0f);

//	if (points.GetSize() > 0) {
//		info.A = this;
//		info.B = &c;
//		info.collision = true;
//		info.avg_collision /= (float)points.GetSize();
//		info.points->SetCapacity(points.GetSize());
//		mtlItem< mmlVector<2> > *i = points.GetFirst();
//		while (i != NULL) {
//			info.points->Add(i->GetItem());
//			i = i->GetNext();
//		}
//	}

//	return info;
//}

Contact PolygonCollider::CollidesWith(const PolygonCollider &c)
{
	Contact c;
	c.point_count = 0;

	return c;
}

PolygonCollider::PolygonCollider( void ) : mtlInherit<Collider, PolygonCollider>(this), m_vert(), m_globalVert()
{
}

PolygonCollider::PolygonCollider(PolygonCollider::Shape shape) : mtlInherit<Collider, PolygonCollider>(this), m_vert(), m_globalVert()
{
	CreateShape(shape);
}

PolygonCollider::PolygonCollider(const mtlArray< mmlVector<2> > &vert, const mmlVector<2> &center) : mtlInherit<Collider, PolygonCollider>(this), m_vert(), m_globalVert()
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

mtlChars ReadFloat(mtlSyntaxParser &parser)
{
	mtlArray<mtlChars> m;
	parser.Match("%r %| %i", m);
	parser.Match("f");
	return m.GetSize() > 0 ? m[0] : "";
}

bool PolygonCollider::Load(const mtlPath &file)
{
	std::cout << "PolygonCollider::Load: " << file.GetPath().GetChars() << std::endl;

	m_vert.Free();

	mtlList< mmlVector<2> > vert;

	mtlString file_contents;
	if (!mtlSyntaxParser::BufferFile(file, file_contents)) {
		std::cout << "\tfailed to open/read file" << std::endl;
		return false;
	}
	mtlSyntaxParser parser;
	parser.SetBuffer(file_contents);
	parser.EnableCaseSensitivity();
	parser.SetHyphenators("");
	while (!parser.IsEnd()) {

		if (parser.Match("v") == 0) {
			mtlChars v1 = ReadFloat(parser);
			mtlChars v2 = ReadFloat(parser);
			mmlVector<2> v;
			if (!v1.ToFloat(v[0]) || !v2.ToFloat(v[1])) {
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

const mmlVector<2> &PolygonCollider::GetWorldVertex(int i) const
{
	return m_globalVert[i];
}

mmlVector<2> &PolygonCollider::GetWorldVertex(int i)
{
	return m_globalVert[i];
}

const mtlArray< mmlVector<2> > &PolygonCollider::GetWorldVertexArray( void ) const
{
	return m_globalVert;
}

mtlArray< mmlVector<2> > &PolygonCollider::GetWorldVertexArray( void )
{
	return m_globalVert;
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

void PolygonCollider::SetHalfExtents(const mmlVector<2> &half)
{
	CreateShape(Rectangle);
	for (int i = 0; i < m_vert.GetSize(); ++i) {
		m_vert[i] *= half;
	}
}

BoundRect PolygonCollider::GetBoundRect( void ) const
{
	BoundRect r;
	if (m_globalVert.GetSize() < 1) {
		r.a[0] = 0.0f;
		r.a[1] = 0.0f;
		r.b[0] = 0.0f;
		r.b[1] = 0.0f;
	} else {
		r.a = r.b = m_globalVert[0];
	}
	for (int i = 0; i < m_globalVert.GetSize(); ++i) {
		r.a = mmlMin(r.a, m_globalVert[i]);
		r.b = mmlMax(r.b, m_globalVert[i]);
	}
	return r;
}

//void PolygonCollider::ResetState( void )
void PolygonCollider::UpdateWorldState( void )
{
	//if (m_transform == NULL) {
	if (m_transform.GetShared() == NULL) {
		m_globalVert.Copy(m_vert);
	} else {
		if (m_vert.GetSize() != m_globalVert.GetSize()) { m_globalVert.Create(m_vert.GetSize()); }
		for (int i = 0; i < m_globalVert.GetSize(); ++i) {
			m_globalVert[i] = GetTransform().TransformPoint(Transform::Global, m_vert[i]);
		}
	}
	//Collider::ResetState();
	Collider::UpdateWorldState();
}

float PolygonCollider::GetCircumference( void ) const
{
	float circ = 0.0f;
	for (int i = 0, j = m_vert.GetSize() - 1; i < m_vert.GetSize(); j=i, ++i) {
		circ += (m_vert[j]-m_vert[i]).Len();
	}
	return circ;
}

float PolygonCollider::GetArea( void ) const
{
	float area = 0.0f;
	for (int i = 0, j = m_vert.GetSize() - 1; i < m_vert.GetSize(); j=i, ++i) {
		area += m_vert[i][0]*(m_vert[(i+1) % m_vert.GetSize()][1] - m_vert[j][1]);
	}
	return 0.5f * area;
}

UnaryCollisionInfo PolygonCollider::Collides(Ray ray) const
{
	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = NULL;
	info.points.New();
	info.points->SetCapacity(m_globalVert.GetSize());

	for (int i = 0, j = m_globalVert.GetSize() - 1; i < m_globalVert.GetSize(); j = i++) {
		UnaryCollisionInfo temp = RayCollide(ray, m_globalVert[j], m_globalVert[i]);
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

UnaryCollisionInfo PolygonCollider::Collides(Cone cone) const
{
	UnaryCollisionInfo info;
	info.collision = false;
	info.collider = this;
	info.points.New();
	info.points->SetCapacity(m_globalVert.GetSize());

	for (int i = 0, j = m_globalVert.GetSize() - 1; i < m_globalVert.GetSize(); j = i++) {
		UnaryCollisionInfo temp = ConeCollide(cone, m_globalVert[j], m_globalVert[i]);
		if (temp.collision) {
			info.collision = true;
			info.points->Add((*temp.points.GetShared())[0]); // cone collide can only generate 2 or 0 points, only add index 0 as index 1 will be added on the next pass
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
	info.points->SetCapacity(m_globalVert.GetSize());

	for (int i = 0, j = m_vert.GetSize() - 1; i < m_globalVert.GetSize(); j = i++) {
		UnaryCollisionInfo temp = PlaneCollide(plane, m_globalVert[j], m_globalVert[i]);
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

bool PolygonCollider::IsConvex( void ) const
{
	int num_vert = m_vert.GetSize();
	if (num_vert > 2) {
		for (int i = 0; i < num_vert; ++i) {
			mmlVector<2> a = m_vert[(i + 1) % num_vert] - m_vert[i];
			mmlVector<2> b = m_vert[(i + 2) % num_vert] - m_vert[i];
			if (mmlCross2(a, b) < 0.0f) { return false; }
		}
	}
	return true;
}
