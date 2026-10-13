#pragma once

#include "main.h"
#include "Config.hpp"

namespace CapArray
{
    struct CAPARRStatus
    {
        struct CapacityEstimateData
        {
            float dQ = 0.0f;
            float lastVCap = 0.0f;
            uint32_t lastTick = 0;
            float dQtodV = CAPARR_DEFUALT_CAPACITY;
            float dVtodQ = (1.0f / CAPARR_DEFUALT_CAPACITY);

            float maxIB = 0.0f;
            float minIB = 0.0f;
        };

        float maxOutCurrent = 2.0f;
        float maxInCurrent = 2.0f;
        CapacityEstimateData capEstData;

        uint16_t warningCnt = 0; // 警告计数
    };
    extern CAPARRStatus capStatus;

    void updateMaxCurrent();

    uint16_t getMaxPowerFeedback();

    void restartEstimation(const uint32_t& _currentTick);

    void updateCurrentforEstimation();

    void estimateCapacity(const uint32_t &_currentTick);

} // namespace CapArray