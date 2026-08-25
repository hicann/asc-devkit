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
    "impl/tensor_api/arch/vector/transform/transform_vf_set.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file transform_vf_set.h
 * \brief Set definitions for instruction-to-wrapper mapping
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_TRANSFORM_TRANSFORM_VF_SET_H
#define IMPL_TENSOR_API_ARCH_VECTOR_TRANSFORM_TRANSFORM_VF_SET_H

#include "impl/tensor_api/arch/vector/axpy/axpy_vf.h"
#include "impl/tensor_api/arch/vector/binary/binary_vf.h"
#include "impl/tensor_api/arch/vector/binary_scalar/binary_scalar_vf.h"
#include "impl/tensor_api/arch/vector/cast/cast_vf.h"
#include "impl/tensor_api/arch/vector/dual/dual_vf.h"
#include "impl/tensor_api/arch/vector/unary/unary_vf.h"
#include "impl/tensor_api/utils/map_impl.h"

namespace AscendC {
namespace Te {

template <typename CalcFunc, typename TraitType>
struct Transform2VFSet {
    using binarySet = Std::tuple<
        Inst::Add, Inst::Sub, Inst::Madd, Inst::And, Inst::Select, Inst::AbsSub, Inst::Min, Inst::Max, Inst::Or,
        Inst::Mul, Inst::ExpSubEven, Inst::ExpSubOdd, Inst::ShiftLeft, Inst::ShiftRight, Inst::Div, Inst::Xor,
        Inst::Prelu>;
    using castSet = Std::tuple<Inst::Cast, Inst::Ceil, Inst::Rint, Inst::Round, Inst::Trunc, Inst::Floor>;
    using binaryScalarSet = Std::tuple<
        Inst::MulScalar, Inst::AddScalar, Inst::MaxScalar, Inst::MinScalar, Inst::ShiftLeftScalar,
        Inst::ShiftRightScalar, Inst::MulsEven, Inst::MulsOdd>;
    using unarySet = Std::tuple<Inst::Not, Inst::Ln, Inst::Abs, Inst::Sqrt, Inst::Exp>;
    using axpySet = Std::tuple<Inst::Axpy>;
    using dualSet = Std::tuple<Inst::Mull>;

    using transformSet = TupleMap<
        Std::tuple<binarySet, Transform2BinaryVF<CalcFunc, TraitType>>,
        Std::tuple<castSet, Transform2CastInstMatch<CalcFunc, TraitType>>,
        Std::tuple<binaryScalarSet, Transform2BinaryScalarVF<CalcFunc, TraitType>>,
        Std::tuple<unarySet, Transform2UnaryVF<CalcFunc, TraitType>>,
        Std::tuple<axpySet, Transform2AxpyVF<CalcFunc, TraitType>>,
        Std::tuple<dualSet, Transform2DualVF<CalcFunc, TraitType>>>;
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_TRANSFORM_TRANSFORM_VF_SET_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
