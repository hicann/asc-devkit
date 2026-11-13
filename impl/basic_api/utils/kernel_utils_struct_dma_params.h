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
#include <cstdint>
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

enum class eltwise_antiq_t {
    S8ANTIQ_Scalar = 0,
    S8ANTIQ_Vector,
    S4ANTIQ_Scalar,
    S4ANTIQ_Vector,
    U8ANTIQ_Scalar,
    U8ANTIQ_Vector,
    S16ANTIQ_Scalar,
    S16ANTIQ_Vector,
    S32ANTIQ_PreRow_Quant,
    NO_ANTIQ
};

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
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162) || \
                              (__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202))
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

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1 && defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
namespace Internal {
inline Nz2DnParams& GetFixpipeNz2dnParams()
{
    static thread_local Nz2DnParams params;
    return params;
}
} // namespace Internal
#endif

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

template <typename T = int16_t>
struct FixPipePreQuantParams {
    __aicore__ FixPipePreQuantParams() {}

    __aicore__ FixPipePreQuantParams(
        const int8_t shiftScaleIn, const int8_t mulScaleIn, const int32_t offset0In, const int8_t offset1In,
        const int16_t offset2In, const T clipMaxValPreIn)
        : shiftScale(shiftScaleIn),
          mulScale(mulScaleIn),
          offset0(offset0In),
          offset1(offset1In),
          offset2(offset2In),
          clipMaxValPre(clipMaxValPreIn)
    {
        isSft = true;
    }

    __aicore__ FixPipePreQuantParams(
        const float scalarValueIn, const int32_t offset0In, const int8_t offset1In, const int16_t offset2In,
        const T clipMaxValPreIn)
        : scalarValue(scalarValueIn),
          offset0(offset0In),
          offset1(offset1In),
          offset2(offset2In),
          clipMaxValPre(clipMaxValPreIn)
    {
        isSft = false;
    }

    int8_t shiftScale = 0;
    float scalarValue = 0.0f;
    int8_t mulScale = 0;
    int32_t offset0 = 0;
    int8_t offset1 = 0;
    int16_t offset2 = 0;
    bool isSft = false;
    bool isSigned = false;
    T clipMaxValPre = static_cast<T>(0);
};

struct FixPipePostQuantParams {
    __aicore__ FixPipePostQuantParams() {}

    __aicore__ FixPipePostQuantParams(
        const int8_t shiftScaleIn, const float scalarValueIn, const int8_t offset0In, const int16_t offset1In,
        const int32_t offset2In)
        : shiftScale(shiftScaleIn),
          scalarValue(scalarValueIn),
          offset0(offset0In),
          offset1(offset1In),
          offset2(offset2In)
    {}

    int8_t shiftScale = 0;
    float scalarValue = 0.0f;
    int8_t offset0 = 0;
    int16_t offset1 = 0;
    int32_t offset2 = 0;
    bool isSigned = false;
};

template <typename T = int8_t>
struct FixPipeEltAntiqParams {
    __aicore__ FixPipeEltAntiqParams() {}

    __aicore__ FixPipeEltAntiqParams(
        const int8_t shiftScaleIn, const int8_t mulScaleIn, const int8_t s4OffsetIn, const T b8OffsetIn,
        const int16_t s16OffsetIn, const T clipMaxValPreIn, const eltwise_antiq_t eltAntiqCfgIn)
        : shiftScale(shiftScaleIn),
          mulScale(mulScaleIn),
          s4Offset(s4OffsetIn),
          b8Offset(b8OffsetIn),
          s16Offset(s16OffsetIn),
          eltAntiqCfg(eltAntiqCfgIn)
    {
        isSft = true;
    }

    __aicore__ FixPipeEltAntiqParams(
        const half scalarValueIn, const int8_t s4OffsetIn, const T b8OffsetIn, const int16_t s16OffsetIn,
        const T clipMaxValPreIn, const eltwise_antiq_t eltAntiqCfgIn)
        : scalarValue(scalarValueIn),
          s4Offset(s4OffsetIn),
          b8Offset(b8OffsetIn),
          s16Offset(s16OffsetIn),
          eltAntiqCfg(eltAntiqCfgIn)
    {
        isSft = false;
    }

    int8_t shiftScale = 0;
    int8_t mulScale = 0;
    half scalarValue = 0.0f;
    int8_t s4Offset = 0;
    T b8Offset = static_cast<T>(0);
    int16_t s16Offset = 0;
    bool isSft = false;
    eltwise_antiq_t eltAntiqCfg = eltwise_antiq_t::S8ANTIQ_Scalar;
};

struct FixpipeEltwiseAddrParams {
    __aicore__ FixpipeEltwiseAddrParams() {}

    __aicore__ FixpipeEltwiseAddrParams(
        const bool c0IndicatorIn, const uint32_t eltSrcAddrIn, const uint32_t srcStride1In, const uint32_t srcStride2In)
        : c0Indicator(c0IndicatorIn), eltSrcAddr(eltSrcAddrIn), srcStride1(srcStride1In), srcStride2(srcStride2In)
    {}

    bool c0Indicator = false;
    uint32_t eltSrcAddr = 0;
    uint32_t srcStride1 = 0;
    uint32_t srcStride2 = 0;
};

template <typename T1, typename T2>
struct FixpipeClipReluParams {
    __aicore__ FixpipeClipReluParams() {}

    __aicore__ FixpipeClipReluParams(const T1 preValueIn, const T2 postValueIn)
        : preValue(preValueIn), postValue(postValueIn)
    {}

    T1 preValue = 0;
    T2 postValue = 0;
};

} // namespace AscendC
#endif // ASCENDC_MODULE_UTILS_STRUCT_DMA_PARAMS_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_DMA_PARAMS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_DMA_PARAMS_H__
#endif
