#pragma once

#define AE_KEY_SPACE              32
#define AE_KEY_APOSTROPHE         39  /* ' */
#define AE_KEY_COMMA              44  /* , */
#define AE_KEY_MINUS              45  /* - */
#define AE_KEY_PERIOD             46  /* . */
#define AE_KEY_SLASH              47  /* / */
#define AE_KEY_0                  48
#define AE_KEY_1                  49
#define AE_KEY_2                  50
#define AE_KEY_3                  51
#define AE_KEY_4                  52
#define AE_KEY_5                  53
#define AE_KEY_6                  54
#define AE_KEY_7                  55
#define AE_KEY_8                  56
#define AE_KEY_9                  57
#define AE_KEY_SEMICOLON          59  /* ; */
#define AE_KEY_EQUAL              61  /* = */
#define AE_KEY_A                  65
#define AE_KEY_B                  66
#define AE_KEY_C                  67
#define AE_KEY_D                  68
#define AE_KEY_E                  69
#define AE_KEY_F                  70
#define AE_KEY_G                  71
#define AE_KEY_H                  72
#define AE_KEY_I                  73
#define AE_KEY_J                  74
#define AE_KEY_K                  75
#define AE_KEY_L                  76
#define AE_KEY_M                  77
#define AE_KEY_N                  78
#define AE_KEY_O                  79
#define AE_KEY_P                  80
#define AE_KEY_Q                  81
#define AE_KEY_R                  82
#define AE_KEY_S                  83
#define AE_KEY_T                  84
#define AE_KEY_U                  85
#define AE_KEY_V                  86
#define AE_KEY_W                  87
#define AE_KEY_X                  88
#define AE_KEY_Y                  89
#define AE_KEY_Z                  90
#define AE_KEY_LEFT_BRACKET       91  /* [ */
#define AE_KEY_BACKSLASH          92  /* \ */
#define AE_KEY_RIGHT_BRACKET      93  /* ] */
#define AE_KEY_GRAVE_ACCENT       96  /* ` */
#define AE_KEY_WORLD_1            161 

#define AE_KEY_ESC	              256
#define AE_KEY_ENTER              257
#define AE_KEY_TAB                258
#define AE_KEY_BACKSPACE          259
#define AE_KEY_INSERT             260
#define AE_KEY_DELETE             261
#define AE_KEY_RIGHT              262
#define AE_KEY_LEFT               263
#define AE_KEY_DOWN               264
#define AE_KEY_UP                 265
#define AE_KEY_PAGE_UP            266
#define AE_KEY_PAGE_DOWN          267
#define AE_KEY_HOME               268
#define AE_KEY_END                269
#define AE_KEY_CAPS_LOCK          280
#define AE_KEY_SCROLL_LOCK        281
#define AE_KEY_NUM_LOCK           282
#define AE_KEY_PRINT_SCREEN       283
#define AE_KEY_PAUSE              284
#define AE_KEY_F1                 290
#define AE_KEY_F2                 291
#define AE_KEY_F3                 292
#define AE_KEY_F4                 293
#define AE_KEY_F5                 294
#define AE_KEY_F6                 295
#define AE_KEY_F7                 296
#define AE_KEY_F8                 297
#define AE_KEY_F9                 298
#define AE_KEY_F10                299
#define AE_KEY_F11                300
#define AE_KEY_F12                301
#define AE_KEY_F13                302
#define AE_KEY_F14                303
#define AE_KEY_F15                304
#define AE_KEY_F16                305
#define AE_KEY_F17                306
#define AE_KEY_F18                307
#define AE_KEY_F19                308
#define AE_KEY_F20                309
#define AE_KEY_F21                310
#define AE_KEY_F22                311
#define AE_KEY_F23                312
#define AE_KEY_F24                313
#define AE_KEY_F25                314
#define AE_KEY_KP_0               320
#define AE_KEY_KP_1               321
#define AE_KEY_KP_2               322
#define AE_KEY_KP_3               323
#define AE_KEY_KP_4               324
#define AE_KEY_KP_5               325
#define AE_KEY_KP_6               326
#define AE_KEY_KP_7               327
#define AE_KEY_KP_8               328
#define AE_KEY_KP_9               329
#define AE_KEY_KP_DECIMAL         330
#define AE_KEY_KP_DIVIDE          331
#define AE_KEY_KP_MULTIPLY        332
#define AE_KEY_KP_SUBTRACT        333
#define AE_KEY_KP_ADD             334
#define AE_KEY_KP_ENTER           335
#define AE_KEY_KP_EQUAL           336
#define AE_KEY_LEFT_SHIFT         340
#define AE_KEY_LEFT_CONTROL       341
#define AE_KEY_LEFT_ALT           342
#define AE_KEY_LEFT_SUPER         343
#define AE_KEY_RIGHT_SHIFT        344
#define AE_KEY_RIGHT_CONTROL      345
#define AE_KEY_RIGHT_ALT          346
#define AE_KEY_RIGHT_SUPER        347
#define AE_KEY_MENU               348
		
#define AE_KEY_LAST               AE_KEY_MENU


namespace AstralEngine
{
	enum class KeyCode
	{
		Space = AE_KEY_SPACE,
		Apostrophe = AE_KEY_APOSTROPHE,
		Comma = AE_KEY_COMMA,
		Minus = AE_KEY_MINUS,
		Period = AE_KEY_PERIOD,      
		Slash = AE_KEY_SLASH,
		
		D0 = AE_KEY_0,
		D1 = AE_KEY_1,
		D2 = AE_KEY_2,
		D3 = AE_KEY_3,
		D4 = AE_KEY_4,
		D5 = AE_KEY_5,
		D6 = AE_KEY_6,
		D7 = AE_KEY_7,
		D8 = AE_KEY_8,
		D9 = AE_KEY_0,
		
		Semicolon = AE_KEY_SEMICOLON,   
		Equal = AE_KEY_EQUAL,       
		
		A = AE_KEY_A,           
		B = AE_KEY_B,           
		C = AE_KEY_C,           
		D = AE_KEY_D,           
		E = AE_KEY_E,           
		F = AE_KEY_F,           
		G = AE_KEY_G,           
		H = AE_KEY_H,           
		I = AE_KEY_I,           
		J = AE_KEY_J,           
		K = AE_KEY_K,           
		L = AE_KEY_L,           
		M = AE_KEY_M,           
		N = AE_KEY_N,           
		O = AE_KEY_O,           
		P = AE_KEY_P,
		Q = AE_KEY_Q,
		R = AE_KEY_R,
		S = AE_KEY_S,
		T = AE_KEY_T,
		U = AE_KEY_U,
		V = AE_KEY_V,
		W = AE_KEY_W,
		X = AE_KEY_X,
		Y = AE_KEY_Y,
		Z = AE_KEY_Z,
		
		LeftBracket = AE_KEY_LEFT_BRACKET,
		BackSlash = AE_KEY_BACKSLASH, 
		RightBracket = AE_KEY_RIGHT_BRACKET,
		GraveAccent = AE_KEY_GRAVE_ACCENT,
		World1 = AE_KEY_WORLD_1,
		
		Esc = AE_KEY_ESC,	       
		Enter = AE_KEY_ENTER,  
		Tab = AE_KEY_TAB,
		Backspace = AE_KEY_BACKSPACE, 
		Insert = AE_KEY_INSERT,
		Delete = AE_KEY_DELETE,
		RightArrow = AE_KEY_RIGHT,       
		LeftArrow = AE_KEY_LEFT,      
		DownArrow = AE_KEY_DOWN,
		UpArrow = AE_KEY_UP,
		PageUp = AE_KEY_PAGE_UP,   
		PageDown = AE_KEY_PAGE_DOWN,
		
		Home = AE_KEY_HOME,       
		End = AE_KEY_END,
		CapsLock = AE_KEY_CAPS_LOCK,  
		ScrollLock = AE_KEY_SCROLL_LOCK,
		NumLock = AE_KEY_NUM_LOCK, 
		PrintScreen = AE_KEY_PRINT_SCREEN,
		Pause = AE_KEY_PAUSE,     
		F1 = AE_KEY_F1,          
		F2 = AE_KEY_F2,          
		F3 = AE_KEY_F3,          
		F4 = AE_KEY_F4,          
		F5 = AE_KEY_F5,          
		F6 = AE_KEY_F6,          
		F7 = AE_KEY_F7,          
		F8 = AE_KEY_F8,          
		F9 = AE_KEY_F9,          
		F10 = AE_KEY_F10,
		F11 = AE_KEY_F11,
		F12 = AE_KEY_F12,
		F13 = AE_KEY_F13,
		F14 = AE_KEY_F14,
		F15 = AE_KEY_F15,
		F16 = AE_KEY_F16,
		F17 = AE_KEY_F17,
		F18 = AE_KEY_F18,
		F19 = AE_KEY_F19,         
		F20 = AE_KEY_F20,         
		F21 = AE_KEY_F21,         
		F22 = AE_KEY_F22,         
		F23 = AE_KEY_F23,         
		F24 = AE_KEY_F24,         
		F25 = AE_KEY_F25, 

		KP0 = AE_KEY_KP_0,        
		KP1 = AE_KEY_KP_1,
		KP2 = AE_KEY_KP_2,
		KP3 = AE_KEY_KP_3,        
		KP4 = AE_KEY_KP_4,        
		KP5 = AE_KEY_KP_5,        
		KP6 = AE_KEY_KP_6,        
		KP7 = AE_KEY_KP_7,        
		KP8 = AE_KEY_KP_8,        
		KP9 = AE_KEY_KP_9,        
		
		KPDecimal = AE_KEY_KP_DECIMAL, 
		KPDivide = AE_KEY_KP_DIVIDE,   
		KPMultiply = AE_KEY_KP_MULTIPLY, 
		KPSubstract = AE_KEY_KP_SUBTRACT, 
		KPAdd = AE_KEY_KP_ADD,      
		KPEnter = AE_KEY_KP_ENTER,    
		KPEqual = AE_KEY_KP_EQUAL,    
		
		LeftShift = AE_KEY_LEFT_SHIFT,  
		LeftCtr = AE_KEY_LEFT_CONTROL,
		LeftAlt = AE_KEY_LEFT_ALT,    
		LeftSuper = AE_KEY_LEFT_SUPER,  
		RightShift = AE_KEY_RIGHT_SHIFT, 
		RightCtr = AE_KEY_RIGHT_CONTROL,
		RightAlt = AE_KEY_RIGHT_ALT,  
		RightSuper = AE_KEY_RIGHT_SUPER, 
		Menu = AE_KEY_MENU,        
		
		Last = AE_KEY_LAST
	};
}