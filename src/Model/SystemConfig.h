#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <sqlite_orm.h>

using namespace sqlite_orm;

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "SystemConfig.db"
#endif // !ORM_DB_NAME

namespace ORM_Model {
    class SystemConfig {
    public:
        uint32_t     id                     = {};    ///< id
        bool         checkUpdate            = {};    ///< 软件检查最新版本
        std::wstring groupName              = {};    ///< 班组
        std::wstring userName               = {};    ///< 用户姓名
        bool         enableProxy            = {};    ///< 代理
        std::wstring httpProxy              = {};    ///< http代理
        bool         enableMeasureThickness = false; ///< 使能测厚功能
        bool         enableNetworkMulti     = false; ///< 使用多通道网络(原10通道)
        bool         enableNetworkTOFD      = false; ///< 使用TOFD版网络
        std::string  ipFPGA                 = {};    ///< FPGA的IP地址
        std::string  ipPC                   = {};    ///< PC的IP地址
        uint16_t     portFPGA               = {};    ///< FPGA的端口
        uint16_t     portPC                 = {};    ///< PC的端口
        int          IDYearMonth            = {};
        int          IDDay                  = {};
        int          IDTime                 = {};
        static auto  storage(void) {
            return make_storage(
                ORM_DB_NAME,
                make_table("SystemConfig", make_column("ID", &SystemConfig::id, primary_key().autoincrement()),
                            make_column("CHECK_UPDATE", &SystemConfig::checkUpdate), make_column("GROUP_NAME", &SystemConfig::userName),
                            make_column("USER_NAME", &SystemConfig::groupName), make_column("ENABLE_PROXY", &SystemConfig::enableProxy),
                            make_column("HTTP_PROXY", &SystemConfig::httpProxy),
                            make_column("ENABLE_MEASURE_THICKNESS", &SystemConfig::enableMeasureThickness),
                            make_column("ENABLE_NETWORK_MULTI", &SystemConfig::enableNetworkMulti),
                            make_column("ENABLE_NETWORK_TOFD", &SystemConfig::enableNetworkTOFD),
                            make_column("IP_FPGA", &SystemConfig::ipFPGA), make_column("IP_PC", &SystemConfig::ipPC),
                            make_column("PORT_FPGA", &SystemConfig::portFPGA), make_column("PORT_PC", &SystemConfig::portPC),
                            make_column("ID_YEAR_MONTH", &SystemConfig::IDYearMonth), make_column("ID_DAY", &SystemConfig::IDDay),
                            make_column("ID_TIME", &SystemConfig::IDTime)
                            ));
        }
    };
} // namespace ORM_Model
