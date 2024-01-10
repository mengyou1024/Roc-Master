#pragma once

#include <map>

#include "ft2build.h"
#include "freetype/freetype.h"

struct Character{
    GLuint     TextureID;  // 字形纹理ID
    glm::ivec2 Size;       // 字形大大小
    glm::ivec2 Bearing;    // 字形基于基线和起点的位置
    GLuint     Advance;    // 起点到下一个字形起点的距离
};

class CFreeFont
{
public:
    CFreeFont();
    ~CFreeFont();

    bool Create(const unsigned int iFontID, int iFontSize);

    void Text(float x, float y, const wchar_t* text, glm::vec3 color, float scale);
    void RightText(float x, float y, const wchar_t* text, glm::vec3 color, float scale);

    void Release();

    Character GetCharacter(const wchar_t ch);

public:
    FT_Library  m_Library;
    FT_Face     m_Face;

    GLuint m_iVAO, m_iVBO;

    std::map<wchar_t, Character> m_Characters;
};

