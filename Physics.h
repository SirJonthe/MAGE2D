#ifndef PHYSICS_H_INCLUDED__
#define PHYSICS_H_INCLUDED__

#include "MiniLib/MML/mmlVector.h"

#include "Collider.h"

// WORD OF WARNING: Physics work properly ONLY if object transform has same scale in X axis as in Y axis
// POSSIBLE WORK-AROUND:
	// 1) Add a pure physics object as the parent transform to the object you want to arbitrarily scale
	// 2) Scale the child object locally
	// 3) Let parent physics object keep X and Y axis scales equal in length (scaling is fine as long as len(X_axis) == len(Y_axis))

// @data Physics
// @info Physics properties of a given rigid body.
class Physics
{
public:
	// @data Force
	// @info A ray with an added component of force.
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
	// @algo ResolveCollision
	// @info Moves colliding objects apart and adds opposing forces (for bouncing).
	// @in
	//   p1 -> The first offending body.
	//   p2 -> The second offending body.
	//   c -> The collision data.
	static void ResolveCollision(Physics &p1, Physics &p2, const CollisionInfo &c);

public:
	Physics( void );

	// @algo SetTransform
	// @info Sets a new shared or unique transform.
	// @in transform -> The new shared or unique transform.
	void SetTransform(mtlShared<Transform> &transform);

	// @algo ApplyForce
	// @info Adds a force to the current physics state.
	// @in force -> The force to add.
	void ApplyForce(const Force &force);
	
	// @algo ResetForce
	// @info Resets the physics state to a zero force state.
	void ResetForce( void );

	// @algo UodatePhysics
	// @info Integrate physics simulation over a given time.
	// @in time_scale -> The time to integrate simulation over.
	void UpdatePhysics(float time_scale);

	// @algo GetVelocity
	// @out The velocity of the object at the origin.
	mmlVector<2> GetVelocity( void ) const;
	
	// @algo GetVelocityAtPoint
	// @in pt -> A point in space.
	// @out The velocity of the physics simulation at the given point.
	mmlVector<2> GetVelocityAtPoint(const mmlVector<2> &pt) const;
	
	// @algo SetVelocity
	// @info Sets the current velocity at the origin to the given velocity.
	// @in velocity_pps -> Velocity in pixels per second.
	void         SetVelocity(const mmlVector<2> &velocity_pps);
	
	// @algo AddVelocity
	// @info Adds a velocity to the velocity at the origin.
	// @in velocity_pps -> Velocity in pixels per second.
	void         AddVelocity(const mmlVector<2> &velocity_pps);
	
	// @algo ScaleVelocity
	// @info Scales current velocity by a given factor.
	// @in factor -> The factor by which the current velocity is scaled.
	void         ScaleVelocity(float factor);
	
	// @algo GetTorque
	// @out The torque (in pixels per second).
	float        GetTorque( void ) const;
	
	// @algo SetTorque
	// @info Sets the torque.
	// @in torque_pps -> The new torque in pixels per second.
	void         SetTorque(float torque_rps);
	
	// @algo AddTorque
	// @info Adds torque to current torque.
	// @in torque_pps -> Torque to add in pixels per second.
	void         AddTorque(float torque_rps);
	
	// @algo ScaleTorque
	// @info Scales current torque by a given factor.
	// @in factor -> The factor by which to scale the current torque.
	void         ScaleTorque(float factor);
	
	// @algo GetForceAtPoint
	// @in pt -> A point in space.
	// @out The force at the given point in space.
	float        GetForceAtPoint(const mmlVector<2> &pt) const;

	// @algo SetMass
	// @in mass -> New mass.
	void  SetMass(float mass);
	
	// @algo GetMass
	// @out Current mass.
	float GetMass( void ) const;

	// @algo SetRestitution
	// @in restitution -> The new restitution (0-1).
	void  SetRestitution(float restitution);
	
	// @algo GetRestitution
	// @out The amount by which a force added to the object results in kinetic energy rather than dissipation.
	float GetRestitution( void ) const;

	// @algo LockPosition
	// @info Fixes position in place around origin.
	void LockPosition( void );
	
	// @algo UnlockPosition
	// @info Unlocks position
	void UnlockPosition( void );
	
	// @algo IsLockedPosition
	// @out TRUE if position is locked.
	bool IsLockedPosition( void ) const;

	// @algo LockRotation
	// @info Locks rotation.
	void LockRotation( void );
	
	// @algo UnlockRotation
	// @info Unlocks rotation.
	void UnlockRotation( void );
	
	// @algo IsLockedRotation
	// @out TRUE if rotation is locked.
	bool IsLockedRotation( void ) const;
};

#endif // PHYSICS_H_INCLUDED__

