#include "pch.h"

#include "AbsPLCIntf.h"
#include "snap7.h"
#include <cstdio>
#include <memory>
#include <mutex>
#include <regex>

using std::lock_guard;
using std::make_unique;
using std::mutex;
using std::unique_ptr;

class _RuitiePLCInfo {
public:
    bool                   m_connected      = false;
    TS7Client*             m_client         = nullptr;
    std::string            m_addr           = {};
    int                    m_rack           = 0;
    int                    m_slot           = 0;
    int                    m_reconnectCount = 0;
    bool                   m_reconnectErr   = false;
    mutex                  m_lock           = {};
    static _RuitiePLCInfo* getInstance() {
        static _RuitiePLCInfo inst;
        return &inst;
    }
};

/**
 * @brief 获取变量映射
 * @param id 变量id
 * @return [area, start, isBit, bitSel, valid]
 */
static std::tuple<uint8_t, int, bool, int, bool> getMapping(std::string id) {
    static std::regex reg(R"(^([IQMV])(\d+)\.?([07])?$)");
    std::smatch       match;
    if (std::regex_match(id, match, reg)) {
        uint8_t area = 0;
        if (match[1].str() == "I") {
            area = S7AreaPE;
        } else if (match[1].str() == "Q") {
            area = S7AreaPA;
        } else if (match[1].str() == "M") {
            area = S7AreaMK;
        } else if (match[1].str() == "V") {
            area = S7AreaDB;
        }
        int  start  = atol(match[2].str().c_str());
        bool isBit  = match[3].matched;
        int  bitSel = 0;
        if (isBit) {
            bitSel = atol(match[3].str().c_str());
        }
        return std::make_tuple(area, start, isBit, bitSel, true);
    }
    return std::make_tuple(0, 0, 0, 0, false);
}

template <class T>
static bool __getVariable(std::string id, T& ret, int type) {
    if (!AbsPLCIntf::isConnected()) {
        return false;
    }
    auto [area, start, isBit, bitSel, valid] = getMapping(id);
    if (!valid) {
        return false;
    }
    auto _ret = doWithReconnect([area, start, type, &ret]() -> bool {
        auto client = _RuitiePLCInfo::getInstance()->m_client;
        return client->ReadArea(area, 1, start, 1, type, &ret) == 0;
    });
    return _ret;
}

template <class T>
static bool __getVariable(std::string id, int sz, std::vector<T>& data, int type) {
    if (!AbsPLCIntf::isConnected()) {
        return false;
    }
    auto [area, start, isBit, bitSel, valid] = getMapping(id);
    if (!valid) {
        return false;
    }
    auto _ret = doWithReconnect([area, start, sz, type, &data]() -> bool {
        auto client = _RuitiePLCInfo::getInstance()->m_client;
        return client->ReadArea(area, 1, start, sz, type, (void*)data.data()) == 0;
    });
    return _ret;
}

template <class T>
static bool __setVariable(std::string id, T ret, int type) {
    if (!AbsPLCIntf::isConnected()) {
        return false;
    }
    auto [area, start, isBit, bitSel, valid] = getMapping(id);
    if (!valid) {
        return false;
    }
    auto _ret = doWithReconnect([area, start, type, &ret]() -> bool {
        auto client = _RuitiePLCInfo::getInstance()->m_client;
        return client->WriteArea(area, 1, start, 1, type, &ret) == 0;
    });
    return _ret;
}

template <class T>
static bool __setVariable(std::string id, int sz, const std::vector<T>& data, int type) {
    if (!AbsPLCIntf::isConnected()) {
        return false;
    }
    auto [area, start, isBit, bitSel, valid] = getMapping(id);
    if (!valid) {
        return false;
    }
    auto _ret = doWithReconnect([area, start, sz, type, &data]() -> bool {
        auto client = _RuitiePLCInfo::getInstance()->m_client;
        return client->WriteArea(area, 1, start, sz, type, (void*)data.data()) == 0;
    });
    return _ret;
}

static bool reconnect() {
    string ip   = "";
    int    slot = 0;
    int    rack = 0;
    AbsPLCIntf::getConnectedInfo(&ip, &rack, &slot);
    if (ip.empty()) {
        return false;
    } else {
        if (_RuitiePLCInfo::getInstance()->m_client == nullptr) {
            _RuitiePLCInfo::getInstance()->m_client = new TS7Client;
        }
        int err = _RuitiePLCInfo::getInstance()->m_client->ConnectTo(ip.c_str(), rack, slot);
        if (err == 0) {
            _RuitiePLCInfo::getInstance()->m_addr      = string(ip.c_str());
            _RuitiePLCInfo::getInstance()->m_rack      = rack;
            _RuitiePLCInfo::getInstance()->m_slot      = slot;
            _RuitiePLCInfo::getInstance()->m_connected = true;
        } else {
            spdlog::warn("reconnect to :{} {} {} error: {}\n", ip, rack, slot, err);
            return false;
        }
    }
    spdlog::warn("reconnect to :{} {} {} success\n", ip, rack, slot);
    return true;
}

static bool doWithReconnect(function<bool(void)> func) {
    lock_guard<mutex> lockGruard(_RuitiePLCInfo::getInstance()->m_lock);
    if (_RuitiePLCInfo::getInstance()->m_reconnectErr) {
        return false;
    }
    while (1) {
        if (!func()) {
            if (_RuitiePLCInfo::getInstance()->m_reconnectCount >= 2) {
                return false;
            }
        } else {
            _RuitiePLCInfo::getInstance()->m_reconnectCount = 0;
            break;
        }
        if (!reconnect()) {
            _RuitiePLCInfo::getInstance()->m_connected    = false;
            _RuitiePLCInfo::getInstance()->m_reconnectErr = true;
            spdlog::warn("reconnect error.");
            return false;
        }
        ++_RuitiePLCInfo::getInstance()->m_reconnectCount;
    }

    return true;
}

static uint16_t AbsPLCIntf::swap(uint16_t sval) {
    return ((sval & 0xFF) << 8) | ((sval & 0xFF00) >> 8);
}

static uint32_t AbsPLCIntf::swap(uint32_t lval) {
    return ((lval & 0xFF000000) >> 24) | ((lval & 0xFF0000) >> 8) | ((lval & 0xFF00) << 8) | ((lval & 0xFF) << 24);
}

static float AbsPLCIntf::swap(float fval) {
    uint32_t temp = *(uint32_t*)&fval;
    temp          = swap(temp);
    float ret     = *(float*)&temp;
    return ret;
}

bool AbsPLCIntf::isConnected() {
    return _RuitiePLCInfo::getInstance()->m_connected;
}

bool AbsPLCIntf::connectTo(const char* addr, int rack, int slot) {
    if (isConnected()) {
        spdlog::info("already connect to :{} {} {}", _RuitiePLCInfo::getInstance()->m_addr.c_str(), _RuitiePLCInfo::getInstance()->m_rack,
                     _RuitiePLCInfo::getInstance()->m_slot);
    } else {
        if (_RuitiePLCInfo::getInstance()->m_client == nullptr) {
            _RuitiePLCInfo::getInstance()->m_client = new TS7Client;
        }
        _RuitiePLCInfo::getInstance()->m_client->SetConnectionType(3);
        int err = _RuitiePLCInfo::getInstance()->m_client->ConnectTo(addr, rack, slot);
        if (err == 0) {
            _RuitiePLCInfo::getInstance()->m_addr           = string(addr);
            _RuitiePLCInfo::getInstance()->m_rack           = rack;
            _RuitiePLCInfo::getInstance()->m_slot           = slot;
            _RuitiePLCInfo::getInstance()->m_connected      = true;
            _RuitiePLCInfo::getInstance()->m_reconnectCount = 0;
            _RuitiePLCInfo::getInstance()->m_reconnectErr   = false;
        } else {
            spdlog::warn("connect to :{} {} {} error: {}", addr, rack, slot, err);
            return false;
        }
    }
    return true;
}

void AbsPLCIntf::disconnect(void) {
    if (_RuitiePLCInfo::getInstance()->m_client) {
        if (isConnected()) {
            _RuitiePLCInfo::getInstance()->m_client->Disconnect();
        }
        delete _RuitiePLCInfo::getInstance()->m_client;
        _RuitiePLCInfo::getInstance()->m_client    = nullptr;
        _RuitiePLCInfo::getInstance()->m_connected = false;
    }
}

void AbsPLCIntf::getConnectedInfo(std::string* addr, int* rack, int* slot) {
    if (addr) {
        *addr = _RuitiePLCInfo::getInstance()->m_addr;
    }
    if (rack) {
        *rack = _RuitiePLCInfo::getInstance()->m_rack;
    }
    if (slot) {
        *slot = _RuitiePLCInfo::getInstance()->m_slot;
    }
}

bool AbsPLCIntf::getVariable(string id, bool& val) {
    auto [area, start, isBit, bitSel, valid] = getMapping(id);
    if (!valid) {
        return false;
    }
    if (!isBit) {
        return false;
    }
    uint8_t data = {};
    auto    _ret = __getVariable<uint8_t>(id, data, S7WLByte);
    val          = (data & (1 << bitSel)) ? true : false;
    return _ret;
}

bool AbsPLCIntf::getVariable(string id, uint8_t& val) {
    return __getVariable<uint8_t>(id, val, S7WLByte);
}

bool AbsPLCIntf::getVariable(string id, uint16_t& val) {
    auto ret = __getVariable<uint16_t>(id, val, S7WLWord);
    if (ret) {
        val = swap(val);
    }
    return ret;
}

bool AbsPLCIntf::getVariable(string id, uint32_t& val) {
    auto ret = __getVariable<uint32_t>(id, val, S7WLDWord);
    if (ret) {
        val = swap(val);
    }
    return ret;
}

bool AbsPLCIntf::getVariable(string id, float& val) {
    auto _ret = __getVariable<float>(id, val, S7WLReal);
    if (_ret) {
        val = swap(val);
    }
    return _ret;
}

bool AbsPLCIntf::getVariable(string id, int sz, std::vector<uint8_t>& data) {
    return __getVariable(id, sz, data, S7WLByte);
}

bool AbsPLCIntf::getVariable(string id, int sz, std::vector<float>& data) {
    auto _ret = __getVariable(id, sz, data, S7WLReal);
    if (_ret) {
        for (auto& it : data) {
            it = swap(it);
        }
    }
    return _ret;
}

bool AbsPLCIntf::setVariable(string id, bool val) {
    auto [area, start, isBit, bitSel, valid] = getMapping(id);
    if (!valid) {
        return false;
    }
    if (!isBit) {
        return false;
    }
    uint8_t data = {};
    auto    ret  = __getVariable(id, data, S7WLByte);
    if (ret) {
        if (val) {
            data |= 1 << bitSel;
        } else {
            data &= ~(1 << bitSel);
        }
        ret = __setVariable(id, data, S7WLByte);
    }
    return ret;
}

bool AbsPLCIntf::setVariable(string id, uint8_t val) {
    return __setVariable(id, val, S7WLByte);
}

bool AbsPLCIntf::setVariable(string id, uint16_t val) {
    val = swap(val);
    return __setVariable(id, val, S7WLWord);
}

bool AbsPLCIntf::setVariable(string id, uint32_t val) {
    val = swap(val);
    return __setVariable(id, val, S7WLDWord);
}

bool AbsPLCIntf::setVariable(string id, float val) {
    val = swap(val);
    return __setVariable(id, val, S7WLReal);
}

bool AbsPLCIntf::setVariable(string id, int sz, const std::vector<uint8_t>& data) {
    return __setVariable(id, sz, data, S7WLByte);
}

bool AbsPLCIntf::setVariable(string id, int sz, const std::vector<float>& data) {
    std::vector<float> copy = data;
    for (auto& it : copy) {
        it = swap(it);
    }
    return __setVariable(id, sz, data, S7WLReal);
}
