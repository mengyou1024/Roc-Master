#pragma once

#include "../TOFDPort.h"
#include "HDBridge.h"
#include <cstdint>
#include <string>

class TOFDMultiPort : public HDBridge {
private:
    int m_type = 0;

public:
    /**
     * @brief 
     * @param type 0-1 USB 2 网口
    */
    explicit TOFDMultiPort(int type) noexcept;

    virtual ~TOFDMultiPort() = default;

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
     * @return
     * @note param coder is not used
     */
    virtual bool resetCoder(int coder) override;
    virtual bool flushSetting() override;

    [[nodiscard]]
    virtual unique_ptr<HDBridge::NM_DATA> readDatas() override;

    [[nodiscard]]
    TOFDPort::NM_DATA *readRawDatas();
    void               freeRawDatas(TOFDPort::NM_DATA *data);

    virtual bool getCoderValue(int &coder0, int &coder1) override;
    virtual bool getCoderValueZ(int &coderZ0, int &coderZ1, int &coderF0, int &coderF1, int &coderC0, int &coderC1) override;
};
