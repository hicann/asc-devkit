/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/common/check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/kernel_api.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CHECK_H__
#endif

#ifndef IMPL_COMMON_CHECK_H
#define IMPL_COMMON_CHECK_H

namespace AscendC {

template <typename T>
__aicore__ inline void CheckTensorPosition(
    const LocalTensor<T>& checkTensor, __gm__ const char* tensorInfo, __gm__ const char* supportPosInfo)
{
#if ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT(
        ((TPosition)checkTensor.GetPosition() == TPosition::VECIN ||
         (TPosition)checkTensor.GetPosition() == TPosition::VECOUT ||
         (TPosition)checkTensor.GetPosition() == TPosition::VECCALC),
        {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check tensor position of %s, current api support positions are %s, current position is %s.",
                tensorInfo, GetPositionDisplay(Hardware::UB, supportPosInfo).c_str(),
                GetPositionDisplay(static_cast<TPosition>(checkTensor.GetPosition())).c_str());
        });
#endif
}

template <typename T>
__aicore__ inline void CheckCalCount(
    const uint32_t calCount, __gm__ const char* calCountInfo, const LocalTensor<T>& checkTensor,
    __gm__ const char* tensorInfo, __gm__ const char* apiInfo)
{
#if ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT((calCount <= checkTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "The %s parameter cannot be %u, should not be larger than %s size %u in %s.", calCountInfo,
            calCount, tensorInfo, checkTensor.GetSize(), apiInfo);
    });
#endif
}

__aicore__ inline void CheckTmpBufferSize(
    const uint32_t checkBufferSize, const uint32_t compBufferSize, const uint32_t tmpBufferSize)
{
    ASCENDC_ASSERT((checkBufferSize > compBufferSize), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Insufficient temporary space, current operation is not enough, "
            "but only %u units are available, please check the host tiling.",
            tmpBufferSize);
    });
}

} // namespace AscendC

#endif // IMPL_COMMON_CHECK_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CHECK_H__
#endif
