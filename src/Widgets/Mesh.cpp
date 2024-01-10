#include "pch.h"

#include "Mesh.h"
#include "OpenGL.h"

Mesh::Mesh(OpenGL* pOpenGL) :
m_pOpenGL(pOpenGL),
m_rcItem{0},
m_iBkVAO(0),
m_iBkVBO(0),
m_iBkEBO(0),
m_iAxisVAO(0),
m_iAxisVBO(0),
m_iAxisSize(0),
m_iQuadVAO(0),
m_iQuadVBO(0),
m_iQuadEBO(0),
m_KeydownType(ITEM_KEYDOWN_TYPE::ITEM_KEYDOWN_NONE),
m_ptLButtonDown{0},
m_pIndexAxisVert{0},
m_pScanMin{0},
m_pScanMax{100, 100},
m_fBkCoord((BK_TEX_DEF + 0.5F) / COLOR_TEX_HEIGHT),
m_fBKAxisCoord((BK_TEX_AXIS_BK + 0.5F) / COLOR_TEX_HEIGHT),
m_fAxisCoord((BK_TEX_AXIS + 0.5F) / COLOR_TEX_HEIGHT),
m_fAxisZeroCoord((BK_TEX_AXIS_ZERO + 0.5F) / COLOR_TEX_HEIGHT) {}

Mesh::~Mesh() {
    DeleteVAO(m_iBkVAO, m_iBkVBO, m_iBkEBO);
    DeleteVAO(m_iAxisVAO, m_iAxisVBO);
    DeleteVAO(m_iQuadVAO, m_iQuadVBO, m_iQuadEBO);

    Release();
}

void Mesh::SetSize(int left, int top, int right, int bottom) {
    m_rcItem.set(left, right, top, bottom);

    CreateBK();
}

int Mesh::Contain(::CPoint pt) {
    return m_rcItem.Contain(pt.x, pt.y);
}

void Mesh::SetLimits(float fMin, float fMax) {}

void Mesh::CreateScan() {}

void Mesh::CreateBK() {
    // 4(bk) + 4(x) + 4(y)
    m_pBkVertices.resize(12);
    m_pBkIndex.resize(18);

    int iIndex = 0, iEbo = 0;
    // BK
    m_pBkVertices[iIndex].x = (float)m_rcItem.left;
    m_pBkVertices[iIndex].y = (float)m_rcItem.top;
    m_pBkVertices[iIndex].z = 0;
    m_pBkVertices[iIndex].s = 0;
    m_pBkVertices[iIndex].t = m_fBkCoord;

    m_pBkVertices[iIndex + 1].x = (float)m_rcItem.right;
    m_pBkVertices[iIndex + 1].y = (float)m_rcItem.top;
    m_pBkVertices[iIndex + 1].z = 0;
    m_pBkVertices[iIndex + 1].s = 1;
    m_pBkVertices[iIndex + 1].t = m_fBkCoord;

    m_pBkVertices[iIndex + 2].x = (float)m_rcItem.left;
    m_pBkVertices[iIndex + 2].y = (float)m_rcItem.bottom;
    m_pBkVertices[iIndex + 2].z = 0;
    m_pBkVertices[iIndex + 2].s = 0;
    m_pBkVertices[iIndex + 2].t = m_fBkCoord;

    m_pBkVertices[iIndex + 3].x = (float)m_rcItem.right;
    m_pBkVertices[iIndex + 3].y = (float)m_rcItem.bottom;
    m_pBkVertices[iIndex + 3].z = 0;
    m_pBkVertices[iIndex + 3].s = 1;
    m_pBkVertices[iIndex + 3].t = m_fBkCoord; // 波形背景区域4个点组成的2个三角形

    m_pBkIndex[iEbo]     = iIndex;
    m_pBkIndex[iEbo + 1] = iIndex + 1;
    m_pBkIndex[iEbo + 2] = iIndex + 2;
    m_pBkIndex[iEbo + 3] = iIndex + 1;
    m_pBkIndex[iEbo + 4] = iIndex + 2;
    m_pBkIndex[iEbo + 5] = iIndex + 3;
    iIndex += 4, iEbo += 6; // 三角形点的绘制顺序 012 /123

    // Axis X BK
    m_pBkVertices[iIndex].x = (float)m_rcItem.vleft;
    m_pBkVertices[iIndex].y = (float)m_rcItem.vtop;
    m_pBkVertices[iIndex].z = 0;
    m_pBkVertices[iIndex].s = 0;
    m_pBkVertices[iIndex].t = m_fBKAxisCoord;

    m_pBkVertices[iIndex + 1].x = (float)m_rcItem.vright;
    m_pBkVertices[iIndex + 1].y = (float)m_rcItem.vtop;
    m_pBkVertices[iIndex + 1].z = 0;
    m_pBkVertices[iIndex + 1].s = 1;
    m_pBkVertices[iIndex + 1].t = m_fBKAxisCoord;

    m_pBkVertices[iIndex + 2].x = (float)m_rcItem.vleft;
    m_pBkVertices[iIndex + 2].y = (float)m_rcItem.top;
    m_pBkVertices[iIndex + 2].z = 0;
    m_pBkVertices[iIndex + 2].s = 0;
    m_pBkVertices[iIndex + 2].t = m_fBKAxisCoord;

    m_pBkVertices[iIndex + 3].x = (float)m_rcItem.vright;
    m_pBkVertices[iIndex + 3].y = (float)m_rcItem.top;
    m_pBkVertices[iIndex + 3].z = 0;
    m_pBkVertices[iIndex + 3].s = 1;
    m_pBkVertices[iIndex + 3].t = m_fBKAxisCoord;

    m_pBkIndex[iEbo]     = iIndex;
    m_pBkIndex[iEbo + 1] = iIndex + 1;
    m_pBkIndex[iEbo + 2] = iIndex + 2;
    m_pBkIndex[iEbo + 3] = iIndex + 1;
    m_pBkIndex[iEbo + 4] = iIndex + 2;
    m_pBkIndex[iEbo + 5] = iIndex + 3;
    iIndex += 4, iEbo += 6;

    // Axis Y BK
    /**/
    m_pBkVertices[iIndex].x = (float)m_rcItem.left;
    m_pBkVertices[iIndex].y = (float)m_rcItem.vtop;
    m_pBkVertices[iIndex].z = 0;
    m_pBkVertices[iIndex].s = 0;
    m_pBkVertices[iIndex].t = m_fBKAxisCoord;

    m_pBkVertices[iIndex + 1].x = (float)m_rcItem.vleft;
    m_pBkVertices[iIndex + 1].y = (float)m_rcItem.vtop;
    m_pBkVertices[iIndex + 1].z = 0;
    m_pBkVertices[iIndex + 1].s = 1;
    m_pBkVertices[iIndex + 1].t = m_fBKAxisCoord;

    m_pBkVertices[iIndex + 2].x = (float)m_rcItem.left;
    m_pBkVertices[iIndex + 2].y = (float)m_rcItem.bottom;
    m_pBkVertices[iIndex + 2].z = 0;
    m_pBkVertices[iIndex + 2].s = 0;
    m_pBkVertices[iIndex + 2].t = m_fBKAxisCoord;

    m_pBkVertices[iIndex + 3].x = (float)m_rcItem.vleft;
    m_pBkVertices[iIndex + 3].y = (float)m_rcItem.bottom;
    m_pBkVertices[iIndex + 3].z = 0;
    m_pBkVertices[iIndex + 3].s = 1;
    m_pBkVertices[iIndex + 3].t = m_fBKAxisCoord;

    m_pBkIndex[iEbo]     = iIndex;
    m_pBkIndex[iEbo + 1] = iIndex + 1;
    m_pBkIndex[iEbo + 2] = iIndex + 2;
    m_pBkIndex[iEbo + 3] = iIndex + 1;
    m_pBkIndex[iEbo + 4] = iIndex + 2;
    m_pBkIndex[iEbo + 5] = iIndex + 3;
    iIndex += 4, iEbo += 6;
}

void Mesh::Setup() {
    // 背景
    if (m_iBkVAO == 0) {
        GenVAO(m_iBkVAO, m_iBkVBO, m_iBkEBO);
        glBindVertexArray(m_iBkVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iBkVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * m_pBkVertices.size(), &m_pBkVertices[0], GL_STREAM_DRAW);
        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBkEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_pBkIndex.size(), &m_pBkIndex[0], GL_STATIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid*)0);
        // tex
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, m_iBkVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PT_V3F_T2F) * m_pBkVertices.size(), &m_pBkVertices[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // 刻度
    if (m_iAxisVAO == 0) {
        GenVAO(m_iAxisVAO, m_iAxisVBO);
        glBindVertexArray(m_iAxisVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iAxisVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * 2, NULL, GL_DYNAMIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid*)0);
        // tex
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    if (m_iQuadVAO == 0) {
        GLuint pIndex[] = {0, 1, 2, 1, 2, 3};
        GenVAO(m_iQuadVAO, m_iQuadVBO, m_iQuadEBO);
        glBindVertexArray(m_iQuadVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuadVertices), m_pQuadVertices, GL_STREAM_DRAW);
        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iQuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pIndex), pIndex, GL_STATIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // tex
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }
}

void Mesh::Release() {}

void Mesh::DrawLineX(float start, float end, float y, glm::vec4 color, float fWidth) {
    m_pQuadVertices[0].x = start;
    m_pQuadVertices[0].y = y;
    m_pQuadVertices[0].r = color.r;
    m_pQuadVertices[0].g = color.g;
    m_pQuadVertices[0].b = color.b;
    m_pQuadVertices[0].a = color.a;

    m_pQuadVertices[1].x = end;
    m_pQuadVertices[1].y = y;
    m_pQuadVertices[1].r = color.r;
    m_pQuadVertices[1].g = color.g;
    m_pQuadVertices[1].b = color.b;
    m_pQuadVertices[1].a = color.a;

    glLineWidth(fWidth);
    glBindVertexArray(m_iQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_iQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * 2, m_pQuadVertices, GL_STREAM_DRAW);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glLineWidth(1);
}

void Mesh::DrawLineY(float x, float start, float end, glm::vec4 color, float fWidth) {
    m_pQuadVertices[0].x = x;
    m_pQuadVertices[0].y = start;
    m_pQuadVertices[0].r = color.r;
    m_pQuadVertices[0].g = color.g;
    m_pQuadVertices[0].b = color.b;
    m_pQuadVertices[0].a = color.a;

    m_pQuadVertices[1].x = x;
    m_pQuadVertices[1].y = end;
    m_pQuadVertices[1].r = color.r;
    m_pQuadVertices[1].g = color.g;
    m_pQuadVertices[1].b = color.b;
    m_pQuadVertices[1].a = color.a;

    glLineWidth(fWidth);
    glBindVertexArray(m_iQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_iQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * 2, m_pQuadVertices, GL_STREAM_DRAW);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glLineWidth(1);
}

void Mesh::DrawScreenLineX(int x, glm::vec4 color, float fWidth) {
    DrawScreenQuad((float)x - fWidth / 2.0f, (float)x + fWidth / 2.0f, (float)m_rcItem.vtop, (float)m_rcItem.vbottom, color);
}

void Mesh::DrawScreenLineY(int y, glm::vec4 color, float fWidth) {
    DrawScreenQuad((float)m_rcItem.vleft, (float)m_rcItem.vright, (float)y - fWidth / 2.0f, (float)y + fWidth / 2.0f, color);
}

void Mesh::DrawScreenQuad(::CPoint pt1, ::CPoint pt2, glm::vec4 color) {
    DrawScreenQuad((float)pt1.x, (float)pt2.x, (float)pt1.y, (float)pt2.y, color);
}

void Mesh::DrawScreenQuad(float l, float r, float t, float b, glm::vec4 color) {
    m_pQuadVertices[0].x = l;
    m_pQuadVertices[0].y = t;
    m_pQuadVertices[0].r = color.r;
    m_pQuadVertices[0].g = color.g;
    m_pQuadVertices[0].b = color.b;
    m_pQuadVertices[0].a = color.a;

    m_pQuadVertices[1].x = r;
    m_pQuadVertices[1].y = t;
    m_pQuadVertices[1].r = color.r;
    m_pQuadVertices[1].g = color.g;
    m_pQuadVertices[1].b = color.b;
    m_pQuadVertices[1].a = color.a;

    m_pQuadVertices[2].x = l;
    m_pQuadVertices[2].y = b;
    m_pQuadVertices[2].r = color.r;
    m_pQuadVertices[2].g = color.g;
    m_pQuadVertices[2].b = color.b;
    m_pQuadVertices[2].a = color.a;

    m_pQuadVertices[3].x = r;
    m_pQuadVertices[3].y = b;
    m_pQuadVertices[3].r = color.r;
    m_pQuadVertices[3].g = color.g;
    m_pQuadVertices[3].b = color.b;
    m_pQuadVertices[3].a = color.a;

    glBindVertexArray(m_iQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_iQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuadVertices), m_pQuadVertices, GL_STREAM_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::DrawAixs() {
    glBindVertexArray(m_iAxisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_iAxisVBO);
    m_pAxisVertices.resize(2);

    // 坐标
    for (int i = 0; i < (int)m_pAxis[AXIS_X].GetCount(); ++i) {
        float x = (float)m_rcItem.vleft + 1 + m_pAxis[AXIS_X][i];
        float y = (float)m_rcItem.vtop;

        float fCoord = m_pAxis[AXIS_X].m_pAxis_value[i] == 0 ? m_fAxisZeroCoord : m_fAxisCoord;

        m_pAxisVertices[0].x = x;
        m_pAxisVertices[0].y = y;
        m_pAxisVertices[0].z = 0;
        m_pAxisVertices[0].s = 0.5F;
        m_pAxisVertices[0].t = fCoord;

        if (m_pAxis[AXIS_X].m_pAxisType[i] == 2)
            y -= 15;
        else if (m_pAxis[AXIS_X].m_pAxisType[i] == 1)
            y -= 10;
        else
            y -= 5;

        m_pAxisVertices[1].x = x;
        m_pAxisVertices[1].y = y;
        m_pAxisVertices[1].z = 0;
        m_pAxisVertices[1].s = 0.5F;
        m_pAxisVertices[1].t = fCoord;

        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * 2, m_pAxisVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 2);
    }

    m_pAxisVertices[0].x = (float)m_rcItem.vleft;
    m_pAxisVertices[0].y = (float)m_rcItem.vtop;
    m_pAxisVertices[0].z = 0;
    m_pAxisVertices[0].s = 0.5F;
    m_pAxisVertices[0].t = m_fAxisCoord;
    m_pAxisVertices[1].x = (float)m_rcItem.vright;
    m_pAxisVertices[1].y = (float)m_rcItem.vtop;
    m_pAxisVertices[1].z = 0;
    m_pAxisVertices[1].s = 0.5F;
    m_pAxisVertices[1].t = m_fAxisCoord;

    glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * 2, m_pAxisVertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);

    // y
    for (int i = 0; i < (int)m_pAxis[AXIS_Y].GetCount(); ++i) {
        float x = (float)m_rcItem.vleft;
        float y = (float)m_rcItem.vbottom - m_pAxis[AXIS_Y][i];

        float fCoord = m_pAxis[AXIS_Y].m_pAxis_value[i] == 0 ? m_fAxisZeroCoord : m_fAxisCoord;

        m_pAxisVertices[0].x = x;
        m_pAxisVertices[0].y = y;
        m_pAxisVertices[0].z = 0;
        m_pAxisVertices[0].s = 0.5F;
        m_pAxisVertices[0].t = fCoord;

        if (m_pAxis[AXIS_Y].m_pAxisType[i] == 2)
            x -= 15;
        else if (m_pAxis[AXIS_Y].m_pAxisType[i] == 1)
            x -= 10;
        else
            x -= 5;

        m_pAxisVertices[1].x = x;
        m_pAxisVertices[1].y = y;
        m_pAxisVertices[1].z = 0;
        m_pAxisVertices[1].s = 0.5F;
        m_pAxisVertices[1].t = fCoord;

        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * 2, m_pAxisVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 2);
    }

    m_pAxisVertices[0].x = (float)m_rcItem.vleft;
    m_pAxisVertices[0].y = (float)m_rcItem.vtop;
    m_pAxisVertices[0].z = 0;
    m_pAxisVertices[0].s = 0.5F;
    m_pAxisVertices[0].t = m_fAxisCoord;
    m_pAxisVertices[1].x = (float)m_rcItem.vleft;
    m_pAxisVertices[1].y = (float)m_rcItem.vbottom;
    m_pAxisVertices[1].z = 0;
    m_pAxisVertices[1].s = 0.5F;
    m_pAxisVertices[1].t = m_fAxisCoord;

    glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * 2, m_pAxisVertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
}

void Mesh::DrawAixsText() {
    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    CString   strInfo;
    glm::vec4 color(1.0f, 0, 0, 1.0f);

    // 坐标
    glScissor(m_rcItem.vleft, m_rcItem.top, m_rcItem.vWidth(), m_rcItem.iAxisWidth);
    glPushMatrix();
    glTranslatef((float)m_rcItem.vleft, (float)m_rcItem.top, 0.0F);
    for (int i = 0; i < (int)m_pAxis[AXIS_X].GetCount(); ++i) {
        if (m_pAxis[AXIS_X].m_pAxisType[i] == 2) {
            // 刻度文字
            if (m_pAxis[AXIS_X].m_pAxis_value[i] == 0) {
                strInfo.Format(_T("%.1fmm"), m_pAxis[AXIS_X].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 0, 0, 1.0f);
            } else {
                strInfo.Format(_T("%.1f"), m_pAxis[AXIS_X].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 1.0f, 0, 1.0f);
            }
            m_pOpenGL->m_Font.Text(m_pAxis[AXIS_X][i] + 1.0f, 2.0f, strInfo, color, 0.75F);
        }
    }
    glPopMatrix();

    // y
    glScissor(m_rcItem.left, m_rcItem.vtop, m_rcItem.iAxisWidth, m_rcItem.vHeight());
    glPushMatrix();
    glTranslatef((float)m_rcItem.left, (float)m_rcItem.bottom, 0.0F);
    glRotatef(-90, 0.0F, 0.0F, 1.0F);
    for (int i = 0; i < (int)m_pAxis[AXIS_Y].GetCount(); ++i) {
        if (m_pAxis[AXIS_Y].m_pAxisType[i] == 2) {
            if (m_pAxis[AXIS_Y].m_pAxis_value[i] == 0) {
                strInfo.Format(_T("%.1fmm"), m_pAxis[AXIS_Y].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 0, 0, 1.0f);
            } else {
                strInfo.Format(_T("%.1f"), m_pAxis[AXIS_Y].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 1.0f, 0, 1.0f);
            }
            m_pOpenGL->m_Font.Text(m_pAxis[AXIS_Y][i] + 1.0f, 2.0f, strInfo, color, 0.75F);
        }
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisable(GL_SCISSOR_TEST);
}

void Mesh::RenderBK() {
    // 背景
    glBindVertexArray(m_iBkVAO);
    glDrawElements(GL_TRIANGLES, (int)m_pBkIndex.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 坐标
    m_pAxis[AXIS_X].Set(m_rcItem.vWidth() - 1, m_pScanMin[AXIS_X], m_pScanMax[AXIS_X]);
    m_pAxis[AXIS_Y].Set(m_rcItem.vHeight() - 1, m_pScanMin[AXIS_Y], m_pScanMax[AXIS_Y]);
    DrawAixs();
}

void Mesh::Render() {}

void Mesh::RenderFore() {
    DrawAixsText();
}

void Mesh::OnLButtonDown(UINT nFlags, ::CPoint pt) {}

void Mesh::OnLButtonUp(UINT nFlags, ::CPoint pt) {}

void Mesh::OnLButtonDClick(UINT nFlags, ::CPoint pt) {}

void Mesh::OnMouseMove(UINT nFlags, ::CPoint pt) {}

void Mesh::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {}

bool Mesh::IsInArea(::CPoint pt) const {
    if (pt.x > m_rcItem.left && pt.x < m_rcItem.right && pt.y < m_rcItem.bottom && pt.y > m_rcItem.top) {
        return true;
    }
    return false;
}
