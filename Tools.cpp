#include "Tools.h"

void KillPlane::OnCollision(Object &collider)
{
	if (collider.GetInstanceType() != KillPlane::GetClassType()) { // planes almost always collide, and we don't want to destroy our kill planes
		collider.Destroy();
	}
}

KillPlane::KillPlane( void ) : Inherit<Object>()
{
	ClearAllTypeFlags();
	SetName("tool_killplane");
	// LoadCollider<PlaneCollider>();
	// Plane *p = GetCollider<PlaneCollider>();
}
