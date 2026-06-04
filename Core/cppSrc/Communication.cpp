#include "Communication.hpp"

#include "main.h"
#include "fdcan.h"

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

    void receiveData()
    {
    }

    void sendData()
    {
    }

} // namespace Communication