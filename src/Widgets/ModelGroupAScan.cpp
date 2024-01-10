#include "pch.h"

#include "MeshAscan.h"
#include "ModelGroupAScan.h"
#include "OpenGL.h"

ModelGroupAScan::ModelGroupAScan(OpenGL* pOpenGL) : Model(pOpenGL), m_iBuffer(0) {}

ModelGroupAScan::~ModelGroupAScan() {
    Release();
}
void ModelGroupAScan::OnLButtonDown(UINT nFlags, ::CPoint pt) {}

void ModelGroupAScan::OnLButtonUp(UINT nFlags, ::CPoint pt) {}

void ModelGroupAScan::OnLButtonDClick(UINT nFlags, ::CPoint pt) {}

void ModelGroupAScan::OnMouseMove(UINT nFlags, ::CPoint pt) {
    printf("nFlags:%x, pt:{%d, %d}\n", nFlags, pt.x, pt.y);
}

void ModelGroupAScan::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {}

void ModelGroupAScan::Init() {
    Release();

    // `VIEW_ASCAN_NUM`个A扫
    for (size_t view = 0; view < HD_CHANNEL_NUM + 4ull; view++) {
        if (m_pMesh.count(view) == 0) {
            m_pMesh.insert(std::pair<size_t, Mesh*>(view, new MeshAscan(m_pOpenGL)));
        }
    }
    m_bSetup = false;
}

void ModelGroupAScan::SetSize(int left, int top, int right, int bottom) {
    mSetSizeRect = {left, top, right, bottom};
    // A扫宽度
    int iAscanWidth = ((right - left) - 3) / VIEW_ASCAN_COLUMNS;
    // A扫高度
    int iAscanHeight = ((bottom - top) - 6) / (VIEW_ASCAN_NUM / VIEW_ASCAN_COLUMNS);

    RECT rc{0};
    // 计算A扫显示区域
    for (int i = 0; i < VIEW_ASCAN_NUM; i++) {
        rc.left  = (i % VIEW_ASCAN_COLUMNS) * (iAscanWidth + 1);
        rc.right = rc.left + iAscanWidth;
        rc.top   = (i / VIEW_ASCAN_COLUMNS) * (iAscanHeight + 1);
        if (i / VIEW_ASCAN_COLUMNS == (VIEW_ASCAN_NUM / VIEW_ASCAN_COLUMNS - 1)) {
            rc.bottom = bottom;
        } else {
            rc.bottom = rc.top + iAscanHeight;
        }

        for (size_t offset = 0ull; offset < (HD_CHANNEL_NUM + 4ull) / VIEW_ASCAN_NUM; offset++) {
            // OpenGL视图坐标0点在左下角
            size_t iView = offset * 4 + static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) +
                           (static_cast<size_t>((VIEW_ASCAN_NUM / VIEW_ASCAN_COLUMNS - 1)) - i / VIEW_ASCAN_COLUMNS) * VIEW_ASCAN_COLUMNS +
                           (i % VIEW_ASCAN_COLUMNS);
            if (m_pMesh.count(iView) != 0) {
                m_pMesh[iView]->SetSize(rc.left, rc.top, rc.right, rc.bottom);
            }
        }
    }
    m_bSetup = false;
}

void ModelGroupAScan::Setup() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr) {
            ptr->Setup();
        }
    }
    m_bSetup = true;
}

void ModelGroupAScan::Release() {
    for (auto& ptr : m_pMesh) {
        delete ptr.second;
        ptr.second = nullptr;
    }
    m_pMesh.clear();
}

void ModelGroupAScan::RenderBK() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr && (index >= mGroupIndex * 4 && index < (static_cast<size_t>(mGroupIndex * 4) + VIEW_ASCAN_NUM))) {
            ptr->RenderBK();
        }
    }
}

void ModelGroupAScan::Render() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr && (index >= mGroupIndex * 4 && index < (static_cast<size_t>(mGroupIndex * 4) + VIEW_ASCAN_NUM))) {
            // 更新A扫数据
            ((MeshAscan*)ptr)->UpdateAScanData();
            ((MeshAscan*)ptr)->UpdateAmpMemoryData();
            ptr->Render();
        }
    }
}

void ModelGroupAScan::RenderFore() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr && (index >= mGroupIndex * 4 && index < (static_cast<size_t>(mGroupIndex * 4) + VIEW_ASCAN_NUM))) {
            ptr->RenderFore();
        }
    }
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    CString strInfo;
    for (size_t iChannel = 0; iChannel < VIEW_ASCAN_NUM; iChannel++) {
        size_t iView = static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) + iChannel;
        if (m_pMesh.count(iView) != 0) {
            strInfo.Format(_T("%lld"), ((iChannel + mGroupIndex * 4) % 12) + 1);
            float l = (((iChannel + mGroupIndex * 4) % 12) + 1) > 9 ? m_pMesh[iView]->m_rcItem.left :
                                                                              m_pMesh[iView]->m_rcItem.left + 5.0f;
            float t = m_pMesh[iView]->m_rcItem.top + 3.0f;
            m_pOpenGL->m_Font.Text(l, t, strInfo, {0.2f, 0.6f, 0.9f}, 1.0f);
        }
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
