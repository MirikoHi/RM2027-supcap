#pragma once

#include "main.h"
#include "Config.hpp"

/**
 * @brief 增量型 PID 控制器类
 *
 * 用于以增量形式计算控制输出，内部保存上一次的目标和测量值。
 */
class IncrementalPID
{
private:
    /** @brief 目标通道的比例系数 */
    float kTP = 0.0f;
    /** @brief 测量通道的比例系数 */
    float kMP = 0.0f;
    /** @brief 积分系数 */
    float kI = 0.0f;
    /** @brief 微分系数 */
    float kD = 0.0f;

    /** @brief 上次的目标值 */
    float lastTarget = 0.0f;
    /** @brief 上次的测量值 */
    float lastMeasurement = 0.0f;
    /** @brief 上上次的误差 */
    float lastLastErr = 0.0f;

    /** @brief 输出的增量值（用于累加到总输出） */
    float deltaOutput = 0.0f;

    /** @brief 输出限幅上界 */
    float clamp_upper = 0.0f;
    /** @brief 输出限幅下界 */
    float clamp_lower = 0.0f;
    /** @brief 是否启用输出限幅 */
    bool clamp_enabled = false;

public:
    IncrementalPID() = default;
    /**
     * @brief 构造函数，设置 PID 参数
     */
    IncrementalPID(float _kTargetP, float _kMeasureP, float _kI, float _kD) : 
        kTP(_kTargetP), kMP(_kMeasureP), kI(_kI), kD(_kD) {}

    /** @brief 设置 PID 参数 */
    void setParameter(float _kTargetP, float _kMeasureP, float _kI, float _kD);

    /**
     *  @brief 在有输出的情况下更新控制器 
    */
    void update(float _target, float _measurement);
    /** @brief 仅更新误差状态但不改变输出 */
    void updateNoOutput(float _target, float _measurement);
    /** @brief 重置误差历史 */
    void resetError();

    /** @brief 获取当前输出（累积后） */
    float getOutput() const;
    /** @brief 获取当前积分增量（或内部增量） */
    float getDeltaI() const;

    /** @brief 设置输出限幅区间 */
    void setClamp(float _lower, float _upper);
    /** @brief 禁用输出限幅 */
    void disableClamp();
};