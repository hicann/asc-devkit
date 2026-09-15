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
 * \file kernel_simt_cast_impl.h
 * \brief
 */
#ifndef IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_CAST_IMPL_H
#define IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_CAST_IMPL_H

#include "../../../basic_api/kernel_utils.h"
#include "impl/simt_api/cpp/dav_3510/kernel_simt_common_impl.h"
#include "impl/simt_api/cpp/dav_3510/kernel_simt_cast_sat_impl.h"

namespace AscendC {
namespace Simt {

#ifndef ASCENDC_CPU_DEBUG
#define REG_ROUND_VEC(type, func_name, len)                                    \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name(type& dst, type& src) \
    {                                                                          \
        for (int i = 0; i < len; i++) {                                        \
            dst[i] = func_name(src[i]);                                        \
        }                                                                      \
    }

#define REG_ROUND_VEC_(dst_type, src_type, d_type, s_type, func_name, len)                \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name##_(dst_type& dst, src_type& src) \
    {                                                                                     \
        for (int i = 0; i < len; i++) {                                                   \
            dst[i] = func_name##_<d_type, s_type>(src[i]);                                \
        }                                                                                 \
    }

#define REG_CAST_IMPL_VEC(type, func_name) \
    REG_ROUND_VEC(type##1, func_name, 1)   \
    REG_ROUND_VEC(type##2, func_name, 2)   \
    REG_ROUND_VEC(type##3, func_name, 3)   \
    REG_ROUND_VEC(type##4, func_name, 4)

#define REG_CAST_IMPL_VEC_(dst_type, src_type, func_name)                      \
    REG_ROUND_VEC_(dst_type##1, src_type##1, dst_type, src_type, func_name, 1) \
    REG_ROUND_VEC_(dst_type##2, src_type##2, dst_type, src_type, func_name, 2) \
    REG_ROUND_VEC_(dst_type##3, src_type##3, dst_type, src_type, func_name, 3) \
    REG_ROUND_VEC_(dst_type##4, src_type##4, dst_type, src_type, func_name, 4)

#define REG_CAST_HF_IMPL_VEC(type, func_name) REG_ROUND_VEC(type##2, func_name, 2)

#define REG_CAST_HF_IMPL_VEC_(dst_type, src_type, func_name) \
    REG_ROUND_VEC_(dst_type##2, src_type##2, dst_type, src_type, func_name, 2)
#else
#define REG_ROUND_VEC_1(type, func_name) \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name(type& dst, type& src) { dst.x = func_name(src.x); }

#define REG_ROUND_VEC_2(type, func_name)                                       \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name(type& dst, type& src) \
    {                                                                          \
        dst.x = func_name(src.x);                                              \
        dst.y = func_name(src.y);                                              \
    }

#define REG_ROUND_VEC_3(type, func_name)                                       \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name(type& dst, type& src) \
    {                                                                          \
        dst.x = func_name(src.x);                                              \
        dst.y = func_name(src.y);                                              \
        dst.z = func_name(src.z);                                              \
    }

#define REG_ROUND_VEC_4(type, func_name)                                       \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name(type& dst, type& src) \
    {                                                                          \
        dst.x = func_name(src.x);                                              \
        dst.y = func_name(src.y);                                              \
        dst.z = func_name(src.z);                                              \
        dst.w = func_name(src.w);                                              \
    }

#define REG_ROUND_VEC_1_(dst_type, src_type, d_type, s_type, func_name)                   \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name##_(dst_type& dst, src_type& src) \
    {                                                                                     \
        dst.x = func_name##_<d_type, s_type>(src.x);                                      \
    }

#define REG_ROUND_VEC_2_(dst_type, src_type, d_type, s_type, func_name)                   \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name##_(dst_type& dst, src_type& src) \
    {                                                                                     \
        dst.x = func_name##_<d_type, s_type>(src.x);                                      \
        dst.y = func_name##_<d_type, s_type>(src.y);                                      \
    }

#define REG_ROUND_VEC_3_(dst_type, src_type, d_type, s_type, func_name)                   \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name##_(dst_type& dst, src_type& src) \
    {                                                                                     \
        dst.x = func_name##_<d_type, s_type>(src.x);                                      \
        dst.y = func_name##_<d_type, s_type>(src.y);                                      \
        dst.z = func_name##_<d_type, s_type>(src.z);                                      \
    }

#define REG_ROUND_VEC_4_(dst_type, src_type, d_type, s_type, func_name)                   \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name##_(dst_type& dst, src_type& src) \
    {                                                                                     \
        dst.x = func_name##_<d_type, s_type>(src.x);                                      \
        dst.y = func_name##_<d_type, s_type>(src.y);                                      \
        dst.z = func_name##_<d_type, s_type>(src.z);                                      \
        dst.w = func_name##_<d_type, s_type>(src.w);                                      \
    }

#define REG_CAST_IMPL_VEC(type, func_name) \
    REG_ROUND_VEC_1(type##1, func_name)    \
    REG_ROUND_VEC_2(type##2, func_name)    \
    REG_ROUND_VEC_3(type##3, func_name)    \
    REG_ROUND_VEC_4(type##4, func_name)

#define REG_CAST_IMPL_VEC_(dst_type, src_type, func_name)                     \
    REG_ROUND_VEC_1_(dst_type##1, src_type##1, dst_type, src_type, func_name) \
    REG_ROUND_VEC_2_(dst_type##2, src_type##2, dst_type, src_type, func_name) \
    REG_ROUND_VEC_3_(dst_type##3, src_type##3, dst_type, src_type, func_name) \
    REG_ROUND_VEC_4_(dst_type##4, src_type##4, dst_type, src_type, func_name)

#define REG_CAST_HF_IMPL_VEC(type, func_name) REG_ROUND_VEC_2(type##2, func_name)

#define REG_CAST_HF_IMPL_VEC_(dst_type, src_type, func_name) \
    REG_ROUND_VEC_2_(dst_type##2, src_type##2, dst_type, src_type, func_name)
#endif

#define REG_ROUND(type, func_name) \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name(type& dst, type& src) { dst = func_name(src); }

#define REG_ROUND_(d_type, s_type, func_name)                                         \
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void func_name##_(d_type& dst, s_type& src) \
    {                                                                                 \
        dst = func_name##_<d_type, s_type>(src);                                      \
    }

#define REG_CAST_IMPL_(dst_type, src_type, func_name) REG_ROUND_(dst_type, src_type, func_name)

#define REG_CAST_VEC(round_mode)         \
    REG_CAST_IMPL_VEC(float, round_mode) \
    REG_CAST_HF_IMPL_VEC(half, round_mode)

#define REG_CAST_(round_mode)                \
    REG_CAST_IMPL_(half, float, round_mode)  \
    REG_CAST_IMPL_(int, float, round_mode)   \
    REG_CAST_IMPL_(long, float, round_mode)  \
    REG_CAST_IMPL_(bhalf, float, round_mode) \
    REG_CAST_IMPL_(float, half, round_mode)  \
    REG_CAST_IMPL_(float, int, round_mode)   \
    REG_CAST_IMPL_(float, long, round_mode)

#define REG_CAST_VEC_(round_mode)               \
    REG_CAST_IMPL_VEC_(int, float, round_mode)  \
    REG_CAST_IMPL_VEC_(long, float, round_mode) \
    REG_CAST_IMPL_VEC_(float, int, round_mode)  \
    REG_CAST_IMPL_VEC_(float, long, round_mode) \
    REG_CAST_HF_IMPL_VEC_(float, half, round_mode)

#ifdef ASCENDC_CPU_DEBUG
REG_CAST_HF_IMPL_VEC_(half, float, Rint)
REG_CAST_HF_IMPL_VEC_(half, float, Floor)
REG_CAST_HF_IMPL_VEC_(half, float, Ceil)
REG_CAST_HF_IMPL_VEC_(half, float, Trunc)
REG_CAST_HF_IMPL_VEC_(half, float, CastNone)
#endif

REG_CAST_(Rint)
REG_CAST_(Floor)
REG_CAST_(Ceil)
REG_CAST_(Trunc)
REG_CAST_(CastNone)

REG_CAST_IMPL_(float, bfloat16_t, CastNone)
REG_CAST_IMPL_(float, bfloat16_t, Ceil)
REG_CAST_IMPL_(float, bfloat16_t, Floor)
REG_CAST_IMPL_(float, bfloat16_t, Trunc)
REG_CAST_IMPL_(float, bfloat16_t, Rint)

REG_CAST_VEC_(Rint)
REG_CAST_VEC_(Floor)
REG_CAST_VEC_(Ceil)
REG_CAST_VEC_(Trunc)
REG_CAST_VEC_(CastNone)

template <typename T, typename U, RoundMode roundMode>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastFallback(U x)
{
    T y;
    switch (roundMode) {
#if (__NPU_ARCH__ == 3510)
        case RoundMode::CAST_EVEN:
            Rint_(y, x);
            break;
        case RoundMode::CAST_ZERO:
            Trunc_(y, x);
            break;
#endif
        case RoundMode::CAST_FLOOR:
            Floor_(y, x);
            break;
        case RoundMode::CAST_CEIL:
            Ceil_(y, x);
            break;
        case RoundMode::CAST_NONE:
            CastNone_(y, x);
            break;
    }
    return y;
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastImpl(U x)
{
#if defined(ASCENDC_CPU_DEBUG)
    return CastFallback<T, U, roundMode>(x);
#else
    if constexpr (
        (roundMode == RoundMode::CAST_EVEN || roundMode == RoundMode::CAST_ZERO) &&
        SupportTypeSimtInternel<
            Tuple<U, T>, Tuple<float, int>, Tuple<int, float>, Tuple<float, int64_t>, Tuple<int64_t, float>,
            Tuple<float, half>, Tuple<float, bfloat16_t> >) {
        return CastFallback<T, U, roundMode>(x);
    }
    if constexpr (
        roundMode == RoundMode::CAST_NONE &&
        SupportTypeSimtInternel<Tuple<U, T>, Tuple<half, float>, Tuple<bfloat16_t, float> >) {
        return CastFallback<T, U, roundMode>(x);
    }
    T y;
    if constexpr (SupportTypeSimtInternel<
                      Tuple<T, U>, Tuple<uint32_t, half>, Tuple<int32_t, half>, Tuple<uint32_t, float>,
                      Tuple<int32_t, float>, Tuple<uint64_t, float>, Tuple<int64_t, float>, Tuple<uint32_t, bfloat16_t>,
                      Tuple<int32_t, bfloat16_t> >) {
        y = CastSat<T, U, roundMode>(x);
    } else if constexpr (SupportTypeSimtInternel<
                             Tuple<T, U>, Tuple<half, uint32_t>, Tuple<float, uint32_t>, Tuple<bfloat16_t, uint32_t>,
                             Tuple<half, int32_t>, Tuple<float, int32_t>, Tuple<bfloat16_t, int32_t>,
                             Tuple<float, uint64_t>, Tuple<float, int64_t>, Tuple<float, half>, Tuple<bfloat16_t, half>,
                             Tuple<half, float>, Tuple<bfloat16_t, float>, Tuple<half, bfloat16_t>,
                             Tuple<float, bfloat16_t> >) {
        switch (satMode) {
            case SatMode::SAT:
                y = CastSat<T, U, roundMode>(x);
                break;
            case SatMode::NO_SAT:
                y = CastNoSat<T, U, roundMode>(x);
                break;
        }
    }
    return y;
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RoundImpl(T x)
{
    return RoundIntrinsicsImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RintImpl(T x)
{
    return RintIntrinsicsImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T FloorImpl(T x)
{
    return FloorIntrinsicsImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CeilImpl(T x)
{
    return CeilIntrinsicsImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T TruncImpl(T x)
{
    if (x > (T)0) {
        return FloorImpl(x);
    } else {
        return CeilImpl(x);
    }
}

} // namespace Simt
} // namespace AscendC
#endif // IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_CAST_IMPL_H
