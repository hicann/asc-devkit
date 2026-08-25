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
    "impl/tensor_api/arch/vector/dual/dual_vf.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file dual_vf.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_DUAL_DUAL_VF_H
#define IMPL_TENSOR_API_ARCH_VECTOR_DUAL_DUAL_VF_H

#include "impl/tensor_api/arch/vector/dual/instruction.h"
#include "impl/tensor_api/arch/vector/utils/mask_utils.h"

namespace AscendC {
namespace Te {

template <typename CalcFunc, typename TraitType>
class DualVF {
public:
    template <typename T>
    __simd_vf__ inline static void Run(
        __ubuf__ T* dst0, __ubuf__ T* dst1, __ubuf__ T* src0, __ubuf__ T* src1, uint16_t repeat, uint16_t oneRepSize,
        uint32_t dataSize)
    {
        using RegType = typename VectorTypeTransform::template Get<T>;

        vector_bool vmask;
        RegType reg_src0;
        RegType reg_src1;
        RegType reg_dst0;
        RegType reg_dst1;
        for (uint16_t i = 0; i < repeat; i++) {
            vmask = Inst::UpdateMask::template Run<T>(dataSize);
            asc_loadalign(reg_src0, src0 + i * oneRepSize);
            asc_loadalign(reg_src1, src1 + i * oneRepSize);
            CalcFunc::template Run<RegType>(reg_dst0, reg_dst1, reg_src0, reg_src1, vmask);
            asc_storealign(dst0 + i * oneRepSize, reg_dst0, vmask);
            asc_storealign(dst1 + i * oneRepSize, reg_dst1, vmask);
        }
    }
};

template <typename CalcFunc, typename TraitType>
class Transform2DualVF {
public:
    template <typename T, typename U, typename V, typename W>
    __aicore__ inline static void Run(const T& dst0, const U& dst1, const V& src0, const W& src1)
    {
        using type = GetAttributeElementType<typename T::elementType*>;
        uint32_t dataSize = dst0.Size();

        uint16_t VECTOR_REG_WIDTH = asc_get_vf_len();
        uint16_t oneRepSize = VECTOR_REG_WIDTH / sizeof(type);
        uint16_t repeat = Std::ceil_division(dataSize, oneRepSize);

        DualVF<CalcFunc, TraitType>::template Run<type>(
            dst0.Data().Get(), dst1.Data().Get(), src0.Data().Get(), src1.Data().Get(), repeat, oneRepSize, dataSize);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_DUAL_DUAL_VF_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
