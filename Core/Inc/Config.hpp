#pragma once

#define M_MIN(a, b) ((a) < (b) ? (a) : (b))
#define M_MAX(a, b) ((a) > (b) ? (a) : (b))

#define M_ABS(x) ((x) < 0 ? -(x) : (x))

#define M_CLAMP(x, min, max) (M_MIN((max), M_MAX((min), (x))))

/*ADC*/

#define ADC12_CHANNAL_COUNT 4 // Number of Conversion
#define ADC3_CHANNAL_COUNT 1
#define HRTIM_INT_SCALER 8
#define ADC12_BUFFER_SIZE (ADC12_CHANNAL_COUNT * HRTIM_INT_SCALER)
#define ADC3_BUFFER_SIZE (ADC3_CHANNAL_COUNT * HRTIM_INT_SCALER)
#define ADC_FILTER_ALPHA 0.7f // 滤波系数

/*HARDWARE*/

#define CAPARR_DCR 0.1f

#define HRTIM_PERIOD 20000

#define SOFT_START_TIME 8

#define MAX_INDUCTOR_CURRENT 25.0f

#define CAPARR_MAX_VOLTAGE 28.8f