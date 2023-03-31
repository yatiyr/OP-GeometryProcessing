#pragma once

#include <Precomp.h>

namespace GP
{

	typedef enum class KeyCode : uint16_t
	{
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

#define GP_KEY_SPACE           ::GP::Key::Space
#define GP_KEY_APOSTROPHE      ::GP::Key::Apostrophe    /* ' */
#define GP_KEY_COMMA           ::GP::Key::Comma         /* , */
#define GP_KEY_MINUS           ::GP::Key::Minus         /* - */
#define GP_KEY_PERIOD          ::GP::Key::Period        /* . */
#define GP_KEY_SLASH           ::GP::Key::Slash         /* / */
#define GP_KEY_0               ::GP::Key::D0
#define GP_KEY_1               ::GP::Key::D1
#define GP_KEY_2               ::GP::Key::D2
#define GP_KEY_3               ::GP::Key::D3
#define GP_KEY_4               ::GP::Key::D4
#define GP_KEY_5               ::GP::Key::D5
#define GP_KEY_6               ::GP::Key::D6
#define GP_KEY_7               ::GP::Key::D7
#define GP_KEY_8               ::GP::Key::D8
#define GP_KEY_9               ::GP::Key::D9
#define GP_KEY_SEMICOLON       ::GP::Key::Semicolon     /* ; */
#define GP_KEY_EQUAL           ::GP::Key::Equal         /* = */
#define GP_KEY_A               ::GP::Key::A
#define GP_KEY_B               ::GP::Key::B
#define GP_KEY_C               ::GP::Key::C
#define GP_KEY_D               ::GP::Key::D
#define GP_KEY_E               ::GP::Key::E
#define GP_KEY_F               ::GP::Key::F
#define GP_KEY_G               ::GP::Key::G
#define GP_KEY_H               ::GP::Key::H
#define GP_KEY_I               ::GP::Key::I
#define GP_KEY_J               ::GP::Key::J
#define GP_KEY_K               ::GP::Key::K
#define GP_KEY_L               ::GP::Key::L
#define GP_KEY_M               ::GP::Key::M
#define GP_KEY_N               ::GP::Key::N
#define GP_KEY_O               ::GP::Key::O
#define GP_KEY_P               ::GP::Key::P
#define GP_KEY_Q               ::GP::Key::Q
#define GP_KEY_R               ::GP::Key::R
#define GP_KEY_S               ::GP::Key::S
#define GP_KEY_T               ::GP::Key::T
#define GP_KEY_U               ::GP::Key::U
#define GP_KEY_V               ::GP::Key::V
#define GP_KEY_W               ::GP::Key::W
#define GP_KEY_X               ::GP::Key::X
#define GP_KEY_Y               ::GP::Key::Y
#define GP_KEY_Z               ::GP::Key::Z
#define GP_KEY_LEFT_BRACKET    ::GP::Key::LeftBracket   /* [ */
#define GP_KEY_BACKSLASH       ::GP::Key::Backslash     /* \ */
#define GP_KEY_RIGHT_BRACKET   ::GP::Key::RightBracket  /* ] */
#define GP_KEY_GRAVE_ACCENT    ::GP::Key::GraveAccent   /* ` */
#define GP_KEY_WORLD_1         ::GP::Key::World1        /* non-US #1 */
#define GP_KEY_WORLD_2         ::GP::Key::World2        /* non-US #2 */

/* Function keys */
#define GP_KEY_ESCAPE          ::GP::Key::Escape
#define GP_KEY_ENTER           ::GP::Key::Enter
#define GP_KEY_TAB             ::GP::Key::Tab
#define GP_KEY_BACKSPACE       ::GP::Key::Backspace
#define GP_KEY_INSERT          ::GP::Key::Insert
#define GP_KEY_DELETE          ::GP::Key::Delete
#define GP_KEY_RIGHT           ::GP::Key::Right
#define GP_KEY_LEFT            ::GP::Key::Left
#define GP_KEY_DOWN            ::GP::Key::Down
#define GP_KEY_UP              ::GP::Key::Up
#define GP_KEY_PAGE_UP         ::GP::Key::PageUp
#define GP_KEY_PAGE_DOWN       ::GP::Key::PageDown
#define GP_KEY_HOME            ::GP::Key::Home
#define GP_KEY_END             ::GP::Key::End
#define GP_KEY_CAPS_LOCK       ::GP::Key::CapsLock
#define GP_KEY_SCROLL_LOCK     ::GP::Key::ScrollLock
#define GP_KEY_NUM_LOCK        ::GP::Key::NumLock
#define GP_KEY_PRINT_SCREEN    ::GP::Key::PrintScreen
#define GP_KEY_PAUSE           ::GP::Key::Pause
#define GP_KEY_F1              ::GP::Key::F1
#define GP_KEY_F2              ::GP::Key::F2
#define GP_KEY_F3              ::GP::Key::F3
#define GP_KEY_F4              ::GP::Key::F4
#define GP_KEY_F5              ::GP::Key::F5
#define GP_KEY_F6              ::GP::Key::F6
#define GP_KEY_F7              ::GP::Key::F7
#define GP_KEY_F8              ::GP::Key::F8
#define GP_KEY_F9              ::GP::Key::F9
#define GP_KEY_F10             ::GP::Key::F10
#define GP_KEY_F11             ::GP::Key::F11
#define GP_KEY_F12             ::GP::Key::F12
#define GP_KEY_F13             ::GP::Key::F13
#define GP_KEY_F14             ::GP::Key::F14
#define GP_KEY_F15             ::GP::Key::F15
#define GP_KEY_F16             ::GP::Key::F16
#define GP_KEY_F17             ::GP::Key::F17
#define GP_KEY_F18             ::GP::Key::F18
#define GP_KEY_F19             ::GP::Key::F19
#define GP_KEY_F20             ::GP::Key::F20
#define GP_KEY_F21             ::GP::Key::F21
#define GP_KEY_F22             ::GP::Key::F22
#define GP_KEY_F23             ::GP::Key::F23
#define GP_KEY_F24             ::GP::Key::F24
#define GP_KEY_F25             ::GP::Key::F25

/* Keypad */
#define GP_KEY_KP_0            ::GP::Key::KP0
#define GP_KEY_KP_1            ::GP::Key::KP1
#define GP_KEY_KP_2            ::GP::Key::KP2
#define GP_KEY_KP_3            ::GP::Key::KP3
#define GP_KEY_KP_4            ::GP::Key::KP4
#define GP_KEY_KP_5            ::GP::Key::KP5
#define GP_KEY_KP_6            ::GP::Key::KP6
#define GP_KEY_KP_7            ::GP::Key::KP7
#define GP_KEY_KP_8            ::GP::Key::KP8
#define GP_KEY_KP_9            ::GP::Key::KP9
#define GP_KEY_KP_DECIMAL      ::GP::Key::KPDecimal
#define GP_KEY_KP_DIVIDE       ::GP::Key::KPDivide
#define GP_KEY_KP_MULTIPLY     ::GP::Key::KPMultiply
#define GP_KEY_KP_SUBTRACT     ::GP::Key::KPSubtract
#define GP_KEY_KP_ADD          ::GP::Key::KPAdd
#define GP_KEY_KP_ENTER        ::GP::Key::KPEnter
#define GP_KEY_KP_EQUAL        ::GP::Key::KPEqual

#define GP_KEY_LEFT_SHIFT      ::GP::Key::LeftShift
#define GP_KEY_LEFT_CONTROL    ::GP::Key::LeftControl
#define GP_KEY_LEFT_ALT        ::GP::Key::LeftAlt
#define GP_KEY_LEFT_SUPER      ::GP::Key::LeftSuper
#define GP_KEY_RIGHT_SHIFT     ::GP::Key::RightShift
#define GP_KEY_RIGHT_CONTROL   ::GP::Key::RightControl
#define GP_KEY_RIGHT_ALT       ::GP::Key::RightAlt
#define GP_KEY_RIGHT_SUPER     ::GP::Key::RightSuper
#define GP_KEY_MENU            ::GP::Key::Menu

#define GP_KEY_LAST               GP_KEY_MENU