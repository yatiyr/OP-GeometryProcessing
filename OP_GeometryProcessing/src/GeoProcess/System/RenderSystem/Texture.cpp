#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/Texture.h>

#include <GeoProcess/System/RenderSystem/Renderer.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLTexture.h>

namespace GP
{
	Ref<Texture2D> Texture2D::CreateF(uint32_t width, uint32_t height, float* data, int channels)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height, data, channels);
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, unsigned char* data, int channels)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height, data, channels);
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, std::string name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height, name);
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height);
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFilter filter, unsigned char* data)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height, filter, data);
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
		}

		return nullptr;
	}
}