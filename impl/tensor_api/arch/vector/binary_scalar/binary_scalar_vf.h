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
    "impl/tensor_api/arch/vector/binary_scalar/binary_scalar_vf.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file binary_scalar_vf.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_BINARY_SCALAR_BINARY_SCALAR_VF_H
#define IMPL_TENSOR_API_ARCH_VECTOR_BINARY_SCALAR_BINARY_SCALAR_VF_H

#include "impl/tensor_api/arch/vector/binary_scalar/instruction.h"
#include "impl/tensor_api/arch/vector/utils/mask_utils.h"

namespace AscendC {
namespace Te {

template <typename CalcFunc, typename TraitType>
class BinaryScalarVF {
public:
    template <typename T, typename U, typename S>
    __simd_vf__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ U* src, S value, uint16_t repeat, uint16_t oneRepSize, uint32_t dataSize)
    {
        using SrcRegType = typename VectorTypeTransform::template Get<U>;
        using DstRegType = typename VectorTypeTransform::template Get<T>;

        vector_bool vmask;
        SrcRegType reg_src;
        DstRegType reg_dst;

        for (uint16_t i = 0; i < repeat; i++) {
            vmask = Inst::UpdateMask::template Run<U>(dataSize);
            asc_loadalign(reg_src, src + i * oneRepSize);
            CalcFunc::template Run(reg_dst, reg_src, value, vmask);
            asc_storealign(dst + i * oneRepSize, reg_dst, vmask);
        }
    }
};

template <typename CalcFunc, typename TraitType>
class Transform2BinaryScalarVF {
public:
    template <typename T, typename U, typename V>
    __aicore__ inline static void Run(const T& dst, const U& src, const V& value)
    {
        using dstType = GetAttributeElementType<typename T::elementType*>;
        using srcType = GetAttributeElementType<typename U::elementType*>;
        uint32_t dataSize = dst.Size();

        uint16_t VECTOR_REG_WIDTH = asc_get_vf_len();
        uint16_t oneRepSize = VECTOR_REG_WIDTH / sizeof(srcType);
        uint16_t repeat = Std::ceil_division(dataSize, oneRepSize);

        BinaryScalarVF<CalcFunc, TraitType>::template Run<dstType, srcType>(
            dst.Data().Get(), src.Data().Get(), value, repeat, oneRepSize, dataSize);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_BINARY_SCALAR_BINARY_SCALAR_VF_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
