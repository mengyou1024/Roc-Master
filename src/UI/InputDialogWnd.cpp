#include "InputDialogWnd.h"

LPCTSTR InputDialogWnd::GetWindowClassName() const {
    return _T("InputDialogWnd");
}

CDuiString InputDialogWnd::GetSkinFile() {
    return _T("Theme\\UI_InputDialogWnd.xml");
}

void InputDialogWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    CenterWindow();
}

void InputDialogWnd::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_RETURN || (msg.sType == DUI_MSGTYPE_CLICK && msg.pSender->GetName() == L"IDOK")) {
        mResult.first  = true;
        auto edit      = m_PaintManager.FindControl<CEditUI *>(L"Edit");
        mResult.second = edit->GetText();
        Close();
    }
    CDuiWindowBase::Notify(msg);
}

std::pair<bool, std::wstring> InputDialogWnd::GetResult() {
    return mResult;
}
