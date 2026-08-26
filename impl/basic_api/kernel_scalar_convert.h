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
 * \file kernel_scalar_convert.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_scalar_convert.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_SCALAR_CONVERT_H__
#endif
#ifndef ASCENDC_SCALAR_CONVERT_H
#define ASCENDC_SCALAR_CONVERT_H
#include "kernel_type_conversion_utils.h"
#include "../utils/std/type_traits/is_same.h"
#include "../utils/std/type_traits/enable_if.h"

namespace AscendC {
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
__aicore__ inline bfloat16_t Cast(const float& fVal)
{
    constexpr uint32_t fp32SignIdx = 31;
    constexpr uint32_t fp32ManLen = 23;
    constexpr int16_t bf16ExpMask = 0x7F80;
    constexpr uint16_t bf16AbsMax = 0x7FFFu;
    constexpr uint16_t bf16ManHideBit = 0x0080u;
    constexpr uint32_t bf16ManLen = 7;
    constexpr int16_t bf16MaxMan = 0x07F;
    constexpr int8_t bf16ManMask = 0x7F;
    constexpr uint16_t bf16MaxExp = 0xFFu;
    constexpr uint32_t fp32ManHideBit = 0x00800000u;
    constexpr uint8_t bf16SignIndex = 15;

    float fpVal = fVal;
    uint32_t u32Val = AscendC::GetScalarBitcodeValue<float, uint32_t>(fpVal); // 1:8:23bit sign:exp:man
    uint16_t u16Num = 0;
    uint16_t sign = (u32Val >> fp32SignIdx) & 1;
    if (IsFp32Inf(u32Val)) {
        u16Num = (bf16ExpMask | (sign << bf16SignIndex));
        return GetScalarBitcodeValue<uint16_t, bfloat16_t>(u16Num);
    }

    if (IsFp32Nan(u32Val)) {
        u16Num = bf16AbsMax;
        return GetScalarBitcodeValue<uint16_t, bfloat16_t>(u16Num);
    }
    uint32_t expFp = ((u32Val)&FP32_EXP_PART_MASK) >> fp32ManLen;
    uint32_t manFp = (u32Val & FP32_MAN_PART_MASK); // 23 bit mantissa don't need to care about denormal
    manFp = (manFp | fp32ManHideBit);

    constexpr uint32_t mLenDelta = fp32ManLen - bf16ManLen;
    uint16_t eRet = static_cast<uint16_t>(expFp);
    uint16_t mRet = static_cast<uint16_t>(manFp >> mLenDelta);

    bool needRound = IsBf16NeedRound(manFp);
    if (needRound) {
        if ((eRet == (bf16MaxExp - 1)) && ((mRet & bf16ManMask) == bf16MaxMan)) {
            u16Num = (bf16ExpMask | (sign << bf16SignIndex));
            return GetScalarBitcodeValue<uint16_t, bfloat16_t>(u16Num);
        }
        ++mRet;
    }

    if (mRet > (bf16ManHideBit | bf16ManMask)) {
        ++eRet;
    }
    u16Num = ((sign) << bf16SignIndex) | ((eRet) << bf16ManLen) | ((mRet)&bf16MaxMan);
    return GetScalarBitcodeValue<uint16_t, bfloat16_t>(u16Num);
}

// ToBfloat16 has been updated, please use Cast instead.
__aicore__ inline bfloat16_t ToBfloat16(const float& fVal) { return Cast(fVal); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <
    typename T, typename U = float,
    typename = Std::enable_if_t<
        (Std::is_same<T, bfloat16_t>::value || Std::is_same<T, hifloat8_t>::value ||
         Std::is_same<T, fp8_e5m2_t>::value || Std::is_same<T, fp8_e4m3fn_t>::value ||
         Std::is_same<T, fp4x2_e1m2_t>::value || Std::is_same<T, fp4x2_e2m1_t>::value),
        void>>
__aicore__ constexpr inline U Cast(T bVal)
{
    static_assert(
        SupportType<T, bfloat16_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp4x2_e1m2_t, fp4x2_e2m1_t>(),
        "Cast to float only support bfloat16_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/fp4x2_e1m2_t/fp4x2_e2m1_t data type "
        "on current device!");
    static_assert(SupportType<U, float>(), "Cast to float only support return float");
    uint8_t uiNum = 0;
    uint32_t result = 0;
    float fNum = 0;

    if constexpr (IsSameType<T, bfloat16_t>::value) {
        fNum = FPTranslation::Bf16ToFp32(bVal);
    } else if constexpr (IsSameType<T, hifloat8_t>::value) {
        uiNum = GetScalarBitcodeValue<hifloat8_t, uint8_t>(bVal);
        result = FPTranslation::Hif8ToFp32(uiNum);
        return GetScalarBitcodeValue<uint32_t, float>(result);
    } else if constexpr (IsSameType<T, fp8_e5m2_t>::value) {
        uiNum = GetScalarBitcodeValue<fp8_e5m2_t, uint8_t>(bVal);
        result = FPTranslation::Fp8e5m2ToFp32(uiNum);
        return GetScalarBitcodeValue<uint32_t, float>(result);
    } else if constexpr (IsSameType<T, fp8_e4m3fn_t>::value) {
        uiNum = GetScalarBitcodeValue<fp8_e4m3fn_t, uint8_t>(bVal);
        result = FPTranslation::Fp8e4m3ToFp32(uiNum);
        return GetScalarBitcodeValue<uint32_t, float>(result);
    } else if constexpr (IsSameType<T, fp4x2_e1m2_t>::value) {
        uiNum = GetScalarBitcodeValue<fp4x2_e1m2_t, uint8_t>(bVal);
        bfloat16_t bf16Val = FPTranslation::Fp4e1m2ToBfloat(uiNum);
        return FPTranslation::Bf16ToFp32(bf16Val);
    } else {
        uiNum = GetScalarBitcodeValue<T, uint8_t>(bVal);
        bfloat16_t bf16Val = FPTranslation::Fp4e2m1ToBfloat(uiNum);
        return FPTranslation::Bf16ToFp32(bf16Val);
    }
    return fNum;
}

// ToFloat has been updated, please use Cast instead.
template <typename T>
__aicore__ constexpr inline float ToFloat(const T& bVal)
{
    static_assert(
        SupportType<T, bfloat16_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp4x2_e1m2_t, fp4x2_e2m1_t>(),
        "ToFloat only support bfloat16_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/fp4x2_e1m2_t/fp4x2_e2m1_t data type on "
        "current device!");
    return Cast<T, float>(bVal);
}

#else
__aicore__ inline float Cast(const bfloat16_t& bVal)
{
    bfloat16_t bNum = bVal;
    union ToFloatUnion {
        __aicore__ ToFloatUnion() {}
        uint32_t val;
        float fNum;
    } floatUnion;
    union ToUint16Union {
        __aicore__ ToUint16Union() {}
        bfloat16_t uret;
        uint16_t num;
    } u16Union;
    u16Union.uret = bNum;
    floatUnion.val = u16Union.num << BF16_TO_FP32_MAN_LEN;
    return floatUnion.fNum;
}

// ToFloat has been updated, please use Cast instead.
__aicore__ inline float ToFloat(const bfloat16_t& bVal) { return Cast(bVal); }
#endif
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                                __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
template <typename T>
__aicore__ constexpr inline float Cast(const T& bVal)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport Cast to float"); });
    return 0.0f;
}

// ToFloat has been updated, please use Cast instead.
template <typename T>
__aicore__ constexpr inline float ToFloat(const T& bVal)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport ToFloat"); });
    return 0.0f;
}

__aicore__ inline bfloat16_t Cast(const float& fVal)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport Cast to bfloat16"); });
    bfloat16_t bNum;
    return bNum;
}

// ToBfloat16 has been updated, please use Cast instead.
__aicore__ inline bfloat16_t ToBfloat16(const float& fVal)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport ToBfloat"); });
    bfloat16_t bNum;
    return bNum;
}

#endif
} // namespace AscendC
#endif // ASCENDC_SCALAR_CONVERT_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_SCALAR_CONVERT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_SCALAR_CONVERT_H__
#endif
