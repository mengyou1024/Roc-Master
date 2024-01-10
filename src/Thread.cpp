#include "pch.h"

#include "Thread.h"

Thread::Thread() : _pthread(nullptr), m_bWorking(false), _hevent(INVALID_HANDLE_VALUE) {}

Thread::~Thread() {
    Close();
}

bool Thread::InitEvent() {
    if (_hevent != INVALID_HANDLE_VALUE)
        return true;

    _hevent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (_hevent == INVALID_HANDLE_VALUE)
        return false;

    return true;
}

bool Thread::WaitEvent(INT iTime) {
    if (WaitForSingleObject(_hevent, iTime) == WAIT_TIMEOUT)
        return false;

    return true;
}

bool Thread::Event(bool bEnable) {
    if (_hevent == INVALID_HANDLE_VALUE)
        return false;

    if (bEnable)
        SetEvent(_hevent);
    else
        ResetEvent(_hevent);

    return true;
}

void Thread::Close() {
    m_bWorking = false;

    if (_pthread) {
        _pthread->join();
        delete _pthread;
        _pthread = nullptr;
    }

    if (_hevent != INVALID_HANDLE_VALUE) {
        CloseHandle(_hevent);
        _hevent = INVALID_HANDLE_VALUE;
    }
}

TaskQueue::TaskQueue() {
    mRunning = true;
    mThread  = std::thread(&TaskQueue::run, this);
    std::unique_lock lock(mTaskQueueMutex);
    mQueueReady.wait(lock);
}

TaskQueue::~TaskQueue() {
    mRunning = false;
    mCVNotify.notify_one();
    mThread.join();
}

void TaskQueue::AddTask(std::function<void(void)> func, std::string id, bool rm_same_id) {
    std::unique_lock lock(mTaskQueueMutex);
    bool             find_same = false;
    for (int i = 0; i < mTaskQueue.size(); i++) {
        auto &[_task, _id] = mTaskQueue.front();
        if (rm_same_id && _id == id) {
            _task     = func;
            find_same = true;
        }
        mTaskQueue.push({_task, _id});
        mTaskQueue.pop();
    }
    if (!rm_same_id || !find_same) {
        mTaskQueue.push({func, id});
    }
    mCVNotify.notify_one();
}

void TaskQueue::run(void) {
    mQueueReady.notify_one();
    while (mRunning) {
        std::unique_lock lock(mTaskQueueMutex);
        mCVNotify.wait(lock);
        while (!mTaskQueue.empty()) {
            auto &[task, id] = mTaskQueue.front();
            task();
            mTaskQueue.pop();
        }
    }
}
