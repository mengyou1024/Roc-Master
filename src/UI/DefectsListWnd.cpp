#include "DefectsListWnd.h"
#include "ModelAScan.h"
#include <HDBridge/TOFDPort.h>
#include <InputDialogWnd.h>
#include <Model/ScanRecord.h>
#include <regex>

using sqlite_orm::c;
using sqlite_orm::column;
using sqlite_orm::columns;
using sqlite_orm::where;

DefectsListWnd::DefectsListWnd() {}

DefectsListWnd::~DefectsListWnd() {}

LPCTSTR DefectsListWnd::GetWindowClassName() const {
    return _T("DefectsListWnd");
}

CDuiString DefectsListWnd::GetSkinFile() {
    return _T(R"(Theme\UI_DefectsListWnd.xml)");
}

void DefectsListWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    CenterWindow();
}

void DefectsListWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == _T("BtnOK")) {
            auto pList = m_PaintManager.FindControl<CListUI*>(L"ListDefects");
            if (pList->GetCurSel() < 0) {
                Close();
                CDuiWindowBase::Notify(msg);
                return;
            }
            auto pLine = static_cast<CListTextElementUI*>(pList->GetItemAt(pList->GetCurSel()));
            mResult    = std::make_tuple<bool, int, int>(true, _wtol(pLine->GetText(2)), _wtol(pLine->GetText(1)));
            Close();
        }
    }
    CDuiWindowBase::Notify(msg);
}

void DefectsListWnd::LoadDefectsList(std::string time) {
    std::regex  reg(R"((\d+)-(\d+)-(\d+)__(.+))");
    std::smatch match;
    if (std::regex_match(time, match, reg)) {
        auto year  = match[1].str();
        auto month = match[2].str();
        auto day   = match[3].str();
        auto tm    = match[4].str();
        auto path  = string(SCAN_DATA_DIR_NAME + GetJobGroup() + "/") + year + month + "/" + day;
        std::replace(path.begin(), path.end(), '/', '\\');
        path += "\\" + tm + APP_SCAN_DATA_SUFFIX;
        auto list  = ORM_Model::ScanRecord::storage(path).get_all<ORM_Model::ScanRecord>();
        auto pList = m_PaintManager.FindControl<CListUI*>(L"ListDefects");
        pList->RemoveAll();
        for (auto& it : list) {
            auto pLine = new CListTextElementUI;
            pLine->SetTag(it.id);
            pList->Add(pLine);
            pLine->SetText(0, std::to_wstring(it.id).data());
            pLine->SetText(1, std::to_wstring(it.channel).data());
            pLine->SetText(2, std::to_wstring(it.startID).data());
            pLine->SetText(3, std::to_wstring(it.endID).data());
        }
    }
}

std::tuple<bool, int, int> DefectsListWnd::getResult() {
    return mResult;
}
