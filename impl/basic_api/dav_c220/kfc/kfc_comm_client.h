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
 * \file kfc_comm_client.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_c220/kfc/kfc_comm_client.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_CLIENT_H__
#endif
#ifndef __KERNEL_KFC_COMM_CLIENT_H__
#define __KERNEL_KFC_COMM_CLIENT_H__

#include "../../kernel_macros.h"
#include "kfc_comm.h"
#include "../../../../include/basic_api/kernel_tpipe.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif

namespace AscendC {
class KfcCommClient {
public:
    // Send Message Queue Maintenance
    __gm__ KfcMsg* msgSendHead;  // Message header
    __gm__ KfcMsg* msgSendStart; // the global position of the initialized message.

    // Receiving Message Queue Maintenance
    __gm__ KfcMsg* msgRcvHead;
    __gm__ KfcMsg* msgRcvStart;

    GM_ADDR ubStart;
    GM_ADDR ubAvalidTail;

    __ubuf__ KfcMsg* ubMsg;
    uint32_t head;
    uint32_t tail;
    uint8_t msgRcvPos;
    uint8_t msgSendPos; // Index used for circular queues. msgQueueHead = msgQueueStart + msgPos
    uint8_t eventID_;
    uint8_t enableHardWare;

public:
    __aicore__ inline KfcCommClient(GM_ADDR workspace, int subBlockID, uint8_t enableHardWare = 0)
    {
        if ASCEND_IS_AIV {
            this->enableHardWare = enableHardWare;
            if (enableHardWare) {
                return;
            }
            ASCENDC_ASSERT((workspace != nullptr), { KERNEL_LOG(KERNEL_ERROR, "workspace can not be nullptr"); });
            ASCENDC_ASSERT((GetTPipePtr() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "tpipe ptr can not be nullptr"); });
            ASCENDC_ASSERT((GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN) != nullptr), {
                KERNEL_LOG(KERNEL_ERROR, "vecin base addr can not be nullptr");
            });
            // Note that the addresses of aic and aiv are exchanged.
            this->msgSendStart = (__gm__ KfcMsg*)GetMsgHead(workspace, subBlockID);

            // The leader of KFC's second queue serves as the matmul count in the superkernel mode
            this->msgRcvStart = this->msgSendStart + MAX_MSG_COUNT;

            this->msgSendHead = this->msgSendStart;
            this->msgSendPos = 0;
            this->msgRcvHead = this->msgRcvStart;
            this->msgRcvPos = 0;

            // During debugging, CPU need to know the global variable address of the tpipe.
#if ASCENDC_CPU_DEBUG
            ubMsg = reinterpret_cast<__ubuf__ KfcMsg*>(
                GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN) + TOTAL_UB_SIZE - sizeof(KfcMsg));
#else
            ubMsg = reinterpret_cast<__ubuf__ KfcMsg*>(TOTAL_UB_SIZE - sizeof(KfcMsg));
#endif
            eventID_ = GetTPipePtr()->AllocEventID<HardEvent::MTE3_S>();
            SetFlag<HardEvent::MTE3_S>((event_t)eventID_);

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
            if (MIX_NUM == 1 && GetSubBlockIdxImpl() == 1) {
                WaitFlag<HardEvent::MTE3_S>((event_t)eventID_);
            }
#endif
            ubStart = GetUBMapAddr(workspace, subBlockID);
            ubAvalidTail = GetUBAvailableAddr(workspace, subBlockID);
            head = 0;
            tail = 0;
        }
    }

    __aicore__ inline ~KfcCommClient()
    {
        if ASCEND_IS_AIV {
            if (this->enableHardWare) {
                return;
            }
            if constexpr (MIX_NUM == 1) {
                // aic-aiv 1:1,aiv subblkidx==0,spkernelaic-aiv1:1--1:2,aiv1 no
                if (GetSubBlockIdxImpl() == 1) {
#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
                    if (GetTaskRationImpl() == 2) {
                        // in sk mode, ops compile in 1:1, execute in 1:2 cv mode
                        WaitForQuitNotification();
                    }
#endif
                    return;
                }
            }
            __gm__ KfcMsg* msg = AllocMessage();
            ASCENDC_ASSERT((msg != nullptr), {
                KERNEL_LOG(KERNEL_ERROR, "ret of alloc message can not be nullptr when client quit");
            });
            uint32_t quitSignal = KfcMsgMakeFlag(KFC_Enum::SERVICE_QUIT, 0);
            *((__gm__ uint32_t*)msg) = quitSignal;
            msg->ubAddr = GetTaskRationImpl(); // vector core nums: 1 & 2

#ifdef __MSTX_DFX_REPORT__
            MstxCrossRecord record = {
                .addr = reinterpret_cast<uint64_t>(msg),
                .flagId = 0,
                .pipe = pipe_t::PIPE_S,
            };
            __mstx_dfx_report_stub(0, sizeof(MstxCrossRecord), &record);
#endif

            dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
            // only executed in 1:2 mode
            if (GetTaskRationImpl() == 2) {
                WaitForQuitNotification();
            }
#endif
        }
    }

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
    __aicore__ inline void WaitForQuitNotification()
    {
        CrossCoreWaitFlag(KFC_SYNC_ID);
        dcci(
            reinterpret_cast<__gm__ int64_t*>(this->msgRcvStart), cache_line_t::SINGLE_CACHE_LINE,
            dcci_dst_t::CACHELINE_OUT);
        // Traverse all matmul objects to compensate for waiting events
        for (int i = 0; i < MAX_MATMUL_OBJ_CNT; i++) {
            uint32_t eventId = reinterpret_cast<__gm__ SuperKernelWaitEventCnt*>(this->msgRcvStart)->eventId[i];
            int32_t eventCnt = reinterpret_cast<__gm__ SuperKernelWaitEventCnt*>(this->msgRcvStart)->eventCnt[i];
            // Get the block id of the corresponding mapped object
            uint32_t mappedBlockId = GetSubBlockIdxImpl() ? 0 : 1;
            if (i * 2 + mappedBlockId == eventId && eventCnt > 0) {
                // After dividing the count value by 16 and taking the modulus, compensate for the waiting event
                int32_t waitCnt = eventCnt % 16;
                for (int i = 0; i < waitCnt; i++) {
                    CrossCoreWaitFlag(eventId);
                }
            }
        }
    }
#endif

    template <bool isAck>
    __disable_kernel_type_autoinfer__ __aicore__ inline void PostMessage(__gm__ KfcMsg* msg)
    {
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
        SetFlag<HardEvent::S_MTE3>(eventID);
        WaitFlag<HardEvent::S_MTE3>(eventID);
        PipeBarrier<PIPE_MTE3>();
        copy_ubuf_to_gm((__gm__ void*)msg, (__ubuf__ void*)ubMsg, 0, 1, sizeof(KfcMsg) / ONE_BLK_SIZE, 0, 0);

#ifdef __MSTX_DFX_REPORT__
        MstxCrossRecord record = {
            .addr = reinterpret_cast<uint64_t>(msg),
            .flagId = 0,
            .pipe = pipe_t::PIPE_MTE3,
        };
        __mstx_dfx_report_stub(0, sizeof(MstxCrossRecord), &record);
#endif

        SetFlag<HardEvent::MTE3_S>((event_t)this->eventID_);
    }

    __aicore__ inline __gm__ KfcMsg* AllocMessage()
    {
        auto ret = AllocMessageImpl(this->msgSendHead, this->msgSendPos, this->msgSendStart);
        WaitFlag<HardEvent::MTE3_S>((event_t)this->eventID_);
        ASCENDC_ASSERT((ret), { KERNEL_LOG(KERNEL_ERROR, "ret of alloc message can not be nullptr"); });
        return ret;
    }

    __aicore__ inline void FreeMessage(__gm__ KfcMsg* msg) { FreeMessageImpl(msg); }

    __aicore__ inline GM_ADDR AllocUB(uint32_t size, int32_t& tailInfo)
    {
#ifdef __MSTX_DFX_REPORT__
        MstxCrossRecord record = {
            .addr = reinterpret_cast<uint64_t>(ubAvalidTail),
            .flagId = 1,
            .pipe = pipe_t::PIPE_S,
            .isMerge = true,
        };
        __mstx_dfx_report_stub(0, sizeof(MstxCrossRecord), &record);
#endif

        GM_ADDR ret;
        if (head + size >= WORKSPACE_UB_SIZE) {
            dcci(
                reinterpret_cast<__gm__ int64_t*>(ubAvalidTail), cache_line_t::SINGLE_CACHE_LINE,
                dcci_dst_t::CACHELINE_OUT);
            tail = *(reinterpret_cast<__gm__ uint32_t*>(ubAvalidTail));
            while (head < tail || tail == 0) {
                Barrier();
                dcci(
                    reinterpret_cast<__gm__ int64_t*>(ubAvalidTail), cache_line_t::SINGLE_CACHE_LINE,
                    dcci_dst_t::CACHELINE_OUT);
                Barrier();
                tail = *(reinterpret_cast<__gm__ uint32_t*>(ubAvalidTail));
            }
            if (tail == head && size == tail) {
                tail = 0;
            }
            head = 0;
        }

        while (head < tail && (head + size >= tail)) {
            Barrier();
            dcci(
                reinterpret_cast<__gm__ int64_t*>(ubAvalidTail), cache_line_t::SINGLE_CACHE_LINE,
                dcci_dst_t::CACHELINE_OUT);
            Barrier();
            tail = *(reinterpret_cast<__gm__ uint32_t*>(ubAvalidTail));
        }

#ifdef __MSTX_DFX_REPORT__
        __mstx_dfx_report_stub(0, sizeof(MstxCrossRecord), &record);
#endif

        ret = ubStart + head;
        head += size;
        tailInfo = head;
        return ret;
    }

    __aicore__ inline __gm__ KfcMsg* RcvMessage()
    {
        auto ret = RcvMessageImpl(this->msgRcvHead, this->msgRcvPos, this->msgRcvStart);
        return ret;
    }
};

#if __NPU_ARCH__ == 2201
#if defined(__DAV_CUBE__)
#elif defined(__DAV_VEC__)
__BLOCK_LOCAL__ __inline__ AscendC::KfcCommClient* g_kfcClient;
#else
__BLOCK_LOCAL__ __inline__ AscendC::KfcCommClient* g_kfcClient;
#endif
#endif

__aicore__ inline AscendC::KfcCommClient* GetKfcClient()
{
#if __NPU_ARCH__ == 2201
#if !defined(__DAV_CUBE__)
    return reinterpret_cast<AscendC::KfcCommClient*>(g_kfcClient);
#else
    return nullptr;
#endif
#else
    ASSERT(g_coreType == AscendC::AIV && "not supported on current device");
    return nullptr;
#endif
}
} // namespace AscendC
#endif // __KERNEL_KFC_COMM_CLIENT_H__
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_CLIENT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_CLIENT_H__
#endif
