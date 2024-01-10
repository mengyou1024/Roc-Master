#include "pch.h"

#include "MainFrameWnd.h"

#include "Mutilple.h"
#include <BusyWnd.h>
#include <ChannelSettingWnd.h>
#include <DefectsListWnd.h>
#include <HardWareWnd.h>
#include <MeshAscan.h>
#include <MeshGroupCScan.h>
#include <Model.h>
#include <ModelGroupAScan.h>
#include <ModelGroupCScan.h>
#include <ParamManagementWnd.h>
#include <RecordSelectWnd.h>
#include <SettingWnd.h>
#include <UI/DetectionInformationEntryWnd.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <future>
#include <iomanip>
#include <regex>
#include <rttr/type.h>
#include <sstream>

using rttr::array_range;
using rttr::property;
using rttr::type;
using sqlite_orm::c;
using sqlite_orm::column;
using sqlite_orm::columns;
using sqlite_orm::where;
constexpr std::wstring_view SCAN_CONFIG_LAST = _T("上一次配置");

#undef GATE_A
#undef GATE_B

enum TIMER_ENUM {
    CSCAN_UPDATE = 0,
    TIMER_SIZE,
};

static constexpr int swapAScanIndex(int x) {
    int result = x / 4;
    int remain = x % 4;
    switch (remain) {
        case 0: remain = 2; break;
        case 1: remain = 3; break;
        case 2: remain = 0; break;
        case 3: remain = 1; break;
        default: break;
    }
    return result * 4 + remain;
}

MainFrameWnd::MainFrameWnd() {
    try {
        mCScanThreadRunning = true;
        mCScanThread        = std::thread(&MainFrameWnd::ThreadCScan, this);
        auto config         = HDBridge::storage().get_all<HDBridge>(where(c(&HDBridge::getName) == std::wstring(SCAN_CONFIG_LAST)));
        if (config.size() == 1) {
            if (config[0].isValid()) {
                auto                 systemConfig = GetSystemConfig();
                unique_ptr<HDBridge> bridge       = nullptr;
                if (systemConfig.enableNetworkTOFD) {
                    bridge = GenerateHDBridge<TOFDMultiPort>(config[0], 2);
                } else {
                    bridge = GenerateHDBridge<TOFDMultiPort>(config[0], 1);
                }
                mUtils = std::make_unique<HD_Utils>(bridge);
                mUtils->getBridge()->syncCache2Board();
#ifndef _DEBUG
                config[0].setValid(false);
#endif // ! _DEBUG
                HDBridge::storage().update(config[0]);
            } else {
                throw std::runtime_error("上一次配置验证失败，可能由于软件运行中异常退出.");
            }
        } else {
            throw std::runtime_error("未获取到上一次配置.");
        }
        auto detectInfos = ORM_Model::DetectInfo::storage().get_all<ORM_Model::DetectInfo>();
        if (detectInfos.size() == 1) {
            mDetectInfo = detectInfos[0];
        }
    } catch (std::runtime_error &e) {
        spdlog::warn(e.what());
        spdlog::warn("将使用默认配置初始化.");
        auto                 systemConfig = GetSystemConfig();
        unique_ptr<HDBridge> bridge       = nullptr;
        if (systemConfig.enableNetworkTOFD) {
            bridge = GenerateHDBridge<TOFDMultiPort>({}, 2);
        } else {
            bridge = GenerateHDBridge<TOFDMultiPort>({}, 1);
        }
        mUtils = std::make_unique<HD_Utils>(bridge);
        mUtils->getBridge()->defaultInit();
    } catch (std::exception &e) { spdlog::error(GB2312ToUtf8(e.what())); }
}

MainFrameWnd::~MainFrameWnd() {
    try {
        mCScanThreadRunning = false;
        mCScanNotify.notify_all();
        mCScanThread.join();
        // 退出前停止扫查并且退出回放模式
        StopScan(true);
        if (mWidgetMode == WidgetMode::MODE_REVIEW) {
            ExitReviewMode();
        }
        auto tick    = GetTickCount64();
        auto bridges = HDBridge::storage().get_all<HDBridge>(where(c(&HDBridge::getName) == std::wstring(SCAN_CONFIG_LAST)));
        if (bridges.size() == 1) {
            bridges[0].setValid(true);
            bridges[0].setCache(mUtils->getCache());
            HDBridge::storage().update(bridges[0]);
        } else {
            auto bridge = mUtils->getBridge();
            bridge->setName(std::wstring(SCAN_CONFIG_LAST));
            bridge->setValid(true);
            HDBridge::storage().insert(*(mUtils->getBridge()));
        }
        auto detectInfos = ORM_Model::DetectInfo::storage().get_all<ORM_Model::DetectInfo>();
        if (detectInfos.size() == 1) {
            mDetectInfo.id = 1;
            ORM_Model::DetectInfo::storage().update(mDetectInfo);
        } else {
            ORM_Model::DetectInfo::storage().insert(mDetectInfo);
        }
        spdlog::debug("take time: {}", GetTickCount64() - tick);
    } catch (std::exception &e) { spdlog::error(GB2312ToUtf8(e.what())); }
}

void MainFrameWnd::OnBtnModelClicked(std::wstring name) {
    auto btnScanMode   = m_PaintManager.FindControl<CButtonUI *>(_T("BtnScanMode"));
    auto btnReviewMode = m_PaintManager.FindControl<CButtonUI *>(_T("BtnReviewMode"));
    if (name == _T("BtnScanMode")) {
        if (btnScanMode->GetBkColor() != 0xFF339933) {
            btnScanMode->SetBkColor(0xFF339933);
            btnReviewMode->SetBkColor(0xFFEEEEEE);
            BusyWnd wnd([this]() { ExitReviewMode(); });
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.ShowModal();
            // 退出后重新开始扫查
            StartScan(false);
        }

    } else {
        RecordSelectWnd selectWnd;
        selectWnd.Create(m_hWnd, selectWnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        selectWnd.CenterWindow();
        selectWnd.ShowModal();
        auto &[res, name] = selectWnd.GetResult();
        if (!res) {
            return;
        }

        // 打开选择窗口
        auto   &selName = name;
        bool    ret     = false;
        BusyWnd wnd([this, &selName, &ret]() {
            // 进入前先暂停扫查
            StopScan(false);
            ret = EnterReviewMode(selName);
        });
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.ShowModal();
        if (ret) {
            btnScanMode->SetBkColor(0xFFEEEEEE);
            btnReviewMode->SetBkColor(0xFF339933);
        }
    }
}

LPCTSTR MainFrameWnd::GetWindowClassName() const {
    return _T("MainFrameWnd");
}

CDuiString MainFrameWnd::GetSkinFile() noexcept {
    return _T("Theme\\UI_MainFrameWnd.xml");
}

void MainFrameWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    // A扫窗口
    m_pWndOpenGL_ASCAN = m_PaintManager.FindControl<CWindowUI *>(_T("WndOpenGL_ASCAN"));
    m_OpenGL_ASCAN.Create(m_hWnd);
    m_OpenGL_ASCAN.Attach(m_pWndOpenGL_ASCAN);
    // C扫窗口
    m_pWndOpenGL_CSCAN = m_PaintManager.FindControl<CWindowUI *>(_T("WndOpenGL_CSCAN"));
    m_OpenGL_CSCAN.Create(m_hWnd);
    m_OpenGL_CSCAN.Attach(m_pWndOpenGL_CSCAN);
    // 初始化
    AddTaskToQueue([this]() {
        // 延迟最大化窗口
        SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

        Sleep(100);

        m_OpenGL_ASCAN.AddGroupAScanModel();
        m_OpenGL_CSCAN.AddGroupCScanModel();
        // 设置板卡参数
        Sleep(100);
        mUtils->start();
        mUtils->addReadCallback(HD_Utils::WrapReadCallback(&MainFrameWnd::UpdateAllGateResult, this));
        mUtils->addReadCallback(HD_Utils::WrapReadCallback(&MainFrameWnd::UpdateAScanCallback, this));
        SelectMeasureThickness(GetSystemConfig().enableMeasureThickness);

        // 进入回放界面、检查更新
        if (!mReviewPathEntry.empty()) {
            if (!EnterReviewMode(mReviewPathEntry)) {
                spdlog::warn("载入文件: {} 出错!", mReviewPathEntry);
            }
        }
        CheckAndUpdate();
    });
    UpdateSliderAndEditValue(mCurrentGroup, mConfigType, mGateType, mChannelSel, true);
}

void MainFrameWnd::DrawReviewCScan() {
    // 删除所有通道的C扫数据
    for (int index = 0; index < HDBridge::CHANNEL_NUMBER + 4; index++) {
        auto mesh = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(index);
        mesh->RemoveDot();
        mesh->RemoveLine();
    }
    for (const auto &data : *mFragmentReview) {
        for (int index = 0; index < HDBridge::CHANNEL_NUMBER; index++) {
            auto mesh = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(index);
            if (data.mScanOrm.mScanGateInfo[index].width != 0.0f) {
                auto &[pos, width, _] = data.mScanOrm.mScanGateInfo[index];
                auto      size        = data.mScanOrm.mScanData[index]->pAscan.size();
                auto      begin       = std::begin(data.mScanOrm.mScanData[index]->pAscan);
                auto      left        = begin + (size_t)((double)pos * (double)size);
                auto      right       = begin + (size_t)((double)(pos + width) * (double)size);
                auto      max         = std::max_element(left, right);
                glm::vec4 color       = {};
                if (*max > data.mScanOrm.mScanData[index]->pGateAmp[1]) {
                    color = {1.0f, 0.f, 0.f, 1.0f};
                } else {
                    color = {1.0f, 1.0f, 1.0f, 1.0f};
                }
                mesh->AppendDot(*max, color, MAXSIZE_T);
            } else {
                mesh->AppendDot(0, {0.0f, 1.0f, 0.0f, 1.0f}, MAXSIZE_T);
            }
        }
        for (size_t index = 0; index < 4ull; index++) {
            auto mesh         = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(HDBridge::CHANNEL_NUMBER + index);
            auto thickness    = data.mScanOrm.mThickness[index];
            auto baseTickness = _wtof(mDetectInfo.thickness.c_str());
            if (baseTickness != 0.0 && baseTickness != -HUGE_VAL && baseTickness != HUGE_VAL) {
                auto relative_error = (thickness - baseTickness) / baseTickness;
                if (relative_error > RELATIVE_ERROR_MAX) {
                    relative_error = RELATIVE_ERROR_MAX;
                } else if (relative_error < -RELATIVE_ERROR_MAX) {
                    relative_error = -RELATIVE_ERROR_MAX;
                }
                glm::vec4 color = {};
                if (relative_error > RELATIVE_ERROR_THRESHOLD) {
                    color = {.0f, 0.f, 1.f, 1.0f};
                } else if (relative_error < -RELATIVE_ERROR_THRESHOLD) {
                    color = {1.0f, 0.f, 0.f, 1.0f};
                } else {
                    color = {.0f, 1.f, 0.f, 1.0f};
                }
                uint8_t value = (((uint8_t)std::round((double)RELATIVE_ERROR_BASE * std::abs(relative_error / RELATIVE_ERROR_MAX))) &
                                 RELATIVE_ERROR_BASE);
                if (relative_error >= 0) {
                    value += RELATIVE_ERROR_BASE;
                } else {
                    value = RELATIVE_ERROR_BASE - value;
                }
                mesh->AppendDot(value, color, MAXSIZE_T);
            } else {
                mesh->AppendDot(0, {1.0f, 1.0f, 1.0f, 1.0f}, MAXSIZE_T);
            }
        }
    }

    // 回放的C扫范围为第一幅图的最小值到最后一幅图的最大值
    if (mFragmentReview->size() > 0) {
        float cScanMinLimits = ((*mFragmentReview)[0]).mScanOrm.mCScanLimits[0];
        float cScanMaxLimits = ((*mFragmentReview)[mFragmentReview->size() - 1]).mScanOrm.mCScanLimits[1];
        m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->SetAxisRange(cScanMinLimits, cScanMaxLimits);
    }
}

void MainFrameWnd::UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel,
                                            bool bypassCheck) {
    if (!bypassCheck && (mCurrentGroup == newGroup && mConfigType == newConfig && mGateType == newGate && mChannelSel == newChannelSel)) {
        return;
    }

    spdlog::debug("Click Button, group:{}, config: {}, gate: {}, channelsel: {}", newGroup, (int)newConfig, (int)newGate,
                  (int)newChannelSel);

    OnBtnSelectGroupClicked(newGroup);
    mConfigType = newConfig;
    mGateType   = newGate;
    mChannelSel = newChannelSel;

    // 设置Edit单位
    auto edit = m_PaintManager.FindControl<CEditUI *>(_T("EditConfig"));
    edit->SetTextValitor(mConfigRegex.at(mConfigType));
    if (edit) {
        edit->SetEnabled(true);
        edit->SetTextExt(mConfigTextext.at(mConfigType));
    }
    // 设置Slider的min、max
    auto slider = m_PaintManager.FindControl<CSliderUI *>(_T("SliderConfig"));
    if (slider) {
        slider->SetEnabled(true);
        slider->SetCanSendMove(true);
        // 重新计算波门起点和波门宽度的最大值
        int    gate    = static_cast<int>(mGateType);
        size_t chIndex = static_cast<size_t>(mChannelSel) + static_cast<size_t>(mCurrentGroup) * 4ull;
        switch (mConfigType) {
            case MainFrameWnd::ConfigType::GateStart: {
                if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                    auto &[_, maxLimits] = mConfigLimits[mConfigType];
                    maxLimits            = static_cast<float>((1.0 - mUtils->getCache().scanGateInfo[chIndex].width) * 100.0);
                    if (maxLimits <= 2) {
                        slider->SetEnabled(false);
                        slider->SetCanSendMove(false);
                        if (edit) {
                            edit->SetEnabled(false);
                        }
                    }
                    break;
                }
                auto &[_, maxLimits] = mConfigLimits[mConfigType];
                auto gateInfo        = mUtils->getBridge()->getGateInfo(gate, (int)chIndex);
                maxLimits            = static_cast<float>((1.0 - gateInfo.width) * 100.0);
                if (maxLimits <= 2) {
                    slider->SetEnabled(false);
                    slider->SetCanSendMove(false);
                    if (edit) {
                        edit->SetEnabled(false);
                    }
                }
                break;
            }
            case MainFrameWnd::ConfigType::GateWidth: {
                if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                    auto &[_, maxLimits] = mConfigLimits[mConfigType];
                    maxLimits            = static_cast<float>((1.0 - mUtils->getCache().scanGateInfo[chIndex].pos) * 100.0);
                    if (maxLimits <= 2) {
                        slider->SetEnabled(false);
                        slider->SetCanSendMove(false);
                        if (edit) {
                            edit->SetEnabled(false);
                        }
                    }
                    break;
                }
                auto &[_, maxLimits] = mConfigLimits[mConfigType];
                auto gateInfo        = mUtils->getBridge()->getGateInfo(gate, (int)chIndex);
                maxLimits            = static_cast<float>((1.0 - gateInfo.pos) * 100.0);
                if (maxLimits <= 2) {
                    slider->SetEnabled(false);
                    slider->SetCanSendMove(false);
                    if (edit) {
                        edit->SetEnabled(false);
                    }
                }
                break;
            }
            default: {
                break;
            }
        }

        slider->SetMinValue(static_cast<int>(mConfigLimits.at(mConfigType).first));
        slider->SetMaxValue(static_cast<int>(mConfigLimits.at(mConfigType).second));
    }
    // DONE: 重新读取数值
    double reloadValue = 0.0;
    int    _channelSel = static_cast<int>(mChannelSel) + mCurrentGroup * 4;
    int    gate        = static_cast<int>(mGateType);
    auto   bridge      = mUtils->getBridge();
    switch (mConfigType) {
        case MainFrameWnd::ConfigType::DetectRange: {
            reloadValue = bridge->time2distance(bridge->getSampleDepth(_channelSel), _channelSel);
            break;
        }
        case MainFrameWnd::ConfigType::Gain: {
            reloadValue = bridge->getGain(_channelSel);
            break;
        }
        case MainFrameWnd::ConfigType::GateStart: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                reloadValue = mUtils->getCache().scanGateInfo[_channelSel].pos * 100.0;
                break;
            }
            reloadValue = bridge->getGateInfo(gate, _channelSel).pos * 100.0;
            break;
        }
        case MainFrameWnd::ConfigType::GateWidth: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                reloadValue = mUtils->getCache().scanGateInfo[_channelSel].width * 100.0;
                break;
            }
            reloadValue = bridge->getGateInfo(gate, _channelSel).width * 100.0;
            break;
        }
        case MainFrameWnd::ConfigType::GateHeight: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                reloadValue = mUtils->getCache().scanGateInfo[_channelSel].height * 100.0;
                break;
            }
            reloadValue = reloadValue = bridge->getGateInfo(gate, _channelSel).height * 100.0;
            break;
        }
        default: {
            break;
        }
    }
    reloadValue = std::round(reloadValue * 100.f) / 100.f;

    slider->SetValue(static_cast<int>(std::round(reloadValue)));
    std::wstring limit = std::to_wstring(reloadValue);
    edit->SetText(limit.c_str());
}

void MainFrameWnd::SetConfigValue(float val, bool sync) {
    auto tick        = GetTickCount64();
    int  _channelSel = static_cast<int>(mChannelSel) + mCurrentGroup * 4;
    int  gate        = static_cast<int>(mGateType);
    auto bridge      = mUtils->getBridge();
    switch (mConfigType) {
        case MainFrameWnd::ConfigType::DetectRange: {
            bridge->setSampleDepth(_channelSel, (float)(bridge->distance2time((double)val, _channelSel)));
            // 重新计算采样因子
            auto depth        = bridge->getSampleDepth(_channelSel);
            auto sampleFactor = static_cast<int>(std::round(depth * 100.0 / 1024.0));
            bridge->setSampleFactor(_channelSel, sampleFactor);
            break;
        }
        case MainFrameWnd::ConfigType::Gain: {
            bridge->setGain(_channelSel, val);
            break;
        }
        case MainFrameWnd::ConfigType::GateStart: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                mUtils->getCache().scanGateInfo[_channelSel].pos = static_cast<float>(val / 100.0);
                auto m = static_cast<MeshAscan *>(m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh[_channelSel]);
                m->UpdateGate(gate, true, mUtils->getCache().scanGateInfo[_channelSel].pos,
                              mUtils->getCache().scanGateInfo[_channelSel].width, mUtils->getCache().scanGateInfo[_channelSel].height);
                break;
            }
            HDBridge::HB_GateInfo g = bridge->getGateInfo(gate, _channelSel);
            g.gate                  = gate;
            g.active                = 1;
            g.pos                   = static_cast<float>(val / 100.0);
            spdlog::debug("set gate info {}, gate: {}", bridge->setGateInfo(_channelSel, g), gate);
            break;
        }
        case MainFrameWnd::ConfigType::GateWidth: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                mUtils->getCache().scanGateInfo[_channelSel].width = static_cast<float>(val / 100.0);
                auto m = static_cast<MeshAscan *>(m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh[_channelSel]);
                m->UpdateGate(gate, true, mUtils->getCache().scanGateInfo[_channelSel].pos,
                              mUtils->getCache().scanGateInfo[_channelSel].width, mUtils->getCache().scanGateInfo[_channelSel].height);
                break;
            }
            HDBridge::HB_GateInfo g = bridge->getGateInfo(gate, _channelSel);
            g.gate                  = gate;
            g.active                = 1;
            g.width                 = static_cast<float>(val / 100.0);
            spdlog::debug("set gate info {}, gate: {}", bridge->setGateInfo(_channelSel, g), gate);
            break;
        }
        case MainFrameWnd::ConfigType::GateHeight: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                mUtils->getCache().scanGateInfo[_channelSel].height = static_cast<float>(val / 100.0);
                auto m = static_cast<MeshAscan *>(m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh[_channelSel]);
                m->UpdateGate(gate, true, mUtils->getCache().scanGateInfo[_channelSel].pos,
                              mUtils->getCache().scanGateInfo[_channelSel].width, mUtils->getCache().scanGateInfo[_channelSel].height);
                break;
            }
            HDBridge::HB_GateInfo g = bridge->getGateInfo(gate, _channelSel);
            g.gate                  = gate;
            g.active                = 1;
            g.height                = static_cast<float>(val / 100.0);
            spdlog::debug("set gate info {}, gate: {}", bridge->setGateInfo(_channelSel, g), gate);
            break;
        }
        default: {
            break;
        }
    }
    if (sync) {
        AddTaskToQueue([bridge]() { bridge->flushSetting(); }, "flushSetting", true);
    }
}

void MainFrameWnd::UpdateAScanCallback(const HDBridge::NM_DATA &data, const HD_Utils &caller) {
    auto model = m_OpenGL_ASCAN.getModel<ModelGroupAScan *>();
    if (model == nullptr || model->m_pMesh.at(data.iChannel) == nullptr) {
        return;
    }
    auto                                  bridge = caller.getBridge();
    auto                                  mesh   = model->getMesh<MeshAscan *>(data.iChannel);
    std::shared_ptr<std::vector<uint8_t>> hdata  = std::make_shared<std::vector<uint8_t>>(data.pAscan);
    if (bridge == nullptr || mesh == nullptr || hdata == nullptr) {
        return;
    }
    // 更新A扫图像
    mesh->hookAScanData(hdata);
    float delay = bridge->getDelay(data.iChannel);
    float depth = bridge->getSampleDepth(data.iChannel) + delay;
    // 设置坐标轴范围
    mesh->SetLimits((float)(bridge->time2distance(delay, data.iChannel)), (float)(bridge->time2distance(depth, data.iChannel)));
    // 更新波门
    for (int i = 0; i < 2; i++) {
        HDBridge::HB_GateInfo g = bridge->getGateInfo(i, data.iChannel);
        mesh->UpdateGate(g.gate, g.active, g.pos, g.width, g.height);
    }
    auto &info = bridge->getCache().scanGateInfo[data.iChannel];
    // 更新扫查波门
    mesh->UpdateGate(2, 1, info.pos, info.width, info.height);

    // 测厚的波门
    if (data.iChannel < 4) {
        auto  mesh   = m_OpenGL_ASCAN.getMesh<MeshAscan *>((size_t)data.iChannel + HDBridge::CHANNEL_NUMBER);
        auto  bridge = mUtils->getBridge();
        auto &info   = bridge->getCache().scanGateInfo[(size_t)data.iChannel + HDBridge::CHANNEL_NUMBER];
        mesh->UpdateGate(2, 1, info.pos, info.width, info.height);
    }
}

void MainFrameWnd::UpdateAllGateResult(const HDBridge::NM_DATA &data, const HD_Utils &caller) {
    auto channel = data.iChannel;
    if (channel >= HDBridge::CHANNEL_NUMBER || channel < 0) {
        return;
    }
    if (GetTickCount64() - mLastGateResUpdate[channel] < 500) {
        return;
    }
    mLastGateResUpdate[channel] = GetTickCount64();
    auto mesh                   = m_OpenGL_ASCAN.getMesh<MeshAscan *>((size_t)channel);
    if (!mesh) {
        return;
    }
    auto bridge = caller.getBridge();
    for (int i = 0; i < 3; i++) {
        auto info            = bridge->getScanGateInfo(channel, i);
        auto [pos, max, res] = bridge->computeGateInfo(data.pAscan, info);
        if (res) {
            mAllGateResult[channel][i].result = true;
            auto [bias, depth]                = bridge->getRangeOfAcousticPath(channel);
            mAllGateResult[channel][i].pos    = pos * (float)depth;
            mAllGateResult[channel][i].max    = (float)max / 2.55f;
            auto gateData                     = std::make_pair(mAllGateResult[channel][i].pos, mAllGateResult[channel][i].max);
            mesh->SetGateData(gateData, i);
        } else {
            mAllGateResult[channel][i].result = false;
            mesh->SetGateData(i);
        }
    }
    if (channel < 4) {
        channel              = HDBridge::CHANNEL_NUMBER + channel;
        mesh                 = m_OpenGL_ASCAN.getMesh<MeshAscan *>(channel);
        int  i               = 2;
        auto info            = bridge->getScanGateInfo(channel, i);
        auto [pos, max, res] = bridge->computeGateInfo(data.pAscan, info);
        if (res) {
            mAllGateResult[channel][i].result = true;
            auto [bias, depth]                = bridge->getRangeOfAcousticPath(channel);
            mAllGateResult[channel][i].pos    = pos * (float)depth;
            mAllGateResult[channel][i].max    = (float)max / 2.55f;
            auto gateData                     = std::make_pair(mAllGateResult[channel][i].pos, mAllGateResult[channel][i].max);
            mesh->SetGateData(gateData, i);
        } else {
            mAllGateResult[channel][i].result = false;
            mesh->SetGateData(i);
        }
        for (int i = 0; i < 2; i++) {
            mAllGateResult[channel][i] = mAllGateResult[(size_t)channel - HDBridge::CHANNEL_NUMBER][i];
            auto res                   = mAllGateResult[channel][i].result;
            if (res) {
                auto gateData = std::make_pair(mAllGateResult[channel][i].pos, mAllGateResult[channel][i].max);
                mesh->SetGateData(gateData, i);
            } else {
                mesh->SetGateData(i);
            }
        }

        auto gateLeft      = bridge->getScanGateInfo(data.iChannel + HDBridge::CHANNEL_NUMBER, 1);
        auto gateRigth     = bridge->getScanGateInfo(data.iChannel + HDBridge::CHANNEL_NUMBER, 2);
        auto [bias, depth] = bridge->getRangeOfAcousticPath(data.iChannel);
        auto thickness     = HDBridge::computeDistance(data.pAscan, depth, gateLeft, gateRigth);
        mUtils->mScanOrm.mThickness[(size_t)channel - HDBridge::CHANNEL_NUMBER] = (float)thickness;
        mesh->SetTickness((float)thickness);
    }
}

void MainFrameWnd::AmpTraceCallback(const HDBridge::NM_DATA &data, const HD_Utils &caller) {
    std::vector<int> traceList = {0, 1, 2, 3};
    auto             bridge    = caller.getBridge();
    if (std::find(traceList.begin(), traceList.end(), data.iChannel) != traceList.end()) {
        // 计算A波门需要调整的偏移
        auto gateInfo        = caller.getBridge()->getScanGateInfo(data.iChannel, 0);
        auto [pos, max, res] = HDBridge::computeGateInfo(data.pAscan, gateInfo);
        if (res) {
            float bias = pos - (gateInfo.pos + gateInfo.width / 2.0f);
            // 调整波门位置
            for (int i = 0; i < 3; i++) {
                auto info = caller.getBridge()->getScanGateInfo(data.iChannel, i);
                info.pos += bias;
                caller.getBridge()->setScanGateInfo(data.iChannel, info, i);
            }
            if (data.iChannel < 4) {
                auto info = caller.getBridge()->getScanGateInfo(data.iChannel + HDBridge::CHANNEL_NUMBER);
                info.pos += bias;
                caller.getBridge()->setScanGateInfo(data.iChannel + HDBridge::CHANNEL_NUMBER, info);
            }
        }
    }
}

void MainFrameWnd::AmpMemeryCallback(const HDBridge::NM_DATA &data, const HD_Utils &caller) {
    auto model = m_OpenGL_ASCAN.getModel<ModelGroupAScan *>();
    if (model == nullptr || model->m_pMesh.at(data.iChannel) == nullptr) {
        return;
    }
    auto                                  bridge = caller.getBridge();
    auto                                  mesh   = model->getMesh<MeshAscan *>(data.iChannel);
    std::shared_ptr<std::vector<uint8_t>> hdata  = std::make_shared<std::vector<uint8_t>>(data.pAscan);
    if (bridge == nullptr || mesh == nullptr || hdata == nullptr) {
        return;
    }
    // 峰值记忆
    for (int i = 0; i < 2; i++) {
        const auto ampData = mesh->GetAmpMemoryData(i);
        auto       g       = bridge->getGateInfo(i, data.iChannel);
        // 获取波门内的数据
        auto left  = data.pAscan.begin() + static_cast<int64_t>((double)data.pAscan.size() * (double)g.pos);
        auto right = data.pAscan.begin() + static_cast<int64_t>((double)data.pAscan.size() * (double)(g.pos + g.width));

        std::vector<uint8_t> newAmpData(left, right);

        if (ampData.size() == newAmpData.size()) {
            for (auto i = 0; i < ampData.size(); i++) {
                if (newAmpData[i] < ampData[i]) {
                    newAmpData[i] = ampData[i];
                }
            }
        }
        mesh->hookAmpMemoryData(i, std::make_shared<std::vector<uint8_t>>(newAmpData));
    }
    const auto           ampData  = mesh->GetAmpMemoryData(2);
    const auto          &gateInfo = mUtils->getCache().scanGateInfo[data.iChannel];
    auto                 start    = (double)gateInfo.pos;
    auto                 end      = (double)(gateInfo.pos + gateInfo.width);
    auto                 left     = data.pAscan.begin() + static_cast<int64_t>((double)data.pAscan.size() * start);
    auto                 right    = data.pAscan.begin() + static_cast<int64_t>((double)data.pAscan.size() * end);
    std::vector<uint8_t> newAmpData(left, right);
    if (ampData.size() == newAmpData.size()) {
        for (auto i = 0; i < ampData.size(); i++) {
            if (newAmpData[i] < ampData[i]) {
                newAmpData[i] = ampData[i];
            }
        }
    }
    mesh->hookAmpMemoryData(2, std::make_shared<std::vector<uint8_t>>(newAmpData));
}

void MainFrameWnd::UpdateCScanOnTimer() {
    mCScanNotify.notify_one();
}

void MainFrameWnd::OnBtnUIClicked(std::wstring &name) {
    if (name == _T("Setting")) {
        auto       config                 = GetSystemConfig();
        auto       enableNetwork          = config.enableNetworkTOFD;
        auto       enableMeasureThickness = config.enableMeasureThickness;
        SettingWnd wnd;
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.ShowModal();
        auto newConfig                 = GetSystemConfig();
        auto newEnableNetwork          = newConfig.enableNetworkTOFD;
        auto newEnableMeasureThickness = newConfig.enableMeasureThickness;
        if (mWidgetMode == WidgetMode::MODE_SCAN && newEnableMeasureThickness != enableMeasureThickness) {
            SelectMeasureThickness(GetSystemConfig().enableMeasureThickness);
        }
        if (enableNetwork != newEnableNetwork) {
            if (newEnableNetwork) {
                mUtils->pushCallback();
                ReconnectBoard(2);
                mUtils->popCallback();
            } else {
                mUtils->pushCallback();
                ReconnectBoard(1);
                mUtils->popCallback();
            }
        }

    } else if (name == _T("AutoScan")) {
        if (mScanningFlag == true) {
            BusyWnd wnd([this]() { StopScan(); });
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.ShowModal();
            auto btn = m_PaintManager.FindControl<CButtonUI *>(_T("BtnUIAutoScan"));
            btn->SetBkColor(0xFFEEEEEE);
        } else {
            if (!mUtils->getBridge()->isOpen()) {
                DMessageBox(L"超声板未打开，请确认是否连接！", L"自动检测");
                return;
            }
            StartScan();
            auto btn = m_PaintManager.FindControl<CButtonUI *>(_T("BtnUIAutoScan"));
            btn->SetBkColor(0xFF339933);
        }
    } else if (name == _T("ParamManagement")) {
        ParamManagementWnd wnd(mUtils->getBridge());
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.ShowModal();
        UpdateSliderAndEditValue(mCurrentGroup, mConfigType, mGateType, mChannelSel, true);
    } else if (name == _T("About")) {
        DMessageBox(_T(APP_VERSION), L"软件版本");
    } else if (name == _T("Freeze")) {
        auto btn = m_PaintManager.FindControl<CButtonUI *>(_T("BtnUIFreeze"));
        if (btn->GetBkColor() == 0xFFEEEEEE) {
            mUtils->pushCallback();
            StopScan(false);
            btn->SetBkColor(0xFF339933);
        } else {
            mUtils->popCallback();
            StartScan(false);
            btn->SetBkColor(0xFFEEEEEE);
        }
    } else if (name == _T("AmpMemory")) {
        auto btn = m_PaintManager.FindControl<CButtonUI *>(_T("BtnUIAmpMemory"));
        if (btn->GetBkColor() == 0xFFEEEEEE) {
            mUtils->addReadCallback(std::bind(&MainFrameWnd::AmpMemeryCallback, this, std::placeholders::_1, std::placeholders::_2),
                                    "AmpMemory");
            btn->SetBkColor(0xFF339933);
        } else {
            mUtils->removeReadCallback("AmpMemory");
            for (auto &[index, mesh] : m_OpenGL_ASCAN.getMesh<MeshAscan *>()) {
                for (int i = 0; i < 3; i++) {
                    mesh->hookAmpMemoryData(i, nullptr);
                    mesh->ClearAmpMemoryData(i);
                }
            }
            btn->SetBkColor(0xFFEEEEEE);
        }
    } else if (name == _T("AmpTrace")) {
        auto btn = m_PaintManager.FindControl<CButtonUI *>(_T("BtnUIAmpTrace"));
        if (btn->GetBkColor() == 0xFFEEEEEE) {
            auto wrap = HD_Utils::WrapReadCallback(&MainFrameWnd::AmpTraceCallback, this);
            mUtils->addReadCallback(wrap, "AmpTrace");
            btn->SetBkColor(0xFF339933);
        } else {
            mUtils->removeReadCallback("AmpTrace");
            btn->SetBkColor(0xFFEEEEEE);
        }
    } else if (name == _T("AutoGain")) {
        if (mGateType == GateType::GATE_SCAN) {
            return;
        }
        BusyWnd wnd([this]() {
            mUtils->autoGain(static_cast<int>(mChannelSel) + 4 * static_cast<int>(mCurrentGroup), static_cast<int>(mGateType));
        });
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.ShowModal();
    } else if (name == _T("InformationEntry")) {
        DetectionInformationEntryWnd wnd;
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.LoadDetectInfo(mDetectInfo, GetSystemConfig().userName, GetSystemConfig().groupName);
        wnd.ShowModal();
        if (wnd.GetResult()) {
            mDetectInfo            = wnd.GetDetectInfo();
            auto systemConfig      = GetSystemConfig();
            systemConfig.groupName = wnd.GetJobGroup().groupName;
            systemConfig.userName  = wnd.GetUser().name;
            UpdateSystemConfig(systemConfig);
        }
    } else if (name == _T("HardPort")) {
        HardWareWnd wnd;
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.ShowModal();
    }
}

void MainFrameWnd::KillUITimer(void) {
    ::KillTimer(m_OpenGL_ASCAN.m_hWnd, 0);
    ::KillTimer(m_OpenGL_CSCAN.m_hWnd, 0);
}

void MainFrameWnd::ResumeUITimer(void) {
    ::SetTimer(m_OpenGL_ASCAN.m_hWnd, 0, 15, NULL);
    ::SetTimer(m_OpenGL_CSCAN.m_hWnd, 0, 15, NULL);
}

void MainFrameWnd::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString   strName = msg.pSender->GetName();
        std::wregex  matchReg(_T(R"(BtnSelectGroup(\d))"));
        std::wsmatch match;
        std::wstring str(strName.GetData());

        long       _currentGroup = mCurrentGroup;
        ConfigType _configType   = mConfigType;
        GateType   _gateType     = mGateType;
        ChannelSel _channelSel   = mChannelSel;

        if (std::regex_match(str, match, matchReg)) {
            _currentGroup = _wtol(match[1].str().data());
            UpdateSliderAndEditValue(_currentGroup, _configType, _gateType, _channelSel);
        }

        matchReg = _T(R"(OptConfigType)");
        auto opt = static_cast<COptionUI *>(msg.pSender);
        if (std::regex_match(str, matchReg)) {
            ConfigType type = static_cast<ConfigType>(_wtol(opt->GetUserData().GetData()));
            _configType     = type;
            UpdateSliderAndEditValue(_currentGroup, _configType, _gateType, _channelSel);
        }

        matchReg = _T(R"(OptGateType)");
        if (std::regex_match(str, matchReg)) {
            GateType type = static_cast<GateType>(_wtol(opt->GetUserData().GetData()));
            _gateType     = type;
            UpdateSliderAndEditValue(_currentGroup, _configType, _gateType, _channelSel);
        }
        matchReg = _T(R"(OptChannel\d)");
        if (std::regex_match(str, matchReg)) {
            ChannelSel type = static_cast<ChannelSel>(_wtol(opt->GetUserData().GetData()));
            _channelSel     = type;
            UpdateSliderAndEditValue(_currentGroup, _configType, _gateType, _channelSel);
        }

        matchReg = _T(R"(BtnUI(.+))");
        if (std::regex_match(str, match, matchReg)) {
            OnBtnUIClicked(match[1].str());
        }

        matchReg = _T(R"((BtnScanMode)|(BtnReviewMode))");
        if (std::regex_match(str, match, matchReg)) {
            OnBtnModelClicked(match[1].str());
        }

        if (msg.pSender->GetName() == L"BtnCScanSelect") {
            if (msg.pSender->GetUserData() == L"1") {
                (*mFragmentReview)++;
            } else if (msg.pSender->GetUserData() == L"-1") {
                (*mFragmentReview)--;
            }
            auto    edit = m_PaintManager.FindControl<CEditUI *>(L"EditCScanSelect");
            CString str;
            str.Format(L"第 %d 帧 / 共 %d 帧", mFragmentReview->getCurFragment(), mFragmentReview->fragments());
            edit->SetText(str);
            DrawReviewCScan();
            auto pt = GetCScainIndexPt(mFragmentReview->getCursor());
            OnLButtonDown(1, pt);
        }

        if (msg.pSender->GetName() == L"BtnCScanIndexSelect") {
            auto &cursor = (*mFragmentReview).getCursor();
            if (msg.pSender->GetUserData() == L"1") {
                cursor++;
                if (cursor >= mFragmentReview->size()) {
                    cursor = mFragmentReview->size() - 1;
                }
            } else if (msg.pSender->GetUserData() == L"-1") {
                cursor--;
                if (cursor < 0) {
                    cursor = 0;
                }
            }
            auto pt = GetCScainIndexPt(mFragmentReview->getCursor());
            OnLButtonDown(1, pt);
        }

        if (msg.pSender->GetName() == _T("BtnExportReport")) {
            std::map<string, string> valueMap = {};
            valueMap["jobGroup"]              = GetJobGroup();
            valueMap["user"]                  = StringFromWString(GetSystemConfig().userName);
            for (const auto &prot : type::get<ORM_Model::DetectInfo>().get_properties()) {
                valueMap[string(prot.get_name())] = StringFromWString(prot.get_value(mDetectInfo).convert<std::wstring>());
            }
            for (auto index = 0; index < mDefectInfo.size(); index++) {
                for (const auto &prot : type::get<ORM_Model::DefectInfo>().get_properties()) {
                    std::stringstream ss;
                    ss << "defect[" << index << "]." << prot.get_name();
                    if (prot.get_name() == "id") {
                        valueMap[ss.str()] = std::to_string(prot.get_value(mDefectInfo[index]).convert<std::uint32_t>());
                    } else {
                        valueMap[ss.str()] = StringFromWString(prot.get_value(mDefectInfo[index]).convert<std::wstring>());
                    }
                }
            }
            CFileDialog dlg(false, L"docx", L"Report.docx", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Word Document (*.docx)| *.docx||");
            if (dlg.DoModal() == IDOK) {
                if (WordTemplateRender(L"./template/template.docx", dlg.GetPathName().GetString(), valueMap) == false) {
                    spdlog::error("export report document error!");
                    DMessageBox(L"导出失败!");
                } else {
                    DMessageBox(L"导出成功!");
                }
            }

        } else if (msg.pSender->GetName() == _T("BtnDetectInformation")) {
            DetectionInformationEntryWnd wnd;
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.LoadDetectInfo(mDetectInfo, GetSystemConfig().userName, GetSystemConfig().groupName);
            wnd.CenterWindow();
            wnd.ShowModal();
            if (wnd.GetResult()) {
                mDetectInfo             = wnd.GetDetectInfo();
                auto mSystemConfig      = GetSystemConfig();
                mSystemConfig.groupName = wnd.GetJobGroup().groupName;
                mSystemConfig.userName  = wnd.GetUser().name;
                UpdateSystemConfig(mSystemConfig);
            }
        }

    } else if (msg.sType == DUI_MSGTYPE_VALUECHANGED) {
        if (msg.pSender->GetName() == _T("SliderConfig")) {
            auto slider = static_cast<CSliderUI *>(msg.pSender);
            auto edit   = m_PaintManager.FindControl<CEditUI *>(_T("EditConfig"));
            if (edit) {
                int     sliderValue = slider->GetValue();
                CString val;
                val.Format(_T("%.2f"), static_cast<float>(sliderValue));
                edit->SetText(val);
                spdlog::debug(_T("setValue: {}"), val);

                // 设置Edit数值
                auto edit = m_PaintManager.FindControl<CEditUI *>(_T("EditConfig"));
                if (edit) {
                    edit->SetText(std::to_wstring(sliderValue).data());
                }
                // 设置超声板数值
                if (msg.pSender->IsEnabled()) {
                    AddTaskToQueue([this, sliderValue]() { SetConfigValue(static_cast<float>(sliderValue)); }, "OnValueChanged", true);
                }
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_VALUECHANGED_MOVE) {
        if (msg.pSender->GetName() == _T("SliderConfig")) {
            auto slider = static_cast<CSliderUI *>(msg.pSender);
            auto edit   = m_PaintManager.FindControl<CEditUI *>(_T("EditConfig"));
            if (edit) {
                int sliderValue = slider->GetValue();
                edit->SetText(std::to_wstring(sliderValue).data());
                SetConfigValue(static_cast<float>(sliderValue), false);
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_RETURN) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            // 限制Edit的输入范围
            auto         edit         = static_cast<CEditUI *>(msg.pSender);
            std::wstring text         = edit->GetText();
            auto         currentValue = _wtof(text.data());
            if (currentValue < mConfigLimits.at(mConfigType).first) {
                currentValue       = mConfigLimits.at(mConfigType).first;
                std::wstring limit = std::to_wstring(currentValue);
                edit->SetText(limit.c_str());
            } else if (currentValue > mConfigLimits.at(mConfigType).second) {
                currentValue       = mConfigLimits.at(mConfigType).second;
                std::wstring limit = std::to_wstring(currentValue);
                edit->SetText(limit.c_str());
            }
            // 重新获取值
            text         = edit->GetText();
            currentValue = _wtof(text.data());
            spdlog::debug("EditConfigSetValue: {}", currentValue);

            // 设置slider 值
            auto slider = m_PaintManager.FindControl<CSliderUI *>(_T("SliderConfig"));
            if (slider) {
                slider->SetValue(static_cast<int>(std::round(currentValue)));
            }

            // 设置超声板数值
            SetConfigValue(static_cast<float>(currentValue));
        }
    } else if (msg.sType == DUI_MSGTYPE_MOUSEWHELL) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            auto         edit         = static_cast<CEditUI *>(msg.pSender);
            auto         currentValue = _wtof(edit->GetText());
            std::wstring text         = edit->GetText();
            if (LOWORD(msg.wParam)) {
                currentValue -= mConfigStep.at(mConfigType);
            } else {
                currentValue += mConfigStep.at(mConfigType);
            }
            if (currentValue < mConfigLimits.at(mConfigType).first) {
                currentValue = mConfigLimits.at(mConfigType).first;
            } else if (currentValue > mConfigLimits.at(mConfigType).second) {
                currentValue = mConfigLimits.at(mConfigType).second;
            }
            text = std::to_wstring(currentValue);
            edit->SetText(text.c_str());
            // 设置slider 值
            auto slider = m_PaintManager.FindControl<CSliderUI *>(_T("SliderConfig"));
            if (slider) {
                slider->SetValue(static_cast<int>(std::round(currentValue)));
            }

            spdlog::debug("Mouse Wheel config value: {}", currentValue);

            // 设置超声板数值
            SetConfigValue(static_cast<float>(currentValue));
        }
    }

    CDuiWindowBase::Notify(msg);
}

void MainFrameWnd::OnLButtonDown(UINT nFlags, ::CPoint pt) {
    POINT point{pt.x, pt.y};
    auto  wnd = dynamic_cast<CWindowUI *>(m_PaintManager.FindControl(point));
    if (wnd) {
        if (wnd->GetName() == _T("WndOpenGL_ASCAN")) {
            m_OpenGL_ASCAN.OnLButtonDown(nFlags, pt);
        } else if (wnd->GetName() == _T("WndOpenGL_CSCAN")) {
            m_OpenGL_CSCAN.OnLButtonDown(nFlags, pt);
        }
    }

    if (mWidgetMode == WidgetMode::MODE_REVIEW && mFragmentReview->size() > 0 && PointInRect(m_pWndOpenGL_CSCAN->GetPos(), pt)) {
        auto temp = pt;
        temp.x -= m_pWndOpenGL_CSCAN->GetX();
        temp.y -= m_pWndOpenGL_CSCAN->GetY();
        int index = (int)(std::round((double)mFragmentReview->size() * (double)temp.x / (double)m_pWndOpenGL_CSCAN->GetWidth()));
        if (index >= mFragmentReview->size()) {
            index = mFragmentReview->size() - 1;
        } else if (index < 0) {
            index = 0;
        }
        auto    edit = m_PaintManager.FindControl<CEditUI *>(L"EditCScanSelect");
        CString str;
        str.Format(L"第 %d 帧 / 共 %d 帧", mFragmentReview->getCurFragment(), mFragmentReview->fragments());
        edit->SetText(str);
        edit = m_PaintManager.FindControl<CEditUI *>(L"EditCScanIndexSelect");
        str.Format(L"第 %d 个点 / 共 %d 个点", index + 1, mFragmentReview->size());
        mFragmentReview->setCursor(index);
        edit->SetText(str);

        auto &bridge = (*mFragmentReview)[index];

        // 扫查通道
        for (int i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
            auto mesh  = m_OpenGL_ASCAN.getMesh<MeshAscan *>(i);
            auto cMesh = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(i);
            // 绘制当前点击的线条
            cMesh->AppendLine(temp.x);
            // 绘制A扫图
            mesh->hookAScanData(std::make_shared<std::vector<uint8_t>>(bridge.mScanOrm.mScanData[i]->pAscan));
            // 绘制波门
            mesh->UpdateGate(0, 1, bridge.mScanOrm.mScanGateAInfo[i].pos, bridge.mScanOrm.mScanGateAInfo[i].width,
                             bridge.mScanOrm.mScanGateAInfo[i].height);
            mesh->UpdateGate(1, 1, bridge.mScanOrm.mScanGateBInfo[i].pos, bridge.mScanOrm.mScanGateBInfo[i].width,
                             bridge.mScanOrm.mScanGateBInfo[i].height);
            mesh->UpdateGate(2, 1, bridge.mScanOrm.mScanGateInfo[i].pos, bridge.mScanOrm.mScanGateInfo[i].width,
                             bridge.mScanOrm.mScanGateInfo[i].height);
            mesh->SetLimits(bridge.mScanOrm.mScanData[i]->aScanLimits[0], bridge.mScanOrm.mScanData[i]->aScanLimits[1]);
            std::array<HDBridge::HB_ScanGateInfo, 3> scanGate = {
                bridge.mScanOrm.mScanGateAInfo[i],
                bridge.mScanOrm.mScanGateBInfo[i],
                bridge.mScanOrm.mScanGateInfo[i],
            };

            for (int j = 0; j < 3; j++) {
                auto [pos, max, res] = HDBridge::computeGateInfo(bridge.mScanOrm.mScanData[i]->pAscan, scanGate[j]);
                auto depth           = bridge.mScanOrm.mScanData[i]->aScanLimits[1];
                if (res) {
                    mesh->SetGateData(std::make_pair(pos * depth, max / 2.55f), j);
                } else {
                    mesh->SetGateData(j);
                }
            }
        }
        // 测厚通道
        for (size_t i = 0; i < 4; i++) {
            auto mesh  = m_OpenGL_ASCAN.getMesh<MeshAscan *>(HDBridge::CHANNEL_NUMBER + i);
            auto cMesh = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(HDBridge::CHANNEL_NUMBER + i);
            // 绘制当前点击的线条
            cMesh->AppendLine(temp.x);
            // 绘制A扫图
            mesh->hookAScanData(std::make_shared<std::vector<uint8_t>>(bridge.mScanOrm.mScanData[i]->pAscan));
            // 绘制波门
            mesh->UpdateGate(0, 1, bridge.mScanOrm.mScanGateAInfo[i].pos, bridge.mScanOrm.mScanGateAInfo[i].width,
                             bridge.mScanOrm.mScanGateAInfo[i].height);
            mesh->UpdateGate(1, 1, bridge.mScanOrm.mScanGateBInfo[i].pos, bridge.mScanOrm.mScanGateBInfo[i].width,
                             bridge.mScanOrm.mScanGateBInfo[i].height);
            mesh->UpdateGate(2, 1, bridge.mScanOrm.mScanGateInfo[i + HDBridge::CHANNEL_NUMBER].pos,
                             bridge.mScanOrm.mScanGateInfo[i + HDBridge::CHANNEL_NUMBER].width,
                             bridge.mScanOrm.mScanGateInfo[i + HDBridge::CHANNEL_NUMBER].height);
            mesh->SetLimits(bridge.mScanOrm.mScanData[i]->aScanLimits[0], bridge.mScanOrm.mScanData[i]->aScanLimits[1]);

            std::array<HDBridge::HB_ScanGateInfo, 3> scanGate = {
                bridge.mScanOrm.mScanGateAInfo[i],
                bridge.mScanOrm.mScanGateBInfo[i],
                bridge.mScanOrm.mScanGateInfo[i + HDBridge::CHANNEL_NUMBER],
            };
            for (int j = 0; j < 3; j++) {
                auto [pos, max, res] = HDBridge::computeGateInfo(bridge.mScanOrm.mScanData[i]->pAscan, scanGate[j]);
                if (res) {
                    auto depth = bridge.mScanOrm.mScanData[i]->aScanLimits[1];
                    mesh->SetGateData(std::make_pair(pos * depth, max / 2.55f), j);
                } else {
                    mesh->SetGateData(j);
                }
            }
            mesh->SetTickness(bridge.mScanOrm.mThickness[i]);
        }
    }
}

void MainFrameWnd::OnLButtonDClick(UINT nFlags, ::CPoint pt) {
    if (PointInRect(m_pWndOpenGL_ASCAN->GetPos(), pt)) {
        auto temp = pt;
        temp.x -= m_pWndOpenGL_ASCAN->GetX();
        temp.y -= m_pWndOpenGL_ASCAN->GetY();
        if (mWidgetMode == WidgetMode::MODE_SCAN) {
            for (const auto &[index, ptr] : m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh) {
                if (index >= static_cast<size_t>(mCurrentGroup * 4) && index < static_cast<size_t>((mCurrentGroup + 1) * 4) &&
                    ptr->IsInArea(temp)) {
                    KillUITimer();
                    // 如果正在扫查则停止扫查
                    if (mScanningFlag == true) {
                        StopScan(false);
                    }
                    spdlog::debug("double click: {}", swapAScanIndex(static_cast<int>(index)));
                    mUtils->pushCallback();
                    // 移交所有权
                    ChannelSettingWnd *wnd = new ChannelSettingWnd(std::move(mUtils), swapAScanIndex(static_cast<int>(index)));
                    wnd->Create(m_hWnd, wnd->GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
                    wnd->CenterWindow();
                    wnd->ShowModal();
                    // 移回所有权
                    mUtils = std::move(wnd->returnHDUtils());
                    delete wnd;
                    mUtils->popCallback();
                    ResumeUITimer();
                    // 如果正在扫查则重新开始扫查
                    if (mScanningFlag == true) {
                        StartScan(false);
                    }
                }
            }
        } else {
            // 列出缺陷列表
            DefectsListWnd wnd;
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.LoadDefectsList(mDetectInfo.time);
            wnd.ShowModal();
            auto &[res, index, channel] = wnd.getResult();
            if (res) {
                OnBtnSelectGroupClicked(channel / 4);
                mFragmentReview->setCurFragment(index / FragmentReview::SIZE_PER_FRAGMENT);
                if (mFragmentReview->begin() != mReviewData.begin() || mFragmentReview->end() != mReviewData.end()) {
                    DrawReviewCScan();
                }
                auto pt = GetCScainIndexPt(index - 1);
                OnLButtonDown(1, pt);
            }
        }
    }
}

void MainFrameWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case CSCAN_UPDATE: {
            UpdateCScanOnTimer();
            break;
        }
        default: break;
    }
}

void MainFrameWnd::EnterReview(std::string path) {
    mReviewPathEntry = path;
}

::CPoint MainFrameWnd::GetCScainIndexPt(int index) const {
    auto     width  = m_pWndOpenGL_CSCAN->GetWidth();
    auto     height = m_pWndOpenGL_CSCAN->GetHeight();
    ::CPoint pt;
    pt.x = (long)(m_pWndOpenGL_CSCAN->GetX() +
                  (float)width * ((float)(index % FragmentReview::SIZE_PER_FRAGMENT) / (float)mFragmentReview->size()));
    pt.y = (long)(m_pWndOpenGL_CSCAN->GetY() + height / 2);
    return pt;
}

void MainFrameWnd::ReconnectBoard(int type) {
    auto bridge    = mUtils->getBridge();
    auto newBridge = GenerateHDBridge<TOFDMultiPort>(*bridge, type);
    bridge->close();
    mUtils->setBridge(newBridge);
    bridge = mUtils->getBridge();
    bridge->open();
    bridge->syncCache2Board();
}

void MainFrameWnd::ReconnectBoard(std::string ip_FPGA, uint16_t port_FPGA, std::string ip_PC, uint16_t port_PC) {
    auto bridge    = mUtils->getBridge();
    auto newBridge = GenerateHDBridge<NetworkMulti>(*bridge, ip_FPGA, port_FPGA, ip_PC, port_PC);
    bridge->close();
    mUtils->setBridge(newBridge);
    bridge = mUtils->getBridge();
    bridge->open();
    bridge->syncCache2Board();
}

void MainFrameWnd::ThreadCScan(void) {
    while (1) {
        std::unique_lock lock(mCScanMutex);
        mCScanNotify.wait(lock);
        if (!mCScanThreadRunning) {
            break;
        }

        // 更新C扫
        std::array<std::shared_ptr<HDBridge::NM_DATA>, HDBridge::CHANNEL_NUMBER> scanData = mUtils->mScanOrm.mScanData;
        for (auto &it : scanData) {
            if (it != nullptr && it->pAscan.size() > 0) {
                auto mesh = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(it->iChannel);
                if (mUtils->getCache().scanGateInfo[it->iChannel].width != 0.0f) {
                    auto     &sacnGateInfo = mUtils->getCache().scanGateInfo[it->iChannel];
                    auto      start        = (double)sacnGateInfo.pos;
                    auto      end          = (double)sacnGateInfo.pos + (double)sacnGateInfo.width;
                    auto      left         = std::begin(it->pAscan) + static_cast<size_t>(start * (double)it->pAscan.size());
                    auto      right        = std::begin(it->pAscan) + static_cast<size_t>(end * (double)it->pAscan.size());
                    auto      max          = std::max_element(left, right);
                    glm::vec4 color        = {};
                    if (*max > it->pGateAmp[1]) {
                        color = {1.0f, 0.f, 0.f, 1.0f};
                    } else {
                        color = {1.0f, 1.0f, 1.0f, 1.0f};
                    }
                    if (*max > 255 / 4) {
                        mDefectJudgmentValue[it->iChannel] = 1;
                    } else {
                        mDefectJudgmentValue[it->iChannel] = 0;
                    }
                    mesh->AppendDot(*max, color);
                }
            }
        }
        // 测厚
        for (uint32_t i = 0ull; i < 4ull; i++) {
            if (mUtils->getCache().scanGateInfo[(size_t)HDBridge::CHANNEL_NUMBER + i].width > 0.0001f) {
                auto   mesh         = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>((size_t)HDBridge::CHANNEL_NUMBER + i);
                double baseTickness = _wtof(mDetectInfo.thickness.c_str());
                if (baseTickness != 0.0f && baseTickness != -HUGE_VAL && baseTickness != HUGE_VAL) {
                    auto relative_error = (mUtils->mScanOrm.mThickness[i] - baseTickness) / baseTickness;
                    if (relative_error > RELATIVE_ERROR_MAX) {
                        relative_error = RELATIVE_ERROR_MAX;
                    } else if (relative_error < -RELATIVE_ERROR_MAX) {
                        relative_error = -RELATIVE_ERROR_MAX;
                    }
                    glm::vec4 color = {};
                    if (relative_error > RELATIVE_ERROR_THRESHOLD) {
                        color = {.0f, 0.f, 1.f, 1.0f};
                    } else if (relative_error < -RELATIVE_ERROR_THRESHOLD) {
                        color = {1.0f, 0.f, 0.f, 1.0f};
                    } else {
                        color = {.0f, 1.f, 0.f, 1.0f};
                    }
                    uint8_t value = (((uint8_t)std::round((double)RELATIVE_ERROR_BASE * std::abs(relative_error / RELATIVE_ERROR_MAX))) &
                                     RELATIVE_ERROR_BASE);
                    if (relative_error >= 0) {
                        value += RELATIVE_ERROR_BASE;
                    } else {
                        value = RELATIVE_ERROR_BASE - value;
                    }
                    mesh->AppendDot(value, color);
                }
            }
        }
        SaveScanData();
    }
}

void MainFrameWnd::OnBtnSelectGroupClicked(long index) {
    if (index == mCurrentGroup) {
        return;
    }
    mCurrentGroup = index;

    // 设置通道选择的Text
    for (int i = 0; i < 4; i++) {
        CString name;
        name.Format(_T("OptChannel%d"), i);
        auto opt = static_cast<COptionUI *>(m_PaintManager.FindControl(name));
        if (opt) {
            CString index;
            index.Format(_T("%d"), (mCurrentGroup * 4 + i) % 12 + 1);
            opt->SetText(index.GetString());
        }
    }
    // 设置波门类型
    if (index == 3) {
        auto layout = m_PaintManager.FindControl<CHorizontalLayoutUI *>(L"LayoutGateType");
        auto opt    = static_cast<COptionUI *>(layout->FindSubControl(L"OptGateType"));
        opt->SetText(L"测厚波门");
    } else {
        auto layout = m_PaintManager.FindControl<CHorizontalLayoutUI *>(L"LayoutGateType");
        auto opt    = static_cast<COptionUI *>(layout->FindSubControl(L"OptGateType"));
        opt->SetText(L"扫查波门");
    }

    // 设置选项按钮的颜色
    for (long i = 0; i < BTN_SELECT_GROUP_MAX; i++) {
        CString str;
        str.Format(_T("BtnSelectGroup%d"), i);
        auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(str));
        if (btn) {
            if (i != index) {
                btn->SetBkColor(0xFFEEEEEE);
            } else {
                btn->SetBkColor(0xFF339933);
            }
        }
    }
    m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->SetViewGroup(mCurrentGroup);
    m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->SetViewGroup(mCurrentGroup);
}

void MainFrameWnd::SaveDefectStartID(int channel) {
    ORM_Model::ScanRecord scanRecord = {};
    scanRecord.startID               = mRecordCount + (int)mReviewData.size();
    scanRecord.channel               = channel;
    mScanRecordCache.push_back(scanRecord);
    mIDDefectRecord[channel] = (int)mScanRecordCache.size() - 1;
}

void MainFrameWnd::SaveDefectEndID(int channel) {
    if (mScanRecordCache.size() == 0) {
        return;
    }
    auto &scanRecord = mScanRecordCache.at(mIDDefectRecord[channel]);
    if (mRecordCount + (int)mReviewData.size() < scanRecord.startID) {
        return;
    }
    scanRecord.endID = mRecordCount + (int)mReviewData.size();
}

void MainFrameWnd::CheckAndUpdate(bool showNoUpdate) {
#if !defined(_DEBUG) && APP_CHECK_UPDATE
    if (!GetSystemConfig().checkUpdate) {
        return;
    }
    auto [tag, body, url] = GetLatestReleaseNote("https://api.github.com/repos/mengyou1024/roc-master-mfc/releases/latest");
    if (Check4Update(APP_VERSION, tag)) {
        std::wstring wBody   = WStringFromString(body);
        std::wstring wTitle  = std::wstring(L"更新可用:") + WStringFromString(tag);
        std::wstring message = std::wstring(_T(APP_VERSION)) + L"--->" + WStringFromString(tag) + L"\n\n是:立即下载\n否:不更新";
        auto         ret     = DMessageBox(message.data(), wTitle.data(), MB_YESNO);
        spdlog::info("更新可用:\ntag: {}\n body: {} \n url: {}", tag, body, url);
        if (ret == 0x6) {
            spdlog::debug("ret = {}", ret);
            auto result = std::async([url]() -> CURLcode {
                CURLcode result = CURL_LAST;
                FILE    *fp     = fopen("./newVersion.exe", "wb");
                CURL    *curl   = curl_easy_init();
                auto     config = GetSystemConfig();
                if (config.enableProxy) {
                    curl_easy_setopt(curl, CURLOPT_PROXY, StringFromWString(config.httpProxy).c_str());
                    curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
                    curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
                }
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
                result = curl_easy_perform(curl);
                fclose(fp);
                curl_easy_cleanup(curl);
                return result;
            });
            if (result.wait_for(std::chrono::milliseconds(60000)) == std::future_status::timeout) {
                DMessageBox(L"下载文件超时.");
                spdlog::warn("下载文件超时.");
                return;
            }

            if (result.get() == CURLE_OK) {
                if (DMessageBox(L"新版本已下载，是否立即更新", L"关闭并更新", MB_YESNO) != 0x06) {
                    return;
                }
            }
            g_MainProcess.RegistFuncOnDestory([]() -> void {
                TCHAR path[MAX_PATH];
                ZeroMemory(path, MAX_PATH);
                GetModuleFileName(NULL, path, MAX_PATH);
                CString strPath = path;
                int     pos     = strPath.ReverseFind('\\');
                strPath         = strPath.Left(pos);
                CString strDir  = strPath;
                strPath += _T("\\newVersion.exe");
                ShellExecute(NULL, _T("open"), strPath, NULL, strDir, SW_HIDE);
            });
            Close();
        }
    } else {
        if (showNoUpdate) {
            DMessageBox(L"当前已是最新版本");
        }
    }
#endif
}

void MainFrameWnd::SaveScanData() {
    // 保存当前波门的位置信息
    mUtils->mScanOrm.mScanGateInfo = mUtils->getCache().scanGateInfo;
    for (size_t i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
        mUtils->mScanOrm.mScanGateAInfo[i] = mUtils->getCache().gateInfo[i];
        mUtils->mScanOrm.mScanGateBInfo[i] = mUtils->getCache().gate2Info[i];
    }
    // 保存C扫的坐标信息
    auto [minLimit, maxLimit]        = m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->GetAxisRange();
    mUtils->mScanOrm.mCScanLimits[0] = minLimit;
    mUtils->mScanOrm.mCScanLimits[1] = maxLimit;
    // 保存扫查数据
    if (mReviewData.size() >= SCAN_RECORD_CACHE_MAX_ITEMS) {
        std::vector<HD_Utils> copyData = mReviewData;
        // 线程中将扫查数据保存
        std::string savePath = mSavePath;
        AddTaskToQueue([savePath, copyData]() { HD_Utils::storage(savePath).insert_range(copyData.begin(), copyData.end()); });
        mRecordCount += (int)mReviewData.size();
        mReviewData.clear();
    } else {
        mReviewData.push_back(*mUtils);
    }
    auto &res = mDetectionSM.UpdateData(mDefectJudgmentValue);
    for (int i = 0; i < res.size(); i++) {
        if (res[i] == DetectionStateMachine::DetectionStatus::Rasing) {
            SaveDefectStartID(i);
        } else if (res[i] == DetectionStateMachine::DetectionStatus::Falling) {
            SaveDefectEndID(i);
        }
    }
}

static constexpr std::array<std::pair<std::wstring_view, bool>, 4> LayoutStatusChange = {
    std::make_pair(L"LayoutParamSetting", false),
    std::make_pair(L"LayoutFunctionButton", false),
    std::make_pair(L"LayoutReviewExt", true),
    std::make_pair(L"LayoutCScanSelect", true),
};

bool MainFrameWnd::EnterReviewMode(std::string name) {
    try {
        auto tick = GetTickCount64();
        // 存放回调函数
        if (mWidgetMode != WidgetMode::MODE_REVIEW) {
            mUtils->pushCallback();
        }
        // 保存配置信息备份
        mDetectInfoBak    = mDetectInfo;
        auto systemConfig = GetSystemConfig();
        mJobGroupNameBak  = systemConfig.groupName;
        // 读取并加载数据
        mDetectInfo            = ORM_Model::DetectInfo::storage(name).get<ORM_Model::DetectInfo>(1);
        systemConfig.groupName = ORM_Model::JobGroup::storage(name).get<ORM_Model::JobGroup>(1).groupName;
        mReviewData            = HD_Utils::storage(name).get_all<HD_Utils>();
        try {
            mDefectInfo = ORM_Model::DefectInfo::storage(name).get_all<ORM_Model::DefectInfo>();
        } catch (std::exception &) { spdlog::warn("文件中没有探伤信息"); }
        SelectMeasureThickness(mDetectInfo.enableMeasureThickness);
        UpdateSystemConfig(systemConfig);
        mFragmentReview = std::make_unique<FragmentReview>(mReviewData);
        DrawReviewCScan();
        OnLButtonDown(1, GetCScainIndexPt(0));

        // 界面布局切换为回放模式
        for (auto &UI_N : LayoutStatusChange) {
            auto &[ui, val] = UI_N;
            auto layout     = m_PaintManager.FindControl<CHorizontalLayoutUI *>(ui.data());
            layout->SetVisible(val);
        }
        mWidgetMode = WidgetMode::MODE_REVIEW;
        spdlog::info("load:{}, frame:{}", name, mReviewData.size());
        spdlog::info("takes time: {} ms", GetTickCount64() - tick);
        return true;
    } catch (std::exception &e) {
        spdlog::error(e.what());
        mUtils->popCallback();
        return false;
    }
}

void MainFrameWnd::ExitReviewMode() {
    if (mWidgetMode == WidgetMode::MODE_REVIEW) {
        mUtils->popCallback();
    }
    // 清除回放数据
    mReviewData.clear();
    mFragmentReview = nullptr;

    // 界面布局切换为扫查模式
    for (auto &UI_N : LayoutStatusChange) {
        auto &[ui, val] = UI_N;
        auto layout     = m_PaintManager.FindControl<CHorizontalLayoutUI *>(ui.data());
        layout->SetVisible(!val);
    }

    // 清除C扫图像, 重新设置扫查波门的位置
    for (int i = 0; i < HDBridge::CHANNEL_NUMBER + 4; i++) {
        auto mesh  = m_OpenGL_ASCAN.getMesh<MeshAscan *>(i);
        auto cMesh = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(i);
        cMesh->RemoveLine();
        cMesh->RemoveDot();
        const auto &[pos, width, height] = mUtils->getBridge()->getScanGateInfo(i);
        mesh->UpdateGate(2, 1, pos, width, height);
    }

    // 恢复备份的配置
    mDetectInfo            = mDetectInfoBak;
    auto systemConfig      = GetSystemConfig();
    systemConfig.groupName = mJobGroupNameBak;
    UpdateSystemConfig(systemConfig);
    SelectMeasureThickness(GetSystemConfig().enableMeasureThickness);
    mWidgetMode = WidgetMode::MODE_SCAN;
}

void MainFrameWnd::SelectMeasureThickness(bool enableMeasure) {
    if (enableMeasure) {
        auto btn = m_PaintManager.FindControl<CButtonUI *>(L"BtnSelectGroup0");
        btn->SetVisible(false);
        btn = m_PaintManager.FindControl<CButtonUI *>(L"BtnSelectGroup3");
        btn->SetVisible(true);
        if (mCurrentGroup == 0) {
            UpdateSliderAndEditValue(3, mConfigType, mGateType, mChannelSel, true);
        }
    } else {
        auto btn = m_PaintManager.FindControl<CButtonUI *>(L"BtnSelectGroup0");
        btn->SetVisible(true);
        btn = m_PaintManager.FindControl<CButtonUI *>(L"BtnSelectGroup3");
        btn->SetVisible(false);
        if (mCurrentGroup == 3) {
            UpdateSliderAndEditValue(0, mConfigType, mGateType, mChannelSel, true);
        }
    }
}

void MainFrameWnd::StartScan(bool changeFlag) {
    if (mWidgetMode != WidgetMode::MODE_SCAN) {
        return;
    }

    if (!changeFlag) {
        if (mScanningFlag == true) {
            SetTimer(CSCAN_UPDATE, 1000 / mSamplesPerSecond);
        }
        return;
    }
    if (mScanningFlag == false) {
        // 保存当前时间
        std::stringstream                     buffer = {};
        std::chrono::system_clock::time_point t      = std::chrono::system_clock::now();
        time_t                                tm     = std::chrono::system_clock::to_time_t(t);
        buffer << std::put_time(localtime(&tm), "%Y-%m-%d__%H-%M-%S");
        mDetectInfo.time                   = buffer.str();
        mDetectInfo.enableMeasureThickness = GetSystemConfig().enableMeasureThickness;

        mDefectJudgmentValue.fill(0);
        std::regex  reg(R"((\d+)-(\d+)-(\d+)__(.+))");
        std::smatch match;
        if (std::regex_match(mDetectInfo.time, match, reg)) {
            auto year           = match[1].str();
            auto month          = match[2].str();
            auto day            = match[3].str();
            auto tm             = match[4].str();
            mScanTime.yearMonth = year + month;
            mScanTime.day       = day;
            mScanTime.time      = tm;
            auto path           = string(SCAN_DATA_DIR_NAME + GetJobGroup() + "/") + mScanTime.yearMonth + "/" + day;
            std::replace(path.begin(), path.end(), '/', '\\');
            CreateMultipleDirectory(WStringFromString(path).data());
            path += "\\" + tm + APP_SCAN_DATA_SUFFIX;
            mSavePath = path;
            // 创建表
            try {
                HD_Utils::storage(path).sync_schema();
                // 探伤信息
                ORM_Model::DetectInfo::storage(path).sync_schema();
                ORM_Model::DetectInfo::storage(path).insert(mDetectInfo);
                ORM_Model::DetectInfo::storage(path).vacuum();
                // 用户信息
                ORM_Model::User::storage(path).sync_schema();
                ORM_Model::User user;
                user.name = GetSystemConfig().userName;
                ORM_Model::User::storage(path).insert(user);
                ORM_Model::User::storage(path).vacuum();
                // 班组信息
                ORM_Model::JobGroup::storage(path).sync_schema();
                ORM_Model::JobGroup jobgroup = {};
                jobgroup.groupName           = GetSystemConfig().groupName;
                ORM_Model::JobGroup::storage(path).insert(jobgroup);
                ORM_Model::JobGroup::storage(path).vacuum();
                // 扫查数据
                ORM_Model::ScanRecord::storage(path).sync_schema();
                mReviewData.clear();
                mRecordCount = 0;
                mScanRecordCache.clear();
                mScanningFlag = true;
                SetTimer(CSCAN_UPDATE, 1000 / mSamplesPerSecond);
            } catch (std::exception &e) {
                spdlog::warn(GB2312ToUtf8(e.what()));
                DMessageBox(L"请勿快速点击扫查按钮");
            }
        }
    }
}

void MainFrameWnd::StopScan(bool changeFlag) {
    if (mWidgetMode != WidgetMode::MODE_SCAN) {
        return;
    }
    if (!changeFlag) {
        if (mScanningFlag == true) {
            KillTimer(CSCAN_UPDATE);
        }
        return;
    }
    if (mScanningFlag == true) {
        mScanningFlag = false;
        KillTimer(CSCAN_UPDATE);
        Sleep(10);
        for (int i = 0; i < HDBridge::CHANNEL_NUMBER + 4; i++) {
            auto meshAScan = m_OpenGL_ASCAN.getMesh<MeshAscan *>(i);
            auto meshCScan = m_OpenGL_CSCAN.getMesh<MeshGroupCScan *>(i);
            meshCScan->RemoveLine();
            meshCScan->RemoveDot();
        }
        mDefectJudgmentValue.fill(0);
        // 保存缺陷记录
        ORM_Model::ScanRecord::storage(mSavePath).insert_range(mScanRecordCache.begin(), mScanRecordCache.end());
        ORM_Model::ScanRecord::storage(mSavePath).vacuum();
        // 保存扫查数据
        HD_Utils::storage(mSavePath).insert_range(mReviewData.begin(), mReviewData.end());
        HD_Utils::storage(mSavePath).vacuum();
        // 保存缺陷数据
        ORM_Model::DefectInfo::storage(mSavePath).sync_schema();
        ORM_Model::DefectInfo::storage(mSavePath).insert_range(mDefectInfo.begin(), mDefectInfo.end());
        ORM_Model::DefectInfo::storage(mSavePath).vacuum();
        // 清除扫查数据
        mDefectInfo.clear();
        mReviewData.clear();
        mRecordCount = 0;
        mScanRecordCache.clear();
    }
}
