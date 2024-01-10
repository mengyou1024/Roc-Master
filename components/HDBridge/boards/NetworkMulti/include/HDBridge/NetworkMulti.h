#pragma once

#include "../NetworkMulti.h"
#include "HDBridge.h"
#include <cstdint>
#include <string>

using std::string;
class NetworkMulti : public HDBridge {
private:
    int      axle       = {}; ///< 轴号
    int      m_index    = 1;  ///< 板卡编号
    string   m_ipFPGA   = {};
    string   m_ipPC     = {};
    uint16_t m_portFPGA = {};
    uint16_t m_portPC   = {};

public:
    NetworkMulti() {
        m_ipFPGA   = "192.168.1.110";
        m_portFPGA = 3500;
        m_ipPC     = "192.168.1.100";
        m_portPC   = 4200;
    }
    NetworkMulti(NetworkMulti &&) = delete;
    NetworkMulti(string ipFPGA, uint16_t portFPGA = 3500, string ipPC = "192.168.1.100", uint16_t portPC = 4200)
        : m_ipFPGA(ipFPGA), m_portFPGA(portFPGA), m_ipPC(ipPC), m_portPC(portPC) {
    }
    virtual ~NetworkMulti() {}

    virtual bool open() override;
    virtual bool isOpen() override;
    virtual bool close() override;
    virtual bool isDeviceExist() override;

    virtual bool setFrequency(int freq) override;
    virtual bool setVoltage(HB_Voltage voltage) override;
    virtual bool setChannelFlag(uint32_t flag) override;
    virtual bool setScanIncrement(int scanIncrement) override;
    virtual bool setLED(int ledStatus) override;
    virtual bool setDamperFlag(int damperFlag) override;
    virtual bool setEncoderPulse(int encoderPulse) override;
    virtual bool setSoundVelocity(int channel, float velocity) override;
    virtual bool setZeroBias(int channel, float zero_us);
    virtual bool setPulseWidth(int channel, float pulseWidth) override;
    virtual bool setDelay(int channel, float delay_us) override;
    virtual bool setSampleDepth(int channel, float sampleDepth) override;
    virtual bool setSampleFactor(int channel, int sampleFactor) override;
    virtual bool setGain(int channel, float gain) override;
    virtual bool setFilter(int channel, HB_Filter filter) override;
    virtual bool setDemodu(int channel, HB_Demodu demodu) override;
    virtual bool setPhaseReverse(int channel, int reverse) override;
    virtual bool setGateInfo(int channel, const HB_GateInfo &info) override;
    virtual bool setGate2Type(int channel, HB_Gate2Type type) override;

    /**
     * @brief 复位编码器
     * @param coder
     * @return true success
     */
    virtual bool resetCoder(int coder) override;
    virtual bool flushSetting() override;

    virtual unique_ptr<HDBridge::NM_DATA> readDatas() override;

    const int getIndex() const;
};
