#pragma once

#include "main.h"
#include "fdcan.h"
#include "Config.hpp"

namespace Communication
{
    /**
     * @brief 接收数据格式（从 CAN 或裁判系统）
     */
    struct __packed RxData
    {
        /** @brief 允许启动 DCDC */
        uint8_t enableDCDC : 1;
        /** @brief 系统请求重启 */
        uint8_t systemRestart : 1;
        /** @brief 保留位 */
        uint8_t resv0 : 3;
        /** @brief 手动清除可清除的错误 */
        uint8_t clearError : 1;
        /** @brief 是否启用主动充电限制 */
        uint8_t enableActiveChargingLimit : 1;
        /** @brief 是否使用新的反馈消息格式 */
        uint8_t useNewFeedbackMessage : 1;

        /** @brief 裁判限制功率，单位 W */
        uint16_t refereePowerLimit;
        /** @brief 裁判能量缓冲，单位 J */
        uint16_t refereeEnergyBuffer;
        /** @brief 主动充电限制比例（能量），0-255 */
        uint8_t activeChargingLimitRatio;
        /** @brief 保留字段 */
        uint16_t resv2;
    };

    /** @brief 当前接收数据实例 */
    extern RxData rxData;

    /**
     * @brief 旧版发送数据格式（useNewFeedbackMessage = 0）
     */
    struct __packed TxData
    {                               // 0x051 (useNewFeedbackMessage = 0)
        /** @brief 状态码 */
        uint8_t statusCode;
        /** @brief 底盘功率，单位 W */
        float chassisPower;
        /** @brief 底盘最大可用功率（包括裁判系统） */
        uint16_t chassisPowerLimit;
        /** @brief 电容能量，0-255 量化 */
        uint8_t capEnergy;
    };

    /**
     * @brief 新版发送数据格式（useNewFeedbackMessage = 1）
     * @details 功率字段采用特定缩放和偏移编码（功率*64+16384）
     */
    struct __packed TxDataNew
    {                               // 0x052 (useNewFeedbackMessage = 1)
        uint8_t statusCode;         // 状态信息
        uint16_t chassisPower;      // 底盘功率，功率*64+16384 (-256W~+768W, 精度0.015625)
        uint16_t refereePower;      // 裁判系统功率，功率*64+16384 (-256W~+768W, 精度0.015625)
        uint16_t chassisPowerLimit; // 底盘最大可用功率（包括裁判系统）
        uint8_t capEnergy;          // 电容现有能量，0-255
    };

    /** @brief 当前发送数据（旧格式）实例 */
    extern TxData txData;
    /** @brief 当前发送数据（新格式）实例 */
    extern TxDataNew txDataNew;

    /** @brief 初始化通信模块（CAN, FDCAN 或其他） */
    void init();

    /** @brief 根据内部状态生成旧格式发送数据 */
    void generateTxData(TxData &td);

    /** @brief 根据内部状态生成新格式发送数据 */
    void generateTxDataNew(TxDataNew &td);
    
    /** @brief 发送当前已生成的发送数据 */
    void sendData();

    /**
     * @brief 处理接收到的数据
     * @param rd 接收到的 `RxData` 数据
     */
    void receiveData(const RxData &rd);

} // namespace Communication