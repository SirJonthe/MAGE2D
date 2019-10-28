//
//  Transform.h
//  Transform
//
//  Created by Jonathan Karlsson on 8/2/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef TRANSFORM_H_INCLUDED__
#define TRANSFORM_H_INCLUDED__

#include "MiniLib/MML/mmlMatrix.h"
#include "MiniLib/MML/mmlVector.h"

// Might be able to make this more efficient by combining position and rotation,
// but I'd rather not break anything right now...

// @data Transform
// @info Stores position, rotation, and scale.
class Transform
{
private:
	mmlMatrix<2,2>	m_rotation;
	mmlVector<2>	m_position;
	const Transform	*m_parent;

private:
	static mmlMatrix<2,2> RotationMatrix(float angle);
	mmlMatrix<2,2> GetParentRotation( void ) const;
	mmlVector<2> GetParentPosition( void ) const;
	float GetParentScaleX( void ) const;
	float GetParentScaleY( void ) const;
	
public:
	// @data Space
	// @info 
	enum Space
	{
		Local, // Translate(x, y, Local) = m_pos += xy
		Global // moves according to screen space axis (down is down, left is left) (I think this requires using inverse parent transform)
	};
	
	Transform( void );
	Transform(const Transform &transform);
	Transform &operator=(const Transform &transform);
	
	const Transform *GetParent( void ) const;
	void SetParent(Space space, const Transform *parent);
	
	mmlVector<2> GetPosition(Space space) const;
	void SetPosition(Space space, mmlVector<2> position);
	void SetPosition(Space space, float x, float y);
	
	void Translate(Space space, mmlVector<2> vector);
	void Translate(Space space, float x, float y);
	
	void AxisTranslate(float x, float y);
	void AxisTranslate(mmlVector<2> vector);
	
	mmlMatrix<2,2> GetRotation(Space space) const;
	void SetRotation(Space space, float angle);
	void SetIdentity(Space space);
	float GetAngle(Space space) const;
	
	void Rotate(float angle);
	void Rotate(Space space, mmlVector<2> point, float angle); // can benefit from 'space' param
	void Rotate(Space space, float x, float y, float angle);
	
	mmlVector<2> GetAxisX(Space space) const;
	void SetAxisX(Space space, mmlVector<2> normal);
	int GetAxisDirectionX( void ) const;
	int GetAxisDirectionX(Space space, mmlVector<2> up = mmlVector<2>(0.0f, 1.0f)) const;
	void SetAxisDirectionX(int x);
	void FlipAxisX( void );
	
	mmlVector<2> GetAxisY(Space space) const;
	void SetAxisY(Space space, mmlVector<2> normal);
	int GetAxisDirectionY( void ) const;
	int GetAxisDirectionY(Space space, mmlVector<2> right = mmlVector<2>(1.0f, 0.0f)) const;
	void SetAxisDirectionY(int y);
	void FlipAxisY( void );
	
	float GetScaleX(Space space) const;
	void SetScaleX(Space space, float scale);
	
	float GetScaleY(Space space) const;
	void SetScaleY(Space space, float scale);

	void SetScale(Space space, float scale);
	
	void Scale(float scale);
	void Scale(float x, float y);
	void Scale(mmlVector<2> scale);
	void ScaleX(float scale);
	void ScaleY(float scale);	
	
	mmlVector<2> TransformPoint(Space space, mmlVector<2> point) const;
	mmlVector<2> TransformPoint(Space space, float x, float y) const;
	mmlVector<2> InverseTransformPoint(Space space, mmlVector<2> point) const;
	mmlVector<2> InverseTransformPoint(Space space, float x, float y) const;
	
	Transform GetIndependentTransform(Space space) const;

	mmlMatrix<3,3> GetTransformMatrix(Space space) const;

	static mmlVector<2> Down( void );
	static mmlVector<2> Right( void );
	static mmlVector<2> Up( void );
	static mmlVector<2> Left( void );
};

#endif
