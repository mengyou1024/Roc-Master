#include "SettingWnd.h"
#include "ModelAScan.h"

SettingWnd::~SettingWnd() {}

LPCTSTR SettingWnd::GetWindowClassName() const {
    return _T("SettingWnd");
}

CDuiString SettingWnd::GetSkinFile() {
    return _T(R"(Theme\UI_SettingWnd.xml)");
}

void SettingWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    auto opt          = m_PaintManager.FindControl<COptionUI*>(L"OptAutoUpgrade");
    auto systemConfig = GetSystemConfig();
    if (opt) {
        opt->Selected(systemConfig.checkUpdate);
    }
    opt = m_PaintManager.FindControl<COptionUI*>(L"OptSystemProxy");
    if (opt) {
        opt->Selected(systemConfig.enableProxy);
    }
    opt = m_PaintManager.FindControl<COptionUI*>(L"OptMeasureThickness");
    if (opt) {
        opt->Selected(systemConfig.enableMeasureThickness);
    }
    auto edit = m_PaintManager.FindControl<CEditUI*>(L"EditSystemProxy");
    if (edit) {
        edit->SetText(systemConfig.httpProxy.c_str());
    }

    opt = m_PaintManager.FindControl<COptionUI*>(L"OptUseNetwork");
    if (opt) {
        opt->Selected(systemConfig.enableNetworkTOFD);
    }

    edit = m_PaintManager.FindControl<CEditUI*>(L"EditUseNetwork");
    if (edit) {
        edit->SetText(WStringFromString(systemConfig.ipFPGA).c_str());
    }
}

void SettingWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_SELECTCHANGED) {
        if (msg.pSender->GetName() == L"OptAutoUpgrade") {
            auto systemConfig        = GetSystemConfig();
            systemConfig.checkUpdate = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        } else if (msg.pSender->GetName() == L"OptSystemProxy") {
            auto systemConfig        = GetSystemConfig();
            systemConfig.enableProxy = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        } else if (msg.pSender->GetName() == L"OptMeasureThickness") {
            auto systemConfig                   = GetSystemConfig();
            systemConfig.enableMeasureThickness = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        } else if (msg.pSender->GetName() == L"OptUseNetwork") {
            auto systemConfig              = GetSystemConfig();
            systemConfig.enableNetworkTOFD = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        }
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        if (msg.pSender->GetName() == L"EditSystemProxy") {
            auto systemConfig      = GetSystemConfig();
            systemConfig.httpProxy = std::wstring(static_cast<CEditUI*>(msg.pSender)->GetText());
            UpdateSystemConfig(systemConfig);
        }
        if (msg.pSender->GetName() == L"EditUseNetwork") {
            auto systemConfig   = GetSystemConfig();
            systemConfig.ipFPGA = StringFromWString(std::wstring(static_cast<CEditUI*>(msg.pSender)->GetText()));
            UpdateSystemConfig(systemConfig);
        }
    }
    CDuiWindowBase::Notify(msg);
}