#include <Precomp.h>

#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLShader.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>




namespace GP
{

	namespace ShaderUtils
	{
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			if (type == "geometry")
				return GL_GEOMETRY_SHADER;
			return 0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
				case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
			}

			return "";
		}
	}


	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		m_Name = name;
		m_Sources[GL_VERTEX_SHADER]   = vertexSrc;
		m_Sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		CreateProgram();
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& geomSrc, const std::string& fragmentSrc)
	{
		m_Name = name;
		m_Sources[GL_VERTEX_SHADER] = vertexSrc;
		m_Sources[GL_GEOMETRY_SHADER] = geomSrc;
		m_Sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		CreateProgram();
	}

	OpenGLShader::~OpenGLShader() { glDeleteProgram(m_RendererID); }
	void OpenGLShader::Bind() const { glUseProgram(m_RendererID); }
	void OpenGLShader::Unbind() const { glUseProgram(0); }
	void OpenGLShader::SetInt(const std::string& name, int value) { UploadUniformInt(name, value); }
	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) { UploadUniformIntArray(name, values, count); }
	void OpenGLShader::SetFloat(const std::string& name, float value) { UploadUniformFloat(name, value); }
	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value) { UploadUniformFloat2(name, value); }
	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) { UploadUniformFloat3(name, value); }
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) { UploadUniformFloat4(name, value); }
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) { UploadUniformMat4(name, value); }
	void OpenGLShader::SetInt(uint32_t loc, int value) { UploadUniformInt(loc, value); }
	void OpenGLShader::SetIntArray(uint32_t loc, int* values, uint32_t count) { UploadUniformIntArray(loc, values, count); }
	void OpenGLShader::SetFloat(uint32_t loc, float value) { UploadUniformFloat(loc, value); }
	void OpenGLShader::SetFloat2(uint32_t loc, const glm::vec2& value) { UploadUniformFloat2(loc, value); }
	void OpenGLShader::SetFloat3(uint32_t loc, const glm::vec3& value) { UploadUniformFloat3(loc, value); }
	void OpenGLShader::SetFloat4(uint32_t loc, const glm::vec4& value) { UploadUniformFloat4(loc, value); }
	void OpenGLShader::SetMat4(uint32_t loc, const glm::mat4& value) {  }

	void OpenGLShader::UploadUniformInt(const std::string& name, int value) 
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vec)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, vec.x, vec.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vec)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, vec.x, vec.y, vec.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vec)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformInt(uint32_t loc, int value) { glUniform1i(loc, value); }
	void OpenGLShader::UploadUniformIntArray(uint32_t loc, int* values, uint32_t count) { glUniform1iv(loc, count, values); }
	void OpenGLShader::UploadUniformFloat(uint32_t loc, float value) { glUniform1f(loc, value); }
	void OpenGLShader::UploadUniformFloat2(uint32_t loc, const glm::vec2& vec) { glUniform2f(loc, vec.x, vec.y); }
	void OpenGLShader::UploadUniformFloat3(uint32_t loc, const glm::vec3& vec) { glUniform3f(loc, vec.x, vec.y, vec.z); }
	void OpenGLShader::UploadUniformFloat4(uint32_t loc, const glm::vec4& vec) { glUniform4f(loc, vec.x, vec.y, vec.z, vec.w); }
	void OpenGLShader::UploadUniformMat3(uint32_t loc, const glm::mat3& matrix) { glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(matrix)); }
	void OpenGLShader::UploadUniformMat4(uint32_t loc, const glm::mat4& matrix) { glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix)); }

	void OpenGLShader::CreateProgram()
	{
		CompileShader();

		GLuint program = glCreateProgram();

		for (auto& el : m_Stages)
			glAttachShader(program, el.second);

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			if(infoLog.size() > 0)
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto& el : m_Stages)
				glDeleteShader(el.second);

			GP_ERROR("Shader program could not be linked! \n {0}", std::string(infoLog.begin(), infoLog.end()));

			return;
		}

		GP_INFO("Shader [{0}] has successfully been linked", m_Name);
		for (auto& el : m_Stages)
			glDetachShader(program, el.second);
	}

	void OpenGLShader::CompileShader()
	{
		GP_INFO("Shader - [{0}] is being compiled", m_Name);
		for (auto& el : m_Sources)
		{
			GLuint shader = glCreateShader(el.first);
			const GLchar* source = (const GLchar*)el.second.c_str();
			glShaderSource(shader, 1, &source, 0);
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);
				GP_ERROR("{0} Shader Could not be compiled! \n {1}", ShaderUtils::GLShaderStageToString(el.first), std::string(infoLog.begin(), infoLog.end()));

				return;
			}
			GP_INFO("Shader - [{0}] has successfully been compiled", m_Name);
			m_Stages[el.first] = shader;
		}
	}

}