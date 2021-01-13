#pragma once
#include "AstralEngine/Core/Core.h"
#include "AstralEngine/Core/Keycodes.h"
#include "AstralEngine/Core/AWindow.h"
#include "Math/AMath.h"
#include "Data Struct/ADynArr.h"
#include "Data Struct/ADelegate.h"
#include "Data Struct/AUnorderedMap.h"
#include "Renderer/Texture.h"

#define UNICODE_CODEPOINT_MAX 0xFFFF
#define TEXT_EDIT_UNDOSTATECOUNT 99
#define TEXT_EDIT_UNDOCHARCOUNT 999

namespace AstralEngine
{
	enum class NavInput
	{
		// Gamepad Mapping
		South,      //X on play station, A on xbox
		East,       //O on play station, B on xbox
		North,      //triangle on play station, Y on xbox
		West,       //square on play station, X on xbox
		DpadLeft,  
		DpadRight, 
		DpadUp,    
		DpadDown,  
		LStickLeft, 
		LStickRight,
		LStickUp,   
		LStickDown, 
		
		FocusPrev,     // next window (w/ PadMenu)                     // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
		FocusNext,     // prev window (w/ PadMenu)                     // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)
		TweakSlow,     // slower tweaks                                // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
		TweakFast,     // faster tweaks                                // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)


		//needed?

		// used internally to differentiate keyboard from gamepad inputs for behaviors that require to differentiate them.
		// Keyboard behavior that have no corresponding gamepad mapping (e.g. CTRL+TAB) will be directly reading from io.KeysDown[] instead of io.NavInputs[].
		KeyMenu,      // toggle menu                                  // = io.KeyAlt
		KeyLeft,      // move left                                    // = Arrow keys
		KeyRight,     // move right
		KeyUp,        // move up
		KeyDown,      // move down
		Count,
		InternalStart = KeyMenu
	};

	enum class MouseCursor
	{
		None = -1,
		Arrow = 0,
		TextInput,         // When hovering over InputText, etc.
		ResizeAll,         // Unused
		ResizeNS,          // When hovering over an horizontal border
		ResizeEW,          // When hovering over a vertical border or a column
		ResizeNESW,        // When hovering over the bottom-left corner of a window
		ResizeNWSE,        // When hovering over the bottom-right corner of a window
		Hand,              // hand symbol (ex: hovering button)
		NotAllowed,        // When hovering something with disallowed interaction. Usually a crossed circle.
		Count
	};

	struct FontConfig
	{
		void* fontData;
		int fontDataSize;
		bool fontDataOwnedByAtlas;
		int fontNum;
		float sizePixels;
		int oversampleH;
		int oversampleV;
		bool pixelSnapH;
		Vector2 glyphExtraSpacing;
		Vector2 glyphOffset;
		const unsigned short* glyphRange;
		float glyphMinAdvanceX;
		float glyphMaxAdvanceX;
		bool mergeMode;
		unsigned int rasterizerFlags;
		float rasterizerMultiply;
		unsigned short ellipsisChar;

		//internal
		char name[40];
		Font* dstFont; //dist font?

		FontConfig();
	};

	struct Glyph
	{
		unsigned int codepoint = 31;
		unsigned int visible = 1; //flag to allow early out when rendering
		float AdvanceX;		  //distance to next character
		float X0, Y0, X1, Y1; //corners of glyph
		float U0, V0, U1, V1; //texture coord
	};

	class FontAtlas;

	//will not be used?
	struct DrawList;

	class Font
	{
	public:
		Font();

		~Font();
		
		const Glyph* FindGlyph(unsigned short c) const;
		
		const Glyph* FindGlyphNoFallback(unsigned short c) const;
		
		float GetCharAdvance(unsigned short c) const
		{
			return ((int)c < m_indexAdvanceX.GetCount()) ? m_indexAdvanceX[(size_t)c] : m_fallbackAdvanceX;
		}
		
		bool IsLoaded() const { return m_atlas != nullptr; }
		
		const char* GetDebugName() const { return m_configData != nullptr ? m_configData->name : "<Unknown>"; }

		Vector2 CalcTextSizeA(float size, float maxWidth, float wrapWidth, 
			const char* textBegin, const char* textEnd = nullptr, const char** remaining = nullptr) const;

		const char* CalcWordWrapPositionA(float scale, const char* text, const char* textEnd, float wrapWidth) const;

		void RenderChar(DrawList* drawList, float size, Vector2 pos, unsigned int col, unsigned short c) const;
		void RenderText(DrawList* drawList, float size, Vector2 pos, unsigned int col, const Vector4& clipRect, 
			const char* textBegin, const char* textEnd, float wrapWidth = 0.0f, bool cpuFineClip = false) const;


	private:
		void BuildLookupTable();
		void ClearOutputData();
		void GrowIndex(int newSize);
		void AddGlyph(unsigned short c, float x0, float y0, float x1, float y1, 
			float u0, float v0, float u1, float v1, float advanceX);
		void AddRemapchar(unsigned short dst, unsigned short src, bool overwriteDst = true);
		void SetGlyphVisible(unsigned short c, bool visible);
		void SetFallbackChar(unsigned short c);
		bool IsGlyphRangedUnused(unsigned int first, unsigned int last);

		ADynArr<float> m_indexAdvanceX;
		float m_fallbackAdvanceX;
		float m_fontSize;

		ADynArr<unsigned short> m_indexLookup;
		ADynArr<Glyph> m_glyphs;
		const Glyph* m_fallbackGlyph;
		Vector2 m_displayOffset;

		FontAtlas* m_atlas;
		const FontConfig* m_configData;
		short m_configDataCount;
		unsigned short m_fallbackChar;
		unsigned short m_ellipsisChar;
		bool m_dirtyLookupTables;
		float m_scale;
		float m_ascent, m_descent;
		int m_metricTotalSurface;
		unsigned char m_used4KPagesMap[(UNICODE_CODEPOINT_MAX + 1) / 4096 / 8];
	};

	enum FontAtlasFlags
	{
		None = 0,
		NoPowerOfTwoHeight = 1 << 0,   // Don't round the height to next power of two
		NoMouseCursors = 1 << 1    // Don't build software mouse cursors into the atlas
	};

	//custom struct used by the font atlas class
	struct FontAtlasCustomRect
	{
		unsigned short width;
		unsigned short height;
		unsigned short x, y;
		unsigned short glyphID;
		float glyphAdvanceX;
		Vector2 glyphOffset;
		Font* font;

		FontAtlasCustomRect() : width(0), height(0), x(0xFFFF), y(0xFFFF), 
			glyphID(0), glyphAdvanceX(0.0f), glyphOffset(Vector2::Zero()), font(nullptr) { }

		bool IsPacked() const { return x != 0xFFFF; }
	};

	class FontAtlas
	{
	public:
		FontAtlas();
		~FontAtlas();

		Font* AddFont(const FontConfig* fontConfig);
		Font* AddFontDefault(const FontConfig* fontConfig = nullptr);
		Font* AddFontFromFileTTF(const char* filename, float sizePixels, 
			const FontConfig* fontConfig = NULL, const unsigned short* glyphRanges = nullptr);
		Font* AddFontFromMemoryTTF(const void* fontData, int fontSize, float sizePixels, 
			const FontConfig* fontConfig = nullptr, const unsigned short* glyphRanges = nullptr);
		Font* AddFontFromMemoryCompressedTTF(const void* compressedFontData, int compressedFontSize, 
			float sizePixels, const FontConfig* fontConfig = nullptr, const unsigned short* glyphRanges = nullptr);
		Font* AddFontFromMemoryCompressedBase85TTF(const char* compressedFontDataBase85, 
			float sizePixels, const FontConfig* fontConfig = nullptr, const unsigned short* glyphRanges = nullptr);

		void ClearInputData();
		void ClearTexData();
		void ClearFonts();
		void Clear();

		bool Build();
		
		//1 byte per pixel
		void GetTexDataAsAlpha8(unsigned char** outPixels, int* outWidth, int* outHeight, int* outBytesPerPixel = nullptr); 

		//4 byte per pixel
		void GetTexDataAsRGBA32(unsigned char** outPixels, int* outWidth, int* outHeight, int* outBytesPerPixel = nullptr);

		bool IsBuilt() const { return m_fonts.GetCount() > 0 && m_texture != nullptr; }
		void SetTexture(AReference<Texture2D> texture) { m_texture = texture; }

		int AddCustomRectRegular(int width, int height);
		int AddCustomRectFontGlyph(Font* f, unsigned short id, int width, int height,
			float advanceX, const Vector2& offset = Vector2::Zero());

	private:
		//helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
		const unsigned short* GetGlyphRangesDefault();   // Basic Latin, Extended Latin
		const unsigned short* GetGlyphRangesKorean();	// Default + Korean characters
		const unsigned short* GetGlyphRangesJapanese();	 // Default + Hiragana, Katakana, Half-Width, Selection of 1946 Ideographs
		const unsigned short* GetGlyphRangesChineseFull(); // Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
		const unsigned short* GetGlyphRangesChineseSimplifiedCommon(); // Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
		const unsigned short* GetGlyphRangesCyrillic();	// Default + about 400 Cyrillic characters
		const unsigned short* GetGlyphRangesThai();	// Default + Thai characters
		const unsigned short* GetGlyphRangesVietnamese(); // Default + Vietnamese characters

		void CalculateCustomRectUV(const FontAtlasCustomRect* rect, Vector2* outUVMin, Vector2* outUVMax) const;

		bool GetMouseCursorTexData(MouseCursor cursor, Vector2* outOffset, Vector2* outSize, Vector2 outUVBorder[2], Vector2 outUVFill[2]);

		bool m_locked;
		FontAtlasFlags m_flags;
		void* m_textureID; //might not be used?
		int m_textureDesiredWidth;
		int m_textureGlyphPadding;

		//internal
		AReference<Texture2D> m_texture;
		ADynArr<Font*> m_fonts;
		ADynArr<FontConfig> m_configData;

	};

	enum UIConfigFlags
	{
		//Base
		None					= 0,
		NavEnableKeyboard		= 1 << 0,
		NavEnableGamepad		= 1 << 2,
		NavNoCaptureKeyboard	= 1 << 3,
		NoMouse					= 1 << 4,
		NoMouseCursorChange		= 1 << 5,

		//Docking
		DockingEnabled = 1 << 6,

		//Viewports
		ViewportEnabled			= 1 << 7,
		DpiEnableScaledViewport = 1 << 8,
		DpiEnableScaledFonts	= 1 << 9,

		//User storage (might not be used)
		IsSRBG			= 1 << 10,
		IsTouchScreen	= 1 << 11,
	};

	enum UIBackEndFlags
	{
		None					= 0,
		HasGamepad				= 1 << 0,
		HasMouseCursor			= 1 << 1,
		HasSetMousePos			= 1 << 2,
		RendererHasVtxOffset	= 1 << 3,

		//viewports
		PlatformHasViewports	= 1 << 4,
		HasMouseHoveredViewport = 1 << 5,
		RendererHasViewports	= 1 << 3,
	};

	class UIIO
	{
	public:

		UIIO();

		// Queue new character input
		void AddInputCharacter(unsigned int c);          
		
		// Queue new character input from an UTF-16 character, it can be a surrogate
		void AddInputCharacterUTF16(unsigned short c);
		
		// Queue new characters input from an UTF-8 string
		void AddInputCharactersUTF8(const char* str);
		
		// Clear the text input buffer manually
		void ClearInputCharacters();                     

	private:
		UIConfigFlags m_configFlags;
		UIBackEndFlags m_backEndFlags;
		Vector2 m_displaySize;
		float m_savingRate; //min time to wait before wait to save positions/sizes in sec
		const char* m_saveFileName; //file to which we write/read size and positions of windows
		const char* m_logFileName;
		float m_mouseDoubleClickTime;
		float m_mouseDoubleClickMaxDist;
		float m_mouseDragThreshold;
		float m_keyRepeatDelay;
		float m_keyRepeatRate;
		void* m_userData;

		FontAtlas** m_fonts;
		float m_fontGlobalScale;
		bool m_allowUserScaleFont;
		Font* m_defaultFont;
		Vector2 m_displayFramebufferScale;

		//Docking options (when enabled)

		bool m_configDockingNoSplit;
		bool m_configDockingWithShift;
		bool m_configDockingAlwaysTabBar;
		bool m_configDockingTransparentPayload;

		//viewport options (when enabled)
		bool m_configViewportNoAutoMerge;
		bool m_configViewportNoTaskBarIcon;
		bool m_configViewportNoDecoration;
		bool m_configViewportNoDefaultParent;

		//misc options

		bool m_drawMouseCursor; //draw the mouse cursor if true
		bool m_configMacOSXBehavior;
		bool m_configInputTextCursorBlink;
		bool m_configWindowsResizeFromEdges;
		bool m_configWindowsMoveFromTitleBarOnly;
		bool m_configWindowsMemoryCompactTimer;

		Vector2 m_mousePos;
		bool m_mouseDown[5];
		float m_mouseWheel;
		float m_mouseWheelH;
		unsigned int m_mouseHoveredViewport;
		float m_gamepadInputs[(int)NavInput::Count];


	};

	enum ViewportFlags
	{
		None = 0,
		NoDecoration = 1 << 0,   
		NoTaskBarIcon = 1 << 1,  
		NoFocusOnCreate = 1 << 2,
		NoFocusOnClick = 1 << 3,   
		NoInputs = 1 << 4,   //make mouse pass through so we can drag this window while peaking behind it.
		NoRendererClear = 1 << 5,   //renderer doesn't need to clear the framebuffer ahead (because we will fill it entirely).
		TopMost = 1 << 6,   //display on top
		Minimized = 1 << 7,   
		NoAutoMerge = 1 << 8,   
		CanHostOtherWindows = 1 << 9
	};

	class Viewport
	{
	public:
		Viewport() : m_id(0), m_flags(ViewportFlags::None), m_parentID(0) { }
		~Viewport();

		Vector2 GetCenter() { return Vector2(m_pos.x + m_size.x * 0.5f, m_pos.y + m_size.y * 0.5f); }
		Vector2 GetWorkPos() { return Vector2(m_pos.x + m_workOffsetMin.x, m_pos.y + m_workOffsetMin.y); }

		Vector2 GetWorkSize() 
		{ 
			return Vector2(m_size.x - m_workOffsetMin.x + m_workOffsetMax.x, m_size.y - m_workOffsetMin.y + m_workOffsetMax.y); 
		}

	protected:
		unsigned int m_id;
		ViewportFlags m_flags;
		Vector2 m_pos;
		Vector2 m_size;
		Vector2 m_workOffsetMin;
		Vector2 m_workOffsetMax;
		float m_dpiScale;
		unsigned int m_parentID;
	};

	enum class Direction
	{
		None = 0,
		Left,
		Right,
		Up,
		Down,
		Count
	};

	// Enumeration of where the color for the style goes
	enum class UIStyleColor
	{
		Text,
		TextDisabled,
		WindowBg,              // Background of normal windows
		ChildBg,               // Background of child windows
		PopupBg,               // Background of popups, menus, tooltips windows
		Border,
		BorderShadow,
		FrameBg,               // Background of checkbox, radio button, plot, slider, text input
		FrameBgHovered,
		FrameBgActive,
		TitleBg,
		TitleBgActive,
		TitleBgCollapsed,
		MenuBarBg,
		ScrollbarBg,
		ScrollbarGrab,
		ScrollbarGrabHovered,
		ScrollbarGrabActive,
		CheckMark,
		SliderGrab,
		SliderGrabActive,
		Button,
		ButtonHovered,
		ButtonActive,
		Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
		HeaderHovered,
		HeaderActive,
		Separator,
		SeparatorHovered,
		SeparatorActive,
		ResizeGrip,
		ResizeGripHovered,
		ResizeGripActive,
		Tab,
		TabHovered,
		TabActive,
		TabUnfocused,
		TabUnfocusedActive,
		DockingPreview,        // Preview overlay color when about to docking something
		DockingEmptyBg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
		PlotLines,
		PlotLinesHovered,
		PlotHistogram,
		PlotHistogramHovered,
		TextSelectedBg,
		DragDropTarget,
		NavHighlight,          // Gamepad/keyboard: current highlighted item
		NavWindowingHighlight, // Highlight window when using CTRL+TAB
		NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
		ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
		Count
	};

	//flags for editing/picking colors
	enum ColorEditFlags
	{
		None = 0,
		NoAlpha = 1 << 1,
		NoPicker = 1 << 2,   
		NoOptions = 1 << 3, //disable toggling options menu when right-clicking on inputs/small preview
		NoSmallPreview = 1 << 4, //disable colored square preview next to the inputs just show the inputs
		NoInputs = 1 << 5,   //disable inputs sliders/text widgets just show preview colored square
		NoTooltip = 1 << 6,   
		NoLabel = 1 << 7,  
		NoSidePreview = 1 << 8,  
		NoDragDrop = 1 << 9, 
		NoBorder = 1 << 10, //on by default

		//user options
		AlphaBar = 1 << 16,  
		AlphaPreview = 1 << 17,
		AlphaPreviewHalf = 1 << 18,  //display half opaque / half checkerboard, instead of opaque.
		HDR = 1 << 19,
		DisplayRGB = 1 << 20, 
		DisplayHSV = 1 << 21, 
		DisplayHex = 1 << 22,
		Uint8 = 1 << 23,  //values formatted as 0 to 255
		Float = 1 << 24,  //formatted as 0.0f to 1.0f floats
		PickerHueBar = 1 << 25,
		PickerHueWheel = 1 << 26, 
		InputRGB = 1 << 27,  //input and output data in RGB format
		InputHSV = 1 << 28,  //input and output data in HSV format

		OptionsDefault = Uint8 | DisplayRGB | InputRGB | PickerHueBar,

		//internal masks
		DisplayMask = DisplayRGB | DisplayHSV | DisplayHex,
		DataTypeMask = Uint8 | Float,
		PickerMask = PickerHueWheel | PickerHueBar,
		InputMask = InputRGB | InputHSV
	};


	struct UIStyle
	{
		float       alpha;                      // Global alpha applies to everything.
		Vector2     windowPadding;              // Padding within a window.
		float       windowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
		float       windowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		Vector2     windowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
		Vector2     windowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
		Direction   windowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to Left.
		float       childRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
		float       childBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		float       popupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
		float       popupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		Vector2      framePadding;               // Padding within a framed rectangle (used by most widgets).
		float       frameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
		float       frameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		Vector2      itemSpacing;                // Horizontal and vertical spacing between widgets/lines.
		Vector2      itemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
		Vector2      touchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
		float       indentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
		float       columnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
		float       scrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
		float       scrollbarRounding;          // Radius of grab corners for scrollbar.
		float       grabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
		float       grabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
		float       tabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
		float       tabBorderSize;              // Thickness of border around tabs.
		float       tabMinWidthForUnselectedCloseButton; // Minimum width for close button to appears on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
		Direction    colorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to Right.
		Vector2      buttonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
		Vector2      selectableTextAlign;        // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
		Vector2      displayWindowPadding;       // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
		Vector2      displaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
		float       mouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). We apply per-monitor DPI scaling over this scale. May be removed later.
		bool        antiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
		bool        antiAliasedFill;            // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
		float       curveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
		float       circleSegmentMaxError;      // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.
		Vector4      colors[(int) UIStyleColor::Count];

		UIStyle();
		void ScaleAllSizes(float scale_factor);
	};

	enum class InputSource
	{
		None = 0,
		Mouse,
		Nav,
		NavKeyboard,
		NavGamepad, 
		Count
	};

	struct UIColorMod
	{
		UIStyleColor color;
		Vector4 backupVal;
	};

	enum UIStyleVar
	{
		// Enum name ------- // Member in structure 
		Alpha,               // float     Alpha
		WindowPadding,       // Vector2   WindowPadding
		WindowRounding,      // float     WindowRounding
		WindowBorderSize,    // float     WindowBorderSize
		WindowMinSize,       // Vector2   WindowMinSize
		WindowTitleAlign,    // Vector2   WindowTitleAlign
		ChildRounding,       // float     ChildRounding
		ChildBorderSize,     // float     ChildBorderSize
		PopupRounding,       // float     PopupRounding
		PopupBorderSize,     // float     PopupBorderSize
		FramePadding,        // Vector2   FramePadding
		FrameRounding,       // float     FrameRounding
		FrameBorderSize,     // float     FrameBorderSize
		ItemSpacing,         // Vector2   ItemSpacing
		ItemInnerSpacing,    // Vector2   ItemInnerSpacing
		IndentSpacing,       // float     IndentSpacing
		ScrollbarSize,       // float     ScrollbarSize
		ScrollbarRounding,   // float     ScrollbarRounding
		GrabMinSize,         // float     GrabMinSize
		GrabRounding,        // float     GrabRounding
		TabRounding,         // float     TabRounding
		ButtonTextAlign,     // Vector2   ButtonTextAlign
		SelectableTextAlign, // Vector2   SelectableTextAlign
		Count
	};

	struct UIStyleMod
	{
		UIStyleVar   varIdx;
		union { int backupInt[2]; float backupFloat[2]; };

		UIStyleMod(UIStyleVar idx, int v) { varIdx = idx; backupInt[0] = v; }
		UIStyleMod(UIStyleVar idx, float v) { varIdx = idx; backupFloat[0] = v; }
		UIStyleMod(UIStyleVar idx, Vector2 v) { varIdx = idx; backupFloat[0] = v.x; backupFloat[1] = v.y; }
	};

	// Storage for current popup stack
	struct UIPopupData
	{
		//fields are set in another callback when the popup is created
		unsigned int popupId;        
		AReference<AWindow> window;      
		AReference<AWindow> sourceWindow;
		int openFrameCount; 
		unsigned int openParentId; 
		Vector2 openPopupPos;   
		Vector2 openMousePos;   

		UIPopupData() : popupId(0), window(nullptr), sourceWindow(nullptr), openFrameCount(-1), openParentId(0) { }
	};

	//helper to keep track of rectangles in the UI display
	struct UIRectangle
	{
		//uses screen coords
		Vector2 minVec;    // Upper-left
		Vector2 maxVec;    // Lower-right

		UIRectangle() : minVec(0.0f, 0.0f), maxVec(0.0f, 0.0f) { }
		UIRectangle(const Vector2& min, const Vector2& max) : minVec(min), maxVec(max) { }
		UIRectangle(const Vector4& v) : minVec(v.x, v.y), maxVec(v.z, v.w) { }
		UIRectangle(float x1, float y1, float x2, float y2) : minVec(x1, y1), maxVec(x2, y2) { }

		Vector2 GetCenter() const { return Vector2((minVec.x + maxVec.x) * 0.5f, (minVec.y + maxVec.y) * 0.5f); }
		
		Vector2 GetSize() const { return Vector2(maxVec.x - minVec.x, maxVec.y - minVec.y); }
		
		float GetWidth() const { return maxVec.x - minVec.x; }
		
		float GetHeight() const { return maxVec.y - minVec.y; }
		
		Vector2 GetTopLeftVertex() const { return minVec; }
		
		Vector2 GetTopRVertex() const { return Vector2(maxVec.x, minVec.y); }
		
		Vector2 GetBottomLeftVertex() const { return Vector2(minVec.x, maxVec.y); }
		
		Vector2 GetBottomRightVertex() const { return maxVec; }

		bool Contains(const Vector2& p) const 
		{ 
			return p.x >= minVec.x && p.x < maxVec.x 
				&& p.y >= minVec.y && p.y < maxVec.y; 
		}
		
		bool Contains(const UIRectangle& r) const 
		{ 
			return r.minVec.x >= minVec.x && r.maxVec.x <= maxVec.x 
				&& r.minVec.y >= minVec.y && r.maxVec.y <= maxVec.y; 
		}
		
		//if r1 contains r2, r1 & r2 do not overlap, if r1 overlaps r2, r2 does not overlap r1
		bool Overlaps(const UIRectangle& r) const 
		{ 
			return r.minVec.y <  maxVec.y && r.maxVec.y >  maxVec.y 
				&& r.maxVec.x <  maxVec.x && r.maxVec.x >  minVec.x; 
		}
		
		//increases size of rectangle so that the diagonal formed by two opposing corners of the rectangle 
		//is at least the length of the provided vector
		void Add(const Vector2& p) 
		{ 
			if (minVec.x > p.x)
			{
				minVec.x = p.x;
			}     
			
			if (minVec.y > p.y)
			{
				minVec.y = p.y;
			}
			
			if (maxVec.x < p.x)
			{
				maxVec.x = p.x;
			}
			
			if (maxVec.y < p.y)
			{
				maxVec.y = p.y;
			}
		}
		
		//increases size of rectangle so that the diagonal formed by two opposing corners of the rectangle 
		//is at least the length of the same diagonal for the provided rectangle
		void Add(const UIRectangle& r) 
		{ 
			if (minVec.x > r.minVec.x)
			{
				minVec.x = r.minVec.x;
			}
			
			if (minVec.y > r.minVec.y)
			{
				minVec.y = r.minVec.y;
			}
			
			if (maxVec.x < r.maxVec.x)
			{
				maxVec.x = r.maxVec.x;
			}
			
			if (maxVec.y < r.maxVec.y)
			{
				maxVec.y = r.maxVec.y;
			}
		}
		
		//expands each vertices of the rectangle by the amount provided
		void Expand(const float amount) 
		{ 
			minVec.x -= amount; 
			minVec.y -= amount;  
			maxVec.x += amount;   
			maxVec.y += amount; 
		}
		
		//expands each vertices of the rectangle by the amount of the x and y direction of the 
		//vector provided by their respective dimension
		void Expand(const Vector2& amount) 
		{ 
			minVec.x -= amount.x; 
			minVec.y -= amount.y; 
			maxVec.x += amount.x; 
			maxVec.y += amount.y; 
		}

		void Translate(const Vector2& d) 
		{ 
			minVec.x += d.x; 
			minVec.y += d.y; 
			maxVec.x += d.x; 
			maxVec.y += d.y; 
		}
		
		void TranslateX(float dx) 
		{ 
			minVec.x += dx; 
			maxVec.x += dx;
		}

		void TranslateY(float dy) 
		{ 
			minVec.y += dy; 
			maxVec.y += dy; 
		}

		//may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
		void ClipWith(const UIRectangle& r) 
		{ 
			minVec = Vector2::Max(minVec, r.minVec);
			maxVec = Vector2::Min(maxVec, r.maxVec); 
		}                   
		
		//ensure both points are fully clipped with no inversion.
		void ClipWithFull(const UIRectangle& r) 
		{ 
			minVec = Vector2::Clamp(minVec, r.minVec, r.maxVec); 
			maxVec = Vector2::Clamp(maxVec, r.minVec, r.maxVec);
		} 

		void Floor() 
		{ 
			minVec.x = Math::Floor(minVec.x); 
			minVec.y = Math::Floor(minVec.y); 
			maxVec.x = Math::Floor(maxVec.x); 
			maxVec.y = Math::Floor(maxVec.y); 
		}

		bool IsInverted() const 
		{ 
			return minVec.x > maxVec.x || minVec.y > maxVec.y; 
		}

		Vector4 ToVec4() const 
		{ 
			return Vector4(minVec.x, minVec.y, maxVec.x, maxVec.y); 
		}
	};

	//for internal use only, the fields declared here are only for the engine's use
	struct ViewportPrivate : public Viewport
	{
		int index;
		int lastFrameActive;
		int lastFrontMostStampCount;
		unsigned int lastNameHash;
		Vector2 lastPos;
		float alpha; // Window opacity (when dragging dockable windows/viewports we make them transparent)
		float lastAlpha;
		short platformMonitor;
		bool platformWindowCreated;
		AReference<AWindow> window; // Set when the viewport is owned by a window 
		Vector2 lastPlatformPos;
		Vector2 lastPlatformSize;
		Vector2 lastRendererSize;
		Vector2 currWorkOffsetMin; // Work area top-left offset being increased during the frame
		Vector2 currWorkOffsetMax; // Work area bottom-right offset being decreased during the frame

		ViewportPrivate() : index(-1), lastFrameActive(-1), lastFrontMostStampCount(-1), 
			lastNameHash(0), alpha(1.0f), lastAlpha(1.0f), platformMonitor(-1), 
			platformWindowCreated(false), window(nullptr), lastPlatformPos(FLT_MAX, FLT_MAX), 
			lastPlatformSize(FLT_MAX, FLT_MAX), lastRendererSize(FLT_MAX, FLT_MAX) { }

		UIRectangle GetMainRect() const { return UIRectangle(m_pos.x, m_pos.y, m_pos.x + m_size.x, m_pos.y + m_size.y); }
		UIRectangle GetWorkRect() const { return UIRectangle(m_pos.x + m_workOffsetMin.x, m_pos.y + m_workOffsetMin.y, m_pos.x + m_size.x + m_workOffsetMax.x, m_pos.y + m_size.y + m_workOffsetMax.y); }
	};

	enum class NavLayer
	{
		Main = 0,    // Main scrolling layer
		Menu = 1,    // Menu layer (access with Alt/Menu)
		Count
	};

	//used to navigate sibling menus through parent from a child menu
	enum NavMoveFlags
	{
		None = 0,
		LoopX = 1 << 0,   //on failed request, restart from opposite side
		LoopY = 1 << 1,
		
		//on failed request, request from opposite side one line 
		//down (when NavDir==right) or one line up (when NavDir==left)
		WrapX = 1 << 2,   
		WrapY = 1 << 3,   //provided for completeness (not usually used)
		AllowCurrentNavId = 1 << 4,   //allow scoring and considering the current NavId as a move target candidate. 
		
		//store alternate result in NavMoveResultLocalVisibleSet that 
		//only comprise elements that are already fully visible.
		AlsoScoreVisibleSet = 1 << 5,   
		ScrollToEdge = 1 << 6
	};

	enum class NavForward
	{
		None,
		ForwardQueued,
		ForwardActive
	};

	enum class  UIDirection
	{
		None = -1,
		Left = 0,
		Right = 1,
		Up = 2,
		Down = 3,
		Count
	};

	struct NavMoveResult
	{
		AReference<AWindow> window; //best candidate window
		unsigned int id; //best candidate ID
		unsigned int focusScopeId; //best candidate focus scope ID
		float distBox; //best candidate box distance to current NavId
		float distCenter; //best candidate center distance to current NavId
		float distAxial;
		UIRectangle rectRel; //best candidate bounding box in window relative space

		NavMoveResult() { Clear(); }

		void Clear() 
		{ 
			window = nullptr; 
			id = 0;
			focusScopeId = 0;
			distBox = FLT_MAX; 
			distCenter = FLT_MAX;
			distAxial = FLT_MAX;
			rectRel = UIRectangle(); 
		}
	};

	enum DragDropFlags
	{
		None = 0,
		// BeginDragDropSource() flags ///////////////////////////////
		SourceNoPreviewTooltip = 1 << 0,   
		SourceNoDisableHover = 1 << 1,

		//disable the opening of tree nodes and collapsing of header by 
		//holding over them while dragging a source item.
		SourceNoHoldToOpenOthers = 1 << 2,
		
		//allow items such as text & images with no unique identifier to be used as drag source, unusual so made explicit.
		SourceAllowNullID = 1 << 3,   

		//external source, won't attempt to read current item/window info. 
		//Will always return true. Only one Extern source can be active simultaneously.
		SourceExtern = 1 << 4,   
		//automatically expire the payload if the source cease to be submitted (persisting while dragged otherwise)
		SourceAutoExpirePayload = 1 << 5,   
		
		// AcceptDragDropPayload() flags//////////////////////////////////////////

		//AcceptDragDropPayload() will returns true even before the mouse button is released
		//so we can call IsDelivery() to test if the payload needs to be delivered
		AcceptBeforeDelivery = 1 << 10,  

		//don't draw the default highlight rectangle when hovering over target
		AcceptNoDrawDefaultRect = 1 << 11,

		//request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site
		AcceptNoPreviewTooltip = 1 << 12,  
		
		// For peeking ahead and inspecting the payload before delivery
		AcceptPeekOnly = AcceptBeforeDelivery | AcceptNoDrawDefaultRect  
	};

	//internal helper for drag and drop support
	struct DragDropPayload
	{
		void* data;
		int dataSize;

		unsigned int sourceId;          
		unsigned int sourceParentId; 
		int dataFrameCount; //used for timestamp?
		char dataType[32 + 1]; //data type tag (short user-supplied string, 32 characters max)
		bool preview; //set when item is hovered but the mouse button has not been released
		bool delivery; //set when mouse button is released over item

		DragDropPayload() { Clear(); }
		void Clear()
		{
			sourceId = 0;
			sourceParentId = 0; 
			data = nullptr; 
			dataSize = 0; 
			memset(dataType, 0, sizeof(dataType)); 
			dataFrameCount = -1; 
			preview = false;
			delivery = false;
		}
		
		bool IsDataType(const char* type) const 
		{ 
			return dataFrameCount != -1 && strcmp(type, dataType) == 0; 
		}
		
		bool IsPreview() const { return preview; }
		
		bool IsDelivery() const { return delivery; }
	};

	enum TabItemFlags
	{
		None = 0,
		UnsavedDocument = 1 << 0, //append '*' to title without affecting the ID
		SetSelected = 1 << 1, //set curr tab as selected through code when calling BeginTabItem()
		NoCloseWithMiddleMouseButton = 1 << 2, //disable behavior of closing tabs with middle mouse button
		NoPushId = 1 << 3, //don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
		NoTooltip = 1 << 4 
	};

	struct TabItem
	{
		unsigned int id;
		TabItemFlags flags;
		AReference<AWindow> window;  //when TabItem is part of a DockNode's TabBar, we hold on to a window.
		int lastFrameVisible;
		int lastFrameSelected; //this allows us to infer an ordered list of the last activated tabs with little maintenance
		int nameOffset;
		float offset;
		float width;
		float contentWidth;

		TabItem() : id(0), flags(TabItemFlags::None), lastFrameVisible(-1), 
			lastFrameSelected(-1), nameOffset(-1), offset(0.0f), width(0.0f), contentWidth(0.0f) { }
	
	};

	enum TabBarFlags
	{
		None = 0,
		Reorderable = 1 << 0, //allow manually dragging tabs to re-order them + New tabs are appended at the end of list
		AutoSelectNewTabs = 1 << 1, //automatically select new tabs when they appear
		TabListPopupButton = 1 << 2, //disable buttons to open the tab list popup
		NoCloseWithMiddleMouseButton = 1 << 3,
		NoTabListScrollingButtons = 1 << 4,
		NoTooltip = 1 << 5,
		FittingPolicyResizeDown = 1 << 6, //resize tabs when they don't fit
		FittingPolicyScroll = 1 << 7, //add scroll buttons when tabs don't fit
		FittingPolicyMask = FittingPolicyResizeDown | FittingPolicyScroll,
		FittingPolicyDefault = FittingPolicyResizeDown
	};

	struct StringBuilder
	{
		//buffer is null terminated
		ADynArr<char> buffer;
		static char EmptyString[1]; //stores the null char

		StringBuilder() { }
		
		inline char operator[](int i) const 
		{ 
			AE_CORE_ASSERT(buffer.GetData() != nullptr, ""); 
			return buffer.GetData()[i]; 
		}
		
		const char* begin() const 
		{ 
			if(!buffer.IsEmpty())
			{
				return &buffer.GetData()[0];
			}

			return EmptyString; 
		}
		
		const char* end() const 
		{ 
			if (!buffer.IsEmpty())
			{
				return &buffer.GetData()[buffer.GetCount() - 1];
			}

			return EmptyString;
		}

		size_t GetCount() const 
		{
			if (buffer.IsEmpty())
			{
				return 0;
			}
			return buffer.GetCount() - 1;
		}

		bool IsEmpty() const 
		{ 
			return buffer.GetCount() <= 1; 
		}
		void Clear() 
		{ 
			buffer.Clear(); 
		}
		
		void Reserve(size_t count) 
		{ 
			buffer.Reserve(count); 
		}
		
		const char* c_str() const 
		{ 
			if (IsEmpty())
			{
				return EmptyString;
			}
			return buffer.GetData();
		}

		void Append(const char* str, const char* str_end = nullptr);
		void Appendf(const char* fmt, ...);
		void Appendfv(const char* fmt, va_list args);
	};

	// Storage for a tab bar (sizeof() 92~96 bytes)
	struct TabBar
	{
		ADynArr<TabItem> tabs;
		unsigned int id;                     // Zero for tab-bars used by docking
		unsigned int selectedTabId;          // Selected tab/window
		unsigned int nextSelectedTabId;
		unsigned int visibleTabId;           // Can occasionally be != SelectedTabId (e.g. when previewing contents for CTRL+TAB preview)
		int currFrameVisible;
		int prevFrameVisible;
		UIRectangle barRect;
		float lastTabContentHeight;   // Record the height of contents submitted below the tab bar
		float offsetMax;              // Distance from BarRect.Min.x, locked during layout
		float offsetMaxIdeal;         // Ideal offset if all tabs were visible and not clipped
		float offsetNextTab;          // Distance from BarRect.Min.x, incremented with each BeginTabItem() call, not used if ImGuiTabBarFlags_Reorderable if set.
		float scrollingAnim;
		float scrollingTarget;
		float scrollingTargetDistToVisibility;
		float scrollingSpeed;
		TabBarFlags flags;
		unsigned int reorderRequestTabId;
		char reorderRequestDir;
		bool wantLayout;
		bool visibleTabWasSubmitted;
		short lastTabItemIdx;         // For BeginTabItem()/EndTabItem()
		Vector2 framePadding;           // style.FramePadding locked at the time of BeginTabBar()
		StringBuilder tabsNames;              // For non-docking tab bar we re-append names in a contiguous buffer.

		TabBar();

		int GetTabOrder(const TabItem* tab) const 
		{ 
			for (int i = 0; i < tabs.GetCount(); i++)
			{
				if (tab == &tabs[i])
				{
					return i;
				}
			}
			return -1;
		}
		
		const char* GetTabName(const TabItem* tab) const
		{
			if (tab->window != nullptr)
			{
				return tab->window->GetTitle().c_str();
			}

			AE_CORE_ASSERT(tab->nameOffset != -1 && tab->nameOffset < tabsNames.buffer.GetCount(), "");
			return tabsNames.buffer.GetData() + tab->nameOffset;
		}
	};

	union StorageData
	{
		int num;
		bool boolean;
		float floatNum;
		void* ptr;
	};

	template<typename T>
	struct UIPool
	{
		ADynArr<T> buffer; //stores the elements
		AUnorderedMap<unsigned int, StorageData> map; //stores the indices of the element assigned to a key
		int freeIndex;

		UIPool() : freeIndex(0) { }
		~UIPool() { Clear(); }

		T* GetByKey(unsigned int key)
		{
			if (map.ContainsKey(key))
			{
				return &buffer[map[key]];
			}
			return nullptr;
		}

		T* GetByIndex(size_t index)
		{
			return &buffer[index];
		}

		int GetIndex(const T* ptr) const
		{
			AE_CORE_ASSERT(Contains(ptr), "ptr provided not contained in the pool");

			return (int)((ptr - buffer.GetData()) / sizeof(T));
		}

		T* GetOrAddByKey(unsigned int key) 
		{ 
			if (map.ContainsKey(key))
			{
				return &buffer[map[key]];
			}
			return Add(); 
		}

		bool Contains(const T* ptr) const
		{
			return ptr >= buffer.GetData() && ptr < buffer.GetData() + (buffer.GetCount() * sizeof(T);
		}

		void Clear() 
		{
			for (auto& pair : map)
			{
				delete &buffer[pair.GetElement()];
			}

			map.Clear(); 
			buffer.Clear(); 
			freeIndex = 0;
		}

		T* Add()
		{
			int i = freeIndex; 
			if (i == buffer.GetCount()) 
			{ 
				buffer.Resize(buffer.GetCount() + 1); 
				freeIndex++;
			}
			else 
			{ 
				freeIndex = *(int*)&buffer[i];
			} 

			&buffer[i] = new T(); 
			return &buffer[i];
		}

		void Remove(unsigned int key, const T* ptr)
		{
			Remove(key, GetIndex(ptr));
		}

		void Remove(unsigned int key, int index)
		{
			delete& buffer[index];
			*(int*)&buffer[index] = freeIndex; 
			freeIndex = index; 

			if (map.ContainsKey(key))
			{
				map[key] = -1;
			}
			else
			{
				map.Add(key, -1);
			}
		}

		void Reserve(size_t count)
		{
			buffer.Reserve(count);
		}

		size_t GetCount() const
		{
			return buffer.GetCount();
		}
	};

	struct ShrinkWidthItem
	{
		int index;
		float width;
	};

	class UndoRecord
	{
	private:
		int m_where;
		int m_insertLength;
		int m_deleteLength;
		int charStorage;
	};


	class UndoState
	{
		friend struct InputTextState;
	private:
		UndoRecord m_undoRec[TEXT_EDIT_UNDOSTATECOUNT];
		unsigned short  m_undoChar[TEXT_EDIT_UNDOCHARCOUNT];
		short m_undoPoint, m_redoPoint;
		int m_undoCharPoint, m_redoCharPoint;
	};


	struct TextEditState
	{
		friend struct InputTextState;
		int cursor; //pos of cursor in the string

		//highlight start and end point in characters. if equal, no selection.
		//note that start may be less than or greater than end if clicked then drag left
		int selectStart;
		int selectEnd;

		//each textfield keeps its own insert mode state
		unsigned char insertMode;

	private:
		unsigned char m_cursorAtEndOfLine; // not implemented yet
		unsigned char m_initialized;
		unsigned char m_hasPreferredX;
		unsigned char m_singleLine;
		unsigned char m_padding1, m_padding2, m_padding3;
		float m_preferredX; // this determines where the cursor up/down tries to seek to along x
		UndoState m_undoState;

	};

	enum InputTextFlags
	{
		None = 0,
		CharsDecimal = 1 << 0,   
		CharsHexadecimal = 1 << 1, 
		CharsUppercase = 1 << 2,   //like caps lock
		CharsNoBlank = 1 << 3,   //filter out spaces, tabs
		AutoSelectAll = 1 << 4,   //select entire text when first taking mouse focus
		
		//return 'true' when Enter is pressed (otherwise we return true every time the value was modified)
		EnterReturnsTrue = 1 << 5,   
		CallbackCompletion = 1 << 6,   //callback on pressing TAB (for completion handling)
		CallbackHistory = 1 << 7,   //callback on pressing Up/Down arrows (for history handling)
		CallbackAlways = 1 << 8,   //callback on each iteration. User code may query cursor position, modify text buffer.
		CallbackCharFilter = 1 << 9,   //callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
		AllowTabInput = 1 << 10,  
		
		//in multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite)
		CtrlEnterForNewLine = 1 << 11, 
		NoHorizontalScroll = 1 << 12,  
		AlwaysInsertMode = 1 << 13,  
		ReadOnly = 1 << 14,  
		Password = 1 << 15,  //password mode, displays all characters as "*"
		NoUndoRedo = 1 << 16,  
		CharsScientific = 1 << 17,  //allow scientific notation input
		CallbackResize = 1 << 18,  //use callback when the string wants to be resized
		
		//internal
		Multiline = 1 << 20, 
		NoMarkEdited = 1 << 21   
	};

	//used by callbacks to gather data
	struct InputTextCallbackData
	{
		InputTextFlags eventFlag; 
		InputTextFlags flags; //what user passed to InputText(), read-only
		void* userData; //what user passed to InputText(), read-only

		unsigned short eventChar;
		KeyCode eventKey; 
		char* buffer;  
		int bufferTextLength;
		int bufferSize; //buffer size in bytes = capacity + 1
		bool bufferDirty; //set if you modify buffer or bufferTextLength
		int cursorPos;      
		int selectionStart; 
		int selectionEnd;   

		InputTextCallbackData();
		void DeleteChars(int pos, int bytesCount);
		void InsertChars(int pos, const char* text, const char* textEnd = nullptr);
		bool HasSelection() const { return selectionStart != selectionEnd; }
	};


	struct InputTextState
	{
		using InputTextCallback = ADelegate<int(InputTextCallbackData*)>;

		unsigned int id;
		int currLengthW, currLengthA; //buffer length for both UTF-8 char and wchar (needed?)
		ADynArr<unsigned short> textW; //main buffer owned by the object
		ADynArr<char> textA; //temp UTF-8 buffer;
		ADynArr<char> initialTextA;
		bool textAIsValid;
		int bufferCapacityA; //needed?
		float scrollX;
		TextEditState state;
		float cursorAnim; //timer for cursor blink
		bool followCursorScroll; //true when we scroll following the cursor
		bool selectedAllMouseLock; //when user double clicks to select all we ignore the mouse drag to update selection
		InputTextFlags userFlags;
		InputTextCallback userCallback;
		void* userCallbackData;

		InputTextState() 
		{ 
			memset(this, 0, sizeof(*this)); 
		}
		void ClearText() 
		{ 
			currLengthW = 0;
			currLengthA = 0; 
			textW[0] = 0; 
			textA[0] = 0; 
			CursorClamp(); 
		}

		void ClearFreeMemory() 
		{ 
			textW.Clear(); 
			textW.ShrinkToFit(); 
			textA.Clear(); 
			textA.ShrinkToFit(); 
			initialTextA.Clear(); 
			initialTextA.ShrinkToFit(); 
		}

		int GetUndoAvailCount() const
		{
			return state.m_undoState.m_undoPoint;
		}

		int GetRedoAvailCount() const 
		{ 
			return TEXT_EDIT_UNDOSTATECOUNT - state.m_undoState.m_redoPoint; 
		}

		void OnKeyPressed(int key);

		void CursorAnimReset() { cursorAnim = -0.30f; }                                   // After a user-input the cursor stays on for a while without blinking
		
		void CursorClamp() 
		{ 
			state.cursor = Math::Min(state.cursor, currLengthW); 
			state.selectStart = Math::Min(state.selectStart, currLengthW); 
			state.selectEnd = Math::Min(state.selectEnd, currLengthW); 
		}
		
		bool HasSelection() const { return state.selectStart != state.selectEnd; }
		
		void ClearSelection() 
		{ 
			state.selectStart = state.cursor;
			state.selectEnd = state.cursor;
		}
		
		void SelectAll() 
		{ 
			state.selectStart = 0; 
			state.cursor = currLengthW;
			state.selectEnd = currLengthW;
			state.m_hasPreferredX = 0; 
		}
	};

	enum class DockRequestType
	{
		None = 0,
		Dock,
		Undock,
		Split//same as Dock but no payload
	};

	enum DockNodeFlags
	{
		None = 0,
		KeepAliveOnly = 1 << 0,
		NoDockingInCentralNode = 1 << 2,
		PassthruCentralNode = 1 << 3,   
		NoSplit = 1 << 4, 
		NoResize = 1 << 5,
		AutoHideTabBar = 1 << 6
	};

	// X/Y enums are fixed to 0/1 so they may be used as index
	enum class Axis
	{
		None = -1,
		X = 0,
		Y = 1
	};

	enum class DockNodeState
	{
		Unknown,
		HostWindowHiddenBecauseSingleWindow,
		HostWindowHiddenBecauseWindowsAreResizing,
		HostWindowVisible
	};

	//indicates the authority source (dock node vs window) of a field
	enum class DockDataAuthority
	{
		Auto,
		DockNode,
		Window
	};

	struct DockNode
	{
		unsigned int id;
		DockNodeFlags sharedFlags; 
		DockNodeFlags localFlags; 
		DockNode* parentNode;
		DockNode* childNodes[2];
		ADynArr<AReference<AWindow>>  windows;
		TabBar* tabBar;
		Vector2 pos;   
		Vector2 size;  
		Vector2 sizeRef;
		Axis splitAxis; 

		DockNodeState state;
		AReference<AWindow> hostWindow;
		AReference<AWindow> visibleWindow;
		DockNode* centralNode;
		DockNode* onlyNodeWithWindows;
		int lastFrameAlive;  
		int lastFrameActive; 
		int lastFrameFocused;
		unsigned int lastFocusedNodeID;
		unsigned int selectedTabID;    
		unsigned int wantCloseTabID;   
		DockDataAuthority authorityForPos = DockDataAuthority::Window;
		DockDataAuthority authorityForSize = DockDataAuthority::Window;
		DockDataAuthority authorityForViewport = DockDataAuthority::Window;
		bool isVisible = true; 
		bool isFocused = true;
		bool hasCloseButton = true;
		bool hasWindowMenuButton = true;
		bool enableCloseButton = true;
		bool wantCloseAll = true; //set when closing all tabs at once.
		bool wantLockSizeOnce = true;
		bool wantMouseMove = true; 
		bool wantHiddenTabBarUpdate = true;
		bool wantHiddenTabBarToggle = true;
		bool markedForPosSizeWrite = true; 

		DockNode(unsigned int id);
		~DockNode();
		bool IsRootNode() const { return parentNode == nullptr; }
		bool IsDockSpace() const { return (localFlags & DockNodePrivateFlags::DockSpace) != 0; }
		bool IsFloatingNode() const { return parentNode == nullptr && (localFlags & DockNodePrivateFlags::DockSpace) == 0; }
		bool IsCentralNode() const { return (localFlags & DockNodePrivateFlags::CentralNode) != 0; }
		bool IsHiddenTabBar() const { return (localFlags & DockNodePrivateFlags::HiddenTabBar) != 0; } 
		bool IsNoTabBar() const { return (localFlags & DockNodePrivateFlags::NoTabBar) != 0; }
		bool IsSplitNode() const { return childNodes[0] != nullptr; }
		bool IsLeafNode() const { return childNodes[0] == nullptr; }
		bool IsEmpty() const { return childNodes[0] == nullptr && windows.GetCount() == 0; }
		DockNodeFlags GetMergedFlags() const { return (DockNodeFlags)(sharedFlags | localFlags); }
		UIRectangle GetRect() const { return UIRectangle(pos.x, pos.y, pos.x + size.x, pos.y + size.y); }

	private:
		//extends DockNodeFlags
		enum DockNodePrivateFlags
		{
			DockSpace = 1 << 10,
			CentralNode = 1 << 11,
			NoTabBar = 1 << 12,
			HiddenTabBar = 1 << 13,
			NoWindowMenuButton = 1 << 14,
			NoCloseButton = 1 << 15,
			NoDocking = 1 << 16,
			NoDockingSplitMe = 1 << 17,
			NoDockingSplitOther = 1 << 18,  //prevent this node from splitting another window/node.
			NoDockingOverMe = 1 << 19,  //prevent another window/node to be docked over this node.
			NoDockingOverOther = 1 << 20,  //prevent this node to be docked over another window/node.
			NoResizeX = 1 << 21,
			NoResizeY = 1 << 22,
			SharedFlagsInheritMask_ = ~0,
			NoResizeFlagsMask = NoResize | NoResizeX | NoResizeY,
			LocalFlagsMask = NoSplit | NoResizeFlagsMask | AutoHideTabBar | DockSpace 
				| CentralNode | NoTabBar | HiddenTabBar | NoWindowMenuButton | NoCloseButton | NoDocking,
			
			//~ is bitwise !
			LocalFlagsTransferMask = LocalFlagsMask & ~DockSpace,
			SavedFlagsMask = NoResizeFlagsMask | DockSpace | CentralNode | NoTabBar 
				| HiddenTabBar | NoWindowMenuButton | NoCloseButton | NoDocking
		};

	};


	struct DockRequest
	{
		DockRequestType    type;
		AReference<AWindow> dockTargetWindow;   // Destination/Target Window to dock into (may be a loose window or a DockNode, might be NULL in which case DockTargetNode cannot be NULL)
		DockNode* dockTargetNode;     // Destination/Target Node to dock into
		AReference<AWindow> dockPayload;        // Source/Payload window to dock (may be a loose window or a DockNode), [Optional]
		UIDirection dockSplitDir;
		float dockSplitRatio;
		bool dockSplitOuter;
		AReference<AWindow> undockTargetWindow;
		DockNode* undockTargetNode;

		DockRequest() : type(DockRequestType::None), dockTargetWindow(nullptr), dockPayload(nullptr), 
			undockTargetWindow(nullptr), dockTargetNode(nullptr), undockTargetNode(nullptr), 
			dockSplitDir(UIDirection::None), dockSplitRatio(0.5f), dockSplitOuter(false) { }
		
	};

	//persistent settings data stored in SettingsNodes
	struct DockNodeSettings
	{
		unsigned int id;
		unsigned int parentNodeID;
		unsigned int parentWindowID;
		unsigned int selectedWindowID;
		signed char splitAxis; //use signed char since default char can be signed or unsigned depending on compiler
		char depth;
		DockNodeFlags  flags; 
		Vector2Short pos;
		Vector2Short size;
		Vector2Short sizeRef;

		DockNodeSettings() : id(0), parentNodeID(0), parentWindowID(0), 
			selectedWindowID(0), splitAxis((signed char)Axis::None), 
			depth(0), flags(DockNodeFlags::None) { }
	};

	struct DockContext
	{
		AUnorderedMap<unsigned int, StorageData> nodes;
		ADynArr<DockRequest> requests;
		ADynArr<DockNodeSettings> nodesSettings; 
		bool wantFullRebuild;
		DockContext() : wantFullRebuild(false) { }
	};

	struct SettingsHandler
	{
		const char* typeName;
		unsigned int typeHash;
		void (*clearAllFn)(UIContext* context, SettingsHandler* handler); 
		void (*readInitFn)(UIContext* context, SettingsHandler* handler);
		void* (*readOpenFn)(UIContext* context, SettingsHandler* handler, const char* name); 
		void (*readLineFn)(UIContext* context, SettingsHandler* handler, void* entry, const char* line);
		void (*applyAllFn)(UIContext* context, SettingsHandler* handler);
		void (*writeAllFn)(UIContext* context, SettingsHandler* handler, StringBuilder* outBuffer); 
		void* userData;

		SettingsHandler() { memset(this, 0, sizeof(*this)); }
	};

	struct UIContext
	{
	private:
		union IntPtr
		{
			int i;
			void* ptr;
		};

	public:

		bool initialized;
		bool fontAtlasOwnedByContext;
		UIIO io;

		Viewport* mainViewport;
		ADynArr<Viewport*> viewports;
		UIStyle style;
		UIConfigFlags configFlagsCurrFrame;
		UIConfigFlags configFlagsLastFrame;
		Font* font;
		float fontSize;
		float fontBaseSize;
		
		ADynArr<AReference<AWindow>> windows;
		ADynArr<AReference<AWindow>> windowFocusOrder;
		ADynArr<AReference<AWindow>> windowTempSortBuffer; //used when reordering windows to keep parents before children
		ADynArr<AReference<AWindow>> currWindowStack;
		AUnorderedMap<unsigned int, StorageData> windowsByID;
		int windowsActiveCount;

		//window state
		AReference<AWindow> currentWindow;
		AReference<AWindow> hoveredWindow;
		AReference<AWindow> hoveredWindowUnderMovingWindow;
		AReference<AWindow> movingWindow;
		AReference<AWindow> wheelingWindow;
		
		Vector2 wheelingWindowRefMousePos;
		float wheelingWindowTimer;

		//Widget tracking info
		unsigned int hoveredID; //id of hovered item
		bool hoveredIDAllowOverlap;
		unsigned int hoveredIDPrevFrame;
		float hoveredIDTimer;
		float hoveredIDNotActiveTimer; //timer for when item is not active
		unsigned int activeID;
		unsigned int activeIDIsAlive;
		float activeIDTimer;

		bool activeIDWasJustActivated;
		bool activeIDAllowOverlap;
		bool activeIDHasBeenPressedBefore;
		bool activeIDHasBeenEditedBefore;
		bool activeIDHasBeenEditedThisFrame;
		unsigned int activeIDUsingNavDirMask;
		unsigned int activeIDUsingNavInputMask;
		unsigned int activeIDUsingKeyInputMask;
		Vector2  activeIDClickOffset;
		AReference<AWindow> activeIDWindow;
		InputSource activeIDSource;
		int activeIDMouseButton;
		unsigned int activeIDPreviousFrame;
		bool activeIDPreviousFrameIsAlive;
		bool activeIDPreviousFrameHasBeenEditedBefore;
		AReference<AWindow> activeIDPreviousFrameWindow;
		unsigned int lastActiveID;
		float lastActiveTimer;

		//stacks
		ADynArr<UIColorMod> colorModifiers;
		ADynArr<UIStyleMod> styleModifiers;
		ADynArr<Font*> fontStack;
		ADynArr<UIPopupData> openPopupStack;
		ADynArr<UIPopupData> beginPopupStack;

		//viewports
		ADynArr<ViewportPrivate*> viewports;
		float currDPIScale;
		ViewportPrivate* currViewport;
		ViewportPrivate* mouseViewport;
		ViewportPrivate* mouseLastHoveredViewport;
		int viewportsFrontMostStampCount;

		//gamepad/keyboard navigation
		AReference<AWindow> navWindow;
		unsigned int navID;
		unsigned int navFocusedScopeID;
		unsigned int navActivateID;
		unsigned int navActivateDownID;
		unsigned int navActivatePressedID;
		unsigned int navInputID;
		unsigned int navJustTabbedID;
		unsigned int navJustMovedToID;
		unsigned int navJustMovedToFocusedScopeID;
		unsigned int navNextActivateID;
		InputSource navInputSource;
		UIRectangle navScoringRectangle;
		int navScoringCount;
		NavLayer navLayer;
		int navIDTabCounter;
		bool navIDIsAlive;
		bool navMousePosDirty; //needed?
		bool navDisableHighlight;
		bool navDisableMouseHover;
		bool navAnyRequest;
		bool navInitRequest;
		bool navInitRequestFromMove;
		unsigned int navInitResultID;
		UIRectangle navInitResultRectRelativeToParent;
		bool navMoveFromClampedRefRect; // if scroll so that navigate id isn't visible we reset navigation from visible items
		bool navMoveRequest;

		NavMoveFlags navMoveRequestFlags;
		NavForward navMoveRequestForward;
		UIDirection navMoveDir;
		UIDirection navMoveDirLast;
		UIDirection navMoveClipDir;
		NavMoveResult navMoveResultLocal;
		NavMoveResult navMoveResultLocalVisibleSet;
		NavMoveResult navMoveResultOther;
		AReference<AWindow> navWrapRequestWindow;
		NavMoveFlags navWrapRequestFlags;

		//navigation windowing
		AReference<AWindow> navWindowingTarget;
		AReference<AWindow> navWindowingTargetAnim;
		AReference<AWindow> navWindowingListWindow;
		float navWindowingTimer;
		float navWindowingHighlightAlpha;
		bool navWindowingToggleLayer;

		//drag and drop
		bool dragDropActive;
		bool dragDropWithinSource;
		bool dragDropWithinTarget;
		DragDropFlags dragDropSourceFlags;
		int dragDropSourceFrameCount; //needed?
		int dragDropMouseButton;
		DragDropPayload dragDropPayload;
		UIRectangle dragDropTargetRectangle;
		unsigned int dragDropTargetID;
		DragDropFlags dragDropAcceptFlags;
		float dragDropAcceptIDCurrRectSurface;
		unsigned int dragDropAcceptIDCurr;
		unsigned int dragDropAcceptIDPrev;
		int dragDropAcceptFrameCount;
		unsigned int dragDropHoldJustPressedID;
		ADynArr<unsigned char> dragDropPayloadHeapBuffer;
		unsigned char dragDropPayloadLocalBuffer[16];

		//tab bars
		TabBar* currTabBar;
		UIPool<TabBar> tabBars;
		ADynArr<IntPtr> currentTabBarStack;
		ADynArr<ShrinkWidthItem> shrinkWidthBuffer;

		//widget state
		Vector2 lastValidMousePos;
		InputTextState inputTextState;
		Font inputTextPasswordFont;
		unsigned int tempInputID;
		ColorEditFlags colorEditOptions;
		float colorEditLastHue;
		float colorEditLastSaturation;
		float colorEditLastColor[3];
		Vector4	colorPickerInitialRef;
		bool dragCurrAccumDirty;
		float dragCurrAccum;
		float dragSpeedDefaultRatio;
		float scrollBarClickDeltaToGrabCenter;
		int tooltipOverrideCount;
		ADynArr<char> clipboardHandlerData;
		ADynArr<unsigned int> menuIDsSubmittedThisFrame;

		//platform support (needed?)
		Vector2 platformImePos;
		Vector2 platformImeLastPos;
		ViewportPrivate* platformImePosViewport;


		DockContext dockingContext;

		//settings
		bool settingsLoaded;
		float settingsDirtyTimer;
		StringBuilder settingsInitialData;
		ADynArr<SettingsHandler> settingsHandler;
		not done

	};

	bool CreateContext(FontAtlas* atlas = nullptr)
	{

	}

}