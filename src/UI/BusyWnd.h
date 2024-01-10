#pragma once
#include "DuiWindowBase.h"
#include <functional>
#include <thread>

class BusyWnd : public CDuiWindowBase {
public:
    BusyWnd(std::function<void(void)> _func);
    virtual ~BusyWnd();
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    virtual void InitWindow() override;

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt);
    virtual void OnRButtonDown(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonUp(UINT nFlags, ::CPoint pt);
    virtual void OnRButtonUp(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonDClick(UINT nFlags, ::CPoint pt);
    virtual void OnMouseMove(UINT nFlags, ::CPoint pt);
    virtual void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt);

private:
    std::function<void(void)> func;
};
