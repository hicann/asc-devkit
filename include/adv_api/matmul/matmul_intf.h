/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file matmul_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "matmul_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_INTF_H__
#endif

#ifndef LIB_MATMUL_MATMUL_INTF_H
#define LIB_MATMUL_MATMUL_INTF_H

#include "../../impl/adv_api/detail/kfc/kfc_register_obj.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "../../impl/adv_api/detail/kfc/kernel_kfc.h"
#else
#include "include/adv_api/matmul/matmul.h"
#endif

#ifdef ASCENDC_MATMUL_AICORE
#include "../../impl/adv_api/detail/matmul/kfc/matmul_client_impl_aicore.h"
#endif

namespace AscendC {
#define REGIST_MATMUL_OBJ_STATIC REGIST_CUBE_OBJ
#define REGIST_MATMUL_OBJ REGIST_CUBE_OBJ
#ifdef ASCENDC_CPU_DEBUG
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#ifdef ASCENDC_CUBE_ONLY
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
using Matmul = MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#else
// Otherwise, use MatmulClient
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>

using Matmul = MatmulClient<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#endif // ASCENDC_CUBE_ONLY

#else
// Use MatmulImpl in other cases
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>

using Matmul = MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#endif // defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201

#else // Not in ASCENDC_CPU_DEBUG mode

#ifdef SPLIT_CORE_CUBE
#ifdef ASCENDC_CUBE_ONLY
// If SPLIT_CORE_CUBE and ASCENDC_CUBE_ONLY modes are enabled, use MatmulImpl
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
using Matmul = MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#else
// Otherwise, use MatmulServiceAux
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
using Matmul = MatmulServiceAux<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#endif // ASCENDC_CUBE_ONLY

#elif defined(SPLIT_CORE_VEC)
// If SPLIT_CORE_VEC mode is enabled, use MatmulClient
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
using Matmul = MatmulClient<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#elif defined(ASCENDC_MATMUL_AICORE) && !defined(ASCENDC_CUBE_ONLY)
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
using Matmul = MatmulClient<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#else
// Default case: use MatmulImpl
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
using Matmul = MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

#endif // SPLIT_CORE_CUBE || SPLIT_CORE_VEC
#endif // ASCENDC_CPU_DEBUG
} // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_INTF_H__
#endif
