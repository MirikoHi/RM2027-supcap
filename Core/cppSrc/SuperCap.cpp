#include "main.h"
#include "iwdg.h"
#include "tim.h"
#include "hrtim.h"
#include "Communication.hpp"
#include "SystemState.hpp"
#include "SampleManager.hpp"
#include "Protection.hpp"
#include "CapArray.hpp"
#include "PowerManager.hpp"
#include "Modulation.hpp"

SystemData sysData;
PowerState psData;

namespace SuperCap
{
    static void loop()
    {
        while (true)
        {
            HAL_IWDG_Refresh(&hiwdg);
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

        Communication::init();

        Protection::checkHardwareUID();

        SampleManager::initAnalog();
        SampleManager::initADC();

        HAL_Delay(600);

        HAL_TIM_Base_Start_IT(&htim2);
        HAL_TIM_Base_Start(&htim16);

        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1 + HRTIM_OUTPUT_TA2);
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1 + HRTIM_OUTPUT_TB2);

        psData.outputABEnabled = 0;
        HRTIM::startTimer();

        configIWDG();

        HAL_Delay(400);
        sysData.systemInited = true;
    }
} // namespace SuperCap

extern "C"
{
    void systemStart()
    {
        SuperCap::init();
        SuperCap::loop();
    }

    void TIM2_IRQHandler(void) // 4kHz
    {
        __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

        // 以下任务1kHz运行
        switch (sysData.lfLoopIndex)
        {
        // 错误处理
        case 0:
            sysData.vTick++;
            Protection::errorHandlerLF();
            sysData.lfLoopIndex++;
            break;

        // can通信处理，can超时处理
        case 1:
            if (sysData.systemInited)
            {
                Communication::sendData();
                Protection::checkRxDataTimeout(sysData.vTick);
            }
            sysData.lfLoopIndex++;
            break;

        // 电容漏电检测
        case 2:
            if (psData.outputABEnabled)
            {
                CapArray::estimateCapacity(sysData.vTick);
            }
            sysData.lfLoopIndex++;
            break;

        // 低电压保护
        case 3:
            Protection::checkLowBatteryLF();
            sysData.lfLoopIndex = 0;
            break;

        default:
            break;
        }

        // 以下任务4kHz运行
        if (sysData.systemInited)
        {
            PowerManager::powerOnOffControl();

#ifdef WPT_HARDWARE

#endif
        }
    }
}