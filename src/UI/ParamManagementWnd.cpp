#include "ParamManagementWnd.h"
#include "ModelAScan.h"
#include <HDBridge/TOFDPort.h>
#include <InputDialogWnd.h>

using sqlite_orm::c;
using sqlite_orm::column;
using sqlite_orm::columns;
using sqlite_orm::where;

ParamManagementWnd::ParamManagementWnd(HDBridge* bridge) : mBridge(bridge) {}

ParamManagementWnd::~ParamManagementWnd() {}

LPCTSTR ParamManagementWnd::GetWindowClassName() const {
    return _T("ParamManagementWnd");
}

CDuiString ParamManagementWnd::GetSkinFile() {
    return _T(R"(Theme\UI_ParamManagementWnd.xml)");
}

void ParamManagementWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    CenterWindow();
    LoadParam();
}

void ParamManagementWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == L"BtnNew") {
            InputDialogWnd wnd;
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.ShowModal();
            auto& [res, str] = wnd.GetResult();
            if (res) {
                try {
                    HDBridge port = {};
                    port          = *mBridge;
                    port.setName(str);
                    auto id    = HDBridge::storage().insert(port);
                    auto pList = m_PaintManager.FindControl<DuiLib::CListUI*>(_T("ListParamName"));
                    auto pLine = new CListTextElementUI;
                    pLine->SetTag(id);
                    pList->Add(pLine);
                    pLine->SetText(0, std::to_wstring(id).data());
                    pLine->SetText(1, str.data());
                } catch (std::exception& e) {
                    spdlog::error("file:{} line:{}", __FILE__, __LINE__);
                    spdlog::error(GB2312ToUtf8(e.what()));
                }
            }
        } else if (msg.pSender->GetName() == L"BtnDelete") {
            auto pList  = m_PaintManager.FindControl<DuiLib::CListUI*>(_T("ListParamName"));
            auto cursel = pList->GetCurSel();
            if (cursel >= 0) {
                auto         it   = static_cast<CListTextElementUI*>(pList->GetItemAt(cursel));
                std::wstring name = it->GetText(1);
                try {
                    HDBridge::storage().remove_all<HDBridge>(where(c(&HDBridge::getName) == name));
                } catch (std::exception& e) {
                    spdlog::error("file:{} line:{}", __FILE__, __LINE__);
                    spdlog::error(GB2312ToUtf8(e.what()));
                }
                pList->RemoveAt(cursel);
            }
        } else if (msg.pSender->GetName() == L"BtnRead") {
            auto         pList  = m_PaintManager.FindControl<DuiLib::CListUI*>(_T("ListParamName"));
            auto         cursel = pList->GetCurSel();
            auto         it     = static_cast<CListTextElementUI*>(pList->GetItemAt(cursel));
            std::wstring name   = it->GetText(1);
            try {
                auto data = HDBridge::storage().get_all<HDBridge>(where(c(&HDBridge::getName) == name));
                if (data.size() == 1) {
                    auto bridge = mBridge;
                    *bridge     = data[0];
                    bridge->syncCache2Board();
                    DMessageBox(L"读取成功!");
                }
            } catch (std::exception& e) {
                spdlog::error("file:{} line:{}", __FILE__, __LINE__);
                spdlog::error(GB2312ToUtf8(e.what()));
            }
        } else if (msg.pSender->GetName() == L"BtnWrite") {
            auto pList  = m_PaintManager.FindControl<DuiLib::CListUI*>(_T("ListParamName"));
            auto cursel = pList->GetCurSel();
            auto it     = static_cast<CListTextElementUI*>(pList->GetItemAt(cursel));
            long id     = _wtol(it->GetText(0));
            try {
                auto port = HDBridge::storage().get<HDBridge>(id);
                port      = *mBridge;
                HDBridge::storage().update(port);
                DMessageBox(L"写入成功!");
            } catch (std::exception& e) { spdlog::error(GB2312ToUtf8(e.what())); }
        }
    }
    CDuiWindowBase::Notify(msg);
}

void ParamManagementWnd::LoadParam() {
    auto list  = HDBridge::storage().get_all<HDBridge>();
    auto pList = m_PaintManager.FindControl<DuiLib::CListUI*>(_T("ListParamName"));
    pList->RemoveAll();
    for (auto& it : list) {
        auto pLine = new CListTextElementUI;
        pLine->SetTag(it.getId());
        pList->Add(pLine);
        pLine->SetText(0, std::to_wstring(it.getId()).data());
        pLine->SetText(1, it.getName().data());
    }
}
