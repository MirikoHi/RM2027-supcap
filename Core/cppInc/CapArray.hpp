#pragma once

#include "main.h"
#include "Config.hpp"

namespace CapArray
{
    /**
     * @brief 电容组状态数据结构
     */
    struct CAPARRStatus
    {
        /**
         * @brief 电容容量估算所用的中间数据
         */
        struct CapacityEstimateData
        {
            /** @brief 估算的电荷变化量 (dQ) */
            float dQ = 0.0f;
            /** @brief 上一次测量的电容电压 */
            float lastVCap = 0.0f;
            /** @brief 上一次更新时间戳（tick） */
            uint32_t lastTick = 0;
            /** @brief 电荷到电压的估算比例，默认值来自 Config */
            float dQtodV = CAPARR_DEFUALT_CAPACITY;
            /** @brief 电压到电荷的估算比例 */
            float dVtodQ = (1.0f / CAPARR_DEFUALT_CAPACITY);

            /** @brief 最大测得的 B 相电流 */
            float maxIB = 0.0f;
            /** @brief 最小测得的 B 相电流 */
            float minIB = 0.0f;
        };

        /** @brief 输出侧允许的最大电流 (A) */
        float maxOutCurrent = 2.0f;
        /** @brief 输入侧允许的最大电流 (A) */
        float maxInCurrent = 2.0f;
        /** @brief 容量估算数据 */
        CapacityEstimateData capEstData;

        /** @brief 警告计数 */
        uint16_t warningCnt = 0;
    };

    /** @brief 全局电容组状态实例 */
    extern CAPARRStatus capStatus;

    /**
     * @brief 更新允许的最大电流（内部策略）
     */
    void updateMaxCurrent();

    /**
     * @brief 获取用于反馈的最大功率值（格式化后用于通信）
     * @return uint16_t 返回功率反馈值
     */
    uint16_t getMaxPowerFeedback();

    /**
     * @brief 重启容量估算（在 tick 时间点）
     * @param _currentTick 当前时钟刻度
     */
    void restartEstimation(const uint32_t& _currentTick);

    /**
     * @brief 用当前采样更新用于估算的电流值
     */
    void updateCurrentforEstimation();

    /**
     * @brief 基于当前时刻估算电容容量
     * @param _currentTick 当前时钟刻度
     */
    void estimateCapacity(const uint32_t &_currentTick);

} // namespace CapArray