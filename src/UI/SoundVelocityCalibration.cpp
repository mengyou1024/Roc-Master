#include "SoundVelocityCalibration.h"
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

SoundVelocityCalibration::SoundVelocityCalibration(float val) :
mResult(false, val) {}

SoundVelocityCalibration::~SoundVelocityCalibration() {
}

LPCTSTR SoundVelocityCalibration::GetWindowClassName() const {
    return _T("SoundVelocityCalibration");
}

CDuiString SoundVelocityCalibration::GetSkinFile() {
    return _T(R"(Theme\UI_SoundVelocityCalibration.xml)");
}

void SoundVelocityCalibration::InitWindow() {
    CDuiWindowBase::InitWindow();
    CenterWindow();
    auto    edit = m_PaintManager.FindControl<CEditUI *>(_T("EditDistance"));
    CString str;
    str.Format(_T("%.2f"), mResult.second);
    edit->SetText(str);
}

void SoundVelocityCalibration::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == _T("BtnOK")) {
            auto &[res, val] = mResult;
            res              = true;
            auto edit        = m_PaintManager.FindControl<CEditUI *>(_T("EditDistance"));
            val              = _wtof(edit->GetText().GetData());
            Close();
        }
    }
    CDuiWindowBase::Notify(msg);
}

SoundVelocityCalibration::TYPE_RES SoundVelocityCalibration::GetResult() {
    return mResult;
}
