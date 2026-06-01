#pragma once

#include "main.h"
#include "Config.hpp"

struct SystemData
{
    uint32_t vTick = 0;

    bool systemInited = false;

    uint32_t hardwareUID[3];

    uint8_t lfLoopIndex = 0; // 低频环路
};

extern SystemData sysData;

enum DCDCMode
{
    BUCK,
    BUCKBOOST,
    BOOSTBUCK,
    BOOST,
    CALIBRATION_A,
    CALIBRATION_B
};

struct PowerState
{
    bool timerEnabled = 0;
    bool outputABEnabled = 0;
    bool outputAllow = 1;

    float dutyByVoltage = 0.0f;
    float efficiency = 1.0f;

    uint8_t softStartCnt = SOFT_START_TIME;
    float iLLimit = MAX_INDUCTOR_CURRENT;

    DCDCMode mode = BUCK;
    float iLTarget = 0.0f;
    float IRQLoad = 0.0f;
};

extern PowerState psData;