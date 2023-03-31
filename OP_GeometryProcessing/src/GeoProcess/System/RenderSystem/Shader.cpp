#include <Precomp.h>

#include <GeoProcess/System/RenderSystem/Shader.h>
#include <GeoProcess/System/RenderSystem/Renderer.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLShader.h>

namespace GP
{
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& geomSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, geomSrc, fragmentSrc);
		}
		return nullptr;
	}
}