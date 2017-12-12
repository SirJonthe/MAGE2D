#ifndef PHYSICS_H_INCLUDED__
#define PHYSICS_H_INCLUDED__

#include "MiniLib/MML/mmlVector.h"

#include "Collider.h"

class Physics
{
private:
	mtlShared<Transform> m_transform;
	mmlVector<2>         m_velocity_pps; // pps = pixels per second
	float                m_torque_rps; // rps = radians per second
	float                m_fric_s;
	float                m_fric_d;
	float                m_restitution;
	float                m_inv_mass;
	bool                 m_lock_position;
	bool                 m_lock_rotation;

private:
	float PixelsToRadians(const mmlVector<2> &center, const mmlVector<2> &outer_pt, float px) const;

public:
	static void ResolveCollision(Physics &p1, Physics &p2, const CollisionInfo &c);

public:
	Physics( void );

	void SetTransform(mtlShared<Transform> &transform);
	void ResetTransform( void );

	void ApplyForce(const Ray &ray);
	void UpdatePhysics(float time_scale);

	void  SetMass(float mass);
	float GetMass( void ) const;

	void LockPosition( void );
	void UnlockPosition( void );
	bool IsLockedPosition( void ) const;

	void LockRotation( void );
	void UnlockRotation( void );
	bool IsLockedRotation( void ) const;
};

#endif // PHYSICS_H_INCLUDED__

