#pragma once
#include <Windows.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

class Thread {
public:
    Thread();
    virtual ~Thread();

    template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, std::thread>, int> = 0>
    bool Create(_Fn&& _Fx, _Args&&... _Ax) {
        m_bWorking = true;
        _pthread   = new std::thread(_STD forward<_Fn>(_Fx), _STD forward<_Args>(_Ax)...);

        return true;
    }

    bool IsNull() {
        return _pthread == nullptr;
    };

    bool InitEvent();
    bool WaitEvent(INT iTime);
    bool Event(bool bEnable);

    void Close();

    bool m_bWorking;

private:
    std::thread* _pthread;
    // 线程事件
    HANDLE _hevent;
};

class TaskQueue {
public:
    using FUNC_PAIR  = std::pair<std::function<void(void)>, std::string>;
    using FUNC_QUEUE = std::queue<FUNC_PAIR>;
    explicit TaskQueue();
    ~TaskQueue();

    void AddTask(std::function<void(void)> func, std::string id = 0, bool rm_same_id = false);

private:
    std::condition_variable_any mCVNotify       = {};
    std::condition_variable_any mQueueReady     = {};
    bool                        mRunning        = {};
    std::recursive_mutex        mTaskQueueMutex = {};
    FUNC_QUEUE                  mTaskQueue      = {};
    std::thread                 mThread         = {};
    void                        run(void);
};
