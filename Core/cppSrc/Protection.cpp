#include "Protection.hpp"
#include "SampleManager.hpp"
#include "SystemState.hpp"
#include "Modulation.hpp"

Protection::ErrorData errorData;

namespace Protection
{
    void errorHandlerLF()
    {
        if (errorData.errorLevel == WARNING)
        {
            // 逐渐减小errorCnt
            if (errorData.overCurrentCnt > 0)
            {
                errorData.overCurrentCnt--;
            }
            if (errorData.overVoltageCnt > 0)
            {
                errorData.overVoltageCnt--;
            }
            if (errorData.shortCircuitCnt > 0)
            {
                errorData.shortCircuitCnt--;
            }
            // 如果Cnt都为0则解除警告状态
            if (!(errorData.overCurrentCnt || errorData.overVoltageCnt || errorData.shortCircuitCnt || errorData.lowBattery))
            {
                errorData.errorLevel = NO_ERROR;
            }
        }

        if (SampleManager::adcData.vA < REFEREE_UVLO_LIMIT)
        {
            errorData.powerOffCnt++;
        }
        else
        {
            errorData.powerOffCnt = 0;
        }

        if (errorData.powerOffCnt > 2000 && errorData.errorLevel != NO_ERROR)
        {
            autoClearError();
            manualClearError();
        }
    }

    void checkLowBatteryLF()
    {
        // 检测低电压保护
        if (errorData.lowBattery)
        {
            if (SampleManager::adcData.vA > BATTERY_LOW_RECOVERY || SampleManager::adcData.vA < REFEREE_UVLO_LIMIT)
            {
                errorData.lowBattery = 0;
                errorData.errorCode &= ~WARNING_LOWBATTERY;
                if (!errorData.errorCode)
                    errorData.errorLevel = NO_ERROR;
            }
        }
        else
        {
            if (SampleManager::adcData.vA < BATTERY_LOW_LIMIT && SampleManager::adcData.vA > REFEREE_UVLO_RECOVERY)
            {
                errorData.lowBatteryCnt++;
                if (errorData.lowBatteryCnt > 1000) // 低电压持续1
                {
                    if (errorData.errorLevel == NO_ERROR)
                    {
                        errorData.errorLevel = WARNING;
                    }
                    errorData.lowBattery = 1;
                    errorData.errorCode |= WARNING_LOWBATTERY;
                    errorData.lowBatteryCnt = 0;
                }
            }
        }
    }

    __attribute__((section(".RamFunc"))) void checkShortCircuit()
    {
        // 检测短路保护，在ADC解码后立刻调用
        if (SampleManager::adcData.vA <= SCP_VOLTAGE && -SampleManager::adcData.iA >= SCP_CURRENT)
        {
            // 裁判系统端或底盘端短路
            errorData.errorLevel = WARNING;
            errorData.shortCircuitCnt += 600;
            if (errorData.shortCircuitCnt > 1700)
            {
                HRTIM::disableOutputAB();
                errorData.errorCode |= ERROR_SCP_A;
                errorData.errorLevel = ERROR_RECOVER_MANUAL;
                errorData.errorVoltage = SampleManager::adcData.vA;
                errorData.errorCurrent = -SampleManager::adcData.iA;
            }
        }

        if (SampleManager::adcData.vB <= SCP_VOLTAGE && SampleManager::adcData.iB >= SCP_CURRENT)
        {
            // 电容端或无线充电端短路（无线充电端短路会通过buck上管短路B端）
            errorData.errorLevel = WARNING;
            errorData.shortCircuitCnt += 300;
            // if(adcData.vB <= 0.08f)
            //     errorData.shortCircuitCnt -= 600;
            // else if(adcData.vB <= 0.2f)
            //     errorData.shortCircuitCnt -= 550;

            if (errorData.shortCircuitCnt > 1700)
            { //
                HRTIM::disableOutputAB();
                errorData.errorCode |= ERROR_SCP_B;
                errorData.errorLevel = ERROR_RECOVER_MANUAL;
                errorData.errorVoltage = psData.softStartCnt; // adcData.vB;
                errorData.errorCurrent = SampleManager::adcData.iB;
            }
        }
    }

    __attribute__((section(".RamFunc"))) void checkEfficiency()
    {
        if (SampleManager::adcData.iA > 0.5f)
        {
            psData.efficiency = (SampleManager::adcData.vB * SampleManager::adcData.iB) / (SampleManager::adcData.vA * SampleManager::adcData.iA);
        }
        else if (SampleManager::adcData.iA < -0.5f)
        {
            psData.efficiency = (SampleManager::adcData.vA * SampleManager::adcData.iA) / (SampleManager::adcData.vB * SampleManager::adcData.iB);
        }
    }
}