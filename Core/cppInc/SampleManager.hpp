#pragma once

#include "main.h"
#include "adc.h"
#include "opamp.h"
#include "Config.hpp"

namespace SampleManager
{
    struct ADCData
    {
        // bool initialized = 0;

        uint32_t rawData12[ADC12_BUFFER_SIZE];
        uint32_t sumData12[ADC12_CHANNAL_COUNT];

        uint32_t rawData3[ADC3_BUFFER_SIZE];
        uint32_t sumData3[ADC3_CHANNAL_COUNT];

        float iA = 0.0f;       // A侧电流
        float iB = 0.0f;       // B侧电流
        float iReferee = 0.0f; // 裁判端电流
        float iCap = 0.0f;     // 电容端电流
        float vA = 0.0f;       // A侧电压
        float vB = 0.0f;       // B侧电压
        // float vReferee = 0.0f; // 裁判端电压
        float vCap = 0.0f;     // 电容端电压
        float pReferee = 0.0f; // 裁判系统功率
        float pChassis = 0.0f; // 底盘功率

        float vWPT = 0.0f; // 无线充电压
        float iWPT = 0.0f; // 无线充电流
        float pWPT = 0.0f; // 无线充功率

        float iChassis = 0.0f;
    };

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

    extern ADCFitParaTypeDef adcFitPara;

    void initAnalog();

    void initADC();

    void updateADCmf(); // 中频ADC环路
}
