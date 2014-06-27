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
	return m_rotation * GetParentWorldRotation();
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

const mmlVector<2> &Transform::GetLocalAxisX( void ) const
{
	return m_rotation[0];
}

const mmlVector<2> &Transform::GetLocalAxisY( void ) const
{
	return m_rotation[1];
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

void Transform::ApplyLocalRotation(float angle)
{
	m_rotation *= GetRotationMatrix(angle);
}

void Transform::ApplyLocalRotation(const mmlVector<2> &around, float angle)
{
	mmlMatrix<2,2> rot = GetRotationMatrix(angle);
	m_rotation *= rot;
	m_position -= around;
	m_position *= rot;
	m_position += around;
}
