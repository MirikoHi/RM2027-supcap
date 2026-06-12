#pragma once

#include "main.h"
#include "Config.hpp"

/**
 * @brief 系统级数据（全局）
 */
struct SystemData
{
    /** @brief 系统全局时钟刻度 */
    uint32_t vTick = 0;

    /** @brief 系统是否已经初始化 */
    bool systemInited = false;

    /** @brief 硬件唯一 ID（3 字） */
    uint32_t hardwareUID[3];

    /** @brief 低频环路索引 */
    uint8_t lfLoopIndex = 0;
};

/** @brief 全局系统数据实例 */
extern SystemData sysData;

/** @brief DCDC 工作模式 */
enum DCDCMode
{
    BUCK,
    BUCKBOOST,
    BOOSTBUCK,
    BOOST,
    CALIBRATION_A,
    CALIBRATION_B
};

/**
 * @brief 电源模块运行状态
 */
struct PowerState
{
    /** @brief 定时器是否已启用 */
    bool timerEnabled = 0;
    /** @brief A/B 输出是否已启用 */
    bool outputABEnabled = 0;
    /** @brief 是否允许输出（总体开关） */
    bool outputAllow = 1;

    /** @brief 根据电压计算得到的占空比目标 */
    float dutyByVoltage = 0.0f;
    /** @brief A 相比较寄存器值（用于 PWM） */
    uint16_t ACMP3 = 0;
    /** @brief B 相比较寄存器值（用于 PWM） */
    uint16_t BCMP3 = 0;
    /** @brief 当前效率估算 */
    float efficiency = 1.0f;

    /** @brief 软启动计数器 */
    uint8_t softStartCnt = SOFT_START_TIME;
    /** @brief 电感电流限制 */
    float iLLimit = MAX_INDUCTOR_CURRENT;

    /** @brief 当前 DCDC 模式 */
    DCDCMode mode = BUCK;
    /** @brief 目标占空比 */
    float dutyTarget = 0.0f;
    /** @brief IRQ 负载估计（用途详见实现） */
    float IRQLoad = 0.0f;
};

/** @brief 全局电源状态实例 */
extern PowerState psData;