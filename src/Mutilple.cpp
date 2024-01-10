
// Mutilple.cpp : Defines the class behaviors for the application.
//
#include "pch.h"

#include "Mutilple.h"
#include "framework.h"
#include <Thread.h>
#include <filesystem>
#include <regex>
namespace fs = std::filesystem;

#ifdef _DEBUG
// #define new DEBUG_NEW

    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>

    #include <crtdbg.h>
// #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// CMutilpleApp

BEGIN_MESSAGE_MAP(CMutilpleApp, CWinApp)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CMutilpleApp construction

CMutilpleApp::CMutilpleApp() {
    // support Restart Manager
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

// The one and only CMutilpleApp object
CString      TITLE;
CString      VERSION;
CMutilpleApp theApp;
MainProcess  g_MainProcess;

// CMutilpleApp initialization

BOOL CMutilpleApp::InitInstance() {
    TCHAR cPath[_MAX_FNAME];
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    GetModuleFileName(NULL, cPath, _MAX_FNAME);
    _tsplitpath_s(cPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
    _stprintf_s(m_pExePath, _T("%s%s"), drive, dir);
    SetCurrentDirectory(m_pExePath);

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls = {};
    InitCtrls.dwSize               = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    // Create the shell manager, in case the dialog contains
    // any shell tree view or shell list view controls.
    m_pShellManager = std::make_unique<CShellManager>();

    // Activate "Windows Native" visual manager for enabling themes in MFC controls
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Roc-Master"));

    //_CrtSetBreakAlloc(2816);
    // 版本
    TITLE   = GetProfileString(_T("Application"), _T("Title"), _T("苏州罗克莱管材探伤机"));
    VERSION = GetProfileString(_T("Application"), _T("Version"), _T(APP_VERSION));

    if (FAILED(::CoInitialize(NULL)))
        return 0;                                         // 初始化COM库
    CPaintManagerUI::SetInstance(AfxGetInstanceHandle()); // 设置渲染实例

    // 主窗口
    m_pMainFrame = std::make_unique<MainFrameWnd>();
    m_pMainFrame->Create(NULL, m_pMainFrame->GetWindowClassName(), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
    m_pMainFrame->CenterWindow();
    string filePath = {};
    {
        spdlog::info(L"CommandLine: {}", std::wstring(::GetCommandLine()));
        const static std::wregex regex(L"\"[^\"]+\"\\s+\"([^\"]+)\"");
        std::wsmatch             match;
        std::wstring             mStr = std::wstring(::GetCommandLine());
        if (std::regex_match(mStr, match, regex)) {
            spdlog::info(L"file path: \"{}\"", match[1].str());
            filePath = StringFromWString(match[1].str());
        }
    }
    m_pMainFrame->EnterReview(filePath);
    m_pMainFrame->ShowModal();

    m_pMainFrame = nullptr;

    WindowImplBase::Term();  // 释放Duilib中的静态资源
    CPaintManagerUI::Term(); // 释放Duilib中的静态资源

    // 删除上面创建的 shell 管理器。
    m_pShellManager = nullptr;

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
#endif

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
