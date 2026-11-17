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
    "impl/tensor_api/utils/spr_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \\"#include \\"tensor_api/utils/utils.h\\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_IMPL_H
#endif
#ifndef IMPL_TENSOR_API_UTILS_SPR_IMPL_H
#define IMPL_TENSOR_API_UTILS_SPR_IMPL_H

#include "c_api/asc_simd.h"
#include "tensor_api/utils/utils.h"

namespace asc {
namespace te {

__aicore__ inline bool get_saturation_flag(saturation_mode mode)
{
    return asc_get_saturation_flag(static_cast<asc_saturation_mode>(mode));
}

__aicore__ inline override_strategy get_saturation_strategy()
{
    return static_cast<override_strategy>(asc_get_saturation_strategy());
}

__aicore__ inline int64_t get_squeeze_status() { return asc_get_squeeze_status(); }

__aicore__ inline void set_saturation_flag(saturation_mode mode, bool enable_sat)
{
    asc_set_saturation_flag(static_cast<asc_saturation_mode>(static_cast<uint8_t>(mode)), enable_sat);
}

__aicore__ inline void set_saturation_strategy(override_strategy strategy)
{
    asc_set_saturation_strategy(static_cast<asc_override_strategy>(strategy));
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_UTILS_SPR_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_IMPL_H
#endif
