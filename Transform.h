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
public:
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

	const mmlVector<2> &GetLocalAxisX( void ) const;
	const mmlVector<2> &GetLocalAxisY( void	) const;

	void ApplyLocalTranslation(float x, float y);
	void ApplyLocalTranslation(const mmlVector<2> &translation);
	void ApplyLocalRotation(float angle);
};

#endif // TRANSFORM_H
