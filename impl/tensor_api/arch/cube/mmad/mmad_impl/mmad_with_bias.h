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
    "impl/tensor_api/arch/cube/mmad/mmad_impl/mmad_with_bias.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file mmad_with_bias.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_IMPL_MMAD_WITH_BIAS_H
#define IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_IMPL_MMAD_WITH_BIAS_H

#include "impl/tensor_api/arch/cube/mmad/mmad_impl/instruction.h"

namespace AscendC {
namespace Te {

class MmadWithBias {
public:
    template <const MmadTrait& trait, typename T, typename U, typename S, typename V, typename Params>
    __aicore__ inline static void Run(const T& dst, const U& fm, const S& filter, const V& bias, const Params& params)
    {
        MmadImpl<trait, T, U, S, V>(dst, fm, filter, bias, params);
    }

private:
    template <const MmadTrait& trait, typename T, typename U, typename S, typename V>
    __aicore__ inline static constexpr void CheckTemplateForNormal()
    {
        CheckLayoutPattern<T, U, S, V>();
        CheckDataType::CheckMmadBiasDataType<T, U, S, V>();
    }

    template <const MmadTrait& trait, typename T, typename U, typename S, typename V>
    __aicore__ inline static constexpr void CheckTemplateForMx()
    {
        CheckLayoutPattern<T, U, S, V>();
        CheckDataType::CheckMxMmadBiasDataType<T, U, S, V>();
    }

    template <const MmadTrait& trait, typename T, typename U, typename S, typename V, typename Params>
    __aicore__ inline static void MmadImpl(
        const T& dst, const U& fm, const S& filter, const V& bias, const Params& params)
    {
        if constexpr (trait.mmadType == MmadType::NORMAL) {
            CheckTemplateForNormal<trait, T, U, S, V>();
        } else if constexpr (trait.mmadType == MmadType::MX) {
            CheckTemplateForMx<trait, T, U, S, V>();
        }

        bool cmatrixSource = false;
        if constexpr (Std::is_same_v<GetMemLocation<V>, Location::BIAS>) {
            cmatrixSource = true;
        }

        if constexpr (trait.mmadType == MmadType::NORMAL) {
            MmadBiasInstr::Mmad(
                dst, fm, filter, bias, params.m, params.k, params.n, params.unitFlag, trait.disableGemv, cmatrixSource,
                false);
        } else if constexpr (trait.mmadType == MmadType::MX) {
            MmadMxBiasInstr::Mmad(
                dst, fm, filter, bias, params.m, params.k, params.n, params.unitFlag, trait.disableGemv, cmatrixSource,
                false);
        }
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_IMPL_MMAD_WITH_BIAS_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
