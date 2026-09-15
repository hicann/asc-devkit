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
 * \file copy_cube_in_ubtol1_singleshape.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_ubtol1_singleshape.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_UBTOL1_SINGLESHAPE_H__
#endif

#ifndef IMPL_MATMUL_MODULES_STAGE_COPY_CUBE_IN_UBTOL1_SINGLESHAPE_H
#define IMPL_MATMUL_MODULES_STAGE_COPY_CUBE_IN_UBTOL1_SINGLESHAPE_H

#include "../copy_tile_to_cube/copy_tile_to_cube.h"
#include "copy_cube_in_intf.h"

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
    enable_if_t<GetCopyCubeInType<INPUT_TYPE, MM_CFG>() == CopyCubeInType::UBTOL1_SINGLESHAPE>> {
    MATMUL_USE_MODULE_ON(CubeInBuffer, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(MatmulTensorInfo, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename Conditional<IsSameType<TransT, fp8_e8m0_t>::value, fp8_e8m0_t, typename INPUT_TYPE::T>::type;

public:
    __aicore__ inline CopyCubeIn() = default;
    __aicore__ inline ~CopyCubeIn() = default;

    __aicore__ inline void Init()
    {
        MATMUL_MODULE(CubeInBuffer)
            ->Init(MATMUL_MODULE(CopyCubeInParams)->GetBufferSize(), MATMUL_MODULE(CopyCubeInParams)->GetDepth());
    }

    __aicore__ inline void Reset() {}

    __aicore__ inline void SetInput(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        MATMUL_MODULE(MatmulTensorInfo)->SetLocalTensor(localMatrix, isTranspose);
    }

    __aicore__ inline void SetInput(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose)
    {
        MATMUL_MODULE(MatmulTensorInfo)->SetGlobalTensor(globalMatrix, isTranspose);
    }

    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> LoadData(
        int32_t curRow, int32_t curCol, int32_t tileHeight, int32_t tileWidth, const ScheduleContext& context = {})
    {
        LocalTensor<TransT> l1;

        TBuffAddr tbuffTmp;
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            tbuffTmp.logicPos = (uint8_t)(TPosition::A1);
        } else {
            tbuffTmp.logicPos = (uint8_t)(TPosition::B1);
        }
        tbuffTmp.bufferAddr = MATMUL_MODULE(CubeInBuffer)->GetBufferHeadAddr();

#ifdef ASCENDC_CPU_DEBUG
        int32_t orgWidth;
        int32_t orgHeight;
        constexpr static int32_t c0Size_ = AuxGetC0Size<TransT>();
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            if constexpr (HasScalePosition<INPUT_TYPE>::value) {
                if constexpr (INPUT_TYPE::isTrans) {
                    orgHeight =
                        Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR) * MX_BASEK_FACTOR;
                    orgWidth = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM(), c0Size_) * c0Size_;
                } else {
                    orgHeight = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM(), BLOCK_CUBE) * BLOCK_CUBE;
                    orgWidth =
                        Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR) * MX_BASEK_FACTOR;
                }
            } else {
                orgWidth = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
                orgHeight = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM();
            }
        } else {
            if constexpr (HasScalePosition<INPUT_TYPE>::value) {
                if constexpr (INPUT_TYPE::isTrans) {
                    orgHeight = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN(), BLOCK_CUBE) * BLOCK_CUBE;
                    orgWidth =
                        Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR) * MX_BASEK_FACTOR;
                } else {
                    orgHeight =
                        Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR) * MX_BASEK_FACTOR;
                    orgWidth = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN(), c0Size_) * c0Size_;
                }
            } else {
                orgWidth = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
                orgHeight = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            }
        }
        tbuffTmp.dataLen = orgHeight * orgWidth * sizeof(TransT);
        tbuffTmp.absAddr = GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::A1)) + tbuffTmp.bufferAddr;
#endif

        l1.SetAddr(tbuffTmp);
        return l1;
    }

    __aicore__ inline void AllocTensor(int32_t iterIndex = 0) {}

    __aicore__ inline void ClearLoadData(
        const LocalTensor<TransT>& tensor = LocalTensor<TransT>{}, int32_t curRow = 0, int32_t curCol = 0)
    {}

    __aicore__ inline void Destroy() {}
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_MODULES_STAGE_COPY_CUBE_IN_UBTOL1_SINGLESHAPE_H

#if defined( \
    __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_UBTOL1_SINGLESHAPE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_UBTOL1_SINGLESHAPE_H__
#endif
