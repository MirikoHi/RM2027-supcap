#pragma once

#include "main.h"
#include "hrtim.h"
#include "Config.hpp"
#include "SystemState.hpp"
#include "SampleManager.hpp"

namespace HRTIM
{
    /** @brief 启动 HRTIM 定时器并开始中断/采样 */
    void startTimer();

    /** @brief 停止 HRTIM 定时器 */
    void stopTimer();

    /** @brief 启用 A/B 输出通道
     *  @return true 若成功启用
     */
    bool enableOutputAB();

    /** @brief 禁用 A/B 输出通道 */
    void disableOutputAB();

    /** @brief HRTIM 的状态机处理函数（切换模式/输出等） */
    void modeStateMachine();
}