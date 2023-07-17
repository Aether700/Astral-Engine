#pragma once
#include "AstralEngine/UI/UIText.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"

namespace AstralEngine
{
	class TTFGlyph;

	struct CmapSubtable;
	struct CmapFormat;
	
	enum class CmapPlatforms : std::uint16_t
	{
		Unicode = 0,
		Macintosh = 1,
		Reserved = 2, //do not use
		Microsoft = 3
	};


	struct CmapIndex
	{
		std::uint16_t version;
		std::uint16_t numSubtables;
	};

	struct CmapSubtable
	{
		CmapPlatforms platformID;
		std::uint16_t platformSpecificEncoding;
		std::uint32_t offset;
	};

	struct CmapFormat
	{
		// returns the id of the glyph corresponding to the character 
		// provided or 0 if the character was not found
		std::uint16_t GetGlyphID(char c) const
		{
			return GetGlyphID((wchar_t)c);
		}

		virtual std::uint16_t GetGlyphID(wchar_t c) const = 0;
	};


	struct Cmap
	{
		CmapIndex index;
		CmapSubtable* subtables;
		CmapFormat* format;

		Cmap() : subtables(nullptr), format(nullptr) { }
		Cmap(Cmap&& other) noexcept : index(other.index), subtables(other.subtables), format(other.format)
		{
			other.subtables = nullptr;
			other.format = nullptr;
		}

		~Cmap()
		{
			delete[] subtables;
			delete format;
		}

		std::uint16_t GetGlyphID(char c) const
		{
			return format->GetGlyphID(c);
		}

		std::uint16_t GetGlyphID(wchar_t c) const
		{
			return format->GetGlyphID(c);
		}

		CmapSubtable* GetSubtable(CmapPlatforms platformID)
		{
			if (subtables == nullptr)
			{
				return nullptr;
			}

			for (std::uint16_t i = 0; i < index.numSubtables; i++)
			{
				if (subtables[i].platformID == platformID)
				{
					return &subtables[i];
				}
			}
			return nullptr;
		}

		Cmap& operator=(Cmap&& other) noexcept
		{
			delete[] subtables;
			delete format;

			index = other.index;
			format = other.format;
			subtables = other.subtables;

			other.subtables = nullptr;
			other.format = nullptr;

			return *this;
		}
	};


	// Do not cache meshes since they can get deleted as the font is manipulated. Always retrieve the mesh from the font
	class TTFFont
	{
		friend class AReference<TTFFont>;
		friend class CompoundTTFGlyph;
	public:
		static AReference<Font> LoadFont(const std::string& filepath);

		size_t GetGlyphTextureWidth() const;
		size_t GetGlyphTextureHeight() const;

		MeshHandle GetCharTexture(char c) const;
		MeshHandle GetCharTexture(wchar_t c) const;
		
		void SetResolution(size_t resolution);
		void SetGlyphTextureSize(size_t width, size_t height);

		Mat4 ComputeTextureGenerationViewProjMatrix();
	private:
		TTFFont();
		void ClearGlyphs();

		Texture2DHandle GetTextureFromCharIndex(std::uint16_t index) const;

		Cmap m_cmap;
		// maps the char index to the texture
		mutable AUnorderedMap<std::uint16_t, Texture2DHandle> m_cachedGlyphs;
		ADynArr<AReference<TTFGlyph>> m_glyphs;
		size_t m_glyphResolution;
		size_t m_glyphTextureWidth;
		size_t m_glyphTextureHeight;
	};
}