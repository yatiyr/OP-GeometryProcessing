#pragma once

#include <Precomp.h>

#include <GeoProcess/System/CoreSystem/Layer.h>
#include <GeoProcess/System/EventSystem/AppEvent.h>
#include <GeoProcess/System/EventSystem/KeyEvent.h>
#include <GeoProcess/System/EventSystem/MouseEvent.h>


namespace GP
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();


		void BlockEvents(bool block) { m_BlockEvents = block; }
		void* GetFontPtr(const std::string& fontName);

		void SetDarkThemeColors();
		void SetGlobalStyles();
	private:
		void LoadFonts(const std::string& fontName);
		void LoadIconFonts();
		std::unordered_map<std::string, void*> ImGuiFontTable;

		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}