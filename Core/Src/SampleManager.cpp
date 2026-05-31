#include "main.h"
#include "SampleManager.hpp"
#include <cstring>

static SampleManager::ADCData adcData;
static SampleManager::ADCFitParaTypeDef adcFitPara;

namespace SampleManager
{
    void initAnalog()
    {
        HAL_OPAMP_SelfCalibrate(&hopamp1); // 采样电压跟随
        HAL_OPAMP_SelfCalibrate(&hopamp2); // 采样电压跟随
        HAL_OPAMP_SelfCalibrate(&hopamp3); // 采样电压跟随

        HAL_Delay(50);

        HAL_OPAMP_Start(&hopamp1);
        HAL_OPAMP_Start(&hopamp2);
        HAL_OPAMP_Start(&hopamp3);
    }

    void initADC()
    {
        // 校准ADC，ADC1和ADC2用于同步采样，ADC3独立模式
        HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
        HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
        HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
        HAL_Delay(50);

        // 双ADC同步采样
        HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t *)adcData.rawData12, ADC12_BUFFER_SIZE);
        HAL_ADC_Start(&hadc2);
        HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adcData.rawData3, ADC3_BUFFER_SIZE);
    }

    __attribute__((section(".RamFunc"))) void updateADCmf()
    {
        for (uint8_t i = 0; i < HRTIM_INT_SCALER; i++)
        {
            for (uint8_t j = 0; j < ADC12_CHANNAL_COUNT; j++)
            {
                adcData.sumData12[j] += adcData.rawData12[i * ADC12_CHANNAL_COUNT + j];
            }

            adcData.sumData3[0] += adcData.rawData3[i];
        }

        // ADC1  vW  iW  iA  vA
        // ADC2  vB  vB  iR  iR
        // ADC3  iB

        adcData.iA = (1 - ADC_FILTER_ALPHA) * adcData.iA +
                     ADC_FILTER_ALPHA * ((uint16_t)adcData.sumData12[2] * adcFitPara.IA_K + adcFitPara.IA_B);
        adcData.vA = (1 - ADC_FILTER_ALPHA) * adcData.vA +
                     ADC_FILTER_ALPHA * ((uint16_t)adcData.sumData12[3] * adcFitPara.VA_K + adcFitPara.VA_B);
        adcData.vB = (1 - ADC_FILTER_ALPHA) * adcData.vB +
                     ADC_FILTER_ALPHA * ((uint16_t)(adcData.sumData12[0] >> 16) * adcFitPara.VB_K + adcFitPara.VB_B);
        adcData.iReferee = (1 - ADC_FILTER_ALPHA) * adcData.iReferee +
                           ADC_FILTER_ALPHA * ((uint16_t)(adcData.sumData12[2] >> 16) * adcFitPara.IR_K + adcFitPara.IR_B);
        adcData.iB = (1 - ADC_FILTER_ALPHA) * adcData.iB +
                     ADC_FILTER_ALPHA * ((uint16_t)adcData.sumData3[0] * adcFitPara.IB_K + adcFitPara.IB_B);

#ifdef WPT_HARDWARE

        adcData.vWPT = (1 - ADC_FILTER_ALPHA) * adcData.vWPT +
                       ADC_FILTER_ALPHA * ((uint16_t)adcData.sumData12[0] * adcFitPara.VW_K + adcFitPara.VW_B);

        adcData.iWPT = (1 - ADC_FILTER_ALPHA) * adcData.iWPT +
                       ADC_FILTER_ALPHA * ((uint16_t)adcData.sumData12[1] * adcFitPara.IW_K + adcFitPara.IW_B);

        adcData.pWPT = adcData.vB * adcData.iWPT;

        adcData.iCap = adcData.iB + adcData.iWPT;

#else
        adcData.iCap = adcData.iB;
        adcData.iWPT = 0.0f;
        adcData.vWPT = 0.0f;
#endif

        adcData.vCap = adcData.vB - adcData.iCap * CAPARR_DCR;
        adcData.iChassis = adcData.iReferee - adcData.iA;
        adcData.pReferee = adcData.vA * adcData.iReferee;
        adcData.pChassis = adcData.vA * adcData.iChassis;

        memset(adcData.sumData12, 0, sizeof(adcData.sumData12));
        adcData.sumData3[0] = 0;
    }
}