#include "main.h"
#include "iwdg.h"
#include "Communication.hpp"

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

        static volatile uint32_t hardwareID[3] = {HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()};

        Communication::init();

        HAL_Delay(600);

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
}