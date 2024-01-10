#pragma once

namespace DuiLib
{
    class CClickEditUI: public CLabelUI
    {
    public:
        CClickEditUI(void);
        ~CClickEditUI(void);
    public:
        virtual LPCTSTR GetClass() const;
        virtual LPVOID GetInterface( LPCTSTR pstrName );
        UINT GetControlFlags() const;
        void DoEvent(TEventUI& event);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void ChangeText(LPCTSTR pstrText);

		const CDuiString& GetType() const noexcept { return m_strType; } 
    public:
		CDuiString m_strType;
		bool m_bFileName;
    };
}
