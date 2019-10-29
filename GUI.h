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

// @algo Init
// @info Initializes GUI.
void Init( void );
	
// @algo Destroy
// @info Destroys GUI subsystem.
void Destroy( void );

// @algo SetColor
// @info Alters the color state of GUI rendering.
// @in r, g, b, a -> Color values in normalized range (0-1)
void SetColor(float r, float g, float b, float a = 1.0f);
	
// @algo SetColor
// @info Alters the color state of GUI rendering.
// @in color -> RGB color values in normalized range (0-1). Alpha set to 1.
void SetColor(mmlVector<3> color);
	
// @algo SetColor
// @info Alters the color state of GUI rendering.
// @in color -> RGBA color values in normalized range (0-1).
void SetColor(mmlVector<4> color);

// @algo U_To_X
// @info Converts normalized screen coordinate (0-1) to absolute screen coordinate (0-width).
// @in u -> Normalized screen coordinate (0-1).
// @out Absolute screen coordinate (0-width).
int U_To_X(float u);
	
// @algo V_To_Y
// @info Converts normalized screen coordinate (0-1) to absolute screen coordinate (0-height).
// @in v -> Normalized screen coordinate (0-1).
// @out Absolute screen coordinate (0-height).
int V_To_Y(float v);
	
// @algo X_To_U
// @info Converts absolute screen coordinate (0-width) to normalized screen coordinate (0-1).
// @in x -> Absolute screen coordinate (0-height).
// @out Normalized screen coordinate (0-1).
float X_To_U(int x);
	
// @algo Y_To_V
// @info Converts absolute screen coordinate (0-height) to normalized screen coordinate (0-1).
// @in y -> Absolute screen coordinate (0-height).
// @out Normalized screen coordinate (0-1).
float Y_To_V(int y);

// @algo SetCaretXY
// @info Sets the rendering caret at the absolute XY coordinate.
// @in x, y -> Absolute screen coordinates.
void SetCaretXY(int x, int y);
	
// @algo SetCaretX
// @info Sets the rendering caret at the absolute X coordinate.
// @in x -> Absolute screen coordinate.
void SetCaretX(int x);
	
// @algo SetCaretY
// @info Sets the rendering caret at the absolute Y coordinate.
// @in y -> Absolute screen coordinate.
void SetCaretY(int y);
	
// @algo SetCaretUV
// @info Sets the rendering caret at the normalized UV coordinate.
// @in u, v -> Normalized screen coordinate.
void SetCaretUV(float u, float v);

// @algo SetCaretU
// @info Sets the rendering caret at the normalized U coordinate.
// @in u -> Normalized screen coordinate.
void SetCaretU(float u);
	
// @algo SetCaretV
// @info Sets the rendering caret at the normalized V coordinate.
// @in v -> Normalized screen coordinate.
void SetCaretV(float v);
	
// @algo GetCaretXY
// @out The absolute caret coordinate.
Point GetCaretXY( void );
	
// @algo GetCaretX
// @out The absolute caret X coordinate.
int GetCaretX( void );
	
// @algo GetCaretY
// @out The absolute caret Y coordinate.
int GetCaretY( void );
	
// @algo GetCaretUV
// @out The normalized caret coordinate.
mmlVector<2> GetCaretUV( void );
	
// @algo GetCaretU
// @out The normalized caret U coordinate.
float GetCaretU( void );
	
// @algo GetCaretV
// @out The normalized caret V coordinate.
float GetCaretV( void );
	
// @algo NewLine
// @info Moves the caret down and resets X to its X origin.
// @in scale -> The text scaling factor
void NewLine(int scale = 1);

// @algo Print
// @info Prints text using the color set with SetColor at the last position set by the caret.
// @note The caret position moves to the end of the printed message.
// @in
//   text -> The message.
//   scale -> The desired text scaling.
void Print(const mtlChars &text, int scale = 1);
	
// @algo Print
// @info Prints text using the color set with SetColor at the last position set by the caret.
// @note The caret position moves to the end of the printed message.
// @in
//   ch -> The message.
//   scale -> The desired text scaling.
void Print(char ch, int scale = 1);
	
// @algo Print
// @info Prints text using the color set with SetColor at the last position set by the caret.
// @note The caret position moves to the end of the printed message.
// @in
//   number -> The message.
//   scale -> The desired text scaling.
void Print(int number, int scale = 1);
	
// @algo Print
// @info Prints text using the color set with SetColor at the last position set by the caret.
// @note The caret position moves to the end of the printed message.
// @in
//   number -> The message.
//   scale -> The desired text scaling.
void Print(float number, int scale = 1);
	
// @algo Print
// @info Prints text using the color set with SetColor at the last position set by the caret.
// @note The caret position moves to the end of the printed message.
// @in
//   p -> The message.
//   scale -> The desired text scaling.
void Print(Point p, int scale = 1);
	
// @algo Print
// @info Prints text using the color set with SetColor at the last position set by the caret.
// @note The caret position moves to the end of the printed message.
// @in
//   r -> The message.
//   scale -> The desired text scaling.
void Print(Rect r, int scale = 1);
	
// @algo Print
// @info Prints text using the color set with SetColor at the last position set by the caret.
// @note The caret position moves to the end of the printed message.
// @in
//   vec -> The message.
//   scale -> The desired text scaling.
template < int n >
void Print(const mmlVector<n> &vec, int scale = 1)
{
	for (int i = 0; i < n-1; ++i) {
		float x = vec[n];
		GUI::Print(x, scale);
		GUI::Print(", ", scale);
	}
	GUI::Print(vec[n-1], scale);
}

// @algo Box
// @info Renders a box.
// @in min, max -> The box min/max bounds in normalized screen coordinates.
void Box(mmlVector<2> min, mmlVector<2> max);
	
// @algo Box
// @info Renders a box.
// @in rect -> The box in absolute screen coordinates.
void Box(Rect rect);
	
// @algo Line
// @info Renders a line.
// @in a, b -> The line segment in normalized screen coordinates.
void Line(mmlVector<2> a, mmlVector<2> b);
	
// @algo Line
// @info Renders a line.
// @info a, b -> The line segment in absolute screen coordinates.
void Line(Point a, Point b);
	
// @algo Arrow
// @info Renders an arrow.
// @in
//   origin, to -> The line segment in normalized screen coordinates.
//   size -> The radius of the arrowhead edges.
void Arrow(mmlVector<2> origin, mmlVector<2> to, float size = 5.0f);
	
// @algo Arrow
// @info Renders an arrow.
// @in
//   origin, to -> The line segment in absolute screen coordinates.
//   size -> The radius of the arrowhead edges.
void Arrow(Point origin, Point to, int size = 5);

/*void DrawLineXY(int x1, int y1, int x2, int y2);
void DrawLineUV(float u1, float v1, float u2, float v2);
void DrawBoxXY(int x1, int y1, int x2, int y2);
void DrawBoxUV(float u1, float v1, float u2, float v2);
void DrawBoxFilledXY(int x1, int y1, int x2, int y2);
void DrawBoxFilledUV(float u1, float v1, float u2, float v2);
void DrawImage(); // image*/

// @algo GetTextSize
// @in
//   text -> The message.
//   scale -> The text scaling.
// @out The width and height of the text in absolute screen space.
Point GetTextSize(const mtlChars &text, int scale = 1);
	
// @algo GetTextSize
// @in
//   number -> The message.
//   scale -> The text scaling.
// @out The width and height of the text in absolute screen space.
Point GetTextSize(int number, int scale = 1);
	
// @algo GetTextSize
// @in
//   number -> The message.
//   scale -> The text scaling.
// @out The width and height of the text in absolute screen space.
Point GetTextSize(float number, int scale = 1);

// @algo GetCharPixelWidth
// @in scale -> The text scale.
// @out The width (in pixels) of a single character.
int GetCharPixelWidth(int scale = 1);
	
// @algo GetCharPixelHeight
// @in scale -> The text scale.
// @out The height (in pixels) of a single character.
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
