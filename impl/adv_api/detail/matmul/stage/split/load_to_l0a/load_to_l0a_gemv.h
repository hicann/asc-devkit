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
 * \file load_to_l0a_gemv.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/split/load_to_l0a/load_to_l0a_gemv.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD_TO_L0A_GEMV_H__
#endif

#ifndef IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_GEMV_H
#define IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_GEMV_H

#include "load_to_l0a_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, typename A_TYPE, const auto& MM_CFG>
class LoadToL0A<
    IMPL, A_TYPE, MM_CFG,
    enable_if_t<GetGemvMode<A_TYPE>() == GemvMode::SCALAR && !MatmulFeatureTrait<MM_CFG>::IsNeedUB()>> {
    using A_T = typename A_TYPE::T;
    using AuxDtype = decltype(GetAuxDataType<A_TYPE>());

public:
    __aicore__ inline LoadToL0A(){};
    __aicore__ inline ~LoadToL0A(){};

    __aicore__ inline void SetScalar(A_T scalar)
    {
        // A/B does not come from GM with IBShare is not supported
        if constexpr (DoMatmulIBShareNorm(MM_CFG) && A_TYPE::ibShare) {
            ASCENDC_ASSERT((false), {
                KERNEL_LOG(KERNEL_ERROR, "It is not allowed to set matrix A with scalar when matmul A is ibShare.");
            });
        }
        aScalar_ = scalar;
    }

    __aicore__ inline void Prepare(bool isATranspose, uint16_t aL1K, uint16_t aL1M) const {};

    __aicore__ inline void Load(
        const LocalTensor<A_T>& l0A, const LocalTensor<A_T>& l1A, uint16_t aL1M, uint16_t aL1K, uint16_t madM,
        uint16_t madK, uint16_t aL1MOffset, uint16_t aL1KOffset, bool isATranspose,
        const LocalTensor<AuxDtype>& l1AAuxMatrix = {}, uint16_t aAuxL1K = 0, uint16_t aAuxL1KOffset = 0) const
    {
        ASSERT(madM == 1);
        InitConstValueParams initConstValueParams{1, (uint16_t)ConstCeil(madK, BLOCK_CUBE * c0Size_), 0, aScalar_};
        InitConstValue(l0A, initConstValueParams);
        return;
    }

private:
    A_T aScalar_;
    constexpr static int32_t c0Size_ = AuxGetC0Size<A_T>();
};

template <typename IMPL, typename A_TYPE, const auto& MM_CFG>
class LoadToL0A<
    IMPL, A_TYPE, MM_CFG,
    enable_if_t<(GetGemvMode<A_TYPE>() == GemvMode::VECTOR) && (!HasScalePosition<A_TYPE>::value)>> {
    using A_T = typename A_TYPE::T;
    using AuxDtype = decltype(GetAuxDataType<A_TYPE>());

public:
    __aicore__ inline LoadToL0A(){};
    __aicore__ inline ~LoadToL0A(){};

    __aicore__ inline void SetScalar(A_T scalar) {};

    __aicore__ inline void Prepare(bool isATranspose, uint16_t aL1K, uint16_t aL1M) {};

    __aicore__ inline void Load(
        LocalTensor<A_T>& l0A, const LocalTensor<A_T>& l1A, uint16_t aL1M, uint16_t aL1K, uint16_t madM, uint16_t madK,
        uint16_t aL1MOffset, uint16_t aL1KOffset, bool isATranspose, const LocalTensor<AuxDtype>& l1AAuxMatrix = {},
        uint16_t aAuxL1K = 0, uint16_t aAuxL1KOffset = 0) const
    {
        if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2()) {
            LoadData2DParamsV2 loadDataParams;
            loadDataParams.mStartPosition = 0;
            loadDataParams.kStartPosition = 0;
            loadDataParams.mStep = CeilDiv(madM, HW_M0);
            loadDataParams.kStep = CeilDiv(madK, c0Size_);
            loadDataParams.srcStride = CeilDiv(aL1M, ALIGN_NUM);
            loadDataParams.dstStride = CeilDiv(madM, ALIGN_NUM);
            loadDataParams.ifTranspose = false;
            LoadData(l0A, l1A[aL1KOffset], loadDataParams);
        } else {
            int FracSize = BYTE_PER_FRACTAL / sizeof(A_T);
            int repeat = Ceil(madK, FracSize);
            LoadData2dParams loadDataParams;
            loadDataParams.repeatTimes = repeat;
            loadDataParams.srcStride = 1;
            LoadData(l0A[0], l1A[aL1KOffset], loadDataParams);
        }
        return;
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<A_T>();
};

template <typename IMPL, typename A_TYPE, const auto& MM_CFG>
class LoadToL0A<
    IMPL, A_TYPE, MM_CFG,
    enable_if_t<(GetGemvMode<A_TYPE>() == GemvMode::VECTOR) && (HasScalePosition<A_TYPE>::value)>> {
    using A_T = typename A_TYPE::T;
    using L0A_T = typename Conditional<
        HasScalePosition<A_TYPE>::value, typename GetL0DataType<typename A_TYPE::T, true>::Type,
        typename GetL0DataType<typename A_TYPE::T, false>::Type>::type;
    using AuxDtype = decltype(GetAuxDataType<A_TYPE>());

public:
    __aicore__ inline LoadToL0A(){};
    __aicore__ inline ~LoadToL0A(){};

    __aicore__ inline void SetScalar(A_T scalar) {};

    __aicore__ inline void Prepare(bool isATranspose, uint16_t aL1K, uint16_t aL1M) {};

    __aicore__ inline void Load(
        LocalTensor<L0A_T>& l0A, const LocalTensor<A_T>& l1A, uint16_t aL1M, uint16_t aL1K, uint16_t madM,
        uint16_t madK, uint16_t aL1MOffset, uint16_t aL1KOffset, bool isATranspose,
        const LocalTensor<AuxDtype>& l1AAuxMatrix = {}, uint16_t aAuxL1K = 0, uint16_t aAuxL1KOffset = 0,
        uint16_t aAuxL1MOffset = 0) const
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = 0;
        loadDataParams.kStartPosition = 0;
        loadDataParams.mStep = 1;
        loadDataParams.kStep = CeilDiv(madK, c0Size_ * HW_M0);
        loadDataParams.srcStride = 1;
        loadDataParams.dstStride = 1;

        uint8_t yStep = CeilDiv(madK, B8_C0SIZE * MX_K_FACTOR);
        LoadData2DMxParams loadDataMxParams;
        loadDataMxParams.xStartPosition = 0;
        loadDataMxParams.yStartPosition = 0;
        loadDataMxParams.xStep = 1;
        loadDataMxParams.yStep = yStep;
        loadDataMxParams.srcStride = yStep;
        loadDataMxParams.dstStride = 0;
        LoadData(l0A, l1A[aL1KOffset], l1AAuxMatrix[aAuxL1KOffset], loadDataParams, loadDataMxParams);
#endif
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<A_T>();
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD_TO_L0A_GEMV_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD_TO_L0A_GEMV_H__
#endif // IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_GEMV_H
