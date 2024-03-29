#pragma once

#include <GeoProcess/System/RenderSystem/CameraSystem/Camera.h>
#include <GeoProcess/System/CoreSystem/TimeStep.h>
#include <GeoProcess/System/EventSystem/Event.h>
#include <GeoProcess/System/EventSystem/MouseEvent.h>

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace GP
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() {}
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;
			UpdateProjection();
		}

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		void SetViewMatrix(const glm::mat4& view) { m_ViewMatrix = view; }

		glm::mat4& GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

		glm::quat GetTargetOrientation() const;
		glm::vec3 CalculateTargetPosition() const;
		glm::vec3 GetTargetForwardDirection() const;

		void FocusOn(glm::vec3 pos);

	private:
		void UpdateProjection();
		void UpdateView(TimeStep ts);

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f;
		float m_AspectRatio = 1.778f;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::quat m_Orientation;
		glm::vec2 m_InitialMousePosition;

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;

		float m_TargetPitch = 0.0f;
		float m_TargetYaw = 0.0f;
		float m_TargetDistance = 10.0f;

		glm::vec3 m_TargetPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_TargetFocalPoint = { 0.0f, 0.0f, 0.0f };

		float m_SmoothTransitionTime = 15.0f;

		float m_ViewportWidth = 1280;
		float m_ViewportHeight = 720;

	};
}