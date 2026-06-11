#include "PowerManager.hpp"
#include "SystemState.hpp"
#include "SampleManager.hpp"
#include "Protection.hpp"
#include "Modulation.hpp"

namespace PowerManager
{
    ControlData ctrlData;
    LoopControlData mfLoop;

    void powerOnOffControl()
    {
        if (psData.outputABEnabled)
        {
            if (SampleManager::adcData.vA < REFEREE_UVLO_LIMIT)
            {
                HRTIM::disableOutputAB();
                psData.softStartCnt = SOFT_START_TIME;
            }
            if (psData.softStartCnt > 0)
            {
                psData.softStartCnt--;
            }
        }
        else
        {
            if (SampleManager::adcData.vA > REFEREE_UVLO_RECOVERY && (Protection::errorData.errorLevel == Protection::NO_ERROR || Protection::errorData.errorLevel == Protection::WARNING) && psData.outputAllow) // TODO
            {
                // psData.iLLimit = 1.0f;
                HRTIM::enableOutputAB();
            }
            psData.softStartCnt = SOFT_START_TIME;
        }
    }

    __attribute__((section(".RamFunc"))) inline void updatePWM()
    {
        __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, psData.ACMP3);
        __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, psData.BCMP3);
    }

}