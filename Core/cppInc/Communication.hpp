#pragma once

#include "main.h"
#include "fdcan.h"
#include "Config.hpp"

namespace Communication
{
    struct __packed RxData
    {
        uint8_t enableDCDC : 1;                // 允许启动DCDC
        uint8_t systemRestart : 1;             // 系统重启
        uint8_t resv0 : 3;                     //
        uint8_t clearError : 1;                // 手动清除可清除的错误
        uint8_t enableActiveChargingLimit : 1; // 是否启用主动充电限制
        uint8_t useNewFeedbackMessage : 1;     // 是否使用新的反馈消息格式

        uint16_t refereePowerLimit;       // 裁判限制功率，单位W
        uint16_t refereeEnergyBuffer;     // 裁判能量缓冲，单位J
        uint8_t activeChargingLimitRatio; // 主动充电限制比例（能量），0-255
        uint16_t resv2;                   //
    };

    extern RxData rxData;

    struct __packed TxData
    {                               // 0x051 (useNewFeedbackMessage = 0)
        uint8_t statusCode;         // 状态信息
        float chassisPower;         // 底盘功率，单位W
        uint16_t chassisPowerLimit; // 底盘最大可用功率（包括裁判系统）
        uint8_t capEnergy;          // 电容现有能量，0-255
    };

    struct __packed TxDataNew
    {                               // 0x052 (useNewFeedbackMessage = 1)
        uint8_t statusCode;         // 状态信息
        uint16_t chassisPower;      // 底盘功率，功率*64+16384 (-256W~+768W, 精度0.015625)
        uint16_t refereePower;      // 裁判系统功率，功率*64+16384 (-256W~+768W, 精度0.015625)
        uint16_t chassisPowerLimit; // 底盘最大可用功率（包括裁判系统）
        uint8_t capEnergy;          // 电容现有能量，0-255
    };

    extern TxData txData;
    extern TxDataNew txDataNew;

    void init();

    void generateTxData(TxData &td);

    void generateTxDataNew(TxDataNew &td);
    
    void sendData();

    void receiveData(const RxData &rd);

} // namespace Communication