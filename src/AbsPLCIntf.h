#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using std::function;
using std::make_pair;
using std::map;
using std::pair;
using std::string;
using std::vector;

namespace AbsPLCIntf {

    /**
     * @brief swap 16bit
     *
     * @param sval
     * @return uint16_t
     */
    uint16_t swap(uint16_t sval);

    /**
     * @brief swap 32bit
     *
     * @param val
     * @return uint32_t
     */
    uint32_t swap(uint32_t lval);

    /**
     * @brief swap float
     *
     * @param fval
     * @return float
     */
    float swap(float fval);

    /**
     * @brief is PLC connected
     *
     * @return true
     * @return false
     */
    bool isConnected();

    /**
     * @brief connected to plc
     *
     * @param addr plc address
     * @param rack plc rack
     * @param slot plc slot
     */
    bool connectTo(const char* addr = "192.168.2.1", int rack = 0, int slot = 1);

    /**
     * @brief disconnect
     *
     */
    void disconnect(void);

    /**
     * @brief Get the Connected Info object
     *
     * @param addr plc address
     * @param rack plc rack
     * @param slot plc slot
     */
    void getConnectedInfo(std::string* addr = nullptr, int* rack = nullptr, int* slot = nullptr);

    // getter

    bool getVariable(string id, bool& val);
    bool getVariable(string id, uint8_t& val);
    bool getVariable(string id, uint16_t& val);
    bool getVariable(string id, uint32_t& val);
    bool getVariable(string id, float& val);
    bool getVariable(string id, int sz, std::vector<uint8_t>& data);
    bool getVariable(string id, int sz, std::vector<float>& data);

    // setter

    bool setVariable(string id, bool val);
    bool setVariable(string id, uint8_t val);
    bool setVariable(string id, uint16_t val);
    bool setVariable(string id, uint32_t val);
    bool setVariable(string id, float val);
    bool setVariable(string id, int sz, const std::vector<uint8_t>& data);
    bool setVariable(string id, int sz, const std::vector<float>& data);

    template <class T>
    std::pair<bool, T> getVariable(string id) {
        T    ret{};
        bool res = getVariable(id, ret);
        return std::make_pair(res, ret);
    }

    template <class T>
    std::pair<bool, std::vector<T>> getVariable(string id, int sz) {
        bool           res = {};
        std::vector<T> ret = {};
        res                = getVariable(id, sz, ret);
        return std::make_pair(res, ret);
    }

}; // namespace AbsPLCIntf
