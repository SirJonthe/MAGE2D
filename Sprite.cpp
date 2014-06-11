#include <iostream>
#include "Sprite.h"
#include "MML/mmlMath.h"
#include "MTL/mtlParser.h"

bool Sprite::LoadMetadata(Sprite::Metadata &out, const mtlDirectory &file, mtlList<mtlDirectory> &filesOpened)
{
	mtlNode<mtlDirectory> *node = filesOpened.GetFirst();
	while (node != NULL) {
		if (node->GetItem().GetDirectory().Compare(file.GetDirectory())) {
			SetError("Recursive file meta data");
			return false;
		}
		node = node->GetNext();
	}
	filesOpened.AddLast(file);

	mtlString fileContents;
	if (!mtlParser::BufferFile(file, fileContents)) {
		SetError("File could not be read");
		return false;
	}
	mtlParser read;
	read.SetBuffer(fileContents);
	int lineNumber = 0;
	while (!read.IsEndOfFile()) {
		++lineNumber;
		mtlList<mtlSubstring> param_val;
		read.ReadLine().SplitByChar(param_val, "=");
		if (param_val.GetSize() != 2) {
			SetError("Parameter/value mismatch");
			return false;
		}
		mtlSubstring param = param_val.GetFirst()->GetItem();
		mtlSubstring val = param_val.GetFirst()->GetNext()->GetItem();
		if (param.Compare("default")) {
			if (lineNumber == 1) {
				SetError("\"default\" must be first parameter of file");
				return false;
			}
			if (val.GetChars()[0] != '\"' || val.GetChars()[val.GetSize()-1] != '\"') {
				SetError("Malformed string value");
				return false;
			}
			mtlDirectory defaultFile = val.GetSubstring(1, val.GetSize()-1);
			Metadata defaultOut;
			if (!LoadMetadata(defaultOut, defaultFile, filesOpened)) {
				return false;
			}
			// merge with OUT
		} else if (param.Compare("image_file")) {
			if (val.GetChars()[0] != '\"' || val.GetChars()[val.GetSize()-1] != '\"') {
				SetError("Malformed string value");
				return false;
			}
			out.file.Copy(val.GetSubstring(1, val.GetSize()-1));
		}
	}
	return true;
}

Sprite::Sprite( void ) : mtlInherit<Graphics>(), m_sheet(), m_frameWidth(0), m_frameHeight(0), m_numFrames(0), m_framesPerSecond(0.0f), /*m_startFrame(0),*/ m_loopBack(0)
{}

int Sprite::GetFramesPerRow( void ) const
{
	return m_sheet.GetAsset() != NULL ? m_sheet.GetAsset()->GetWidth() / m_frameWidth : 0;
}

int Sprite::GetFramesPerColumn( void ) const
{
	return m_sheet.GetAsset() != NULL ? m_sheet.GetAsset()->GetHeight() / m_frameHeight : 0;
}

int Sprite::GetFrameCount( void ) const
{
	return m_numFrames;
}

int Sprite::GetWidth( void ) const
{
	return m_frameWidth;
}

int Sprite::GetHeight( void ) const
{
	return m_frameHeight;
}

float Sprite::GetFramesPerSecond( void ) const
{
	return m_framesPerSecond;
}

float Sprite::GetFrameDelay( void ) const
{
	return m_framesPerSecond != 0.0f ? 1.0f / m_framesPerSecond : 0.0f;
}

float Sprite::GetAnimationTime( void ) const
{
	return GetFrameDelay() * GetFrameCount();
}

int Sprite::GetLoopbackFrame( void ) const
{
	return m_loopBack;
}

/*int Sprite::GetStartFrame( void ) const
{
	return m_startFrame;
}*/

int Sprite::GetFrameIndex(float time) const
{
	if (time <= 0.0f) { return 0; }

	const float delay = GetFrameDelay();
	const float totalAnimationTime = delay * GetFrameCount();
	float progress = (time / totalAnimationTime) /*+ delay * GetStartFrame()*/;
	if (progress > 1.0f) {
		progress = mmlMax2((delay * GetLoopbackFrame()) / totalAnimationTime, progress - int(progress));
	}
	return int(progress * GetFrameCount());
}

bool Sprite::Load(const mtlDirectory &file)
{
	Destroy();

	if (!file.GetExtension().Compare("sprite")) {
		SetError("Unexpected file extension");
		return false;
	}

	mtlList<mtlDirectory> filesOpened;
	Metadata out;
	if (!LoadMetadata(out, file, filesOpened)) {
		return false;
	}

	// calculate final values

	m_sheet = mtlAsset<Graphics>::Load<Image>(out.file);
	if (!m_sheet.IsNull()) {

		mtlArray< mmlVector<2> > vtx;
		vtx.Create(6);
		vtx[0][0] = 0.0f;
		vtx[0][1] = 0.0f;
		vtx[1][0] = float(GetWidth());
		vtx[1][1] = 0.0f;
		vtx[2][0] = 0.0f;
		vtx[2][1] = float(GetHeight());

		vtx[3][0] = float(GetWidth());
		vtx[3][1] = 0.0f;
		vtx[4][0] = float(GetWidth());
		vtx[4][1] = float(GetHeight());
		vtx[5][0] = 0.0f;
		vtx[5][1] = float(GetHeight());
		LoadVertexArray(vtx);

		mtlArray< mmlVector<2> > uv;
		uv.Create(GetFrameCount() * 6);
		const float uInc = 1.0f / GetWidth();
		for (int i = 0; i < GetFrameCount(); ++i) {
			const int vtx = i * 6;
			uv[vtx+0][0] = uInc * i;
			uv[vtx+0][1] = 1.0f;
			uv[vtx+1][0] = uInc * (i+1);
			uv[vtx+1][1] = 1.0f;
			uv[vtx+2][0] = uInc * i;
			uv[vtx+2][1] = 0.0f;

			uv[vtx+3][0] = uInc * (i+1);
			uv[vtx+3][1] = 1.0f;
			uv[vtx+4][0] = uInc * i;
			uv[vtx+4][1] = 1.0f;
			uv[vtx+5][0] = uInc * i;
			uv[vtx+5][1] = 0.0f;
		}
		LoadUVArray(uv);

		return true;
	}

	return false;
}

void Sprite::Destroy( void )
{
	m_sheet.Release();
	m_frameWidth = 0;
	m_frameHeight = 0;
	m_numFrames = 0;
	m_framesPerSecond = 0.0f;
	m_loopBack = 0;
	m_startFrame = 0;
}

void Sprite::Draw(float time) const
{
	if (!m_sheet.IsNull()) {
		DrawGraphics(0, GetFrameIndex(time)*2, GL_TRIANGLES, 2, m_sheet.GetAsset()->GetTextureID());
	}
}
