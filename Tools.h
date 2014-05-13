#ifndef TOOLS_H
#define TOOLS_H

#include "Object.h"
#include "Common.h"
#include "MTL/mtlDuplex.h"

class KillPlane : public mtlInherit<Object>
{
protected:
	void OnCollision(Object &collider);
public:
	KillPlane( void ); // grab all collisions, is defined as nothing, set collision shape to plane
};

#endif // TOOLS_H
