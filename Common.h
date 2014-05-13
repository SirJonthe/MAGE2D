#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <cstddef>
#include "MML/mmlVector.h"
#include "MTL/mtlList.h"

struct Point
{
	int x, y;
};

struct Rect
{
	int x, y;
	int w, h;

	Point GetCenterPoint( void ) const { Point p = { x + (w>>1), y + (h>>1) }; return p; }
	mmlVector<2> GetCenterVector( void ) const { return mmlVector<2>(float(x) + float(w)/2.0f, float(y) + float(h)/2.0f); }
};

inline float Cross2D(mmlVector<2> a, mmlVector<2> b)
{
	 return a[0]*b[1] - a[1]*b[0];
}

inline mmlVector<2> LineTangent(mmlVector<2> l1, mmlVector<2> l2)
{
	return mmlNormalize(l2 - l1);
}

inline mmlVector<2> LineNormal(mmlVector<2> l1, mmlVector<2> l2)
{
	mmlVector<2> t = LineTangent(l1, l2);
	return mmlVector<2>(-t[1], t[0]);
}

#endif // COMMON_H
