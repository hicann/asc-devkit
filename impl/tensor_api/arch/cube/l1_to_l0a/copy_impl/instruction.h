/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning                                                                                                               \
    "impl/tensor_api/arch/cube/l1_to_l0a/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace AscendC {
namespace Te {

struct CopyL12L0ATrait {};

constexpr Img2ColParams<int16_t> DEFAULT_IMG2COL_PARAMS{};

class LoadCbufToCa {
public:
    template <bool transpose, typename T, typename U, typename... Params>
    __aicore__ inline static void LoadData(const T& dst, const U& src, const Params& ...params)
    {
        LoadCbufToCaImpl<transpose>(dst.Data().Get(), src.Data().Get(), params...);
    }

private:
    template <bool transpose, typename T>
    __aicore__ inline static void LoadCbufToCaImpl(__ca__ T* dst, __cbuf__ T* src, uint16_t mStartPosition,
        uint16_t kStartPosition, uint8_t mStep, uint8_t kStep, int16_t srcStride, uint16_t dstStride)
    {
        if ASCEND_IS_AIV {
            return;
        }
        if constexpr (transpose) {
            asc_copy_l12l0a_transpose(dst, src, mStartPosition, kStartPosition, mStep, kStep, srcStride, dstStride);
        } else {
            asc_copy_l12l0a(dst, src, mStartPosition, kStartPosition, mStep, kStep, srcStride, dstStride);
        }
    }
};

class LoadCbufToCaImg2Col {
public:
    __aicore__ inline static void SetFMatrix(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
    {
        if ASCEND_IS_AIV {
            return;
        }
        asc_l13d_fmatrix_config config;
        config.l1_height = l1H;
        config.l1_width = l1W;
        config.padding_left_size = padList[0];
        config.padding_right_size = padList[1];
        config.padding_top_size = padList[2];
        config.padding_bottom_size = padList[3];
        asc_set_l13d_fmatrix(config);
    }

    template <typename T>
    __aicore__ inline static void SetPadding(T padValue)
    {
        if ASCEND_IS_AIV {
            return;
        }
        asc_set_l13d_padding(padValue);
    }

    __aicore__ inline static void SetRepeat(uint16_t dstStride)
    {
        if ASCEND_IS_AIV {
            return;
        }
        asc_load3d_v2_config config;
        config.rpt_stride = 0;
        config.rpt_time = 1;
        config.rpt_mode = 0;
        config.config |= static_cast<uint64_t>(dstStride) << 32;
        asc_set_l13d_rpt(config);
    }

    template <typename T, typename U, typename... Params>
    __aicore__ inline static void LoadData(const T& dst, const U& src, const Params&... params)
    {
        LoadCbufToCaImg2ColImpl(dst.Data().Get(), src.Data().Get(), params...);
    }

private:
    template <typename T>
    __aicore__ inline static void LoadCbufToCaImg2ColImpl(
        __ca__ T* dst, __cbuf__ T* src, uint16_t kExtension, uint16_t mExtension, uint16_t kStartPt, uint16_t mStartPt,
        uint8_t strideW, uint8_t strideH, uint8_t filterW, uint8_t filterH, uint8_t dilationFilterW,
        uint8_t dilationFilterH, bool filterSizeW, bool filterSizeH, bool transpose, bool fMatrixCtrl,
        uint16_t channelSize)
    {
        if ASCEND_IS_AIV {
            return;
        }
        asc_copy_l12l0a(
            dst, src, kExtension, mExtension, kStartPt, mStartPt, strideW, strideH, filterW, filterH, dilationFilterW,
            dilationFilterH, filterSizeW, filterSizeH, transpose, fMatrixCtrl, channelSize);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
