#include "Protection.hpp"
#include "SampleManager.hpp"
#include "SystemState.hpp"
#include "Modulation.hpp"
#include <cstring>

const SampleManager::ADCFitParaTypeDef adcFitList[ADC_FIT_LIST_NUM] = {

};

namespace Protection
{
    ErrorData errorData;
    
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

    __RAM_FUNC void checkShortCircuit()
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
                errorData.errorVoltage = SampleManager::adcData.vB;
                errorData.errorCurrent = SampleManager::adcData.iB;
            }
        }
    }

    __RAM_FUNC void checkEfficiency()
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

    void hrtimFaultHandler()
    { // 过压/过流保护触发

        if (SampleManager::adcData.vA > OVP_A) // vA过压保护触发
        {
            errorData.errorCode |= ERROR_OVP_A;
            errorData.errorLevel = ERROR_RECOVER_AUTO;
            HRTIM::disableOutputAB();
        }
        if (SampleManager::adcData.iA > OCP_CAPARR) // iA过流保护触发
        {
            errorData.errorCode |= ERROR_OCP_A;
            errorData.errorLevel = ERROR_RECOVER_AUTO;
            HRTIM::disableOutputAB();
        }
        if (SampleManager::adcData.iReferee > OCP_REFEREE) // iR过流保护触发
        {
            errorData.errorCode |= ERROR_OCP_R;
            errorData.errorLevel = ERROR_RECOVER_AUTO;
            HRTIM::disableOutputAB();
        }
        if (SampleManager::adcData.vB > OVP_B) // vB过压保护触发
        {
            errorData.errorCode |= ERROR_OVP_B;
            errorData.errorLevel = ERROR_RECOVER_AUTO;
            HRTIM::disableOutputAB();
        }
        if (SampleManager::adcData.iB > OCP_CAPARR) // iB过流保护触发
        {
            errorData.errorCode |= ERROR_OCP_B;
            errorData.errorLevel = ERROR_RECOVER_AUTO;
            HRTIM::disableOutputAB();
        }
    }

    bool checkHardwareUID()
    {
        // 读取寄存器的UID

        sysData.hardwareUID[0] = HAL_GetUIDw0();
        sysData.hardwareUID[1] = HAL_GetUIDw1();
        sysData.hardwareUID[2] = HAL_GetUIDw2();

        for (uint8_t i = 0; i < ADC_FIT_LIST_NUM; i++)
        {
            if (!memcmp((const void *)sysData.hardwareUID, &adcFitList[i], 12))
            {
                SampleManager::adcFitPara = adcFitList[i];
                return true;
            }
        }

        return false;
    }

    void autoClearError()
    {
        if (errorData.errorLevel == ERROR_RECOVER_AUTO)
        {
            errorData.errorCode = 0;
            errorData.overCurrentCnt = 0;
            errorData.overVoltageCnt = 0;
            errorData.errorLevel = NO_ERROR;
            HRTIM::enableOutputAB();
        }
    }
    void manualClearError()
    {
        if (errorData.errorLevel == ERROR_RECOVER_MANUAL)
        {
            errorData.errorCode = 0;
            errorData.shortCircuitCnt = 0;

            errorData.errorLevel = NO_ERROR;
            HRTIM::enableOutputAB();
        }
    }
    
    void checkRxDataTimeout(const uint32_t &currentTick)
    {
        
    }
} // namespace Protection
