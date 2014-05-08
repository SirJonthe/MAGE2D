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
	void New( void );
	void Delete( void );
	bool IsNull( void ) const;
	template < typename derived_t >
	static Shared<type_t> Create( void );
	static Shared<type_t> Create( void );
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
void Shared<type_t>::New( void )
{
	New<type_t>();
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

template < typename type_t >
template < typename derived_t >
Shared<type_t> Shared<type_t>::Create( void )
{
	Shared<type_t> a;
	a.New<derived_t>();
	return a;
}

template < typename type_t >
Shared<type_t> Shared<type_t>::Create( void )
{
	return Create<type_t>();
}

template < typename type_t >
class Borrowed;

template < typename type_t >
class Owned
{
	friend class Borrowed<type_t>;
private:
	Shared<type_t>							m_obj; // why a Shared? because then we can copy-construct internally without duplicating data
	Shared< mtlList< Borrowed<type_t> > >	m_tracker;
private:
	Owned(const Owned &owned);
	Owned &operator=(const Owned &owned);
public:
	Owned( void );
	~Owned( void );
	template < typename derived_t >
	void New( void );
	void New( void );
	void Delete( void );
	template < typename derived_t >
	static Owned<type_t> Create( void );
	static Owned<type_t> Create( void );
	bool IsNull( void ) const;
	type_t *operator->( void );
	const type_t *operator->( void ) const;
};

template < typename type_t >
Owned<type_t>::Owned(const Owned<type_t> &owned) : m_obj(owned.m_obj), m_tracker(owned.m_tracker)
{
}

template < typename type_t >
Owned<type_t> &Owned<type_t>::operator=(const Owned<type_t> &owned)
{
	if (&owned != this) {
		Delete();
		m_obj = owned.m_obj;
		m_tracker = owned.m_tracker;
	}
	return *this;
}

template < typename type_t >
template < typename derived_t >
void Owned<type_t>::New( void )
{
	Delete();
	m_obj.New<derived_t>();
	m_tracker.New();
}

template < typename type_t >
void Owned<type_t>::New( void )
{
	New<type_t>();
}

template < typename type_t >
void Owned<type_t>::Delete( void )
{
	m_obj.Delete();
	if (m_obj.IsNull() && !m_tracker.IsNull()) {
		mtlNode< Borrowed<type_t> > *n = m_tracker->GetFirst();
		while (n != NULL) {
			n->GetItem().m_borrowed = NULL;
			n->GetItem().m_trackerItem = NULL;
			n = n->GetNext();
		}
		m_tracker.Delete();
	}
}

template < typename type_t >
template < typename derived_t >
Owned<type_t> Owned<type_t>::Create( void )
{
	Owned<type_t> o;
	o.New<derived_t>();
	return o;
}

template < typename type_t >
Owned<type_t> Owned<type_t>::Create( void )
{
	return Create<type_t>();
}

template < typename type_t >
bool Owned<type_t>::IsNull( void ) const
{
	return m_obj.IsNull();
}

template < typename type_t >
type_t *Owned<type_t>::operator ->( void )
{
	return m_obj.operator->();
}

template < typename type_t >
const type_t *Owned<type_t>::operator ->( void ) const
{
	return m_obj.operator->();
}

template < typename type_t >
class Borrowed
{
	friend class Owned<type_t>;
private:
	type_t						*m_borrowed;
	mtlNode< Borrowed<type_t> >	*m_trackerItem;
public:
	Borrowed( void );
	Borrowed(const Borrowed &borrowed);
	Borrowed(Owned<type_t> &owned);
	Borrowed &operator=(const Borrowed &borrowed);
	Borrowed &operator=(Owned<type_t> &owned);
	~Borrowed( void );
	void Delete( void );
	bool IsNull( void ) const;
	type_t *operator->( void );
	const type_t *operator->( void ) const;
};

template < typename type_t >
Borrowed<type_t>::Borrowed( void ) : m_borrowed(NULL), m_trackerItem(NULL)
{
}

template < typename type_t >
Borrowed<type_t>::Borrowed(const Borrowed<type_t> &borrowed) : m_borrowed(NULL), m_trackerItem(NULL)
{
	if (borrowed.m_borrowed != NULL) {
		*this = *borrowed.m_borrowed;
	}
}

template < typename type_t >
Borrowed<type_t>::Borrowed(Owned<type_t> &owned) : m_borrowed(owned.m_obj.operator->()), m_trackerItem(NULL)
{
	if (!owned.IsNull()) {
		owned.m_tracker->AddLast(this);
		m_trackerItem = owned.m_tracker->GetLast();
	}
}

template < typename type_t >
Borrowed<type_t> &Borrowed<type_t>::operator=(const Borrowed<type_t> &borrowed)
{
	if (&borrowed != this) {
		Delete();
		if (borrowed.m_borrowed != NULL) {
			*this = *borrowed.m_borrowed;
		}
	}
	return *this;
}

template < typename type_t >
Borrowed<type_t> &Borrowed<type_t>::operator=(Owned<type_t> &owned)
{
	if (owned.m_obj.operator->() != m_borrowed) {
		Delete();
		m_borrowed = owned.m_obj.operator->();
		if (m_borrowed != NULL) {
			owned.m_tracker->AddLast(this);
			m_trackerItem = owned.m_tracker->GetLast();
		}
	}
}

template < typename type_t >
Borrowed<type_t>::~Borrowed( void )
{
	Delete();
}


template < typename type_t >
void Borrowed<type_t>::Delete( void )
{
	if (m_borrowed != NULL) {
		m_borrowed = NULL;
		m_trackerItem->Remove();
		m_trackerItem = NULL;
	}
}

template < typename type_t >
bool Borrowed<type_t>::IsNull( void ) const
{
	return m_borrowed != NULL;
}

template < typename type_t >
type_t *Borrowed<type_t>::operator->( void )
{
	return m_borrowed;
}

template < typename type_t >
const type_t *Borrowed<type_t>::operator->( void ) const
{
	return m_borrowed;
}

#endif // COMMON_H
