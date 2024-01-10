#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <sqlite_orm.h>

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "DetectInfo.db"
#endif // !ORM_DB_NAME

namespace ORM_Model {
    using namespace sqlite_orm;
    class DetectInfo {
    public:
        uint32_t    id                     = {};    ///< id
        std::string time                   = {};    ///< 扫查的时间
        bool        enableMeasureThickness = false; ///< 使能测厚功能
        // 基本信息
        std::wstring customer               = {}; ///< 客户
        std::wstring customerContractNumber = {}; ///< 客户合同编号
        std::wstring workOrder              = {}; ///< 工作令
        std::wstring reportNumber           = {}; ///< 报告编号
        // 工件
        std::wstring description       = {}; ///< 描述
        std::wstring materialStandards = {}; ///< 材质标准
        std::wstring surfaceStates     = {}; ///< 表面状态
        std::wstring surveyedArea      = {}; ///< 检测区域
        std::wstring thickness         = {}; ///< 厚度
        // 器材及参数
        std::wstring unitType            = {}; ///< 设备型号
        std::wstring probeType           = {}; ///< 探头信号
        std::wstring referenceBlock      = {}; ///< 参考试块
        std::wstring waveform            = {}; ///< 波形
        std::wstring scanningSensitivity = {}; ///< 扫查灵敏度
        std::wstring couplingAgent       = {}; ///< 耦合剂
        // 探头型号
        std::wstring executiveStandard   = {}; ///< 执行标准
        std::wstring acceptanceStandard  = {}; ///< 验收标准
        std::wstring detectRatio         = {}; ///< 检测比例
        std::wstring detectProcessNumber = {}; ///< 检测工艺编号

        static auto storage(std::string name) {
            return make_storage(
                name,
                make_table(
                    "DetectInfo", make_column("ID", &DetectInfo::id, primary_key().autoincrement()), make_column("TIME", &DetectInfo::time),
                    make_column("ENABLE_MEASURE_THICKNESS", &DetectInfo::enableMeasureThickness),
                    make_column("CUSTOMER", &DetectInfo::customer),
                    make_column("CUSTOMER_CONTRACT_NUMBER", &DetectInfo::customerContractNumber),
                    make_column("WORK_ORDER", &DetectInfo::workOrder), make_column("REPORT_NUMBER", &DetectInfo::reportNumber),
                    make_column("DESCRIPTION", &DetectInfo::description), make_column("MATERIAL_STANDARDS", &DetectInfo::materialStandards),
                    make_column("SURFACE_STATES", &DetectInfo::surfaceStates), make_column("SURVEYED_AREA", &DetectInfo::surveyedArea),
                    make_column("THICKNESS", &DetectInfo::thickness), make_column("UNIT_TYPE", &DetectInfo::unitType),
                    make_column("PROBE_TYPE", &DetectInfo::probeType), make_column("REFERENCE_BLOCK", &DetectInfo::referenceBlock),
                    make_column("WAVEFORM", &DetectInfo::waveform), make_column("SCANNING_SENSITIVITY", &DetectInfo::scanningSensitivity),
                    make_column("COUPLING_AGENT", &DetectInfo::couplingAgent),
                    make_column("EXECUTIVE_STANDARD", &DetectInfo::executiveStandard),
                    make_column("ACCEPTANCE_STANDARD", &DetectInfo::acceptanceStandard),
                    make_column("DETECT_RATIO", &DetectInfo::detectRatio),
                    make_column("DETECT_PROCESS_NUMBER", &DetectInfo::detectProcessNumber)));
        }

        static auto storage(void) {
            return storage(ORM_DB_NAME);
        }
    };
} // namespace ORM_Model
