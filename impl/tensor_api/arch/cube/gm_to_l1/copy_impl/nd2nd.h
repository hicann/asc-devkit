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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nd2nd.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nd2nd.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_ND2ND_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_ND2ND_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

class CopyGmToCbufAlignV2ND {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        // Batch layouts carry a leading B axis: (B, (row, col)) -> depth 3,
        // (B, ((1, row), (1, col))) -> depth 5. Non-batch layouts are depth 2/4.
        constexpr auto gmDepth = NestingDepthV<decltype(src.Layout().Shape())>;
        if constexpr (gmDepth == THREE_DIM_DATA || gmDepth == FIVE_DIM_DATA) {
            BatchDataCopyImpl<trait, T, U>(dst, src);
        } else {
            DataCopyImpl<trait, T, U>(dst, src);
        }
    }

private:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<U, T>();
        CheckDataType::CheckGm2L1AlignV2NDDataType<T, U>();
    }

    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src)
    {
        CheckTemplate<trait, T, U>();

        using type = typename U::elementType;
        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint32_t srcShapeRows;
        uint32_t srcShapeColumns;
        uint32_t srcStrideRows;
        uint32_t dstShapeColumns;
        uint32_t dstStrideRows;

        if constexpr (IsSatisfiedPtnFormatV<T, NDLayoutPtn>) {
            srcShapeRows = GetElement<AttrInfo::Shape, AttrInfo::Row>(srcLayout);
            srcShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column>(srcLayout);
            srcStrideRows = GetElement<AttrInfo::Stride, AttrInfo::Row>(srcLayout);
        } else {
            srcShapeRows = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
            srcShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
            srcStrideRows = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
        }

        if constexpr (IsSatisfiedPtnFormatV<T, NDLayoutPtn>) {
            dstShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column>(dstLayout);
            dstStrideRows = GetElement<AttrInfo::Stride, AttrInfo::Row>(dstLayout);
        } else {
            dstShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);
            dstStrideRows = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);
        }

        uint8_t cacheMode = src.Engine().GetCacheMode();

        // normal mode, dst_stride % C0_SIZE should be 0
        // compact mode, blockLen equals dstStride
        // multi rows copy, dst non-contiguous case
        uint32_t blockCount = srcShapeRows;
        uint32_t blockLen = srcShapeColumns * sizeof(type);
        uint64_t srcStride = srcStrideRows * sizeof(type);
        uint32_t dstStride = dstStrideRows * sizeof(type);

        if ((srcShapeRows == 1) || (srcShapeColumns == 1) ||
            (srcStrideRows == srcShapeColumns && dstStrideRows == dstShapeColumns && srcStrideRows == dstStrideRows)) {
            // compact mode, one line
            blockCount = 1;
            // must use srcShape, there is a scenario of small to large, using dstShape will cause src out of bound
            blockLen = srcShapeRows * srcShapeColumns * sizeof(type);
            srcStride = 0;
            dstStride = blockLen;
        }
        if constexpr (IsB4Type<type>) {
            // move fp4 as b8, need to be divided by 2
            blockLen = blockLen >> 1;
            srcStride = srcStride >> 1;
            dstStride = dstStride >> 1;
        }
        CopyGmToCbufAlignV2Base::DataCopy(dst, src, blockCount, blockLen, 0, 0, cacheMode, srcStride, dstStride);
    }

    // Batch case: layout is (B, (M, N)) with strides (sB, (sM, sN)). Per-matrix internal
    // compactness is assumed (sM == N), so a single matrix can be moved as one blockLen-sized block.
    // Four DataCopy params are derived directly from the batched layout:
    //   - GetElement<Shape, Row/Column> on the batched layout returns sub-matrix M/N (the
    //     SelectRowColTuples helper in is_format.h handles batch-axis stripping).
    //   - Get<0>(Shape/Stride) returns the batch size and per-matrix start-to-start stride.
    //   - When batches are also contiguous (srcBatchStride == M*N && dstBatchStride == M*N),
    //     fold the B blocks into a single B*M*N block; otherwise emit B blocks, one per matrix.
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void BatchDataCopyImpl(const T& dst, const U& src)
    {
        CheckTemplate<trait, T, U>();

        using type = typename U::elementType;
        auto srcLayout = src.Layout();
        auto dstLayout = dst.Layout();

        uint32_t batchSize = Get<0>(srcLayout.Shape());
        uint64_t srcBatchStride = Get<0>(srcLayout.Stride());
        uint32_t dstBatchStride = Get<0>(dstLayout.Stride());

        // Strip the leading B axis before calling GetElement, otherwise the dim=0/1 split below
        // would read the batch axis as row/column.
        auto srcInner = Te::Get<1>(srcLayout);

        uint32_t srcShapeRows;
        uint32_t srcShapeColumns;
        if constexpr (IsSatisfiedPtnFormatV<T, NDLayoutPtn>) {
            srcShapeRows = GetElement<AttrInfo::Shape, AttrInfo::Row>(srcInner);
            srcShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column>(srcInner);
        } else {
            srcShapeRows = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcInner);
            srcShapeColumns = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcInner);
        }

        uint32_t matrixElems = srcShapeRows * srcShapeColumns;
        uint8_t cacheMode = src.Engine().GetCacheMode();

        // batch-strided: B blocks, stride = per-matrix start-to-start in bytes.
        uint32_t blockCount = batchSize;
        uint32_t blockLen = matrixElems * sizeof(type);
        uint64_t srcStride = srcBatchStride * sizeof(type);
        uint32_t dstStride = dstBatchStride * sizeof(type);

        if constexpr (IsB4Type<type>) {
            // move fp4 as b8, need to be divided by 2
            blockLen = blockLen >> 1;
            srcStride = srcStride >> 1;
            dstStride = dstStride >> 1;
        }
        CopyGmToCbufAlignV2Base::DataCopy(dst, src, blockCount, blockLen, 0, 0, cacheMode, srcStride, dstStride);
    }
};
} // namespace Te
} // namespace AscendC

#endif

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
