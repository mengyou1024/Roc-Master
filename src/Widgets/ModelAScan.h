#pragma once

#include "Mesh.h"
#include "Model.h"
#include <atltypes.h>
#include <map>

class ModelAScan : public Model {
private:
    RECT   mSetSizeRect  = {};
    size_t mChannelIndex = 0;

public:
    ModelAScan(OpenGL* pOpenGL);
    virtual ~ModelAScan();

    virtual void Init() override;
    virtual void SetSize(int left, int top, int right, int bottom) override;

    virtual void Setup() override;
    virtual void Release() override;

    virtual void RenderBK() override;
    virtual void Render() override;
    virtual void RenderFore() override;

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt) override;
    virtual void OnLButtonUp(UINT nFlags, ::CPoint pt) override;
    virtual void OnLButtonDClick(UINT nFlags, ::CPoint pt) override;
    virtual void OnMouseMove(UINT nFlags, ::CPoint pt) override;
    virtual void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) override;

    void SetViewChanel(const size_t ch) {
        mChannelIndex = ch;
    }
    const size_t GetViewChanel() const {
        return mChannelIndex;
    }

public:
    int m_iBuffer;
};
