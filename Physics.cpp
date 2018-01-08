#include "Physics.h"

float Physics::PixelsToRadians(float radius, float px) const
{
	float circumference = mmlPI * (radius + radius);
	float ang = (px / circumference) * mmlRAD_MAX;
	return ang;
}

float Physics::RadiansToPixels(float radius, float rad) const
{
	float circumference  = mmlPI * (radius + radius);
	float turns = rad / mmlRAD_MAX;
	float px = turns * circumference;
	return px;
}

void Physics::ResolveCollision(Physics &p1, Physics &p2, const CollisionInfo &c)
{
	//mmlVector<2> c1_vel = p1.GetVelocityAtPoint(c.avg_collision);
	//mmlVector<2> c2_vel = p2.GetVelocityAtPoint(c.avg_collision);
	mmlVector<2> c1_vel = p1.GetVelocity();
	mmlVector<2> c2_vel = p2.GetVelocity();
	if (mmlDot(c1_vel, c2_vel) < 0.0f) { return; } // forces already resolved, early exit to avoid componding separating forces

	//float restitution = p1.GetRestitution() * p2.GetRestitution();
	float restitution = 1.0f;
	float c1_vel1 = c1_vel.Len();
	float c2_vel1 = c2_vel.Len();


	Force f1;
	f1.origin = c.avg_collision;
	f1.force = c1_vel1 * p1.GetMass() * restitution;
	f1.direction = c1_vel1 > 0.0f ? c1_vel / c1_vel1 : mmlVector<2>(0.0f, 0.0f);

	Force f2;
	f2.origin = c.avg_collision;
	f2.force = c2_vel1 * p2.GetMass() * restitution;
	f2.direction = c2_vel1 > 0.0f ? c2_vel / c2_vel1 : mmlVector<2>(0.0f, 0.0f);

	p1.ApplyForce(f2);
	p2.ApplyForce(f1);

	// Add 4 forces
		// the force A inflicts to B
		// the counter force A inflicts to A (the "inverse" restitution)
		// the force B inflicts to A
		// the counter force B inflicts to B (the "inverse" restitution)
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
		float translation_factor = mmlAbs(mmlDot(com_col_normal, force.direction));
		m_velocity_pps += com_col_normal * force.force * translation_factor * m_inv_mass;
	}


	if (!IsLockedRotation()) {
		float rotation_factor = mmlCross2(com_col_normal, force.direction);
		m_torque_rps -= PixelsToRadians((center_of_mass - force.origin).Len(), force.force) * rotation_factor * m_inv_mass;
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
	mmlVector<2> center_of_mass = m_transform->GetPosition(Transform::Global);
	mmlVector<2> radius_vec = pt - center_of_mass;
	mmlVector<2> rot_vel = mmlTangent(mmlNormalize(-radius_vec)) * RadiansToPixels(radius_vec.Len(), m_torque_rps);
	return GetVelocity() + rot_vel;
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
	m_velocity_pps[0] = 0.0f;
	m_velocity_pps[1] = 0.0f;
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
	m_torque_rps = 0.0f;
}

void Physics::UnlockRotation( void )
{
	m_lock_rotation = false;
}

bool Physics::IsLockedRotation( void ) const
{
	return m_lock_rotation;
}
