#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"

class SoundVelocityCalibration : public CDuiWindowBase {
public:
    using TYPE_RES = std::pair<bool, float>;
    SoundVelocityCalibration(float val = 0);
    ~SoundVelocityCalibration();

    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    void               InitWindow() override;
    void               Notify(TNotifyUI& msg) override;


    TYPE_RES GetResult();

private:
    TYPE_RES mResult = {};
};
