#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"
#include <HDBridge.h>

class DefectsListWnd : public CDuiWindowBase {
public:
    DefectsListWnd();
    ~DefectsListWnd();

    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    void               InitWindow() override;
    void               Notify(TNotifyUI& msg) override;
    void               LoadDefectsList(std:: string time);
    std::tuple<bool, int, int> getResult();

private:
    std::tuple<bool, int, int> mResult = {false, 0, 0};
    HDBridge *mBridge = nullptr;
};
