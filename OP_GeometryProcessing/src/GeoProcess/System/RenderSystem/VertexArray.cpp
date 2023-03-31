#include <Precomp.h>

#include <GeoProcess/System/RenderSystem/VertexArray.h>
#include <GeoProcess/System/RenderSystem/Renderer.h>

#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLVertexArray.h>

namespace GP
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}

		return nullptr;
	}
}