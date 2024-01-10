#include "stdafx.h"
#include "UIClickNum.h"

namespace DuiLib
{
	CClickNumUI::CClickNumUI(void)
	{
		m_dwBackColor = 0xFFFFFFFF;

		m_nBorderSize = 2;
		m_dwFocusBorderColor = 0xFF00FF00;

		m_fMax = 99999999.9F;
		m_fMin =-99999999.9F;

		m_fValue = 0;
		m_fStep = 1.0F;

		m_iValue = 0;
		m_iStep  = 1;

		m_iType = 1;
		m_iAutoFormat = 0;
		m_iFormat = 3;

		m_bSelectedNum = FALSE;
	}

	CClickNumUI::~CClickNumUI(void)
	{
	}

	LPCTSTR CClickNumUI::GetClass() const
    {
        // 这个是类名，去掉开头的是C字符
        return DUI_CTR_CLICKNUM;
    }

    LPVOID CClickNumUI::GetInterface(LPCTSTR pstrName)
    {
        // 这个是xml控件的名称
        if (_tcscmp(pstrName, DUI_CTR_CLICKNUM) == 0)
            return static_cast<CClickNumUI*>(this);

        return CLabelUI::GetInterface(pstrName);
    }

    UINT CClickNumUI::GetControlFlags() const
    {
        return CLabelUI::GetControlFlags();
    }

    void CClickNumUI::DoEvent(TEventUI& event)
    {
        if( event.Type == UIEVENT_BUTTONDOWN )
        {
            if( !m_bSelectedNum )
			{
				m_bSelectedNum = TRUE;
			}
			else
			{
				if( m_iType == 0 )
				{
					if( m_iStep == 1 )
						m_iStep = 10;
					else if( m_iStep == 10 )
						m_iStep = 100;
					else if( m_iStep == 100 )
						m_iStep = 1000;
					else
						m_iStep = 1;
				}
				else
				{
					if( m_fStep == 0.1F )
						m_fStep = 1.0F;
					else if( m_fStep == 1.0F )
						m_fStep = 10.0F;
					else if( m_fStep == 10.0F )
						m_fStep = 100.0F;
					else 
						m_fStep = 0.1F;
				}

				Invalidate();
			}

			return;
        }
		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) 
				m_pManager->SendNotify(this, DUI_MSGTYPE_DBCLICK);
			return;
		}
        if( event.Type == UIEVENT_MOUSEMOVE )
        {
            return;
        }
		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bSelectedNum = FALSE;
			Invalidate();
		}
		if( event.Type == UIEVENT_KEYDOWN )
		{
			switch( event.chKey ) 
			{
#if _DEBUG
			case VK_UP:
#else
			case VK_DOWN:
#endif
				{
					if( m_iType == 0 )
					{
						if( m_iValue + m_iStep < int(m_fMax) )
							m_iValue += m_iStep;
						else
							m_iValue = int(m_fMax);
					}
					else
					{
						if( m_fValue + m_fStep < m_fMax )
							m_fValue += m_fStep;
						else
							m_fValue = m_fMax;
					}

					m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
					Invalidate();
					return;
				}
				break;
#if _DEBUG
			case VK_DOWN:
#else
			case VK_UP:
#endif
				{
					if( m_iType == 0 )
					{
						if( m_iValue - m_iStep > int(m_fMin) )
							m_iValue -= m_iStep;
						else
							m_iValue = int(m_fMin);
					}
					else
					{
						if( m_fValue - m_fStep > m_fMin )
							m_fValue -= m_fStep;
						else
							m_fValue = m_fMin;
					}

					m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
					Invalidate();
					return;
				}
				break;
			case VK_RETURN:
				{
					if( m_iType == 0 )
					{
						if( m_iStep == 1 )
							m_iStep = 10;
						else if( m_iStep == 10 )
							m_iStep = 100;
						else if( m_iStep == 100 )
							m_iStep = 1000;
						else
							m_iStep = 1;
					}
					else
					{
						if( m_fStep == 0.1F )
							m_fStep = 1.0F;
						else if( m_fStep == 1.0F )
							m_fStep = 10.0F;
						else if( m_fStep == 10.0F )
							m_fStep = 100.0F;
						else 
							m_fStep = 0.1F;
					}

					Invalidate();
					return;
				}
				break;
			}
		}

        CLabelUI::DoEvent(event);
    }

    void CClickNumUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if( _tcscmp(pstrName, _T("type")) == 0 ) 
        {
			if( _tcscmp(pstrValue, _T("int")) == 0 ) 
				m_iType = 0;
			else
				m_iType = 1;
        }
		else if( _tcscmp(pstrName, _T("format")) == 0 ) 
		{
			_stscanf_s( pstrValue, _T("%d"), &m_iAutoFormat );
		}
		else if( _tcscmp(pstrName, _T("max")) == 0 ) 
		{
			_stscanf_s( pstrValue, _T("%f"), &m_fMax );
		}
		else if( _tcscmp(pstrName, _T("min")) == 0 ) 
		{
			_stscanf_s( pstrValue, _T("%f"), &m_fMin );
		}
		else if( _tcscmp(pstrName, _T("step")) == 0 ) 
		{
			_stscanf_s( pstrValue, _T("%f"), &m_fStep );
			m_iStep = INT(m_fStep + 0.5);
		}
        else 
        {
            CLabelUI::SetAttribute(pstrName, pstrValue);
        }
    }

	LPCTSTR CClickNumUI::GetNumText() const
	{
		static TCHAR pBuf[32] = {0};

		if( m_iType == 0 )
		{
			_stprintf_s( pBuf, _T("%d"), m_iValue );
		}
		else
		{				
			TCHAR pFormat[32] = {0};
			_stprintf_s( pFormat, _T("%%.%df"), m_iFormat );
			_stprintf_s( pBuf, pFormat, m_fValue );
		}

		return pBuf;
	}

	void CClickNumUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;

		_stscanf_s( pstrText, _T("%f"), &m_fValue );
		m_iValue = INT(m_fValue + 0.5);

		m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
		Invalidate();
	}

	void CClickNumUI::SetValue(int iValue)
	{
		m_iValue = iValue;

		m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
		Invalidate();
	}

	void CClickNumUI::SetValue(FLOAT fValue)
	{
		m_fValue = fValue;

		m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
		Invalidate();
	}

	void CClickNumUI::SetValueNoMsg(int iValue)
	{
		m_iValue = iValue;
		Invalidate();
	}

	void CClickNumUI::SetValueNoMsg(FLOAT fValue)
	{
		m_fValue = fValue;

		Invalidate();
	}

    void CClickNumUI::ChangeText(LPCTSTR pstrText)
    {
        SetText(pstrText);
    }

	void CClickNumUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		rc.left  += 2;
		rc.right -= 2;

		if( m_iType == 0 )
			PaintIntText(hDC, rc);
		else
			PaintFloatText(hDC, rc);
	}

	void CClickNumUI::PaintIntText(HDC hDC, RECT rc)
	{
		TCHAR pBuffer[32] = {0};
		_stprintf_s( pBuffer, _T("%d"), m_iValue );

		int nLinks = 0;
		if( IsEnabled() ) 
		{
			CRenderEngine::DrawText(hDC, m_pManager, rc, pBuffer, m_dwTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else 
		{
			CRenderEngine::DrawText(hDC, m_pManager, rc, pBuffer, m_dwDisabledTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
		}

		_stprintf_s( pBuffer, _T("±%d"), m_iStep );

		UINT iTextStyle = DT_SINGLELINE | DT_VCENTER | DT_RIGHT;
		CRenderEngine::DrawText(hDC, m_pManager, rc, pBuffer, m_dwDisabledTextColor, m_iFont, DT_SINGLELINE | iTextStyle);
	}

	void CClickNumUI::PaintFloatText(HDC hDC, RECT rc)
	{
		m_iFormat = m_iAutoFormat;
		if( m_fValue != 0 )
		{
			if( m_iFormat == 0 )
			{	
				m_iFormat = 3;

				INT   iTempValue = int(m_fValue*1000 + m_fValue/abs(m_fValue)*0.5);
				while( iTempValue % 10 == 0 )
				{
					iTempValue = iTempValue / 10;
					m_iFormat--;

					if( m_iFormat <= 0 )
						break;
				}
			}
		}
		else
		{
			m_iFormat = 0;
		}

		TCHAR pBuffer[32] = {0};
		TCHAR pFormat[32] = {0};
		_stprintf_s( pFormat, _T("%%.%df"), m_iFormat );
		_stprintf_s( pBuffer, pFormat, m_fValue );

		int nLinks = 0;
		if( IsEnabled() ) 
		{
			CRenderEngine::DrawText(hDC, m_pManager, rc, pBuffer, m_dwTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else 
		{
			CRenderEngine::DrawText(hDC, m_pManager, rc, pBuffer, m_dwDisabledTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
		}

		if( m_fStep < 1 )
			_stprintf_s( pBuffer, _T("±%.1f"), m_fStep );
		else
			_stprintf_s( pBuffer, _T("±%.0f"), m_fStep );

		UINT iTextStyle = DT_SINGLELINE | DT_VCENTER | DT_RIGHT;
		CRenderEngine::DrawText(hDC, m_pManager, rc, pBuffer, m_dwDisabledTextColor, m_iFont, DT_SINGLELINE | iTextStyle);
	}

	void CClickNumUI::PaintBorder(HDC hDC)
	{
		if( IsFocused() )
			CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, GetAdjustColor(m_dwFocusBorderColor));
	}
}
