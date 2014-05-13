#include "Tools.h"
#include "Engine.h"

ENGINE_REGISTER_OBJECT_TYPE(KillPlane);

void KillPlane::OnCollision(Object &collider)
{
	if (collider.GetInstanceType() != KillPlane::GetClassType()) { // planes almost always collide, and we don't want to destroy our kill planes
		collider.Destroy();
	}
}

KillPlane::KillPlane( void ) : mtlInherit<Object>()
{
	ClearAllObjectFlags(); // other objects can not collide with us, *we* collide with *them*
	SetName("tool_killplane");
	// LoadCollider<PlaneCollider>();
	// Plane *p = GetCollider<PlaneCollider>();
}
