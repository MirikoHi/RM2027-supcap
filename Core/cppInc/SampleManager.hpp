#pragma once

#include "main.h"
#include "Config.hpp"

#define ADC12_CHANNAL_COUNT 4 // Number of Conversion
#define ADC3_CHANNAL_COUNT 1
#define HRTIM_INT_SCALER 8
#define ADC12_BUFFER_SIZE (ADC12_CHANNAL_COUNT * HRTIM_INT_SCALER)
#define ADC3_BUFFER_SIZE (ADC3_CHANNAL_COUNT * HRTIM_INT_SCALER)

namespace SampleManager
{
    struct ADCData
    {
        // bool initialized = 0;

        uint32_t rawData12[ADC12_BUFFER_SIZE];
        uint32_t sumData12[ADC12_CHANNAL_COUNT];

        uint32_t rawData3[ADC3_BUFFER_SIZE];
        uint32_t sumData3[ADC3_CHANNAL_COUNT];

        /** @brief A 相电流 (A) */
        float iA = 0.0f;
        /** @brief B 相电流 (A) */
        float iB = 0.0f;
        /** @brief 裁判端电流 (A) */
        float iReferee = 0.0f;
        /** @brief 电容端电流 (A) */
        float iCap = 0.0f;
        /** @brief A 相电压 (V) */
        float vA = 0.0f;
        /** @brief B 相电压 (V) */
        float vB = 0.0f;
        /** @brief 电容端电压 (V) */
        float vCap = 0.0f;
        /** @brief 裁判系统功率 (W) */
        float pReferee = 0.0f;
        /** @brief 底盘功率 (W) */
        float pChassis = 0.0f;

        /** @brief 无线充电电压 (V) */
        float vWPT = 0.0f;
        /** @brief 无线充电电流 (A) */
        float iWPT = 0.0f;
        /** @brief 无线充电功率 (W) */
        float pWPT = 0.0f;

        /** @brief 底盘侧电流 (A) */
        float iChassis = 0.0f;
    };

    /** @brief ADC 采样与计算后的数据 */
    extern ADCData adcData;

    struct ADCFitParaTypeDef
    {
        uint32_t UID0;
        uint32_t UID1;
        uint32_t UID2;

        float VA_K;
        float VA_B;

        float VB_K;
        float VB_B;

        float VW_K;
        float VW_B;

        float IA_K;
        float IA_B;

        float IB_K;
        float IB_B;

        float IR_K;
        float IR_B;

        float IW_K;
        float IW_B;
    };
    /**
     * @brief ADC 校准/拟合参数结构体
     * @details 包含设备唯一 ID 以及每一路的线性拟合系数 K/B
     */
    struct ADCFitParaTypeDef
    {
        uint32_t UID0;
        uint32_t UID1;
        uint32_t UID2;

        float VA_K;
        float VA_B;

        float VB_K;
        float VB_B;

        float VW_K;
        float VW_B;

        float IA_K;
        float IA_B;

        float IB_K;
        float IB_B;

        float IR_K;
        float IR_B;

        float IW_K;
        float IW_B;
    };

    /** @brief ADC 校准/拟合参数实例 */
    extern ADCFitParaTypeDef adcFitPara;

    /** @brief 初始化模拟部分（参考电平、放大等） */
    void initAnalog();

    /** @brief 初始化 ADC 硬件与 DMA */
    void initADC();

    /** @brief 更新中频（中等速率）ADC 采样处理 */
    void updateADCmf(); // 中频ADC环路
}
