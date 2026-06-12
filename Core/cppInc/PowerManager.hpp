#pragma once

#include "Config.hpp"
#include "PID.hpp"
#include "Communication.hpp"
#include "hrtim.h"
#include "main.h"

namespace PowerManager
{
    enum LimitFactor
    {
        REFEREE_POWER,
        CAPARR_VOLTAGE_MAX,
        CAPARR_VOLTAGE_NORMAL,
        IB_POSITIVE,
        IB_NEGATIVE,
    };

    /** @brief 限制因子类型，用于标识当前受何种约束限制 */

    enum WPTStatus
    {
        WPT_ERROR = 0,    // 非无线充电硬件，或发生错误
        WPT_OFF = 1,      // 无线充电关闭
        WPT_CHARGING = 2, // 无线充电中
        WPT_FINISHED = 3  // 无线充电完成(电压>98%, 能量大于96%)
    };

    /** @brief 无线充电（WPT）状态 */

    struct ControlData
    {
        struct RefereeData
        {
            float kP = 1.0f, kI = 0.04f, kD = 1.5f; // 积分增益
            uint16_t lastError = 0.0f;              // 上次误差
            float integral = 0.0f;                  // 积分值
            int16_t error = 0U;
            uint32_t lastTimestamp = 0;
            float pRefereeBias = 0.0f;
            bool isConnected = 0;
            bool useNewFeedbackMessage = 0;
        };

        /** @brief 当前限制因子 */
        LimitFactor limitFactor;

        /** @brief 裁判环路相关数据 */
        RefereeData refLoop;

        /** @brief 裁判目标功率 */
        float pRefereeTarget = REFEREE_DEFUALT_POWER;

        /** @brief 电容组正常工作电压阈值 */
        float vCapArrNormal = CAPARR_MAX_VOLTAGE;

        /** @brief 是否允许充电 */
        bool allowCharge = false;

        /** @brief 无线充电状态 */
        WPTStatus wptStatus = WPT_OFF;
    };

    extern ControlData ctrlData;

    struct LoopControlData
    {
        IncrementalPID iRPID{0.1f, 0.2f, 0.10f, 0.01f}; // 裁判系统功率环
        // IncrementalPID vCapPID {0.0f, 0.0f, 0.02f, 0.0f};

        float currentLimitKI = 0.8f;  // 电容组电流环
        float voltageLimitKI = 0.01f; // 电容组电压环
        float burstKI = 2.0f;

        float vWPTTarget = 26.2f; // 无线充电目标电压
        float wptVoltageKI = 0.001f;

        float deltaDR;
        float dDL_VCap_Max;
        // float dIL_VCap_MaxBurst;
        float dDL_IB_Positive;
        float dDL_IB_Negative;

        float dIL_recoverBurst;
    };

    extern LoopControlData mfLoop;

    void powerOnOffControl();

    void updatePWM();

    void updateMFLoop();

    void updateRefereePower(const Communication::RxData &rd, const uint32_t &currentTick);
}