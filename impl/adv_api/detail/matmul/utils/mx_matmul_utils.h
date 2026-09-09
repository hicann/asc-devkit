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
 * \file mx_matmul_utils.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/utils/mx_matmul_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MX_MATMUL_UTILS_H__
#endif
#ifndef IMPL_MATMUL_UTILS_MX_MATMUL_UTILS_H
#define IMPL_MATMUL_UTILS_MX_MATMUL_UTILS_H

#include "matmul_type_def.h"
#include "../feature_trait/matmul_feature_trait.h"
namespace AscendC {

template <typename T, typename U>
constexpr bool IsSameTypeV = AscendC::IsSameType<T, U>::value;

template <typename T, typename... Others>
struct IsTypeOneOf {
    static constexpr bool value = false;
};

template <typename T, typename First, typename... Others>
struct IsTypeOneOf<T, First, Others...> {
    static constexpr bool value = IsSameTypeV<T, First> || IsTypeOneOf<T, Others...>::value;
};

template <typename T, typename... Others>
constexpr bool IsTypeOneOfV = IsTypeOneOf<T, Others...>::value;

template <typename T>
struct GetMmDstType {
    using Type = T;
};

template <typename T, bool isMxType = false>
struct GetL0DataType {
    using Type = T;
};

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
template <>
struct GetMmDstType<fp8_e4m3fn_t> {
    using Type = float;
};

template <>
struct GetMmDstType<fp8_e5m2_t> {
    using Type = float;
};

template <>
struct GetMmDstType<hifloat8_t> {
    using Type = float;
};

template <>
struct GetMmDstType<fp4x2_e2m1_t> {
    using Type = float;
};

template <>
struct GetMmDstType<fp4x2_e1m2_t> {
    using Type = float;
};

template <>
struct GetL0DataType<fp8_e5m2_t, true> {
    using Type = AscendC::mx_fp8_e5m2_t;
};

template <>
struct GetL0DataType<fp8_e5m2_t, false> {
    using Type = fp8_e5m2_t;
};

template <>
struct GetL0DataType<fp8_e4m3fn_t, true> {
    using Type = AscendC::mx_fp8_e4m3_t;
};

template <>
struct GetL0DataType<fp8_e4m3fn_t, false> {
    using Type = fp8_e4m3fn_t;
};
#endif

template <typename SrcT>
__aicore__ inline constexpr static int32_t AuxGetC0Size()
{
    if (sizeof(SrcT) == sizeof(float)) {
        return Impl::B32_C0SIZE;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    else if (IsTypeOneOfV<SrcT, uint8_t, int8_t, hifloat8_t, fp8_e4m3fn_t, fp8_e5m2_t, fp8_e8m0_t>) {
        return Impl::B8_C0SIZE;
    } else if (IsTypeOneOfV<SrcT, int4b_t, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
        return Impl::B4_C0SIZE;
    }
#else
    else if (IsSameType<SrcT, int8_t>::value) {
        return Impl::B8_C0SIZE;
    } else if (IsSameType<SrcT, int4b_t>::value) {
        return Impl::B4_C0SIZE;
    }
#endif
    return Impl::B16_C0SIZE;
}

template <typename SrcT>
__aicore__ inline constexpr bool IsSupportB32()
{
    if (IsTypeOneOfV<SrcT, int32_t, float>) {
        return true;
    }
    return false;
}

template <typename SrcT>
__aicore__ inline constexpr bool IsSupportB8()
{
    if (IsSameTypeV<SrcT, int8_t>) {
        return true;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    if (IsTypeOneOfV<SrcT, hifloat8_t, fp8_e4m3fn_t, fp8_e5m2_t>) {
        return true;
    }
#endif
    return false;
}

template <typename SrcT>
__aicore__ inline constexpr bool IsSupportB4()
{
    if (IsSameTypeV<SrcT, int4b_t>) {
        return true;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    if (IsTypeOneOfV<SrcT, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
        return true;
    }
#endif
    return false;
}

template <typename SrcT>
__aicore__ inline constexpr bool IsSupportMxFp4()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    if (IsTypeOneOfV<SrcT, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
        return true;
    }
#endif
    return false;
}

template <typename SrcT>
__aicore__ inline constexpr bool IsSupportMxFp8()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    if (IsTypeOneOfV<SrcT, fp8_e4m3fn_t, fp8_e5m2_t>) {
        return true;
    }
#endif
    return false;
}

template <typename T>
__aicore__ inline constexpr static bool IsNeedC0Align()
{
    return IsSupportB8<T>() || IsSupportB4<T>();
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
constexpr uint8_t INTRA_MODE = 4;
template <typename INPUT_TYPE>
__aicore__ constexpr bool PhyMxScalePosIsL1()
{
    if constexpr (HasScalePosition<INPUT_TYPE>::value) {
        return PhyPosIsL1(INPUT_TYPE::scalePosition);
    }
    return false;
}

template <typename INPUT_TYPE>
__aicore__ constexpr bool PhyMxScalePosIsUB()
{
    if constexpr (HasScalePosition<INPUT_TYPE>::value) {
        return PhyPosIsUB(INPUT_TYPE::scalePosition);
    }
    return false;
}

template <typename INPUT_TYPE>
__aicore__ constexpr bool PhyMxScalePosIsGM()
{
    if constexpr (HasScalePosition<INPUT_TYPE>::value) {
        return PhyPosIsGM(INPUT_TYPE::scalePosition);
    }
    return false;
}
#endif

template <typename T>
__aicore__ constexpr int32_t GetBitSize()
{
    if constexpr (std::is_arithmetic<T>::value) {
        return sizeof(T) * ONE_BYTE_BIT_SIZE;
    }
    if constexpr (IsSameTypeV<T, AscendC::int4b_t>) {
        return ONE_BYTE_BIT_SIZE / 2;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    if constexpr (IsTypeOneOfV<T, fp8_e8m0_t, hifloat8_t, fp8_e4m3fn_t, fp8_e5m2_t>) {
        return ONE_BYTE_BIT_SIZE;
    }
    if constexpr (IsTypeOneOfV<T, fp4x2_e2m1_t, fp4x2_e1m2_t>) {
        return ONE_BYTE_BIT_SIZE / 2;
    }
#endif
#if __NPU_ARCH__ == 5102
    if (IsSameTypeV<T, AscendC::int2b_t>) {
        return ONE_BYTE_BIT_SIZE / 4;
    }
#endif

    return ONE_BYTE_BIT_SIZE * 2;
}

template <typename T>
constexpr bool IsScaleTransWithInlv =
    (HasScalePosition<T>::value && PhyPosIsGM(T::pos) && (T::format == CubeFormat::ND) && PhyPosIsL1(T::scalePosition));

template <typename A_TYPE, typename B_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr bool IsL1BNeedTrans()
{
    if constexpr (!Impl::Detail::MatmulFeatureTrait<MM_CFG>::IsMmadInstrSupportAntiQuant()) {
        if constexpr (GetBitSize<typename B_TYPE::T>() == GetBitSize<typename A_TYPE::T>()) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}

template <typename A_TYPE, typename B_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr auto GetTransBDataType()
{
    if constexpr (HasScalePosition<A_TYPE>::value) {
        B_TYPE mxBType;
        return mxBType;
    }
#if __NPU_ARCH__ == 5102
    else if constexpr (
        DecompMode(MM_CFG) == DecompressionMode::DECOMP_1bitTo4bit ||
        DecompMode(MM_CFG) == DecompressionMode::DECOMP_2bitTo4bit) {
        MatmulType<TPosition::GM, CubeFormat::NZ, int4b_t> bType;
        return bType;
    } else if constexpr (DecompMode(MM_CFG) == DecompressionMode::DECOMP_4bitTo8bit) {
        MatmulType<TPosition::GM, CubeFormat::NZ, int8_t> bType;
        return bType;
    }
#else
    else if constexpr (IsL1BNeedTrans<A_TYPE, B_TYPE, MM_CFG>()) {
        A_TYPE aType;
        return aType;
    }
#endif
    else {
        B_TYPE bType;
        return bType;
    }
}
template <typename INPUT_TYPE>
__aicore__ inline constexpr bool IsScaleTag()
{
    return INPUT_TYPE::TAG == InputTypeTag::scaleA || INPUT_TYPE::TAG == InputTypeTag::scaleB;
}

template <typename INPUT_TYPE>
__aicore__ inline constexpr bool InputPhyPosIsGM()
{
    if constexpr (IsScaleTag<INPUT_TYPE>()) {
        return PhyPosIsGM(INPUT_TYPE::scalePosition);
    } else {
        return PhyPosIsGM(INPUT_TYPE::pos);
    }
}

template <typename INPUT_TYPE>
__aicore__ inline constexpr bool InputPhyPosIsL1()
{
    if constexpr (IsScaleTag<INPUT_TYPE>()) {
        return PhyPosIsL1(INPUT_TYPE::scalePosition);
    } else {
        return PhyPosIsL1(INPUT_TYPE::pos);
    }
}

template <typename INPUT_TYPE>
__aicore__ inline constexpr bool InputPhyPosIsUB()
{
    if constexpr (IsScaleTag<INPUT_TYPE>()) {
        return PhyPosIsUB(INPUT_TYPE::scalePosition);
    } else {
        return PhyPosIsUB(INPUT_TYPE::pos);
    }
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
template <typename T>
constexpr bool SupportMXFP8 = IsTypeOneOfV<T, fp8_e4m3fn_t, fp8_e5m2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>;
#else
template <typename T>
constexpr bool SupportMXFP8 = false;
#endif
template <typename AType, typename BType, const auto& MM_CFG>
constexpr bool IsMxDisableUnitFlag =
    (EnUnitFlag(MM_CFG) && HasScalePosition<AType>::value &&
     (AType::isTrans || !BType::isTrans || IsStaticPaddingEnable(MM_CFG)) && SupportMXFP8<typename AType::T>);
} // namespace AscendC
#endif // _MATMUL_UTILS_H_
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MX_MATMUL_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MX_MATMUL_UTILS_H__
#endif
