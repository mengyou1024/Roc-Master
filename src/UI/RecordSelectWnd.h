#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"

class RecordSelectWnd : public CDuiWindowBase {
public:
    using TYPE_RES = std::pair<bool, std::string>;
    ~RecordSelectWnd();

    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    void               InitWindow() override;
    void               Notify(TNotifyUI& msg) override;

    void OnNotifyUnique(TNotifyUI& msg);

    TYPE_RES GetResult();
    void     LoadRecordUnique() const;
    void     ListYearMonth() const;
    void     ListDay() const;
    void     ListTime() const;

private:
    TYPE_RES mResult = {};
};
