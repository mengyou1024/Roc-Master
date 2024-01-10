#ifndef __WINDOWUI_H__
#define __WINDOWUI_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CWindowUI : public CControlUI
	{
	public:
		CWindowUI(void);
		~CWindowUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		virtual void SetVisible(bool bVisible = true);

		virtual void SetPos(RECT rc);
		virtual BOOL Attach(HWND hWndNew);
		virtual void Detach();
	protected:
		HWND m_hWnd;
	};

}

#endif // __UILABEL_H__