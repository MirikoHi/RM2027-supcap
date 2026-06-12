#include "PowerManager.hpp"
#include "SystemState.hpp"
#include "SampleManager.hpp"
#include "Protection.hpp"
#include "Modulation.hpp"
#include "CapArray.hpp"

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
            if (SampleManager::adcData.vA > REFEREE_UVLO_RECOVERY && (Protection::errorData.errorLevel == Protection::NO_ERROR || Protection::errorData.errorLevel == Protection::WARNING) && psData.outputAllow)
            {
                // psData.iLLimit = 1.0f;
                HRTIM::enableOutputAB();
            }
            psData.softStartCnt = SOFT_START_TIME;
        }
    }

    __RAM_FUNC inline void updatePWM()
    {
        __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, psData.ACMP3);
        __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, psData.BCMP3);
    }

    __RAM_FUNC void updateMFLoop()
    {
        // 计算B侧电流限制
        CapArray::updateMaxCurrent();

        if (SampleManager::adcData.vCap > ctrlData.vCapArrNormal + 0.1f)
        {
            ctrlData.allowCharge = false;
        }
        else if (SampleManager::adcData.vCap < ctrlData.vCapArrNormal - 0.1f)
        {
            ctrlData.allowCharge = true;
        }

        if ((ctrlData.allowCharge || !Communication::rxData.enableActiveChargingLimit) && !psData.softStartCnt) //
        {
            mfLoop.iRPID.update((ctrlData.pRefereeTarget / SampleManager::adcData.vA), SampleManager::adcData.iReferee);
            ctrlData.limitFactor = REFEREE_POWER;
        }
        else
        {
            mfLoop.iRPID.update(6.0f / SampleManager::adcData.vA, SampleManager::adcData.iReferee);
            ctrlData.limitFactor = CAPARR_VOLTAGE_NORMAL;
        }

        // 默认设为裁判系统功率PID的输出
        mfLoop.deltaIL = mfLoop.iRPID.getOutput();

        mfLoop.dIL_VCap_Max = mfLoop.voltageLimitKI * (CAPARR_MAX_VOLTAGE - SampleManager::adcData.vCap);
        mfLoop.dIL_IB_Positive = mfLoop.currentLimitKI * (CapArray::capStatus.maxInCurrent - SampleManager::adcData.iCap);
        mfLoop.dIL_IB_Negative = mfLoop.currentLimitKI * (-SampleManager::adcData.iCap - CapArray::capStatus.maxOutCurrent);

        if ((SampleManager::adcData.vCap > CAPARR_MAX_VOLTAGE * 0.9f) && (mfLoop.dIL_VCap_Max < mfLoop.deltaIL))
        {
            mfLoop.deltaIL = mfLoop.dIL_VCap_Max;
            ctrlData.limitFactor = CAPARR_VOLTAGE_MAX;
        }
        else if (SampleManager::adcData.iCap > CapArray::capStatus.maxInCurrent && mfLoop.dIL_IB_Positive < mfLoop.deltaIL)
        {
            mfLoop.deltaIL = mfLoop.dIL_IB_Positive;
            ctrlData.limitFactor = IB_POSITIVE;
        }
        else if (SampleManager::adcData.iCap < -CapArray::capStatus.maxOutCurrent && mfLoop.dIL_IB_Negative > mfLoop.deltaIL)
        {
            mfLoop.deltaIL = mfLoop.dIL_IB_Negative;
            ctrlData.limitFactor = IB_NEGATIVE;
        }

        psData.iLTarget += mfLoop.deltaIL;
        psData.iLTarget = M_CLAMP(psData.iLTarget, -psData.iLLimit, psData.iLLimit);
    }

    void updateRefereePower(const Communication::RxData &rd, const uint32_t &currentTick)
    {
        if (ctrlData.limitFactor == REFEREE_POWER && psData.outputABEnabled)
        {
            ctrlData.refLoop.error = (rd.refereeEnergyBuffer - REFEREE_ENERGY_BUFFER);
            ctrlData.refLoop.pRefereeBias = ctrlData.refLoop.kP * ctrlData.refLoop.error +
                                            ctrlData.refLoop.kI * ctrlData.refLoop.integral +
                                            ctrlData.refLoop.kD * (ctrlData.refLoop.error - ctrlData.refLoop.lastError);
            ctrlData.refLoop.lastError = ctrlData.refLoop.error;
            ctrlData.refLoop.integral += ctrlData.refLoop.error;

            ctrlData.refLoop.pRefereeBias = M_CLAMP(ctrlData.refLoop.pRefereeBias, -REFEREE_POWER_BIAS_LIMIT, REFEREE_POWER_BIAS_LIMIT);
        }
        else
        {
            ctrlData.refLoop.lastError = 0.0f;
            ctrlData.refLoop.integral = 0.0f;
        }

        ctrlData.pRefereeTarget = M_CLAMP(ctrlData.refLoop.pRefereeBias + rd.refereePowerLimit, 5.0f, 135.0f);
        ctrlData.refLoop.lastTimestamp = currentTick;
    }
}