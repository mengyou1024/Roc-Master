#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <sqlite_orm.h>

using namespace sqlite_orm;

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "DefectInfo.db"
#endif // !ORM_DB_NAME

namespace ORM_Model {
    class DefectInfo {
    public:
        DefectInfo() = default;

        uint32_t     id          = {}; ///< id
        std::wstring product     = {};
        std::wstring description = {};
        std::wstring flawno      = {};
        std::wstring dac         = {};
        std::wstring type        = {};
        std::wstring location    = {};
        std::wstring length      = {};
        std::wstring depth       = {};
        std::wstring width       = {};
        std::wstring result      = {};

        bool isLogin = false; ///< ÊÇ·ñµÇÂ¼

        static auto storage(std::string name) {
            return make_storage(name, make_table("DefectInfo", make_column("ID", &DefectInfo::id, primary_key().autoincrement()),
                                                 make_column("PRODUCT", &DefectInfo::product),
                                                 make_column("DESCRIPTION", &DefectInfo::description),
                                                 make_column("FLAWNO", &DefectInfo::flawno), make_column("DAC", &DefectInfo::dac),
                                                 make_column("TYPE", &DefectInfo::type), make_column("LOCATION", &DefectInfo::location),
                                                 make_column("LENGTH", &DefectInfo::length), make_column("DEPTH", &DefectInfo::depth),
                                                 make_column("WIDTH", &DefectInfo::width), make_column("RESULT", &DefectInfo::result)));
        }

        static auto storage(void) {
            return storage(ORM_DB_NAME);
        }
    };

} // namespace ORM_Model
