#pragma once


#include <GeoProcess/System/EventSystem/Event.h>
#include <GeoProcess/System/CoreSystem/Core.h>
#include <Precomp.h>


namespace GP
{
	struct WinProperties
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WinProperties(const std::string& title = "GP App",
			          uint32_t width = 1280,
					  uint32_t height = 720) :
			          Title(title), Width(width), Height(height) {}
	};


	class Window
	{
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() {}
		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window Attributes
		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual float GetTime() const = 0;

		virtual float GetDpiScale() const = 0;

		virtual void HideMouseCursor() const = 0;
		virtual void ShowMouseCursor() const = 0;

		static Window* Create(const WinProperties& props = WinProperties());
	};
}