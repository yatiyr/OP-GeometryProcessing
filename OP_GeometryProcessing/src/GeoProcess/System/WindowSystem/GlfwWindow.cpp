#include <Precomp.h>
#include <GeoProcess/System/WindowSystem/GlfwWindow.h>

#include <GeoProcess/System/EventSystem/AppEvent.h>
#include <GeoProcess/System/EventSystem/MouseEvent.h>
#include <GeoProcess/System/EventSystem/KeyEvent.h>

#include <GeoProcess/System/RenderSystem/OpenGL/OpenGLGraphicsContext.h>


namespace GP
{
	static bool is_glfw_initialized;

	void GLFWErrorCallback(int error, const char* description)
	{
		GP_ERROR("GLFW Error ({0}): {1}", error, description)
	}

	Window* Window::Create(const WinProperties& props)
	{
		return new GlfwWindow(props);
	}

	GlfwWindow::GlfwWindow(const WinProperties& props)
	{
		Init(props);
	}

	GlfwWindow::~GlfwWindow()
	{
		Shutdown();
	}

	void GlfwWindow::Init(const WinProperties& props)
	{
		m_Data.Title  = props.Title;
		m_Data.Width  = props.Width;
		m_Data.Height = props.Height;

		GP_INFO("Window is being created {0} ({1}, {2})", props.Title, props.Width, props.Height)

		if (!is_glfw_initialized)
		{
			int flag = glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
			is_glfw_initialized = true;
		}

		glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = new OpenGLGraphicsContext(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set callbacks of GLFW
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window);
				data.Width  = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mode)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event((KeyCode)key, (uint16_t)0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event((KeyCode)key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event((KeyCode)key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event((KeyCode)keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	void GlfwWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void GlfwWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void GlfwWindow::SetVSync(bool enable)
	{
		if (enable)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		m_Data.VSync = enable;
	}

	bool GlfwWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	float GlfwWindow::GetTime() const
	{
		return glfwGetTime();
	}

	float GlfwWindow::GetDpiScale() const
	{
		float xscale, yscale;
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		glfwGetMonitorContentScale(monitor, &xscale, &yscale);

		return xscale;
	}

	void GlfwWindow::HideMouseCursor() const
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void GlfwWindow::ShowMouseCursor() const
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}