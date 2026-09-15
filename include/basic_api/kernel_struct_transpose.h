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
 * \file kernel_struct_transpose.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_TRANSPOSE_H__
#endif

#ifndef ASCENDC_MODULE_STRUCT_TRANSPOSE_H
#define ASCENDC_MODULE_STRUCT_TRANSPOSE_H

#include <cstdint>
#include "../../impl/basic_api/kernel_macros.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "stub_def.h"
#endif

namespace AscendC {
enum class TransposeType : uint8_t {
    // default value
    TRANSPOSE_TYPE_NONE,
    // { shape:[B, A1, A3 / 16, A2 / 16, 16, 16], format:"NZ"} -->{ shape:[B, A2, A1, A3], ori_shape:[B, A2, A1, A3],
    // format:"ND"}
    TRANSPOSE_NZ2ND_0213,
    // { shape:[B, A1, A3 / 16, A2 / 16, 16, 16], format:"NZ"}-->{ shape:[B, A2, A3 / 16, A1 / 16, 16, 16],
    // origin_shape:[B, A2, A1, A3], format:"NZ"}
    TRANSPOSE_NZ2NZ_0213,
    // { shape:[B, H / 16, S / 16, 16, 16], format:"NZ"}-->{ shape:[B, N, H/N/16, S / 16, 16, 16], ori_shape:[B, N, S,
    // H/N], format:"NZ"}
    TRANSPOSE_NZ2NZ_012_WITH_N,
    // { shape:[B, H / 16, S / 16, 16, 16], format:"NZ"}-->{ shape:[B, N, S, H/N], ori_shape:[B, N, S, H/N],
    // format:"ND"}
    TRANSPOSE_NZ2ND_012_WITH_N,
    // { shape:[B, N, H/N/16, S/16, 16, 16], format:"NZ"}-->{ shape:[B, S, H], ori_shape:[B, S, H], format:"ND"}
    TRANSPOSE_NZ2ND_012_WITHOUT_N,
    // { shape:[B, N, H/N/16, S/16, 16, 16], format:"NZ"}-->{ shape:[B, H/16, S/16, 16, 16], ori_shape:[B, S, H],
    // format:"NZ"}
    TRANSPOSE_NZ2NZ_012_WITHOUT_N,
    TRANSPOSE_ND2ND_ONLY,    // { shape:[H, W], format:"ND"} -->{ shape:[W, H], format:"ND"}
    TRANSPOSE_ND_UB_GM,      //  [B, N, S, H/N] -> [B, S, H]
    TRANSPOSE_GRAD_ND_UB_GM, //  [B, S, H] -> [B, N, S, H/N]
    TRANSPOSE_ND2ND_B16,     // { shape:[16, 16], format:"ND", dataType: B16} -->{ shape:[16, 16], format:"ND"}
    TRANSPOSE_NCHW2NHWC,     // [ N, C, H, W] -> [N, H, W, C]
    TRANSPOSE_NHWC2NCHW,     // [ N, H, W, C] -> [N, C, H, W]
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
    TRANSPOSE_ND2ND_021,       // [H, W]->[W, H], [N, H, W]->[N, W, H]
    TRANSPOSE_ND2ND_102,       // [N, H, W]->[H, N, W]
    TRANSPOSE_ND2ND_210,       // [N, H, W]->[W, H, N]
    TRANSPOSE_ND2NZ_WITH_INTLV // [N, D] -> [N, Z]
#endif
};

struct TransDataTo5HDParams {
    constexpr __aicore__ TransDataTo5HDParams() = default;

    constexpr __aicore__ TransDataTo5HDParams(
        const bool dstHighHalfIn, const bool srcHighHalfIn, const uint8_t repeatTimesIn, const uint16_t dstRepStrideIn,
        const uint16_t srcRepStrideIn)
        : dstHighHalf(dstHighHalfIn),
          srcHighHalf(srcHighHalfIn),
          repeatTimes(repeatTimesIn),
          dstRepStride(dstRepStrideIn),
          srcRepStride(srcRepStrideIn)
    {}

    bool dstHighHalf = false;
    bool srcHighHalf = false;
    uint8_t repeatTimes = 1;
    uint16_t dstRepStride = 0;
    uint16_t srcRepStride = 0;
};

struct TransposeParamsExt {
    constexpr __aicore__ TransposeParamsExt() = default;

    constexpr __aicore__ TransposeParamsExt(
        const uint16_t nSizeIn, const uint16_t cSizeIn, const uint16_t hSizeIn, const uint16_t wSizeIn,
        const TransposeType transposeTypeIn)
        : nSize(nSizeIn), cSize(cSizeIn), hSize(hSizeIn), wSize(wSizeIn), transposeType(transposeTypeIn)
    {}

    uint16_t nSize = 0;
    uint16_t cSize = 0;
    uint16_t hSize = 0;
    uint16_t wSize = 0;
    TransposeType transposeType = TransposeType::TRANSPOSE_ND2ND_B16;
};
} // namespace AscendC
#endif // ASCENDC_MODULE_STRUCT_TRANSPOSE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_TRANSPOSE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_TRANSPOSE_H__
#endif
