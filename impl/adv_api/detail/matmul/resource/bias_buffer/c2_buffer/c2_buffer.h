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
 * \file c2_buffer.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/resource/bias_buffer/c2_buffer/c2_buffer.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_H
#define IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_H

#include "c2_buffer_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {

/**
 * C2Buffer: responsible for C2 buffer management.
 * This module provides abilities to allocate or free one C2 buffer block, and pipeline synchronization.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * CopyBiasIn is only for internal usage, does not support extension or customized specialization!
 */
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
constexpr int32_t BIAS_TABLE_SIZE = 1024 * 4;
#else
constexpr int32_t BIAS_TABLE_SIZE = 1024;
#endif
template <typename IMPL, typename L0cT, class A_TYPE, const auto& MM_CFG>
class C2Buffer<
    IMPL, L0cT, A_TYPE, MM_CFG,
    enable_if_t<
        ToMatmulConfig(MM_CFG).enableSetBias &&
        (DoMatmulMDL(MM_CFG) || isNormEnableScheduler<A_TYPE, MM_CFG> || IsBmmEnableScheduler<A_TYPE, MM_CFG> ||
         DoMatmulSpecialMDL(MM_CFG) || IsBasicBlockEnable<MM_CFG> || DoMatmulIBShareNorm(MM_CFG))>> {
public:
    __aicore__ inline C2Buffer() {}
    __aicore__ inline ~C2Buffer() {}

    __aicore__ inline void Init() { GetTPipePtr()->InitBuffer(biasBuf_, BIAS_TABLE_SIZE); }

    __aicore__ inline LocalTensor<L0cT> Allocate()
    {
        WaitFlag<HardEvent::M_MTE1>(EVENT_ID2);
        LocalTensor<L0cT> biasC2 = biasBuf_.Get<L0cT>();
        return biasC2;
    }

    __aicore__ inline void Free() { SetFlag<HardEvent::M_MTE1>(EVENT_ID2); }

    __aicore__ inline void EnQue() { SetFlag<HardEvent::MTE1_M>(EVENT_ID2); }

    __aicore__ inline void DeQue() { WaitFlag<HardEvent::MTE1_M>(EVENT_ID2); }

private:
    TBuf<TPosition::C2> biasBuf_;
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _C2_BUFFER_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_H__
#endif
