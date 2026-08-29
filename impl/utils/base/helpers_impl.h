/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#warning \
    "impl/utils/base/helpers_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "include/utils/base/helpers.h"" and use public functions or variables defined in interface headers files."
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_HELPERS_H__
#endif

#ifndef IMPL_UTILS_BASE_HELPERS_IMPL_H
#define IMPL_UTILS_BASE_HELPERS_IMPL_H

#include "../debug/asc_debug_utils.h"

namespace asc {
namespace details {
template <auto funcPtr, typename DebugTag = AscendC::Internal::SimdVfDebugTag, typename... Args>
__aicore__ static inline void asc_vf_call_impl(Args&&... args)
{
    if constexpr (AscendC::Internal::SimdVfDebugTraits<DebugTag>::enabled) {
        AscVFDebugInitUb();
    }

    funcPtr(args...);

    if constexpr (AscendC::Internal::SimdVfDebugTraits<DebugTag>::enabled) {
        AscVFDebugFinish();
        const bool transferFailed = AscVFDebugTransferUb();
#ifdef ASCENDC_INTERNAL_SIMD_VF_OVERFLOW_WARNING_ENABLED
        if (transferFailed) {
            __asc_aicore::printf_impl(
                "[WARNING]: SIMD VF debug record exceeds the 2024-byte limit. The record was dropped.\n");
        }
#else
        (void)transferFailed;
#endif
    }
}
} // namespace details
} // namespace asc

#endif // IMPL_UTILS_BASE_HELPERS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_HELPERS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_HELPERS_H__
#endif
