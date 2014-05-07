#include "Transform.h"

mmlMatrix<3,3> Transform::GetRotationTransform(float angle, mmlVector<2> around)
{
	float S = sin(angle);
	float C = cos(angle);
	mmlMatrix<3,3> m =
		mmlMatrix<3,3>(1.0f, 0.0f, around[0], 0.0f, 1.0f, around[1], 0.0f, 0.0f, 1.0f) *
		mmlMatrix<3,3>(C, -S, 0.0f, S, C, 0.0f, 0.0f, 0.0f, 1.0f) *
		mmlMatrix<3,3>(1.0f, 0.0f, -around[0], 0.0f, 1.0f, -around[1], 0.0f, 0.0f, 1.0f);
	return m;
}

mmlMatrix<2,2> Transform::GetRotationMatrix(float angle)
{
	float S = sin(angle);
	float C = cos(angle);
	mmlMatrix<2,2> m = mmlMatrix<2,2>(C, -S, S, C);
	return m;
}

Transform::Transform( void ) : m_parent(NULL), m_transform(mmlMatrix<3,3>::IdentityMatrix()), m_name()
{
}

Transform::Transform(const Transform &transform) : m_parent(transform.m_parent), m_transform(transform.m_transform), m_name()
{
	m_name.Copy(transform.m_name);
}

Transform::Transform(mmlVector<2> position, float rotation, mmlVector<2> around, Transform *parent, bool preserveTransform) : m_parent(NULL), m_transform(mmlMatrix<3,3>::IdentityMatrix()), m_name()
{
	SetLocalPosition(position);
	SetLocalRotation(rotation, around);
	SetParentTransform(parent, preserveTransform);
}

Transform &Transform::operator=(const Transform &transform)
{
	if (&transform != this) {
		m_parent = transform.m_parent;
		m_transform = transform.m_transform;
		m_name.Copy(transform.m_name);
	}
	return *this;
}

const mtlString &Transform::GetName( void ) const
{
	return m_name;
}

void Transform::SetName(const mtlChars &name)
{
	m_name.Copy(name);
}

mmlMatrix<3,3> Transform::GetLocalTransform( void ) const
{
	return m_transform;
}

mmlMatrix<3,3> Transform::GetWorldTransform( void ) const
{
	mmlMatrix<3,3> transform = m_transform;
	if (m_parent != NULL) {
		transform = m_parent->GetWorldTransform() * transform;
	}
	return transform;
}

void Transform::SetLocalTransform(mmlMatrix<3,3> transform)
{
	m_transform = transform;
}

void Transform::SetWorldTransform(mmlMatrix<3,3> transform)
{
	if (m_parent != NULL) {
		m_transform = transform * mmlInv(m_parent->GetWorldTransform());
	} else {
		m_transform = transform;
	}
}

mmlVector<2> Transform::GetLocalPosition( void ) const
{
	return mmlVector<2>(m_transform[0][2], m_transform[1][2]);
}

mmlVector<2> Transform::GetWorldPosition( void ) const
{
	return mmlVector<2>(mmlVector<3>(m_transform[0][2], m_transform[1][2], 1.0f) * GetWorldTransform());
}

void Transform::SetLocalPosition(mmlVector<2> position)
{
	SetLocalPosition(position[0], position[1]);
}

void Transform::SetLocalPosition(float x, float y)
{
	m_transform[0][2] = x;
	m_transform[1][2] = y;
}

void Transform::SetWorldPosition(mmlVector<2> position)
{
	if (m_parent != NULL) {
		mmlVector<3> p(position[0], position[1], 1.0f);
		SetLocalPosition(mmlVector<2>(p * mmlInv(m_parent->GetWorldTransform())));
	} else {
		SetLocalPosition(position);
	}
}

void Transform::SetWorldPosition(float x, float y)
{
	SetWorldPosition(mmlVector<2>(x, y));
}

mmlMatrix<2,2> Transform::GetLocalRotation( void ) const
{
	return mmlMatrix<2,2>(m_transform);
}

mmlMatrix<2,2> Transform::GetWorldRotation( void ) const
{
	mmlMatrix<2,2> rotation = GetLocalRotation();
	if (m_parent != NULL) {
		rotation = m_parent->GetWorldRotation() * rotation;
	}
	return rotation;
}

void Transform::SetLocalRotation(mmlMatrix<2,2> rotation)
{
	m_transform[0][0] = rotation[0][0];
	m_transform[0][1] = rotation[0][1];
	m_transform[1][0] = rotation[1][0];
	m_transform[1][1] = rotation[1][1];
}

void Transform::SetLocalRotation(float angle, mmlVector<2> around)
{
	m_transform = GetRotationTransform(angle, around);
}

void Transform::SetLocalRotation(float angle)
{
	mmlMatrix<2,2> m = GetRotationMatrix(angle);
	m_transform[0][0] = m[0][0];
	m_transform[0][1] = m[0][1];
	m_transform[1][0] = m[1][0];
	m_transform[1][1] = m[1][1];
}

void Transform::SetWorldRotation(mmlMatrix<2,2> rotation)
{
	if (m_parent != NULL) {
		SetLocalRotation(rotation * mmlInv(m_parent->GetWorldRotation()));
	} else {
		SetLocalRotation(rotation);
	}
}

void Transform::SetWorldRotation(float angle, mmlVector<2> around)
{
	mmlMatrix<3,3> m = GetRotationTransform(angle, around);
	SetWorldTransform(m);
}

void Transform::SetWorldRotation(float angle)
{
	SetWorldRotation(GetRotationMatrix(angle));
}

const Transform *Transform::GetParentTransform( void ) const
{
	return m_parent;
}

void Transform::SetParentTransform(const Transform *parent, bool preserveTransform)
{
	if (preserveTransform) {
		if (parent != NULL) {
			m_transform = GetWorldTransform() * mmlInv(parent->GetWorldTransform());
		} else {
			m_transform = GetWorldTransform();
		}
	}
	m_parent = parent;
}

mmlVector<2> Transform::GetAbsoluteAxisX( void )
{
	return mmlVector<2>(1.0f, 0.0f);
}

mmlVector<2> Transform::GetAbsoluteAxisY( void )
{
	return mmlVector<2>(0.0f, -1.0f);
}

mmlVector<2> Transform::GetLocalAxisX( void ) const
{
	return mmlVector<2>(m_transform[0][0], m_transform[0][1]);
}

mmlVector<2> Transform::GetWorldAxisX( void ) const
{
	return GetLocalAxisX() * GetWorldRotation();
}

void Transform::SetLocalAxisX(mmlVector<2> x)
{
	if (!x.IsNormalized()) { x.Normalize(); }
	m_transform[0][0] = x[0];
	m_transform[0][1] = x[1];
	mmlVector<3> y = mmlCross(mmlVector<3>(x[0], x[1], 0.0f), mmlVector<3>(0.0f, 0.0f, -1.0f));
	m_transform[1][0] = y[0];
	m_transform[1][1] = y[1];
}

void Transform::SetLocalAxisX(float x, float y)
{
	SetLocalAxisX(mmlVector<2>(x, y));
}

void Transform::SetWorldAxisX(mmlVector<2> x)
{
	x *= mmlInv(GetWorldRotation());
	SetLocalAxisX(x);
}

void Transform::SetWorldAxisX(float x, float y)
{
	SetWorldAxisX(mmlVector<2>(x, y));
}

void Transform::LocalAxisXLookAt(mmlVector<2> point)
{
	mmlVector<2> vec = GetLocalPosition() - point;
	SetLocalAxisX(vec);
}

void Transform::LocalAxisXLookAt(float x, float y)
{
	LocalAxisXLookAt(mmlVector<2>(x, y));
}

void Transform::WorldAxisXLookAt(mmlVector<2> point)
{
	mmlVector<2> vec = GetWorldPosition() - point;
	SetWorldAxisX(vec);
}

void Transform::WorldAxisXLookAt(float x, float y)
{
	WorldAxisXLookAt(mmlVector<2>(x, y));
}

mmlVector<2> Transform::GetLocalAxisY( void ) const
{
	return mmlVector<2>(-m_transform[1][0], -m_transform[1][1]);
}

mmlVector<2> Transform::GetWorldAxisY( void ) const
{
	return GetLocalAxisY() * GetWorldRotation();
}

void Transform::SetLocalAxisY(mmlVector<2> y)
{
	if (!y.IsNormalized()) { y.Normalize(); }
	m_transform[1][0] = y[0];
	m_transform[1][1] = y[1];
	mmlVector<3> x = mmlCross(mmlVector<3>(y[0], y[1], 0.0f), mmlVector<3>(0.0f, 0.0f, -1.0f));
	m_transform[0][0] = x[0];
	m_transform[0][1] = x[1];
}

void Transform::SetLocalAxisY(float x, float y)
{
	SetLocalAxisY(mmlVector<2>(x, y));
}

void Transform::SetWorldAxisY(mmlVector<2> y)
{
	y *= mmlInv(GetWorldRotation());
	SetLocalAxisY(y);
}

void Transform::SetWorldAxisY(float x, float y)
{
	SetLocalAxisY(x, y);
}

void Transform::LocalAxisYLookAt(mmlVector<2> point)
{
	mmlVector<2> vec = GetLocalPosition() - point;
	SetLocalAxisY(vec);
}

void Transform::LocalAxisYLookAt(float x, float y)
{
	LocalAxisYLookAt(x, y);
}

void Transform::WorldAxisYLookAt(mmlVector<2> point)
{
	mmlVector<2> vec = GetWorldPosition() - point;
	SetWorldAxisY(vec);
}

void Transform::WorldAxisYLookAt(float x, float y)
{
	WorldAxisYLookAt(mmlVector<2>(x, y));
}

void Transform::ApplyLocalTranslation(mmlVector<2> vector)
{
	SetLocalPosition(GetLocalPosition() + vector);
}

void Transform::ApplyLocalTranslation(float x, float y)
{
	ApplyLocalTranslation(mmlVector<2>(x, y));
}

void Transform::ApplyWorldTranslation(mmlVector<2> vector)
{
	SetWorldPosition(GetWorldPosition() + vector);
}

void Transform::ApplyWorldTranslation(float x, float y)
{
	ApplyWorldTranslation(mmlVector<2>(x, y));
}

void Transform::ApplyLocalRotation(mmlMatrix<2,2> rotation)
{
	SetLocalRotation(GetLocalRotation() * rotation);
}

void Transform::ApplyLocalRotation(float angle, mmlVector<2> around)
{
	mmlMatrix<3,3> m = GetRotationTransform(angle, around);
	m_transform = m_transform * m;
}

void Transform::ApplyLocalRotation(float angle)
{
	mmlMatrix<2,2> m = GetRotationMatrix(angle);
	SetLocalRotation(GetLocalRotation() * m);
}

void Transform::ApplyWorldRotation(mmlMatrix<2,2> rotation)
{
	SetWorldRotation(GetWorldRotation() * rotation);
}

void Transform::ApplyWorldRotation(float angle, mmlVector<2> around)
{
	SetWorldTransform(m_transform * GetRotationTransform(angle, around));
}

void Transform::ApplyWorldRotation(float angle)
{
	SetWorldRotation(GetWorldRotation() * GetRotationMatrix(angle));
}

void Transform::ApplyLocalTransform(Transform transform)
{
	m_transform *= transform.GetWorldTransform();
}

void Transform::ApplyWorldTransform(Transform transform)
{
	m_transform = transform.GetWorldTransform() * mmlInv(GetWorldTransform());
}
