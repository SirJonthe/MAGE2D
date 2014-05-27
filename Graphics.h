#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/gl.h>
#include "MTL/mtlAsset.h"
#include "MTL/mtlType.h"
#include "Transform.h"

class Renderer;

class Graphics : public mtlAssetInterface, public mtlBase
{
public:
	virtual ~Graphics( void ) {}
	virtual void Bind( void ) = 0;
	virtual void Unbind( void ) = 0;
	virtual void Draw(int animation, int frame, Transform transform, mmlVector<4> tint) = 0;
};

class GraphicsInstance
{
protected:
	mtlAsset<Graphics>	m_graphics;
	Transform			m_transform;
	mmlVector<4>		m_tint;
public:
	GraphicsInstance( void ) : m_graphics(), m_transform(), m_tint(1.0f, 1.0f, 1.0f, 1.0f) {}

	const mtlAsset<Graphics> &GetGraphics( void ) const;
	void SetGraphics(const mtlAsset<Graphics> &graphics);
	template < typename graphics_t >
	bool LoadGraphics(const mtlChars &file);

	Transform &GetTransform( void );
	const Transform &GetTransform( void ) const;

	const mmlVector<4> &GetTint( void ) const;
	void SetTint(const mmlVector<4> &tint);
	void SetTint(const mmlVector<3> &tint, float a = 1.0f);
	void SetTint(float r, float g, float b, float a = 1.0f);

	float GetRed( void ) const;
	void SetRed(float r);
	float GetGreen( void ) const;
	void SetGreen(float g);
	float GetBlue( void ) const;
	void SetBlue(float b);
	float GetAlpha( void ) const;
	void SetAlpha(float a);

	virtual void SetAnimation(const mtlChars &name) = 0;
	virtual void PlayAnimation( void ) = 0;
	virtual void PauseAnimation( void ) = 0;
	virtual void StopAnimation( void ) = 0;
	virtual void Update( void ) = 0;
	virtual void Draw( void ) = 0;
};

template < typename graphics_t >
bool GraphicsInstance::LoadGraphics(const mtlChars &file)
{
	m_graphics = mtlAsset<Graphics>::Load<graphics_t>(file);
	return m_graphics.GetAsset() != NULL;
}

#endif // GRAPHICS_H
