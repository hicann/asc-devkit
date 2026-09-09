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
 * \file kernel_kfc.h
 * \brief
 */
#ifndef LIB_MATMUL_KERNEL_KFC_H
#define LIB_MATMUL_KERNEL_KFC_H

#if ASCENDC_CPU_DEBUG
#include <cstring>
#include <unistd.h>
#endif

#include "../../../../include/basic_api/kernel_basic_intf.h"
#ifdef LIB_MATMUL_MATMUL_INTF_H
#include "../../../../include/adv_api/matmul/matmul_client.h"
#include "../matmul/kfc/matmul_server_aux.h"
#endif
namespace AscendC {
constexpr uint16_t WORKSPACE_SYNC_ID = 15;
__aicore__ inline void clearWorkspace(__gm__ uint8_t* workspace)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    AscendC::SetAtomicNone();
    if ASCEND_IS_AIC {
        AscendC::SetMaskNorm();
        AscendC::SetLoadDataBoundary((uint64_t)0);
        AscendC::SetLoadDataPaddingValue((uint64_t)0);
        AscendC::ClearWorkspaceImpl(workspace);
        AscendC::NotifyEvent<PIPE_MTE3>(WORKSPACE_SYNC_ID);
    } else {
        SetVectorMask<uint64_t, MaskMode::NORMAL>((uint64_t)-1, (uint64_t)-1);
        AscendC::SetMaskNorm();
    }
#endif
}

template <uint8_t enableHardPoll = 0>
class KfcServer { // AIC side
public:
    __aicore__ inline void Init(GM_ADDR workspaceGM)
    {
        ASSERT(workspaceGM != nullptr && "workspaceGM cannot be nullptr when init kfc server");
        workspace = workspaceGM;
        quitSize = 0;
        for (int32_t i = 0; i < MIX_NUM; i++) {
            kfcCommSrv[i].Init(workspace, i); // Initialize the message queue on the server.
        }
    }

    __aicore__ inline bool isRun()
    {
        // The function exists when all AIVs exit. The client sends a Quit message when the destructor ends.
        return quitSize < MIX_NUM;
    }
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) && KFC_C310_SSBUF
    template <class T, class... Args>
    __aicore__ inline void Run(T& a, Args&&... b)
    {
        TRACE_START(TraceId::KFC_SERVER_RUN);
        auto ptr = kfcCommSrv;
        MSG_POS KfcMsg* msg;
        bool ret = true;
        int i = 0;
        bool mcgSyncSwitch = true;
        bool freeMsg = true;
        uint32_t intraId = 15;
        bool firstSameABMsg = 1;
        if constexpr (MIX_NUM == 1) {
            mcgSyncSwitch = false;
        }
        while (true) { // Get messages of each AIV core in polling mode.
            TRACE_START(TraceId::KFC_SERVER_REV_MSG);
            if constexpr (enableHardPoll == 1) { // it's all sameAB
                CrossCoreWaitFlag<KFC_INTRA_MODE, PIPE_S>(intraId);
            }
            msg = ptr->RcvMessage(msgHead);
            TRACE_STOP(TraceId::KFC_SERVER_REV_MSG);
            if constexpr (enableHardPoll != 1) {
                // soft kfc
                if (msg == nullptr) {
                    if (ret && mcgSyncSwitch) { // = false, lock a queue and must wait for release.
                        i = 1 - i;
                        ptr = &(kfcCommSrv[i]);
                    }
                    continue;
                }
            }
            // The check message is public
            TRACE_START(TraceId::KFC_SERVER_PROCESS_MSG);
            auto funID = KfcMsgGetFunID(msgHead);
            if constexpr (enableHardPoll == 2) {
                auto isSameAB = KfcMsgGetIsSameAB(msgHead);
                // first sameAB is v1, rollback
                if (i == 1 && firstSameABMsg && isSameAB) {
                    ptr->RollBackMsg();
                    i = 1 - i;
                    ptr = &(kfcCommSrv[i]);
                    continue;
                }
            }
            auto srvID =
                static_cast<KFC_Enum>(static_cast<uint16_t>(funID) & static_cast<uint16_t>(KFC_Enum::SERVICE_ID_MASK));
            freeMsg = true;
            if (srvID == KFC_Enum::SERVICE_ID_MATMUL) {
                ret = RunAux(i, msg, funID, freeMsg, a, b...);
            } else if (srvID == KFC_Enum::SERVICE_ID_SCM) {
                if (funID == KFC_Enum::SCMFUN_GM2L1) {
                    ScmDataCopy(&msg->buffer);
                } else if (funID == KFC_Enum::SCMFUN_GM2L1ND2NZ) {
                    ScmDataCopyND2NZ(&msg->buffer);
                }
            } else if (funID == KFC_Enum::SERVICE_QUIT) {
                quitSize++;
                if (msg->userCustomData == 1) {
                    quitSize = MIX_NUM;
                }
                ptr->FreeMessage(msg);
                if (quitSize == MIX_NUM) {
                    return;
                } else {
                    i = 1 - i; // switch to the opposite end and don't switch back
                    ptr = &(kfcCommSrv[i]);
                    mcgSyncSwitch = false;
                    intraId = 46 - intraId;
                    continue;
                }
            } else {
                ASSERT("unsupported service id !");
            }
            if constexpr (enableHardPoll == 1) {
                ptr->FreeMessage(msg);
                TRACE_STOP(TraceId::KFC_SERVER_PROCESS_MSG);
            } else {
                if (freeMsg) {
                    ptr->FreeMessage(msg); // Move the message backward by one after the message processed.
                    TRACE_STOP(TraceId::KFC_SERVER_PROCESS_MSG);
                    if constexpr (enableHardPoll == 2) {
                        if (KfcMsgGetIsSameAB(msgHead)) {
                            firstSameABMsg = 0;
                        }
                    }
                } else {
                    ptr->RollBackMsg();
                    i = 1 - i;
                    ptr = &(kfcCommSrv[i]);
                    continue;
                }
            }
            if (ret && mcgSyncSwitch) { // = false, lock a queue and must wait for release.
                i = 1 - i;
                ptr = &(kfcCommSrv[i]);
                intraId = 46 - intraId;
            }
        }
        TRACE_STOP(TraceId::KFC_SERVER_RUN);
    }
#else
    template <class T, class... Args>
    __aicore__ inline void Run(T& a, Args&&... b)
    {
        TRACE_START(TraceId::KFC_SERVER_RUN);
        auto ptr = kfcCommSrv;
        __gm__ KfcMsg* msg;
        bool switchPoll = true;
        for (int i = 0; i < MIX_NUM;) { // Get messages of each AIV core in polling mode.
            TRACE_START(TraceId::KFC_SERVER_REV_MSG);
            msg = ptr->RcvMessage();
            TRACE_STOP(TraceId::KFC_SERVER_REV_MSG);
            if (msg) {
                // The check message is public
                TRACE_START(TraceId::KFC_SERVER_PROCESS_MSG);
                auto funID = KfcMsgGetFunID(msg->head);
                auto srvID = static_cast<KFC_Enum>(
                    static_cast<uint16_t>(funID) & static_cast<uint16_t>(KFC_Enum::SERVICE_ID_MASK));
                bool freeMsg = true;
                if (srvID == KFC_Enum::SERVICE_ID_MATMUL) {
                    switchPoll = RunAux(i, msg, funID, freeMsg, a, b...);
                } else if (srvID == KFC_Enum::SERVICE_ID_SCM) {
                    if (funID == KFC_Enum::SCMFUN_GM2L1) {
                        ScmDataCopy(&msg->buffer);
                    } else if (funID == KFC_Enum::SCMFUN_GM2L1ND2NZ) {
                        ScmDataCopyND2NZ(&msg->buffer);
                    }
                    if (unlikely(msg->ubAddr >= 0)) {
                        ptr->FreeUB(msg->ubAddr);
                    }
                    switchPoll = true;
                } else if (funID == KFC_Enum::SERVICE_QUIT) {
                    quitSize++;
                    switchPoll = true;

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
                    // only executed in a 1:2 mode
                    if (quitSize == 2) {
                        // after receiving two exit message, AIC sends a message to AIV and performs a counter reset.
                        auto secondMsgStartPos = ptr->GetSecondBuffStart();
                        // scalar write the GM, dcci write back to GM
                        dcci(
                            reinterpret_cast<__gm__ int64_t*>(secondMsgStartPos), cache_line_t::SINGLE_CACHE_LINE,
                            dcci_dst_t::CACHELINE_OUT);
                        SetFlag<HardEvent::S_MTE3>(EVENT_ID0);
                        WaitFlag<HardEvent::S_MTE3>(EVENT_ID0);
                        NotifyEvent<PIPE_MTE3>(KFC_SYNC_ID);
                    }
#endif

                    if (msg->ubAddr == 1) {
                        quitSize++;
                        TRACE_STOP(TraceId::KFC_SERVER_RUN);
                        return;
                    }
                } else {
                    ASSERT("unsupported service id !");
                }
                if (freeMsg) {
                    ptr->FreeMessage(msg); // Move the message backward by one after the message processed.
                    TRACE_STOP(TraceId::KFC_SERVER_PROCESS_MSG);
                } else {
                    ptr->RollBackMsg();
                    i++;
                    ptr++;
                    continue;
                }
            }
            if (switchPoll) { // = false, lock a queue and must wait for release.
                i++;
                ptr++;
            }
        }
        TRACE_STOP(TraceId::KFC_SERVER_RUN);
    }
#endif

    template <class T, class... Args>
    __aicore__ inline void InitObj(TPipe* tpipe, T& a, Args&&... b)
    {
        if constexpr (sizeof(T) == sizeof(void*)) { // Skip previous invalid pointer for compatibility
            InitObj(b...);
        } else {
            auto ptr = reinterpret_cast<KFC_COMM_SERVER_PTR>(kfcCommSrv);
            for (int i = 0; i < MIX_NUM; i++, ptr++) {
                InitObjAux(tpipe, ptr, i, 0, a, b...);
            }
        }
    }

    __aicore__ inline void Quit() {}

    template <class T, class... Args>
    __aicore__ static inline constexpr bool isTiling()
    {
        return sizeof(T) == sizeof(void*);
    }

    template <class T, class... Args>
    __aicore__ static T* GetTiling(T* t, Args&&... b)
    {
        return t;
    }

private:
    template <class T, class... Args>
    __aicore__ inline bool RunAuxSkip(
        int subBlockID, MSG_POS KfcMsg* msg, KFC_Enum funID, bool& freeMsg, T& a, Args&&... b)
    {
        return RunAux(subBlockID, msg, funID, freeMsg, b...);
    }
    template <class T, class... Args>
    __aicore__ inline bool RunAux(int subBlockID, MSG_POS KfcMsg* msg, KFC_Enum funID, bool& freeMsg, T& a, Args&&... b)
    {
        ASSERT(msg != nullptr && "msg cannot be nullptr when kfc server run aux");
        ASSERT(subBlockID >= 0 && subBlockID < MIX_NUM && "sub block id should be [0, MIX_NUM)");
        if (a.cubeObj.cubeObj[0].IsSharedObj()) {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) && KFC_C310_SSBUF
            if constexpr ((sizeof...(b) == 1)) {
                // b == 1 and is tiling, a process, b == 1 and is mm, instID continue judge, may b
                if constexpr (isTiling<Args...>()) {
                    if (a.cubeObj.cubeObj[0].template SkipMsg<enableHardPoll>(funID, freeMsg, lastMsgId, subBlockID)) {
                        return true;
                    }
                    freeMsg = true;
                    a.cubeObj.cubeObj[0].SetSubBlockIdx(static_cast<uint8_t>(subBlockID));
                    auto ret = a.cubeObj.cubeObj[0].Process(msg, funID);
                    if (a.cubeObj.cubeObj[0].template LockMsgQueue<enableHardPoll>(
                            funID, freeMsg, lastMsgId, subBlockID, msg)) {
                        return false; // lock on queue of v0
                    }
                    return ret;
                }
            } else if (a.cubeObj.cubeObj[0].GetInstID() == KfcMsgGetInstID(msgHead)) {
#else
            if (a.cubeObj.cubeObj[0].GetInstID() == KfcMsgGetInstID(msg->head)) {
#endif
                if (a.cubeObj.cubeObj[0].template SkipMsg<enableHardPoll>(funID, freeMsg, lastMsgId, subBlockID)) {
                    return true;
                }
                freeMsg = true;
                a.cubeObj.cubeObj[0].SetSubBlockIdx(static_cast<uint8_t>(subBlockID));
                auto ret = a.cubeObj.cubeObj[0].Process(msg, funID);
                if (a.cubeObj.cubeObj[0].template LockMsgQueue<enableHardPoll>(
                        funID, freeMsg, lastMsgId, subBlockID, msg)) {
                    return false; // lock on queue of v0
                }
                return ret;
            } else if constexpr (sizeof...(b) == 0) {
                ASSERT(0);
                return true;
            } else if constexpr (isTiling<Args...>()) {
                if constexpr (sizeof...(b) > 1) {
                    return RunAuxSkip(subBlockID, msg, funID, freeMsg, b...);
                }
            } else if constexpr (sizeof...(b) >= 1) {
                return RunAux(subBlockID, msg, funID, freeMsg, b...);
            }
            return true;
        } else {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) && KFC_C310_SSBUF
            if (a.cubeObj.cubeObj[subBlockID].GetInstID() == KfcMsgGetInstID(msgHead)) {
#else
            if (a.cubeObj.cubeObj[subBlockID].GetInstID() == KfcMsgGetInstID(msg->head)) {
#endif
                if (a.cubeObj.cubeObj[subBlockID].template SkipMsg<enableHardPoll>(
                        funID, freeMsg, lastMsgId, subBlockID)) {
                    return true;
                }
                freeMsg = true;
                a.cubeObj.cubeObj[subBlockID].SetSubBlockIdx(static_cast<uint8_t>(subBlockID));
                auto ret = a.cubeObj.cubeObj[subBlockID].Process(msg, funID);
                if (a.cubeObj.cubeObj[subBlockID].template LockMsgQueue<enableHardPoll>(
                        funID, freeMsg, lastMsgId, subBlockID, msg)) {
                    return false; // lock on queue of v0
                }
                return ret;
            } else if constexpr (sizeof...(b) == 0) {
                ASSERT(0);
                return true;
            } else if constexpr (isTiling<Args...>()) {
                if constexpr (sizeof...(b) > 1) {
                    return RunAuxSkip(subBlockID, msg, funID, freeMsg, b...);
                }
            } else if constexpr (sizeof...(b) >= 1) {
                return RunAux(subBlockID, msg, funID, freeMsg, b...);
            }
            return true;
        }
    }

    template <class T, class... Args>
    __aicore__ inline void InitObjAuxSkip(
        TPipe* tpipe, KFC_COMM_SERVER_PTR kfc, int subBlockID, int instID, T* a, Args&&... b)
    {
        InitObjAux(tpipe, kfc, subBlockID, instID, b...);
    }

    template <class T, class... Args>
    __aicore__ inline void InitObjAux(
        TPipe* tpipe, KFC_COMM_SERVER_PTR kfc, int subBlockID, int instID, T& a, Args&&... b)
    {
        ASSERT(kfc != nullptr && "kfc cannot be nullptr when kfc server init obj aux");
        ASSERT(subBlockID >= 0 && subBlockID < MIX_NUM && "sub block id should be [0, MIX_NUM)");
        ASSERT(tpipe != nullptr);
        ASSERT(instID >= 0 && instID < MAX_MATMUL_OBJ && "matmul instID id be  [0, MAX_MATMUL_OBJ)");

        if constexpr (sizeof...(b) == 0) {
            if (a.cubeObj.cubeObj[0].IsSharedObj()) {
                if (subBlockID == 0) {
                    a.cubeObj.cubeObj[0].InitKfc(tpipe, (void*)nullptr, kfc, instID, workspace);
                }
            } else {
                a.cubeObj.cubeObj[subBlockID].InitKfc(tpipe, (void*)nullptr, kfc, instID, workspace);
            }
        } else if constexpr (isTiling<Args...>()) {
            auto tiling = GetTiling(b...);
            if (a.cubeObj.cubeObj[0].IsSharedObj()) {
                if (subBlockID == 0) {
                    a.cubeObj.cubeObj[0].InitKfc(tpipe, tiling, kfc, instID, workspace);
                    if constexpr (sizeof...(b) > 1) {
                        InitObjAuxSkip(tpipe, kfc, subBlockID, instID + 1, b...);
                    }
                } else {
                    if constexpr (sizeof...(b) > 1) {
                        InitObjAuxSkip(tpipe, kfc, subBlockID, instID + 1, b...);
                    }
                }
            } else {
                a.cubeObj.cubeObj[subBlockID].InitKfc(tpipe, tiling, kfc, instID, workspace);
                if constexpr (sizeof...(b) > 1) {
                    InitObjAuxSkip(tpipe, kfc, subBlockID, instID + 1, b...);
                }
            }
        } else {
            a.cubeObj.cubeObj[subBlockID].InitKfc(tpipe, (void*)nullptr, kfc, instID, workspace);
            if constexpr (sizeof...(b) >= 1) {
                InitObjAux(tpipe, kfc, subBlockID, instID + 1, b...);
            }
        }
    }

    // Apply for two servers on the server. aic<->aiv 1:1
    KFC_COMM_SERVER kfcCommSrv[MIX_NUM];
    GM_ADDR workspace;
    uint8_t quitSize;
    int lastMsgId = 1;
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) && KFC_C310_SSBUF
    uint32_t msgHead;
#endif
};

template <class T, class... Args>
__aicore__ inline void SetMatrixKfcSkip(
    TPipe* pipe, KfcCommClient* kfcClient, const int32_t instID, GM_ADDR workspace, T& cubeObj, Args&&... b)
{
    SetMatrixKfc(pipe, kfcClient, instID, workspace, b...);
}

template <class T, class... Args>
__aicore__ inline void SetMatrixKfc(
    TPipe* pipe, KfcCommClient* kfcClient, const int32_t instID, GM_ADDR workspace, T& cubeObj, Args&&... b)
{
    ASSERT((pipe != nullptr) && "pipe should not be nullptr.");
    ASSERT((kfcClient != nullptr) && "kfcClient should not be nullptr.");
    ASSERT((workspace != nullptr) && "workspace should not be nullptr.");

    if constexpr (sizeof...(b) == 0) {
        InitKfcClient(cubeObj, (void*)nullptr, pipe, kfcClient, instID, workspace);
    } else if constexpr (KfcServer<false>::isTiling<Args...>()) {
        auto tiling = KfcServer<false>::GetTiling(b...);
        InitKfcClient(cubeObj, tiling, pipe, kfcClient, instID, workspace);
        if constexpr (sizeof...(b) > 1) {
            SetMatrixKfcSkip(pipe, kfcClient, instID + 1, workspace, b...);
        }
    } else {
        InitKfcClient(cubeObj, (void*)nullptr, pipe, kfcClient, instID, workspace);
        if constexpr (sizeof...(b) >= 1) {
            SetMatrixKfc(pipe, kfcClient, instID + 1, workspace, b...);
        }
    }
}
}; // namespace AscendC
// Compatible with the previously used matmul namespace
namespace matmul = AscendC;
#endif
