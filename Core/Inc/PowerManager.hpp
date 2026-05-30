#pragma once

#include "Communication.hpp"
#include "PID.hpp"
#include "MathUtil.hpp"
#include "adc.h"
#include "hrtim.h"
#include "main.h"
#include "tim.h"
#include "opamp.h"

#define ERROR_UNDER_VOLTAGE 0b00000001
#define ERROR_OVER_VOLTAGE 0b00000010
#define ERROR_BUCK_BOOST 0b00000100
#define ERROR_SHORT_CIRCUIT 0b00001000
#define ERROR_HIGH_TEMPERATURE 0b00010000
#define ERROR_NO_POWER_INPUT 0b00100000
#define ERROR_CAPACITOR 0b01000000

enum DCDCMode
{
    BUCK,
    BUCKBOOST,
    BOOSTBUCK,
    BOOST,
    CALIBRATION_A,
    CALIBRATION_B
};

namespace SampleManager
{
    
}