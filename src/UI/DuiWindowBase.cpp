#include "pch.h"

#include "DMessageBox.h"
#include "DuiWindowBase.h"
#include "Mutilple.h"
#include <iostream>

CDuiWindowBase::CDuiWindowBase(void) {}

CDuiWindowBase::~CDuiWindowBase(void) {}

void CDuiWindowBase::InitWindow() {
    WindowImplBase::InitWindow();

    m_pControlTitile = m_PaintManager.FindControl(_T("LabelTitle"));
    m_pControlBK     = m_PaintManager.FindControl(_T("VLBackground"));

    if (m_pControlTitile != NULL) {
        m_pControlTitile->SetText(TITLE);
    }

    if (m_pControlBK != NULL) {
        m_pControlBK->SetBkImage(_T("Theme\\BK\\BK.jpg"));
    }
}

LRESULT CDuiWindowBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg > WM_LBUTTONDOWN && uMsg < WM_MBUTTONDBLCLK) {
        // TRACE("%d-%d", WM_RBUTTONDOWN, uMsg);
    }
    switch (uMsg) {
        case WM_TIMER: {
            OnTimer((int)wParam);
            break;
        }
        case WM_LBUTTONDOWN: {
            OnLButtonDown(LOWORD(wParam), (::CPoint)lParam);
            break;
        }
        case WM_LBUTTONUP: {
            OnLButtonUp(LOWORD(wParam), (::CPoint)lParam);
            break;
        }
        case WM_RBUTTONDOWN: {
            OnRButtonDown(LOWORD(wParam), (::CPoint)lParam);
            break;
        }
        case WM_RBUTTONUP: {
            OnRButtonUp(LOWORD(wParam), (::CPoint)lParam);
            break;
        }
        case WM_LBUTTONDBLCLK: {
            OnLButtonDClick(LOWORD(wParam), (::CPoint)lParam);
            break;
        }
        case WM_MOUSEMOVE: {
            OnMouseMove(LOWORD(wParam), (::CPoint)lParam);
            break;
        }
        case WM_MOUSEWHEEL: {
            ::CPoint pt;
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);

            ScreenToClient(m_hWnd, &pt);
            OnMouseWheel(LOWORD(wParam), (short)HIWORD(wParam), pt);
            break;
        }
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_SPACE:
                    int test = 0;
                    break;
                    // 其它按键响应
            }
            break;
        }
    }

    return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void CDuiWindowBase::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("IDOK")) {
            Close(IDOK);
            return;
        } else if (strName == _T("IDYES")) {
            Close(IDYES);
            return;
        } else if (strName == _T("IDCANCEL")) {
            Close(IDCANCEL);
            return;
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE) {
        int test = 0;
    }
    WindowImplBase::Notify(msg);
}

UINT CDuiWindowBase::DMessageBox(LPCTSTR lpStr, LPCTSTR lpTitle /*= SYS_TITLE*/, UINT iType /*= MB_OK*/) {
    if (!lpTitle)
        lpTitle = AfxGetAppName();

    CDMessageBox Msgbox(lpStr, lpTitle, iType);
    Msgbox.Create(m_hWnd, _T("MessageBox"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    Msgbox.CenterWindow();
    UINT iRet = Msgbox.ShowModal();
    SetFocus(m_hWnd);
    return iRet;
}

UINT_PTR CDuiWindowBase::SetTimer(UINT_PTR iIDEvent, UINT iElapse) {
    return ::SetTimer(m_hWnd, iIDEvent, iElapse, NULL);
}

BOOL CDuiWindowBase::KillTimer(UINT_PTR iIDEvent) {
    return ::KillTimer(m_hWnd, iIDEvent);
}

void CDuiWindowBase::OnTimer(int iIdEvent) {}

void CDuiWindowBase::OnLButtonDown(UINT nFlags, ::CPoint pt) {}

void CDuiWindowBase::OnRButtonDown(UINT nFlags, ::CPoint pt) {}

void CDuiWindowBase::OnLButtonUp(UINT nFlags, ::CPoint pt) {}

void CDuiWindowBase::OnRButtonUp(UINT nFlags, ::CPoint pt) {}

void CDuiWindowBase::OnLButtonDClick(UINT nFlags, ::CPoint pt) {}

void CDuiWindowBase::OnMouseMove(UINT nFlags, ::CPoint pt) {}

void CDuiWindowBase::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {}
