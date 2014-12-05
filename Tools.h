#ifndef TOOLS_H
#define TOOLS_H

#include "Object.h"
#include "Common.h"
#include "MTL/mtlDuplex.h"

ObjectDeclaration(KillPlane)
{
private:
	Plane m_plane;

protected:
	void OnUpdate( void );

public:
	KillPlane( void ); // grab all collisions, is defined as nothing, set collision shape to plane
	void SetPlane(mmlVector<2> point, mmlVector<2> normal);
};

ObjectDeclaration(Console)
{
private:
	mmlVector<4>			m_bgColor;
	mtlList< mtlString >	m_lines;
	int						m_maxHistory;
	float					m_screenHeightRatio;
	SDLKey					m_envokationKey;
	// format for altering color in text
	// \{0,255,128}
	// not printed, only alters color
	// each new line starts off as white

protected:
	void OnUpdate( void );
	void OnDraw( void );

public:
	Console( void ); // no collisions, toggle visibility on key press

	void SetBackgroundColor(float r, float g, float b, float a = 1.0f);
	void SetBackgroundColor(const mmlVector<4> &color);
	void SetMaxHistory(int lines);
	void SetEnvokationKey(SDLKey key);

	void Print(const mtlChars &text);
	void Print(int num);
	void Print(float num);
	void NewLine( void );
	void SetColor(float r, float g, float b); // appends \{r,g,b}
	void SetColor(const mmlVector<3> &color);
};

ObjectDeclaration(SpriteEditor)
{
private:
	class Button
	{
	private:
		Point		m_position;
		mtlString	m_label;
		bool		m_active;
		bool		m_hidden;

	public:
		Button( void );
		void Draw(bool hover);
		void Activate( void );
		void Deactivate( void );
		bool IsActive( void ) const;
		bool IsHovering(int x, int y) const;
		void SetPosition(int x, int y);
		void SetLabel(const mtlChars &label);
		void Hide( void );
		void Show( void );
	};

private:
	mtlString							m_currentFile;
	ObjectRef							m_sprite;
	//mtlList< mtlList< mmlVector<2> > >	m_colliders;
	Button								m_file;

private:
	SpriteEditor(const SpriteEditor&);
	SpriteEditor &operator=(const SpriteEditor&);

protected:
	void OnInit( void );
	void OnUpdate( void );
	void OnGUI( void );

public:
	SpriteEditor( void );
};

#endif // TOOLS_H
