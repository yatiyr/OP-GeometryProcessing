#pragma once

#include <Precomp.h>
#include <GeoProcess/System/CoreSystem/Core.h>


namespace GP
{
	enum class TextureFilter
	{
		TEX_LINEAR,
		TEX_NEAREST
	};

	enum class TextureDataFormat
	{
		TEX_RGB,
		TEX_R
	};

	enum class TextureInternalDataFormat
	{
		TEX_RGB16,
		TEX_UNSIGNED_BYTE
	};

	class Texture
	{
	public:
		virtual ~Texture() {}

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind() const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		std::string m_Name;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> CreateF(uint32_t width, uint32_t height, float* data, int channels);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, unsigned char* data, int channels);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, std::string name);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFilter filter, unsigned char* data);
		static Ref<Texture2D> Create(const std::string& path);

	};
}