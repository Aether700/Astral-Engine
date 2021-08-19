#include "aepch.h"
#include "UIContext.h"
#include "AstralEngine/Renderer/Renderer2D.h"

namespace AstralEngine
{
    //invalid Unicode code point (standard value).
    #define IM_UNICODE_CODEPOINT_INVALID 0xFFFD
    
    // Maximum Unicode code point supported (might need to be changed for larger char values?)
    #define IM_UNICODE_CODEPOINT_MAX     0xFFFF     

    // Helpers ////////////////////////////////////////////////////

    static inline bool CharIsBlank(char c) 
    { 
        return c == ' ' || c == '\t'; 
    }

    static inline bool CharIsBlankW(unsigned int c) 
    { 
        return c == ' ' || c == '\t' || c == 0x3000; 
    }

    // Convert UTF-8 to 32-bit character, process single character input.
    // Based on stb_from_utf8() from github.com/nothings/stb/
    // We handle UTF-8 decoding error by skipping forward.
    int TextCharFromUtf8(unsigned int* outChar, const char* inText, const char* inTextEnd)
    {
        unsigned int c = (unsigned int)-1;
        const unsigned char* str = (const unsigned char*)inText;
        if (!(*str & 0x80))
        {
            c = (unsigned int)(*str++);
            *outChar = c;
            return 1;
        }
        if ((*str & 0xe0) == 0xc0)
        {
            *outChar = IM_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
            if (inTextEnd != nullptr && inTextEnd - (const char*)str < 2)
            {
                return 1;
            }
            if (*str < 0xc2)
            {
                return 2;
            }
            c = (unsigned int)((*str++ & 0x1f) << 6);
            if ((*str & 0xc0) != 0x80)
            {
                return 2;
            }
            c += (*str++ & 0x3f);
            *outChar = c;
            return 2;
        }

        if ((*str & 0xf0) == 0xe0)
        {
            *outChar = IM_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
            if (inTextEnd != nullptr && inTextEnd - (const char*)str < 3)
            {
                return 1;
            }
            
            if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf))
            {
                return 3;
            }
            
            if (*str == 0xed && str[1] > 0x9f)
            {
                return 3; // str[1] < 0x80 is checked below
            }
            
            c = (unsigned int)((*str++ & 0x0f) << 12);
            if ((*str & 0xc0) != 0x80)
            {
                return 3;
            }

            c += (unsigned int)((*str++ & 0x3f) << 6);
            if ((*str & 0xc0) != 0x80)
            {
                return 3;
            }

            c += (*str++ & 0x3f);
            *outChar = c;
            return 3;
        }

        if ((*str & 0xf8) == 0xf0)
        {
            *outChar = IM_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
            if (inTextEnd != nullptr && inTextEnd - (const char*)str < 4)
            {
                return 1;
            }
            
            if (*str > 0xf4)
            {
                return 4;
            }
            
            if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
            {
                return 4;
            }
            
            if (*str == 0xf4 && str[1] > 0x8f)
            {
                return 4; // str[1] < 0x80 is checked below
            }
            
            c = (unsigned int)((*str++ & 0x07) << 18);
            if ((*str & 0xc0) != 0x80)
            {
                return 4;
            }

            c += (unsigned int)((*str++ & 0x3f) << 12);
            if ((*str & 0xc0) != 0x80)
            {
                return 4;
            }
            
            c += (unsigned int)((*str++ & 0x3f) << 6);
            if ((*str & 0xc0) != 0x80)
            {
                return 4;
            }
            
            c += (*str++ & 0x3f);
            // utf-8 encodings of values used in surrogate pairs are invalid
            if ((c & 0xFFFFF800) == 0xD800)
            {
                return 4;
            }
            
            // If codepoint does not fit in ImWchar, use replacement character U+FFFD instead
            if (c > IM_UNICODE_CODEPOINT_MAX)
            {
                c = IM_UNICODE_CODEPOINT_INVALID;
            }

            *outChar = c;
            return 4;
        }

        *outChar = 0;
        return 0;
    }

    // DrawCmdData ///////////////////////////////////////////////////////////////////////////////////////

    void DrawCmdData::Draw() const
    {
        //skip if no size
        if (m_size.x == 0.0f || m_size.y == 0.0f)
        {
            return;
        }

        if (m_texture == nullptr && m_subTexture == nullptr && m_rotation == 0.0f)
        {
            Renderer2D::DrawQuad(m_position, m_size, m_color);
        }
        else if (m_texture == nullptr && m_subTexture == nullptr)
        {
            Renderer2D::DrawRotatedQuad(m_position, m_rotation, m_size, m_color);
        }
        else if (m_rotation == 0.0f && m_subTexture == nullptr)
        {
            //here color acts as tint color
            Renderer2D::DrawQuad(m_position, m_size, m_texture, m_tileFactor, m_color);
        }
        else if (m_rotation == 0.0f && m_texture == nullptr)
        {
            //here color acts as tint color
            Renderer2D::DrawQuad(m_position, m_size, m_subTexture, m_tileFactor, m_color);
        }
        else if (m_subTexture == nullptr)
        {
            //here color acts as tint color
            Renderer2D::DrawRotatedQuad(m_position, m_rotation, m_size, m_texture, m_tileFactor, m_color);
        }
        else
        {
            //here color acts as tint color
            Renderer2D::DrawRotatedQuad(m_position, m_rotation, m_size, m_subTexture, m_tileFactor, m_color);
        }
    }

    // FontConfig ////////////////////////////////////////////////////////////////////////////////////////
    FontConfig::FontConfig() : fontData(nullptr), fontDataSize(0), fontDataOwnedByAtlas(true), fontNum(0),
        sizePixels(0.0f), oversampleH(3), // FIXME: 2 may be a better default? (change?)
        oversampleV(1), pixelSnapH(false), glyphExtraSpacing(Vector2::Zero()), glyphOffset(Vector2::Zero()),
        glyphRange(nullptr), glyphMinAdvanceX(0.0f), glyphMaxAdvanceX(FLT_MAX), mergeMode(false),
        rasterizerFlags(0x00), rasterizerMultiply(1.0f), ellipsisChar((unsigned short)-1), dstFont(nullptr)
    {
        memset(name, 0, sizeof(name));
    }

    // Font ////////////////////////////////////////////////////////////////////////////////////////
    
    Font::Font() :
        m_fontSize(0.0f), m_fallbackAdvanceX(0.0f), m_fallbackChar((unsigned short)'?'), m_ellipsisChar((unsigned short)-1),
        m_displayOffset(Vector2::Zero()), m_fallbackGlyph(nullptr), m_atlas(nullptr), m_configData(nullptr),
        m_configDataCount(0), m_dirtyLookupTables(false), m_scale(1.0f), m_ascent(0.0f), m_descent(0.0f),
        m_metricTotalSurface(0)
    {
        memset(m_used4KPagesMap, 0, sizeof(m_used4KPagesMap));
    }

    Font::~Font()
    {
        ClearOutputData();
    }

    const Glyph* Font::FindGlyph(unsigned short c) const
    {
        if (c >= m_indexLookup.GetCount())
        {
            return m_fallbackGlyph;
        }

        const unsigned short i = m_indexLookup[c];
        if (i == (unsigned short)-1)
        {
            return m_fallbackGlyph;
        }
        
        return &m_glyphs[i];
    }

    const Glyph* Font::FindGlyphNoFallback(unsigned short c) const
    {
        if (c >= m_indexLookup.GetCount())
        {
            return nullptr;
        }
        
        const unsigned short i = m_indexLookup[c];
        if (i == (unsigned short)-1)
        {
            return nullptr;
        }
        return &m_glyphs[i];
    }


    Vector2 Font::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
    {
        if (!text_end)
            text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

        const float line_height = size;
        const float scale = size / m_fontSize;

        Vector2 text_size = Vector2::Zero();
        float line_width = 0.0f;

        const bool word_wrap_enabled = (wrap_width > 0.0f);
        const char* word_wrap_eol = nullptr;

        const char* s = text_begin;
        while (s < text_end)
        {
            if (word_wrap_enabled)
            {
                // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
                if (!word_wrap_eol)
                {
                    word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                    
                    // Wrap_width is too small to fit anything. 
                    //Force displaying 1 character to minimize the height discontinuity.
                    if (word_wrap_eol == s) 
                    {
                        // +1 may not be a character start point in UTF-8 but 
                        //it's ok because we use s >= word_wrap_eol below
                        word_wrap_eol++;    
                    }
                }

                if (s >= word_wrap_eol)
                {
                    if (text_size.x < line_width)
                    {
                        text_size.x = line_width;
                    }
                    text_size.y += line_height;
                    line_width = 0.0f;
                    word_wrap_eol = nullptr;

                    // Wrapping skips upcoming blanks
                    while (s < text_end)
                    {
                        const char c = *s;
                        if (CharIsBlank(c)) 
                        {
                            s++; 
                        }
                        else if (c == '\n') 
                        { 
                            s++; 
                            break; 
                        }
                        else 
                        { 
                            break; 
                        }
                    }
                    continue;
                }
            }

            // Decode and advance source
            const char* prev_s = s;
            unsigned int c = (unsigned int)*s;
            if (c < 0x80)
            {
                s += 1;
            }
            else
            {
                s += TextCharFromUtf8(&c, s, text_end);
                if (c == 0) // Malformed UTF-8?
                    break;
            }

            if (c < 32)
            {
                if (c == '\n')
                {
                    text_size.x = Math::Max(text_size.x, line_width);
                    text_size.y += line_height;
                    line_width = 0.0f;
                    continue;
                }
                if (c == '\r')
                    continue;
            }

            const float char_width = ((int)c < m_indexAdvanceX.GetCount() ? m_indexAdvanceX[c] : m_fallbackAdvanceX) * scale;
            if (line_width + char_width >= max_width)
            {
                s = prev_s;
                break;
            }

            line_width += char_width;
        }

        if (text_size.x < line_width)
        {
            text_size.x = line_width;
        }

        if (line_width > 0 || text_size.y == 0.0f)
        {
            text_size.y += line_height;
        }

        if (remaining)
        {
            *remaining = s;
        }

        return text_size;
    }

    const char* Font::CalcWordWrapPositionA(float scale, const char* text, const char* textEnd, float wrapWidth) const
    {
        // Simple word-wrapping for English, not full-featured.
        // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", 
        //more sensible support for punctuations, support for Unicode punctuations, etc.) fix?

        // For references, possible wrap point marked with ^
        //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
        //      ^    ^    ^   ^   ^__    ^    ^

        // List of hardcoded separators: .,;!?'"

        // Skip extra blanks after a line returns (that includes not counting them in width computation)
        // e.g. "Hello    world" --> "Hello" "World"

        // Cut words that cannot possibly fit within one line.
        // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

        float lineWidth = 0.0f;
        float wordWidth = 0.0f;
        float blankWidth = 0.0f;
        
        //ignore scale to avoid scaling every character
        wrapWidth /= scale; 

        const char* wordEnd = text;
        const char* prevWordEnd = nullptr;
        bool insideWord = true;

        const char* s = text;
        while (s < textEnd)
        {
            unsigned int c = (unsigned int)*s;
            const char* nextS;
            if (c < 0x80)
            {
                nextS = s + 1;
            }
            else
            {
                nextS = s + TextCharFromUtf8(&c, s, textEnd);
            }
            
            if (c == 0)
            {
                break;
            }

            if (c < 32)
            {
                if (c == '\n')
                {
                    lineWidth = 0.0f;
                    wordWidth = 0.0f;
                    blankWidth = 0.0f;
                    insideWord = true;
                    s = nextS;
                    continue;
                }

                if (c == '\r')
                {
                    s = nextS;
                    continue;
                }
            }

            const float charWidth = (c < m_indexAdvanceX.GetCount() ? m_indexAdvanceX[c] : m_fallbackAdvanceX);
            if (CharIsBlankW(c))
            {
                if (insideWord)
                {
                    lineWidth += blankWidth;
                    blankWidth = 0.0f;
                    wordEnd = s;
                }
                blankWidth += charWidth;
                insideWord = false;
            }
            else
            {
                wordWidth += charWidth;
                if (insideWord)
                {
                    wordEnd = nextS;
                }
                else
                {
                    prevWordEnd = wordEnd;
                    lineWidth += wordWidth + blankWidth;
                    wordWidth = blankWidth = 0.0f;
                }

                // Allow wrapping after punctuation.
                insideWord = (c != '.' && c != ',' && c != ';' && c != '!' && c != '?' && c != '\"');
            }

            // We ignore blank width at the end of the line (they can be skipped)
            if (lineWidth + wordWidth > wrapWidth)
            {
                // Words that cannot possibly fit within an entire line will be cut anywhere.
                if (wordWidth < wrapWidth)
                {
                    s = prevWordEnd != nullptr ? prevWordEnd : wordEnd;
                }
                break;
            }

            s = nextS;
        }

        return s;
    }

    void Font::RenderChar(DrawList* drawList, float size, Vector2 pos, unsigned int col, unsigned short c) const
    {
        const Glyph* glyph = FindGlyph(c);
        if (glyph != nullptr || !glyph->visible)
        {
            return;
        }
        float scale = (size >= 0.0f) ? (size / m_fontSize) : 1.0f;
        pos.x = Math::Floor(pos.x + m_displayOffset.x);
        pos.y = Math::Floor(pos.y + m_displayOffset.y);

        AReference<SubTexture2D> subTexture = AReference<SubTexture2D>::Create(m_atlas->GetTexture(), 
            glyph->minTexCoord, glyph->maxTexCoord);

        drawList->AddCmd((const Vector3&)pos, (const Vector2)Vector2(size * scale, size * scale), 
            (const AReference<SubTexture2D>)subTexture); 
    }

    void Font::RenderText(DrawList* drawList, float size, Vector2 pos, unsigned int col, 
        const Vector4& clipRect, const char* textBegin, const char* textEnd, float wrapWidth, bool cpuFineClip) const
    {
        if (textEnd == nullptr)
        {
            textEnd = textBegin + strlen(textBegin); 
        }

        // Align to be pixel perfect
        pos.x = Math::Floor(pos.x + m_displayOffset.x);
        pos.y = Math::Floor(pos.y + m_displayOffset.y);
        float x = pos.x;
        float y = pos.y;
        if (y > clipRect.w)
        {
            return;
        }

        const float scale = size / m_fontSize;
        const float lineHeight = m_fontSize * scale;
        const bool wordWrapEnabled = (wrapWidth > 0.0f);
        const char* wordWrapEOL = nullptr;

        // Fast-forward to first visible line
        const char* s = textBegin;
        if (y + lineHeight < clipRect.y && !wordWrapEnabled)
        {
            while (y + lineHeight < clipRect.y && s < textEnd)
            {
                //read line by line
                s = (const char*)memchr(s, '\n', textEnd - s);

                s = s != nullptr ? s + 1 : textEnd;
                y += lineHeight;
            }
        }

        // For large text, scan for the last visible line in order to avoid over-reserving in the call to PrimReserve()
        // Note that very large horizontal line will still be affected by the issue (e.g. a one megabyte string buffer without a newline will likely crash atm)
        //(needed since we batch render the whole thing?)
        if (textEnd - s > 10000 && !wordWrapEnabled)
        {
            const char* sEnd = s;
            float yEnd = y;
            while (yEnd < clipRect.w && sEnd < textEnd)
            {
                sEnd = (const char*)memchr(sEnd, '\n', textEnd - sEnd);
                sEnd = sEnd ? sEnd + 1 : textEnd;
                yEnd += lineHeight;
            }
            textEnd = sEnd;
        }
        
        if (s == textEnd)
        {
            return;
        }

        /* old ImGui code keep/needed?
        // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
        const int vtx_count_max = (int)(textEnd - s) * 4;
        const int idx_count_max = (int)(textEnd - s) * 6;
        const int idx_expected_size = drawList->IdxBuffer.Size + idx_count_max;
        draw_list->PrimReserve(idx_count_max, vtx_count_max);

        ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
        ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
        unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;
        */

        while (s < textEnd)
        {
            if (wordWrapEnabled)
            {
                // Calculate how far we can render. Requires two passes on 
                //the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
                if (wordWrapEOL == nullptr)
                {
                    wordWrapEOL = CalcWordWrapPositionA(scale, s, textEnd, wrapWidth - (x - pos.x));
                    
                    //Wrap_width is too small to fit anything. 
                    //Force displaying 1 character to minimize the height discontinuity.
                    if (wordWrapEOL == s) 
                    {
                        // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
                        wordWrapEOL++;    
                    }
                }

                if (s >= wordWrapEOL)
                {
                    x = pos.x;
                    y += lineHeight;
                    wordWrapEOL = nullptr;

                    // Wrapping skips upcoming blanks
                    while (s < textEnd)
                    {
                        const char c = *s;
                        if (CharIsBlank(c)) 
                        { 
                            s++; 
                        }
                        else if (c == '\n') 
                        { 
                            s++; 
                            break; 
                        }
                        else 
                        { 
                            break; 
                        }
                    }
                    continue;
                }
            }

            // Decode and advance source
            unsigned int c = (unsigned int)*s;
            if (c < 0x80)
            {
                s += 1;
            }
            else
            {
                s += TextCharFromUtf8(&c, s, textEnd);
                
                // Malformed UTF-8?
                if (c == 0) 
                {
                    break;
                }
            }

            if (c < 32)
            {
                if (c == '\n')
                {
                    x = pos.x;
                    y += lineHeight;
                    if (y > clipRect.w)
                    {
                        break; // break out of main loop
                    }
                    continue;
                }

                if (c == '\r')
                {
                    continue;
                }
            }

            const Glyph* glyph = FindGlyph(c);
            if (glyph == NULL)
            {
                continue;
            }

            float charWidth = glyph->advanceX * scale;
            if (glyph->visible)
            {
                // We don't do a second finer clipping test on the Y axis as 
                //we've already skipped anything before clipRect.y and exit once we pass clipRect.w
                float x1 = x + glyph->minCoord.x * scale;
                float x2 = y + glyph->maxCoord.x * scale;
                float y1 = x + glyph->minCoord.y * scale;
                float y2 = y + glyph->maxCoord.y * scale;
                if (x1 <= clipRect.z && x2 >= clipRect.x)
                {
                    // Render a character
                    float u1 = glyph->minTexCoord.x;
                    float v1 = glyph->minTexCoord.y;
                    float u2 = glyph->maxTexCoord.x;
                    float v2 = glyph->maxTexCoord.y;

                    //CPU side clipping used to fit text in their frame when the frame is too small. 
                    //Only does clipping for axis aligned quads.
                    if (cpuFineClip)
                    {
                        if (x1 < clipRect.x)
                        {
                            u1 = u1 + (1.0f - (x2 - clipRect.x) / (x2 - x1)) * (u2 - u1);
                            x1 = clipRect.x;
                        }
                        
                        if (y1 < clipRect.y)
                        {
                            v1 = v1 + (1.0f - (y2 - clipRect.y) / (y2 - y1)) * (v2 - v1);
                            y1 = clipRect.y;
                        }
                        
                        if (x2 > clipRect.z)
                        {
                            u2 = u1 + ((clipRect.z - x1) / (x2 - x1)) * (u2 - u1);
                            x2 = clipRect.z;
                        }
                        
                        if (y2 > clipRect.w)
                        {
                            v2 = v1 + ((clipRect.w - y1) / (y2 - y1)) * (v2 - v1);
                            y2 = clipRect.w;
                        }

                        if (y1 >= y2)
                        {
                            x += charWidth;
                            continue;
                        }
                    }

                    //get middle of the glyph and render the character
                    RenderChar(drawList, scale, { (x1 + x2) / 2.0f, (y1 + y2) / 2.0f }, col, c);

                    /* old ImGui code keep/needed?
                    // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug builds. Inlined here:
                    {
                        idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1); idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                        idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2); idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                        vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                        vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                        vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                        vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                        vtx_write += 4;
                        vtx_current_idx += 4;
                        idx_write += 6;
                    }
                    */
                }
            }
            x += charWidth;
        }

        /*old ImGui code keep/needed?
        // Give back unused vertices (clipped ones, blanks) ~ this is essentially a PrimUnreserve() action.
        drawList->VtxBuffer.Size = (int)(vtx_write - draw_list->VtxBuffer.Data); // Same as calling shrink()
        drawList->IdxBuffer.Size = (int)(idx_write - draw_list->IdxBuffer.Data);
        drawList->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
        drawList->_VtxWritePtr = vtx_write;
        drawList->_IdxWritePtr = idx_write;
        drawList->_VtxCurrentIdx = vtx_current_idx;
        */
    }


}