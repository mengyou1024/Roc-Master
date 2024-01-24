#include "pch.h"

#include "MeshGroupCScan.h"
#include "ModelGroupCScan.h"
#include "OpenGL.h"

ModelGroupCScan::ModelGroupCScan(OpenGL *pOpenGL) : Model(pOpenGL), m_iBuffer(0) {}

ModelGroupCScan::~ModelGroupCScan() {
    Release();
}

void ModelGroupCScan::Init() {
    Release();

    // `VIEW_CSCAN_NUM`个A扫
    for (size_t view = 0; view < (size_t)(HD_CHANNEL_NUM + 4ull); view++) {
        if (m_pMesh.count(view) == 0) {
            m_pMesh.insert(std::pair<size_t, Mesh *>(view, new MeshGroupCScan(m_pOpenGL)));
        }
    }
    m_bSetup = false;
}

void ModelGroupCScan::SetSize(int left, int top, int right, int bottom) {
    // 当前视口
    mCurrentViewPort.left   = left;
    mCurrentViewPort.right  = right;
    mCurrentViewPort.bottom = bottom;
    mCurrentViewPort.top    = top;
    // 偏移一个坐标轴的高度
    bottom -= 26;
    // C扫宽度
    int iViewWidth = ((right - left) - 3) / VIEW_CSCAN_COLUMNS;
    // C扫高度
    int iViewHeight = ((bottom - top) - 6) / (VIEW_CSCAN_NUM / VIEW_CSCAN_COLUMNS);

    mAxisViewPort.left   = left - 1;
    mAxisViewPort.right  = right - 4;
    mAxisViewPort.bottom = bottom - 25;
    mAxisViewPort.top    = bottom + 1;

    RECT rc{0};
    // 计算A扫显示区域
    for (int i = 0; i < VIEW_CSCAN_NUM; i++) {
        rc.left  = (i % VIEW_CSCAN_COLUMNS) * (iViewWidth + 1);
        rc.right = rc.left + iViewWidth;
        rc.top   = (i / VIEW_CSCAN_COLUMNS) * (iViewHeight + 1) + top;
        if (i / VIEW_CSCAN_COLUMNS == (VIEW_CSCAN_NUM / VIEW_CSCAN_COLUMNS - 1)) {
            rc.bottom = bottom;
        } else {
            rc.bottom = rc.top + iViewHeight;
        }

        // OpenGL视图坐标0点在左下角
        for (int offset = 0; offset < 4; offset++) {
            size_t iView = offset * 4 + static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN_0) +
                           (static_cast<size_t>((VIEW_CSCAN_NUM / VIEW_CSCAN_COLUMNS - 1)) - i / VIEW_CSCAN_COLUMNS) * VIEW_CSCAN_COLUMNS +
                           (i % VIEW_CSCAN_COLUMNS);
            if (m_pMesh.count(iView) != 0) {
                m_pMesh[iView]->SetSize(rc.left, rc.top, rc.right, rc.bottom);
            }
        }
    }
    m_bSetup = false;
}

void ModelGroupCScan::Setup() {
    for (auto it = m_pMesh.begin(); it != m_pMesh.end(); ++it) {
        if (it->second) {
            it->second->Setup();
        }
    }
    if (m_iAxisVAO == 0) {
        GenVAO(m_iAxisVAO, m_iAxisVBO);
        glBindVertexArray(m_iAxisVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_iAxisVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F), NULL, GL_DYNAMIC_DRAW);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(PT_V3F_T2F), nullptr);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid *)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    m_bSetup = true;
}

void ModelGroupCScan::Release() {
    for (auto &ptr : m_pMesh) {
        delete ptr.second;
        ptr.second = nullptr;
    }
    m_pMesh.clear();
}

void ModelGroupCScan::UpdateData() {}

void ModelGroupCScan::RenderBK() {
    for (auto &[index, ptr] : m_pMesh) {
        if (ptr && (index >= mGroupIndex * 4 && index < (static_cast<size_t>(mGroupIndex * 4) + VIEW_CSCAN_NUM))) {
            ptr->RenderBK();
        }
    }
}

void ModelGroupCScan::Render() {
    for (auto &[index, ptr] : m_pMesh) {
        if (ptr && (index >= mGroupIndex * 4 && index < (static_cast<size_t>(mGroupIndex * 4) + VIEW_CSCAN_NUM))) {
            ((MeshGroupCScan *)(ptr))->UpdateCScanData();
            ptr->Render();
        }
    }
}

void ModelGroupCScan::RenderFore() {
    for (auto &[index, ptr] : m_pMesh) {
        if (ptr && (index >= mGroupIndex * 4 && index < (static_cast<size_t>(mGroupIndex * 4) + VIEW_CSCAN_NUM))) {
            ptr->RenderFore();
        }
    }
    glViewport(mAxisViewPort.left, mAxisViewPort.top, std::abs(mAxisViewPort.right - mAxisViewPort.left),
               std::abs(mAxisViewPort.top - mAxisViewPort.bottom));
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glScissor(mAxisViewPort.left, mAxisViewPort.top, std::abs(mAxisViewPort.right - mAxisViewPort.left),
              std::abs(mAxisViewPort.top - mAxisViewPort.bottom));
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void ModelGroupCScan::OnLButtonDown(UINT nFlags, ::CPoint pt) {}
