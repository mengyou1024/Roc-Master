#include "pch.h"

#include "MeshAscan.h"
#include "ModelAScan.h"
#include "OpenGL.h"

ModelAScan::ModelAScan(OpenGL* pOpenGL) : Model(pOpenGL), m_iBuffer(0) {}

ModelAScan::~ModelAScan() {
    Release();
}
void ModelAScan::OnLButtonDown(UINT nFlags, ::CPoint pt) {}

void ModelAScan::OnLButtonUp(UINT nFlags, ::CPoint pt) {}

void ModelAScan::OnLButtonDClick(UINT nFlags, ::CPoint pt) {}

void ModelAScan::OnMouseMove(UINT nFlags, ::CPoint pt) {
    printf("nFlags:%x, pt:{%d, %d}\n", nFlags, pt.x, pt.y);
}

void ModelAScan::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {}

void ModelAScan::Init() {
    Release();
    m_pMesh.insert(std::pair<size_t, Mesh*>(0, new MeshAscan(m_pOpenGL)));
    m_bSetup = false;
}

void ModelAScan::SetSize(int left, int top, int right, int bottom) {
    mSetSizeRect = {left, top, right, bottom};
    // A扫宽度
    int iAscanWidth = ((right - left) - 3);
    // A扫高度
    int iAscanHeight = ((bottom - top) - 6);

    for (auto& [_, ptr] : m_pMesh) {
        if (ptr) {
            ptr->SetSize(left, top, right, bottom);
        }
    }
    m_bSetup = false;
}

void ModelAScan::Setup() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr) {
            ptr->Setup();
        }
    }
    m_bSetup = true;
}

void ModelAScan::Release() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr)
        delete ptr;
        ptr = nullptr;
    }
    m_pMesh.clear();
}

void ModelAScan::RenderBK() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr) {
            ptr->RenderBK();
        }
    }
}

void ModelAScan::Render() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr) {
            // 更新A扫数据
            ((MeshAscan*)ptr)->UpdateAScanData();
            ((MeshAscan*)ptr)->UpdateAmpMemoryData();
            ptr->Render();
        }
    }
}

void ModelAScan::RenderFore() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr) {
            ptr->RenderFore();
        }
    }
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    CString strInfo;
    strInfo.Format(_T("%lld"), mChannelIndex + 1);
    float l = (mChannelIndex + 1) > 9 ? m_pMesh[0]->m_rcItem.left : m_pMesh[0]->m_rcItem.left + 5.0f;
    float t = m_pMesh[0]->m_rcItem.top + 3.0f;
    m_pOpenGL->m_Font.Text(l, t, strInfo, {0.2f, 0.6f, 0.9f}, 1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
