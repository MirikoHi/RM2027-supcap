#pragma once

#include "main.h"
#include "Config.hpp"

namespace Protection
{
    enum
    {
        ERROR_POWERSTATE = 0b0000000000000001,
        ERROR_CAPARR = 0b0000000000000010,

        // 短路保护
        ERROR_SCP_A = 0b0000000000000100,
        ERROR_SCP_B = 0b0000000000001000,

        // 过流保护
        ERROR_OCP_A = 0b0000000000010000,
        ERROR_OCP_B = 0b0000000000100000,
        ERROR_OCP_R = 0b0000000001000000,

        // 过压保护
        ERROR_OVP_A = 0b0000000010000000,
        ERROR_OVP_B = 0b0000000100000000,

        // 低电压保护
        WARNING_LOWBATTERY = 0b0000001000000000
    };

    enum ErrorLevel
    {
        NO_ERROR = 0,             // 无错误
        ERROR_RECOVER_AUTO = 1,   // 错误，可通过自动恢复
        ERROR_RECOVER_MANUAL = 2, // 错误，可通过发信息恢复
        ERROR_UNRECOVERABLE = 3,  // 错误，不可恢复
        WARNING = 4               // 警告
    };

    struct ErrorData
    {
        uint16_t errorCode = 0;
        uint16_t shortCircuitCnt = 0;
        uint16_t overVoltageCnt = 0;
        uint16_t overCurrentCnt = 0;
        bool lowBattery = 0;
        uint16_t lowBatteryCnt = 0;       // 低电压计数
        ErrorLevel errorLevel = NO_ERROR; // 错误等级
        uint32_t powerOffCnt = 0;         // 关机计数

        float errorVoltage = 0.0f;
        float errorCurrent = 0.0f;
    };

    extern ErrorData errorData;

    /// @brief 错误处理主函数
    void errorHandlerLF();

    /// @brief 检查低电池电压保护
    void checkLowBatteryLF();

    /// @brief 检查短路保护
    void checkShortCircuit();

    /// @brief 计算电源转换效率
    void checkEfficiency();

    /// @brief HRTIM 故障中断处理函数
    void hrtimFaultHandler();

    /// @brief 检查硬件唯一 ID
    bool checkHardwareUID();

    /// @brief 自动恢复错误清除函数
    void autoClearError();

    /// @brief 手动恢复错误清除函数
    void manualClearError();

    /// @brief CAN通信超时检测
    void checkRxDataTimeout(const uint32_t &currentTick);
}