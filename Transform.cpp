#include "Transform.h"

mmlMatrix<2,2> Transform::GetParentWorldRotation( void ) const
{
	if (m_parent != NULL) {
		return m_parent->GetWorldRotation();
	}
	return mmlMatrix<2,2>::IdentityMatrix();
}

mmlVector<2> Transform::GetParentWorldPosition( void ) const
{
	mmlVector<2> p;
	if (m_parent != NULL) {
		p = m_parent->GetWorldPosition();
	} else {
		p[0] = 0.0f;
		p[1] = 0.0f;
	}
	return p;
}

mmlMatrix<2,2> Transform::GetRotationMatrix(float angle)
{
	const float SIN = sin(angle);
	const float COS = cos(angle);
	mmlMatrix<2,2> m;
	m[0][0] = COS; m[0][1] = -SIN;
	m[1][0] = SIN; m[1][1] = COS;
	return m;
}

Transform::Transform( void ) : m_parent(NULL), m_rotation(mmlMatrix<2,2>::IdentityMatrix()), m_position(0.0f, 0.0f, 0.0f), m_name()
{}

Transform::Transform(const Transform &transform) : m_parent(transform.m_parent), m_rotation(transform.m_rotation), m_position(transform.m_position)
{
	m_name.Copy(transform.m_name);
}

Transform &Transform::operator =(const Transform &transform)
{
	if (&transform != this) {
		m_parent = transform.m_parent;
		m_rotation = transform.m_rotation;
		m_position = transform.m_position;
		m_name.Copy(transform.m_name);
	}
	return *this;
}

const Transform *Transform::GetParentTransform( void ) const
{
	return m_parent;
}

void Transform::SetParentTransform(const Transform *parent, bool preserve)
{
	if (preserve) {
		if (parent == NULL) {
			m_position = GetWorldPosition();
			m_rotation = GetWorldRotation();
		} else {
			// GetWorldRotation() = parent->GetWorldRotation() * X;
			// X = new local rotation (m_rotation)
			//	inv(parent->GetWorldRotation()) * parent->GetWorldRotation() = I
			//	inv(parent->GetWorldRotation()) * GetWorldRotation() = inv(parent->GetWorldRotation()) * parent->GetWorldRotation() * X
			//	inv(parent->GetWorldRotation()) * parent->GetWorldRotation() * X = X
			SetWorldPosition(parent->GetWorldPosition() + m_position);
			m_rotation = mmlInv(parent->GetWorldRotation()) * GetWorldRotation();
		}
	}
	m_parent = parent;
}

const mtlString &Transform::GetName( void ) const
{
	return m_name;
}

const mmlMatrix<2,2> &Transform::GetLocalRotation( void ) const
{
	return m_rotation;
}

mmlMatrix<2,2> Transform::GetWorldRotation( void ) const
{
	//return m_rotation * GetParentWorldRotation();
	return GetParentWorldRotation() * m_rotation;
}

void Transform::SetName(const mtlChars &name)
{
	m_name.Copy(name);
}

const mmlVector<2> &Transform::GetLocalPosition( void ) const
{
	return m_position;
}

void Transform::SetLocalPosition(float x, float y)
{
	m_position[0] = x;
	m_position[1] = y;
}

void Transform::SetLocalPosition(const mmlVector<2> &position)
{
	m_position = position;
}

mmlVector<2> Transform::GetWorldPosition( void ) const
{
	return m_position + GetParentWorldPosition();
}

void Transform::SetWorldPosition(float x, float y)
{
	SetWorldPosition(mmlVector<2>(x, y));
}

void Transform::SetWorldPosition(const mmlVector<2> &position)
{
	m_position += (position - GetWorldPosition());
}

const mmlVector<2> &Transform::GetLocalAxisX( void ) const
{
	return m_rotation[0];
}

const mmlVector<2> &Transform::GetLocalAxisY( void ) const
{
	return m_rotation[1];
}

mmlVector<2> Transform::GetWorldAxisX( void )
{
	const static mmlVector<2> xAxis(1.0f, 0.0f);
	return xAxis;
}

mmlVector<2> Transform::GetWorldAxisY( void )
{
	const static mmlVector<2> yAxis(0.0f, 1.0f);
	return yAxis;
}

void Transform::ApplyLocalTranslation(float x, float y)
{
	m_position[0] += x;
	m_position[1] += y;
}

void Transform::ApplyLocalTranslation(const mmlVector<2> &translation)
{
	m_position += translation;
}

void Transform::ApplyWorldTranslation(float x, float y)
{
	ApplyWorldTranslation(mmlVector<2>(x, y));
}

void Transform::ApplyWorldTranslation(const mmlVector<2> &translation)
{
	const mmlVector<2> worldTrans = translation * GetWorldRotation();
	ApplyLocalTranslation(worldTrans);
}

void Transform::ApplyRotation(float angle)
{
	m_rotation *= GetRotationMatrix(angle);
}

void Transform::ApplyRotation(const mmlVector<2> &around, float angle)
{
	mmlMatrix<2,2> rot = GetRotationMatrix(angle);
	m_rotation *= rot;
	m_position -= around;
	m_position *= rot;
	m_position += around;
}

mmlVector<2> Transform::TransformLocalPoint(const mmlVector<2> &point) const
{
	return (point * m_rotation) + m_position;
}

mmlVector<2> Transform::TransformLocalPoint(float x, float y) const
{
	return TransformLocalPoint(mmlVector<2>(x, y));
}

mmlVector<2> Transform::TransformWorldPoint(const mmlVector<2> &point) const
{
	return (point * GetWorldRotation()) + GetWorldPosition();
}

mmlVector<2> Transform::TransformWorldPoint(float x, float y) const
{
	return TransformWorldPoint(x, y);
}

void Transform::Scale(float scaleFactor)
{
	m_rotation *= mmlMatrix<2,2>::ScaleMatrix(scaleFactor);
}
