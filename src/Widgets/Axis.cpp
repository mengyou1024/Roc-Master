#include "pch.h"

#include "Axis.h"

CAxis::CAxis(void) {
}

CAxis::~CAxis(void) {
}

void CAxis::Set(int iLen, int iCount, float fMin, float fMax) {
    float fScale = float(iLen) / float(iCount);
    int   iSize  = iCount + 1;

    float fStep = (fMax - fMin) / float(iCount);

    m_pAxis.resize(iSize);
    m_pAxis_value.resize(iSize);
    m_pAxisType.resize(iSize);

    for (int i = 0; i < iSize; i++) {
        m_pAxis[i]       = fScale * i;       // 坐标像素
        m_pAxis_value[i] = fMin + i * fStep; // 坐标实际值

        if (i % 10 == 0)
            m_pAxisType[i] = 2; // 区分大小刻度
        else if (i % 5 == 0)
            m_pAxisType[i] = 1;
        else
            m_pAxisType[i] = 0;
    }
}

void CAxis::Set(int iLen, float fMin, float fMax) {
    float min = ::min(fMin, fMax);
    float max = ::max(fMin, fMax);

    // 计算像素大小
    float fPixel = (max - min) / iLen;
    // iAxis:10*每刻度大小，去掉小数值取整
    int iAxis = (int(fPixel * 10 * 10) / 5 + 1) * 5;

    // 刻度过大时，去除小刻度
    int iComp = 100;
    int iScal = 50;
    while (iAxis > iComp) {
        iAxis = iAxis / iScal * iScal;
        iComp *= 10;
        iScal *= 10;
    }

    // 计算左侧最小刻度
    float fLeft = float((int(min * 10) / iAxis) * iAxis) / 10.0F;
    if (fLeft < min)
        fLeft += (iAxis / 10.0F);

    // 计算刻度总数
    int iSize = int((max - fLeft) * 10 / iAxis + 1);

    m_pAxis.resize(iSize);
    m_pAxis_value.resize(iSize);
    m_pAxisType.resize(iSize);

    for (int i = 0; i < iSize; i++) {
        m_pAxis_value[i] = fLeft + i * iAxis / 10.0F;
        if (m_pAxis_value[i] / iAxis == int(m_pAxis_value[i]) / iAxis)
            m_pAxisType[i] = 2;
        else if (m_pAxis_value[i] * 2 / iAxis == int(m_pAxis_value[i] * 2) / iAxis)
            m_pAxisType[i] = 1;
        else
            m_pAxisType[i] = 0;

        m_pAxis[i] = iLen * (m_pAxis_value[i] - fMin) / (fMax - fMin);
    }
}

void CAxis::Set(int iLen, int iAxisScale, int iZero) {
    int iSize = iLen / iAxisScale + 1;
    m_pAxis.resize(iSize);
    m_pAxisType.resize(iSize);
    int iIndex = 0;
    for (int i = 0; i < iLen; i++) {
        if ((i - iZero) % (iAxisScale * 10) == 0) {
            m_pAxis[iIndex]     = (float)i;
            m_pAxisType[iIndex] = 2;
            iIndex++;
        } else if ((i - iZero) % (iAxisScale * 5) == 0) {
            m_pAxis[iIndex]     = (float)i;
            m_pAxisType[iIndex] = 1;
            iIndex++;
        } else if ((i - iZero) % iAxisScale == 0) {
            m_pAxis[iIndex]     = (float)i;
            m_pAxisType[iIndex] = 0;
            iIndex++;
        }
    }
}

float& CAxis::operator[](size_t iIndex) {
    return m_pAxis[iIndex];
}