#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <cstddef>
#include "MML/mmlVector.h"

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

// http://stackoverflow.com/questions/18731075/c-fast-dynamic-type-subtype-check

typedef unsigned long long TypeID;

class Base
{
public:
	static TypeID GetClassType( void ) { return 0; }
	virtual TypeID GetInstanceType( void ) const { return GetClassType(); }
};

template < typename type_t >
class Inherit : public type_t
{
private:
	static const char m_typeAddress;
public:
	static TypeID GetClassType( void ) { return (TypeID)(&m_typeAddress); }
	virtual TypeID GetInstanceType( void ) const { return GetClassType(); }
	static TypeID GetParentClassType( void ) { return type_t::GetClassType(); }
	virtual TypeID GetParentInstanceType( void ) const { return type_t::GetInstanceType(); }
};

template < typename type_t > const char Inherit<type_t>::m_typeAddress = 0;

// References are made invalid automatically when what it references to is destroyed
/*template < typename type_t >
class Reference
{
};*/

struct SharedReference
{
	void	*m_obj;
	int		m_count;
};

// Resources are reference counted objects
template < typename type_t >
class Shared
{
private:
	SharedReference *m_ref;
public:
	Shared( void );
	~Shared( void );
	template < typename derived_t >
	Shared(const Shared<derived_t> &shared)
	{
		// compiler will type check for compatibility
		// optimizer will remove if type check passes
		const type_t *err1 = NULL;
		const derived_t *err2 = NULL;
		err1 = err2;

		if (!shared.IsNull()) {
			m_ref = shared.m_ref;
			++m_ref->m_count;
		}
	}
	template < typename derived_t >
	Shared &operator=(const Shared<derived_t> &shared)
	{
		// compiler will type check for compatibility
		// optimizer will remove if type check passes
		const type_t *err1 = NULL;
		const derived_t *err2 = NULL;
		err1 = err2;

		if (m_ref != shared.m_ref) {
			Delete();
			m_ref = shared.m_ref;
			if (!IsNull()) { ++m_ref->m_count; }
		}
		return *this;
	}
	type_t *operator->( void );
	const type_t *operator->( void ) const;
	template < typename derived_t >
	void New( void );
	void Delete( void );
	bool IsNull( void ) const;
	template < typename derived_t >
	static Shared<type_t> Create( void )
	{
		Shared<type_t> a;
		a.New<derived_t>();
		return a;
	}
};

template < typename type_t >
Shared<type_t>::Shared( void ) : m_ref(NULL)
{
}

template < typename type_t >
Shared<type_t>::~Shared( void )
{
	Delete();
}

template < typename type_t >
type_t *Shared<type_t>::operator ->( void )
{
	return (type_t*)m_ref->m_obj;
}

template < typename type_t >
const type_t *Shared<type_t>::operator ->( void ) const
{
	return (const type_t*)m_ref->m_obj;
}

template < typename type_t >
template < typename derived_t >
void Shared<type_t>::New( void )
{
	Delete();
	m_ref = new SharedReference;
	m_ref->m_obj = new derived_t;
	m_ref->m_count = 1;
}

template < typename type_t >
void Shared<type_t>::Delete( void )
{
	if (!IsNull()) {
		--m_ref->m_count;
		if (m_ref->m_count == 0) {
			type_t *obj = (type_t*)m_ref->m_obj; // don't delete void pointer (will not call destructor)
			delete obj;
			delete m_ref;
		}
	}
	m_ref = NULL;
}

template < typename type_t >
bool Shared<type_t>::IsNull( void ) const
{
	return m_ref == NULL;
}

#endif // COMMON_H
