#ifndef TOOLS_H
#define TOOLS_H

#include "Object.h"
#include "Common.h"
#include "MTL/mtlDuplex.h"

class KillPlane : public mtlInherit<Object>
{
protected:
	void OnCollision(Object &collider);
public:
	KillPlane( void ); // grab all collisions, is defined as nothing, set collision shape to plane
};

class Console : public mtlInherit<Object>
{
private:
	mmlVector<4>			m_bgColor;
	mtlList< mtlString >	m_lines;
	int						m_maxHistory;
	float					m_screenHeightRatio;
	SDLKey					m_envokationKey;
	// format for altering color in text
	// \{0,255,128}
	// not printed, only alters color
	// each new line starts off as white

protected:
	void OnUpdate( void );
	void OnDraw( void );

public:
	Console( void ); // no collisions, toggle visibility on key press

	void SetBackgroundColor(float r, float g, float b, float a = 1.0f);
	void SetBackgroundColor(const mmlVector<4> &color);
	void SetMaxHistory(int lines);
	void SetEnvokationKey(SDLKey key);

	void Print(const mtlChars &text);
	void Print(int num);
	void Print(float num);
	void NewLine( void );
	void SetColor(float r, float g, float b); // appends \{r,g,b}
	void SetColor(const mmlVector<3> &color);
};

#endif // TOOLS_H
