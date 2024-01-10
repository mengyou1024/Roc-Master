#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"
#include <Model/DetectInfo.h>
#include <Model/UserModel.h>

class DetectionInformationEntryWnd : public CDuiWindowBase {
public:
    DetectionInformationEntryWnd();
    ~DetectionInformationEntryWnd();

    virtual LPCTSTR              GetWindowClassName() const override;
    virtual CDuiString           GetSkinFile() override;
    void                         InitWindow() override;
    void                         Notify(TNotifyUI &msg) override;
    const ORM_Model::JobGroup   &GetJobGroup() const noexcept;
    const ORM_Model::User       &GetUser() const noexcept;
    const ORM_Model::DetectInfo &GetDetectInfo() const noexcept;
    bool                         GetResult() const noexcept;
    void                         LoadDetectInfo(const ORM_Model::DetectInfo &info, std::wstring userName = {}, std::wstring groupName = {});

private:
    inline static const std::map<std::wstring_view, std::wstring_view> mInputValitor = {
        {L"Editthickness",  LR"(\d+\.?\d{0,2})"       },
        {L"EditUserAdd",    LR"([\u4e00-\u9fa5\w\d]+)"},
        {L"EditJobGroupAdd", LR"([\u4e00-\u9fa5\w\d]+)"},
    };
    void LoadUserGroupInfo();
    bool                  mResult     = false;
    ORM_Model::JobGroup   mJobGroup   = {};
    ORM_Model::User       mUser       = {};
    ORM_Model::DetectInfo mDetectinfo = {};
};
