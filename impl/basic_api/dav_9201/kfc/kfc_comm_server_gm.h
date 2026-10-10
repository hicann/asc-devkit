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
 * \file kfc_comm_server_gm.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kfc/kfc_comm_server_gm.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_SERVER_GM_H__
#endif
#ifndef KFC_COMM_SERVER_GM_H
#define KFC_COMM_SERVER_GM_H

#include "kfc_comm_gm.h"

namespace AscendC {
class KfcCommServer {
public:
    __gm__ KfcMsg* msgSendHead;  // Message header
    __gm__ KfcMsg* msgSendStart; // the global position of the initialized message.

    // Receiving Message Queue Maintenance
    __gm__ KfcMsg* msgRcvHead;
    __gm__ KfcMsg* msgRcvStart;

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
    // The KFC's second queue serves as the matmul count in the superkernel mode
    __gm__ KfcMsg* msgCntStart;
#endif

    GM_ADDR ubAvalidTail;

    uint8_t msgSendPos; // for the subBlockID of the AIC core
    uint8_t msgRcvPos;  // for the subBlockID of the AIC core
    uint8_t subBlockID; // for the subBlockID of the AIC core

public:
    __aicore__ inline void Init(GM_ADDR workspace, int i)
    {
        // the Rcv on the server is the same as the Send on the client. The addresses of aic and aiv are swap.
        this->msgRcvStart = (__gm__ KfcMsg*)GetMsgHead(workspace, i);
        this->msgSendStart = this->msgRcvStart + MAX_MSG_COUNT;

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
        // vec0 stores the sendEvent and eventId of the vec1, and the vec1 same as vec0.
        // Therefore, the address position of the other party is taken for writing
        uint8_t mapId = i != 0 ? 0 : 1;
        this->msgCntStart = (__gm__ KfcMsg*)GetMsgHead(workspace, mapId) + MAX_MSG_COUNT;
#endif

        this->msgSendHead = this->msgSendStart;
        this->msgSendPos = 0;
        this->msgRcvHead = this->msgRcvStart;
        this->msgRcvPos = 0;
        this->subBlockID = i;
        ASCENDC_ASSERT(
            (this->msgSendStart != nullptr), { KERNEL_LOG(KERNEL_ERROR, "msgSendStart can not be nullptr"); });
        ASCENDC_ASSERT((this->msgRcvStart != nullptr), { KERNEL_LOG(KERNEL_ERROR, "msgRcvStart can not be nullptr"); });
        ubAvalidTail = GetUBAvailableAddr(workspace, i);
    }

    __aicore__ inline __gm__ KfcMsg* AllocMessage()
    {
        return AllocMessageImpl(this->msgSendHead, this->msgSendPos, this->msgSendStart);
    }

    __aicore__ inline void FreeMessage(__gm__ KfcMsg* msg) { FreeMessageImpl(msg); }

    // 310没有L1-GM通道, 改为scalar写gm，dcci
    __aicore__ inline void FreeUB(int32_t addr)
    {
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE2));
        SetFlag<HardEvent::MTE3_MTE2>(eventID);
        WaitFlag<HardEvent::MTE3_MTE2>(eventID);

        eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
        SetFlag<HardEvent::S_MTE3>(eventID);
        WaitFlag<HardEvent::S_MTE3>(eventID);

#ifdef __MSTX_DFX_REPORT__
        MstxCrossRecord record = {
            .addr = reinterpret_cast<uint64_t>(ubAvalidTail),
            .flagId = 1,
            .pipe = pipe_t::PIPE_MTE3,
        };
        __mstx_dfx_report_stub(1, sizeof(MstxCrossRecord), &record);
#endif
        // 添加scalar写gm，dcci刷新清零
        *((__gm__ uint32_t*)ubAvalidTail) = addr;
        dcci(
            reinterpret_cast<__gm__ int64_t*>(ubAvalidTail), cache_line_t::SINGLE_CACHE_LINE,
            dcci_dst_t::CACHELINE_OUT);
    }

    __aicore__ inline __gm__ KfcMsg* RcvMessage()
    {
        auto msg = (__gm__ KfcMsg*)RcvMessageImpl(this->msgRcvHead, this->msgRcvPos, this->msgRcvStart);
        return msg;
    }

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
    __aicore__ inline __gm__ KfcMsg* GetSecondBuffStart() { return this->msgCntStart; }
#endif

    __aicore__ inline void RollBackMsg()
    {
        RollBackMsgImpl(this->msgRcvHead, this->msgRcvPos);
        return;
    }
};

typedef KfcCommServer* KFC_COMM_SERVER_PTR;
#define KFC_COMM_SERVER KfcCommServer
} // namespace AscendC
#endif // KFC_COMM_SERVER_GM_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_SERVER_GM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_SERVER_GM_H__
#endif
