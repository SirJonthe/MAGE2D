#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <cstddef>
#include "MML/mmlVector.h"
#include "MTL/mtlList.h"
#include "MTL/mtlArray.h"
#include "MTL/mtlList.h"

#define ShallowArray(type_t) mtlShared< mtlArray<type_t> >
#define ShallowList(type_t) mtlShared< mtlList<type_t> >

template < typename type_t >
class DeepArray : public mtlArray<type_t>
{
public:
	DeepArray( void ) : mtlArray<type_t>() {}
	DeepArray(int size) : mtlArray<type_t>(size) {}
	DeepArray(const DeepArray &arr) : mtlArray<type_t>()
	{
		Copy(arr);
	}
	DeepArray &operator=(const DeepArray &arr)
	{
		if (this != &arr) {
			Copy(arr);
		}
		return *this;
	}
};

template < typename type_t >
class DeepList : public mtlList<type_t>
{
public:
	DeepList( void ) : mtlList<type_t>() {}
	DeepList(const DeepList &list) : mtlList<type_t>()
	{
		Copy(list);
	}
	DeepList &operator=(const DeepList &list)
	{
		if (this != &list) {
			Copy(list);
		}
		return *this;
	}
};

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

typedef unsigned long long int flags_t;
#define AllFlagsOn 0xffffffffffffffff

#endif // COMMON_H
