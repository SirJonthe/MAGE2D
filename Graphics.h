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

#include <iostream>

// @data Graphics
// @info Generic data structure designed to contain any graphics data.
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

	// @algo GetWidth
	// @out The maximum width of the graphics object (virtual).
	virtual int  GetWidth( void )                  const = 0;
	
	// @algo GetHeight
	// @out the maximum height of the graphics object (virtual).
	virtual int  GetHeight( void )                 const = 0;
	
	// @algo IsGood
	// @out The error state of the graphics object (TRUE if everything is OK) (virtual).
	virtual bool IsGood( void )                    const = 0;
	
	// @algo Destroy
	// @info Releases any resources used by the graphics object (virtual).
	virtual void Destroy( void )                         = 0;
	
	// @algo Draw
	// @info Renders the graphics object (virtual).
	// @in
	//   frame -> The integer index of the part of the graphics to be drawn (think of this as index of frame in an animation).
	//   part_frame -> The fractional part of the frame (possibly used for frame interpolation for smooth transitions).
	virtual void Draw(int frame, float part_frame) const = 0;

	// @algo Draw
	// @info Renders the first frame of the graphics object (virtual).
	void Draw( void ) const { Draw(0, 0.0f); }

	// @algo GetArea
	// @out The integer area of the bounding box containing the full graphics object.
	int GetArea( void ) const;

	// @algo GetTextureID
	// @out The API specific identifier used by OpenGL to perform operations on graphics object.
	GLuint GetTextureID( void ) const;

	// @algo Unbind
	// @info The API specific method for unbinding the graphics resource from the graphics state machine.
	static void Unbind( void );
};

// @data GraphicsInstance
// @info An instance of a graphics object with some additional mutable data such as time and color. Automatically animates graphics objects.
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

	// @algo GetGraphics
	// @out Returns the graphics object.
	const mtlAsset<Graphics> &GetGraphics( void ) const;
	
	// @algo SetGraphics
	// @info Sets a new graphics object.
	// @in graphics -> The new graphics object.
	void                      SetGraphics(const mtlAsset<Graphics> &graphics);
	
	// @algo LoadGraphics
	// @info Loads a specified type of graphics object from a file.
	// @in file -> The file containing serialized graphics data.
	// @out The result of the file operation (TRUE on successful read).
	template < typename graphics_t >
	bool                      LoadGraphics(const mtlChars &file);
	
	// @algo DeleteGraphics
	// @info Frees all resources used by the graphics object.
	void                      DeleteGraphics( void );
	
	// @algo GetGraphicsWidth
	// @out The maximum width of the graphics.
	int                       GetGraphicsWidth( void ) const;
	
	// @algo GetGraphicsHeight
	// @out The maximum height of the graphics.
	int                       GetGraphicsHeight( void ) const;
	
	// @algo GetGraphicsArea
	// @out The integer area of the bounding box containing the graphics object.
	int                       GetGraphicsArea( void ) const;
	
	// @algo IsGood
	// @out The error state of the graphics object (TRUE if everything is OK).
	bool                      IsGood( void ) const;

	// @algo GetTint
	// @out The multiplicative tint of the graphics object.
	const mmlVector<4> &GetTint( void ) const;
	
	// @algo SetTint
	// @info Sets a multiplicative tint to the graphics object.
	// @in tint -> The normalized tint values (RGBA, 0-1).
	void                SetTint(const mmlVector<4> &tint);
	
	// @algo SetTint
	// @info Sets a multiplicative tint to the graphics object.
	// @in
	//   tint -> The normalized tint values (RGB, 0-1).
	//   a -> The normalized alpha value (0-1)
	void                SetTint(const mmlVector<3> &tint, float a = 1.0f);
	
	// @algo SetTint
	// @info Sets a multiplicative tint to the graphics object.
	// @in
	//   r -> The normalized red value (0-1)
	//   g -> The normalized green value (0-1)
	//   b -> The normalized blue value (0-1)
	//   a -> The normalized alpha value (0-1)
	void                SetTint(float r, float g, float b, float a = 1.0f);

	// @algo GetRed
	// @out The red multiplicative tint in normalized range (0-1)
	float GetRed( void ) const;
	
	// @algo SetRed
	// @in The new red multiplicative tint value in normalized range (0-1)
	void  SetRed(float r);
	
	// @algo GetGreen
	// @out The green multiplicative tint in normalized range (0-1)
	float GetGreen( void ) const;
	
	// @algo SetGreen
	// @in The new green multiplicative tint value in normalized range (0-1)
	void  SetGreen(float g);
	
	// @algo GetBlue
	// @out The blue multiplicative tint in normalized range (0-1)
	float GetBlue( void ) const;
	
	// @algo SetBlue
	// @in The new blue multiplicative tint value in normalized range (0-1)
	void  SetBlue(float b);
	
	// @algo GetAlpha
	// @out The alpha multiplicative tint in normalized range (0-1)
	float GetAlpha( void ) const;
	
	// @algo SetAlpha
	// @in The new alpha multiplicative tint value in normalized range (0-1)
	void  SetAlpha(float a);

	// @algo GetIntervalsPerSecond
	// @out The number of beats/ticks/intervals generated by the timer every second.
	float GetIntervalsPerSecond( void ) const;
	
	// @algo SetIntervalsPerSecond
	// @info Sets a new timer interval per second
	// @in intervals -> The number of intervals per second.
	void  SetIntervalsPerSecond(float intervals);

	// @algo GetInterval
	// @out The absolute timer interval.
	float GetInterval( void ) const;
	
	// @algo SetInterval
	// @info Sets a new absolute timer interval.
	// @in interval -> The interval in seconds.
	void  SetInterval(float interval);

	// @algo Start
	// @info Start the timer.
	void Start( void );
	
	// @algo Stop
	// @info Stop the timer.
	void Stop( void );
	
	// @algo Restart
	// @info Restart the timer.
	void Restart( void );
	
	// @algo IsStopped
	// @out TRUE if timer is not ticking.
	bool IsStopped( void ) const;
	
	// @algo IsTicking
	// @out TRUE if timer is ticking
	bool IsTicking( void ) const;

	// @algo Draw
	// @info Renders the current graphics object given the current internal state of the graphics instance.
	void Draw( void );
};

template < typename graphics_t >
bool GraphicsInstance::LoadGraphics(const mtlChars &file)
{
	m_graphics = mtlAsset<Graphics>::Load<graphics_t>(file);
	return m_graphics.GetAsset() != NULL;
}

#endif // GRAPHICS_H
