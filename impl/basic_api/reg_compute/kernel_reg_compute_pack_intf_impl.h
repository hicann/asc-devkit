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
 * \file kernel_reg_compute_pack_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_pack_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_pack_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_PACK_INTF_IMPL__
#endif

#ifndef ASCENDC_KERNEL_REG_COMPUTE_PACK_INTERFACE_IMPL_H
#define ASCENDC_KERNEL_REG_COMPUTE_PACK_INTERFACE_IMPL_H

#if __NPU_ARCH__ == 3003
#include "../../basic_api/reg_compute/dav_l300/kernel_reg_compute_pack_impl.h"
#elif __NPU_ARCH__ == 3113
#include "../../basic_api/reg_compute/dav_l311/kernel_reg_compute_pack_impl.h"
#elif __NPU_ARCH__ == 5102
#include "../../basic_api/reg_compute/dav_m510/kernel_reg_compute_pack_impl.h"
#elif __NPU_ARCH__ == 5162
#include "../../basic_api/reg_compute/dav_v516/kernel_reg_compute_pack_impl.h"
#else
#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_pack_impl.h"
#endif

namespace AscendC {
namespace Reg {
template <typename T, typename U, HighLowPart part, typename S, typename V>
__simd_callee__ inline void Pack(S& dstReg, V& srcReg)
{
    PackImpl<T, U, part, S, V>(dstReg, srcReg);
}

template <typename T, typename U, HighLowPart part, typename S, typename V>
__simd_callee__ inline void UnPack(S& dstReg, V& srcReg)
{
    UnPackImpl<T, U, part, S, V>(dstReg, srcReg);
}
} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_KERNEL_REG_COMPUTE_PACK_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_PACK_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_PACK_INTF_IMPL__
#endif
