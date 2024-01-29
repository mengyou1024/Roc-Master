#pragma once

#include "DuiWindowBase.h"
#include "OpenGL.h"
#include <FragmentLoader.hpp>
#include <HDBridge.h>
#include <HDBridge/NetworkMulti.h>
#include <HDBridge/TOFDPort.h>
#include <HDBridge/Utils.h>
#include <Model/DefectInfo.h>
#include <Model/DetectInfo.h>
#include <Model/ScanRecord.h>
#include <Model/SystemConfig.h>
#include <Model/UserModel.h>
#include <map>
#include <memory>

#pragma push_macro("GATE_A")
#pragma push_macro("GATE_B")

#undef GATE_A
#undef GATE_B

class MainFrameWnd : public CDuiWindowBase {
private: /* 类型 */
    enum class WidgetMode {
        MODE_SCAN = 0,
        MODE_REVIEW,
    };

    /// 配置类型
    enum class ConfigType {
        DetectRange = 0,
        Gain,
        GateStart,
        GateWidth,
        GateHeight,
    };

    /// 波门类型
    enum class GateType {
        GATE_A = 0,
        GATE_B,
        GATE_SCAN,
    };

    /// 通道选择
    enum class ChannelSel {
        CHANNEL_1 = 0,
        CHANNEL_2,
        CHANNEL_3,
        CHANNEL_4,
    };

    struct GateResult {
        bool result = false;
        operator bool() {
            return result;
        }
        float pos = 0.0f;
        float max = 0.0f;
    };

    struct {
        string yearMonth = {};
        string day       = {};
        string time      = {};
    } mScanTime; ///< 扫查时间

    using FragmentReview     = FragmentLoader<std::vector<HD_Utils>>;
    using ARR_GateRes        = std::array<GateResult, 3>;
    using ARR_GateResCh      = std::array<ARR_GateRes, HDBridge::CHANNEL_NUMBER + 4>;
    using GateResultTimeNote = std::array<uint64_t, HDBridge::CHANNEL_NUMBER>;
    using UPTR_Utils         = std::unique_ptr<HD_Utils>;
    using VEC_Utils          = std::vector<HD_Utils>;
    using UPTR_FragReview    = std::unique_ptr<FragmentReview>;
    using ARRAY_CHNUMB       = std::array<int, HDBridge::CHANNEL_NUMBER>;
    using VEC_ScanRecord     = std::vector<ORM_Model::ScanRecord>;
    using VEC_DefectInfo     = std::vector<ORM_Model::DefectInfo>;

private: /* 常量 */
    // 扫查数据记录缓冲最大数量
    constexpr static int  SCAN_RECORD_CACHE_MAX_ITEMS = 1000;
    constexpr static auto BTN_SELECT_GROUP_MAX        = 4; ///< 按钮选择分组的最大值(分层、横向、纵向、测厚)

    // 测厚相对误差配置
    constexpr static uint8_t RELATIVE_ERROR_BASE      = 0xFF >> 1; ///< 相对误差在C扫图中的基线
    constexpr static double  RELATIVE_ERROR_MAX       = 0.08;      ///< 最大相对误差
    constexpr static double  RELATIVE_ERROR_THRESHOLD = 0.02;      ///< 相对误差的颜色阈值

    /// Edit控件显示的单位文本
    const static inline std::map<ConfigType, CString> mConfigTextext = {
        {ConfigType::DetectRange, _T("mm")},
        {ConfigType::Gain,        _T("dB")},
        {ConfigType::GateStart,   _T("%") },
        {ConfigType::GateWidth,   _T("%") },
        {ConfigType::GateHeight,  _T("%") },
    };

    const static inline std::map<ConfigType, std::wstring> mConfigRegex = {
        {ConfigType::DetectRange, _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::Gain,        _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::GateStart,   _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::GateWidth,   _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::GateHeight,  _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
    };

    /// Slider的极限范围
    static inline std::map<ConfigType, std::pair<float, float>> mConfigLimits = {
        {ConfigType::DetectRange, {0.f, 5000.f}},
        {ConfigType::Gain,        {0.f, 110.f} },
        {ConfigType::GateStart,   {0.f, 100.f} },
        {ConfigType::GateWidth,   {0.f, 100.f} },
        {ConfigType::GateHeight,  {0.f, 100.f} },
    };

    /// Edit控件鼠标滚轮事件的步进
    const static inline std::map<ConfigType, double> mConfigStep = {
        {ConfigType::DetectRange, 0.1},
        {ConfigType::Gain,        0.1},
        {ConfigType::GateStart,   0.1},
        {ConfigType::GateWidth,   0.1},
        {ConfigType::GateHeight,  0.1},
    };

private: /* 私有变量参数 */
    /// 扫查数据最大缓存数量

    string mSavePath = {}; ///< 保存路径

    /// 缺陷判决值
    std::array<uint8_t, HDBridge::CHANNEL_NUMBER> mDefectJudgmentValue = {};

    /**
     * @brief 当前选中的通道
     * @note 并不是表示实际的通道值, 而是表示第几个选项，例如`CHANNEL_1`表示第一个通道选项
     * 实际的通道号可能是 1,5,9 中的一个
     */
    ChannelSel              mChannelSel         = ChannelSel::CHANNEL_1;
    CWindowUI*              m_pWndOpenGL_ASCAN  = nullptr;                 ///< A扫Duilib的窗口指针
    CWindowUI*              m_pWndOpenGL_CSCAN  = nullptr;                 ///< C扫Duilib的窗口指针
    OpenGL                  m_OpenGL_ASCAN      = {};                      ///< A扫OpenGL窗口
    OpenGL                  m_OpenGL_CSCAN      = {};                      ///< C扫OpenGL窗口
    long                    mCurrentGroup       = 0;                       ///< 当前分组
    ConfigType              mConfigType         = ConfigType::DetectRange; ///< 当前选中设置类型
    GateType                mGateType           = GateType::GATE_A;        ///< 当前选中的波门类型
    UPTR_Utils              mUtils              = nullptr;                 ///< 硬件接口
    WidgetMode              mWidgetMode         = {WidgetMode::MODE_SCAN}; ///< 当前窗口的模式
    VEC_Utils               mReviewData         = {};                      ///< 扫查缺陷数据
    UPTR_FragReview         mFragmentReview     = {};                      ///< 分片加载回放
    int                     mSamplesPerSecond   = 33;                      ///< C扫图每秒钟采点个数
    ARRAY_CHNUMB            mIDDefectRecord     = {};                      ///< 缺陷记录的索引ID
    ORM_Model::DetectInfo   mDetectInfo         = {};                      ///< 探伤信息
    int                     mRecordCount        = {};                      ///< 扫查数据计数
    VEC_ScanRecord          mScanRecordCache    = {};                      ///< 扫查记录缓存(缺陷)
    DetectionStateMachine   mDetectionSM        = {};                      ///< 探伤的状态机
    VEC_DefectInfo          mDefectInfo         = {};                      ///< 探伤缺陷
    bool                    mScanningFlag       = false;                   ///< 判断当前是否正在扫查
    std::string             mReviewPathEntry    = {};                      ///< 回放路径入口
    std::thread             mCScanThread        = {};                      ///< C扫线程
    std::condition_variable mCScanNotify        = {};                      ///< C扫线程更新数据通知
    std::mutex              mCScanMutex         = {};                      ///< C扫线程互斥量
    bool                    mCScanThreadRunning = {};                      ///< C扫线程运行
    ARR_GateResCh           mAllGateResult      = {};                      ///< 所有波门的计算结果
    GateResultTimeNote      mLastGateResUpdate  = {};                      ///< 上一次波门结果更新的时间
    bool                    mClearMTXValue      = false;                      ///< Clear Measure Thickness X Value 
    // 参数备份
    ORM_Model::DetectInfo mDetectInfoBak   = {};
    std::wstring          mJobGroupNameBak = {};

public:
    MainFrameWnd();
    virtual ~MainFrameWnd();
    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() noexcept override;
    virtual void       InitWindow() override;
    virtual void       Notify(TNotifyUI& msg) override;
    virtual void       OnLButtonDown(UINT nFlags, ::CPoint pt) override;
    virtual void       OnLButtonDClick(UINT nFlags, ::CPoint pt) override;
    virtual void       OnTimer(int iIdEvent) override;
    virtual void       OnMouseMove(UINT nFlags, ::CPoint pt) override;

    void EnterReview(std::string path = {});

private:

    /**
     * @brief 报表导出按钮事件
     * 
     * @param msg 
     */
    void OnBtnReportExport(TNotifyUI &msg);

    /**
     * @brief 获取C扫图像索引点在界面上的位置
     * @param index 数据点索引
     * @return 界面上的位置点
     */
    ::CPoint GetCScanPtFromIndex(int index) const;

    /**
     * @brief 从点获取C扫图像的索引
     * 
     * @param pt 当前点(相对于整个界面)
     * @return int 索引
     */
    int GetCScanIndexFromPt(::CPoint pt) const;

    /**
     * @brief 更新帧区域
     * 
     * @param index 
     */
    void UpdateFrame(int index);

    /**
     * @brief 绘制回放C扫图
     */
    void DrawReviewCScan();

    /**
     * @brief 通过重新连接TOFD超声板
     */
    void ReconnectBoard(int type);

    /**
     * @brief 通过网络重新连接超声板
     * @param ip_FPGA
     * @param port_FPGA
     * @param ip_PC
     * @param port_PC
     */
    void ReconnectBoard(std::string ip_FPGA, uint16_t port_FPGA, std::string ip_PC, uint16_t port_PC);

    /**
     * @brief 生成一个HDBridge
     * @tparam BR HDBridge的派生类
     * @tparam ...Args 构造函数参数
     * @param config 配置文件
     * @param ...args 构造函数参数
     * @return HDBridge的独占指针
     */
    template <class BR, class... Args> static std::unique_ptr<HDBridge> GenerateHDBridge(const HDBridge& config, Args&&... args) {
        static_assert(std::is_base_of_v<HDBridge, BR> && !std::is_same_v<BR, HDBridge>);
        auto ret = std::unique_ptr<HDBridge>(new BR(std::forward<Args>(args)...));
        *ret     = config;
        return ret;
    }

    /**
     * @brief C扫线程
     */
    void ThreadCScan(void);

    /**
     * @brief 选组按钮单击回调函数
     * @param index 索引
     */
    void OnBtnSelectGroupClicked(long index);

    /**
     * @brief 模式按钮单击回调函数
     * @param name 按钮ID
     */
    void OnBtnModelClicked(std::wstring name);

    /**
     * @brief 更新Slider和Edit控件的值
     * @param newGroup
     * @param newConfig
     * @param newGate
     * @param newChannelSel
     * @praram bypassCheck 绕过重复检查
     */
    void UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel,
                                  bool bypassCheck = false);

    /**
     * @brief 设置Config值
     * @param val value
     * @param sync 控制是否立即同步
     */
    void SetConfigValue(float val, bool sync = true);

    /**
     * @brief 更新A扫回调函数
     * @param data A扫数据
     * @param caller 调用类
     */
    void UpdateAScanCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller);

    /**
     * @brief 更新所有波门的计算结果
     * @param data A扫数据
     * @param caller 调用类
     * @param result 结果
     */
    void UpdateAllGateResult(const HDBridge::NM_DATA& data, const HD_Utils& caller);

    /**
     * @brief 波门追踪回调函数
     * @param data A扫数据
     * @param caller 调用类
     */
    void AmpTraceCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller);

    /**
     * @brief 峰值记忆
     * @param data A扫数据
     * @param caller 调用类
     */
    void AmpMemeryCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller);

    /**
     * @brief 定时器中更新C扫图像
     */
    void UpdateCScanOnTimer();

    /**
     * @brief UI 按钮信号
     * @param name 按键名称
     */
    void OnBtnUIClicked(std::wstring& name);

    /**
     * @brief 停止所有定时器
     * @param
     */
    void KillUITimer(void);

    /**
     * @brief 重启所有定时器
     * @param
     */
    void ResumeUITimer(void);

    /**
     * @brief 保存缺陷数据
     */
    void SaveScanData();

    /**
     * @brief 进入回放模式
     * @param name 缺陷记录的名称
     * @return true 成功
     */
    [[nodiscard]]
    bool EnterReviewMode(std::string name);

    /**
     * @brief 退出回放模式
     */
    void ExitReviewMode();

    /**
     * @brief 选择测厚功能
     * @param enableMeasure 开启测厚功能
     */
    void SelectMeasureThickness(bool enableMeasure = true);

    /**
     * @brief 开始扫查
     * @param changeFlag 是否改变标志位
     */
    void StartScan(bool changeFlag = true);

    /**
     * @brief 停止扫查
     * @param changeFlag 是否改变标志位
     */
    void StopScan(bool changeFlag = true);

    /**
     * @brief 保存缺陷的起始ID
     * @param channel 通道号
     */
    void SaveDefectStartID(int channel);

    /**
     * @brief 保存缺陷的终止ID
     * @param channel 通道号
     */
    void SaveDefectEndID(int channel);

    /**
     * @brief 检查并更新
     * @param showNoUpdate 无更新可用时是否显示窗口
     */
    void CheckAndUpdate(bool showNoUpdate = false);
};

#pragma pop_macro("GATE_A")
#pragma pop_macro("GATE_B")
