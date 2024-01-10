#pragma once

#include "Axis.h"

class OpenGL;
class Mesh
{
public:
    Mesh(OpenGL* pOpenGL);
    virtual ~Mesh();

    virtual void SetSize(int left, int top, int right, int bottom);
    virtual int  Contain(::CPoint pt);

    virtual void SetLimits(float fMin, float fMax);

    virtual void CreateScan();
    virtual void CreateBK();

    virtual void Setup();

    virtual void DrawLineX(float start, float end, float y, glm::vec4 color, float fWidth);
    virtual void DrawLineY(float x, float start, float end, glm::vec4 color, float fWidth);

    virtual void DrawScreenLineX(int x, glm::vec4 color, float fWidth);
    virtual void DrawScreenLineY(int y, glm::vec4 color, float fWidth);
    virtual void DrawScreenQuad(::CPoint pt1, ::CPoint pt2, glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
    virtual void DrawScreenQuad(float l, float r, float t, float b, glm::vec4 color);

    virtual void DrawAixs();
    virtual void DrawAixsText();

    virtual void RenderBK();
    virtual void Render();
    virtual void RenderFore();

    virtual void Release();

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonUp(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonDClick(UINT nFlags, ::CPoint pt);
    virtual void OnMouseMove(UINT nFlags, ::CPoint pt);
    virtual void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt);

    virtual bool IsInArea(::CPoint pt) const final;

public:
    OpenGL* m_pOpenGL;

    //É¨²é·¶Î§
    FLOAT m_pScanMin[2];
    FLOAT m_pScanMax[2];
    CAxis m_pAxis[2];

    GL_RECT m_rcItem;

    float m_fBkCoord, m_fBKAxisCoord;
    float m_fAxisCoord, m_fAxisZeroCoord;

    ITEM_KEYDOWN_TYPE m_KeydownType;
    ::CPoint m_ptLButtonDown;
    ::CPoint m_ptMouseMove;

    GLuint m_iBkVAO, m_iBkVBO, m_iBkEBO;
    std::vector<PT_V3F_T2F> m_pBkVertices;
    std::vector<GLuint> m_pBkIndex;
    int m_pIndexAxisVert[2];

    GLuint m_iAxisVAO, m_iAxisVBO;
    std::vector<PT_V3F_T2F> m_pAxisVertices;
    size_t m_iAxisSize;

    GLuint m_iQuadVAO, m_iQuadVBO, m_iQuadEBO;
    PT_V2F_C4F m_pQuadVertices[4];
};

