#pragma once

#include "Mesh.h"
#include <Define.h>
#include <Windows.h>
#include <gl/glew.h>
#include <list>
#include <vector>

using std::vector;

class MeshGroupCScan : public Mesh {
public:
    int mCurrentIndex = 0;
    MeshGroupCScan(OpenGL* pOpenGL);
    virtual ~MeshGroupCScan();

    virtual void SetSize(int left, int top, int right, int bottom) override;

    virtual void CreateBK() override;
    virtual void Setup() override;
    virtual void DrawAixs() override;
    virtual void DrawAixsText() override;
    virtual void Render() override;
    virtual void RenderBK() override;

public:
    struct _ClickPt_info {
        bool  bEnable;
        float fXPos;
        float fYPos;
        float fWidth;
    } m_ClickPt;

    RECTF m_Limits;

public:
    GLuint             m_iScanTexture;
    std::vector<DWORD> m_pScanBits;

    GLuint                  m_iMeshVAO, m_iMeshVBO, m_iMeshEBO;
    std::vector<PT_V2F_C4F> m_pMeshVertices;

    std::vector<int> mLineX = {};


    void AppendLine(int x);
    void RemoveLine();
    void AppendDot(uint8_t value, glm::vec4 color = {1.0f, 0.0f, 0.0f, 1.0f}, size_t maxSize = 512);
    void RemoveDot();
    void UpdateCScanData();

private:
    struct GroupCScanData {
        uint8_t   data;
        glm::vec4 color;
    };
    std::mutex                mGMutex          = {};
    std::vector<PT_V2F_C4F>   m_pCScanVertices = {};
    GLuint                    m_iCScanVAO      = 0;
    GLuint                    m_iCScanVBO      = 0;
    std::list<GroupCScanData> mRawCScanData    = {};
    size_t                    m_iCScanSize     = 0;
};
