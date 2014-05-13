#include "Camera.h"
#include "Renderer.h"
#include "Engine.h"

ENGINE_REGISTER_OBJECT_TYPE(Camera);

void Camera::OnFinal( void )
{
	if (m_renderer.IsNull()) {
		m_renderer.GetShared()->SetView(*this);
		m_renderer.GetShared()->RenderView();
	}
}

Camera::Camera( void ) : mtlInherit<Object>(), m_pixelsInViewWidth(SDL_GetVideoSurface()->w), m_pixelsInViewHeight(SDL_GetVideoSurface()->h), m_renderer()
{
	DisableCollisions();
	ClearAllObjectFlags();
	ClearAllCollisionMasks();
	SetName("object_camera");
}

Camera::Camera(float pixelsInViewWidth, float pixelsInViewHeight) : mtlInherit<Object>(), m_pixelsInViewWidth(pixelsInViewWidth), m_pixelsInViewHeight(pixelsInViewHeight), m_renderer()
{
	DisableCollisions();
	ClearAllObjectFlags();
	ClearAllCollisionMasks();
	SetName("object_camera");
}

void Camera::ZoomIn(float scale)
{
	m_pixelsInViewWidth /= scale;
	m_pixelsInViewHeight /= scale;
}

void Camera::ZoomOut(float scale)
{
	m_pixelsInViewWidth *= scale;
	m_pixelsInViewHeight *= scale;
}

float Camera::GetPixelsInViewWidth( void ) const
{
	return m_pixelsInViewWidth;
}

float Camera::GetPixelsInViewHeight( void ) const
{
	return m_pixelsInViewHeight;
}

void Camera::SetPixelsInViewWidth(float width)
{
	m_pixelsInViewWidth = width;
}

void Camera::SetPixelsInViewHeight(float height)
{
	m_pixelsInViewHeight = height;
}

void Camera::SetRenderer(const mtlShared<Renderer> &renderer)
{
	m_renderer = renderer;
}
