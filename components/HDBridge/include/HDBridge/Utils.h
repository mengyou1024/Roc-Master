#pragma once

#include "../HDBridge.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <mutex>
#include <set>
#include <spdlog/spdlog.h>
#include <stack>
#include <thread>
#include <type_traits>

#ifdef USE_SQLITE_ORM
    #include <sqlite_orm/sqlite_orm.h>

struct HD_ScanORM {
    #pragma pack(1)
    std::array<float, 4>                                                mThickness     = {};
    std::array<float, 2>                                                mCScanLimits   = {};
    std::array<HDBridge::HB_ScanGateInfo, HDBridge::CHANNEL_NUMBER + 4> mScanGateInfo  = {};
    std::array<HDBridge::HB_ScanGateInfo, HDBridge::CHANNEL_NUMBER>     mScanGateAInfo = {};
    std::array<HDBridge::HB_ScanGateInfo, HDBridge::CHANNEL_NUMBER>     mScanGateBInfo = {};
    #pragma pack()
    std::array<shared_ptr<HDBridge::NM_DATA>, HDBridge::CHANNEL_NUMBER> mScanData = {};
};

namespace sqlite_orm {
    template <>
    struct type_printer<HD_ScanORM> : public blob_printer {};
    template <>
    struct statement_binder<HD_ScanORM> {
        int bind(sqlite3_stmt* stmt, int index, const HD_ScanORM& value) {
            std::vector<char> blobValue = {};
            blobValue.resize(blobValue.size() + (sizeof(HD_ScanORM) - sizeof(HD_ScanORM::mScanData)));
            memcpy(blobValue.data(), &value, (sizeof(HD_ScanORM) - sizeof(HD_ScanORM::mScanData)));
            for (auto i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
                std::shared_ptr<HDBridge::NM_DATA> temp = value.mScanData[i];
                if (temp == nullptr) {
                    temp           = std::make_shared<HDBridge::NM_DATA>();
                    temp->iChannel = i;
                }
                size_t lastSize = blobValue.size();
                blobValue.resize(blobValue.size() + sizeof(HDBridge::NM_DATA) - sizeof(vector<uint8_t>));
                memcpy(&blobValue[lastSize], &(temp->iChannel), sizeof(HDBridge::NM_DATA) - sizeof(vector<uint8_t>));
                if (temp->iAScanSize > 0 && temp->pAscan.size() > 0) {
                    lastSize = blobValue.size();
                    blobValue.resize(blobValue.size() + std::min((size_t)temp->iAScanSize, temp->pAscan.size()));
                    memcpy(&blobValue[lastSize], temp->pAscan.data(), std::min((size_t)temp->iAScanSize, temp->pAscan.size()));
                }
            }
            return statement_binder<std::vector<char>>().bind(stmt, index, blobValue);
        }
    };
    template <>
    struct field_printer<HD_ScanORM> {
        std::string operator()(const HD_ScanORM& value) {
            return {};
        }
    };
    template <>
    struct row_extractor<HD_ScanORM> {
        HD_ScanORM extract(sqlite3_stmt* stmt, int index) {
            char*      blobPointer = (char*)sqlite3_column_blob(stmt, index);
            HD_ScanORM value;
            memcpy(&value, blobPointer, (sizeof(HD_ScanORM) - sizeof(HD_ScanORM::mScanData)));
            size_t pointerOffset = (sizeof(HD_ScanORM) - sizeof(HD_ScanORM::mScanData));

            for (auto i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
                value.mScanData[i] = std::make_shared<HDBridge::NM_DATA>();
                memcpy(&value.mScanData[i]->iChannel, &blobPointer[pointerOffset], sizeof(HDBridge::NM_DATA) - sizeof(vector<uint8_t>));
                pointerOffset += sizeof(HDBridge::NM_DATA) - sizeof(vector<uint8_t>);
                if (value.mScanData[i]->iAScanSize > 0) {
                    value.mScanData[i]->pAscan.resize(value.mScanData[i]->iAScanSize);
                    memcpy(value.mScanData[i]->pAscan.data(), &blobPointer[pointerOffset], value.mScanData[i]->iAScanSize);
                    pointerOffset += value.mScanData[i]->iAScanSize;
                }
            }
            return value;
        }
    };
} // namespace sqlite_orm
#endif

class HD_Utils {
public:
    int id = 0;

#ifdef USE_SQLITE_ORM
    using HD_ScanORM = ::HD_ScanORM;
#else
    struct HD_ScanORM {
        shared_ptr<HDBridge::NM_DATA> mScanData[HDBridge::CHANNEL_NUMBER] = {};
    };
#endif

    HD_ScanORM mScanOrm = {};

    explicit HD_Utils(std::unique_ptr<HDBridge>& bridge)
        : mBridge(std::move(bridge)) {
        if (mBridge) {
            if (!mBridge->open()) {
                spdlog::error("open board failed");
                for (auto& d : mScanOrm.mScanData) {
                    d = std::make_shared<HDBridge::NM_DATA>();
                }
                return;
            }
            if (!mBridge->isOpen()) {
                spdlog::error("HDBridge not open");
            } else {
                spdlog::debug("HDBridge open");
            }
        }
    }

    explicit HD_Utils(const HD_Utils& other)
        : HD_Utils() {
        id       = other.id;
        mScanOrm = other.mScanOrm;
    }

    HD_Utils& operator=(const HD_Utils& other) {
        id       = other.id;
        mScanOrm = other.mScanOrm;
        return *this;
    }

    explicit HD_Utils() = default;

    ~HD_Utils() {
        if (mBridge) {
            mBridge->close();
        }
        waitExit();
    }

    std::thread::id start();

    void setBridge(std::unique_ptr<HDBridge>& bridge) {
        mBridge = std::move(bridge);
    }

    template <class T>
    T getBridge() const {
        static_assert(std::is_pointer_v<T> && std::is_base_of_v<HDBridge, std::remove_pointer_t<T>>);
        return dynamic_cast<T>(mBridge.get());
    }

    HDBridge* getBridge() const {
        return mBridge.get();
    }

    HDBridge::cache_t& getCache() const {
        return mBridge->getCache_ref();
    }

    void stop();
    void waitExit();

    void addReadCallback(const std::function<void(const HDBridge::NM_DATA&, const HD_Utils&)> callback, std::string name = {});
    void removeReadCallback(std::string name = {});

    /**
     * @brief 包装回调函数
     * @param func 类方法
     * @param th 类指针
     * @param ...args 除了`const HDBridge::NM_DATA& data, const HD_Utils& caller`以外的参数
     * @return 可调用对象
     */
    template <class Fn, class Th>
    static auto WrapReadCallback(Fn func, Th th) {
        return std::bind(func, th, std::placeholders::_1, std::placeholders::_2);
    }

    /**
     * @brief 将回调函数列表压入栈,
     * @note 此操作在压入栈后, 会调用`removeReadCallback`删除所有的回调函数
     */
    void pushCallback();

    /**
     * @brief 从栈中恢复回调函数列表
     */
    void popCallback();

    /**
     * @brief 自动增益
     * @param channel 通道号
     * @param gateIndex 波门索引
     * @param goal 目标值
     * @param gainStep 增益步进
     */
    void autoGain(int channel, int gateIndex, float goal = 0.8f, float gainStep = 0.1f);

#ifdef USE_SQLITE_ORM

    #ifndef ORM_DB_NAME
    static constexpr std::string_view ORM_DB_NAME = "HD_Utils.db";
    #endif // !ORM_DB_NAME

    static auto storage(std::string dbName) {
        using namespace sqlite_orm;
        return make_storage(dbName,
                            make_table("HD_Utils",
                                       make_column("ID", &HD_Utils::id, primary_key()),
                                       make_column("DATA", &HD_Utils::mScanOrm)));
    }

    static auto storage() {
        return storage(std::string(ORM_DB_NAME));
    }

#endif

private:
    using HDUtilsCallback       = std::function<void(const HDBridge::NM_DATA&, const HD_Utils&)>;
    using HDUtilsCallbackVector = std::multimap<std::string, HDUtilsCallback>;
    using HDUtilsCallbackStack  = std::stack<HDUtilsCallbackVector>;

    std::mutex                mScanDataMutex     = {};
    std::mutex                mReadCallbackMutex = {};
    bool                      mReadThreadExit    = false;
    std::unique_ptr<HDBridge> mBridge            = nullptr;

    HDUtilsCallbackVector mReadCallback  = {};
    HDUtilsCallbackStack  mCallbackStack = {};

    std::thread mReadThread;

    void readThread();
};
