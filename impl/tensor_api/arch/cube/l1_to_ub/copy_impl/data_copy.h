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
    "impl/tensor_api/arch/cube/l1_to_ub/copy_impl/data_copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file data_copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_UB_COPY_IMPL_DATA_COPY_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_UB_COPY_IMPL_DATA_COPY_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/cube/l1_to_ub/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

class CopyL12UBND {
public:
    template <const CopyL12UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        Execute<trait>(dst, src);
    }

private:
    template <const CopyL12UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Execute(const T& dst, const U& src)
    {
        using srcType = typename U::elementType;
        using dstType = typename T::elementType;

        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint16_t blockCount = GetTotalRowShape(srcLayout);
        uint32_t blockLen = Std::ceil_division(GetTotalColumnShape(srcLayout), C0_ELEMENT<srcType>);

        uint32_t srcRowStride;
        uint32_t dstRowStride;
        if constexpr (IsSatisfiedPtnFormatV<U, NDLayoutPtn>) {
            srcRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row>(srcLayout);
        } else {
            srcRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
        }
        if constexpr (IsSatisfiedPtnFormatV<T, NDLayoutPtn>) {
            dstRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row>(dstLayout);
        } else {
            dstRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);
        }
        int64_t srcStride = Std::ceil_division(srcRowStride - GetTotalColumnShape(srcLayout), C0_ELEMENT<srcType>);
        int64_t dstStride = Std::ceil_division(dstRowStride - GetTotalColumnShape(srcLayout), C0_ELEMENT<dstType>);

        CopyCbufToUbufInstr::DataCopy(dst, src, blockCount, blockLen, srcStride, dstStride);
    }
};

class CopyL12UBDN {
public:
    template <const CopyL12UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        Execute<trait>(dst, src);
    }

private:
    template <const CopyL12UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Execute(const T& dst, const U& src)
    {
        using srcType = typename U::elementType;
        using dstType = typename T::elementType;

        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint16_t blockCount = GetTotalColumnShape(srcLayout);
        uint32_t blockLen = Std::ceil_division(GetTotalRowShape(srcLayout), C0_ELEMENT<srcType>);

        uint32_t srcColumnStride;
        uint32_t dstColumnStride;
        if constexpr (IsSatisfiedPtnFormatV<U, DNLayoutPtn>) {
            srcColumnStride = GetElement<AttrInfo::Stride, AttrInfo::Column>(srcLayout);
        } else {
            srcColumnStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);
        }
        if constexpr (IsSatisfiedPtnFormatV<T, DNLayoutPtn>) {
            dstColumnStride = GetElement<AttrInfo::Stride, AttrInfo::Column>(dstLayout);
        } else {
            dstColumnStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout);
        }
        int64_t srcStride = Std::ceil_division(srcColumnStride - GetTotalRowShape(srcLayout), C0_ELEMENT<srcType>);
        int64_t dstStride = Std::ceil_division(dstColumnStride - GetTotalRowShape(srcLayout), C0_ELEMENT<dstType>);

        CopyCbufToUbufInstr::DataCopy(dst, src, blockCount, blockLen, srcStride, dstStride);
    }
};

class CopyL12UBNZ {
public:
    template <const CopyL12UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        Execute<trait>(dst, src);
    }

private:
    template <const CopyL12UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Execute(const T& dst, const U& src)
    {
        using srcType = typename U::elementType;
        using dstType = typename T::elementType;

        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint16_t blockCount = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
        uint32_t blockLen = GetTotalRowShape(srcLayout);
        int64_t srcStride =
            GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout) / C0_ELEMENT<srcType> - blockLen;
        int64_t dstStride =
            GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout) / C0_ELEMENT<dstType> - blockLen;

        CopyCbufToUbufInstr::DataCopy(dst, src, blockCount, blockLen, srcStride, dstStride);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_UB_COPY_IMPL_DATA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
