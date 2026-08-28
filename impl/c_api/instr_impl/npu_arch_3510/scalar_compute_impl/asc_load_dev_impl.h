/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)

#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SCALAR_COMPUTE_IMPL_ASC_LOAD_DEV_IMPL_H
#define IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SCALAR_COMPUTE_IMPL_ASC_LOAD_DEV_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

constexpr int16_t ASC_C_API_LOAD_DEV_DEFAULT_OFFSET = 0;

__aicore__ inline int8_t asc_load_dev_impl(__gm__ int8_t* addr)
{
    return ld_dev(reinterpret_cast<__gm__ uint8_t*>(addr), ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}

__aicore__ inline uint8_t asc_load_dev_impl(__gm__ uint8_t* addr)
{
    return ld_dev(addr, ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}

__aicore__ inline int16_t asc_load_dev_impl(__gm__ int16_t* addr)
{
    return ld_dev(reinterpret_cast<__gm__ uint16_t*>(addr), ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}

__aicore__ inline uint16_t asc_load_dev_impl(__gm__ uint16_t* addr)
{
    return ld_dev(addr, ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}

__aicore__ inline int32_t asc_load_dev_impl(__gm__ int32_t* addr)
{
    return ld_dev(reinterpret_cast<__gm__ uint32_t*>(addr), ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}

__aicore__ inline uint32_t asc_load_dev_impl(__gm__ uint32_t* addr)
{
    return ld_dev(addr, ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}

__aicore__ inline int64_t asc_load_dev_impl(__gm__ int64_t* addr)
{
    return ld_dev(addr, ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}

__aicore__ inline uint64_t asc_load_dev_impl(__gm__ uint64_t* addr)
{
    return ld_dev(addr, ASC_C_API_LOAD_DEV_DEFAULT_OFFSET);
}
#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
