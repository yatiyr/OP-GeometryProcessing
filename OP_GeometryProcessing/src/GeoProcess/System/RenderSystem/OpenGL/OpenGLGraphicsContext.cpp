#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLGraphicsContext.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace GP
{
	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) {}

	void OpenGLGraphicsContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		GP_INFO("Information about OPENGL Renderer and Graphics Card");
		GP_INFO("  Vendor: {0}", fmt::ptr(glGetString(GL_VENDOR)));
		GP_INFO("  Renderer: {0}", fmt::ptr(glGetString(GL_RENDERER)));
		GP_INFO("  Version: {0}", fmt::ptr(glGetString(GL_VERSION)));
	}

	void OpenGLGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
