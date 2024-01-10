#pragma once

#include "Resource.h"

// 引入Duilib界面库
#include "UIlib.h"
using namespace DuiLib;
#pragma comment(lib, "DuiLib.lib")

class CDuiWindowBase : public WindowImplBase {
public:
    CDuiWindowBase(void);
    virtual ~CDuiWindowBase(void);

public:
    virtual UILIB_RESOURCETYPE GetResourceType() const {
        return UILIB_ZIPRESOURCE;
    }
    virtual CDuiString GetZIPFileName() const {
        return _T("Theme.zip");
    }
    LPCTSTR GetResourceID() const {
        return MAKEINTRESOURCE(IDR_ZIPRES1);
    }

    // 窗口初始化
    virtual void InitWindow();
    // 消息处理
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    // DUI消息处理
    virtual void Notify(TNotifyUI &msg);
    // 自定义Messagebox
    UINT DMessageBox(LPCTSTR lpStr, LPCTSTR lpTitle = NULL, UINT iType = MB_OK);

    virtual UINT_PTR SetTimer(UINT_PTR iIDEvent, UINT iElapse);
    virtual BOOL     KillTimer(UINT_PTR iIDEvent);
    virtual void     OnTimer(int iIdEvent);

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt);
    virtual void OnRButtonDown(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonUp(UINT nFlags, ::CPoint pt);
    virtual void OnRButtonUp(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonDClick(UINT nFlags, ::CPoint pt);
    virtual void OnMouseMove(UINT nFlags, ::CPoint pt);
    virtual void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt);

public:
    // 背景控件
    CControlUI *m_pControlTitile, *m_pControlBK;
};
