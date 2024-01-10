// #include <HDBridge.h>
// #include <HDBridge/TOFDPort.h>
// #include <HDBridge/Utils.h>

#include <HDBridge/TOFDPort.h>
#include <HDBridge/Utils.h>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sqlite_orm/sqlite_orm.h>
#include <thread>

#ifdef _WIN32
    #include <Windows.h>
#endif

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "SystemConfig.db"
#endif // !ORM_DB_NAME

int main() {
    using namespace sqlite_orm;
    auto storageUtils = HD_Utils::storage();
    storageUtils.sync_schema();
    auto storagePort = TOFDUSBPort::storage();
    storagePort.sync_schema();

    auto tt = storagePort.get_all<TOFDUSBPort>(where(c(&TOFDUSBPort::name) == L"bridge"));
    if (tt.size() != 0)
        tt[0].syncCache2Board();
    auto beforeTime = std::chrono::steady_clock::now();
    auto uuu        = storageUtils.get_all<HD_Utils>();
    for (auto& u : uuu) {
        std::cout << u.mScanOrm.mScanData[0]->iPackage << std::endl;
    }
    auto   afterTime           = std::chrono::steady_clock::now();
    double duration_millsecond = std::chrono::duration<double, std::micro>(afterTime - beforeTime).count();
    std::cout << "read HD_Utils" << duration_millsecond << "us" << std::endl;

    TOFDUSBPort* bridge = new TOFDUSBPort;
    bridge->name        = L"123123";
    std::cout << "=======================================" << std::endl;
    HD_Utils utils(bridge);
    utils.start();
    utils.addReadCallback([](const HDBridge::NM_DATA& data, const HD_Utils&) {
        // printf("iChannel: %d\r\n", data.iChannel);
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    });
    uint32_t i = 0;
    while (1) {
        if (++i > 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "thread running ..." << std::endl;
    }
    std::cout << "thread exit" << std::endl;
    utils.waitExit();
    // storage.insert(*bridge);
    storageUtils.insert(utils);
    TOFDUSBPort a      = {};
    a.name             = L"12312312";
    a.mCache.delay[0]  = 100.f;
    a.mCache.frequency = 999;
    // a.gain        = {0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6};
    try {
        storagePort.insert(a);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    auto ttt = storagePort.get_all<TOFDUSBPort>();
    for (auto& t : ttt) {
        std::wcout << t.name << std::endl;
        std::cout << t.id << std::endl;
        std::cout << t.mCache.frequency << std::endl;
        std::cout << "========================================" << std::endl;
    }
    return 0;
}