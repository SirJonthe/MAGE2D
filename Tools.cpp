#include "Tools.h"
#include "Engine.h"
#include "GUI.h"

ENGINE_REGISTER_OBJECT_TYPE(KillPlane);

void KillPlane::OnUpdate( void )
{
	mtlList<Object*> objects;
	GetEngine()->FilterByPlaneCollision(GetEngine()->GetObjects(), objects, m_plane);
	mtlNode<Object*> *object = objects.GetFirst();
	while (object != NULL) {
		object->GetItem()->Destroy();
	}
}

KillPlane::KillPlane( void ) : mtlInherit<Object>()
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

ENGINE_REGISTER_OBJECT_TYPE(Console);

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

	mtlNode<mtlString> *l = m_lines.GetLast();
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
	mtlInherit<Object>(),
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
