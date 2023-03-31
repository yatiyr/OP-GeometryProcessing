#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/Framebuffer.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLFramebuffer.h>
#include <GeoProcess/System/RenderSystem/Renderer.h>


namespace GP
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLFramebuffer>(spec);
		}
		return nullptr;
	}

}