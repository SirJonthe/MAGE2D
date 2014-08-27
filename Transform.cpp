//
//  Transform.cpp
//  Transform
//
//  Created by Jonathan Karlsson on 8/2/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#include "Transform.h"
#include <cstdlib>

mmlMatrix<2,2> Transform::RotationMatrix(float angle)
{
	const float Sin = sin(angle);
	const float Cos = cos(angle);

	mmlMatrix<2,2> r;
	r[0][0] = Cos; r[0][1] = -Sin;
	r[1][0] = Sin; r[1][1] = Cos;
	
	return r;
}

mmlMatrix<2,2> Transform::GetParentRotation( void ) const
{
	if (m_parent != NULL) {
		return m_parent->m_rotation * m_parent->GetParentRotation();
	}
	return mmlMatrix<2,2>::IdentityMatrix();
}

mmlVector<2> Transform::GetParentPosition( void ) const
{
	if (m_parent != NULL) {
		return m_parent->GetPosition(Global);
	}
	return mmlVector<2>(0.0f, 0.0f);
}

float Transform::GetParentScaleX( void ) const
{
	if (m_parent != NULL) {
		return m_parent->GetScaleX(Global);
	}
	return 1.0f;
}

float Transform::GetParentScaleY( void ) const
{
	if (m_parent != NULL) {
		return m_parent->GetScaleY(Global);
	}
	return 1.0f;
}

Transform::Transform( void ) : m_position(0.0f, 0.0f), m_rotation(mmlMatrix<2,2>::IdentityMatrix()), m_parent(NULL)
{}

Transform::Transform(const Transform &transform) : m_position(transform.m_position), m_rotation(transform.m_rotation), m_parent(transform.m_parent)
{}

Transform &Transform::operator=(const Transform &transform)
{
	if (this != &transform) {
		m_position = transform.m_position;
		m_rotation = transform.m_rotation;
		m_parent = transform.m_parent;
	}
	return *this;
}

const Transform *Transform::GetParent( void ) const
{
	return m_parent;
}

void Transform::SetParent(Transform::Space space, const Transform *parent)
{
	if (m_parent == parent) { return; }
	
	if (space == Global) {
		if (parent == NULL) {
			m_position = GetPosition(Global);
			m_rotation = GetRotation(Global);
		} else {
			// GetWorldRotation() = parent->GetWorldRotation() * X;
			// X = new local rotation (m_rotation)
			//	inv(parent->GetWorldRotation()) * parent->GetWorldRotation() = I
			//	inv(parent->GetWorldRotation()) * GetWorldRotation() = inv(parent->GetWorldRotation()) * parent->GetWorldRotation() * X
			//	inv(parent->GetWorldRotation()) * parent->GetWorldRotation() * X = X
			mmlMatrix<2,2> parentRotation = parent->GetRotation(Global);
			m_position = (GetPosition(Global) - parent->GetPosition(Global)) * parentRotation;
			m_rotation = mmlInv(parentRotation) * GetRotation(Global);
		}
		m_parent = parent;
	} else if (space == Local) {
		m_parent = parent;
	}
}

mmlVector<2> Transform::GetPosition(Transform::Space space) const
{
	if (space == Local) {
		return m_position;
	} else if (space == Global) {
		return m_position * mmlInv(GetParentRotation()) + GetParentPosition();
	}
	return mmlVector<2>(0.0f, 0.0f);
}

void Transform::SetPosition(Transform::Space space, mmlVector<2> position)
{
	if (space == Local) {
		m_position = position;
	} else if (space == Global) {
		mmlVector<2> vector = (position - GetPosition(Global)) * RotationMatrix(GetAngle(Global) - GetAngle(Local));
		m_position += vector;
	}
}

void Transform::SetPosition(Transform::Space space, float x, float y)
{
	SetPosition(space, mmlVector<2>(x, y));
}

void Transform::Translate(Transform::Space space, mmlVector<2> vector)
{
	SetPosition(space, GetPosition(space) + vector);
}

void Transform::Translate(Transform::Space space, float x, float y)
{
	Translate(space, mmlVector<2>(x, y));
}

void Transform::AxisTranslate(float x, float y)
{
	m_position += GetAxisX(Local) * x + GetAxisY(Local) * y;
}

void Transform::AxisTranslate(mmlVector<2> vector)
{
	AxisTranslate(vector[0], vector[1]);
}

mmlMatrix<2,2> Transform::GetRotation(Transform::Space space) const
{
	if (space == Local) {
		return m_rotation;
	} else if (space == Global) {
		return m_rotation * GetParentRotation();
	}
	return mmlMatrix<2,2>::IdentityMatrix();
}

void Transform::SetRotation(Transform::Space space, float angle)
{
	if (space == Local) {
		m_rotation = RotationMatrix(angle);
		m_rotation[0] *= GetScaleX(Local);
		m_rotation[1] *= GetScaleY(Local);
	} else if (space == Global) {
		m_rotation = RotationMatrix(GetAngle(Local) + angle - GetAngle(Global));
		m_rotation[0] *= GetScaleX(Local);
		m_rotation[1] *= GetScaleY(Local);
	}
}

void Transform::SetIdentity(Transform::Space space)
{
	if (space == Local) {
		m_rotation = mmlMatrix<2,2>::IdentityMatrix();
	} else if (space == Global) {
		Transform t;
		t.SetParent(Global, m_parent);
		m_rotation = t.m_rotation;
	}
}

float Transform::GetAngle(Transform::Space space) const
{
	mmlMatrix<2,2> m = GetRotation(space);
	return atan2(m[1][0], m[0][0]);
}

void Transform::Rotate(float angle)
{
	m_rotation *= RotationMatrix(angle);
}

void Transform::Rotate(Transform::Space space, mmlVector<2> point, float angle)
{
	mmlVector<2> position = GetPosition(space);
	mmlVector<2> vector = point - position;
	mmlVector<2> rotationVector = -vector;
	rotationVector *= RotationMatrix(angle);
	m_rotation *= RotationMatrix(-angle);
	m_position = rotationVector + m_position + vector;
}

void Transform::Rotate(Transform::Space space, float x, float y, float angle)
{
	Rotate(space, mmlVector<2>(x, y), angle);
}

mmlVector<2> Transform::GetAxisX(Transform::Space space) const
{
	return mmlNormalize(GetRotation(space)[0]);
}

void Transform::SetAxisX(Transform::Space space, mmlVector<2> normal)
{
	if (!normal.IsNormalized()) { normal.Normalize(); }
	Transform t;
	t.m_rotation[0] = normal;
	t.m_rotation[1][0] = -normal[1];
	t.m_rotation[1][1] = normal[0];
	m_rotation *= RotationMatrix(t.GetAngle(Local) - GetAngle(space));
}

int Transform::GetAxisDirectionX( void ) const
{
	return (mmlCross2(m_rotation[0], m_rotation[1]) < 0.0f) ? -1 : 1;
}

int Transform::GetAxisDirectionX(Space space, mmlVector<2> up) const
{
	return (mmlCross2(GetAxisX(space), up) < 0.0f) ? -1 : 1;
}

void Transform::SetAxisDirectionX(int x)
{
	int dir = GetAxisDirectionX();
	if ((x < 0 && dir > 0) || (x > 0 && dir < 0)) {
		FlipAxisX();
	}
}

void Transform::FlipAxisX( void )
{
	m_rotation[0] = -m_rotation[0];
}

mmlVector<2> Transform::GetAxisY(Transform::Space space) const
{
	return mmlNormalize(GetRotation(space)[1]);
}

void Transform::SetAxisY(Transform::Space space, mmlVector<2> normal)
{
	if (!normal.IsNormalized()) { normal.Normalize(); }
	Transform t;
	t.m_rotation[1] = normal;
	t.m_rotation[0][0] = normal[1];
	t.m_rotation[0][1] = -normal[0];
	m_rotation *= RotationMatrix(t.GetAngle(Local) - GetAngle(space));
}

int Transform::GetAxisDirectionY( void ) const
{
	return (mmlCross2(m_rotation[1], m_rotation[0]) < 0.0f) ? -1 : 1;
}

int Transform::GetAxisDirectionY(Space space, mmlVector<2> right) const
{
	return (mmlCross2(GetAxisY(space), right) < 0.0f) ? -1 : 1;
}

void Transform::SetAxisDirectionY(int y)
{
	int dir = GetAxisDirectionY();
	if ((y < 0 && dir > 0) || (y > 0 && dir < 0)) {
		FlipAxisY();
	}
}

void Transform::FlipAxisY( void )
{
	m_rotation[1] = -m_rotation[1];
}

float Transform::GetScaleX(Transform::Space space) const
{
	return GetRotation(space)[0].Len();
}

void Transform::SetScaleX(Transform::Space space, float scale)
{
	if (space == Local) {
		m_rotation[0] = mmlNormalize(m_rotation[0]) * scale;
	} else if (space == Global) {
		m_rotation[0] = mmlNormalize(m_rotation[0]) * (GetParentScaleX() / scale);
	}
}

float Transform::GetScaleY(Transform::Space space) const
{
	return GetRotation(space)[1].Len();
}

void Transform::SetScaleY(Transform::Space space, float scale)
{
	if (space == Local) {
		m_rotation[1] = mmlNormalize(m_rotation[1]) * scale;
	} else if (space == Global) {
		m_rotation[1] = mmlNormalize(m_rotation[1]) * (GetParentScaleY() / scale);
	}
}

void Transform::SetScale(Space space, float scale)
{
	SetScaleX(space, scale);
	SetScaleY(space, scale);
}

void Transform::Scale(float scale)
{
	m_rotation[0] *= scale;
	m_rotation[1] *= scale;
}

void Transform::Scale(float x, float y)
{
	m_rotation[0] *= x;
	m_rotation[1] *= y;
}

void Transform::Scale(mmlVector<2> scale)
{
	m_rotation[0] *= scale[0];
	m_rotation[1] *= scale[1];
}

void Transform::ScaleX(float scale)
{
	m_rotation[0] *= scale;
}

void Transform::ScaleY(float scale)
{
	m_rotation[1] *= scale;
}

mmlVector<2> Transform::TransformPoint(Space space, mmlVector<2> point) const
{
	/*Transform t;
	if (space == Global) { // point is in global space, transform it to local space
		t.SetParent(Global, this);
		t.SetPosition(Global, point);
		return t.GetPosition(Local);
	} else if (space == Local) { // point is in local space, transform it to global space
		t.SetPosition(Local, point);
		t.SetParent(Local, this);
		return t.GetPosition(Global);
	}
	return t.GetPosition(Local);*/

	return (point * mmlInv(GetRotation(space))) + GetPosition(space);
}

mmlVector<2> Transform::TransformPoint(Space space, float x, float y) const
{
	return TransformPoint(space, mmlVector<2>(x, y));
}

Transform Transform::GetIndependentTransform(Transform::Space space) const
{
	Transform t;
	t.m_position = GetPosition(space);
	t.m_rotation = GetRotation(space);
	return t;
}

mmlMatrix<3,3> Transform::GetTransformMatrix(Transform::Space space) const
{
	mmlMatrix<2,2> r = GetRotation(space);
	mmlVector<2> p = GetPosition(space);
	return mmlMatrix<3,3>(
		r[0][0], r[0][1], p[0],
		r[1][0], r[1][1], p[1],
		0.0f,    0.0f,    1.0f
	);
}

mmlVector<2> Transform::Down( void )
{
	static mmlVector<2> down = mmlVector<2>(0.0f, 1.0f);
	return down;
}

mmlVector<2> Transform::Right( void )
{
	static mmlVector<2> right = mmlVector<2>(1.0f, 0.0f);
	return right;
}

mmlVector<2> Transform::Up( void )
{
	return -Down();
}

mmlVector<2> Transform::Left( void )
{
	return -Right();
}
