#include "stdafx.h"
#include "UIColorUI.h"

namespace DuiLib
{
	CColorUI::CColorUI(void)
	{
		m_hMemDC = NULL;
		m_hMemBmp = NULL;
	}

	CColorUI::~CColorUI(void)
	{
		if( m_hMemDC != NULL )
			DeleteDC( m_hMemDC );

		if( m_hMemBmp != NULL )
			DeleteObject( m_hMemBmp );
	}

	LPCTSTR CColorUI::GetClass() const
	{
		// 这个是类名，去掉开头的是C字符
		return DUI_CTR_COLOR;
	}

	LPVOID CColorUI::GetInterface(LPCTSTR pstrName)
	{
		// 这个是xml控件的名称
		if (_tcscmp(pstrName, DUI_CTR_COLOR) == 0)
			return static_cast<CColorUI*>(this);

		return CControlUI::GetInterface(pstrName);
	}

	void CColorUI::PaintBkColor(HDC hDC)
	{
		if( m_hMemDC == NULL )
		{
			m_hMemDC = CreateCompatibleDC( hDC );
			if( m_hMemDC == NULL )
				return;

			m_hMemBmp = CreateCompatibleBitmap( hDC, COLOR_WIDTH, 1 );
			if( m_hMemBmp == NULL )
			{
				DeleteDC( m_hMemDC );
				m_hMemDC = NULL;
				return;
			}

			SelectObject( m_hMemDC, m_hMemBmp );
			SetBitmapBits( m_hMemBmp, COLOR_WIDTH * sizeof(DWORD), m_pBmpbits );
			Invalidate();
		}
		else
		{
			StretchBlt( hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, m_hMemDC, 0, 0, COLOR_WIDTH, 1, SRCCOPY );
		}
	}

	void CColorUI::SetColor(DWORD *pBmpbits)
	{
		memcpy( m_pBmpbits, pBmpbits, sizeof(DWORD)*COLOR_WIDTH );

		if( m_hMemBmp != NULL )
			SetBitmapBits( m_hMemBmp, COLOR_WIDTH * sizeof(DWORD), m_pBmpbits );

		Invalidate();
	}
}
