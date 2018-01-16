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
	// CollisionInfo needs to generate more useful data. Averages only seem to produce a spherical repulsion effect...
	// check this out: http://www.dyn4j.org/2011/11/contact-points-using-clipping/

	mmlVector<2> c1_vel = p1.GetVelocityAtPoint(c.avg_collision);
	mmlVector<2> c2_vel = p2.GetVelocityAtPoint(c.avg_collision);

	float        restitution = p1.GetRestitution() * p2.GetRestitution();
	mmlVector<2> sum_vec = (c1_vel * p1.GetMass() - c2_vel * p2.GetMass()) * restitution;
	float        sum_vec_len = sum_vec.Len();
	float        inv_tot_mass = 1.0f / (p1.GetMass() + p2.GetMass());

	Force force;
	force.origin    = c.avg_collision;
	force.force     = sum_vec_len * p1.GetMass() * inv_tot_mass;
	force.direction = sum_vec_len > 0.0f ? sum_vec / sum_vec_len : mmlVector<2>(0.0f, 0.0f);
	p2.ApplyForce(force);

	force.force     = sum_vec_len * p2.GetMass() * inv_tot_mass;
	force.direction = -force.direction;
	p1.ApplyForce(force);
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

	//mmlVector<2> start_force = GetVelocityAtPoint(force.origin);

	if (!IsLockedPosition()) {
		float translation_factor = mmlAbs(mmlDot(com_col_normal, force.direction));
		m_velocity_pps += com_col_normal * force.force * m_inv_mass * translation_factor;
	}


	if (!IsLockedRotation()) {
		float rotation_factor = mmlCross2(com_col_normal, force.direction);
		m_torque_rps -= PixelsToRadians((center_of_mass - force.origin).Len(), force.force * m_inv_mass) * rotation_factor;
	}

	//mmlVector<2> end_force = GetVelocityAtPoint(force.origin);
	//float force_tot = (end_force - start_force).Len();
	//std::cout << "force ratio=" << force_tot << "/" << force.force << "=" << force_tot / force.force << " (should at max be 1)" << std::endl;
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
	float radius = radius_vec.Len();
	mmlVector<2> rot_vel = radius > 0.0f ? mmlTangent(radius_vec / radius) * RadiansToPixels(radius, m_torque_rps) : mmlVector<2>(0.0f, 0.0f);
	return GetVelocity() + rot_vel;
}

void Physics::SetVelocity(const mmlVector<2> &velocity_pps)
{
	if (!IsLockedPosition()) {
		m_velocity_pps = velocity_pps;
	}
}

void Physics::AddVelocity(const mmlVector<2> &velocity_pps)
{
	SetVelocity(m_velocity_pps + velocity_pps);
}

void Physics::ScaleVelocity(float factor)
{
	m_velocity_pps *= factor;
}

float Physics::GetTorque( void ) const
{
	return !IsLockedRotation() ? m_torque_rps : 0.0f;
}

void Physics::SetTorque(float torque_rps)
{
	if (!IsLockedRotation()) {
		m_torque_rps = torque_rps;
	}
}

void Physics::AddTorque(float torque_rps)
{
	SetTorque(m_torque_rps + torque_rps);
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

void Physics::SetRestitution(float restitution)
{
	m_restitution = mmlClamp(0.0f, restitution, 1.0f);
}

float Physics::GetRestitution( void ) const
{
	return m_restitution;
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
