#include "pch.h"

#include "DMessageBox.h"

CDMessageBox::CDMessageBox(LPCTSTR lpStr, LPCTSTR lpTitle, UINT iType) {
    m_pString = lpStr;
    m_pTitle  = lpTitle;
    m_iType   = iType;
}

CDMessageBox::~CDMessageBox(void) {}

LRESULT CDMessageBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                Close(IDCANCEL);
                return 0;
            } else if (wParam == VK_RETURN) {
                if (m_iType == MB_YESNO)
                    Close(IDYES);
                else
                    Close(IDOK);
                return 0;
            }
        } break;
    }

    return CDuiWindowBase::HandleMessage(uMsg, wParam, lParam);
}

void CDMessageBox::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("IDOK")) {
            Close(IDOK);
            return;
        } else if (strName == _T("IDYES")) {
            Close(IDYES);
            return;
        }
    }

    CDuiWindowBase::Notify(msg);
}

void CDMessageBox::InitWindow() {
    CDuiWindowBase::InitWindow();

    auto m_pTabBtn = m_PaintManager.FindControl<CTabLayoutUI *>(_T("TabBtn"));

    if (m_iType == MB_YESNO)
        m_pTabBtn->SelectItem(1);
    else
        m_pTabBtn->SelectItem(0);

    auto m_pTextTitle  = m_PaintManager.FindControl<CLabelUI *>(_T("LabelTitle"));
    auto m_pTextString = m_PaintManager.FindControl<CTextUI *>(_T("LabelString"));
    m_pTextTitle->SetText(m_pTitle);
    m_pTextString->SetText(m_pString);
}
