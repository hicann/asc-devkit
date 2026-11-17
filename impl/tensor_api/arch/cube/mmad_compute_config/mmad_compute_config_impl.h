/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \\
    "impl/tensor_api/arch/cube/mmad_compute_config/mmad_compute_config_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \\"#include \\"tensor_api/arch/cube/mmad_compute_config/mmad_compute_config.h\\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MMAD_COMPUTE_CONFIG_IMPL_H
#endif
/*!
 * \file mmad_compute_config_impl.h
 * \brief MMAD computation configuration implementation.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_MMAD_COMPUTE_CONFIG_MMAD_COMPUTE_CONFIG_IMPL_H
#define IMPL_TENSOR_API_ARCH_CUBE_MMAD_COMPUTE_CONFIG_MMAD_COMPUTE_CONFIG_IMPL_H

#include "c_api/asc_simd.h"

namespace asc {
namespace te {

__aicore__ inline void enable_hf32() { asc_enable_hf32(); }

__aicore__ inline void disable_hf32() { asc_disable_hf32(); }

__aicore__ inline void set_hf32_round_mode(hf32_round_mode mode)
{
    switch (mode) {
        case hf32_round_mode::nearest_away:
            asc_set_hf32_round_mode(asc_hf32_round_mode::NEAREST_AWAY);
            break;
        case hf32_round_mode::nearest_even:
            asc_set_hf32_round_mode(asc_hf32_round_mode::NEAREST_EVEN);
            break;
        default:
            break;
    }
}

__aicore__ inline void set_mmad_direction(mmad_direction direction)
{
    switch (direction) {
        case mmad_direction::m:
            asc_set_mmad_direction_m();
            break;
        case mmad_direction::n:
            asc_set_mmad_direction_n();
            break;
        default:
            break;
    }
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_MMAD_COMPUTE_CONFIG_MMAD_COMPUTE_CONFIG_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MMAD_COMPUTE_CONFIG_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MMAD_COMPUTE_CONFIG_IMPL_H
#endif
