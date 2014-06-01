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
		typedef mtlList< mtlShared<Entry> > List;
		typedef mtlNode< mtlShared<Entry> > ListNode;
		mtlHash					hash;
		mutable mtlShared<List>	entries;

		bool operator<(mtlHash r) const { return hash < r; }
		bool operator==(mtlHash r) const { return hash == r; }
		bool operator<(const HashNode &r) const { return hash < r.hash; }
		bool operator==(const HashNode &r) const { return hash == r.hash; }
	};
private:
	mtlBinaryTree<HashNode> m_table;
public:
	mtlBranch<HashNode> *GetNode(const mtlChars &name) const;
public:
	bool			CreateEntry(const mtlChars &name);
	void			RemoveEntry(const mtlChars &name);
	void			RemoveAll( void );
	const type_t	*GetEntry(const mtlChars &name) const;
	type_t			*GetEntry(const mtlChars &name);
};

template < typename type_t >
mtlBranch<mtlStringMap::HashNode> *mtlStringMap::GetNode(const mtlChars &name) const
{
	return m_table.GetRoot() != NULL ? m_table.GetRoot()->Find(mtlHash(name)) : NULL;
}

template < typename type_t >
bool mtlStringMap<type_t>::CreateEntry(const mtlChars &name)
{
	mtlHash hash(name);
	const mtlBranch<HashNode> *b = m_table.GetRoot();
	if (b != NULL) {
		b->Find(hash);
	}
	HashNode::ListNode *n = NULL;
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
void mtlStringMap<type_t>::RemoveEntry(const mtlChars &name)
{
	mtlBranch<HashNode> *hashNode = GetNode(name);
	const HashNode::ListNode *node = hashNode->GetItem()->entries.GetShared()->GetFirst();
	while (node != NULL) {
		if (node->GetItem().GetShared()->name.Compare(name)) {
			node->Remove();
			break;
		}
		node = node->GetNext();
	}
	if (hashNode->GetItem()->entries.GetShared()->GetSize() == 0) {
		hashNode->Remove();
	}
}

template < typename type_t >
void mtlStringMap<type_t>::RemoveAll( void )
{
	m_table.RemoveAll();
}

template < typename type_t >
const type_t *mtlStringMap<type_t>::GetEntry(const mtlChars &name) const
{
	mtlBranch<HashNode> *b = GetNode(name);
	HashNode::ListNode *n = b->GetItem().entries.GetShared()->GetFirst();
	while (n != NULL) {
		if (n->GetItem().GetShared()->name.Compare(name)) {
			return &n->GetItem().GetShared()->entry;
		}
		n = n->GetNext();
	}
	return NULL;
}

template < typename type_t >
type_t *mtlStringMap<type_t>::GetEntry(const mtlChars &name)
{
	mtlBranch<HashNode> *b = GetNode(name);
	HashNode::ListNode *n = b->GetItem().entries.GetShared()->GetFirst();
	while (n != NULL) {
		if (n->GetItem().GetShared()->name.Compare(name)) {
			return &n->GetItem().GetShared()->entry;
		}
		n = n->GetNext();
	}
	return NULL;
}


#endif // MTL_STRINGMAP_INCLUDED__
