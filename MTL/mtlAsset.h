#ifndef MTL_ASSET_H_INCLUDED__
#define MTL_ASSET_H_INCLUDED__

#include "mtlBinaryTree.h"
#include "mtlList.h"
#include "mtlDirectory.h"
#include "mtlPointer.h"

class mtlAssetInterface
{
protected:
	mtlString m_error;
public:
	virtual			~mtlAssetInterface( void ) {}
	virtual bool	Load(const mtlDirectory &p_filename) = 0;
	const mtlString	&GetError( void ) const { return m_error; }
};

template < typename type_t >
class mtlAsset
{
private:
	struct Instance
	{
		type_t			*asset;
		mtlDirectory	file;
		Instance( void ) : asset(NULL), file("") {}
		~Instance( void ) { delete asset; }
	};
	struct AssetNode // store this at every node
	{
		mtlHash												hash;
		mutable mtlShared< mtlList< mtlShared<Instance> > >	assets; // all assets with same hash
		bool operator>(const mtlHash &h) const { return hash.value > h.value; }
		bool operator==(const mtlHash &h) const { return hash.value == h.value; }
		bool operator>(const AssetNode &n) const { return hash.value > n.hash.value; }
		bool operator==(const AssetNode &n) const { return hash.value == n.hash.value; }
	};
	static mtlBinaryTree<AssetNode> &GetAssetBank( void );
private:
	mtlShared<Instance>	m_ref;
public:
	mtlAsset( void ) {}
	mtlAsset(const mtlAsset<type_t> &asset);
	mtlAsset<type_t> &operator=(const mtlAsset<type_t> &asset);

	const type_t *GetAsset( void ) const;

	template < typename derived_t >
	static mtlAsset<type_t> Load(const mtlChars &file);
	static mtlAsset<type_t> Load(const mtlChars &file);
	static void Purge( void );
};

template < typename type_t >
mtlBinaryTree<typename mtlAsset<type_t>::AssetNode> &mtlAsset<type_t>::GetAssetBank( void )
{
	static mtlBinaryTree<AssetNode> assetBank;
	return assetBank;
}

//template < typename type_t >
//mtlBinaryTree<typename mtlAsset<type_t>::AssetNode> mtlAsset<type_t>::m_assetBank;

template < typename type_t >
mtlAsset<type_t>::mtlAsset(const mtlAsset<type_t> &asset) : m_ref(asset.m_ref)
{
}

template < typename type_t >
mtlAsset<type_t> &mtlAsset<type_t>::operator=(const mtlAsset<type_t> &asset)
{
	if (&asset != this) {
		m_ref = asset.m_ref;
	}
	return *this;
}

template < typename type_t >
const type_t *mtlAsset<type_t>::GetAsset( void ) const
{
	return m_ref.GetShared()->asset;
}

template < typename type_t >
template < typename derived_t >
mtlAsset<type_t> mtlAsset<type_t>::Load(const mtlChars &file)
{
	mtlHash hash(file);
	mtlBranch<AssetNode> *branch = GetAssetBank().GetRoot();
	if (branch != NULL) {
		branch = branch->Find(hash);
	}

	mtlAsset<type_t> asset;
	mtlNode< mtlShared<Instance> > *node = NULL;
	if (branch != NULL) {
		node = branch->GetItem().assets.GetShared()->GetFirst();
		while (node != NULL) {
			if (node->GetItem().GetShared()->file.GetDirectory().Compare(file)) {
				break;
			}
			node = node->GetNext();
		}
	}
	if (node == NULL) {
		mtlShared<Instance> instance;
		instance.New();
		instance.GetShared()->asset = new derived_t;
		instance.GetShared()->file = file;
		if (instance.GetShared()->asset->Load(file)) {
			if (branch == NULL) {
				AssetNode assetNode;
				assetNode.assets.New();
				assetNode.hash = hash;
				branch = GetAssetBank().Insert(assetNode);
			}
			branch->GetItem().assets.GetShared()->AddLast(instance);
		}
		asset.m_ref = instance;
	} else {
		asset.m_ref = node->GetItem();
	}
	return asset;
}

template < typename type_t >
mtlAsset<type_t> mtlAsset<type_t>::Load(const mtlChars &file)
{
	return Load<type_t>(file);
}

template < typename type_t >
void mtlAsset<type_t>::Purge( void )
{
	// in order traversal of tree
	// at every node, remove all assets that only have 1 reference
	// if list at node is empty, remove node
		// requires Remove to be implemented in mtlBranch

	/*mtlNode< mtlShared<Instance> > *node = GetAssetBank().GetFirst();
	while (node != NULL) {
		if (node->GetItem().GetCount() == 1) { // only this list references it
			node = node->Remove(); // purge it from list
		} else {
			node = node->GetNext();
		}
	}*/
}

/*template < typename type_t >
class mtlAsset
{
private:
	struct Ref
	{
		type_t			*asset;
		int				count;
		mtlHash			hash;
		mtlNode<Ref>	*node;
		mtlDirectory	filename;
						Ref( void ) : asset(NULL), count(0), hash(""), node(NULL), filename("") {}
	};
private:
	static mtlList<Ref> assets;
private:
	Ref			*m_ref;
	mtlString	m_error;
private:
	void CopyReference(const mtlAsset<type_t> &p_asset);
	void DeleteReference( void );
	bool IsValid( void ) const { return m_ref != NULL && m_ref->count > 0 && m_ref->asset != NULL; }
public:
						mtlAsset( void ) : m_ref(NULL) {}
	explicit			mtlAsset(const mtlAsset<type_t> &p_asset) : m_ref(NULL) { CopyReference(p_asset); }
	//template < typename derived_t >
	//explicit			mtlAsset(const mtlChars &p_filename) : m_ref(NULL) { Load<derived_t>(p_filename); }
						~mtlAsset( void ) { DeleteReference(); }
	mtlAsset<type_t>	&operator=(const mtlAsset<type_t> &p_asset) { CopyReference(p_asset); return *this; }
	template < typename derived_t >
	bool				Load(const mtlChars &p_filename);
	bool				Load(const mtlChars &p_filename);
	void				Invalidate( void ) { DeleteReference(); }
	const type_t		*GetAsset( void ) const { return IsValid() ? m_ref->asset : NULL; }
	const mtlDirectory	&GetFilename( void ) const { return IsValid() ? m_ref->filename : mtlDirectory(""); }
	const mtlString		&GetError( void ) const { return m_error; }
	static void			Purge( void );
	static int			GetSize( void ) { return assets.GetSize(); }
};

template < typename type_t >
mtlList<typename mtlAsset<type_t>::Ref> mtlAsset<type_t>::assets;

template < typename type_t >
void mtlAsset<type_t>::CopyReference(const mtlAsset<type_t> &p_asset)
{
	if (p_asset.m_ref != m_ref) {
		DeleteReference();
		if (p_asset.IsValid()) {
			m_ref = p_asset.m_ref;
			++m_ref->count;
		}
		m_error.Copy(p_asset.m_error);
	}
}

template < typename type_t >
void mtlAsset<type_t>::DeleteReference( void )
{
	if (IsValid()) {
		--m_ref->count;
	}
	m_ref = NULL;
}

template < typename type_t >
template < typename derived_t >
bool mtlAsset<type_t>::Load(const mtlChars &p_filename)
{
	DeleteReference();
	mtlDirectory dir = p_filename;
	mtlHash hash = mtlHash(dir.GetDirectory()); // should really forcibly convert filename to path relative to working directory
	mtlNode<Ref> *node = assets.GetFirst();
	while (node != NULL) {
		if (node->GetItem().hash.value == hash.value) { break; } // REMEMBER: must also compare filename to account for hash collision
		node = node->GetNext();
	}
	if (node != NULL) {
		m_ref = &node->GetItem();
		++m_ref->count;
	} else {
		assets.AddLast(Ref());
		m_ref = &assets.GetLast()->GetItem();
		m_ref->asset = new derived_t;
		if (m_ref->asset->Load(p_filename)) {
			m_ref->count = 1;
			m_ref->hash = hash;
			m_ref->node = assets.GetLast();
			m_ref->filename = dir;
			m_error.Copy("");
		} else {
			m_error.Copy(m_ref->asset->GetError());
			delete m_ref->asset;
			assets.Remove(assets.GetLast());
			m_ref = NULL;
			return false;
		}
	}
	return true;
}

template < typename type_t >
bool mtlAsset<type_t>::Load(const mtlChars &p_filename)
{
	return Load<type_t>(p_filename);
}

template < typename type_t >
void mtlAsset<type_t>::Purge( void )
{
	mtlNode<Ref> *node = assets.GetFirst();
	while (node != NULL) {
		if (node->GetItem().count <= 0) {
			delete node->GetItem().asset;
			node = assets.Remove(node);
		} else {
			node = node->GetNext();
		}
	}
}*/

#endif
