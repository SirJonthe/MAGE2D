#ifndef PHYSICS_H_INCLUDED__
#define PHYSICS_H_INCLUDED__

#include "MiniLib/MML/mmlVector.h"

#include "Collider.h"

// WORD OF WARNING: Physics work properly ONLY if object transform has same scale in X axis as in Y axis
// POSSIBLE WORK-AROUND:
	// 1) Add a pure physics object as the parent transform to the object you want to arbitrarily scale
	// 2) Scale the child object locally
	// 3) Let parent physics object keep X and Y axis scales equal in length (scaling is fine as long as len(X_axis) == len(Y_axis))

class Physics
{
public:
	struct Force : public Ray
	{
		float force;
	};

private:
	mtlShared<Transform> m_transform;
	mmlVector<2>         m_velocity_pps; // pps = pixels per second
	float                m_torque_rps; // rps = radians per second
	float                m_fric_s;
	float                m_fric_d;
	float                m_restitution;
	float                m_inv_mass;
	float                m_mass;
	bool                 m_lock_position;
	bool                 m_lock_rotation;

private:
	float PixelsToRadians(float radius, float px) const;
	float RadiansToPixels(float radius, float rad) const;

public:
	static void ResolveCollision(Physics &p1, Physics &p2, const CollisionInfo &c);

public:
	Physics( void );

	void SetTransform(mtlShared<Transform> &transform);

	void ApplyForce(const Force &force);
	void ResetForce( void );

	void UpdatePhysics(float time_scale);

	mmlVector<2> GetVelocity( void ) const;
	mmlVector<2> GetVelocityAtPoint(const mmlVector<2> &pt) const;
	void         ScaleVelocity(float factor);
	float        GetTorque( void ) const;
	void         ScaleTorque(float factor);
	float        GetForceAtPoint(const mmlVector<2> &pt) const;

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

