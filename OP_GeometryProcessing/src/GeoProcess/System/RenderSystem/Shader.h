#pragma once

#include <Precomp.h>
#include <glm/glm.hpp>

namespace GP
{
	class Shader
	{
	public:
		virtual ~Shader() {}
		
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;


		virtual void SetInt(uint32_t loc, int value) = 0;
		virtual void SetIntArray(uint32_t loc, int* values, uint32_t count) = 0;
		virtual void SetFloat(uint32_t loc, float value) = 0;
		virtual void SetFloat2(uint32_t loc, const glm::vec2& value) = 0;
		virtual void SetFloat3(uint32_t loc, const glm::vec3& value) = 0;
		virtual void SetFloat4(uint32_t loc, const glm::vec4& value) = 0;
		virtual void SetMat4(uint32_t loc, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filePath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& geomSrc, const std::string& fragmentSrc);
	};
}