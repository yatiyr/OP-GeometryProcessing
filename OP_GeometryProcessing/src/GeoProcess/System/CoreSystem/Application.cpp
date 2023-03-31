#include <Precomp.h>

#include <GeoProcess/System/CoreSystem/Application.h>
#include <GeoProcess/System/ResourceSystem/ResourceManager.h>
#include <GeoProcess/System/RenderSystem/RenderCommand.h>

#include <GeoProcess/System/InputSystem/Input.h>
#include <GeoProcess/System/RenderSystem/Renderer.h>


namespace GP
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create(WinProperties(name)));
		m_Window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));

		m_DpiScale = m_Window->GetDpiScale();

		RenderCommand::Enable(MODE::DITHER);
		RenderCommand::Enable(MODE::TEXTURE_CUBE_MAP_SEAMLESS);

		ResourceManager::Init(std::filesystem::current_path());
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_isRunning = false;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		while (m_isRunning)
		{
			float time = (float)Application::GetWindow().GetTime();
			TimeStep timestep = time - m_LastFrameTime;

			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			{
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		m_isRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}