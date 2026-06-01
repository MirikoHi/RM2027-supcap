#pragma once

#include "main.h"
#include "hrtim.h"
#include "Config.hpp"
#include "SystemState.hpp"
#include "SampleManager.hpp"

namespace HRTIM
{
    void startTimer();

    void stopTimer();

    bool enableOutputAB();

    void disableOutputAB();

    void modeStateMachine();
}