/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_simt_common_impl.h
 * \brief
 */
#ifndef IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_COMMON_IMPL_H
#define IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_COMMON_IMPL_H

#include <type_traits>
#include "impl/simt_api/cpp/dav_3510/kernel_simt_cmp_impl.h"

namespace AscendC {

namespace Simt {

template <typename T, typename... Args>
constexpr bool SupportTypeSimtInternel = (std::is_same_v<T, Args> || ...);

typedef bfloat16_t bhalf;

constexpr int32_t THREAD_GROUP_SIZE = 32;

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t GetWarpSizeImpl() { return THREAD_GROUP_SIZE; }

template <int32_t dim = 0>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t GetThreadNumImpl()
{
    if constexpr (dim == 0) {
        return blockDim.x;
    } else if constexpr (dim == 1) {
        return blockDim.y;
    } else if constexpr (dim == 2) {
        return blockDim.z;
    }
    return 0;
}

template <int32_t dim = 0>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t GetThreadIdxImpl()
{
    static_assert((dim >= 0 && dim <= 2), "dim is out of range [0, 2]");
    if constexpr (dim == 0) {
        return threadIdx.x;
    } else if constexpr (dim == 1) {
        return threadIdx.y;
    } else if constexpr (dim == 2) {
        return threadIdx.z;
    }
    return 0;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t GetBlockIdxImpl() { return blockIdx.x; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t GetBlockNumImpl() { return gridDim.x; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t GetBf16U16(
    float f32, uint32_t u16, uint32_t u32, uint32_t bf16LastBit, RoundMode rnd)
{
    uint32_t carryOutBit = 0x10000;
    uint32_t topU32 = (u32 & ConstantsInternal::HIGH_16_BIT) + carryOutBit;
    uint32_t botU32 = (u32 & ConstantsInternal::HIGH_16_BIT);
#if (__NPU_ARCH__ == 3510)
    if (rnd == RoundMode::CAST_EVEN) {
        if (topU32 - u32 > u32 - botU32) {
            u16 = botU32 >> ConstantsInternal::U16_BIT;
        } else if (topU32 - u32 < u32 - botU32) {
            u16 = topU32 >> ConstantsInternal::U16_BIT;
        } else {
            if (bf16LastBit == 0) {
                u16 = botU32 >> ConstantsInternal::U16_BIT;
            } else {
                u16 = topU32 >> ConstantsInternal::U16_BIT;
            }
        }
    } else if (rnd == RoundMode::CAST_FLOOR) {
#else
    if (rnd == RoundMode::CAST_FLOOR) {
#endif
        if (f32 > 0) {
            u16 = botU32 >> ConstantsInternal::U16_BIT;
        } else {
            u16 = topU32 >> ConstantsInternal::U16_BIT;
        }
    } else if (rnd == RoundMode::CAST_CEIL) {
        if (f32 > 0) {
            u16 = topU32 >> ConstantsInternal::U16_BIT;
        } else {
            u16 = botU32 >> ConstantsInternal::U16_BIT;
        }
#if (__NPU_ARCH__ == 3510)
    } else if (rnd == RoundMode::CAST_ZERO) {
        u16 = botU32 >> ConstantsInternal::U16_BIT;
#endif
    }
    return u16;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t F32ToBf16(float f32, RoundMode rnd, bool satMode)
{
    if (IsNanImpl(f32)) {
        if (satMode) {
            return 0;
        }
        return f32;
    }
    uint32_t* u32ptr = (uint32_t*)&f32;
    uint32_t u32 = *u32ptr;
    uint32_t bf16LastBit = (u32 >> ConstantsInternal::U16_BIT) & 0x1;
    bfloat16_t sign = (f32 >= 0) ? 1 : -1;
    uint32_t u16 = 0;
    if (f32 == 0) {
        return 0;
    }
    bfloat16_t bf16MaxVal = 3.3895313e+38;
    if (IsInfImpl(f32)) {
        if (satMode) {
            return sign * bf16MaxVal;
        }
        return sign * ConstantsInternal::SIMT_FP32_INF;
    }
    u16 = GetBf16U16(f32, u16, u32, bf16LastBit, rnd);
    bfloat16_t bf16 = 0;
    uint16_t* u16ptr = (uint16_t*)&bf16;
    *u16ptr = u16;
    if (((u16 == ConstantsInternal::B_HALF_INF) || (u16 == ConstantsInternal::B_HALF_NEG_INF))) {
        if (satMode) {
            bf16 = sign * bf16MaxVal;
        } else {
            bf16 = sign * ConstantsInternal::SIMT_FP32_INF;
        }
    }
    return bf16;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half F32Tof16(float f32, RoundMode rnd)
{
    uint32_t* u32Ptr = (uint32_t*)&f32;
    uint32_t u32 = *u32Ptr;
    int32_t exponent = u32 & ConstantsInternal::INF;
    uint32_t f32ManU32 = u32 & 0x7fffff;
    int32_t expDiff = 0;
    int32_t f32ExpVal = (exponent - 0x3f800000) >> 23;
    if (f32ExpVal < ConstantsInternal::F16_MIN_VAL) {
        int32_t maxShiftRBit = 12;
        // -14 - f32ExpVal gt 12 => -f32ExpVal gt 26
        expDiff = (-f32ExpVal > 0x1a) ? maxShiftRBit : ConstantsInternal::F16_MIN_VAL - f32ExpVal;
        f32ManU32 += 0x800000;
    }
    uint32_t disBitLen = 0xd + expDiff;
    uint32_t botManU32 = (f32ManU32 >> disBitLen) << disBitLen;
    uint32_t topManU32 = botManU32 + (1 << disBitLen);
    bool isCarryOut = false;
#if (__NPU_ARCH__ == 3510)
    if (rnd == RoundMode::CAST_EVEN) {
        uint32_t midBit = 1 << (disBitLen - 1);
        if (topManU32 - f32ManU32 < midBit) {
            isCarryOut = true;
        } else if (topManU32 - f32ManU32 == midBit) {
            isCarryOut = (f32ManU32 >> disBitLen) & 0x1;
        }
    } else if (rnd == RoundMode::CAST_FLOOR) {
#else
    if (rnd == RoundMode::CAST_FLOOR) {
#endif
        isCarryOut = f32ManU32 != botManU32 && f32 < 0;
    } else if (rnd == RoundMode::CAST_CEIL) {
        isCarryOut = f32ManU32 != botManU32 && f32 > 0 && ConstantsInternal::F16_MIN_VAL <= f32ExpVal;
    }
    uint32_t rndU32 = 0;
    rndU32 = isCarryOut ? topManU32 : botManU32;
    uint32_t f16Man = (rndU32 >> disBitLen) & 0x3ff;
    int32_t f16ExpVal = f32ExpVal + 0xf + (topManU32 == 0x800000 && isCarryOut);
    uint16_t u16 = 0;
    uint32_t sign = (f32 < 0) ? 0x8000 : 0;
    if (exponent == ConstantsInternal::INF && f32ManU32 > 0) {
        u16 = 0x7E00;
    } else if (f32 == 0) {
        u16 = sign ^ 0;
    } else if (isCarryOut && expDiff > ConstantsInternal::F16_MAN_BIT_LEN) {
        u16 = sign ^ isCarryOut;
    } else if (f16ExpVal >= 0x1f) {
        u16 = sign ^ 0x7c00;
    } else if (f16ExpVal < 0) {
        u16 = sign ^ f16Man;
    } else {
        u16 = sign ^ ((f16ExpVal << ConstantsInternal::F16_MAN_BIT_LEN) + f16Man);
    }
    half f16 = 0;
    uint16_t* u16ptr = (uint16_t*)&f16;
    *u16ptr = u16;
    return f16;
}

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline half RoundInf(half x) { return x; }
#endif
} // namespace Simt
} // namespace AscendC

#if defined(ASCENDC_CPU_DEBUG)
#include <cmath>

#include "../../../basic_api/kernel_utils.h"
#include "stub_def.h"
#endif

namespace AscendC {
namespace Simt {

#if defined(ASCENDC_CPU_DEBUG)
// Find the largest power of 2 that is less than x
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T FindNear2ndPow(T x)
{
    if ((T)2 >= x) {
        return (T)1;
    } else {
        return (T)pow(2, static_cast<uint32_t>(log2(x)));
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t FloorIntrinsicsImpl(bfloat16_t x)
{
    if (IsInfImpl(x)) {
        return x;
    }
    uint16_t* uintX = (uint16_t*)(&x);
    uint16_t bf16Sign15Bit = 0xf;
    uint16_t bf16Frac7Bit = 0x7;
    int8_t sign = ((((*uintX) >> bf16Sign15Bit) & 0x1) == 0) ? 1 : -1;
    uint16_t exp = ((*uintX) >> bf16Frac7Bit) & 0xff;
    bfloat16_t frac = (*uintX) & 0x7f;
    bfloat16_t two = 2.0;
    uint16_t bfl16Exp126Bit = 0x7e;
    uint16_t bfl16Exp127Bit = 0x7f;
    if (exp == 0xff && frac == 0) {
        return (bfloat16_t)(sign * INFINITY);
    } else if (exp == 0xff && frac != 0) {
        return (bfloat16_t)(sign * NAN);
    } else {
        bfloat16_t base;
        bfloat16_t minVal;
        if (exp == 0) {
            base = sign * (pow(two, (exp - bfl16Exp126Bit)) * (frac / (pow(two, bf16Frac7Bit))));
            if (exp - bfl16Exp126Bit > 0) {
                minVal = sign * (pow(two, (exp - bfl16Exp126Bit)));
            } else {
                minVal = 0;
            }
        } else {
            base = sign * (pow(two, (exp - bfl16Exp127Bit)) * (1 + (frac / (pow(two, bf16Frac7Bit)))));
            if (exp - bfl16Exp127Bit > 0) {
                minVal = sign * (pow(two, (exp - bfl16Exp127Bit)));
            } else {
                minVal = 0;
            }
        }
        if (base > minVal) {
            while (base - minVal >= 1) {
                minVal += FindNear2ndPow(base - minVal);
            }
            return minVal;
        } else if (base < minVal) {
            while (minVal - base > 0) {
                minVal -= FindNear2ndPow(minVal - base);
            }
            return minVal;
        } else {
            return base;
        }
    }
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T FloorIntrinsicsImpl(T x)
{
    if constexpr (SupportTypeSimtInternel<T, int32_t, int64_t>) {
        return x;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float FloorIntrinsicsImpl(float x) { return floor(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half FloorIntrinsicsImpl(half x)
{
    if (IsInfImpl(x)) {
        return RoundInf(x);
    }
    return half(floor(float(x)));
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType Floor_(SrcType x)
{
    if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<bfloat16_t, float>>) {
        return F32ToBf16(x, RoundMode::CAST_FLOOR, false);
    } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<half, float>>) {
        return F32Tof16(x, RoundMode::CAST_FLOOR);
    } else {
        if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int32_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S32_MAX_VAL)) {
                return ConstantsInternal::S32_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S32_MIN_VAL)) {
                return ConstantsInternal::S32_MIN_VAL;
            }
        } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int64_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S64_MAX_VAL)) {
                return ConstantsInternal::S64_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S64_MIN_VAL)) {
                return ConstantsInternal::S64_MIN_VAL;
            }
        }
        return (DstType)FloorIntrinsicsImpl(x);
    }
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T FloorIntrinsicsImpl(T x)
{
    if constexpr (SupportTypeSimtInternel<T, int32_t, int64_t>) {
        return x;
    } else if constexpr (SupportTypeSimtInternel<T, half, float, bfloat16_t>) {
        return __floorf(x);
    }
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType Floor_(SrcType x)
{
    if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<bfloat16_t, float>>) {
        return F32ToBf16(x, RoundMode::CAST_FLOOR, false);
    } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<half, float>>) {
        return F32Tof16(x, RoundMode::CAST_FLOOR);
    } else {
        SrcType res = FloorIntrinsicsImpl(x);
        return (DstType)(res);
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void Floor_(half2& dst, float2& src)
{
    dst = __cvt_half2<ROUND::F, RoundingSaturation::RS_DISABLE_VALUE>(src);
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t RoundIntrinsicsImpl(bfloat16_t x)
{
    if (IsInfImpl(x)) {
        return x;
    }
    bfloat16_t sign = 1;
    if (x < 0) {
        sign = -1;
        x = (bfloat16_t)0 - x;
    }
    bfloat16_t floorX = FloorIntrinsicsImpl(x);
    bfloat16_t midVal = 0.5;
    if (x - floorX >= midVal) {
        return sign * (floorX + (bfloat16_t)1);
    } else {
        return sign * floorX;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float RoundIntrinsicsImpl(float x) { return round(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half RoundIntrinsicsImpl(half x)
{
    if (IsInfImpl(x)) {
        return RoundInf(x);
    }
    return half(round(float(x)));
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType RoundImpl(SrcType x)
{
    return (DstType)RoundIntrinsicsImpl(x);
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RoundIntrinsicsImpl(T x)
{
    if constexpr (SupportTypeSimtInternel<T, int32_t, int64_t>) {
        return x;
    } else if constexpr (SupportTypeSimtInternel<T, half, float, bfloat16_t, hifloat8_t>) {
        return __roundf(x);
    }
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType RoundImpl(SrcType x)
{
    SrcType res = RoundIntrinsicsImpl(x);
    return (DstType)(res);
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t RintIntrinsicsImpl(bfloat16_t x)
{
    if (IsInfImpl(x)) {
        return x;
    }
    bfloat16_t floorX = FloorIntrinsicsImpl(x);
    if (x == floorX) {
        return x;
    }
    bfloat16_t ceilX = floorX + (bfloat16_t)1;
    bfloat16_t midVal = 0.5;
    if (x - floorX > midVal) {
        return ceilX;
    } else if (x - floorX < midVal) {
        return floorX;
    } else {
        // Same Distance Round to Even
        uint16_t two = 2;
        if (((int32_t)ceilX) % two == 0) {
            return ceilX;
        } else {
            return floorX;
        }
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float RintIntrinsicsImpl(float x) { return rint(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half RintIntrinsicsImpl(half x)
{
    if (IsInfImpl(x)) {
        return RoundInf(x);
    }
    return half(rint(float(x)));
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType Rint_(SrcType x)
{
    if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<bfloat16_t, float>>) {
#if (__NPU_ARCH__ == 3510)
        return F32ToBf16(x, RoundMode::CAST_EVEN, false);
#endif
    } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<half, float>>) {
#if (__NPU_ARCH__ == 3510)
        return F32Tof16(x, RoundMode::CAST_EVEN);
#endif
    } else {
        if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int32_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S32_MAX_VAL)) {
                return ConstantsInternal::S32_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S32_MIN_VAL)) {
                return ConstantsInternal::S32_MIN_VAL;
            }
        } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int64_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S64_MAX_VAL)) {
                return ConstantsInternal::S64_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S64_MIN_VAL)) {
                return ConstantsInternal::S64_MIN_VAL;
            }
        }
        return (DstType)RintIntrinsicsImpl(x);
    }
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RintIntrinsicsImpl(T x)
{
    if constexpr (SupportTypeSimtInternel<T, int32_t, int64_t>) {
        return x;
    } else if constexpr (SupportTypeSimtInternel<T, half, float, bfloat16_t>) {
        return __rintf(x);
    }
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType Rint_(SrcType x)
{
    if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<bfloat16_t, float>>) {
#if (__NPU_ARCH__ == 3510)
        return F32ToBf16(x, RoundMode::CAST_EVEN, false);
#endif
    } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<half, float>>) {
#if (__NPU_ARCH__ == 3510)
        return F32Tof16(x, RoundMode::CAST_EVEN);
#endif
    } else {
        return static_cast<DstType>(RintIntrinsicsImpl(x));
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void Rint_(half2& dst, float2& src)
{
    dst = __cvt_half2<ROUND::R, RoundingSaturation::RS_DISABLE_VALUE>(src);
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t CeilIntrinsicsImpl(bfloat16_t x)
{
    bfloat16_t floorX = FloorIntrinsicsImpl(x);
    if (floorX == x) {
        return floorX;
    }
    return floorX + (bfloat16_t)1;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CeilIntrinsicsImpl(T x)
{
    if constexpr (SupportTypeSimtInternel<T, int32_t, int64_t>) {
        return x;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CeilIntrinsicsImpl(float x) { return ceil(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half CeilIntrinsicsImpl(half x)
{
    if (IsInfImpl(x)) {
        return RoundInf(x);
    }
    return half(ceil(float(x)));
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType Ceil_(SrcType x)
{
    if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<bfloat16_t, float>>) {
        return F32ToBf16(x, RoundMode::CAST_CEIL, false);
    } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<half, float>>) {
        return F32Tof16(x, RoundMode::CAST_CEIL);
    } else {
        if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int32_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S32_MAX_VAL)) {
                return ConstantsInternal::S32_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S32_MIN_VAL)) {
                return ConstantsInternal::S32_MIN_VAL;
            }
        } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int64_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S64_MAX_VAL)) {
                return ConstantsInternal::S64_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S64_MIN_VAL)) {
                return ConstantsInternal::S64_MIN_VAL;
            }
        }
        return (DstType)CeilIntrinsicsImpl(x);
    }
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CeilIntrinsicsImpl(T x)
{
    if constexpr (SupportTypeSimtInternel<T, int32_t, int64_t>) {
        return x;
    } else if constexpr (SupportTypeSimtInternel<T, half, float, bfloat16_t>) {
        return __ceilf(x);
    }
}

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType Ceil_(SrcType x)
{
    if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<bfloat16_t, float>>) {
        return F32ToBf16(x, RoundMode::CAST_CEIL, false);
    } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<half, float>>) {
        return F32Tof16(x, RoundMode::CAST_CEIL);
    } else {
        SrcType res = CeilIntrinsicsImpl(x);
        return (DstType)(res);
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void Ceil_(half2& dst, float2& src)
{
    dst = __cvt_half2<ROUND::C, RoundingSaturation::RS_DISABLE_VALUE>(src);
}
#endif

#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void Trunc_(half2& dst, float2& src)
{
    dst = __cvt_half2<ROUND::Z, RoundingSaturation::RS_DISABLE_VALUE>(src);
}
#endif

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType Trunc_(SrcType x)
{
    if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<bfloat16_t, float>>) {
#if (__NPU_ARCH__ == 3510)
        return F32ToBf16(x, RoundMode::CAST_ZERO, false);
#endif
    } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<half, float>>) {
#if (__NPU_ARCH__ == 3510)
        return F32Tof16(x, RoundMode::CAST_ZERO);
#endif
    } else {
        DstType res;
        if (x > (SrcType)0) {
            res = static_cast<DstType>(FloorIntrinsicsImpl(x));
        } else {
            res = static_cast<DstType>(CeilIntrinsicsImpl(x));
        }
        if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int32_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S32_MAX_VAL)) {
                res = ConstantsInternal::S32_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S32_MIN_VAL)) {
                res = ConstantsInternal::S32_MIN_VAL;
            }
        } else if constexpr (SupportTypeSimtInternel<Tuple<DstType, SrcType>, Tuple<int64_t, float>>) {
            if (x >= static_cast<float>(ConstantsInternal::S64_MAX_VAL)) {
                res = ConstantsInternal::S64_MAX_VAL;
            } else if (x <= static_cast<float>(ConstantsInternal::S64_MIN_VAL)) {
                res = ConstantsInternal::S64_MIN_VAL;
            }
        }
        return res;
    }
}

#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void CastNone_(half2& dst, float2& src) {}
#endif

template <typename DstType, typename SrcType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline DstType CastNone_(SrcType x)
{
#ifdef ASCENDC_CPU_DEBUG
    if constexpr (SupportTypeSimtInternel<Tuple<SrcType, DstType>, Tuple<half, float>>) {
        if (IsInfImpl(x)) {
            return (x >= (half)0) ? ConstantsInternal::SIMT_FP32_INF : -ConstantsInternal::SIMT_FP32_INF;
        }
    }
#endif
    return (DstType)(x);
}

} // namespace Simt
} // namespace AscendC
#endif // IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_COMMON_IMPL_H
