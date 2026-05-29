#include "PID.hpp"
#include "MathUtil.hpp"

void IncrementalPID::setParameter(float _kTargetP, float _kMeasureP, float _kI, float _kD)
{
    kTP = _kTargetP;
    kMP = _kMeasureP;
    kI = _kI;
    kD = _kD;
}

void IncrementalPID::setClamp(float _lower, float _upper)
{
    this->clamp_lower = _lower;
    this->clamp_upper = _upper;
    this->clamp_enabled = true;
}

void IncrementalPID::disableClamp()
{
    clamp_enabled = false;
}

void IncrementalPID::resetError()
{
    this->deltaOutput = 0.0f;
    this->lastTarget = 0.0f;
    this->lastMeasurement = 0.0f;
    this->lastLastErr = 0.0f;
}

float IncrementalPID::getOutput() const
{
    return this->deltaOutput;
}

float IncrementalPID::getDeltaI() const
{
    return this->kI * (this->lastTarget - this->lastMeasurement);
}

void IncrementalPID::update(float _target, float _measurement)
{
    this->deltaOutput = kTP * (_target - this->lastTarget) 
        + kMP * (_measurement - this->lastMeasurement) 
        + kI * (_target - _measurement) 
        + kD * ((_target - _measurement) - 2 * (this->lastTarget - this->lastMeasurement) + this->lastLastErr);

    this->lastLastErr = this->lastTarget - this->lastMeasurement;
    this->lastTarget = _target;
    this->lastMeasurement = _measurement;

    if (clamp_enabled)
        this->deltaOutput = M_CLAMP(this->deltaOutput, this->clamp_lower, this->clamp_upper);
}

void IncrementalPID::updateNoOutput(float _target, float _measurement)
{
    this->lastLastErr = this->lastTarget - this->lastMeasurement;
    this->lastMeasurement     = _measurement;
    this->lastTarget          = _target;
}