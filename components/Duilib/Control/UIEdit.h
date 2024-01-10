#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

#include <regex>
#include <string>
#include <utility>
#include <iostream>
#include <sstream>

namespace DuiLib
{
	class CEditWnd;

	class UILIB_API CEditUI : public CLabelUI
	{
		friend class CEditWnd;
	public:
		CEditUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		void SetTextExt(LPCTSTR pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(TCHAR cPasswordChar);
		TCHAR GetPasswordChar() const;
		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;
		int GetWindowStyls() const;

		void SetNativeEditBkColor(DWORD dwBkColor);
		DWORD GetNativeEditBkColor() const;
		void SetNativeEditTextColor( LPCTSTR pStrColor );
		DWORD GetNativeEditTextColor() const;

		void SetSel(long nStartChar, long nEndChar);
        std::pair<uint16_t, uint16_t> GetSel();
		void SetSelAll();
		void SetReplaceSel(LPCTSTR lpszReplace);

		void SetTipValue(LPCTSTR pStrTipValue);
		LPCTSTR GetTipValue();
		void SetTipValueColor(LPCTSTR pStrColor);
		DWORD GetTipValueColor();

		void SetPos(RECT rc);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

		void SetTextValitor(const std::wregex& reg, bool enable = true);
        void SetTextValitor(const std::wstring_view& reg_str, bool enable = true);
        void DisableTextValitor() noexcept;

		void SetNumberModeEnable(bool enable = true) noexcept;
        void SetWheelStep(double step) noexcept;
        void SetNumberLimits(double min, double max) noexcept;

	protected:
		CEditWnd* m_pWindow;

		UINT m_uMaxChar;
		bool m_bReadOnly;
		bool m_bPasswordMode;
		TCHAR m_cPasswordChar;
		UINT m_uButtonState;
		CDuiString m_sTipValue;
		DWORD m_dwTipValueColor;
		DWORD m_dwEditbkColor;
		DWORD m_dwEditTextColor;
		int m_iWindowStyls;
		CDuiString m_sTextExt;
        double                       m_numberWheelStep = 1.0;
        bool                         m_attrNumberExt   = false;
        std::pair<double, double>    m_numterLimits    = {-HUGE_VAL, HUGE_VAL};
        std::pair<bool, std::wregex> m_textValitor     = {false, {}};
	};
}
#endif // __UIEDIT_H__