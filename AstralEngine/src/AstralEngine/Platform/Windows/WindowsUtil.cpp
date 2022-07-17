#include "aepch.h"
#include "WindowsUtil.h"

namespace AstralEngine
{
	struct OpenGL1_1Module
	{
		static constexpr const char* s_moduleName = "opengl32.dll";
		HMODULE moduleHandle;
		OpenGL1_1Module()
		{
			moduleHandle = LoadLibrary(StrToWindowsStr(s_moduleName).c_str());

			if (moduleHandle == nullptr)
			{
				AE_CORE_ERROR("Could not load \'%s\'. Error Code: %L", s_moduleName, GetLastError());
			}
		}

		~OpenGL1_1Module()
		{
			if (FreeLibrary(moduleHandle) == 0)
			{
				AE_CORE_ERROR("Could not free \'%s\'. Error Code: %L", s_moduleName, GetLastError());
			}
		}
	};

	WindowsStr StrToWindowsStr(const std::string& str)
	{
		#ifdef UNICODE
			// fill wstring with space character then copy str to wstr
			std::wstring wstr = std::wstring(str.length(), ' ');
			std::copy(str.begin(), str.end(), wstr.begin());
			return wstr;
		#else
			return str;
		#endif
	}

	std::string WindowsStrToStr(const WindowsStr& windowStr)
	{
		#ifdef UNICODE
			// fill string with space character then copy wstr to str
			std::string str = std::string(windowStr.length(), ' ');
			std::copy(windowStr.begin(), windowStr.end(), str.begin());
			return str;
		#else
			return str;
		#endif
	}

	std::wstring WindowsStrToWStr(const WindowsStr& windowStr)
	{
		#ifdef UNICODE
			return windowStr;
		#else
			std::wstring wstr = std::wstring(windowStr.length, ' ');
			std::copy(windowStr.begin(), windowStr.end(), wstr.begin());
			return wstr;
		#endif
	}

	void* RetrieveOpenGLFunction(const char* functionName)
	{
		void* function = wglGetProcAddress(functionName);
		if (function == nullptr)
		{
			static OpenGL1_1Module opengl1_1Module;

			function = GetProcAddress(opengl1_1Module.moduleHandle, functionName);

			if (function == nullptr)
			{
				AE_CORE_ERROR("Could not load function \'%s\'. Error Code: %L", functionName, GetLastError());
			}
		}
		return function;
	}

	bool CheckIfOpenGLExtensionIsSupported(const char* extensionName)
	{
		// function which returns a list of the supported extensions
		static const char* (*getSupportedExtensionStr)()
			= (const char* (*)())wglGetProcAddress("wglGetExtensionsStringEXT");

		if (getSupportedExtensionStr == nullptr)
		{
			AE_CORE_ERROR("WindowsWindow could not retrieve \"wglGetExtensionsStringEXT\"");
			return false;
		}

		// check if the extention name is in the list
		return std::strstr(getSupportedExtensionStr(), extensionName) != nullptr;
	}

	std::uint16_t& GetLowOrderWord(std::uint32_t& i)
	{
		return reinterpret_cast<std::uint16_t&>(i);
	}

	std::uint32_t& GetLowOrderWord(std::uint64_t& i)
	{
		return reinterpret_cast<std::uint32_t&>(i);
	}

	std::uint16_t& GetHighOrderWord(std::uint32_t& i)
	{
		return *(&reinterpret_cast<std::uint16_t&>(i) + 1);
	}

	std::uint32_t& GetHighOrderWord(std::uint64_t& i)
	{
		return *(&reinterpret_cast<std::uint32_t&>(i) + 1);
	}

	//returns KeyCode::Count if the key could not be found/mapped
	KeyCode WindowsKeyCodesToInternalKeyCode(int windowKeyCode)
	{
		switch(windowKeyCode)
		{
			case VK_SPACE: return KeyCode::Space;
			case VK_OEM_7: return KeyCode::Apostrophe;
			case VK_OEM_COMMA: return KeyCode::Comma;
			case VK_OEM_MINUS: return KeyCode::Minus;
			case VK_OEM_PERIOD: return KeyCode::Period;
			case VK_DIVIDE: return KeyCode::Slash;

			case 0x30: return KeyCode::D0;
			case 0x31: return KeyCode::D1;
			case 0x32: return KeyCode::D2;
			case 0x33: return KeyCode::D3;
			case 0x34: return KeyCode::D4;
			case 0x35: return KeyCode::D5;
			case 0x36: return KeyCode::D6;
			case 0x37: return KeyCode::D7;
			case 0x38: return KeyCode::D8;
			case 0x39: return KeyCode::D9;
			
			case VK_OEM_1: return KeyCode::Semicolon;
			case VK_OEM_3: return KeyCode::Tilda;

			case 0x41: return KeyCode::A;
			case 0x42: return KeyCode::B;
			case 0x43: return KeyCode::C;
			case 0x44: return KeyCode::D;
			case 0x45: return KeyCode::E;
			case 0x46: return KeyCode::F;
			case 0x47: return KeyCode::G;
			case 0x48: return KeyCode::H;
			case 0x49: return KeyCode::I;
			case 0x4A: return KeyCode::J;
			case 0x4B: return KeyCode::K;
			case 0x4C: return KeyCode::L;
			case 0x4D: return KeyCode::M;
			case 0x4E: return KeyCode::N;
			case 0x4F: return KeyCode::O;
			case 0x50: return KeyCode::P;
			case 0x51: return KeyCode::Q;
			case 0x52: return KeyCode::R;
			case 0x53: return KeyCode::S;
			case 0x54: return KeyCode::T;
			case 0x55: return KeyCode::U;
			case 0x56: return KeyCode::V;
			case 0x57: return KeyCode::W;
			case 0x58: return KeyCode::X;
			case 0x59: return KeyCode::Y;
			case 0x5A: return KeyCode::Z;
			
			case VK_OEM_4: return KeyCode::LeftBracket;
			case VK_OEM_5: return KeyCode::BackSlash;
			case VK_OEM_6: return KeyCode::RightBracket;

			case VK_ESCAPE: return KeyCode::Esc;
			case VK_RETURN: return KeyCode::Enter;
			case VK_TAB: return KeyCode::Tab;
			case VK_BACK: return KeyCode::Backspace;
			case VK_INSERT: return KeyCode::Insert;
			case VK_DELETE: return KeyCode::Delete;
			case VK_RIGHT: return KeyCode::RightArrow;
			case VK_LEFT: return KeyCode::LeftArrow;
			case VK_DOWN: return KeyCode::DownArrow;
			case VK_UP: return KeyCode::UpArrow;
			case VK_PRIOR: return KeyCode::PageUp;
			case VK_NEXT: return KeyCode::PageDown;

			case VK_HOME: return KeyCode::Home;
			case VK_END: return KeyCode::End;
			case VK_CAPITAL: return KeyCode::CapsLock;
			case VK_SCROLL: return KeyCode::ScrollLock;
			case VK_NUMLOCK: return KeyCode::NumLock;
			case VK_SNAPSHOT: return KeyCode::PrintScreen;
			case VK_PAUSE: return KeyCode::Pause;
			case VK_F1: return KeyCode::F1;
			case VK_F2: return KeyCode::F2;
			case VK_F3: return KeyCode::F3;
			case VK_F4: return KeyCode::F4;
			case VK_F5: return KeyCode::F5;
			case VK_F6: return KeyCode::F6;
			case VK_F7: return KeyCode::F7;
			case VK_F8: return KeyCode::F8;
			case VK_F9: return KeyCode::F9;
			case VK_F10: return KeyCode::F10;
			case VK_F11: return KeyCode::F11;
			case VK_F12: return KeyCode::F12;
			case VK_F13: return KeyCode::F13;
			case VK_F14: return KeyCode::F14;
			case VK_F15: return KeyCode::F15;
			case VK_F16: return KeyCode::F16;
			case VK_F17: return KeyCode::F17;
			case VK_F18: return KeyCode::F18;
			case VK_F19: return KeyCode::F19;
			case VK_F20: return KeyCode::F20;
			case VK_F21: return KeyCode::F21;
			case VK_F22: return KeyCode::F22;
			case VK_F23: return KeyCode::F23;
			case VK_F24: return KeyCode::F24;

			case VK_NUMPAD0: return KeyCode::KeyPad0;
			case VK_NUMPAD1: return KeyCode::KeyPad1;
			case VK_NUMPAD2: return KeyCode::KeyPad2;
			case VK_NUMPAD3: return KeyCode::KeyPad3;
			case VK_NUMPAD4: return KeyCode::KeyPad4;
			case VK_NUMPAD5: return KeyCode::KeyPad5;
			case VK_NUMPAD6: return KeyCode::KeyPad6;
			case VK_NUMPAD7: return KeyCode::KeyPad7;
			case VK_NUMPAD8: return KeyCode::KeyPad8;
			case VK_NUMPAD9: return KeyCode::KeyPad9;
	
			case VK_DECIMAL: return KeyCode::KeyPadDecimal;
			case VK_MULTIPLY: return KeyCode::KeyPadMultiply;
			case VK_SUBTRACT: return KeyCode::KeyPadSubstract;
			case VK_ADD: return KeyCode::KeyPadAdd;

			case VK_SHIFT:
			case VK_LSHIFT: return KeyCode::LeftShift;


			case VK_CONTROL:
			case VK_LCONTROL: return KeyCode::LeftCtrl;

			case VK_LMENU: return KeyCode::LeftAlt;
			case VK_LWIN: return KeyCode::LeftSystem;
			case VK_RSHIFT: return KeyCode::RightShift;
			case VK_RCONTROL: return KeyCode::RightCtrl;
			case VK_RMENU: return KeyCode::RightAlt;
			case VK_RWIN: return KeyCode::RightSystem;
			case VK_MENU: return KeyCode::Menu;
		}
		
		return KeyCode::Count;
	}

	MouseButtonCode WindowsMouseCodesToInternalMouseCode(int windowMouseCode)
	{
		switch(windowMouseCode)
		{
			case VK_LBUTTON: return MouseButtonCode::Left;
			case VK_RBUTTON: return MouseButtonCode::Right;
			case VK_MBUTTON: return MouseButtonCode::Middle;
			case VK_XBUTTON1: return MouseButtonCode::B4;
			case VK_XBUTTON2: return MouseButtonCode::B5;
		}
		return MouseButtonCode::Count;
	}
}