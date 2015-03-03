#ifndef GUI_H
#define GUI_H

#include "Platform.h"

#include "Common.h"
#include "MiniLib/MTL/mtlString.h"
#include "MiniLib/MTL/mtlPointer.h"
#include "MiniLib/MTL/mtlAsset.h"
#include "MiniLib/MTL/mtlType.h"

namespace GUI
{

void Init( void );
void Destroy( void );

void SetColor(float r, float g, float b, float a = 1.0f);
void SetColor(mmlVector<3> color);
void SetColor(mmlVector<4> color);

int U_To_X(float u);
int V_To_Y(float v);
float X_To_U(int x);
float Y_To_V(int y);

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
void NewLine(int scale = 1);

void Print(const mtlChars &text, int scale = 1);
void Print(char ch, int scale = 1);
void Print(int number, int scale = 1);
void Print(float number, int scale = 1);
void Print(Point p, int scale = 1);
void Print(Rect r, int scale = 1);
template < int n >
void Print(const mmlVector<n> &vec, int scale = 1)
{
	for (int i = 0; i < n-1; ++i) {
		GUI::Print(vec[n], scale);
		GUI::Print(", ", scale);
	}
	GUI::Print(vec[n-1], scale);
}

void Box(Rect rect);
void Box(mmlVector<2> min, mmlVector<2> max);

/*void DrawLineXY(int x1, int y1, int x2, int y2);
void DrawLineUV(float u1, float v1, float u2, float v2);
void DrawBoxXY(int x1, int y1, int x2, int y2);
void DrawBoxUV(float u1, float v1, float u2, float v2);
void DrawBoxFilledXY(int x1, int y1, int x2, int y2);
void DrawBoxFilledUV(float u1, float v1, float u2, float v2);
void DrawImage(); // image*/

Point GetTextSize(const mtlChars &text, int scale = 1);
Point GetTextSize(int number, int scale = 1);
Point GetTextSize(float number, int scale = 1);

int GetCharPixelWidth(int scale = 1);
int GetCharPixelHeight(int scale = 1);

class GraphicsRect
{
private:
	Rect			m_rect;
	mmlVector<4>	m_color;
	GLuint			m_id;

public:
	GraphicsRect( void );
	GraphicsRect(int x, int y, int w, int h);
	~GraphicsRect( void );

	void SetBounds(int x, int y, int w, int h);
	void SetColor(float r, float g, float b, float a = 1.0f);
	void DrawLines( void ) const;
	void DrawFilled( void ) const;

	Rect GetRect( void ) const;
};

class Form;
class Manager;

class Control : public mtlBase
{
	friend class GUI::Form;
	friend class GUI::Manager;

private:
	GUI::Manager					*m_manager;
	GUI::Form						*m_form;
	GUI::Control					*m_parent;
	mtlList< mtlShared<Control> >	m_children;
	GUI::GraphicsRect				m_rect;
	mtlString						m_name;
	int								m_textScale;
	bool							m_hasFocus; // only one child can have focus
	bool							m_visible;
	bool							m_locked;

private:
	//ContentRect ClipRect(ContentRect rect) const;
	Control(const Control&) : mtlBase(this) {}
	Control &operator=(const Control&) { return *this; }

protected:
	virtual void OnDraw( void ) const {}
	virtual void OnClick(int x, int y) {}
	virtual void OnKey(SDLKey key) {}
	virtual void OnUpdate( void ) {}
	virtual void OnInit( void ) {}
	virtual void OnDestroy( void ) {}

	void DrawChildren(Rect clip) const;
	static Rect Clip(Rect a, Rect b);

public:
	Control( void );
	virtual ~Control( void );

	void SetPositionXY(int x, int y);
	void SetPositionUV(float u, float v);
	void SetDimensionsXY(int w, int h);
	void SetDimensionsUV(float u, float v);

	void Lock( void );
	void Unlock( void );

	void Focus( void ); // send message to parent that this control requests focus
	void Unfocus( void ); // just set hasFocus to false (no other controls need to be focused)

	void Hide( void );
	void Show( void );

	void Draw(Rect clip) const;
	void Update( void );
	void Init( void );
	void Destroy( void ); // send signal to manager to delete this and all children
	void Kill( void ); // destroy without calling OnDestroy

	template < typename control_t >
	mtlShared<GUI::Control> AddControl( void ) { m_children.AddLast(mtlShared<GUI::Control>::Create<control_t>()); return m_children.GetLast()->GetItem(); }
};

class Label : public mtlInherit<GUI::Control, GUI::Label>
{
private:
	mtlString m_text;

private:
	Label(const Label&) : mtlInherit<GUI::Control, GUI::Label>(this) {}
	Label &operator=(const Label&) { return *this; }

protected:
	void OnDraw( void ) const;

public:
	Label( void ) : mtlInherit<GUI::Control, GUI::Label>(this), m_text() {}

	const mtlString &GetLabel( void ) const;
	void SetLabel(const mtlChars &text);
};

class Form : public mtlInherit<GUI::Control, GUI::Form>
{
	friend class GUI::Manager;

public:
	struct Theme
	{
		int				textScale;
		int				paddingX;
		int				paddingY;
		mmlVector<4>	fontColor;
		mmlVector<4>	addColor;
		mmlVector<4>	mulColor;
		float			blur;
	};

private:
	GUI::Form::Theme m_theme;

private:
	Form(const Form&) : mtlInherit<GUI::Control, GUI::Form>(this) {}
	Form &operator=(const Form&) { return *this; }

protected:
	void OnDraw( void ) const;

public:
	Form( void );
};

// scroll bars and such
/*class View : public mtlInherit<GUI::Control>
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
};*/

class Manager
{
private:
	mtlList< mtlShared<GUI::Form> >	m_forms;
	mtlShared<GUI::Control>			m_focus; // input is redirected to this object
	mmlVector<4>					m_color;
	int								m_textCaretX, m_textCaretY;
	int								m_newlineHeight;

private:
	void HandleInput( void );
	void DrawForms( void ) const;

public:
	enum Format {
		XY,
		UV
	};

	Manager( void );
	~Manager( void );

	void Init( void );
	void Destroy( void );

	void SetTextColor(float r, float g, float b, float a = 1.0f);
	void SetTextColor(mmlVector<4> color);
	void SetTextColor(mmlVector<3> color);
	void Print(const mtlChars &text, int scale = 1);
	void Print(int number, int scale = 1);
	void Print(float number, int scale = 1);
	void PrintNewLine(int scale = 1);
	void SetTextCaret(int x, int y);

	void DrawFilledBox(int x1, int y1, int x2, int y2);
	void DrawBox(int x1, int y1, int x2, int y2);
	void DrawLine(int x1, int y1, int x2, int y2);

	void Update( void );

	template < typename form_t >
	mtlShared<GUI::Form> AddForm( void )
	{
		m_forms.AddFirst(mtlShared<GUI::Form>::Create<form_t>());
		GUI::Form *f = m_forms.GetFirst()->GetItem().GetShared();
		f->m_manager = this;
		f->m_parent = NULL;
		f->m_form = f;
		f->Focus();
		return m_forms.GetFirst()->GetItem();
	}
};

}

#endif // FONT_H
