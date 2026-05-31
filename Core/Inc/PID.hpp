#pragma once

#include "main.h"
#include "Config.hpp"

class IncrementalPID
{
private:
    float kTP = 0.0f;
    float kMP = 0.0f;
    float kI = 0.0f;
    float kD = 0.0f;

    float lastTarget = 0.0f;          // 上次的目标值
    float lastMeasurement = 0.0f;     // 上次的测量值
    float lastLastErr = 0.0f; // 上上次的误差

    float deltaOutput = 0.0f; // 输出增量

    float clamp_upper = 0.0f;
    float clamp_lower = 0.0f;
    bool clamp_enabled = false;

public:
    IncrementalPID() = default;
    IncrementalPID(float _kTargetP, float _kMeasureP, float _kI, float _kD) : 
        kTP(_kTargetP), kMP(_kMeasureP), kI(_kI), kD(_kD) {}
    void setParameter(float _kTargetP, float _kMeasureP, float _kI, float _kD);

    void update(float _target, float _measurement);
    void updateNoOutput(float _target, float _measurement);
    void resetError();

    float getOutput() const;
    float getDeltaI() const;

    void setClamp(float _lower, float _upper);
    void disableClamp();
};