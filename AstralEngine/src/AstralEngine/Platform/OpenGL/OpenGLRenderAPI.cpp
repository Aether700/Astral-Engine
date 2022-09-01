#include "aepch.h"
#include "OpenGLRenderAPI.h"

#include <glad/glad.h>

namespace AstralEngine
{
	RenderAPI::API RenderAPI::s_api = RenderAPI::API::OpenGL;

	static unsigned int RenderingPrimitiveToOpenGLPrimitive(RenderingPrimitive primitive) 
	{
		switch (primitive) 
		{
		case RenderingPrimitive::Triangles:
			return GL_TRIANGLES;
		}

		AE_CORE_ERROR("Unknown Rendering primitive provided");
		return 0;
	}

	void OpenGLRenderAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRenderAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	size_t OpenGLRenderAPI::GetNumTextureSlots()
	{
		int textureSlots;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureSlots);
		return (size_t)textureSlots;
	}

	size_t OpenGLRenderAPI::GetMaxNumVertices()
	{
		int numVertices;
		glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &numVertices);
		return (size_t)numVertices;

	}

	size_t OpenGLRenderAPI::GetMaxNumIndices()
	{
		int numIndices;
		glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &numIndices);
		return (size_t)numIndices;
	}

	void OpenGLRenderAPI::EnableBlending(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void OpenGLRenderAPI::DrawIndexed(const AReference<IndexBuffer>& indexBuffer)
	{
		glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderAPI::DrawIndexed(const AReference<IndexBuffer>& indexBuffer, unsigned int count)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderAPI::DrawIndexed(RenderingPrimitive primitive, const AReference<IndexBuffer>& indexBuffer, unsigned int count) 
	{
		glDrawElements(RenderingPrimitiveToOpenGLPrimitive(primitive), count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderAPI::DrawInstancedIndexed(const AReference<IndexBuffer>& indexBuffer, 
		unsigned int instanceAmount, unsigned int count)
	{
		if (count == 0)
		{
			count = indexBuffer->GetCount();
		}
		glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instanceAmount);
	}
}