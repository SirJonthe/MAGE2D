#include "Tools.h"
#include "Engine.h"
#include "GUI.h"

#include <fstream>

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
	} else if (m_active) {
		GUI::SetColor(0.8f, 0.8f, 0.8f);
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

void SpriteEditor::Button::ToggleActive( void )
{
	m_active = !m_active;
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

int SpriteEditor::Button::GetWidth( void ) const
{
	return GUI::GetTextSize(m_label).x;
}

int SpriteEditor::Button::GetHeight( void ) const
{
	return GUI::GetTextSize(m_label).y;
}

int SpriteEditor::Button::GetEndX( void ) const
{
	return GetWidth() + m_position.x;
}

int SpriteEditor::Button::GetEndY( void ) const
{
	return GetHeight() + m_position.y;
}

void SpriteEditor::Button::ToggleHidden( void )
{
	m_hidden = !m_hidden;
}

int SpriteEditor::Button::GetStartX( void ) const
{
	return m_position.x;
}

int SpriteEditor::Button::GetStartY( void ) const
{
	return m_position.y;
}

bool SpriteEditor::Save(const mtlChars &out_file)
{
	m_unsavedChanges = false;
	return SaveCollider(out_file) && SaveSprite(out_file);
}

bool SpriteEditor::SaveCollider(const mtlChars &out_file) const
{
	mtlString file_name;
	file_name.Copy(out_file);
	file_name.Append(".obj");
	std::ofstream fout(file_name.GetChars());
	if (!fout.is_open()) {
		return false;
	}
	fout << "# OBJ file created by MAGE2D SpriteEditor" << std::endl;
	fout << "o polygon_collider" << std::endl << std::endl;
	fout << "v " << "0.0";
	fout.close();
	return true;
}

bool SpriteEditor::SaveSprite(const mtlChars &out_file) const
{
	return true;
}

SpriteEditor::SpriteEditor(const SpriteEditor&) : mtlInherit(this) {}

SpriteEditor &SpriteEditor::operator=(const SpriteEditor&) { return *this; }

void SpriteEditor::OnInit( void )
{
	SetName("tool_SpriteEditor");
	DisableCollisions();
	SetObjectFlags(0);
	SetCollisionMasks(0);
	GetEngine()->SetCamera(GetEngine()->GetSelf(this));

	m_btnFile.SetLabel("File");
	m_btnLoad.SetLabel("Load");
	m_btnLoad.SetPosition(0, m_btnFile.GetEndY());
	m_btnLoad.Hide();
	m_btnSave.SetLabel("Save");
	m_btnSave.SetPosition(0, m_btnLoad.GetEndY());
	m_btnSave.Hide();

	m_btnCollider.SetLabel("Collider");
	m_btnCollider.SetPosition(m_btnFile.GetEndX() + GUI::GetCharPixelWidth(), 0);
	m_btnClear.SetLabel("Clear");
	m_btnClear.SetPosition(m_btnCollider.GetStartX(), m_btnCollider.GetEndY());
	m_btnClear.Hide();

	m_btnAnimation.SetLabel("Animation");
	m_btnAnimation.SetPosition(m_btnCollider.GetEndX() + GUI::GetCharPixelWidth(), 0);

	m_sprite = GetEngine()->AddObject<Object>();
	m_sprite->SetGraphics(GetEngine()->LoadGraphics<Image>("../tmp/test.png"));

	m_colliders.AddFirst();
}

void SpriteEditor::OnUpdate( void )
{
	// Controls
	// right arrow key for next frame
	// left arrow key for prev frame
	// Space for start/stop animation playback
	// Backspace for restart animation
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

	Point iMouse = GetEngine()->GetMousePosition();
	mmlVector<2> wMouse = GetEngine()->GetWorldMousePosition();

	if (GetEngine()->IsPressed(SDLK_DOWN)) {
		GetTransform().SetScale(Transform::Local, mmlMin2(1.0f, GetTransform().GetScaleX(Transform::Local) * 2.0f));
	} else if (GetEngine()->IsPressed(SDLK_UP)) {
		GetTransform().SetScale(Transform::Local, mmlMax2(1.0f / 16.0f, GetTransform().GetScaleX(Transform::Local) / 2.0f));
	}

	if (GetEngine()->IsDown(MouseButton::Right)) {
		GetTransform().Translate(Transform::Local, -GetEngine()->GetWorldMouseMovement());
	}

	if (GetEngine()->IsPressed(MouseButton::Left)) {
		if (m_btnFile.IsHovering(iMouse.x, iMouse.y)) {
			m_btnFile.ToggleActive();
			m_btnLoad.Deactivate();
			m_btnLoad.ToggleHidden();
			m_btnSave.Deactivate();
			m_btnSave.ToggleHidden();

			m_btnCollider.Deactivate();
			m_btnClear.Deactivate();
			m_btnClear.Hide();

			m_btnAnimation.Deactivate();
		} else if (m_btnCollider.IsHovering(iMouse.x, iMouse.y)) {
			m_btnFile.Deactivate();
			m_btnLoad.Deactivate();
			m_btnLoad.Hide();
			m_btnSave.Deactivate();
			m_btnSave.Hide();

			m_btnCollider.ToggleActive();
			m_btnClear.Deactivate();
			m_btnClear.ToggleHidden();

			m_btnAnimation.Deactivate();
		} else if (m_btnAnimation.IsHovering(iMouse.x, iMouse.y)) {
			m_btnFile.Deactivate();
			m_btnLoad.Deactivate();
			m_btnLoad.Hide();
			m_btnSave.Deactivate();
			m_btnSave.Hide();

			m_btnCollider.Deactivate();
			m_btnClear.Deactivate();
			m_btnClear.Hide();

			m_btnAnimation.ToggleActive();
		} else if (m_btnClear.IsHovering(iMouse.x, iMouse.y)) {
			m_colliders.GetLast()->GetItem().RemoveAll();
		} else if (m_btnCollider.IsActive()) {
			m_colliders.GetLast()->GetItem().AddLast(wMouse);
			m_unsavedChanges = true;
		}
	}

	if (GetEngine()->IsHeld(SDLK_LCTRL) || GetEngine()->IsHeld(SDLK_RCTRL)) {
		if (GetEngine()->IsPressed(SDLK_z)) {
			if (m_colliders.GetLast()->GetItem().GetSize() > 0) {
				m_colliders.GetLast()->GetItem().RemoveLast();
				m_unsavedChanges = true;
			}
		}
	}
}

void SpriteEditor::OnDraw( void )
{

}

void SpriteEditor::OnGUI( void )
{
	if ((m_colliders.GetLast()->GetItem().GetSize() > 0 && m_btnCollider.IsActive()) || m_colliders.GetLast()->GetItem().GetSize() > 1) {
		mtlItem< mmlVector<2> > *point = m_colliders.GetLast()->GetItem().GetFirst();
		mmlVector<2> first =GetTransform().InverseTransformPoint(Transform::Local, point->GetItem());
		first = GetEngine()->GetScreenPoint(first);
		mmlVector<2> last = GetTransform().InverseTransformPoint(Transform::Local, m_colliders.GetLast()->GetItem().GetLast()->GetItem());
		last = GetEngine()->GetScreenPoint(last);
		mmlVector<2> p = first;
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		while (point != NULL) {
			glVertex2f(p[0], p[1]);
			point = point->GetNext();
			if (point == NULL) {
				glVertex2f(first[0], first[1]);
			} else {
				p = GetTransform().InverseTransformPoint(Transform::Local, point->GetItem());
				p = GetEngine()->GetScreenPoint(p);
				glVertex2f(p[0], p[1]);
			}
		}
		if (m_btnCollider.IsActive()) {
			glColor3f(1.0f, 0.0f, 0.0);
			mmlVector<2> wMouse = GetEngine()->GetScreenPoint(GetEngine()->GetWorldMousePosition());
			glVertex2f(first[0], first[1]);
			glVertex2f(wMouse[0], wMouse[1]);
			glVertex2f(last[0], last[1]);
			glVertex2f(wMouse[0], wMouse[1]);
		}
		glEnd();
	}

	Point mouse = GetEngine()->GetMousePosition();
	m_btnFile.Draw(m_btnFile.IsHovering(mouse.x, mouse.y));
	m_btnLoad.Draw(m_btnLoad.IsHovering(mouse.x, mouse.y));
	m_btnSave.Draw(m_btnSave.IsHovering(mouse.x, mouse.y));
	m_btnCollider.Draw(m_btnCollider.IsHovering(mouse.x, mouse.y));
	m_btnClear.Draw(m_btnClear.IsHovering(mouse.x, mouse.y));
	m_btnAnimation.Draw(m_btnAnimation.IsHovering(mouse.x, mouse.y));

	GUI::SetColor(1.0f, 1.0f, 1.0f);

	mmlVector<2> mouseLocation = GetEngine()->GetWorldMousePosition();
	GUI::SetCaretXY(0, GetEngine()->GetVideoHeight() - GUI::GetCharPixelHeight());
	GUI::Print((int)mouseLocation[0]);
	GUI::Print(", ");
	GUI::Print((int)mouseLocation[1]);

	GUI::SetCaretXY(GetEngine()->GetVideoWidth() - GUI::GetCharPixelWidth()*5, GetEngine()->GetVideoHeight() - GUI::GetCharPixelHeight());
	GUI::Print((int)(1.0f / GetTransform().GetScaleX(Transform::Local)));
	GUI::Print("x");

	GUI::SetCaretXY(0, GetEngine()->GetVideoHeight() - GUI::GetCharPixelHeight() * 2);
	GUI::Print("Frame: ");
	GUI::Print(m_currentFrame);

	if (m_btnCollider.IsActive()) {
		mtlItem< mmlVector<2> > *point = m_colliders.GetLast()->GetItem().GetFirst();
		GUI::SetColor(0.0, 1.0f, 0.0f);
		GUI::SetCaretXY(0, GUI::GetCharPixelHeight()*2);
		while (point != NULL) {
			GUI::Print(point->GetItem()[0]);
			GUI::Print(", ");
			GUI::Print(point->GetItem()[1]);
			GUI::NewLine();
			point = point->GetNext();
		}
	}
}

SpriteEditor::SpriteEditor( void ) : mtlInherit(this), m_currentFrame(0) {}

RegisterObject(SpriteEditor);
