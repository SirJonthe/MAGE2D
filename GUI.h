#ifndef GUI_H
#define GUI_H

#include "Common.h"
#include "MTL/mtlString.h"
#include "MTL/mtlPointer.h"
#include "MTL/mtlAsset.h"

namespace GUI
{

void Init( void );
void Destroy( void );

void SetGUIView( void );

void SetColor(float r, float g, float b, float a = 1.0f);
void SetColor(mmlVector<3> color);
void SetColor(mmlVector<4> color);

void SetCaretXY(int x, int y);
void SetCaretX(int x);
void SetCaretY(int y);
void SetCaretUV(float u, float v);
void SetCaretU(float u);
void SetCaretV(float v);
Point GetCaretXY( void );
int GetCaretX( void );
int GetCaretY( void );
mmlVector<2> GetCaretUV( void );
float GetCaretU( void );
float GetCaretV( void );
void NewLine( void );

void Text(const mtlChars &text, int scale = 1);
void Text(const mtlChars &text, int x, int y, int scale = 1);
void Text(const mtlChars &text, float u, float v, int scale = 1);
void Text(int number, int scale = 1);
void Text(int number, int x, int y, int scale = 1);
void Text(int number, float u, float v, int scale = 1);
void Text(float number, int scale = 1);
void Text(float number, int x, int y, int scale = 1);
void Text(float number, float u, float v, int scale = 1);
void Box(Rect rect);
void Box(mmlVector<2> min, mmlVector<2> max);
//void Image(const Image &image, Rect rect);
//void Image(const Image &image, mmlVector<2> min, mmlVector<2> max);

Point GetTextSize(const mtlChars &text, int scale = 1);

int GetCharPixelWidth(int scale = 1);
int GetCharPixelHeight(int scale = 1);

/*
class Manager;

class Control : public mtlBase
{
private:
	GUI::Manager					*m_manager;
	GUI::Control					*m_parent;
	mtlList< mtlShared<Control> >	m_children;
	Rect							m_rect;
	mtlString						m_name;
	int								m_textScale;
	bool							m_hasFocus; // only one child can have focus
	bool							m_locked;
	bool							m_autoResize;
	int								m_originX; // modifies behavior of SetPosition (-1, 0, 1)
	int								m_originY; // modifies behavior of SetPosition (-1, 0, 1)
	int								m_anchorMaskX; // resize to parent X border
	int								m_anchorMaskY; // resize to parent Y border

protected:
	virtual void OnDraw( void ) = 0;
	virtual void OnUpdate( void ) = 0;
	virtual void OnFocus( void ) = 0;
	virtual void OnUnfocus( void ) = 0;
	virtual void OnHover( void ) = 0;
	virtual void OnDestroy( void ) = 0;

public:
	void FitToContents( void );
	void EnableAutoResize( void );
	void DisableAutoResize( void );

	void SetPositionXY(int x, int y);
	void SetPositionUV(float u, float v); // always sets a position in relation to its parent where 0,0 is top left and 1,1 is lower right (depending on originX and originY)
	void SetDimensionsXY(int w, int h);
	void SetDimensionsUV(float u, float v); // always sets a dimension in relation to its parent where 0,0 is area=0 and 1,1 is the full size of the parent control

	void Lock( void );
	void Unlock( void );

	void Focus( void ); // send message to parent that this control requests focus
	void Unfocus( void ); // just set hasFocus to false (no other controls need to be focused)

	void Hide( void );
	void Show( void );
	void ToggleVisible( void );

	void Destroy( void ); // send signal to manager to delete this and all children

	template < typename control_t >
	control_t *NewControl( void ) { m_children.AddLast(mtlShared<GUI::Control>::Create<control_t>()); return m_children.GetLast()->GetItem()->GetShared(); }
};

class Window : public mtlInherit<GUI::Control>
{
	friend class GUI::Manager;

public:
	struct Theme
	{
		int				textScale;
		int				paddingX;
		int				paddingY;
		mmlVector<4>	primaryColor;
		mmlVector<4>	secondaryColor;
		mmlVector<4>	activationColor;
		mmlVector<4>	hoverColor;
	};
private:
	GUI::Window::Theme m_theme;

public:
	bool LoadLayout(const mtlChars &file);
};

// scroll bars and such
class View : public mtlInherit<GUI::Control>
{
};

class Box : public mtlInherit<GUI::Control>
{
};

class Image : public mtlInherit<GUI::Control>
{
private:
	mtlAsset<Image> m_image;
	float m_scale;
};

class Button : public mtlInherit<GUI::Control>
{
};

class Label : public mtlInherit<GUI::Control>
{
private:
	mtlString m_contents;
};

class TextBox : public mtlInherit<GUI::Label>
{
protected:
	void OnInput(const mtlString &input);
};

class Menu : public mtlInherit<GUI::Control>
{
private:
	mtlList< mtlShared<GUI::Button> > m_entries;
};

class Manager
{
private:
	mtlList< mtlShared<GUI::Window> >	m_windows;
	mtlShared<GUI::Control>				m_focus;
	//mtlAsset<Font>					m_font;
	int									m_textCaretX, m_textCaretY;

public:
	enum Format {
		XY,
		UV
	};

	Manager( void );
	~Manager( void );

	void Init( void );
	void Destroy( void );

	void SetDefaultView( void );

	void Print(const mtlChars &text);
	void Print(int number);
	void Print(float number);
	void PrintNewLine();
	void SetCaretXY(int x, int y);
	void SetCaretUV(float u, float v);

	void DrawFilledBox();
	void DrawBox();
	void DrawLine();

	template < typename window_t >
	void NewWindow( void )
	{
		m_windows.AddFirst(mtlShared<GUI::Window>::Create<window_t>());
		GUI::Window *w = m_windows.GetFirst()->GetItem().GetShared();
		w->m_manager = this;
		w->m_parent = NULL;
		w->Focus();
	}
	void NewWindow(const mtlChars &file)
	{
		m_windows.AddFirst();
		GUI::Window *w = m_windows.GetFirst()->GetItem().GetShared();
		w->m_manager = this;
		w->m_parent = NULL;
		if (!w->LoadLayout(file)) {
			w->Destroy();
		} else {
			w->Focus();
		}
	}
};*/

}

#endif // FONT_H
