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

const mtlNode<Instance> *Renderer::GetGraphics( void ) const
{
	return m_graphics.GetFirst();
}

mmlMatrix<3,3> Renderer::ToView(const mmlMatrix<3,3> &world) const
{
	return world * m_view;
}

void Renderer::ClearGraphicsQueue( void )
{
	m_graphics.Resize(0);
}

void Renderer::SortGraphicsQueue( void )
{
	mtlArray<Graphics::Instance> sorted;
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

	/*m_tex[0] = 0.0f;
	m_tex[1] = 1.0f;

	m_tex[2] = 1.0f;
	m_tex[3] = 1.0f;

	m_tex[4] = 1.0f;
	m_tex[5] = 0.0f;

	m_tex[6] = 0.0f;
	m_tex[7] = 0.0f;*/

	//glBindBuffer(GL_ARRAY_BUFFER, m_texId);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(m_tex), m_tex, GL_STATIC_DRAW);
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

void Renderer::AddGraphics(const Transform &transform, const mtlAsset<Image> &image, const mmlVector<3> &tint)
{
	if (image.GetAsset() == NULL) { return; }
	mmlVector<2> uv1;
	uv1[0] = uv1[1] = 0.0f;
	mmlVector<2> uv2;
	uv2[0] = uv2[1] = 1.0f;
	AddGraphics(transform, image, uv1, uv2, tint);
}

void Renderer::AddGraphics(const Transform &transform, const mtlAsset<Image> &image, const mmlVector<2> &uv1, const mmlVector<2> &uv2, const mmlVector<3> &tint)
{
	if (image.GetAsset() == NULL) { return; }

	Instance *inst = &m_graphics.AddLast();
	inst->texture = image;
	inst->tint = tint;
	inst->worldTransform = transform.GetWorldTransform();

	inst->vertex.Create(4*2);
	inst->vertex[0] = 0.0f;
	inst->vertex[1] = fabs(uv2[1] - uv1[1]) * image.GetAsset()->GetHeight();

	inst->vertex[2] = fabs(uv2[0] - uv1[0]) * image.GetAsset()->GetWidth();
	inst->vertex[3] = fabs(uv2[1] - uv1[1]) * image.GetAsset()->GetHeight();

	inst->vertex[4] = fabs(uv2[0] - uv1[0]) * image.GetAsset()->GetWidth();
	inst->vertex[5] = 0.0f;

	inst->vertex[6] = 0.0f;
	inst->vertex[7] = 0.0f;

	inst->uv.Create(4*2);
	inst->uv[0] = uv1[0];
	inst->uv[1] = uv2[1];

	inst->uv[2] = uv2[0];
	inst->uv[3] = uv2[1];

	inst->uv[4] = uv2[0];
	inst->uv[5] = uv1[1];

	inst->uv[6] = uv1[0];
	inst->uv[7] = uv1[1];
}

void Renderer::AddGraphics(const Transform &transform, const mtlArray< mmlVector<2> > &vert, const mmlVector<3> &tint)
{
	if (vert.GetSize() == 0) { return; }

	Instance *inst = m_graphics.AddLast();
	inst->worldTransform = transform.GetWorldTransform();
	inst->vertex.Create(vert.GetSize() * 2);
	for (int i = 0; i < vert.GetSize(); ++i) {
		inst->vertex[(i << 1)] = vert[i][0];
		inst->vertex[(i << 1) + 1] = vert[i][1];
	}
	inst->tint = tint;
}

void Renderer::AddGraphics(const Transform &transform, const mtlAsset<Image> &image, const mtlArray< mmlVector<2> > &vert, const mtlArray< mmlVector<2> > &uv, const mmlVector<3> &tint)
{
	if (image.GetAsset() == NULL) { return; }
	if (vert.GetSize() == 0) { return; }
	if (uv.GetSize() == 0) { return; }
	if (vert.GetSize() != uv.GetSize()) { return; }

	Instance *inst = m_graphics.AddLast();
	inst->texture = image;
	inst->vertex.Create(vert.GetSize() * 2);
	for (int i = 0; i < vert.GetSize(); ++i) {
		inst->vertex[(i << 1)] = vert[i][0];
		inst->vertex[(i << 1) + 1] = vert[i][1];
	}
	inst->uv.Create(uv.GetSize() * 2);
	for (int i = 0; i < uv.GetSize(); ++i) {
		inst->uv[(i << 1)] = uv[i][0];
		inst->uv[(i << 1) + 1] = uv[i][1];
	}
	inst->tint = tint;
	inst->worldTransform = transform.GetWorldTransform();
}

void Renderer::RenderView( void )
{
	SortGraphicsQueue();

	const mtlNode<Instance> *graphics = GetGraphics();
	while (graphics != NULL) {

		mmlMatrix<3,3> t = ToView(graphics->GetItem().worldTransform);
		GLfloat m[16] = {
			t[0][0], t[0][1], 0.0f, 0.0f,
			t[1][0], t[1][1], 0.0f, 0.0f,
			0.0f,    0.0f,    1.0f, 0.0f,
			t[0][2], t[1][2], 0.0f, 1.0f
		};
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(m);

		glColor4f(graphics->GetItem().tint[0], graphics->GetItem().tint[1], graphics->GetItem().tint[2], graphics->GetItem().tint[3]);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertId);
		glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);
		glBufferData(GL_ARRAY_BUFFER, graphics->GetItem().vertex.GetSize() * sizeof(float), &graphics->GetItem().vertex[0], GL_STATIC_DRAW);

		if (graphics->GetItem().image.GetAsset() != NULL) {
			graphics->GetItem().image.GetAsset()->Bind();
			glBindBuffer(GL_ARRAY_BUFFER, m_texId);
			glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);
			glBufferData(GL_ARRAY_BUFFER, graphics->GetItem().uv.GetSize() * sizeof(float), &graphics->GetItem().uv[0], GL_STATIC_DRAW);
		} else {
			Image::Unbind();
			// unbind uv...
			glBindBuffer(GL_ARRAY_BUFFER, m_texId);
		}

		glDrawArrays(GL_QUADS, 0, 4);

		graphics = graphics->GetNext();
	}
	Image::Unbind();
	ClearGraphicsQueue();
}
