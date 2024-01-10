#include "stdafx.h"
#include "UIClickEdit.h"

namespace DuiLib
{
    CClickEditUI::CClickEditUI(void)
    {
        m_dwBackColor = 0xFFFFFFFF;
		m_bFileName = false;
		m_strType = _T("Char");
    }

    CClickEditUI::~CClickEditUI(void)
    {
    }

    LPCTSTR CClickEditUI::GetClass() const
    {
        return DUI_CTR_CLICKEDIT;
    }

    LPVOID CClickEditUI::GetInterface(LPCTSTR pstrName)
    {
        // 这个是xml控件的名称
        if (_tcscmp(pstrName, DUI_CTR_CLICKEDIT) == 0)
            return static_cast<CClickEditUI*>(this);

        return CLabelUI::GetInterface(pstrName);
    }

    UINT CClickEditUI::GetControlFlags() const
    {
        return CLabelUI::GetControlFlags();
    }

    void CClickEditUI::DoEvent(TEventUI& event)
    {
        if( event.Type == UIEVENT_BUTTONDOWN )
        {
            if( IsEnabled() ) 
                m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
        }
        if( event.Type == UIEVENT_MOUSEMOVE )
        {
            return;
        }
        CLabelUI::DoEvent(event);
    }

    void CClickEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if( _tcscmp(pstrName, _T("filename")) == 0 ) 
		{
			if( _tcscmp(pstrValue, _T("true")) == 0 ) 
				m_bFileName = true;
			else
				m_bFileName = false;
		}
		else  if( _tcscmp(pstrName, _T("type")) == 0 ) 
		{
			m_strType = pstrValue;
		}
		else
        {
            CLabelUI::SetAttribute(pstrName, pstrValue);
        }
    }

    void CClickEditUI::ChangeText(LPCTSTR pstrText)
    {
        CLabelUI::SetText(pstrText);
        m_pManager->SendNotify(this, DUI_MSGTYPE_TEXTCHANGED);
    }
}