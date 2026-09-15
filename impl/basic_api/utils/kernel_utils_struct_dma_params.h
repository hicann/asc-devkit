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
 * \file kernel_utils_struct_dma_params.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_utils_struct_dma_params.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_DMA_PARAMS_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_STRUCT_DMA_PARAMS_H
#define ASCENDC_MODULE_UTILS_STRUCT_DMA_PARAMS_H
#include "kernel_utils_mode.h"

namespace AscendC {

struct QuantParams {
    __aicore__ QuantParams() {}
    __aicore__ QuantParams(const QuantMode_t quantPreIn) : quantPre(quantPreIn) {}
    __aicore__ QuantParams(const QuantMode_t quantPreIn, const uint64_t deqScalarIn)
        : quantPre(quantPreIn), deqScalar(deqScalarIn)
    {}
    QuantMode_t quantPre = QuantMode_t::NoQuant;
    uint64_t deqScalar;
};

struct Nz2NdParams {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
    __aicore__ Nz2NdParams() {}

    __aicore__ Nz2NdParams(const uint16_t ndNumIn, const uint16_t srcNdStrideIn, const uint32_t dstNdStrideIn)
    {
        ndNum = ndNumIn;
        srcNdStride = srcNdStrideIn;
        dstNdStride = dstNdStrideIn;
    }

    uint16_t ndNum = 1;       // loop3Size
    uint16_t srcNdStride = 0; // loop3SrcStride
    uint32_t dstNdStride = 0; // loop3DstStride
#else
    __aicore__ Nz2NdParams()
    {
        nz2ndEn = false;
        ndNum = 1;
        srcNdStride = 0;
        dstNdStride = 0;
        originalNSize = 0;
    }

    __aicore__ Nz2NdParams(
        const bool nz2ndEnIn, const uint16_t ndNumIn, const uint16_t srcNdStrideIn, const uint16_t dstNdStrideIn,
        const uint16_t originalNSizeIn)
    {
        nz2ndEn = nz2ndEnIn;
        ndNum = ndNumIn;
        srcNdStride = srcNdStrideIn;
        dstNdStride = dstNdStrideIn;
        originalNSize = originalNSizeIn;
    }

    bool nz2ndEn = false;
    uint16_t ndNum = 1;
    uint16_t srcNdStride = 0;
    uint16_t dstNdStride = 0;
    uint16_t originalNSize = 0;
#endif
};

struct Nz2DnParams {
    __aicore__ Nz2DnParams() {}

    __aicore__ Nz2DnParams(
        const uint16_t dnNumIn, const uint16_t srcNzMatrixStrideIn, const uint32_t dstDnMatrixStrideIn,
        const uint16_t srcNzC0StrideIn)
    {
        dnNum = dnNumIn;
        srcNzMatrixStride = srcNzMatrixStrideIn;
        dstDnMatrixStride = dstDnMatrixStrideIn;
        srcNzC0Stride = srcNzC0StrideIn;
    }

    uint16_t dnNum = 1;             // loop3Size
    uint16_t srcNzMatrixStride = 0; // loop3SrcStride
    uint32_t dstDnMatrixStride = 0; // loop3DstStride
    uint16_t srcNzC0Stride = 0;     // loop0SrcStride
};

template <typename T = int32_t>
struct FixpipeParams {
    __aicore__ FixpipeParams()
    {
        cburstNum = DEFAULT_DATA_COPY_NBURST;
        burstLen = 1;
        srcStride = DEFAULT_DATA_COPY_STRIDE;
        dstStride = DEFAULT_DATA_COPY_STRIDE;
        reluEn = false;
        unitFlag = 0;
    }

    __aicore__ FixpipeParams(
        const uint16_t count, const uint16_t len, const uint16_t srcStrideIn, const uint32_t dstStrideIn)
    {
        cburstNum = count;
        burstLen = len;
        dstStride = dstStrideIn;
        srcStride = srcStrideIn;
    }

    uint16_t cburstNum = 0;
    uint16_t burstLen = 0;
    uint32_t dstStride = 0;
    uint16_t srcStride = 0;
    // extend param
    QuantParams quantParams;
    bool reluEn = false;
    Nz2NdParams nz2ndParams;
    uint8_t unitFlag = 0;
};

} // namespace AscendC
#endif // ASCENDC_MODULE_UTILS_STRUCT_DMA_PARAMS_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_DMA_PARAMS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_DMA_PARAMS_H__
#endif
