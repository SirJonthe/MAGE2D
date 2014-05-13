#ifndef MTL_TYPE_H_INCLUDED__
#define MTL_TYPE_H_INCLUDED__

// http://stackoverflow.com/questions/18731075/c-fast-dynamic-type-subtype-check

typedef unsigned long long TypeID;

class mtlBase
{
public:
	static TypeID GetClassType( void ) { return 0; }
	virtual TypeID GetInstanceType( void ) const { return GetClassType(); }
};

template < typename type_t >
class mtlInherit : public type_t
{
private:
	static const char m_typeAddress;
public:
	static TypeID GetClassType( void ) { return (TypeID)(&m_typeAddress); }
	virtual TypeID GetInstanceType( void ) const { return GetClassType(); }
	static TypeID GetParentClassType( void ) { return type_t::GetClassType(); }
	virtual TypeID GetParentInstanceType( void ) const { return type_t::GetInstanceType(); }
};

template < typename type_t > const char mtlInherit<type_t>::m_typeAddress = 0;

#endif
