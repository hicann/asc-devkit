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
 * \file load_to_l0b_load2dV2.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/split/load_to_l0b/load_to_l0b_load2dV2.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_LOAD2DV2_H__
#endif

#ifndef IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD2DV2_H
#define IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD2DV2_H

#include "load_to_l0b_intf.h"
#include "../load_to_l0_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, typename INPUT_TYPE, const auto& MM_CFG>
class LoadToL0B<
    IMPL, INPUT_TYPE, MM_CFG,
    enable_if_t<
        (GetGemvMode<INPUT_TYPE>() == GemvMode::MATRIX) &&
        (GetLoadInstrType<typename INPUT_TYPE::T, MM_CFG>() == LoadInstrType::LOAD2DV2)>> {
    using B_T = typename INPUT_TYPE::TRANS_T;
    using L0B_T = typename Conditional<
        HasScalePosition<INPUT_TYPE>::value, typename GetL0DataType<typename INPUT_TYPE::TRANS_T, true>::Type,
        typename GetL0DataType<typename INPUT_TYPE::TRANS_T, false>::Type>::type;
    using AuxDtype = decltype(GetAuxDataType<INPUT_TYPE>());

public:
    __aicore__ inline LoadToL0B(){};
    __aicore__ inline ~LoadToL0B(){};

    __aicore__ inline void Load(
        const LocalTensor<L0B_T>& dst, const LocalTensor<B_T>& bMatrix, uint16_t bL1N, uint16_t bL1K, uint16_t madN,
        uint16_t madK, uint16_t bL1NOffset, uint16_t bL1KOffset, bool isBTranspose,
        const LocalTensor<AuxDtype>& l1BAuxMatrix = {}, uint16_t bAuxL1K = 0, uint16_t bAuxL1KOffset = 0,
        uint16_t bAuxL1NOffset = 0) const
    {
        if (isBTranspose) {
            // Mx should run for MXLoad
            if constexpr (!HasScalePosition<INPUT_TYPE>::value) {
                TransLoadDataToL0(dst, bMatrix, bL1N, madN, madK, bL1NOffset, bL1KOffset);
            } else {
                MxTransLoadDataToL0(
                    dst, bMatrix, bL1N, madN, madK, bL1NOffset, bL1KOffset, l1BAuxMatrix, bAuxL1K, bAuxL1KOffset,
                    bAuxL1NOffset);
            }
        } else {
            // Mx should run for MXLoad
            if constexpr (!HasScalePosition<INPUT_TYPE>::value) {
                LoadDataToL0(dst, bMatrix, bL1K, madN, madK, bL1NOffset, bL1KOffset);
            } else {
                MxLoadDataToL0(
                    dst, bMatrix, bL1K, madN, madK, bL1NOffset, bL1KOffset, l1BAuxMatrix, bAuxL1K, bAuxL1KOffset,
                    bAuxL1NOffset);
            }
        }
    }

    __aicore__ inline void Prepare(bool isBTranspose, uint16_t bL1K) const {};

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<B_T>();

    __aicore__ inline void TransLoadDataToL0(
        const LocalTensor<B_T>& dst, const LocalTensor<B_T>& bMatrix, uint16_t bL1N, uint16_t madN, uint16_t madK,
        uint16_t bL1NOffset, uint16_t bL1KOffset) const
    {
        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(bL1NOffset, BLOCK_CUBE);
        loadDataParams.kStartPosition = CeilDiv(bL1KOffset, c0Size_);
        loadDataParams.mStep = CeilDiv(madN, HW_M0);
        loadDataParams.kStep = CeilDiv(madK, c0Size_);
        loadDataParams.srcStride = CeilDiv(bL1N, ALIGN_NUM);
        loadDataParams.dstStride = CeilDiv(madN, ALIGN_NUM);
        loadDataParams.ifTranspose = false;
        LoadData(dst, bMatrix, loadDataParams);
    }

    __aicore__ inline void LoadDataToL0(
        const LocalTensor<B_T>& dst, const LocalTensor<B_T>& bMatrix, uint16_t bL1K, uint16_t madN, uint16_t madK,
        uint16_t bL1NOffset, uint16_t bL1KOffset) const
    {
        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(bL1KOffset, BLOCK_CUBE);
        loadDataParams.kStartPosition = CeilDiv(bL1NOffset, c0Size_);
        loadDataParams.kStep = CeilDiv(madN, c0Size_);
        if constexpr (IsSameType<B_T, float>::value) {
            // K step must be multiples of 2 when transpose is enabled ane .type = .b32
            loadDataParams.kStep = CeilAlign(loadDataParams.kStep, K_STEP_MIN_VAL_B32);
        }

        loadDataParams.srcStride = CeilDiv(bL1K, ALIGN_NUM);
        loadDataParams.dstStride = CeilDiv(madN, ALIGN_NUM);
        loadDataParams.ifTranspose = true;
        loadDataParams.mStep = CeilDiv(madK, HW_M0);

        if constexpr (IsSupportB4<B_T>()) {
            // M step must be multiples of 4 when transpose is enabled and .type = .b4
            loadDataParams.mStep = CeilAlign(loadDataParams.mStep, M_STEP_MIN_VAL_B4);
        }

        if constexpr (IsSupportB8<B_T>()) {
            // M step must be multiples of 2 when transpose is enabled and .type = .b8
            uint16_t l0BLoop = CeilAlign(loadDataParams.mStep, M_STEP_MIN_VAL_B8) / M_STEP_MIN_VAL_B8;
            uint64_t dstOffset = 0;
            uint64_t dstAddrStride = CeilAlign(madN, ALIGN_NUM) * ONE_BLK_SIZE;
            loadDataParams.mStep = M_STEP_MIN_VAL_B8;
            uint16_t oriMstartPos = loadDataParams.mStartPosition;
            // K axis is m direction, and M axis is k direction in load2dv2 intrin
            for (uint16_t idx = 0; idx < l0BLoop; ++idx) {
                loadDataParams.mStartPosition = oriMstartPos + M_STEP_MIN_VAL_B8 * idx;
                LoadData(dst[dstOffset], bMatrix, loadDataParams);
                dstOffset += dstAddrStride;
            }
        }
#if __NPU_ARCH__ == 5102
        else if constexpr (IsSupportB4<B_T>()) {
            uint16_t l0BLoop = loadDataParams.mStep / M_STEP_MIN_VAL_B4;
            uint64_t dstOffset = 0;
            uint64_t dstAddrStride = CeilAlign(madN, ALIGN_NUM) * ONE_BLK_SIZE * 2;
            loadDataParams.mStep = M_STEP_MIN_VAL_B4;
            uint16_t oriMstartPos = loadDataParams.mStartPosition;
            // K axis is m direction, and M axis is k direction in load2dv2 intrin
            for (uint16_t idx = 0; idx < l0BLoop; ++idx) {
                loadDataParams.mStartPosition = oriMstartPos + M_STEP_MIN_VAL_B4 * idx;
                LoadData(dst[dstOffset], bMatrix, loadDataParams);
                dstOffset += dstAddrStride;
            }
        }
#endif
        else if constexpr (IsSameType<B_T, float>::value) {
            // in case of mdl && basek=8, the unit of mStartPosition is 16, so don't use it
            loadDataParams.mStartPosition = 0;
            loadDataParams.kStartPosition = 0;
            uint64_t matrixOffset = bL1NOffset * CeilAlign(bL1K, BLOCK_CUBE) + bL1KOffset * B32_C0SIZE;
            LoadData(dst, bMatrix[matrixOffset], loadDataParams);
        } else {
            LoadData(dst, bMatrix, loadDataParams);
        }
    }

    __aicore__ inline void MxTransLoadDataToL0(
        const LocalTensor<L0B_T>& dst, const LocalTensor<B_T>& bMatrix, uint16_t bL1N, uint16_t madN, uint16_t madK,
        uint16_t bL1NOffset, uint16_t bL1KOffset, const LocalTensor<AuxDtype>& l1BAuxMatrix, uint16_t bAuxL1K,
        uint16_t bAuxL1KOffset, uint16_t bAuxL1NOffset) const
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        uint8_t mStep = CeilDiv(madN, HW_M0);
        uint8_t kStep = CeilDiv(madK, c0Size_);

        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(bL1NOffset, BLOCK_CUBE);
        loadDataParams.kStartPosition = CeilDiv(bL1KOffset, c0Size_);
        loadDataParams.mStep = mStep;
        loadDataParams.kStep = kStep;
        loadDataParams.srcStride = CeilDiv(bL1N, HW_M0);
        loadDataParams.dstStride = CeilDiv(madN, HW_M0);

        LoadData2DMxParams loadDataMxParams;
        loadDataMxParams.xStartPosition = CeilDiv(bAuxL1NOffset, BLOCK_CUBE);
        loadDataMxParams.xStep = mStep;
        if constexpr (SupportType<B_T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            loadDataMxParams.yStartPosition = CeilDiv(bAuxL1KOffset, FP4_TWO);
            loadDataMxParams.yStep = kStep;
            loadDataMxParams.srcStride = CeilDiv(bAuxL1K, FP4_TWO);
            loadDataMxParams.dstStride = kStep;
        } else if constexpr (SupportType<B_T, fp8_e5m2_t, fp8_e4m3fn_t>()) {
            // for FP8 ,two K0 on the k axis correspond to a small z fractal.
            uint8_t scaleKStep = CeilDiv(madK, c0Size_ * FP8_TWO);
            loadDataMxParams.yStartPosition = CeilDiv(bAuxL1KOffset, FP8_TWO);
            loadDataMxParams.yStep = scaleKStep;
            loadDataMxParams.srcStride = CeilDiv(bAuxL1K, FP8_TWO);
            loadDataMxParams.dstStride = scaleKStep;
        }
        LoadData(dst, bMatrix, l1BAuxMatrix, loadDataParams, loadDataMxParams);
#endif
    }

    __aicore__ inline void MxLoadDataToL0(
        const LocalTensor<L0B_T>& dst, const LocalTensor<B_T>& bMatrix, uint16_t bL1K, uint16_t madN, uint16_t madK,
        uint16_t bL1NOffset, uint16_t bL1KOffset, const LocalTensor<AuxDtype>& l1BAuxMatrix, uint16_t bAuxL1K,
        uint16_t bAuxL1KOffset, uint16_t bAuxL1NOffset) const
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        uint8_t mStep = CeilDiv(madN, HW_M0);
        uint8_t kStep = CeilDiv(madK, c0Size_);

        LoadData2DParamsV2 loadDataParams;
        loadDataParams.mStartPosition = CeilDiv(bL1KOffset, BLOCK_CUBE);
        loadDataParams.kStartPosition = CeilDiv(bL1NOffset, c0Size_);
        loadDataParams.mStep = CeilDiv(madK, HW_M0);
        loadDataParams.kStep = CeilDiv(madN, c0Size_);
        loadDataParams.srcStride = CeilDiv(bL1K, HW_M0);
        loadDataParams.dstStride = CeilDiv(madN, HW_M0);

        loadDataParams.ifTranspose = true;

        LoadData2DMxParams loadDataMxParams;
        loadDataMxParams.xStartPosition = CeilDiv(bAuxL1NOffset, BLOCK_CUBE);
        loadDataMxParams.xStep = mStep;
        if constexpr (SupportType<B_T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            loadDataMxParams.yStartPosition = CeilDiv(bAuxL1KOffset, FP4_TWO);
            loadDataMxParams.yStep = kStep;
            loadDataMxParams.srcStride = CeilDiv(bAuxL1K, FP4_TWO);
            loadDataMxParams.dstStride = kStep;
        } else if constexpr (SupportType<B_T, fp8_e5m2_t, fp8_e4m3fn_t>()) {
            // for FP8 ,two K0 on the k axis correspond to a small z fractal.
            uint8_t scaleKStep = CeilDiv(madK, c0Size_ * FP8_TWO);
            loadDataMxParams.yStartPosition = CeilDiv(bAuxL1KOffset, FP8_TWO);
            loadDataMxParams.yStep = scaleKStep;
            loadDataMxParams.srcStride = CeilDiv(bAuxL1K, FP8_TWO);
            loadDataMxParams.dstStride = scaleKStep;
        }
        LoadData(dst, bMatrix, l1BAuxMatrix, loadDataParams, loadDataMxParams);
#endif
    }
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_LOAD2DV2_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_LOAD2DV2_H__
#endif // IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD2DV2_H
