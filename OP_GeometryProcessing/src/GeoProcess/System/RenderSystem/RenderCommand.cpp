#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/RenderCommand.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLRendererAPI.h>


namespace GP
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}