#include "Communication.hpp"
#include "SystemState.hpp"
#include "PowerManager.hpp"
#include "Protection.hpp"
#include "CapArray.hpp"
#include "Modulation.hpp"
#include "SampleManager.hpp"

#include "main.h"
#include "fdcan.h"

#include <cmath>

namespace Communication
{
    RxData rxData;
    TxData txData;
    TxDataNew txDataNew;

    static FDCAN_TxHeaderTypeDef txHeader = {
        .Identifier = TX_IDENTIFIER,
        .IdType = FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC_BYTES_8,
        .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .FDFormat = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0};

    static FDCAN_TxHeaderTypeDef txHeaderNew = {
        .Identifier = TX_IDENTIFIER_NEW,
        .IdType = FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC_BYTES_8,
        .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .FDFormat = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0};

    static FDCAN_RxHeaderTypeDef rxHeader;

    void init()
    {
        static_assert(sizeof(RxData) == 8, "RxData size error");
        static_assert(sizeof(TxData) == 8, "TxData size error");

        FDCAN_FilterTypeDef filter;
        filter.IdType = FDCAN_STANDARD_ID;
        filter.FilterIndex = 0;
        filter.FilterType = FDCAN_FILTER_DUAL;
        filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
        filter.FilterID1 = RX_FILTER << 5;
        filter.FilterID2 = RX_FILTER << 5;

        rxData.enableDCDC = 1;
        rxData.systemRestart = 0;
        rxData.clearError = 0;
        rxData.enableActiveChargingLimit = 0;

        HAL_FDCAN_ConfigFilter(&hfdcan2, &filter);
        HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
        HAL_FDCAN_Start(&hfdcan2);
    }

    void generateTxData(TxData &td)
    {
        td = {};
        // PowerManager::status.errorCode | ((uint8_t) !PowerManager::status.outputEnabled) << 7;
        td.statusCode = 0x00;
        td.statusCode = (psData.outputABEnabled << 7) | (PowerManager::ctrlData.refLoop.useNewFeedbackMessage << 6) |
                        (PowerManager::ctrlData.wptStatus << 4) |
                        (((PowerManager::ctrlData.limitFactor >= 4) ? 0b11 : (PowerManager::ctrlData.limitFactor & 0x03)) << 2) |
                        (Protection::errorData.errorLevel & 0x03);
        td.capEnergy = (SampleManager::adcData.vCap * SampleManager::adcData.vCap * (1 / (CAPARR_MAX_VOLTAGE * CAPARR_MAX_VOLTAGE))) * 250U;

#ifdef WPT_HARDWARE
        if (psData.outputEEnabled)
            td.chassisPower = SampleManager::adcData.pChassis - SampleManager::adcData.pWPT;
        else
            td.chassisPower = SampleManager::adcData.pChassis;
#else
        td.chassisPower = SampleManager::adcData.pChassis;
#endif

        td.chassisPowerLimit = CapArray::getMaxPowerFeedback() + rxData.refereePowerLimit; // TODO
    }

    void generateTxDataNew(TxDataNew &td)
    {
        td = {};
        td.statusCode = 0x00;
        td.statusCode = (psData.outputABEnabled << 7) | (PowerManager::ctrlData.refLoop.useNewFeedbackMessage << 6) |
                        (PowerManager::ctrlData.wptStatus << 4) |
                        (((PowerManager::ctrlData.limitFactor >= 4) ? 0b11 : (PowerManager::ctrlData.limitFactor & 0x03)) << 2) |
                        (Protection::errorData.errorLevel & 0x03);
        td.capEnergy = (SampleManager::adcData.vCap * SampleManager::adcData.vCap * (1 / (CAPARR_MAX_VOLTAGE * CAPARR_MAX_VOLTAGE))) * 250U;

#ifdef WPT_HARDWARE
        if (psData.outputEEnabled)
            td.chassisPower = (SampleManager::adcData.pChassis - SampleManager::adcData.pWPT) * 64U + 16384U;
        else
            td.chassisPower = SampleManager::adcData.pChassis * 64U + 16384U;
#else
        td.chassisPower = SampleManager::adcData.pChassis * 64U + 16384U;
#endif

        td.refereePower = SampleManager::adcData.pReferee * 64U + 16384U;
        td.chassisPowerLimit = CapArray::getMaxPowerFeedback() + rxData.refereePowerLimit;
    }

    void receiveData(const RxData &rd)
    {

        psData.outputAllow = rd.enableDCDC;
        PowerManager::ctrlData.refLoop.useNewFeedbackMessage = rd.useNewFeedbackMessage;

        if (!rd.enableDCDC && psData.outputABEnabled)
        {
            HRTIM::disableOutputAB();
        }
        if (rd.systemRestart)
        {
            HRTIM::disableOutputAB();
            __disable_irq();
            while (true)
                NVIC_SystemReset();
        }
        if (rd.clearError)
        {
            Protection::autoClearError();
            Protection::manualClearError();
        }
        if (rd.enableActiveChargingLimit)
        {
            PowerManager::ctrlData.vCapArrNormal =
                M_CLAMP(sqrtf((rd.activeChargingLimitRatio / 255.0f)) * CAPARR_MAX_VOLTAGE, CAPARR_LOW_VOLTAGE, CAPARR_MAX_VOLTAGE);
        }
        else
        {
            PowerManager::ctrlData.vCapArrNormal = CAPARR_MAX_VOLTAGE;
        }
    }

    void sendData()
    {
        if (hfdcan2.Instance->PSR & FDCAN_PSR_BO_Msk)
        {
            hfdcan2.Instance->CCCR &= ~FDCAN_CCCR_INIT;
        }

        if (!PowerManager::ctrlData.refLoop.useNewFeedbackMessage)
        {
            generateTxData(txData);
            HAL_FDCAN_AddMessageToTxFifoQ(
                &hfdcan2,
                &txHeader,
                reinterpret_cast<uint8_t *>(&txData));
        }
        else
        {
            generateTxDataNew(txDataNew);
            HAL_FDCAN_AddMessageToTxFifoQ(
                &hfdcan2,
                &txHeaderNew,
                reinterpret_cast<uint8_t *>(&txDataNew));
        }
    }

} // namespace Communication

extern "C"
{
    void FDCAN2_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&hfdcan2);

        if ((hfdcan2.Instance->RXF0S & FDCAN_RXF0S_F0FL) == 0U)
            return;

        while (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &Communication::rxHeader, (uint8_t *)&Communication::rxData) == HAL_OK)
        {
            if ((Communication::rxHeader.Identifier == 0x061) && (Communication::rxHeader.DataLength == 0x8) && (Communication::rxHeader.IdType == FDCAN_STANDARD_ID)) //
            {
                PowerManager::ctrlData.refLoop.isConnected = 1;
                Communication::receiveData(Communication::rxData);
                PowerManager::updateRefereePower(Communication::rxData, sysData.vTick);
            }
        }
    }
}