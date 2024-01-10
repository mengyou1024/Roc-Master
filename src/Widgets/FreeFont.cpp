#include "pch.h"

#include "FreeFont.h"

#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "msvcrt.lib")
#pragma comment(lib, "msvcmrt.lib")

CFreeFont::CFreeFont() :
m_Library(0), m_Face(0), m_iVAO(0), m_iVBO(0) {
}

CFreeFont::~CFreeFont() {
    Release();
}

bool CFreeFont::Create(const unsigned int iFontID, int iFontSize) {
    FT_Init_FreeType(&m_Library);
    if (m_Library == 0)
        return false;

    HMODULE  hModule   = GetModuleHandle(NULL);
    HRSRC    hFont     = FindResource(hModule, MAKEINTRESOURCE(iFontID), _T("MYFONT"));
    DWORD    dwSize    = SizeofResource(hModule, hFont);
    FT_Byte* hFontData = (FT_Byte*)LoadResource(hModule, hFont);

    FT_New_Memory_Face(m_Library, hFontData, dwSize, 0, &m_Face);
    FT_Set_Char_Size(m_Face, iFontSize << 6, iFontSize << 6, 72, 72);
    ASSERT(m_Face != 0);

    GLuint  VEO            = 0;
    GLfloat vertices[4][7] = {0};
    GLuint  pIndex[]       = {0, 1, 2, 1, 2, 3};
    glGenVertexArrays(1, &m_iVAO);
    glGenBuffers(1, &m_iVBO);
    glGenBuffers(1, &VEO);
    glBindVertexArray(m_iVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pIndex), pIndex, GL_STATIC_DRAW);
    // Position
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 7 * sizeof(GLfloat), (GLvoid*)0);
    // tex
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 7 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    // Color
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
    glBindVertexArray(0);

    return true;
}

void CFreeFont::Text(float x, float y, const wchar_t* text, glm::vec3 color, float scale) {
    if (m_Face == 0)
        return;

    glBindVertexArray(m_iVAO);
    glColor3f(color.r, color.g, color.b);

    size_t iSize = wcslen(text);
    for (size_t i = 0; i < iSize; i++) {
        Character ch = GetCharacter(text[i]);

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[4][7] = {
            {xpos,     ypos,     0.0, 1.0, color.r, color.g, color.b},
            {xpos + w, ypos,     1.0, 1.0, color.r, color.g, color.b},
            {xpos,     ypos + h, 0.0, 0.0, color.r, color.g, color.b},
            {xpos + w, ypos + h, 1.0, 0.0, color.r, color.g, color.b},
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CFreeFont::RightText(float x, float y, const wchar_t* text, glm::vec3 color, float scale) {
    if (m_Face == 0)
        return;

    glBindVertexArray(m_iVAO);

    int iSize = (int)wcslen(text);
    for (int i = iSize - 1; i >= 0; i--) {
        Character ch = GetCharacter(text[i]);
        x -= (ch.Advance >> 6) * scale;

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[4][7] = {
            {xpos,     ypos,     0.0, 1.0, color.r, color.g, color.b},
            {xpos + w, ypos,     1.0, 1.0, color.r, color.g, color.b},
            {xpos,     ypos + h, 0.0, 0.0, color.r, color.g, color.b},
            {xpos + w, ypos + h, 1.0, 0.0, color.r, color.g, color.b},
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Character CFreeFont::GetCharacter(const wchar_t ch) {
    auto it = m_Characters.find(ch);
    if (it == m_Characters.end()) {
        FT_Load_Char(m_Face, ch, FT_LOAD_RENDER);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ALPHA,
            m_Face->glyph->bitmap.width,
            m_Face->glyph->bitmap.rows,
            0,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            m_Face->glyph->bitmap.buffer);
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character =
            {
                texture,
                {m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows},
                {m_Face->glyph->bitmap_left,  m_Face->glyph->bitmap_top },
                (GLuint)m_Face->glyph->advance.x
        };
        m_Characters.insert(std::pair<wchar_t, Character>(ch, character));
    }

    return m_Characters[ch];
}

void CFreeFont::Release() {
    if (m_Face != 0) {
        FT_Done_Face(m_Face);
        m_Face = 0;
    }

    if (m_Library != 0) {
        FT_Done_FreeType(m_Library);
        m_Library = 0;
    }

    if (m_iVAO != 0) {
        glDeleteVertexArrays(1, &m_iVAO);
        m_iVAO = 0;
    }

    if (m_iVBO != 0) {
        glDeleteBuffers(1, &m_iVBO);
        m_iVBO = 0;
    }

    for (auto it = m_Characters.begin(); it != m_Characters.end(); it++) {
        if (m_Characters[it->first].TextureID != 0) {
            glDeleteTextures(1, &m_Characters[it->first].TextureID);
            m_Characters[it->first].TextureID = 0;
        }
    }
    m_Characters.clear();
}