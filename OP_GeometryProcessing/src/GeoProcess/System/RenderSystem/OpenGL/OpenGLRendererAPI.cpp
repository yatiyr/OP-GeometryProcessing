#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLRendererAPI.h>

#include <glad/glad.h>

namespace GP
{
	static int OPFaceToGL(FACE face)
	{
		switch (face)
		{
			case GP::FACE::FRONT:
				return GL_FRONT;
			case GP::FACE::BACK:
				return GL_BACK;
			case GP::FACE::FRONT_AND_BACK:
				return GL_FRONT_AND_BACK;
		}
	}

	static int OPPolygonModeToGL(POLYGONMODE mode)
	{
		switch(mode)
		{
			case GP::POLYGONMODE::POINT:
				return GL_POINT;
			case GP::POLYGONMODE::LINE:
				return GL_LINE;
			case GP::POLYGONMODE::FILL:
				return GL_FILL;
		}
	}

	static int OPModeToGL(MODE mode)
	{
		switch (mode)
		{
			case GP::MODE::TEXTURE_CUBE_MAP_SEAMLESS:
				return GL_TEXTURE_CUBE_MAP_SEAMLESS;
			case GP::MODE::DITHER:
				return GL_DITHER;
			case GP::MODE::DEPTH_TEST:
				return GL_DEPTH_TEST;
			case GP::MODE::CULL_FACE:
				return GL_CULL_FACE;
			default:
				return -1;
		}
	}

	static int OPDepthFuncToGL(DEPTHFUNC func)
	{
		switch (func)
		{
			case GP::DEPTHFUNC::NEVER:
				return GL_NEVER;
			case GP::DEPTHFUNC::LESS:
				return GL_LESS;
			case GP::DEPTHFUNC::LEQUAL:
				return GL_LEQUAL;
			case GP::DEPTHFUNC::EQUAL:
				return GL_EQUAL;
			case GP::DEPTHFUNC::GREATER:
				return GL_GREATER;
			case GP::DEPTHFUNC::NOTEQUAL:
				return GL_NOTEQUAL;
			case GP::DEPTHFUNC::GEQUAL:
				return GL_GEQUAL;
			case GP::DEPTHFUNC::ALWAYS:
				return GL_ALWAYS;
			default:
				break;
		}
	}

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINE_STRIP, 0, count);
	}

	void OpenGLRendererAPI::DrawIndexedBinded(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		vertexArray->Unbind();
	}

	void OpenGLRendererAPI::Enable(MODE mode)
	{
		glEnable(OPModeToGL(mode));
	}

	void OpenGLRendererAPI::Disable(MODE mode)
	{
		glDisable(OPModeToGL(mode));
	}

	void OpenGLRendererAPI::DepthFunc(DEPTHFUNC func)
	{
		glDepthFunc(OPDepthFuncToGL(func));
	}

	void OpenGLRendererAPI::PolygonMode(FACE face, POLYGONMODE mode)
	{
		glPolygonMode(OPFaceToGL(face), OPPolygonModeToGL(mode));
	}
}