/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file asc_simd_debug_traits.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_SIMD_DEBUG_TRAITS_H
#define IMPL_UTILS_DEBUG_ASC_SIMD_DEBUG_TRAITS_H

namespace AscendC {
namespace Internal {
namespace {
struct SimdVfDebugTag {};
} // namespace

template <typename Tag>
struct SimdVfDebugTraits {
    static constexpr bool enabled = false;
};

} // namespace Internal
} // namespace AscendC

#endif // IMPL_UTILS_DEBUG_ASC_SIMD_DEBUG_TRAITS_H
