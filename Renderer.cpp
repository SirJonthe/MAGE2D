#include "Renderer.h"
#include <GL/glext.h>

bool Renderer::GraphicsInstance::operator <(const Renderer::GraphicsInstance &inst) const
{
	return graphics < inst.graphics;
}

int Renderer::GetGraphicsQueueSize( void ) const
{
	return m_graphics.GetSize();
}

const Graphics *Renderer::GetGraphics(int i) const
{
	return m_graphics[i].graphics;
}

mmlMatrix<3,3> Renderer::GetGraphicsTransform(int i) const
{
	return m_graphics[i].transform * m_view;
}

void Renderer::AddToGraphicsQueue(const Graphics &graphics, const mmlMatrix<3,3> &worldTransform)
{
	if (m_graphics.GetSize() + 1 > m_graphics.GetCapacity()) {
		m_graphics.SetCapacity(m_graphics.GetCapacity() * 2);
	}
	m_graphics.Resize(m_graphics.GetSize() + 1);
	int index = m_graphics.GetSize() - 1;
	m_graphics[index].graphics = &graphics;
	m_graphics[index].transform = worldTransform;
}

void Renderer::ClearGraphicsQueue( void )
{
	m_graphics.Resize(0);
}

void Renderer::SortGraphicsQueue( void )
{
	mtlArray<GraphicsInstance> sorted;
	m_graphics.MergeSort(sorted);
	m_graphics.Copy(sorted);
}

void Renderer::SetViewTransform(mmlMatrix<3,3> viewTransform)
{
	m_view = viewTransform;
}

Renderer::Renderer( void ) : m_graphics(), m_view(mmlMatrix<3,3>::IdentityMatrix()), m_vertId(0), m_texId(0)
{
	m_graphics.poolMemory = true;
	m_graphics.SetCapacity(512);

	glGenBuffers(1, &m_vertId);
	glGenBuffers(1, &m_texId);

	m_tex[0] = 0.0f;
	m_tex[1] = 1.0f;

	m_tex[2] = 1.0f;
	m_tex[3] = 1.0f;

	m_tex[4] = 1.0f;
	m_tex[5] = 0.0f;

	m_tex[6] = 0.0f;
	m_tex[7] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, m_texId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_tex), m_tex, GL_STATIC_DRAW);
}

Renderer::~Renderer( void )
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_vertId);
	glDeleteBuffers(1, &m_texId);
}

void Renderer::SetView(const Camera &camera)
{
	SetViewTransform(camera.GetTransform().GetWorldTransform());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, (camera.GetPixelsInViewWidth() - 1) + 0.5, (camera.GetPixelsInViewHeight() - 1) + 0.5, -0.5, 0.0, 1.0);
}

void Renderer::AddGraphics(const Graphics &graphics, const mmlMatrix<3,3> &worldTransform)
{
	if (graphics.GetInstanceType() == Image::GetClassType()) {
		AddToGraphicsQueue(graphics, worldTransform);
	}/* else if (graphics.GetInstanceType() == Polygon::GetClassType()) {

	}*/
	// accept more types
}

void Renderer::RenderView( void )
{
	glBindBuffer(GL_ARRAY_BUFFER, m_texId);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertId);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	SortGraphicsQueue();

	const void *currentGraphics = NULL;

	for (int i = 0; i < GetGraphicsQueueSize(); ++i) {
		mmlMatrix<3,3> t = GetGraphicsTransform(i);
		GLfloat m[16] = {
			t[0][0], t[0][1], 0.0f, 0.0f,
			t[1][0], t[1][1], 0.0f, 0.0f,
			0.0f,    0.0f,    1.0f, 0.0f,
			t[0][2], t[1][2], 0.0f, 1.0f
		};
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(m);

		const Image *graphics = dynamic_cast<const Image*>(GetGraphics(i));
		if (currentGraphics != (const void*)graphics) {
			currentGraphics = graphics;

			m_vert[0] = 0.0f;
			m_vert[1] = (float)graphics->GetHeight();

			m_vert[2] = (float)graphics->GetWidth();
			m_vert[3] = (float)graphics->GetHeight();

			m_vert[4] = (float)graphics->GetWidth();
			m_vert[5] = 0.0f;

			m_vert[6] = 0.0f;
			m_vert[7] = 0.0f;

			glBufferData(GL_ARRAY_BUFFER, sizeof(m_vert), m_vert, GL_STATIC_DRAW);
			graphics->BindTexture();
		}

		glDrawArrays(GL_QUADS, 0, 4);

	}
	Image::UnbindTexture();
	ClearGraphicsQueue();
}
