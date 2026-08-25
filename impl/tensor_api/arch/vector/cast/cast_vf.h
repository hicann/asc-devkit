/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/vector/cast/cast_vf.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file cast_vf.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_VF_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_VF_H
#include "impl/tensor_api/arch/vector/cast/cast_enums.h"
#include "impl/tensor_api/arch/vector/cast/cast_trait_map.h"
#include "impl/tensor_api/arch/vector/cast/instruction.h"
#include "impl/tensor_api/arch/vector/utils/mask_utils.h"

namespace AscendC {
namespace Te {

struct CastWidthSrcEqDst {}; // 1:1: float<->int32, half<->int16
struct CastWidthDst2xSrc {}; // 1:2: half->float, int16->int32
struct CastWidthSrc2xDst {}; // 2:1: float->half, float->int16
struct CastWidthDst4xSrc {}; // 1:4: fp8->float, int8->int32
struct CastWidthSrc4xDst {}; // 4:1: float->fp8, int32->uint8

template <typename WidthCategory>
class CastVFLoop;

//   T = dst element type, U = src element type, sizeof(T) == sizeof(U)
template <>
class CastVFLoop<CastWidthSrcEqDst> {
public:
    template <typename CalcFunc, typename T, typename U, typename DstRegType, typename SrcRegType>
    __simd_callee__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ U* src, int32_t repeat, int32_t oneRepSize, uint32_t dataSize, DstRegType& dstReg,
        SrcRegType& srcReg, vector_bool& dstMask, vector_bool fullMask)
    {
        for (uint16_t i = 0; i < repeat; i++) {
            dstMask = Inst::UpdateMask::template Run<U>(dataSize);

            asc_loadalign(srcReg, src + i * oneRepSize);
            CalcFunc::template Run(dstReg, srcReg, fullMask);
            asc_storealign(dst + i * oneRepSize, dstReg, dstMask);
        }
    }
};

//   T = dst element type, U = src element type, sizeof(T) == 2 * sizeof(U)
template <>
class CastVFLoop<CastWidthDst2xSrc> {
public:
    template <typename CalcFunc, typename T, typename U, typename DstRegType, typename SrcRegType>
    __simd_callee__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ U* src, int32_t repeat, int32_t oneRepSize, uint32_t dataSize, DstRegType& dstReg,
        SrcRegType& srcReg, vector_bool& dstMask, vector_bool fullMask)
    {
        for (uint16_t i = 0; i < repeat; i++) {
            dstMask = Inst::UpdateMask::template Run<T>(dataSize);

            asc_loadalign_unpack_postupdate(srcReg, src, oneRepSize);
            CalcFunc::template Run(dstReg, srcReg, fullMask);
            asc_storealign(dst + i * oneRepSize, dstReg, dstMask);
        }
    }
};

//   T = dst element type, U = src element type, sizeof(U) == 2 * sizeof(T)
template <>
class CastVFLoop<CastWidthSrc2xDst> {
public:
    template <typename CalcFunc, typename T, typename U, typename DstRegType, typename SrcRegType>
    __simd_callee__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ U* src, int32_t repeat, int32_t oneRepSize, uint32_t dataSize, DstRegType& dstReg,
        SrcRegType& srcReg, vector_bool& dstMask, vector_bool fullMask)
    {
        auto dstCur = reinterpret_cast<__ubuf__ U*>(dst);
        for (uint16_t i = 0; i < repeat; i++) {
            dstMask = Inst::UpdateMask::template Run<U>(dataSize);

            asc_loadalign(srcReg, src + i * oneRepSize);
            CalcFunc::template Run(dstReg, srcReg, fullMask);
            asc_storealign_pack_postupdate(dstCur, *reinterpret_cast<SrcRegType*>(&dstReg), oneRepSize / 2, dstMask);
        }
    }
};

//   T = dst element type, U = src element type, sizeof(T) == 4 * sizeof(U)
template <>
class CastVFLoop<CastWidthDst4xSrc> {
public:
    template <typename CalcFunc, typename T, typename U, typename DstRegType, typename SrcRegType>
    __simd_callee__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ U* src, int32_t repeat, int32_t oneRepSize, uint32_t dataSize, DstRegType& dstReg,
        SrcRegType& srcReg, vector_bool& dstMask, vector_bool fullMask)
    {
        for (uint16_t i = 0; i < repeat; i++) {
            dstMask = Inst::UpdateMask::template Run<T>(dataSize);

            asc_loadalign_unpack_postupdate_v2(srcReg, src, oneRepSize);
            CalcFunc::template Run(dstReg, srcReg, fullMask);
            asc_storealign(dst + i * oneRepSize, dstReg, dstMask);
        }
    }
};

//   T = dst element type, U = src element type, sizeof(U) == 4 * sizeof(T)
template <>
class CastVFLoop<CastWidthSrc4xDst> {
public:
    template <typename CalcFunc, typename T, typename U, typename DstRegType, typename SrcRegType>
    __simd_callee__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ U* src, int32_t repeat, int32_t oneRepSize, uint32_t dataSize, DstRegType& dstReg,
        SrcRegType& srcReg, vector_bool& dstMask, vector_bool fullMask)
    {
        auto dstCur = reinterpret_cast<__ubuf__ U*>(dst);
        for (uint16_t i = 0; i < repeat; i++) {
            dstMask = Inst::UpdateMask::template Run<U>(dataSize);

            asc_loadalign(srcReg, src + i * oneRepSize);
            CalcFunc::template Run(dstReg, srcReg, fullMask);
            asc_storealign_pack_postupdate_v2(dstCur, *reinterpret_cast<SrcRegType*>(&dstReg), oneRepSize / 4, dstMask);
        }
    }
};

// ==================== CastVF ====================
template <typename CalcFunc, typename TraitType, typename WidthCategory>
class CastVF {
public:
    template <typename T, typename U>
    __simd_vf__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ U* src, int32_t repeat, int32_t oneRepSize, uint32_t dataSize)
    {
        using DstRegType = typename VectorTypeTransform::template Get<T>;
        using SrcRegType = typename VectorTypeTransform::template Get<U>;

        DstRegType dstReg;
        SrcRegType srcReg;

        vector_bool dstMask;
        vector_bool fullMask = Inst::CreateMask::template Run<int32_t, decltype(PAT_ALL)>();
        CastVFLoop<WidthCategory>::template Run<CalcFunc, T, U>(
            dst, src, repeat, oneRepSize, dataSize, dstReg, srcReg, dstMask, fullMask);
    }
};

// ==================== Transform2CastVF ====================
template <typename CalcFunc, typename TraitType>
class Transform2CastVF {
public:
    template <typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        using dstType = GetAttributeElementType<typename T::elementType*>;
        using srcType = GetAttributeElementType<typename U::elementType*>;

        uint32_t dstSize = dst.Size();

        int32_t VECTOR_REG_WIDTH = asc_get_vf_len();
        using greaterType = Std::conditional_t<(sizeof(dstType) > sizeof(srcType)), dstType, srcType>;
        int32_t oneRepSize = VECTOR_REG_WIDTH / sizeof(greaterType);

        int32_t repeat = Std::ceil_division(dstSize, oneRepSize);

        constexpr bool src_eq_dst = (sizeof(dstType) == sizeof(srcType));     // 1:1: float<->int32, half<->int16
        constexpr bool dst_2x_src = (sizeof(dstType) == 2 * sizeof(srcType)); // 1:2: half->float, int16->int32
        constexpr bool src_2x_dst = (sizeof(srcType) == 2 * sizeof(dstType)); // 2:1: float->half, float->int16
        constexpr bool dst_4x_src = (sizeof(dstType) == 4 * sizeof(srcType)); // 1:4: fp8->float, int8->int32
        constexpr bool src_4x_dst = (sizeof(srcType) == 4 * sizeof(dstType)); // 4:1: float->fp8, int32->uint8

        // Select width category tag based on size relationship
        using WidthCategory = Std::conditional_t<
            src_eq_dst, CastWidthSrcEqDst,
            Std::conditional_t<
                dst_2x_src, CastWidthDst2xSrc,
                Std::conditional_t<
                    src_2x_dst, CastWidthSrc2xDst,
                    Std::conditional_t<dst_4x_src, CastWidthDst4xSrc, CastWidthSrc4xDst>>>>;

        CastVF<CalcFunc, TraitType, WidthCategory>::template Run(
            dst.Data().Get(), src.Data().Get(), repeat, oneRepSize, dstSize);
    }
};

// Two-level lookup to avoid template recursion depth overflow.
// First level: CastSubMap<SrcType, DstType> -> sub-map.  Second level: (RoundMode, SatMode, IndexPos) -> Inst.
struct CastTrait2CastFunc {
private:
    template <typename SrcType, typename DstType, typename RoundMode, typename SatMode, typename IndexPos>
    struct Impl {
        using SubMap = typename CastSubMap<SrcType, DstType>::type;
        using type = typename SubMap::template Get<Std::tuple<RoundMode, SatMode, IndexPos>>;
    };

public:
    template <typename SrcType, typename DstType, typename RoundMode, typename SatMode, typename IndexPos>
    using Lookup = typename Impl<SrcType, DstType, RoundMode, SatMode, IndexPos>::type;
};

template <typename CalcFunc, typename TraitType>
class Transform2CastInstMatch {
public:
    template <typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        using srcType = GetAttributeElementType<typename U::elementType*>;
        using dstType = GetAttributeElementType<typename T::elementType*>;
        using trait = Std::conditional_t<Std::is_same_v<TraitType, Std::ignore_t>, CastTraitDefault, TraitType>;

        using roundMode = Std::conditional_t<Std::is_same_v<srcType, dstType>, CalcFunc, typename trait::roundMode>;
        using satMode = Std::conditional_t<
            Std::is_same_v<typename trait::satMode, CastSatMode::NoSat>, Std::ignore_t, typename trait::satMode>;
        using indexPos = Std::conditional_t<
            (Std::is_same_v<typename trait::indexPos, CastIndexPos::Even> ||
             Std::is_same_v<typename trait::indexPos, CastIndexPos::PartP0>),
            Std::ignore_t, typename trait::indexPos>;

        using func = CastTrait2CastFunc::template Lookup<srcType, dstType, roundMode, satMode, indexPos>;
        static_assert(
            !Std::is_same_v<func, Std::ignore_t>,
            "Unsupported cast trait: no matching entry found in CastTrait2CastFunc.");
        Transform2CastVF<func, Std::ignore_t>::template Run(dst, src);
    }
};
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_VF_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
