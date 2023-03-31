#include <Precomp.h>

#include <GeoProcess/System/RenderSystem/Renderer.h>
#include <GeoProcess/System/RenderSystem/UniformBuffer.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLUniformBuffer.h>

namespace GP
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		return nullptr;
	}
}