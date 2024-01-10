#pragma once

namespace DuiLib
{
	class CClickNumUI: public CLabelUI
	{
	public:
		CClickNumUI(void);
		~CClickNumUI(void);

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface( LPCTSTR pstrName );
		UINT GetControlFlags() const;
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void ChangeText(LPCTSTR pstrText);

		virtual LPCTSTR GetNumText() const;
		virtual void SetText(LPCTSTR pstrText);

		virtual void SetValue(int iValue);
		virtual void SetValue(FLOAT fValue);
		virtual void SetValueNoMsg(int iValue);
		virtual void SetValueNoMsg(FLOAT fValue);

		void PaintText(HDC hDC);
		void PaintIntText(HDC hDC, RECT rc);
		void PaintFloatText(HDC hDC, RECT rc);
		void PaintBorder(HDC hDC);
	public:
		BOOL m_bSelectedNum;

		//类型，0：int, 1:float
		INT   m_iType;
		//小数位数
		INT m_iAutoFormat;
		INT m_iFormat;

		FLOAT m_fMax;
		FLOAT m_fMin;
		//INT
		INT m_iValue;
		INT m_iStep;

		//FLOAT
		FLOAT m_fValue;
		FLOAT m_fStep;
	};
}