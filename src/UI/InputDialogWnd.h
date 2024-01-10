#pragma once
#include "DuiWindowBase.h"
#include <functional>
#include <string>

class InputDialogWnd : public CDuiWindowBase {
public:
    InputDialogWnd() = default;

    virtual ~InputDialogWnd() = default;

public: 
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    virtual void InitWindow() override;
    virtual void Notify(TNotifyUI &msg);

    std::pair<bool, std::wstring> GetResult();

private:
    std::pair<bool, std::wstring> mResult = std::make_pair<bool, std::wstring>(false, L"");
};
