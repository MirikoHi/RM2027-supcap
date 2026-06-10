#include "CapArray.hpp"
#include "SampleManager.hpp"

namespace CapArray
{
    CAPARRStatus capStatus;

    __attribute__((section(".code_in_ram"))) void updateMaxCurrent()
    {
        if (SampleManager::adcData.vCap > CAPARR_LOW_VOLTAGE)
        {
            capStatus.maxOutCurrent = CAPARR_MAX_CURRENT;
            capStatus.maxInCurrent = CAPARR_MAX_CURRENT;
        }
        else if (SampleManager::adcData.vCap > CAPARR_CUTOFF_VOLTAGE)
        {
            capStatus.maxOutCurrent = (CAPARR_MAX_CURRENT - 1.0f) / (CAPARR_LOW_VOLTAGE - CAPARR_CUTOFF_VOLTAGE) * (SampleManager::adcData.vCap - CAPARR_CUTOFF_VOLTAGE) + 1.0f;
            capStatus.maxInCurrent = capStatus.maxOutCurrent;
        }
        else
        {
            capStatus.maxOutCurrent = 0.2f + (0.8f / 5.0f) * SampleManager::adcData.vCap;
            capStatus.maxInCurrent = 1.0f;
        }
    }

    __attribute__((section(".code_in_ram"))) inline void updateCurrentforEstimation()
    {
        capStatus.capEstData.maxIB = M_MAX(capStatus.capEstData.maxIB, SampleManager::adcData.iB);
        capStatus.capEstData.minIB = M_MIN(capStatus.capEstData.minIB, SampleManager::adcData.iB);
    }

    uint16_t getMaxPowerFeedback()
    {
        if (SampleManager::adcData.vCap > CAPARR_LOW_VOLTAGE)
        {
            return (uint16_t)(CM01_CURRENT_LIMIT * SampleManager::adcData.vCap);
        }
        else if (SampleManager::adcData.vCap > CAPARR_CUTOFF_VOLTAGE)
        {
            return (uint16_t)((CM01_CURRENT_LIMIT - 1.0f) / (CAPARR_LOW_VOLTAGE - CAPARR_CUTOFF_VOLTAGE) * (SampleManager::adcData.vCap - CAPARR_CUTOFF_VOLTAGE) + 1.0f) * SampleManager::adcData.vCap;
        }
        else
        {
            return (uint16_t)(0.2f + (0.8f / 5.0f) * SampleManager::adcData.vCap) * SampleManager::adcData.vCap;
        }
    }

    void restartEstimation(const uint32_t &_currentTick)
    {
        capStatus.capEstData.dQ = 0.0f;
        capStatus.capEstData.lastVCap = SampleManager::adcData.vCap;
        capStatus.capEstData.maxIB = SampleManager::adcData.iB;
        capStatus.capEstData.minIB = SampleManager::adcData.iB;
        capStatus.capEstData.lastTick = _currentTick;
    }

    void estimateCapacity(const uint32_t &_currentTick)
    {
        capStatus.capEstData.dQ += SampleManager::adcData.iCap;

        if (M_ABS(SampleManager::adcData.vCap - capStatus.capEstData.lastVCap) > 0.7f) // 电压变化超过阈值
        {
            if (M_ABS(capStatus.capEstData.maxIB - capStatus.capEstData.minIB) < 4.5f)
            {
                capStatus.capEstData.dQtodV = capStatus.capEstData.dQ * (1.0f / 1000.0f) / (SampleManager::adcData.vCap - capStatus.capEstData.lastVCap);

                if (capStatus.capEstData.dQtodV > CAPARR_CAPACITY_HT || capStatus.capEstData.dQtodV < CAPARR_CAPACITY_LT)
                    capStatus.warningCnt += 9;
                else if (capStatus.warningCnt > 0)
                    capStatus.warningCnt--;
            }
            restartEstimation(_currentTick);
        }
        else if (M_ABS(capStatus.capEstData.dQ) > 600.0f) // 累计电荷变化超过阈值
        {
            if (M_ABS(capStatus.capEstData.maxIB - capStatus.capEstData.minIB) < 4.5f)
            {
                capStatus.capEstData.dVtodQ = (SampleManager::adcData.vCap - capStatus.capEstData.lastVCap) / (capStatus.capEstData.dQ * (1.0f / 1000.0f));

                if (capStatus.capEstData.dVtodQ < (1.0f / CAPARR_CAPACITY_HT) || capStatus.capEstData.dVtodQ > (1.0f / CAPARR_CAPACITY_LT))
                    capStatus.warningCnt += 4;
                else if (capStatus.warningCnt > 0)
                    capStatus.warningCnt--;
            }
            restartEstimation(_currentTick);
        }
        else if (_currentTick - capStatus.capEstData.lastTick > 1000) //<这个timeout控制触发阈值，现在0.4A漏电稳定触发，0.3A稳定不触发
        {
            if (capStatus.warningCnt > 0)
                capStatus.warningCnt--;
            restartEstimation(_currentTick); // 如果过了timeout，重新开始估算
        }

        if (capStatus.warningCnt > 15)
        {
            capStatus.warningCnt = 0;
        }
    }
}