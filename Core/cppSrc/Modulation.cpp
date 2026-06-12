#include "Modulation.hpp"

namespace HRTIM
{
    void startTimer()
    {
        __HAL_HRTIM_MASTER_ENABLE_IT(&hhrtim1, HRTIM_MASTER_IT_MREP);
        HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_MASTER);
        HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
        HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
        psData.timerEnabled = 1;
    }

    void stopTimer()
    {
        disableOutputAB();
        __HAL_HRTIM_MASTER_DISABLE_IT(&hhrtim1, HRTIM_MASTER_IT_MREP);
        HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_MASTER);
        HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_A);
        HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_B);
        psData.timerEnabled = 0;
    }

    bool enableOutputAB()
    {
        if (!psData.timerEnabled || !psData.outputAllow)
        {
            return false;
        }
        HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 + HRTIM_OUTPUT_TA2);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1 + HRTIM_OUTPUT_TB2);
        psData.outputABEnabled = 1;
        return true;
    }

    void disableOutputAB()
    {
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1 + HRTIM_OUTPUT_TA2);
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1 + HRTIM_OUTPUT_TB2);
        psData.outputABEnabled = 0;
    }

    extern SampleManager::ADCData adcData;
    __RAM_FUNC void modeStateMachine()
    {
        // 计算占空比
        psData.dutyByVoltage = psData.dutyTarget;

        // 根据占空比进行状态切换
        switch (psData.mode)
        {
        case BUCK:
            if (psData.dutyByVoltage > 0.84f)
            {
                psData.mode = BUCKBOOST;
            }
            break;
        case BUCKBOOST:
            if (psData.dutyByVoltage < 0.80f)
            {
                psData.mode = BUCK;
            }
            else if (psData.dutyByVoltage > 1.02f)
            {
                psData.mode = BOOSTBUCK;
            }
            break;
        case BOOSTBUCK:
            if (psData.dutyByVoltage < 0.82f)
            {
                psData.mode = BUCK;
            }
            else if (psData.dutyByVoltage < 0.98f)
            {
                psData.mode = BUCKBOOST;
            }
            else if (psData.dutyByVoltage > 1.25f)
            {
                psData.mode = BOOST;
            }
            break;
        case BOOST:
            if (psData.dutyByVoltage < 0.82f)
            {
                psData.mode = BUCK;
            }
            else if (psData.dutyByVoltage < 1.19f)
            {
                psData.mode = BOOSTBUCK;
            }
            break;
        default:
            break;
        }

        // 根据状态计算A和B的比较值
        switch (psData.mode)
        {
        case BUCK:
            psData.ACMP3 = HRTIM_PERIOD - (psData.dutyByVoltage * HRTIM_PERIOD);
            psData.BCMP3 = 0;
            break;
        case BUCKBOOST:
            psData.ACMP3 = HRTIM_PERIOD - (psData.dutyByVoltage * 0.44f * HRTIM_PERIOD + 0.44f * HRTIM_PERIOD);
            psData.BCMP3 = HRTIM_PERIOD - (0.44f * HRTIM_PERIOD / psData.dutyByVoltage + 0.44f * HRTIM_PERIOD);
            break;
        case BOOSTBUCK:
            psData.ACMP3 = HRTIM_PERIOD - (psData.dutyByVoltage * 0.44f * HRTIM_PERIOD + 0.44f * HRTIM_PERIOD);
            psData.BCMP3 = HRTIM_PERIOD - (0.44f * HRTIM_PERIOD / psData.dutyByVoltage + 0.44f * HRTIM_PERIOD);
            break;
        case BOOST:
            psData.ACMP3 = 0;
            psData.BCMP3 = HRTIM_PERIOD - (HRTIM_PERIOD / psData.dutyByVoltage);
            break;
        case CALIBRATION_B:
            // A侧固定80%占空比
            psData.ACMP3 = HRTIM_PERIOD * 0.20f;
            // B侧固定100%占空比
            psData.BCMP3 = 0;
            break;
        case CALIBRATION_A:
            // A侧固定100%占空比
            psData.ACMP3 = 0;
            // B侧固定80%占空比
            psData.BCMP3 = HRTIM_PERIOD * 0.20f;
            break;
        default:
            break;
        }
    }

}