/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kfc_comm_server.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kfc/kfc_comm_server.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_SERVER_H__
#endif
#ifndef __INTF_KFC_COMM_SERVER_H__
#define __INTF_KFC_COMM_SERVER_H__

#include "kfc_comm.h"

namespace AscendC {
template <int8_t enableHardPoll = false>
class KfcCommServer {
public:
    // 接收消息队列维护
    MSG_POS KfcMsg* msgRcvHead;
    uint8_t msgRcvPos;
    uint8_t subBlockID; // 为AIC核上的subBlockID;

public:
    __aicore__ inline void Init(GM_ADDR workspace, int i)
    {
        this->msgRcvHead = (MSG_POS KfcMsg*)GetMsgHead(i);
        this->msgRcvPos = 0;
        this->subBlockID = i;
    }

    __aicore__ inline void FreeMessage(MSG_POS KfcMsg* msg) { *(reinterpret_cast<MSG_POS uint64_t*>(msg)) = 0; }

    __aicore__ inline MSG_POS KfcMsg* RcvMessage(uint32_t& msgHead)
    {
        auto msg = msgRcvHead + msgRcvPos;
        msgHead = msg->head;
        if constexpr (enableHardPoll != 1) {
            if (!(static_cast<bool>(KfcMsgGetState(msgHead)))) {
                return nullptr;
            }
        }

        msgRcvPos = (msgRcvPos + 1) & (MAX_MSG_COUNT_Arch3510 - 1);
        return msg;
    }

    __aicore__ inline void RollBackMsg() { msgRcvPos = (msgRcvPos - 1) & (MAX_MSG_COUNT_Arch3510 - 1); }
};

template <int NUM>
__aicore__ inline void RecvSSbufData(uint64_t* ptr, MSG_POS uint64_t* ptrMsg)
{
#pragma unroll
    for (int i = 0; i < NUM; i++) {
        *(ptr + i) = *(ptrMsg + i);
    }
}

typedef KfcCommServer<false>* KFC_COMM_SERVER_PTR;
#define KFC_COMM_SERVER KfcCommServer<enableHardPoll>
} // namespace AscendC
#endif // __INTF_KFC_COMM_SERVER_H__
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_SERVER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_SERVER_H__
#endif
