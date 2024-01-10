#pragma once

class CAxis
{
public:
	CAxis(void);
	~CAxis(void);

	void Set( int iLen, int iCount, float fMin, float fMax );
	void Set( int iLen, float fMin, float fMax);
	void Set( int iLen, int iAxisScale, int iZero );

	size_t GetCount(){ return m_pAxis.size(); }
	float& operator[]( size_t iIndex );

public:
	vector<FLOAT> m_pAxis;			//刻度
	vector<int> m_pAxisType;		//0:普通刻度，1:小刻度， 2：大刻度
	vector<FLOAT> m_pAxis_value;	//刻度值
};