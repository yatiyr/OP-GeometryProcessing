#include <Precomp.h>
#include <GeoProcess/System/GuiSystem/ImGuiLayer.h>

#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GeoProcess/System/CoreSystem/Application.h>
#include <GLFW/glfw3.h>
#include <ImGuizmo.h>

#include <GeoProcess/System/GuiSystem/Font/Font.h>

namespace GP
{
	extern ImFont* ImGuiIconFontBg = nullptr;
	extern ImFont* ImGuiIconFontMd = nullptr;
	extern ImFont* ImGuiIconFontText = nullptr;

	std::unordered_map<std::string, void*> ImGuiFontTable{};

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer() {}


	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		// Add Implot
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoTaskBarIcon = true;

		LoadFonts("Inter");
		LoadFonts("Ubuntu");
		LoadFonts("OpenSans");
		LoadIconFonts();

		io.FontDefault = (ImFont*)ImGuiFontTable["Inter-Light-12"];

		// Setup Dear ImGui Styles
		ImGui::StyleColorsDark();

		// When viewports are enabled, we tweak WindowRounding/WindowBg so platform windows can look identical
		// to regular ones
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();
		SetGlobalStyles();

		// Setup Platform Renderer Backends
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void* ImGuiLayer::GetFontPtr(const std::string& fontName)
	{
		return ImGuiFontTable[fontName];
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Text Colors
		colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		colors[ImGuiCol_TextDisabled] = ImVec4{ 0.645f, 0.645f, 0.645f, 1.0f };

		// Window Colors
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1372f, 0.1372f, 0.1764f, 1.0f };
		colors[ImGuiCol_ChildBg] = ImVec4{ 0.1372f, 0.1372f, 0.1764f, 1.0f };
		colors[ImGuiCol_PopupBg] = ImVec4{ 0.2941f, 0.1568f, 0.2941f, 0.7843f };


		// Border Colors
		colors[ImGuiCol_Border] = ImVec4{ 0.1019f, 0.1019f, 0.1019f, 1.0f };
		colors[ImGuiCol_BorderShadow] = ImVec4{ 0.1019f, 0.1019f, 0.1019f, 0.6f };

		// Frame Colors
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2352f, 0.1960f, 0.3529f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 120.0f / 255.0f, 100.0f / 255.0f, 1.0f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 160.0f / 255.0f, 160.0f / 255.0f, 1.0f, 1.0f };

		// Menu bar Color
		colors[ImGuiCol_MenuBarBg] = ImVec4{ 90.0f / 255.0f, 150.0f / 255.0f, 1.0f, 1.0f };

		// Scrollbar  Colors
		colors[ImGuiCol_ScrollbarBg] = ImVec4{ 50.0f / 255.0f, 70.0f / 255.0f, 120.0f / 255.0f, 1.0f };
		colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 140.0f / 255.0f, 140.0f / 255.0f, 1.0f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 70.0f / 255.0f, 170.0f / 255.0f, 1.0f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 200.0f / 255.0f, 1.0f, 1.0f, 1.0f };

		// Checkmark
		colors[ImGuiCol_CheckMark] = ImVec4{ 100.0f / 255.0f, 1.0f, 1.0f, 1.0f };

		// Slider Colors
		colors[ImGuiCol_SliderGrab] = ImVec4{ 190.0f / 255.0f, 40.0f / 255.0f, 140.0f / 255.0f, 1.0f };
		colors[ImGuiCol_SliderGrabActive] = ImVec4{ 1.0f, 40.0f / 255.0f, 90.0f / 255.0f, 1.0f };

		// Button Colors
		colors[ImGuiCol_Button] = ImVec4{ 35.0f / 255.0f, 35.0f / 255.0f, 45.0f / 255.0f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 65.0f / 255.0f, 65.0f / 255.0f, 155.0f / 255.0f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 110.0f / 255.0f, 140.0f / 255.0f, 200.0f / 255.0f, 1.0f };

		// TreeNode, Selectable, Menu Item, Collapsing Header Colors
		colors[ImGuiCol_Header] = ImVec4{ 35.0f / 255.0f, 35.0f / 255.0f, 45.0f / 255.0f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 100.0f / 255.0f, 140.0f / 255.0f, 200.0f / 255.0f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 120.0f / 255.0f, 200.0f / 255.0f, 1.0f, 1.0f };

		// Separator Colors TODO: Sort This Out!!!!
		colors[ImGuiCol_Separator] = ImVec4{ 150.0f / 255.0f, 1.0f, 1.0f, 20.0f / 255.0f };
		colors[ImGuiCol_SeparatorHovered] = ImVec4{ 150.0f / 255.0f, 1.0f, 1.0f, 130.0f / 255.0f };
		colors[ImGuiCol_SeparatorActive] = ImVec4{ 150.0f / 255.0f, 1.0f, 1.0f, 255.0f / 255.0f };

		// Resize Grip
		colors[ImGuiCol_ResizeGrip] = ImVec4{ 150.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 120.0f / 255.0f };
		colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 150.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 190.0f / 255.0f };
		colors[ImGuiCol_ResizeGripActive] = ImVec4{ 150.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f };

		// Tab Colors
		colors[ImGuiCol_Tab] = ImVec4{ 45.0f / 255.0f, 45.0f / 255.0f, 55.0f / 255.0f, 255.0f / 255.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 70.0f / 255.0f, 70.0f / 255.0f, 120.0f / 255.0f, 255.0f / 255.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 100.0f / 255.0f, 100.0f / 255.0f, 210.0f / 255.0f, 255.0f / 255.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 45.0f / 255.0f, 45.0f / 255.0f, 55.0f / 255.0f, 150.0f / 255.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 100.0f / 255.0f, 100.0f / 255.0f, 210.0f / 255.0f, 190.0f / 255.0f };

		// Docking
		colors[ImGuiCol_DockingPreview] = ImVec4{ 120.0f / 255.0f, 200.0f / 255.0f, 255.0f / 255.0f, 150.0f / 255.0f };
		colors[ImGuiCol_DockingEmptyBg] = ImVec4{ 120.0f / 255.0f, 200.0f / 255.0f, 255.0f / 255.0f, 90.0f / 255.0f };

		// Title Colors
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };


		// TODO: Docking, PlotLines, Histogram, Table, DragDrop Target, Nav, Model colors
		// need to be implemented!

	}

	void ImGuiLayer::SetGlobalStyles()
	{
		ImGuiStyle& style = ImGui::GetStyle();


		// TODO: Global styles will be implemented!
		style.FrameRounding = 0.0f;
		style.WindowPadding = ImVec2{ 4.0f, 4.0f };
		style.FramePadding = ImVec2{ 2.0f, 1.0f };
		style.CellPadding = ImVec2{ 3.0f, 1.0f };

		style.ItemSpacing = ImVec2{ 3.0f, 3.0f };
		style.ItemInnerSpacing = ImVec2{ 3.0f, 3.0f };
		style.ScrollbarSize = 10.0f;
		style.GrabMinSize = 7.0f;
		style.LogSliderDeadzone = 2.0f;
		style.ColorButtonPosition = ImGuiDir_Left;
		style.WindowMenuButtonPosition = ImGuiDir_Right;

		style.WindowBorderSize = 1.0f;
		style.TabRounding = 0.0f;
		style.AntiAliasedLines = true;
		style.FrameBorderSize = 0.0f;
		style.ChildBorderSize = 0.0f;
		style.PopupBorderSize = 1.0f;

		style.ScrollbarRounding = false;
		style.WindowTitleAlign = ImVec2{ 0.5f, 0.5f };

		style.TabBorderSize = 0.0f;
		style.WindowRounding = 1.0f;
		style.FrameRounding = 1.0f;

		// style.WindowPadding = ImVec2{ 0.0f, 0.0f };

		style.AntiAliasedLinesUseTex = true;
		style.AntiAliasedFill = true;

		style.WindowMinSize.x = 150.0f;
		style.WindowMinSize.y = 10.0f;

		style.ScaleAllSizes(Application::Get().GetDpiScale() * 1.1f);


	}

	void ImGuiLayer::LoadFonts(const std::string& fontName)
	{

		GP_WARN("{0}", Application::Get().GetDpiScale());

		float dpiScale = Application::Get().GetDpiScale();

		ImGuiIO& io = ImGui::GetIO();
		// io.Fonts->AddFontDefault();

		std::string fontStyles[4] = { "Light", "Regular", "Medium", "Bold" };
		int fontSizes[8] = { 6, 8, 10, 12, 14, 16, 18, 20 };

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				ImFontConfig fontCfg;
				fontCfg.OversampleH = 8;
				fontCfg.OversampleV = 2;
				fontCfg.RasterizerMultiply = 1.2f;

				std::string path = std::string("assets/fonts/") + fontName + "/" + fontName + "-" + fontStyles[1] + ".ttf";
				std::string finalName = fontName + "-" + fontStyles[i] + "-" + std::to_string(fontSizes[j]);
				ImGuiFontTable[finalName] = (void*)io.Fonts->AddFontFromFileTTF(path.c_str(), fontSizes[j] * dpiScale * 1.2f, &fontCfg);
			}
		}
	}

	void ImGuiLayer::LoadIconFonts()
	{
		float dpiScale = Application::Get().GetDpiScale();

		ImFontConfig fontCfg;
		fontCfg.OversampleH = 1;
		fontCfg.OversampleV = 1;
		fontCfg.RasterizerMultiply = 1.2f;
		// fontCfg.MergeMode = true;
		// fontCfg.GlyphMinAdvanceX = 28.0f * dpiScale;
		// fontCfg.GlyphOffset.y = 10.0;
		// fontCfg.GlyphMinAdvanceX = 28.0f;
		// fontCfg.PixelSnapH = true;

		ImGuiIO& io = ImGui::GetIO();
		static const ImWchar icons_ranges[] = { ICON_MIN_GP, ICON_MAX_GP, 0 };
		ImGuiIconFontBg = io.Fonts->AddFontFromFileTTF("assets/fonts/Webfonts/icomoon.ttf", 32.0f * dpiScale, &fontCfg, icons_ranges);

		ImGuiIconFontMd = io.Fonts->AddFontFromFileTTF("assets/fonts/Webfonts/icomoon.ttf", 24.0f * dpiScale, &fontCfg, icons_ranges);

		ImGuiIconFontText = io.Fonts->AddFontFromFileTTF("assets/fonts/Webfonts/icomoon.ttf", 14.0f * dpiScale, &fontCfg, icons_ranges);
	}
}