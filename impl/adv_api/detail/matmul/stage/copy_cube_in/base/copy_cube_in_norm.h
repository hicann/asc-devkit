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
 * \file copy_cube_in_norm.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_norm.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_NORM_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_NORM_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_NORM_H

#include "../copy_tile_to_cube/copy_tile_to_cube.h"
#include "copy_cube_in_intf.h"
#include "copy_cube_in_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    CopyCubeIn is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CopyCubeIn is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, const auto& MM_CFG>
class CopyCubeIn<
    IMPL, INPUT_TYPE, MM_CFG,
    enable_if_t<
        !MatmulFeatureTrait<MM_CFG>::IsNeedUB() && GetCopyCubeInType<INPUT_TYPE, MM_CFG>() == CopyCubeInType::NORMAL>>
    : public CopyCubeInBase<IMPL, MM_CFG, INPUT_TYPE> {
    MATMUL_USE_MODULE_ON(CubeInBuffer, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(DataCopyUtils, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(MatmulTensorInfo, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulSubBlockInfo);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(KLoop);
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;

public:
    using BASE_MODULE = AscendC::Impl::Detail::CopyCubeInBase<IMPL, MM_CFG, INPUT_TYPE>;
    __aicore__ inline CopyCubeIn() = default;
    __aicore__ inline ~CopyCubeIn() = default;

    __aicore__ inline void SetInput(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose)
    {
        if constexpr (ToMatmulConfig(MM_CFG).intraBlockPartSum) {
            if (MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() == 0) {
                MATMUL_MODULE(MatmulTensorInfo)->template SetGlobalTensor<false>(globalMatrix, isTranspose);
            } else {
                MATMUL_MODULE(MatmulTensorInfo)->template SetGlobalTensor<true>(globalMatrix, isTranspose);
            }
        } else {
            MATMUL_MODULE(MatmulTensorInfo)->template SetGlobalTensor<false>(globalMatrix, isTranspose);
        }
        MATMUL_MODULE(CubeInBuffer)->Reset();
        if constexpr (IsSameABTemplate<INPUT_TYPE, MM_CFG>()) {
            MATMUL_MODULE(CubeInBuffer)->SetOrgTensor(MATMUL_MODULE(MatmulTensorInfo)->GetGlobalTensor());
        }
    }

    __aicore__ inline void SetInput(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        BASE_MODULE::SetInput(localMatrix, isTranspose);
    }

    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> LoadData(
        int32_t curRow, int32_t curCol, int32_t tileHeight, int32_t tileWidth, const ScheduleContext& context = {})
    {
        LocalTensor<TransT> l1;
        auto posL1 = GetIterIndex(curRow, curCol);
        if (MATMUL_MODULE(CubeInBuffer)->Hit(posL1)) {
            l1 = MATMUL_MODULE(CubeInBuffer)->GetBuffer(posL1);
        } else {
            l1 = MATMUL_MODULE(CubeInBuffer)->AllocTensor(posL1);
            if constexpr (ToMatmulConfig(MM_CFG).intraBlockPartSum) {
                if (MATMUL_MODULE(MatmulSubBlockInfo)->IsFakeIntraBlock()) {
                    MATMUL_MODULE(DataCopyUtils)
                        ->template CopyTileToCube<false>(l1, curRow, curCol, tileHeight, tileWidth);
                } else {
                    MATMUL_MODULE(DataCopyUtils)
                        ->template CopyTileToCube<true>(l1, curRow, curCol, tileHeight, tileWidth);
                }
            } else {
                MATMUL_MODULE(DataCopyUtils)->template CopyTileToCube<false>(l1, curRow, curCol, tileHeight, tileWidth);
            }
            MATMUL_MODULE(CubeInBuffer)->EnQue(l1);
            MATMUL_MODULE(CubeInBuffer)->DeQue();
        }
        return l1;
    }

    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> AsyncLoadData(
        int32_t curRow, int32_t curCol, int32_t tileHeight, int32_t tileWidth, const ScheduleContext& context = {})
    {
        LocalTensor<TransT> l1;
        auto posL1 = GetIterIndex(curRow, curCol);
        if (MATMUL_MODULE(CubeInBuffer)->Hit(posL1)) {
            return MATMUL_MODULE(CubeInBuffer)->GetBuffer(posL1);
        } else {
            l1 = MATMUL_MODULE(CubeInBuffer)->AllocTensor(posL1);
            MATMUL_MODULE(DataCopyUtils)->CopyTileToCube(l1, curRow, curCol, tileHeight, tileWidth);
            MATMUL_MODULE(CubeInBuffer)->EnQue(l1);
            return l1;
        }
    }

    __aicore__ inline void AwaitLoadData() { MATMUL_MODULE(CubeInBuffer)->DeQue(); }

    __aicore__ inline void ClearLoadData(
        const LocalTensor<TransT>& tensor = LocalTensor<TransT>{}, int32_t curRow = 0, int32_t curCol = 0)
    {
        auto posL1 = GetIterIndex(curRow, curCol);
        MATMUL_MODULE(CubeInBuffer)->FreeTensor(posL1, tensor);
    }

private:
    __aicore__ int32_t GetIterIndex(int32_t curRow, int32_t curCol)
    {
        if constexpr (GetCubeInBufferType<INPUT_TYPE, MM_CFG>() == CubeInBufferType::SINGLE_BUFFER) {
            return 0;
        } else if constexpr (
            GetCubeInBufferType<INPUT_TYPE, MM_CFG>() == CubeInBufferType::NORMAL ||
            GetCubeInBufferType<INPUT_TYPE, MM_CFG>() == CubeInBufferType::SINGLE_GLOBAL_BUFFER ||
            GetCubeInBufferType<INPUT_TYPE, MM_CFG>() == CubeInBufferType::DOUBLE_GLOBAL_BUFFER) {
            return GetIterIndexInner(curRow, curCol);
        }
    }

    template <typename INPUT_TYPE_ALIAS = INPUT_TYPE>
    __aicore__ enable_if_t<INPUT_TYPE_ALIAS::TAG == InputTypeTag::A, int32_t> GetIterIndexInner(
        int32_t curRow, int32_t curCol)
    {
        if constexpr (DoMatmulNorm(MM_CFG) || DoMatmulIBShareNorm(MM_CFG) || DoMatmulBasicBlock(MM_CFG)) {
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::UNDEF) {
                if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                    static_cast<int>(IterateOrder::ORDER_M)) {
                    return curCol;
                } else {
                    if constexpr (
                        INPUT_TYPE::layout == LayoutMode::NONE &&
                        ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
                        return (curRow +
                                (curRow / DOUBLE_QUE) * DOUBLE_QUE * (MATMUL_MODULE(KLoop)->GetTotalIter() - 1) +
                                curCol * DOUBLE_QUE) %
                               (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() *
                                MATMUL_MODULE(KLoop)->GetTotalIter());
                    }
                    return (curRow * MATMUL_MODULE(KLoop)->GetTotalIter() + curCol) %
                           (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() *
                            MATMUL_MODULE(KLoop)->GetTotalIter());
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                return curCol;
            } else {
                if constexpr (
                    INPUT_TYPE::layout == LayoutMode::NONE &&
                    ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
                    return (curRow + (curRow / DOUBLE_QUE) * DOUBLE_QUE * (MATMUL_MODULE(KLoop)->GetTotalIter() - 1) +
                            curCol * DOUBLE_QUE) %
                           (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() *
                            MATMUL_MODULE(KLoop)->GetTotalIter());
                }
                return (curRow * MATMUL_MODULE(KLoop)->GetTotalIter() + curCol) %
                       (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() *
                        MATMUL_MODULE(KLoop)->GetTotalIter());
            }
        } else if constexpr (DoMatmulSpecialBasicBlock(MM_CFG)) {
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::UNDEF) {
                if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                    static_cast<int>(IterateOrder::ORDER_M)) {
                    return curCol;
                } else {
                    return (curRow * MATMUL_MODULE(KLoop)->GetTotalIter() + curCol) %
                           (ToMatmulConfig(MM_CFG).stepM * ToMatmulConfig(MM_CFG).singleCoreK /
                            ToMatmulConfig(MM_CFG).basicK);
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                return curCol;
            } else {
                return (curRow * MATMUL_MODULE(KLoop)->GetTotalIter() + curCol) %
                       (ToMatmulConfig(MM_CFG).stepM * ToMatmulConfig(MM_CFG).singleCoreK /
                        ToMatmulConfig(MM_CFG).basicK);
            }
        } else {
            return 0;
        }
    }

    template <typename INPUT_TYPE_ALIAS = INPUT_TYPE>
    __aicore__ enable_if_t<INPUT_TYPE_ALIAS::TAG == InputTypeTag::B, int32_t> GetIterIndexInner(
        int32_t curRow, int32_t curCol)
    {
        if constexpr (DoMatmulNorm(MM_CFG) || DoMatmulIBShareNorm(MM_CFG) || DoMatmulBasicBlock(MM_CFG)) {
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::UNDEF) {
                return GetNormIterBIdxForUndef(curRow, curCol);
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                if constexpr (
                    INPUT_TYPE::layout == LayoutMode::NONE &&
                    ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
                    return (curCol + (curCol / DOUBLE_QUE) * DOUBLE_QUE * (MATMUL_MODULE(KLoop)->GetTotalIter() - 1) +
                            curRow * DOUBLE_QUE) %
                           (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() *
                            MATMUL_MODULE(KLoop)->GetTotalIter());
                }
                return (curRow + curCol * MATMUL_MODULE(KLoop)->GetTotalIter()) %
                       (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() *
                        MATMUL_MODULE(KLoop)->GetTotalIter());
            } else {
                return curRow;
            }
        } else if (DoMatmulSpecialBasicBlock(MM_CFG)) {
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::UNDEF) {
                if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                    static_cast<int>(IterateOrder::ORDER_M)) {
                    return (curRow + curCol * MATMUL_MODULE(KLoop)->GetTotalIter()) % ToMatmulConfig(MM_CFG).stepN *
                           ToMatmulConfig(MM_CFG).singleCoreK / ToMatmulConfig(MM_CFG).basicK;
                } else {
                    return curRow;
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                return (curRow + curCol * MATMUL_MODULE(KLoop)->GetTotalIter()) % ToMatmulConfig(MM_CFG).stepN *
                       ToMatmulConfig(MM_CFG).singleCoreK / ToMatmulConfig(MM_CFG).basicK;
            } else {
                return curRow;
            }
        } else {
            return 0;
        }
    }

    __aicore__ int32_t GetNormIterBIdxForUndef(int32_t curRow, int32_t curCol)
    {
        if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
            static_cast<int>(IterateOrder::ORDER_M)) {
            if constexpr (
                INPUT_TYPE::layout == LayoutMode::NONE &&
                ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
                return (curCol + (curCol / DOUBLE_QUE) * DOUBLE_QUE * (MATMUL_MODULE(KLoop)->GetTotalIter() - 1) +
                        curRow * DOUBLE_QUE) %
                       (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() *
                        MATMUL_MODULE(KLoop)->GetTotalIter());
            }
            return (curRow + curCol * MATMUL_MODULE(KLoop)->GetTotalIter()) %
                   (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() * MATMUL_MODULE(KLoop)->GetTotalIter());
        } else {
            if constexpr (DoMatmulIBShareNorm(MM_CFG) && MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2()) {
                return (curRow + curCol * MATMUL_MODULE(KLoop)->GetTotalIter()) %
                       (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() *
                        MATMUL_MODULE(KLoop)->GetTotalIter());
            }
            return curRow;
        }
    }
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _COPY_CUBE_IN_NORM_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_NORM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_NORM_H__
#endif
