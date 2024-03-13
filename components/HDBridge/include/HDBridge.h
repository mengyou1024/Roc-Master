#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

#ifdef USE_SQLITE_ORM
    #include <sqlite_orm/sqlite_orm.h>
#endif

#define HDBRIDGE_COPY_GATE 1

class NetworkMulti;
class TOFDMultiPort;

class HDBridge {
    friend class NetworkMulti;
    friend class TOFDMultiPort;

public:
    enum class HB_Voltage : uint32_t {
        Voltage_50V = 0,
        Voltage_100V,
        Voltage_200V,
        Voltage_260V,
    };

    enum class HB_Filter : uint32_t {
        Filter_NONE = 0x00, ///< 不滤波
        Filter_2_5  = 0x01, ///< 2.5MHz
        Filter_5_0  = 0x02, ///< 5.0MHz
    };

    enum class HB_Demodu : uint32_t {
        Demodu_RF = 0x00, ///< 射频
        Demodu_Full,      ///< 全波
        Demodu_Positive,  ///< 正半波
        Demodu_Negative,  ///< 负半波
    };

    struct HB_GateInfo;

    struct HB_ScanGateInfo {
        float pos    = {};
        float width  = {};
        float height = {};

        HB_ScanGateInfo &operator=(const HB_GateInfo &val) {
            pos    = val.pos;
            width  = val.width;
            height = val.height;
            return *this;
        }
    };

    struct HB_GateInfo {
        int   gate      = {};
        int   active    = {};
        int   alarmType = {};
        float pos       = {};
        float width     = {};
        float height    = {};

        operator HB_ScanGateInfo() {
            HB_ScanGateInfo ret = {};
            ret.pos             = pos;
            ret.width           = width;
            ret.height          = height;
            return ret;
        }

        HB_GateInfo &operator=(const HDBridge::HB_ScanGateInfo &val) {
            pos    = val.pos;
            width  = val.width;
            height = val.height;
            return *this;
        }
    };

    enum class HB_Gate2Type : uint32_t {
        Fixed = 0, ///< 固定
        Follow,    ///< 跟随以波门1最高回波为零点
    };

    struct NM_DATA {
#pragma pack(1)
        int32_t                iChannel    = {}; ///< 通道号
        int32_t                iPackage    = {}; ///< 包序列
        int32_t                iAScanSize  = {}; ///< A扫长度
        std::array<int32_t, 2> pCoder      = {}; ///< 编码器值
        std::array<int32_t, 2> pGatePos    = {}; ///< 波门位置
        std::array<int32_t, 2> pAlarm      = {}; ///< 波门报警
        std::array<uint8_t, 2> pGateAmp    = {}; ///< 波门波幅
        uint16_t               reserved    = {}; ///< 保留
        std::array<float, 2>   aScanLimits = {}; ///< A扫图的坐标范围
#pragma pack()
        vector<uint8_t> pAscan = {}; // A扫数据
    };

    constexpr static int CHANNEL_NUMBER = 12; ///< 通道数

#pragma pack(1)
    struct cache_t {
        // cache
        int                                      frequency     = {}; ///< 重复频率
        HB_Voltage                               voltage       = {}; ///< 发射电压
        uint32_t                                 channelFlag   = {}; ///< 通道标志
        int                                      scanIncrement = {}; ///< 扫查增量
        int                                      ledStatus     = {}; ///< LED状态
        int                                      damperFlag    = {}; ///< 阻尼标志
        int                                      encoderPulse  = {}; ///< 编码器脉冲
        std::array<float, CHANNEL_NUMBER>        soundVelocity = {}; ///< 声速
        std::array<float, CHANNEL_NUMBER>        zeroBias      = {}; ///< 零点偏移
        std::array<float, CHANNEL_NUMBER>        pulseWidth    = {}; ///< 脉冲宽度
        std::array<float, CHANNEL_NUMBER>        delay         = {}; ///< 延时
        std::array<float, CHANNEL_NUMBER>        sampleDepth   = {}; ///< 采样深度
        std::array<int, CHANNEL_NUMBER>          sampleFactor  = {}; ///< 采样因子
        std::array<float, CHANNEL_NUMBER>        gain          = {}; ///< 增益
        std::array<HB_Filter, CHANNEL_NUMBER>    filter        = {}; ///< 滤波
        std::array<HB_Demodu, CHANNEL_NUMBER>    demodu        = {}; ///< 检波方式
        std::array<int, CHANNEL_NUMBER>          phaseReverse  = {}; ///< 相位翻转
        std::array<HB_GateInfo, CHANNEL_NUMBER>  gateInfo      = {}; ///< 波门信息
        std::array<HB_GateInfo, CHANNEL_NUMBER>  gate2Info     = {}; ///< 波门2信息
        std::array<HB_Gate2Type, CHANNEL_NUMBER> gate2Type     = {}; ///< 波门2类型

        std::array<HB_ScanGateInfo, CHANNEL_NUMBER + 4> scanGateInfo = {}; ///< 扫查波门
    };
#pragma pack()

private:
    int          mId      = {};
    std::wstring mName    = {};
    bool         mIsValid = {};
    cache_t      mCache   = {};

public:
    HDBridge() {
        for (auto &g : mCache.gate2Info) {
            g.gate = 1;
        }
        mCache.soundVelocity.fill(5920.0f);
    }

    virtual ~HDBridge() = default;

    HDBridge &operator=(const HDBridge &other) {
        mId      = other.mId;
        mName    = other.mName;
        mIsValid = other.mIsValid;
        mCache   = other.mCache;
        return *this;
    }

    virtual const int getId() const noexcept final {
        return mId;
    }

    virtual void setId(int id) final {
        mId = id;
    }

    virtual const std::wstring &getName() const final {
        return mName;
    }

    virtual void setName(std::wstring name) final {
        mName = name;
    }

    virtual const bool &isValid() const final {
        return mIsValid;
    }

    virtual void setValid(bool valid) final {
        mIsValid = valid;
    }

    virtual const cache_t &getCache() const final {
        return mCache;
    }

    virtual cache_t &getCache_ref() final {
        return mCache;
    }

    virtual void setCache(cache_t cache) final {
        mCache = cache;
    }

    virtual bool open() {
        return false;
    }
    virtual bool isOpen() {
        return false;
    }
    virtual bool close() {
        return false;
    }
    virtual bool isDeviceExist() {
        return false;
    }

    virtual bool setFrequency(int freq) {
        return false;
    }
    virtual const int getFrequency() const final {
        return mCache.frequency;
    }

    virtual bool setVoltage(HB_Voltage voltage) {
        return false;
    }
    virtual const HB_Voltage getVoltage() const final {
        return mCache.voltage;
    }

    virtual bool setChannelFlag(uint32_t flag) {
        return false;
    }
    virtual const uint32_t getChannelFlag() const final {
        return mCache.channelFlag;
    }

    virtual bool setScanIncrement(int scanIncrement) {
        return false;
    }
    virtual const int getScanIncrement() const final {
        return mCache.scanIncrement;
    }

    virtual bool setLED(int ledStatus) {
        return false;
    }
    virtual const bool getLED() const final {
        return mCache.ledStatus;
    };

    virtual bool setDamperFlag(int damperFlag) {
        return false;
    }
    virtual const int getDamperFlag() const final {
        return mCache.damperFlag;
    }

    virtual bool setEncoderPulse(int encoderPulse) {
        return false;
    }
    virtual const int getEncoderPulse() const final {
        return mCache.encoderPulse;
    }

    virtual bool setSoundVelocity(int channel, float velocity) {
        return false;
    }
    virtual const float getSoundVelocity(int channel) const final {
        return mCache.soundVelocity[channel % CHANNEL_NUMBER];
    }

    virtual bool setZeroBias(int channel, float zero_us) {
        return false;
    }
    virtual const float getZeroBias(int channel) const final {
        return mCache.zeroBias[channel % CHANNEL_NUMBER];
    }

    virtual bool setPulseWidth(int channel, float pulseWidth) {
        return false;
    }
    virtual const float getPulseWidth(int channel) const final {
        return mCache.pulseWidth[channel % CHANNEL_NUMBER];
    }

    virtual bool setDelay(int channel, float delay_us) {
        return false;
    }
    virtual const float getDelay(int channel) const final {
        return mCache.delay[channel % CHANNEL_NUMBER];
    }
    virtual bool setSampleDepth(int channel, float sampleDepth) {
        return false;
    }
    virtual const float getSampleDepth(int channel) const final {
        return mCache.sampleDepth[channel % CHANNEL_NUMBER];
    }
    virtual bool setSampleFactor(int channel, int sampleFactor) {
        return false;
    }
    virtual const int getSampleFactor(int channel) const final {
        return mCache.sampleFactor[channel % CHANNEL_NUMBER];
    }
    virtual bool setGain(int channel, float gain) {
        return false;
    }
    virtual const float getGain(int channel) const final {
        return mCache.gain[channel % CHANNEL_NUMBER];
    }
    virtual bool setFilter(int channel, HB_Filter filter) {
        return false;
    }
    virtual const HB_Filter getFilter(int channel) const final {
        return mCache.filter[channel % CHANNEL_NUMBER];
    }
    virtual bool setDemodu(int channel, HB_Demodu demodu) {
        return false;
    }
    virtual const HB_Demodu getDemodu(int channel) const final {
        return mCache.demodu[channel % CHANNEL_NUMBER];
    }
    virtual bool setPhaseReverse(int channel, int reverse) {
        return false;
    }
    virtual const int getPhaseReverse(int channel) const final {
        return mCache.phaseReverse[channel % CHANNEL_NUMBER];
    }
    virtual bool setGateInfo(int channel, const HB_GateInfo &info) {
        return false;
    }
    virtual bool setScanGateInfo(int channel, const HB_ScanGateInfo &info, int index = 2) final {
        if (index == 2) {
            mCache.scanGateInfo[channel % (CHANNEL_NUMBER + 4)] = info;
        } else if (index == 1) {
            mCache.gate2Info[channel % CHANNEL_NUMBER] = info;
        } else if (index == 0) {
            mCache.gateInfo[channel % CHANNEL_NUMBER] = info;
        }
        return true;
    }
    virtual const HB_GateInfo getGateInfo(int index, int channel) const final {
        if (index == 0) {
            return mCache.gateInfo[channel % CHANNEL_NUMBER];
        } else {
            return mCache.gate2Info[channel % CHANNEL_NUMBER];
        }
    }
    virtual const HB_ScanGateInfo getScanGateInfo(int channel, int index = 2) const final {
        if (index == 2) {
            return mCache.scanGateInfo[channel % (CHANNEL_NUMBER + 4)];
        } else {
            return static_cast<HB_GateInfo>(getGateInfo(index, channel));
        }
    }
    virtual bool setGate2Type(int channel, HB_Gate2Type type) {
        return false;
    }
    virtual const HB_Gate2Type getGate2Type(int channel) const final {
        return mCache.gate2Type[channel % CHANNEL_NUMBER];
    }
    virtual bool resetCoder(int coder) {
        return false;
    }
    virtual bool flushSetting() {
        return false;
    }

    virtual bool getCoderValue(int &coder0, int &coder1) {
        return false;
    }

    virtual bool getCoderValueZ(int &coderZ0, int &coderZ1, int &coderF0, int &coderF1, int &coderC0, int &coderC1) {
        return false;
    }

    [[nodiscard]]
    virtual unique_ptr<NM_DATA> readDatas() {
        return nullptr;
    }

    virtual void syncCache2Board() final {
        setFrequency(mCache.frequency);
        setVoltage(mCache.voltage);
        setChannelFlag(mCache.channelFlag);
        setScanIncrement(mCache.scanIncrement);
        setLED(mCache.ledStatus);
        setDamperFlag(mCache.damperFlag);
        setEncoderPulse(mCache.encoderPulse);
        for (int i = 0; i < CHANNEL_NUMBER; ++i) {
            setSoundVelocity(i, mCache.soundVelocity[i]);
            setZeroBias(i, mCache.zeroBias[i]);
            setPulseWidth(i, mCache.pulseWidth[i]);
            setDelay(i, mCache.delay[i]);
            setSampleDepth(i, mCache.sampleDepth[i]);
            setSampleFactor(i, mCache.sampleFactor[i]);
            setGain(i, mCache.gain[i]);
            setFilter(i, mCache.filter[i]);
            setDemodu(i, mCache.demodu[i]);
            setPhaseReverse(i, mCache.phaseReverse[i]);
            setGateInfo(i, mCache.gateInfo[i]);
            setGateInfo(i, mCache.gate2Info[i]);
            setGate2Type(i, mCache.gate2Type[i]);
        }
        flushSetting();
    }

    virtual void defaultInit() final {
        setFrequency(1200);
        setVoltage(HB_Voltage::Voltage_100V);
        setChannelFlag(0xFFF0FFF);
        setScanIncrement(0);
        setLED(0);
        setDamperFlag(0xFFF);
        setEncoderPulse(1);
        for (int i = 0; i < CHANNEL_NUMBER; ++i) {
            setPulseWidth(i, 210.f);
            setZeroBias(i, static_cast<float>(distance2time(0.0, i)));
            setDelay(i, static_cast<float>(distance2time(0.0, i)));
            setSampleDepth(i, static_cast<float>(distance2time(200.0, i)));
            setSampleFactor(i, 13);
            setGain(i, 30.f);
            setFilter(i, static_cast<HB_Filter>(3));
            setDemodu(i, HB_Demodu::Demodu_Full);
            setPhaseReverse(i, 0);
            HB_GateInfo info = {
                0, 1, 0, 0.2f, 0.2f, 0.5f};
            setGateInfo(i, mCache.gateInfo[i]);
            info.gate   = 1;
            info.active = 1;
            setGateInfo(i, mCache.gateInfo[i]);
            setGate2Type(i, mCache.gate2Type[i]);
        }
        flushSetting();
    }

    /**
     * @brief 时间转距离
     * @param time_us 微秒时间
     * @param velocity_in_m_per_s 声速(m/s)
     * @return 距离 (mm)
     */
    static constexpr double time2distance(double time_us, double velocity_in_m_per_s) {
        return time_us * velocity_in_m_per_s / 2000.0;
    }

    /**
     * @brief 距离转时间
     * @param distance_mm
     * @param velocity_in_m_per_s
     * @return 时间 (us)
     */
    static constexpr double distance2time(double distance_mm, double velocity_in_m_per_s) {
        if (velocity_in_m_per_s == 0.0) {
            return 0;
        } else {
            return distance_mm * 2000.0 / velocity_in_m_per_s;
        }
    }

    /**
     * @brief 从距离和时间计算声速
     *
     * @param distance_mm 距离
     * @param time_us 时间
     * @return constexpr double 声速(m/s)
     */
    static constexpr double velocityFromDistanceAndTime(double distance_mm, double time_us) {
        return distance_mm * 2000.0 / time_us;
    }

    virtual double time2distance(double time_us, int channel) final {
        return time2distance(time_us, (double)mCache.soundVelocity[channel % CHANNEL_NUMBER]);
    }

    virtual double distance2time(double distance_mm, int channel) final {
        return distance2time(distance_mm, (double)mCache.soundVelocity[channel % CHANNEL_NUMBER]);
    }

    virtual std::pair<double, double> getAxisLimits(int ch) final {
        auto delay = getDelay(ch);
        auto depth = getSampleDepth(ch) + delay;
        auto min   = time2distance(delay, ch);
        auto max   = time2distance(depth, ch);
        return std::make_pair(min, max);
    }

    /**
     * @brief 计算波门信息
     * @param data 扫查数据
     * @param info 波门信息
     * @return [波门内最高波的位置, 最高波的值, false if err]
     */
    static std::tuple<float, uint8_t, bool> computeGateInfo(const std::vector<uint8_t> &data, const HB_ScanGateInfo &info) {
        try {
            double start = (double)info.pos;
            if (data.size() < 100) {
                return std::make_tuple(0.0f, 0, false);
            }
            if (info.width <= 0.001) {
                return std::make_tuple(0.0f, 0, false);
            }
            if (std::abs(start - 1.0) < 0.00001) {
                return std::make_tuple(0.0f, 0, false);
            }
            if (info.pos < 0.0) {
                return std::make_tuple(0.0f, 0, false);
            }
            double end = (double)(info.pos + info.width);
            if (end > 1.0) {
                end = 1.0;
            }
            auto left  = data.begin() + static_cast<int64_t>((double)data.size() * (double)info.pos);
            auto right = data.begin() + static_cast<int64_t>((double)data.size() * (double)(info.pos + info.width));
            auto max   = std::max_element(left, right);
            if (*max == 255) {
                std::vector<decltype(left)> minMaxVec = {};
                for (auto &_left = left, &_right = right; _left != _right; _left++) {
                    if (*_left == 255) {
                        minMaxVec.push_back(_left);
                    }
                }
                max = minMaxVec[0] + minMaxVec.size() / 2;
            }
            auto pos = (float)((double)std::distance(data.begin(), max) / (double)data.size());
            if (pos < 0.0f) {
                return std::make_tuple(0.0f, 0, false);
            }
            return std::make_tuple(pos, *max, true);
        } catch (...) {
            return std::make_tuple(0.0f, 0, false);
        }
    }

    /**
     * @brief 获取波门最高回波的波幅
     * @param data 扫查数据
     * @param channel 通道号
     * @param gateIndex 波门索引
     * @return [Amp, is_ok] Amp is 0-100.f
     */
    std::pair<double, bool> getGateAmp(const std::vector<uint8_t> &data, int channel, int gateIndex) const {
        auto [_, max, res] = computeGateInfo(data, getScanGateInfo(channel, gateIndex));
        return std::make_pair((double)max / 2.55, res);
    }

    /**
     * @brief 计算两个最高波间的距离
     * @param data 波形数组
     * @param range 声程范围
     * @param gateA 第一个波门
     * @param gateB 第二个波门
     * @param abs 计算结果取绝对值
     * @return 距离
     */
    static double computeDistance(const std::vector<uint8_t> &data, double range,
                                  const HB_ScanGateInfo &gateA, const HB_ScanGateInfo &gateB,
                                  bool abs = false) {
        auto [posA, maxA, resA] = computeGateInfo(data, gateA);
        auto [posB, maxB, resB] = computeGateInfo(data, gateB);
        if (resA && resB) {
            if (abs && posA > posB) {
                std::swap(posA, posB);
            }
            return range * (posB - posA);
        }
        return 0.0;
    }

    /**
     * @brief 获取声程范围
     * @param channel 通道号
     * @return [bias, depth] (mm)
     */
    std::pair<double, double> getRangeOfAcousticPath(int channel) {
        return std::make_pair((double)time2distance(getDelay(channel), channel), (double)time2distance(getSampleDepth(channel), channel));
    }

    /*
     * @brief 通道参数复制参数
     * @param src 复制源
     * @param dist 复制目标列表
     */
    virtual void paramCopy(size_t src, std::vector<size_t> dist) {
        if (src >= static_cast<size_t>(CHANNEL_NUMBER)) {
            return;
        }
        auto soundVelocity = mCache.soundVelocity[src];
        auto zeroBias      = mCache.zeroBias[src];
        auto pluseWidth    = mCache.pulseWidth[src];
        auto delay         = mCache.delay[src];
        auto sampleDepth   = mCache.sampleDepth[src];
        auto sampleFactor  = mCache.sampleFactor[src];
        auto gain          = mCache.gain[src];
        auto filter        = mCache.filter[src];
        auto demodu        = mCache.demodu[src];
        auto phaseReverse  = mCache.phaseReverse[src];
#if HDBRIDGE_COPY_GATE
        auto gateInfo     = mCache.gateInfo[src];
        auto gate2Info    = mCache.gate2Info[src];
        auto gate2Type    = mCache.gate2Type[src];
        auto scanGateInfo = mCache.scanGateInfo[src];
#endif

        for (auto i : dist) {
            if (i == src) {
                continue;
            }
            if (i > 0 && i < HDBridge::CHANNEL_NUMBER) {
                mCache.soundVelocity[i] = soundVelocity;
                mCache.zeroBias[i]      = zeroBias;
                mCache.pulseWidth[i]    = pluseWidth;
                mCache.delay[i]         = delay;
                mCache.sampleDepth[i]   = sampleDepth;
                mCache.sampleFactor[i]  = sampleFactor;
                mCache.gain[i]          = gain;
                mCache.filter[i]        = filter;
                mCache.demodu[i]        = demodu;
                mCache.phaseReverse[i]  = phaseReverse;
#if HDBRIDGE_COPY_GATE
                mCache.gateInfo[i]     = gateInfo;
                mCache.gate2Info[i]    = gate2Info;
                mCache.gate2Type[i]    = gate2Type;
                mCache.scanGateInfo[i] = scanGateInfo;
                if (i <= 3) {
                    mCache.scanGateInfo[i + HDBridge::CHANNEL_NUMBER] = scanGateInfo;
                }
#endif
            }
        }

        syncCache2Board();
    }
#ifdef USE_SQLITE_ORM
    #ifndef ORM_DB_NAME
        #define ORM_DB_NAME "HDBridge.db"
    #endif // !ORM_DB_NAME

    static auto storage(std::string fileName) {
        using namespace sqlite_orm;
        return make_storage(fileName,
                            make_table("HDBridge",
                                       make_column("ID", &HDBridge::getId, &HDBridge::setId, primary_key()),
                                       make_column("NAME", &HDBridge::getName, &HDBridge::setName, unique()),
                                       make_column("VALID", &HDBridge::isValid, &HDBridge::setValid),
                                       make_column("CACHE", &HDBridge::getCache, &HDBridge::setCache)));
    }

    static auto storage() {
        return storage(ORM_DB_NAME);
    }
#endif
};

#ifdef USE_SQLITE_ORM
namespace sqlite_orm {
    template <>
    struct type_printer<HDBridge::cache_t> : public blob_printer {};
    template <>
    struct statement_binder<HDBridge::cache_t> {
        int bind(sqlite3_stmt *stmt, int index, const HDBridge::cache_t &value) {
            std::vector<char> blobValue;
            blobValue.resize(sizeof(HDBridge::cache_t));
            blobValue.reserve(sizeof(HDBridge::cache_t));
            memcpy_s(blobValue.data(), blobValue.capacity(), &value, sizeof(HDBridge::cache_t));
            return statement_binder<std::vector<char>>().bind(stmt, index, blobValue);
        }
    };
    template <>
    struct field_printer<HDBridge::cache_t> {
        std::string operator()(const HDBridge::cache_t &value) {
            return {};
        }
    };
    template <>
    struct row_extractor<HDBridge::cache_t> {
        HDBridge::cache_t extract(sqlite3_stmt *stmt, int index) {
            auto              blobPointer = sqlite3_column_blob(stmt, index);
            HDBridge::cache_t value;
            memcpy(&value, blobPointer, sizeof(HDBridge::cache_t));
            return value;
        }
    };
} // namespace sqlite_orm

#endif
