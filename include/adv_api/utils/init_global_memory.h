/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file init_global_memory.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "init_global_memory.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INIT_GLOBAL_MEMORY_H__
#endif

#ifndef LIB_UTILS_INIT_GLOBAL_MEMORY_H
#define LIB_UTILS_INIT_GLOBAL_MEMORY_H
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "../../../impl/adv_api/detail/utils/init_global_memory/init_global_memory_v200_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "../../../impl/adv_api/detail/utils/init_global_memory/init_global_memory_v220_impl.h"
#elif (                                                                                               \
    defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 3102 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163))
#include "../../../impl/adv_api/detail/utils/init_global_memory/init_global_memory_v310_impl.h"
#endif

namespace AscendC {
/* !
 * \brief This function realizes the clear global memory function.
 *
 * \note support data type: uint16_t, int16_t, half, float, uint32_t, int32_t
 *
 * \param [out] GlobalTensor
 * \param [in] size, size of space to be initialized
 * \param [in] value, value to be initialized in global memory
 */
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
template <typename T>
__aicore__ inline __in_pipe__(V)
    __out_pipe__(MTE3, S) void Fill(GlobalTensor<T>& gmWorkspaceAddr, const uint64_t size, const T value)
{
    InitGlobalMemoryImpl<T>(gmWorkspaceAddr, size, value);
}
#else
/* !
 * \brief This function realizes the clear global memory function.
 *
 * \note support data type: uint16_t, int16_t, half, float, uint32_t, int32_t
 *
 * \param [out] GlobalTensor
 * \param [in] size, size of space to be initialized
 * \param [in] value, value to be initialized in global memory
 */
template <typename T>
__aicore__ inline __in_pipe__(V)
    __out_pipe__(MTE3) void Fill(GlobalTensor<T>& gmWorkspaceAddr, const uint64_t size, const T value)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 3102 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || \
     __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || \
     __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    InitGlobalMemoryImpl<T>(gmWorkspaceAddr, size, value);
#endif
}
#endif
} // namespace AscendC
#endif // LIB_UTILS_INIT_GLOBAL_MEMORY_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INIT_GLOBAL_MEMORY_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INIT_GLOBAL_MEMORY_H__
#endif
