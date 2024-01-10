#include "ChannelSettingWnd.h"
#include "MeshAScan.h"
#include "ModelAScan.h"
#include <vector>

ChannelSettingWnd::ChannelSettingWnd(std::unique_ptr<HD_Utils> utils, int channel) : mUtils(std::move(utils)), mChannel(channel) {}

ChannelSettingWnd::~ChannelSettingWnd() {
    HCURSOR cursor = LoadCursorW(NULL, IDC_ARROW);
    ::SetCursor(cursor);
}

LPCTSTR ChannelSettingWnd::GetWindowClassName() const {
    return _T("ChannelSettingWnd");
}

CDuiString ChannelSettingWnd::GetSkinFile() {
    return _T(R"(Theme\UI_ChannelSettingWnd.xml)");
}

void ChannelSettingWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    m_pWndOpenGL = m_PaintManager.FindControl<CWindowUI*>(_T("WndOpenGL"));
    m_OpenGL     = std::make_unique<OpenGL>();
    m_OpenGL->Create(m_hWnd);
    m_OpenGL->Attach(m_pWndOpenGL);
    m_OpenGL->AddModel<ModelAScan>();
    m_OpenGL->getModel<ModelAScan*>()->SetViewChanel(mChannel);
    mUtils->addReadCallback(std::bind(&ChannelSettingWnd::UpdateAScanCallback, this, std::placeholders::_1, std::placeholders::_2));
    ReadValue2UI();
}

void ChannelSettingWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_RETURN || msg.sType == DUI_MSGTYPE_MOUSEWHELL) {
        if (msg.pSender->GetName() == _T("EditSoundVelocity")) {
            mUtils->getBridge()->setSoundVelocity(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditFrequency")) {
            mUtils->getBridge()->setFrequency(_wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditChannelFlag")) {
            mUtils->getBridge()->setChannelFlag(_wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditScanIncrement")) {
            mUtils->getBridge()->setScanIncrement(_wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditZeroBias")) {
            mUtils->getBridge()->setZeroBias(mChannel,
                                             (float)mUtils->getBridge()->distance2time(_wtof(msg.pSender->GetText().GetData()), mChannel));
        } else if (msg.pSender->GetName() == _T("EditPulseWidth")) {
            mUtils->getBridge()->setPulseWidth(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditDelay")) {
            mUtils->getBridge()->setDelay(mChannel,
                                          (float)mUtils->getBridge()->distance2time(_wtof(msg.pSender->GetText().GetData()), mChannel));
        } else if (msg.pSender->GetName() == _T("EditSampleDepth")) {
            mUtils->getBridge()->setSampleDepth(
                mChannel, (float)mUtils->getBridge()->distance2time(_wtof(msg.pSender->GetText().GetData()), mChannel));
            auto depth        = mUtils->getBridge()->getSampleDepth(mChannel);
            auto sampleFactor = static_cast<int>(std::round(depth * 100.0 / 1024.0));
            mUtils->getBridge()->setSampleFactor(mChannel, sampleFactor);
            auto    edit = m_PaintManager.FindControl<CEditUI*>(L"EditSampleFactor");
            CString str;
            str.Format(L"%d", sampleFactor);
            edit->SetText(str);
        } else if (msg.pSender->GetName() == _T("EditSampleFactor")) {
            mUtils->getBridge()->setSampleFactor(mChannel, _wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditGain")) {
            mUtils->getBridge()->setGain(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        if (msg.pSender->GetName() == _T("ComboVolatage")) {
            auto voltage = static_cast<CComboUI*>(msg.pSender);
            auto index   = voltage->GetCurSel();
            mUtils->getBridge()->setVoltage(static_cast<HDBridge::HB_Voltage>(index));
        } else if (msg.pSender->GetName() == _T("ComboFilter")) {
            auto p     = static_cast<CComboUI*>(msg.pSender);
            auto index = p->GetCurSel();
            mUtils->getBridge()->setFilter(mChannel, static_cast<HDBridge::HB_Filter>(index));
        } else if (msg.pSender->GetName() == _T("CombotDemodu")) {
            auto p     = static_cast<CComboUI*>(msg.pSender);
            auto index = p->GetCurSel();
            mUtils->getBridge()->setDemodu(mChannel, static_cast<HDBridge::HB_Demodu>(index));
        } else if (msg.pSender->GetName() == _T("CombotGateBType")) {
            auto p     = static_cast<CComboUI*>(msg.pSender);
            auto index = p->GetCurSel();
            mUtils->getBridge()->setGate2Type(mChannel, static_cast<HDBridge::HB_Gate2Type>(index));
        }
    } else if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == L"BtnCopy") {
            std::vector<size_t> dist = {};
            for (size_t i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
                CString str;
                str.Format(L"OptCH%lld", i + 1);
                auto s = static_cast<COptionUI*>(m_PaintManager.FindControl(str));
                if (s->IsSelected()) {
                    dist.push_back(i);
                }
            }
            mUtils->getBridge()->paramCopy(static_cast<size_t>(mChannel), dist);
            DMessageBox(L"通道拷贝成功!");
        }
    }
    mUtils->getBridge()->flushSetting();
    CDuiWindowBase::Notify(msg);
}

std::unique_ptr<HD_Utils>&& ChannelSettingWnd::returnHDUtils() {
    spdlog::debug("channel: {}", mChannel);
    mUtils->removeReadCallback();
    return std::move(mUtils);
}

void ChannelSettingWnd::ReadValue2UI() {
    auto edit = m_PaintManager.FindControl<CEditUI*>(_T("EditSoundVelocity"));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getSoundVelocity(mChannel));
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditFrequency"));
    if (edit) {
        CString str;
        str.Format(_T("%d"), mUtils->getBridge()->getFrequency());
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditChannelFlag"));
    if (edit) {
        CString str;
        str.Format(_T("%08X"), mUtils->getBridge()->getChannelFlag());
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditScanIncrement"));
    if (edit) {
        CString str;
        str.Format(_T("%d"), mUtils->getBridge()->getScanIncrement());
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditZeroBias"));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->time2distance(mUtils->getBridge()->getZeroBias(mChannel), mChannel));
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditPulseWidth"));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getPulseWidth(mChannel));
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditDelay"));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->time2distance(mUtils->getBridge()->getDelay(mChannel), mChannel));
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditSampleDepth"));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->time2distance(mUtils->getBridge()->getSampleDepth(mChannel), mChannel));
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditSampleFactor"));
    if (edit) {
        CString str;
        str.Format(_T("%d"), mUtils->getBridge()->getSampleFactor(mChannel));
        edit->SetText(str);
    }
    edit = m_PaintManager.FindControl<CEditUI*>(_T("EditGain"));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getGain(mChannel));
        edit->SetText(str);
    }
}

void ChannelSettingWnd::UpdateAScanCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller) {
    if (data.iChannel == mChannel) {
        auto model  = static_cast<ModelAScan*>(m_OpenGL->m_pModel[0]);
        auto bridge = caller.getBridge();
        auto mesh   = static_cast<MeshAscan*>(model->m_pMesh[0]);
        auto hdata  = std::make_shared<std::vector<uint8_t>>(data.pAscan);
        if (model == nullptr || bridge == nullptr || mesh == nullptr || hdata == nullptr) {
            return;
        }
        mesh->hookAScanData(hdata);
        float delay = bridge->getDelay(data.iChannel);
        float depth = bridge->getSampleDepth(data.iChannel) + delay;
        mesh->SetLimits((float)(bridge->time2distance(delay, data.iChannel)), (float)(bridge->time2distance(depth, data.iChannel)));
        for (int i = 0; i < 2; i++) {
            HDBridge::HB_GateInfo g = bridge->getGateInfo(i, data.iChannel);
            mesh->UpdateGate(g.gate, g.active, g.pos, g.width, g.height);
        }
    }
}
