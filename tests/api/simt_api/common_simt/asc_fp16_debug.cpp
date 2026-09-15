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
 * \file asc_fp16_debug.cpp
 * \brief
 */

#if defined(ASCENDC_CPU_DEBUG)
#include "stub_def.h"

constexpr uint32_t half_inf = 0x7C00;
constexpr uint32_t half_max_nan = 0x7FFF;
constexpr uint32_t half_neg_inf = 0xFC00;
constexpr int32_t MAX_SHLF_OFFSET = 31;
constexpr int32_t WARP_SIZE = 32;

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
bool __isnan(half x)
{
    uint16_t* intX = (uint16_t*)&x;
    return (*intX > half_inf && *intX <= half_max_nan) || (*intX > half_neg_inf);
}

bool __isinf(half x)
{
    uint16_t* intX = (uint16_t*)&x;
    return (*intX == half_inf) || (*intX == half_neg_inf);
}

half __fma(half x, half y, half z)
{
    if (__isnan(z) || __isnan(x) || __isnan(y)) {
        return NAN;
    }
    return (static_cast<float>(x) * static_cast<float>(y)) + static_cast<float>(z);
}

half __expf(half x)
{
    float tmp = x.ToFloat();
    tmp = exp(tmp);
    return half(tmp);
}

half __logf(half x)
{
    float tmp = x.ToFloat();
    tmp = logf(tmp);
    return half(tmp);
}

half __sqrtf(half x)
{
    float tmp = x.ToFloat();
    tmp = sqrtf(tmp);
    return half(tmp);
}

half __floorf(half x)
{
    if (__isinf(x)) {
        return x;
    }
    return half(floor(float(x)));
}

half __rintf(half x)
{
    if (__isinf(x)) {
        return x;
    }
    return half(rint(float(x)));
}

half __ceilf(half x)
{
    if (__isinf(x)) {
        return x;
    }
    return half(ceil(float(x)));
}

half2 __expf(half2 x)
{
    half htmp1 = x.x;
    half htmp2 = x.y;
    float tmp1 = htmp1.ToFloat();
    float tmp2 = htmp2.ToFloat();
    tmp1 = exp(tmp1);
    tmp2 = exp(tmp2);
    htmp1 = half(tmp1);
    htmp2 = half(tmp2);
    return {htmp1, htmp2};
}

half2 __logf(half2 x)
{
    half htmp1 = x.x;
    half htmp2 = x.y;
    float tmp1 = htmp1.ToFloat();
    float tmp2 = htmp2.ToFloat();
    tmp1 = log(tmp1);
    tmp2 = log(tmp2);
    htmp1 = half(tmp1);
    htmp2 = half(tmp2);
    return {htmp1, htmp2};
}

half2 __sqrtf(half2 x)
{
    half htmp1 = x.x;
    half htmp2 = x.y;
    float tmp1 = htmp1.ToFloat();
    float tmp2 = htmp2.ToFloat();
    tmp1 = sqrt(tmp1);
    tmp2 = sqrt(tmp2);
    htmp1 = half(tmp1);
    htmp2 = half(tmp2);
    return {htmp1, htmp2};
}
#endif
#endif
