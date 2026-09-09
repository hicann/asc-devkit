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
 * \file c1_buffer.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/resource/bias_buffer/c1_buffer/c1_buffer.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_H
#define IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_H

#include "c1_buffer_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {

/**
 * C1Buffer: responsible for L1 bias buffer management.
 * This module provides abilities to allocate or free one l1 bias buffer block, and pipeline synchronization.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * CopyBiasIn is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, class BIAS_TYPE, class A_TYPE, const auto& MM_CFG>
class C1Buffer<
    IMPL, BIAS_TYPE, A_TYPE, MM_CFG,
    enable_if_t<
        ToMatmulConfig(MM_CFG).enableSetBias &&
        (DoMatmulMDL(MM_CFG) || isNormEnableScheduler<A_TYPE, MM_CFG> || IsBmmEnableScheduler<A_TYPE, MM_CFG> ||
         DoMatmulSpecialMDL(MM_CFG) || IsBasicBlockEnable<MM_CFG> || DoMatmulIBShareNorm(MM_CFG))>> {
    MATMUL_USE_MODULE(L1Manager);
    using BiasT = typename BIAS_TYPE::T;

public:
    __aicore__ inline C1Buffer() {}
    __aicore__ inline ~C1Buffer() {}
    __aicore__ inline void Init(int32_t biasLen)
    {
        // l1 split to upper and lower half
        if constexpr (ToMatmulConfig(MM_CFG).enableL1BankConflictOptimise) {
            auto initBufferSize = biasLen * sizeof(BiasT);
            auto addr0 = AscendC::Std::make_tuple(MATMUL_MODULE(L1Manager)->GetCurrentUpperHalfAddr(), initBufferSize);
            GetTPipePtr()->InitBuffer(qidBias_, addr0);
            MATMUL_MODULE(L1Manager)->SetCurrentAddress(initBufferSize);
        } else {
            GetTPipePtr()->InitBuffer(qidBias_, 1, biasLen * sizeof(BiasT));
        }
    }

    __aicore__ inline LocalTensor<BiasT> AllocTensor() { return qidBias_.template AllocTensor<BiasT>(); }

    __aicore__ inline void FreeTensor(const LocalTensor<BiasT>& tensor = LocalTensor<BiasT>{})
    {
        qidBias_.FreeTensor(const_cast<LocalTensor<BiasT>&>(tensor));
    }

    __aicore__ inline void EnQue(LocalTensor<BiasT>& tensor) { qidBias_.EnQue(tensor); }

    __aicore__ inline LocalTensor<BiasT> DeQue() { return qidBias_.template DeQue<BiasT>(); }

    __aicore__ inline void Destroy() { qidBias_.FreeAllEvent(); }

    __aicore__ inline uint64_t GetBufferHeadAddr()
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        return GetTQueHeadAddr(qidBias_);
#else
        return 0;
#endif
    }

private:
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    static constexpr TQueConfig staticL1Evt = {
        .nd2nz = false,
        .nz2nd = false,
        .scmBlockGroup = false,
        .bufferLen = 0,
        .bufferNumber = 1,
        .consumerSize = 0,
        .consumer = {},
        .enableStaticEvtId = true};
    TQue<TPosition::C1, QUEUE_DEPTH, &staticL1Evt> qidBias_;
#else
    TQue<TPosition::C1, QUEUE_DEPTH> qidBias_;
#endif
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _C1_BUFFER_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_H__
#endif
