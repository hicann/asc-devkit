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
 * \file kernel_scalar.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_scalar.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_SCALAR_H__
#endif
#ifndef ASCENDC_KERNEL_SCALAR_IMPL_H
#define ASCENDC_KERNEL_SCALAR_IMPL_H

namespace AscendC {
template <int countValue>
__aicore__ inline int64_t GetBitCountImpl(uint64_t valueIn)
{
    if constexpr (countValue == 1) {
        return bcnt1(valueIn);
    } else if constexpr (countValue == 0) {
        return bcnt0(valueIn);
    } else {
        static_assert(((countValue == 0) || (countValue == 1)) && "countValue must be 1 or 0");
        return 0;
    }
}

__aicore__ inline int64_t CountLeadingZeroImpl(uint64_t valueIn) { return clz(valueIn); }

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
__aicore__ inline uint64_t GetUintDivMagic(uint64_t dividend, uint64_t divisor)
{
    uint64_t quotient = 0;
    uint64_t remainder = dividend;
    uint64_t borrow = 0;

    // handle low 64 bit
    for (int i = 0; i < ConstantsInternal::BIT_64_LEN; i++) {
        quotient <<= 1;
        borrow = (remainder & ConstantsInternal::UINT_64_HIGHEST_BIT_MASK) > 0 ? 1 : 0;
        remainder = (remainder << 1);

        if (borrow == 1) {
            remainder = ConstantsInternal::UINT_64_MAX - divisor + 1 + remainder;
            quotient |= 1;
        } else if (remainder >= divisor) {
            remainder -= divisor;
            quotient |= 1;
        }
    }

    return quotient + 1;
}

template <typename T>
__aicore__ inline void GetUintDivMagicAndShiftImpl(T& magic, T& shift, T divisor)
{
    static_assert(SupportType<T, uint32_t, uint64_t>(), "Input type T only supports uint32_t, uint64_t.");

    int64_t pos = ConstantsInternal::BIT_64_LEN - CountLeadingZeroImpl(divisor);
    int64_t cnt1 = GetBitCountImpl<1>(divisor);
    shift = cnt1 == 1 ? pos - 1 : pos;

    if constexpr (std::is_same<T, uint32_t>::value) {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT(divisor <= ConstantsInternal::INT_32_MAX && divisor > 0, {
            KERNEL_LOG(KERNEL_ERROR, "divisor must not be greater than INT32_MAX");
        });
#endif
        magic = (1l << ConstantsInternal::BIT_32_LEN) * ((1l << shift) - divisor) / divisor + 1;
    } else if constexpr (std::is_same<T, uint64_t>::value) {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT(divisor <= ConstantsInternal::INT_64_MAX && divisor > 0, {
            KERNEL_LOG(KERNEL_ERROR, "divisor must not be greater than INT_64_MAX");
        });
#endif
        uint64_t dividend = 0;
        if (shift < ConstantsInternal::BIT_64_LEN) {
            dividend = (1l << shift) - divisor;
        } else if (shift == ConstantsInternal::BIT_64_LEN) {
            // divisor must be greater than 0, so will not overflow
            dividend = ConstantsInternal::UINT_64_MAX - divisor + 1;
        } else {
            KERNEL_LOG(KERNEL_ERROR, "Shift must be in [0, 64].");
            return;
        }
        magic = GetUintDivMagic(dividend, divisor);
    }
}
#endif

__aicore__ inline int64_t CountBitsCntSameAsSignBitImpl(int64_t valueIn) { return sflbits(valueIn); }

template <int countValue>
__aicore__ inline int64_t GetSFFValueImpl(uint64_t valueIn)
{
    if constexpr (countValue == 1) {
        return sff1(valueIn);
    } else if constexpr (countValue == 0) {
        return sff0(valueIn);
    } else {
        static_assert(((countValue == 0) || (countValue == 1)) && "countValue must be 1 or 0");
        return 0;
    }
}

#if __NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T>
__aicore__ inline void WriteGmByPassDCacheImpl(__gm__ T* addr, T value)
{
    static_assert(
        SupportType<T, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>(),
        "WriteGmByPassDCache only support int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t "
        "data type on current device!");

    if constexpr (SupportBytes<T, 8>()) {
        st_dev(*(reinterpret_cast<uint64_t*>(&value)), reinterpret_cast<__gm__ uint64_t*>(addr), 0);
    } else if constexpr (SupportBytes<T, 4>()) {
        st_dev(*(reinterpret_cast<uint32_t*>(&value)), reinterpret_cast<__gm__ uint32_t*>(addr), 0);
    } else if constexpr (SupportBytes<T, 2>()) {
        st_dev(*(reinterpret_cast<uint16_t*>(&value)), reinterpret_cast<__gm__ uint16_t*>(addr), 0);
    } else {
        st_dev(*(reinterpret_cast<uint8_t*>(&value)), reinterpret_cast<__gm__ uint8_t*>(addr), 0);
    }
}

template <typename T>
__aicore__ inline T ReadGmByPassDCacheImpl(__gm__ T* addr)
{
    static_assert(
        SupportType<T, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>(),
        "ReadGmByPassDCache only support int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t "
        "data type on current device!");

    if constexpr (SupportBytes<T, 8>()) {
        return ld_dev(reinterpret_cast<__gm__ uint64_t*>(addr), 0);
    } else if constexpr (SupportBytes<T, 4>()) {
        return ld_dev(reinterpret_cast<__gm__ uint32_t*>(addr), 0);
    } else if constexpr (SupportBytes<T, 2>()) {
        return ld_dev(reinterpret_cast<__gm__ uint16_t*>(addr), 0);
    } else {
        return ld_dev(reinterpret_cast<__gm__ uint8_t*>(addr), 0);
    }
}
#endif

template <RoundMode roundMode>
__aicore__ inline half CastF322F16Impl(float valueIn)
{
    switch (roundMode) {
        case RoundMode::CAST_ODD:
            return conv_f322f16o(valueIn);
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            return 0;
    }
}

template <RoundMode roundMode>
__aicore__ inline int32_t CastF322S32Impl(float valueIn)
{
    switch (roundMode) {
        case RoundMode::CAST_ROUND:
            return conv_f322s32a(valueIn);
        case RoundMode::CAST_CEIL:
            return conv_f322s32c(valueIn);
        case RoundMode::CAST_FLOOR:
            return conv_f322s32f(valueIn);
        case RoundMode::CAST_RINT:
            return conv_f322s32r(valueIn);
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            return 0;
    }
}

template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U CastImpl(T valueIn)
{
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3113) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    if constexpr (std::is_same<U, half>::value) {
        return CastF322F16Impl<roundMode>(valueIn);
    } else if constexpr (std::is_same<U, int32_t>::value) {
        return CastF322S32Impl<roundMode>(valueIn);
    } else {
        static_assert(
            ((sizeof(U) == sizeof(half)) || (sizeof(U) == sizeof(int32_t))), "U only support half or int32_t");
        return 0;
    }
#else
    ASCENDC_ASSERT((false), "Cast is not supported on current device");
    return 0;
#endif
}

template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U ScalarCastImpl(T valueIn)
{
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 2103) || \
     (__NPU_ARCH__ == 3103) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    return CastImpl<T, U, roundMode>(valueIn);
#else
    ASCENDC_ASSERT((false), "ScalarCast is not supported on current device");
    return 0;
#endif
}
} // namespace AscendC
#endif // ASCENDC_KERNEL_SCALAR_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_SCALAR_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_SCALAR_H__
#endif
