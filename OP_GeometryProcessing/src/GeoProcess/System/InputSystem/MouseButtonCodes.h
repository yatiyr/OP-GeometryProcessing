#pragma once

#include <Precomp.h>

namespace GP
{
	typedef enum class MouseButtonCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} MouseButton;

	inline std::ostream& operator<<(std::ostream& os, MouseButtonCode mouseButtonCode)
	{
		os << static_cast<int32_t>(mouseButtonCode);
		return os;
	}
}

/* Mouse button code taken from glfw */
#define GP_MOUSE_BUTTON_0      ::GP::Mouse::Button0
#define GP_MOUSE_BUTTON_1      ::GP::Mouse::Button1
#define GP_MOUSE_BUTTON_2      ::GP::Mouse::Button2
#define GP_MOUSE_BUTTON_3      ::GP::Mouse::Button3
#define GP_MOUSE_BUTTON_4      ::GP::Mouse::Button4
#define GP_MOUSE_BUTTON_5      ::GP::Mouse::Button5
#define GP_MOUSE_BUTTON_6      ::GP::Mouse::Button6
#define GP_MOUSE_BUTTON_7      ::GP::Mouse::Button7
#define GP_MOUSE_BUTTON_LAST   ::GP::Mouse::ButtonLast
#define GP_MOUSE_BUTTON_LEFT   ::GP::Mouse::ButtonLeft
#define GP_MOUSE_BUTTON_RIGHT  ::GP::Mouse::ButtonRight
#define GP_MOUSE_BUTTON_MIDDLE ::GP::Mouse::ButtonMiddle