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
    "impl/tensor_api/arch/vector/unary/unary_vf.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file unary_vf.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UNARY_UNARY_VF_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UNARY_UNARY_VF_H

#include "impl/tensor_api/arch/vector/unary/instruction.h"
#include "impl/tensor_api/arch/vector/utils/mask_utils.h"

namespace AscendC {
namespace Te {

template <typename CalcFunc, typename TraitType>
class UnaryVF {
public:
    template <typename T>
    __simd_vf__ inline static void Run(
        __ubuf__ T* dst, __ubuf__ T* src, uint16_t repeat, uint16_t oneRepSize, uint32_t dataSize)
    {
        using RegType = typename VectorTypeTransform::template Get<T>;

        vector_bool vmask;
        RegType reg_src;
        RegType reg_dst;
        for (uint16_t i = 0; i < repeat; i++) {
            vmask = Inst::UpdateMask::template Run<T>(dataSize);
            asc_loadalign(reg_src, src + i * oneRepSize);
            CalcFunc::template Run<RegType>(reg_dst, reg_src, vmask);
            asc_storealign(dst + i * oneRepSize, reg_dst, vmask);
        }
    }
};

template <typename CalcFunc, typename TraitType>
class Transform2UnaryVF {
public:
    template <typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        using type = GetAttributeElementType<typename T::elementType*>;
        uint32_t dataSize = dst.Size();

        uint16_t VECTOR_REG_WIDTH = asc_get_vf_len();
        uint16_t oneRepSize = VECTOR_REG_WIDTH / sizeof(type);
        uint16_t repeat = Std::ceil_division(dataSize, oneRepSize);

        UnaryVF<CalcFunc, TraitType>::template Run<type>(
            dst.Data().Get(), src.Data().Get(), repeat, oneRepSize, dataSize);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UNARY_UNARY_VF_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
