#pragma once

#include <functional>
#include <Model/SystemConfig.h>

class MainProcess {
public:
    MainProcess();
    ~MainProcess();

    void InitStroage();
    void RegistFuncOnDestory(std::function<void(void)> func);

private:
    std::vector<std::function<void(void)>> mFuncWhenDestory = {};
#if _DEBUG
    FILE *mFile = nullptr;
#endif
};
