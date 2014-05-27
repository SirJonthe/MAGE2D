#ifndef MTL_STRINGMAP_INCLUDED__
#define MTL_STRINGMAP_INCLUDED__

#include "mtlString.h"
#include "mtlBinaryTree.h"
#include "mtlPointer.h"

template < typename type_t >
class mtlStringMap
{
private:
	struct Entry
	{
		mtlString	name;
		type_t		entry;
	};
	struct HashNode
	{
		mtlHash											hash;
		mutable mtlShared< mtlList<mtlShared<Entry> > >	entries;

		bool operator<(mtlHash r) const { return hash < r; }
		bool operator==(mtlHash r) const { return hash == r; }
		bool operator<(const HashNode &r) const { return hash < r.hash; }
		bool operator==(const HashNode &r) const { return hash == r.hash; }
	};
private:
	mtlBinaryTree<HashNode> m_table;
public:
	bool CreateEntry(const mtlChars &name);
	//void CollapseEntry(const mtlChars &name);
	const type_t *GetEntry(const mtlChars &name);
	type_t *GetEntry(const mtlChars &name);
};

template < typename type_t >
bool mtlStringMap::CreateEntry(const mtlChars &name)
{
	mtlHash hash(name);
	const mtlBranch<HashNode> *b = m_table.GetRoot();
	if (b != NULL) {
		b->Find(hash);
	}
	mtlNode< mtlShared<Entry> > *n = NULL;
	if (b != NULL) {
		n = b->GetItem().entries.GetShared()->GetFirst();
		while (n != NULL) {
			if (n->GetItem().GetShared()->name.Compare(name)) {
				return false;
			}
			n = n->GetNext();
		}
	} else {
		HashNode node;
		node.hash = hash;
		b = m_table.Insert(node);
		b->GetItem().entries.New();
	}
	mtlShared<Entry> entry;
	entry.New();
	entry.GetShared()->name.Copy(name);
	b->GetItem().entries.GetShared()->AddLast(entry);
	return true;
}

template < typename type_t >
const type_t *mtlStringMap::GetEntry(const mtlChars &name) const
{
	mtlHash hash(name);
	mtlBranch<HashNode> *b = m_table.GetRoot();
	if (b == NULL) { return NULL; }

	b = b->Find(hash);
	if (b == NULL) { return NULL; }

	mtlNode< mtlShared<Entry> > *n = b->GetItem().entries.GetShared()->GetFirst();
	while (n != NULL) {
		if (n->GetItem().GetShared()->name.Compare(name)) {
			return &n->GetItem().GetShared()->entry;
		}
		n = n->GetNext();
	}
	return NULL;
}

template < typename type_t >
type_t *mtlStringMap::GetEntry(const mtlChars &name)
{
	mtlHash hash(name);
	mtlBranch<HashNode> *b = m_table.GetRoot();
	if (b == NULL) { return NULL; }

	b = b->Find(hash);
	if (b == NULL) { return NULL; }

	mtlNode< mtlShared<Entry> > *n = b->GetItem().entries.GetShared()->GetFirst();
	while (n != NULL) {
		if (n->GetItem().GetShared()->name.Compare(name)) {
			return &n->GetItem().GetShared()->entry;
		}
		n = n->GetNext();
	}
	return NULL;
}


#endif // MTL_STRINGMAP_INCLUDED__
