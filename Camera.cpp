#include "Camera.h"
#include "Renderer.h"

void Camera::OnFinal( void )
{
	if (m_renderer.IsNull()) {
		m_renderer->SetView(*this);
		m_renderer->RenderView();
	}
}

Camera::Camera(float pixelsInViewWidth, float pixelsInViewHeight) : Inherit<Object>(), m_pixelsInViewWidth(pixelsInViewWidth), m_pixelsInViewHeight(pixelsInViewHeight), m_renderer()
{
	DisableCollisions();
	ClearAllTypeFlags();
	ClearAllCollisionFlags();
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

void Camera::SetRenderer(const Shared<Renderer> &renderer)
{
	m_renderer = renderer;
}
