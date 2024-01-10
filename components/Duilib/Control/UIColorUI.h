#ifndef __UICOLOR_H__
#define __UICOLOR_H__

#pragma once

namespace DuiLib
{
	const int COLOR_WIDTH = 200;

	class CColorUI: public CControlUI
	{
	public:
		CColorUI(void);
		~CColorUI(void);

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface( LPCTSTR pstrName );
		virtual void PaintBkColor( HDC hDC );

		virtual void SetColor(DWORD *pBmpbits);

	public:
		DWORD  m_pBmpbits[COLOR_WIDTH];
		//»æÍ¼Í¼ÏñDC
		HDC m_hMemDC;
		HBITMAP m_hMemBmp;
	};
}


#endif //__UICOLOR_H__