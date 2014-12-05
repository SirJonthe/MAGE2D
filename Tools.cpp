#include "Tools.h"
#include "Engine.h"
#include "GUI.h"

RegisterObject(KillPlane);

void KillPlane::OnUpdate( void )
{
	mtlList<ObjectRef> objects;
	GetEngine()->FilterByPlaneCollision(GetEngine()->GetObjects(), objects, m_plane);
	mtlItem<ObjectRef> *object = objects.GetFirst();
	while (object != NULL) {
		object->GetItem().GetShared()->Destroy();
	}
}

KillPlane::KillPlane( void ) : mtlInherit(this)
{
	ClearAllObjectFlags(); // other objects can not collide with us, *we* collide with *them*
	SetName("tool_killplane");
	SetPlane(mmlVector<2>(0.0f, 0.0f), mmlVector<2>(0.0f, -1.0f));
}

void KillPlane::SetPlane(mmlVector<2> point, mmlVector<2> normal)
{
	m_plane.point = point;
	m_plane.normal = mmlNormalize(normal);
}

RegisterObject(Console);

void Console::OnUpdate( void )
{
	if (GetEngine()->IsPressed(m_envokationKey)) {
		ToggleGraphics();
	}
}

void Console::OnDraw( void )
{
	Engine::SetGUIProjection();
	Engine::SetGUIView();

	GUI::SetColor(m_bgColor);
	GUI::Box(mmlVector<2>(0.0f, 0.0f), mmlVector<2>(Engine::GetVideoWidth(), Engine::GetVideoHeight() * m_screenHeightRatio));

	GUI::SetCaretXY(0, Engine::GetVideoHeight() - GUI::GetCharPixelHeight());

	mtlItem<mtlString> *l = m_lines.GetLast();
	if (l != NULL) {
		mmlVector<3> c = mmlVector<3>(0.0f, 0.0f, 0.0f);
		GUI::SetColor(c[0], c[1], c[2]);
		int i = 0;
		while (i < l->GetItem().GetSize()) {
			int end = l->GetItem().FindFirstChar("\\");
			if (end >= 0 && end < l->GetItem().GetSize()-1) {
				// color definition!
				GUI::SetColor(c[0], c[1], c[2]);
			}
			GUI::Print(l->GetItem().GetSubstring(i, end));
			i = end;
		}
	}
}

Console::Console( void ) :
	mtlInherit(this),
	m_bgColor(0.0f, 0.0f, 0.0f),
	m_lines(), m_maxHistory(10),
	m_screenHeightRatio(0.5f),
	m_envokationKey(SDLK_HOME)
{
	ClearAllCollisionMasks();
	ClearAllObjectFlags();
	DisableCollisions();
}

void Console::SetBackgroundColor(float r, float g, float b, float a)
{
	SetBackgroundColor(mmlVector<4>(r, g, b, a));
}

void Console::SetBackgroundColor(const mmlVector<4> &color)
{
	m_bgColor = color;
}

void Console::SetMaxHistory(int lines)
{
	m_maxHistory = mmlMax2(1, lines);
}

void Console::SetEnvokationKey(SDLKey key)
{
	m_envokationKey = key;
}

void Console::Print(const mtlChars &text)
{
	if (m_lines.GetSize() == 0) {
		NewLine();
	}
	m_lines.GetLast()->GetItem().Append(text);
}

void Console::Print(int num)
{
	mtlString s;
	s.FromInt(num);
	Print(s);
}

void Console::Print(float num)
{
	mtlString s;
	s.FromFloat(num);
	Print(s);
}

void Console::NewLine( void )
{
	m_lines.AddLast();
	while (m_lines.GetSize() > m_maxHistory) {
		m_lines.RemoveFirst();
	}
}

SpriteEditor::Button::Button( void ) : m_active(false), m_hidden(false)
{
	m_position.x = 0;
	m_position.y = 0;
	m_label.Copy("Button");
}

void SpriteEditor::Button::Draw(bool hover)
{
	if (m_hidden) { return; }
	if (hover) {
		GUI::SetColor(1.0f, 1.0f, 1.0f);
	} else if (hover) {
		GUI::SetColor(0.9f, 0.9f, 0.9f);
	} else {
		GUI::SetColor(0.5f, 0.5f, 0.5f);
	}
	GUI::SetCaretXY(m_position.x, m_position.y);
	GUI::Print(m_label);
}

void SpriteEditor::Button::Activate( void )
{
	m_active = true;
}

void SpriteEditor::Button::Deactivate( void )
{
	m_active = false;
}

bool SpriteEditor::Button::IsActive( void ) const
{
	return m_active;
}

bool SpriteEditor::Button::IsHovering(int x, int y) const
{
	if (m_hidden) { return false; }
	Point text_size = GUI::GetTextSize(m_label);
	text_size.x += m_position.x;
	text_size.y += m_position.y;
	return (x >= m_position.x && x <= text_size.x && y >= m_position.y && y <= text_size.y);
}

void SpriteEditor::Button::SetPosition(int x, int y)
{
	m_position.x = x;
	m_position.y = y;
}

void SpriteEditor::Button::SetLabel(const mtlChars &label)
{
	m_label.Copy(label);
}

void SpriteEditor::Button::Hide( void )
{
	m_hidden = true;
}

void SpriteEditor::Button::Show( void )
{
	m_hidden = false;
}

SpriteEditor::SpriteEditor(const SpriteEditor&) : mtlInherit(this) {}

SpriteEditor &SpriteEditor::operator=(const SpriteEditor&) { return *this; }

void SpriteEditor::OnInit( void )
{
	m_file.SetLabel("File");
	SetName("tool_SpriteEditor");
	GetEngine()->SetCamera(GetEngine()->GetSelf(this));
	m_sprite = GetEngine()->AddObject<Object>();
	m_sprite->SetGraphics(GetEngine()->LoadGraphics<Image>("../tmp/test.png"));
}

void SpriteEditor::OnUpdate( void )
{
	// Controls
	// right arrow key for next frame
	// left arrow key for prev frame
	// Space for start/stop animation playback
	// Ctrl-S for saving
	// Ctrl-O for loading

	// GUI Controls
	// Collider - make a collider for current frame (support for Ctrl-Z and Ctrl-Y)
		// Free form - click this again to stop drawing collision shape
		// Clear - remove current collider
	// File - load an image file
	// Animation
		// various ways to set the animation (used in .sprite format)

	// Output:
	// sprite format file
	// Wavefront OBJ file for collider

	if (GetEngine()->IsPressed(SDLK_DOWN)) {
		GetTransform().SetScale(Transform::Local, mmlMin2(1.0f, GetTransform().GetScaleX(Transform::Local) * 2.0f));
	} else if (GetEngine()->IsPressed(SDLK_UP)) {
		GetTransform().SetScale(Transform::Local, mmlMax2(1.0f / 16.0f, GetTransform().GetScaleX(Transform::Local) / 2.0f));
	}

	if (GetEngine()->IsDown(MouseButton::Right)) {
		GetTransform().Translate(Transform::Local, -GetEngine()->GetWorldMouseMovement());
	}
}

void SpriteEditor::OnGUI( void )
{
	Point mouse = GetEngine()->GetMousePosition();
	m_file.Draw(m_file.IsHovering(mouse.x, mouse.y));

	GUI::SetColor(1.0f, 1.0f, 1.0f);

	mmlVector<2> mouseLocation = GetEngine()->GetWorldMousePosition();
	GUI::SetCaretXY(0, GetEngine()->GetVideoHeight() - GUI::GetCharPixelHeight());
	GUI::Print((int)mouseLocation[0]);
	GUI::Print(", ");
	GUI::Print((int)mouseLocation[1]);

	GUI::SetCaretXY(GetEngine()->GetVideoWidth() - GUI::GetCharPixelWidth()*5, GetEngine()->GetVideoHeight() - GUI::GetCharPixelHeight());
	GUI::Print((int)(1.0f / GetTransform().GetScaleX(Transform::Local)));
	GUI::Print("x");
}

SpriteEditor::SpriteEditor( void ) : mtlInherit(this)
{
}

RegisterObject(SpriteEditor);
