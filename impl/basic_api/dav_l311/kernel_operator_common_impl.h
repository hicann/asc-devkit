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
 * \file kernel_operator_common_impl.h
 * \brief AscendC l311 support common api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l311/kernel_operator_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_COMMON_IMPL_H
#define ASCENDC_MODULE_OPERATOR_COMMON_IMPL_H
#include "../../../include/basic_api/kernel_struct_mm.h"
#include "../utils/kernel_utils_mode.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../kernel_utils.h"
#if defined(__DAV_VEC__) && (__NPU_ARCH__ == 3113)
using namespace __cce_simd;
#endif
#define GetLoopBoundB8(x) (((x) - 1) / VECTOR_REG_WIDTH)
#define GetLoopBoundB16(x) (((x) - 1) / (VECTOR_REG_WIDTH / B16_BYTE_SIZE))
#define GetLoopBoundB32(x) (((x) - 1) / (VECTOR_REG_WIDTH / B32_BYTE_SIZE))

namespace AscendC {

template <typename T>
struct TypeGet;
template <>
struct TypeGet<uint32_t> {
    using T = vector_u32;
};
template <>
struct TypeGet<int32_t> {
    using T = vector_s32;
};
template <>
struct TypeGet<float> {
    using T = vector_f32;
};
template <>
struct TypeGet<uint16_t> {
    using T = vector_u16;
};
template <>
struct TypeGet<half> {
    using T = vector_f16;
};
template <>
struct TypeGet<int16_t> {
    using T = vector_s16;
};
template <>
struct TypeGet<uint8_t> {
    using T = vector_u8;
};
template <>
struct TypeGet<int8_t> {
    using T = vector_s8;
};

template <>
struct TypeGet<bool> {
    using T = vector_s8;
};

template <>
struct TypeGet<int4x2_t> {
    using T = vector_s4x2;
};

using MaskReg = vector_bool;
using UnalignReg = vector_align;
using AddrReg = vector_address;

#define LocalMem __ubuf__

template <typename T>
struct RegTensor {
    __aicore__ inline RegTensor(){};
    using RegType = typename TypeGet<T>::T;
    RegType reg;

    __aicore__ inline operator RegType&() { return reg; }
    __aicore__ void Print() const;
};

template <class T>
__aicore__ inline void LocalMemBar(T mem_type);

enum class PPMode { UNKNOWN = -1, ZERO, ONE, TWO, THREE };

enum class PartMode { UNKNOWN = -1, EVEN, ODD };

enum class SatMode { UNKNOWN = -1, NO_SAT, SAT };

enum class Rnd { NO_RND_VALUE, RND_VALUE };

enum class StoreMode { NOSTORED, STORED };

enum class BinLiteral {
    BIN_0,
    BIN_1,
    BIN_2,
    BIN_3,
};

enum class Order { INC_ORDER_VALUE, DEC_ORDER_VALUE };

enum class Mode { UNKNOWN, MERGING, ZEROING };

enum class Pos { LOWEST, HIGHEST };
enum class HiloPart { Lower, Higher };
enum class PostLiteral { POST_MODE_NORAML, POST_MODE_UPDATE };
enum class Pat { ALL, VL1, VL2, VL3, VL4, VL8, VL16, VL32, VL64, VL128, M3, M4, H, Q, ALLF = 15 };
enum class Dist {
    DIST_NORM, // vld, pld, pst
    DIST_BRC_B8,
    DIST_BRC_B16,
    DIST_BRC_B32,
    DIST_US_B8 = 6,
    DIST_US_B16,
    DIST_DS_B8,
    DIST_DS_B16,
    DIST_BDINTLV,
    DIST_DINTLV_B8,
    DIST_DINTLV_B16,
    DIST_UNPK_B8,
    DIST_UNPK_B16,
    DIST_BLK,
    DIST_E2B_B16,
    DIST_E2B_B32,
    DIST_UNPK_B32,
    DIST_DINTLV_B32,
    DIST_UNPK4_B8,
    DIST_SPLT4CHN_B8,
    DIST_SPLT2CHN_B8,
    DIST_SPLT2CHN_B16,
    DIST_US = 1, // pld
    DIST_DS = 2, // pld
    DIST_PK = 1  // pst
};
enum class DistVST {
    DIST_NORM_B8,
    DIST_NORM_B16,
    DIST_NORM_B32,
    DIST_ONEPT_B8,
    DIST_ONEPT_B16,
    DIST_ONEPT_B32,
    DIST_PK_B16,
    DIST_PK_B32,
    DIST_INTLV_B8,
    DIST_INTLV_B16,
    DIST_PK_B64,
    DIST_INTLV_B32,
    DIST_PK4_B32,
    DIST_MRG4CHN_B8,
    DIST_MRG2CHN_B8,
    DIST_MRG2CHN_B16,
    DIST_NORM
};

template <typename T, DistVST dist>
__aicore__ inline constexpr DistVST GetDistVst()
{
    if constexpr (dist == DistVST::DIST_NORM) {
        if constexpr (sizeof(T) == 1) {
            return DistVST::DIST_NORM_B8;
        } else if constexpr (sizeof(T) == 2) {
            return DistVST::DIST_NORM_B16;
        } else if constexpr (sizeof(T) == 4) {
            return DistVST::DIST_NORM_B32;
        }
    }
    return dist;
}

__aicore__ inline void SetSysWorkspace(GM_ADDR workspace)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASSERT((workspace != nullptr) && "workspace can not be nullptr");
#else
    if (g_sysWorkspaceReserved == nullptr) {
        g_sysWorkspaceReserved = workspace;
    }
#endif
}

__aicore__ inline void SetSysWorkspaceForce(GM_ADDR workspace)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASSERT((workspace != nullptr) && "workspace can not be nullptr");
#else
    g_sysWorkspaceReserved = workspace;
#endif
}

__aicore__ inline GM_ADDR GetUserWorkspace(GM_ADDR workspace)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASSERT((workspace != nullptr) && "workspace can not be nullptr");
    return workspace;
#else
    (void)(workspace);
    // reserved 0 Bytes
    return g_sysWorkspaceReserved;
#endif
}

template <atomic_type_t type, atomic_op_t op>
__aicore__ inline void SetStoreAtomicConfigImpl()
{
    ASCENDC_ASSERT((false), "SetStoreAtomicConfig is not supported!");
}

__aicore__ inline int64_t GetStoreAtomicConfigImpl()
{
    ASCENDC_ASSERT((false), "GetStoreAtomicConfig is not supported!");
    return 0;
}

__aicore__ inline void GetStoreAtomicConfigImpl(uint16_t& atomicType, uint16_t& atomicOp)
{
    ASCENDC_ASSERT((false), "GetStoreAtomicConfig is not supported!");
}

__aicore__ inline void CheckLocalMemoryIAImpl(const CheckLocalMemoryIAParam& checkParams)
{
    ASCENDC_ASSERT((false), "enableBit is not supported!");
}

// api

template <RoundMode rndMode>
struct RndConverter {
    static constexpr decltype(auto) value = ROUND_H;
};

template <>
struct RndConverter<RoundMode::CAST_RINT> {
    static constexpr decltype(auto) value = ROUND_R;
};

template <>
struct RndConverter<RoundMode::CAST_ROUND> {
    static constexpr decltype(auto) value = ROUND_A;
};

template <>
struct RndConverter<RoundMode::CAST_FLOOR> {
    static constexpr decltype(auto) value = ROUND_F;
};

template <>
struct RndConverter<RoundMode::CAST_CEIL> {
    static constexpr decltype(auto) value = ROUND_C;
};

template <>
struct RndConverter<RoundMode::CAST_TRUNC> {
    static constexpr decltype(auto) value = ROUND_Z;
};

template <>
struct RndConverter<RoundMode::CAST_ODD> {
    static constexpr decltype(auto) value = ROUND_O;
};

template <Mode mode>
__aicore__ inline constexpr auto GetMaskMergeMode()
{
// To avoid naming conflicts of mode struct in cpu debug.
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return std::integral_constant<::CpuMode, static_cast<::CpuMode>(mode)>();
#else
    return std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
#endif
}

// with #rnd, #sat, #part
// vcvt_fi f322s64/f322s16/f162s8/f162u8/bf162s32
// vcvt_ff f322f16/f322bf16/f162HiF8
template <typename T, typename U, RoundMode roundMode, Mode mode, SatMode satMode, PartMode partMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) rnd = RndConverter<roundMode>::value;
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) sat =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr decltype(auto) part = std::integral_constant<::Part, static_cast<::Part>(partMode)>();
    vcvt(dstReg, srcReg, mask, rnd, sat, part, md);
}

// with #rnd, #sat
// vcvt_fi f322s32/f162s16
template <typename T, typename U, RoundMode roundMode, Mode mode, SatMode satMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) rnd = RndConverter<roundMode>::value;
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) sat =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    vcvt(dstReg, srcReg, mask, rnd, sat, md);
}

// with #rnd, #sat, #pp
// vcvt_fi f162s4
// vcvt_ff f322e4m3/f322e5m2/f322HiF8
template <typename T, typename U, RoundMode roundMode, Mode mode, SatMode satMode, PPMode ppMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) rnd = RndConverter<roundMode>::value;
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) sat =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr decltype(auto) pp = std::integral_constant<::Part_T, static_cast<::Part_T>(ppMode)>();
    vcvt(dstReg, srcReg, mask, rnd, sat, pp, md);
}

// with #rnd, #part
// vcvt_fi f162s32
// vcvt_if s642f32
template <typename T, typename U, RoundMode roundMode, Mode mode, PartMode partMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) rnd = RndConverter<roundMode>::value;
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) part = std::integral_constant<::Part, static_cast<::Part>(partMode)>();
    vcvt(dstReg, srcReg, mask, rnd, part, md);
}

// with #sat, #part
// vcvt_ii u162u8/s162u8/u322u16/u322s16/s322u16/s322s16/s642s32
template <typename T, typename U, Mode mode, SatMode satMode, PartMode partMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) sat =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr decltype(auto) part = std::integral_constant<::Part, static_cast<::Part>(partMode)>();
    vcvt(dstReg, srcReg, mask, sat, part, md);
}

// with #sat, #pp
// vcvt_ii s162s4/u322u8/s322u8
// vcvt_ii u322u8/s322u8
template <typename T, typename U, Mode mode, SatMode satMode, PPMode ppMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) sat =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr decltype(auto) pp = std::integral_constant<::Part_T, static_cast<::Part_T>(ppMode)>();
    vcvt(dstReg, srcReg, mask, sat, pp, md);
}

// with #part
// vcvt_ff f162f32/bf162f32/HiF82f16
// vcvt_if u82f16/s82f16/s162f32
// vcvt_ii u82u16/s82s16/u162u32/s162u32/s162s32/s322s64
template <typename T, typename U, PartMode partMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg)
{
    constexpr decltype(auto) part = std::integral_constant<::Part, static_cast<::Part>(partMode)>();
    vcvt(dstReg, srcReg, part);
}

template <typename T, typename U, Mode mode, PartMode partMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) part = std::integral_constant<::Part, static_cast<::Part>(partMode)>();
    vcvt(dstReg, srcReg, mask, part, md);
}

// with #pp
// vcvt_ff HiF82f32/e4m32f32/e5m22f32
// vcvt_if s42f16
// vcvt_ii s42s16/u82u32/s82s32
template <typename T, typename U, PPMode ppMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg)
{
    constexpr decltype(auto) pp = std::integral_constant<::Part_T, static_cast<::Part_T>(ppMode)>();
    vcvt(dstReg, srcReg, pp);
}

template <typename T, typename U, Mode mode, PPMode ppMode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    constexpr decltype(auto) pp = std::integral_constant<::Part_T, static_cast<::Part_T>(ppMode)>();
    if constexpr (IsSameType<T, half>::value && IsSameType<U, int8_t>::value) {
        vcvt_s42f16(dstReg, srcReg, mask, pp, md);
    } else {
        vcvt(dstReg, srcReg, mask, pp, md);
    }
}

// with #rnd
// vcvt_if s162f16/s322f32
template <typename T, typename U, RoundMode roundMode, Mode mode>
__aicore__ inline void Cast(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) rnd = RndConverter<roundMode>::value;
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    vcvt(dstReg, srcReg, mask, rnd, md);
}

// truncate
template <typename T, RoundMode roundMode, Mode mode = Mode::ZEROING>
__aicore__ inline void Truncate(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr decltype(auto) rnd = RndConverter<roundMode>::value;
    constexpr decltype(auto) md = std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
    vtrc(dstReg, srcReg, rnd, mask, md);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Abs(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vabs(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Relu(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vrelu(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Exp(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vexp(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Sqrt(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vsqrt(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Rsqrt(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vrsqrt(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Rec(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vrec(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Ln(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vln(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Neg(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vneg(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Not(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vnot(dstReg, srcReg, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void CountBit(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vbcnt(dstReg, srcReg, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void CountLeadingSignBits(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcls(dstReg, srcReg, mask, modeValue);
}
template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Axpy(RegTensor<T>& dstReg, RegTensor<T> srcReg, const T scalar, const MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vaxpy(dstReg, srcReg, scalar, mask, modeValue);
}

template <typename T, StoreMode mode>
__aicore__ inline void Squeeze(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto modeValue = std::integral_constant<::StoreMode, static_cast<::StoreMode>(mode)>();
    vsqz(dstReg, srcReg, mask, modeValue);
}

template <typename T>
__aicore__ inline void UnSqueeze(RegTensor<T>& dstReg, MaskReg& mask)
{
    vusqz(dstReg, mask);
}
template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void ReduceSum(RegTensor<T>& dstReg, RegTensor<U> srcReg0, MaskReg mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcadd(dstReg, srcReg0, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void ReduceMax(RegTensor<T>& dstReg, RegTensor<T> srcReg0, MaskReg mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcmax(dstReg, srcReg0, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void ReduceMin(RegTensor<T>& dstReg, RegTensor<T> srcReg0, MaskReg mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcmin(dstReg, srcReg0, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void ReduceMax(RegTensor<T>& dstReg, MaskReg& dstReg1, RegTensor<T> srcReg0, MaskReg pReg)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcbmax(dstReg, dstReg1, srcReg0, pReg, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void ReduceMin(RegTensor<T>& dstReg, MaskReg& dstReg1, RegTensor<T> srcReg0, MaskReg pReg)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcbmin(dstReg, dstReg1, srcReg0, pReg, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void BlockReduceSum(RegTensor<T>& dstReg, RegTensor<T> srcReg0, MaskReg pReg)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgadd(dstReg, srcReg0, pReg, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void BlockReduceMax(RegTensor<T>& dstReg, RegTensor<T> srcReg0, MaskReg pReg)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgmax(dstReg, srcReg0, pReg, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void BlockReduceMin(RegTensor<T>& dstReg, RegTensor<T> srcReg0, MaskReg pReg)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgmin(dstReg, srcReg0, pReg, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void PairReduceSum(RegTensor<T>& dstReg, RegTensor<T> srcReg0, MaskReg pReg)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcpadd(dstReg, srcReg0, pReg, modeValue);
}

template <typename T, typename U, BinLiteral bin>
__aicore__ inline void Dhist(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr auto binValue = std::integral_constant<::Bin, static_cast<::Bin>(bin)>();
    dhistv2(dstReg, srcReg, mask, binValue);
}

template <typename T, typename U, BinLiteral bin>
__aicore__ inline void Chist(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr auto binValue = std::integral_constant<::Bin, static_cast<::Bin>(bin)>();
    chistv2(dstReg, srcReg, mask, binValue);
}
template <typename T, typename U>
__aicore__ inline void Duplicate(RegTensor<T>& dstReg, U scalar)
{
    vbr(dstReg, (T)scalar);
}

template <typename T, Mode mode = Mode::ZEROING, typename U>
__aicore__ inline void Duplicate(RegTensor<T>& dstReg, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vdup(dstReg, (T)scalar, mask, modeValue);
}

template <typename T, Pos pos = Pos::LOWEST, Mode mode = Mode::ZEROING>
__aicore__ inline void Duplicate(RegTensor<T>& dstReg, RegTensor<T>& srcReg, MaskReg& mask)
{
    constexpr auto posValue = std::integral_constant<::Pos, static_cast<::Pos>(pos)>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vdup(dstReg, srcReg, mask, posValue, modeValue);
}

template <typename T>
__simd_callee__ inline void Interleave(
    RegTensor<T>& dstReg0, RegTensor<T>& dstReg1, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1)
{
    vintlv(dstReg0, dstReg1, srcReg0, srcReg1);
}

template <typename T>
__simd_callee__ inline void DeInterleave(
    RegTensor<T>& dstReg0, RegTensor<T>& dstReg1, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1)
{
    vdintlv(dstReg0, dstReg1, srcReg0, srcReg1);
}
template <typename T, Order order = Order::INC_ORDER_VALUE, typename U>
__aicore__ inline void CreateVecIndex(RegTensor<T>& dstReg, U scalar)
{
    constexpr auto orderMode = std::integral_constant<::Order, static_cast<::Order>(order)>();
    vci(dstReg, (T)scalar, orderMode);
}

template <typename T, typename U>
__aicore__ inline void CreateVecIndexWithPattern(RegTensor<T>& dstReg, U scalar)
{
    vcp(dstReg, (T)scalar);
}

template <typename T, CMPMODE mode = CMPMODE::EQ>
__simd_callee__ inline void Compare(MaskReg& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    if constexpr (mode == CMPMODE::EQ) {
        vcmp_eq(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::NE) {
        vcmp_ne(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::GT) {
        vcmp_gt(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::GE) {
        vcmp_ge(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::LT) {
        vcmp_lt(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::LE) {
        vcmp_le(dstReg, srcReg0, srcReg1, mask);
    }
}

template <typename T, CMPMODE mode = CMPMODE::EQ>
__simd_callee__ inline void CompareScalar(MaskReg& dstReg, RegTensor<T>& srcReg0, T scalar, MaskReg& mask)
{
    if constexpr (mode == CMPMODE::EQ) {
        vcmps_eq(dstReg, srcReg0, scalar, mask);
    } else if constexpr (mode == CMPMODE::NE) {
        vcmps_ne(dstReg, srcReg0, scalar, mask);
    } else if constexpr (mode == CMPMODE::GT) {
        vcmps_gt(dstReg, srcReg0, scalar, mask);
    } else if constexpr (mode == CMPMODE::GE) {
        vcmps_ge(dstReg, srcReg0, scalar, mask);
    } else if constexpr (mode == CMPMODE::LT) {
        vcmps_lt(dstReg, srcReg0, scalar, mask);
    } else if constexpr (mode == CMPMODE::LE) {
        vcmps_le(dstReg, srcReg0, scalar, mask);
    }
}

template <typename T>
__aicore__ inline void Select(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    vsel(dstReg, srcReg0, srcReg1, mask);
}

template <typename T>
__aicore__ inline void Slide(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, uint16_t slideAmount)
{
    vslide(dstReg, srcReg0, srcReg1, slideAmount);
}

template <typename T>
__aicore__ inline void Selr(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1)
{
    vselr(dstReg, srcReg0, srcReg1);
}

template <typename T>
__aicore__ inline void Addcs(
    MaskReg& carryp, RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& carrysrcp,
    MaskReg& mask)
{
    vaddcs(carryp, dstReg, srcReg0, srcReg1, carrysrcp, mask);
}

template <typename T>
__aicore__ inline void Subcs(
    MaskReg& carryp, RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& carrysrcp,
    MaskReg& mask)
{
    vsubcs(carryp, dstReg, srcReg0, srcReg1, carrysrcp, mask);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void Adds(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vadds(dstReg, srcReg0, scalar, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void Muls(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmuls(dstReg, srcReg0, scalar, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void Maxs(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmaxs(dstReg, srcReg0, scalar, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void Mins(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmins(dstReg, srcReg0, scalar, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void ShiftLefts(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vshls(dstReg, srcReg0, scalar, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void ShiftRights(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vshrs(dstReg, srcReg0, scalar, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void Rounds(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, U scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vrnds(dstReg, srcReg0, scalar, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void LeakyRelu(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, T scalar, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vlrelu(dstReg, srcReg0, scalar, mask, modeValue);
}
template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Add(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vadd(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Sub(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vsub(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Mul(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmul(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Div(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Max(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmax(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Min(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmin(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void ShiftLeft(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<U>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vshl(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void ShiftRight(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<U>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vshr(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void And(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vand(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Or(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vor(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Xor(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vxor(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, typename U, Mode mode = Mode::ZEROING>
__aicore__ inline void Round(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<U>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vrnd(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Prelu(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vprelu(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T>
__aicore__ inline void Mod(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<Mode::ZEROING>();
    vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
    vmul(dstReg, srcReg1, dstReg, mask, modeValue);
    vsub(dstReg, srcReg0, dstReg, mask, modeValue);
}

template <typename T>
__aicore__ inline void Mull(
    RegTensor<T>& dstReg0, RegTensor<T>& dstReg1, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    vmull(dstReg0, dstReg1, srcReg0, srcReg1, mask);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void Mula(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmula(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T>
__aicore__ inline void AddCarryOut(
    MaskReg& carryp, RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    vaddc(carryp, dstReg, srcReg0, srcReg1, mask);
}

template <typename T>
__aicore__ inline void SubCarryOut(
    MaskReg& carryp, RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    vsubc(carryp, dstReg, srcReg0, srcReg1, mask);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void SaturationAdd(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vsadd(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Mode mode = Mode::ZEROING>
__aicore__ inline void SaturationSub(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vssub(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, Rnd rnd, Mode mode = Mode::ZEROING>
__aicore__ inline void Average(RegTensor<T>& dstReg, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vavg(dstReg, srcReg0, srcReg1, rnd, mask, modeValue);
}
template <typename T, typename U, Mode part = Mode::ZEROING, Mode mode = Mode::ZEROING>
__aicore__ inline void Pack(RegTensor<T>& dstReg, RegTensor<U>& srcReg)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto partValue = std::integral_constant<::Mode, static_cast<::Mode>(part)>();
    vpack(dstReg, srcReg, partValue, modeValue);
}

template <typename T, typename U, Mode part = Mode::ZEROING>
__aicore__ inline void UnPack(RegTensor<T>& dstReg, RegTensor<U>& srcReg)
{
    constexpr auto partValue = std::integral_constant<::Mode, static_cast<::Mode>(part)>();
    vunpack(dstReg, srcReg, partValue);
}

template <typename T>
__simd_callee__ inline MaskReg CreatePredicate(uint32_t& scalar)
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "CreatePredicate only support type b8/b16/b32");

    MaskReg reg;
    if constexpr (sizeof(T) == 1) {
        reg = plt_b8(scalar, POST_UPDATE);
    } else if constexpr (sizeof(T) == 2) {
        reg = plt_b16(scalar, POST_UPDATE);
    } else if constexpr (sizeof(T) == 4) {
        reg = plt_b32(scalar, POST_UPDATE);
    }
    return reg;
}

template <typename T, Pat mode = Pat::ALL>
__simd_callee__ inline MaskReg CreatePredicate()
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "CreatePredicate only support type b8/b16/b32");

    constexpr auto modeValue = std::integral_constant<::Pat, static_cast<::Pat>(mode)>();
    MaskReg reg;
    if constexpr (sizeof(T) == 1) {
        reg = pset_b8(modeValue);
    } else if constexpr (sizeof(T) == 2) {
        reg = pset_b16(modeValue);
    } else if constexpr (sizeof(T) == 4) {
        reg = pset_b32(modeValue);
    }
    return reg;
}

__aicore__ inline void PredicateNot(MaskReg& dstMask, MaskReg& srcMask, MaskReg& mask) { pnot(dstMask, srcMask, mask); }

__aicore__ inline void PredicateAnd(MaskReg& dstMask, MaskReg& srcMask0, MaskReg& srcMask1, MaskReg& mask)
{
    pand(dstMask, srcMask0, srcMask1, mask);
}

__aicore__ inline void PredicateOr(MaskReg& dstMask, MaskReg& srcMask0, MaskReg& srcMask1, MaskReg& mask)
{
    por(dstMask, srcMask0, srcMask1, mask);
}

__aicore__ inline void PredicateXor(MaskReg& dstMask, MaskReg& srcMask0, MaskReg& srcMask1, MaskReg& mask)
{
    pxor(dstMask, srcMask0, srcMask1, mask);
}

__aicore__ inline void PredicateMov(MaskReg& dstMask, MaskReg& srcMask, MaskReg& mask) { pmov(dstMask, srcMask, mask); }

__aicore__ inline void PredicateMov(MaskReg& dstMask, MaskReg& srcMask) { pmov(dstMask, srcMask); }

template <typename T>
__aicore__ inline void PredicateSlide(MaskReg& dstMask, MaskReg& srcMask0, MaskReg& srcMask1, const int16_t slideAmount)
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "PredicateSlide only support type b8/b16/b32 ");
    if constexpr (sizeof(T) == 1) {
        pslide_b8(dstMask, srcMask0, srcMask1, slideAmount);
    } else if constexpr (sizeof(T) == 2) {
        pslide_b16(dstMask, srcMask0, srcMask1, slideAmount);
    } else if constexpr (sizeof(T) == 4) {
        pslide_b32(dstMask, srcMask0, srcMask1, slideAmount);
    }
}

template <typename T>
__simd_callee__ inline void PredicateInterleave(
    MaskReg& dstMask0, MaskReg& dstMask1, MaskReg& srcMask0, MaskReg& srcMask1)
{
    static_assert(
        sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "PredicateInterleave only support type b8/b16/b32 ");
    if constexpr (sizeof(T) == 1) {
        pintlv_b8(dstMask0, dstMask1, srcMask0, srcMask1);
    } else if constexpr (sizeof(T) == 2) {
        pintlv_b16(dstMask0, dstMask1, srcMask0, srcMask1);
    } else if constexpr (sizeof(T) == 4) {
        pintlv_b32(dstMask0, dstMask1, srcMask0, srcMask1);
    }
}

template <typename T>
__simd_callee__ inline void PredicateDeInterleave(
    MaskReg& dstMask0, MaskReg& dstMask1, MaskReg& srcMask0, MaskReg& srcMask1)
{
    static_assert(
        sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "PredicateDeInterleave only support type b8/b16/b32 ");
    if constexpr (sizeof(T) == 1) {
        pdintlv_b8(dstMask0, dstMask1, srcMask0, srcMask1);
    } else if constexpr (sizeof(T) == 2) {
        pdintlv_b16(dstMask0, dstMask1, srcMask0, srcMask1);
    } else if constexpr (sizeof(T) == 4) {
        pdintlv_b32(dstMask0, dstMask1, srcMask0, srcMask1);
    }
}

__aicore__ inline void PredicateSel(MaskReg& dstMask, MaskReg& srcMask0, MaskReg& srcMask1, MaskReg& mask)
{
    psel(dstMask, srcMask0, srcMask1, mask);
}

template <HiloPart part = HiloPart::Lower>
__simd_callee__ inline void PredicatePack(MaskReg& dstMask, MaskReg& srcMask)
{
    constexpr auto partValue = std::integral_constant<::HiloPart, static_cast<::HiloPart>(part)>();
    ppack(dstMask, srcMask, partValue);
}

template <HiloPart part = HiloPart::Lower>
__simd_callee__ inline void PredicateUnPack(MaskReg& dstMask, MaskReg& srcMask)
{
    constexpr auto partValue = std::integral_constant<::HiloPart, static_cast<::HiloPart>(part)>();
    punpack(dstMask, srcMask, partValue);
}

template <typename T>
__aicore__ inline MaskReg MovePredicate()
{
    static_assert(SupportBytes<T, 2, 4>(), "MovePredicate only support type b16/b32");
    if constexpr (sizeof(T) == 2) {
        return movp_b16();
    } else {
        return movp_b32();
    }
}
// vlds
template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(RegTensor<T>& dstReg, LocalMem T* srcUbAddr, int32_t offset)
{
    if constexpr (sizeof(T) == 8) {
        vlds(dstReg, srcUbAddr, offset); // use default Dist::DIST_DINTLV_B32
    } else {
        constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
        vlds(dstReg, srcUbAddr, offset, distValue);
    }
}

template <typename T, PostLiteral postMode, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(RegTensor<T>& dstReg, LocalMem T*& srcUbAddr, int32_t offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vlds(dstReg, srcUbAddr, offset, distValue, postValue);
}

template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(
    RegTensor<T>& dstReg0, RegTensor<T>& dstReg1, LocalMem T* srcUbAddr, int32_t offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    vlds(dstReg0, dstReg1, srcUbAddr, offset, distValue);
}

template <typename T, PostLiteral postMode, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(
    RegTensor<T>& dstReg0, RegTensor<T>& dstReg1, LocalMem T*& srcUbAddr, int32_t offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vlds(dstReg0, dstReg1, srcUbAddr, offset, distValue, postValue);
}

// vld
template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(RegTensor<T>& dstReg, LocalMem T* srcUbAddr, AddrReg offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    vld(dstReg, srcUbAddr, offset, distValue);
}

template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(RegTensor<T>& dstReg, LocalMem T* srcUbAddr)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    vld(dstReg, srcUbAddr, distValue);
}

template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(
    RegTensor<T>& dstReg0, RegTensor<T>& dstReg1, LocalMem T* srcUbAddr, AddrReg offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    vld(dstReg0, dstReg1, srcUbAddr, offset, distValue);
}

template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(RegTensor<T>& dstReg0, RegTensor<T>& dstReg1, LocalMem T* srcUbAddr)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    vld(dstReg0, dstReg1, srcUbAddr, distValue);
}

// vsts
template <typename T, DistVST dist = DistVST::DIST_NORM>
__simd_callee__ inline void DataCopy(LocalMem T* dstUbAddr, RegTensor<T>& srcReg, int32_t offset, MaskReg& mask)
{
    if constexpr (sizeof(T) == 8) {
        vsts(srcReg, dstUbAddr, offset, mask); // use default Dist::DIST_INTLV_B32
    } else {
        constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetDistVst<T, dist>())>();
        vsts(srcReg, dstUbAddr, offset, distValue, mask);
    }
}

template <typename T, PostLiteral postMode, DistVST dist = DistVST::DIST_NORM>
__simd_callee__ inline void DataCopy(LocalMem T*& dstUbAddr, RegTensor<T>& srcReg, int32_t offset, MaskReg& mask)
{
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetDistVst<T, dist>())>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vsts(srcReg, dstUbAddr, offset, distValue, mask, postValue);
}

template <typename T, DistVST dist = DistVST::DIST_NORM>
__simd_callee__ inline void DataCopy(
    LocalMem T* dstUbAddr, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, int32_t offset, MaskReg& mask)
{
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetDistVst<T, dist>())>();
    vsts(srcReg0, srcReg1, dstUbAddr, offset, distValue, mask);
}

// vst
template <typename T, DistVST dist = DistVST::DIST_NORM>
__simd_callee__ inline void DataCopy(LocalMem T* dstUbAddr, RegTensor<T>& srcReg, AddrReg offset, MaskReg& mask)
{
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetDistVst<T, dist>())>();
    vst(srcReg, dstUbAddr, offset, distValue, mask);
}

template <typename T, DistVST dist = DistVST::DIST_NORM>
__simd_callee__ inline void DataCopy(
    LocalMem T* dstUbAddr, RegTensor<T>& srcReg0, RegTensor<T>& srcReg1, AddrReg offset, MaskReg& mask)
{
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetDistVst<T, dist>())>();
    vst(srcReg0, srcReg1, dstUbAddr, offset, distValue, mask);
}

// vsldb
template <typename T>
__simd_callee__ inline void DataCopy(
    RegTensor<T>& dstReg, LocalMem T* srcUbAddr, uint32_t blockStride, uint32_t repeatStride, MaskReg& mask)
{
    vsldb(dstReg, srcUbAddr, (blockStride << 16u) | (repeatStride & 0xFFFFU), mask);
}

template <typename T, PostLiteral postMode>
__simd_callee__ inline void DataCopy(
    RegTensor<T>& dstReg, LocalMem T*& srcUbAddr, uint32_t blockStride, uint32_t repeatStride, MaskReg& mask)
{
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vsldb(dstReg, srcUbAddr, (blockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
}

// vsstb
template <typename T>
__simd_callee__ inline void DataCopy(
    LocalMem T* dstUbAddr, RegTensor<T>& srcReg, uint32_t blockStride, uint32_t repeatStride, MaskReg& mask)
{
    vsstb(srcReg, dstUbAddr, (blockStride << 16u) | (repeatStride & 0xFFFFU), mask);
}

template <typename T, PostLiteral postMode>
__simd_callee__ inline void DataCopy(
    LocalMem T*& dstUbAddr, RegTensor<T>& srcReg, uint32_t blockStride, uint32_t repeatStride, MaskReg& mask)
{
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vsstb(srcReg, dstUbAddr, (blockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
}

// vldas/vldus
template <typename T>
__simd_callee__ inline void DataCopyUnAlignPre(UnalignReg& ureg, LocalMem T* srcUbAddr)
{
    vldas(ureg, srcUbAddr);
}

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlign(
    RegTensor<T>& dstReg, UnalignReg& ureg, LocalMem T*& srcUbAddr, uint32_t stride)
{
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vldus(dstReg, ureg, srcUbAddr, stride, postValue);
}

template <typename T>
__simd_callee__ inline void DataCopyUnAlign(RegTensor<T>& dstReg, UnalignReg& ureg, LocalMem T* srcUbAddr)
{
    vldus(dstReg, ureg, srcUbAddr);
}

// vlda/vldu
template <typename T>
__simd_callee__ inline void DataCopyUnAlignPre(UnalignReg& ureg, LocalMem T* srcUbAddr, AddrReg& areg)
{
    vlda(ureg, srcUbAddr, areg);
}

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlign(
    RegTensor<T>& dstReg, UnalignReg& ureg, LocalMem T*& srcUbAddr, AddrReg& areg, uint32_t inc)
{
    vldu(dstReg, ureg, areg, srcUbAddr, inc);
}

// vstus/vstas
template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlign(
    LocalMem T*& dstUbAddr, RegTensor<T>& srcReg, UnalignReg& ureg, uint32_t offset)
{
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vstus(ureg, offset, srcReg, dstUbAddr, postValue);
}

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlignPost(LocalMem T*& dstUbAddr, UnalignReg& ureg, int32_t offset)
{
    if constexpr (postMode == PostLiteral::POST_MODE_UPDATE) {
        vstas(ureg, dstUbAddr, offset, POST_UPDATE);
    } else {
        vstas(ureg, dstUbAddr, offset);
    }
}

// vstu/vsta
template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlign(
    LocalMem T*& dstUbAddr, RegTensor<T>& srcReg, UnalignReg& ureg, AddrReg& areg)
{
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vstu(ureg, areg, srcReg, dstUbAddr, postValue);
}

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlignPost(LocalMem T*& dstUbAddr, UnalignReg& ureg, AddrReg& areg)
{
    vsta(ureg, dstUbAddr, areg);
}

// vstur/vstar
template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlign(LocalMem T*& dstUbAddr, RegTensor<T>& srcReg, UnalignReg& ureg)
{
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    vstur(ureg, srcReg, dstUbAddr, postValue);
}

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlignPost(LocalMem T*& dstUbAddr, UnalignReg& ureg)
{
    vstar(ureg, dstUbAddr);
}

// vgather2
template <typename T, typename U, typename S>
__simd_callee__ inline void DataCopyGather(
    RegTensor<T>& dstReg, LocalMem U* baseAddr, RegTensor<S>& index, MaskReg& mask)
{
    static_assert(
        (sizeof(U) == 1 && sizeof(T) == 2 && Std::is_same_v<S, uint16_t>) ||
            (sizeof(U) == 2 && sizeof(T) == 2 && Std::is_same_v<S, uint16_t>) ||
            (sizeof(U) == 4 && sizeof(T) == 4 && Std::is_same_v<S, uint32_t>),
        "DataCopyGather only support src data type b8/b16/b32 with dst type is b16/b16/b32 respectively and each index "
        "type is u16/u16/u32 respectively");
    if constexpr (sizeof(U) == 1 && sizeof(T) == 2) {
        vgather2((vector_s16&)dstReg, (__ubuf__ int8_t*)baseAddr, index, mask);
    } else if constexpr (sizeof(U) == 2 && sizeof(T) == 2) {
        vgather2((vector_s16&)dstReg, (__ubuf__ int16_t*)baseAddr, index, mask);
    } else {
        vgather2((vector_s32&)dstReg, (__ubuf__ int32_t*)baseAddr, index, mask);
    }
}

// vgatherb
// mask will be treated as PAT_ALL in this l311 api.
template <typename T>
__simd_callee__ inline void DataCopyGatherB(
    RegTensor<T>& dstReg, LocalMem T* baseAddr, RegTensor<uint32_t>& index, MaskReg& mask)
{
    vgatherb(dstReg, baseAddr, index);
}

// vscatter
template <typename T, typename U>
__simd_callee__ inline void DataCopyScatter(
    LocalMem T* baseAddr, RegTensor<T>& srcReg, RegTensor<U>& index, MaskReg& mask)
{
    vscatter(srcReg, baseAddr, index, mask);
}

// plds/psts
template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(MaskReg& dstReg, LocalMem T* srcUbAddr, int32_t offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    plds(dstReg, srcUbAddr, offset, distValue);
}

template <typename T, PostLiteral post, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(MaskReg& dstReg, LocalMem T*& srcUbAddr, int32_t offset)
{
    // post为plds/psts对应的入参，为0时（NO_POST_UPDATE），每次存在srcUbAddr+offset的地址；
    // 为1时，存在srcUbAddr的地址并对srcUbAddr进行+offset
    // 因为存在预编译的静态assert，这里生成的postValue均为post为1，通过判断入参来确定到底走哪个template
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(Post::POST_UPDATE_VALUE)>();
    if (post == PostLiteral::POST_MODE_UPDATE) {
        plds(dstReg, srcUbAddr, offset, distValue, postValue);
    } else {
        plds(dstReg, srcUbAddr, offset, distValue);
    }
}

template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(LocalMem T* dstUbAddr, MaskReg& dstReg, int32_t offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    psts(dstReg, dstUbAddr, offset, distValue);
}

template <typename T, PostLiteral post, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(LocalMem T*& dstUbAddr, MaskReg& dstReg, int32_t offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(Post::POST_UPDATE_VALUE)>();
    if (post == PostLiteral::POST_MODE_UPDATE) {
        psts(dstReg, dstUbAddr, offset, distValue, postValue);
    } else {
        psts(dstReg, dstUbAddr, offset, distValue);
    }
}

// pld/pst
template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(MaskReg& dstReg, LocalMem T* srcUbAddr, AddrReg offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    pld(dstReg, srcUbAddr, offset, distValue);
}

template <typename T, Dist dist = Dist::DIST_NORM>
__simd_callee__ inline void DataCopy(LocalMem T* dstUbAddr, MaskReg& dstReg, AddrReg offset)
{
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    pst(dstReg, dstUbAddr, offset, distValue);
}

template <typename T, Mode mode = Mode::MERGING>
__simd_callee__ inline void Copy(RegTensor<T>& dstReg, RegTensor<T> srcReg, MaskReg mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmov(dstReg, srcReg, mask, modeValue);
}

template <typename T>
__simd_callee__ inline void Copy(RegTensor<T>& dstReg, RegTensor<T> srcReg)
{
    vmov(dstReg, srcReg);
}

template <typename T>
__simd_callee__ inline auto CreateAddrReg(uint16_t stride0)
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "CreateAddrReg only support type b8/b16/b32");
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0);
    }
    return vag_b32(stride0);
}

template <typename T>
__simd_callee__ inline auto CreateAddrReg(uint16_t stride0, uint16_t stride1)
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "CreateAddrReg only support type b8/b16/b32");
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0, stride1);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0, stride1);
    }
    return vag_b32(stride0, stride1);
}

template <typename T>
__simd_callee__ inline auto CreateAddrReg(uint16_t stride0, uint16_t stride1, uint16_t stride2)
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "CreateAddrReg only support type b8/b16/b32");
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0, stride1, stride2);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0, stride1, stride2);
    }
    return vag_b32(stride0, stride1, stride2);
}

template <typename T>
__simd_callee__ inline auto CreateAddrReg(uint16_t stride0, uint16_t stride1, uint16_t stride2, uint16_t stride3)
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "CreateAddrReg only support type b8/b16/b32");
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0, stride1, stride2, stride3);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0, stride1, stride2, stride3);
    }
    return vag_b32(stride0, stride1, stride2, stride3);
}

__aicore__ inline void Barrier()
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    __asm__ __volatile__("" ::: "memory");
#else
    __asm__ __volatile__("");
#endif
}

template <int8_t startBit, int8_t endBit>
__aicore__ static inline void SetCtrlSprImpl(int64_t value)
{
    static_assert((startBit <= endBit && startBit >= 0 && endBit < 64), "Invalid bit range on current device!");
    static_assert(
        (6 <= startBit && startBit <= 10 && 6 <= endBit && endBit <= 10) ||
            (startBit == endBit && (startBit == 45 || startBit == 48 || startBit == 50 || startBit == 53 ||
                                    startBit == 59 || startBit == 60)),
        "Invalid startBit/endBit on current device!");
    if (endBit - startBit == 63) {
        set_ctrl(value);
        return;
    }
    uint64_t mask = ((uint64_t(1) << (endBit - startBit + 1)) - 1) << startBit;
    mask = ~mask;
    int64_t setValue = get_ctrl() & mask;
    setValue |= (value << startBit);
    set_ctrl(setValue);
}

template <int8_t startBit, int8_t endBit>
__aicore__ static inline int64_t GetCtrlSprImpl()
{
    static_assert((startBit <= endBit && startBit >= 0 && endBit < 64), "Invalid bit range on current device!");
    int64_t value = get_ctrl();
    if (endBit - startBit == 63) {
        return value;
    }
    value = value >> startBit;
    value &= ((uint64_t(1) << (endBit - startBit + 1)) - 1);
    return value;
}

template <int8_t startBit, int8_t endBit>
__aicore__ static inline void ResetCtrlSprImpl()
{
    static_assert((startBit <= endBit && startBit >= 0 && endBit < 64), "Invalid bit range on current device!");
    static_assert(
        (6 <= startBit && startBit <= 10 && 6 <= endBit && endBit <= 10) ||
            (startBit == endBit && (startBit == 45 || startBit == 48 || startBit == 50 || startBit == 53 ||
                                    startBit == 59 || startBit == 60)),
        "Invalid startBit/endBit on current device!");
    int64_t defaultCtrl = 0x1000000000000008; // default value of ctrl
    if (endBit - startBit == 63) {
        set_ctrl(defaultCtrl);
        return;
    }
    uint64_t mask = ((uint64_t(1) << (endBit - startBit + 1)) - 1) << startBit;
    defaultCtrl = defaultCtrl & mask;
    mask = ~mask;
    int64_t value = get_ctrl() & mask;
    value = value | defaultCtrl;
    set_ctrl(value);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_COMMON_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_IMPL_H__
#endif
