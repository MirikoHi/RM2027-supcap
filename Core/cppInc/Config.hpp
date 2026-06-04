#pragma once

// Math Funtion
#define M_MIN(a, b) ((a) < (b) ? (a) : (b))
#define M_MAX(a, b) ((a) > (b) ? (a) : (b))
#define M_ABS(x) ((x) < 0 ? -(x) : (x))
#define M_CLAMP(x, min, max) (M_MIN((max), M_MAX((min), (x))))

#define ADC_FILTER_ALPHA 0.7f // ADC滤波系数

// Hardware
#define CAPARR_DCR 0.1f

// Developer
#define HRTIM_PERIOD 20000
#define ADC_FIT_LIST_NUM 50
#define SOFT_START_TIME 8          // 软启动计数值，单位为(1/4kHz)
#define MAX_INDUCTOR_CURRENT 25.0f // 最大电感电流

// 电容组保护
#define CAPARR_CUTOFF_VOLTAGE 5.0f
#define CAPARR_LOW_VOLTAGE 10.0f
#define CAPARR_MAX_VOLTAGE 28.8f
#define CAPARR_MAX_CURRENT 15.0f

// CAN通信Filter
#define RX_FILTER 0x061
#define TX_IDENTIFIER 0x051
#define TX_IDENTIFIER_NEW 0x052

// 裁判系统关断
#define REFEREE_UVLO_LIMIT 18.0f // 轮腿特殊阈值15V，正常阈值18V
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