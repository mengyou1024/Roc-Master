#include "stdafx.h"
#include "UIWindow.h"


namespace DuiLib
{
	CWindowUI::CWindowUI(void)
	{
	}

	CWindowUI::~CWindowUI(void)
	{
	}

	LPCTSTR CWindowUI::GetClass() const
	{
		return DUI_CTR_WINDOW;
	}

	LPVOID CWindowUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_WINDOW) == 0 ) return static_cast<CWindowUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CWindowUI::SetVisible(bool bVisible)
	{
		__super::SetVisible(bVisible);

		if( bVisible )
		{
			::ShowWindow(m_hWnd, SW_NORMAL);
			::SetForegroundWindow(m_hWnd);
		}
		else
		{
			::ShowWindow(m_hWnd, SW_HIDE);
		}
	}

	void CWindowUI::SetPos(RECT rc)
	{
		__super::SetPos(rc);
		::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	BOOL CWindowUI::Attach(HWND hWndNew)
	{
		if (!::IsWindow(hWndNew))
		{
			return FALSE;
		}

		m_hWnd = hWndNew;
		return TRUE;
	}

	void CWindowUI::Detach()
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
	}
}