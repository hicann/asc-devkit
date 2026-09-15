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
 * \file load_to_l0b_load3dv2.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/split/load_to_l0b/load_to_l0b_loadInstr.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_LOADINSTR_H__
#endif

#ifndef IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOADINSTR_H
#define IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOADINSTR_H

#include "load_to_l0b_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {
constexpr uint8_t SPARSE_NUM_RATE = 2;
template <typename IMPL, class INPUT_TYPE, const auto& MM_CFG>
class LoadToL0B<
    IMPL, INPUT_TYPE, MM_CFG,
    enable_if_t<
        !(DoMatmulBasicBlock(MM_CFG) || DoMatmulSpecialBasicBlock(MM_CFG)) &&
        (MatmulFeatureTrait<MM_CFG>::IsSupportLoad3dV2()) && !(MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2())>> {
    using B_T = typename INPUT_TYPE::T;
    using TransT = typename INPUT_TYPE::TRANS_T;
    using AuxDtype = decltype(GetAuxDataType<INPUT_TYPE>());

public:
    __aicore__ inline LoadToL0B(){};
    __aicore__ inline ~LoadToL0B(){};

    __aicore__ inline void Prepare(bool isBTranspose, uint16_t bL1K) const
    {
        if constexpr (!isFmatrixUpdate_) {
            SetFmatrix(isBTranspose, bL1K);
        }
    }

    __aicore__ inline void Load(
        const LocalTensor<TransT>& l0B, const LocalTensor<TransT>& l1B, uint16_t bL1N, uint16_t bL1K, uint16_t madN,
        uint16_t madK, uint16_t bL1NOffset, uint16_t bL1KOffset, bool isBTranspose,
        const LocalTensor<AuxDtype>& l1BAuxMatrix = {}, uint16_t bAuxL1K = 0, uint16_t bAuxL1KOffset = 0) const
    {
        if constexpr (
            MatmulFeatureTrait<MM_CFG>::IsNeedUB() && IsSameType<TransT, int8_t>::value &&
            IsSameType<B_T, int8_t>::value) {
            isBTranspose = true;
        }
        if (isBTranspose) {
            TransLoadDataToL0(l0B, l1B, bL1N, madN, madK, bL1NOffset, bL1KOffset, l1BAuxMatrix);
        } else {
            LoadDataToL0(l0B, l1B, bL1N, bL1K, madN, madK, bL1NOffset, bL1KOffset, isBTranspose);
        }
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<TransT>();
    static constexpr bool isFmatrixUpdate_ = !MatmulFeatureTrait<MM_CFG>::IsSupportFmatrixB();

    __aicore__ inline void SetFmatrix(bool isBTranspose, uint16_t bL1K) const
    {
        if constexpr (
            MatmulFeatureTrait<MM_CFG>::IsNeedUB() && IsSameType<TransT, int8_t>::value &&
            IsSameType<B_T, int8_t>::value) {
            return; // transpose is true, MTE1 is load2d
        }
        if (!isBTranspose) {
            uint16_t wAlign = CeilAlign(bL1K, HW_M0);
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportFmatrixB()) {
                Load3DSetFMatrixBCal(1, wAlign, padList);
            } else {
                Load3DSetFMatrixCal(1, wAlign, padList);
            }
        }
    }

    __aicore__ inline void TransLoadDataToL0(
        const LocalTensor<TransT>& l0B, const LocalTensor<TransT>& l1B, uint16_t bL1N, uint16_t madN, uint16_t madK,
        uint16_t bL1NOffset, uint16_t bL1KOffset, const LocalTensor<AuxDtype>& l1BAuxMatrix = {}) const
    {
        // SET LOAD2D parameters , loop axis: K or M, or 1
        if constexpr (HasSparseIndex<INPUT_TYPE>()) {
            madK = Ceil(madK, SPARSE_NUM_RATE);
        }
        // k is c0Size_ aligned for f32
        uint16_t kC0 = Ceil(madK, c0Size_);
        uint16_t nFraC0 = Ceil(madN, HW_N0);
        uint16_t l0bLoop = 1;
        uint64_t l0bSrcAddrStride = 0;
        uint64_t l0bDstAddrStride = 0;
        uint8_t l0bRepeat = kC0 * nFraC0;
        uint16_t l0bSrcstride = 1;
        uint16_t l0bDststride = 0;

        if (nFraC0 * HW_N0 == bL1N) { // loop=1
            l0bLoop = 1;
        } else if (nFraC0 >= kC0) { // LOOP is K and repeat is n axis
            l0bLoop = kC0;
            l0bSrcAddrStride = bL1N * c0Size_;
            l0bDstAddrStride = nFraC0 * HW_N0 * c0Size_;
            l0bRepeat = nFraC0;

            l0bSrcstride = 1;
            l0bDststride = 0;
        } else { // LOOP is N  and repeat is K axis
            l0bLoop = nFraC0;
            l0bSrcAddrStride = HW_N0 * c0Size_;
            l0bDstAddrStride = HW_N0 * c0Size_;
            l0bRepeat = kC0;

            l0bSrcstride = (bL1N + HW_N0 - 1) / HW_N0;
            l0bDststride = nFraC0 - 1;
        }
        // use load2d for L1_2_L0B
        // startIndex, repeatTimes, srcStride, sid, dstGap, ifTranspose, addrmode
        LoadData2dParams loadDataParams{0, l0bRepeat, l0bSrcstride, 0, l0bDststride, 0, 0};
        if constexpr (HasSparseIndex<INPUT_TYPE>()) {
            bL1KOffset = bL1KOffset >> 1;
        }
        uint64_t l1bOffset = bL1NOffset * c0Size_ + bL1KOffset * bL1N;
        uint64_t l0bOffset = 0;
        for (uint64_t i = 0; i < l0bLoop; i++) {
            if constexpr (HasSparseIndex<INPUT_TYPE>()) {
                LoadDataWithSparse(
                    l0B[l0bOffset], l1B[l1bOffset], l1BAuxMatrix[l1bOffset >> INDEX_SHIFT], loadDataParams);
            } else {
                LoadData(l0B[l0bOffset], l1B[l1bOffset], loadDataParams);
            }
            l1bOffset += (l0bSrcAddrStride);
            l0bOffset += (l0bDstAddrStride);
        }
    }

    __aicore__ inline void LoadDataToL0(
        const LocalTensor<TransT>& l0B, const LocalTensor<TransT>& l1B, uint16_t bL1N, uint16_t bL1K, uint16_t madN,
        uint16_t madK, uint16_t bL1NOffset, uint16_t bL1KOffset, bool isBTranspose) const
    {
        if constexpr (GetLoadL0bInstrType<TransT, MM_CFG>() == LoadL0bInstrType::LOAD2DTRANSPOSE) {
            // use load2d transpose for L1_2_L0B
            uint16_t l0bloop = Ceil(madK, c0Size_);
            uint16_t l0bSrcstride = Ceil(bL1K, c0Size_);
            uint16_t l0bRepeat = Ceil(madN, c0Size_);
            uint64_t l0bSrcAddrStride = c0Size_ * c0Size_;
            uint64_t l0bDstAddrStride = Ceil(madN, ALIGN_NUM) * ALIGN_NUM * c0Size_;
            uint64_t l1bOffset = bL1NOffset * bL1K + bL1KOffset * c0Size_;
            uint64_t l0bOffset = 0;

            // startIndex, repeatTimes, srcStride, dstGap, dstFracGap, addrMode
            LoadData2dTransposeParams loadData2dTransposeParams{0,  static_cast<uint8_t>(l0bRepeat), l0bSrcstride, 1, 0,
                                                                inc};

            if constexpr (IsSameType<TransT, int4b_t>::value) {
                loadData2dTransposeParams.dstGap = Ceil(c0Size_, ALIGN_NUM) - 1;
            }

            for (uint64_t i = 0; i < l0bloop; i++) {
                LoadDataWithTranspose(l0B[l0bOffset], l1B[l1bOffset], loadData2dTransposeParams);
                l1bOffset += l0bSrcAddrStride;
                l0bOffset += l0bDstAddrStride;
            }
        } else {
            if constexpr (isFmatrixUpdate_) {
                SetFmatrix(isBTranspose, bL1K);
            }
            // use load3dv2 for L1_2_L0B
            // n_axis is K direction, need to be 16 aligned
            uint16_t kAlign = CeilAlign(madN, ALIGN_NUM);
            // channel size need to be 16 aligned
            uint16_t cAlign = CeilAlign(bL1N, ALIGN_NUM);
            // k_axis is M direction, need to be HW_M0 aligned
            uint16_t mAlign = CeilAlign(madK, HW_M0);
            // StepN need to be aligned
            LoadData3DParamsV2Pro loadData3DV2;
            loadData3DV2.channelSize = cAlign;
            loadData3DV2.extConfig = ((uint64_t)bL1KOffset << M_POS_BIT) | ((uint64_t)bL1NOffset << K_POS_BIT) |
                                     ((uint64_t)mAlign << M_STEP_BIT) | (uint64_t)kAlign;
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportFmatrixB()) {
                loadData3DV2.fMatrixCtrl = true;
            }
            LoadData<TransT>(l0B[0], l1B[0], loadData3DV2);
        }
    }
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_LOADINSTR_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_LOADINSTR_H__
#endif // IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOADINSTR_H
