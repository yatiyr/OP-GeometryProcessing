#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/CameraSystem/EditorCamera.h>

#include <GeoProcess/System/InputSystem/Input.h>
#include <GeoProcess/System/InputSystem/KeyCodes.h>
#include <GeoProcess/System/InputSystem/MouseButtonCodes.h>

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace GP
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov),
		  m_AspectRatio(aspectRatio),
		  Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		m_FarClip  = farClip;
		m_NearClip = nearClip;

		m_Orientation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView(TimeStep ts)
	{
		float interpolationVal = std::clamp(m_SmoothTransitionTime * ts, 0.0f, 1.0f);

		m_FocalPoint = glm::mix(m_FocalPoint, m_TargetFocalPoint, interpolationVal);
		m_Yaw        = m_Yaw * (1.0f - interpolationVal) + m_TargetYaw * interpolationVal;
		m_Pitch      = m_Pitch * (1.0f - interpolationVal) + m_TargetPitch * interpolationVal;
		m_Distance   = m_Distance * (1.0f - interpolationVal) + m_TargetDistance * interpolationVal;

		m_Orientation = GetOrientation();
		m_Position = m_FocalPoint - GetForwardDirection() * m_Distance;

		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(m_Orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const { return 0.8f; }
	float EditorCamera::ZoomSpeed() const
	{
		float distance = std::max(m_Distance * 0.2f, 0.0f);
		float speed    = std::min(distance * distance, 100.0f);

		return speed;
	}

	void EditorCamera::OnUpdate(TimeStep ts)
	{
		if (Input::IsKeyPressed(Key::LeftAlt))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(MouseButton::ButtonMiddle))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(MouseButton::ButtonRight))
				MouseZoom(delta.y);
		}

		UpdateView(ts);
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNCTION(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();

		m_TargetFocalPoint += -GetRightDirection() * delta.x * xSpeed * m_TargetDistance;
		m_TargetFocalPoint += GetUpDirection() * delta.y * ySpeed * m_TargetDistance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0.0 ? -1.0f : 1.0f;

		m_TargetYaw   += yawSign * delta.x * RotationSpeed();
		m_TargetPitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_TargetDistance -= delta * ZoomSpeed();
		m_TargetDistance = m_TargetDistance < 1.0f ? 1.0f : m_TargetDistance;
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(m_Orientation, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(m_Orientation, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(m_Orientation, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::vec3 EditorCamera::CalculateTargetPosition() const
	{
		return m_TargetFocalPoint - GetTargetForwardDirection() * m_TargetDistance;
	}

	glm::vec3 EditorCamera::GetTargetForwardDirection() const
	{
		return glm::rotate(GetTargetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	void EditorCamera::FocusOn(glm::vec3 pos)
	{
		m_TargetFocalPoint = pos;
	}

	glm::quat EditorCamera::GetTargetOrientation() const
	{
		return glm::quat(glm::vec3(-m_TargetPitch, -m_TargetYaw, 0.0f));
	}

}