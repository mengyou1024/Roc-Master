#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"
#include <HDBridge.h>

class ParamManagementWnd : public CDuiWindowBase {
public:
    ParamManagementWnd(HDBridge *bridge);
    ~ParamManagementWnd();

    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    void               InitWindow() override;
    void               Notify(TNotifyUI& msg) override;
    void               LoadParam();

private:
    HDBridge *mBridge = nullptr;
};
