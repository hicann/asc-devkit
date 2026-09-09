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
 * \file mmad_compute.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/compute/mmad_compute.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COMPUTE_MMAD_COMPUTE_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COMPUTE_MMAD_COMPUTE_H
#define IMPL_MATMUL_STAGE_COMPUTE_MMAD_COMPUTE_H

#include "../../utils/matmul_module.h"
#include "../../utils/matmul_param.h"
#include "../../feature_trait/matmul_feature_trait.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, typename C_T, typename A_TYPE, typename B_T, const auto& MM_CFG>
class MmadCompute {
    using A_T = typename Conditional<
        HasScalePosition<A_TYPE>::value, typename GetL0DataType<typename A_TYPE::T, true>::Type,
        typename GetL0DataType<typename A_TYPE::T, false>::Type>::type;

public:
    template <bool hasSpIdx = false>
    inline __aicore__ void Compute(
        const LocalTensor<C_T>& cMatrix, const LocalTensor<A_T>& l0A, const LocalTensor<B_T>& l0B, uint16_t mmadM,
        uint16_t mmadK, uint16_t mmadN, bool isATrans, bool isBTrans, uint8_t unitFlag = 0, bool cmatrixSource = false,
        bool cmatrixInitVal = true, bool isBias = false)
    {
        MmadParams mmadParams;
        mmadParams.m = mmadM;
        mmadParams.k = mmadK;
        mmadParams.n = mmadN;

#if __NPU_ARCH__ == 5102
        if constexpr (IsSameTypeV<A_T, half>) {
            mmadParams.fixShiftVal = MATMUL_CONST_PARAM_VAR.fixShiftValue;
        }
#endif

        if constexpr (MatmulFeatureTrait<MM_CFG>::IsUnitFlagEnabled()) {
            mmadParams.unitFlag = unitFlag;
        }
        if constexpr (
            IsSameType<C_T, float>::value && IsSameType<A_T, float>::value &&
            !MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2()) {
            if (isATrans) {
                mmadParams.kDirectionAlign = 1;
            }
        }

        if constexpr (!MatmulFeatureTrait<MM_CFG>::IsSupportCmatrixInitVal()) {
            mmadParams.cmatrixSource = cmatrixSource;
            mmadParams.cmatrixInitVal = cmatrixInitVal;
        } else {
            mmadParams.isBias = isBias;
        }
        if constexpr (hasSpIdx) {
            // C310 Not support mmadwithsparse
            MmadWithSparse(cMatrix, l0A, l0B, mmadParams);
        } else {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
            if constexpr (A_TYPE::format != CubeFormat::VECTOR) {
                mmadParams.disableGemv = true;
            }
#endif
            Mmad(cMatrix, l0A, l0B, mmadParams);
        }

        if ((mmadM / ALIGN_NUM) * (mmadN / ALIGN_NUM) < LIMIT_MNSIZE) { // add pipe_M required by aicore
            PipeBarrier<PIPE_M>();
        }
    }

private:
    static constexpr uint16_t LIMIT_MNSIZE = 10;
    static constexpr uint16_t ALIGN_NUM = 16;
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC

#endif // IMPL_MATMUL_STAGE_COMPUTE_MMAD_COMPUTE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COMPUTE_MMAD_COMPUTE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COMPUTE_MMAD_COMPUTE_H__
#endif
