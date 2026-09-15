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
 * \file load_bias_to_c2.h
 * \brief load bias from c1 to c2 buffer
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/bias/load_bias_to_c2.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BIAS_LOAD_BIAS_TO_C2_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BIAS_LOAD_BIAS_TO_C2_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BIAS_LOAD_BIAS_TO_C2_H

#include "../../../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {

constexpr int32_t DOUBLE_NUM = 2;
constexpr int32_t LEN_CEIL_DIV_NUM = 32;

/**
 * LoadBias2C2: responsible for load bias data into C2 buffer.
 * This module provides abilities to copy bias data in C2 Buffer.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * LoadBias2C2 is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, class A_TYPE, class BIAS_TYPE, const auto& MM_CFG, typename = void>
class LoadBias2C2 {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    using BiasT = typename BIAS_TYPE::T;

public:
    __aicore__ inline LoadBias2C2() = default;
    __aicore__ inline ~LoadBias2C2() = default;

    /**
     * @description: Load bias data from C1 to C2
     * @param: biasC2: dst tensor in c2
     * @param: bias: src tensor from l1
     * @param: dataLen: block length of one load
     * @return: void
     */
    __aicore__ inline void Load(const LocalTensor<L0cT>& biasC2, const LocalTensor<BiasT>& bias, int32_t dataLen) {}
};

template <typename IMPL, class A_TYPE, class BIAS_TYPE, const auto& MM_CFG>
class LoadBias2C2<
    IMPL, A_TYPE, BIAS_TYPE, MM_CFG,
    enable_if_t<
        ToMatmulConfig(MM_CFG).enableSetBias && !MatmulFeatureTrait<MM_CFG>::IsNeedUB() &&
        (DoMatmulMDL(MM_CFG) || isNormEnableScheduler<A_TYPE, MM_CFG> || IsBmmEnableScheduler<A_TYPE, MM_CFG> ||
         DoMatmulSpecialMDL(MM_CFG) || IsBasicBlockEnable<MM_CFG> || DoMatmulIBShareNorm(MM_CFG))>> {
    using A_T = typename A_TYPE::T;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    using BiasT = typename BIAS_TYPE::T;

public:
    __aicore__ inline LoadBias2C2() = default;
    __aicore__ inline ~LoadBias2C2() = default;

    __aicore__ inline void Load(const LocalTensor<L0cT>& biasC2, const LocalTensor<BiasT>& bias, int32_t dataLen)
    {
        constexpr auto oneDataLen = (GetBitSize<L0cT>() == GetBitSize<BiasT>()) ? 2 : 1; // 2:f32, 1:f16/bf16
        if (MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2()) {
            uint16_t lenBurst = CeilDiv(dataLen * oneDataLen * DOUBLE_NUM, LEN_CEIL_DIV_NUM);
            if constexpr (GetBitSize<L0cT>() == GetBitSize<BiasT>()) {
                lenBurst = CeilAlign(lenBurst, DOUBLE_NUM);
            }
            DataCopy(biasC2, bias, {1, lenBurst, 0, 0});
            return;
        }
        uint16_t lenBurst = (dataLen * oneDataLen * 2 + 63) / 64;
        DataCopy(biasC2, bias, {1, lenBurst, 0, 0});
    }
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _LOAD_BIAS_TO_C2_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BIAS_LOAD_BIAS_TO_C2_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BIAS_LOAD_BIAS_TO_C2_H__
#endif
