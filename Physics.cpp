#include "Physics.h"

float Physics::PixelsToRadians(const mmlVector<2> &center_pt, const mmlVector<2> &outer_pt, float px) const
{
	float radius = (outer_pt - center_pt).Len();
	float circumference = mmlPI * (radius + radius);
	float ang = (px / circumference) * mmlRAD_MAX;
//	float ang = acos((2.0f*radius*radius - px*px) / (2.0f*radius*radius));
	return ang;
}

void Physics::ResolveCollision(Physics &p1, Physics &p2, const CollisionInfo &c)
{
//	std::cout << "Resolving" << std::endl;

	mmlVector<2> c1_vel = p1.GetVelocityAtPoint(c.c1.avg_collision);
	mmlVector<2> c2_vel = p2.GetVelocityAtPoint(c.c2.avg_collision);
	if (mmlDot(c1_vel, c2_vel) < 0.0f) { return; } // forces already resolved, early exit to avoid componding separating forces

	float c1_force = c1_vel.Len() * p1.GetMass();
	float c2_force = c2_vel.Len() * p2.GetMass();

	// direction of the bounce
		// the reflecting vector based on both of the velocity vectors and the average surface normal of the intersection collisions per object
	// Not sure if this is realistic enough:

	Force f1;
	f1.origin = c.c2.avg_collision;
	f1.direction = mmlNormalize(c1_vel - c2_vel);
	f1.force = c1_force;
	Force f2;
	f2.origin = c.c1.avg_collision;
	f2.direction = -f1.direction;
	f2.force = c2_force;

	p1.ApplyForce(f2);
	p2.ApplyForce(f1);
}

Physics::Physics( void ) :
	m_transform(),
	m_velocity_pps(0.0f, 0.0f),
	m_torque_rps(0.0f),
	m_fric_s(0.0f),
	m_fric_d(0.0f),
	m_restitution(1.0f),
	m_inv_mass(1.0f),
	m_mass(1.0f),
	m_lock_position(false),
	m_lock_rotation(false)
{}

void Physics::SetTransform(mtlShared<Transform> &transform)
{
	m_transform = transform;
}

void Physics::ApplyForce(const Physics::Force &force)
{
	// This function applies a translational force and a rotational force to a physics object
	// Note: It takes no account of the geometry of the physics object. However for best effect, it should. Do this by intersecting the ray with the collider, and then using the resulting intersection as the ray for this function.

	if (m_transform.GetShared() == NULL) { return; }

	mmlVector<2> center_of_mass = m_transform->GetPosition(Transform::Global); // just assume that center of mass = object position
	mmlVector<2> com_col_normal = mmlNormalize(center_of_mass - force.origin); // the normal between the center of mass and the collision point

	if (!IsLockedPosition()) {
		float translation_factor = mmlDot(com_col_normal, force.direction);
		m_velocity_pps += com_col_normal * force.force * translation_factor * m_inv_mass;
	}


	if (!IsLockedRotation()) {
		float rotation_factor = mmlCross2(com_col_normal, force.direction);
		m_torque_rps += PixelsToRadians(center_of_mass, force.origin, force.force) * rotation_factor * m_inv_mass;
	}
}

void Physics::ResetForce( void )
{
	m_velocity_pps[0] = 0.0f;
	m_velocity_pps[1] = 0.0f;
	m_torque_rps = 0.0f;
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

mmlVector<2> Physics::GetVelocity( void ) const
{
	return !IsLockedPosition() ? m_velocity_pps : mmlVector<2>(0.0f, 0.0f);
}

mmlVector<2> Physics::GetVelocityAtPoint(const mmlVector<2> &pt) const
{
	mmlVector<2> rel_pt1 = pt - (m_transform.GetShared() != NULL ? m_transform->GetPosition(Transform::Global) : mmlVector<2>(0.0f, 0.0f));
	mmlVector<2> rel_pt2 = rel_pt1 * mml2DRotationMatrix(GetTorque()); // This will fail to give correct result if object transform does not have same X axis scale as Y axis
	return GetVelocity() + (rel_pt2 - rel_pt1);
}

void Physics::ScaleVelocity(float factor)
{
	m_velocity_pps *= factor;
}

float Physics::GetTorque( void ) const
{
	return !IsLockedRotation() ? m_torque_rps : 0.0f;
}

void Physics::ScaleTorque(float factor)
{
	m_torque_rps *= factor;
}

float Physics::GetForceAtPoint(const mmlVector<2> &pt) const
{
	// Approximation, does not take into account the circular movement of rotation, which means the system loses some force when using this function
	return GetVelocityAtPoint(pt).Len() * m_mass;
}

void Physics::SetMass(float mass)
{
	m_mass = mmlMax(0.0f, mass);
	m_inv_mass = m_mass > 0.0f ? 1.0f / m_mass : 0.0f;
}

float Physics::GetMass( void ) const
{
	return m_mass;
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
