#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "MML/mmlMatrix.h"
#include "MTL/mtlString.h"

class Transform
{
private:
	const Transform	*m_parent;
	mmlMatrix<2,2>	m_rotation;
	mmlVector<2>	m_position;
	mtlString		m_name;

private:
	mmlMatrix<2,2> GetParentWorldRotation( void ) const;
	mmlVector<2> GetParentWorldPosition( void ) const;
	static mmlMatrix<2,2> GetRotationMatrix(float angle);
	// functions for concatenating transform into a single matrix
	//mmlMatrix<3,3> GetLocalTransformMatrix( void ) const;
	//mmlMatrix<3,3> GetWorldTransformMatrix( void ) const;
	//mmlMatrix<3,3> GetParentWorldTransformMatrix( void ) const;

public:
	enum Space
	{
		Local,
		World
	};

	Transform( void );
	Transform(const Transform &transform);
	Transform &operator=(const Transform &transform);

	const Transform *GetParentTransform( void ) const;
	void SetParentTransform(const Transform *parent, bool preserve = true);

	const mtlString &GetName( void ) const;
	void SetName(const mtlChars &name);

	const mmlMatrix<2,2> &GetLocalRotation( void ) const;
	mmlMatrix<2,2> GetWorldRotation( void ) const;

	const mmlVector<2> &GetLocalPosition( void ) const;
	void SetLocalPosition(float x, float y);
	void SetLocalPosition(const mmlVector<2> &position);

	mmlVector<2> GetWorldPosition( void ) const;
	void SetWorldPosition(float x, float y);
	void SetWorldPosition(const mmlVector<2> &position);

	const mmlVector<2> &GetLocalAxisX( void ) const;
	const mmlVector<2> &GetLocalAxisY( void	) const;
	mmlVector<2> GetWorldAxisX( void ) const;
	mmlVector<2> GetWorldAxisY( void ) const;

	static mmlVector<2> AbsoluteUp( void );
	static mmlVector<2> AbsoluteRight( void );

	// LookAtLocal(float x, float y);
	// LookAtLocal(const mmlVector<2> &point);
	// LookAtWorld(float x, float y);
	// LookAtWorld(const mmlVector<2> &point);

	void ApplyLocalTranslation(float x, float y);
	void ApplyLocalTranslation(const mmlVector<2> &translation);

	void ApplyWorldTranslation(float x, float y);
	void ApplyWorldTranslation(const mmlVector<2> &translation);

	void ApplyRotation(float angle);
	void ApplyRotation(const mmlVector<2> &around, float angle);

	// ERROR HERE:
	// This function fundamentally transforms points differently than
	// GetWorldPosition does. I have to fix how GetWorldPosition works
	// and how the position is resolved in SetParentTransform.
	// Maybe, compose rotation+position in 3x3 matrix like in Engine.
	mmlVector<2> TransformLocalPoint(const mmlVector<2> &point) const;
	mmlVector<2> TransformLocalPoint(float x, float y) const;
	mmlVector<2> TransformWorldPoint(const mmlVector<2> &point) const;
	mmlVector<2> TransformWorldPoint(float x, float y) const;

	void Scale(float scaleFactor);
};

#endif // TRANSFORM_H
