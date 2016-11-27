#ifndef PHYSICS_H_INCLUDED__
#define PHYSICS_H_INCLUDED__

#include "MiniLib/MML/mmlVector.h"

class Physics
{
private:
	mmlVector<2> m_center;
	float        m_weight;
	mmlVector<2> m_dir_momentum;
	float        m_ang_momentum;
	bool         m_gravitational_pull;
};

#endif // PHYSICS_H_INCLUDED__

