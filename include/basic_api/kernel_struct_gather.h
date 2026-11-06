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
 * \file kernel_struct_gather.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_struct_gather.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_GATHER_H__
#endif

#ifndef ASCENDC_MODULE_STRUCT_GATHER_H
#define ASCENDC_MODULE_STRUCT_GATHER_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_constants.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {
struct GatherRepeatParams {
    __aicore__ GatherRepeatParams() {}

    __aicore__ GatherRepeatParams(const uint8_t dstBlkStrideIn, const uint8_t dstRepStrideIn)
        : dstBlkStride(dstBlkStrideIn), dstRepStride(dstRepStrideIn)
    {}

    uint32_t blockNumber = DEFAULT_BLK_NUM;
    uint16_t dstRepStride = DEFAULT_REPEAT_STRIDE;
    uint8_t dstBlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src0BlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src1BlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src0RepStride = DEFAULT_REPEAT_STRIDE;
    uint8_t src1RepStride = DEFAULT_REPEAT_STRIDE;
    bool repeatStrideMode = false;
    bool strideSizeMode = false;
};

enum class GatherMaskMode : uint8_t { VERSION_V1 = 0, VERSION_V2 = 1 };

#if (__NPU_ARCH__ == 2201)
const GatherMaskMode defaultGahterMaskMode = GatherMaskMode::VERSION_V2;
const GatherMaskMode defaultGatherMaskMode = GatherMaskMode::VERSION_V2;
#else
const GatherMaskMode defaultGahterMaskMode = GatherMaskMode::VERSION_V1;
const GatherMaskMode defaultGatherMaskMode = GatherMaskMode::VERSION_V1;
#endif

struct GatherMaskParams {
    __aicore__ GatherMaskParams() {}

    __aicore__ GatherMaskParams(
        const uint8_t src0BlockStrideIn, const uint16_t repeatTimesIn, const uint16_t src0RepeatStrideIn,
        const uint8_t src1RepeatStrideIn)
        : src0BlockStride(src0BlockStrideIn),
          repeatTimes(repeatTimesIn),
          src0RepeatStride(src0RepeatStrideIn),
          src1RepeatStride(src1RepeatStrideIn)
    {}

    uint8_t src0BlockStride = DEFAULT_BLK_STRIDE;
    uint16_t repeatTimes = 0;
    uint16_t src0RepeatStride = DEFAULT_REPEAT_STRIDE;
    uint8_t src1RepeatStride = DEFAULT_REPEAT_STRIDE;
};

enum class GatherPaddingMode : uint8_t { FIRST_ELEM_EVERY_BRUST = 0, CONSTANT_PADDING_MODE = 1, INVALID = 2 };

template <typename T>
struct GatherPadParams {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202) || (__NPU_ARCH__ == 5162))
    using TYPE = typename GetPadValueType<T>::Type;
    __aicore__ GatherPadParams() : padSel(GatherPaddingMode::FIRST_ELEM_EVERY_BRUST), paddingValue(0) {}

    __aicore__ GatherPadParams(const GatherPaddingMode padSelect, const T padValue)
        : padSel(padSelect), paddingValue(*(reinterpret_cast<const TYPE*>(&padValue)))
    {}

    GatherPaddingMode padSel;
    TYPE paddingValue;
#endif
};

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202) || (__NPU_ARCH__ == 5162))
struct GatherParams {
    __aicore__ GatherParams() {}

    __aicore__ GatherParams(const uint16_t offsetCount, const uint32_t len, const uint32_t stride)
        : offsetNum(offsetCount), burstLen(len), dstStride(stride) {};

    uint16_t offsetNum = DEFAULT_DATA_COPY_NBURST;
    uint32_t burstLen = 0;
    uint32_t dstStride = DEFAULT_DATA_COPY_STRIDE;
};

struct ScatterParams {
    __aicore__ ScatterParams() {};

    __aicore__ ScatterParams(const uint16_t offsetCount, const uint32_t len, const uint32_t stride)
        : offsetNum(offsetCount), burstLen(len), srcStride(stride) {};
    uint16_t offsetNum = DEFAULT_DATA_COPY_NBURST;
    uint32_t burstLen = 0;
    uint32_t srcStride = DEFAULT_DATA_COPY_STRIDE;
};
#endif
} // namespace AscendC
#endif // ASCENDC_MODULE_STRUCT_GATHER_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_GATHER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_GATHER_H__
#endif
