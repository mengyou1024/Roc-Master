#include "pch.h"

#include "MeshAscan.h"
#include "OpenGL.h"

MeshAscan::MeshAscan(OpenGL* pOpenGL) :
Mesh(pOpenGL),
m_iAscanVAO(0),
m_iAscanVBO(0),
m_iDACVAO(0),
m_iDACVBO(0),
m_iMeshVAO(0),
m_iMeshVBO(0),
m_iMeshEBO(0),
m_fScanMin(0),
m_fScanMax(1),
m_iAScanSize(0),
m_Gate{0} {
    m_pAscanVertices.resize(1024);
    m_pDACVertices.resize(1024);
    for (int i = 0; i < MAX_DAC_LINES_NUM; i++) {
        m_iDACLineVAO[i] = 0;
        m_iDACLineVBO[i] = 0;
        m_pDACLineVertices[i].resize(1024);
    }
}

MeshAscan::~MeshAscan() {
    DeleteVAO(m_iAscanVAO, m_iAscanVBO);
    for (int i = 0; i < MAX_DAC_LINES_NUM; i++) {
        DeleteVAO(m_iDACLineVAO[i], m_iDACLineVBO[i]);
    }
    DeleteVAO(m_iMeshVAO, m_iMeshVBO);
    DeleteVAO(m_iDACVAO, m_iDACVBO);
}

void MeshAscan::SetLimits(float fMin, float fMax) {
    m_fScanMin = fMin;
    m_fScanMax = fMax;
}

void MeshAscan::CreateBK() {
    Mesh::CreateBK();

    m_pMeshVertices.resize(24);

    for (int i = 0; i <= 5; i++) {
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i)].x = i / 5.0F;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i)].y = 0;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i)].a = 0.7f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i)].r = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i)].g = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i)].b = 0.0f;

        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 1].x = i / 5.0F;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 1].y = 1;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 1].a = 0.7f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 1].r = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 1].g = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 1].b = 0.0f;

        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 2].x = 0;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 2].y = i / 5.0F;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 2].a = 0.7f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 2].r = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 2].g = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 2].b = 0.0f;

        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 3].x = 1;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 3].y = i / 5.0F;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 3].a = 0.7f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 3].r = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 3].g = 1.0f;
        m_pMeshVertices[4 * static_cast<std::vector<PT_V2F_C4F, std::allocator<PT_V2F_C4F>>::size_type>(i) + 3].b = 0.0f;
    }
}

void MeshAscan::Setup() {
    Mesh::Setup();

    // A Scan
    if (m_iAscanVAO == 0) {
        GenVAO(m_iAscanVAO, m_iAscanVBO);
        glBindVertexArray(m_iAscanVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iAscanVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pAscanVertices.size(), &m_pAscanVertices[0], GL_DYNAMIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // Color
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    // 母线DAC
    if (m_iDACVAO == 0) {
        GenVAO(m_iDACVAO, m_iDACVBO);
        glBindVertexArray(m_iDACVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iDACVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pDACVertices.size(), &m_pDACVertices[0], GL_DYNAMIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // Color
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    // DAC

    for (int i = 0; i < MAX_DAC_LINES_NUM; i++) {
        if (m_iDACLineVAO[i] == 0) {
            GenVAO(m_iDACLineVAO[i], m_iDACLineVBO[i]);
            glBindVertexArray(m_iDACLineVAO[i]);
            // VBO
            glBindBuffer(GL_ARRAY_BUFFER, m_iDACLineVBO[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pDACLineVertices[i].size(), &m_pDACLineVertices[i][0], GL_DYNAMIC_DRAW);
            // Position
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
            // Color
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
            glBindVertexArray(0);
        }
    }

    // 网格
    if (m_iMeshVAO == 0) {
        GenVAO(m_iMeshVAO, m_iMeshVBO);
        glBindVertexArray(m_iMeshVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iMeshVBO);
        if (m_pMeshVertices.size() > 0) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pMeshVertices.size(), &m_pMeshVertices[0], GL_STATIC_DRAW);
        }
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // Color
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    // 峰值记忆
    for (int i = 0; i < MAX_GATE_NUM; i++) {
        if (m_iAmpMemoryLineVAO[i] == 0) {
            GenVAO(m_iAmpMemoryLineVAO[i], m_iAmpMemoryLineVBO[i]);
            glBindVertexArray(m_iAmpMemoryLineVAO[i]);
            // VBO
            glBindBuffer(GL_ARRAY_BUFFER, m_iAmpMemoryLineVBO[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pAmpMemoryLineVertices[i].size(), m_pAmpMemoryLineVertices[i].data(),
                         GL_DYNAMIC_DRAW);
            // Position
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
            // Color
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
            glBindVertexArray(0);
        }
    }
}

void MeshAscan::UpdateAScanData() {
    if (mRawAScanData == nullptr) {
        return;
    }
    std::lock_guard lock(mGMutex);

    if (m_pAscanVertices.size() < mRawAScanData->size()) {
        m_pAscanVertices.resize(mRawAScanData->size());
        glBindBuffer(GL_ARRAY_BUFFER, m_iAscanVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * mRawAScanData->size(), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    for (size_t i = 0; i < mRawAScanData->size(); i++) {
        m_pAscanVertices[i].x = float(i) / float(mRawAScanData->size() - 1);
        m_pAscanVertices[i].y = (*mRawAScanData)[i] / 255.0f;
        m_pAscanVertices[i].a = 1.0f;
        m_pAscanVertices[i].r = 1.0f;
        m_pAscanVertices[i].g = 1.0f;
        m_pAscanVertices[i].b = 0.0f;
    }

    m_iAScanSize = (GLsizei)mRawAScanData->size();

    glBindBuffer(GL_ARRAY_BUFFER, m_iAscanVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PT_V2F_C4F) * m_pAscanVertices.size(), &m_pAscanVertices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshAscan::UpdateAmpMemoryData() {
    for (int i = 0; i < MAX_GATE_NUM; i++) {
        if (mAmpMemoryData[i] == nullptr) {
            continue;
        }
        std::lock_guard lock(mGMutex);

        if (m_pAmpMemoryLineVertices[i].size() < mAmpMemoryData[i]->size()) {
            m_pAmpMemoryLineVertices[i].resize(mAmpMemoryData[i]->size());
            glBindBuffer(GL_ARRAY_BUFFER, m_iAmpMemoryLineVBO[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * mAmpMemoryData[i]->size(), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        for (size_t j = 0; j < mAmpMemoryData[i]->size(); j++) {
            m_pAmpMemoryLineVertices[i][j].x = m_Gate[i].fPos + float(j) / float(mAmpMemoryData[i]->size() - 1) * m_Gate[i].fWidth;
            m_pAmpMemoryLineVertices[i][j].y = (*mAmpMemoryData[i])[j] / 255.0f;
            m_pAmpMemoryLineVertices[i][j].a = 1.0f;
            m_pAmpMemoryLineVertices[i][j].r = .8f;
            m_pAmpMemoryLineVertices[i][j].g = 1.0f;
            m_pAmpMemoryLineVertices[i][j].b = 1.0f;
        }

        m_iAmpMemoryLineSize[i] = (GLsizei)mAmpMemoryData[i]->size();

        glBindBuffer(GL_ARRAY_BUFFER, m_iAmpMemoryLineVBO[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PT_V2F_C4F) * m_pAmpMemoryLineVertices[i].size(), m_pAmpMemoryLineVertices[i].data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void MeshAscan::hookAmpMemoryData(int index, const std::shared_ptr<std::vector<uint8_t>> data) {
    std::lock_guard lock(mGMutex);
    mAmpMemoryData[index] = data;
}

void MeshAscan::hookAScanData(const std::shared_ptr<std::vector<uint8_t>> data) {
    std::lock_guard lock(mGMutex);
    mRawAScanData = data;
}

void MeshAscan::SetGateData(const std::pair<float, float>& data, int index) {
    CString str;
    str.Format(L"%c深度:%.1fmm, %c波幅:%.1f%%", 'A' + index, data.first, 'A' + index, data.second);
    m_sGateDataShow[index] = str;
}

void MeshAscan::SetGateData(int index) {
    CString str;
    str.Format(L"%c深度:无, %c波幅:无", 'A' + index, 'A' + index);
    m_sGateDataShow[index] = str;
}

void MeshAscan::SetTickness(float thickness) {
    EnableTickness(true);
    m_Tickness = thickness;
}

void MeshAscan::EnableTickness(bool en) {
    m_EnableThickness = en;
}

const std::vector<uint8_t> MeshAscan::GetAmpMemoryData(int index) const {
    if (mAmpMemoryData[index] == nullptr) {
        return {};
    }
    return *mAmpMemoryData[index];
}

void MeshAscan::ClearAmpMemoryData(int index) {
    m_iAmpMemoryLineSize[index] = 0;
}

void MeshAscan::UpdateGate(int iGate, bool bEnable, float fPos, float fWidth, float fHeight) {
    if (iGate >= MAX_GATE_NUM) {
        return;
    }
    // std::lock_guard lock(mGMutex);
    m_Gate[iGate].bEnable = bEnable;
    m_Gate[iGate].fPos    = fPos;
    m_Gate[iGate].fWidth  = fWidth;
    m_Gate[iGate].fHeight = fHeight;
}

void MeshAscan::DrawGate() {
    std::lock_guard lock(mGMutex);
    glm::vec4       color;
    float           fLineWidth = 2.0f;
    for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
        if (iGate == 0)
            color = glm::vec4(1.0f, 0, 0, 1.0f);
        else if (iGate == 2)
            color = glm::vec4(0, 1.0f, 0.0f, 1.0f);
        else
            color = glm::vec4(0, 0, 1.0f, 1.0f);

        if (m_Gate[iGate].bEnable) {
            DrawLineX(m_Gate[iGate].fPos, m_Gate[iGate].fPos + m_Gate[iGate].fWidth, m_Gate[iGate].fHeight, color, fLineWidth);
        }
    }
}

void MeshAscan::DrawAixsText() {
    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    CString   strInfo;
    glm::vec4 color(1.0f, 0, 0, 1.0f);

    // 坐标
    // x
    glScissor(m_rcItem.vleft, m_rcItem.top, m_rcItem.vWidth(), m_rcItem.iAxisWidth);
    glPushMatrix();
    glTranslatef((float)m_rcItem.vleft, (float)m_rcItem.top, 0.0F);
    for (int i = 0; i < (int)m_pAxis[AXIS_X].GetCount(); ++i) {
        if (m_pAxis[AXIS_X].m_pAxisType[i] == 2) {
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
                strInfo.Format(_T("%.1f%%"), m_pAxis[AXIS_Y].m_pAxis_value[i]);
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

    ShowGateData();
    ShowTickness();
}

void MeshAscan::ShowGateData() {
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glScissor(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glPushMatrix();
    auto           index = 2;
    constexpr auto step  = 25;
    for (auto& s : m_sGateDataShow) {
        m_pOpenGL->m_Font.Text((float)(m_rcItem.vleft + 5), (float)(m_rcItem.vtop + (index--) * step + m_rcItem.vHeight() - 70), s,
                               glm::vec4(0.6, 0.8f, 0.8f, 1.f), 1.f);
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void MeshAscan::ShowTickness() {
    if (!m_EnableThickness) {
        return;
    }
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glScissor(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glPushMatrix();
    CString str;
    str.Format(L"测厚: %.1fmm", m_Tickness);
    m_pOpenGL->m_Font.Text((float)(m_rcItem.vleft + 240), (float)(m_rcItem.vtop + m_rcItem.vHeight() - 20), str,
                           glm::vec4(0.8f, 0.8f, 1.0f, 1.0f), 1.f);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void MeshAscan::RenderBK() {
    // 背景
    glBindVertexArray(m_iBkVAO);
    glDrawElements(GL_TRIANGLES, (int)m_pBkIndex.size(), GL_UNSIGNED_INT, 0); // 三个区域 6个三角形 ，18个点组成
    glBindVertexArray(0);
    // 坐标
    m_pAxis[AXIS_X].Set(m_rcItem.vWidth() - 1, 50, m_fScanMin, m_fScanMax); // 坐标50等份
    m_pAxis[AXIS_Y].Set(m_rcItem.vHeight() - 1, 20, 100.0f, 0.0f);          // 坐标20等份
    DrawAixs();
}

void MeshAscan::RenderFore() {
    Mesh::RenderFore();
}

void MeshAscan::Render() {
    glViewport(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1.0F, -0.001F, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 网格
    glEnable(GL_BLEND);
    glLineStipple(1, 0x0101);
    glEnable(GL_LINE_STIPPLE);
    glBindVertexArray(m_iMeshVAO);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_BLEND);

    glBindVertexArray(m_iAscanVAO);
    glDrawArrays(GL_LINE_STRIP, 0, m_iAScanSize);
    glBindVertexArray(0);

    for (int i = 0; i < MAX_DAC_LINES_NUM; i++) {
        glBindVertexArray(m_iDACLineVAO[i]);
        glDrawArrays(GL_LINE_STRIP, 0, m_iDACLineSize[i]);
        glBindVertexArray(0);
    }

    for (int i = 0; i < MAX_GATE_NUM; i++) {
        glBindVertexArray(m_iAmpMemoryLineVAO[i]);
        glDrawArrays(GL_LINE_STRIP, 0, m_iAmpMemoryLineSize[i]);
        glBindVertexArray(0);
    }

    glBindVertexArray(m_iDACVAO);
    glDrawArrays(GL_LINE_STRIP, 0, m_iDACSize);
    glBindVertexArray(0);

    DrawGate();
}

int MeshAscan::GetPixel(int iAxis, float fValue) {
    int iPixel = 0;

    if (iAxis == AXIS_X) {
        iPixel = int(m_rcItem.vleft + fValue * m_rcItem.vWidth() + 0.5f);
    } else if (iAxis == AXIS_Y) {
        iPixel = int(m_rcItem.vtop + (1.0f - fValue) * m_rcItem.vHeight() + 0.5f);
    }

    return iPixel;
}

float MeshAscan::GetValue(int iAxis, int iPixel) {
    float fValue = 0;

    if (iAxis == AXIS_X) {
        fValue = float(iPixel - m_rcItem.vleft) / float(m_rcItem.vWidth());
    } else if (iAxis == AXIS_Y) {
        fValue = 1.0f + float(iPixel - m_rcItem.vtop) / float(m_rcItem.vHeight()) * -1.0f;
    }

    return fValue;
}
