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
    __attribute__((section(".RamFunc"))) void modeStateMachine()
    {
        // 根据电压计算占空比
        psData.dutyByVoltage = M_MAX(adcData.vB, 0.01f) / adcData.vA;

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

        // 根据状态操作HRTIM寄存器，并分别计算A和B的占空比
        switch (psData.mode)
        {
        case BUCK:
            // A侧限制94%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.06f);
            // B侧常开
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, 0U);

            break;
        case BUCKBOOST:
            // A侧限制94%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.06f);
            // B侧固定84%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.16f);

            break;
        case BOOSTBUCK:
            // A侧固定84%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.16f);
            // B侧限制94%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.06f);
            break;
        case BOOST:
            // A侧常开
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, 0U);
            // B侧限制94%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.06f);
            break;
        case CALIBRATION_B:
            // A侧固定80%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.20f);
            // B侧固定100%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, 0U);
            break;
        case CALIBRATION_A:
            // B侧固定80%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, HRTIM_PERIOD * 0.20f);
            // A侧固定100%占空比
            __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, 0U);
            break;
        default:
            break;
        }
    }

}