#ifndef _TOFDPORT_H_
#define _TOFDPORT_H_

#ifdef DLL_IMPLEMENT  
#define Dll_API _declspec(dllexport)
#else  
#define Dll_API _declspec(dllimport)  
#endif  

namespace TOFDPort
{
	typedef struct _tagNM_DATA
	{
		int iChannel;					//通道号
		int iPackage;					//包序列
		int iAScanSize;					//A扫长度
		unsigned char* pAscan;			//A扫数据
		int pCoder[2];					//编码器值
		int pCoderZF[2];				//Z值第一圈
		int pCoderZC[2];				//Z值圈数
		int pCoderZ[2];					//Z值
		int pGatePos[2];				//波门位置
		unsigned char pGateAmp[2];		//波门波幅
		int pAlarm[2];					//波门报警
	}NM_DATA, * P_NM_DATA;

	//连接超声板卡
	Dll_API bool TOFD_PORT_OpenDevice(int type);
	//断开已连接的超声板
	Dll_API bool TOFD_PORT_CloseDevice();
	//判断超声板与电脑是否连接
	Dll_API bool TOFD_PORT_IsOpen();
	Dll_API bool TOFD_PORT_IsConfigChange();
	//判断超声板与电脑是否连接
	Dll_API bool TOFD_PORT_IsDeviceExist();

	//设置总重复频率
	Dll_API bool TOFD_PORT_SetFrequency(int iFrequency);
	//设置高压
	Dll_API bool TOFD_PORT_SetVoltage(int iVoltage);
	//设置通道标志
	Dll_API bool TOFD_PORT_SetChannelFlag(int iChannelFlag);
	//设置扫查增量
	Dll_API bool TOFD_PORT_SetScanIncrement(int iScanIncrement);
	//设置编码器清零标志位
	Dll_API bool TOFD_PORT_SetResetCoder(int iResetCoder);
	//设置系统指示灯
	Dll_API bool TOFD_PORT_SetLED(int iLEDStatus);
	//设置公共通道继电器
	Dll_API bool TOFD_PORT_SetDamperFlag(int iDamperFlag);
	//设置编码器触发脉冲
	Dll_API bool TOFD_PORT_SetEncoderPulse(int iEncoderPulse);

	//设置通道脉冲宽度
	Dll_API bool TOFD_PORT_SetPulseWidth(int iChannel, float fPulseWidth/*ns*/);
	//设置通道延时
	Dll_API bool TOFD_PORT_SetDelay(int iChannel, float fDelay/*us*/);
	//设置通道采样深度
	Dll_API bool TOFD_PORT_SetSampleDepth(int iChannel, float fSampleDepth/*us*/);
	//设置通道采样因子
	Dll_API bool TOFD_PORT_SetSampleFactor(int iChannel, int iSampleFactor);
	//设置通道增益
	Dll_API bool TOFD_PORT_SetGain(int iChannel, float fGain/*dB*/);
	//设置滤波频带
	Dll_API bool TOFD_PORT_SetFilter(int iChannel, int iFilter);
	//设置检波方式
	Dll_API bool TOFD_PORT_SetDemodu(int iChannel, int iDemodu);
	//设置A扫信号相位
	Dll_API bool TOFD_PORT_SetPhaseReverse(int iChannel, int iPhaseReverse);


	//设置波门信息
	Dll_API bool TOFD_PORT_SetGateInfo(int iChannel, int iGate, int iActive, int iAlarmType, float fPos/*%*/, float fWidth/*%*/, float fHeight/*%*/);
	//设置波门2类型
	Dll_API bool TOFD_PORT_SetGate2Type(int iChannel, int iType);
	//下发参数
	Dll_API bool TOFD_PORT_FlushSetting();

	//获取数据
	Dll_API bool TOFD_PORT_ReadDatas(unsigned char* pBuff, unsigned int iSize);
	//获取NM_DATA数据包
	Dll_API NM_DATA* TOFD_PORT_ReadDatasFormat();
	//释放NM_DATA对象
	Dll_API void TOFD_PORT_Free_NM_DATA(NM_DATA* pData);
	//获取编码器值
	Dll_API bool TOFD_PORT_GetCoderValue(int* pCoder0, int* pCoder1);
	//获取编码器值
	Dll_API bool TOFD_PORT_GetCoderValueZ(int* pCoderZ0, int* pCoderZ1, int* pCoderF0, int* pCoderF1, int* pCoderC0, int* pCoderC1);

	//编码器清零
	Dll_API bool TOFD_PORT_ResetCoder_Immediate();
}


//扩展项

namespace TOFDPortExtensions
{
	//动态库初始化一次
	Dll_API void ITS_init();
	//usb是否插入
	Dll_API bool ITS_IsExist();
	//usb是否已打开
	Dll_API bool ITS_IsOpen();
	//打开USB
	Dll_API bool ITS_OpenUSB();
	//关闭USB
	Dll_API void ITS_CloseUSB();

	//设置通道
	Dll_API void ITS_SetCh(unsigned int ch_left_s, unsigned int ch_left_r, unsigned int ch_right_s, unsigned int ch_right_r);
	//设置通道频率 [当前取左右设置值的和 hz_left+hz_right 为总重复频率)
	Dll_API void ITS_SetHZ(unsigned int hz_left, unsigned int hz_right);
	//设置硬件零点 [In 采样点数 10ns一个点]
	Dll_API void ITS_SetHard_Delayns(unsigned int delayns);

	//设置脉冲宽度 [In 单位10ns  例如plus_left = 16 表示160ns]
	Dll_API void ITS_SetPlusWidth(unsigned int plus_left, unsigned int plus_right);
	//设置延迟 [In 采样点数 10ns一个点]
	Dll_API void ITS_SetXmove(unsigned int xmove_left, unsigned int xmove_right);
	//设置增益 [In 单位0.1db 例如dB_left = 312 表示31.2dB]
	Dll_API void ITS_SetdB(unsigned int dB_left, unsigned int dB_right);
	//设置声程 [In 采样点数 10ns一个点]
	Dll_API void ITS_SetDis(unsigned int dis_left, unsigned int dis_right);


	//出厂测试(设置公共通道)
	Dll_API void ITS_Selfcheck(unsigned int ch_left, unsigned int ch_right);
	//设置水平基线位置
	Dll_API void ITS_SetZeroLeavel(unsigned int zl_left, unsigned int  zl_right);
	//设置出厂测试接口DA
	Dll_API void ITS_TestSetDB(unsigned int  dB_A1, unsigned int  dB_A2, unsigned int  dB_A3, unsigned int  dB_B1, unsigned int  dB_B2, unsigned int  dB_B3);
	//获取出厂测试接口DA
	Dll_API void ITS_TestGetDB(unsigned int* dB_A1, unsigned int* dB_A2, unsigned int* dB_A3, unsigned int* dB_B1, unsigned int* dB_B2, unsigned int* dB_B3);

	//设置压缩比 [单次采样点 相对于1024的比]
	Dll_API void ITS_SetZip(unsigned int zip_left, unsigned int zip_right);
	//计算压缩比
	Dll_API unsigned int ITS_GetZip(unsigned int ch);


	//重置编码器
	Dll_API void ITS_ReSetEncoder(unsigned int ch, unsigned int flag);
	//获取编码器的值
	Dll_API signed int ITS_GetEncoder(unsigned int ch, unsigned int mode);

	//启动采集
	Dll_API bool ITS_Start(unsigned char* buf_left, unsigned char* buf_right);

	///(板卡不包含该功能)

	//设置编码器频率
	Dll_API void ITS_SetEncoder_frequency(unsigned int ch, unsigned int freq);
	//设置编码器频率
	Dll_API unsigned int ITS_GetWorkID();

	///疑问项 ：1.通道频率 2.获取编码器

}

#endif