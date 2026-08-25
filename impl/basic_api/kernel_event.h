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
 * \file kernel_event.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_event.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_EVENT_H__
#endif

#ifndef ASCENDC_KERNEL_EVENT_IMPL_H
#define ASCENDC_KERNEL_EVENT_IMPL_H

#include "kernel_macros.h"
#include "kernel_log.h"
#include "common_types.h"
#include "impl/utils/common_types.h"
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#include "stub_fun.h"
#endif

namespace AscendC {
using QuePosition = TPosition;

enum class HardEvent : uint8_t {
    // src_dst
    MTE2_MTE1,
    MTE1_MTE2,
    MTE1_M,
    M_MTE1,
    MTE2_V,
    V_MTE2,
    MTE3_V,
    V_MTE3,
    M_V,
    V_M,
    V_V,
    MTE3_MTE1,
    MTE1_MTE3,
    MTE1_V,
    MTE2_M,
    M_MTE2,
    V_MTE1,
    M_FIX,
    FIX_M,
    MTE3_MTE2,
    MTE2_MTE3,
    S_V,
    V_S,
    S_MTE2,
    MTE2_S,
    S_MTE3,
    MTE3_S,
    MTE2_FIX,
    FIX_MTE2,
    FIX_S,
    M_S,
    FIX_MTE3,
    MTE1_FIX,
    FIX_MTE1,
    FIX_FIX,
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003))
    FIX_V,
    V_FIX,
#endif
    MAX,
};

enum class HardEventAic : uint8_t {
    // src_dst
    MTE2_MTE1,
    MTE1_MTE2,
    MTE1_M,
    M_MTE1,
    MTE3_MTE1,
    MTE1_MTE3,
    MTE2_M,
    M_MTE2,
    M_FIX,
    FIX_M,
    MTE3_MTE2,
    MTE2_MTE3,
    S_MTE2,
    MTE2_S,
    S_MTE3,
    MTE3_S,
    MTE2_FIX,
    FIX_MTE2,
    FIX_S,
    M_S,
    FIX_MTE3,
    MTE1_FIX,
    FIX_MTE1,
    FIX_FIX,
    MAX,
};

enum class HardEventAiv : uint8_t {
    // src_dst
    MTE2_V,
    V_MTE2,
    MTE3_V,
    V_MTE3,
    V_V,
    MTE3_MTE2,
    MTE2_MTE3,
    S_V,
    V_S,
    S_MTE2,
    MTE2_S,
    S_MTE3,
    MTE3_S,
    MAX,
};

enum class MemoryT : uint8_t { L1 = 0, L0A, L0B, L0C, UB, BIAS };

enum class MemDsbT : uint8_t { ALL = 0, DDR, UB, SEQ };

#if defined(ASCENDC_CPU_DEBUG) || (defined(__NPU_ARCH__) && (__NPU_ARCH__ != 2201) && (__NPU_ARCH__ != 3510))
constexpr uint8_t EVENT_NUM = static_cast<uint8_t>(HardEvent::MAX);
#else
#ifdef SPLIT_CORE_CUBE
constexpr uint8_t EVENT_NUM = static_cast<uint8_t>(HardEventAic::MAX);
#else
constexpr uint8_t EVENT_NUM = static_cast<uint8_t>(HardEventAiv::MAX);
#endif
#endif

__aicore__ constexpr uint8_t EventToIndexAic(HardEvent evt)
{
    // in v220 aic, only 21 events is useful, so convert evt to index in event pool
    // in other chip version, all events are valid;
    if (evt == HardEvent::MTE2_MTE1) {
        return static_cast<uint8_t>(HardEventAic::MTE2_MTE1);
    } else if (evt == HardEvent::MTE1_MTE2) {
        return static_cast<uint8_t>(HardEventAic::MTE1_MTE2);
    } else if (evt == HardEvent::MTE1_M) {
        return static_cast<uint8_t>(HardEventAic::MTE1_M);
    } else if (evt == HardEvent::M_MTE1) {
        return static_cast<uint8_t>(HardEventAic::M_MTE1);
    } else if (evt == HardEvent::MTE3_MTE1) {
        return static_cast<uint8_t>(HardEventAic::MTE3_MTE1);
    } else if (evt == HardEvent::MTE1_MTE3) {
        return static_cast<uint8_t>(HardEventAic::MTE1_MTE3);
    } else if (evt == HardEvent::MTE2_M) {
        return static_cast<uint8_t>(HardEventAic::MTE2_M);
    } else if (evt == HardEvent::M_MTE2) {
        return static_cast<uint8_t>(HardEventAic::M_MTE2);
    } else if (evt == HardEvent::M_FIX) {
        return static_cast<uint8_t>(HardEventAic::M_FIX);
    } else if (evt == HardEvent::FIX_M) {
        return static_cast<uint8_t>(HardEventAic::FIX_M);
    } else if (evt == HardEvent::MTE3_MTE2) {
        return static_cast<uint8_t>(HardEventAic::MTE3_MTE2);
    } else if (evt == HardEvent::MTE2_MTE3) {
        return static_cast<uint8_t>(HardEventAic::MTE2_MTE3);
    } else if (evt == HardEvent::S_MTE2) {
        return static_cast<uint8_t>(HardEventAic::S_MTE2);
    } else if (evt == HardEvent::MTE2_S) {
        return static_cast<uint8_t>(HardEventAic::MTE2_S);
    } else if (evt == HardEvent::S_MTE3) {
        return static_cast<uint8_t>(HardEventAic::S_MTE3);
    } else if (evt == HardEvent::MTE3_S) {
        return static_cast<uint8_t>(HardEventAic::MTE3_S);
    } else if (evt == HardEvent::MTE2_FIX) {
        return static_cast<uint8_t>(HardEventAic::MTE2_FIX);
    } else if (evt == HardEvent::FIX_MTE2) {
        return static_cast<uint8_t>(HardEventAic::FIX_MTE2);
    } else if (evt == HardEvent::FIX_S) {
        return static_cast<uint8_t>(HardEventAic::FIX_S);
    } else if (evt == HardEvent::M_S) {
        return static_cast<uint8_t>(HardEventAic::M_S);
    } else if (evt == HardEvent::FIX_MTE3) {
        return static_cast<uint8_t>(HardEventAic::FIX_MTE3);
    } else if (evt == HardEvent::FIX_MTE1) {
        return static_cast<uint8_t>(HardEventAic::FIX_MTE1);
    } else if (evt == HardEvent::MTE1_FIX) {
        return static_cast<uint8_t>(HardEventAic::MTE1_FIX);
    } else if (evt == HardEvent::FIX_FIX) {
        return static_cast<uint8_t>(HardEventAic::FIX_FIX);
    } else {
        return static_cast<uint8_t>(HardEventAic::MAX);
    }
}

__aicore__ constexpr uint8_t EventToIndexAiv(HardEvent evt)
{
    // in v220 aiv, only 13 events is useful, so convert evt to index in event pool
    // in other chip version, all events are valid;
    if (evt == HardEvent::MTE2_V) {
        return static_cast<uint8_t>(HardEventAiv::MTE2_V);
    } else if (evt == HardEvent::V_MTE2) {
        return static_cast<uint8_t>(HardEventAiv::V_MTE2);
    } else if (evt == HardEvent::MTE3_V) {
        return static_cast<uint8_t>(HardEventAiv::MTE3_V);
    } else if (evt == HardEvent::V_MTE3) {
        return static_cast<uint8_t>(HardEventAiv::V_MTE3);
    } else if (evt == HardEvent::V_V) {
        return static_cast<uint8_t>(HardEventAiv::V_V);
    } else if (evt == HardEvent::MTE3_MTE2) {
        return static_cast<uint8_t>(HardEventAiv::MTE3_MTE2);
    } else if (evt == HardEvent::MTE2_MTE3) {
        return static_cast<uint8_t>(HardEventAiv::MTE2_MTE3);
    } else if (evt == HardEvent::S_V) {
        return static_cast<uint8_t>(HardEventAiv::S_V);
    } else if (evt == HardEvent::V_S) {
        return static_cast<uint8_t>(HardEventAiv::V_S);
    } else if (evt == HardEvent::S_MTE2) {
        return static_cast<uint8_t>(HardEventAiv::S_MTE2);
    } else if (evt == HardEvent::MTE2_S) {
        return static_cast<uint8_t>(HardEventAiv::MTE2_S);
    } else if (evt == HardEvent::S_MTE3) {
        return static_cast<uint8_t>(HardEventAiv::S_MTE3);
    } else if (evt == HardEvent::MTE3_S) {
        return static_cast<uint8_t>(HardEventAiv::MTE3_S);
    } else {
        return static_cast<uint8_t>(HardEventAiv::MAX);
    }
}

__aicore__ constexpr uint8_t EventToIndex(HardEvent evt)
{
#if defined(ASCENDC_CPU_DEBUG) || \
    defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5102) || ((__NPU_ARCH__ != 2201) && (__NPU_ARCH__ != 3510)))
    return static_cast<uint8_t>(evt);
#elif defined(SPLIT_CORE_CUBE)
    return EventToIndexAic(evt);
#else
    return EventToIndexAiv(evt);
#endif
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 1001) && (__NPU_ARCH__ != 2002)
constexpr int32_t PIPE_NUM = 7;
constexpr pipe_t SUPPORTED_PIPE[PIPE_NUM] = {PIPE_S, PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, PIPE_FIX};
#else
constexpr int32_t PIPE_NUM = 6;
constexpr pipe_t SUPPORTED_PIPE[PIPE_NUM] = {PIPE_S, PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3};
#endif

#if defined(__NPU_ARCH__)
template <pipe_t pipe>
__aicore__ inline constexpr bool IsSplitVectorPipe()
{
    return pipe == PIPE_S || pipe == PIPE_V || pipe == PIPE_MTE2 || pipe == PIPE_MTE3;
}

template <pipe_t pipe>
__aicore__ inline constexpr bool IsSplitCubePipe()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
    return pipe == PIPE_S || pipe == PIPE_MTE1 || pipe == PIPE_MTE2 || pipe == PIPE_MTE3 || pipe == PIPE_M;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    return pipe == PIPE_S || pipe == PIPE_MTE1 || pipe == PIPE_MTE2 || pipe == PIPE_FIX || pipe == PIPE_M;
#else
    return pipe == PIPE_S || pipe == PIPE_MTE1 || pipe == PIPE_MTE2 || pipe == PIPE_MTE3 || pipe == PIPE_FIX ||
           pipe == PIPE_M;
#endif
}

template <pipe_t pipe>
__aicore__ inline void PipeBarrierInternal()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    if constexpr (IsSplitVectorPipe<pipe>() || pipe == PIPE_ALL) {
        if ASCEND_IS_AIV {
            pipe_barrier(pipe);
        }
    }
    if constexpr (IsSplitCubePipe<pipe>() || pipe == PIPE_ALL) {
        if ASCEND_IS_AIC {
            pipe_barrier(pipe);
        }
    }
#else
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    if constexpr (pipe == PIPE_V) {
        return;
    }
#endif
    pipe_barrier(pipe);
#endif
}

#if defined(__ENABLE_SUPER_KERNEL_INNER_CORE_SYNC_CHECK__)
__BLOCK_LOCAL__ __inline__ int32_t g_superKernelSetWaitFlagCountDifference;
#endif

template <pipe_t srcPipe, pipe_t dstPipe>
__aicore__ inline void SetFlagInternal(event_t evt)
{
#if defined(__ENABLE_SUPER_KERNEL_INNER_CORE_SYNC_CHECK__)
    g_superKernelSetWaitFlagCountDifference += 1;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    if constexpr (IsSplitVectorPipe<srcPipe>() && IsSplitVectorPipe<dstPipe>()) {
        if ASCEND_IS_AIV {
            set_flag(srcPipe, dstPipe, evt);
        }
    }
    if constexpr (IsSplitCubePipe<srcPipe>() && IsSplitCubePipe<dstPipe>()) {
        if ASCEND_IS_AIC {
            set_flag(srcPipe, dstPipe, evt);
        }
    }
#else
    set_flag(srcPipe, dstPipe, evt);
#endif
}

template <pipe_t srcPipe, pipe_t dstPipe>
__aicore__ inline void WaitFlagInternal(event_t evt)
{
#if defined(__ENABLE_SUPER_KERNEL_INNER_CORE_SYNC_CHECK__)
    g_superKernelSetWaitFlagCountDifference -= 1;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    (void)evt;
    if constexpr (IsSplitVectorPipe<srcPipe>() && IsSplitVectorPipe<dstPipe>()) {
        if ASCEND_IS_AIV {
            wait_flag(srcPipe, dstPipe, evt);
        }
    }
    if constexpr (IsSplitCubePipe<srcPipe>() && IsSplitCubePipe<dstPipe>()) {
        if ASCEND_IS_AIC {
            wait_flag(srcPipe, dstPipe, evt);
        }
    }
#else
    wait_flag(srcPipe, dstPipe, evt);
#endif
}
#endif

__aicore__ constexpr bool IsSupportedPipe(pipe_t pipe)
{
    for (int i = 0; i < PIPE_NUM; i++) {
        if (pipe == SUPPORTED_PIPE[i]) {
            return true;
        }
    }
    return false;
}

__aicore__ constexpr TPosition GetDefaultPosition(Hardware hard)
{
    if (hard == Hardware::UB) {
        return TPosition::VECCALC;
    } else if (hard == Hardware::L1) {
        return TPosition::A1;
    } else if (hard == Hardware::L0A) {
        return TPosition::A2;
    } else if (hard == Hardware::L0B) {
        return TPosition::B2;
    } else if (hard == Hardware::L0C) {
        return TPosition::CO1;
    } else if (hard == Hardware::BIAS) {
        return TPosition::C2;
    } else if (hard == Hardware::FIXBUF) {
        return TPosition::C2PIPE2GM;
    }
    return TPosition::MAX;
}

__aicore__ constexpr Hardware GetPhyType(TPosition pos)
{
    ASSERT(pos != TPosition::MAX);
    Hardware hard = Hardware::UB;
    if (pos == TPosition::GM) {
        hard = Hardware::GM;
    } else if (pos == TPosition::A1) {
        hard = Hardware::L1;
    } else if (pos == TPosition::A2) {
        hard = Hardware::L0A;
    } else if (pos == TPosition::B1) {
        hard = Hardware::L1;
    } else if (pos == TPosition::B2) {
        hard = Hardware::L0B;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
    } else if (pos == TPosition::C1) {
        hard = Hardware::UB;
    } else if (pos == TPosition::C2) {
        hard = Hardware::L0C;
    } else if (pos == TPosition::CO2) {
        hard = Hardware::UB;
#elif (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3003)
    } else if (pos == TPosition::C1) {
        hard = Hardware::L1;
    } else if (pos == TPosition::C2) {
        hard = Hardware::BIAS;
    } else if (pos == TPosition::CO2) {
        hard = Hardware::GM;
    } else if (pos == TPosition::C2PIPE2GM) {
        hard = Hardware::FIXBUF;
#elif (__NPU_ARCH__ == 3002)
    } else if (pos == TPosition::C1) {
        hard = Hardware::L1;
    } else if (pos == TPosition::C2) {
        hard = Hardware::BIAS;
    } else if (pos == TPosition::C2PIPE2GM) {
        hard = Hardware::FIXBUF;
#elif (__NPU_ARCH__ == 3102)
    } else if (pos == TPosition::C1) {
        hard = Hardware::L1;
    } else if (pos == TPosition::C2) {
        hard = Hardware::BIAS;
#elif (__NPU_ARCH__ == 5102)
    } else if (pos == TPosition::C1) {
        hard = Hardware::L1;
    } else if (pos == TPosition::C2) {
        hard = Hardware::BIAS;
    } else if (pos == TPosition::CO2) {
        hard = Hardware::GM;
    } else if (pos == TPosition::C2PIPE2GM) {
        hard = Hardware::FIXBUF;
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 3113))
    } else if (pos == TPosition::C1) {
        hard = Hardware::L1;
    } else if (pos == TPosition::C2) {
        hard = Hardware::BIAS;
    } else if (pos == TPosition::CO2) {
        hard = Hardware::GM;
    } else if (pos == TPosition::C2PIPE2GM) {
        hard = Hardware::FIXBUF;
#endif
    } else if (pos == TPosition::CO1) {
        hard = Hardware::L0C;
    } else if (pos == TPosition::SHM) {
        hard = Hardware::L1;
    } else if (pos == TPosition::TSCM) {
        hard = Hardware::L1;
    }
    return hard;
}

__aicore__ constexpr TPosition GetPosition(TPosition srcPos, TPosition dstPos)
{
    // unsupported data stream
    ASSERT(!((srcPos == TPosition::CO2) && (dstPos == TPosition::SHM)));
    ASSERT(!((srcPos == TPosition::VECOUT) && (dstPos == TPosition::SHM)));
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
    if (dstPos == TPosition::GM || ((dstPos == TPosition::CO2) && (srcPos == TPosition::CO1))) {
        return srcPos;
    }
#elif defined(__NPU_ARCH__) &&                                                                               \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    if ((dstPos == TPosition::GM) || (dstPos == TPosition::CO2)) {
        return srcPos;
    }
#endif
    return dstPos;
}

__aicore__ constexpr Hardware GetBufferPos(TPosition srcPos, TPosition dstPos)
{
    // unsupported data stream
    ASSERT(!((srcPos == TPosition::CO2) && (dstPos == TPosition::SHM)));
    ASSERT(!((srcPos == TPosition::VECOUT) && (dstPos == TPosition::SHM)));
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
    if ((dstPos == TPosition::GM) || ((dstPos == TPosition::CO2) && (srcPos == TPosition::CO1))) {
        return GetPhyType(srcPos);
    }
#elif defined(__NPU_ARCH__) &&                                                                               \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    if ((dstPos == TPosition::GM) || (dstPos == TPosition::CO2)) {
        return GetPhyType(srcPos);
    }
#endif
    return GetPhyType(dstPos);
}

__aicore__ constexpr TPosition GetBufferLogicPos(TPosition pos, bool isSrc)
{
    ASSERT(pos != TPosition::GM);
    ASSERT(pos != TPosition::VECCALC);
    ASSERT(pos != TPosition::MAX);
    if (pos == TPosition::A1) {
        return isSrc ? TPosition::GM : TPosition::A1;
    } else if (pos == TPosition::B1) {
        return isSrc ? TPosition::GM : TPosition::B1;
    } else if (pos == TPosition::C1) {
        return isSrc ? TPosition::GM : TPosition::C1;
    } else if (pos == TPosition::A2) {
        return isSrc ? TPosition::A1 : TPosition::A2;
    } else if (pos == TPosition::B2) {
        return isSrc ? TPosition::B1 : TPosition::B2;
    } else if (pos == TPosition::C2) {
        return isSrc ? TPosition::C1 : TPosition::C2;
    } else if (pos == TPosition::CO1) {
        return isSrc ? TPosition::CO1 : TPosition::CO2;
    } else if (pos == TPosition::CO2) {
        return isSrc ? TPosition::CO2 : TPosition::GM;
    } else if (pos == TPosition::VECIN) {
        return isSrc ? TPosition::GM : TPosition::VECIN;
    } else if (pos == TPosition::VECOUT) {
        return isSrc ? TPosition::VECOUT : TPosition::GM;
    } else if (pos == TPosition::SPM) {
        return isSrc ? TPosition::VECOUT : TPosition::GM;
    } else if (pos == TPosition::C2PIPE2GM) {
        return isSrc ? TPosition::B1 : TPosition::C2PIPE2GM;
    }
    return TPosition::MAX;
}

__aicore__ constexpr bool IsVecPos(TPosition pos)
{
    return pos == TPosition::VECIN || pos == TPosition::VECOUT || pos == TPosition::VECCALC;
}

__aicore__ constexpr bool IsTQuePositionForVec(TPosition src, TPosition dst)
{
    return (IsVecPos(src) || IsVecPos(dst)) && src != TPosition::TSCM && dst != TPosition::TSCM;
}

template <typename T>
__aicore__ constexpr bool UseBufIdSync()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    return IsTQuePositionForVec(T::srcPosition, T::dstPosition) || T::config.enableStaticEvtId;
#else
    return false;
#endif
}

__aicore__ constexpr bool UseBufIdSync(TPosition src, TPosition dst, bool enableStaticEvtId)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    return IsTQuePositionForVec(src, dst) || enableStaticEvtId;
#else
    return false;
#endif
}

__aicore__ constexpr HardEvent GetQueEvt(
    Hardware src, Hardware dst, bool fwdDirect, bool nd2nz = false, bool nz2nd = false)
{
    (void)(nz2nd);
    ASSERT(
        (src == Hardware::GM) || (src == Hardware::UB) || (src == Hardware::L1) || (src == Hardware::L0A) ||
        (src == Hardware::L0B) || (src == Hardware::L0C));
    ASSERT(src != Hardware::MAX);
    ASSERT(dst != Hardware::MAX);
    if (src == Hardware::GM) { // MTE2
        ASSERT(dst != Hardware::GM);
        ASSERT(dst != Hardware::L0C);
        ASSERT(dst != Hardware::BIAS);
        ASSERT(dst != Hardware::FIXBUF);
        if (dst == Hardware::UB) { // MTE3
            return fwdDirect ? HardEvent::MTE2_V : HardEvent::V_MTE2;
        } else if (dst == Hardware::L1) { // MTE1
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 1001) && (__NPU_ARCH__ != 2002)
            (void)(nd2nz);
#else
            // in v100/v200, nd2nz was simulated with vector intrins, so event changed event to mte3
            if (nd2nz) {
                return fwdDirect ? HardEvent::MTE3_MTE1 : HardEvent::MTE1_MTE3;
            }
#endif
            return fwdDirect ? HardEvent::MTE2_MTE1 : HardEvent::MTE1_MTE2;
        } else if (dst == Hardware::L0A) {
            return fwdDirect ? HardEvent::MTE2_M : HardEvent::M_MTE2;
        } else if (dst == Hardware::L0B) {
            return fwdDirect ? HardEvent::MTE2_M : HardEvent::M_MTE2;
        }
    } else if (src == Hardware::UB) { // MTE3
        ASSERT(dst != Hardware::L0A);
        ASSERT(dst != Hardware::L0B);
        ASSERT(dst != Hardware::BIAS);
        ASSERT(dst != Hardware::FIXBUF);
        if (dst == Hardware::GM) {
            return fwdDirect ? HardEvent::V_MTE3 : HardEvent::MTE3_V;
        } else if (dst == Hardware::L1) { // MTE1
            return fwdDirect ? HardEvent::MTE3_MTE1 : HardEvent::MTE1_MTE3;
        } else if (dst == Hardware::L0C) {
            return fwdDirect ? HardEvent::V_V : HardEvent::MAX; // HardEvent::M_V
        } else if (dst == Hardware::UB) {
            return fwdDirect ? HardEvent::MTE2_MTE3 : HardEvent::MTE3_MTE2;
        }
    } else if (src == Hardware::L1) { // MTE1
        ASSERT(dst != Hardware::GM);
        ASSERT(dst != Hardware::L1);
        ASSERT(dst != Hardware::L0C);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
        ASSERT(dst != Hardware::BIAS);
        ASSERT(dst != Hardware::FIXBUF);
#endif
        if (dst == Hardware::UB) {
            return fwdDirect ? HardEvent::MTE1_V : HardEvent::V_MTE1;
        } else if (dst == Hardware::L0A) {
            return fwdDirect ? HardEvent::MTE1_M : HardEvent::M_MTE1;
        } else if (dst == Hardware::L0B) {
            return fwdDirect ? HardEvent::MTE1_M : HardEvent::M_MTE1;
        } else if (dst == Hardware::FIXBUF) {
            return fwdDirect ? HardEvent::MTE1_FIX : HardEvent::FIX_MTE1;
        } else if (dst == Hardware::BIAS) {
            return fwdDirect ? HardEvent::MTE1_M : HardEvent::M_MTE1;
        }
    } else if (src == Hardware::L0A) {
        ASSERT(dst == Hardware::L0C);
        return fwdDirect ? HardEvent::M_V : HardEvent::V_M;
    } else if (src == Hardware::L0B) {
        ASSERT(dst == Hardware::L0C);
        return fwdDirect ? HardEvent::M_V : HardEvent::V_M;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
    } else if (src == Hardware::L0C) {
        ASSERT(dst == Hardware::UB);
        return fwdDirect ? HardEvent::M_V : HardEvent::V_M;
    }
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    } else if (src == Hardware::L0C) {
        ASSERT(dst == Hardware::GM);
        return fwdDirect ? HardEvent::M_FIX : HardEvent::FIX_M;
    }
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003))
    } else if (src == Hardware::L0C) {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003)
        ASSERT(dst == Hardware::GM || dst == Hardware::UB || dst == Hardware::L1);
        return fwdDirect ? HardEvent::M_FIX : HardEvent::FIX_M;
#else
        ASSERT(dst == Hardware::GM || dst == Hardware::UB);
        return fwdDirect ? HardEvent::M_FIX : HardEvent::FIX_M;
#endif
    }
#elif (__NPU_ARCH__ == 3102)
    } else if (src == Hardware::L0C) {
        ASSERT(dst == Hardware::GM || dst == Hardware::UB);
        return fwdDirect ? HardEvent::M_FIX : HardEvent::FIX_M;
    }
#elif defined(__NPU_ARCH__) && (((__NPU_ARCH__ == 3113)))
    } else if (src == Hardware::L0C) {
        ASSERT(dst == Hardware::GM || dst == Hardware::UB || dst == Hardware::L1);
        return fwdDirect ? HardEvent::M_FIX : HardEvent::FIX_M;
    }
#else
    }
#endif
    return HardEvent::MAX;
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
__aicore__ constexpr HardEvent GetQueEvt(TPosition src, TPosition dst, bool fwdDirect)
{
    if (src == TPosition::GM) { // MTE2
        if (dst == TPosition::A1 || dst == TPosition::A2 || dst == TPosition::B2) {
            return fwdDirect ? HardEvent::MTE2_M : HardEvent::M_MTE2;
        } else if (dst == TPosition::B1 || dst == TPosition::C1) {
            return fwdDirect ? HardEvent::MTE2_MTE1 : HardEvent::MTE1_MTE2;
        }
    } else if (src == TPosition::A1) {
        if (dst == TPosition::A2) {
            return fwdDirect ? HardEvent::MTE2_M : HardEvent::M_MTE2;
        }
    } else if (src == TPosition::B1) {
        if (dst == TPosition::B2) {
            return fwdDirect ? HardEvent::MTE1_M : HardEvent::M_MTE1;
        } else if (dst == TPosition::C2PIPE2GM) {
            return fwdDirect ? HardEvent::MTE1_M : HardEvent::M_MTE1;
        }
    } else if (src == TPosition::C1) {
        if (dst == TPosition::C2) {
            return fwdDirect ? HardEvent::MTE1_M : HardEvent::M_MTE1;
        }
    }
    return GetQueEvt(GetPhyType(src), GetPhyType(dst), fwdDirect, false, false);
}
#endif

using TBufId = uint8_t;

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
#ifdef ASCENDC_CPU_DEBUG
class BufIdTracker {
public:
    struct BufState {
        pipe_t pipe;
        bool mode;
        bool released;
    };

    static BufIdTracker& GetInstance()
    {
        static BufIdTracker tracker;
        return tracker;
    }

    bool AddBufIdEvent(uint8_t bufId, pipe_t pipe, bool mode, bool released);
    bool GetState();
    void Reset();

private:
    BufIdTracker() {}
    bool UpdateState(uint8_t bufId, pipe_t pipe, bool mode, bool released);

    std::map<uint8_t, BufState> bufIdMapAiv_;
    std::map<uint8_t, BufState> bufIdMapAic_;

    bool aivState_ = true;
    bool aicState_ = true;
};
#endif

constexpr TBufId INVALID_TBUFID = (static_cast<TBufId>(-1));
constexpr TBufId MAX_TBUFID = (static_cast<TBufId>(31));
constexpr TBufId MAX_MUTEXID = (static_cast<TBufId>(27));

template <pipe_t pipe, bool mode>
__aicore__ inline void GetBufInternal(uint8_t bufId)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            get_buf(pipe, bufId, mode);
        }
    }
    if constexpr (IsSplitCubePipe<pipe>()) {
        if ASCEND_IS_AIC {
            get_buf(pipe, bufId, mode);
        }
    }
#else
    get_buf(pipe, bufId, mode);
#endif
}

template <pipe_t pipe, bool mode>
__aicore__ inline void RlsBufInternal(uint8_t bufId)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            rls_buf(pipe, bufId, mode);
        }
    }
    if constexpr (IsSplitCubePipe<pipe>()) {
        if ASCEND_IS_AIC {
            rls_buf(pipe, bufId, mode);
        }
    }
#else
    rls_buf(pipe, bufId, mode);
#endif
}

__aicore__ constexpr pipe_t GetPipeByPos(TPosition pos, TPosition altPos)
{
    if (pos == TPosition::GM) {
        return altPos == TPosition::VECIN ? PIPE_MTE2 : PIPE_MTE3;
    } else if (pos == TPosition::VECIN) {
        return altPos == TPosition::GM ? PIPE_V : PIPE_MTE2;
    } else if (pos == TPosition::VECCALC) {
        return PIPE_V;
    } else if (pos == TPosition::VECOUT) {
        return altPos == TPosition::GM ? PIPE_V : PIPE_MTE3;
    }
    return PIPE_V;
}

__aicore__ constexpr pipe_t GetPipe(Hardware src, Hardware dst, bool fwdDirect)
{
    if (src == Hardware::GM) {
        if (dst == Hardware::UB) {
            return fwdDirect ? PIPE_MTE2 : PIPE_V;
        }
        if (dst == Hardware::L1) {
            return fwdDirect ? PIPE_MTE2 : PIPE_MTE1;
        }
    } else if (src == Hardware::UB) {
        if (dst == Hardware::GM) {
            return fwdDirect ? PIPE_V : PIPE_MTE3;
        }
        if (dst == Hardware::L1) {
            return fwdDirect ? PIPE_MTE3 : PIPE_MTE1;
        }
        if (dst == Hardware::UB) {
            return fwdDirect ? PIPE_MTE2 : PIPE_MTE3;
        }
    } else if (src == Hardware::L1) {
        if (dst == Hardware::L0A) {
            return fwdDirect ? PIPE_MTE1 : PIPE_M;
        } else if (dst == Hardware::L0B) {
            return fwdDirect ? PIPE_MTE1 : PIPE_M;
        }
    } else if (src == Hardware::L0C) {
        return fwdDirect ? PIPE_M : PIPE_FIX;
    }
    return PIPE_S;
}

template <pipe_t pipe, bool mode>
__aicore__ inline void GetBuffImpl(uint8_t bufId)
{
    ASCENDC_ASSERT((bufId <= MAX_TBUFID), {
        KERNEL_LOG(KERNEL_ERROR, "current id is %u, max buffer ID is %u", bufId, MAX_TBUFID);
    });
    DEBUG_CODE(BufIdTracker::GetInstance().AddBufIdEvent(bufId, pipe, mode, false));
    GetBufInternal<pipe, mode>(bufId);
}

template <pipe_t pipe, bool mode>
__aicore__ inline void ReleaseBuffImpl(uint8_t bufId)
{
    ASCENDC_ASSERT((bufId <= MAX_TBUFID), {
        KERNEL_LOG(KERNEL_ERROR, "current id is %u, max buffer ID is %u", bufId, MAX_TBUFID);
    });
    DEBUG_CODE(BufIdTracker::GetInstance().AddBufIdEvent(bufId, pipe, mode, true));
    RlsBufInternal<pipe, mode>(bufId);
}

__aicore__ constexpr bool IsUseBufId(Hardware src, Hardware dst)
{
    return (src == Hardware::UB) || (dst == Hardware::UB);
}
#endif

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
template <MemDsbT arg>
__aicore__ inline void DataSyncBarrierImpl()
{
    if constexpr (arg == MemDsbT::UB) {
        if ASCEND_IS_AIC {
            return;
        }
    }
    dsb((mem_dsb_t)arg);
}

template <HardEvent event, MemoryT memT, bool isVirtual>
__aicore__ inline void HSetFlagImpl(int32_t eventID)
{
    ASCENDC_ASSERT((eventID >= 0 && eventID < QUE_MAX_EVENT), {
        KERNEL_LOG(KERNEL_ERROR, "For HSetFlag, eventID %d should be in range [0, %d)", eventID, QUE_MAX_EVENT);
    });
    static_assert(
        ((int32_t)memT >= 0 && memT <= MemoryT::BIAS && memT != MemoryT::UB && memT != MemoryT::L1),
        "For HSetFlag, memT only support L0A, L0B, L0C, BIAS.");

    event_t e = static_cast<event_t>(eventID);

    switch (event) {
        case HardEvent::MTE1_M:
            ASCENDC_ASSERT(
                (memT != MemoryT::L1 && memT != MemoryT::L0C), "memT only support L0A, L0B, BIAS in MTE1_M.");
            hset_flag(PIPE_MTE1, PIPE_M, e, (mem_t)memT, isVirtual);
            break;
        case HardEvent::M_MTE1:
            ASCENDC_ASSERT(
                (memT != MemoryT::L1 && memT != MemoryT::L0C), "memT only support L0A, L0B, BIAS in M_MTE1.");
            hset_flag(PIPE_M, PIPE_MTE1, e, (mem_t)memT, isVirtual);
            break;
        case HardEvent::M_FIX:
            ASCENDC_ASSERT((memT == MemoryT::L0C), "memT only support L0C in M_FIX.");
            hset_flag(PIPE_M, PIPE_FIX, e, (mem_t)memT, isVirtual);
            break;
        case HardEvent::FIX_M:
            ASCENDC_ASSERT((memT == MemoryT::L0C), "memT only support L0C in FIX_M.");
            hset_flag(PIPE_FIX, PIPE_M, e, (mem_t)memT, isVirtual);
            break;
        default:
            ASCENDC_ASSERT((0), KERNEL_LOG(KERNEL_ERROR, "invalid event %d", static_cast<int32_t>(event)););
            break;
    }
}

template <HardEvent event, MemoryT memT, bool isVirtual>
__aicore__ inline void HWaitFlagImpl(int32_t eventID)
{
    ASCENDC_ASSERT((eventID >= 0 && eventID < QUE_MAX_EVENT), {
        KERNEL_LOG(KERNEL_ERROR, "For HWaitFlag, eventID %d should be in range [0, %d)", eventID, QUE_MAX_EVENT);
    });
    static_assert(
        ((int32_t)memT >= 0 && memT <= MemoryT::BIAS && memT != MemoryT::UB && memT != MemoryT::L1),
        "For HWaitFlag, memT only support L0A, L0B, L0C, BIAS.");

    event_t e = static_cast<event_t>(eventID);

    switch (event) {
        case HardEvent::MTE1_M:
            ASCENDC_ASSERT(
                (memT != MemoryT::L1 && memT != MemoryT::L0C), "memT only support L0A, L0B, BIAS in MTE1_M.");
            hwait_flag(PIPE_MTE1, PIPE_M, e, (mem_t)memT, isVirtual);
            break;
        case HardEvent::M_MTE1:
            ASCENDC_ASSERT(
                (memT != MemoryT::L1 && memT != MemoryT::L0C), "memT only support L0A, L0B, BIAS in M_MTE1.");
            hwait_flag(PIPE_M, PIPE_MTE1, e, (mem_t)memT, isVirtual);
            break;
        case HardEvent::M_FIX:
            ASCENDC_ASSERT((memT == MemoryT::L0C), "memT only support L0C in M_FIX.");
            hwait_flag(PIPE_M, PIPE_FIX, e, (mem_t)memT, isVirtual);
            break;
        case HardEvent::FIX_M:
            ASCENDC_ASSERT((memT == MemoryT::L0C), "memT only support L0C in FIX_M.");
            hwait_flag(PIPE_FIX, PIPE_M, e, (mem_t)memT, isVirtual);
            break;
        default:
            ASCENDC_ASSERT((0), KERNEL_LOG(KERNEL_ERROR, "invalid event %d", static_cast<int32_t>(event)););
            break;
    }
}
#endif

template <HardEvent event>
__aicore__ inline void SetFlagImpl(int32_t eventID)
{
    ASCENDC_ASSERT((eventID >= 0 && eventID < QUE_MAX_EVENT), {
        KERNEL_LOG(KERNEL_ERROR, "eventID %d should be in range [0, %d)", eventID, QUE_MAX_EVENT);
    });
    event_t e = static_cast<event_t>(eventID);
    switch (event) {
        case HardEvent::MTE2_MTE1:
            SetFlagInternal<PIPE_MTE2, PIPE_MTE1>(e);
            break;
        case HardEvent::MTE1_MTE2:
            SetFlagInternal<PIPE_MTE1, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE2_MTE3:
            SetFlagInternal<PIPE_MTE2, PIPE_MTE3>(e);
            break;
        case HardEvent::MTE3_MTE2:
            SetFlagInternal<PIPE_MTE3, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE1_M:
            SetFlagInternal<PIPE_MTE1, PIPE_M>(e);
            break;
        case HardEvent::M_MTE1:
            SetFlagInternal<PIPE_M, PIPE_MTE1>(e);
            break;
        case HardEvent::MTE2_V:
            SetFlagInternal<PIPE_MTE2, PIPE_V>(e);
            break;
        case HardEvent::V_MTE2:
            SetFlagInternal<PIPE_V, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE3_V:
            SetFlagInternal<PIPE_MTE3, PIPE_V>(e);
            break;
        case HardEvent::V_MTE3:
            SetFlagInternal<PIPE_V, PIPE_MTE3>(e);
            break;
        case HardEvent::M_V:
            SetFlagInternal<PIPE_M, PIPE_V>(e);
            break;
        case HardEvent::M_S:
            SetFlagInternal<PIPE_M, PIPE_S>(e);
            break;
        case HardEvent::V_M:
            SetFlagInternal<PIPE_V, PIPE_M>(e);
            break;
        case HardEvent::S_V:
            SetFlagInternal<PIPE_S, PIPE_V>(e);
            break;
        case HardEvent::V_S:
            SetFlagInternal<PIPE_V, PIPE_S>(e);
            break;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ != 3002) || (__NPU_ARCH__ == 3003))
        case HardEvent::V_V:
            PipeBarrierInternal<PIPE_V>();
            return;
#endif
        case HardEvent::MTE3_MTE1:
            SetFlagInternal<PIPE_MTE3, PIPE_MTE1>(e);
            break;
        case HardEvent::MTE1_MTE3:
            SetFlagInternal<PIPE_MTE1, PIPE_MTE3>(e);
            break;
        case HardEvent::MTE1_V:
            SetFlagInternal<PIPE_MTE1, PIPE_V>(e);
            break;
        case HardEvent::MTE2_M:
            SetFlagInternal<PIPE_MTE2, PIPE_M>(e);
            break;
        case HardEvent::M_MTE2:
            SetFlagInternal<PIPE_M, PIPE_MTE2>(e);
            break;
        case HardEvent::S_MTE2:
            SetFlagInternal<PIPE_S, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE2_S:
            SetFlagInternal<PIPE_MTE2, PIPE_S>(e);
            break;
        case HardEvent::V_MTE1:
            SetFlagInternal<PIPE_V, PIPE_MTE1>(e);
            break;
        case HardEvent::S_MTE3:
            SetFlagInternal<PIPE_S, PIPE_MTE3>(e);
            break;
        case HardEvent::MTE3_S:
            SetFlagInternal<PIPE_MTE3, PIPE_S>(e);
            break;

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || ((__NPU_ARCH__ == 3113)))
        case HardEvent::M_FIX:
            SetFlagInternal<PIPE_M, PIPE_FIX>(e);
            break;
        case HardEvent::FIX_M:
            SetFlagInternal<PIPE_FIX, PIPE_M>(e);
            break;
        case HardEvent::FIX_MTE3:
            SetFlagInternal<PIPE_FIX, PIPE_MTE3>(e);
            break;
        case HardEvent::FIX_MTE2:
            SetFlagInternal<PIPE_FIX, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE2_FIX:
            SetFlagInternal<PIPE_MTE2, PIPE_FIX>(e);
            break;
        case HardEvent::FIX_S:
            SetFlagInternal<PIPE_FIX, PIPE_S>(e);
            break;
        case HardEvent::MTE1_FIX:
            SetFlagInternal<PIPE_MTE1, PIPE_FIX>(e);
            break;
        case HardEvent::FIX_MTE1:
            SetFlagInternal<PIPE_FIX, PIPE_MTE1>(e);
            break;
        case HardEvent::FIX_FIX:
            PipeBarrierInternal<PIPE_FIX>();
            break;
#endif
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5102))
        case HardEvent::FIX_V:
            SetFlagInternal<PIPE_FIX, PIPE_V>(e);
            break;
        case HardEvent::V_FIX:
            SetFlagInternal<PIPE_V, PIPE_FIX>(e);
            break;
#endif
        case HardEvent::MAX:
            break;
        default:
            ASCENDC_ASSERT((0), { KERNEL_LOG(KERNEL_ERROR, "invalid event %d", static_cast<int32_t>(event)); });
            break;
    }
}

__aicore__ inline void WaitFlagImpl(const HardEvent event, int32_t eventID)
{
    ASCENDC_ASSERT((eventID >= 0 && eventID < QUE_MAX_EVENT), {
        KERNEL_LOG(KERNEL_ERROR, "eventID %d should be in range [0, %d)", eventID, QUE_MAX_EVENT);
    });
#ifdef ASCENDC_CPU_DEBUG
    if ASCEND_IS_AIC {
        if (event == HardEvent::MTE2_V || event == HardEvent::V_MTE2 || event == HardEvent::MTE3_V ||
            event == HardEvent::V_MTE3 || event == HardEvent::V_V || event == HardEvent::S_V ||
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
            event == HardEvent::V_S || event == HardEvent::MTE2_MTE3 || event == HardEvent::MTE3_MTE2 ||
            event == HardEvent::MTE3_S || event == HardEvent::S_MTE3) {
#else
            event == HardEvent::V_S) {
#endif
            return;
        }
    }
    if ASCEND_IS_AIV {
        if ((event == HardEvent::MTE2_MTE1) || (event == HardEvent::MTE1_MTE2) || (event == HardEvent::MTE1_M) ||
            (event == HardEvent::M_MTE1) || (event == HardEvent::M_FIX) || (event == HardEvent::FIX_M)) {
            return;
        }
    }
#endif
    event_t e = static_cast<event_t>(eventID);
    switch (event) {
#ifndef SPLIT_CORE_VEC // CUBE core
        case HardEvent::MTE2_MTE1:
            WaitFlagInternal<PIPE_MTE2, PIPE_MTE1>(e);
            break;
        case HardEvent::MTE1_MTE2:
            WaitFlagInternal<PIPE_MTE1, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE1_M:
            WaitFlagInternal<PIPE_MTE1, PIPE_M>(e);
            break;
        case HardEvent::M_MTE1:
            WaitFlagInternal<PIPE_M, PIPE_MTE1>(e);
            break;
        case HardEvent::MTE3_MTE1:
            WaitFlagInternal<PIPE_MTE3, PIPE_MTE1>(e);
            break;
        case HardEvent::MTE1_MTE3:
            WaitFlagInternal<PIPE_MTE1, PIPE_MTE3>(e);
            break;
        case HardEvent::MTE2_M:
            WaitFlagInternal<PIPE_MTE2, PIPE_M>(e);
            break;
        case HardEvent::M_MTE2:
            WaitFlagInternal<PIPE_M, PIPE_MTE2>(e);
            break;
#endif
#ifndef SPLIT_CORE_CUBE // VECTOR core
        case HardEvent::MTE2_V:
            WaitFlagInternal<PIPE_MTE2, PIPE_V>(e);
            break;
        case HardEvent::V_MTE2:
            WaitFlagInternal<PIPE_V, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE3_V:
            WaitFlagInternal<PIPE_MTE3, PIPE_V>(e);
            break;
        case HardEvent::V_MTE3:
            WaitFlagInternal<PIPE_V, PIPE_MTE3>(e);
            break;
#endif
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ != 2201) && (__NPU_ARCH__ != 3510))
        case HardEvent::M_V:
            WaitFlagInternal<PIPE_M, PIPE_V>(e);
            break;
        case HardEvent::V_M:
            WaitFlagInternal<PIPE_V, PIPE_M>(e);
            break;
        case HardEvent::MTE1_V:
            WaitFlagInternal<PIPE_MTE1, PIPE_V>(e);
            break;
        case HardEvent::V_MTE1:
            WaitFlagInternal<PIPE_V, PIPE_MTE1>(e);
            break;
#endif
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || ((__NPU_ARCH__ == 3113)))
        case HardEvent::FIX_M:
            WaitFlagInternal<PIPE_FIX, PIPE_M>(e);
            break;
        case HardEvent::M_FIX:
            WaitFlagInternal<PIPE_M, PIPE_FIX>(e);
            break;
        case HardEvent::MTE2_FIX:
            WaitFlagInternal<PIPE_MTE2, PIPE_FIX>(e);
            break;
        case HardEvent::FIX_MTE2:
            WaitFlagInternal<PIPE_FIX, PIPE_MTE2>(e);
            break;
        case HardEvent::FIX_S:
            WaitFlagInternal<PIPE_FIX, PIPE_S>(e);
            break;
        case HardEvent::FIX_MTE3:
            WaitFlagInternal<PIPE_FIX, PIPE_MTE3>(e);
            break;
        case HardEvent::MTE1_FIX:
            WaitFlagInternal<PIPE_MTE1, PIPE_FIX>(e);
            break;
        case HardEvent::FIX_MTE1:
            WaitFlagInternal<PIPE_FIX, PIPE_MTE1>(e);
            break;
        case HardEvent::FIX_FIX:
            PipeBarrierInternal<PIPE_FIX>();
            break;
#endif
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5102))
        case HardEvent::FIX_V:
            WaitFlagInternal<PIPE_FIX, PIPE_V>(e);
            break;
        case HardEvent::V_FIX:
            WaitFlagInternal<PIPE_V, PIPE_FIX>(e);
            break;
#endif
        case HardEvent::MTE3_MTE2:
            WaitFlagInternal<PIPE_MTE3, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE2_MTE3:
            WaitFlagInternal<PIPE_MTE2, PIPE_MTE3>(e);
            break;
        case HardEvent::S_MTE2:
            WaitFlagInternal<PIPE_S, PIPE_MTE2>(e);
            break;
        case HardEvent::MTE2_S:
            WaitFlagInternal<PIPE_MTE2, PIPE_S>(e);
            break;
        case HardEvent::S_MTE3:
            WaitFlagInternal<PIPE_S, PIPE_MTE3>(e);
            break;
        case HardEvent::MTE3_S:
            WaitFlagInternal<PIPE_MTE3, PIPE_S>(e);
            break;
        case HardEvent::M_S:
            WaitFlagInternal<PIPE_M, PIPE_S>(e);
            break;
        case HardEvent::S_V:
            WaitFlagInternal<PIPE_S, PIPE_V>(e);
            break;
        case HardEvent::V_S:
            WaitFlagInternal<PIPE_V, PIPE_S>(e);
            break;
        case HardEvent::V_V:
            return;
        case HardEvent::MAX:
            break;
        default:
            break;
    }
    return;
}
} // namespace AscendC

#endif // ASCENDC_KERNEL_EVENT_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_EVENT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_EVENT_H__
#endif
