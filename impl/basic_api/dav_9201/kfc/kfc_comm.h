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
 * \file kfc_comm.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kfc/kfc_comm.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_H__
#endif
#ifndef __KERNEL_KFC_COMM_H__
#define __KERNEL_KFC_COMM_H__

#include "../../../../include/basic_api/kernel_common.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../kernel_operator_common_impl.h"
#include "kfc_log.h"

using MEM_ADDR = SSBUF_ADDR;
#define MSG_POS __ssbuf__

namespace AscendC {
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
    MMFUN_SET_HF32,
    MMFUN_SET_USER_DEF_INFO,
    MMFUN_ITERATE_BATCH_ALL,
    MMFUN_ITERATE_N_BATCH_ALL,
    MMFUN_END,
    MCG_SYNC = 0xfc00,
    SERVICE_QUIT = 0xfd00,
    SERVICE_BALANCE = 0xfe00,
    SERVICE_ID_NONE = 0xff00
};

enum class MSG_STATE : uint8_t {
    STATE_INVALID,
    STATE_SET,
};

enum class VEC_WAIT_INTRA_Enum : uint16_t {
    UB_L1_L1_L0AB = 0,
    WAIT_FIXP, // fixp比较特殊，有多条通路
    MAX
};

enum class CUBE_WAIT_INTRA_Enum : uint16_t {
    L1_L0AB_UB_L1 = 0,
    GM_L1_UB_GM,
    WAIT_FIXP, // fixp比较特殊，有多条通路
    MAX
};
#ifdef __MIX_CORE_AIC_RATION__
constexpr int32_t MIX_NUM = __MIX_CORE_AIC_RATION__;
#elif defined(__DAV_310R6__) || (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201 || __NPU_ARCH__ == 9202))
constexpr int32_t MIX_NUM = 1; // solomon 1:1
#else
constexpr int32_t MIX_NUM = 2; // david 1:2
#endif
constexpr int MAX_MSG_MASK = 3;
constexpr int MAX_MSG_COUNT_Arch3510 =
    (1 << MAX_MSG_MASK); // 2KB支持16个消息，每个消息大小128B，给两个v用，每个V可分得8个消息
constexpr int BIDIRECTION_NUM = 1; // 单向
constexpr int MAX_MATMUL_OBJ = 4;
constexpr uint64_t INC_PROCESS_CHECK = 14;
constexpr uint8_t VALID_ADDR_BITS_NUM = 56;
constexpr uint16_t KFC_INTRA_MODE = 4;
constexpr uint16_t KFC_SYNC_ID = 15;
struct MatmulConfigParams {
    // offset->8
    uint32_t enAtomic : 2;
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
    uint32_t setOrgShape : 1;
    uint32_t quantMode : 2;
    uint32_t userInfoType : 1;
    uint32_t hasSetWorkspace : 1;

    uint32_t userCustomData; // 8B pad

    uint64_t aAddr;
    uint64_t bAddr;
    uint64_t cAddr;
    uint32_t orgM; // setOrgShape
    uint32_t orgN;
    uint32_t orgKa;
    uint32_t orgKb;
    uint32_t orgKc;
    uint32_t singleM; // tail
    uint32_t singleN;
    uint32_t singleK;
    uint64_t biasAddr;    // 56 bytes, but access to the maximum message size (64 bytes)
    uint64_t quantAddr;   // 64 bytes
    uint64_t quantScalar; // 80 bytes
    uint32_t quantSize;
    uint32_t batchA;
    uint32_t batchB;
    uint32_t matrixStrideA;
    uint32_t matrixStrideB;
    uint32_t matrixStrideC;
    uint32_t batchLoop;
    uint32_t reserved2; // 120 bytes,pad
};

struct MatmulUserDefInfo {
    uint64_t tilingPtr;
};

constexpr uint16_t KFC_MSG_BYTE_OFFSET = 16;
constexpr uint16_t KFC_MSG_BYTE_OFFSET_8 = 8;
// AIV->AIC, set the instance ID
__aicore__ inline uint16_t KfcMsgGetInstID(uint32_t flag) { return flag & 0x000000ff; }
__aicore__ inline KFC_Enum KfcMsgGetFunID(uint32_t flag)
{
    return static_cast<KFC_Enum>((flag & 0xffff0000) >> KFC_MSG_BYTE_OFFSET);
}
__aicore__ inline uint32_t KfcMsgGetState(uint32_t flag) { return (flag & 0x00008000); }
__aicore__ inline uint32_t KfcMsgMakeFlag(KFC_Enum funID, uint16_t instID, uint16_t isSameAB = 0)
{
    return (
        ((static_cast<uint16_t>(funID) << KFC_MSG_BYTE_OFFSET) + 0x8000) + (isSameAB << KFC_MSG_BYTE_OFFSET_8) +
        (instID));
}
__aicore__ inline uint32_t KfcMsgGetIsSameAB(uint32_t flag) { return (flag & 0x00000100); }

// Currently, the maximum message size is 64 bytes, which is the same as the size of a CacheLine.
struct KfcMsg {
    volatile uint32_t head = 0;
    uint32_t userCustomData;
    union {
        uint32_t buffer[30]; // 用于自定义消息，进行格式化
        MatmulConfigParams body;
        MatmulUserDefInfo userDefInfo;
    };
};

struct MsgMatmulL1Addr {
    volatile uint32_t valid : 1;
    uint32_t l1aAddr;
    uint32_t l1bAddr;
    uint32_t l1biasAddr;
    uint32_t l1aScaleAddr;
    uint32_t l1bScaleAddr;
    uint32_t res0;
    uint32_t res1;
};

struct TilingInfo {
    volatile uint32_t valid = 0;
    uint32_t res;
    TCubeTiling tCubeTiling; // tiling的GM地址
};

struct SsbufWorkspaceDesc {
    KfcMsg kfcMsg[MIX_NUM * BIDIRECTION_NUM * MAX_MSG_COUNT_Arch3510];
    MsgMatmulL1Addr matmulL1AddrMsg[MIX_NUM * MAX_MSG_COUNT_Arch3510];
    TilingInfo tilingInfo[MIX_NUM];
};

__aicore__ inline MEM_ADDR GetTilingAddr(int subblockID)
{
#if ASCENDC_CPU_DEBUG
    auto ptr = reinterpret_cast<__ssbuf__ struct SsbufWorkspaceDesc*>(ConstDefiner::Instance().cpuSSbuf);
#else
    auto ptr = reinterpret_cast<__ssbuf__ struct SsbufWorkspaceDesc*>(0);
#endif
    return reinterpret_cast<MEM_ADDR>(&ptr->tilingInfo[subblockID]);
}

__aicore__ inline MEM_ADDR GetMsgHead(int subblockID)
{
    ASCENDC_ASSERT((subblockID >= 0 && subblockID < MIX_NUM), {
        KERNEL_LOG(KERNEL_ERROR, "input i is %d, which should be in range [0, %d)", subblockID, MIX_NUM);
    });
#if ASCENDC_CPU_DEBUG
    auto ptr = reinterpret_cast<__ssbuf__ struct SsbufWorkspaceDesc*>(ConstDefiner::Instance().cpuSSbuf);
#else
    auto ptr = reinterpret_cast<__ssbuf__ struct SsbufWorkspaceDesc*>(0);
#endif
    return reinterpret_cast<MEM_ADDR>(&ptr->kfcMsg[subblockID * BIDIRECTION_NUM * MAX_MSG_COUNT_Arch3510]);
}

__aicore__ inline MEM_ADDR GetMatmulL1AddrMsg(int subblockID, uint16_t instID)
{
    ASCENDC_ASSERT((subblockID >= 0 && subblockID < MIX_NUM), {
        KERNEL_LOG(KERNEL_ERROR, "input i is %d, which should be in range [0, %d)", subblockID, MIX_NUM);
    });
    ASCENDC_ASSERT((instID < MAX_MATMUL_OBJ), {
        KERNEL_LOG(KERNEL_ERROR, "input instID is %d, which should be in range [0, %d)", subblockID, MAX_MATMUL_OBJ);
    });
#if ASCENDC_CPU_DEBUG
    auto ptr = reinterpret_cast<__ssbuf__ struct SsbufWorkspaceDesc*>(ConstDefiner::Instance().cpuSSbuf);
#else
    auto ptr = reinterpret_cast<__ssbuf__ struct SsbufWorkspaceDesc*>(0);
#endif
    return reinterpret_cast<MEM_ADDR>(&ptr->matmulL1AddrMsg[subblockID * MAX_MSG_COUNT_Arch3510 + instID]);
}

__aicore__ inline void ClearSSbufImpl()
{
#if ASCENDC_CPU_DEBUG
    auto ptr = reinterpret_cast<__ssbuf__ uint32_t*>(ConstDefiner::Instance().cpuSSbuf);
#else
    auto ptr = reinterpret_cast<__ssbuf__ uint32_t*>(0);
#endif
    constexpr uint32_t kfcSSbufSize =
        MIX_NUM * BIDIRECTION_NUM * MAX_MSG_COUNT_Arch3510 * sizeof(KfcMsg) / sizeof(uint32_t);
    constexpr uint32_t l1MsgSize = 16 * sizeof(MsgMatmulL1Addr) / sizeof(uint32_t);
    constexpr uint32_t tilingSize = sizeof(TilingInfo) / sizeof(uint32_t);
#pragma unroll
    for (int i = 0; i < MAX_MSG_COUNT_Arch3510 * MIX_NUM; i++) {
        *(ptr + i * sizeof(KfcMsg) / sizeof(uint32_t)) = 0;
        *(ptr + kfcSSbufSize + i * sizeof(MsgMatmulL1Addr) / sizeof(uint32_t)) = 0;
    }
    *(ptr + kfcSSbufSize + l1MsgSize) = 0;
    *(ptr + kfcSSbufSize + l1MsgSize + tilingSize) = 0;
    return;
}
} // namespace AscendC
#endif // __KERNEL_KFC_COMM_H__
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_COMM_H__
#endif
