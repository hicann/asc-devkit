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
    "impl/tensor_api/arch/cube/l1_to_fb/copy_impl/nd2nd.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nd2nd.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_IMPL_ND2ND_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_IMPL_ND2ND_H

#include "impl/tensor_api/arch/cube/l1_to_fb/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

class CopyL12FBND {
public:
    template <const CopyL12FBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        DataCopyImpl<trait, T, U>(dst, src);
    }

private:
    template <const CopyL12FBTrait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckDataType::CheckL12FbDataType<T, U>();
        CheckLayoutPattern<U, T>();
    }

    template <const CopyL12FBTrait& trait, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src)
    {
        CheckTemplate<trait, T, U>();

        using srcType = typename U::elementType;
        using dstType = typename T::elementType;

        constexpr uint32_t deqTensorAddrAlignValue = 128;
        constexpr uint16_t fbufBurstLenUnit = 64;

        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint16_t srcCol;
        uint16_t srcRow;
        uint16_t dstCol;
        uint16_t dstRow;
        uint16_t blockCount;
        if constexpr (IsSatisfiedPtnFormatV<U, NDLayoutPtn>) {
            srcCol = GetElement<AttrInfo::Shape, AttrInfo::Column>(srcLayout);
            srcRow = GetElement<AttrInfo::Stride, AttrInfo::Row>(srcLayout);
            blockCount = GetElement<AttrInfo::Shape, AttrInfo::Row>(srcLayout);
        } else {
            srcCol = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
            srcRow = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
            blockCount = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
        }
        if constexpr (IsSatisfiedPtnFormatV<T, NDLayoutPtn>) {
            dstCol = GetElement<AttrInfo::Shape, AttrInfo::Column>(dstLayout);
            dstRow = GetElement<AttrInfo::Stride, AttrInfo::Row>(dstLayout);
        } else {
            dstCol = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);
            dstRow = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);
        }

        uint16_t blockLen = Std::ceil_align(srcCol * sizeof(srcType), deqTensorAddrAlignValue) / fbufBurstLenUnit;
        uint16_t srcStride = Std::ceil_division(srcRow * sizeof(srcType), C0_SIZE<>);
        uint16_t dstStride = Std::ceil_align(dstRow * sizeof(dstType), deqTensorAddrAlignValue) / fbufBurstLenUnit;

        CopyL12FBInstr::DataCopy(dst, src, blockCount, blockLen, srcStride, dstStride);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_IMPL_ND2ND_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
