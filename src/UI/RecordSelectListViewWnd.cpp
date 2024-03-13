#include "RecordSelectListViewWnd.h"
#include "ModelAScan.h"
#include <BusyWnd.h>
#include <HDBridge/Utils.h>
#include <Model/ScanRecord.h>
#include <filesystem>
#include <iostream>
#include <regex>

using std::filesystem::directory_iterator;
using std::filesystem::file_type;
using std::filesystem::path;
namespace fs = std::filesystem;

using sqlite_orm::c;
using sqlite_orm::column;
using sqlite_orm::columns;
using sqlite_orm::where;

RecordSelectListViewWnd::~RecordSelectListViewWnd() {
    try {
        auto pListYearMonth      = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
        auto pListDay            = m_PaintManager.FindControl<CListUI*>(L"ListDay");
        auto pListTime           = m_PaintManager.FindControl<CListUI*>(L"ListTime");
        auto systemConfig        = GetSystemConfig();
        systemConfig.IDYearMonth = pListYearMonth->GetCurSel();
        systemConfig.IDDay       = pListDay->GetCurSel();
        systemConfig.IDTime      = pListTime->GetCurSel();
        UpdateSystemConfig(systemConfig);
    } catch (std::exception& e) { spdlog::error(GB2312ToUtf8(e.what())); }
}

LPCTSTR RecordSelectListViewWnd::GetWindowClassName() const {
    return _T("RecordSelectListViewWnd");
}

CDuiString RecordSelectListViewWnd::GetSkinFile() {
    return _T(R"(Theme\UI_RecordSelectListViewWnd.xml)");
}

void RecordSelectListViewWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    CenterWindow();
    LoadRecordUnique();
}

void RecordSelectListViewWnd::Notify(TNotifyUI& msg) {
    OnNotifyUnique(msg);
    CDuiWindowBase::Notify(msg);
}

void RecordSelectListViewWnd::OnNotifyUnique(TNotifyUI& msg) {
    spdlog::debug(L"ID:{} msg:{}", std::wstring(msg.pSender->GetName().GetData()), std::wstring(msg.sType.GetData()));
    if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        if (msg.pSender->GetName() == L"ListYearMonth") {
            auto pListDay = m_PaintManager.FindControl<CListUI*>(L"ListDay");
            pListDay->RemoveAll();
            ListDay();
            try {
                auto pListYearMonth      = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
                auto pListDay            = m_PaintManager.FindControl<CListUI*>(L"ListDay");
                auto pListTime           = m_PaintManager.FindControl<CListUI*>(L"ListTime");
                auto systemConfig        = GetSystemConfig();
                systemConfig.IDYearMonth = pListYearMonth->GetCurSel();
                systemConfig.IDDay       = 0;
                systemConfig.IDTime      = 0;
                UpdateSystemConfig(systemConfig);
            } catch (std::exception& e) { spdlog::error(GB2312ToUtf8(e.what())); }
        } else if (msg.pSender->GetName() == L"ListDay") {
            auto pListTime = m_PaintManager.FindControl<CListUI*>(L"ListTime");
            pListTime->RemoveAll();
            ListTime();
            try {
                auto pListYearMonth      = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
                auto pListDay            = m_PaintManager.FindControl<CListUI*>(L"ListDay");
                auto pListTime           = m_PaintManager.FindControl<CListUI*>(L"ListTime");
                auto systemConfig        = GetSystemConfig();
                systemConfig.IDYearMonth = pListYearMonth->GetCurSel();
                systemConfig.IDDay       = pListDay->GetCurSel();
                systemConfig.IDTime      = 0;
                UpdateSystemConfig(systemConfig);
            } catch (std::exception& e) { spdlog::error(GB2312ToUtf8(e.what())); }
        } else if (msg.pSender->GetName() == L"ListTime") {
            try {
                auto pListYearMonth      = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
                auto pListDay            = m_PaintManager.FindControl<CListUI*>(L"ListDay");
                auto pListTime           = m_PaintManager.FindControl<CListUI*>(L"ListTime");
                auto systemConfig        = GetSystemConfig();
                systemConfig.IDYearMonth = pListYearMonth->GetCurSel();
                systemConfig.IDDay       = pListDay->GetCurSel();
                systemConfig.IDTime      = pListTime->GetCurSel();
                UpdateSystemConfig(systemConfig);
            } catch (std::exception& e) { spdlog::error(GB2312ToUtf8(e.what())); }
        }
    } else if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == _T("closebtn")) {
            auto& [ret, str] = mResult;
            ret              = false;
            str              = "";
        } else if (msg.pSender->GetName() == _T("BtnOK")) {
            auto pListYearMonth = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
            auto pListDay       = m_PaintManager.FindControl<CListUI*>(L"ListDay");
            auto pListTime      = m_PaintManager.FindControl<CListUI*>(L"ListTime");
            if (pListYearMonth->GetItemAt(pListYearMonth->GetCurSel()) == nullptr ||
                pListDay->GetItemAt(pListDay->GetCurSel()) == nullptr ||
                pListTime->GetItemAt(pListTime->GetCurSel()) == nullptr) {
                auto& [ret, str] = mResult;
                ret              = false;
                str              = "";
                Close();
                return;
            }

            auto listYearMonthValue = ((CListLabelElementUI*)pListYearMonth->GetItemAt(pListYearMonth->GetCurSel()))->GetText();
            auto listDayValue       = ((CListLabelElementUI*)pListDay->GetItemAt(pListDay->GetCurSel()))->GetText();
            auto listTimeValue      = ((CListTextElementUI*)pListTime->GetItemAt(pListTime->GetCurSel()))->GetText(0);
            if (std::wstring(listTimeValue).empty() || listDayValue.IsEmpty() || listYearMonthValue.IsEmpty()) {
                auto& [ret, str] = mResult;
                ret              = false;
                str              = "";
                Close();
                return;
            }
            auto& [ret, str] = mResult;
            ret              = true;
            str              = StringFromWString(_T(SCAN_DATA_DIR_NAME) + GetSystemConfig().groupName + L"/" +
                                                 std::wstring(listYearMonthValue.GetData()) + L"/" + std::wstring(listDayValue.GetData()) +
                                                 L"/" + std::wstring(listTimeValue) + _T(APP_SCAN_DATA_SUFFIX));
            Close();
        } else if (msg.pSender->GetName() == _T("BtnDEL")) {
            auto                      pListYearMonth     = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
            auto                      pListDay           = m_PaintManager.FindControl<CListUI*>(L"ListDay");
            auto                      pListTime          = m_PaintManager.FindControl<CListUI*>(L"ListTime");
            std::optional<CDuiString> listYearMonthValue = {};
            std::optional<CDuiString> listDayValue       = {};
            std::optional<CDuiString> listTimeValue      = {};
            if (pListYearMonth->GetItemAt(pListYearMonth->GetCurSel())) {
                listYearMonthValue = ((CListLabelElementUI*)pListYearMonth->GetItemAt(pListYearMonth->GetCurSel()))->GetText();
            }
            if (pListDay->GetItemAt(pListDay->GetCurSel())) {
                listDayValue = ((CListLabelElementUI*)pListDay->GetItemAt(pListDay->GetCurSel()))->GetText();
            }
            if (pListTime->GetItemAt(pListTime->GetCurSel())) {
                listTimeValue = ((CListTextElementUI*)pListTime->GetItemAt(pListTime->GetCurSel()))->GetText(0);
                listTimeValue = listTimeValue.value() + _T(APP_SCAN_DATA_SUFFIX);
            }
            std::optional<fs::path> basePath      = std::wstring(L"./") + _T(SCAN_DATA_DIR_NAME) + GetSystemConfig().groupName;
            std::optional           yearMonthPath = basePath.value() / std::wstring(listYearMonthValue.value_or(L""));
            std::optional           dayPath       = yearMonthPath.value() / std::wstring(listDayValue.value_or(L""));
            std::optional           timePath      = dayPath.value() / std::wstring(listTimeValue.value_or(L""));
            try {
                if (msg.pSender->GetUserData() == L"Day") {
                    fs::remove_all(dayPath.value());
                } else if (msg.pSender->GetUserData() == L"Month") {
                    fs::remove_all(yearMonthPath.value());
                } else if (msg.pSender->GetUserData() == L"All") {
                    fs::remove_all(basePath.value());
                } else {
                    fs::remove(timePath.value());
                }
                pListTime->RemoveAt(pListTime->GetCurSel());
                if (pListTime->GetCount() == 0) {
                    fs::remove(dayPath.value());
                    pListDay->RemoveAt(pListDay->GetCurSel());
                    if (pListDay->GetCount() == 0) {
                        fs::remove(yearMonthPath.value());
                        pListYearMonth->RemoveAt(pListYearMonth->GetCurSel());
                    }
                }
            } catch (std::exception& e) {
                try {
                    spdlog::warn(GB2312ToUtf8(e.what()));
                    DMessageBox(WStringFromString(string(e.what())).data());
                } catch (...) {}
            }

            auto YearMonth = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
            ListYearMonth();
        }
    }
}

RecordSelectListViewWnd::TYPE_RES RecordSelectListViewWnd::GetResult() {
    return mResult;
}

void RecordSelectListViewWnd::LoadRecordUnique() const {
    ListYearMonth();
}

void RecordSelectListViewWnd::ListYearMonth() const {
    try {
        auto pListYearMonth = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
        pListYearMonth->RemoveAll();
        std::wstring dirName = WStringFromString(string("./") + SCAN_DATA_DIR_NAME + GetJobGroup());
        for (auto& v : directory_iterator(dirName)) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::directory) {
                auto fileName = v.path().filename().string();
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pListYearMonth->Add(list);
            }
        }
        if (GetSystemConfig().IDYearMonth >= 0 && pListYearMonth->GetCount() > GetSystemConfig().IDYearMonth) {
            auto it = static_cast<CListLabelElementUI*>(pListYearMonth->GetItemAt(GetSystemConfig().IDYearMonth));
            it->Select();
        } else if (pListYearMonth->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListYearMonth->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(GB2312ToUtf8(e.what()));
    }
}

void RecordSelectListViewWnd::ListDay() const {
    try {
        auto         pList  = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
        std::wstring parent = ((CListLabelElementUI*)pList->GetItemAt(pList->GetCurSel()))->GetText();
        if (parent == L"") {
            return;
        }
        std::wstring dirName  = WStringFromString(string("./") + SCAN_DATA_DIR_NAME + GetJobGroup());
        auto         pListDay = m_PaintManager.FindControl<CListUI*>(L"ListDay");
        pListDay->RemoveAll();
        auto pListTime = m_PaintManager.FindControl<CListUI*>(L"ListTime");
        pListTime->RemoveAll();
        for (auto& v : directory_iterator(dirName + L"/" + std::wstring(parent))) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::directory) {
                auto fileName = v.path().filename().string();
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pListDay->Add(list);
            }
        }
        if (GetSystemConfig().IDDay >= 0 && pListDay->GetCount() > GetSystemConfig().IDDay) {
            auto it = static_cast<CListLabelElementUI*>(pListDay->GetItemAt(GetSystemConfig().IDDay));
            it->Select();
        } else if (pListDay->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListDay->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(GB2312ToUtf8(e.what()));
    }
}

void RecordSelectListViewWnd::ListTime() const {
    try {
        auto pListYearMonth     = m_PaintManager.FindControl<CListUI*>(L"ListYearMonth");
        auto pListDay           = m_PaintManager.FindControl<CListUI*>(L"ListDay");
        auto listYearMonthValue = ((CListLabelElementUI*)pListYearMonth->GetItemAt(pListYearMonth->GetCurSel()))->GetText();
        auto listDayValue       = ((CListLabelElementUI*)pListDay->GetItemAt(pListDay->GetCurSel()))->GetText();
        if (listYearMonthValue == L"" || listDayValue == L"") {
            return;
        }
        std::wstring parent    = listYearMonthValue + L"/" + listDayValue;
        std::wstring dirName   = WStringFromString(string("./") + SCAN_DATA_DIR_NAME + GetJobGroup());
        auto         pListTime = m_PaintManager.FindControl<CListUI*>(L"ListTime");
        pListTime->RemoveAll();
        for (auto& v : directory_iterator(dirName + L"/" + std::wstring(parent))) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::regular) {
                auto fileName = v.path().filename().string();
                auto list     = new CListTextElementUI;
                pListTime->Add(list);
                list->SetText(0, WStringFromString(fileName.substr(0, 8)).data());
                // TODO: 添加缺陷数量
                auto path = StringFromWString(_T(SCAN_DATA_DIR_NAME) + GetSystemConfig().groupName + L"/" +
                                              std::wstring(listYearMonthValue.GetData()) + L"/" + std::wstring(listDayValue.GetData()) +
                                              L"/" + WStringFromString(fileName));
                auto size = ORM_Model::ScanRecord::storage(path).get_all<ORM_Model::ScanRecord>();
                list->SetText(1, std::to_wstring(size.size()).data());
            }
        }
        if (GetSystemConfig().IDTime >= 0 && pListTime->GetCount() > GetSystemConfig().IDTime) {
            auto it = static_cast<CListLabelElementUI*>(pListTime->GetItemAt(GetSystemConfig().IDTime));
            it->Select();
        } else if (pListTime->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListTime->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(GB2312ToUtf8(e.what()));
    }
}
