#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Platform.h"

#include "MiniLib/MTL/mtlAsset.h"
#include "MiniLib/MTL/mtlType.h"
#include "MiniLib/MTL/mtlArray.h"
#include "Transform.h"
#include "Timer.h"

// Idea:
// Graphics has Draw function, does not pass an animation parameter (time parameter instead of frame parameter)
// Separate animations are stored in separate files (separate Graphics objects)
// In order to play another animation, simply load that animation via GraphicsInstance::Load<Sprite>("animation.sprite");
	// or preload all animations and store them in directly accessible Graphics classes, then assign to main graphics object to change animation
	// basically circumvents the need for a binary tree inside Sprite class (in order to find animation string)

class Graphics : public mtlAssetInterface, public mtlBase
{
private:
	Graphics(const Graphics&) : mtlBase(this) {}
	Graphics &operator=(const Graphics&) { return *this; }

protected:
	struct BindID { GLuint vtx, uv, tex; };

protected:
	BindID m_id; // generate buffers for all of these, even if empty

protected:
	void LoadVertexArray(const mtlArray< mmlVector<2> > &array) const;
	void LoadUVArray(const mtlArray< mmlVector<2> > &array) const;
	void LoadTexture(const GLvoid *pixels, GLsizei width, GLsizei height, GLint bytesPerPixel, GLint format) const;
	void DrawGraphics(int vtxOffset, int uvOffset, GLenum mode, GLsizei count, long long textureID = -1) const;

public:
	Graphics( void ); // construct buffers
	virtual ~Graphics( void );

	virtual int  GetWidth( void ) const = 0;
	virtual int  GetHeight( void ) const = 0;
	virtual bool IsGood( void ) const = 0;
	virtual void Destroy( void ) = 0;
	virtual void Draw(int frame, float part_frame) const = 0;

	int GetArea( void ) const;

	GLuint GetTextureID( void ) const;

	static void Unbind( void );
};

class GraphicsInstance
{
private:
	mtlAsset<Graphics> m_graphics;
	mmlVector<4>       m_tint;
	Timer              m_timer;

private:
	void Unbind( void ) const;

public:
	GraphicsInstance( void ) : m_graphics(), m_tint(1.0f, 1.0f, 1.0f, 1.0f), m_timer() {}
	GraphicsInstance(const GraphicsInstance &instance) : m_graphics(instance.m_graphics), m_tint(instance.m_tint), m_timer(instance.m_timer) {}
	GraphicsInstance(const mtlAsset<Graphics> &graphics) : m_graphics(graphics), m_tint(1.0f, 1.0f, 1.0f, 1.0f), m_timer() {}
	GraphicsInstance &operator=(const GraphicsInstance &instance);
	GraphicsInstance &operator=(const mtlAsset<Graphics> &graphics);

	const mtlAsset<Graphics> &GetGraphics( void ) const;
	void                      SetGraphics(const mtlAsset<Graphics> &graphics);
	template < typename graphics_t >
	bool                      LoadGraphics(const mtlChars &file);
	void                      DeleteGraphics( void );
	int                       GetGraphicsWidth( void ) const;
	int                       GetGraphicsHeight( void ) const;
	int                       GetGraphicsArea( void ) const;
	bool                      IsGood( void ) const;

	const mmlVector<4> &GetTint( void ) const;
	void                SetTint(const mmlVector<4> &tint);
	void                SetTint(const mmlVector<3> &tint, float a = 1.0f);
	void                SetTint(float r, float g, float b, float a = 1.0f);

	float GetRed( void ) const;
	void  SetRed(float r);
	float GetGreen( void ) const;
	void  SetGreen(float g);
	float GetBlue( void ) const;
	void  SetBlue(float b);
	float GetAlpha( void ) const;
	void  SetAlpha(float a);

	float GetIntervalsPerSecond( void ) const;
	void SetIntervalsPerSecond(float intervals);

	float GetInterval( void ) const;
	void  SetInterval(float interval);

	void Start( void );
	void Stop( void );
	void Restart( void );
	bool IsStopped( void ) const;
	bool IsTicking( void ) const;

	void Draw( void );
};

template < typename graphics_t >
bool GraphicsInstance::LoadGraphics(const mtlChars &file)
{
	m_graphics = mtlAsset<Graphics>::Load<graphics_t>(file);
	return m_graphics.GetAsset() != NULL;
}

#endif // GRAPHICS_H
