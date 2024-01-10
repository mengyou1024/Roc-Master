#include "NetworkMulti.h"
#include <HDBridge/NetworkMulti.h>

bool NetworkMulti::open() {
    int _index = UNION_PORT_Open((char *)m_ipFPGA.c_str(), m_portFPGA, (char *)m_ipPC.c_str(), m_portPC);
    if (_index > 0) {
        m_index = _index;
        return true;
    }
    return false;
}

bool NetworkMulti::isOpen() {
    return UNION_PORT_IsOpen(m_index);
}

bool NetworkMulti::close() {
    if (m_index > 0) {
        UNION_PORT_Close(m_index);
        return true;
    }
    return false;
}

bool NetworkMulti::isDeviceExist() {
    return false;
}

bool NetworkMulti::setFrequency(int freq) {
    mCache.frequency = freq;
    return UNION_PORT_SetFrequency(m_index, freq);
}

bool NetworkMulti::setVoltage(HB_Voltage voltage) {
    mCache.voltage = voltage;
    return UNION_PORT_SetVoltage(m_index, static_cast<int>(voltage));
}

bool NetworkMulti::setChannelFlag(uint32_t flag) {
    mCache.channelFlag = flag;
    return UNION_PORT_SetChMode(m_index, flag);
}

bool NetworkMulti::setScanIncrement(int scanIncrement) {
    mCache.scanIncrement = scanIncrement;
    return UNION_PORT_SetScanIncrement(m_index, scanIncrement);
}

bool NetworkMulti::setLED(int ledStatus) {
    mCache.ledStatus = ledStatus;
    return false;
}

bool NetworkMulti::setDamperFlag(int damperFlag) {
    mCache.damperFlag = damperFlag;
    return false;
}

bool NetworkMulti::setEncoderPulse(int encoderPulse) {
    mCache.encoderPulse = encoderPulse;
    return false;
}

bool NetworkMulti::setSoundVelocity(int channel, float velocity) {
    mCache.soundVelocity[channel % CHANNEL_NUMBER] = velocity;
    return true;
}

bool NetworkMulti::setZeroBias(int channel, float zero_us) {
    mCache.zeroBias[channel] = zero_us;
    return UNION_PORT_SetDelay(m_index, channel % CHANNEL_NUMBER, zero_us + mCache.delay[channel % CHANNEL_NUMBER]);
}

bool NetworkMulti::setPulseWidth(int channel, float pulseWidth) {
    this->mCache.pulseWidth[channel % CHANNEL_NUMBER] = pulseWidth;
    return UNION_PORT_SetPulseWidth(m_index, (int)(pulseWidth + 0.5f));
}

bool NetworkMulti::setDelay(int channel, float delay_us) {
    this->mCache.delay[channel % CHANNEL_NUMBER] = delay_us;
    return UNION_PORT_SetDelay(m_index, channel % CHANNEL_NUMBER, delay_us + mCache.zeroBias[channel % CHANNEL_NUMBER]);
}

bool NetworkMulti::setSampleDepth(int channel, float sampleDepth) {
    this->mCache.sampleDepth[channel % CHANNEL_NUMBER] = sampleDepth;
    return UNION_PORT_SetSampleDepth(m_index, channel % CHANNEL_NUMBER, sampleDepth + mCache.zeroBias[channel % CHANNEL_NUMBER]);
}

bool NetworkMulti::setSampleFactor(int channel, int sampleFactor) {
    this->mCache.sampleFactor[channel % CHANNEL_NUMBER] = sampleFactor;
    return UNION_PORT_SetSampleFactor(m_index, channel % CHANNEL_NUMBER, sampleFactor);
}

bool NetworkMulti::setGain(int channel, float gain) {
    this->mCache.gain[channel % CHANNEL_NUMBER] = gain;
    return UNION_PORT_SetGain(m_index, channel % CHANNEL_NUMBER, gain);
}

bool NetworkMulti::setFilter(int channel, HB_Filter filter) {
    this->mCache.filter[channel % CHANNEL_NUMBER] = filter;
    return UNION_PORT_SetFilter(m_index, channel % CHANNEL_NUMBER, static_cast<int>(filter));
}

bool NetworkMulti::setDemodu(int channel, HB_Demodu demodu) {
    this->mCache.demodu[channel % CHANNEL_NUMBER] = demodu;
    return UNION_PORT_SetDemodu(m_index, channel % CHANNEL_NUMBER, static_cast<int>(demodu));
}

bool NetworkMulti::setPhaseReverse(int channel, int reverse) {
    this->mCache.phaseReverse[channel % CHANNEL_NUMBER] = reverse;
    return UNION_PORT_SetPhaseReverse(m_index, channel % CHANNEL_NUMBER, reverse);
}

bool NetworkMulti::setGateInfo(int channel, const HB_GateInfo &info) {
    if (info.pos < 0.0f || info.width < 0.0f) {
        throw std::runtime_error("波门位置或宽度小于0");
    }
    if (info.gate == 0) {
        this->mCache.gateInfo[channel % CHANNEL_NUMBER] = info;
    } else {
        this->mCache.gate2Info[channel % CHANNEL_NUMBER] = info;
    }
    return UNION_PORT_SetGateInfo(m_index, channel % CHANNEL_NUMBER, info.gate, info.active, info.alarmType, info.pos, info.width, info.height);
}

bool NetworkMulti::setGate2Type(int channel, HB_Gate2Type type) {
    this->mCache.gate2Type[channel % CHANNEL_NUMBER] = type;
    return UNION_PORT_SetGate2Type(m_index, channel % CHANNEL_NUMBER, static_cast<int>(type));
}

bool NetworkMulti::resetCoder(int coder) {
    return UNION_PORT_ResetCoder(m_index, coder);
}

bool NetworkMulti::flushSetting() {
    return UNION_PORT_FlushSetting(m_index);
}

unique_ptr<HDBridge::NM_DATA> NetworkMulti::readDatas() {
    auto data = UNION_PORT_ReadDatas(m_index);
    if (!data) {
        return nullptr;
    }

    auto ret        = make_unique<HDBridge::NM_DATA>();
    ret->iChannel   = data->iChannel;
    ret->iPackage   = data->iPackage;
    ret->iAScanSize = data->iAScanSize;
    ret->pAscan.resize(data->iAScanSize);
    memcpy(ret->pAscan.data(), data->pAscan, ret->iAScanSize);
    memcpy(ret->pCoder.data(), data->pCoder, sizeof(int32_t) * 2);
    memcpy(ret->pGatePos.data(), data->pGatePos, sizeof(int32_t) * 2);
    memcpy(ret->pGateAmp.data(), data->pGateAmp, sizeof(uint8_t) * 2);
    memcpy(ret->pAlarm.data(), data->pAlarm, sizeof(int32_t) * 2);
    return ret;
}

const int NetworkMulti::getIndex() const {
    return m_index;
}
