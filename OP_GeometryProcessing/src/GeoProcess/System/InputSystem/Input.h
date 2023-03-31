#pragma once

#include <Precomp.h>
#include <GeoProcess/System/CoreSystem/Core.h>
#include <GeoProcess/System/InputSystem/MouseButtonCodes.h>
#include <GeoProcess/System/InputSystem/KeyCodes.h>

namespace GP
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseButtonCode button);

		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}