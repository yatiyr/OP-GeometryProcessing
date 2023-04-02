#pragma once

#include <GeoProcess/System/RenderSystem/Framebuffer.h>
#include <GeoProcess/System/EventSystem/MouseEvent.h>
#include <glm/glm.hpp>

namespace GP
{
	class ViewportComponent
	{
	public:
		ViewportComponent() = default;
		ViewportComponent(const Ref<Framebuffer>& framebuffer, const Ref<Framebuffer>& triangleIDFramebuffer)
			: m_Framebuffer(framebuffer), m_TriangleIDFramebuffer(triangleIDFramebuffer) {}

		void OnImGuiRender();
		void SetFramebuffer(Ref<Framebuffer> framebuffer, Ref<Framebuffer> entityIDFramebuffer) 
		{ 
			m_Framebuffer = framebuffer;
			m_TriangleIDFramebuffer = entityIDFramebuffer;
		}

		void OnAttach();
		void ResizeFramebuffer();

		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		glm::vec2 GetViewportSize()
		{
			return m_ViewportSize;
		}

	private:
		void HandleImGuizmo();

	private:

		Ref<Framebuffer> m_Framebuffer;
		Ref<Framebuffer> m_TriangleIDFramebuffer;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };

		friend class EditorLayer;
	};
}