#pragma once

#include "Mesh.h"
#include <memory>
#include <thread>

class Channel;
class MeshAscan : public Mesh {
public:
    constexpr static int MAX_DAC_LINES_NUM = 2;
    MeshAscan(OpenGL *pOpenGL);
    virtual ~MeshAscan();

    virtual void SetLimits(float fMin, float fMax) override;

    virtual void CreateBK() override;
    virtual void Setup() override;

    virtual void RenderBK();
    virtual void RenderFore() override;
    virtual void Render();

    virtual int   GetPixel(int iAxis, float fValue);
    virtual float GetValue(int iAxis, int iPixel);

    void UpdateGate(int iGate, bool bEnable, float fPos, float fWidth, float fHeight);
    void UpdateAScanData();
    void UpdateAmpMemoryData();
    void hookAmpMemoryData(int index, const std::shared_ptr<std::vector<uint8_t>> data);
    void hookAScanData(const std::shared_ptr<std::vector<uint8_t>> data);
    void SetGateData(const std::pair<float, float> &data, int index);
    void SetGateData(int index);
    void SetTickness(float thickness);
    void EnableTickness(bool en = true);

    const std::vector<uint8_t> GetAmpMemoryData(int index) const;

    void ClearAmpMemoryData(int index);

private:
    void DrawGate();
    void DrawAixsText();
    void ShowGateData();
    void ShowTickness();

    std::array<CString, 3> m_sGateDataShow = {
        L"A深度:无, A波幅:无",
        L"B深度:无, B波幅:无",
        L"C深度:无, C波幅:无",
    };

    FLOAT   m_fScanMin;
    FLOAT   m_fScanMax;
    GLsizei m_iAScanSize;
    GLsizei m_iDACSize;                         // 母线DAC
    GLsizei m_iDACLineSize[MAX_DAC_LINES_NUM];  // RL SL
    GLsizei m_iAmpMemoryLineSize[MAX_GATE_NUM]; ///< 峰值记忆的大小
    struct _gate_info {
        bool  bEnable;
        float fPos;
        float fWidth;
        float fHeight;
    } m_Gate[MAX_GATE_NUM];

    bool  m_EnableThickness = false;
    float m_Tickness        = 0.0f;

    GLuint                  m_iAscanVAO, m_iAscanVBO;
    std::vector<PT_V2F_C4F> m_pAscanVertices;
    GLuint                  m_iMeshVAO, m_iMeshVBO, m_iMeshEBO;
    std::vector<PT_V2F_C4F> m_pMeshVertices;
    GLuint                  m_iDACVAO, m_iDACVBO;
    std::vector<PT_V2F_C4F> m_pDACVertices;
    GLuint                  m_iDACLineVAO[MAX_DAC_LINES_NUM], m_iDACLineVBO[MAX_DAC_LINES_NUM];
    std::vector<PT_V2F_C4F> m_pDACLineVertices[MAX_DAC_LINES_NUM];

    GLuint                  m_iAmpMemoryLineVAO[MAX_GATE_NUM]      = {};
    GLuint                  m_iAmpMemoryLineVBO[MAX_GATE_NUM]      = {};
    std::vector<PT_V2F_C4F> m_pAmpMemoryLineVertices[MAX_GATE_NUM] = {};

    std::mutex                            mGMutex                      = {};
    std::shared_ptr<std::vector<uint8_t>> mRawAScanData                = nullptr;
    std::shared_ptr<std::vector<uint8_t>> mAmpMemoryData[MAX_GATE_NUM] = {};
};
