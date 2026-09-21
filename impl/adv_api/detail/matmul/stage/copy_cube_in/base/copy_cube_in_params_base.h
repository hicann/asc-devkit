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
 * \file copy_cube_in_params_base.h
 * \brief copy cube in variable manager module
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_params_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_BASE_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_BASE_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_BASE_H

#include "../../../param/matmul_shape_tiling.h"

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
class CopyCubeInParams<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<INPUT_TYPE::TAG == InputTypeTag::A>> {
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(KLoop);
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;

public:
    __aicore__ inline int32_t GetDepthL1CacheUB()
    {
        return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetDepthAL1CacheUB();
    }

    __aicore__ constexpr int32_t GetCopyHeight(int32_t i)
    {
        return MATMUL_MODULE(MLoop)->GetTileShapeOf(MATMUL_MODULE(MLoop)->GetOuterIdx() + i);
    }

    template <bool IS_TRANS = false>
    __aicore__ constexpr int32_t GetCopyWidth(int32_t i, int32_t baseWidth)
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MLoop)->GetTileShapeOf(MATMUL_MODULE(MLoop)->GetOuterIdx() + i);
        } else {
            return MATMUL_MODULE(KLoop)->GetTileShapeAOf(MATMUL_MODULE(KLoop)->GetOuterKaIdx() + i);
        }
    }

    __aicore__ constexpr bool IsBufferPosEnd(int32_t i)
    {
        return MATMUL_MODULE(MLoop)->GetOuterIdx() + i >= MATMUL_MODULE(MLoop)->GetOuterIter();
    }

    __aicore__ constexpr bool IsBufferPosEnd()
    {
        return MATMUL_MODULE(MLoop)->GetOuterIdx() == MATMUL_MODULE(MLoop)->GetOuterIter() - 1;
    }

    __aicore__ inline bool IsBufferKPosEnd(int32_t i)
    {
        auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepKaIter =
            Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), tiling.GetBaseK() * tiling.GetStepKa());
        return MATMUL_MODULE(KLoop)->GetOuterKaIdx() + i >= stepKaIter;
    }

    __aicore__ inline bool IsBufferKPosEnd()
    {
        auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepKaIter =
            Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), tiling.GetBaseK() * tiling.GetStepKa());
        return MATMUL_MODULE(KLoop)->GetOuterKaIdx() == stepKaIter - 1;
    }

    __aicore__ constexpr bool IsL1KFullLoad() const { return MATMUL_CONST_PARAM_VAR.isA1KFullLoad_; }

    __aicore__ inline bool IsTranspose() { return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA(); }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetStepCol() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
        }
    }

    __aicore__ inline int32_t GetBufferPos()
    {
        if constexpr (DoMatmulMDL(MM_CFG) || DoMatmulSpecialMDL(MM_CFG)) {
            return MATMUL_MODULE(KLoop)->IsAKL1FullLoad() ? MATMUL_MODULE(MLoop)->GetOuterIdx() :
                                                            MATMUL_MODULE(KLoop)->GetOuterKaIdx();
        } else {
            return 0;
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetStepRow() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
        }
    }

    __aicore__ inline int32_t GetBufferSize()
    {
        if constexpr (PhyPosIsUB(INPUT_TYPE::pos) && MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
            return GetOrgSizeAlign();
        } else {
            return GetBaseSize();
        }
    }

    __aicore__ inline int GetDepth() const { return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetDepthA1(); }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
    __aicore__ inline int32_t GetOrgHeight()
    {
        if constexpr (PhyPosIsUB(INPUT_TYPE::pos)) {
            if constexpr (IS_INTRA_BLOCK) {
                return CeilAlign<int32_t>(GetSingleHeight<IS_TRANS, true>(), (int32_t)BLOCK_CUBE);
            } else {
                return GetSingleHeight<IS_TRANS, false>();
            }
        } else {
            if constexpr (IS_TRANS) {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKa<IS_INTRA_BLOCK>();
            } else {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<IS_INTRA_BLOCK>();
            }
        }
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
    __aicore__ inline int32_t GetOrgWidth()
    {
        if constexpr (PhyPosIsUB(INPUT_TYPE::pos)) {
            if constexpr (IS_INTRA_BLOCK) {
                return CeilAlign<int32_t>(GetSingleWidth<IS_TRANS, true>(), c0Size_);
            } else {
                return GetSingleWidth<IS_TRANS, false>();
            }
        } else {
            if constexpr (IS_TRANS) {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<IS_INTRA_BLOCK>();
            } else {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKa<IS_INTRA_BLOCK>();
            }
        }
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
    __aicore__ inline int32_t GetSingleHeight() const
    {
        // Constantized scenario
        // You can set IS_BASIC to false, if you don't need to use constantized parameters
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>();
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreM<IS_INTRA_BLOCK, IS_BASIC>();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBaseHeight() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        }
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
    __aicore__ inline int32_t GetSingleWidth() const
    {
        // Constantized scenario
        // You can set IS_BASIC to false, if you don't need to use constantized parameters
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreM<IS_INTRA_BLOCK, IS_BASIC>();
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBaseWidth() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetTotalRow()
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(KLoop)->GetTotalIter();
        } else {
            return MATMUL_MODULE(MLoop)->GetTotalIter();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetTotalCol()
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MLoop)->GetTotalIter();
        } else {
            return MATMUL_MODULE(KLoop)->GetTotalIter();
        }
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline bool IsKRowDirec() const
    {
        return MATMUL_MODULE(MatmulShapeInfo)->template IsTransposeA<IS_INTRA_BLOCK>();
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<TransT>();

    __aicore__ inline int32_t GetBaseHeightAlign() const
    {
        if constexpr (IsSameTypeV<SrcT, float>) {
            return Align(GetBaseHeight(), (int32_t)BLOCK_CUBE);
        } else if constexpr (IsNeedC0Align<SrcT>() && INPUT_TYPE::isTrans) {
            return Align(GetBaseHeight(), c0Size_);
        } else {
            return GetBaseHeight();
        }
    }

    __aicore__ inline int32_t GetBaseWidthAlign() const
    {
        if constexpr (IsSameTypeV<SrcT, float> && INPUT_TYPE::isTrans) {
            return Align(GetBaseWidth(), (int32_t)BLOCK_CUBE);
        } else if constexpr (IsTypeOneOfV<SrcT, float> || IsNeedC0Align<SrcT>()) {
            return Align(GetBaseWidth(), c0Size_);
        } else {
            return GetBaseWidth();
        }
    }

    __aicore__ inline int32_t GetOrgSizeAlign()
    {
        int32_t orgHeightAlign = (IsNeedC0Align<SrcT>() && INPUT_TYPE::isTrans) ?
                                     Align<int32_t>(GetOrgHeight(), c0Size_) :
                                     Align<int32_t>(GetOrgHeight(), (int32_t)BLOCK_CUBE);
        int32_t orgWidthAlign = (IsTypeOneOfV<SrcT, float> || IsNeedC0Align<SrcT>()) ?
                                    Align<int32_t>(GetOrgWidth(), c0Size_) :
                                    Align<int32_t>(GetOrgWidth(), (int32_t)BLOCK_CUBE);
        if constexpr (HasScalePosition<INPUT_TYPE>::value) {
            if constexpr (INPUT_TYPE::isTrans) {
                orgHeightAlign = CeilAlign(GetOrgHeight<INPUT_TYPE::isTrans>(), MX_BASEK_FACTOR);
                const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
                orgWidthAlign = CeilAlign(GetOrgWidth<INPUT_TYPE::isTrans>(), c0Size_);
            } else {
                orgHeightAlign = CeilAlign(GetOrgHeight<INPUT_TYPE::isTrans>(), (int32_t)BLOCK_CUBE);
                orgWidthAlign = CeilAlign(GetOrgWidth<INPUT_TYPE::isTrans>(), MX_BASEK_FACTOR);
            }
        }
        return orgHeightAlign * orgWidthAlign;
    }

    __aicore__ inline int32_t GetBaseSize()
    {
        if constexpr (
            INPUT_TYPE::format == CubeFormat::VECTOR && !(IsTypeOneOfV<SrcT, float> || IsNeedC0Align<SrcT>())) {
#ifdef ASCENDC_CPU_DEBUG
            // cpu mode need L1 buffer aligned to 512B
            return GetBaseHeightAlign() * GetBaseWidthAlign();
#else
            return GetBaseWidth();
#endif
        } else {
            return GetBaseHeightAlign() * GetBaseWidthAlign();
        }
    }
};

template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
class CopyCubeInParams<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<INPUT_TYPE::TAG == InputTypeTag::B>> {
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;

public:
    __aicore__ inline int32_t GetDepthL1CacheUB()
    {
        return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetDepthBL1CacheUB();
    }

    __aicore__ constexpr int32_t GetCopyHeight(int32_t i)
    {
        return MATMUL_MODULE(NLoop)->GetTileShapeOf(MATMUL_MODULE(NLoop)->GetOuterIdx() + i);
    }

    template <bool IS_TRANS = false>
    __aicore__ constexpr int32_t GetCopyWidth(int32_t i, int32_t baseWidth)
    {
        return MATMUL_MODULE(NLoop)->GetTileShapeOf(MATMUL_MODULE(NLoop)->GetOuterIdx() + i);
    }

    __aicore__ constexpr bool IsBufferPosEnd(int32_t i)
    {
        return MATMUL_MODULE(NLoop)->GetOuterIdx() + i >= MATMUL_MODULE(NLoop)->GetOuterIter();
    }

    __aicore__ constexpr bool IsBufferPosEnd()
    {
        return MATMUL_MODULE(NLoop)->GetOuterIdx() == MATMUL_MODULE(NLoop)->GetOuterIter() - 1;
    }

    __aicore__ inline bool IsBufferKPosEnd(int32_t i)
    {
        auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepKbIter =
            Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), tiling.GetBaseK() * tiling.GetStepKb());
        return MATMUL_MODULE(KLoop)->GetOuterKbIdx() + i >= stepKbIter;
    }

    __aicore__ inline bool IsBufferKPosEnd()
    {
        auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepKbIter =
            Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), tiling.GetBaseK() * tiling.GetStepKb());
        return MATMUL_MODULE(KLoop)->GetOuterKaIdx() == stepKbIter - 1;
    }

    __aicore__ constexpr bool IsL1KFullLoad() const { return MATMUL_CONST_PARAM_VAR.isB1KFullLoad_; }

    __aicore__ inline bool IsTranspose() { return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB(); }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetStepCol() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetStepRow() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
        }
    }

    __aicore__ inline int32_t GetBufferPos()
    {
        if constexpr (DoMatmulMDL(MM_CFG) || DoMatmulSpecialMDL(MM_CFG)) {
            return MATMUL_MODULE(KLoop)->IsBKL1FullLoad() ? MATMUL_MODULE(NLoop)->GetOuterIdx() :
                                                            MATMUL_MODULE(KLoop)->GetOuterKbIdx();
        } else {
            return 0;
        }
    }

    __aicore__ inline int32_t GetBufferSize()
    {
        if constexpr (PhyPosIsUB(INPUT_TYPE::pos) && MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
            return GetOrgSizeAlign();
        } else {
            return GetBaseSize();
        }
    }

    __aicore__ inline int GetDepth() const { return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetDepthB1(); }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
    __aicore__ inline int32_t GetOrgHeight()
    {
        if constexpr (PhyPosIsUB(INPUT_TYPE::pos)) {
            if constexpr (IS_INTRA_BLOCK) {
                return CeilAlign<int32_t>(GetSingleHeight<IS_TRANS, true>(), (int32_t)BLOCK_CUBE);
            } else {
                return GetSingleHeight<IS_TRANS, false>();
            }
        } else {
            if constexpr (IS_TRANS) {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<IS_INTRA_BLOCK>();
            } else {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKb<IS_INTRA_BLOCK>();
            }
        }
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
    __aicore__ inline int32_t GetOrgWidth()
    {
        if constexpr (PhyPosIsUB(INPUT_TYPE::pos)) {
            if constexpr (IS_INTRA_BLOCK) {
                return CeilAlign<int32_t>(GetSingleWidth<IS_TRANS, true>(), c0Size_);
            } else {
                return GetSingleWidth<IS_TRANS, false>();
            }
        } else {
            if constexpr (IS_TRANS) {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKb<IS_INTRA_BLOCK>();
            } else {
                return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<IS_INTRA_BLOCK>();
            }
        }
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
    __aicore__ inline int32_t GetSingleHeight() const
    {
        // Constantized scenario
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreN<IS_INTRA_BLOCK, IS_BASIC>();
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBaseHeight() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        }
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
    __aicore__ inline int32_t GetSingleWidth() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>();
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreN<IS_INTRA_BLOCK, IS_BASIC>();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBaseWidth() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetTotalRow()
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(NLoop)->GetTotalIter();
        } else {
            return MATMUL_MODULE(KLoop)->GetTotalIter();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetTotalCol()
    {
        if constexpr (IS_TRANS) {
            return MATMUL_MODULE(KLoop)->GetTotalIter();
        } else {
            return MATMUL_MODULE(NLoop)->GetTotalIter();
        }
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline bool IsKRowDirec() const
    {
        return !MATMUL_MODULE(MatmulShapeInfo)->template IsTransposeB<IS_INTRA_BLOCK>();
    }

private:
    __aicore__ inline int32_t GetOrgSizeAlign()
    {
        int32_t orgHeightAlign = (IsNeedC0Align<SrcT>()) ? Align<int32_t>(GetOrgHeight(), c0Size_) :
                                                           Align<int32_t>(GetOrgHeight(), (int32_t)BLOCK_CUBE);
        int32_t orgWidthAlign = (IsSameTypeV<SrcT, float> || (IsNeedC0Align<SrcT>() && !INPUT_TYPE::isTrans)) ?
                                    Align<int32_t>(GetOrgWidth(), c0Size_) :
                                    Align<int32_t>(GetOrgWidth(), (int32_t)BLOCK_CUBE);
        if constexpr (HasScalePosition<INPUT_TYPE>::value) {
            if constexpr (INPUT_TYPE::isTrans) {
                orgHeightAlign = CeilAlign(GetOrgHeight<INPUT_TYPE::isTrans>(), (int32_t)BLOCK_CUBE);
                orgWidthAlign = CeilAlign(GetOrgWidth<INPUT_TYPE::isTrans>(), MX_BASEK_FACTOR);
            } else {
                orgHeightAlign = CeilAlign(GetOrgHeight<INPUT_TYPE::isTrans>(), MX_BASEK_FACTOR);
                orgWidthAlign = CeilAlign(GetOrgWidth<INPUT_TYPE::isTrans>(), c0Size_);
            }
        }
        return orgHeightAlign * orgWidthAlign;
    }

    __aicore__ inline int32_t GetBaseHeightAlign() const
    {
        if constexpr (IsSameTypeV<SrcT, float> && !INPUT_TYPE::isTrans) {
            return Align(GetBaseHeight(), (int32_t)BLOCK_CUBE);
        } else if constexpr (IsNeedC0Align<SrcT>()) {
            return Align(GetBaseHeight(), c0Size_);
        } else {
            return GetBaseHeight();
        }
    }

    __aicore__ inline int32_t GetBaseWidthAlign() const
    {
        if constexpr (IsSameTypeV<SrcT, float> || (IsNeedC0Align<SrcT>() && !INPUT_TYPE::isTrans)) {
            return Align(GetBaseWidth(), c0Size_);
        } else {
            return GetBaseWidth();
        }
    }

    __aicore__ inline int32_t GetBaseSize()
    {
        if constexpr (
            INPUT_TYPE::format == CubeFormat::VECTOR && !(IsTypeOneOfV<SrcT, float> || IsNeedC0Align<SrcT>())) {
            return GetBaseWidth();
        } else {
            return GetBaseHeightAlign() * GetBaseWidthAlign();
        }
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<TransT>();
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_BASE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_BASE_H__
#endif
