#pragma once

#include <GeoProcess/System/RenderSystem/Shader.h>
#include <glm/glm.hpp>

typedef unsigned int GLenum;
typedef unsigned int GLuint;

namespace GP
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& geomSrc, const std::string& fragmentSrc);

		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetInt(uint32_t loc, int value) override;
		virtual void SetIntArray(uint32_t loc, int* values, uint32_t count) override;
		virtual void SetFloat(uint32_t loc, float value) override;
		virtual void SetFloat2(uint32_t loc, const glm::vec2& value) override;
		virtual void SetFloat3(uint32_t loc, const glm::vec3& value) override;
		virtual void SetFloat4(uint32_t loc, const glm::vec4& value) override;
		virtual void SetMat4(uint32_t loc, const glm::mat4& value) override;

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vec);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vec);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vec);
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformInt(uint32_t loc, int value);
		void UploadUniformIntArray(uint32_t loc, int* values, uint32_t count);
		void UploadUniformFloat(uint32_t loc, float value);
		void UploadUniformFloat2(uint32_t loc, const glm::vec2& vec);
		void UploadUniformFloat3(uint32_t loc, const glm::vec3& vec);
		void UploadUniformFloat4(uint32_t loc, const glm::vec4& vec);
		void UploadUniformMat3(uint32_t loc, const glm::mat3& matrix);
		void UploadUniformMat4(uint32_t loc, const glm::mat4& matrix);
	private:

		void CreateProgram();
		void CompileShader();

	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::unordered_map<GLenum, std::string> m_Sources;
		std::unordered_map<GLenum, uint32_t> m_Stages;
	};
}