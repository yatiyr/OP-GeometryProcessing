#pragma once

#include <GeoProcess/System/RenderSystem/GraphicsContext.h>

struct GLFWwindow;

namespace GP
{
	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}