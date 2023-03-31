#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLFramebuffer.h>

#include <glad/glad.h>

namespace GP
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace OpenGLFramebufferUtils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void BindCubemap(uint32_t id)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		}

		static void CreateTextureArray(uint32_t* outID, uint32_t count)
		{
			glCreateTextures(GL_TEXTURE_2D_ARRAY, count, outID);
		}

		static void CreateCubemap(uint32_t* outID, uint32_t count)
		{
			glCreateTextures(GL_TEXTURE_CUBE_MAP, count, outID);
		}

		static void CreateCubemapArray(uint32_t* outID, uint32_t count)
		{
			glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, count, outID);
		}

		static void BindTextureArray(uint32_t id)
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		}

		static void BindCubemapArray(uint32_t id)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, id);
		}

		static void RenderbufferResize(uint32_t id, uint32_t width, uint32_t height)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, id);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_TRUE);
				glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachIntTexture(uint32_t id, uint32_t width, uint32_t height, int index)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_TRUE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				if (format == GL_DEPTH_COMPONENT16)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				}
			}
			float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthCubemapTexture(uint32_t id, GLenum format, uint32_t width, uint32_t height)
		{
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_FLOAT, width, height);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0);
		}

		static void AttachArrayShadowMap_Dir_Spot(uint32_t id, GLenum format, uint32_t width, uint32_t height, uint32_t layerCount)
		{
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, width, height, layerCount);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0);
		}

		static void AttachCubemap(uint32_t id, uint32_t width, uint32_t height, bool isDepth)
		{
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, isDepth ? GL_DEPTH_COMPONENT24 : GL_RGB16F, width, height);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glFramebufferTexture(GL_FRAMEBUFFER, isDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, id, 0);
		}

		static void AttachCubemap_MIP(uint32_t id, uint32_t width, uint32_t height)
		{
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, 5, GL_RGB16F, width, height);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id, 0);
		}

		static void AttachArrayCubemap_Point(uint32_t id, GLenum format, uint32_t width, uint32_t height, uint32_t layerCount)
		{
			glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32F, width, height, layerCount);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0);
		}

		static void AttachCubemapPointBlur(uint32_t id, uint32_t width, uint32_t height, uint32_t layerCount)
		{
			glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_RGBA32F, width, height, layerCount);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id, 0);
		}

		static void AttachArrayShadowMapVariance_Dir_Spot(uint32_t id, uint32_t width, uint32_t height, uint32_t layerCount)
		{
			// glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, width, height, layerCount, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, width, height, layerCount);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			//float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			//glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
			float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			case FramebufferTextureFormat::DEPTH16: return true;
			case FramebufferTextureFormat::SHADOWMAP_ARRAY_DEPTH: return true;
			case FramebufferTextureFormat::CUBEMAP_ARRAY_DEPTH: return true;
			case FramebufferTextureFormat::CUBEMAP_DEPTH: return true;
			case FramebufferTextureFormat::CUBEMAP_DEPTH_RBO: return true;
			}

			return false;
		}

		static GLenum FBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
			case FramebufferTextureFormat::RGBA32F: return GL_RGBA32F;
			case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}
			return 0;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{

		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!OpenGLFramebufferUtils::IsDepthFormat(spec.TextureFormat))
			{
				m_ColorAttachmentSpecifications.emplace_back(spec);
			}
			else
			{
				m_DepthAttachmentSpecification = spec;
			}
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_DepthAttachment = 0;
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());

			if (m_ColorAttachmentSpecifications.size() == 1 && m_ColorAttachmentSpecifications[0].TextureFormat == FramebufferTextureFormat::SM_VARIANCE32F)
			{
				OpenGLFramebufferUtils::CreateTextureArray(&m_ColorAttachments[0], 1);
				OpenGLFramebufferUtils::BindTextureArray(m_ColorAttachments[0]);
				OpenGLFramebufferUtils::AttachArrayShadowMapVariance_Dir_Spot(m_ColorAttachments[0], m_Specification.Width, m_Specification.Height, m_ColorAttachmentSpecifications[0].layerCount);

			}
			else if (m_ColorAttachmentSpecifications.size() == 1 && m_ColorAttachmentSpecifications[0].TextureFormat == FramebufferTextureFormat::SM_POINT_LIGHT_BLUR)
			{
				OpenGLFramebufferUtils::CreateTextureArray(&m_ColorAttachments[0], 1);
				OpenGLFramebufferUtils::BindCubemapArray(m_ColorAttachments[0]);
				OpenGLFramebufferUtils::AttachCubemapPointBlur(m_ColorAttachments[0], m_Specification.Width, m_Specification.Height, m_ColorAttachmentSpecifications[0].pointLightLayerCount);

			}
			else if (m_ColorAttachmentSpecifications.size() == 1 && m_ColorAttachmentSpecifications[0].TextureFormat == FramebufferTextureFormat::CUBEMAP)
			{
				OpenGLFramebufferUtils::CreateCubemap(&m_ColorAttachments[0], 1);
				OpenGLFramebufferUtils::BindCubemap(m_ColorAttachments[0]);
				OpenGLFramebufferUtils::AttachCubemap(m_ColorAttachments[0], m_Specification.Width, m_Specification.Height, false);

			}
			else if (m_ColorAttachmentSpecifications.size() == 1 && m_ColorAttachmentSpecifications[0].TextureFormat == FramebufferTextureFormat::CUBEMAP_MIP)
			{
				OpenGLFramebufferUtils::CreateCubemap(&m_ColorAttachments[0], 1);
				OpenGLFramebufferUtils::BindCubemap(m_ColorAttachments[0]);
				OpenGLFramebufferUtils::AttachCubemap_MIP(m_ColorAttachments[0], m_Specification.Width, m_Specification.Height);
			}
			else
			{
				OpenGLFramebufferUtils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

				for (size_t i = 0; i < m_ColorAttachments.size(); i++)
				{
					OpenGLFramebufferUtils::BindTexture(multisample, m_ColorAttachments[i]);

					switch (m_ColorAttachmentSpecifications[i].TextureFormat)
					{
					case FramebufferTextureFormat::RGBA8:
						OpenGLFramebufferUtils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RGBA32F:
						OpenGLFramebufferUtils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA32F, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						OpenGLFramebufferUtils::AttachIntTexture(m_ColorAttachments[i], m_Specification.Width, m_Specification.Height, i);
						break;
					}
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{

			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				OpenGLFramebufferUtils::CreateTextures(multisample, &m_DepthAttachment, 1);
				OpenGLFramebufferUtils::BindTexture(multisample, m_DepthAttachment);
				OpenGLFramebufferUtils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			case FramebufferTextureFormat::DEPTH16:
				OpenGLFramebufferUtils::CreateTextures(multisample, &m_DepthAttachment, 1);
				OpenGLFramebufferUtils::BindTexture(multisample, m_DepthAttachment);
				OpenGLFramebufferUtils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			case FramebufferTextureFormat::CUBEMAP_ARRAY_DEPTH:
				OpenGLFramebufferUtils::CreateCubemapArray(&m_DepthAttachment, 1);
				OpenGLFramebufferUtils::BindCubemapArray(m_DepthAttachment);
				OpenGLFramebufferUtils::AttachArrayCubemap_Point(m_DepthAttachment, m_Specification.Samples, m_Specification.Width, m_Specification.Height, m_DepthAttachmentSpecification.pointLightLayerCount);
				break;
			case FramebufferTextureFormat::CUBEMAP_DEPTH:
				OpenGLFramebufferUtils::CreateCubemap(&m_DepthAttachment, 1);
				OpenGLFramebufferUtils::BindCubemap(m_DepthAttachment);
				OpenGLFramebufferUtils::AttachCubemap(m_DepthAttachment, m_Specification.Width, m_Specification.Height, true);
				break;
			case FramebufferTextureFormat::SHADOWMAP_ARRAY_DEPTH:
				OpenGLFramebufferUtils::CreateTextureArray(&m_DepthAttachment, 1);
				OpenGLFramebufferUtils::BindTextureArray(m_DepthAttachment);
				OpenGLFramebufferUtils::AttachArrayShadowMap_Dir_Spot(m_DepthAttachment, m_Specification.Samples, m_Specification.Width, m_Specification.Height, m_DepthAttachmentSpecification.layerCount);
				break;
			case FramebufferTextureFormat::CUBEMAP_DEPTH_RBO:
				glGenRenderbuffers(1, &m_RenderBufferID);
				glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Specification.Width, m_Specification.Height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
				break;
			}
		}

		if (m_ColorAttachments.size() >= 1)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth pass
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE && glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
		{
			GP_ERROR("FRAMEBUFFER INVALIDATION FAILED {0}", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void OpenGLFramebuffer::FreeFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_DepthAttachment = 0;
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::BindRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::BindDraw()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::BindNoResize()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			GP_WARN("Failed framebuffer change to size {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		if (m_RenderBufferID == 0)
			Invalidate();
		else
			OpenGLFramebufferUtils::RenderbufferResize(m_RenderBufferID, width, height);
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFramebuffer::BlitFramebuffer(Ref<Framebuffer> dst, uint32_t BufferBit)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
		dst->BindDraw();
		glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height, 0, 0, m_Specification.Width, m_Specification.Height, BufferBit, GL_NEAREST);
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			OpenGLFramebufferUtils::FBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);

	}

	void OpenGLFramebuffer::GetSize(float& x, float& y)
	{
		x = m_Specification.Width;
		y = m_Specification.Height;
	}
}