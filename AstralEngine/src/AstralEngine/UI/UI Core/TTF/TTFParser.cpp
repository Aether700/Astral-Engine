#include "aepch.h"
#include "TTFParser.h"
#include "AstralEngine/Renderer/Tessellation/Tessellation.h"

#include <utility>

namespace AstralEngine
{
	class SimpleTTFGlyph;
	class CompoundTTFGlyph;

	// ttf variable types
	typedef std::int16_t shortFrac;
	typedef std::int16_t Fixed;
	typedef std::int16_t FWord;
	typedef std::uint16_t uFWord;
	typedef std::int16_t F2Dot14;
	typedef std::int64_t longDateTime;

	// tags
	
	static constexpr std::uint32_t s_headTag = 0x64616568;
	static constexpr std::uint32_t s_hheaTag = 0x61656868;
	static constexpr std::uint32_t s_hmtxTag = 0x78746D68;
	static constexpr std::uint32_t s_maxpTag = 0x7078616D;
	static constexpr std::uint32_t s_cmapTag = 0x70616D63;
	static constexpr std::uint32_t s_glyfTag = 0x66796C67;
	static constexpr std::uint32_t s_locaTag = 0x61636F6C;

	//part of the font directory which is the first table of the ttf file
	struct OffsetSubtable
	{
		std::uint32_t scalerType;
		std::uint16_t numTables;
		std::uint16_t searchRange;
		std::uint16_t entrySelector;
		std::uint16_t rangeShift;
	};

	//one per table
	struct TableDirectory
	{
		std::uint32_t tag;
		//detail on how to verify checkSum at 
		//https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html#ScalerTypeNote
		std::uint32_t checkSum; 
		std::uint32_t offset;
		std::uint32_t length;

		bool operator==(const TableDirectory& other) const
		{
			return tag == other.tag;
		}

		bool operator!=(const TableDirectory& other) const
		{
			return !(*this == other);
		}
	};

	struct HeaderTable // head
	{
		Fixed version;
		Fixed fontRevision;
		std::uint32_t checkSumAdjustment;
		std::uint32_t magicNumber;
		std::uint16_t flags;
		std::uint16_t unitsPerEm;
		longDateTime created;
		longDateTime modified;
		FWord xMin;
		FWord yMin;
		FWord xMax;
		FWord yMax;
		std::uint16_t macStyle;
		std::uint16_t lowestRecPPEM;
		std::int16_t fontDirectionHint;
		std::int16_t indexToLocFormat;
		std::int16_t glyphDataFormat;
	};

	struct IndexToLocationTable //loca
	{
		bool isShort;
		union OffsetArr
		{
			std::uint16_t* shortArr;
			std::uint32_t* longArr;
		} offsetArr;
		size_t arrLen;

		IndexToLocationTable() : isShort(true), arrLen(0)
		{
			offsetArr.shortArr = nullptr;
		}

		IndexToLocationTable(IndexToLocationTable&& other) noexcept : isShort(other.isShort), arrLen(other.arrLen)
		{
			if (isShort)
			{
				offsetArr.shortArr = other.offsetArr.shortArr;
				other.offsetArr.shortArr = nullptr;
			}
			else
			{
				offsetArr.longArr = other.offsetArr.longArr;
				other.offsetArr.longArr = nullptr;
			}
			other.arrLen = 0;
		}

		~IndexToLocationTable()
		{
			if (isShort)
			{
				delete[] offsetArr.shortArr;
			}
			else
			{
				delete[] offsetArr.longArr;
			}
		}

		// retrieves the position of the glyph in the font file
		std::uint32_t GetGlyphOffset(size_t glyphIndex)
		{
			if (isShort)
			{
				return offsetArr.shortArr[glyphIndex] * 2;
			}
			return offsetArr.longArr[glyphIndex];
		}

		IndexToLocationTable& operator=(IndexToLocationTable&& other) noexcept
		{
			if (isShort)
			{
				delete[] offsetArr.shortArr;
			}
			else
			{
				delete[] offsetArr.longArr;
			}

			isShort = other.isShort;
			arrLen = other.arrLen;
			if (isShort)
			{
				offsetArr.shortArr = other.offsetArr.shortArr;
				other.offsetArr.shortArr = nullptr;
			}
			else
			{
				offsetArr.longArr = other.offsetArr.longArr;
				other.offsetArr.longArr = nullptr;
			}
			other.arrLen = 0;

			return *this;
		}
	};

	struct HorizontalHeader // hhea
	{
		Fixed version;
		FWord ascent;
		FWord descent;
		FWord lineGap;
		uFWord advanceWidthMax;
		FWord minLeftSideBearing;
		FWord minRightSideBearing;
		FWord xMaxExtent;
		std::int16_t caretSlopeRise;
		std::int16_t caretSlopeRun;
		FWord caretOffset;
		std::int64_t reserved; //set to 0
		std::int16_t metricDataFormat; // 0 for current format
		std::uint16_t numOfLongHorMetrics;
	};

	// used in the hmtx table. The hmtx table is an array of LongHorizontalMetric structs 
	// with an optional array (this optional array is ommitted here)
	struct LongHorizontalMetric
	{
		std::uint16_t advanceWidth;
		std::int16_t leftSideBearing;

		bool operator==(const LongHorizontalMetric& other) const
		{
			return advanceWidth == other.advanceWidth && leftSideBearing == other.leftSideBearing;
		}

		bool operator!=(const LongHorizontalMetric& other) const
		{
			return !(*this == other);
		}
	};

	// table used to track the memory requirements of the font
	struct MaximumProfileTable // maxp table
	{
		Fixed version;
		std::uint16_t numGlyphs;
		std::uint16_t maxPoints; // max number of points in non-compound glyphs
		std::uint16_t maxContours; // max number of contours in non-compound glyphs
		std::uint16_t maxComponentPoints;
		std::uint16_t maxComponentContours;
		std::uint16_t maxZones; // set to 2
		std::uint16_t maxTwilightPoints; // points used in twilight zone
		std::uint16_t maxStorage;
		std::uint16_t maxFunctionDefs;
		std::uint16_t maxInstructionDefs;
		std::uint16_t maxStackElements; // max stack depth
		std::uint16_t maxSizeOfInstructions; // max size of instructions for a single glyph in bytes
		std::uint16_t maxComponentElements; // maximum number of simple glyphs used in a single component glyph
		
		// max recursion depth when defining compound glyphs. 
		// If there are only simple glyphs set to 0
		std::uint16_t maxComponentDepth; 
	};
	
	/*
	struct CmapIndex
	{
		std::uint16_t version;
		std::uint16_t numSubtables;
	};

	enum class CmapPlatforms : std::uint16_t
	{
		Unicode = 0,
		Macintosh = 1,
		Reserved = 2, //do not use
		Microsoft = 3
	};
	*/

	enum class UnicodeEncoding : std::uint16_t
	{
		Version10 = 0, // for version 1.0
		Version11 = 1, // for version 1.1
		ISO_10646_1993 = 2, //deprecated
		Unicode2OrLaterBmpOnly = 3,
		Unicode2OrLaterNoBmpAllowed = 4,
		UnicodeVariationSequences = 5,
		LastResort = 6
	};

	enum class WindowEncoding : std::uint16_t
	{
		Symbol = 0,
		UnicodeBmpOnly = 1,
		ShiftJIS = 2,
		PRC = 3,
		BigFive = 4,
		Johab = 5,
		UnicodeUCS4 = 10
	};

	/*
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
		std::uint16_t GetGlyphID(char c)
		{
			return GetGlyphID((wchar_t)c);
		}

		virtual std::uint16_t GetGlyphID(wchar_t c) = 0;
	};
	*/

	struct CmapFormat4 : public CmapFormat
	{
		/* this format contains an array of glyphs which maps characters to their glyph 
		   however the array is not dense and might have indices with invalid characters 
		   (those characters map to index 0 which is the missing character glyph). A series 
		   of valid characters in that array is called a "segment" and we search through 
		   those segments to find a provided character quickly
		*/
		std::uint16_t format;
		std::uint16_t length;
		std::uint16_t language;
		std::uint16_t segCount; // segCount * 2
		std::uint16_t searchRange;
		std::uint16_t entrySelector;
		std::uint16_t rangeShift;
		std::uint16_t* endCode; // the end character for each segment
		std::uint16_t reservedPad; // should be 0
		std::uint16_t* startCode; // starting character for each segment
		std::int16_t* idDelta; // delta for every character code in segment
		std::uint16_t* idRangeOffset; // offset in bytes to glyph indexArray or 0
		std::uint16_t* glyphIndexArray;

		CmapFormat4() : endCode(nullptr), startCode(nullptr), idDelta(nullptr), 
			idRangeOffset(nullptr), glyphIndexArray(nullptr)
		{
		}

		~CmapFormat4()
		{
			delete[] endCode;
			delete[] startCode;
			delete[] idDelta;
			delete[] idRangeOffset;
			delete[] glyphIndexArray;
		}

		// returns the id of the glyph corresponding to the character 
		// provided or 0 if the character was not found
		virtual std::uint16_t GetGlyphID(wchar_t c) const override
		{
			std::uint16_t &charID = reinterpret_cast<std::uint16_t &>(c);

			// look through each segment to see if the character could be inside of it
			for (std::uint16_t i = 0; i < segCount; i++)
			{
				if (endCode[i] >= charID)
				{
					// verify that the character really is in this segment
					if (startCode[i] > charID)
					{
						break;
					}

					// the character is in this segment and we need to extract it. Equations taken from 
					// apple documentation: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6cmap.html

					if (idRangeOffset[i] == 0)
					{
						return (idDelta[i] + charID) % 65536;
					}
					else
					{
						std::uint16_t index = glyphIndexArray[i - segCount + idRangeOffset[i] / 2 
							+ (charID - startCode[i])];
						
						if (index == 0)
						{
							break;
						}
						return (idDelta[i] + index) % 65536;
					}
				}
			}

			return 0;
		}

	};
	/*
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

		std::uint16_t GetGlyphID(char c)
		{
			return format->GetGlyphID(c);
		}

		std::uint16_t GetGlyphID(wchar_t c)
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
	*/
	
	enum TTFOutlineFlags : std::uint8_t // used for simple glyphs
	{
		TTFOutlineFlagsOnCurve = 1,
		TTFOutlineXShortVec = 1 << 1, // if set the coordinate is 1 byte long otherwise it is 2 bytes long
		TTFOutlineYShortVec = 1 << 2, // if set the coordinate is 1 byte long otherwise it is 2 bytes long
		
		// if set the next byte specifies the number of additional times this set 
		// of flags is to be repeated (the number of flags can be smaller than the 
		// number of points on the glyph)
		TTFOutlineRepeat = 1 << 3, 

		// meaning of the bit depends on the X/YShort bit. See documentation for details: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
		TTFOutlineXSameOrPositive = 1 << 4,
		TTFOutlineYSameOrPositive = 1 << 5,
		TTFOutlineReserved1 = 1 << 6,
		TTFOutlineReserved2 = 1 << 7
	};

	struct GlyphData
	{
	};

	struct SimpleGlyphData : public GlyphData
	{	
		std::uint16_t* endPtsOfContours; // array of length numberOfContours
		std::uint16_t instructionLength; // in bytes
		std::uint8_t* instructions; // array of length instructionLength
		// unpacked arrays of length NumPoints
		TTFOutlineFlags* flags; 
		std::int16_t* xCoordinates;
		std::int16_t* yCoordinates;

		// for internal use only, not part of the documentation
		size_t numOfContours;

		~SimpleGlyphData()
		{
			delete[] endPtsOfContours;
			delete[] instructions;
			delete[] xCoordinates;
			delete[] yCoordinates;
		}

		TTFOutlineFlags GetFlags(size_t index)
		{
			AE_CORE_ASSERT(index < GetNumPoints(), "Index out of bounds");
			return flags[index];
		}
		
		Vector2Int GetCoords(size_t index)
		{
			return Vector2Int(xCoordinates[index], yCoordinates[index]);
		}
		
		std::uint16_t GetNumPoints() const
		{
			return endPtsOfContours[numOfContours - 1] + 1;
		}
	};

	enum TTFCompoundGlyphComponentFlags : std::uint16_t
	{
		TTFCompoundGlyphComponentFlagsArgs1And2AreWords = 1,
		TTFCompoundGlyphComponentFlagsArgsAreXYValues = 1 << 1,
		TTFCompoundGlyphComponentFlagsRoundXYToGrid = 1 << 2,
		TTFCompoundGlyphComponentFlagsHasAScale = 1 << 3,
		TTFCompoundGlyphComponentFlagsObsolete = 1 << 4, // ignore
		TTFCompoundGlyphComponentFlagsMoreComponents = 1 << 5,
		TTFCompoundGlyphComponentFlagsHasXYScale = 1 << 6,
		TTFCompoundGlyphComponentFlagsIsTwoByTwo = 1 << 7,
		TTFCompoundGlyphComponentFlagsHasInstructions = 1 << 8,
		TTFCompoundGlyphComponentFlagsUsesComponentMetric = 1 << 9,
		TTFCompoundGlyphComponentFlagsOverlapCompound = 1 << 10
	};

	enum class TTFCompoundGlyphTransformation
	{
		SameScale, // entry 1
		TwoScales, // entry 2
		FourScales // entry 3
	};

	struct CompoundGlyphData : public GlyphData
	{
		TTFCompoundGlyphComponentFlags flags;
		std::uint16_t glyphIndex;

		union CompoundGlyphArg
		{
			std::uint8_t byte;
			std::uint16_t word;
		};

		CompoundGlyphArg arg1;
		CompoundGlyphArg arg2;

		// some of these variables might not be set/used depending on the flags of the components
		F2Dot14 scale; 
		F2Dot14 xScale; 
		F2Dot14 yScale; 
		F2Dot14 scale01;
		F2Dot14 scale10;

		//TTFCompoundGlyphTransformation transformation;

		CompoundGlyphData* nextGlyph = nullptr;
		
		// only set for "root" glyph data
		std::uint16_t instructionLength;
		std::uint8_t* instructions = nullptr;

		~CompoundGlyphData() 
		{
			delete nextGlyph;
			delete instructions;
		}
	};

	struct GlyphDescription // used in glyf
	{
		std::int16_t numberOfContours; // if > 0 -> simple glyph, if < 0 compound glyph if == 0 no glyph data
		FWord xMin;
		FWord yMin;
		FWord xMax;
		FWord yMax;
		GlyphData* data;

		GlyphDescription() : data(nullptr) { }
		GlyphDescription(GlyphDescription&& other) noexcept : data(other.data), 
			numberOfContours(other.numberOfContours), xMin(other.xMin), yMin(other.yMin), 
			xMax(other.xMax), yMax(other.yMax) 
		{
			other.data = nullptr;
		}

		~GlyphDescription() 
		{
			delete data;
		}

		GlyphDescription& operator=(GlyphDescription&& other) noexcept
		{
			delete data;
			data = other.data;
			other.data = nullptr;

			numberOfContours = other.numberOfContours;
			xMin = other.xMin;
			yMin = other.yMin;
			xMax = other.xMax;
			yMax = other.yMax;

			return *this;
		}

		bool operator==(const GlyphDescription& other) const
		{
			return numberOfContours == other.numberOfContours && data == other.data;
		}

		bool operator!=(const GlyphDescription& other) const
		{
			return !(*this == other);
		}
	};

	struct VerticalHeader // vhea
	{
		std::uint32_t version;
		std::int16_t vertTypoAscender;
		std::int16_t vertTypoDescender;
		std::int16_t vertTypoLineGap;
		std::int16_t advanceHeightMax;
		std::int16_t minTopSideBearing;
		std::int16_t minBottomSideBearing;
		std::int16_t yMaxExtent;
		std::int16_t caretSlopeRise;
		std::int16_t caretSlopeRun;
		std::int16_t caretOffset;
		std::int64_t reserved; // set to 0
		std::int16_t metricDataFormat; // set to 0
		std::uint16_t numOfLongVerMetrics;
	};

	// Internal Font Objects //////////////////////////////////////////////

	class TTFGlyph
	{
	public:
		virtual void SetResolution(float glyphResolution) = 0;
		virtual MeshHandle GenerateMesh() const = 0;

		static AReference<TTFGlyph> Create(GlyphDescription&& description, AReference<TTFFont>& owningFont)
		{
			if (description.numberOfContours > 0)
			{
				return AReference<SimpleTTFGlyph>::Create(std::forward<GlyphDescription>(description));
			}
			
			return AReference<CompoundTTFGlyph>::Create(std::forward<GlyphDescription>(description), owningFont.Get());
		}
	};

	class SimpleTTFGlyph : public TTFGlyph
	{
		struct GlyphPoint;
		using Contour = ADoublyLinkedList<GlyphPoint>;

	public:
		SimpleTTFGlyph(GlyphDescription&& description)
			: m_endPointsOfContours(nullptr), m_basicContours(false)
		{
			AE_CORE_ASSERT(description.numberOfContours > 0, "Initializing Simple glyph with data from a compound glyph");
			m_numContours = description.numberOfContours;
			m_outlineMin = Vector2Short(description.xMin, description.yMin);
			m_outlineMax = Vector2Short(description.xMax, description.yMax);

			SimpleGlyphData* data = (SimpleGlyphData*)description.data;
			size_t numPoints = (size_t)data->GetNumPoints();
			m_endPointsOfContours = new ADynArr<std::uint16_t>(data->numOfContours);
			m_basePoints.Reserve(numPoints);
			m_contours.Reserve(data->numOfContours);
			
			for (size_t i = 0; i < numPoints; i++)
			{
				TTFOutlineFlags flags = data->GetFlags(i);
				m_basePoints.EmplaceBack(data->GetCoords(i), (flags & TTFOutlineFlagsOnCurve), false);
			}

			for (size_t i = 0; i < data->numOfContours; i++)
			{
				m_endPointsOfContours->Add(data->endPtsOfContours[i]);
			}
			ResetContours();
		}

		SimpleTTFGlyph(SimpleTTFGlyph&& other) noexcept : m_basicContours(other.m_basicContours), m_contours(std::move(other.m_contours)),
			m_endPointsOfContours(other.m_endPointsOfContours), m_outlineMin(other.m_outlineMin), 
			m_outlineMax(other.m_outlineMax), m_basePoints(std::move(other.m_basePoints)), 
			m_numContours(other.m_numContours)
		{
			other.m_endPointsOfContours = nullptr;
		}

		~SimpleTTFGlyph() { delete m_endPointsOfContours; }

		virtual void SetResolution(float glyphResolution) override
		{
			AE_PROFILE_FUNCTION();
			if (m_endPointsOfContours == nullptr || glyphResolution < 0.0f)
			{
				return;
			}

			ResetContours();
			if (glyphResolution == 0.0f)
			{
				return;
			}

			m_basicContours = false;

			// generate midpoints
			for (Contour& c : m_contours)
			{
				for (size_t i = 1; i < c.GetCount(); i++)
				{
					GlyphPoint& firstPoint = c[i - 1];
					GlyphPoint& secondPoint = c[i];

					if (!firstPoint.isOnCurve && !secondPoint.isOnCurve)
					{
						GlyphPoint midpoint = GlyphPoint(
							Vector2(((float)(firstPoint.coords.x + secondPoint.coords.x) / 2.0f),
								((float)(firstPoint.coords.y + secondPoint.coords.y) / 2.0f)), false, true);
						c.Insert(midpoint, i);
						i++;
					}
				}
			}

			// add midpoints to smooth the glyph's contours
			for (Contour& c : m_contours)
			{
				Contour contourCopy = Contour(c);
				c.Clear();
				c.Add(contourCopy[0]);
				for (size_t i = 1; i < contourCopy.GetCount(); i++)
				{
					if (!contourCopy[i].isOnCurve && !contourCopy[i].isMidpoint)
					{
						GlyphPoint firstPoint = i == 0 ? contourCopy[0] : contourCopy[i - 1];
						GlyphPoint secondPoint = contourCopy[i];
						GlyphPoint thirdPoint = i + 1 < contourCopy.GetCount() ? contourCopy[i + 1] : contourCopy[0];

						for (int j = 0; j <= glyphResolution; j++)
						{
							float t = (float)j / glyphResolution;
							GlyphPoint point = GlyphPoint(Vector2(
								Math::BezierQuadratic(firstPoint.coords.x,
									secondPoint.coords.x, thirdPoint.coords.x, t),
								Math::BezierQuadratic(firstPoint.coords.y,
									secondPoint.coords.y, thirdPoint.coords.y, t)), true, false);

							if (!ContainsCoords(c, point)) 
							{
								c.AddLast(point);
							}
						}
					}
					else
					{
						if (!ContainsCoords(c, contourCopy[i]))
						{
							c.AddLast(contourCopy[i]);
						}
					}
				}
			}
		}

		virtual MeshHandle GenerateMesh() const override
		{
			ADoublyLinkedList<ADynArr<Vector2>> points;
			for (const Contour& contour : m_contours)
			{
				ADynArr<Vector2>& currContour = points.EmplaceBack(contour.GetCount());
				for (const GlyphPoint& p : contour)
				{
					currContour.AddLast(p.coords);
				}
			}

			return Tessellation::EarClipping(points, TessellationWindingOrder::CounterClockWise);
		}

		SimpleTTFGlyph& operator=(SimpleTTFGlyph&& other) noexcept
		{
			delete[] m_endPointsOfContours;

			m_numContours = other.m_numContours;
			m_endPointsOfContours = other.m_endPointsOfContours;
			m_basicContours = other.m_basicContours;
			m_contours = std::move(other.m_contours);
			m_outlineMin = other.m_outlineMin;
			m_outlineMax = other.m_outlineMax;
			m_basePoints = std::move(other.m_basePoints);

			other.m_endPointsOfContours = nullptr;

			return *this;
		}

	private:
		struct GlyphPoint
		{
			Vector2 coords;
			bool isOnCurve;
			bool isMidpoint; // might want to remove?

			GlyphPoint() : isOnCurve(false), isMidpoint(false) { }
			GlyphPoint(const Vector2& c, bool onCurve = false, bool midpoint = false) : coords(c), isOnCurve(onCurve), 
				isMidpoint(midpoint) { }

			bool operator==(const GlyphPoint& other) const
			{
				return isOnCurve == other.isOnCurve && isMidpoint == other.isMidpoint && coords == other.coords;
			}

			bool operator!=(const GlyphPoint& other) const
			{
				return !(*this == other);
			}
		};

		void ResetContours()
		{
			if (m_endPointsOfContours == nullptr || m_basicContours)
			{
				return;
			}

			m_contours.Clear();
			ADoublyLinkedList<GlyphPoint>* currContour = &m_contours.EmplaceBack();
			for (size_t i = 0; i < m_basePoints.GetCount(); i++)
			{
				currContour->Add(m_basePoints[i]);
				if (m_endPointsOfContours->Contains((std::uint16_t)i) && m_contours.GetCount() < m_numContours)
				{
					currContour = &m_contours.EmplaceBack();
				}
			}

			m_basicContours = true;
		}

		// returns true if the contour contains the provided points. Only checks the coordinates of the 
		// contour not isOnCurve and isMidpoint
		bool ContainsCoords(const Contour& c, const GlyphPoint& point) const 
		{
			for (const GlyphPoint& curr : c) 
			{
				if (curr.coords == point.coords) 
				{
					return true;
				}
			}
			return false;
		}

		std::int16_t m_numContours;
		Vector2Short m_outlineMin;
		Vector2Short m_outlineMax;
		ADynArr<std::uint16_t>* m_endPointsOfContours;
		ADynArr<GlyphPoint> m_basePoints;
		
		// the contours are an ordered list of glyph points which form the individual contours of the glyph 
		// needed to render the glyph. The winding of the points indicate whether the contour is to be rendered 
		// or to be cut out of another contour.
		ADynArr<Contour> m_contours; 
		
		// keeps track of whether or not the contours contain only the basic 
		// points provided in the ttf file or not.
		bool m_basicContours; 
	};

	class CompoundTTFGlyph : public TTFGlyph
	{
	public:
		CompoundTTFGlyph(GlyphDescription&& description, TTFFont* owningFont) 
			: m_owningFont(owningFont), m_description(std::move(description))
		{
			AE_CORE_ASSERT(owningFont != nullptr && m_description.numberOfContours <= 0, "");
		}

		virtual void SetResolution(float glyphResolution) override
		{
			// don't do anything the font object will set the resolution of the simple glyphs of the component glyphs
		}

		virtual MeshHandle GenerateMesh() const override
		{
			ASinglyLinkedList<MeshHandle> componentGlyphMeshes;
			CompoundGlyphData* curr = GetData();

			// start rendering here
			do
			{
				componentGlyphMeshes.Add(m_owningFont->GetMeshFromCharIndex(curr->glyphIndex));
				curr = curr->nextGlyph;
			} 
			while (curr->nextGlyph != nullptr);
			// end rendering here

			AE_CORE_ERROR("Not Implemented yet");
			return NullHandle;
		}

	private:
		CompoundGlyphData* GetData() const { return (CompoundGlyphData*)m_description.data; }

		TTFFont* m_owningFont;
		GlyphDescription m_description;
	};

	// Read functions /////////////////////////////////////////////////////

	/* Variable Reader functions
	   these functions read the given variable from the provided file and then skips the 
	   padding as necessary. TTF files are 32 bit aligned and padded with 0s
	*/

	template<typename T>
	T ReadTTFVar(std::ifstream& file)
	{
		T data;
		file.read((char*)&data, sizeof(T));

		T var;
		AssertDataEndianness(&data, &var, sizeof(T), Endianness::BigEndian);
		return var;
	}

	template<typename T>
	T* ReadTTFArr(std::ifstream& file, size_t arraySize)
	{
		T* arr = new T[arraySize];

		for (size_t i = 0; i < arraySize; i++)
		{
			arr[i] = ReadTTFVar<T>(file);
		}

		return arr;
	}

	Fixed ReadFixed(std::ifstream& file)
	{
		Fixed data;
		file.read((char*)&data, sizeof(Fixed));
		file.seekg((size_t)file.tellg() + 2);

		Fixed var;
		AssertDataEndianness(&data, &var, sizeof(Fixed), Endianness::BigEndian);
		return var;
	}

	FWord ReadFWord(std::ifstream& file)
	{
		FWord data;
		file.read((char*)&data, sizeof(FWord));

		FWord var;
		AssertDataEndianness(&data, &var, sizeof(FWord), Endianness::BigEndian);
		return var;
	}
	
	uFWord ReadUFWord(std::ifstream& file)
	{
		uFWord data;
		file.read((char*)&data, sizeof(uFWord));

		uFWord var;
		AssertDataEndianness(&data, &var, sizeof(uFWord), Endianness::BigEndian);
		return var;
	}

	longDateTime ReadLongDateTime(std::ifstream& file)
	{
		longDateTime data;
		file.read((char*)&data, sizeof(longDateTime));

		longDateTime var;
		AssertDataEndianness(&data, &var, sizeof(longDateTime), Endianness::BigEndian);
		return var;
	}

	// Table Reader functions /////////////////////////////////////////////////////////////

	OffsetSubtable ReadOffsetSubtable(std::ifstream& file)
	{
		OffsetSubtable offset;

		std::uint32_t data;
		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &offset.scalerType, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.numTables, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.searchRange, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.entrySelector, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.rangeShift, sizeof(std::uint16_t), Endianness::BigEndian);

		return offset;
	}

	TableDirectory ReadTableDir(std::ifstream& file)
	{
		TableDirectory dir;
		file.read((char*)&dir.tag, sizeof(std::uint32_t));

		std::uint32_t data;
		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &dir.checkSum, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &dir.offset, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &dir.length, sizeof(std::uint32_t), Endianness::BigEndian);
		
		return dir;
	}

	HeaderTable ReadHeaderTable(std::ifstream& file)
	{
		HeaderTable head;

		head.version = ReadFixed(file);
		head.fontRevision = ReadFixed(file);
		
		head.checkSumAdjustment = ReadTTFVar<std::uint32_t>(file);
		head.magicNumber = ReadTTFVar<std::uint32_t>(file);
		head.flags = ReadTTFVar<std::uint16_t>(file);
		head.unitsPerEm = ReadTTFVar<std::uint16_t>(file);

		head.created = ReadLongDateTime(file);
		head.modified = ReadLongDateTime(file);

		head.xMin = ReadFWord(file);
		head.yMin = ReadFWord(file);
		head.xMax = ReadFWord(file);
		head.yMax = ReadFWord(file);

		head.macStyle = ReadTTFVar<std::uint16_t>(file);
		head.lowestRecPPEM = ReadTTFVar<std::uint16_t>(file);
		head.fontDirectionHint = ReadTTFVar<std::int16_t>(file);
		head.indexToLocFormat = ReadTTFVar<std::int16_t>(file);
		head.glyphDataFormat = ReadTTFVar<std::int16_t>(file);

		return head;
	}

	IndexToLocationTable ReadLoca(std::ifstream& file, HeaderTable& head, MaximumProfileTable& maxp)
	{
		IndexToLocationTable loca;
		loca.isShort = head.indexToLocFormat == 0;
		loca.arrLen = maxp.numGlyphs + 1;
		if (loca.isShort)
		{
			loca.offsetArr.shortArr = ReadTTFArr<std::uint16_t>(file, loca.arrLen);
		}
		else
		{
			loca.offsetArr.longArr = ReadTTFArr<std::uint32_t>(file, loca.arrLen);
		}

		return loca;
	}

	HorizontalHeader ReadHorizontalHeader(std::ifstream& file)
	{
		HorizontalHeader hhea;
		hhea.version = ReadFixed(file);
		hhea.ascent = ReadFWord(file);
		hhea.descent = ReadFWord(file);
		hhea.lineGap = ReadFWord(file);

		hhea.advanceWidthMax = ReadUFWord(file);
		hhea.minLeftSideBearing = ReadFWord(file);
		hhea.minRightSideBearing = ReadFWord(file);
		hhea.xMaxExtent = ReadFWord(file);

		hhea.caretSlopeRise = ReadTTFVar<std::int16_t>(file);
		hhea.caretSlopeRun = ReadTTFVar<std::int16_t>(file);

		hhea.caretOffset = ReadFWord(file);
		hhea.reserved = ReadTTFVar<std::int64_t>(file);
		hhea.metricDataFormat = ReadTTFVar<std::int16_t>(file);
		hhea.numOfLongHorMetrics = ReadTTFVar<std::uint16_t>(file);

		return hhea;
	}

	LongHorizontalMetric ReadLongHorMetric(std::ifstream& file)
	{
		LongHorizontalMetric longHorMetric;

		longHorMetric.advanceWidth = ReadTTFVar<std::uint16_t>(file);
		longHorMetric.leftSideBearing = ReadTTFVar<std::int16_t>(file);

		return longHorMetric;
	}

	MaximumProfileTable ReadMaximumProfileTable(std::ifstream& file)
	{
		MaximumProfileTable maxp;
		maxp.version = ReadFixed(file);
		maxp.numGlyphs = ReadTTFVar<std::uint16_t>(file);
		maxp.maxPoints = ReadTTFVar<std::uint16_t>(file);
		maxp.maxContours = ReadTTFVar<std::uint16_t>(file);

		maxp.maxComponentPoints = ReadTTFVar<std::uint16_t>(file);
		maxp.maxComponentContours = ReadTTFVar<std::uint16_t>(file);
		maxp.maxZones = ReadTTFVar<std::uint16_t>(file);
		maxp.maxTwilightPoints = ReadTTFVar<std::uint16_t>(file);

		maxp.maxStorage = ReadTTFVar<std::uint16_t>(file);
		maxp.maxFunctionDefs = ReadTTFVar<std::uint16_t>(file);
		maxp.maxInstructionDefs = ReadTTFVar<std::uint16_t>(file);
		maxp.maxStackElements = ReadTTFVar<std::uint16_t>(file);

		maxp.maxSizeOfInstructions = ReadTTFVar<std::uint16_t>(file);
		maxp.maxComponentElements = ReadTTFVar<std::uint16_t>(file);
		maxp.maxComponentDepth = ReadTTFVar<std::uint16_t>(file);

		return maxp;
	}

	CmapIndex ReadCmapIndex(std::ifstream& file)
	{
		CmapIndex index;
		index.version = ReadTTFVar<std::uint16_t>(file);
		index.numSubtables = ReadTTFVar<std::uint16_t>(file);

		return index;
	}

	CmapSubtable ReadCmapSubtable(std::ifstream& file)
	{
		CmapSubtable subtable;
		subtable.platformID = ReadTTFVar<CmapPlatforms>(file);
		subtable.platformSpecificEncoding = ReadTTFVar<std::uint16_t>(file);
		subtable.offset = ReadTTFVar<std::uint32_t>(file);

		return subtable;
	}

	CmapSubtable* ReadCmapSubtableArr(std::ifstream& file, std::uint16_t numSubtables)
	{
		CmapSubtable* subtables = new CmapSubtable[numSubtables];

		for (std::uint16_t i = 0; i < numSubtables; i++)
		{
			subtables[i] = ReadCmapSubtable(file);
		}

		return subtables;
	}

	CmapFormat4* ReadCmapFormat4(std::ifstream& file)
	{
		CmapFormat4* format4 = new CmapFormat4();
		format4->format = 4;
		format4->length = ReadTTFVar<std::uint16_t>(file);
		format4->language = ReadTTFVar<std::uint16_t>(file);
		format4->segCount = ReadTTFVar<std::uint16_t>(file) / 2;
		format4->searchRange = ReadTTFVar<std::uint16_t>(file);
		format4->entrySelector = ReadTTFVar<std::uint16_t>(file);
		format4->rangeShift = ReadTTFVar<std::uint16_t>(file);
		format4->endCode = ReadTTFArr<std::uint16_t>(file, format4->segCount);
		format4->reservedPad = ReadTTFVar<std::uint16_t>(file);
		format4->startCode = ReadTTFArr<std::uint16_t>(file, format4->segCount);
		format4->idDelta = ReadTTFArr<std::int16_t>(file, format4->segCount);
		format4->idRangeOffset = ReadTTFArr<std::uint16_t>(file, format4->segCount);
		size_t dataReadSoFar = 14 + 2 * 4 * format4->segCount; // data read for the Cmap format so far in bytes
		format4->glyphIndexArray = ReadTTFArr<std::uint16_t>(file, (format4->length - dataReadSoFar) / 2);

		return format4;
	}

	CmapFormat* ReadCmapFormat(std::ifstream& file)
	{
		std::uint16_t format = ReadTTFVar<std::uint16_t>(file);

		switch(format)
		{
		case 4:
			return ReadCmapFormat4(file);

		case 6:
			AE_CORE_WARN("Cmap format 6 is not yet supported");
			return nullptr;
		}

		AE_CORE_ERROR("Cmap format not supported");
		return nullptr;
	}

	Cmap ReadCmap(std::ifstream& file)
	{
		Cmap c;
		c.index = ReadCmapIndex(file);
		c.subtables = ReadCmapSubtableArr(file, c.index.numSubtables);
		c.format = ReadCmapFormat(file);
		return c;
	}

	void ReadSimpleGlyphCoords(std::ifstream& file, SimpleGlyphData* data, 
		std::int16_t* coordArr, TTFOutlineFlags shortFlag, TTFOutlineFlags positiveRepeatFlag)
	{
		std::int16_t prevCoord = 0;
		for (size_t i = 0; i < data->GetNumPoints(); i++)
		{
			TTFOutlineFlags currFlag = data->GetFlags(i);

			if (currFlag & shortFlag)
			{
				std::int16_t coordDelta = (std::int16_t)ReadTTFVar<std::uint8_t>(file);
				if (!(currFlag & positiveRepeatFlag))
				{
					coordDelta *= -1;
				}
				coordArr[i] = coordDelta + prevCoord;
			}
			else if (currFlag & positiveRepeatFlag)
			{
				if (i != 0)
				{
					coordArr[i] = coordArr[i - 1];
				}
				else
				{
					coordArr[i] = 0;
				}
			}
			else
			{
				coordArr[i] = ReadTTFVar<std::int16_t>(file) + prevCoord;
			}

			prevCoord = coordArr[i];
		}
	}

	SimpleGlyphData* ReadSimpleGlyphData(std::ifstream& file, std::int16_t numContours)
	{
		AE_CORE_ASSERT(numContours > 0, "Number of contours cannot be <= 0 for a simple glyph");

		SimpleGlyphData* data = new SimpleGlyphData();
		data->numOfContours = numContours;
		data->endPtsOfContours = ReadTTFArr<std::uint16_t>(file, (size_t)numContours); 
		data->instructionLength = ReadTTFVar<std::uint16_t>(file);
		if (data->instructionLength > 0)
		{
			data->instructions = ReadTTFArr<std::uint8_t>(file, (size_t)data->instructionLength);
		}
		else
		{
			data->instructions = nullptr;
		}

		// read flags
		size_t numPoints = data->GetNumPoints();
		data->flags = new TTFOutlineFlags[numPoints];

		for (std::int16_t i = 0; i < numPoints; i++)
		{
			TTFOutlineFlags currFlag = ReadTTFVar<TTFOutlineFlags>(file);
			data->flags[i] = currFlag;

			if (currFlag & TTFOutlineRepeat)
			{
				std::uint8_t repeatCount = ReadTTFVar<std::uint8_t>(file);
				for (std::uint8_t j = 0; j < repeatCount; j++)
				{
					i++;
					data->flags[i] = data->flags[i - 1];
				}
			}
		}

		// read X coords
		data->xCoordinates = new std::int16_t[numPoints];
		ReadSimpleGlyphCoords(file, data, data->xCoordinates, TTFOutlineXShortVec, TTFOutlineXSameOrPositive);
		// read y coords
		data->yCoordinates = new std::int16_t[numPoints];
		ReadSimpleGlyphCoords(file, data, data->yCoordinates, TTFOutlineYShortVec, TTFOutlineYSameOrPositive);

		return data;
	}

	CompoundGlyphData* ReadCompoundGlyphData(std::ifstream& file)
	{
		CompoundGlyphData* data = new CompoundGlyphData();
		CompoundGlyphData* currGlyph = data;
		
		do
		{
			currGlyph->flags = ReadTTFVar<TTFCompoundGlyphComponentFlags>(file);
			currGlyph->glyphIndex = ReadTTFVar<std::uint16_t>(file);


			if (currGlyph->flags & TTFCompoundGlyphComponentFlagsArgs1And2AreWords)
			{
				currGlyph->arg1.word = ReadTTFVar<std::uint16_t>(file);
				currGlyph->arg2.word = ReadTTFVar<std::uint16_t>(file);
			}
			else
			{
				currGlyph->arg1.byte = ReadTTFVar<std::uint8_t>(file);
				currGlyph->arg2.byte = ReadTTFVar<std::uint8_t>(file);
			}

			if (currGlyph->flags & TTFCompoundGlyphComponentFlagsHasAScale)
			{
				currGlyph->scale = ReadTTFVar<F2Dot14>(file);
			}
			else if (currGlyph->flags & TTFCompoundGlyphComponentFlagsHasXYScale)
			{
				currGlyph->xScale = ReadTTFVar<F2Dot14>(file);
				currGlyph->yScale = ReadTTFVar<F2Dot14>(file);
			}
			else if (currGlyph->flags & TTFCompoundGlyphComponentFlagsIsTwoByTwo)
			{
				currGlyph->xScale = ReadTTFVar<F2Dot14>(file);
				currGlyph->scale01 = ReadTTFVar<F2Dot14>(file);
				currGlyph->scale10 = ReadTTFVar<F2Dot14>(file);
				currGlyph->yScale = ReadTTFVar<F2Dot14>(file);
			}

			if (currGlyph->flags & TTFCompoundGlyphComponentFlagsMoreComponents)
			{
				CompoundGlyphData* nextGlyph = new CompoundGlyphData();
				currGlyph->nextGlyph = nextGlyph;
				currGlyph = nextGlyph;
			}
		} 
		while (currGlyph->flags & TTFCompoundGlyphComponentFlagsMoreComponents);

		if (currGlyph->flags & TTFCompoundGlyphComponentFlagsHasInstructions)
		{
			data->instructionLength = ReadTTFVar<std::uint16_t>(file);
			data->instructions = ReadTTFArr<std::uint8_t>(file, data->instructionLength);
		}

		return data;
	}

	GlyphDescription ReadGlyphDescription(std::ifstream& file)
	{
		AE_CORE_ASSERT(file.good(), "");

		GlyphDescription glyph;
		
		glyph.numberOfContours = ReadTTFVar<std::int16_t>(file); 
		glyph.xMin = ReadFWord(file);
		glyph.yMin = ReadFWord(file);
		glyph.xMax = ReadFWord(file);
		glyph.yMax = ReadFWord(file);
		
		AE_CORE_ASSERT(file.good(), "");
		if (glyph.numberOfContours > 0)
		{
			glyph.data = ReadSimpleGlyphData(file, glyph.numberOfContours);
			AE_CORE_ASSERT(file.good(), "");
		}
		else if (glyph.numberOfContours < 0)
		{
			glyph.data = ReadCompoundGlyphData(file);
			AE_CORE_ASSERT(file.good(), "");
		}
		// else there is no data so leave data to nullptr (set by constructor)

		return glyph;
	}

	// returns true if the check sum test was correct, false otherwise
	// do not use this function to validate the "head" table
	bool ValidateCheckSum(std::uint32_t* table, std::uint32_t tableSize, std::uint32_t targetCheckSum)
	{
		// checksums might be invalid as many fonts build their checksum differently than per specifications
		// look at https://stackoverflow.com/questions/54151857/missing-pieces-for-ttf-font-building-from-spec
		// for more info on how to properly verify the checksum of the font

		std::uint32_t calculatedCheckSum = 0;
		std::uint32_t numLongsInTable = (tableSize + 3) / 4;
		while (numLongsInTable > 0)
		{
			numLongsInTable--;
			calculatedCheckSum += *table;
			table++;
		}

		return calculatedCheckSum == targetCheckSum;
	}

	TableDirectory* FindTable(ASinglyLinkedList<TableDirectory*>& tableList, std::uint32_t tag)
	{
		for (TableDirectory* currDir : tableList)
		{
			if (currDir->tag == tag)
			{
				return currDir;
			}
		}
		return nullptr;
	}

	// TTFParser //////////////////////////////////////////////////////////

	AReference<Font> TTFFont::LoadFont(const std::string& filepath)
	{
		std::ifstream file = std::ifstream(filepath, std::ios_base::binary);
		if (!file)
		{
			AE_CORE_WARN("Could not open file %S", filepath);
			return nullptr;
		}

		OffsetSubtable offsetSubtable = ReadOffsetSubtable(file);

		HeaderTable head;
		HorizontalHeader hhea;
		ADynArr<LongHorizontalMetric> hmtx;
		ADynArr<AReference<TTFGlyph>> glyf;
		MaximumProfileTable maxp;
		Cmap cmap;
		IndexToLocationTable loca;

		TableDirectory* locaDir = nullptr;
		ASinglyLinkedList<TableDirectory> tableDirectories;
		ASinglyLinkedList<TableDirectory*> dependencyTables; // tables required by other tables for initialization

		for (std::uint16_t i = 0; i < offsetSubtable.numTables; i++)
		{
			tableDirectories.Add(ReadTableDir(file));
			TableDirectory* currDir = &tableDirectories[0];

			switch(currDir->tag)
			{
			case s_hheaTag:
			case s_maxpTag:
			case s_headTag:
				dependencyTables.Add(currDir);
				break;

			case s_locaTag:
				locaDir = currDir;
				break;
			}
		}

		for (TableDirectory* dir : dependencyTables)
		{
			size_t oldPos = file.tellg();
			file.seekg(dir->offset);
			switch(dir->tag)
			{
			case s_headTag:
				head = ReadHeaderTable(file);
				break;

			case s_hheaTag:
				hhea = ReadHorizontalHeader(file);
				break;

			case s_maxpTag:
				maxp = ReadMaximumProfileTable(file);
				break;
			}
			file.seekg(oldPos);
		}

		{
			AE_CORE_ASSERT(locaDir != nullptr, "");
			size_t oldPos = file.tellg();
			file.seekg(locaDir->offset);
			loca = ReadLoca(file, head, maxp); 
			file.seekg(oldPos);
		}

		AReference<TTFFont> loadedFont = AReference<TTFFont>::Create();

		for (TableDirectory& dir : tableDirectories)
		{
			if (FindTable(dependencyTables, dir.tag) != nullptr)
			{
				continue;
			}
			file.seekg(dir.offset);

			switch(dir.tag)
			{
			case s_hmtxTag:
				hmtx.Reserve(hhea.numOfLongHorMetrics);
				for (size_t i = 0; i < (size_t)hhea.numOfLongHorMetrics; i++)
				{
					hmtx.Add(ReadLongHorMetric(file));
				}
				break;

			case s_cmapTag:
				cmap = std::move(ReadCmap(file));
				break;
				
			case s_glyfTag:
				glyf.Reserve(maxp.numGlyphs);
				for (size_t i = 0; i < maxp.numGlyphs; i++)
				{
					file.seekg(loca.GetGlyphOffset(i) + dir.offset);
					glyf.Add(TTFGlyph::Create(std::forward<GlyphDescription>(ReadGlyphDescription(file)), loadedFont));
				}
				break;
			}
			AE_CORE_ASSERT(file.good(), "");
		}

		loadedFont->m_glyphs = std::move(glyf);
		loadedFont->m_cmap = std::move(cmap);
		return loadedFont;
	}

	MeshHandle TTFFont::GetCharMesh(char c) const
	{
		return GetCharMesh((wchar_t)c);
	}
	
	MeshHandle TTFFont::GetCharMesh(wchar_t c) const
	{
		std::uint16_t index = 1042;//m_cmap.GetGlyphID(c);
		return GetMeshFromCharIndex(index);
	}

	void TTFFont::SetResolution(size_t resolution)
	{
		if (m_glyphResolution != resolution)
		{
			m_glyphResolution = resolution;
			ClearGlyphs();
		}
	}

	TTFFont::TTFFont() { }

	void TTFFont::ClearGlyphs()
	{
		for (auto& pair : m_cachedGlyphs)
		{
			ResourceHandler::DeleteMesh(pair.GetElement());
		}
		m_cachedGlyphs.Clear();
	}

	MeshHandle TTFFont::GetMeshFromCharIndex(std::uint16_t index) const
	{
		if (!m_cachedGlyphs.ContainsKey(index))
		{
			AReference<TTFGlyph>& g = const_cast<AReference<TTFGlyph>&>(m_glyphs[index]);
			g->SetResolution(m_glyphResolution);
			MeshHandle glyphMesh = g->GenerateMesh();
			m_cachedGlyphs[index] = glyphMesh;
			return glyphMesh;
		}

		return m_cachedGlyphs[index];
	}

}