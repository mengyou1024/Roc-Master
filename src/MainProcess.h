#pragma once

#include <Model/SystemConfig.h>
#include <functional>

class MainProcess {
public:
    MainProcess();
    ~MainProcess();

    void InitStroage();
    void RegistFuncOnDestory(std::function<void(void)> func);

private:
    std::vector<std::function<void(void)>> mFuncWhenDestory = {};
#ifndef APP_RELEASE
    FILE *mFile = nullptr;
#endif
};
