#pragma once

#include <cstdint>

#ifdef DLL_IMPLEMENT
    #define DLL_API __declspec(dllexport)
#else
    #define DLL_API __declspec(dllimport)
#endif

typedef struct _tagNM_DATA {
    __int32        iAxle;       // 轴号
    __int32        iChannel;    // 通道号
    __int32        iPackage;    // 包序列
    __int32        iAScanSize;  // A扫长度
    unsigned char *pAscan;      // A扫数据
    __int32        pCoder[2];   // 编码器值
    __int32        pGatePos[2]; // 波门位置
    unsigned char  pGateAmp[2]; // 波门波幅
    __int32        pAlarm[2];   // 波门报警
} NM_DATA, *P_NM_DATA;

typedef struct _tagNM_IP_CONFIG {
    unsigned char pIpFPGA[4];  // 超声板IP,pIpFPGA[0]=192,pIpFPGA[1]=168,pIpFPGA[2]=1,pIpFPGA[3]=110
    unsigned int  iPortFPGA;   // 超声板端口,3500
    unsigned char pIpPC[4];    // PC IP,pIpPC[0]=192,pIpPC[1]=168,pIpPC[2]=1,pIpPC[3]=100
    unsigned int  iPortPC;     // PC 端口,4200
    unsigned char pGateway[4]; // 网关,pGateway[0]=192,pGateway[1]=168,pGateway[2]=1,pGateway[3]=1
    unsigned char pSubnet[4];  // 子网掩码,pSubnet[0]=255,pSubnet[1]=255,pSubnet[2]=255,pSubnet[3]=0
    unsigned char pMAC[6];     // 超声板MAC地址,pMAC[0]=0,pMAC[1]=10,pMAC[2]=53,pMAC[3]=1,pMAC[4]=254,pMAC[5]=192
} NM_IP_CONFIG, *P_NM_IP_CONFIG;

/************************************************************************/
/*连接板卡，设置板卡FPGA与PC电脑IP地址                                  */
/*返回板卡编号，失败返回-1			                                    */
/************************************************************************/
extern "C" DLL_API int UNION_PORT_Open(char *pFPGA_IP = const_cast<char *>("192.168.1.110"), unsigned short iPortFPGA = 3500, char *pPC_IP = const_cast<char *>("192.168.1.100"), unsigned short iPortPC = 4200);

/************************************************************************/
/*板卡是否连接                                                          */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_IsOpen(int iIndex);

/************************************************************************/
/* 断开板卡                                                             */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API void UNION_PORT_Close(int iIndex);

/************************************************************************/
/* 版本号信息															*/
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/* 6Bytes,前2Bytes标识版本号，后4Bytes表示修改日期                      */
/* 例: {0x01, 0x03, 0x14, 0x15, 0x0A, 0x19}                             */
/* Version:1.3                                                          */
/* 修改日期:2021-10-25(0x14*100+0x15 = 2021, 0x0A=10, 0x19=25)          */
/************************************************************************/
extern "C" DLL_API unsigned char *UNION_PORT_GetVersion(int iIndex);

/************************************************************************/
/* 设置超声板卡通信IP, 重启后生效                                       */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetIP(int iIndex, NM_IP_CONFIG IP);

/************************************************************************/
/* 获取超声板卡IP地址                                                   */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API P_NM_IP_CONFIG UNION_PORT_GetIP(int iIndex);

/************************************************************************/
/* 获取通道A扫长度                                                      */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API unsigned int UNION_PORT_GetAScanSize(int iIndex, int iChannel);

/************************************************************************/
/*设置重复频率[50Hz, 5000Hz]                                            */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetFrequency(int iIndex, int iFrequency);

/************************************************************************/
/*设置高压.0:低压，1:中压，2:高压                                       */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetVoltage(int iIndex, int iVoltage);

/************************************************************************/
/*设置脉冲宽度.1:宽(200),0:窄(100)                                      */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetPulseWidth(int iIndex, int iPulseWidth);

/************************************************************************/
/*设置通道发射标志.                                                     */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/*iTxFlag:bit0～bit9分表代表通道0到通道9，1表示选中，0表示忽略          */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetTxFlag(int iIndex, int iTxFlag);

/************************************************************************/
/*设置通道接收标志.                                                     */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/*iRxFlag:bit0～bit9分表代表通道0到通道9，1表示选中，0表示忽略          */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetRxFlag(int iIndex, int iRxFlag);

/************************************************************************/
/*设置通道工作模式.                                                     */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/*iChMode:bit0～bit9分表代表通道0到通道9，1表示双晶模式，0表示普通模式  */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetChMode(int iIndex, int iChMode);

/************************************************************************/
/* 扫查增量，预留                                                       */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetScanIncrement(int iIndex, int iScanIncrement);

/************************************************************************/
/* 编码器清零，0：计数，1：清零                                         */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_ResetCoder(int iIndex, int iResetCoder);

/************************************************************************/
/* 系统工作模式:                                                        */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/* 0:手动工作模式，通过StartSequencer与StopSequencer控制超声板工作状态  */
/* 1:自动工作模式，由控制系统发来的工作控制信号和计轴信号控制           */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetWorkType(int iIndex, int iWorkType);

/************************************************************************/
/* 控制信号消抖时间:                                                    */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/* iTimes: 0 - 255, ms													*/
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetControlTime(int iIndex, int iTimes);

/************************************************************************/
/* 计轴信号消抖时间:                                                    */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/* iTimes: 0 - 255, ms													*/
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetAxleTime(int iIndex, int iTimes);

/************************************************************************/
/* 设置通道延时，[-10.24us, 1700us]                                     */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetDelay(int iIndex, int iChannel, float fDelay /*us*/);

/************************************************************************/
/* 设置通道采样深度，[0us, 1700us]                                      */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetSampleDepth(int iIndex, int iChannel, float fSampleDepth /*us*/);

/************************************************************************/
/* 设置通道采样因子，[1, 255]                                           */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetSampleFactor(int iIndex, int iChannel, int iSampleFactor);

/************************************************************************/
/* 设置通道增益，[0, 110dB]                                             */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetGain(int iIndex, int iChannel, float fGain /*dB*/);

/************************************************************************/
/* 设置滤波频带，0x00:不滤波, 0x01：频率2.5MHz, 0x02：频率5.0MHz        */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetFilter(int iIndex, int iChannel, int iFilter);

/************************************************************************/
/* 设置检波方式，0x00:射频, 0x01：全波, 0x02：正半波，0x03:负半波       */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetDemodu(int iIndex, int iChannel, int iDemodu);

/************************************************************************/
/* 相位反转，0x00:不反转, 0x01：反转                                    */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/*iChannel:通道编号			                                            */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetPhaseReverse(int iIndex, int iChannel, int iPhaseReverse);

/************************************************************************/
/* 设置波门信息                                                         */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/*iChannel:通道编号			                                            */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetGateInfo(int iIndex, int iChannel, int iGate, int iActive, int iAlarmType, float fPos /*%*/, float fWidth /*%*/, float fHeight /*%*/);

/************************************************************************/
/* 设置波门2类型, 0：固定，1:跟随以波门1最高回波为0点                   */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/*iChannel:通道编号			                                            */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetGate2Type(int iIndex, int iChannel, int iType);

/************************************************************************/
/* 参数下发后生效                                                       */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_FlushSetting(int iIndex);

/************************************************************************/
/*获取配置结果                                                          */
/*return:1:成功，2:失败，0:未获取到结果                                 */
/************************************************************************/
extern "C" DLL_API int  UNION_PORT_GetFlushResult(int iIndex);
extern "C" DLL_API bool UNION_PORT_ClearFlushResult(int iIndex);

/************************************************************************/
/* 手动工作模式下开始采样                                               */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_StartSequencer(int iIndex);

/************************************************************************/
/* 手动工作模式下停止采样                                               */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_StopSequencer(int iIndex);

/************************************************************************/
/* 获取丢包率                                                           */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/*iChannel:通道编号			                                            */
/************************************************************************/
extern "C" DLL_API float UNION_PORT_GetPacketLoss(int iIndex, int iChannel);

/************************************************************************/
/* 获取波形数据                                                         */
/*iIndex:板卡编号，由UNION_PORT_Open获得                                */
/************************************************************************/
extern "C" DLL_API NM_DATA *UNION_PORT_ReadDatas(int iIndex);
