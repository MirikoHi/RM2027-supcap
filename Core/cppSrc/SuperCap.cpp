#include "main.h"
#include "iwdg.h"
#include "tim.h"
#include "hrtim.h"
#include "Communication.hpp"
#include "SystemState.hpp"
#include "SampleManager.hpp"

SystemData sysData;
PowerState psData;

namespace SuperCap
{
    static void loop()
    {
        while (true)
        {
            __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
            HAL_Delay(1);
        }
    }

    void configIWDG()
    {
        hiwdg.Instance = IWDG;
        __HAL_IWDG_START(&hiwdg);
        IWDG_ENABLE_WRITE_ACCESS(&hiwdg);
        hiwdg.Instance->PR = IWDG_PRESCALER_4;
        hiwdg.Instance->RLR = 1000;
        while ((hiwdg.Instance->SR & (IWDG_SR_WVU | IWDG_SR_RVU | IWDG_SR_PVU)) != 0x00u)
        {
        }
        __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
    }

    void init()
    {
        HAL_Delay(200); // 等待电压稳定

        sysData.hardwareUID[0] = HAL_GetUIDw0();
        sysData.hardwareUID[1] = HAL_GetUIDw1();
        sysData.hardwareUID[2] = HAL_GetUIDw2();

        Communication::init();

        SampleManager::initAnalog();
        SampleManager::initADC();

        HAL_Delay(600);

        HAL_TIM_Base_Start_IT(&htim2);
        HAL_TIM_Base_Start(&htim16);

        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1 + HRTIM_OUTPUT_TA2);
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1 + HRTIM_OUTPUT_TB2);

        configIWDG();
    }
} // namespace SuperCap

extern "C"
{
    void systemStart()
    {
        SuperCap::init();
        SuperCap::loop();
    }

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim == &htim2) // 4kHz
        {
            // 以下任务1kHz运行
            switch (sysData.lfLoopIndex)
            {
            // 错误处理
            case 0:
                sysData.vTick++;
                sysData.lfLoopIndex++;
                break;

            // can通信处理，can超时处理
            case 1:
                if (sysData.systemInited)
                {
                    Communication::sendData();
                }
                sysData.lfLoopIndex++;
                break;

            // 电容漏电检测
            case 2:
                if (psData.outputABEnabled)
                {
                }
            }
        }
    }
}