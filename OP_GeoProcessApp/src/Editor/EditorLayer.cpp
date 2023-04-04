#include <Editor/EditorLayer.h>
#include <imgui/imgui.h>
#include <implot/implot.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ImGuizmo/ImGuizmo.h>
#include <GeoProcess/System/ResourceSystem/ResourceManager.h>


#include <GeoProcess/System/GuiSystem/Font/Font.h>

#include <glad/glad.h>

namespace GP
{
	EditorLayer* EditorLayer::s_Instance = nullptr;

	EditorLayer* EditorLayer::CreateEditor()
	{
		s_Instance = new EditorLayer();
		return s_Instance;
	}

	EditorLayer::EditorLayer() : Layer("EditorLayer") {}

	void EditorLayer::OnAttach()
	{

		MainRender::Init(1280, 720);
		m_Framebuffer = MainRender::GetFinalFramebuffer();
		m_RenderFramebuffer = MainRender::GetMainRenderFramebuffer();
		m_TriangleIdFramebuffer = MainRender::GetTriangleIDFramebuffer();

		// Final framebuffer
		FramebufferSpecification finalFBSpec;
		finalFBSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		finalFBSpec.Width = 1280;
		finalFBSpec.Height = 720;
		m_FinalFramebuffer = Framebuffer::Create(finalFBSpec);

		m_GridShader = ResourceManager::GetShader("Grid.glsl");
		m_Plane = Plane::Create();
		m_Cube = Cube::Create();
		m_Sphere = Icosphere::Create(1.0f, 2, false);

		m_EditorCamera = EditorCamera(30.0f, 1280.0f / 720.0f, 0.1, 5000.0f);

		m_ModelTransform = glm::mat4(1.0);

		m_ViewportComponent.SetFramebuffer(m_FinalFramebuffer, m_TriangleIdFramebuffer);

	}

	void EditorLayer::RenderDockspace()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_AutoHideTabBar;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("##GP_Dockspace", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;
	}

	void EditorLayer::OnDetach() {}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		m_ViewportComponent.ResizeFramebuffer();

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0 });
		RenderCommand::Clear();

		// Clear our triangle ID attachment to -1
		m_TriangleIdFramebuffer->Bind();
		m_TriangleIdFramebuffer->ClearAttachment(0, -1);

		FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		glm::vec2 viewportSize = m_ViewportComponent.GetViewportSize();

		if (spec.Width != viewportSize.x && spec.Height != viewportSize.y)
		{
			MainRender::ResizeViewport(viewportSize.x, viewportSize.y);
		}

		// update camera
		m_EditorCamera.OnUpdate(ts);


		MainRender::Render(m_EditorCamera, ts);

		m_Framebuffer->Unbind();
		m_Framebuffer->BlitFramebuffer(m_FinalFramebuffer, (uint32_t)BufferBit::COLOR_BUFFER_BIT);
		m_RenderFramebuffer->BlitFramebuffer(m_FinalFramebuffer, (uint32_t)BufferBit::DEPTH_BUFFER_BIT);

		m_FinalFramebuffer->Bind();

		RenderCommand::PolygonMode(FACE::FRONT_AND_BACK, POLYGONMODE::FILL);
		m_GridShader->Bind();
		m_Plane->Draw();

		m_FinalFramebuffer->Unbind();
	}


	void EditorLayer::OnImGuiRender()
	{
		RenderDockspace();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10,10 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{ 1000, 200 });
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				// ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					
				}

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }
				/*if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; } */
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();


		m_ViewportComponent.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoTitleBar);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

		RenderSpecs* spc = &MainRender::GetEditorMesh()->m_RenderSpecs;

		ImGui::Checkbox("Fill", &spc->fill);
		ImGui::Checkbox("Line", &spc->line);
		ImGui::ColorEdit4("Line Color", glm::value_ptr(spc->lineColor));
		ImGui::Checkbox("Point", &spc->point);
		ImGui::ColorEdit4("Point Color", glm::value_ptr(spc->pointColor));
		if (ImGui::Checkbox("Backface Culling", &spc->backfaceCulling))
		{
			if (spc->backfaceCulling)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		int currentSelectedIDMethod = MainRender::GetEditorMesh()->m_GeodesicDistanceCalcMethod;
		std::vector<std::string> calcMethodNames = { "Min Heap", "Array" };
		if (ImGui::BeginCombo("CalcMethod", MainRender::GetEditorMesh()->GiveCalcMethodName().c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			for (int i = 0; i < calcMethodNames.size(); i++)
			{
				const bool isSelected = (currentSelectedIDMethod == i);
				if (ImGui::Selectable(calcMethodNames[i].c_str(), isSelected))
				{
					currentSelectedIDMethod = i;
					MainRender::GetEditorMesh()->m_GeodesicDistanceCalcMethod = i;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		int currentSelectedIDRenderMode = (int)MainRender::GetEditorMesh()->m_RenderSpecs.renderMode;
		std::vector<std::string> renderModeNames = { "AGD", "Flat", "GC", "Triangle Quality", "Smooth"};
		if (ImGui::BeginCombo("Render Mode", MainRender::GetEditorMesh()->GiveRenderMethodName().c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			for (int i = 0; i < renderModeNames.size(); i++)
			{
				const bool isSelected = (currentSelectedIDRenderMode == i);
				if (ImGui::Selectable(renderModeNames[i].c_str(), isSelected))
				{
					currentSelectedIDRenderMode = i;
					MainRender::GetEditorMesh()->m_RenderSpecs.renderMode = (RENDERMODE)i;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}


		ImGui::Text("Distance Calc Time %f", MainRender::GetEditorMesh()->m_CalcTime);
		if (ImGui::InputInt("StartIndex", MainRender::GetGeoDistStartIndex()))
		{
			MainRender::GetEditorMesh()->SetupLineVertices();
		}
		if (ImGui::InputInt("EndIndex", MainRender::GetGeoDistEndIndex()))
		{
			MainRender::GetEditorMesh()->SetupLineVertices();
		};
		ImGui::ColorEdit4("DistLine Color", glm::value_ptr(MainRender::GetEditorMesh()->m_LineColor));
		if (ImGui::DragFloat("Line Displacement", &MainRender::GetEditorMesh()->m_RenderSpecs.lineDisplacement, 0.1f, 0.0f))
		{
			MainRender::GetEditorMesh()->SetupLineVertices();
		}
		ImGui::Checkbox("Show Line", MainRender::GetShowLine());

		if (ImGui::Button("Smoothing Function"))
		{
			MainRender::GetEditorMesh()->SmoothingFunction();
		}

		ImGui::PopStyleVar();
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();


	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FUNCTION(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(EditorLayer::OnWindowResized));
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		m_ViewportComponent.OnMouseButtonPressed(e);
		return false;
	}

	bool EditorLayer::OnWindowResized(WindowResizeEvent& e)
	{
		// MainRender::ResizeViewport(e.GetWidth(), e.GetHeight());
		return false;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		GP_TRACE("Key: {0} has been pressed.", e.GetKeyCode());
		switch (e.GetKeyCode())
		{
			// Gizmo stuff
			case Key::Q:
				m_GizmoType = -1;
				break;
			case Key::W:
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}

		return false;
	}

}