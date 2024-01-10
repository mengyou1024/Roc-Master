#pragma once
#include <type_traits>

template <typename T, int N = 512>
class FragmentLoader {
public:
    constexpr static auto SIZE_PER_FRAGMENT = N;
    FragmentLoader(T &rvData, int minFrag = SIZE_PER_FRAGMENT) :
    mRV(rvData),
    mFragments((int)std::ceil((double)rvData.size() / (double)SIZE_PER_FRAGMENT)) {
        if ((int)mRV.size() >= SIZE_PER_FRAGMENT) {
            mFragmentSize = SIZE_PER_FRAGMENT;
        } else {
            mFragmentSize = (int)mRV.size();
        }
    }

    int size() const {
        return mFragmentSize;
    }

    int fragments() const {
        return mFragments;
    }

    void setCurFragment(int cursor) {
        mCurFragment = cursor;
        if (mCurFragment >= mFragments) {
            mCurFragment = mFragments - 1;
        } else if (mCurFragment < 0) {
            mCurFragment = 0;
        }
        if (mCurFragment == mFragments - 1) {
            mFragmentSize = (int)mRV.size() % SIZE_PER_FRAGMENT;
        } else {
            mFragmentSize = SIZE_PER_FRAGMENT;
        }
        mFragmentStart = mCurFragment * SIZE_PER_FRAGMENT;
    }

    FragmentLoader &operator++(int) {
        mCurFragment++;
        if (mCurFragment >= mFragments) {
            mCurFragment = mFragments - 1;
        }
        if (mCurFragment == mFragments - 1) {
            mFragmentSize = (int)mRV.size() % SIZE_PER_FRAGMENT;
        } else {
            mFragmentSize = SIZE_PER_FRAGMENT;
        }
        mFragmentStart = mCurFragment * SIZE_PER_FRAGMENT;
        return *this;
    }

    FragmentLoader &operator--(int) {
        mCurFragment--;
        if (mCurFragment < 0) {
            mCurFragment = 0;
        }
        mFragmentStart = mCurFragment * SIZE_PER_FRAGMENT;
        return *this;
    }

    const auto &operator[](int sz) const {
        if (sz > mFragmentSize) {
            throw std::out_of_range("sz out of range");
        }
        return mRV[(size_t)(mFragmentStart + sz)];
    }

    auto begin() const {
        return mRV.begin() + mFragmentStart;
    }

    auto end() const {
        return mRV.begin() + mFragmentSize + mFragmentStart;
    }

    int getCurFragment() const {
        return mCurFragment + 1;
    }

    void setCursor(int cur) {
        mCursor = cur;
    }

    int &getCursor() {
        return mCursor;
    }

private:
    int       mCursor        = 0;
    const int mFragments     = 0;
    int       mCurFragment   = 0;
    int       mFragmentStart = 0;
    int       mFragmentSize  = 0;
    T        &mRV;
};