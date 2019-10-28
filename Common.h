#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <cstddef>
#include "MiniLib/MML/mmlVector.h"
#include "MiniLib/MTL/mtlList.h"
#include "MiniLib/MTL/mtlArray.h"
#include "MiniLib/MTL/mtlList.h"

// @data ShallowArray
// @info A generic array that can be used as return values without copying (in lieu of move semantics).
#define ShallowArray(type_t) mtlShared< mtlArray<type_t> >

// @data ShallowList
// @info A generic list that can be used as return values without copying (in lieu of move semantics).
#define ShallowList(type_t) mtlShared< mtlList<type_t> >

// @data DeepArray
// @info A generic array that can be copied.
template < typename type_t >
class DeepArray : public mtlArray<type_t>
{
public:
	DeepArray( void ) : mtlArray<type_t>() {}
	DeepArray(int size) : mtlArray<type_t>(size) {}
	DeepArray(const DeepArray &arr) : mtlArray<type_t>()
	{
		this->Copy(arr);
	}
	DeepArray &operator=(const DeepArray &arr)
	{
		if (this != &arr) {
			this->Copy(arr);
		}
		return *this;
	}
};

// @data DeepList
// @info A generic list that can be copied.
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

// @data Point
// @info Description of a point in 2D space.
struct Point
{
	int x, y;
};

// @data Rect
// @info Description of a rectangle in 2D space.
struct Rect
{
	int x, y;
	int w, h;

	// @algo GetCenterPoint
	// @out The center point of the rectangle.
	Point GetCenterPoint( void ) const { Point p = { x + (w>>1), y + (h>>1) }; return p; }
	
	// @algo GetCenterVector
	// @info The center vector (float, float) of the rectangle.
	mmlVector<2> GetCenterVector( void ) const { return mmlVector<2>(float(x) + float(w)/2.0f, float(y) + float(h)/2.0f); }
};

// @algo Cross2D
// @in a, b -> The vectors of which the cross product is sought.
// @out Returns the "cross product" in 2 dimensions.
inline float Cross2D(mmlVector<2> a, mmlVector<2> b)
{
	 return a[0]*b[1] - a[1]*b[0];
}

// @algo LineTangent
// @in l1, l2 -> The vectors that form an unbounded line.
// @out The tangent of the line.
inline mmlVector<2> LineTangent(mmlVector<2> l1, mmlVector<2> l2)
{
	return mmlNormalize(l2 - l1);
}

// @algo LineNormal
// @in l1, l2 -> The vectors that form an unbounded line.
// @out The normal of the line
inline mmlVector<2> LineNormal(mmlVector<2> l1, mmlVector<2> l2)
{
	mmlVector<2> t = LineTangent(l1, l2);
	return mmlVector<2>(-t[1], t[0]);
}

// @data flags_t
// @info Type for storing bit flags.
typedef unsigned long long int flags_t;

// @data AllFlagsOn
// @info Value representing all flags 1.
#define AllFlagsOn 0xffffffffffffffff

#endif // COMMON_H
