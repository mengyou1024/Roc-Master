#include "pch.h"

#include "DMessageBox.h"
#include "ModelGroupAScan.h"
#include "ModelGroupCScan.h"
#include "Mutilple.h"
#include "OpenGL.h"

// TODO: 暂时屏蔽警告
#pragma warning(disable :4267 4244 4552 4305 4101)

constexpr auto TIMER_OPENGL_RENDER = 0x00;
#define OpenGL_PTR          _T("OpenGL")

OpenGL::OpenGL(void) : m_hWnd(nullptr), m_hDC(nullptr), m_hRC(nullptr), m_iWidth(1028), m_iHeight(502), m_iColorTexture(0) {
    m_pColor = new DWORD[COLOR_TEX_WIDTH * COLOR_TEX_HEIGHT];
}

OpenGL::~OpenGL(void) {
    Release();

    delete[] m_pColor;
    KillTimer(m_hWnd, TIMER_OPENGL_RENDER);
}

bool OpenGL::Create(HWND hParent) {
    CString strWindow;
    strWindow.Format(_T("OpenglWindow_%lld"), GetTickCount64());
    WNDCLASSEX wc = {sizeof(WNDCLASSEX),
                     CS_CLASSDC | CS_DBLCLKS,
                     __WndProc,
                     0L,
                     0L,
                     NULL,
                     NULL,
                     LoadCursor(NULL, IDC_ARROW),
                     NULL,
                     NULL,
                     strWindow,
                     NULL};
    RegisterClassEx(&wc);

    long lStyle = WS_VISIBLE | WS_CHILD | WS_DISABLED;
    m_hWnd      = ::CreateWindow(strWindow, strWindow, lStyle, 0, 0, m_iWidth, m_iHeight, hParent, NULL, wc.hInstance, NULL);
    if (m_hWnd == nullptr)
        return false;
    m_hDC = GetDC(m_hWnd);
    if (m_hDC == nullptr)
        return false;

    ::SetProp(m_hWnd, OpenGL_PTR, (HANDLE)this);

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
        1,                             // version number
        PFD_DRAW_TO_WINDOW |           // support window
            PFD_SUPPORT_OPENGL |       // support OpenGL
            PFD_DOUBLEBUFFER,          // double buffered
        PFD_TYPE_RGBA,                 // RGBA type
        24,                            // 24-bit color depth
        0,
        0,
        0,
        0,
        0,
        0, // color bits ignored
        0, // no alpha buffer
        0, // shift bit ignored
        0, // no accumulation buffer
        0,
        0,
        0,
        0,              // accum bits ignored
        16,             // 16-bit z-buffer
        0,              // no stencil buffer
        0,              // no auxiliary buffer
        PFD_MAIN_PLANE, // main layer
        0,              // reserved
        0,
        0,
        0 // layer masks ignored
    };

    int iPixelFormat = ::ChoosePixelFormat(m_hDC, &pfd);
    if (::SetPixelFormat(m_hDC, iPixelFormat, &pfd) == FALSE)
        return false;

    m_hRC = ::wglCreateContext(m_hDC);
    if (m_hRC == nullptr)
        return false;

    SupportOpenGL();

    Init();

    return true;
}

bool OpenGL::SupportOpenGL() {
    wglMakeCurrent(m_hDC, m_hRC);
    const GLubyte* OpenGLVersion = glGetString(GL_VERSION);

    return true;
}

bool OpenGL::Attach(CWindowUI* pWindow) {
    if (!m_hWnd)
        return false;
    return pWindow->Attach(m_hWnd);
}

void OpenGL::Init() {
    wglMakeCurrent(m_hDC, m_hRC);

    // Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // 加载字体
    m_Font.Create(IDR_MYFONT1, 20);

    SetTimer(m_hWnd, TIMER_OPENGL_RENDER, 15, NULL); // 15FPS刷新速度。

    // 初始化色彩纹理
    InitColorTable();
}

void OpenGL::InitColorTable() {
    for (int y = 0; y < COLOR_TEX_HEIGHT; y++) {
        int iIndex = y * COLOR_TEX_WIDTH;

        switch (y) {
                // 波幅
            case BK_TEX_AMP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = COLOR_TAB[1][x];
                break;
                // A扫
            case BK_TEX_RANGE:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF306090;
                break;
                // UT
            case BK_TEX_UT:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFBA55D3;
                break;
                // Step
            case BK_TEX_STEP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF6E6B3D;
                break;
                // Scan
            case BK_TEX_SCAN:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF218868;
                break;
                // 刻度背景
            case BK_TEX_AXIS_BK:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF000000;
                break;
                // 刻度
            case BK_TEX_AXIS:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFFFFFFF;
                break;
                // 零刻度
            case BK_TEX_AXIS_ZERO:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFFF0000;
                break;
                // TOFD
            case BK_TEX_TOFD_AMP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF << 24 | x << 16 | x << 8 | x;
                break;
                // 网格
            case BK_TEX_MESH:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0x7FFFFF00;
                break;
                // 默认色
            default:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0x00000000;
                break;
        }
    }

    glGenTextures(1, &m_iColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_iColorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, COLOR_TEX_WIDTH, COLOR_TEX_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_pColor);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGL::InitViewport() {
    glViewport(0, 0, m_iWidth, m_iHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_iWidth, 0, m_iHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void OpenGL::AddGroupAScanModel() {
    RemoveModel();

    Model* pModel = new ModelGroupAScan(this);
    pModel->Init();
    pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

    m_pModel.push_back(pModel);
}

void OpenGL::AddGroupCScanModel() {
    RemoveModel();

    Model* pModel = new ModelGroupCScan(this);
    pModel->Init();
    pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

    m_pModel.push_back(pModel);
}


void OpenGL::RemoveModel() {
    for (auto i = 0; i < m_pModel.size(); i++) {
        delete m_pModel[i];
        m_pModel[i] = nullptr;
    }
    m_pModel.clear();
}

void OpenGL::SetPos(int iWidth, int iHeight) {
    m_iWidth  = iWidth;
    m_iHeight = iHeight;

    for (auto i = 0; i < m_pModel.size(); i++) {
        m_pModel[i]->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);
    }
}

float OpenGL::FPS() {
    static float fps         = 0;    // 我们需要计算的FPS值
    static int   frameCount  = 0;    // 帧数
    static float currentTime = 0.0f; // 当前时间
    static float lastTime    = 0.0f; // 持续时间

    frameCount++; // 每调用一次Get_FPS()函数，帧数自增1
    currentTime =
        GetTickCount64() * 0.001f; // 获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间，所以需要乘以0.001，得到单位为秒的时间

    // 如果当前时间减去持续时间大于了1秒钟，就进行一次FPS的计算和持续时间的更新，并将帧数值清零
    if (currentTime - lastTime > 1.0f) // 将时间控制在1秒钟
    {
        fps        = (float)frameCount / (currentTime - lastTime); // 计算这1秒钟的FPS值
        lastTime   = currentTime; // 将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
        frameCount = 0;           // 将本次帧数frameCount值清零
    }

    return fps;
}

void OpenGL::Render() {
    // 设定OpenGL当前线程的渲染环境
    wglMakeCurrent(m_hDC, m_hRC);

    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        if (!m_pModel[i]->m_bSetup) {
            m_pModel[i]->Setup();
        }
    }

    // 重置视口
    InitViewport();

    // 当前缓冲区清除
    glClearColor(0.95F, 0.95F, 0.95F, 0.95F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_iColorTexture);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->RenderBK();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->Render();
    }

    InitViewport();

    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->RenderFore(); // 文字
    }

#ifdef _DEBUG
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    CString strInfo;
    if (iSize != 0) {
        // strInfo.Format(_T("FPS:%.1f"), g_MainProcess.m_HDBridge.m_fFPS);
        strInfo.Format(_T("FPS:%.0f "), FPS());
        m_Font.RightText((float)m_iWidth, (float)m_iHeight - 24, strInfo, {0.15f, 0.35f, 0.55f}, 0.75F);
    } else {
        const char* version = (const char*)glGetString(GL_VERSION);
        USES_CONVERSION;
        strInfo.Format(_T("OpenGL: %s"), A2W(version));
        m_Font.RightText((float)m_iWidth, (float)m_iHeight - 24, strInfo, {0.15f, 0.35f, 0.55f}, 1.0F);
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
#endif

    // Swap the screen buffers
    ::SwapBuffers(m_hDC);
}

void OpenGL::Release() {
    RemoveModel();

    DeleteTexture(m_iColorTexture);
}

// 按键 上下左右
void OpenGL::OnKeyUp() { // 上一圈

}
void OpenGL::OnKeyDown() { // 下一圈

}

void OpenGL::OnKeyLeft() { // 前一个点

}
void OpenGL::OnKeyRight() { // 后一个点

}

void OpenGL::OnLButtonDown(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnLButtonDown(nFlags, pt);
    }
}

void OpenGL::OnLButtonUp(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnLButtonUp(nFlags, pt);
    }
}
void OpenGL::OnRButtonDown(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        //  m_pModel[i]->OnRButtonDown(nFlags, pt);
    }
}

void OpenGL::OnRButtonUp(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        // m_pModel[i]->OnRButtonUp(nFlags, pt);
    }
}
void OpenGL::OnLButtonDClick(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnLButtonDClick(nFlags, pt);
    }
}
void OpenGL::OnRButtonDClick(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        // m_pModel[i]->OnRButtonDClick(nFlags, pt);
    }
}

void OpenGL::OnMouseMove(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnMouseMove(nFlags, pt);
    }
}

void OpenGL::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnMouseWheel(nFlags, zDelta, pt);
    }
}

void OpenGL::OnKey(TCHAR cKey) {}

LRESULT CALLBACK OpenGL::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    OpenGL* pthis = reinterpret_cast<OpenGL*>(::GetProp(hWnd, OpenGL_PTR));
    if (uMsg >= WM_LBUTTONDOWN && uMsg <= WM_MBUTTONDBLCLK) {
        TRACE("%d-%d", WM_RBUTTONDOWN, uMsg);
    }
    switch (uMsg) {
        case WM_SIZE: {
            if (pthis)
                pthis->SetPos(LOWORD(lParam), HIWORD(lParam));
        } break;
        case WM_TIMER: {
            switch (wParam) {
                case TIMER_OPENGL_RENDER: pthis->Render(); break;
            }
        } break;
        case WM_LBUTTONDOWN: {
            pthis->OnLButtonDown(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_LBUTTONUP: {
            pthis->OnLButtonUp(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_RBUTTONDOWN: {
            pthis->OnRButtonDown(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_RBUTTONUP: {
            pthis->OnRButtonUp(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_LBUTTONDBLCLK: {
            pthis->OnLButtonDClick(LOWORD(wParam), (::CPoint)lParam);
        } break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
