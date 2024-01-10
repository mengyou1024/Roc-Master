#include "pch.h"

#include "MeshGroupCScan.h"
#include "OpenGL.h"

MeshGroupCScan::MeshGroupCScan(OpenGL* pOpenGL) :
Mesh(pOpenGL),
m_iMeshVAO(0),
m_iMeshVBO(0),
m_iMeshEBO(0),
m_iScanTexture(0),
m_Limits{0} {
    m_pCScanVertices.resize(1024);
}

MeshGroupCScan::~MeshGroupCScan() {
    DeleteTexture(m_iScanTexture);
    DeleteVAO(m_iCScanVAO, m_iCScanVBO);
}

void MeshGroupCScan::SetSize(int left, int top, int right, int bottom) {
    m_rcItem.set(left, right, top, bottom, 0);
}

void MeshGroupCScan::CreateBK() {}

void MeshGroupCScan::Setup() {
    // 用于 `DrawLineY`
    if (m_iQuadVAO == 0) {
        GLuint pIndex[] = {0, 1, 2, 1, 2, 3};
        GenVAO(m_iQuadVAO, m_iQuadVBO, m_iQuadEBO);
        glBindVertexArray(m_iQuadVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuadVertices), m_pQuadVertices, GL_STREAM_DRAW);
        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iQuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pIndex), pIndex, GL_STREAM_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // tex
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }
    if (m_iCScanVAO == 0) {
        GenVAO(m_iCScanVAO, m_iCScanVBO);
        glBindVertexArray(m_iCScanVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_iCScanVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pCScanVertices.size(), &m_pCScanVertices[0], GL_DYNAMIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // Color
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }
}

void MeshGroupCScan::DrawAixs() {
    // 不绘制坐标轴
}

void MeshGroupCScan::DrawAixsText() {
    // 不绘制坐标轴文本
}

void MeshGroupCScan::Render() {

    // 绘制线条
    glViewport(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1.0F, -0.001F, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindVertexArray(m_iCScanVAO);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)m_iCScanSize);
    glBindVertexArray(0);

    for (const auto& it : mLineX) {
        DrawLineY(((float)(it + m_rcItem.left) / (float)m_rcItem.Width()), 0, 1, {1.f, .8f, .8f, 1.f}, 1);
    }
}

void MeshGroupCScan::RenderBK() {
    // 清除区域
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.01f, 0.01f, 1.0f);
    glScissor(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void MeshGroupCScan::AppendLine(int x) {
    mLineX.clear();
    mLineX.push_back(x);
}

void MeshGroupCScan::RemoveLine() {
    mLineX.clear();
}

void MeshGroupCScan::AppendDot(uint8_t value, glm::vec4 color, size_t maxSize) {
    std::lock_guard lock(mGMutex);
    mRawCScanData.push_back({value, color});
    if (mRawCScanData.size() > maxSize) {
        mRawCScanData.pop_front();
    }
}

void MeshGroupCScan::RemoveDot() {
    std::lock_guard lock(mGMutex);
    mRawCScanData.clear();
    m_iCScanSize = 0;
}

void MeshGroupCScan::UpdateCScanData() {
    std::lock_guard lock(mGMutex);

    if (m_pCScanVertices.size() < mRawCScanData.size()) {
        m_pCScanVertices.resize(mRawCScanData.size());
        glBindBuffer(GL_ARRAY_BUFFER, m_iCScanVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * mRawCScanData.size(), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    uint32_t index = 0;
    for (auto& data : mRawCScanData) {
        m_pCScanVertices[index].x = float(index) / float(mRawCScanData.size() - 1);
        m_pCScanVertices[index].y = data.data / 255.0f;
        m_pCScanVertices[index].a = data.color.a;
        m_pCScanVertices[index].r = data.color.r;
        m_pCScanVertices[index].g = data.color.g;
        m_pCScanVertices[index].b = data.color.b;
        index++;
        m_iCScanSize = (GLsizei)index;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_iCScanVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PT_V2F_C4F) * m_pCScanVertices.size(), m_pCScanVertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
