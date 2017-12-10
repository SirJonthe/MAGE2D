#ifndef TOOLS_H
#define TOOLS_H

#include "Object.h"
#include "Common.h"
#include "MiniLib/MTL/mtlDuplex.h"

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
	mtlString				m_userInput;
	bool					m_addNewLine;
	// format for altering color in text
	// \{0,255,128}
	// not printed, only alters color
	// each new line starts off as white

private:
	void SubmitInput( void );

protected:
	void OnUpdate( void );
	void OnGUI( void );
	void OnInit( void );

public:
	Console( void ); // no collisions, toggle visibility on key press

	void SetBackgroundColor(float r, float g, float b, float a = 1.0f);
	void SetBackgroundColor(const mmlVector<4> &color);
	void SetMaxHistory(int lines);
	void SetEnvokationKey(SDLKey key);

	void Print(const mtlChars &text);
	void Print(int num);
	void Print(float num);
	void PrintRaw(const mtlChars &str);
	void NewLine( void );
	void SetColor(float r, float g, float b); // appends \{r,g,b}
	void SetColor(const mmlVector<3> &color);
};

ObjectDeclaration(SpriteEditor)
{
private:
	class Button
	{
	protected:
		Point		m_position;
		mtlString	m_label;
		bool		m_active;
		bool		m_hidden;

	public:
		Button( void );
		virtual void Draw(bool hover);
		void Activate( void );
		void Deactivate( void );
		void ToggleActive( void );
		bool IsActive( void ) const;
		bool IsHovering(int x, int y) const;
		void SetPosition(int x, int y);
		void SetLabel(const mtlChars &label);
		void Hide( void );
		void Show( void );
		int GetWidth( void ) const;
		int GetHeight( void ) const;
		int GetEndX( void ) const;
		int GetEndY( void ) const;
		void ToggleHidden( void );
		int GetStartX( void ) const;
		int GetStartY( void ) const;
	};

	/*class InputBox : public Button
	{
	protected:
		mtlString	m_input;
		Button		m_btnOK;
		Button		m_btnCancel;
		int			m_caret;
	public:
		InputBox( void );
		void Draw(bool hover);
		void Input(const mtlChars &input);
		void Delete( void );
	};*/

	ObjectDeclaration(EditableSprite)
	{
	private:
		Sprite		m_sprite;
		Timer		m_timer;
		mtlString	m_image_filename;
	protected:
		void OnDraw( void );
	public:
		EditableSprite( void );
		bool LoadImage(const mtlChars &file);
		bool LoadSprite(const mtlChars &file);
		bool SaveSprite(const mtlChars &file) const;
		void TogglePlayback( void );
		Sprite &GetSprite( void );
	};

private:
	mtlString							m_currentFile;
	EditableSprite						*m_sprite;
	mtlList< mtlList< mmlVector<2> > >	m_colliders;
	Button								m_btnFile;
	Button								m_btnNew;
	Button								m_btnLoad;
	Button								m_btnSave;
	Button								m_btnCollider;
	Button								m_btnClear;
	Button								m_btnAnimation;
	Button								m_btnImage;
	int									m_currentFrame;
	mtlList< mmlVector<2> >				*m_currentCollider;
	bool								m_unsavedChanges;

private:
	SpriteEditor(const SpriteEditor&);
	SpriteEditor &operator=(const SpriteEditor&);

private:
	bool Save(const mtlChars &out_file);
	bool SaveCollider(const mtlChars &out_file) const;
	bool SaveSprite(const mtlChars &out_file) const;

protected:
	void OnInit( void );
	void OnUpdate( void );
	void OnGUI( void );

public:
	SpriteEditor( void );
};

ObjectDeclaration(DebugCamera)
{
protected:
	void OnUpdate( void );
	void OnGUI( void );

public:
	DebugCamera( void );
};

#endif // TOOLS_H
