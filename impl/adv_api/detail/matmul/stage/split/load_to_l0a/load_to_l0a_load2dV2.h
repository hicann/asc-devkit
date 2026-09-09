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
 * \file load_to_l0a_load2dV2.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/split/load_to_l0a/load_to_l0a_load2dV2.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD_TO_L0A_LOAD2DV2_H__
#endif

#ifndef IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD2DV2_H
#define IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD2DV2_H

#include "load_to_l0a_intf.h"
#include "../load_to_l0_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, typename A_TYPE, const auto& MM_CFG>
class LoadToL0A<
    IMPL, A_TYPE, MM_CFG,
    enable_if_t<
        (GetGemvMode<A_TYPE>() == GemvMode::MATRIX) &&
        (GetLoadInstrType<typename A_TYPE::T, MM_CFG>() == LoadInstrType::LOAD2DV2)>> {
    using A_T = typename A_TYPE::T;
    using L0A_T = typename Conditional<
        HasScalePosition<A_TYPE>::value, typename GetL0DataType<typename A_TYPE::T, true>::Type,
        typename GetL0DataType<typename A_TYPE::T, false>::Type>::type;
    using AuxDtype = decltype(GetAuxDataType<A_TYPE>());

public:
    __aicore__ inline LoadToL0A(){};
    __aicore__ inline ~LoadToL0A(){};

    __aicore__ inline void Prepare(bool isATranspose, uint16_t aL1K, uint16_t aL1M) const {};
    __aicore__ inline void SetScalar(A_T scalar) {};

    __aicore__ inline void Load(
        const LocalTensor<L0A_T>& dst, const LocalTensor<A_T>& aMatrix, uint16_t aL1M, uint16_t aL1K, uint16_t madM,
        uint16_t madK, uint16_t aL1MOffset, uint16_t aL1KOffset, bool isATranspose,
        const LocalTensor<AuxDtype>& l1AAuxMatrix = {}, uint16_t aAuxL1K = 0, uint16_t aAuxL1KOffset = 0,
        uint16_t aAuxL1MOffset = 0) const
    {
        if (isATranspose) {
            // Mx should run for MXLoad
            if constexpr (!HasScalePosition<A_TYPE>::value) {
                TransLoadDataToL0(dst, aMatrix, aL1K, madM, madK, aL1MOffset, aL1KOffset);
            } else {
                MxTransLoadDataToL0(
                    dst, aMatrix, aL1K, madM, madK, aL1MOffset, aL1KOffset, l1AAuxMatrix, aAuxL1K, aAuxL1KOffset,
                    aAuxL1MOffset);
            }
        } else {
            // Mx should run for MXLoad
            if constexpr (!HasScalePosition<A_TYPE>::value) {
                LoadDataToL0(dst, aMatrix, aL1M, madM, madK, aL1MOffset, aL1KOffset);
            } else {
                MxLoadDataToL0(
                    dst, aMatrix, aL1M, madM, madK, aL1MOffset, aL1KOffset, l1AAuxMatrix, aAuxL1K, aAuxL1KOffset,
                    aAuxL1MOffset);
            }
        }
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<A_T>();

    __aicore__ inline void TransLoadDataToL0(
        const LocalTensor<A_T>& dst, const LocalTensor<A_T>& aMatrix, uint16_t aL1K, uint16_t madM, uint16_t madK,
        uint16_t aL1MOffset, uint16_t aL1KOffset) const
    {
        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(aL1KOffset, BLOCK_CUBE);
        loadDataParams.kStartPosition = CeilDiv(aL1MOffset, c0Size_);
        loadDataParams.kStep = CeilDiv(madM, c0Size_);
        if constexpr (IsSameType<A_T, float>::value) {
            // K step must be multiples of 2 when transpose is enabled ane .type = .b32
            loadDataParams.kStep = CeilAlign(loadDataParams.kStep, K_STEP_MIN_VAL_B32);
        }
        loadDataParams.srcStride = CeilDiv(aL1K, ALIGN_NUM);
        loadDataParams.dstStride = CeilDiv(madM, ALIGN_NUM);
        loadDataParams.ifTranspose = true;
        loadDataParams.mStep = CeilDiv(madK, HW_M0);
        if constexpr (IsSupportB4<A_T>()) {
            // M step must be multiples of 4 when transpose is enabled and .type = .b4
            loadDataParams.mStep = CeilAlign(loadDataParams.mStep, M_STEP_MIN_VAL_B4);
        }

        if constexpr (IsSupportB8<A_T>()) {
            // M step must be multiples of 2 when transpose is enabled and .type = .b8
            uint16_t l0ALoop = CeilAlign(loadDataParams.mStep, M_STEP_MIN_VAL_B8) / M_STEP_MIN_VAL_B8;
            uint64_t dstOffset = 0;
            uint64_t dstAddrStride = CeilAlign(madM, ALIGN_NUM) * ONE_BLK_SIZE;
            loadDataParams.mStep = M_STEP_MIN_VAL_B8;
            uint16_t oriMstartPos = loadDataParams.mStartPosition;
            // K axis is m direction, and M axis is k direction in load2dv2 intrin
            for (uint16_t idx = 0; idx < l0ALoop; ++idx) {
                loadDataParams.mStartPosition = oriMstartPos + M_STEP_MIN_VAL_B8 * idx;
                LoadData(dst[dstOffset], aMatrix, loadDataParams);
                dstOffset += dstAddrStride;
            }
        }
#if __NPU_ARCH__ == 5102
        else if constexpr (IsSupportB4<A_T>()) {
            uint16_t l0ALoop = loadDataParams.mStep / M_STEP_MIN_VAL_B4;
            uint64_t dstOffset = 0;
            uint64_t dstAddrStride = CeilAlign(madM, ALIGN_NUM) * ONE_BLK_SIZE * 2;
            loadDataParams.mStep = M_STEP_MIN_VAL_B4;
            uint16_t oriMstartPos = loadDataParams.mStartPosition;
            // K axis is m direction, and M axis is k direction in load2dv2 intrin
            for (uint16_t idx = 0; idx < l0ALoop; ++idx) {
                loadDataParams.mStartPosition = oriMstartPos + M_STEP_MIN_VAL_B4 * idx;
                LoadData(dst[dstOffset], aMatrix, loadDataParams);
                dstOffset += dstAddrStride;
            }
        }
#endif
        else if constexpr (IsSameType<A_T, float>::value) {
            // in case of mdl && basek=8, the unit of mStartPosition is 16, so don't use it
            loadDataParams.mStartPosition = 0;
            loadDataParams.kStartPosition = 0;
            uint64_t matrixOffset = aL1MOffset * CeilAlign(aL1K, BLOCK_CUBE) + aL1KOffset * B32_C0SIZE;
            LoadData(dst, aMatrix[matrixOffset], loadDataParams);
        } else {
            LoadData(dst, aMatrix, loadDataParams);
        }
    }

    __aicore__ inline void LoadDataToL0(
        const LocalTensor<A_T>& dst, const LocalTensor<A_T>& aMatrix, uint16_t aL1M, uint16_t madM, uint16_t madK,
        uint16_t aL1MOffset, uint16_t aL1KOffset) const
    {
        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(aL1MOffset, BLOCK_CUBE);
        loadDataParams.kStartPosition = CeilDiv(aL1KOffset, c0Size_);
        loadDataParams.mStep = CeilDiv(madM, HW_M0);
        loadDataParams.kStep = CeilDiv(madK, c0Size_);
        loadDataParams.srcStride = CeilDiv(aL1M, ALIGN_NUM);
        loadDataParams.dstStride = CeilDiv(madM, ALIGN_NUM);
        loadDataParams.ifTranspose = false;
        LoadData(dst, aMatrix, loadDataParams);
    }

    __aicore__ inline void MxTransLoadDataToL0(
        const LocalTensor<L0A_T>& dst, const LocalTensor<A_T>& aMatrix, uint16_t aL1K, uint16_t madM, uint16_t madK,
        uint16_t aL1MOffset, uint16_t aL1KOffset, const LocalTensor<AuxDtype>& l1AAuxMatrix, uint16_t aAuxL1K,
        uint16_t aAuxL1KOffset, uint16_t aAuxL1MOffset) const
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        uint16_t mStep = CeilDiv(madM, HW_M0);
        uint16_t kStep = CeilDiv(madK, c0Size_);

        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(aL1KOffset, ALIGN_NUM);
        loadDataParams.kStartPosition = CeilDiv(aL1MOffset, c0Size_);
        loadDataParams.mStep = CeilDiv(madK, HW_M0);
        loadDataParams.kStep = CeilDiv(madM, c0Size_);
        loadDataParams.srcStride = CeilDiv(aL1K, HW_M0);
        loadDataParams.dstStride = CeilDiv(madM, HW_M0);
        loadDataParams.ifTranspose = true;

        LoadData2DMxParams loadDataMxParams;
        loadDataMxParams.xStartPosition = CeilDiv(aAuxL1MOffset, ALIGN_NUM);
        loadDataMxParams.xStep = mStep;
        if constexpr (SupportType<A_T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            loadDataMxParams.yStartPosition = CeilDiv(aAuxL1KOffset, FP4_TWO);
            loadDataMxParams.yStep = kStep;
            loadDataMxParams.srcStride = CeilDiv(aAuxL1K, FP4_TWO);
            loadDataMxParams.dstStride = CeilDiv(madK, c0Size_);
        } else if constexpr (SupportType<A_T, fp8_e5m2_t, fp8_e4m3fn_t>()) {
            // for FP8 ,two K0 on the k axis correspond to a small z fractal.
            loadDataMxParams.yStartPosition = CeilDiv(aAuxL1KOffset, FP8_TWO);
            loadDataMxParams.yStep = CeilDiv(madK, c0Size_ * FP8_TWO);
            loadDataMxParams.srcStride = CeilDiv(aAuxL1K, FP8_TWO);
            loadDataMxParams.dstStride = CeilDiv(madK, c0Size_ * FP8_TWO);
        }
        LoadData(dst, aMatrix, l1AAuxMatrix, loadDataParams, loadDataMxParams);
#endif
    }

    __aicore__ inline void MxLoadDataToL0(
        const LocalTensor<L0A_T>& dst, const LocalTensor<A_T>& aMatrix, uint16_t aL1M, uint16_t madM, uint16_t madK,
        uint16_t aL1MOffset, uint16_t aL1KOffset, const LocalTensor<AuxDtype>& l1AAuxMatrix, uint16_t aAuxL1K,
        uint16_t aAuxL1KOffset, uint16_t aAuxL1MOffset) const
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        uint16_t mStep = CeilDiv(madM, HW_M0);
        uint16_t kStep = CeilDiv(madK, c0Size_);

        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(aL1MOffset, BLOCK_CUBE);
        loadDataParams.kStartPosition = CeilDiv(aL1KOffset, c0Size_);
        loadDataParams.mStep = mStep;
        loadDataParams.kStep = kStep;
        loadDataParams.srcStride = CeilDiv(aL1M, HW_M0);
        loadDataParams.dstStride = CeilDiv(madM, HW_M0);

        LoadData2DMxParams loadDataMxParams;
        loadDataMxParams.xStartPosition = CeilDiv(aAuxL1MOffset, BLOCK_CUBE);
        loadDataMxParams.xStep = mStep;
        if constexpr (SupportType<A_T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            loadDataMxParams.yStartPosition = CeilDiv(aAuxL1KOffset, FP4_TWO);
            loadDataMxParams.yStep = kStep;
            loadDataMxParams.srcStride = CeilDiv(aAuxL1K, FP4_TWO);
            loadDataMxParams.dstStride = CeilDiv(madK, c0Size_);
        } else if constexpr (SupportType<A_T, fp8_e5m2_t, fp8_e4m3fn_t>()) {
            // for FP8 ,two K0 on the k axis correspond to a small z fractal.
            loadDataMxParams.yStartPosition = CeilDiv(aAuxL1KOffset, FP8_TWO);
            loadDataMxParams.yStep = CeilDiv(madK, c0Size_ * FP8_TWO);
            loadDataMxParams.srcStride = CeilDiv(aAuxL1K, FP8_TWO);
            loadDataMxParams.dstStride = CeilDiv(madK, c0Size_ * FP8_TWO);
        }
        LoadData(dst, aMatrix, l1AAuxMatrix, loadDataParams, loadDataMxParams);
#endif
    }
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD_TO_L0A_LOAD2DV2_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD_TO_L0A_LOAD2DV2_H__
#endif // IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0A_LOAD2DV2_H
