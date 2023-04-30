#pragma once

#include <Gp.h>

#include <GeoProcess/System/RenderSystem/CameraSystem/EditorCamera.h>
#include <GeoProcess/System/Geometry/Plane.h>
#include <GeoProcess/System/Geometry/Cube.h>
#include <GeoProcess/System/Geometry/Icosphere.h>

#include <MainRender/MainRender.h>
#include <Editor/ViewportComponent.h>

namespace GP
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() {}
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(TimeStep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;

		static EditorLayer* CreateEditor();
		static EditorLayer* GetEditor() { return s_Instance; }

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		void RenderDockspace();
	private:
		ViewportComponent m_ViewportComponent;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		Ref<Framebuffer> m_Framebuffer;
		Ref<Framebuffer> m_RenderFramebuffer;
		Ref<Framebuffer> m_FinalFramebuffer;
		Ref<Framebuffer> m_TriangleIdFramebuffer;

		Ref<Shader> m_GridShader;
		Ref<Plane> m_Plane;

		int m_GizmoType = -1;

		Ref<Cube> m_Cube;
		Ref<Icosphere> m_Sphere;

		EditorCamera m_EditorCamera;

		glm::mat4 m_ModelTransform;

		bool m_ExportInProgress = false;
		std::future<void> m_ExportState;
	private:
		static EditorLayer* s_Instance;


		friend class ViewportComponent;
	};
}