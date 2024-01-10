#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"
#include <HDBridge.h>
#include <HDBridge/Utils.h>

class ChannelSettingWnd : public CDuiWindowBase {
public:
    ChannelSettingWnd(std::unique_ptr<HD_Utils> utils, int channel);
    ~ChannelSettingWnd();

    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    void               InitWindow() override;
    void               Notify(TNotifyUI& msg) override;

    std::unique_ptr<HD_Utils>&& returnHDUtils();

private:
    const inline static std::map<std::wstring_view, std::wstring_view> inputValitor = {
        {_T("EditSoundVelocity"), _T(R"()")},
    };
    CWindowUI*                m_pWndOpenGL = nullptr;
    std::unique_ptr<OpenGL>   m_OpenGL     = nullptr;
    std::unique_ptr<HD_Utils> mUtils;
    int                       mChannel = 0;
    void                      ReadValue2UI();
    void                      UpdateAScanCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller);
};
