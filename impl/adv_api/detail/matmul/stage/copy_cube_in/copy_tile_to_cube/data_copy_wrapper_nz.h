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
 * \file data_copy_wrapper_nz.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/copy_tile_to_cube/data_copy_wrapper_nz.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_NZ_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_NZ_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_NZ_H

#include "data_copy_wrapper_intf.h"
#include "data_copy_wrapper_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
class DataCopyWrapper<
    IMPL, MM_CFG, INPUT_TYPE,
    enable_if_t<
        INPUT_TYPE::format == CubeFormat::NZ &&
        !(INPUT_TYPE::TAG == InputTypeTag::scaleA || INPUT_TYPE::TAG == InputTypeTag::scaleB)>> {
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(LocalWorkspace);

    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;

public:
    __aicore__ inline DataCopyWrapper() = default;
    __aicore__ inline ~DataCopyWrapper() = default;

    __aicore__ inline void CopyNZ2NZ(
        const LocalTensor<TransT>& dst, const GlobalTensor<SrcT>& src, const int32_t row, const int32_t col,
        const int32_t height, const int32_t width, const int32_t gRow, const bool kAlignToC0Size = false)
    {
        constexpr bool hasScalePos = HasScalePosition<INPUT_TYPE>::value;
        CopyNZ2NZImpl<SrcT, TransT, hasScalePos>(dst, src, row, col, height, width, gRow, kAlignToC0Size);
    }

    __aicore__ inline void CopyNZ2NZDecompMode(
        const LocalTensor<TransT>& dst, const GlobalTensor<SrcT>& src, const int32_t row, const int32_t col,
        const int32_t height, const int32_t width, const int32_t gRow, uint64_t qtable0, uint64_t qtable1 = 0,
        const bool kAlignToC0Size = false)
    {}

    __aicore__ inline void CopyNZ2NZ(
        const LocalTensor<TransT>& dst, const LocalTensor<SrcT>& src, const int32_t row, const int32_t col,
        const int32_t height, const int32_t width, const int32_t gRow)
    {
        CopyNZ2NZImpl(dst, src, row, col, height, width, gRow);
    }

    template <bool IS_TRANS = false>
    __aicore__ void CopyNZ2NZWithTransData(
        const LocalTensor<TransT>& dst, LocalTensor<SrcT>& src, int row, int col, int tileHeight, int tileWidth)
    {
        int64_t size = tileHeight * tileWidth;
        LocalTensor<TransT> trans =
            MATMUL_MODULE(LocalWorkspace)->GetND2NZWorkspace(size).template ReinterpretCast<TransT>();
        trans.SetSize(size);
        int srcOffset = row * MATMUL_MODULE(CopyCubeInParams)->template GetBaseHeight<IS_TRANS>() * c0Size_ +
                        col * MATMUL_MODULE(CopyCubeInParams)->template GetBaseWidth<IS_TRANS>() *
                            MATMUL_MODULE(CopyCubeInParams)->template GetOrgHeight<IS_TRANS>();
        TransDataNZBMatrix<SrcT, TransT, IS_TRANS>(trans, src[srcOffset], tileHeight, tileWidth);
        event_t eventIDVToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventIDVToMte3);
        WaitFlag<HardEvent::V_MTE3>(eventIDVToMte3);
        CopyNZ2NZImpl(dst, trans, 0, 0, tileWidth, tileHeight, tileWidth);
    }

    template <bool IS_TRANS = false>
    __aicore__ void CopyNZ2NZWithTransData(
        const LocalTensor<TransT>& dst, GlobalTensor<SrcT>& src, int row, int col, int tileHeight, int tileWidth)
    {
        int calcWidth = CeilT(tileWidth, c0Size_) * c0Size_;
        int calcHeight = CeilT(tileHeight, c0Size_) * c0Size_;
        int64_t size = static_cast<int64_t>(calcHeight * calcWidth);
        LocalTensor<TransT> rightMatrix =
            MATMUL_MODULE(LocalWorkspace)->GetND2NZWorkspace(0).template ReinterpretCast<TransT>();
        rightMatrix.SetSize(size);
        int srcOffset = row * MATMUL_MODULE(CopyCubeInParams)->template GetBaseHeight<IS_TRANS>() * c0Size_ +
                        col * MATMUL_MODULE(CopyCubeInParams)->template GetBaseWidth<IS_TRANS>() *
                            MATMUL_MODULE(CopyCubeInParams)->template GetOrgHeight<IS_TRANS>();
        int dstOffset = 0;
        int srcHigh = CeilT(MATMUL_MODULE(CopyCubeInParams)->template GetOrgHeight<IS_TRANS>(), 16) * 16 * c0Size_;
        int dstHigh = tileHeight < c0Size_ ? tileHeight * c0Size_ : calcHeight * c0Size_;
        for (int i = 0; i < CeilT(tileWidth, c0Size_); i++) {
            DataCopy(rightMatrix[dstOffset], src[srcOffset], dstHigh);
            srcOffset += srcHigh;
            dstOffset += dstHigh;
        }
        LocalTensor<TransT> trans =
            MATMUL_MODULE(LocalWorkspace)->GetND2NZWorkspace(size).template ReinterpretCast<TransT>();
        trans.SetSize(size);
        event_t eventIDMte2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
        SetFlag<HardEvent::MTE2_V>(eventIDMte2ToV);
        WaitFlag<HardEvent::MTE2_V>(eventIDMte2ToV);
        TransDataNZBMatrix<SrcT, TransT, IS_TRANS>(trans, rightMatrix, tileHeight, tileWidth);
        event_t eventIDVToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventIDVToMte3);
        WaitFlag<HardEvent::V_MTE3>(eventIDVToMte3);
        CopyNZ2NZImpl(dst, trans, 0, 0, calcWidth, calcHeight, calcWidth);
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<SrcT>();
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_NZ_H

#if defined( \
    __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_NZ_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_NZ_H__
#endif
