#include <iostream>
#include "Sprite.h"
#include "MML/mmlMath.h"
#include "MTL/mtlParser.h"

bool Sprite::LoadMetadata(Sprite::Metadata &out, const mtlDirectory &file, mtlList<mtlDirectory> &filesOpened)
{
	mtlItem<mtlDirectory> *node = filesOpened.GetFirst();
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
	while (!read.IsEnd()) {
		++lineNumber;
		mtlList<mtlChars> param_val;
		read.ReadLine().SplitByChar(param_val, "=");
		if (param_val.GetSize() != 2) {
			SetError("Parameter/value mismatch");
			return false;
		}
		mtlChars param = param_val.GetFirst()->GetItem();
		mtlChars val = param_val.GetFirst()->GetNext()->GetItem();
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
			out.file.Copy(defaultOut.file);
			out.frameCount = defaultOut.frameCount;
			out.framesPerSecond = defaultOut.framesPerSecond;
			out.frameWidth = defaultOut.frameWidth;
			out.loopBack = defaultOut.loopBack;
		} else if (param.Compare("image_file")) {
			if (val.GetChars()[0] != '\"' || val.GetChars()[val.GetSize()-1] != '\"') {
				SetError("Malformed string value");
				return false;
			}
			out.file.Copy(val.GetSubstring(1, val.GetSize()-1));
		} else if (param.Compare("frame_width")) {
			if (!val.ToInt(out.frameWidth)) {
				SetError("frame_width must be integer");
				return false;
			} else if (out.frameWidth < 0) {
				SetError("frame_width must be positive integer");
				return false;
			}
		} else if (param.Compare("frame_count")) {
			if (!val.ToInt(out.frameCount) || out.frameCount < 0) {
				SetError("frame_count must be positive integer");
				return false;
			}
		} else if (param.Compare("frame_height")) {
			if (!val.ToInt(out.frameHeight) || out.frameHeight < 0) {
				SetError("frame_height must be positive integer");
				return false;
			}
		} else if (param.Compare("frames_per_second")) {
			if (!val.ToFloat(out.framesPerSecond) || out.framesPerSecond < 0.0f) {
				SetError("frames_per_second must be positive float");
				return false;
			}
		} else if (param.Compare("frame_delay_ms")) {
			int i;
			if (!val.ToInt(i) || i < 0) {
				SetError("frame_delay_ms must be positive integer");
				return false;
			}
			out.framesPerSecond = 1.0f / (float(i) / 1000.0f);
		} else if (param.Compare("frame_delay_seconds")) {
			int i;
			if (!val.ToInt(i) || i < 0) {
				SetError("frame_delay_seconds must be positive integer");
				return false;
			}
			out.framesPerSecond = 1.0f / float(i);
		} else if (param.Compare("is_looping")) {
			if (!val.ToBool(out.isLooping)) {
				SetError("is_looping must be boolean");
				return false;
			}
		} else if (param.Compare("loopback_frame")) {
			if (!val.ToInt(out.loopBack) || out.loopBack < 0) {
				SetError("loopback_frame must be positive integer");
				return false;
			}
		} else {
			mtlString error("Unknown parameter: \"");
			error.Append(param).Append("\"");
			SetError(error);
			return false;
		}
	}
	return true;
}

Sprite::Sprite( void ) : mtlInherit(this), m_sheet(), m_frameWidth(0), m_frameHeight(0), m_numFrames(0), m_framesPerSecond(0.0f), /*m_startFrame(0),*/ m_loopBack(0)
{}

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
	float progress = (time / totalAnimationTime);
	if (progress > 1.0f) {
		progress = mmlMax2((delay * GetLoopbackFrame()) / totalAnimationTime, progress - int(progress));
	}
	return int(progress * GetFrameCount());
}

bool Sprite::Load(const mtlDirectory &file)
{
	std::cout << "Sprite::Load: " << file.GetDirectory().GetChars() << std::endl;

	Destroy();

	if (!file.GetExtension().Compare("sprite")) {
		SetError("Unexpected file extension");
		return false;
	}

	mtlList<mtlDirectory> filesOpened;
	Metadata out;
	if (!LoadMetadata(out, file, filesOpened)) {
		std::cout << "\tfailed: " << GetError().GetChars() << std::endl;
		return false;
	}

	// calculate final values
	m_sheet = mtlAsset<Graphics>::Load<Image>(out.file);
	if (!m_sheet.IsNull()) {
		if (out.frameHeight	== -1) {
			m_frameHeight = m_sheet.GetAsset()->GetHeight();
		} else {
			m_frameHeight = mmlMin2(m_sheet.GetAsset()->GetHeight(), out.frameHeight);
		}
		if (out.frameCount == -1 && out.frameWidth == -1) {
			m_numFrames = 1;
			m_frameWidth = m_sheet.GetAsset()->GetWidth();
		} else if (out.frameCount == -1) {
			m_frameWidth = out.frameWidth;
			m_numFrames = m_sheet.GetAsset()->GetWidth() / m_frameWidth;
		} else if (out.frameWidth == -1) {
			m_numFrames = out.frameCount;
			m_frameWidth = m_sheet.GetAsset()->GetWidth() / m_numFrames;
		} else {
			m_numFrames = out.frameCount;
			m_frameWidth = out.frameWidth;
		}
		m_framesPerSecond = out.framesPerSecond;
		if (out.isLooping) {
			m_loopBack = out.loopBack;
		} else {
			m_loopBack = m_numFrames - 1;
		}

		mtlArray< mmlVector<2> > vtx;
		vtx.Create(6);
		float w = float(GetWidth()) * 0.5f;
		float h = float(GetHeight()) * 0.5f;
		vtx[0][0] = -w;
		vtx[0][1] = -h;
		vtx[1][0] = w;
		vtx[1][1] = -h;
		vtx[2][0] = -w;
		vtx[2][1] = h;

		vtx[3][0] = w;
		vtx[3][1] = -h;
		vtx[4][0] = w;
		vtx[4][1] = h;
		vtx[5][0] = -w;
		vtx[5][1] = h;
		LoadVertexArray(vtx);

		mtlArray< mmlVector<2> > uv;
		uv.Create(GetFrameCount() * 6);
		const float uInc = (float)GetWidth() / (float)m_sheet.GetAsset()->GetWidth();
		const float vMax = (float)GetHeight() / (float)m_sheet.GetAsset()->GetHeight();
		for (int i = 0; i < GetFrameCount(); ++i) {
			const int v = i * 6;
			uv[v+0][0] = uInc * i;
			uv[v+0][1] = 0.0f;
			uv[v+1][0] = uInc * (i+1);
			uv[v+1][1] = 0.0f;
			uv[v+2][0] = uInc * i;
			uv[v+2][1] = vMax;

			uv[v+3][0] = uInc * (i+1);
			uv[v+3][1] = 0.0f;
			uv[v+4][0] = uInc * (i+1);
			uv[v+4][1] = vMax;
			uv[v+5][0] = uInc * i;
			uv[v+5][1] = vMax;
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
}

void Sprite::Draw(float time) const
{
	if (!m_sheet.IsNull()) {
		DrawGraphics(0, GetFrameIndex(time)*6, GL_TRIANGLES, 2, m_sheet.GetAsset()->GetTextureID());
	}
}

bool Sprite::IsGood( void ) const
{
	return !m_sheet.IsNull() && m_sheet.GetAsset()->IsGood();
}
