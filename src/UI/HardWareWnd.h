#pragma once
#include "Define.h"
#include "DuiWindowBase.h"

class HardWareWnd : public CDuiWindowBase {
public:
    HardWareWnd();
    virtual ~HardWareWnd();

    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() noexcept override;
    virtual void       InitWindow() override;
    virtual void       Notify(TNotifyUI& msg) override;
private:
};
