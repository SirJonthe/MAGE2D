#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "MML/mmlMatrix.h"
#include "MTL/mtlString.h"

// POTENTIAL ERRORS:
// Cross products need flipped z axis (0,0,1)
// transforms need to be applied in reverse order (parent * child)

class Transform
{
private:
	const Transform	*m_parent;
	mmlMatrix<3,3>	m_transform;
	mtlString		m_name;
private:
	static mmlMatrix<3,3> GetRotationTransform(float angle, mmlVector<2> around);
	static mmlMatrix<2,2> GetRotationMatrix(float angle);
public:
	Transform( void );
	Transform(const Transform &transform);
	Transform(mmlVector<2> position, float rotation, mmlVector<2> around, Transform *parent = NULL, bool preserveTransform = true);
	Transform &operator=(const Transform &transform);

	const mtlString &GetName( void ) const;
	void SetName(const mtlChars &name);

	mmlMatrix<3,3> GetLocalTransform( void ) const;
	mmlMatrix<3,3> GetWorldTransform( void ) const;
	void SetLocalTransform(mmlMatrix<3,3> transform);
	void SetWorldTransform(mmlMatrix<3,3> transform);

	mmlVector<2> GetLocalPosition( void ) const;
	mmlVector<2> GetWorldPosition( void ) const;
	void SetLocalPosition(mmlVector<2> position);
	void SetLocalPosition(float x, float y);
	void SetWorldPosition(mmlVector<2> position);
	void SetWorldPosition(float x, float y);

	mmlMatrix<2,2> GetLocalRotation( void ) const;
	mmlMatrix<2,2> GetWorldRotation( void ) const;
	void SetLocalRotation(mmlMatrix<2,2> rotation);
	void SetLocalRotation(float angle, mmlVector<2> around);
	void SetLocalRotation(float angle);
	void SetWorldRotation(mmlMatrix<2,2> rotation);
	void SetWorldRotation(float angle, mmlVector<2> around);
	void SetWorldRotation(float angle);

	const Transform *GetParentTransform( void ) const;
	void SetParentTransform(const Transform *parent, bool preserveTransform = true);

	static mmlVector<2> GetAbsoluteAxisX( void );
	static mmlVector<2> GetAbsoluteAxisY( void );

	mmlVector<2> GetLocalAxisX( void ) const;
	mmlVector<2> GetWorldAxisX( void ) const;
	void SetLocalAxisX(mmlVector<2> x);
	void SetLocalAxisX(float x, float y);
	void SetWorldAxisX(mmlVector<2> x);
	void SetWorldAxisX(float x, float y);
	void LocalAxisXLookAt(mmlVector<2> point);
	void LocalAxisXLookAt(float x, float y);
	void WorldAxisXLookAt(mmlVector<2> point);
	void WorldAxisXLookAt(float x, float y);

	mmlVector<2> GetLocalAxisY( void ) const;
	mmlVector<2> GetWorldAxisY( void ) const;
	void SetLocalAxisY(mmlVector<2> y);
	void SetLocalAxisY(float x, float y);
	void SetWorldAxisY(mmlVector<2> y);
	void SetWorldAxisY(float x, float y);
	void LocalAxisYLookAt(mmlVector<2> point);
	void LocalAxisYLookAt(float x, float y);
	void WorldAxisYLookAt(mmlVector<2> point);
	void WorldAxisYLookAt(float x, float y);

	/*int GetAxisXDirection( void ) const;
	void SetAxisXDirection(int x);
	void FlipAxisXDirection( void );

	int GetAxisYDirection( void ) const;
	void SetAxisYDirection(int y);
	void FlipAxisYDirection( void );*/

	void ApplyLocalTranslation(mmlVector<2> vector);
	void ApplyLocalTranslation(float x, float y);
	void ApplyWorldTranslation(mmlVector<2> vector);
	void ApplyWorldTranslation(float x, float y);
	void ApplyLocalRotation(mmlMatrix<2,2> rotation);
	void ApplyLocalRotation(float angle, mmlVector<2> around);
	void ApplyLocalRotation(float angle);
	void ApplyWorldRotation(mmlMatrix<2,2> rotation);
	void ApplyWorldRotation(float angle, mmlVector<2> around);
	void ApplyWorldRotation(float angle);
	void ApplyLocalTransform(Transform transform);
	void ApplyWorldTransform(Transform transform);
};

#endif // TRANSFORM_H
