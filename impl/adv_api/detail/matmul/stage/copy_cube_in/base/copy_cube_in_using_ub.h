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
 * \file copy_cube_in_using_ub.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_using_ub.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_USING_UB_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_USING_UB_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_USING_UB_H

#include "../../../utils/matmul_module.h"
#include "../../../utils/matmul_param.h"
#include "copy_cube_in_intf.h"
#include "copy_cube_in_base.h"
#include "../copy_tile_to_cube/copy_tile_to_cube.h"

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
        MatmulFeatureTrait<MM_CFG>::IsNeedUB() &&
        ((GetCopyCubeInType<INPUT_TYPE, MM_CFG>() == CopyCubeInType::NORMAL) ||
         (GetCopyCubeInType<INPUT_TYPE, MM_CFG>() == CopyCubeInType::MDL))>>
    : public CopyCubeInBase<IMPL, MM_CFG, INPUT_TYPE> {
    MATMUL_USE_MODULE_ON(CubeInBuffer, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(MatmulTensorInfo, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(DataCopyUtils, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(LocalWorkspace);
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);

    using TRANS_T = typename INPUT_TYPE::TRANS_T;
    using SRC_T = typename INPUT_TYPE::T;

    constexpr static int32_t c0Size_ = AuxGetC0Size<TRANS_T>();

public:
    using BASE_MODULE = AscendC::Impl::Detail::CopyCubeInBase<IMPL, MM_CFG, INPUT_TYPE>;
    inline __aicore__ CopyCubeIn() = default;
    inline __aicore__ ~CopyCubeIn() = default;

    __aicore__ inline void Init()
    {
        MATMUL_MODULE(CubeInBuffer)
            ->Init(MATMUL_MODULE(CopyCubeInParams)->GetBufferSize(), MATMUL_MODULE(CopyCubeInParams)->GetDepth());

        if constexpr (DoMatmulMDL(MM_CFG) && ToMatmulConfig(MM_CFG).enableL1CacheUB) {
            if (MATMUL_MODULE(CopyCubeInParams)->GetDepthL1CacheUB() > 0) {
                GetTPipePtr()->InitBuffer(
                    MATMUL_MODULE(DataCopyUtils)->GetQidUbCache(), 1,
                    MATMUL_MODULE(CopyCubeInParams)->GetDepthL1CacheUB() *
                        MATMUL_MODULE(CopyCubeInParams)->GetStepCol() * MATMUL_MODULE(CopyCubeInParams)->GetStepRow() *
                        MATMUL_MODULE(CopyCubeInParams)->GetBufferSize() * sizeof(SRC_T));
            }
        }
    }

    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TRANS_T> LoadData(
        int32_t curRow, int32_t curCol, int32_t tileHeight, int32_t tileWidth, const ScheduleContext& context = {})
    {
        LocalTensor<TRANS_T> l1;
        if constexpr (DoMatmulMDL(MM_CFG)) {
            auto posL1 = GetIterIndex(curRow, curCol);
            auto bufferPos = MATMUL_MODULE(CopyCubeInParams)->GetBufferPos();
            if (MATMUL_MODULE(CubeInBuffer)->Hit(posL1, bufferPos)) {
                l1 = MATMUL_MODULE(CubeInBuffer)->GetBuffer(posL1, bufferPos);
            } else {
                l1 = MATMUL_MODULE(CubeInBuffer)->AllocTensor(bufferPos);
                if (MATMUL_MODULE(CopyCubeInParams)->IsTranspose()) {
                    MATMUL_MODULE(DataCopyUtils)->CopyTileToCube(l1, curCol, curRow, tileWidth, tileHeight);
                } else {
                    MATMUL_MODULE(DataCopyUtils)->CopyTileToCube(l1, curRow, curCol, tileHeight, tileWidth);
                }
                MATMUL_MODULE(CubeInBuffer)->EnQue(l1);
                MATMUL_MODULE(CubeInBuffer)->DeQue();
            }
        } else {
            auto posL1 = GetIterIndex(curRow, curCol);
            if (MATMUL_MODULE(CubeInBuffer)->Hit(posL1)) {
                l1 = MATMUL_MODULE(CubeInBuffer)->GetBuffer(posL1);
            } else {
                l1 = MATMUL_MODULE(CubeInBuffer)->AllocTensor(posL1);
                if (MATMUL_MODULE(CopyCubeInParams)->IsTranspose()) {
                    MATMUL_MODULE(DataCopyUtils)->CopyTileToCube(l1, curCol, curRow, tileWidth, tileHeight);
                } else {
                    MATMUL_MODULE(DataCopyUtils)->CopyTileToCube(l1, curRow, curCol, tileHeight, tileWidth);
                }
                MATMUL_MODULE(CubeInBuffer)->EnQue(l1);
                MATMUL_MODULE(CubeInBuffer)->DeQue();
            }
        }
        return l1;
    }

    __aicore__ inline void ClearLoadData(
        const LocalTensor<TRANS_T>& aMatrix = LocalTensor<TRANS_T>{}, int32_t curRow = 0, int32_t curCol = 0)
    {
        if constexpr (DoMatmulMDL(MM_CFG)) {
            auto bufferPos = MATMUL_MODULE(CopyCubeInParams)->GetBufferPos();
            MATMUL_MODULE(CubeInBuffer)->FreeTensor(bufferPos);
        } else {
            auto posL1 = GetIterIndex(curRow, curCol);
            MATMUL_MODULE(CubeInBuffer)->FreeTensor(posL1, aMatrix);
        }
    }

private:
    template <typename INPUT_TYPE_ALIAS = INPUT_TYPE>
    __aicore__ enable_if_t<INPUT_TYPE_ALIAS::TAG == InputTypeTag::A, int32_t> GetIterIndexInner(
        int32_t curRow, int32_t curCol)
    {
        if constexpr (DoMatmulNorm(MM_CFG) || DoMatmulIBShareNorm(MM_CFG)) {
            if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                static_cast<int>(IterateOrder::ORDER_M)) {
                return curCol;
            } else {
                return (curRow * MATMUL_MODULE(KLoop)->GetTotalIter() + curCol) %
                       (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() *
                        MATMUL_MODULE(KLoop)->GetTotalIter());
            }
        } else if constexpr (DoMatmulSpecialBasicBlock(MM_CFG)) {
            if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                static_cast<int>(IterateOrder::ORDER_M)) {
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
        if constexpr (DoMatmulNorm(MM_CFG) || DoMatmulIBShareNorm(MM_CFG)) {
            if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                static_cast<int>(IterateOrder::ORDER_M)) {
                return (curRow + curCol * MATMUL_MODULE(KLoop)->GetTotalIter()) %
                       (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() *
                        MATMUL_MODULE(KLoop)->GetTotalIter());
            } else {
                return curRow;
            }
        } else if (DoMatmulSpecialBasicBlock(MM_CFG)) {
            if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                static_cast<int>(IterateOrder::ORDER_M)) {
                return (curRow + curCol * MATMUL_MODULE(KLoop)->GetTotalIter()) % ToMatmulConfig(MM_CFG).stepN *
                       ToMatmulConfig(MM_CFG).singleCoreK / ToMatmulConfig(MM_CFG).basicK;
            } else {
                return curRow;
            }
        } else {
            return 0;
        }
    }

    template <const auto& MM_CFG_ALIAS = MM_CFG>
    __aicore__ enable_if_t<DoMatmulNorm(MM_CFG_ALIAS), int32_t> GetIterIndex(int32_t curRow, int32_t curCol)
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

    template <const auto& MM_CFG_ALIAS = MM_CFG>
    __aicore__ enable_if_t<DoMatmulMDL(MM_CFG_ALIAS), int32_t> GetIterIndex(int32_t curRow, int32_t curCol)
    {
        return BASE_MODULE::GetIterIndex(curRow, curCol);
    }
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_SET_UB_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_USING_UB_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_USING_UB_H__
#endif
