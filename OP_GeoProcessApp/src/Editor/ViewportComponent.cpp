#include <Precomp.h>

#include <Editor/ViewportComponent.h>
#include <GeoProcess/System/CoreSystem/Application.h>

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <Editor/EditorLayer.h>
#include <MainRender/MainRender.h>

#include <glm/gtc/type_ptr.hpp>
#include <Math/Math.h>

#include <GeoProcess/System/RenderSystem/Framebuffer.h>

namespace GP
{
	void ViewportComponent::OnAttach() {}

	void ViewportComponent::ResizeFramebuffer()
	{
		EditorLayer* elInstance = EditorLayer::GetEditor();
		FramebufferSpecification spec = m_Framebuffer->GetSpecification();

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
			elInstance->m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	bool ViewportComponent::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == (int)MouseButton::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyCode::LeftAlt))
			{
				// GP_TRACE("Mouse button pressed.");
			}

		}
		return false;
	}

	void ViewportComponent::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);

		ImGui::Begin("##Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset    = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x,
								viewportMinRegion.y + viewportOffset.y };

		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x,
								viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
		{
			m_ViewportSize = { (uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y };
		}

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		HandleImGuizmo();

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void ViewportComponent::HandleImGuizmo()
	{
		EditorLayer* editorInstance = EditorLayer::GetEditor();

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y,
						  m_ViewportBounds[1].x - m_ViewportBounds[0].x,
						  m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		// Editor Camera
		glm::mat4 cameraProjection = editorInstance->m_EditorCamera.GetProjection();
		glm::mat4 cameraView       = editorInstance->m_EditorCamera.GetViewMatrix();

		if (editorInstance->m_GizmoType != -1)
		{
			auto& transform = EditorLayer::s_Instance->m_ModelTransform;
			
			// Snap
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;

			// Snap to 5 degrees if operation is rotation
			if (editorInstance->m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 5.0f;

			glm::vec3 oldTranslation, oldRotation, oldScale;
			Math::DecomposeTransform(transform, oldTranslation, oldRotation, oldScale);

			float snapValues[3] = { snapValue, snapValue, snapValue };
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)editorInstance->m_GizmoType, ImGuizmo::LOCAL,
				glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 dRotation = rotation - oldRotation;			
				oldRotation += dRotation;
				glm::mat4 rotMat = glm::toMat4(glm::quat(oldRotation));
				transform = glm::translate(glm::mat4(1.0f), translation) * rotMat * glm::scale(glm::mat4(1.0f), scale);
				MainRender::SetModelTransform(transform);
			}

		}
	}

}