
// Mutilple.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'pch.h' before including this file for PCH"
#endif

#include "MainFrameWnd.h"
#include "MainProcess.h"
#include "resource.h" // main symbols

// CMutilpleApp:
// See Mutilple.cpp for the implementation of this class
//

class CMutilpleApp : public CWinApp {
public:
    CMutilpleApp();

    // Overrides
public:
    virtual BOOL InitInstance();

    // Implementation

    DECLARE_MESSAGE_MAP()

public:
    std::unique_ptr<MainFrameWnd>  m_pMainFrame           = nullptr;
    TCHAR                          m_pExePath[_MAX_FNAME] = {};
    std::unique_ptr<CShellManager> m_pShellManager        = nullptr;
};

extern CMutilpleApp theApp;
extern MainProcess  g_MainProcess;
extern CString      TITLE;
extern CString      VERSION;