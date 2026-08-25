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
#warning \
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scalea_zz2zz.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file scalea_zz2zz.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_SCALEA_ZZ2ZZ_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_SCALEA_ZZ2ZZ_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

class CopyGmToCbufScaleAZz2Zz {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        DataCopyImpl<trait>(dst, src);
    }

private:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<U, T>();
        CheckDataType::CheckGm2L1ScaleDataType<T, U>();
    }

    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src)
    {
        CheckTemplate<trait, T, U>();
        if constexpr (U::layoutType::depth == FIVE_DIM_DATA) {
            auto srcLayout = src.Layout();
            auto dstLayout = dst.Layout();
            EmitCopy(
                dst, src, RemoveBatchDim(srcLayout), RemoveBatchDim(dstLayout), Get<0>(srcLayout.Shape()),
                Get<0>(srcLayout.Stride()), Get<0>(dstLayout.Stride()));
        } else {
            EmitCopy(dst, src, src.Layout(), dst.Layout(), 1, 0, 0);
        }
    }

    template <typename T, typename U, typename SrcLayout, typename DstLayout>
    __aicore__ inline static void EmitCopy(
        const T& dst, const U& src, const SrcLayout& srcLayout, const DstLayout& dstLayout, uint16_t batchNum,
        uint64_t srcBatchStride, uint64_t dstBatchStride)
    {
        using type = typename U::elementType;

        auto srcShapeRowsB = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
        auto srcShapeRowsS = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(srcLayout);
        auto srcShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
        auto srcStrideRowsB = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
        auto srcStrideRowsS = GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(srcLayout);

        auto dstShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);
        auto dstStrideRows = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);

        uint8_t cacheMode = src.Engine().GetCacheMode();

        // lprp mode, dst_stride % C0_SIZE should be 0
        // multi rows copy, dst non-contiguous case

        uint32_t blockCount = srcShapeRowsB;
        uint32_t blockLen = srcShapeColumns * sizeof(type) * srcShapeRowsS * srcStrideRowsS;
        uint64_t srcStride = srcStrideRowsB * sizeof(type);
        uint32_t dstStride = dstStrideRows * sizeof(type);

        uint8_t leftPaddingCnt = 0;
        uint8_t rightPaddingCnt = 0;
        for (uint16_t batchIndex = 0; batchIndex < batchNum; ++batchIndex) {
            CopyGmToCbufAlignV2Base::CopyGmToCbufAlignV2(
                (__cbuf__ half*)((dst.Data() + batchIndex * dstBatchStride).Get()),
                (__gm__ half*)((src.Data() + batchIndex * srcBatchStride).Get()), blockCount, blockLen, leftPaddingCnt,
                rightPaddingCnt, cacheMode, srcStride, dstStride);
        }
    }
};

} // namespace Te
} // namespace AscendC

#endif

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
