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

void SpriteEditor::OnInit( void )
{
	GetEngine()->SetCamera(GetObjectReference());
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
		// Free form
		// Autofit
		// Clear - remove current
	// File - load an image file
	// Animation
		// various ways to set the animation (used in .sprite format)

	// Output:
	// sprite format file
	// Wavefront OBJ file for collider

	if (GetEngine()->IsPressed(SDLK_MINUS)) {
		GetTransform().SetScale(Transform::Local, mmlMax2(1.0f, GetTransform().GetScaleX(Transform::Local) - 1.0f));
	} else if (GetEngine()->IsPressed(SDLK_PLUS)) {
		GetTransform().SetScale(Transform::Local, GetTransform().GetScaleX(Transform::Local) + 1.0f);
	}

	if (GetEngine()->IsDown(MouseButton::Right)) {
		GetTransform().Translate(Transform::Local, GetEngine()->GetWorldMouseMovement());
	}
}

void SpriteEditor::OnGUI( void )
{
	Point mouse = GetEngine()->GetMousePosition();
	//mmlVector<2> mouseLocation = (mmlVector<2>((float)mouse.x, (float)mouse.y) - GetTransform().GetPosition(Transform::Local)) / GetTransform().GetScaleX(Transform::Local);
	mmlVector<2> mouseLocation = GetEngine()->GetWorldMousePosition();
	GUI::Print((int)mouseLocation[0]);
	GUI::Print(", ");
	GUI::Print((int)mouseLocation[1]);
	GUI::NewLine();
	GUI::Print((int)GetTransform().GetScaleX(Transform::Local));
	GUI::Print("x");
}
