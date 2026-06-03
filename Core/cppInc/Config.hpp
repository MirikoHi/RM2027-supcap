#pragma once

#define M_MIN(a, b) ((a) < (b) ? (a) : (b))
#define M_MAX(a, b) ((a) > (b) ? (a) : (b))

#define M_ABS(x) ((x) < 0 ? -(x) : (x))

#define M_CLAMP(x, min, max) (M_MIN((max), M_MAX((min), (x))))

#define ADC_FILTER_ALPHA 0.7f // ADC滤波系数

#define CAPARR_DCR 0.1f

#define HRTIM_PERIOD 20000

#define ADC_FIT_LIST_NUM 50

#define SOFT_START_TIME 8

#define MAX_INDUCTOR_CURRENT 25.0f

#define CAPARR_MAX_VOLTAGE 28.8f

// 裁判系统关断
#define REFEREE_UVLO_LIMIT 18.0f // 狗腿特殊阈值15V，正常阈值18V
#define REFEREE_UVLO_RECOVERY 20.0f

// 裁判系统欠压
#define BATTERY_LOW_LIMIT 20.92f
#define BATTERY_LOW_RECOVERY 21.6f

// 短路保护阈值
#define SCP_VOLTAGE 5.0f
#define SCP_CURRENT 5.0f
#define SCP_RECOVER_TIME 1000

// 过压保护阈值
#define OVP_A 29.0f
#define OVP_B 30.5f

// 过流保护阈值
#define OCP_CAPARR 25.5f
#define OCP_CHASSIS 20.0f
#define OCP_REFEREE 6.5f