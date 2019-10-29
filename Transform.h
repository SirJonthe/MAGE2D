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
	// @info The mode at which transforms occur.
	enum Space
	{
		Local, // Translate(x, y, Local) = m_pos += xy
		Global // moves according to screen space axis (down is down, left is left) (I think this requires using inverse parent transform)
	};
	
	Transform( void );
	Transform(const Transform &transform);
	Transform &operator=(const Transform &transform);
	
	// @algo GetParent
	// @out The parent transform.
	const Transform *GetParent( void ) const;
	
	// @algo SetParent
	// @info Attaches a parent transform to the current transform.
	// @in
	//   space -> The mode at which the transform will be attached. Local will move object to parent space, and Global will fix position in world space.
	//   parent -> The new parent.
	void SetParent(Space space, const Transform *parent);
	
	// @algo GetPosition
	// @in space -> Local space or global space.
	// @out The position.
	mmlVector<2> GetPosition(Space space) const;
	
	// @algo SetPosition
	// @in
	//   space -> The mode at which the position will be altered.
	//   position -> The new position.
	void SetPosition(Space space, mmlVector<2> position);
	
	// @algo SetPosition
	// @in
	//   space -> The mode at which the position will be altered.
	//   x, y -> The new position.
	void SetPosition(Space space, float x, float y);
	
	// @algo Translate
	// @info Moves the current position by a specified amount.
	// @in
	//   space -> The mode at which the position will be translated.
	//   vector -> The movement vector.
	void Translate(Space space, mmlVector<2> vector);
	
	// @algo Translate
	// @info Moves the current position by a specified amount.
	// @in
	//   space -> The mode at which the position will be translated.
	//   x, y -> The movement vector.
	void Translate(Space space, float x, float y);
	
	// @algo AxisTranslate
	// @info Moves the current positions by a specified amount based on the orientation of the transform.
	// @in x, y -> The movement vector.
	void AxisTranslate(float x, float y);
	
	// @algo AxisTranslate
	// @info Moves the current position by a specified amout based on the orientation of the transform.
	// @in vector -> The movement vector.
	void AxisTranslate(mmlVector<2> vector);
	
	// @algo GetRotation
	// @out The rotation of the transform.
	mmlMatrix<2,2> GetRotation(Space space) const;
	
	// @algo SetRotation
	// @info Alters the rotaiton of the transform.
	// @in
	//    space -> The space in which the angle will correspond to the input angle.
	//    angle -> The new angle (radians).
	// @out
	void SetRotation(Space space, float angle);
	
	// @algo SetIdentity
	// @info Sets the identity transform.
	// @in space -> The space in which the transform will be identity.
	void SetIdentity(Space space);
	
	// @algo GetAngle
	// @in space -> The space in which the user is requesting angle information.
	// @out The angle of the transform (radians).
	float GetAngle(Space space) const;
	
	// @algo Rotate
	// @info Rotates the transform by an angle amount.
	// @in angle -> Angle amount (radians).
	// @out
	void Rotate(float angle);
	
	// @algo Rotate
	// @info Rotates the transform around a point.
	// @in
	//   space -> The space in which the rotation will take place.
	//   point -> The point around which the rotation will take place.
	//   angle -> Angle amount (radians).
	void Rotate(Space space, mmlVector<2> point, float angle); // can benefit from 'space' param
	
	// @algo Rotate
	// @info Rotates the transform around a point.
	// @in
	//   space -> The space in which the rotation will take place.
	//   x, y -> The point around which the rotation will take place.
	//   angle -> Angle amount (radians).
	void Rotate(Space space, float x, float y, float angle);
	
	// @algo GetAxisX
	// @in space -> The space in which the axis is requested.
	// @out The X axis of the transform.
	mmlVector<2> GetAxisX(Space space) const;
	
	// @algo SetAxisX
	// @info Alters the X axis of the transform and automatically adjusts the Y axis.
	// @in
	//   space -> The space in which to set the axis.
	//   normal -> The axis.
	void SetAxisX(Space space, mmlVector<2> normal);
	
	// @algo GetAxisDirectionX
	// @out The binary axis direction.
	int GetAxisDirectionX( void ) const;
	
	// @algo GetAxisDirectionX
	// @info Returns axis direction with regards to an up vector.
	// @in
	//   space -> The space in which to return the direction.
	//   up -> The up vector in the specified space.
	// @out The binary direction (-1, 1)
	int GetAxisDirectionX(Space space, mmlVector<2> up = mmlVector<2>(0.0f, 1.0f)) const;
	
	// @algo SetAxisDirectionZ
	// @in x -> Binary direction (-1, 1).
	void SetAxisDirectionX(int x);
	
	// @algo FlipAxisX
	// @info Flips the current X axis.
	void FlipAxisX( void );
	
	// @algo GetAxisY
	// @in space -> The space in which the axis is requested.
	// @out The Y axis of the transform.
	mmlVector<2> GetAxisY(Space space) const;
	
	// @algo SetAxisY
	// @info Alters the Y axis of the transform and automatically adjusts the X axis.
	// @in
	//   space -> The space in which to set the axis.
	//   normal -> The axis.
	void SetAxisY(Space space, mmlVector<2> normal);
	
	// @algo GetAxisDirectionY
	// @out The binary axis direction.
	int GetAxisDirectionY( void ) const;
	
	// @algo GetAxisDirectionY
	// @info Returns axis direction with regards to an up vector.
	// @in
	//   space -> The space in which to return the direction.
	//   up -> The up vector in the specified space.
	// @out The binary direction (-1, 1)
	int GetAxisDirectionY(Space space, mmlVector<2> right = mmlVector<2>(1.0f, 0.0f)) const;
	
	// @algo
	// @in y -> Binary direction (-1, 1).
	void SetAxisDirectionY(int y);
	
	// @algo FlipAxisY
	// @info Flips the current Y axis.
	void FlipAxisY( void );
	
	// @algo GetScaleX
	// @in
	//    space -> The space in which to retrieve the transform scale.
	// @out The scale in X axis.
	float GetScaleX(Space space) const;
	
	// @algo SetScaleX
	// @info Alters scale in X axis.
	// @in
	//   space -> The space in which to alter X scale.
	//   scale -> The new scale.
	void SetScaleX(Space space, float scale);
	
	// @algo GetScaleY
	// @in
	//    space -> The space in which to retrieve the transform scale.
	// @out The scale in Y axis.
	float GetScaleY(Space space) const;
	
	// @algo
	// @info Alters scale in Y axis.
	// @in
	//   space -> The space in which to alter Y scale.
	//   scale -> The new scale.
	void SetScaleY(Space space, float scale);

	// @algo SetScale
	// @info Alters scale of both axis.
	// @in
	//   space -> The space in which to alter scale.
	//   scale -> The new scale.
	void SetScale(Space space, float scale);
	
	// @algo Scale
	// @info Multiplies scale by factor.
	// @in scale -> Factor by multiply current scale by.
	void Scale(float scale);
	
	// @algo Scale
	// @info Multiplies scale by factors.
	// @in x, y -> Factors by multiply current scale by.
	void Scale(float x, float y);
	
	// @algo Scale
	// @info Multiplies scale by factors.
	// @in scale -> Factors by multiply current scale by.
	void Scale(mmlVector<2> scale);
	
	// @algo ScaleX
	// @info Multiplies X scale by factor.
	// @in scale -> Factor by multiply current X scale by.
	void ScaleX(float scale);
	
	// @algo
	// @info Multiplies Y scale by factor.
	// @in scale -> Factor by multiply current Y scale by.
	void ScaleY(float scale);	
	
	// @algo TransformPoint
	// @info Transforms a point using the current transform.
	// @in
	//   space -> The transform space to use.
	//   point -> The point to be transformed.
	// @out The transformed point.
	mmlVector<2> TransformPoint(Space space, mmlVector<2> point) const;
	
	// @algo TransformPoint
	// @info Transforms a point using the current transform.
	// @in
	//   space -> The transform space to use.
	//   x, y -> The point to be transformed.
	// @out The transformed point.
	mmlVector<2> TransformPoint(Space space, float x, float y) const;
	
	// @algo InverseTransformPoint
	// @info Inversely transforms a point using the current transform.
	// @in
	//   space -> The transform space to use.
	//   point -> The point to be transformed.
	// @out The transformed point.
	mmlVector<2> InverseTransformPoint(Space space, mmlVector<2> point) const;
	
	// @algo InverseTransformPoint
	// @info Inversely transforms a point using the current transform.
	// @in
	//   space -> The transform space to use.
	//   x, y -> The point to be transformed.
	// @out The transformed point.
	mmlVector<2> InverseTransformPoint(Space space, float x, float y) const;
	
	// @algo GetIndependentTransform
	// @in space -> The space in which the transform is requested.
	// @out A transform without any parents that corresponds to the requested space.
	Transform GetIndependentTransform(Space space) const;

	// @algo GetTransformMatrix
	// @in space -> The space for which the transform is requested.
	// @out The transform matrix.
	mmlMatrix<3,3> GetTransformMatrix(Space space) const;

	// @algo Down
	// @out The global down vector.
	static mmlVector<2> Down( void );
	
	// @algo Right
	// @out The global right vector.
	static mmlVector<2> Right( void );
	
	// @algo Up
	// @out The global up vector.
	static mmlVector<2> Up( void );
	
	// @algo Left
	// @out The global left vector.
	static mmlVector<2> Left( void );
};

#endif
