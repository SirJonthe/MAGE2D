#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"

class Renderer;

class Camera : public mtlInherit<Object>
{
private:
	float				m_pixelsInViewWidth;
	float				m_pixelsInViewHeight;
	mtlShared<Renderer>	m_renderer;
protected:
	void OnFinal( void ); // SetView(); Render();
public:
	Camera( void );
	Camera(float pixelsInViewWidth, float pixelsInViewHeight); // collisions disabled, collision flags cleared, type flags cleared
	void ZoomIn(float scale);
	void ZoomOut(float scale);
	float GetPixelsInViewWidth( void ) const;
	float GetPixelsInViewHeight( void ) const;
	void SetPixelsInViewWidth(float width);
	void SetPixelsInViewHeight(float height);
	void SetRenderer(const mtlShared<Renderer> &renderer);
};

#endif // CAMERA_H
