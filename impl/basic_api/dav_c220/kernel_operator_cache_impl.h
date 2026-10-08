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
 * \file kernel_operator_cache_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_c220/kernel_operator_cache_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CACHE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_CACHE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_CACHE_IMPL_H

#include <cstdint>
#include "../kernel_log.h"
#include "../kernel_macros.h"

namespace AscendC {

template <typename T>
__aicore__ inline void DataCachePreloadImpl(const GlobalTensor<uint64_t>& src, const T cacheOffset)
{
    if constexpr ((IsSameType<T, int16_t>::value) || (IsSameType<T, int64_t>::value)) {
        dc_preload((__gm__ uint64_t*)src.GetPhyAddr(), cacheOffset);
    } else {
        ASCENDC_REPORT_NOT_SUPPORT(false, "current data type");
    }
}

__aicore__ inline void PreLoadImpl(void* pc, const int64_t preFetchLen)
{
    static const struct BinaryMetaIcachePreloadFlag ascIcachePreloadFlag
        __attribute__((used, section(".ascend.meta"))) = {{B_TYPE_ICACHE_PRELOAD_FLAG, 2 * sizeof(uint16_t)}, 1, 0};
    preload(pc, preFetchLen);
}

__aicore__ inline void PreLoad(const int64_t preFetchLen)
{
    int64_t pc = get_pc() & 0xFFFFFFFFFFFF;
    PreLoadImpl(reinterpret_cast<void*>(pc), preFetchLen);
}

__aicore__ inline int64_t GetICachePreloadStatusImpl() { return get_icache_prl_st(); }

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_CACHE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CACHE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CACHE_IMPL_H__
#endif
