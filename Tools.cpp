#include "MiniLib/MTL/mtlParser.h"

#include "Tools.h"
#include "Engine.h"
#include "GUI.h"

#include <fstream>
#include <iostream>

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

KillPlane::KillPlane( void ) : ConstructObject(KillPlane)
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

void Console::SubmitInput( void )
{
	// commands:
	// destroy type [type]
	// id [type]
	// count name [name]
	// count type [type]
	// count id [id]
	// info name [name]
	// info type [type]
	// info id [id]
	// quit // call EndGame()
	// clear // removes everything except console
	// camera position x, y
	// camera rotation r
	// camera zoom z
	// pause name [name]
	// pause id [id]
	// pause all
	// resume name name
	// resume id id
	// resume all
	if (m_lines.GetLast()->GetItem().GetSize() != 0) {
		NewLine();
	}
	Print(m_userInput);
	NewLine();

	mtlParser parser(m_userInput);

	while (!parser.IsEnd()) {
		mtlChars cmd = parser.ReadWord();
		if (cmd.Compare("spawn")) {
			mtlChars object = parser.ReadWord();
			if (GetEngine()->AddObject(object).IsNull()) {
				PrintRaw("{0.796,1.0,0.094}No such object");
			}
		} else if (cmd.Compare("destroy")) {
			cmd = parser.ReadWord();
			mtlChars destroy_type = parser.ReadLine();
			mtlList<ObjectRef> objects;
			if (cmd.Compare("name")) {
				GetEngine()->FilterByName(GetEngine()->GetObjects(), objects, destroy_type);
			} else if (cmd.Compare("id")) {
				mtlString id(destroy_type);
				GetEngine()->FilterByStaticType(GetEngine()->GetObjects(), objects, atoi(id.GetChars()));
			} else {
				PrintRaw("{0.796,1.0,0.094}No such command");
				NewLine();
			}
			mtlItem<ObjectRef> *i = objects.GetFirst();
			while (i != NULL) {
				i->GetItem()->Destroy();
				i = i->GetNext();
			}
		} else if (cmd.Compare("info")) {
			cmd = parser.ReadWord();
			mtlList<ObjectRef> objects;
			if (cmd.Compare("name")) {
				mtlChars info_type = parser.ReadLine();
				GetEngine()->FilterByName(GetEngine()->GetObjects(), objects, info_type);
			} else if (cmd.Compare("id")) {
				mtlChars info_type = parser.ReadLine();
				mtlString id(info_type);
				GetEngine()->FilterByStaticType(GetEngine()->GetObjects(), objects, atoi(id.GetChars()));
			} if (cmd.Compare("all")) {
				GetEngine()->FilterByDynamicType<Object>(GetEngine()->GetObjects(), objects);
			} else {
				PrintRaw("{0.796,1.0,0.094}No such command");
				NewLine();
			}
			mtlItem<ObjectRef> *i = objects.GetFirst();
			while (i != NULL) {
				Print(i->GetItem()->GetName());
				Print("    ");
				Print((int)i->GetItem()->GetInstanceType());
				NewLine();
				i = i->GetNext();
			}
		} else if (cmd.Compare("quit")) {
			GetEngine()->EndGame();
		} else if (cmd.Compare("echo")) {
			PrintRaw(parser.ReadLine());
		} else {
			PrintRaw("{0.796,1.0,0.094}No such command");
			NewLine();
		}
	}

	m_userInput.Free();
}

void Console::OnUpdate( void )
{
	if (GetEngine()->IsPressed(m_envokationKey)) {
		ToggleGraphics();
	}
	if (IsVisible()) {
		if (GetEngine()->IsPressed(SDLK_RETURN)) {
			SubmitInput();
		} else {
			char ch[2] = {0,0};
			bool shift = GetEngine()->IsDown(SDLK_LSHIFT) || GetEngine()->IsDown(SDLK_RSHIFT);
			for (int i = (int)SDLK_a; i <= (int)SDLK_z; ++i) {
				if (GetEngine()->IsPressed((SDLKey)i)) {
					ch[0] = (i - (int)SDLK_a) + 'a';
					if (shift) { ch[0] = mtlChars::ToUpper(ch[0]); }
					m_userInput.Append(ch);
				}
			}
			for (int i = (int)SDLK_0; i <= (int)SDLK_9; ++i) {
				if (GetEngine()->IsPressed((SDLKey)i)) {
					ch[0] = (i - (int)SDLK_0) + '0';
					m_userInput.Append(ch);
				}
			}
			if (GetEngine()->IsPressed(SDLK_SPACE)) {
				m_userInput.Append(" ");
			}
			if (GetEngine()->IsPressed(SDLK_BACKSPACE) && m_userInput.GetSize() > 0) {
				m_userInput.Remove(m_userInput.GetSize()-1, 1);
			}
		}
	}
}

void Console::OnGUI( void )
{
	if (IsVisible()) {
		GUI::SetColor(m_bgColor);
		GUI::Box(mmlVector<2>(0.0f, 0.0f), mmlVector<2>(Engine::GetVideoWidth(), Engine::GetVideoHeight() * m_screenHeightRatio));

		GUI::SetCaretXY(0, Engine::GetVideoHeight() - GUI::GetCharPixelHeight());

		mtlItem<mtlString> *l = m_lines.GetLast();
		int start_y = GetEngine()->GetVideoHeight() * m_screenHeightRatio - GUI::GetCharPixelHeight();
		int y = start_y - GUI::GetCharPixelHeight();
		while (l != NULL) {
			if (y + GUI::GetCharPixelHeight() < 0) { break; }

			mmlVector<3> c = mmlVector<3>(0.76f, 0.8f, 0.87f);
			GUI::SetColor(c[0], c[1], c[2]);
			GUI::SetCaretXY(0, y);

			GUI::Print(l->GetItem());

			y -= GUI::GetCharPixelHeight();
			l = l->GetPrev();
		}
		GUI::SetCaretXY(0, start_y);
		GUI::SetColor(0.796f, 1.0f, 0.094f);
		GUI::Print(m_userInput);
	}
}

void Console::OnInit( void )
{
	SetName("tool_console");
	// prevent more than one console
	const mtlItem<ObjectRef> *items = GetEngine()->GetObjects().GetFirst();
	while (items != NULL) {
		if (items->GetItem()->IsInstanceType<Console>() && items->GetItem().GetShared() != this) {
			Destroy();
			break;
		}
		items = items->GetNext();
	}
}

Console::Console( void ) :
	ConstructObject(Console),
	m_bgColor(43.0f/255.0f, 56.0f/255.0f, 88.0f/255.0f),
	m_lines(), m_maxHistory(20),
	m_screenHeightRatio(0.5f),
	m_envokationKey(SDLK_HOME),
	m_addNewLine(false)
{
	ClearAllCollisionMasks();
	ClearAllObjectFlags();
	DisableCollisions();
	DisableGraphics();
	for (int i = 0; i < m_maxHistory; ++i) {
		m_lines.AddLast();
	}
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
	PrintRaw("{0.76,0.8,0.87}");
	PrintRaw(text);
}

void Console::Print(int num)
{
	mtlString s;
	s.FromInt(num);
	mtlString final("{0.78,0.58,0.77}");
	final.Append(s);
	PrintRaw(final);
}

void Console::Print(float num)
{
	mtlString s;
	s.FromFloat(num);
	mtlString final("{0.78,0.58,0.77}");
	final.Append(s);
	PrintRaw(final);
}

void Console::PrintRaw(const mtlChars &str)
{
	if (m_lines.GetSize() == 0) {
		return;
	}
	if (m_addNewLine) {
		m_lines.AddLast();
		while (m_lines.GetSize() > m_maxHistory) {
			m_lines.RemoveFirst();
		}
		m_addNewLine = false;
	}
	m_lines.GetLast()->GetItem().Append(str);
}

void Console::NewLine( void )
{
	m_addNewLine = true;
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

/*SpriteEditor::InputBox::InputBox( void ) : Button(), m_caret(0)
{
}

void SpriteEditor::InputBox::Draw(bool hover)
{
	if (hover) {
		GUI::SetColor(1.0f, 1.0f, 1.0f);
	} else if (m_active) {
		GUI::SetColor(0.8f, 0.8f, 0.8f);
	} else {
		GUI::SetColor(0.5f, 0.5f, 0.5f);
	}
	GUI::SetCaretXY(m_position.x, m_position.y);
	GUI::Print(m_label);
	GUI::Print(m_input);
	m_btnOK.SetPosition(m_position.x, m_position.y + GUI::GetCharPixelHeight());
	m_btnOK.Draw(hover);
	m_btnCancel.SetPosition(m_btnOK.GetEndX() + GUI::GetCharPixelWidth(), m_btnOK.GetStartY());
	m_btnCancel.Draw(hover);
}*/

void SpriteEditor::EditableSprite::OnDraw( void )
{
	m_sprite.Draw(m_timer.GetTimeDeltaTick());
}

SpriteEditor::EditableSprite::EditableSprite( void ) : mtlInherit<Object, EditableSprite>(this)
{
	m_timer.Stop();
}

bool SpriteEditor::EditableSprite::LoadImage(const mtlChars &file)
{
	m_image_filename.Copy(file);
	return m_sprite.LoadSpriteSheet(file);
}

bool SpriteEditor::EditableSprite::LoadSprite(const mtlChars &file)
{
	return m_sprite.Load(file);
}

bool SpriteEditor::EditableSprite::SaveSprite(const mtlChars &file) const
{
	mtlString c_file(file);
	std::ofstream fout(c_file.GetChars());
	if (!fout.is_open()) {
		std::cout << "Failed to save sprite." << std::endl;
		return false;
	}
	fout << "image_file = " << m_image_filename.GetChars() << std::endl;
	fout << "frame_width = " << m_sprite.GetWidth() << std::endl;
	fout << "frame_height = " << m_sprite.GetHeight() << std::endl;
	fout << "frame_count = " << m_sprite.GetFrameCount() << std::endl;
	fout << "frames_per_second = " << m_sprite.GetFramesPerSecond() << std::endl;
	fout << "loopback_frame = " << m_sprite.GetLoopbackFrame() << std::endl;
	fout.close();
	return true;
}

void SpriteEditor::EditableSprite::TogglePlayback( void )
{
	if (m_timer.IsStopped()) {
		m_timer.Start();
	} else {
		m_timer.Stop();
	}
}

bool SpriteEditor::Save(const mtlChars &out_file)
{
	m_unsavedChanges = false;
	if (out_file.GetSize() == 0) {
		std::string file;
		std::cout << "> Output file: ";
		std::cin >> file;
		if (file.size() > 0) {
			m_currentFile.Copy(mtlChars::FromDynamic(file.c_str()));
		} else {
			m_currentFile.Copy("unnamed");
		}
	} else {
		m_currentFile.Copy(out_file);
	}
	return SaveCollider(m_currentFile) && SaveSprite(m_currentFile);
}

bool SpriteEditor::SaveCollider(const mtlChars &out_file) const
{
	const mtlItem< mtlList< mmlVector<2> > > *collider = m_colliders.GetFirst();
	int file_number = 0;
	bool success = true;
	while (collider != NULL) {
		mtlString file;
		file.Copy(out_file);
		mtlString number_str;
		number_str.FromInt(file_number);
		file.Append(number_str);
		file.Append(".obj");

		std::ofstream fout(file.GetChars());
		if (!fout.is_open()) {
			success = false;
		} else {
			fout << "o poly_collider" << std::endl;
			const mtlItem< mmlVector<2> > *point = collider->GetItem().GetFirst();
			while (point != NULL) {
				fout << "v " << point->GetItem()[0] << " " << point->GetItem()[1] << " 0.0" << std::endl;
				point = point->GetNext();
			}
			fout.close();
		}

		collider = collider->GetNext();
	}
	return success;
}

bool SpriteEditor::SaveSprite(const mtlChars &out_file) const
{
	mtlString file(out_file);
	file.Append(".sprite");
	return m_sprite->SaveSprite(file);
}

SpriteEditor::SpriteEditor(const SpriteEditor&) : mtlInherit<Object, SpriteEditor>(this) {}

SpriteEditor &SpriteEditor::operator=(const SpriteEditor&) { return *this; }

void SpriteEditor::OnInit( void )
{
	SetName("tool_SpriteEditor");
	DisableCollisions();
	SetObjectFlags(0);
	SetCollisionMasks(0);
	GetEngine()->SetCamera(GetEngine()->GetSelf(this));

	m_btnFile.SetLabel("File");
	m_btnNew.SetLabel("New");
	m_btnNew.SetPosition(0, m_btnFile.GetEndY());
	m_btnNew.Hide();
	m_btnLoad.SetLabel("Load");
	m_btnLoad.SetPosition(0, m_btnNew.GetEndY());
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
	m_btnImage.SetLabel("Image");
	m_btnImage.SetPosition(m_btnAnimation.GetStartX(), m_btnAnimation.GetEndY());
	m_btnImage.Hide();

	m_sprite = GetEngine()->AddObject<EditableSprite>()->GetAsDynamicType<EditableSprite>();
	if (m_sprite == NULL) {
		std::cout << "Critical error: sprite failed to convert to EditableSprite" << std::endl;
		GetEngine()->DestroyAllObjects();
	} else {
		m_sprite->DisableCollisions();
		m_sprite->SetCollisionMasks(0);
		m_sprite->SetObjectFlags(0);
	}

	m_colliders.AddFirst(); // there's always one
	m_currentCollider = &m_colliders.GetFirst()->GetItem();
}

void SpriteEditor::OnUpdate( void )
{
	// Controls
	// right arrow key for next frame
	// left arrow key for prev frame
	// Space for start/stop animation playback
	// Backspace for restart animation

	// GUI Controls
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
			m_btnNew.Deactivate();
			m_btnNew.ToggleHidden();
			m_btnLoad.Deactivate();
			m_btnLoad.ToggleHidden();
			m_btnSave.Deactivate();
			m_btnSave.ToggleHidden();

			m_btnCollider.Deactivate();
			m_btnClear.Deactivate();
			m_btnClear.Hide();

			m_btnAnimation.Deactivate();
			m_btnImage.Deactivate();
			m_btnImage.Hide();
		} else if (m_btnCollider.IsHovering(iMouse.x, iMouse.y)) {
			m_btnFile.Deactivate();
			m_btnNew.Deactivate();
			m_btnNew.Hide();
			m_btnLoad.Deactivate();
			m_btnLoad.Hide();
			m_btnSave.Deactivate();
			m_btnSave.Hide();

			m_btnCollider.ToggleActive();
			m_btnClear.Deactivate();
			m_btnClear.ToggleHidden();

			m_btnAnimation.Deactivate();
			m_btnImage.Deactivate();
			m_btnImage.Hide();
		} else if (m_btnAnimation.IsHovering(iMouse.x, iMouse.y)) {
			m_btnFile.Deactivate();
			m_btnNew.Deactivate();
			m_btnNew.Hide();
			m_btnLoad.Deactivate();
			m_btnLoad.Hide();
			m_btnSave.Deactivate();
			m_btnSave.Hide();

			m_btnCollider.Deactivate();
			m_btnClear.Deactivate();
			m_btnClear.Hide();

			m_btnAnimation.ToggleActive();
			m_btnImage.Deactivate();
			m_btnImage.ToggleHidden();
		} else if (m_btnLoad.IsHovering(iMouse.x, iMouse.y)) {
			std::cout << "Input file: ";
			std::string file;
			std::cin >> file;
			m_sprite->LoadSprite(mtlChars::FromDynamic(file.c_str()));
		} else if (m_btnSave.IsHovering(iMouse.x, iMouse.y)) {
			Save(m_currentFile);
		} else if (m_btnClear.IsHovering(iMouse.x, iMouse.y)) {
			m_currentCollider->RemoveAll();
		} else if (m_btnImage.IsHovering(iMouse.x, iMouse.y)) {
			std::cout << "> Image file: ";
			std::string file;
			std::cin >> file;
			m_sprite->LoadImage(mtlChars::FromDynamic(file.c_str()));
		} else if (m_btnCollider.IsActive()) {
			m_currentCollider->AddLast(wMouse);
			m_unsavedChanges = true;
		}
	}

	if (GetEngine()->IsHeld(SDLK_LCTRL) || GetEngine()->IsHeld(SDLK_RCTRL)) {
		if (GetEngine()->IsPressed(SDLK_z)) {
			if (m_currentCollider->GetSize() > 0) {
				m_currentCollider->RemoveLast();
				m_unsavedChanges = true;
			}
		} else if (GetEngine()->IsPressed(SDLK_s)) {
			Save(m_currentFile);
		} else if (GetEngine()->IsPressed(SDLK_o)) {
			// Load();
		}
	}
}

void SpriteEditor::OnGUI( void )
{
	if (m_btnCollider.IsActive()) {
		Point iMouse = GetEngine()->GetMousePosition();
		glColor3f(0.0f, 1.0f, 1.0f);

		glBegin(GL_LINES);

		glVertex2f((GLfloat)iMouse.x, (GLfloat)0);
		glVertex2f((GLfloat)iMouse.x, (GLfloat)GetEngine()->GetVideoHeight());

		glVertex2f((GLfloat)0, (GLfloat)iMouse.y);
		glVertex2f((GLfloat)GetEngine()->GetVideoWidth(), (GLfloat)iMouse.y);

		glEnd();
	}

	if ((m_currentCollider->GetSize() > 0 && m_btnCollider.IsActive()) || m_currentCollider->GetSize() > 1) {
		mtlItem< mmlVector<2> > *point = m_currentCollider->GetFirst();
		mmlVector<2> first =GetTransform().InverseTransformPoint(Transform::Local, point->GetItem());
		first = GetEngine()->GetScreenPoint(first);
		mmlVector<2> last = GetTransform().InverseTransformPoint(Transform::Local, m_currentCollider->GetLast()->GetItem());
		last = GetEngine()->GetScreenPoint(last);
		mmlVector<2> p = first;
		glBegin(GL_LINES);
		if (m_btnCollider.IsActive()) {
			glColor3f(1.0f, 0.0f, 0.0);
			Point iMouse = GetEngine()->GetMousePosition();
			glVertex2f(first[0], first[1]);
			glVertex2f((GLfloat)iMouse.x, (GLfloat)iMouse.y);
			glVertex2f(last[0], last[1]);
			glVertex2f((GLfloat)iMouse.x, (GLfloat)iMouse.y);
		}
		glColor3f(0.0f, 1.0f, 0.0f);
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
		glEnd();
	}

	Point mouse = GetEngine()->GetMousePosition();
	m_btnFile.Draw(m_btnFile.IsHovering(mouse.x, mouse.y));
	m_btnNew.Draw(m_btnNew.IsHovering(mouse.x, mouse.y));
	m_btnLoad.Draw(m_btnLoad.IsHovering(mouse.x, mouse.y));
	m_btnSave.Draw(m_btnSave.IsHovering(mouse.x, mouse.y));
	m_btnCollider.Draw(m_btnCollider.IsHovering(mouse.x, mouse.y));
	m_btnClear.Draw(m_btnClear.IsHovering(mouse.x, mouse.y));
	m_btnAnimation.Draw(m_btnAnimation.IsHovering(mouse.x, mouse.y));
	m_btnImage.Draw(m_btnImage.IsHovering(mouse.x, mouse.y));

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
		mtlItem< mmlVector<2> > *point = m_currentCollider->GetFirst();
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

SpriteEditor::SpriteEditor( void ) : mtlInherit<Object, SpriteEditor>(this), m_currentFrame(0) {}

RegisterObject(SpriteEditor);
