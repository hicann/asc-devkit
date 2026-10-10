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
 * \file kfc_comm_gm.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kfc/kfc_comm_gm.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_GM_H__
#endif
#ifndef KFC_COMM_GM_H
#define KFC_COMM_GM_H

#include "../../../../include/basic_api/kernel_common.h"
#include "../kernel_operator_common_impl.h"
#include "../kernel_operator_set_atomic_impl.h"
#include "../kernel_tpipe_impl_c310.h"
#include "kfc_log.h"
#include "../../../../include/basic_api/kernel_tpipe.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif
#include "../../../../include/basic_api/kernel_operator_block_sync_intf.h"

namespace AscendC {
#define MSG_POS __gm__

__aicore__ inline void Barrier()
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    __asm__ __volatile__("" ::: "memory");
#else
    __asm__ __volatile__("");
#endif
}

enum class KFC_Enum : uint16_t {
    SERVICE_ID_MASK = 0xFF00,
    SERVICE_ID_SCM = 0x0100,
    SCMFUN_GM2L1,
    SCMFUN_GM2L1ND2NZ,
    SERVICE_ID_MATMUL = 0x0300,
    MMFUN_MASK = 0x0380,    // Indicates that Iterate needs to be invoked.
    MMFUN_ITERATE = 0x0380, // Iterate and IterateAll occupy 2 bits to save the cost of judgment.
    MMFUN_ITERATE_ALL = 0x0381,
    MMFUN_INIT = 0x0301,
    MMFUN_GET_TENSOR_C,
    MMFUN_ITERATE_ALL_RESP,
    MMFUN_GET_TENSOR_C_RESP,
    MMFUN_GET_OFFSET_C,
    MMFUN_GET_OFFSET_C_RESP,
    MMFUN_SET_ORG_SHAPE,
    MMFUN_SET_HF32,
    MMFUN_SET_USER_DEF_INFO,
    MMFUN_ITERATE_BATCH_ALL,
    MMFUN_ITERATE_BATCH_ALL_RESP,
    MMFUN_ITERATE_N_BATCH_ALL,
    MMFUN_ITERATE_N_BATCH_ALL_RESP,
    MMFUN_END,
    CONVFUNC_ITERATE,
    CONVFUNC_ITERATE_ALL,
    CONVFUNC_GET_TENSOR_C,
    CONVFUNC_END,
    SERVICE_QUIT = 0xfd00,
    SERVICE_BALANCE = 0xfe00,
    SERVICE_ID_NONE = 0xff00
};

enum class MSG_STATE : uint8_t {
    STATE_INVALID,
    STATE_SET,
};

// The message length is 128 bytes and must be 512 bytes aligned.
// Therefore, MAX_MSG_COUNT must be an integer multiple of 8.
#ifdef __MIX_CORE_AIC_RATION__
constexpr int32_t MIX_NUM = __MIX_CORE_AIC_RATION__;
#else
constexpr int32_t MIX_NUM = 2;
#endif
constexpr int32_t MAX_BLOCK_AIV_NUM = 2;
constexpr int32_t MIX_COEFFICIENT = 1;
constexpr int32_t MAX_MATMUL_OBJ = 8;
constexpr int MAX_AIV_NUM = 72;
constexpr int MAX_AIC_NUM = 36;
constexpr int ALIGN_SIZE = 32;
constexpr int BIDIRECTION_NUM = 2;
constexpr bool KFC_APPLY_MSG = true;
constexpr uint64_t INC_PROCESS_CHECK = 14;
constexpr uint64_t WORKSPACE_UB_SIZE = TOTAL_UB_SIZE + 8 * 1024;
constexpr int32_t MAX_GROUP_ID = 32;
constexpr int32_t MM_CNT_MAX = 1024;
constexpr int32_t QUIT_CNT = 4;
constexpr int32_t MAX_SYNC_COUNT = 100000000;
constexpr int32_t MMCNT_L1_RESERVERD_SIZE = 64; // Reserved L1 64B for MMCNT Query
constexpr uint16_t KFC_SYNC_ID = 15;

#ifdef __ASCENDC_ENABLE_SUPER_KERNEL__
// the max cnt of matmul object in superkernel mode
constexpr int MAX_MATMUL_OBJ_CNT = 4;
struct SuperKernelWaitEventCnt {
    uint32_t eventId[MAX_MATMUL_OBJ_CNT] = {0};
    int32_t eventCnt[MAX_MATMUL_OBJ_CNT] = {0};
};
#endif

struct TilingInfo {
    GM_ADDR tilingAddr; // the GM address of the tiling.
};

struct MatmulOrgShape {
    uint32_t orgM;
    uint32_t orgN;
    uint32_t orgKa;
    uint32_t orgKb;
    uint32_t orgKc;
};

#ifdef __MSTX_DFX_REPORT__
struct MstxCrossRecord {
    uint64_t addr;
    uint16_t flagId;
    pipe_t pipe;
    bool isMore;
    bool isMerge;
};
#endif

struct MatmulConfigParams {
    // offset->8
    uint32_t enAtomic : 8;
    uint32_t enSequentialWrite : 1;
    uint32_t isTransA : 1;
    uint32_t isTransB : 1;
    uint32_t enPartialSum : 1;
    uint32_t setTail : 1;
    uint32_t setTensorA : 1;
    uint32_t setTensorB : 1;
    uint32_t setTensorBias : 1;
    uint32_t setClearBias : 1;
    uint32_t cIsTscm : 1;
    uint32_t isFirstIter : 1;
    uint32_t sync : 1;
    uint32_t enHF32 : 1;
    uint32_t hf32TransMode : 1;
    uint32_t setQuant : 1;
    uint32_t setBatch : 1;
    uint32_t waitIterateAll : 1;
    uint32_t waitIterateBatch : 1;
    uint32_t iterateFakeMsg : 1;

    uint32_t singleM;
    uint32_t singleN;
    uint32_t singleK;
    uint32_t sizeAmatrix;
    uint32_t sizeBmatrix;

    uint64_t aAddr;
    uint64_t bAddr;
    uint64_t cAddr;
    uint64_t biasAddr;  // 56 bytes, but access to the maximum message size (64 bytes)
    uint64_t quantAddr; // 64 bytes
    uint32_t quantSize;
    uint32_t quantMode;
    uint64_t quantScalar; // 80 bytes
    uint32_t batchA;
    uint32_t batchB;
    uint32_t matrixStrideA;
    uint32_t matrixStrideB;
    uint32_t matrixStrideC;
    uint32_t batchLoop;
    uint32_t counterId; // query mm task process
    uint32_t reserved0;
    uint64_t dataPtr; // 120 bytes
};

struct Conv3DBpInputConfigParams {
    uint32_t enAtomic : 8;
    uint32_t enSequentialWrite : 1;
    uint32_t setTensorWeight : 1;
    uint32_t sync : 1;
    uint32_t isFirstIter : 1;
    uint32_t setTensorFmap : 1;
    uint32_t setTensorOutBackprop : 1;
    uint32_t setSingleShape : 1;
    uint32_t setStartIdx : 1;
    uint32_t enPartialSum : 1;

    uint32_t singleShapeN;
    uint32_t singleShapeD;
    uint32_t curDinStartIdx;

    uint32_t singleShapeM;
    uint32_t singleShapeK;

    uint64_t weightAddr;
    uint64_t outBackpropAddr;
    uint64_t outputAddr;

    int32_t curHoStartIdx;
    uint32_t res;
    uint32_t res1[16];
};

struct Conv3DBpFilterConfigParams {
    uint32_t enAtomic : 8;
    uint32_t enSequentialWrite : 1;
    uint32_t setTensorFmap : 1;
    uint32_t setTensorOutBackprop : 1;
    uint32_t setSingleShape : 1;
    uint32_t sync : 1;
    uint32_t isFirstIter : 1;
    uint32_t waitIterateAll : 1;
    uint32_t enPartialSum : 1;

    uint32_t singleShapeM;
    uint32_t singleShapeN;
    uint32_t singleShapeK;

    uint64_t fmapAddr;
    uint64_t outBackpropAddr;
    uint64_t outputAddr;

    uint32_t curHoStartIdx;
    uint32_t fmapSize;
    uint32_t outBackpropSize;
    uint32_t res;

    uint32_t res1[16];
};

struct Conv3DForwardConfigParams {
    uint32_t enAtomic : 8; // for enable atomic add
    uint32_t enSequentialWrite : 1;
    uint32_t enSetTensorFmap : 1;        // for enable SetTensorFmap intf
    uint32_t enSetTensorWeight : 1;      // for enable SetTensorWeight intf
    uint32_t enSetTensorBias : 1;        // for enable SetTensorBias intf
    uint32_t sync : 1;                   // for control sync aic/aiv
    uint32_t enSetSingleOutputShape : 1; // for enable SetSingleOutputShape intf
    uint32_t enSetFmapStartPosition : 1; // for enable SetFmapStartPosition intf
    uint32_t waitIterateAll : 1;
    uint32_t enPartialSum : 1; // for part of results of L0C write to gm
    uint32_t fmapSize;

    uint64_t fmapAddr;
    uint64_t weightAddr;
    uint64_t biasAddr;
    uint64_t outputAddr;

    uint32_t weightSize;
    uint32_t biasSize;
    uint32_t singleCoreBatch;
    uint32_t singleCo;
    uint32_t singleDo;
    uint32_t singleCoreM;
    uint32_t singleGroupOpt;
    uint32_t diStartPos;
    uint32_t mStartPos;
    uint32_t ciStartPos;
    uint32_t res[10]; // Reserved，for 120Byte ailgn
};

struct MatmulUserDefInfo {
    uint64_t tilingPtr;
};

constexpr uint16_t KFC_MSG_BYTE_OFFSET = 16;
// AIC->AIV, set the event count.
__aicore__ inline uint16_t KfcMsgGetEvtCnt(uint32_t flag) { return flag & 0x00007fff; }
// AIV->AIC, set the instance ID
__aicore__ inline uint16_t KfcMsgGetInstID(uint32_t flag) { return flag & 0x000000ff; }
__aicore__ inline KFC_Enum KfcMsgGetFunID(uint32_t flag)
{
    return static_cast<KFC_Enum>((flag & 0xffff0000) >> KFC_MSG_BYTE_OFFSET);
}
__aicore__ inline uint32_t KfcMsgGetState(uint32_t flag) { return (flag & 0x00008000); }
__aicore__ inline uint32_t KfcMsgMakeFlag(KFC_Enum funID, uint16_t instID)
{
    return (((static_cast<uint16_t>(funID) << KFC_MSG_BYTE_OFFSET) + 0x8000) + (instID));
}

// Currently, the maximum message size is 64 bytes, which is the same as the size of a CacheLine.
struct KfcMsg {
    uint32_t head = 0;
    int32_t ubAddr = -1;
    union {
        uint8_t buffer[120]; // Used for customizing and formatting messages
        TilingInfo tilingInfo;
        MatmulConfigParams body;
        MatmulOrgShape orgShape;
        MatmulUserDefInfo userDefInfo;
        Conv3DBpInputConfigParams convBpInputBody;
        Conv3DBpFilterConfigParams convBpFilterBody;
        Conv3DForwardConfigParams convForwardBody;
    };
};
struct MsgUBAvailable {
    uint32_t head;
    uint32_t res;
    uint8_t buffer[56];
};
struct MsgMatmulCnt {
    uint32_t head;
    uint32_t res;
    uint8_t buffer[56];
};

struct QuitCnt {
    int32_t head;
    uint32_t res;
    uint8_t buffer[56];
};

struct MmTaskCnt {
    uint32_t head;
    uint32_t res;
    uint8_t buffer[56];
};

struct MsgGroupSync {
    int32_t syncCount; // current sync aiv number
    uint8_t res[28];
    uint32_t allNumber; // aiv number in one group
    uint8_t buffer[28];
};

struct MsgGroupSyncAux {
    // for reliable and dfx
    int32_t curNumber; // number of aiv has been in group
    uint8_t res[28];
    uint32_t idField; // curBlockID << 16 + groupID
    uint8_t buffer[28];
};

__aicore__ inline constexpr int AlignTo32(int size) { return (size + ALIGN_SIZE - 1) / ALIGN_SIZE * ALIGN_SIZE; }

struct SysWorkspaceDesc {
    KfcMsg kfcMsg[MAX_AIV_NUM * BIDIRECTION_NUM * MAX_MSG_COUNT * MIX_COEFFICIENT];
    MsgMatmulCnt cntMsg[MAX_AIV_NUM * MIX_COEFFICIENT][MAX_MATMUL_OBJ];
    MsgUBAvailable ubMsg[MAX_AIV_NUM];
    uint8_t ubMap[MAX_AIV_NUM][WORKSPACE_UB_SIZE];
    QuitCnt quitCnt[QUIT_CNT];
    MmTaskCnt mmTaskCnt[MM_CNT_MAX];
    MsgGroupSync groupSyncMsg[MAX_GROUP_ID];
    MsgGroupSyncAux groupSyncAuxMsg[MAX_GROUP_ID];
};

__aicore__ inline void ClearWorkspaceImpl(__gm__ uint8_t* workspace)
{
    if (GetBlockIdxImpl() >= MAX_AIV_NUM) {
        // the kfc does nothing with the block idx out of mix_aic_num.
        return;
    }
    // v0 清除  vec0: 0~size;v1 清除 vec1:size1 ~ size2; v2 清除 vec3:size2 ~ size3
    constexpr uint32_t size = BIDIRECTION_NUM * MAX_MSG_COUNT * AlignTo32(sizeof(KfcMsg));
    constexpr uint32_t sizeUbmsg = AlignTo32(sizeof(MsgUBAvailable));
    constexpr uint32_t offsetUbMsg =
        MAX_AIV_NUM * BIDIRECTION_NUM * MAX_MSG_COUNT * MIX_COEFFICIENT * AlignTo32(sizeof(KfcMsg)) +
        MAX_AIV_NUM * MIX_COEFFICIENT * MAX_MATMUL_OBJ * AlignTo32(sizeof(MsgMatmulCnt));
    constexpr uint32_t block = size / 2048;
    uint32_t ubOffset11 = 0;
    uint32_t msgOffset11 = 0;
    if constexpr (MIX_NUM == 1) {
        msgOffset11 = BIDIRECTION_NUM * MAX_MSG_COUNT * AlignTo32(sizeof(KfcMsg));
        ubOffset11 = AlignTo32(sizeof(MsgUBAvailable));
    }
    __gm__ uint8_t* msgStartAddr = (__gm__ uint8_t*)(workspace + (size + msgOffset11) * GetBlockIdxImpl());
    __gm__ uint8_t* ubMsgStartAddr =
        (__gm__ uint8_t*)(workspace + offsetUbMsg + (sizeUbmsg + ubOffset11) * GetBlockIdxImpl());

#if ASCENDC_CPU_DEBUG
    auto ubMsg = reinterpret_cast<__ubuf__ uint64_t*>(
        GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN) + TOTAL_UB_SIZE - sizeof(KfcMsg));
#else
    auto ubMsg = reinterpret_cast<__ubuf__ uint64_t*>(0);
#endif
    for (int i = 0; i < 16; i++) {
        *(ubMsg + i) = 0;
    };

    // 硬件时间类型需修改为 S_MTE3
    SetFlag<HardEvent::S_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::S_MTE3>(EVENT_ID0);

    for (size_t i = 0; i < block; i++) {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201 || __NPU_ARCH__ == 9202)
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)msgStartAddr, (__ubuf__ void*)ubMsg, 0, 16, 128, false, false, 0, 0, 128, 0, false);
#else
        // 第四参数：blockCount: 16 ; 第五参数： burstLength: 128; 第七参数: dstStride: 128
        copy_ubuf_to_gm_align_v2((__gm__ void*)msgStartAddr, (__ubuf__ void*)ubMsg, 0, 16, 128, 0, 128, 0);
#endif
        msgStartAddr += 2048;
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201 || __NPU_ARCH__ == 9202)
    copy_ubuf_to_gm_align_v2(
        (__gm__ void*)ubMsgStartAddr, (__ubuf__ void*)ubMsg, 0, 1, sizeUbmsg, false, false, 0, 0, 0, 0, false);
#else
    copy_ubuf_to_gm_align_v2((__gm__ void*)ubMsgStartAddr, (__ubuf__ void*)ubMsg, 0, 1, sizeUbmsg, 0, 0, 0);
#endif
    PipeBarrier<PIPE_ALL>();
}
__aicore__ inline GM_ADDR GetMsgHead(GM_ADDR workspace, int i)
{
    // Get the message header of the MSG based on the block/
    // MSG arrangement       AIV                    AIV
    //  MSG0 AIV0-->AIC0   GetBlockIdx()=0,  GetSubBlockIdx()=0,
    //  MSG1 AIV0<--AIC0   GetBlockIdx()=0,  GetSubBlockIdx()=0,
    //  MSG2 AIV1-->AIC0   GetBlockIdx()=0,  GetSubBlockIdx()=1,
    //  MSG3 AIV1<--AIC0   GetBlockIdx()=0,  GetSubBlockIdx()=1,

    ASCENDC_ASSERT((i >= 0 && i < MIX_NUM), {
        KERNEL_LOG(KERNEL_ERROR, "input i is %d, which should be in range [0, %d)", i, MIX_NUM);
    });
    ASCENDC_ASSERT((workspace != nullptr), { KERNEL_LOG(KERNEL_ERROR, "workspace can not be nullptr"); });

    auto ptr = reinterpret_cast<__gm__ struct SysWorkspaceDesc*>(workspace);
    // Initialize based on the input i value on AIC.
    auto flatBlockID = get_block_idx() * MAX_BLOCK_AIV_NUM + i;
    return reinterpret_cast<GM_ADDR>(&ptr->kfcMsg[flatBlockID * BIDIRECTION_NUM * MAX_MSG_COUNT]);
}

__aicore__ inline GM_ADDR GetUBMapAddr(GM_ADDR workspace, int i = 0)
{
    // Retrieve the corresponding UB mapped address
    // { 48*2* max_size * sizeof(msg); } after MSG
    // UBMap   address arrangement
    //  AIV0   0~256K
    //  AIV1   256K ~ 512K
    //  ...
    ASCENDC_ASSERT((workspace != nullptr), { KERNEL_LOG(KERNEL_ERROR, "workspace can not be nullptr"); });

    auto flatBlockID = get_block_idx() * MAX_BLOCK_AIV_NUM + i;
    auto ptr = reinterpret_cast<__gm__ struct SysWorkspaceDesc*>(workspace);
    return reinterpret_cast<GM_ADDR>(ptr->ubMap[flatBlockID]);
}

__aicore__ inline GM_ADDR GetMatmulIncAddr(GM_ADDR workspace, uint32_t flatBlockID, uint32_t instID)
{
    // Maximum MAX_MATMUL_OBJ Matmul objects can be created
    ASCENDC_ASSERT((instID < MAX_MATMUL_OBJ), {
        KERNEL_LOG(KERNEL_ERROR, "instID i is %d, max matmul num is %d", instID, MAX_MATMUL_OBJ);
    });
    ASCENDC_ASSERT((workspace != nullptr), { KERNEL_LOG(KERNEL_ERROR, "workspace can not be nullptr"); });
    auto ptr = reinterpret_cast<__gm__ struct SysWorkspaceDesc*>(workspace);
    return reinterpret_cast<GM_ADDR>(&(ptr->cntMsg[flatBlockID][instID]));
}

__aicore__ inline GM_ADDR GetUBAvailableAddr(GM_ADDR workspace, uint32_t i = 0)
{
    ASSERT(workspace != nullptr);
    auto flatBlockID = get_block_idx() * MAX_BLOCK_AIV_NUM + i;
    auto ptr = reinterpret_cast<__gm__ struct SysWorkspaceDesc*>(workspace);
    return reinterpret_cast<GM_ADDR>(&(ptr->ubMsg[flatBlockID]));
}

__aicore__ inline __gm__ KfcMsg* AllocMessageImpl(
    __gm__ KfcMsg*& msgSendHead, uint8_t& msgSendPos, __gm__ KfcMsg*& msgSendStart)
{
    auto msg = msgSendHead;
    ASCENDC_ASSERT((msgSendHead != nullptr), { KERNEL_LOG(KERNEL_ERROR, "msgSendHead can not be nullptr"); });
    ASCENDC_ASSERT((msgSendStart != nullptr), { KERNEL_LOG(KERNEL_ERROR, "msgSendStart can not be nullptr"); });
    if constexpr (KFC_APPLY_MSG) {
        dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        while (static_cast<bool>(KfcMsgGetState(msg->head))) {
            Barrier();
            dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
            Barrier();
        }
    }
    ASCENDC_ASSERT((!KfcMsgGetState(msg->head)), {
        KERNEL_LOG(KERNEL_ERROR, "message state is %u, which should be 0", KfcMsgGetState(msg->head));
    });
    msgSendPos++;
    if (msgSendPos >= MAX_MSG_COUNT) {
        msgSendPos = 0;
        msgSendHead = msgSendStart;
    } else {
        msgSendHead++;
    }
    return msg;
}

__aicore__ inline __gm__ KfcMsg* RcvMessageImpl(
    __gm__ KfcMsg*& msgRcvHead, uint8_t& msgRcvPos, __gm__ KfcMsg*& msgRcvStart)
{
    // The size of a single cacheline is 64 bytes.
    // only a single cacheline needs to be updated because runtime can ensure message alignment
    ASCENDC_ASSERT((msgRcvHead != nullptr), { KERNEL_LOG(KERNEL_ERROR, "msgRcvHead can not be nullptr"); });
    ASCENDC_ASSERT((msgRcvStart != nullptr), { KERNEL_LOG(KERNEL_ERROR, "msgRcvStart can not be nullptr"); });
    ASCENDC_ASSERT((msgRcvPos >= 0 && msgRcvPos <= MAX_MSG_COUNT), {
        KERNEL_LOG(KERNEL_ERROR, "msgRcvPos is %d, which should be in range of [0, %d]", msgRcvPos, MAX_MSG_COUNT);
    });
    __gm__ KfcMsg* msg = msgRcvHead;
    Barrier();
    dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
    Barrier();

    dc_preload((__gm__ uint64_t*)msg, int64_t(0));

#ifdef __MSTX_DFX_REPORT__
    MstxCrossRecord record = {
        .addr = reinterpret_cast<uint64_t>(msg),
        .flagId = 0,
        .pipe = pipe_t::PIPE_S,
        .isMore = true,
    };
    __mstx_dfx_report_stub(1, sizeof(MstxCrossRecord), &record);
#endif

    if (!(static_cast<bool>(KfcMsgGetState(msg->head)))) {
        return nullptr;
    }
    msgRcvPos++;
    if (msgRcvPos >= MAX_MSG_COUNT) {
        msgRcvPos = 0;
        msgRcvHead = msgRcvStart;
    } else {
        msgRcvHead++;
    }
    return msg;
}

__aicore__ inline void FreeMessageImpl(__gm__ KfcMsg* msg)
{
    ASCENDC_ASSERT((msg != nullptr), { KERNEL_LOG(KERNEL_ERROR, "msg can not be nullptr"); });
    __asm__ __volatile__("" ::: "memory");
    *(reinterpret_cast<__gm__ uint64_t*>(msg)) = 0;
    Barrier();
    dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
    Barrier();
}

__aicore__ inline void RollBackMsgImpl(__gm__ KfcMsg*& msgRcvHead, uint8_t& msgRcvPos)
{
    if (msgRcvPos == 0) {
        msgRcvPos = MAX_MSG_COUNT;
        msgRcvHead = msgRcvHead + MAX_MSG_COUNT - 1;
    } else {
        msgRcvPos--;
        msgRcvHead--;
    }
}

} // namespace AscendC
#endif // __KFC_COMM_GM_H__
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_GM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_GM_H__
#endif
