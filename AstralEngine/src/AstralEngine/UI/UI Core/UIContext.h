#pragma once
#include "AstralEngine/Core/Core.h"
#include "AstralEngine/Core/Keycodes.h"
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/ADelegate.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"
#include "AstralEngine/Renderer/Texture.h"

#define UNICODE_CODEPOINT_MAX 0xFFFF
#define TEXT_EDIT_UNDOSTATECOUNT 99
#define TEXT_EDIT_UNDOCHARCOUNT 999

namespace AstralEngine
{

	//forward declarations

	class FontAtlas;
	class Font;

	struct TabBar;
	struct UIWindow;

	//stores data for a single draw command (1 cmd != 1 draw call)
	class DrawCmdData
	{
	public:
		DrawCmdData() : m_rotation(0.0f), m_tileFactor(1.0f), m_color(1, 1, 1, 1) { }

		DrawCmdData(const Vector2& position, const Vector2& size, const Vector4& color) 
			: m_position(position), m_rotation(0.0f), m_tileFactor(1.0f), m_color(color), m_size(size) { }
		DrawCmdData(const Vector3& position, const Vector2& size, const Vector4& color)
			: m_position(position), m_rotation(0.0f), m_tileFactor(1.0f), m_color(color), m_size(size) { }

		DrawCmdData(const Vector2& position, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 }) 
			: m_position(position), m_rotation(0.0f), m_tileFactor(tileFactor), m_color(tintColor),
			m_size(size), m_texture(texture) { }
		DrawCmdData(const Vector3& position, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
			: m_position(position), m_rotation(0.0f), m_tileFactor(tileFactor), m_color(tintColor),
			m_size(size), m_texture(texture) { }

		DrawCmdData(const Vector3& position, const Vector2& size, AReference<SubTexture2D> subTexture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
			: m_position(position), m_rotation(0.0f), m_tileFactor(tileFactor), m_color(tintColor),
			m_size(size), m_subTexture(subTexture) { }

		
		DrawCmdData(const Vector2& position, float rotation, const Vector2& size, const Vector4& color)
			: m_position(position), m_rotation(rotation), m_tileFactor(1.0f), m_color(color), m_size(size) { }
		DrawCmdData(const Vector3& position, float rotation, const Vector2& size, const Vector4& color)
			: m_position(position), m_rotation(rotation), m_tileFactor(1.0f), m_color(color), m_size(size) { }

	
		DrawCmdData(const Vector2& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
			: m_position(position), m_rotation(rotation), m_tileFactor(tileFactor), m_color(tintColor),
			m_size(size), m_texture(texture) { }
		DrawCmdData(const Vector3& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
			: m_position(position), m_rotation(rotation), m_tileFactor(tileFactor), m_color(tintColor),
			m_size(size), m_texture(texture) { }
		DrawCmdData(const Vector3& position, float rotation, const Vector2& size, AReference<SubTexture2D> subTexture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
			: m_position(position), m_rotation(rotation), m_tileFactor(tileFactor), m_color(tintColor),
			m_size(size), m_subTexture(subTexture) { }

		//calls the appropriate Draw Command from Renderer2D according to what data this draw command contains
		void Draw() const;

		bool operator==(const DrawCmdData& other) const
		{
			return m_position == other.m_position && m_rotation == other.m_rotation 
				&& m_size == other.m_size && m_texture == other.m_texture 
				&& m_tileFactor == other.m_tileFactor && m_color == other.m_color;
		}

		bool operator!=(const DrawCmdData& other) const
		{
			return !(*this == other);
		}

	private:
		Vector3 m_position;
		float m_rotation;
		Vector2 m_size; 
		AReference<Texture2D> m_texture;
		AReference<Texture2D> m_subTexture;
		float m_tileFactor;
		Vector4 m_color; //also acts at tint color
	};

	//keeps a list of all the DrawCmdData objects and submits them to the renderer when the time comes
	class DrawList
	{
	public:
		DrawList() { }

		void AddCmd(const DrawCmdData& cmd) { m_data.Add(cmd); }

		void AddCmd(const Vector2& position, const Vector2& size, const Vector4& color)
		{
			DrawCmdData cmd = DrawCmdData(position, size, color);
			AddCmd(cmd);
		}

		void AddCmd(const Vector2& position, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
		{
			DrawCmdData cmd = DrawCmdData(position, size, texture, tileFactor, tintColor);
			AddCmd(cmd);
		}

		void AddCmd(const Vector3& position, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
		{
			DrawCmdData cmd = DrawCmdData(position, size, texture, tileFactor, tintColor);
			AddCmd(cmd);
		}

		void AddCmd(const Vector3& position, const Vector2& size, AReference<SubTexture2D> subTexture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
		{
			DrawCmdData cmd = DrawCmdData(position, size, subTexture, tileFactor, tintColor);
			AddCmd(cmd);
		}

		void AddCmd(const Vector2& position, float rotation, const Vector2& size, const Vector4& color)
		{
			DrawCmdData cmd = DrawCmdData(position, rotation, size, color);
			AddCmd(cmd);
		}

		void AddCmd(const Vector3 & position, float rotation, const Vector2 & size, const Vector4 & color)
		{
			DrawCmdData cmd = DrawCmdData(position, rotation, size, color);
			AddCmd(cmd);
		}


		void AddCmd(const Vector2& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
		{
			DrawCmdData cmd = DrawCmdData(position, rotation, size, texture, tileFactor, tintColor);
			AddCmd(cmd);
		}

		void AddCmd(const Vector3& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
		{
			DrawCmdData cmd = DrawCmdData(position, rotation, size, texture, tileFactor, tintColor);
			AddCmd(cmd);
		}

		void AddCmd(const Vector3& position, float rotation, const Vector2& size, AReference<SubTexture2D> subTexture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 })
		{
			DrawCmdData cmd = DrawCmdData(position, rotation, size, subTexture, tileFactor, tintColor);
			AddCmd(cmd);
		}

		//submits all draw commands in the list to be rendered
		void DrawAll() const
		{
			for (const DrawCmdData& cmd : m_data)
			{
				cmd.Draw();
			}
		}

		void Clear() { m_data.Clear(); }

	private:
		ADynArr<DrawCmdData> m_data;
	};


	//enum operators so the bitwise operators work on enum classes
	template<class T>
	inline constexpr T operator|(T lhs, T rhs)
	{
		return (T)((int)lhs | (int)rhs);
	}

	template<class T>
	constexpr T operator&(T lhs, T rhs)
	{
		return (T)((int)lhs & (int)rhs);
	}

	template<class T>
	constexpr T operator~(T lhs)
	{
		return (T)(~(int)lhs);
	}

	//keeps track of when to set variables
	//do not combine with binary operators, use as a simple enum not as flags/binary mask
	enum class UICondition
	{
		None = 0, //same as always  
		Always = 1 << 0,   
		
		//only once per runtime
		Once = 1 << 1,   

		//if has no entry in .ini file only
		FirstUseEver = 1 << 2,   
		
		//as is appearing after being hidden/inactive or the first time
		Appearing = 1 << 3    
	};

	union IntPtr
	{
		int i;
		void* ptr;
	};

	bool operator==(const IntPtr& lhs, const IntPtr& rhs);
	bool operator!=(const IntPtr& lhs, const IntPtr& rhs);

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

		bool operator==(const FontConfig& other) const
		{
			return fontData == other.fontData && fontDataSize == other.fontDataSize 
				&& fontNum == other.fontNum;
		}

		bool operator!=(const FontConfig& other) const
		{
			return !(*this == other);
		}
	};

	struct Glyph
	{
		unsigned int codepoint = 31;
		unsigned int visible = 1; //flag to allow early out when rendering
		float advanceX;		  //distance to next character
		Vector2 minCoord, maxCoord; //corners of glyph
		Vector2 minTexCoord, maxTexCoord; //texture coord

		bool operator==(const Glyph& other) const
		{
			return codepoint == other.codepoint && visible == other.visible && advanceX == other.advanceX;
		}

		bool operator!=(const Glyph& other) const
		{
			return !(*this == other);
		}
	};

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

	enum class FontAtlasFlags
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

		const AReference<Texture2D>& GetTexture() const { return m_texture; }

		bool IsBuilt() const { return m_fonts.GetCount() > 0 && m_texture != nullptr; }
		void SetTexture(AReference<Texture2D> texture) { m_texture = texture; }

		int AddCustomRectRegular(int width, int height);
		int AddCustomRectFontGlyph(Font* f, unsigned short id, int width, int height,
			float advanceX, const Vector2& offset = Vector2::Zero());

		void Lock() { m_locked = true; }
		void Unlock() { m_locked = false; }

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
			return r.minVec.y <  maxVec.y&& r.maxVec.y >  maxVec.y
				&& r.maxVec.x <  maxVec.x&& r.maxVec.x >  minVec.x;
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

		bool operator==(const UIRectangle& other) const
		{
			return minVec == other.minVec && maxVec == other.maxVec;
		}

		bool operator!=(const UIRectangle& other) const
		{
			return !(*this == other);
		}
	};

	enum class ViewportFlags
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
		Viewport() : id(0), flags(ViewportFlags::None), parentID(0) { }
		~Viewport();

		Vector2 GetCenter() { return Vector2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f); }
		Vector2 GetWorkPos() { return Vector2(pos.x + workOffsetMin.x, pos.y + workOffsetMin.y); }

		Vector2 GetWorkSize()
		{
			return Vector2(size.x - workOffsetMin.x + workOffsetMax.x, size.y - workOffsetMin.y + workOffsetMax.y);
		}

		unsigned int id;
		ViewportFlags flags;
		Vector2 pos;
		Vector2 size;
		Vector2 workOffsetMin;
		Vector2 workOffsetMax;
		float dpiScale;
		unsigned int parentID;
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
		UIWindow* window; // Set when the viewport is owned by a window 
		Vector2 lastPlatformPos;
		Vector2 lastPlatformSize;
		Vector2 lastRendererSize;
		Vector2 currWorkOffsetMin; // Work area top-left offset being increased during the frame
		Vector2 currWorkOffsetMax; // Work area bottom-right offset being decreased during the frame

		ViewportPrivate() : index(-1), lastFrameActive(-1), lastFrontMostStampCount(-1),
			lastNameHash(0), alpha(1.0f), lastAlpha(1.0f), platformMonitor(-1),
			platformWindowCreated(false), window(nullptr), lastPlatformPos(FLT_MAX, FLT_MAX),
			lastPlatformSize(FLT_MAX, FLT_MAX), lastRendererSize(FLT_MAX, FLT_MAX) { }

		UIRectangle GetMainRect() const { return UIRectangle(pos.x, pos.y, pos.x + size.x, pos.y + size.y); }

		UIRectangle GetWorkRect() const
		{
			return UIRectangle(pos.x + workOffsetMin.x, pos.y + workOffsetMin.y,
				pos.x + size.x + workOffsetMax.x, pos.y + size.y + workOffsetMax.y);
		}
	};


	enum class UIConfigFlags
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

	enum class UIBackEndFlags
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

	struct WindowSettings
	{
		unsigned int id;
		
		//position is relative to viewport
		Vector2Short pos;            
		Vector2Short size;
		Vector2Short viewportPos;
		unsigned int viewportID;

		//id of last known DockNode or 0 if none.
		unsigned int dockID;         
		
		// ID of window class if specified (needed?)
		unsigned int classID;       

		short dockOrder;
		bool collapsed;
		bool wantApply;

		WindowSettings() 
			: id(0), viewportID(0), dockID(0), classID(0), dockOrder(-1), 
			collapsed(false), wantApply(false) { }
		
		//should remove?
		char* GetName() { return (char*)(this + 1); }
	};

	struct UIIO
	{
		friend class UIContext;
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

		UIConfigFlags configFlags;
		UIBackEndFlags backEndFlags;
		Vector2 displaySize;
		float savingRate; //min time to wait before wait to save positions/sizes in sec
		const char* saveFileName; //file to which we write/read size and positions of windows
		const char* logFileName;
		float mouseDoubleClickTime;
		float mouseDoubleClickMaxDist;
		float mouseDragThreshold;
		float keyRepeatDelay;
		float keyRepeatRate;
		void* userData;

		FontAtlas* fonts;
		float fontGlobalScale;
		bool allowUserScaleFont;
		Font* defaultFont;
		Vector2 displayFramebufferScale;

		//Docking options (when enabled)

		bool configDockingNoSplit;
		bool configDockingWithShift;
		bool configDockingAlwaysTabBar;
		bool configDockingTransparentPayload;

		//viewport options (when enabled)
		bool configViewportNoAutoMerge;
		bool configViewportNoTaskBarIcon;
		bool configViewportNoDecoration;
		bool configViewportNoDefaultParent;

		//misc options

		bool drawMouseCursor; //draw the mouse cursor if true
		bool configMacOSXBehavior;
		bool configInputTextCursorBlink;
		bool configWindowsResizeFromEdges;
		bool configWindowsMoveFromTitleBarOnly;
		bool configWindowsMemoryCompactTimer;

		Vector2 mousePos;
		bool mouseDown[5];
		float mouseWheel;
		float mouseWheelH;
		unsigned int mouseHoveredViewport;
		float gamepadInputs[(int)NavInput::Count];
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

	enum class NavLayer
	{
		Main = 0,    // Main scrolling layer
		Menu = 1,    // Menu layer (access with Alt/Menu)
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

	enum class UIColumnFlags
	{
		None = 0,
		NoBorder = 1 << 0,   //disable column dividers
		NoResize = 1 << 1,
		NoPreserveWidths = 1 << 2,
		NoForceWithinWindow = 1 << 3,
		GrowParentContentsSize = 1 << 4    //dear ImGui wants to remove, remove?
	};

	struct UIColumnData
	{
		/*column start offset, normalized
		  0.0 (far left) -> 1.0 (far right)
		*/
		float offsetNorm;
		float offsetNormBeforeResize;
		UIColumnFlags flags;
		UIRectangle clipRect;

		UIColumnData() : offsetNorm(0.0f), offsetNormBeforeResize(0.0f), flags(UIColumnFlags::None) { }

		bool operator==(const UIColumnData& other) const
		{
			return offsetNorm == other.offsetNorm && offsetNormBeforeResize == other.offsetNormBeforeResize 
				&& flags == other.flags && clipRect == other.clipRect;
		}

		bool operator!=(const UIColumnData& other) const
		{
			return !(*this == other);
		}
	};


	//column measurements
	struct UIMenuColumns
	{
		float spacing;
		float width, nextWidth;
		float pos[3], nextWidths[3];

		UIMenuColumns();
		void Update(int count, float spacing, bool clear);
		float DeclColumns(float w0, float w1, float w2);
		float CalcExtraSpace(float avail_w) const;
	};


	struct UIColumn
	{
		unsigned int id;
		UIColumnFlags flags;
		bool isFirstFrame;
		bool isBeingResized;
		int curr;
		int count;
		float offsetMinX, offsetMaxX;
		float lineMinY, lineMaxY;
		float hostCursorPosY;
		float hostCursorMaxPosX;
		UIRectangle hostInitialClipRect;
		UIRectangle hostBackupClipRect;
		UIRectangle hostWorkRect;
		ADynArr<UIColumnData> columns;

		UIColumn() { Clear(); }
		
		void Clear()
		{
			id = 0;
			flags = UIColumnFlags::None;
			isFirstFrame = false;
			isBeingResized = false;
			curr = 0;
			count = 1;
			offsetMinX = offsetMaxX = 0.0f;
			lineMinY = lineMaxY = 0.0f;
			hostCursorPosY = 0.0f;
			hostCursorMaxPosX = 0.0f;
			columns.Clear();
		}

		bool operator==(const UIColumn& other) const
		{
			return id == other.id;
		}

		bool operator!=(const UIColumn& other) const
		{
			return !(*this == other);
		}
	};

	//enum fixed to 0/1 so can be used as index
	enum class UILayoutType
	{
		Horizontal = 0,
		Vertical = 1
	};

	//Transient per-window flags, reset at the beginning of the frame. For child window, inherited from parent on first Begin()
	enum class ItemFlags
	{
		None = 0,
		NoTabStop = 1 << 0,
		ButtonRepeat = 1 << 1, // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
		Disabled = 1 << 2, // Disable interactions but doesn't affect visuals yet. (fix visuals?)
		NoNav = 1 << 3,
		NoNavDefaultFocus = 1 << 4,
		SelectableDontClosePopup = 1 << 5, // MenuItem/Selectable() automatically closes current Popup window

										   //Represent a mixed/indeterminate value, generally multi-selection where values differ. 
		//Currently only supported by Checkbox() (later should support all sorts of widgets) (fix for multiple support?)
		MixedValue = 1 << 6,
		Default = 0
	};


	//storage for LastItem data
	enum class ItemStatusFlags
	{
		None = 0,
		HoveredRect = 1 << 0,
		HasDisplayRect = 1 << 1,
		Edited = 1 << 2,
		ToggledSelection = 1 << 3,
		ToggledOpen = 1 << 4,
		HasDeactivated = 1 << 5,
		Deactivated = 1 << 6
	};

	//stacked storage data for BeginGroup()/EndGroup()
	struct UIGroupData
	{
		Vector2 backupCursorPos;
		Vector2 backupCursorMaxPos;
		float backupIndent;
		float backupGroupOffset;
		Vector2 backupCurrLineSize;
		float backupCurrLineTextBaseOffset;
		unsigned int backupActiveIDIsAlive;
		bool backupActiveIDPreviousFrameIsAlive;
		bool emitItem;

		bool operator==(const UIGroupData& other) const
		{
			return backupCursorPos == other.backupCursorPos && backupIndent == other.backupIndent 
				&& emitItem == other.emitItem;
		}

		bool operator!=(const UIGroupData& other) const
		{
			return !(*this == other);
		}
	};

	//flags for BeginUI() (change name?)
	enum class UIWindowFlags
	{
		None = 0,
		NoTitleBar = 1 << 0,  
		NoResize = 1 << 1,   
		NoMove = 1 << 2,   
		NoScrollbar = 1 << 3, //disable the ui scrollbar(s) mouse wheel still work
		NoScrollWithMouse = 1 << 4,   
		NoCollapse = 1 << 5,   
		AlwaysAutoResize = 1 << 6,  
		NoBackground = 1 << 7,   
		
		//never load/save settings in .ini file
		NoSavedSettings = 1 << 8,
		NoMouseInputs = 1 << 9,   //hovering still works even with flag enabled
		MenuBar = 1 << 10, 
		HorizontalScrollbar = 1 << 11,  
		NoFocusOnAppearing = 1 << 12,  
		NoBringToFrontOnFocus = 1 << 13,  
	
		//show the scrollbar (vertical or horizontal) even if the content fits in the window without need for scrolling
		AlwaysVerticalScrollbar = 1 << 14,
		AlwaysHorizontalScrollbar = 1 << 15,  

		AlwaysUseWindowPadding = 1 << 16,  
		
		//disables gamepad/keyboard navigation inside the window
		NoNavInputs = 1 << 18,  
		
		//window does not take focus when being navigated by gamepad/keyboard
		NoNavFocus = 1 << 19,  

		/*append '*' to title without changing the ID. When used in a tab/docking context, 
		  tab is selected on closure and closure is deferred by one frame to allow 
		  code to cancel the closure and display a popup without flickering
		*/
		UnsavedDocument = 1 << 20,

		//only affects this window
		NoDocking = 1 << 21,  

		NoNav = NoNavInputs | NoNavFocus,
		NoDecoration = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
		NoInputs = NoMouseInputs | NoNavInputs | NoNavFocus,

		//for internal use (should not be used by user)
		
		//allow gamepad/keyboard navigation to cross over parent border to this child 
		//only use on child that have no scrolling
		NavFlattened = 1 << 23, 
		ChildWindow = 1 << 24, 
		Tooltip = 1 << 25, 
		Popup = 1 << 26,
		Modal = 1 << 27,
		ChildMenu = 1 << 28,
		DockNodeHost = 1 << 29 
	};

	struct UIWindowTempData
	{

		//layout data
		Vector2 cursorPos;
		Vector2 cursorPosPrevLine;
		Vector2 cursorStartPos;
		Vector2 cursorMaxPos; //used to calculate size of content
		Vector2 currLineSize;
		Vector2 prevLineSize;
		float currLineTextBaseOffset;
		float prevLineTextBaseOffset;
		float indent;
		float columnOffset;
		float groupOffset;

		//last item status
		unsigned int lastItemID;
		ItemStatusFlags lastItemStatusFlags;
		UIRectangle lastItemRectangle;
		UIRectangle lastItemDisplayRectangle;


		//navigation
		NavLayer navLayerCurr;
		int navLayerCurrMask;
		int navLayerActiveMask;
		int navLayerActiveMaskNext;
		unsigned int navFocusScopeIDCurr;
		bool navHideHighlightOneFrame;
		bool navHasScroll;

		//misc
		bool menuBarAppending; //dear ImGui wants to remove it, remove it?
		Vector2 menuBarOffset;
		UIMenuColumns menuColumns;
		int treeDepth;
		uint32_t treeJumpToParentOnPopMask;
		ADynArr<UIWindow*> childWindows;
		ADynArr<IntPtr>* stateStorage;
		UIColumn* currColumns;
		UILayoutType layoutType;
		UILayoutType parentLayoutType;
		int focusCounterRegular;
		int focusCounterTabStop;

		/*local parameters stacks
		  We store the current settings outside of the vectors to increase memory
		  locality and reduce cache misses.
		*/
		ItemFlags itemFlags;
		float itemWidth;
		float textWrapPos;
		ADynArr<ItemFlags> itemFlagsStack;
		ADynArr<float> itemWidthStack;
		ADynArr<float> textWrapPosStack;
		ADynArr<UIGroupData> groupStack;
		short stackSizeBackup[6]; //stores size of various stacks (used when asserting)

		UIWindowTempData() : cursorPos(Vector2::Zero()), cursorPosPrevLine(Vector2::Zero()), cursorStartPos(Vector2::Zero()),
			cursorMaxPos(Vector2::Zero()), currLineSize(Vector2::Zero()), prevLineSize(Vector2::Zero()),
			currLineTextBaseOffset(0.0f), prevLineTextBaseOffset(0.0f), indent(0.0f),
			columnOffset(0.0f), groupOffset(0.0f), lastItemID(0),
			lastItemStatusFlags(ItemStatusFlags::None),

			navLayerActiveMask(0x00), navLayerActiveMaskNext(0x00),
			navLayerCurr(NavLayer::Main), navLayerCurrMask(1 << (int)NavLayer::Main),
			navFocusScopeIDCurr(0), navHideHighlightOneFrame(false),
			navHasScroll(false),

			menuBarAppending(false), menuBarOffset(Vector2::Zero()),
			treeDepth(0), treeJumpToParentOnPopMask(0x00),
			stateStorage(nullptr), currColumns(nullptr),
			layoutType(UILayoutType::Vertical), parentLayoutType(UILayoutType::Vertical),
			focusCounterRegular(-1), focusCounterTabStop(-1),

			itemFlags(ItemFlags::Default),
			itemWidth(0.0f),
			textWrapPos(-1.0f)
		{
			memset(stackSizeBackup, 0, sizeof(stackSizeBackup));
		}
	};

	enum class TabItemFlags
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
		UIWindow* window;  //when TabItem is part of a DockNode's TabBar, we hold on to a window.
		int lastFrameVisible;
		int lastFrameSelected; //this allows us to infer an ordered list of the last activated tabs with little maintenance
		int nameOffset;
		float offset;
		float width;
		float contentWidth;

		TabItem() : id(0), flags(TabItemFlags::None), lastFrameVisible(-1),
			lastFrameSelected(-1), nameOffset(-1), offset(0.0f), width(0.0f), contentWidth(0.0f) { }

		bool operator==(const TabItem& other) const
		{
			return id == other.id && window == other.window;
		}

		bool operator!=(const TabItem& other) const
		{
			return !(*this == other);
		}
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
			if (!buffer.IsEmpty())
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


	enum class TabBarFlags
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

	enum class DockNodeState
	{
		Unknown,
		HostWindowHiddenBecauseSingleWindow,
		HostWindowHiddenBecauseWindowsAreResizing,
		HostWindowVisible
	};

	// X/Y enums are fixed to 0/1 so they may be used as index
	enum class Axis
	{
		None = -1,
		X = 0,
		Y = 1
	};

	//indicates the authority source (dock node vs window) of a field
	enum class DockDataAuthority
	{
		Auto,
		DockNode,
		Window
	};

	enum class DockRequestType
	{
		None = 0,
		Dock,
		Undock,
		Split//same as Dock but no payload
	};

	enum class DockNodeFlags
	{
		None = 0,
		KeepAliveOnly = 1 << 0,
		NoDockingInCentralNode = 1 << 2,
		PassthruCentralNode = 1 << 3,
		NoSplit = 1 << 4,
		NoResize = 1 << 5,
		AutoHideTabBar = 1 << 6
	};

	struct DockNode
	{
		unsigned int id;
		DockNodeFlags sharedFlags;
		DockNodeFlags localFlags;
		DockNode* parentNode;
		DockNode* childNodes[2];
		ADynArr<UIWindow*>  windows;
		TabBar* tabBar;
		Vector2 pos;
		Vector2 size;
		Vector2 sizeRef;
		Axis splitAxis;

		DockNodeState state;
		UIWindow* hostWindow;
		UIWindow* visibleWindow;
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
		bool IsDockSpace() const { return (int)(localFlags & (DockNodeFlags)(int)DockNodePrivateFlags::DockSpace) != 0; }
		bool IsFloatingNode() const {
			return parentNode == nullptr &&
				(int)(localFlags & (DockNodeFlags)(int)DockNodePrivateFlags::DockSpace) == 0;
		}
		bool IsCentralNode() const { return (int)(localFlags & (DockNodeFlags)(int)DockNodePrivateFlags::CentralNode) != 0; }
		bool IsHiddenTabBar() const { return (int)(localFlags & (DockNodeFlags)(int)DockNodePrivateFlags::HiddenTabBar) != 0; }
		bool IsNoTabBar() const { return (int)(localFlags & (DockNodeFlags)(int)DockNodePrivateFlags::NoTabBar) != 0; }
		bool IsSplitNode() const { return childNodes[0] != nullptr; }
		bool IsLeafNode() const { return childNodes[0] == nullptr; }
		bool IsEmpty() const { return childNodes[0] == nullptr && windows.GetCount() == 0; }
		DockNodeFlags GetMergedFlags() const { return (DockNodeFlags)(sharedFlags | localFlags); }
		UIRectangle GetRect() const { return UIRectangle(pos.x, pos.y, pos.x + size.x, pos.y + size.y); }

	private:
		//extends DockNodeFlags
		enum class DockNodePrivateFlags
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
			NoResizeFlagsMask = (int)DockNodeFlags::NoResize | NoResizeX | NoResizeY,
			LocalFlagsMask = (int)DockNodeFlags::NoSplit | NoResizeFlagsMask | (int)DockNodeFlags::AutoHideTabBar | DockSpace
			| CentralNode | NoTabBar | HiddenTabBar | NoWindowMenuButton | NoCloseButton | NoDocking,

			//~ is bitwise !
			LocalFlagsTransferMask = LocalFlagsMask & ~DockSpace,
			SavedFlagsMask = NoResizeFlagsMask | DockSpace | CentralNode | NoTabBar
			| HiddenTabBar | NoWindowMenuButton | NoCloseButton | NoDocking
		};

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

	//contains the data of window inside of the 
	//actual window on the platform
	struct UIWindow
	{
		char* name;
		unsigned int id;
		UIWindowFlags flags;
		ViewportPrivate* viewport;
		unsigned int viewportID;
		Vector2 viewportPos;
		Vector2 pos;
		Vector2 size; //current size (even if collapsed)
		Vector2 sizeFull; //size when not collapsed
		Vector2 contentSize;
		Vector2 contentSizeExplicit; //needed?
		Vector2 windowPadding;
		float windowRounding; //needed?
		float windowBorderSize;
		int nameBufferLength; //can be larger than strlen(name)
		unsigned int moveID;
		unsigned int childID;
		Vector2 scroll;
		Vector2 scrollMax;
		Vector2 scrollTarget;
		Vector2 scrollTargetCenterRatio;
		Vector2 scrollbarSize;
		bool scrollbarXVisible, scrollbarYVisible;
		bool viewportOwned;
		bool active;
		bool wasActive;
		bool writeAccessed; //true when a widget accesses the current window
		bool collapsed;
		bool wantCollapseToggle;

		//used when items are clipped and don't need to be rendered (needed? do we modify original window for that?)
		bool skipItems;
		bool appearing;
		bool hidden;
		bool isFallbackWindow; //set on the debug default window (needed?)
		bool hasCloseButton;

		//current border is being held for resize
		signed char resizeBorderHeld;

		short beginCount; //number of Begin during current frame (needed?)
		short beginOrderWithinParent;
		short beginOrderWithinContext;
		unsigned int popupID;
		signed char autoFitFramesX, autoFitFramesY;
		signed char autoFitChildAxises;
		bool autoFitOnlyGrows;
		UIDirection autoPosLastDirection;

		//hides and skips the rendering of the window for the next N frames without allowing items to be submitted (needed?)
		int hideWindowSkipItems;

		//same as above but allow items to be submitted for size measuring (needed?)
		int hideWindowDontSkipItems;

		UICondition setWindowPosAllowedFlags; //represents the allowed flags for the setWindowPos function
		UICondition setWindowSizeAllowedFlags;
		UICondition setWindowCollapsedAllowedFlags;
		UICondition setWindowDockAllowedFlags;
		Vector2 setWindowPosVal;
		Vector2 setWindowPosPivot;

		ADynArr<unsigned int> idStack;
		UIWindowTempData tempData;

		UIRectangle outerRectClipped;
		UIRectangle innerRectangle;
		UIRectangle innerClippedRectangle;
		UIRectangle workRectangle;
		UIRectangle clipRectangle;
		UIRectangle contentRegionRectangle;
		Vector2Short hitTestHoleSize, hitTestHoleOffset;

		int lastFrameActive;
		int lastFrameJustFocused;
		float lastTimeActive;
		float itemWidthDefault;
		ADynArr<IntPtr> stateStorage;
		ADynArr<UIColumn> columnStorage;
		float fontWindowScale;
		float fontDpiScale;
		int settingsOffset;

		UIWindow* parentWindow;
		UIWindow* rootWindow;
		UIWindow* rootWindowDockStop;
		UIWindow* rootWindowForTitleBarHighlight;
		UIWindow* rootWindowForNav;

		UIWindow* navLastChildWindow;
		unsigned int navLastIDs[(unsigned long long)NavLayer::Count];
		UIRectangle navRectangleRelative[(unsigned long long)NavLayer::Count];

		bool memoryCompacted;

		//docking
		DockNode* dockNode;
		DockNode* dockNodeAsHost;
		unsigned int dockID;
		ItemStatusFlags dockTabItemStatusFlags;
		UIRectangle dockTabItemRectangle;
		short dockOrder;
		bool dockIsActive : 1; //set to 1 by default?
		bool dockTabIsVisible : 1; //set to 1 by default?
		bool dockTabWantClose : 1; //set to 1 by default?

	public:
		UIWindow(UIContext* context, const char* name);
		~UIWindow();

		unsigned int GetID(const char* str, const char* str_end = nullptr);
		unsigned int GetID(const void* ptr);
		unsigned int GetID(int n);
		unsigned int GetIDNoKeepAlive(const char* str, const char* str_end = nullptr);
		unsigned int GetIDNoKeepAlive(const void* ptr);
		unsigned int GetIDNoKeepAlive(int n);
		unsigned int GetIDFromRectangle(const UIRectangle& r_abs);

		// We don't use g.FontSize because the window may be != g.CurrentWidow.
		UIRectangle Rect() const { return UIRectangle(pos.x, pos.y, pos.x + size.x, pos.y + size.y); }

		float CalcFontSize() const;
		UIRectangle TitleBarRect() const { return UIRectangle(pos, Vector2(pos.x + sizeFull.x, pos.y + TitleBarHeight())); }

		float TitleBarHeight() const;
		float MenuBarHeight() const;

		UIRectangle MenuBarRect() const
		{
			float y1 = pos.y + TitleBarHeight();
			return UIRectangle(pos.x, y1, pos.x + sizeFull.x, y1 + MenuBarHeight());
		}
	};


	// Storage for a tab bar (sizeof() 92~96 bytes)
	struct TabBar
	{
		ADynArr<TabItem> tabs;
		unsigned int id;                     // Zero for tab-bars used by docking
		unsigned int selectedTabID;          // Selected tab/window
		unsigned int nextSelectedTabID;
		unsigned int visibleTabID;           // Can occasionally be != SelectedTabId (e.g. when previewing contents for CTRL+TAB preview)
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
				return tab->window->name;
			}

			AE_CORE_ASSERT(tab->nameOffset != -1 && tab->nameOffset < tabsNames.buffer.GetCount(), "");
			return tabsNames.buffer.GetData() + tab->nameOffset;
		}

		bool operator==(const TabBar& other) const
		{
			return id == other.id;
		}

		bool operator!=(const TabBar& other) const
		{
			return !(*this == other);
		}
	};


	//flags for editing/picking colors
	enum class ColorEditFlags
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

		bool operator==(const UIColorMod& other) const
		{
			return color == other.color && backupVal == other.backupVal;
		}

		bool operator!=(const UIColorMod& other) const
		{
			return !(*this == other);
		}
	};

	enum class UIStyleVar
	{
		None = -1,

		// Enum name ------- // Member in structure 
		Alpha = 0,           // float     Alpha
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

		UIStyleMod() : varIdx(UIStyleVar::None) { }
		UIStyleMod(UIStyleVar idx, int v) : varIdx(idx) { backupInt[0] = v; }
		UIStyleMod(UIStyleVar idx, float v) : varIdx(idx) { backupFloat[0] = v; }
		UIStyleMod(UIStyleVar idx, Vector2 v) : varIdx(idx) { backupFloat[0] = v.x; backupFloat[1] = v.y; }

		bool operator==(const UIStyleMod& other) const
		{
			return varIdx == other.varIdx;
		}

		bool operator!=(const UIStyleMod& other) const
		{
			return !(*this == other);
		}
	};

	// Storage for current popup stack
	struct UIPopupData
	{
		//fields are set in another callback when the popup is created
		unsigned int popupID;        
		UIWindow* window;      
		UIWindow* sourceWindow;
		int openFrameCount; 
		unsigned int openParentID; 
		Vector2 openPopupPos;   
		Vector2 openMousePos;   

		UIPopupData() : popupID(0), window(nullptr), sourceWindow(nullptr), openFrameCount(-1), openParentID(0) { }

		bool operator==(const UIPopupData& other) const
		{
			return popupID == other.popupID;
		}

		bool operator!=(const UIPopupData& other) const
		{
			return !(*this == other);
		}
	};

	//used to navigate sibling menus through parent from a child menu
	enum class NavMoveFlags
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

	struct NavMoveResult
	{
		UIWindow* window; //best candidate window
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

	enum class DragDropFlags
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
			return ptr >= buffer.GetData() && ptr < buffer.GetData() + (buffer.GetCount() * sizeof(T));
		}

		void Clear() 
		{
			for (auto& pair : map)
			{
				delete &buffer[pair.GetElement().num];
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

		bool operator==(const ShrinkWidthItem& other) const
		{
			return index == other.index && width == other.width;
		}

		bool operator!=(const ShrinkWidthItem& other) const
		{
			return !(*this == other);
		}
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

	enum class InputTextFlags
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

	struct DockRequest
	{
		DockRequestType    type;
		UIWindow* dockTargetWindow;   // Destination/Target Window to dock into (may be a loose window or a DockNode, might be NULL in which case DockTargetNode cannot be NULL)
		DockNode* dockTargetNode;     // Destination/Target Node to dock into
		UIWindow* dockPayload;        // Source/Payload window to dock (may be a loose window or a DockNode), [Optional]
		UIDirection dockSplitDir;
		float dockSplitRatio;
		bool dockSplitOuter;
		UIWindow* undockTargetWindow;
		DockNode* undockTargetNode;

		DockRequest() : type(DockRequestType::None), dockTargetWindow(nullptr), dockPayload(nullptr), 
			undockTargetWindow(nullptr), dockTargetNode(nullptr), undockTargetNode(nullptr), 
			dockSplitDir(UIDirection::None), dockSplitRatio(0.5f), dockSplitOuter(false) { }
		
		bool operator==(const DockRequest& other) const
		{
			return type == other.type && dockTargetWindow == other.dockTargetWindow && dockTargetNode == other.dockTargetNode;
		}

		bool operator!=(const DockRequest& other) const
		{
			return !(*this == other);
		}
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

		bool operator==(const DockNodeSettings& other) const
		{
			return id == other.id && parentNodeID == other.parentNodeID && parentWindowID == other.parentWindowID;
		}

		bool operator!=(const DockNodeSettings& other) const
		{
			return !(*this == other);
		}
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

		bool operator==(const SettingsHandler& other) const
		{
			return typeName == other.typeName && typeHash == other.typeHash;
		}

		bool operator!=(const SettingsHandler& other) const
		{
			return !(*this == other);
		}
	};

	//builds and iterate a contiguous stream of variable-sized structures.
	//stores the chunk size first, and align the final size on 4 bytes boundaries 
	//(this what the '(X + 3) & ~3' statement is for)
	template<typename T>
	struct ChunkStream
	{
		ADynArr<char>  buffer;

		void Clear() { buffer.Clear(); }
		bool IsEmpty() const { return buffer.IsEmpty() == 0; }
		size_t GetCount() const { return buffer.GetCount(); }

		T* AllocateChunk(size_t size) 
		{ 
			size_t headerSize = 4;
			//makes sure size has the lower bits (1 & 2) set to 0 ex: ...100
			size = ((headerSize + size) + 3u) & ~3u;
			int offset = buffer.GetCount();
			buffer.Reserve(offset + size);
			((int*)(void*)(buffer.GetData() + offset))[0] = (int)size;
			return (T*)(void*)(buffer.GetData() + offset + (int)headerSize);
		}
		
		T* begin() 
		{ 
			size_t headerSize = 4; 
			if (buffer.GetData() == nullptr)
			{
				return nullptr;
			}
			return (T*)(void*)(buffer.GetData() + headerSize);
		}

		T* NextChunk(T* ptr) 
		{ 
			size_t headerSize = 4;
			AE_CORE_ASSERT(ptr >= begin() && ptr < end(), "invalid ptr provided");
			ptr = (T*)(void*)((char*)(void*)ptr + ChunkSize(ptr));
			if (ptr == (T*)(void*)((char*)end() + headerSize))
			{
				return nullptr;
			}
			AE_CORE_ASSERT(ptr < end(), "invalid ptr provided"); 
			return ptr; 
		}
		
		size_t ChunkSize(const T* ptr) 
		{ 
			return ((const int*)ptr)[-1]; 
		}

		T* end() 
		{ 
			return (T*)(void*)(buffer.GetData() + buffer.GetCount()); 
		}
		
		int OffsetFromPtr(const T* ptr) 
		{ 
			AE_CORE_ASSERT(ptr >= begin() && ptr < end(), "Invalid ptr provided"); 
			const long long offset = (const char*)ptr - buffer.GetData();
			return (int)offset;
		}
		
		T* PtrFromOffset(int offset) 
		{ 
			AE_CORE_ASSERT(offset >= 4 && offset < buffer.GetCount(), "Invalid offset provided");
			return (T*)(void*)(buffer.GetData() + offset);
		}

	};

	enum class LogType
	{
		None = 0,
		TTY,
		File,
		Buffer,
		Clipboard
	};

	struct UIContext
	{
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
		
		ADynArr<UIWindow*> windows;
		ADynArr<UIWindow*> windowFocusOrder;
		ADynArr<UIWindow*> windowTempSortBuffer; //used when reordering windows to keep parents before children
		ADynArr<UIWindow*> currWindowStack;
		AUnorderedMap<unsigned int, StorageData> windowsByID;
		int windowsActiveCount;

		//window state
		UIWindow* currentWindow;
		UIWindow* hoveredWindow;
		UIWindow* hoveredWindowUnderMovingWindow;
		UIWindow* movingWindow;
		UIWindow* wheelingWindow;
		
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
		UIWindow* activeIDWindow;
		InputSource activeIDSource;
		int activeIDMouseButton;
		unsigned int activeIDPreviousFrame;
		bool activeIDPreviousFrameIsAlive;
		bool activeIDPreviousFrameHasBeenEditedBefore;
		UIWindow* activeIDPreviousFrameWindow;
		unsigned int lastActiveID;
		float lastActiveTimer;

		//stacks
		ADynArr<UIColorMod> colorModifiers;
		ADynArr<UIStyleMod> styleModifiers;
		ADynArr<Font*> fontStack;
		ADynArr<UIPopupData> openPopupStack;
		ADynArr<UIPopupData> beginPopupStack;

		//viewports
		ADynArr<ViewportPrivate*> privateViewports;
		float currDPIScale;
		ViewportPrivate* currViewport;
		ViewportPrivate* mouseViewport;
		ViewportPrivate* mouseLastHoveredViewport;
		int viewportsFrontMostStampCount;

		//gamepad/keyboard navigation
		UIWindow* navWindow;
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
		UIWindow* navWrapRequestWindow;
		NavMoveFlags navWrapRequestFlags;

		//navigation windowing
		UIWindow* navWindowingTarget;
		UIWindow* navWindowingTargetAnim;
		UIWindow* navWindowingListWindow;
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
		ChunkStream<WindowSettings> settingsWindows;

		//capture/logging
		bool logEnabled;
		LogType logType;
		FILE* logFile; //switch to std::ofstream?
		StringBuilder logBuffer;
		float logLinePosY;
		bool logLineFirstItem;
		int logDepthRef;
		int logDepthToExpand;
		int logDepthToExpandDefault;

		//debug tool
		bool debugItemPickerActive;
		unsigned int debugItemPickerBreakID;

		//misc
		float framerateSecPerFrame[120];
		int framerateSecPerFrameIndex;
		float framerateSecPerFrameAccum;
		int wantCaptureMouseNextFrame;
		int wantCaptureKeyboardNextFrame;
		int wantTextInputNextFrame;
		char tempBuffer[1024 * 3 + 1];
		size_t frameCount;

		UIContext(FontAtlas* sharedFontAtlas)
			: initialized(false), configFlagsCurrFrame(UIConfigFlags::None), configFlagsLastFrame(UIConfigFlags::None),
			fontAtlasOwnedByContext(sharedFontAtlas != nullptr ? false : true), font(nullptr),
			fontSize(0.0f), fontBaseSize(0.0f), windowsActiveCount(0), wheelingWindowTimer(0.0f),
			hoveredID(0), hoveredIDAllowOverlap(false), hoveredIDPrevFrame(0),
			hoveredIDTimer(0.0f), hoveredIDNotActiveTimer(0.0f), activeID(0),
			activeIDIsAlive(0), activeIDTimer(0.0f), activeIDWasJustActivated(false),
			activeIDAllowOverlap(false), activeIDHasBeenPressedBefore(false), activeIDHasBeenEditedBefore(false),
			activeIDHasBeenEditedThisFrame(false), activeIDUsingNavDirMask(0x00), activeIDUsingNavInputMask(0x00),
			activeIDUsingKeyInputMask(0x00), activeIDClickOffset(-1, -1), activeIDSource(InputSource::None),
			activeIDMouseButton(0), activeIDPreviousFrame(0),
			activeIDPreviousFrameIsAlive(false), activeIDPreviousFrameHasBeenEditedBefore(false),
			lastActiveID(0), lastActiveTimer(0.0f), currDPIScale(0.0f),
			currViewport(nullptr), mouseViewport(nullptr), mouseLastHoveredViewport(nullptr),
			viewportsFrontMostStampCount(0), navWindow(nullptr),
			navID(0), navFocusedScopeID(0), navActivateID(0), navActivateDownID(0), navActivatePressedID(0), navInputID(0),
			navJustTabbedID(0), navJustMovedToID(0), navJustMovedToFocusedScopeID(0), navNextActivateID(0),
			navInputSource(InputSource::None), navScoringCount(0), navLayer(NavLayer::Main), navIDTabCounter(INT_MAX),
			navIDIsAlive(false), navMousePosDirty(false), navDisableHighlight(true), navDisableMouseHover(false),
			navAnyRequest(false), navInitRequest(false), navInitRequestFromMove(false),
			navInitResultID(0), navMoveFromClampedRefRect(false),
			navMoveRequest(false), navMoveRequestFlags(NavMoveFlags::None),
			navMoveRequestForward(NavForward::None), navMoveDir(UIDirection::None),
			navMoveDirLast(UIDirection::None), navMoveClipDir(UIDirection::None),
			navWrapRequestWindow(nullptr), navWrapRequestFlags(NavMoveFlags::None),
			navWindowingTarget(nullptr), navWindowingTargetAnim(nullptr), navWindowingListWindow(nullptr),
			navWindowingTimer(0.0f), navWindowingHighlightAlpha(0.0f), navWindowingToggleLayer(false),
			dragDropActive(false), dragDropWithinSource(false), dragDropWithinTarget(false),
			dragDropSourceFlags(DragDropFlags::None), dragDropSourceFrameCount(-1), dragDropMouseButton(-1),
			dragDropTargetID(0), dragDropAcceptFlags(DragDropFlags::None),
			dragDropAcceptIDCurrRectSurface(0.0f), dragDropAcceptIDPrev(0), dragDropAcceptIDCurr(0),
			dragDropAcceptFrameCount(-1), dragDropHoldJustPressedID(0), currTabBar(nullptr),
			tempInputID(0), colorEditOptions(ColorEditFlags::OptionsDefault),
			colorEditLastHue(0.0f), colorEditLastSaturation(0.0f),
			dragCurrAccumDirty(false), dragCurrAccum(0.0f),
			dragSpeedDefaultRatio(1.0f / 100.0f), scrollBarClickDeltaToGrabCenter(0.0f),
			tooltipOverrideCount(0), platformImePos(FLT_MAX, FLT_MAX), platformImeLastPos(FLT_MAX, FLT_MAX),
			platformImePosViewport(0), settingsLoaded(false), settingsDirtyTimer(0.0f),
			logEnabled(false), logType(LogType::None), logFile(NULL),
			logLinePosY(FLT_MAX), logLineFirstItem(false),
			logDepthRef(0), logDepthToExpand(2), logDepthToExpandDefault(2),
			debugItemPickerActive(false), debugItemPickerBreakID(0),
			framerateSecPerFrameIndex(0), framerateSecPerFrameAccum(0.0f),
			wantCaptureMouseNextFrame(-1), wantCaptureKeyboardNextFrame(-1), wantTextInputNextFrame(-1)
			{
				io.fonts = sharedFontAtlas != nullptr ? sharedFontAtlas : new FontAtlas();
				colorEditLastColor[0] = FLT_MAX;
				colorEditLastColor[1] = FLT_MAX;
				colorEditLastColor[2] = FLT_MAX;
				memset(dragDropPayloadLocalBuffer, 0, sizeof(dragDropPayloadLocalBuffer));
				memset(tempBuffer, 0, sizeof(tempBuffer));
				memset(framerateSecPerFrame, 0, sizeof(framerateSecPerFrame));
			}

	};

	UIContext* CreateContext(FontAtlas* sharedFontAtlas = nullptr);
	
	//set to nullptr to destroy current context
	void DestroyContext(UIContext* context = nullptr);
	UIContext* GetCurrentContext();
	void SetCurrentContext(UIContext* context);
	void Initialize(UIContext* context);

	bool Begin(const char* name, bool* pOpen = nullptr, UIWindowFlags flags = UIWindowFlags::None);
	void End();

	//needed?
	unsigned int HashStr(const char* str, size_t num = 0, size_t seed = 0);
}