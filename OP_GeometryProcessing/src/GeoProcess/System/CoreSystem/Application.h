#pragma once

#include <GeoProcess/System/WindowSystem/Window.h>
#include <GeoProcess/System/CoreSystem/Core.h>
#include <GeoProcess/System/CoreSystem/LayerStack.h>
#include <GeoProcess/System/EventSystem/Event.h>
#include <GeoProcess/System/EventSystem/AppEvent.h>

#include <GeoProcess/System/CoreSystem/TimeStep.h>
#include <GeoProcess/System/GuiSystem/ImGuiLayer.h>

namespace GP
{

	class Application
	{
	public:
		Application(const std::string& name = "App");
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);
		

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }

		float GetDpiScale() { return m_DpiScale; }

	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Ref<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_isRunning = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		float m_DpiScale = 0.0f;
	private:
		static Application* s_Instance;
	};

	// Client will define this
	Application* CreateApplication();
}