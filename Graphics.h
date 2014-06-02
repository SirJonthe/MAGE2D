#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/gl.h>
#include "MTL/mtlAsset.h"
#include "MTL/mtlType.h"
#include "Transform.h"
#include "Timer.h"

class Renderer;

// Idea:
// Graphics has Draw function, does not pass an animation parameter (time parameter instead of frame parameter)
// Separate animations are stored in separate files (separate Graphics objects)
// In order to play another animation, simply load that animation via GraphicsInstance::Load<Sprite>("animation.sprite");
	// or preload all animations and store them in directly accessible Graphics classes, then assign to main graphics object to change animation
	// basically circumvents the need for a binary tree inside Sprite class (in order to find animation string)

class Graphics : public mtlAssetInterface, public mtlBase
{
protected:
	struct BindID { GLuint vertexBufferID, uvBufferID, normalBufferID, textureID; };
	BindID m_id;
	static BindID &Bound( void ) { static BindID b = {0,0,0,0}; return b; }
public:
	virtual ~Graphics( void ) {}
	virtual int GetWidth( void ) const = 0;
	virtual int GetHeight( void ) const = 0;
	virtual void Destroy( void ) = 0;
	virtual void Bind( void ) = 0;
	virtual void Unbind( void ) = 0;
	virtual void Draw(Renderer &renderer, Transform transform, mmlVector<4> tint, float time) = 0; // RENDER HERE AND NOW (traditionally only called directly by renderer)
};

class GraphicsInstance
{
protected:
	mtlAsset<Graphics>	m_graphics;
	Transform			m_transform;
	mmlVector<4>		m_tint;
	Timer				m_timer;
	float				m_time;
public:
	GraphicsInstance( void ) : m_graphics(), m_transform(), m_tint(1.0f, 1.0f, 1.0f, 1.0f), m_timer(1.0f), m_time(0.0f) {}
	GraphicsInstance(const GraphicsInstance &instance) : m_graphics(instance.m_graphics), m_transform(instance.m_transform), m_tint(instance.m_tint), m_timer(instance.m_timer), m_time(instance.m_time) {}
	GraphicsInstance(const mtlAsset<Graphics> &graphics) : m_graphics(graphics), m_transform(), m_tint(1.0f, 1.0f, 1.0f, 1.0f), m_timer(1.0f), m_time(0.0f) {}
	GraphicsInstance &operator=(const GraphicsInstance &instance);
	GraphicsInstance &operator=(const mtlAsset<Graphics> &graphics);

	const mtlAsset<Graphics>	&GetGraphics( void ) const;
	void						SetGraphics(const mtlAsset<Graphics> &graphics);
	template < typename graphics_t >
	bool						LoadGraphics(const mtlChars &file);
	void						DeleteGraphics( void );

	Transform		&GetTransform( void );
	const Transform	&GetTransform( void ) const;

	const mmlVector<4>	&GetTint( void ) const;
	void				SetTint(const mmlVector<4> &tint);
	void				SetTint(const mmlVector<3> &tint, float a = 1.0f);
	void				SetTint(float r, float g, float b, float a = 1.0f);

	float	GetRed( void ) const;
	void	SetRed(float r);
	float	GetGreen( void ) const;
	void	SetGreen(float g);
	float	GetBlue( void ) const;
	void	SetBlue(float b);
	float	GetAlpha( void ) const;
	void	SetAlpha(float a);

	float	GetIntervalsPerSecond( void ) const;
	void	SetIntervalsPerSecond(float intervals);

	float	GetInterval( void ) const;
	void	SetInterval(float interval);

	void	Start( void );
	void	Stop( void );
	void	Restart( void );
	bool	IsStopped( void ) const;
	bool	IsTicking( void ) const;

	void	Draw(Renderer &renderer); // DEFER RENDERING TO WHEN THE RENDERER SEES FIT (renderer gets a chance to sort the rendering order)
};

template < typename graphics_t >
bool GraphicsInstance::LoadGraphics(const mtlChars &file)
{
	m_graphics = mtlAsset<Graphics>::Load<graphics_t>(file);
	return m_graphics.GetAsset() != NULL;
}

#endif // GRAPHICS_H
