#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "MTL/mtlAsset.h"
#include "MTL/mtlType.h"
#include "Common.h"
#include "Transform.h"

class Graphics : public mtlAssetInterface, public mtlBase
{
public:
	class Instance
	{
	protected:
		mtlAsset<Graphics>	m_graphics;
		Transform			m_transform;
		mmlVector<4>		m_tintColor;
	public:
		Instance( void ) : m_graphics(), m_transform(), m_tintColor(1.0f, 1.0f, 1.0f, 1.0f) {}

		const Graphics		*GetGraphics( void ) const { return m_graphics.GetAsset(); }
		void				SetGraphics(const mtlAsset<Graphics> &graphics) { m_graphics = graphics; }

		const Transform		&GetTransform( void ) const { return m_transform; }
		Transform			&GetTransform( void ) { return m_transform; }

		const mmlVector<4>	&GetTintColor( void ) const { return m_tintColor; }
		void				SetTintColor(const mmlVector<4> &tintColor) { m_tintColor = tintColor; }
		void				SetTintColor(float r, float g, float b, float a = 1.0f);

		float				GetTintRed( void ) const { return m_tintColor[0]; }
		void				SetTintRed(float r) { m_tintColor[0] = r; }

		float				GetTintGreen( void ) const { return m_tintColor[1]; }
		void				SetTintGreen(float g) { m_tintColor[1] = g; }

		float				GetTintBlue( void ) const { return m_tintColor[2]; }
		void				SetTintBlue(float b) { m_tintColor[2] = b; }

		float				GetTintAlpha( void ) const { return m_tintColor[3]; }
		void				SetTintAlpha(float a) { m_tintColor[3] = a; }

		virtual void		Update(float time) = 0;
		virtual void		Draw( void ) = 0;
	};
public:
	virtual ~Graphics( void ) {}
};

#endif // GRAPHICS_H
