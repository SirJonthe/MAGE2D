#include "Physics.h"

float Physics::PixelsToRadians(const mmlVector<2> &center_pt, const mmlVector<2> &outer_pt, float px) const
{
	// the distance in pixels is the partial circumference of the circle portion
	// find the radian that corresponds to that circumference given
	// https://math.stackexchange.com/questions/185829/how-do-you-find-an-angle-between-two-points-on-the-edge-of-a-circle

	float radius = (outer_pt - center_pt).Len();
	float ang = acos((2.0f*radius*radius - px*px) / (2.0f*radius*radius));
	//return px > 0.0f ? ang : -ang;
	return ang;
}

void Physics::ResolveCollision(Physics &p1, Physics &p2, const CollisionInfo &c)
{
	// early exit if the two have velcity directions moving away from each other
	// NOTE: this is not the full truth, objects that move away from each other can still collide if protruding geometry collides via spinning
	if (mmlDot(p1.m_velocity_pps, p2.m_velocity_pps) < 0.0f) { return; }

	// the total force is the length of the contributed by an object is
		// float force_pps = Length(velocity_pps) + torque_pps

	// direction of the bounce
		// the reflecting vector based on both of the velocity vectors and the average surface normal of the intersection collisions per object

	// p1.ApplyForce(r2, f2);
	// p2.ApplyForce(r1, f1);
}

Physics::Physics( void ) :
	m_transform(),
	m_velocity_pps(0.0f, 0.0f),
	m_torque_rps(0.0f),
	m_fric_s(0.0f),
	m_fric_d(0.0f),
	m_restitution(1.0f),
	m_inv_mass(1.0f),
	m_lock_position(false),
	m_lock_rotation(false)
{}

void Physics::SetTransform(mtlShared<Transform> &transform)
{
	m_transform = transform;
}

void Physics::ResetTransform( void )
{
	m_velocity_pps[0] = 0.0f;
	m_velocity_pps[1] = 0.0f;
	m_torque_rps = 0.0f;
}

void Physics::ApplyForce(const Physics::Force &force)
{
	// This function applies a translational force and a rotational force to a physics object
	// Note: It takes no account of the geometry of the physics object. However for best effect, it should. Do this by intersecting the ray with the collider, and then using the resulting intersection as the ray for this function.

	if (m_transform.GetShared() == NULL) { return; }

	mmlVector<2> center_of_mass = m_transform->GetPosition(Transform::Global); // just assume that center of mass = object position
	mmlVector<2> com_col_normal = mmlNormalize(center_of_mass - force.origin); // the normal between the center of mass and the collision point

	float translation_factor = mmlDot(com_col_normal, force.direction);
	if (!IsLockedPosition()) {
		m_velocity_pps += com_col_normal * force.force * translation_factor * m_inv_mass;
	}

	float rotation_factor = mmlCross2(com_col_normal, force.direction);
	if (!IsLockedRotation()) {
		m_torque_rps += PixelsToRadians(center_of_mass, force.origin, force.force) * rotation_factor * m_inv_mass;
	}
}

void Physics::UpdatePhysics(float time_scale)
{
	if (m_transform.GetShared() == NULL) { return; }

	if (!IsLockedPosition()) {
		m_transform->Translate(Transform::Global, m_velocity_pps * time_scale);
	}
	if (!IsLockedRotation()) {
		m_transform->Rotate(m_torque_rps * time_scale);
	}
}

void Physics::ScaleTorque(float factor)
{
	m_torque_rps *= factor;
}

void Physics::ScaleVelocity(float factor)
{
	m_velocity_pps *= factor;
}

void Physics::SetMass(float mass)
{
	if (mass = 0.0f) {
		m_inv_mass = 0.0f;
	} else {
		m_inv_mass = mass;
	}
}

float Physics::GetMass( void ) const
{
	if (m_inv_mass == 0.0f) { return m_inv_mass; }
	return 1.0f / m_inv_mass;
}

void Physics::LockPosition( void )
{
	m_lock_position = true;
}

void Physics::UnlockPosition( void )
{
	m_lock_position = false;
}

bool Physics::IsLockedPosition( void ) const
{
	return m_lock_position;
}

void Physics::LockRotation( void )
{
	m_lock_rotation = true;
}

void Physics::UnlockRotation( void )
{
	m_lock_rotation = false;
}

bool Physics::IsLockedRotation( void ) const
{
	return m_lock_rotation;
}
