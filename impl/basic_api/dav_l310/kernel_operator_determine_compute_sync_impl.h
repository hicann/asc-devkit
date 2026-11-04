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
 * \file kernel_operator_determine_compute_sync_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l310/kernel_operator_determine_compute_sync_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H__
#endif
#include "../../../include/basic_api/kernel_operator_common_intf.h"
#ifndef ASCENDC_MODULE_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H

namespace AscendC {
__aicore__ inline void InitDetermineComputeWorkspaceCalc(
    GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported InitDetermineComputeWorkspace"); });
}

__aicore__ inline void WaitPreBlockCalc(const GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported WaitPreBlock"); });
}

__aicore__ inline void NotifyNextBlockCalc(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported NotifyNextBlock"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H__
#endif
