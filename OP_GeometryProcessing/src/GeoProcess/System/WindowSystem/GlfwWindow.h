#pragma once

#include <GeoProcess/System/WindowSystem/Window.h>
#include <GeoProcess/System/RenderSystem/GraphicsContext.h>
#include <GLFW/glfw3.h>

namespace GP
{
	class GlfwWindow : public Window
	{
	public:
		GlfwWindow(const WinProperties& props);
		virtual ~GlfwWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFunction& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
		virtual float GetTime() const override;

		virtual float GetDpiScale() const override;

		virtual void HideMouseCursor() const override;
		virtual void ShowMouseCursor() const override;
	private:
		virtual void Init(const WinProperties& props);
		virtual void Shutdown();

		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WinData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunction EventCallback;
		};

		WinData m_Data;
	};
}